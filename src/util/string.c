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
  while (*c)
    c++;

  *c = value;
  c++;
  *c = '\0';

  return value;
}

int pop(char *str) {
  char *c = str;
  while (*c)
    c++;

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
