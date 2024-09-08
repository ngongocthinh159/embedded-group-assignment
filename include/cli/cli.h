#pragma once

#include "lib/stack.h"

#define COMMAND_MAX_SIZE 200

enum MODE {
  CLI,
  VIDEO_PLAYER,
  GAME
};

extern volatile int current_mode;

extern char command[];

extern char stack_buffer[];
extern Stack *cmd_st;

extern char auto_complete_buffer[];
extern Stack *auto_complete_st;

void showinfo();
void _handle_baudrate_config();
void _handle_stopbit_config();
void _print_list_of_valid_baudrates();
void _print_current_baudrate_and_stopbit();
int welcome();
void handle_cli_mode();
void print_prefix();
void switch_to_cli_mode();
void switch_to_video_player_mode();
void switch_to_game_mode();
int get_current_mode();
int is_cli_mode();
int is_video_player_mode();
int is_game_mode();
void _print_help();
