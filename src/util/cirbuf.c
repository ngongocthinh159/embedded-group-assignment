#include "util/cirbuf.h"
#include "util/string.h"

/**
 * naive implementation of circular buffer
 */

int cir_buf_push(char *data) {
  int next;

  // get next position after head
  next = history_head + 1;
  // reset if overflow
  if (next >= HISTORY_LENGTH)
    next = 0;

  // load data to buffer
  strcpy(history_buffer[history_head], data);

  // head to next data offset.
  history_head = next;

  return 0; // return success to indicate successful push.
}

void cir_buf_latest(char **res) {
  int counter = 0;
  int start = history_head - 1;

  do {
    res[counter] = history_buffer[start];

    // get latest to oldest
    if (start == 0) {
      start = HISTORY_LENGTH - 1;
    } else {
      start--;
    }
  } while (start != history_head - 1);
}
