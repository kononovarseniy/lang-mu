#include "strutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "log.h"

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

void strtolower(char *s)
{
    while (*s != '\0')
    {
        *s = tolower(*s);
        s++;
    }
}

int hextodec(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

char tohex(int digit)
{
    return "0123456789abcdef"[digit];
}

char *unescape_string(char *str, int len)
{
    char *res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
    {
        perror("unescape_string: malloc failed");
        return NULL;
    }
    int pos = 0;
    int i = 0 ;
    while (i < len)
    {
        char c = str[i++];
        if (c != '\\')
            res[pos++] = c;
        else
        {
            c = str[i++];
            switch (tolower(c))
            {
            case '\'': res[pos++] = '\''; break;
            case '\"': res[pos++] = '\"'; break;
            case '\\': res[pos++] = '\\'; break;
            case '0': res[pos++] = '\0'; break;
            case 'a': res[pos++] = '\a'; break;
            case 'b': res[pos++] = '\b'; break;
            case 'f': res[pos++] = '\f'; break;
            case 'n': res[pos++] = '\n'; break;
            case 'r': res[pos++] = '\r'; break;
            case 't': res[pos++] = '\t'; break;
            case 'v': res[pos++] = '\v'; break;
            case 'x':
                {
                    char d1, d2;
                    if (len - i < 2 ||
                        (d1 = hextodec(str[i++])) == -1 ||
                        (d2 = hextodec(str[i++])) == -1)
                    {
                        log("Unrecognized escape sequence \\xnn");
                        free(res);
                        return NULL;
                    }
                    res[pos++] = d1 * 16 + d2;
                }
                break;
            default:
                logf("Unrecognized escape sequence \\%c", c);
                free(res);
                return NULL;
            }
        }
    }
    res[pos] = '\0';
    return res;
}

void write_to_buf(char **buf, int *len, int pos, char ch)
{
    if (*len == -1) return; // Caller ignored previous error
    if (pos >= *len)
    {
        *len = pos + 1;
        char *new_buf = realloc(*buf, *len);
        if (new_buf == NULL)
        {
            perror("append_to_buf: realloc failed");
            *len = -1;
            return;
        }
        *buf = new_buf;
    }
    (*buf)[pos] = ch;
}

char *escape_string(char *str, int len)
{
    char *main_chars = "\\\'\"\a\b\f\n\r\t\v";
    char *main_esc = "\\\'\"abfnrtv";

    int buf_len = len + 1;
    char *buf = malloc(buf_len * sizeof(char));
    if (buf == NULL)
    {
        perror("escape_string: malloc failed");
        return NULL;
    }
    int dst = 0;
    int src = 0;
    while (src < len)
    {
        char c = str[src++];
        if (c >= 0x20 && c < 0x7F)
        {
            write_to_buf(&buf, &buf_len, dst++, c);
        }
        else if (c == '\0')
        {
            write_to_buf(&buf, &buf_len, dst++, '\\');
            write_to_buf(&buf, &buf_len, dst++, '0');
        }
        else
        {
            char *main_ind = strchr(main_chars, c);

            if (main_ind != NULL)
            {
                write_to_buf(&buf, &buf_len, dst++, '\\');
                write_to_buf(&buf, &buf_len, dst++, main_esc[main_ind - main_chars]);
            }
            else
            {
                write_to_buf(&buf, &buf_len, dst++, '\\');
                write_to_buf(&buf, &buf_len, dst++, 'x');
                write_to_buf(&buf, &buf_len, dst++, tohex('c' / 10 % 10));
                write_to_buf(&buf, &buf_len, dst++, tohex('c' % 10));
            }
        }
    }
    write_to_buf(&buf, &buf_len, dst++, '\0');

    // handle errors
    if (len == -1)
    {
        free(buf);
        return NULL;
    }

    return buf;
}
