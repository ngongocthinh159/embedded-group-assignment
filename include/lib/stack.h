#pragma once

typedef struct {
    char *buffer;
    int buffer_size;
    int top;
} Stack;

void st_init(Stack *st_obj, char *buffer, unsigned int buffer_size);
int st_push(Stack *st_obj, unsigned char x);
int st_pop(Stack *st_obj);
void st_reset_buffer(Stack *st_obj);
int st_size(Stack *st_obj);
char *st_get_buffer(Stack *st_obj);
