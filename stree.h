#ifndef STREE_H_INCLUDED
#define STREE_H_INCLUDED

enum NodeType
{
    NODE_NONE,
    NODE_LIST,
    NODE_NAME,
    NODE_INT,
    NODE_STR,
    NODE_CHAR
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
        char *str_val;
        pSTree child;

        long int_val;
        char char_val;
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
