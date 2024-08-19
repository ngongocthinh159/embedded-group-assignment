#pragma once

#define ST_SIZE 200

int st_push(unsigned char x);
int st_pop();
char* st_flush_buffer();
void st_reset_buffer();
int st_cnt();
char * get_buffer();
