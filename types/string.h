#ifndef LANG_MU_TYPES_STRING_H_INCLUDED
#define LANG_MU_TYPES_STRING_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#define STR_ERROR SIZE_MAX

typedef struct String String, *pString;

struct String
{
    size_t len;
    char *buf;
};

pString string_allocate_memory(size_t len);
void free_string(pString str);
pString string_clone(pString str);

pString string_from_cstring(char *str);
char *string_to_cstring(pString str);
pString string_from_array(char *str, size_t len);
pString string_repeat_char(char ch, size_t count);
pString string_repeat_string(pString str, size_t count);

int string_compare(pString a, pString b);
int string_compare_sub(pString a, size_t a_ind, size_t a_len, pString b, size_t b_ind, size_t b_len);
pString string_concat(pString *strings, size_t count);
size_t string_index_of(pString str, pString sub, size_t start, size_t count);
size_t string_index_of_char(pString str, char sub, size_t start, size_t count);
size_t string_last_index_of(pString str, pString sub, size_t start, size_t count);
size_t string_last_index_of_char(pString str, char sub, size_t start, size_t count);

pString string_insert(pString str, size_t index, pString sub);
pString string_insert_char(pString str, size_t index, char ch);
pString string_substring(pString str, size_t index, size_t length);
pString string_remove(pString str, size_t index, size_t len);

pString string_to_upper(pString str);
pString string_to_lower(pString str);

#endif // LANG_MU_TYPES_STRING_H_INCLUDED
