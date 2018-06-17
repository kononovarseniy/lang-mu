#include "strutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "types/string.h"

#include "log.h"

char *copy_string(const char *s)
{
    char *res = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(res, s);
    return res;
}

void str_to_lower(char *s)
{
    while (*s != '\0')
    {
        *s = (char) tolower(*s);
        s++;
    }
}

int hex_to_int(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

char int_to_hex(int digit)
{
    return "0123456789abcdef"[digit];
}

pString unescape_string(char *str, int len)
{
    char *res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
    {
        perror("unescape_string: malloc failed");
        return NULL;
    }
    size_t pos = 0;
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
                    int d1, d2;
                    if (len - i < 2 ||
                        (d1 = hex_to_int(str[i++])) == -1 ||
                        (d2 = hex_to_int(str[i++])) == -1)
                    {
                        log("Unrecognized escape sequence \\xnn");
                        free(res);
                        return NULL;
                    }
                    res[pos++] = (char) (d1 * 16 + d2);
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

    pString res_str = string_from_array(res, pos);
    free(res);
    if (res_str == NULL)
    {
        log("unescape_string: string_from_array failed");
        return NULL;
    }
    return res_str;
}

void write_to_buf(char **buf, size_t *len, size_t pos, char ch)
{
    if (*len == -1) return; // Caller ignored previous error
    if (pos >= *len)
    {
        *len = pos + 1;
        char *new_buf = realloc(*buf, *len);
        if (new_buf == NULL)
        {
            perror("append_to_buf: realloc failed");
            *len = SIZE_MAX;
            return;
        }
        *buf = new_buf;
    }
    (*buf)[pos] = ch;
}

char *escape_string(char *str, size_t len)
{
    char *main_chars = "\\\'\"\a\b\f\n\r\t\v";
    char *main_esc = "\\\'\"abfnrtv";

    size_t buf_len = len + 1;
    char *buf = malloc(buf_len * sizeof(char));
    if (buf == NULL)
    {
        perror("escape_string: malloc failed");
        return NULL;
    }
    size_t dst = 0;
    size_t src = 0;
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
                write_to_buf(&buf, &buf_len, dst++, int_to_hex('c' / 10 % 10));
                write_to_buf(&buf, &buf_len, dst++, int_to_hex('c' % 10));
            }
        }
    }
    write_to_buf(&buf, &buf_len, dst, '\0');

    // handle errors
    if (len == SIZE_MAX)
    {
        free(buf);
        return NULL;
    }

    return buf;
}
