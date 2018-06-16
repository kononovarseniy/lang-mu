#include "longint.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <memory.h>
#include "utils/log.h"

struct LongInt
{
    long num;
};

pLongInt create_longint()
{
    pLongInt res = malloc(sizeof(LongInt));
    if (res == NULL)
    {
        perror("create_longint: allocation failed");
        return NULL;
    }
    return res;
}

void free_longint(pLongInt a)
{
    free(a);
}

pLongInt longint_copy(pLongInt num)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_copy: create_longint failed");
        return NULL;
    }
    res->num = num->num;
    return res;
}

pLongInt longint_zero()
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_zero: create_longint failed");
        return NULL;
    }
    res->num = 0;
    return res;
}

pLongInt longint_one()
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_one: create_longint failed");
        return NULL;
    }
    res->num = 1;
    return res;
}

pLongInt longint_negative_one()
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_negative_one: create_longint failed");
        return NULL;
    }
    res->num = -1;
    return res;
}

pLongInt longint_parse(char *num, int s)
{
    errno = 0;
    char* end = num;
    long result = strtol(num, &end, s);
    if (errno == ERANGE)
    {
        perror("longint_parse: integer overflow");
        return NULL;
    }
    else if (*end)
    {
        log("longint_parse: not a valid integer");
        return NULL;
    }
    return longint_from_long(result);
}

pLongInt longint_from_long(long num)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longnum_from_int: create_longint failed");
        return NULL;
    }
    res->num = num;
    return res;
}

pLongInt longint_from_double(double num)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_from_double: create_longint failed");
        return NULL;
    }
    res->num = (long)num;
    return res;
}

char *longint_to_string(pLongInt a, int base)
{
    char *sym = "0123456789abcdefghijklmnopqarsuvwxyz";
    if (base < 2 || base > 36)
    {
        log("longint_to_string: invalid base");
        exit(1);
    }
    if (a->num == 0)
    {
        char *res = malloc(2 * sizeof(char));
        res[0] = '0';
        res[1] = '\0';
        return res;
    }
    // count digits
    int digits = 0;
    long tmp = a->num;
    if (tmp < 0)
    {
        digits++;
        tmp = -tmp;
    }
    while (tmp > 0)
    {
        tmp /= base;
        digits++;
    }
    // int to string
    char *res = malloc((digits + 1) * sizeof(char));
    if (res == NULL)
    {
        perror("longint_to_string: allocation failed");
        return NULL;
    }
    int i = 0;
    tmp = a->num;
    if (tmp < 0)
    {
        tmp = -tmp;
        res[0] = '-';
    }
    while (tmp > 0)
    {
        res[digits - ++i] = sym[tmp % base];
        tmp /= base;
    }
    res[digits] = '\0';
    return res;
}

long longint_to_long(pLongInt a)
{
    return a->num;
}

double longint_to_double(pLongInt a)
{
    return a->num;
}

//Функции с основными арифметическими действиями
pLongInt longint_sum(pLongInt a, pLongInt b)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_sum: create_longint failed");
        return NULL;
    }
    res->num = a->num + b->num;
    return res;
}

pLongInt longint_difference(pLongInt a, pLongInt b)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_difference: create_longint failed");
        return NULL;
    }
    res->num = a->num - b->num;
    return res;
}

pLongInt longint_inverse(pLongInt a)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_inverse: create_longint failed");
        return NULL;
    }
    res->num = -a->num;
    return res;
}

pLongInt longint_product(pLongInt a, pLongInt b)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_product: create_longint failed");
        return NULL;
    }
    res->num = a->num * b->num;
    return res;
}

pLongInt longint_quotient(pLongInt a, pLongInt b)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_quotient: create_longint failed");
        return NULL;
    }
    res->num = a->num / b->num;
    return res;
}

pLongInt longint_remainder(pLongInt a, pLongInt b)
{
    pLongInt res = create_longint();
    if (res == NULL)
    {
        log("longint_remainder: create_longint failed");
        return NULL;
    }
    res->num = a->num % b->num;
    return res;
}

pLongInt longint_gcd(pLongInt a, pLongInt b)
{
    long n1 = a->num, n2 = b->num;
    while (n1 != 0 && n2 != 0)
    {
        if (n1 > n2)
            n1 %= n2;
        else
            n2 %= n1;
    }
    return longint_from_long(n1 + n2);
}

//Функции сравнения двух длинных чисел
int longint_compare(pLongInt a, pLongInt b)
{
    if (a->num > b->num)
        return 1;
    else if (a->num < b->num)
        return -1;
    return 0;
}
