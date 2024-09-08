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
  if (strstr(command, CSI_CUU) || str_equal(command, "w")) {
    println("UP");
  } else if (strstr(command, CSI_CUD) || str_equal(command, "s")) {
    println("DOWN");
  } else if (strstr(command, CSI_CUF) || str_equal(command, "a")) {
    println("LEFT");
  } else if (strstr(command, CSI_CUB) || str_equal(command, "d")) {
    println("RIGHT");
  } else if (str_equal(command, BACK_TICK)) {
    switch_to_cli_mode();
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