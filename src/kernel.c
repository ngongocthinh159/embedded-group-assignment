#include "cli/cli.h"
#include "cli/command.h"
#include "game/game.h"
#include "lib/framebf.h"
#include "lib/uart.h"
#include "util/tty.h"
#include "video-player/video-player.h"
#include "lib/stack.h"

void main() {
    // set up serial console
    uart_init();

    // set up framebuffer
    framebf_init(1024, 768, 1024, 768, 0, 0);

    // init global command shell buffer
    st_init(cmd_st, stack_buffer, COMMAND_MAX_SIZE);

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
