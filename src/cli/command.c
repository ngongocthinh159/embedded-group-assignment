#include "cli/command.h"

Command all_commands[] = {
  { .name = CMD_SWITCH_TO_VIDEO_PLAYER_MODE, .length = 9, .help = "Play a video" },
  { .name = CMD_SWITCH_TO_GAME_MODE, .length = 8, .help = "Initialise pre-installed video game" },
  { .name = CMD_EXIT, .length = 4, .help = "Exit the OS" },

  { .name = CMD_HELP, .length = 4, .help = "Show help message" },
  { .name = CMD_CLEAR, .length = 5, .help = "Clear current shell" },
  { .name = CMD_SHOW_INFO, .length = 8, .help = "Show hardware information" },

  { .name = CMD_VIDEO_RESUME, .length = 6, .help = "" },
  { .name = CMD_VIDEO_PAUSE, .length = 5, .help = "" },

  { .name = CMD_SHOW_IMAGE, .length = 12, .help = "Display an image" },
};

int all_commands_size = 9;

