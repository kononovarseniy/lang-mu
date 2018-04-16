#include "stree.h"
#include <stdlib.h>
#include <stdio.h>
#include "log.h"

pSTree create_stree(void)
{
    pSTree res;
    if ((res = malloc(sizeof(STree))) == NULL)
    {
        perror("create_stree: malloc failed");
        return NULL;
    }

    res->next = NULL;
    res->type = NODE_NONE;
    res->data = NULL;
    return res;
}

void free_stree(pSTree tree)
{
    switch (tree->type)
    {
    case NODE_STR:
        free(tree->str_val);
        break;
    case NODE_NAME:
        free(tree->name);
        break;
    case NODE_LIST:
        if (tree->child != NULL)
            free_stree(tree->child);
        break;
    default:
        break;
    }
    if (tree->next != NULL)
        free_stree(tree->next);
    free(tree);
}

pSTree stree_get_next(pSTree node, int offset)
{
    while (offset != 0 && node != NULL)
    {
        node = node->next;
        offset--;
    }
    return node;
}

pSTree stree_get_child(pSTree node, int offset)
{
    if (node == NULL || node->type != NODE_LIST) return NULL;
    return stree_get_next(node->child, offset);
}

void stree_append(pSTree node, pSTree newNode)
{
    if (node == NULL)
    {
        log("WARNING: stree_append: appending to NULL");
        return;
    }
    while (node->next != NULL) node = node->next;
    node->next = newNode;
}

void stree_append_child(pSTree node, pSTree newNode)
{
    if (node == NULL || node->type != NODE_LIST)
    {
        log("WARNING: stree_append_child: appending to NULL");
        return;
    }
    stree_append(node->child, newNode);
}

int stree_count(pSTree node)
{
    int cnt = 0;
    while (node != NULL)
    {
        node = node->next;
        cnt++;
    }
    return cnt;
}

int stree_count_children(pSTree node)
{
    if (node == NULL || node->type != NODE_LIST) return 0;
    return stree_count(node->child);
}
