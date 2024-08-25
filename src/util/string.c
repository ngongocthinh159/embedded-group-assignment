#include "lib/uart.h"

char space_characters[] = {'\n', ' ', '\t', '\r'};

int strcmp(const char *str1, const char *str2) {
    const unsigned char *p1 = (const unsigned char *)str1;
    const unsigned char *p2 = (const unsigned char *)str2;

    while (*p1 != '\0') {
        if (*p2 == '\0') {
            return 1;
        }
        if (*p2 > *p1) {
            return -1;
        }
        if (*p1 > *p2) {
            return 1;
        }

        p1++;
        p2++;
    }

    if (*p2 != '\0') {
        return -1;
    }

    return 0;
}

int str_equal(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

char push(char *str, char value) {
    char *c = str;
    while (*c) c++;

    *c = value;
    c++;
    *c = '\0';

    return value;
}

int str_is_space_charaters(char c) {
    for (int i = 0; i < sizeof(space_characters)/sizeof(char); i++) {
        if (c == space_characters[i]) return 1;
    }
    return 0;
}

// example: " \n \t   help    playvideo  \r \n  "
// output: "help    playvideo"
int str_strip(char *str, unsigned int size) {
    int startIdx = -1, endIdx = -2;
    for (int i = 0; i < size; i++) {
        if (startIdx == -1 && !str_is_space_charaters(str[i])) {
            startIdx = i;
            endIdx = i;
        } else if (startIdx != -1 && !str_is_space_charaters(str[i])) {
            endIdx = i;
        }
    }

    // uart_hex(startIdx);
    // uart_sendc(',');
    // uart_hex(endIdx);
    // uart_puts("\n");

    int new_size = 0;
    for (int i = 0, j = startIdx; i < size && j <= endIdx; i++, j++) {
        new_size++;
        str[i] = str[j];
    }
    if (endIdx != -2) {
        str[endIdx - startIdx + 1] = '\0';
    }
    return new_size;
}

void str_replace_all(char *str, unsigned int size, char old_char, char new_char) {
    for (int i = 0; i < size; i++) {
        if (str[i] == old_char) str[i] = new_char;
    }
}

// look for complete list of space characters: char space_characters[]
int compress_mutiple_space_chars_into_one_space(char *str, unsigned int size) {
    int i = 0, j = 0;
    int new_size = 0;
    while (i < size) {
        if (!str_is_space_charaters(str[i])) {
            str[j++] = str[i++];
            new_size++;
            continue;
        }

        // keep skipping char is space character
        while (i + 1 < size && str_is_space_charaters(str[i + 1])) {
            i++;
        }

        // write one ' ' represent all skipped chars
        str[j] = ' ';
        new_size++;

        j++;
        i++;
    }
    str[new_size] = '\0';
    return new_size;
}

// example: " \n \t   help  \n \t  playvideo  \r \n  "
// output: "help playvideo"
int str_beautify(char *str, unsigned int size) {
    int new_size = 0;

    // compress mutiple space into one
    new_size = compress_mutiple_space_chars_into_one_space(str, size);

    // strip space characters in two side
    new_size = str_strip(str, new_size);

    return new_size;
}

int str_is_blank(char *str, unsigned int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < sizeof(space_characters)/sizeof(char); j++) {
            if (str[i] != space_characters[j]) return 0;
        }
    }
    return 1;
}

// fetch last token from str to buffer
// str must be called with str_beautify() first
// must make sure the buffer size is enought for storing token
// example: "fsdf my_token"
// return: "my_token"
int str_last_token(char *str, char *buffer, int str_size) {
    if (str_size == 0) return 0;

    int i = str_size - 1;
    int start_idx = -1;
    while (i >= 0 && !str_is_space_charaters(str[i])) {
        start_idx = i;
        i--;
    }

    if (start_idx == -1) return 0;

    int end_idx = str_size - 1;
    int token_size = 0;
    for (int i = start_idx, j = 0; i <= end_idx; i++, j++) {
        buffer[j] = str[i];
        token_size++;
    }

    return token_size;
}

int str_start_with(char *str, char *prefix, int str_size, int prefix_size) {
    if (str_size < prefix_size) return 0;

    for (int i = 0; i < prefix_size; i++) {
        if (str[i] != prefix[i]) return 0;
    }
    return 1;
}

int pop(char *str) {
    char *c = str;
    while (*c) c++;

    *(c - 2) = '\0';

    return 1;
}

int shift(char *str, int location) {
    char *c = str;
    for (int i = 0; *c; i++) {
        if (i == location) {
            *c = *(c + 1);
            c++;
        }
    }

    return 1;
}

void clean(char *str) {
    char *c = str;
    while (*c) {
        *c = '\0';
        c++;
    }
}
