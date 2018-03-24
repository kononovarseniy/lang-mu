#include "stree.h"
#include <stdlib.h>

pSTree create_stree(void)
{
    pSTree res = malloc(sizeof(STree));
    res->next = NULL;
    res->child = NULL;
    res->type = NODE_NONE;
    return res;
}

void free_stree(pSTree tree)
{
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
    if (node == NULL) return NULL;
    return stree_get_next(node->child, offset);
}

void stree_append(pSTree node, pSTree newNode)
{
    if (node == NULL) return;
    while (node->next != NULL) node = node->next;
    node->next = newNode;
}

void stree_append_child(pSTree node, pSTree newNode)
{
    if (node == NULL) return;
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
    if (node == NULL) return 0;
    return stree_count(node->child);
}
