#include "lib/stack.h"
#include "util/string.h"
#include "lib/uart.h"

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

int st_copy_from_st(Stack *st_src, Stack *st_dest) {
    char *src_p = st_get_buffer(st_src);
    char *dest_p = st_get_buffer(st_dest);

    int dest_size = 0;
    for (int i = 0; i < st_size(st_src); i++) {
        dest_size++;
        dest_p[i] = src_p[i];
    }

    dest_p[dest_size] = '\0';
    st_dest->top = dest_size - 1;

    return st_size(st_dest);
}

int st_copy_from_str(char *str_src, Stack *st_dest, int str_size) {
    char *dest_p = st_get_buffer(st_dest);
    for (int i = 0; i < str_size; i++) {
        dest_p[i] = str_src[i];
    }

    st_get_buffer(st_dest)[str_size] = '\0';
    st_dest->top = str_size - 1;

    return st_size(st_dest);
}

int st_append_from_st(Stack *st_src, Stack *st_dest) {
    char *src_p = st_get_buffer(st_src);
    char *dest_p = st_get_buffer(st_dest);
    int dest_org_size = st_size(st_dest);

    for (int i = 0; i < st_size(st_src); i++) {
        dest_p[dest_org_size + i] = src_p[i];
    }

    int new_size = st_size(st_src) + st_size(st_dest);
    st_get_buffer(st_dest)[new_size] = '\0';
    st_dest->top = new_size - 1;
    
    return st_size(st_dest);
}

int st_beautify_buffer(Stack *st_obj) {
    int new_size = str_beautify(st_obj->buffer, st_size(st_obj));

    st_get_buffer(st_obj)[new_size] = '\0';
    st_obj->top = new_size - 1;

    return st_size(st_obj);
}
