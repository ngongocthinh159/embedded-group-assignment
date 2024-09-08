#pragma once

void wait_msec(unsigned int msVal);
void set_wait_timer(int set, unsigned int msVal);
void set_wait_timer_cb1(int set, unsigned int msVal, void (*cb)());
