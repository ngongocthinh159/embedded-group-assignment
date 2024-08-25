#pragma once

int strcmp(char *str1, char *str2);
int str_equal(const char *str1, const char *str2);
int str_strip(char *str, unsigned int size);
int str_beautify(char *str, unsigned int size);
int str_is_blank(char *str, unsigned int size);
int str_strip(char *str, unsigned int size);
int str_is_space_charaters(char c);
void str_replace_all(char *str, unsigned int size, char old_char, char new_char);
int str_last_token(char *str, char *buffer, int str_size);
int str_start_with(char *str, char *prefix, int str_size, int prefix_size);

char push(char *str, char value);
int pop(char *str);
void clean(char *str);
