#pragma once

#include "lib/stack.h"

enum MODE {
  CLI,
  VIDEO_PLAYER,
  GAME
};

extern volatile int current_mode;

extern const unsigned int COMMAND_MAX_SIZE;
extern char command[];
extern char stack_buffer[];
extern Stack *cmd_st;

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
