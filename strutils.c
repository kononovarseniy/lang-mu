#include "strutils.h"

#include <string.h>
#include <memory.h>

char *copystr(const char *s)
{
    char *res = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(res, s);
    return res;
}

char *concatstr(const char *a, const char *b)
{
    int len1 = strlen(a);
    int len2 = strlen(b);
    char *res = malloc((len1 + len2 + 1) * sizeof(char));
    strcpy(res, a);
    strcpy(res + len1, b);
    return res;
}
