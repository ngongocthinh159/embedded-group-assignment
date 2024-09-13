#include "game/game.h"
#include "lib/framebf.h"
#include "util/tty.h"
#include "lib/uart.h"
#include "lib/color.h"
#include "cli/cli.h"
#include "game/game-screen-welcome.h"

// 0: new game
// 1: difficulty
// 2: how to play
// 3: exit game
int current_index = 0;
int current_difficulty = 0; // 0: easy, 1: hard

int handle_screen_welcome() {
  int is_handled = 1;
  if (_is_up_command()) {
    println("ACK: UP");
    if (current_index != 0) {
      current_index--;
    }
  } else if (_is_down_command()) {
    println("ACK: DOWN");
    if (current_index != 3) {
      current_index++;
    }
  } else if (_is_left_command()) {
    println("ACK: LEFT");
    if (current_index == 1) {
      _toggle_difficulty();
    }
  } else if (_is_right_command()) {
    println("ACK: RIGHT");
    if (current_index == 1) {
      _toggle_difficulty();
    }
  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
    if (current_index == 1) {
      _toggle_difficulty();
    } else if (current_index == 3) {
      _back_to_cli();
    }
  } else if (_is_back_tick_command()) {
    println("ACK: BACK TICK");
    _back_to_cli();
  } else {
    is_handled = 0;
  }

  if (is_handled) {
    displayWelcomeScreen(current_index);
    displayDifficultyText();
    uart_dec(current_index);
  }

  return is_handled;
}

void _toggle_difficulty() {
  if (current_difficulty == 0) current_difficulty = 1;
  else current_difficulty = 0;
}

void displayDifficultyText() {
  drawString(660, 440, current_difficulty == 0 ? "Easy" : "Hard", COLOR_WHITE, 2);
}

void _back_to_cli() {
  drawRectARGB32(0, 0, width - 1, height - 1, COLOR_BLACK, 1);
  switch_to_cli_mode();
}
