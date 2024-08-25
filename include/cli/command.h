#pragma once

// flow control command
#define CMD_SWITCH_TO_VIDEO_PLAYER_MODE "playvideo"
#define CMD_SWITCH_TO_GAME_MODE "playgame"
#define CMD_SHOW_IMAGE "displayimage"
#define CMD_EXIT "exit"

// cli command
#define CMD_HELP "help"
#define CMD_CLEAR "clear"
#define CMD_SHOW_INFO "showinfo"
#define CMD_HISTORY "history"

#define CMD_ARROW_UP "arrow_up"
#define CMD_ARROW_DOWN "arrow_down"
#define CMD_ARROW_RIGHT "arrow_right"
#define CMD_ARROW_LEFT "arrow_left"

// video player control command
#define CMD_VIDEO_RESUME "resume"
#define CMD_VIDEO_PAUSE "pause"

extern char *all_commands[];

void auto_complete_command();
