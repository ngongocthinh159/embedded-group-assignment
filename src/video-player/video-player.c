#include "cli/cli.h"
#include "lib/uart.h"
#include "lib/command.h"
#include "util/string.h"
#include "util/tty.h"

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
                break;
            } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
                switch_to_game_mode();
                break;
            }

            // handle command in video player mode

            print_prefix();
        }
    }
}
