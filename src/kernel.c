#include "lib/uart.h"
#include "lib/command.h"
#include "lib/framebf.h"
#include "cli/cli.h"
#include "video-player/video-player.h"
#include "game/game.h"
#include "util/tty.h"

void main()
{
  // set up serial console
  uart_init();

  // set up framebuffer
  framebf_init(1920, 1080, 1920, 1080, 0, 0);

  clrscr();
  welcome();
  print_prefix();

  while (1)
  {
    if (is_cli_mode()) {
      handle_cli_mode();
    } else if (is_video_player_mode()) {
      handle_video_player_mode();
    } else if (is_game_mode()) {
      handle_game_mode();
    }
  }
}
