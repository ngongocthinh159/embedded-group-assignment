#include "lib/uart.h"

void print(char *str) {
  char *c = str;
  while (*c)
    uart_sendc(*c++);
}

void println(char *str) {
  // carriage return + line feed
  // https://developer.mozilla.org/en-US/docs/Glossary/CRLF
  char crlf[] = "\r\n";
  print(str);
  print(crlf);
}

void clrscr() {
  // send H + J for clearing the screen
  // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#cursor-positioning
  char clr_sequence[] = "\033[H";
  print(clr_sequence);
  char home_sequence[] = "\033[J";
  print(home_sequence);
}
