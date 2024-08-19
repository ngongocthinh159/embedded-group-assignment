#include "../include/lib/stack.h"

int top = -1;

char st_buffer[ST_SIZE + 1];

int st_push(unsigned char x)
{
    if (top == ST_SIZE - 1)
    {
        // buffer overflow
        return 0;
    }
    
    top = top + 1;
    st_buffer[top] = x;
    return 1;
}

int st_pop()
{
    if (top == -1)
    {
        // end of element
        return 0;
    }
    top = top - 1;
    return 1;
}

void st_reset_buffer() {
    top = -1;
}

char* st_flush_buffer()
{
    st_buffer[top + 1] = '\0';
    st_reset_buffer();
    return st_buffer;
}

int st_cnt() {
    return top + 1;
}

char * get_buffer() {
    return st_buffer;
}
