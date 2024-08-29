#include "lib/keyboard.h"
#include "lib/stack.h"
#include "lib/uart.h"
#include "util/string.h"
#include "cli/cli.h"
#define UART_BASE 0x101f1000
#define UART_LCR (UART_BASE + 0x0C)  // Line Control Register

#define LCR_STOPBIT_1 0x00  // 1 stop bit
#define LCR_STOPBIT_2 0x04  // 2 stop bits
#define SYSTEM_CLOCK_FREQ 250000000
#include "util/tty.h"
#include "lib/color.h"
#include "cli/command.h"

/* Private function prototype */
void _handle_auto_completion();
int _transfer_from_stack_buffer_to(char* buffer);

int new_line_received = 0; // used in for loop to check if there is a new command
int in_auto_completion = 0;



void uart_set_stopbit(int stopbit) {
    unsigned int lcr = *(volatile unsigned int *)UART_LCR;
    
    if (stopbit == 1) {
        lcr &= ~LCR_STOPBIT_2;
    } else if (stopbit == 2) {
        lcr |= LCR_STOPBIT_2;
    } else {
        // Invalid stop bit setting
        return;
    }
    
    *(volatile unsigned int *)UART_LCR = lcr;
}

// New function: Check and return if no new character, don't wait
// Usage: unsigned char c = uart_getc_non_block()
// 		  if (c != 0) {};
unsigned char uart_getc_non_block() {
    unsigned char ch = 0;
    if (uart_isReadByteReady()) ch = uart_getc();
    return ch;
}

void uart_scanning() {
    char ch = uart_getc_non_block();
    if (ch != 0) {
        if (ch == ENTER) {
            // handle when buffer is empty
            if (str_is_blank(cmd_st->buffer, st_size(cmd_st))) {
                uart_sendc(ENTER);
            
            // when buffer is not empty => turn on new_line_received flag
            } else {
                new_line_received = 1;
                
                uart_sendc(ENTER);
            }
        } else if (ch == ESC) {
            for (int i = 0; i < st_size(cmd_st); i++) {
                uart_puts(REMOVE_A_CHAR);
            }
            st_reset_buffer(cmd_st);
        } else if (ch == TAB) {
            _handle_auto_completion();
        } else if (ch == BACKSPACE) {  // DEL
            int pop_success = st_pop(cmd_st);
            if (pop_success) {
                uart_puts(REMOVE_A_CHAR);
            }
        } else {
            int push_success = st_push(cmd_st, ch);
            if (push_success) {
                uart_sendc(ch);
            } else {
                // TODO: when stack buffer is full might do something..
            }
        }
    }
}

int is_there_new_line() { return new_line_received; }

// Flush all chars inside stack buffer into input buffer
// AND reset the stack
void get_line(char *buffer) {
    new_line_received = 0;  // reset

    // transfer chars from stack buffer to command buffer
    int size = _transfer_from_stack_buffer_to(buffer);

    // remove all redudant chars
    str_beautify(buffer, size);

    // reset stack after transering
    st_reset_buffer(cmd_st);
}

// return size of the buffer after transfering
int _transfer_from_stack_buffer_to(char *buffer) {
    char *saved_buffer = st_get_buffer(cmd_st);
    int size = 0;
    for (int i = 0; i < st_size(cmd_st); i++) {
        buffer[i] = saved_buffer[i];
        size++;
    }
    buffer[st_size(cmd_st)] = '\0';
    return size;
}

void _handle_auto_completion() {
    if (in_auto_completion) {

    } else {
        
    }
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n') uart_sendc('\r');
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

/*
**
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
        int digit = num % 10;  // get last digit
        num = num / 10;        // remove last digit from the number
        str[len - (i + 1)] = digit + '0';
    }
    str[len] = '\0';

    uart_puts(str);
}

void uart_init() {
    // Disable UART
    UART0_CR = 0x00000000;

    // Setup the GPIO pins 14 and 15 for UART functionality
    gpio_set_function(14, GPIO_FUNC_ALT0);
    gpio_set_function(15, GPIO_FUNC_ALT0);

    // Clear pending interrupts
    UART0_ICR = 0x7FF;

    // Set integer & fractional part of baud rate to 115200 as default
    uart_set_baudrate(115200);  // Use previously defined function to set the baud rate

    // Enable FIFO & 8-bit data transmission (1 stop bit, no parity)
    UART0_LCRH = (UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT);

    // Enable UART, receive & transmit
    UART0_CR = (UART0_CR_UARTEN | UART0_CR_TXE | UART0_CR_RXE);
}

void uart_set_baudrate(int baudrate) {
    // Calculate the baud rate divisor
    unsigned int baudrate_divisor = SYSTEM_CLOCK_FREQ / (16 * baudrate);  // Integer part
    unsigned int remainder = SYSTEM_CLOCK_FREQ % (16 * baudrate);
    unsigned int fraction = ((remainder * 64) + (baudrate / 2)) / baudrate;  // Fractional part

    // Disable UART to set baud rate
    UART0_CR &= ~(UART0_CR_UARTEN);

    // Set the integer and fractional part of the baud rate divisor
    UART0_IBRD = baudrate_divisor;  // Integer part
    UART0_FBRD = fraction;          // Fractional part

    // Enable UART after setting baud rate
    UART0_CR |= (UART0_CR_UARTEN);
}