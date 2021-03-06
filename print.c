#include "print.h"

#include <string.h>
#include "utils/log.h"
#include "utils/strutils.h"

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
        if (flags & PF_FORCE_QUOTE)
        {
            // quote only first quotation
            flags &= ~PF_SHORT_QUOTE;
        }
        else
        {
            print_expression(f, exec, list[1], flags | PF_FORCE_QUOTE, indent);
            free(list);
            return;
        }
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

void print_int(FILE *f, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);

    expr = dereference(expr);

    char *str = longint_to_string(expr.val_int, 10);
    fprintf(f, "%s", str);
    free(str);

    print_wrap(f, flags);
}

void print_real(FILE *f, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);
    fprintf(f, "%g", expr.val_real);
    print_wrap(f, flags);
}

void print_string(FILE *f, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);

    expr = dereference(expr);

    char *cstr = string_to_cstring(expr.val_str);
    if (flags & PF_UNESCAPE)
        fprintf(f, "%s", cstr);
    else
    {
        char *res = escape_string(cstr, strlen(cstr));
        fprintf(f, "\"%s\"", res);
        free(res);
    }
    free(cstr);
    print_wrap(f, flags);
}

void print_char(FILE *f, Expr expr, enum PrintingFlags flags, int indent)
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

void print_func(FILE *f, Expr expr, enum PrintingFlags flags, int indent)
{
    print_indent(f, indent, flags);
    print_quotation(f, &flags);

    expr = dereference(expr);

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
    case VT_INT_PTR:
        print_int(f, expr, flags, indent);
        break;
    case VT_REAL:
        print_real(f, expr, flags, indent);
        break;
    case VT_STRING_PTR:
        print_string(f, expr, flags, indent);
        break;
    case VT_CHAR:
        print_char(f, expr, flags, indent);
        break;
    case VT_FUNC_PTR:
        print_func(f, expr, flags, indent);
        break;
    default:
        log("print_expression: unknown value type");
    }
}
