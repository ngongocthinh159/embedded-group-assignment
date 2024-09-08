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
#include "lib/timer.h"
#include "game/game.h"

/* Function prototypes */
void _uart_scanning_callback();
void _handle_game_mode_internal();
void _handle_timing_events();
void _handle_events_call_every_50ms();
void _handle_events_call_every_100ms();
void _handle_events_call_every_200ms();
void _handle_events_call_every_500ms();
void _handle_events_call_every_1s();

/* Global variables */  
int should_exit_game_mode = 0;
const int smallest_interval_ms = 10;

// call_every_ms should be multiple of 10
volatile Event events[] = {
  {.call_every_ms = 50, .handler = _handle_events_call_every_50ms, .counter = 0 },
  {.call_every_ms = 100, .handler = _handle_events_call_every_100ms, .counter = 0 },
  {.call_every_ms = 200, .handler = _handle_events_call_every_200ms, .counter = 0 },
  {.call_every_ms = 500, .handler = _handle_events_call_every_500ms, .counter = 0 },
  {.call_every_ms = 1000, .handler = _handle_events_call_every_1s, .counter = 0 },
};

void handle_game_mode() {
  print_color("\n\nGame mode!\n", CMD_COLOR_YEL);
  print_prefix();
  should_exit_game_mode = 0;

  while (is_game_mode() && !should_exit_game_mode) {
    set_wait_timer_cb1(1, smallest_interval_ms, _uart_scanning_callback);
    _handle_timing_events();
    set_wait_timer_cb1(0, smallest_interval_ms, _uart_scanning_callback);
  }
}

void _uart_scanning_callback() {
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

void _handle_timing_events() {
  for (int i = 0; i < sizeof(events)/sizeof(Event); i++) {
    events[i].counter++;
    if (events[i].counter >= events[i].call_every_ms/smallest_interval_ms + 1) {
      events[i].counter = 0;
      events[i].handler();
    }
  }
}

void _handle_events_call_every_50ms() {

}

void _handle_events_call_every_100ms() {

}

void _handle_events_call_every_200ms() {

}

void _handle_events_call_every_500ms() {

}

void _handle_events_call_every_1s() {
  
}

void _handle_game_mode_internal() {
  if (_is_up_command()) {
    println("UP");
  } else if (_is_down_command()) {
    println("DOWN");
  } else if (_is_left_command()) {
    println("LEFT");
  } else if (_is_right_command()) {
    println("RIGHT");
  } else if (_is_enter_or_space_command()) {
    println("SPACE or ENTER");
  } else if (_is_back_tick_command()) {
    println("BACK TICK");
  } else {  
    _print_error_game_mode();
  }

  print_prefix();
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
  return strstr(command, CSI_CUB) || str_equal(command, "a");
}

int _is_right_command() {
  return strstr(command, CSI_CUF) || str_equal(command, "d");
}

int _is_back_tick_command() {
  return str_equal(command, BACK_TICK_STR);
}

int _is_enter_or_space_command() {
  return str_equal(command, SPACE_STR);
}

void _exit_game() {
  drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
  switch_to_cli_mode();
}
