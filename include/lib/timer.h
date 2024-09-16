#pragma once

#include "lib/gpio.h"

#define SYS_TIMER_COUNTER_LOWER (* (volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYS_TIMER_COUNTER_HIGHER (* (volatile unsigned int*)(MMIO_BASE+0x00003008))

void wait_msec(unsigned int msVal);
void set_wait_timer(int set, unsigned int msVal);
void set_wait_timer_cb1(int set, unsigned int msVal, void (*cb)());
void set_wait_timer_cb2(int set, unsigned int msVal, void (*cb)());
