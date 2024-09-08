#include "cli/cli.h"
#include "cli/command.h"
#include "lib/color.h"
#include "lib/framebf.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"
#include "util/cirbuf.h"
#include "lib/keyboard.h"
#include "game/game.h"

int _handle_game_mode_internal() {
  if (_is_up_command()) {
    println("UP");
  } else if (_is_down_command()) {
    println("DOWN");
  } else if (_is_left_command()) {
    println("LEFT");
  } else if (_is_right_command()) {
    println("RIGHT");
  } else if (_is_exit_game_command()) {
    _exit_game();
    return 1;
  } else {
    _print_error_game_mode();
  }

  print_prefix();

  return 1;
}

void handle_game_mode() {
  print_color("\n\nGame mode!\n", CMD_COLOR_YEL);
  print_prefix();

  while (is_game_mode()) {
    uart_scanning();  // always scanning for new char

    if (is_there_ansi_escape()) {
      get_ansi_control(command);

      _handle_game_mode_internal();
    }

    if (is_there_new_line()) {
      get_line(command);

      _handle_game_mode_internal();
    }
  }
}

void _print_error_game_mode() {
  print_color("Unknown command received in ", CMD_COLOR_RED);
  print_color("GAME", CMD_COLOR_GRN);
  print_color(" mode!\n", CMD_COLOR_RED);
}

int _is_up_command() {
  return strstr(command, CSI_CUU) || str_equal(command, "w");
}

int _is_down_command() {
  return strstr(command, CSI_CUD) || str_equal(command, "s");
}

int _is_left_command() {
  return strstr(command, CSI_CUF) || str_equal(command, "a");
}

int _is_right_command() {
  return strstr(command, CSI_CUB) || str_equal(command, "d");
}

int _is_exit_game_command() {
  return str_equal(command, BACK_TICK);
}

void _exit_game() {
  drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
  switch_to_cli_mode();
}
