#pragma once

typedef struct {
  int call_every_ms; // should be mutiple of 10
  void (*handler)();
  volatile int counter;
} Event;

void handle_game_mode();
void _print_error_game_mode();
int _is_up_command();
int _is_down_command();
int _is_left_command();
int _is_right_command();
int _is_back_tick_command();
int _is_enter_or_space_command();
void _exit_game();
void _print_error_game_mode();
