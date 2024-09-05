#include "cli/command.h"
#include "cli/cli.h"
#include "lib/framebf.h"
#include "util/tty.h"

Command all_commands[] = {
  { .name = CMD_SWITCH_TO_VIDEO_PLAYER_MODE, .length = 9, .help = "Play a video" },
  { .name = CMD_SWITCH_TO_GAME_MODE, .length = 8, .help = "Initialise pre-installed video game" },
  { .name = CMD_EXIT, .length = 4, .help = "Exit the OS" },

  { .name = CMD_HELP, .length = 4, .help = "Show help message", .fn = _print_help },
  { .name = CMD_CLEAR, .length = 5, .help = "Clear current shell", .fn = clrscr },
  { .name = CMD_SHOW_INFO, .length = 8, .help = "Show hardware information", .fn = showinfo },

  { .name = CMD_VIDEO_RESUME, .length = 6, .help = "" },
  { .name = CMD_VIDEO_PAUSE, .length = 5, .help = "" },

  { .name = CMD_SHOW_IMAGE, .length = 12, .help = "Display an image", .fn = displayWelcomeImage },

  { .name = CMD_BAUDRATE_PREFIX, .length = 8, .help = "Change board baud rate", .fn = _handle_baudrate_config },
  { .name = CMD_STOPBIT_PREFIX, .length = 7, .help = "Change board stopbit", .fn = _handle_stopbit_config },
};

int all_commands_size = 11;

