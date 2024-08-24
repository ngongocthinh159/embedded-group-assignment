#include "lib/uart.h"
#include "lib/mbox.h"
#include "lib/gpio.h"
#include "lib/stack.h"
#include "lib/keyboard.h"

int new_line_received = 0;

// New function: Check and return if no new character, don't wait
// Usage: unsigned char c = uart_getc_non_block()
// 		  if (c != 0) {};
unsigned char uart_getc_non_block()
{
	unsigned char ch = 0;
	if (uart_isReadByteReady())
		ch = uart_getc();
	return ch;
}

void uart_scanning() {
	char ch = uart_getc_non_block();
	if (ch != 0) {
		if (ch == ENTER || ch == ESC) {
			new_line_received = 1;
			
			uart_sendc('\n');
		} else if (ch == DEL) { // DEL
			int pop_success = st_pop();
			if (pop_success) {
				uart_puts("\b \b");
			}
		} else {
			int push_success = st_push(ch);
			if (push_success) {
				uart_sendc(ch);
			} else {
				// TODO: when stack buffer is full might do something..
			}
		}
	}
}

int is_there_new_line() {
	return new_line_received;
}

// Flush all chars inside stack buffer into input buffer
// AND reset the stack
void get_line(char *buffer) {
	new_line_received = 0; // reset
	char *saved_buffer = get_buffer();
	for (int i = 0; i < st_cnt(); i++) {
		buffer[i] = saved_buffer[i];
	}
	buffer[st_cnt()] = '\0';

	st_reset_buffer();
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
	while (*s)
	{
		/* convert newline to carriage return + newline */
		if (*s == '\n')
			uart_sendc('\r');
		uart_sendc(*s++);
	}
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num)
{
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4)
	{

		// Get highest 4-bit nibble
		char digit = (num >> pos) & 0xF;

		/* Convert to ASCII code */
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		digit += (digit > 9) ? (-10 + 'A') : '0';
		uart_sendc(digit);
	}
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num)
{
	// A string to store the digit characters
	char str[33] = "";

	// Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10)
	{
		len++;
		temp = temp / 10;
	}

	// Store into the string and print out
	for (int i = 0; i < len; i++)
	{
		int digit = num % 10; // get last digit
		num = num / 10;		  // remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}
