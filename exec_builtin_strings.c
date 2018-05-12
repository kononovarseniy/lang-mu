#include "exec_builtin.h"

#include "log.h"
#include "stdlib/longnum_stub.h"
#include "stdlib/str.h"

void hlp_check_args_count(int argc, int min, int max, char *caller)
{
    if (argc < min)
    {
        logf("%s: too few arguments", caller);
        exit(1);
    }
    if (argc > max)
    {
        logf("%s: too many arguments", caller);
        exit(1);
    }
}

BUILTIN_FUNC(str_from_list)
{
    if (argc < 2)
    {
        log("__str-from-list: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("__str-from-list: too many arguments");
        exit(1);
    }
    // Evaluate all arguments
    Expr list = exec_eval(exec, callContext, args[0]);
    Expr length = dereference(exec_eval(exec, callContext, args[1]));

    int len;
    Expr *items = get_list(exec, list, &len);

    size_t res_len = (size_t)longnum_to_long(length.val_int);

    pString str = string_allocate_memory(res_len);
    for (size_t i = 0; i < res_len; i++)
    {
        if (!is_char(items[i]))
        {
            free_string(str);
            log("__str-from-list: `list` argument must contain only characters");
            exit(1);
        }
        str->buf[i] = items[i].val_char;
    }
    free(items);

    Expr res = make_string(exec, str);
    free_string(str);
    if (is_none(res))
    {
        log("__str-from-list: make_string failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_to_list)
{
    if (argc < 3)
    {
        log("__str-to-list: too few arguments");
        exit(1);
    }
    if (argc > 3)
    {
        log("__str-to-list: too many arguments");
        exit(1);
    }
    Expr str_arg = dereference(exec_eval(exec, callContext, args[0]));
    Expr start_arg = dereference(exec_eval(exec, callContext, args[1]));
    Expr length_arg = dereference(exec_eval(exec, callContext, args[2]));

    pString str = str_arg.val_str;
    size_t start = (size_t)longnum_to_long(start_arg.val_int);
    size_t length = (size_t)longnum_to_long(length_arg.val_int);

    Expr *list = malloc(length * sizeof(Expr));
    if (list == NULL)
    {
        perror("__str-to-list: memory allocation failed");
        exit(1);
    }
    for (size_t i = 0; i < length; i++)
    {
        list[i] = make_char(exec, str->buf[start + i]);
    }
    Expr res = make_list(exec, list, length);
    free(list);
    if (is_none(res))
    {
        log("__str-to-list: make_list failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_repeat_char)
{
    hlp_check_args_count(argc, 2, 2, "__str-repeat-char");
    Expr sub = EVAL_ARG(0);
    Expr count = dereference(EVAL_ARG(1));

    // FIXME: handle very big counts
    size_t len = (size_t)longnum_to_long(count.val_int);

    pString str = string_repeat_char(sub.val_char, len);
    if (str == NULL)
    {
        log("__str-repeat-char: string_repeat-char failed");
        exit(1);
    }
    Expr res = make_string(exec, str);
    free_string(str);
    if (is_none(res))
    {
        log("__str-repeat-char: make_string failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_repeat_str)
{
    hlp_check_args_count(argc, 2, 2, "__str-repeat-str");
    Expr sub = dereference(EVAL_ARG(0));
    Expr count = dereference(EVAL_ARG(1));

    // FIXME: handle very big counts
    size_t len = (size_t)longnum_to_long(count.val_int);

    pString str = string_repeat_string(sub.val_str, len);
    if (str == NULL)
    {
        log("__str-repeat-str: string_repeat_string failed");
        exit(1);
    }
    Expr res = make_string(exec, str);
    free_string(str);
    if (is_none(res))
    {
        log("__str-repeat-str: make_string failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_len)
{
    if (argc < 1)
    {
        log("str-len: too few arguments");
        exit(1);
    }
    if (argc > 1)
    {
        log("str-len: too many arguments");
        exit(1);
    }
    Expr str_arg = exec_eval(exec, callContext, args[0]);
    if (!is_string(str_arg))
    {
        log("str-len: argument `string` not a string");
        exit(1);
    }
    pString str = dereference(str_arg).val_str;

    Expr res = make_int_from_long(exec, str->len);
    if (is_none(res))
    {
        log("str-len: make_int_from_long failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_at)
{
    if (argc < 2)
    {
        log("str-at: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("str-at: too many arguments");
        exit(1);
    }
    Expr str_arg = exec_eval(exec, callContext, args[0]);
    if (!is_string(str_arg))
    {
        log("str-at: argument `string` not a string");
        exit(1);
    }
    pString str = dereference(str_arg).val_str;

    Expr ind_arg = exec_eval(exec, callContext, args[1]);
    if (!is_int(ind_arg))
    {
        log("str-at: argument `index` not a string");
        exit(1);
    }
    ind_arg = dereference(ind_arg);

    pLongNum zero = longnum_zero();
    pLongNum long_len = longnum_from_long(str->len);
    _Bool out_of_range =
        longnum_compare(ind_arg.val_int, zero) < 0 ||
        longnum_compare(ind_arg.val_int, long_len) >= 0;
    free_longnum(zero);
    free_longnum(long_len);
    if (out_of_range)
    {
        log("str-at: argument `index` is out of range");
        exit(1);
    }
    size_t ind = (size_t)longnum_to_long(ind_arg.val_int);

    return make_char(exec, str->buf[ind]);
}

BUILTIN_FUNC(str_cmp_sub)
{
    hlp_check_args_count(argc, 6, 6, "__str-cmp-sub");
    pString a = dereference(EVAL_ARG(0)).val_str;
    size_t a_start = longnum_to_long(dereference(EVAL_ARG(1)).val_int);
    size_t a_len = longnum_to_long(dereference(EVAL_ARG(2)).val_int);
    pString b = dereference(EVAL_ARG(3)).val_str;
    size_t b_start = longnum_to_long(dereference(EVAL_ARG(4)).val_int);
    size_t b_len = longnum_to_long(dereference(EVAL_ARG(5)).val_int);

    int cmp_res = string_compare_sub(a, a_start, a_len, b, b_start, b_len);
    Expr res = make_int_from_long(exec, cmp_res);
    if (is_none(res))
    {
        log("__str-cmp-sub: make_int_from_long failed");
        exit(1);
    }
    return res;
}
BUILTIN_FUNC(str_cat)
{
    pString *strs = malloc(argc * sizeof(pString));
    if (strs == NULL)
    {
        log("__str-cat: memory allocation failed");
        exit(1);
    }
    for (size_t i = 0; i < argc; i++)
        strs[i] = dereference(EVAL_ARG(i)).val_str;
    pString res_str = string_concat(strs, argc);
    free(strs);

    if (res_str == NULL)
    {
        log("__str-cat: string_concat failed");
        exit(1);
    }
    Expr res = make_string(exec, res_str);
    free_string(res_str);
    if (is_none(res))
    {
        log("__str-cat: make_string failed");
        exit(1);
    }
    return res;
}
BUILTIN_FUNC(str_ind)
{
    hlp_check_args_count(argc, 5, 5, "__str-ind");

    bool reverse = is_true(exec, EVAL_ARG(0));
    pString str = dereference(EVAL_ARG(1)).val_str;
    Expr arg_sub = EVAL_ARG(2);
    size_t start = (size_t)longnum_to_long(dereference(EVAL_ARG(3)).val_int);
    size_t count = (size_t)longnum_to_long(dereference(EVAL_ARG(4)).val_int);

    size_t ind;
    if (is_string(arg_sub))
    {
        pString sub = dereference(arg_sub).val_str;
        // I'm just playing with C syntax :) It should call string_index_of or string_last_index_of
        ind = (reverse ? string_last_index_of : string_index_of)(str, sub, start, count);
    }
    else if (is_char(arg_sub))
    {
        char sub = arg_sub.val_char;
        ind = (reverse ? string_last_index_of_char : string_index_of_char)(str, sub, start, count);
    }
    else
    {
        log("__str-ind: third argument must be string or char");
        exit(1);
    }

    Expr res;
    if (ind == STR_ERROR)
        res = make_int_negative_one(exec);
    else
        res = make_int_from_long(exec, (long)ind);
    if (is_none(res))
    {
        log("__str-ind: make_int failed");
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(str_insert);
BUILTIN_FUNC(str_sub);
BUILTIN_FUNC(str_remove);

BUILTIN_FUNC(to_lower);
BUILTIN_FUNC(to_upper);
