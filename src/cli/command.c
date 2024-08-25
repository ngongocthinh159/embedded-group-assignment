#include "cli/command.h"

int all_commands_size = 8;
char *all_commands[] = {
    CMD_SWITCH_TO_VIDEO_PLAYER_MODE,
    CMD_SWITCH_TO_GAME_MODE,
    CMD_EXIT,
    CMD_HELP,
    CMD_CLEAR,
    CMD_SHOW_INFO,
    CMD_VIDEO_RESUME,
    CMD_VIDEO_PAUSE,
    CMD_SHOW_IMAGE,
};
int cmd_len[] = {
    9,
    8,
    4,
    
    4,
    5,
    8,

    6,
    5
};
