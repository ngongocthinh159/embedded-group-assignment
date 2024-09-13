#pragma once

#include "game/game.h"

extern Point points_buffer_angle_rotated[];
extern Point points_buffer_angle_0[];
extern Point center_point_buffer;

extern Point init_pos_shape_I[];
extern Point init_center_shape_I;
extern Point init_pos_shape_O[];
extern Point init_center_shape_O;
extern Point init_pos_shape_T[];
extern Point init_center_shape_T;
extern Point init_pos_shape_S[];
extern Point init_center_shape_S;
extern Point init_pos_shape_Z[];
extern Point init_center_shape_Z;
extern Point init_pos_shape_J[];
extern Point init_center_shape_J;
extern Point init_pos_shape_L[];
extern Point init_center_shape_L;

/* Function prototypes */
void _print_error_game_mode();
int _is_up_command();
int _is_down_command();
int _is_left_command();
int _is_right_command();
int _is_back_tick_command();
int _is_enter_or_space_command();

// Drawing
void _draw_game_piece(Piece *piece);
void _clear_game_piece(Piece *piece);
void _draw_game_point(int x, int y, Color color);

void _draw_next_frame_piece(Piece *piece);
void _clear_next_frame_piece(Piece *piece);
void _draw_next_frame_point(int x, int y, Color color);
void _adjust_x_y_for_center_drawing_next_frame(int *x, int *y, Shape shape);

void _draw_game_scores(unsigned int score);

// Game play utils
void _spawn_random_piece_to(Piece *piece); // spawn a random piece into piece buffer passed in
void _rotate_piece(Piece *piece); // rotate only Angle attribute
Point* _get_init_points(Piece *piece); // init points (0 xy and 0 rotation) based on piece's shape
Point _get_init_center_point(Piece *piece); // init center point (0 xy and 0 rotation) based on piece's shape
Point* _copy_piece_angle_0_points_to_buffer(Piece *piece, Point buffer[]); // copy points at angle 0 but have center point offset to buffer
void _copy_piece_rotated_points_to_buffer(Piece *piece, Point buffer[]); // copy points at rotate angle and have center point offset to buffer
void _adjust_center_point_if_overflow(Piece *piece, Point points[]); // adjust center point if rotate angle points overflow
int _get_angle_multiplier_sin(Angle angle);
int _get_angle_multiplier_cos(Angle angle);
