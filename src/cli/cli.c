#include "cli/cli.h"
#include "cli/command.h"
#include "cli/help_text.h"
#include "cli/welcome_text.h"
#include "lib/color.h"
#include "lib/kernel.h"
#include "lib/keyboard.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "lib/framebf.h"
#include "util/cirbuf.h"
#include "util/string.h"
#include "util/tty.h"

char *OS_NAME = "FIRE_OS";
char *GROUP_NAME = "FIRE OS";

char command[COMMAND_MAX_SIZE + 1]; // global buffer for command shell: after strip from stack buffer (size + 1 for '\0' character)

char stack_buffer[COMMAND_MAX_SIZE]; // global buffer for command shell: record all user input
Stack *cmd_st; // wrapper for command shell buffer (when wrapped the buffer can be treated as a stack and use like a stack)


char auto_complete_buffer[COMMAND_MAX_SIZE + 1];
Stack *auto_complete_st;

volatile int current_mode = CLI; // mode switching in kernel.c
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
  } else if (str_equal(command, CMD_SHOW_IMAGE)){
      displayWelcomeImage();
  } else if (str_equal(command, CMD_HISTORY)) {
    for (int i = 0; i < HISTORY_LENGTH; i++) {
      uart_dec(i);
      print(". ");
      uart_dec(str_equal(history_buffer[i], ""));
      println(history_buffer[i]);
    }
  }
}

void _handle_history() {
  if (strstr(command, CSI_CUU)) {
    if (history_req < history_head) 
    history_req++;
  } else if (strstr(command, CSI_CUD)) {
    if (history_req > 0)
    history_req--;
  }
}

void handle_cli_mode() {
  uart_puts("\n\nCLI mode!\n");
  print_prefix();

  while (is_cli_mode()) {
    uart_scanning(); // always scanning for new char

    if (is_there_ansi_escape()) {
      get_ansi_control(command);

      _handle_history();

      clrln();
      print_prefix();
      if (history_req) {
        char* cmd = history_buffer[history_head - history_req];
        print(cmd);
        st_copy_from_str(cmd, cmd_st, strlen(cmd));
      } else {
        // TODO: left arrow and right arrow will ALSO clear this, fix.
        st_copy_from_str("", cmd_st, 0);
      }
    }

    if (is_there_new_line()) {
      // clear history_req buffer
      history_req = 0;

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
