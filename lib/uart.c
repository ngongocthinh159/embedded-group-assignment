#include "lib/uart.h"
#include "lib/mbox.h"
#include "lib/gpio.h"
#include "./uart/uart_common.c"

#ifdef UART0
#include "./uart/uart0.c"
#else
#include "./uart/uart1.c"
#endif
