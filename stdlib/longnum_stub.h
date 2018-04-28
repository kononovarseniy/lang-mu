#ifndef LONGNUM_H_INCLUDED
#define LONGNUM_H_INCLUDED


#define DIGIT_SIZE sizeof(short)
typedef struct LongNum
{
    long num;
} LongNum, *pLongNum;

pLongNum create_longnum(); //Создание числа
void free_longnum(pLongNum a); //Удаление числа
pLongNum longnum_copy(pLongNum num);
pLongNum longnum_zero();
pLongNum longnum_one();
pLongNum longnum_negative_one();

pLongNum longnum_parse(char *num, int s); //Строка в число
pLongNum longnum_from_int(int num);
char *longnum_to_string(pLongNum a, int base);
long longnum_to_int(pLongNum a);
double longnum_to_double(pLongNum a); //Длинное число в число с плавающей точкой
//pLongNum longnum_trans_to_dec(pLongNum a, int s); //Перевод в десятичную СС - в разработке
//pLongNum longnum_trans_from_dec(pLongNum a, int s); //Перевод из десятичной СС - в разработке

//Функции с основными арифметическими действиями
pLongNum longnum_add(pLongNum a, pLongNum b); //"+"
pLongNum longnum_sub(pLongNum a, pLongNum b); //"-"
pLongNum longnum_inverse(pLongNum a);
pLongNum longnum_product(pLongNum a, pLongNum b); //"*"
pLongNum longnum_div(pLongNum a, pLongNum b); //Целое от деления
pLongNum longnum_rem(pLongNum a, pLongNum b); //Остаток от деления
pLongNum longnum_greatest_common_divisor(pLongNum a, pLongNum b); //Наибольший общий делитель

//Функции сравнения двух длинных чисел
_Bool longnum_greater(pLongNum a, pLongNum b); //">"
_Bool longnum_less(pLongNum a, pLongNum b); //"<"
_Bool longnum_equal(pLongNum a, pLongNum b); //"="
_Bool longnum_greater_or_equal(pLongNum a, pLongNum b); //">="
_Bool longnum_less_or_equal(pLongNum a, pLongNum b); //"<="
int longnum_compare(pLongNum a, pLongNum b);

#endif // LONGNUM_H_INCLUDED
