#include "../include/lib/uart.h"
#include "../include/lib/mbox.h"
#include "../include/lib/gpio.h"
#include "./uart/uart_common.c"

#ifdef UART0
#include "./uart/uart0.c"
#else
#include "./uart/uart1.c"
#endif
