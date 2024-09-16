#include "game/game.h"
#include "game/game-utils.h"

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

int debug = 0;
int is_print_statictics = 0;

/* Buffers */
Point points_buffer_angle_rotated[__size];
Point points_buffer_angle_0[__size];  // DO NOT USE THIS BUFFER: Only for get
                                      // init position
Point center_point_buffer;

/* Init positions */
Point init_pos_shape_I[] = {
    {.x = 0, .y = 0},
    {.x = 1, .y = 0},
    {.x = 2, .y = 0},
    {.x = 3, .y = 0},
};
Point init_center_shape_I = {.x = 1, .y = 0};

Point init_pos_shape_O[] = {
    {.x = 0, .y = 0},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = 1, .y = 1},
};
Point init_center_shape_O = {.x = 0, .y = 0};

Point init_pos_shape_T[] = {
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = 1, .y = 1},
    {.x = 2, .y = 1},
};
Point init_center_shape_T = {.x = 1, .y = 1};

Point init_pos_shape_S[] = {
    {.x = 0, .y = 1},
    {.x = 1, .y = 1},
    {.x = 1, .y = 0},
    {.x = 2, .y = 0},
};
Point init_center_shape_S = {.x = 1, .y = 1};

Point init_pos_shape_Z[] = {
    {.x = 0, .y = 0},
    {.x = 1, .y = 0},
    {.x = 1, .y = 1},
    {.x = 2, .y = 1},
};
Point init_center_shape_Z = {.x = 1, .y = 1};

Point init_pos_shape_J[] = {
    {.x = 0, .y = 0},
    {.x = 0, .y = 1},
    {.x = 1, .y = 1},
    {.x = 2, .y = 1},
};
Point init_center_shape_J = {.x = 1, .y = 1};

Point init_pos_shape_L[] = {
    {.x = 2, .y = 0},
    {.x = 0, .y = 1},
    {.x = 1, .y = 1},
    {.x = 2, .y = 1},
};
Point init_center_shape_L = {.x = 1, .y = 1};

/* Only for hard level */
const int fronzen_level_threshold[] = {5, 10, 15, 20, 25, 30, 32, 34, 36, 38}; // if number of spawned pieces reach any of the threshold, the frozen level increase by 1

void _increase_random_counter() {
  random_counter++;
  if (random_counter >= 2000000000) {
    random_counter = 0;
  }
}

void _print_error_game_mode() {
  print_color("NAK: Unknown command received in ", CMD_COLOR_RED);
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

int _is_back_tick_command() { return str_equal(command, BACK_TICK_STR); }

int _is_enter_or_space_command() { return str_equal(command, SPACE_STR); }

void _clear_game_piece(Piece *piece) {
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    _draw_game_point(points_buffer_angle_rotated[i].x,
                     points_buffer_angle_rotated[i].y, CLEAR);
  }
}

void _draw_game_piece(Piece *piece) {
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);
  for (int i = 0; i < __size; i++) {
    _draw_game_point(points_buffer_angle_rotated[i].x,
                     points_buffer_angle_rotated[i].y, piece->color);
  }
}

void _draw_game_point(int x, int y, Color color) {
  if (y < VIRTUAL_GAME_FIELD_OFFSET) {  // no draw virtual point
    return;
  }

  int real_point_x = x;
  int real_point_y = y - VIRTUAL_GAME_FIELD_OFFSET;

  if (real_point_y >= GAME_FIELD_HEIGHT) return;

  int physical_point_x =
      OFFSET_PHYSICAL_GAME_FIELD_X + real_point_x * BLOCK_SIZE;
  int physical_point_y =
      OFFSET_PHYSICAL_GAME_FIELD_Y + real_point_y * BLOCK_SIZE;
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
  } else if (color == BRICK) {
    drawBrickBlock(physical_point_x, physical_point_y);
  }
}

// init points with center point move out of init center point
Point *_copy_piece_angle_0_points_to_buffer(Piece *piece, Point buffer[]) {
  Point *init_points = _get_init_points(piece);
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
Point *_get_init_points(Piece *piece) {
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

void _adjust_center_point_if_overflow(Piece *piece, Point points[]) {
  int x_adjust = 0;
  int y_adjust = 0;
  int min_x = 9999;
  int max_x = -9999;
  int min_y = 9999;
  int max_y = -9999;
  for (int i = 0; i < __size; i++) {
    min_x = min(min_x, points[i].x);
    max_x = max(max_x, points[i].x);
    min_y = min(min_y, points[i].y);
    max_y = max(max_y, points[i].y);
  }

  if (min_x < 0) x_adjust = (-min_x);
  else if (max_x >= GAME_FIELD_WIDHT) x_adjust = -(max_x - GAME_FIELD_WIDHT + 1);

  int real_min_y = min_y - VIRTUAL_GAME_FIELD_OFFSET;
  int real_max_y = max_y - VIRTUAL_GAME_FIELD_OFFSET;

  if (real_min_y < 0) {
    y_adjust = (-real_min_y - 1);
  } else if (real_max_y >= GAME_FIELD_HEIGHT) {
    y_adjust = -(real_max_y - GAME_FIELD_HEIGHT + 1);
  }

  piece->center_point.x += x_adjust;
  piece->center_point.y += y_adjust;
}

void _copy_piece_rotated_points_to_buffer(Piece *piece, Point buffer[]) {
  Point *_angle_0_init_points =
      _copy_piece_angle_0_points_to_buffer(piece, points_buffer_angle_0);
  for (int i = 0; i < __size; i++) {
    int x_0 = _angle_0_init_points[i].x - piece->center_point.x;
    int y_0 = _angle_0_init_points[i].y - piece->center_point.y;

    int x_1 = x_0 * _get_angle_multiplier_cos(piece->angle) -
              y_0 * _get_angle_multiplier_sin(piece->angle);
    int y_1 = x_0 * _get_angle_multiplier_sin(piece->angle) -
              y_0 * _get_angle_multiplier_cos(piece->angle);

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

void _spawn_random_piece_to(Piece *piece) {
  spawned_pieces++;
  
  int rand_angle = rand(1, 4);
  int rand_color = rand(1, 7);
  int rand_shape = rand(1, 7);
  int rand_center_point_x = rand(0, GAME_FIELD_WIDHT - 1);

  if (rand_angle == 1) {
    piece->angle = ANGLE_0;
  } else if (rand_angle == 2) {
    piece->angle = ANGLE_90;
  } else if (rand_angle == 3) {
    piece->angle = ANGLE_180;
  } else if (rand_angle == 4) {
    piece->angle = ANGLE_270;
  }

  if (rand_color == 1) {
    piece->color = CYAN;
  } else if (rand_color == 2) {
    piece->color = YELLOW;
  } else if (rand_color == 3) {
    piece->color = PURPLE;
  } else if (rand_color == 4) {
    piece->color = GREEN;
  } else if (rand_color == 5) {
    piece->color = RED;
  } else if (rand_color == 6) {
    piece->color = BLUE;
  } else if (rand_color == 7) {
    piece->color = ORANGE;
  }

  if (rand_shape == 1) {
    piece->shape = SHAPE_I;
  } else if (rand_shape == 2) {
    piece->shape = SHAPE_O;
  } else if (rand_shape == 3) {
    piece->shape = SHAPE_T;
  } else if (rand_shape == 4) {
    piece->shape = SHAPE_S;
  } else if (rand_shape == 5) {
    piece->shape = SHAPE_Z;
  } else if (rand_shape == 6) {
    piece->shape = SHAPE_J;
  } else if (rand_shape == 7) {
    piece->shape = SHAPE_L;
  }

  piece->center_point.x = rand_center_point_x;
  piece->center_point.y = 0;

  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  _adjust_center_point_if_overflow(piece, points_buffer_angle_rotated);

  if (is_print_statictics) {
    println("");
    println("");
    println("New piece spawned:");
    
    print_color("+ Shape: ", CMD_COLOR_YEL);
    print_color(_get_shape_str(piece->shape), CMD_COLOR_YEL);
    println("");

    print_color("+ Color: ", CMD_COLOR_GRN);
    print_color(_get_color_str(piece->color), CMD_COLOR_GRN);
    println("");

    print_color("+ Rotation: ", CMD_COLOR_MAG);
    print_color(_get_angle_str(piece->angle), CMD_COLOR_MAG);
    print_color(" degrees", CMD_COLOR_MAG);
    println("");

    print_color("+ Center point: ", CMD_COLOR_BLU);
    print("x = ");
    uart_dec(piece->center_point.x);
    print(", y = ");
    uart_dec(piece->center_point.y);
    println("");
    println("");
  }
}

char* _get_shape_str(Shape shape) {
  if (shape == SHAPE_I) {
    return "I";
  } else if (shape == SHAPE_O) {
    return "O";
  } else if (shape == SHAPE_T) {
    return "T";
  } else if (shape == SHAPE_S) {
    return "S";
  } else if (shape == SHAPE_Z) {
    return "Z";
  } else if (shape == SHAPE_J) {
    return "J";
  } else {
    return "L";
  }
}

char* _get_color_str(Color color) {
  if (color == CYAN) {
    return "CYAN";
  } else if (color == YELLOW) {
    return "YELLOW";
  } else if (color == PURPLE) {
    return "PURPLE";
  } else if (color == GREEN) {
    return "GREEN";
  } else if (color == RED) {
    return "RED";
  } else if (color == BLUE) {
    return "BLUE";
  } else if (color == ORANGE) {
    return "ORANGE";
  } else if (color == CLEAR) {
    return "CLEAR";
  } else if (color == BRICK) {
    return "BRICK";
  }
  return "";
}

char* _get_angle_str(Angle angle) {
  if (angle == ANGLE_0) {
    return "0";
  } else if (angle == ANGLE_90) {
    return "90";
  } else if (angle == ANGLE_180) {
    return "180";
  } else {
    return "270";
  }
  return "";
}

void _draw_next_frame_piece(Piece *piece) {
  Point *points = _get_init_points(piece);
  for (int i = 0; i < __size; i++) {
    int final_x = points[i].x;
    int final_y = points[i].y;
    _adjust_x_y_for_center_drawing_next_frame(&final_x, &final_y, piece->shape);
    _draw_next_frame_point(final_x, final_y, piece->color);
  }
}

void _clear_next_frame_piece(Piece *piece) {
  Point *points = _get_init_points(piece);
  for (int i = 0; i < __size; i++) {
    int final_x = points[i].x;
    int final_y = points[i].y;
    _adjust_x_y_for_center_drawing_next_frame(&final_x, &final_y, piece->shape);
    _draw_next_frame_point(final_x, final_y, CLEAR);
  }
}

void _draw_next_frame_point(int x, int y, Color color) {
  int physical_point_x = OFFSET_PHYSICAL_NEXT_FRAME_X + x * BLOCK_SIZE;
  int physical_point_y = OFFSET_PHYSICAL_NEXT_FRAME_Y + y * BLOCK_SIZE;

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
  } else if (color == BRICK) {
    drawBrickBlock(physical_point_x, physical_point_y);
  }
}

void _adjust_x_y_for_center_drawing_next_frame(int *x, int *y, Shape shape) {
  if (shape == SHAPE_I) {
    *y += 2;
  } else if (shape == SHAPE_O) {
    *x += 1;
    *y += 1;
  } else if (shape == SHAPE_T) {
    *x += 1;
    *y += 1;
  } else if (shape == SHAPE_S) {
    *x += 1;
    *y += 1;
  } else if (shape == SHAPE_Z) {
    *x += 1;
    *y += 1;
  } else if (shape == SHAPE_J) {
    *x += 1;
    *y += 1;
  } else {
    *x += 1;
    *y += 1;
  }
}

void _draw_game_scores(unsigned int score) {
  char score_buffer[10];
  int_to_string_padding(score, score_buffer, 5);
  drawString(602 + 12, 64 * 3/2 + 32, score_buffer, COLOR_YEL, 2);
}

void _clear_game_scores(unsigned int score) {
  char score_buffer[10];
  int_to_string_padding(score, score_buffer, 5);
  drawString(602 + 12, 64 * 3/2 + 32, score_buffer, COLOR_GAME_FIELD_CLEAR, 2);
}

void _init_static_game_field() {
  for (int i = 0; i < GAME_FIELD_FULL_HEIGHT; i++) {
    for (int j = 0; j < GAME_FIELD_WIDHT; j++) {
      static_game_field[i][j].color = CLEAR;
    }
  }
}

int _is_occupied_by_static_field(int x, int y) {
  return static_game_field[y][x].color == CLEAR ? 0 : 1;
}

int _is_point_settle_down_by_overflow_y_or_static_field_point(int x, int y) {
  int check_y = y + 1;
  if (check_y >= GAME_FIELD_FULL_HEIGHT) return 1;
  if (static_game_field[check_y][x].color != CLEAR) return 1;
  return 0;
}

void _transfer_piece_to_static_field(Piece *piece) {
  _copy_piece_rotated_points_to_buffer(piece, points_buffer_angle_rotated);
  int p_x, p_y;
  for (int i = 0; i < __size; i++) {
    p_x = points_buffer_angle_rotated[i].x;
    p_y = points_buffer_angle_rotated[i].y;
    static_game_field[p_y][p_x].color = piece->color;
    
    if (debug) {
      println("");
      print("set static game field: at x = ");
      uart_dec(p_x);
      print(", y = ");
      uart_dec(p_y);
      print(" to color: ");
      print(_get_color_str(piece->color));
      println("");
    }
  }
}

void _copy_piece_data(Piece *from, Piece *to) {
  to->shape = from->shape;
  to->angle = from->angle;
  to->center_point.x = from->center_point.x;
  to->center_point.y = from->center_point.y;
  to->color = from->color;
}

// possibly adjust
void _adjust_complete_rows_and_frozen_rows() {
  int isClearStaticField = 0;
  // int hasAtLeastOneCompleteRow = 0;
  for (int y = 0; y < GAME_FIELD_FULL_HEIGHT; y++) {
    int is_row_complete = 1;
    for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
      if (static_game_field[y][x].color == CLEAR || static_game_field[y][x].color == BRICK) {
        is_row_complete = 0;
        break;
      }
    }
    if (is_row_complete) {
      // hasAtLeastOneCompleteRow = 1;
     
      if (debug) {
        println("row complete");
        uart_dec(y);
        println("");
      }

      if (!isClearStaticField) {
        _clear_static_field();
        isClearStaticField = 1;
      }
      
      _adjust_static_field_on_complete_row(y);

      // score change
      _make_score_change(score_step);
      if (is_print_statictics) {
        _print_score_change(y);
      }

      completed_rows++;
    }
  }

  // frozen blocks
  if (_is_hard_mode()) {
    if (_should_increase_fronzen_level()) {
      if (!isClearStaticField) {
        _clear_static_field();
        isClearStaticField = 1;
      }
      _handle_fronzen_blocks();
    }
  }

  if (isClearStaticField) {
    _draw_static_field();
  }
}

void _adjust_static_field_on_complete_row(int complete_row_idx) {
  for (int y = complete_row_idx; y >= 1; y--) {
    for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
      static_game_field[y][x].color = static_game_field[y - 1][x].color;
    }
  }
  for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
    static_game_field[0][x].color = CLEAR;
  }
}

void _clear_static_field() {
  for (int y = 0; y < GAME_FIELD_FULL_HEIGHT; y++) {
    for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
      if (static_game_field[y][x].color != CLEAR) {
        _draw_game_point(x, y, CLEAR);
      }
    }
  }
}
 
void _draw_static_field() {
  for (int y = 0; y < GAME_FIELD_FULL_HEIGHT; y++) {
    for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
      if (static_game_field[y][x].color != CLEAR) {
        _draw_game_point(x, y, static_game_field[y][x].color);
      }
    }
  }
}

void _print_score_change(int complete_row_number) {
  println("");
  print("Row ");
  uart_dec(complete_row_number - 3 + 1);
  print(" is cleared! Score +");
  uart_dec(score_step);
  println("");
  println("");
  print_prefix();
}

void _make_score_change(int complete_row_number) {
  displayScoreBackground();
  scores += complete_row_number;
  _draw_game_scores(scores);
}

// check last settle down piece is overflowing top
int _is_game_over(Piece *piece) {
  for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
    if (_is_occupied_by_static_field(x, VIRTUAL_GAME_FIELD_OFFSET - 1)) {
      if (is_print_statictics) {
        _print_game_over_statistic();
      }
      return 1;
    }
  }
  return 0;
}

void _handle_fronzen_blocks() {
  for (int y = 1; y <= GAME_FIELD_FULL_HEIGHT - frozen_level - 1; y++) {
    for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
      static_game_field[y - 1][x].color = static_game_field[y][x].color;
    }
  }

  // turn to brick
  for (int x = 0; x < GAME_FIELD_WIDHT; x++) {
    static_game_field[GAME_FIELD_FULL_HEIGHT - frozen_level - 1][x].color = BRICK;
  }

  frozen_level++;

  if (is_print_statictics) {
    println("");
    print_color("Game difficulty adjust: increasing frozen level", CMD_COLOR_YEL);
    println("");
  }
}

int _should_increase_fronzen_level() {
  for (int i = 0; i < sizeof(fronzen_level_threshold)/sizeof(int); i++) {
    if (spawned_pieces == fronzen_level_threshold[i]) return 1;
  }
  return 0;
}

void _print_game_over_statistic() {
  println("");
  println("");
  println_color("***************************************", CMD_COLOR_YEL);
  println_color("***************************************", CMD_COLOR_YEL);
  println("");
  println_color("GAME OVER! ", CMD_COLOR_RED);
  
  print_color("Final scores: ", CMD_COLOR_GRN);
  uart_dec(scores);
  println("");

  print_color("Rows clear: ", CMD_COLOR_GRN);
  uart_dec(completed_rows);
  println("");

  print_color("Spawned pieces: ", CMD_COLOR_GRN);
  uart_dec(spawned_pieces);
  println("");

  print_color("Total commands received in game: ", CMD_COLOR_GRN);
  uart_dec(total_received_commands);
  println("");

  println("");
  println_color("***************************************", CMD_COLOR_YEL);
  println_color("***************************************", CMD_COLOR_YEL);
  println("");
  println("");
  
  print_prefix();
}
