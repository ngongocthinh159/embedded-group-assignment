#include "game/game.h"
#include "game/game-utils.h"
#include "game/game-screen-welcome.h"
#include "game/game-screen-how-to-play.h"
#include "game/game-screen-over.h"
#include "lib/framebf.h"
#include "util/tty.h"
#include "util/string.h"

//  index 0: choose "NEW GAME"      index 1: choose "EXIT"
volatile int current_game_over_idx = 0;

int handle_screen_game_over() {
  int is_handled = 1;
  if (_is_up_command()) {
    println("ACK: UP");

  } else if (_is_down_command()) {
    println("ACK: DOWN");

  } else if (_is_left_command()) {
    println("ACK: LEFT");
    if (current_game_over_idx == 1) {
      current_game_over_idx--;
      _draw_pop_up();
    }
  } else if (_is_right_command()) {
    println("ACK: RIGHT");
    if (current_game_over_idx == 0) {
      current_game_over_idx++;
      _draw_pop_up();
    }
  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
    if (current_game_over_idx == 0) {
      switch_to_game_play_screen();
    } else if (current_game_over_idx == 1) {
      switch_to_welcome_screen();
    }
  } else if (_is_back_tick_command()) {
    println("ACK: BACK TICK");
    switch_to_welcome_screen();
  } else {
    is_handled = 0;
  }

  if (is_handled) {

  }

  return is_handled;
}

void switch_to_game_over_screen() {
  current_game_over_idx = 0;
  current_screen = SCREEN_GAME_OVER;
  _draw_pop_up();
}

void _draw_pop_up() {
  char score_str[10];
  int_to_string_padding(scores, score_str, 5);
  displayGameOverPopUp(current_game_over_idx, score_str);
}
