#include "game/game.h"
#include "game/game-utils.h"
#include "game/game-screen-welcome.h"
#include "game/game-screen-how-to-play.h"

#include "cli/cli.h"
#include "cli/command.h"
#include "lib/color.h"
#include "lib/framebf.h"
#include "lib/keyboard.h"
#include "lib/math.h"
#include "lib/timer.h"
#include "lib/uart.h"
#include "util/cirbuf.h"
#include "util/string.h"
#include "util/tty.h"

/* Events: call_every_ms should be multiple of 10 */
volatile Event events[] = {
    {.call_every_ms = 50,
     .handler = _handle_events_call_every_50ms,
     .counter = 0},
    {.call_every_ms = 100,
     .handler = _handle_events_call_every_100ms,
     .counter = 0},
    {.call_every_ms = 200,
     .handler = _handle_events_call_every_200ms,
     .counter = 0},
    {.call_every_ms = 500,
     .handler = _handle_events_call_every_500ms,
     .counter = 0},
    {.call_every_ms = 1000,
     .handler = _handle_events_call_every_1s,
     .counter = 0},
};

/* Global variables */
int should_exit_game_mode = 0;
const int smallest_interval_ms = 10;
int selected_difficulty = 0;

/* Game variables */
unsigned int scores = 120;

/* Offsets */
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_X = 242;
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_Y = 64;
const unsigned int OFFSET_PHYSICAL_NEXT_FRAME_X = 602;
const unsigned int OFFSET_PHYSICAL_NEXT_FRAME_Y = 544;

/* Game init */
const int GAME_FIELD_WIDHT = 11;
const int GAME_FIELD_HEIGHT = 21;
const int VIRTUAL_GAME_FIELD_OFFSET = 3;
const int BLOCK_SIZE = 30;
volatile SCREEN current_screen = SCREEN_WELCOME;

Piece dynamic_piece = {.shape = SHAPE_I,
                       .color = RED,
                       .center_point = {.x = 1, .y = 0},
                       .angle = ANGLE_0};

Piece next_piece = {.shape = SHAPE_I,
                    .color = RED,
                    .center_point = {.x = 1, .y = 0},
                    .angle = ANGLE_0};

void handle_game_mode() {
  print_color("\n\nGame mode!\n", CMD_COLOR_YEL);
  print_prefix();
  should_exit_game_mode = 0;
  reset_welcome_screen_state();
  switch_to_welcome_screen();

  while (is_game_mode() && !should_exit_game_mode) {
    set_wait_timer_cb1(1, smallest_interval_ms, _uart_scanning_callback);
    _handle_timing_events();
    set_wait_timer_cb1(0, smallest_interval_ms, _uart_scanning_callback);
  }
}

void _handle_game_mode_internal() {
  int is_handled = 0;
  if (current_screen == SCREEN_WELCOME) {
    is_handled |= handle_screen_welcome();
  } else if (current_screen == SCREEN_HOW_TO_PLAY) {
    is_handled |= handle_screen_how_to_play();
  } else if (current_screen == SCREEN_GAME_PLAY) {
    is_handled |= _handle_screen_game_play_internal();
  } else if (current_screen == SCREEN_GAME_PAUSE) {

  } else if (current_screen == SCREEN_GAME_OVER) {
    
  }

  if (!is_handled) {
    _print_error_game_mode();
  }

  print_prefix();
}

int _handle_screen_game_play_internal() {
  int is_handled = 1;
  if (_is_up_command()) {
    println("ACK: UP");

  } else if (_is_down_command()) {
    println("ACK: DOWN");

  } else if (_is_left_command()) {
    println("ACK: LEFT");
  } else if (_is_right_command()) {
    println("ACK: RIGHT");
  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
  } else if (_is_back_tick_command()) {
    println("ACK: BACK TICK");
  } else {
    is_handled = 0;
  }

  if (is_handled) {

  }

  return is_handled;
}

void _handle_events_call_every_50ms() {}

void _handle_events_call_every_100ms() {}

void _handle_events_call_every_200ms() {
  _clear_game_piece(&dynamic_piece);
  _increase_current_piece();
  _draw_game_piece(&dynamic_piece);
}

void _handle_events_call_every_500ms() {}

void _handle_events_call_every_1s() {}

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

void _init_game() {
  scores = 0;
  _spawn_random_piece_to(&dynamic_piece);
  _spawn_random_piece_to(&next_piece);
  _reset_timer_counters();

  // draw
  displayGamePlayScreen();
  _draw_next_frame_piece(&next_piece);
  _draw_game_scores(scores);
}

void _reset_timer_counters() {
  for (int i = 0; i < sizeof(events)/sizeof(Event); i++) {
    events[i].counter = 0;
  }
}

void _increase_current_piece() { dynamic_piece.center_point.y += 1; }

void _handle_timing_events() {
  for (int i = 0; i < sizeof(events) / sizeof(Event); i++) {
    events[i].counter++;
    if (events[i].counter >=
        events[i].call_every_ms / smallest_interval_ms + 1) {
      events[i].counter = 0;
      if (current_screen == SCREEN_GAME_PLAY) events[i].handler();
    }
  }
}

void switch_to_game_play_screen() {
  _init_game();
  current_screen = SCREEN_GAME_PLAY;
}
