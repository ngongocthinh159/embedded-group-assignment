#include "lib/uart.h"
#include "lib/mbox.h"
#include "lib/gpio.h"
#include "cli/baudrate.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* NEW: set up UART clock for consistent divisor values
	--> may not work with QEMU, but will work with real board */
	mBuf[0] = 9 * 4;
	mBuf[1] = MBOX_REQUEST;
	mBuf[2] = MBOX_TAG_SETCLKRATE; // set clock rate
	mBuf[3] = 12;				   // Value buffer size in bytes
	mBuf[4] = 0;				   // REQUEST CODE = 0
	mBuf[5] = 2;				   // clock id: UART clock
	mBuf[6] = 48000000;			   // rate: 48Mhz
	mBuf[7] = 0;				   // clear turbo
	mBuf[8] = MBOX_TAG_LAST;
	mbox_call_no_print(ADDR(mBuf), MBOX_CH_PROP);

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RBP3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}								   // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}			   // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	// 115200 baud with uart0_clock = 48Mhz
	UART0_IBRD = 26;
  UART0_FBRD = 3;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_sendc(char c)
{

	/* Check Flags Register */
	/* And wait until transmitter is not full */
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF);

	/* Write our data byte out to the data register */
	UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
	char c = 0;

	/* Check Flags Register */
	/* Wait until Receiver is not empty
	 * (at least one byte data in receive fifo)*/
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_RXFE);

	/* read it and return */
	c = (unsigned char)(UART0_DR);

	/* convert carriage return to newline */
	return (c == '\r' ? '\n' : c);
}

// Check if the user has just inputted a new key
unsigned int uart_isReadByteReady()
{
	return (!(UART0_FR & UART0_FR_RXFE));
}

void uart_init_with_config(Baudrate* baudrate, unsigned int numberOfStopBits) {
  unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* NEW: set up UART clock for consistent divisor values
	--> may not work with QEMU, but will work with real board */
	mBuf[0] = 9 * 4;
	mBuf[1] = MBOX_REQUEST;
	mBuf[2] = MBOX_TAG_SETCLKRATE; // set clock rate
	mBuf[3] = 12;				   // Value buffer size in bytes
	mBuf[4] = 0;				   // REQUEST CODE = 0
	mBuf[5] = 2;				   // clock id: UART clock
	mBuf[6] = 48000000;			   // rate: 48Mhz
	mBuf[7] = 0;				   // clear turbo
	mBuf[8] = MBOX_TAG_LAST;
	mbox_call_no_print(ADDR(mBuf), MBOX_CH_PROP);

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RBP3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}								   // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}			   // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	// config appropriate baudrate
	UART0_IBRD = baudrate->UART0_IBRD_value;
  UART0_FBRD = baudrate->UART0_FBRD_value;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 or 2 stop bits */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;
  if (numberOfStopBits == 2) { // enable two stop bits
    UART0_LCRH = UART0_LCRH | UART0_LCRH_STP2; // set bit STP2
  } else {
    UART0_LCRH = UART0_LCRH & ~(UART0_LCRH_STP2); // clear bit STP2
  }

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO

  /* Keep track current baudrate and stopbits */
  current_baudrate = baudrate;
  current_stopbits = numberOfStopBits;
}
