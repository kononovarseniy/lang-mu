#ifndef LONGNUM_H_INCLUDED
#define LONGNUM_H_INCLUDED

typedef struct LongNum
{
    int *n;
    int l;
    int s;
} LongNum, *pLongNum;

pLongNum create_longnum();
void free_longnum(pLongNum a);

pLongNum longnum_parse(char *num, int s); //Преобразование строки в длинное число
// TODO: replace by function longnum_to_string
void longnum_print(pLongNum a);


// Функция тестирования кода
int longnum_test_main();

#endif // LONGNUM_H_INCLUDED
