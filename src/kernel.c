#include "cli/welcome.h"
#include "lib/mbox.h"
#include "lib/uart.h"
#include "util/string.h"
#include "util/tty.h"

void main() {
  char shell_buffer[256];

  // set up serial console
  uart_init();

  clrscr();
  welcome();
  print("shell > ");

  while (1) {
    // read each char
    char c = uart_getc();
    // send back
    uart_sendc(c);

    if (c == '\n') {
      println(shell_buffer);
      clean(shell_buffer);
      print("shell > ");
    } else if (c) {
      push(shell_buffer, c);
    }
  }
}
