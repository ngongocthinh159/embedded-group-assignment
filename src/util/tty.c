#include "util/tty.h"
#include "lib/uart.h"
#include "lib/color.h"

char buff[200];

void print(char *str) {
  char *c = str;
  while (*c)
    uart_sendc(*c++);
}

void println(char *str) {
  // carriage return + line feed
  // https://developer.mozilla.org/en-US/docs/Glossary/CRLF
  print(str);
  print("\r\n");
}

void print_color(char *str, char *color_code) {
    int i = 0;

    char *c = color_code;
    while (*c) {
        buff[i++] = *c;
        c++;
    }
        

    c = str;
    while (*c) {
        buff[i++] = *c;
        c++;
    }

    c = CMD_COLOR_RESET;
    while (*c) {
        buff[i++] = *c;
        c++;
    }
    
    buff[i] = '\0';

    uart_puts(buff);
}

void println_color(char *str, char *color_code) {
    print_color(str, color_code);
    print("\r\n");
}

void clrscr() {
  // send H + J for clearing the screen
  // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#cursor-positioning
  char clr_sequence[] = "\033[H";
  print(clr_sequence);
  char home_sequence[] = "\033[J";
  print(home_sequence);
}
