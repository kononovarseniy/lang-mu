#ifndef EXEC_H_INCLUDED
#define EXEC_H_INCLUDED

#include "context.h"
#include "stree.h"

#define MAX_ATOMS 102400
#define MAX_PAIRS 102400

enum VarType;
enum FunctionType;
typedef struct TypedValue TypedValue, *pTypedValue;
typedef struct Function Function, *pFunction;
typedef struct Pair Pair, *pPair;
typedef struct Executor Executor, *pExecutor;
#define BUILTIN_FUNC(NAME) pTypedValue NAME(pExecutor exec, pContext defContext, pContext callContext, int *args, int argc)
typedef BUILTIN_FUNC((*BuiltinFunction));

enum ValueType
{
    VT_NONE,
    VT_EXPR,
    VT_FUNC,
    VT_INT,
    VT_CHAR,
    VT_STRING
};

enum FunctionType
{
    FT_NONE,
    FT_BUILTIN
};

struct TypedValue
{
    int links;
    enum ValueType type;
    union
    {
        int expr;
        pFunction func;
    };
};

struct Function
{
    enum FunctionType type;
    pContext context;
    union
    {
        BuiltinFunction builtin;
    };
};

struct Pair
{
    int left;
    int right;
};

struct Executor
{
    int atomsCount;
    int pairsCount;
    char **atoms;
    Pair *pairs;

    int nil;
    int t;
};


pExecutor create_executor(void);
void free_executor(pExecutor exec);

void exec_init(pExecutor exec, pContext context);
int exec_load_tree(pExecutor exec, pSTree tree);
pTypedValue exec_eval(pExecutor exec, int expr, pContext context);

#endif // EXEC_H_INCLUDED
