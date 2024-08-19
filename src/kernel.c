#include "all.h"

char* OS_NAME = "os_name";

void print_prefix() {
  print(OS_NAME);
  print(" > ");
}

void main()
{
  char shell_buffer[256];

  // set up serial console
  uart_init();

  clrscr();
  welcome();
  print_prefix();

  char command[201];

  while (1)
  {
    uart_scanning(); // always scanning for new char

    if (is_there_new_line()) {
      get_line(command);

      print("Command received: ");
      println(command);

      if (str_equal(command, CMD_VIDEO_PLAYER)) {
        println("play video now..");
      }

      print_prefix();
    }
  }
}
