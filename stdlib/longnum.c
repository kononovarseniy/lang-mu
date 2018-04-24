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
    short *b = malloc(a->l * DIGIT_SIZE);
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
    for (int i = 0; i < a->l; i++)
    {
        int d = num % 10;
        a->n[i] = d;
        num /= 10;
    }
    longnum_nulldel(a);
    return a;
}
pLongNum longnum_parse(char *num, int s) //Преобразование строки в длинное число
{
    pLongNum a = create_longnum();

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
    pLongNum c = create_longnum();

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
            *(c->n + i + 1) = *(c->n + i) / 10;
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
    pLongNum c = create_longnum();

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
    pLongNum c = create_longnum();

    c->l = a->l + b->l;
    c->s = a->s * b->s;
    longnum_mem(c);
    *c->n = 0;

    for(int i = 0; i < b->l; i++)
    {
        pLongNum p = create_longnum();
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
pLongNum longnum_product_10(pLongNum a, int d)
{
    if(a == NULL)
    {
        printf("longnum_product_10: NULL pointer\n");
        exit(1);
    }
    if(d < 0)
    {
        printf("longnum_product_10: negative number\n");
        exit(1);
    }
    int l = a->l;
    longnum_remem(a, l + d);
    for(int i = 0; i < a->l; i++)
    {
        if(i < l)
        {
            *(a->n + a->l - i - 1) = *(a->n + a->l - d - i - 1);
        }
        else
        {
            *(a->n + a->l - i - 1) = 0;
        }
    }
    return a;
}
pLongNum longnum_div(pLongNum a, pLongNum b) //Целое от деления
{
    if(a == NULL || b == NULL)
    {
        printf("longnum_div: NULL pointer\n");
        exit(1);
    }
    pLongNum div = longnum_parse("0", 10);
    pLongNum one = longnum_parse("1", 10);
    pLongNum nill = div;
    if(longnum_greater(b, a))
    {
        return div;
    }
    pLongNum x = a;
    pLongNum y = b;
    pLongNum c;
    pLongNum d;
    pLongNum e;
    int dl;
    for(int i = 0; i < a->l; i++)
    {
        c = y;
        dl = x->l - y->l;
        if(dl > 1) c = longnum_product_10(y, dl - 1);
        while(longnum_greater_or_equal(x, c))
        {
            d = longnum_sub(x, c);
            x = d;
            d = longnum_product_10(one, i);
            e = div;
            div = longnum_add(e , d);
        }
        longnum_nulldel(x);
        if(longnum_equal(x, nill)) break;
    }
    //free_longnum(c);
    //free_longnum(d);
    //free_longnum(e);
    return div;
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
    pLongNum c = a;
    pLongNum d = b;
    pLongNum e = create_longnum();

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

    a = longnum_parse("100", 10);
    b = longnum_parse("2", 10);

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

    //c = longnum_rem(a, b);
    //longnum_print(c);
    //longnum_print(a);
    //longnum_print(b);

    //c = longnum_product_10(a, 10);
    //longnum_print(c);

    free_longnum(a);
    free_longnum(b);
    free_longnum(c);

    return 0;
}
