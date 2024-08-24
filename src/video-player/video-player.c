#include "cli/cli.h"
#include "lib/uart.h"
#include "lib/command.h"
#include "util/string.h"
#include "util/tty.h"
#include "lib/framebf.h"
#include "lib/color.h"

extern int width, height, pitch;

void _mode_exit() {
    drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
}

void handle_video_player_mode() {
    char command[201];

    uart_puts("Video player mode!\n");
    print_prefix();

    while (is_video_player_mode()) {
        uart_scanning(); // always scanning for new char
  
        if (is_there_new_line()) {
            get_line(command);

            print("Command received: ");
            println(command);

            if (str_equal(command, CMD_ESC))
            {
                switch_to_cli_mode();
                _mode_exit();
                break;
            } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
                switch_to_game_mode();
                _mode_exit();
                break;
            }

            // handle command in video player mode

            print_prefix();
        }
    }
}
