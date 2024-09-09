#include "lib/timer.h"

/* Function to wait for some msec: the program will stop there */
void wait_msec(unsigned int msVal) {
  register unsigned long f, t, r, expiredTime;  // 64 bits

  // Get the current counter frequency (Hz), 1Hz = 1 pulses/second
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

  // Read the current counter value
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));

  // Calculate expire value for counter
  /* Note: both expiredTime and counter value t are 64 bits,
  thus, it will still be correct when the counter is overflow */
  expiredTime = t + f * msVal / 1000;

  do {
    asm volatile("mrs %0, cntpct_el0" : "=r"(r));
  } while (r < expiredTime);
}

/* Function to start a timer (set = 1) or wait for it to expire (set = 0) */
/* Example Use:
    //set timer with 100ms
    set_wait_timer(1, 100);

    // go doing other tasks (which must take less than 100ms)
    // ..........

    //come back and wait for the timer to expire
    set_wait_timer(0, 100);
*/
void set_wait_timer(int set, unsigned int msVal) {
  static unsigned long expiredTime = 0;  // declare static to keep value
  register unsigned long r, f, t;

  if (set) { /* SET TIMER */
    // Get the current counter frequency (Hz)
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expired time:
    expiredTime = t + f * msVal / 1000;
  } else { /* WAIT FOR TIMER TO EXPIRE */
    do {
      asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
  }
}

void set_wait_timer_cb1(int set, unsigned int msVal, void (*cb)()) {
  static unsigned long expiredTime_1 = 0;  // declare static to keep value
  register unsigned long r, f, t;

  if (set) { /* SET TIMER */
    // Get the current counter frequency (Hz)
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expired time:
    expiredTime_1 = t + f * msVal / 1000;
  } else { /* WAIT FOR TIMER TO EXPIRE */
    do {
      cb();
      asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime_1);
  }
}
