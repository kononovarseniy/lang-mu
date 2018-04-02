#include "print.h"

#include <string.h>
#include "log.h"
#include "strutils.h"

void print_indent(FILE *f, int i, enum PrintingFlags flags)
{
    if (flags & PF_PRETTY)
    {
        while (i-- > 0)
        {
            fprintf(f, "    ");
        }
    }
}

void print_wrap(FILE *f, enum PrintingFlags flags)
{
    if (flags & PF_PRETTY)
        fprintf(f, "\n");
}

void print_quotation(FILE *f, enum PrintingFlags *flags)
{
    if (*flags & PF_FORCE_QUOTE)
    {
        fputc('\'', f);
        *flags &= ~PF_FORCE_QUOTE;
    }
}

void print_atom(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    char *name = exec->atoms[expr.val_atom];

    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    fprintf(f, "%s", name);
    print_wrap(f, flags);
}

int is_quote_struct(pExecutor exec, Expr *list, int len)
{
    return  len == 2 &&
            list[0].type == VT_ATOM &&
            list[0].val_atom == exec->quote.val_atom;
}

void print_list(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    int len;
    Expr *list = get_list(exec, expr, &len);

    int is_quote = is_quote_struct(exec, list, len);
    if ((flags & PF_SHORT_QUOTE) && is_quote)
    {
        print_expression(f, exec, list[1], flags | PF_FORCE_QUOTE, indent);
        free(list);
        return;
    }

    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    fprintf(f, "(");
    print_wrap(f, flags);

    for (int i = 0; i < len; i++)
    {
        print_expression(f, exec, list[i], flags, indent + 1);
        if (!(flags & PF_PRETTY) && i + 1 < len)
            fprintf(f, " ");
    }

    print_indent(f, indent, flags);
    fprintf(f, ")");
    print_wrap(f, flags);

    free(list);
}

void print_int(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    fprintf(f, "%ld", expr.val_int);
    print_wrap(f, flags);
}

void print_string(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    if (flags & PF_UNESCAPE)
        fprintf(f, "%s", expr.val_str);
    else
    {
        char *res = escape_string(expr.val_str, strlen(expr.val_str));
        fprintf(f, "\"%s\"", res);
        free(res);
    }
    print_wrap(f, flags);
}

void print_char(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    if (flags & PF_UNESCAPE)
        fprintf(f, "%c", expr.val_char);
    else
    {
        char str[] = { expr.val_char };
        char *res = escape_string(str, 1);
        fprintf(f, "\'%s\'", res);
        free(res);
    }
    print_wrap(f, flags);
}

void print_func(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);

    // TODO: implement print_func
    log("print_func: not implemented");
    fprintf(f, "`print_func: not implemented`\n");

    print_wrap(f, flags);
}

void print_expression(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flags, int indent)
{
    switch (expr.type)
    {
    case VT_ATOM:
        print_atom(f, exec, expr, flags, indent);
        break;
    case VT_PAIR:
        print_list(f, exec, expr, flags, indent);
        break;
    case VT_INT:
        print_int(f, exec, expr, flags, indent);
        break;
    case VT_STRING:
        print_string(f, exec, expr, flags, indent);
        break;
    case VT_CHAR:
        print_char(f, exec, expr, flags, indent);
        break;
    case VT_FUNC:
        print_func(f, exec, expr, flags, indent);
        break;
    default:
        log("print_expression: unknown value type");
    }
}
