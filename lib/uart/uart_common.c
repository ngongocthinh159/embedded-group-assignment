#include "cli/cli.h"
#include "lib/command.h"
#include "lib/keyboard.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "util/string.h"

int new_line_received =
    0; // used in for loop to check if there is a new command
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
    if (ch == ENTER) {
      // handle when buffer is empty
      if (str_is_blank(cmd_st->buffer, st_size(cmd_st))) {
        uart_sendc(ENTER);

        // when buffer is not empty => turn on new_line_received flag
      } else {
        new_line_received = 1;

        uart_sendc(ENTER);
      }
      //        } else if (ch == ESC) {
      //            for (int i = 0; i < st_size(cmd_st); i++) {
      //                uart_puts(REMOVE_A_CHAR);
      //            }
      //            st_reset_buffer(cmd_st);
    } else if (ch == ESC) {
      ansi_escaped_received = 1;
    } else if (ch == TAB) {
      // TODO: display autocomplete command
    } else if (ch == BACKSPACE) { // DEL
      int pop_success = st_pop(cmd_st);
      if (pop_success) {
        uart_puts(REMOVE_A_CHAR);
      }
    } else {
      int push_success = st_push(cmd_st, ch);
      if (push_success && !ansi_escaped_received) {
        uart_sendc(ch);
      } else if (ansi_escaped_received) {
        if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D') {
          ansi_escaped_received = 2;
        }
      } else {
        // TODO: when stack buffer is full might do something..
      }
    }
  }
}

int is_there_new_line() { return new_line_received; }

int is_there_ansi_escape() { return ansi_escaped_received; }

// Flush all chars inside stack buffer into input buffer
// AND reset the stack
void get_line(char *buffer) {
  new_line_received = 0; // reset

  // transfer chars from stack buffer to command buffer
  char *saved_buffer = st_get_buffer(cmd_st);
  int size = 0;
  for (int i = 0; i < st_size(cmd_st); i++) {
    buffer[i] = saved_buffer[i];
    size++;
  }
  buffer[st_size(cmd_st)] = '\0';

  // remove all redudant chars
  str_beautify(buffer, size);

  // reset stack after transering
  st_reset_buffer(cmd_st);
}

void get_ansi_control(char *buffer) {
  char *ansi_buffer = st_get_buffer(cmd_st);

  if (str_equal(ansi_buffer, "[A")) {
    strcpy(buffer, CMD_ARROW_UP);
  } else if (str_equal(ansi_buffer, "[B")) {
    strcpy(buffer, CMD_ARROW_DOWN);
  } else if (str_equal(ansi_buffer, "[C")) {
    strcpy(buffer, CMD_ARROW_RIGHT);
  } else if (str_equal(ansi_buffer, "[D")) {
    strcpy(buffer, CMD_ARROW_LEFT);
  }

  st_reset_buffer(cmd_st);
  ansi_escaped_received = 0;
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
