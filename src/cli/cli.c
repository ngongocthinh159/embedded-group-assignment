#include "cli/cli.h"

#include "cli/command.h"
#include "cli/help_text.h"
#include "cli/show_boardrev.h"
#include "cli/welcome_text.h"
#include "lib/color.h"
#include "lib/kernel.h"
#include "lib/mbox.h"
#include "lib/keyboard.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "lib/framebf.h"
#include "util/cirbuf.h"
#include "util/string.h"
#include "util/tty.h"

/* Functions prototype */
void showinfo();

char *OS_NAME = "FIRE_OS";
char *GROUP_NAME = "FIRE OS";

char command[COMMAND_MAX_SIZE +
             1];  // global buffer for command shell: after strip from stack
                  // buffer (size + 1 for '\0' character)

char stack_buffer[COMMAND_MAX_SIZE];  // global buffer for command shell: record
                                      // all user input
Stack *cmd_st;  // wrapper for command shell buffer (when wrapped the buffer can
                // be treated as a stack and use like a stack)

char auto_complete_buffer[COMMAND_MAX_SIZE + 1];
Stack *auto_complete_st;

volatile int current_mode = CLI;  // mode switching in kernel.c
int history_req = 0;


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

void _handle_internal() {
    if (str_equal(command, CMD_HELP)) {
        print(help_text);
    } else if (str_equal(command, CMD_EXIT)) {
        // exit();
    } else if (str_equal(command, CMD_CLEAR)) {
        clrscr();
    } else if (str_equal(command, CMD_HISTORY)) {
        for (int i = 0; i < HISTORY_LENGTH; i++) {
            uart_dec(i);
            print(". ");
            println(history_buffer[i]);
        }
    } else if (str_equal(command, CMD_SHOW_INFO)) {
        showinfo();

    }
}

void _handle_history() {
  if (strstr(command, CSI_CUU)) {
    history_req = cir_buf_previous(history_req);
  } else if (strstr(command, CSI_CUD)) {
    history_req = cir_buf_next(history_req);
  }
}

void handle_cli_mode() {
    uart_puts("\n\nCLI mode!\n");
    print_prefix();

    while (is_cli_mode()) {
        uart_scanning();  // always scanning for new char


    if (is_there_ansi_escape()) {
      get_ansi_control(command);

      _handle_history();

      clrln();
      print_prefix();
      char* cmd = history_buffer[history_req];
      print(cmd);
      st_copy_from_str(cmd, cmd_st, strlen(cmd));
    }

    if (is_there_new_line()) {
      // clear history_req buffer
      history_req = history_head + 1;

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
       _handle_internal();

       print_prefix();
        }
    }
}

void switch_to_cli_mode() { current_mode = CLI; }

void switch_to_video_player_mode() { current_mode = VIDEO_PLAYER; }

void switch_to_game_mode() { current_mode = GAME; }

int get_current_mode() { return current_mode; }

int is_cli_mode() { return current_mode == CLI; }

int is_video_player_mode() { return current_mode == VIDEO_PLAYER; }

int is_game_mode() { return current_mode == GAME; }

void showinfo() {
    mBuf[0] = 12 * 4;
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_GETBOARDREV;
    mBuf[3] = 4;// tag value's buffer size in bytes
    mBuf[4] = 0; // tag request code
    mBuf[5] = 0; // reserved for repsonse value buffer

    mBuf[6] = MBOX_TAG_GETMAC;
    mBuf[7] = 6; // tag value's buffer size in bytes
    mBuf[8] = 0; // tag request code
    mBuf[9] = 0; // reserved for repsonse value buffer
    mBuf[10] = 0;  // reserved for repsonse value buffer

    mBuf[11] = MBOX_TAG_LAST;

    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
        print("\nResponse Code for whole message: ");
        uart_hex(mBuf[1]);

        print("\n\n+ Response Code in Message TAG MBOX_TAG_GETBOARDREV: ");
        uart_hex(mBuf[4]);
        uart_puts("\nDATA: Board revision = ");
        unsigned int revision = mBuf[5];
        uart_hex(mBuf[5]);

        print("\n\n");
        
        show_boardrev(revision);

        uart_puts("\n+ Response Code in Message TAG MBOX_TAG_GETMAC: ");
        uart_hex(mBuf[8]);
        uart_puts("\nDATA: MAC address = ");
        // unsigned int MAC_address = &mBuf[9];
        unsigned char *byte_ptr = (unsigned char *)(&mBuf[9]);
        for (int i = 0; i <= 5; i++) {
            uart_hex_no_base(*byte_ptr, 2);
            if (i != 5) {
                print(":");
            }
            byte_ptr++;
        }
        print("\n");
        print("\n");
    } else {
        uart_puts("Unable to query!\n");
    }
}
