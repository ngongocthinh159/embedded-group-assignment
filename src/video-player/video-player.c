#include "cli/cli.h"
#include "cli/command.h"
#include "lib/color.h"
#include "lib/framebf.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"
#include "video-player/video-player.h"
#include "util/cirbuf.h"
#include "video-player/video.h"
#include "video-player/img-background.h"

void _mode_exit() {
  drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
}

volatile int is_playing = 1;

int _get_cinema_background_fit_x() {
    return (width - video_pixels_width)/2 + 18;
}

int _get_cinema_background_fit_y() {
    return 48;
}

int _handle_video_mode_internal() {
  int is_handled = 0;
  if (str_equal(command, CMD_VIDEO_PAUSE)) {
    pause_video();
    is_handled = 1;
  } else if (str_equal(command, CMD_VIDEO_RESUME)) {
    resume_video();
    is_handled = 1;
  }

  return is_handled;
}

void handle_video_player_mode() {
  print_color("\n\nVideo mode!\n", CMD_COLOR_YEL);
  print_prefix();
  
  // draw background
  drawImage(img_cinema, 0, 0, img_pixels_width, img_pixels_height);

  while (is_video_player_mode()) {
    _draw_video_if_is_playing(); // draw video first then handle command
    
    uart_scanning();  // always scanning for new char

    if (is_there_ansi_escape()) {
      get_ansi_control(command);

      handle_history();

      clrln();
      print_prefix();
      char *cmd = history_buffer[history_req];
      print(cmd);
      st_copy_from_str(cmd, cmd_st, strlen(cmd));
    }

    if (is_there_new_line()) {
      // clear history_req buffer
      history_req = history_head + 1;

      get_line(command);
      // push command to history, similar to bash history
      cir_buf_push(command);

      print_command_received();

      if (handle_flow_control_commands()) break;

      int command_is_handled = 0;
      command_is_handled |= handle_global_commands();
      
      if (!command_is_handled) {
        command_is_handled |= _handle_video_mode_internal();
      }

      if (!command_is_handled) {
        print_command_not_regconized_error();
      }

      print_prefix();
    }
  }
}

void resume_video() {
  is_playing = 1;
}

void pause_video() {
  is_playing = 0;
}

void _draw_video_if_is_playing() {
  if (is_playing) {
    drawVideo(_get_cinema_background_fit_x(), _get_cinema_background_fit_y(),
    video_array, video_pixels_width, video_pixels_height, video_array_len);
  }
}
