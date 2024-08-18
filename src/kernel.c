#include "lib/mbox.h"
#include "lib/uart.h"

void main() {
  // set up serial console
  uart_init();
  // say hello
  uart_puts("Hello World!\n");
  // mailbox data buffer: Read ARM frequency
  mBuf[0] =
      8 *
      4; // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
  mBuf[2] = 0x00030002;   // TAG Identifier: Get clock rate
  mBuf[3] =
      8; // Value buffer size in bytes (max of request and response lengths)
  mBuf[4] = 0; // REQUEST CODE = 0
  mBuf[5] = 3; // clock id: ARM system clock
  mBuf[6] = 0; // clear output buffer (response data are mBuf[5] & mBuf[6])
  mBuf[7] = MBOX_TAG_LAST;
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    uart_puts("Response Code for whole message: ");
    uart_hex(mBuf[1]);
    uart_puts("\n");
    uart_puts("Response Code in Message TAG: ");
    uart_hex(mBuf[4]);
    uart_puts("\n");
    uart_puts("DATA: ARM clock rate = ");
    uart_dec(mBuf[6]);
    uart_puts("\n");
  } else {
    uart_puts("Unable to query!\n");
  }
  while (1) {
    // read each char
    char c = uart_getc();
    // send back
    uart_sendc(c);
  }
}
