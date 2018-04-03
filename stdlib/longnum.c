#include "longnum.h"

#include <stdlib.h>
#include <stdio.h>

//Работа с памятью
pLongNum create_longnum()
{
    pLongNum res = malloc(sizeof(LongNum));
    if (res == NULL)
    {
        printf("create_longnum: malloc failed\n");
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
    a->n = malloc(a->l * sizeof(int));
}
void longnum_remem(pLongNum a, int l)
{
    int *b = malloc(a->l * sizeof(int));
    for (int i = 0; i < a->l; i++)
        *(b + i) = *(a->n + i);

    free (a->n);
    a->n = malloc(l * sizeof(int));


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
        printf("%d", *(a->n + i));
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
        else
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
    }
    else
    {
        if(a->s == -1)
            e = 1;
        else
            e = 0;
    }

    return e;
}
_Bool longnum_equal(pLongNum a, pLongNum b) //"="
{
    _Bool e = 0;

    if(a->s == b->s)
        if(a->l == b->l)
            for(int i = 0; i < a->l; i++)
                if(*(a->n + i) != *(b->n + i))
                    break;
                else
                    e = 1;

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
_Bool longnum_unequal(pLongNum a, pLongNum b) //"!="
{
    _Bool e = 0;
    if(longnum_equal(a, b) == 0)
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

//Арифметические операции
pLongNum longnum_parse(char *num, int s) //Преобразование строки в длинное число
{
    //Не работает с системами счисления, отличными от десятичной, пока

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
            *(a->n + a->l - k - 1) += longnum_ntd(*(num + k), s);
            *(a->n + a->l - k) += (int)(*(a->n + a->l - k - 1) / 10);
            *(a->n + a->l - k - 1) %= 10;
        }
        k++;
    }

    longnum_nulldel(a);

    return a;
}
pLongNum longnum_def(pLongNum a, pLongNum b) //Разность длинных числел
{
    pLongNum c = malloc(sizeof(LongNum));

    return c;
}
pLongNum longnum_sum(pLongNum a, pLongNum b) //Сумма длинных чисел
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
            c = longnum_def(a, b);
            b->s = -1;
        }
        else
        {
            a->s = 1;
            c = longnum_def(b, a);
            a->s = -1;
        }
    }

    longnum_nulldel(a);
    longnum_nulldel(b);
    longnum_nulldel(c);
    return c;
}
pLongNum longnum_comp(pLongNum a, pLongNum b) //Произведение длинных чисел
{
    pLongNum c = malloc(sizeof(LongNum));

    return c;
}

// Весь тестовый код здесь
int longnum_test_main()
{
    pLongNum a = create_longnum();
    pLongNum b = create_longnum();

    a = longnum_parse("-63", 10);
    b = longnum_parse("-63", 10);

    longnum_print(a);
    longnum_print(b);

    //c = sum(a, b);
    //longnum_prt(c);

    //c = comp(a, b);
    //longnum_prt(c);

    free_longnum(a);
    free_longnum(b);
    //longnum_del(c);

    return 0;
}
