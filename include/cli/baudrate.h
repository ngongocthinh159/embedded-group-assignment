#pragma once

typedef struct {
  char *toString;
  unsigned int value;
  unsigned int UART1_BAUDRATE_REG_value;
  unsigned int UART0_IBRD_value;
  unsigned int UART0_FBRD_value;
} Baudrate;

extern Baudrate valid_baudrates[];
extern Baudrate* current_baudrate;
extern unsigned int current_stopbits;
extern Baudrate invalid_baudrate;

Baudrate* get_baudrate(const char* inputString);
int is_valid_baudrate(Baudrate* baudrate);
int number_of_valid_baudrates();
