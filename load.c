#include "load.h"

#include "utils/log.h"

Expr load_atom(pExecutor exec, pSTree item)
{
    return make_atom(exec, item->name);
}

Expr load_int(pExecutor exec, pSTree item)
{
    return make_int(exec, item->val_int);
}

Expr load_real(pExecutor exec, pSTree item)
{
    return make_real(exec, item->val_real);
}

Expr load_char(pExecutor exec, pSTree item)
{
    return make_char(exec, item->val_char);
}

Expr load_string(pExecutor exec, pSTree item)
{
    return make_string(exec, item->val_str);
}

Expr load_item(pExecutor exec, pSTree item)
{
    switch (item->type)
    {
    case NODE_LIST:
        return load_parsed_tree(exec, item->child);
    case NODE_NAME:
        return load_atom(exec, item);
    case NODE_INT:
        return load_int(exec, item);
    case NODE_REAL:
        return load_real(exec, item);
    case NODE_CHAR:
        return load_char(exec, item);
    case NODE_STR:
        return load_string(exec, item);
    default:
        log("load_item: unknown node type");
        return make_none();
    }
}

Expr load_parsed_tree(pExecutor exec, pSTree tree)
{
    if (tree == NULL) return exec->nil;

    // Load head
    Expr head = load_item(exec, tree);
    if (is_none(head))
    {
        log("load_parsed_tree: head loading failed");
        return make_none();
    }

    // Load tail
    Expr tail = load_parsed_tree(exec, tree->next);
    if (is_none(tail))
    {
        log("load_parsed_tree: tail loading failed");
        return make_none();
    }

    // Make pair
    Expr pair = make_pair(exec, head, tail);
    if (is_none(pair))
    {
        log("load_parsed_tree: make_pair failed");
        return make_none();
    }
    return pair;
}
