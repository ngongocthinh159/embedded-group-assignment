#pragma once

#define TAB 9
#define ESC 27
#define BACKSPACE 127
#define ENTER 10
#define SPACE 32
#define BACK_TICK 96

#define BACKSPACE_STR "\b"
#define SPACE_STR " "
#define BACK_TICK_STR "`"

#define UP_ARROW 'A'
#define DOWN_ARROW 'B'
#define RIGHT_ARROW 'C'
#define LEFT_ARROW 'D'

// ansi escape sequence
#define CSI_CUU "[A" // up
#define CSI_CUD "[B" // down
#define CSI_CUF "[C" // right
#define CSI_CUB "[D" // left

// special characters
#define REMOVE_A_CHAR "\b \b"
