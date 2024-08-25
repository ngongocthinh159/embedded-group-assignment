#include "cli/cli.h"
#include "cli/command.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"
#include "lib/framebf.h"
#include "lib/color.h"
#include "lib/timer.h"
#include "video-player/video.h"
#include "video-player/img-background.h"

extern int width, height, pitch;

void _mode_exit() {
    drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
}

volatile int is_playing = 1;

// int _get_cinema_background_fit_x() {
//     return (width - video_pixels_width)/2 + 18;
// }

// int _get_cinema_background_fit_y() {
//     return 48;
// }

void _handle_video_mode_internal() {
    if (str_equal(command, CMD_VIDEO_PAUSE) && is_playing) {
        is_playing = 0;
    } else if (str_equal(command, CMD_VIDEO_RESUME) && !is_playing) {
        is_playing = 1;
    }

    if (is_playing) {
        // drawVideo(_get_cinema_background_fit_x(), _get_cinema_background_fit_y(), video_array, video_pixels_width, video_pixels_height, video_array_len);
    }
}

void handle_video_player_mode() {

    uart_puts("\n\nVideo player mode!\n");
    print_prefix();

    // draw background
    // drawImage(img_cinema, 0, 0, img_pixels_width, img_pixels_height);

    while (is_video_player_mode()) {
        uart_scanning(); // always scanning for new char
  
        if (is_there_new_line()) {
            get_line(command);

            print("Command received: ");
            println(command);

            if (str_equal(command, CMD_EXIT))
            {
                switch_to_cli_mode();
                _mode_exit();
                break;
            } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
                switch_to_game_mode();
                _mode_exit();
                break;
            }

            print_prefix();
        }

        // handle command in video player mode
        _handle_video_mode_internal();
    }
}
