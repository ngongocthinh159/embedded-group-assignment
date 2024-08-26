#include "cli/cli.h"

#include "cli/help_text.h"
#include "cli/welcome_text.h"
#include "lib/command.h"
#include "lib/kernel.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"

char *OS_NAME = "os_name";
char *GROUP_NAME = "6";

volatile int current_mode = CLI;  // mode switching in kernel.c

int welcome() {
    println(welcome_text);
    println("");
    println("");
    print("Group ");
    print(GROUP_NAME);
    println(" - EEET2490 - Embedded System");
    println("");
    return 0;
}

void print_prefix() {
    print(OS_NAME);
    print(" > ");
}

int _handle_internal() {
    int x;
    if (str_equal(command, CMD_HELP)) {
        print(help_text);
            x = 0;
    } else if (str_equal(command, CMD_EXIT)) {
        // exit();
        x= -1;
    } else if (str_equal(command, CMD_CLEAR)) {
        clrscr();
        x = 0;
    }
    return x;
}

int handle_cli_mode() {
    int handle_cli_mode_exit = 0;
    uart_puts("\n\nCLI mode!\n");
    print_prefix();
    while (is_cli_mode()) {
        uart_scanning();  // always scanning for new char

        if (is_there_new_line()) {
            get_line(command);

            print("Command received: ");
            println(command);

            // flow control
            if (str_equal(command, CMD_SWITCH_TO_VIDEO_PLAYER_MODE)) {
                switch_to_video_player_mode();
                break;
            } else if (str_equal(command, CMD_SWITCH_TO_GAME_MODE)) {
                switch_to_game_mode();
                break;
            }

            // handle command in cli mode
            int _handle_internal_return = _handle_internal();
            //print(_handle_internal_return);
            if(_handle_internal_return ==-1){
                handle_cli_mode_exit = _handle_internal_return;
                break;
            }else{
                handle_cli_mode_exit = 0;
            }
            print_prefix();
        }
    }
    return handle_cli_mode_exit;

}

void switch_to_cli_mode() { current_mode = CLI; }

void switch_to_video_player_mode() { current_mode = VIDEO_PLAYER; }

void switch_to_game_mode() { current_mode = GAME; }

int get_current_mode() { return current_mode; }

int is_cli_mode() { return current_mode == CLI; }

int is_video_player_mode() { return current_mode == VIDEO_PLAYER; }

int is_game_mode() { return current_mode == GAME; }
