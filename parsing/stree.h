#ifndef STREE_H_INCLUDED
#define STREE_H_INCLUDED

#include "types/longint.h"
#include "types/string.h"

enum NodeType
{
    NODE_NONE,
    NODE_NAME,
    NODE_LIST,
    NODE_CHAR,
    NODE_INT,
    NODE_REAL,
    NODE_STR
};

typedef struct STree STree, *pSTree;
struct STree
{
    pSTree next;
    enum NodeType type;
    union
    {
        void *data;
        char *name;
        pSTree child;

        char val_char;
        pLongInt val_int;
        double val_real;
        pString val_str;
    };
};

pSTree create_stree(void);
void free_stree(pSTree tree);
pSTree stree_get_next(pSTree node, int offset);
pSTree stree_get_child(pSTree node, int offset);
void stree_append(pSTree node, pSTree newNode);
void stree_append_child(pSTree node, pSTree newNode);
int stree_count(pSTree node);
int stree_count_children(pSTree node);

#endif // STREE_H_INCLUDED
