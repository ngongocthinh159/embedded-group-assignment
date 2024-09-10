#include "util/string.h"

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

int strcpy(char *dest, char *src) {
  int i = 0;
  while (1) {
    dest[i] = src[i];
    if (dest[i] == '\0') break;
    i++;
  }

  return 1;
}

// BSD strlen implementation
// https://cvsweb.openbsd.org/cgi-bin/cvsweb/src/lib/libc/string/strlen.c?rev=1.7&content-type=text/x-cvsweb-markup
int strlen(const char *str) {
  const char *s;
  for (s = str; *s; ++s);
  return (s - str);
}

int str_equal(const char *str1, const char *str2) {
  return strcmp(str1, str2) == 0;
}

const int strstr(const char *source, const char *match) {
  while (*source != '\0') {
    if ((*source == *match) && str_equal(source, match)) {
      return 1;
    }
    source++;
  }
  return 0;
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
  for (int i = 0; i < sizeof(space_characters) / sizeof(char); i++) {
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

void str_replace_all(char *str, unsigned int size, char old_char,
                     char new_char) {
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
    for (int j = 0; j < sizeof(space_characters) / sizeof(char); j++) {
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

// No need str size and prefix size but must ensure both string is not null
int str_start_with_2(const char *str, const char *prefix) {
  // Check each character in prefix to see if it matches the start of str.
  while (*prefix) {
    if (*str == '\0' || *str != *prefix) {
      return 0;  // Mismatch found or str is shorter than prefix.
    }
    str++;
    prefix++;
  }

  return 1;  // Reached the end of prefix without a mismatch.
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

// This function will modify the input str and return the char pointer at the
// start of the token char* result = get_nth_token(inputString, tokenIndex,
// separator); if (*result != '\0') {
//     // do not have token
// } else {
//     // have token
// }
char *get_nth_token_to_pointer(char *str, unsigned int tokenIndex,
                               char separator) {
  unsigned int currentToken = 0;
  char *tokenStart = str;

  // Iterate over the string to find tokens
  while (*str != '\0') {
    // Check if the current character is the separator
    if (*str == separator) {
      // Check if the current token is the one we are looking for
      if (currentToken == tokenIndex) {
        *str = '\0';  // Null-terminate the token
        return tokenStart;
      }
      // Not the token we're looking for, move to the next one
      currentToken++;
      tokenStart = str + 1;  // Set the start of the next token
    }
    str++;
  }

  // Handle the last token case because it might not end with a separator
  if (currentToken == tokenIndex) {
    return tokenStart;
  }

  // If the requested token number is greater than the number of tokens
  // available
  static char empty = '\0';
  return &empty;
}

// Usage:
// const char *str = "hello,world,this,is,a,test";
// char separator = ',';
// unsigned int tokenIndex = 2;  // We want the third token

// TokenIndex token = get_nth_token_indices(str, tokenIndex, separator);

// Display the result
// if (token.start != -1) {
//     printf("Token %u starts at index %d and ends at index %d\n", tokenIndex,
//     token.start, token.end);
// } else {
//     printf("Token %u not found\n", tokenIndex);
// }
TokenIndex get_nth_token_indices(const char *str, unsigned int tokenIndex,
                                 char separator) {
  unsigned int currentToken = 0;
  int currentIndex = 0;         // Keep track of the current character index
  TokenIndex token = {-1, -1};  // Initialize start and end indices to -1

  // Early return if the input string is NULL
  // if (str == NULL) {
  //     return token;
  // }

  int start =
      (str[0] != separator) ? 0 : -1;  // Properly initialize the start index

  // Iterate over the string to find tokens
  while (str[currentIndex] != '\0') {
    if (str[currentIndex] == separator) {
      if (start != -1 && currentToken == tokenIndex) {
        token.start = start;
        token.end = currentIndex - 1;  // Set end index before the separator
        return token;
      }
      currentToken++;
      start = currentIndex +
              1;  // Update start index to the position after the separator
    }
    currentIndex++;
  }

  // Handle the last token case
  if (currentToken == tokenIndex && start != -1) {
    token.start = start;
    token.end = currentIndex - 1;  // End at the end of the string
    return token;
  }

  // If the requested token number is greater, return -1 for both indices
  return token;
}

void int_to_string_padding(unsigned int x, char* buffer, int width) {
    // Ensure the buffer is large enough for 5 digits plus the null terminator
    buffer[width] = '\0';  // Set the null terminator

    // Fill the buffer from the end
    for (int i = width - 1; i >= 0; i--) {
        buffer[i] = '0' + (x % 10);  // Get the last digit and convert to character
        x /= 10;  // Remove the last digit
    }
}
