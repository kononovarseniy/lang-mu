#include "longnum_stub.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <memory.h>
#include "../log.h"

pLongNum create_longnum()
{
    pLongNum res = malloc(sizeof(LongNum));
    if (res == NULL)
    {
        perror("create_longnum: allocation failed");
        return NULL;
    }
    return res;
}
void free_longnum(pLongNum a)
{
    free(a);
}
pLongNum longnum_copy(pLongNum num)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_copy: create_longnum failed");
        return NULL;
    }
    res->num = num->num;
    return res;
}
pLongNum longnum_zero()
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_zero: create_longnum failed");
        return NULL;
    }
    res->num = 0;
    return res;
}
pLongNum longnum_one()
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_one: create_longnum failed");
        return NULL;
    }
    res->num = 1;
    return res;
}
pLongNum longnum_negative_one()
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_negative_one: create_longnum failed");
        return NULL;
    }
    res->num = -1;
    return res;
}

pLongNum longnum_parse(char *num, int s)
{
    errno = 0;
    char* end = num;
    long result = strtol(num, &end, s);
    if (errno == ERANGE)
    {
        perror("longnum_parse: integer overflow");
        return NULL;
    }
    else if (*end)
    {
        log("longnum_parse: not a valid integer");
        return NULL;
    }
    return longnum_from_int(result);
}
pLongNum longnum_from_int(int num)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_from_int: create_longnum failed");
        return NULL;
    }
    res->num = num;
    return res;
}
pLongNum longnum_from_double(double num)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_from_double: create_longnum failed");
        return NULL;
    }
    res->num = (long)num;
    return res;
}
char *longnum_to_string(pLongNum a, int base)
{
    char *sym = "0123456789abcdefghijklmnopqarsuvwxyz";
    if (base < 2 || base > 36)
    {
        log("longnum_to_string: invalid base");
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
        perror("longnum_to_string: allocation failed");
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
long longnum_to_int(pLongNum a)
{
    return a->num;
}
double longnum_to_double(pLongNum a)
{
    return a->num;
}

//Функции с основными арифметическими действиями
pLongNum longnum_add(pLongNum a, pLongNum b)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_add: create_longnum failed");
        return NULL;
    }
    res->num = a->num + b->num;
    return res;
}
pLongNum longnum_sub(pLongNum a, pLongNum b)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_sub: create_longnum failed");
        return NULL;
    }
    res->num = a->num - b->num;
    return res;
}
pLongNum longnum_inverse(pLongNum a)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_inverse: create_longnum failed");
        return NULL;
    }
    res->num = -a->num;
    return res;
}
pLongNum longnum_product(pLongNum a, pLongNum b)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_product: create_longnum failed");
        return NULL;
    }
    res->num = a->num * b->num;
    return res;
}
pLongNum longnum_div(pLongNum a, pLongNum b)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_div: create_longnum failed");
        return NULL;
    }
    res->num = a->num / b->num;
    return res;
}
pLongNum longnum_rem(pLongNum a, pLongNum b)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_rem: create_longnum failed");
        return NULL;
    }
    res->num = a->num % b->num;
    return res;
}
pLongNum longnum_greatest_common_divisor(pLongNum a, pLongNum b)
{
    long n1 = a->num, n2 = b->num;
    while (n1 != 0 && n2 != 0)
    {
        if (n1 > n2)
            n1 %= n2;
        else
            n2 %= n1;
    }
    return longnum_from_int(n1+n2);
}

//Функции сравнения двух длинных чисел
int longnum_compare(pLongNum a, pLongNum b)
{
    if (a->num > b->num)
        return 1;
    else if (a->num < b->num)
        return -1;
    return 0;
}
