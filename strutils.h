#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

#include "stdlib/str.h"

char *copystr(const char *s);
void strtolower(char *s);

pString unescape_string(char *str, int len);
char *escape_string(char *str, int len);

#endif // STRUTILS_H_INCLUDED
