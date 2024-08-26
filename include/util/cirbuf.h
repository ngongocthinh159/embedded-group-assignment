#pragma once

#define HISTORY_LENGTH 10
#define HISTORY_ENTRY_LENGTH 100

extern char history_buffer[HISTORY_LENGTH][HISTORY_ENTRY_LENGTH];
extern int history_head;

int cir_buf_push(char *data);
void cir_buf_latest(char **res);
