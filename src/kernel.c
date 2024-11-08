#include "cli/cli.h"
#include "cli/baudrate.h"
#include "game/game.h"
#include "lib/framebf.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "util/cirbuf.h"
#include "util/tty.h"
#include "video-player/video-player.h"

char __attribute__((
    aligned(16))) history_buffer[HISTORY_LENGTH][COMMAND_MAX_SIZE];
int history_head = 0;

void main() {
  // set up serial console with initial baudrate and stopbit
  uart_init_with_config(current_baudrate, current_stopbits);

  // setup framebuffer
  framebf_init(1024, 768, 1024, 768, 0, 0);

  // init global command shell buffer
  Stack _cmd_st = {stack_buffer, COMMAND_MAX_SIZE, -1}; cmd_st = &_cmd_st;
  Stack _auto_comp = {auto_complete_buffer, COMMAND_MAX_SIZE, -1}; auto_complete_st = &_auto_comp;

  // clear screen and print welcome text
  clrscr();
  welcome();

  while (1) {
    if (is_cli_mode()) {
      handle_cli_mode();
    } else if (is_video_player_mode()) {
      handle_video_player_mode();
    } else if (is_game_mode()) {
      handle_game_mode();
    } else {
      uart_puts("Wrong program state!");
    }
  }
}
