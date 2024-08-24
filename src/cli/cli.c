#include "cli/cli.h"
#include "lib/command.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"
#include "cli/welcome_text.h"

char *OS_NAME = "os_name";
char *GROUP_NAME = "6";

int current_mode = CLI;

int welcome()
{
  println(welcome_text);
  println("");
  println("");
  print("Group ");
  print(GROUP_NAME);
  println(" - EEET2490 - Embedded System");
  println("");
  println("");
  return 0;
}

void print_prefix()
{
  print(OS_NAME);
  print(" > ");
}

extern int current_mode; // mode switching in kernel.c

void handle_cli_mode()
{
  char command[201];

  while (is_cli_mode())
  {
    uart_scanning(); // always scanning for new char

    if (is_there_new_line())
    {
      get_line(command);

      print("Command received: ");
      println(command);

      if (str_equal(command, CMD_SWITCH_TO_VIDEO_PLAYER_MODE))
      {
        switch_to_video_player_mode();
        break;
      } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
        switch_to_game_mode();
        break;
      }

      // handle command in cli mode

      print_prefix();
    }
  }
}

void switch_to_cli_mode()
{
  current_mode = CLI;
}

void switch_to_video_player_mode()
{
  current_mode = VIDEO_PLAYER;
}

void switch_to_game_mode()
{
  current_mode = GAME;
}

int get_current_mode()
{
  return current_mode;
}

int is_cli_mode()
{
  return current_mode == CLI;
}

int is_video_player_mode()
{
  return current_mode == VIDEO_PLAYER;
}

int is_game_mode()
{
  return current_mode == GAME;
}
