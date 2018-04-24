#include "longnum.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../log.h"

//Работа с памятью
pLongNum create_longnum()
{
    pLongNum res = malloc(sizeof(LongNum));
    if (res == NULL)
    {
        perror("create_longnum: malloc failed\n");
        return NULL;
    }
    return res;
}
void free_longnum(pLongNum a)
{
    free(a->n);
    free(a);
}


//Вспомогательные функции
void longnum_mem(pLongNum a)
{
    if (a == NULL)
    {
        log("longnum_mem: NULL pointer");
        exit(1);
    }
    a->n = malloc(a->l * DIGIT_SIZE);
    if (a->n == NULL)
    {
        perror("longnum_mem: malloc failed");
        exit(1);
    }
}
void longnum_remem(pLongNum a, int l)
{
    int *b = malloc(a->l * DIGIT_SIZE);
    for (int i = 0; i < a->l; i++)
        *(b + i) = *(a->n + i);

    free (a->n);
    a->n = malloc(l * DIGIT_SIZE);


    for(int j = 0; j < l; j++)
    {
        if(j < a->l)
            *(a->n + j) = *(b + j);
        else *(a->n + j) = 0;
    }

    a->l = l;
    free(b);
}
void longnum_nulldel(pLongNum a)
{
    int i = a->l;
    while((*(a->n + i - 1) == 0) && (i != 1))
        i--;
    longnum_remem(a, i);
}
void longnum_print(pLongNum a)
{
    if(a->s == -1)
        printf("-");
    for(int i = (a->l - 1); i >= 0; i--)
        printf("%d|", *(a->n + i));
    printf("\n");
}
int longnum_ntd(char s, int i) //Посимвольное преобразование в "цифры"
{
    int a;
    if(s >= '0' && s <= '9')
    {
        a = s - '0';
    }
    if(s >= 'a' && s <= 'z')
    {
        a = s - 'a' + 10;
    }
    if(s >= 'A' && s <= 'Z')
    {
        a = s - 'A' + 10;
    }
    if(s == '-')
        a = -1;
    if(a >= i)
        a = -2;
    return a;
}

pLongNum longnum_copy(pLongNum num)
{
    pLongNum res = create_longnum();
    if (res == NULL)
    {
        log("longnum_copy: create_longnum failed");
        return NULL;
    }
    res->l = num->l;
    res->s = num->s;
    // Allocate memory
    longnum_mem(res);
    memcpy(res->n, num->n, num->l * DIGIT_SIZE);
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
    res->l = 1;
    res->s = 1;
    longnum_mem(res);
    res->n[0] = 0;

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
    res->l = 1;
    res->s = 1;
    longnum_mem(res);
    res->n[0] = 1;

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
    res->l = 1;
    res->s = -1;
    longnum_mem(res);
    res->n[0] = 1;

    return res;
}

//Функции сравнения
_Bool longnum_greater(pLongNum a, pLongNum b) //">"
{
    _Bool e = 0;

    if(a->s == b->s)
    {
        if(a->s == 1)
        {
            if(a->l == b->l)
            {
                for(int i = a->l - 1; i >= 0; i--)
                {
                    if(*(a->n + i) != *(b->n + i))
                    {
                        if(*(a->n + i) > *(b->n + i))
                        {
                            e = 1;
                            break;
                        }
                        else
                        {
                            e = 0;
                            break;
                        }
                    }
                }
            }
            else
            {
                if(a->l > b->l)
                    e = 1;
                else
                    e = 0;
            }
        }
        else
        {
            if(a->l == b->l)
            {
                for(int i = a->l - 1; i >= 0; i--)
                {
                    if(*(a->n + i) != *(b->n + i))
                    {
                        if(*(a->n + i) < *(b->n + i))
                        {
                            e = 1;
                            break;
                        }
                        else
                        {
                            e = 0;
                            break;
                        }
                    }
                }
            }
            else
            {
                if(a->l < b->l)
                    e = 1;
                else
                    e = 0;
            }
        }
    }
    else
    {
        if(a->s == 1)
            e = 1;
        else
            e = 0;
    }

    return e;
}
_Bool longnum_less(pLongNum a, pLongNum b) //"<"
{
    _Bool e = 0;
    if(longnum_greater(a, b) == 0 && longnum_equal(a, b) == 0)
        e = 1;
    return e;
}
_Bool longnum_equal(pLongNum a, pLongNum b) //"="
{
    _Bool e = 0;

    if(a->s == b->s)
        if(a->l == b->l)
            for(int i = 0; i < a->l; i++)
            {
                if(*(a->n + i) != *(b->n + i))
                    break;
                else
                    e = 1;
            }
    return e;
}
_Bool longnum_greater_or_equal(pLongNum a, pLongNum b) //">="
{
    _Bool e = 0;
    if(longnum_greater(a, b) || longnum_equal(a, b))
        e = 1;
    return e;
}
_Bool longnum_less_or_equal(pLongNum a, pLongNum b) //"<="
{
    _Bool e = 0;
    if(longnum_less(a, b) || longnum_equal(a, b))
        e = 1;
    return e;
}
int longnum_compare(pLongNum a, pLongNum b)
{
    int e = 0;
    if(longnum_greater(a, b))
        e = 1;
    if(longnum_less(a, b))
        e = -1;
    return e;
}

//Конвертация типов и систем счисления
pLongNum longnum_from_int(int num)
{
    pLongNum a = create_longnum();
    if(num < 0) a->s = -1;
    else a->s = 1;

    a->l = (int)log10(num) + 1;
    longnum_mem(a);
    for(int i = 0; i < a->l; i++)
    {
        *(a->n + i) = (int)(num / pow(10, i)) % (int)pow(10, a->l - i - 1);
    }
    longnum_nulldel(a);
    return a;
}
pLongNum longnum_parse(char *num, int s) //Преобразование строки в длинное число
{
    pLongNum a = malloc(sizeof(LongNum));

    if(longnum_ntd(*num, s) == -1)
        a->s = -1;
    else
        a->s = 1;

    a->l = 0;
    while(*(num + a->l) != 0)
        a->l++;
    longnum_mem(a);

    int k = 0;
    while (k < a->l)
    {
        if((longnum_ntd(*(num + k), s) != -1) && (longnum_ntd(*(num + k), s) != -2))
        {
            *(a->n + a->l - k - 1) = longnum_ntd(*(num + k), s);
        }
        k++;
    }

    longnum_nulldel(a);

    return a;
}
char *longnum_to_string(pLongNum a)
{
    int l = a->l;
    if(a->s == -1)
        l++;
    char *num = malloc((l + 1) * sizeof(char));

    for(int i = 0; i < a->l; i++)
    {
        *(num + l - i - 1) = (char)(*(a->n + i) + '0');
    }
    if(a->s == -1)
        *num = '-';
    *(num + l) = 0;
    return num;
}
double longnum_to_double(pLongNum a)
{
    long double num;
    if(a->l >= 308)
    {
        if(a->s == -1)
            num = -1 * INFINITY;
        else
            num = INFINITY;
    }
    else
    {
        double numm = 0;
        for(int i = 0; i < 10; i++)
            numm += *(a->n + a->l - i) * pow(10, -i);
        num = numm * pow(10, a->l);
    }
    return num;
}

/*pLongNum longnum_trans_to_dec(pLongNum a, int s)
{
    pLongNum c = malloc(sizeof(LongNum));
    pLongNum d = malloc(sizeof(LongNum));
    pLongNum b = malloc(sizeof(LongNum));
    b->l = 2;
    longnum_mem(b);
    b->s = a->s;
    *(b->n) = *(a->n + a->l - 1);
    *(b->n + 1) = (int)(*(b->n) / 10);
    *(b->n) %= 10;

    for(int i = 1; i < a->l - 1; i++)
    {
        d = longnum_from_int(*(a->n + a->l - 1 - i));
        c = longnum_mult(b, d);
        d = longnum_from_int(*(a->n + a->l - 2 - i));
        b = longnum_sum(c, d);
    }

    longnum_nulldel(b);

    return b;
}
pLongNum longnum_trans_from_dec(pLongNum a, int s)
{

}*/

//Арифметические операции
pLongNum longnum_add(pLongNum a, pLongNum b) //Сумма длинных чисел
{
    pLongNum c = malloc(sizeof(LongNum));

    if(a->s == b->s)
    {
        c->s = a->s;

        if(a->l > b->l)
            c->l = a->l + 1;
        else
            c->l = b->l + 1;
        longnum_mem(c);
        longnum_remem(a, c->l);
        longnum_remem(b, c->l);

        *c->n = 0;
        for(int i = 0; i < c->l; i++)
        {
            *(c->n + i) += *(a->n + i) + *(b->n + i);
            *(c->n + i + 1) = (int)(*(c->n + i) / 10);
            *(c->n + i) %= 10;
        }
    }
    else
    {
        if(b->s == -1)
        {
            b->s = 1;
            c = longnum_sub(a, b);
            b->s = -1;
        }
        else
        {
            a->s = 1;
            c = longnum_sub(b, a);
            a->s = -1;
        }
    }

    longnum_nulldel(a);
    longnum_nulldel(b);
    longnum_nulldel(c);
    return c;
}
pLongNum longnum_sub(pLongNum a, pLongNum b) //Разность длинных числел
{
    pLongNum c = malloc(sizeof(LongNum));

    if(a->s != b->s)
    {
        a->s *= -1;
        c = longnum_add(a, b);
        a->s *= -1;
        c->s = a->s;
        return c;
    }
    else
    {
        if(longnum_greater(b, a))
        {
            c = longnum_sub(b, a);
            c->s *= -1;
            return c;
        }
        else
        {
            c->s = a->s;

            if(a->l < b->l)
            {
                c->l = a->l + 1;
            }
            else
            {
                c->l = b->l + 1;
            }

            longnum_mem(c);
            longnum_remem(a, c->l);
            longnum_remem(b, c->l);
            *c->n = 0;

            for(int i = 0; i < c->l; i++)
            {
                *(c->n + i) += *(a->n + i) - *(b->n + i);
                if(*(c->n + i) < 0)
                {
                    *(c->n + i + 1) = -1;
                    *(c->n + i) += 10;
                }
                else
                {
                    *(c->n + i + 1) = 0;
                }
            }

            longnum_nulldel(a);
            longnum_nulldel(b);
            longnum_nulldel(c);
            return c;
        }
    }
}
pLongNum longnum_product(pLongNum a, pLongNum b) //Произведение длинных чисел
{
    pLongNum c = malloc(sizeof(LongNum));

    c->l = a->l + b->l;
    c->s = a->s * b->s;
    longnum_mem(c);
    *c->n = 0;

    for(int i = 0; i < b->l; i++)
    {
        pLongNum p = malloc(sizeof(LongNum));
        p->l = a->l + 1;
        longnum_mem(p);
        *p->n = 0;
        for(int j = 0; j < a->l; j++)
        {
            *(p->n + j) += *(a->n + j) * *(b->n + i);
            *(p->n + j + 1) = *(p->n + j) / 10;
            *(p->n + j) %= 10;
        }
        for(int k = 0; k < p->l; k++)
        {
            *(c->n + i + k) += *(p->n + k);
            *(c->n + i + k + 1) += *(c->n + i + k) / 10;
            *(c->n + i + k) %= 10;
        }
        free_longnum(p);
    }

    longnum_nulldel(c);

    return c;
}
pLongNum longnum_div(pLongNum a, pLongNum b) //Целое от деления
{
    int bs = b->s;
    b->s = 1;
    pLongNum c = malloc(sizeof(LongNum));
    c = a;
    c->s = 1;
    pLongNum e = malloc(sizeof(LongNum));
    pLongNum d = malloc(sizeof(LongNum));
    d->s = a->s * b->s;
    d->l = 1;
    longnum_mem(d);
    *d->n = 0;

    while(!longnum_less(c, b))
    {
        e = longnum_sub(c, b);
        c = e;
        d = longnum_add(d, longnum_parse("1", 10));
    }

    b->s = bs;
    d->s = a->s * b->s;
    free_longnum(c);
    //free_longnum(e);

    return d;
}
pLongNum longnum_rem(pLongNum a, pLongNum b) //Остаток от деления
{
    pLongNum d = longnum_div(a, b);
    pLongNum e = longnum_product(b, d);
    pLongNum c = longnum_sub(a, e);
    c->s = 1;

    free_longnum(d);
    free_longnum(e);

    return c;
}
pLongNum longnum_greatest_common_divisor(pLongNum a, pLongNum b) //Наибольший общий делитель
{
    pLongNum c = malloc(sizeof(LongNum));
    c = a;
    pLongNum d = malloc(sizeof(LongNum));
    d = b;
    pLongNum e = malloc(sizeof(LongNum));

    while(!longnum_equal(a, b))
    {
        if(longnum_greater(a, b))
        {
            a = longnum_sub(a, b);
        }
        else
        {
            b = longnum_sub(b, a);
        }
    }
    e = a;
    a = c;
    b = d;
    free_longnum(c);
    free_longnum(d);

    return e;
}

// Весь тестовый код здесь
int longnum_test_main()
{
    pLongNum a, b, c;

    a = longnum_parse("255", 10);
    b = longnum_parse("25", 10);

    longnum_print(a);
    longnum_print(b);

    //printf("%lf\n", longnum_to_double(a));
    //printf("%s\n", longnum_to_string(a));

    //c = longnum_sum(a, b);
    //longnum_print(c);

    //c = longnum_mult(a, b);
    //longnum_print(c);
    //printf("%s", longnum_to_string(c));

    //c = longnum_def(a, b);
    //longnum_print(c);

    //c = longnum_greatest_common_divisor(a, b);
    //longnum_print(c);

    c = longnum_div(a, b);
    longnum_print(c);

    c = longnum_rem(a, b);
    longnum_print(c);
    longnum_print(a);
    longnum_print(b);

    free_longnum(a);
    free_longnum(b);
    free_longnum(c);

    return 0;
}
