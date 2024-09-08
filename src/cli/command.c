#include "cli/command.h"
#include "cli/cli.h"
#include "lib/framebf.h"
#include "util/tty.h"

Command all_commands[] = {
  // flow control
  { .name = CMD_SWITCH_TO_VIDEO_PLAYER_MODE, .length = 9, .help = "Switch to video play mode (available in: CLI mode)" },
  { .name = CMD_SWITCH_TO_GAME_MODE, .length = 8, .help = "Switch to game play mode (available in: CLI and Video modes)" },
  { .name = CMD_EXIT, .length = 4, .help = "Exit the current program's mode (available in: Video modes)" },

  // global command: work for CLI mode and video player mode
  { .name = CMD_HELP, .length = 4, .help = "Show help message (available in: CLI and Video modes)", .fn = _print_help },
  { .name = CMD_CLEAR, .length = 5, .help = "Clear current shell (available in: CLI and Video modes)", .fn = clrscr },
  { .name = CMD_SHOW_INFO, .length = 8, .help = "Show hardware information (available in: CLI and Video modes)", .fn = showinfo },
  { .name = CMD_CURRENT_MODE, .length = 12, .help = "Show the current program's mode (available in: CLI and Video modes)" },

  // only work for video player mode
  { .name = CMD_VIDEO_RESUME, .length = 6, .help = "Resume video player (available in: Video mode)" },
  { .name = CMD_VIDEO_PAUSE, .length = 5, .help = "Stop video player (available in: Video mode)" },

  // only work for cli mode
  { .name = CMD_SHOW_IMAGE, .length = 12, .help = "Display an image with team member's names (available in: CLI mode)", .fn = displayWelcomeImage },
  { .name = CMD_BAUDRATE_PREFIX, .length = 8, .help = "Change board baud rate (available in: CLI mode)", .fn = _handle_baudrate_config },
  { .name = CMD_STOPBIT_PREFIX, .length = 7, .help = "Change board stopbit (available in: CLI mode)", .fn = _handle_stopbit_config },
};

int all_commands_size = 12;
