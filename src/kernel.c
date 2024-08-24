#include "lib/uart.h"
#include "lib/command.h"
#include "lib/framebf.h"
#include "cli/cli.h"
#include "video-player/video-player.h"
#include "game/game.h"
#include "util/tty.h"

char command[201]; // global buffer for shell command

void main()
{
  // set up serial console
  uart_init();

  // set up framebuffer
  framebf_init(1024, 768, 1024, 768, 0, 0);

  clrscr();
  welcome();

  while (1)
  {
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
