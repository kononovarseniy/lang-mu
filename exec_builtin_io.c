#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"

#include "print.h"

BUILTIN_FUNC(print)
{
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stdout, exec, value, PF_SHORT_QUOTE, 0);
        if (i < argc - 1) printf(" ");
    }
    return exec->t;
}

BUILTIN_FUNC(prints)
{
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stdout, exec, value, PF_SHORT_QUOTE | PF_UNESCAPE, 0);
        if (i < argc - 1) printf(" ");
    }
    return exec->t;
}

#define BLOCK_SIZE 256
char *read_line(FILE *file)
{
	int size = BLOCK_SIZE;
	char *res = malloc(BLOCK_SIZE * sizeof(char));
	char ch;
	int i = 0;
	int isEnd = 0;
	do
	{
		ch = fgetc(file);
		if (i >= size)
		{
			size += BLOCK_SIZE;
			res = realloc(res, size * sizeof(char));
		}
		if (ch == '\n' || ch == EOF)
		{
			res[i++] = '\0';
			isEnd = 1;
		}
		else
		{
			res[i++] = ch;
		}
	} while (!isEnd);
	if (i > 0 && res[i - 1] == '\r')
    {
        res[i - 1] = '\0';
        i--;
    }
	return realloc(res, i * sizeof(char));
}

BUILTIN_FUNC(scanline)
{
    fflush(stdout);
    char *str = read_line(stdin);

    Expr res = make_string_from_array(exec, str, strlen(str));
    free(str);
    if (is_none(res))
    {
        log("scanline: make_string_from_array failed");
        exit(1);
    }
    return res;
}
