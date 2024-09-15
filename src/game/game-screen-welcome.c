#include "game/game-screen-welcome.h"

#include "cli/cli.h"
#include "game/game-screen-how-to-play.h"
#include "game/game-utils.h"
#include "game/game.h"
#include "lib/color.h"
#include "lib/framebf.h"
#include "lib/uart.h"
#include "util/tty.h"

// 0: new game
// 1: difficulty
// 2: how to play
// 3: exit game
int current_welcome_index = 0;

int handle_screen_welcome() {
  int is_handled = 1;
  if (_is_up_command()) {
    println("ACK: UP");
    if (current_welcome_index != 0) {
      current_welcome_index--;
    }
  } else if (_is_down_command()) {
    println("ACK: DOWN");
    if (current_welcome_index != 3) {
      current_welcome_index++;
    }
  } else if (_is_left_command()) {
    println("ACK: LEFT");
    if (current_welcome_index == 1) {
      if (current_difficulty > 0) current_difficulty--;
    }
  } else if (_is_right_command()) {
    println("ACK: RIGHT");
    if (current_welcome_index == 1) {
      if (current_difficulty < 2) current_difficulty++;
    }
  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
    if (current_welcome_index == 0) {
      switch_to_game_play_screen();
    } else if (current_welcome_index == 1) {
      current_difficulty++;
      current_difficulty = current_difficulty%3;
    } else if (current_welcome_index == 2) {
      switch_to_how_to_play_screen();
    } else if (current_welcome_index == 3) {
      switch_to_cli_mode();
    }
  } else if (_is_back_tick_command()) {
    println("ACK: BACK TICK");
    switch_to_cli_mode();
  } else {
    is_handled = 0;
  }

  if (is_handled) {
    if (current_screen == SCREEN_WELCOME && is_game_mode())
      draw_current_welcome_screen();
  }

  return is_handled;
}

void draw_current_welcome_screen() {
  displayWelcomeScreen(current_welcome_index);
  displayDifficultyText();
}

void reset_welcome_screen_state() { current_welcome_index = 0; }

void _toggle_difficulty() {
  if (current_difficulty == 0)
    current_difficulty = 1;
  else
    current_difficulty = 0;
}

void displayDifficultyText() {
  drawString(660, 440,
             current_difficulty == 0
                 ? "Easy"
                 : (current_difficulty == 1 ? "Med" : "Hard"),
             COLOR_WHITE, 2);
}

void switch_to_welcome_screen() {
  current_screen = SCREEN_WELCOME;
  draw_current_welcome_screen();
}
