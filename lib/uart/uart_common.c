#include "cli/cli.h"
#include "cli/command.h"
#include "lib/keyboard.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "lib/timer.h"
#include "util/string.h"
#include "cli/cli.h"
#include "util/tty.h"
#include "lib/color.h"
#include "cli/command.h"

/* Private function prototype */
void _handle_auto_completion(char ch);
int char_buffer_cpy(char *src, char *dest, int src_size);
void _print_auto_completion_if_applicable();
void _internal_char_handle(char ch);
void _apply_auto_completion();
void _abort_auto_completion();

int new_line_received =
    0; // used in for loop to check if there is a new command
volatile int in_auto_completion = 0;
/**
 * 0: uninitiated
 * 1: ESC 
 * 2: [
 * 3: A-D
 */
int ansi_escaped_received = 0;

// New function: Check and return if no new character, don't wait
// Usage: unsigned char c = uart_getc_non_block()
// 		  if (c != 0) {};
unsigned char uart_getc_non_block() {
  unsigned char ch = 0;
  if (uart_isReadByteReady())
    ch = uart_getc();
  return ch;
}

void uart_scanning() {
    char ch = uart_getc_non_block();
    if (ch != 0) {
        _internal_char_handle(ch);
    }
}

void _internal_char_handle(char ch) {
    if (in_auto_completion) {
        _handle_auto_completion(ch);
        return;
    }

    if (ch == ENTER && !is_game_mode()) {
        // handle when buffer is empty
        if (str_is_blank(cmd_st->buffer, st_size(cmd_st))) {
            uart_sendc(ENTER);
        
        // when buffer is not empty => turn on new_line_received flag
        } else {
            new_line_received = 1;
            
            uart_sendc(ENTER);
        }
    } else if (ch == UNDERSCORE || ch == PLUS) {
      ansi_escaped_received = 3;
      st_push(cmd_st, ch);
    } else if (ch == ESC) {
        // for (int i = 0; i < st_size(cmd_st); i++) {
        //     uart_puts(REMOVE_A_CHAR);
        // }
        // st_reset_buffer(cmd_st);

        // wait for 100ms for next scan sequence
        // set_wait_timer(1, 100);
        // set received to 1st char
        ansi_escaped_received = 1;
    // Auto completion handler
    } else if (ch == TAB && !str_is_blank(cmd_st->buffer, st_size(cmd_st))) {
        in_auto_completion = 1;
        _print_auto_completion_if_applicable();
    } else if (ch == BACKSPACE) {  // DEL
        int pop_success = st_pop(cmd_st);
        if (pop_success) {
            uart_puts(REMOVE_A_CHAR);
        }
    } else {
      // await any pending timer
      // set_wait_timer(0, 100);

      if (ansi_escaped_received) {
        // handle ongoing escape sequence
        // 2nd char sequence
        if (ch == '[') {
          // set received to 2nd char;
          ansi_escaped_received++;
          // set_wait_timer(1, 100);
        } else if (ansi_escaped_received == 2 && ch >= UP_ARROW && ch <= LEFT_ARROW) {
          ansi_escaped_received++;
        } else {
          ansi_escaped_received = 0;
          // pop everything 
          for (int i = 0; i <= ansi_escaped_received; i++) {
            st_pop(cmd_st);
          }
        }
      
      // handle game mode only receive ansi or 1 char
      } else if (is_game_mode()) {
        new_line_received = 1;
      }

      int push_success = st_push(cmd_st, ch);
      if (push_success && !ansi_escaped_received) {
          if (is_game_mode()) return; // not output receved char in game mode 
          uart_sendc(ch);
      }  else {
          // TODO: when stack buffer is full might do something..
      }
    }
}

int is_there_new_line() { return new_line_received; }

int is_there_ansi_escape() { return ansi_escaped_received == 3; }

// Flush all chars inside stack buffer into input buffer
// AND reset the stack
void get_line(char *buffer) {
  new_line_received = 0; // reset

    // transfer chars from stack buffer to command buffer
    int size = char_buffer_cpy(st_get_buffer(cmd_st), buffer, st_size(cmd_st));

  // remove all redudant chars
  str_beautify(buffer, size);

  // reset stack after transering
  st_reset_buffer(cmd_st);
}

// return size of destination buffer after transfering
// make sure dest buffer has enough buffer memory (>= src_size + 1 for '\0')
int char_buffer_cpy(char *src, char *dest, int src_size) {
    for (int i = 0; i < src_size; i++) {
        dest[i] = src[i];
    }
    dest[src_size] = '\0';
    return src_size;
}

void _handle_auto_completion(char ch) {
    // apply
    if (ch == TAB || ch == ENTER) {
        _apply_auto_completion();
        in_auto_completion = 0;

    // abort
    } else {
        _abort_auto_completion();
        in_auto_completion = 0;

        // handle char normal flow
        if (ch != BACKSPACE) _internal_char_handle(ch);
    }
}

void _apply_auto_completion() {
    // remove draft guess char
    for (int i = 0; i < st_size(auto_complete_st); i++) {
        uart_puts(REMOVE_A_CHAR);
    }

    // add real guess char
    uart_puts(st_get_buffer(auto_complete_st));
    st_append_from_st(auto_complete_st, cmd_st);

    // reset auto complet stack
    st_reset_buffer(auto_complete_st);
}

void _abort_auto_completion() {
    for (int i = 0; i < st_size(auto_complete_st); i++) {
        uart_puts(REMOVE_A_CHAR);
    }
    st_reset_buffer(auto_complete_st);
}

int _get_suffix_auto_complete(char *token, char *suffix_buffer, int token_size) {
    int suffix_size = 0;
    int j = 0;
    for (int i = 0; i < all_commands_size; i++) {
        char *command_name = all_commands[i].name;
        int command_size = all_commands[i].length;
        if (command_size > token_size && str_start_with(command_name, token, command_size, token_size)) {
            for (int k = token_size; k < command_size; k++, j++) {
                suffix_buffer[j] = all_commands[i].name[k];
                suffix_size++;
            }
            suffix_buffer[suffix_size] = '\0';
            break;
        }
    }

    return suffix_size;
}

void _print_auto_completion_if_applicable() {
    // example: playvideo
    char last_token[50]; // playvi
    char suffix_guess[50]; // deo

    st_copy_from_st(cmd_st, auto_complete_st);

    st_beautify_buffer(auto_complete_st);

    int token_size = str_last_token(st_get_buffer(auto_complete_st), last_token, st_size(auto_complete_st));

    int ok = 0;
    if (token_size) {
        int suffix_size = _get_suffix_auto_complete(last_token, suffix_guess, token_size);

        if (suffix_size) {
            st_copy_from_str(suffix_guess, auto_complete_st, suffix_size);
            print_color(suffix_guess, CMD_COLOR_YEL_UNDER_LINE);

            ok = 1;
        }
    }

    if (!ok) in_auto_completion = 0;
}

void get_ansi_control(char *buffer) {
  ansi_escaped_received = 0;

  // transfer chars from stack buffer to command buffer
  int size = char_buffer_cpy(st_get_buffer(cmd_st), buffer, st_size(cmd_st));

  // remove all redudant chars
  str_beautify(buffer, size);

  st_reset_buffer(cmd_st);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
  while (*s) {
    /* convert newline to carriage return + newline */
    if (*s == '\n')
      uart_sendc('\r');
    uart_sendc(*s++);
  }
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num) {
  uart_puts("0x");
  for (int pos = 28; pos >= 0; pos = pos - 4) {
    // Get highest 4-bit nibble
    char digit = (num >> pos) & 0xF;

    /* Convert to ASCII code */
    // 0-9 => '0'-'9', 10-15 => 'A'-'F'
    digit += (digit > 9) ? (-10 + 'A') : '0';
    uart_sendc(digit);
  }
}
void uart_hex_no_base(unsigned int num, unsigned int width){
  int printed = 0;
  for (int position = 28; position >= 0; position = position - 4){
    char digit = (num >> position) & 0xF;

    digit += (digit > 9) ? (-10 + 'A') : '0';
    if (digit != '0' || printed == 1 || position <= 4 * (width - 1)){
      uart_sendc(digit);
      printed = 1;
    }
  }
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num) {
  // A string to store the digit characters
  char str[33] = "";

  // Calculate the number of digits
  int len = 1;
  int temp = num;
  while (temp >= 10) {
    len++;
    temp = temp / 10;
  }

  // Store into the string and print out
  for (int i = 0; i < len; i++) {
    int digit = num % 10; // get last digit
    num = num / 10;       // remove last digit from the number
    str[len - (i + 1)] = digit + '0';
  }
  str[len] = '\0';

  uart_puts(str);
}
