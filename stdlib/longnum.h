#ifndef LONGNUM_H_INCLUDED
#define LONGNUM_H_INCLUDED

typedef struct LongNum
{
    short *n;
    unsigned int l;
    int s;
} LongNum, *pLongNum;

pLongNum create_longnum(); //Создание числа
void free_longnum(pLongNum a); //Удаление числа

pLongNum longnum_parse(char *num, int s); //Строка в число
char *longnum_to_string(pLongNum a); //Число в строку - не работает
double longnum_to_double(pLongNum a); //Длинное число в число с плавающей точкой

//Функции с основными арифметическими действиями
pLongNum longnum_sum(pLongNum a, pLongNum b); //"+" - работает частично
//pLongNum longnum_def(pLongNum a, pLongNum b); //"-" - не работает
pLongNum longnum_mult(pLongNum a, pLongNum b); //"*"

//Функции сравнения двух длинных чисел
_Bool longnum_greater(pLongNum a, pLongNum b); //">"
_Bool longnum_less(pLongNum a, pLongNum b); //"<"
_Bool longnum_equal(pLongNum a, pLongNum b); //"="
_Bool longnum_greater_or_equal(pLongNum a, pLongNum b); //">="
_Bool longnum_less_or_equal(pLongNum a, pLongNum b); //"<="
int longnum_compare(pLongNum a, pLongNum b);

// Функция тестирования кода
int longnum_test_main();

#endif // LONGNUM_H_INCLUDED
