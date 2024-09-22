#include "game/game.h"

#include "cli/cli.h"
#include "cli/command.h"
#include "game/game-screen-how-to-play.h"
#include "game/game-screen-over.h"
#include "game/game-screen-welcome.h"
#include "game/game-utils.h"
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
volatile int current_difficulty = 0;  // 0: easy, 1: med, 2: hard
volatile int random_counter = 0;
const int score_step = 10;
volatile int spawned_pieces = 0;
volatile int frozen_level = 0;
volatile int completed_rows = 0;
volatile int total_received_commands = 0;
volatile int total_rotation_commands = 0;
volatile int total_left_commands = 0;
volatile int total_right_commands = 0;
volatile int total_down_commands = 0;

/* Game variables */
unsigned int scores = 120;
unsigned int brick_height = 0;

/* Offsets */
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_X = 242;
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_Y = 64;
const unsigned int OFFSET_PHYSICAL_NEXT_FRAME_X = 602;
const unsigned int OFFSET_PHYSICAL_NEXT_FRAME_Y = 544;

/* Game init */
volatile SCREEN current_screen = SCREEN_WELCOME;

Piece dynamic_piece = {.shape = SHAPE_I,
                       .color = RED,
                       .center_point = {.x = 1, .y = 0},
                       .angle = ANGLE_0};

Piece next_piece = {.shape = SHAPE_I,
                    .color = RED,
                    .center_point = {.x = 1, .y = 0},
                    .angle = ANGLE_0};

Block static_game_field[GAME_FIELD_FULL_HEIGHT][GAME_FIELD_WIDTH];

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
  total_received_commands++;

  int is_handled = 0;
  if (current_screen == SCREEN_WELCOME) {
    is_handled |= handle_screen_welcome();
  } else if (current_screen == SCREEN_HOW_TO_PLAY) {
    is_handled |= handle_screen_how_to_play();
  } else if (current_screen == SCREEN_GAME_PLAY) {
    is_handled |= _handle_screen_game_play_internal();
  } else if (current_screen == SCREEN_GAME_PAUSE) {
  } else if (current_screen == SCREEN_GAME_OVER) {
    is_handled |= handle_screen_game_over();
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
    _move_piece_down(&dynamic_piece);
    total_down_commands++;
  } else if (_is_left_command()) {
    println("ACK: LEFT");
    _move_piece_left(&dynamic_piece);
    total_left_commands++;
  } else if (_is_right_command()) {
    println("ACK: RIGHT");
    _move_piece_right(&dynamic_piece);
    total_right_commands++;
  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
    _rotate_piece(&dynamic_piece);
    total_rotation_commands++;
  } else if (_is_back_tick_command()) {
    println("ACK: BACK TICK");
    _print_game_over_statistic();
    switch_to_welcome_screen();
  } else {
    is_handled = 0;
  }

  if (is_handled) {
  }

  return is_handled;
}

void _handle_events_call_every_50ms() {
  if (_is_hard_mode()) {
    _game_progress_event();
  }
}

void _handle_events_call_every_100ms() {}

void _handle_events_call_every_200ms() {
  if (_is_medium_mode()) {
    _game_progress_event();
  }
}

void _handle_events_call_every_500ms() {}

void _handle_events_call_every_1s() {
  if (_is_easy_mode()) {
    _game_progress_event();
  }
}

void _game_progress_event() {
  _clear_game_piece(&dynamic_piece);
  _increase_current_piece();
  _draw_game_piece(&dynamic_piece);

  set_wait_timer_cb2(1, 200, _uart_scanning_callback);
  set_wait_timer_cb2(0, 200, _uart_scanning_callback);

  _check_settle_down_and_move_game_state(&dynamic_piece);
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

  _increase_random_counter();
}

void _init_game() {
  scores = 0;
  spawned_pieces = 0;
  frozen_level = 0;
  completed_rows = 0;
  total_received_commands = 0;
  total_rotation_commands = 0;
  total_left_commands = 0;
  total_right_commands = 0;
  total_down_commands = 0;
  _spawn_random_piece_to(&dynamic_piece);
  _spawn_random_piece_to(&next_piece);
  _reset_timer_counters();
  _init_static_game_field();

  // draw
  displayGamePlayScreen(current_difficulty == 0
                            ? "Mode: Easy"
                            : (current_difficulty == 1 ? "Mode: Medium" : "Mode: Hard"));
  _draw_next_frame_piece(&next_piece);
  _draw_game_scores(scores);
}

void _increase_current_piece() { dynamic_piece.center_point.y += 1; }

void _reset_timer_counters() {
  for (int i = 0; i < sizeof(events) / sizeof(Event); i++) {
    events[i].counter = 0;
  }
}

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

void _move_piece_left(Piece *piece) {
  _clear_game_piece(piece);

  piece->center_point.x--;
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    if (_is_occupied_by_static_field(points_buffer_angle_rotated[i].x,
                                     points_buffer_angle_rotated[i].y)) {
      piece->center_point.x++;
      _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
      _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
      break;
    }
  }

  _check_settle_down_and_move_game_state(piece);

  _draw_game_piece(piece);
}

void _move_piece_right(Piece *piece) {
  _clear_game_piece(piece);

  piece->center_point.x++;
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    if (_is_occupied_by_static_field(points_buffer_angle_rotated[i].x,
                                     points_buffer_angle_rotated[i].y)) {
      piece->center_point.x--;
      _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
      _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
      break;
    }
  }

  _check_settle_down_and_move_game_state(piece);

  _draw_game_piece(piece);
}

void _move_piece_down(Piece *piece) {
  _clear_game_piece(piece);

  piece->center_point.y++;
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    if (_is_occupied_by_static_field(points_buffer_angle_rotated[i].x,
                                     points_buffer_angle_rotated[i].y)) {
      piece->center_point.y--;
      break;
    }
  }

  _check_settle_down_and_move_game_state(piece);
  _reset_timer_counters();

  _draw_game_piece(piece);
}

void _rotate_piece(Piece *piece) {
  if (piece->shape == SHAPE_O) return;

  _clear_game_piece(piece);

  Angle before_rotate_angle = piece->angle;
  if (piece->angle == ANGLE_0) {
    piece->angle = ANGLE_90;
  } else if (piece->angle == ANGLE_90) {
    piece->angle = ANGLE_180;
  } else if (piece->angle == ANGLE_180) {
    piece->angle = ANGLE_270;
  } else if (piece->angle == ANGLE_270) {
    piece->angle = ANGLE_0;
  }

  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    if (_is_occupied_by_static_field(points_buffer_angle_rotated[i].x, points_buffer_angle_rotated[i].y)) {
      piece->angle = before_rotate_angle; // restore angle if we can not rotate
      break;
    }
  }

  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  _check_settle_down_and_move_game_state(piece);

  _draw_game_piece(piece);
}

void _check_settle_down_and_move_game_state(Piece *piece) {
  int deepest_y = -9999;
  int height_y_occupied_by_static_field = 9999;
  int deepest_y_occupied_by_static_field = -9999;
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  int should_settle = 0;
  int adjust_y = 0;

  for (int i = 0; i < __size; i++) {
    int p_x = points_buffer_angle_rotated[i].x;
    int p_y = points_buffer_angle_rotated[i].y;

    deepest_y = max(p_x, deepest_y);

    // check collapsed with static field
    if (_is_occupied_by_static_field(p_x, p_y)) {
      height_y_occupied_by_static_field =
          max(height_y_occupied_by_static_field, p_y);
      deepest_y_occupied_by_static_field =
          min(deepest_y_occupied_by_static_field, p_y);
      should_settle = 1;
    }

    // check under each piece
    if (_is_point_settle_down_by_overflow_y_or_static_field_point(p_x, p_y)) {
      should_settle = 1;
    }
  }

  // have collapse with static field => adjust piece
  if (height_y_occupied_by_static_field != 9999) {
    adjust_y = (height_y_occupied_by_static_field -
                deepest_y_occupied_by_static_field +
                1);  // should be negative for offset piece up
  }
  piece->center_point.y += adjust_y;

  if (should_settle) {
    _draw_game_piece(piece);
    _transfer_piece_to_static_field(piece);

    // check game over before checking completed row
    if (_is_game_over(piece)) {
      switch_to_game_over_screen();
      return;
    }

    _adjust_complete_rows_and_frozen_rows();

    // check game over after possibly adding frozen rows
    if (_is_game_over(piece)) {
      switch_to_game_over_screen();
      return;
    }

    _prepare_next_game_state_after_settling();
  }
}

void _prepare_next_game_state_after_settling() {
  _copy_piece_data(&next_piece,
                   &dynamic_piece);  // copy next piece to dynamic piece
  _clear_next_frame_piece(&next_piece);
  _spawn_random_piece_to(&next_piece);
  _draw_next_frame_piece(&next_piece);

  _reset_timer_counters();
}

int _is_easy_mode() { return current_difficulty == 0; }

int _is_medium_mode() { return current_difficulty == 1; }

int _is_hard_mode() { return current_difficulty == 2; }
