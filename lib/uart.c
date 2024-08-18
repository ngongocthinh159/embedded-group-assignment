#include "lib/uart.h"
#include "lib/gpio.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init() {
  unsigned int r;
  /* initialize UART */
  // enable mini UART (UART1)
  AUX_ENABLE |= 1;
  // stop transmitter and receiver
  AUX_MU_CNTL = 0;
  // 8-bit mode (also enable bit 1 to be used for RBP3)
  AUX_MU_LCR = 3;
  // clear RTS (request to send)
  AUX_MU_MCR = 0;
  // disable interrupts
  AUX_MU_IER = 0;
  // enable and clear FIFOs
  AUX_MU_IIR = 0xc6;
  // configure 115200 baud [system_clk_freq/(baud_rate*8) - 1]
  AUX_MU_BAUD = 270;
  // map UART1 to GPIO pins 14 and 15
  r = GPFSEL1;
  // clear bits 17-12 (FSEL15, FSEL14)
  r &= ~((7 << 12) | (7 << 15));
  // set value 2 (select ALT5: TXD1/RXD1)
  r |= (2 << 12) | (2 << 15);
  GPFSEL1 = r;
  // enable GPIO 14, 15. no pull up/down control
  GPPUD = 0;

  // wait 150 cycles
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // enable clock for GPIO 14, 15
  GPPUDCLK0 = (1 << 14) | (1 << 15);

  // wait 150 cycles
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // flush GPIO setup
  GPPUDCLK0 = 0;
  // enable transmitter and receiver (Tx, Rx)
  AUX_MU_CNTL = 3;
}

/**
 * Send a character
 */
void uart_sendc(char c) {
  // wait until transmitter is empty
  do {
    asm volatile("nop");
  } while (!(AUX_MU_LSR & 0x20));

  // write the character to the buffer
  AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc() {
  char c;

  // wait until data is ready (one symbol)
  do {
    asm volatile("nop");
  } while (!(AUX_MU_LSR & 0x01));

  // read it and return
  c = (unsigned char)(AUX_MU_IO);
  // convert carriage return to newline character
  return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
  while (*s) {
    // convert newline to carriage return + newline
    if (*s == '\n')
      uart_sendc('\r');
    uart_sendc(*s++);
  }
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num) {
  uart_puts("0x");
  for (int pos = 28; pos >= 0; pos = pos - 4) {
    // Get highest 4-bit nibble
    char digit = (num >> pos) & 0xF;
    /* Convert to ASCII code */
    // 0-9 => '0'-'9', 10-15 => 'A'-'F'
    digit += (digit > 9) ? (-10 + 'A') : '0';
    uart_sendc(digit);
  }
}

/**
 * Display a value in decimal format
 */
void uart_dec(int num) {
  // A string to store the digit characters
  char str[33] = "";
  // Calculate the number of digits
  int len = 1;
  int temp = num;
  while (temp >= 10) {
    len++;
    temp = temp / 10;
  }
  // Store into the string and print out
  for (int i = 0; i < len; i++) {
    // get last digit
    int digit = num % 10;
    // remove last digit from the number
    num = num / 10;
    str[len - (i + 1)] = digit + '0';
    str[len] = '\0';
    uart_puts(str);
  }
}
