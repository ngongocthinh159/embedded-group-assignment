#include "cli/cli.h"
#include "cli/command.h"
#include "cli/help_text.h"
#include "cli/welcome_text.h"
#include "lib/color.h"
#include "lib/kernel.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "util/cirbuf.h"
#include "util/string.h"
#include "util/tty.h"
#include "lib/mbox.h"
#include "lib/math.h"


char *OS_NAME = "FIRE_OS";
char *GROUP_NAME = "FIRE OS";

char command[COMMAND_MAX_SIZE + 1]; // global buffer for command shell: after strip from stack buffer (size + 1 for '\0' character)

char stack_buffer[COMMAND_MAX_SIZE]; // global buffer for command shell: record all user input
Stack *cmd_st; // wrapper for command shell buffer (when wrapped the buffer can be treated as a stack and use like a stack)


char auto_complete_buffer[COMMAND_MAX_SIZE + 1];
Stack *auto_complete_st;

volatile int current_mode = CLI; // mode switching in kernel.c

int welcome() {
  println(welcome_text);
  println("");
  println("");
  print("Group ");
  print(GROUP_NAME);
  println(" - EEET2490 - Embedded System");
  println("");
  return 0;
}

void print_prefix() {
  print_color(OS_NAME, CMD_COLOR_RED);
  print_color(" > ", CMD_COLOR_RED);
}
void showinfo(){
    mBuf[0] = 10*4;
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_GETBOARDREV;
    mBuf[3] = 4;
    mBuf[4] = 0;
    mBuf[5] = 0;

    mBuf[6] = MBOX_TAG_GETMAC;
    mBuf[7] = 6;
    mBuf[8] = 0;
    mBuf[9] = 0;

    mBuf[10] = MBOX_TAG_LAST;

    if(mbox_call(ADDR(mBuf),MBOX_CH_PROP)){
      print("\nResponse Code for whole message: ");
      uart_hex(mBuf[1]);

      print("\n+ Response Code in Message TAG: ");
      uart_hex(mBuf[2]);
      uart_puts("\nDATA: Board revision = ");
      uart_dec(mBuf[5]);

      uart_puts("\n+ Response Code in Message TAG: ");
      uart_hex(mBuf[6]);
      uart_puts("\nDATA: MAC address = ");
      uart_hex(mBuf[9]);
      uart_puts("\n");
      //uart_puts(mBuf[9]);
      
      unsigned int MAC = mBuf[9];
      
      /*
      unsigned int masks[12];
      unsigned int numbers[12];
      for (int i = 48; i > 0; i = i-4)
      { 
        int array_count = 11;
        unsigned int number = (MAC >>i);
        numbers[array_count] = number;
        array_count--;
      }
      for (int i = 4; i <=48; i=i+4)
      { 
        int array_count = 0;
        unsigned int mask = (pow(2,i)-1);
        masks[array_count] = mask;
        array_count++;
      }*/
      uart_puts("00:");
      for (int i = 7; i >=0; i--)
      { 
        int j = 0xF;
        unsigned int number = (MAC >> (i*4)) & j;
        uart_dec(number);
        if(i%2 ==0 && i >0){
          print(":");
        }
        j = j * 0x10;
      }
      uart_puts(":00");
      /*
      print("\n");
      unsigned int first_chunk = (MAC >> 44) & 0xF;
     uart_dec(first_chunk);
     print("\n");
     uart_hex(MAC);
     print("\n");
      unsigned int second_num = (MAC >>40) & 0xF0;
      uart_dec(second_num);
      unsigned int third = (MAC >> 36) & 0xFF;
      uart_dec(third);*/
      
    }
    else {
        uart_puts("Unable to query!\n");
    }
}

int _handle_internal() {
    int x;
  if (str_equal(command, CMD_HELP)) {
    print(help_text);
            x = 0;
  } else if (str_equal(command, CMD_EXIT)) {
    // exit();
        x= -1;
  } else if (str_equal(command, CMD_CLEAR)) {
    clrscr();
  } else if (str_equal(command, CMD_HISTORY)) {
    for (int i = 0; i < HISTORY_LENGTH; i++) {
      uart_dec(i);
      print(". ");
      println(history_buffer[i]);
    }
      x = 0;
  }else if(str_equal(command,CMD_SHOW_INFO)){
    showinfo();
  }
    return x;
}

int handle_cli_mode() {
  uart_puts("\n\nCLI mode!\n");
  print_prefix();
        int handle_cli_mode_exit;

  while (is_cli_mode()) {
    uart_scanning(); // always scanning for new char

    if (is_there_ansi_escape() == 2) {
      get_ansi_control(command);

      print("ANSI escape sequence received: ");
      println(command);

      _handle_internal();
      print_prefix();
    }

    if (is_there_new_line()) {
      get_line(command);
      // push command to history, similar to bash history
      cir_buf_push(command);

      print("Command received: ");
      println(command);

      // flow control
      if (str_equal(command, CMD_SWITCH_TO_VIDEO_PLAYER_MODE)) {
        switch_to_video_player_mode();
        break;
      } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
        switch_to_game_mode();
        break;
      }
      // handle command in cli mode
      int _handle_internal_return = _handle_internal();
            //print(_handle_internal_return);
            if(_handle_internal_return ==-1){
                handle_cli_mode_exit = _handle_internal_return;
                break;
            }else{
                handle_cli_mode_exit = 0;
            }
      print_prefix();
    }
  }
    return handle_cli_mode_exit;

}

void switch_to_cli_mode() { current_mode = CLI; }

void switch_to_video_player_mode() { current_mode = VIDEO_PLAYER; }

void switch_to_game_mode() { current_mode = GAME; }

int get_current_mode() { return current_mode; }

int is_cli_mode() { return current_mode == CLI; }

int is_video_player_mode() { return current_mode == VIDEO_PLAYER; }

int is_game_mode() { return current_mode == GAME; }
