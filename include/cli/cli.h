#pragma once

enum MODE {
  CLI,
  VIDEO_PLAYER,
  GAME
};

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
