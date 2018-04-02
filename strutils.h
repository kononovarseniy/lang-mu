#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

char *copystr(const char *s);
char *concatstr(const char *a, const char *b);
void strtolower(char *s);

char *unescape_string(char *str, int len);
char *escape_string(char *str, int len);

#endif // STRUTILS_H_INCLUDED
