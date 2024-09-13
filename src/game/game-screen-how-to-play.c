#include "game/game.h"
#include "game/game-utils.h"
#include "game/game-screen-welcome.h"
#include "game/game-screen-how-to-play.h"
#include "lib/framebf.h"
#include "util/tty.h"

int handle_screen_how_to_play() {
  int is_handled = 1;
  if (_is_up_command()) {
    println("ACK: UP");

  } else if (_is_down_command()) {
    println("ACK: DOWN");

  } else if (_is_left_command()) {
    println("ACK: LEFT");

  } else if (_is_right_command()) {
    println("ACK: RIGHT");

  } else if (_is_enter_or_space_command()) {
    println("ACK: SPACE or ENTER");
    switch_to_welcome_screen();
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

void switch_to_how_to_play_screen() {
  current_screen = SCREEN_HOW_TO_PLAY;
  displayHowToPlayScreen();
}
