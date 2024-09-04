#pragma once

typedef struct {
  int start;
  int end;
} TokenIndex;

int strcmp(const char *str1, const char *str2);
int strcpy(char *dest, char *src);
int strlen(const char *str);
int str_equal(const char *str1, const char *str2);
int strstr(const char *source, const char *match);
int str_strip(char *str, unsigned int size);
int str_beautify(char *str, unsigned int size);
int str_is_blank(char *str, unsigned int size);
int str_strip(char *str, unsigned int size);
int str_is_space_charaters(char c);
void str_replace_all(char *str, unsigned int size, char old_char, char new_char);
int str_last_token(char *str, char *buffer, int str_size);
int str_start_with(char *str, char *prefix, int str_size, int prefix_size);
int str_start_with_2(const char *str, const char *prefix);
char* get_nth_token_to_pointer(char* str, unsigned int tokenIndex, char separator);
TokenIndex get_nth_token_indices(const char* str, unsigned int tokenNumber, char separator);

char push(char *str, char value);
int pop(char *str);
void clean(char *str);
