#include "cli/cli.h"
#include "cli/help_text.h"
#include "cli/welcome_text.h"
#include "cli/command.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"
#include "lib/stack.h"
#include "lib/color.h"
#include "lib/uart/uart_common.c"

char *OS_NAME = "FIRE_OS";
char *GROUP_NAME = "FIRE OS";

const unsigned int COMMAND_MAX_SIZE = 200;
char command[201]; // global buffer for command shell: after strip from stack buffer (size + 1 for '\0' character)

char stack_buffer[200]; // global buffer for command shell: record all user input
Stack *cmd_st; // wrapper for command shell buffer (when wrapped the buffer can be treated as a stack and use like a stack)

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
    print_color(OS_NAME, CMD_COLOR_RED);
    print_color(" > ", CMD_COLOR_RED);
}


void set_baudrate(int rate) {
    uart_set_baudrate(rate);
    print("Baudrate set to ");
    println(int_to_str(rate));
}

void set_stopbit(int stopbit) {
    uart_set_stopbit(stopbit);
    print("Stopbit set to ");
    println(int_to_str(stopbit));
}

void _handle_internal() {
    if (str_equal(command, CMD_HELP)) {
        print(help_text);
    } else if (str_equal(command, CMD_EXIT)) {
        // exit();
    } else if (str_equal(command, CMD_CLEAR)) {
        clrscr();
    } else if (str_equal(command, CMD_SHOW_INFO)) {
        showinfo();
    } else if (str_equal(command, CMD_STOPBIT)) {
        // Extract the stop bit value and set it
        int stopbit = extract_value_from_command(command, CMD_STOPBIT);
        set_stopbit(stopbit);
    }
}

int extract_value_from_command(char *command, char *cmd_type) {
    char *value_str = command + strlen(cmd_type) + 1; // Skip past the command and space
    return atoi(value_str);
}

void handle_cli_mode() {
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
            _handle_internal();

            print_prefix();
        }
    }
}

void switch_to_cli_mode() { current_mode = CLI; }

void switch_to_video_player_mode() { current_mode = VIDEO_PLAYER; }

void switch_to_game_mode() { current_mode = GAME; }

int get_current_mode() { return current_mode; }

int is_cli_mode() { return current_mode == CLI; }

int is_video_player_mode() { return current_mode == VIDEO_PLAYER; }

int is_game_mode() { return current_mode == GAME; }
