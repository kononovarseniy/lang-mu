#include "string.h"

#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>

#include "utils/log.h"

void string_copy_chars(char *dst, char *src, size_t len)
{
    memcpy(dst, src, len * sizeof(char));
}

void string_set_chars(char *dst, char chr, size_t len)
{
    char *end = dst + len;
    while (dst < end)
        *(dst++) = chr;
}

int string_cmp_chars(char *a, char *b, size_t len)
{
    return memcmp(a, b, len * sizeof(char));
}

pString string_allocate_memory(size_t len)
{
    char *buf = malloc(len * sizeof(char));
    if (buf == NULL)
    {
        perror("string_allocate_memory: buffer allocation failed");
        return NULL;
    }
    pString res = malloc(sizeof(String));
    if (res == NULL)
    {
        perror("string_allocate_memory: string object allocation failed");
        free(buf);
        return NULL;
    }
    res->len = len;
    res->buf = buf;
    return res;
}
void free_string(pString str)
{
    free(str->buf);
    free(str);
}

pString string_clone(pString str)
{
    pString res = string_allocate_memory(str->len);
    if (res == NULL)
    {
        log("string_clone: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str->buf, str->len);
    return res;
}

pString string_from_cstring(char *str)
{
    return string_from_array(str, strlen(str));
}

char *string_to_cstring(pString str)
{
    char *res = malloc((str->len + 1) * sizeof(char));
    if (res == NULL)
    {
        perror("string_to_cstring: memory allocation failed");
        return NULL;
    }
    string_copy_chars(res, str->buf, str->len);
    res[str->len] = '\0';
    return res;
}

pString string_from_array(char *str, size_t len)
{
    pString res = string_allocate_memory(len);
    if (res == NULL)
    {
        log("string_from_array: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str, len);
    return res;
}

pString string_repeat_char(char ch, size_t count)
{
    pString res = string_allocate_memory(count);
    if (res == NULL)
    {
        log("string_repeat_char: string_allocate_memory failed");
        return NULL;
    }
    string_set_chars(res->buf, ch, count);
    return res;
}

pString string_repeat_string(pString str, size_t count)
{
    size_t str_len = str->len;
    pString res = string_allocate_memory(str_len * count);
    if (res == NULL)
    {
        log("string_repeat_string: string_allocate_memory failed");
        return NULL;
    }
    for (size_t i = 0; i < count; i++)
        string_copy_chars(res->buf + i * str_len, str->buf, str_len);
    return res;
}

int string_compare(pString a, pString b)
{
    return string_compare_sub(a, 0, a->len, b, 0, b->len);
}
int string_compare_sub(pString a, size_t a_ind, size_t a_len, pString b, size_t b_ind, size_t b_len)
{
    if (a_ind > a->len || a_ind + a_len > a->len ||
        b_ind > b->len || b_ind + b_len > b->len)
    {
        log("string_compare_sub: index out of range");
        exit(1);
    }

    char *a_start = a->buf + a_ind;
    char *b_start = b->buf + b_ind;
    char *a_end = a_start + a_len;
    char *b_end = b_start + b_len;

    // Swap if second string is longer
    int swap = 1;
    if (a_len < b_len)
    {
        swap = -1;

        char *tmp;
        tmp = a_start;
        a_start = b_start;
        b_start = tmp;

        tmp = a_end;
        a_end = b_end;
        b_end = tmp;
    }

    while (b_start < b_end)
    {
        if (*a_start != *b_start)
            break;
        a_start++;
        b_start++;
    }
    if (b_start == b_end) // longer strings starts from shorter
    {
        if (a_len == b_len)
            return 0;
        else
            return swap;
    }
    // difference at some char
    if (*a_start > *b_start)
        return swap;
    else if (*a_start < *b_start)
        return -swap;
    return 0;
}

pString string_concat(pString *strings, size_t count)
{
    size_t res_len = 0;
    for (size_t i = 0; i < count; i++)
        res_len += strings[i]->len;

    pString res = string_allocate_memory(res_len);
    if (res == NULL)
    {
        log("string_concat: string_allocate_memory failed");
        return NULL;
    }

    size_t pos = 0;
    for (size_t i = 0; i < count; i++)
    {
        string_copy_chars(res->buf + pos, strings[i]->buf, strings[i]->len);
        pos += strings[i]->len;
    }
    return res;
}

size_t string_index_of(pString str, pString sub, size_t start, size_t count)
{
    if (start > str->len || start + count > str->len)
    {
        log("string_index_of: index out of range");
        exit(1);
    }
    if (sub->len == 0)
        return start;

    char *left = str->buf + start;
    char *right = left + count - sub->len + 1;
    while (left < right)
    {
        if (string_cmp_chars(left, sub->buf, sub->len) == 0)
            return left - str->buf;
        left++;
    }
    return STR_ERROR;
}

size_t string_index_of_char(pString str, char sub, size_t start, size_t count)
{
    if (start > str->len || start + count > str->len)
    {
        log("string_index_of_char: index out of range");
        exit(1);
    }

    char *left = str->buf + start;
    char *right = left + count;
    while (left < right)
    {
        if (*left == sub)
            return left - str->buf;
        left++;
    }
    return STR_ERROR;
}

size_t string_last_index_of(pString str, pString sub, size_t start, size_t count)
{
    if (start + 1 < count)
    {
        log("string_last_index_of: index out of range");
        exit(1);
    }
    if (start > str->len)
    {
        log("string_last_index_of: index out of range");
        exit(1);
    }
    if (count == 0)
    {
        if (sub->len == 0)
            return start;
        else
            return -1;
    }
    if (start == str->len)
    {
        if (str->len == 0)
        {
            if (sub->len == 0)
                return 0;
            else
                return -1;
        }
        else // start == str->len > 0
        {
            start--;
            count--;
        }
    }
    if (sub->len == 0)
        return start + 1;

    char *left = str->buf + start - count + 1;
    char *right = str->buf + start - sub->len + 1;
    while (left <= right)
    {
        if (string_cmp_chars(right, sub->buf, sub->len) == 0)
            return right - str->buf;
        right--;
    }
    return STR_ERROR;
}

size_t string_last_index_of_char(pString str, char sub, size_t start, size_t count)
{
    if (start + 1 < count)
    {
        log("string_last_index_of_char: index out of range");
        exit(1);
    }
    if (start > str->len)
    {
        log("string_last_index_of_char: index out of range");
        exit(1);
    }
    if (count == 0)
    {
        return -1;
    }
    if (start == str->len)
    {
        if (str->len == 0)
        {
            return -1;
        }
        else
        {
            start--;
            count--;
        }
    }

    char *left = str->buf + start - count + 1;
    char *right = str->buf + start - 1 + 1;
    while (left <= right)
    {
        if (*right == sub)
            return right - str->buf;
        right--;
    }
    return STR_ERROR;
}

pString string_insert(pString str, size_t index, pString sub)
{
    if (index > str->len)
    {
        log("string_insert: index out of range");
        exit(1);
    }
    pString res = string_allocate_memory(str->len + sub->len);
    if (res == NULL)
    {
        log("string_insert: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str->buf, index);
    string_copy_chars(res->buf + index, sub->buf, sub->len);
    string_copy_chars(res->buf + index + sub->len, str->buf + index, str->len - index);
    return res;
}

pString string_insert_char(pString str, size_t index, char ch)
{
    if (index > str->len)
    {
        log("string_insert_char: index out of range");
        exit(1);
    }
    pString res = string_allocate_memory(str->len + 1);
    if (res == NULL)
    {
        log("string_insert_char: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str->buf, index);
    res->buf[index] = ch;
    string_copy_chars(res->buf + index + 1, str->buf + index, str->len - index);
    return res;
}

pString string_substring(pString str, size_t index, size_t length)
{
    if (index > str->len || index + length > str->len)
    {
        log("string_substring: index out of range");
        exit(1);
    }
    pString res = string_allocate_memory(length);
    if (res == NULL)
    {
        log("string_substring: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str->buf + index, length);
    return res;
}

pString string_remove(pString str, size_t index, size_t len)
{
    if (index > str->len || index + len > str->len)
    {
        log("string_remove: index out of range");
        exit(1);
    }
    pString res = string_allocate_memory(str->len - len);
    if (res == NULL)
    {
        log("string_remove: string_allocate_memory failed");
        return NULL;
    }
    string_copy_chars(res->buf, str->buf, index);
    string_copy_chars(res->buf + index, str->buf + index + len, str->len - index - len);
    return res;
}

pString string_to_upper(pString str)
{
    pString res = string_allocate_memory(str->len);
    if (res == NULL)
    {
        log("string_to_upper: string_allocate_memory failed");
        return NULL;
    }
    for (size_t i = 0; i < str->len; i++)
        res->buf[i] = toupper(str->buf[i]);
    return res;
}

pString string_to_lower(pString str)
{
    pString res = string_allocate_memory(str->len);
    if (res == NULL)
    {
        log("string_to_upper: string_allocate_memory failed");
        return NULL;
    }
    for (size_t i = 0; i < str->len; i++)
        res->buf[i] = tolower(str->buf[i]);
    return res;
}
