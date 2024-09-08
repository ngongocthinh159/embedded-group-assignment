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
#include "lib/math.h"

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

/* Offsets */
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_X = 242;
const unsigned int OFFSET_PHYSICAL_GAME_FIELD_Y = 64;

/* call_every_ms should be multiple of 10 */
volatile Event events[] = {
  {.call_every_ms = 50, .handler = _handle_events_call_every_50ms, .counter = 0 },
  {.call_every_ms = 100, .handler = _handle_events_call_every_100ms, .counter = 0 },
  {.call_every_ms = 200, .handler = _handle_events_call_every_200ms, .counter = 0 },
  {.call_every_ms = 500, .handler = _handle_events_call_every_500ms, .counter = 0 },
  {.call_every_ms = 1000, .handler = _handle_events_call_every_1s, .counter = 0 },
};

/* Game init */
static const int GAME_FIELD_WIDHT = 11;
static const int GAME_FIELD_HEIGHT = 21;
static const int VIRTUAL_GAME_FIELD_OFFSET = 3;
static const int BLOCK_SIZE = 30;

static Point init_pos_shape_I[] = {
  {.x = 0, .y = 0},
  {.x = 1, .y = 0},
  {.x = 2, .y = 0},
  {.x = 3, .y = 0},
};
static Point init_center_shape_I = {.x = 1, .y = 0};

static Point init_pos_shape_O[] = {
  {.x = 0, .y = 0},
  {.x = 1, .y = 0},
  {.x = 0, .y = 1},
  {.x = 1, .y = 1},
};
static Point init_center_shape_O = {.x = 0, .y = 0};

static Point init_pos_shape_T[] = {
  {.x = 1, .y = 0},
  {.x = 0, .y = 1},
  {.x = 1, .y = 1},
  {.x = 2, .y = 1},
};
static Point init_center_shape_T = {.x = 1, .y = 1};

static Point init_pos_shape_S[] = {
  {.x = 0, .y = 1},
  {.x = 1, .y = 1},
  {.x = 1, .y = 0},
  {.x = 2, .y = 0},
};
static Point init_center_shape_S = {.x = 1, .y = 1};

static Point init_pos_shape_Z[] = {
  {.x = 0, .y = 0},
  {.x = 1, .y = 0},
  {.x = 1, .y = 1},
  {.x = 2, .y = 1},
};
static Point init_center_shape_Z = {.x = 1, .y = 1};

static Point init_pos_shape_J[] = {
  {.x = 0, .y = 0},
  {.x = 0, .y = 1},
  {.x = 1, .y = 1},
  {.x = 2, .y = 1},
};
static Point init_center_shape_J = {.x = 1, .y = 1};

static Point init_pos_shape_L[] = {
  {.x = 2, .y = 0},
  {.x = 0, .y = 1},
  {.x = 1, .y = 1},
  {.x = 2, .y = 1},
};
static Point init_center_shape_L = {.x = 1, .y = 1};

Point points_buffer_angle_rotated[__size];
Point points_buffer_angle_0[__size]; // DO NOT USE THIS BUFFER: Only for get init position
Point center_point_buffer;

Piece dynamic_piece = {.shape = SHAPE_I, .color = RED, .center_point = {.x = 1, .y = 0}, .angle = ANGLE_0};

void handle_game_mode() {
  print_color("\n\nGame mode!\n", CMD_COLOR_YEL);
  print_prefix();
  should_exit_game_mode = 0;

  _init_game();

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
  _clear_game_piece(&dynamic_piece);
  _increase_current_piece();
  _draw_game_piece(&dynamic_piece);
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
    _clear_game_piece(&dynamic_piece);
    _rotate_piece(&dynamic_piece);
    _draw_game_piece(&dynamic_piece);
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
  return strstr(command, CSI_CUU) || str_equal(command, KEYBOARD_W);
}

int _is_down_command() {
  return strstr(command, CSI_CUD) || str_equal(command, KEYBOARD_S);
}

int _is_left_command() {
  return strstr(command, CSI_CUB) || str_equal(command, KEYBOARD_A);
}

int _is_right_command() {
  return strstr(command, CSI_CUF) || str_equal(command, KEYBOARD_D);
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

void _init_game() {
  displayGamePlayScreen();
  _spawn_random_dynamic_piece();
}

void _increase_current_piece() {
  dynamic_piece.center_point.y += 1;
}

void _draw_game_state() {
  _draw_game_piece(&dynamic_piece);
}

void _clear_game_piece(Piece *piece) {
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    draw_game_point(points_buffer_angle_rotated[i].x, points_buffer_angle_rotated[i].y, CLEAR);
  }
}

void _draw_game_piece(Piece *piece) {
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    draw_game_point(points_buffer_angle_rotated[i].x, points_buffer_angle_rotated[i].y, piece->color);
  }
}

void draw_game_point(int x, int y, Color color) {
  if (y <= VIRTUAL_GAME_FIELD_OFFSET) { // no draw virtual point
    return;
  }

  int real_point_x = x;
  int real_point_y = y - VIRTUAL_GAME_FIELD_OFFSET;

  if (real_point_y >= GAME_FIELD_HEIGHT) return;

  int physical_point_x = OFFSET_PHYSICAL_GAME_FIELD_X + real_point_x*BLOCK_SIZE;
  int physical_point_y = OFFSET_PHYSICAL_GAME_FIELD_Y + real_point_y*BLOCK_SIZE;
  if (color == CYAN) {
    drawCyanBlock(physical_point_x, physical_point_y);
  } else if (color == YELLOW) {
    drawYellowBlock(physical_point_x, physical_point_y);
  } else if (color == PURPLE) {
    drawPurpleBlock(physical_point_x, physical_point_y);
  } else if (color == GREEN) {
    drawGreenBlock(physical_point_x, physical_point_y);
  } else if (color == RED) {
    drawRedBlock(physical_point_x, physical_point_y);
  } else if (color == BLUE) {
    drawBlueBlock(physical_point_x, physical_point_y);
  } else if (color == ORANGE) {
    drawOrangeBlock(physical_point_x, physical_point_y);
  } else if (color == CLEAR) {
    drawClearBlock(physical_point_x, physical_point_y);
  }
}

// init points with center point move out of init center point
Point* _copy_piece_angle_0_points_to_buffer(Piece *piece, Point buffer[]) {
  Point* init_points = _get_init_points(piece);
  Point init_center_point = _get_init_center_point(piece);

  int adjust_x = piece->center_point.x - init_center_point.x;
  int adjust_y = piece->center_point.y - init_center_point.y;
  for (int i = 0; i < __size; i++) {
    buffer[i].x = init_points[i].x + adjust_x;
    buffer[i].y = init_points[i].y + adjust_y;
  }
  return buffer;
}

// init points with init center point (zero point spawn position)
Point* _get_init_points(Piece *piece) {
  if (piece->shape == SHAPE_I) {
    return init_pos_shape_I;
  } else if (piece->shape == SHAPE_O) {
    return init_pos_shape_O;
  } else if (piece->shape == SHAPE_T) {
    return init_pos_shape_T;
  } else if (piece->shape == SHAPE_S) {
    return init_pos_shape_S;
  } else if (piece->shape == SHAPE_Z) {
    return init_pos_shape_Z;
  } else if (piece->shape == SHAPE_J) {
    return init_pos_shape_J;
  } else {
    return init_pos_shape_L;
  }
}

// init points with init center point (zero point spawn position)
Point _get_init_center_point(Piece *piece) {
  if (piece->shape == SHAPE_I) {
    return init_center_shape_I;
  } else if (piece->shape == SHAPE_O) {
    return init_center_shape_O;
  } else if (piece->shape == SHAPE_T) {
    return init_center_shape_T;
  } else if (piece->shape == SHAPE_S) {
    return init_center_shape_S;
  } else if (piece->shape == SHAPE_Z) {
    return init_center_shape_Z;
  } else if (piece->shape == SHAPE_J) {
    return init_center_shape_J;
  } else {
    return init_center_shape_L;
  }
}

void _rotate_piece(Piece *piece) {
  if (piece->shape == SHAPE_O) return;

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
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
}

void _adjust_center_point_if_overflow(Piece *piece, Point points[]) {
  int x_adjust = 0;
  int y_adjust = 0;
  for (int i = 0; i < __size; i++) {
    int real_x = points[i].x;
    int real_y = points[i].y - VIRTUAL_GAME_FIELD_OFFSET;

    if (real_x < 0) {
      x_adjust += 1;
    } else if (real_x >= GAME_FIELD_WIDHT) {
      x_adjust -= 1;
    }
    if (real_y < 0) {
      y_adjust += 1;
    } else if (real_y >= GAME_FIELD_HEIGHT) {
      y_adjust -= 1;
    }
  }

  piece->center_point.x += x_adjust;
  piece->center_point.y += y_adjust;
}

void _copy_piece_rotated_points_to_buffer(Piece *piece, Point buffer[]) {
  Point* _angle_0_init_points = _copy_piece_angle_0_points_to_buffer(piece, points_buffer_angle_0);
  for (int i = 0; i < __size; i++) {
    int x_0 = _angle_0_init_points[i].x - piece->center_point.x;
    int y_0 = _angle_0_init_points[i].y - piece->center_point.y;

    int x_1 = x_0 * _get_angle_multiplier_cos(piece->angle) - y_0 * _get_angle_multiplier_sin(piece->angle);
    int y_1 = x_0 * _get_angle_multiplier_sin(piece->angle) - y_0 * _get_angle_multiplier_cos(piece->angle);

    x_1 += piece->center_point.x;
    y_1 += piece->center_point.y;

    buffer[i].x = x_1;
    buffer[i].y = y_1;
  }
}

int _get_angle_multiplier_sin(Angle angle) {
  if (angle == ANGLE_0) {
    return 0;
  } else if (angle == ANGLE_90) {
    return 1;
  } else if (angle == ANGLE_180) {
    return 0;
  } else {
    return -1;
  }
}

int _get_angle_multiplier_cos(Angle angle) {
  if (angle == ANGLE_0) {
    return 1;
  } else if (angle == ANGLE_90) {
    return 0;
  } else if (angle == ANGLE_180) {
    return -1;
  } else {
    return 0;
  }
}

void _spawn_random_dynamic_piece() {
  int rand_angle = rand(1, 4);
  int rand_color = rand(1, 7);
  int rand_shape = rand(1, 7);
  int rand_center_point_x = rand(0, GAME_FIELD_WIDHT - 1);
  
  if (rand_angle == 1) {
    dynamic_piece.angle = ANGLE_0;
  } else if (rand_angle == 2) {
    dynamic_piece.angle = ANGLE_90;
  } else if (rand_angle == 3) {
    dynamic_piece.angle = ANGLE_180;
  } else if (rand_angle == 4) {
    dynamic_piece.angle = ANGLE_270;
  }

  if (rand_color == 1) {
    dynamic_piece.color = CYAN;
  } else if (rand_color == 2) {
    dynamic_piece.color = YELLOW;
  } else if (rand_color == 3) {
    dynamic_piece.color = PURPLE;
  } else if (rand_color == 4) {
    dynamic_piece.color = GREEN;
  } else if (rand_color == 5) {
    dynamic_piece.color = RED;
  } else if (rand_color == 6) {
    dynamic_piece.color = BLUE;
  } else if (rand_color == 7) {
    dynamic_piece.color = ORANGE;
  }

  if (rand_shape == 1) {
    dynamic_piece.shape = SHAPE_I;
  } else if (rand_shape == 2) {
    dynamic_piece.shape = SHAPE_O;
  } else if (rand_shape == 3) {
    dynamic_piece.shape = SHAPE_T;
  } else if (rand_shape == 4) {
    dynamic_piece.shape = SHAPE_S;
  } else if (rand_shape == 5) {
    dynamic_piece.shape = SHAPE_Z;
  } else if (rand_shape == 6) {
    dynamic_piece.shape = SHAPE_J;
  } else if (rand_shape == 7) {
    dynamic_piece.shape = SHAPE_L;
  }

  dynamic_piece.center_point.x = rand_center_point_x;
  dynamic_piece.center_point.y = 0;

  _copy_piece_rotated_points_to_buffer(&dynamic_piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(&dynamic_piece, points_buffer_angle_rotated);
}
