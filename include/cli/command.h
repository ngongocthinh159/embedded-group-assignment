#pragma once

typedef struct {
  char *name;
  int length;
  char *help;
  char *full_help;
  void (*fn)();
} Command;

// flow control command
#define CMD_SWITCH_TO_VIDEO_PLAYER_MODE "playvideo"
#define CMD_SWITCH_TO_GAME_MODE "playgame"
#define CMD_SHOW_IMAGE "displayimage"
#define CMD_EXIT "exit"
#define CMD_CURRENT_MODE "currentmode"

// cli command
#define CMD_HELP "help"
#define CMD_CLEAR "clear"
#define CMD_SHOW_INFO "showinfo"
#define CMD_HISTORY "history"
#define CMD_BAUDRATE_PREFIX "baudrate"
#define CMD_STOPBIT_PREFIX "stopbit"

// video player control command
#define CMD_VIDEO_RESUME "resume"
#define CMD_VIDEO_PAUSE "pause"

int resolve_command(char *command);

extern Command all_commands[];
extern int all_commands_size;

