#pragma once
#include "cli/cli.h"

#define HISTORY_LENGTH 10

extern char history_buffer[HISTORY_LENGTH][COMMAND_MAX_SIZE];
extern int history_head;

int cir_buf_push(char *data);
void cir_buf_latest(char **res);
