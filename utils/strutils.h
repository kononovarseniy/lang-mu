#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

#include "types/string.h"

char *copy_string(const char *s);
void str_to_lower(char *s);

pString unescape_string(char *str, int len);
char *escape_string(char *str, size_t len);

#endif // STRUTILS_H_INCLUDED
