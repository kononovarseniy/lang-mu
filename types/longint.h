#ifndef LANG_MU_TYPES_LONGINT_H_INCLUDED
#define LANG_MU_TYPES_LONGINT_H_INCLUDED


typedef struct LongInt LongInt, *pLongInt;

pLongInt create_longint(); //Создание числа
void free_longint(pLongInt a); //Удаление числа
pLongInt longint_copy(pLongInt num);

pLongInt longint_zero();

pLongInt longint_one();

pLongInt longint_negative_one();

pLongInt longint_parse(char *num, int s); //Строка в число
pLongInt longint_from_long(long num);

pLongInt longint_from_double(double num);

char *longint_to_string(pLongInt a, int base);

long longint_to_long(pLongInt a);

double longint_to_double(pLongInt a); //Длинное число в число с плавающей точкой

//Функции с основными арифметическими действиями
pLongInt longint_sum(pLongInt a, pLongInt b);

pLongInt longint_difference(pLongInt a, pLongInt b);

pLongInt longint_inverse(pLongInt a);

pLongInt longint_product(pLongInt a, pLongInt b);

pLongInt longint_quotient(pLongInt a, pLongInt b);

pLongInt longint_remainder(pLongInt a, pLongInt b);

pLongInt longint_gcd(pLongInt a, pLongInt b);

int longint_compare(pLongInt a, pLongInt b);

#endif // LANG_MU_TYPES_LONGINT_H_INCLUDED
