#include "lib/stack.h"

void st_init(Stack *st_obj, char *buffer, unsigned int buffer_size) {
    st_obj->buffer = buffer;
    st_obj->buffer_size = buffer_size;
    st_obj->top = -1;
}

int st_push(Stack *st_obj, unsigned char x) {
    if (st_obj->top == st_obj->buffer_size - 1) {
        // buffer overflow
        return 0;
    }

    st_obj->top += 1;
    st_obj->buffer[st_obj->top] = x;
    return 1;
}

int st_pop(Stack *st_obj) {
    if (st_obj->top == -1) {
        // end of element
        return 0;
    }
    st_obj->top -= 1;
    return 1;
}

void st_reset_buffer(Stack *st_obj) {
    st_obj->top = -1;
    st_obj->buffer[0] = '\0';
}

int st_size(Stack *st_obj) { return st_obj->top + 1; }

char *st_get_buffer(Stack *st_obj) { return st_obj->buffer; }
