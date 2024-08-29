#include "cli/command.h"
#define CMD_STOPBIT "stopbit"

char* all_commands[] = {
    CMD_SWITCH_TO_VIDEO_PLAYER_MODE,
    CMD_SWITCH_TO_GAME_MODE,
    CMD_EXIT,
    CMD_HELP,
    CMD_CLEAR,
    CMD_SHOW_INFO,
    CMD_STOPBIT,
    CMD_VIDEO_RESUME,
    CMD_VIDEO_PAUSE,
    CMD_SHOW_IMAGE,
    CMD_BAUDRATE,
};
void auto_complete_command() {

}
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
