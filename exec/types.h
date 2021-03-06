#ifndef LANG_MU_EXEC_TYPES_H_INCLUDED
#define LANG_MU_EXEC_TYPES_H_INCLUDED

#include <limits.h>

#include "types/longint.h"
#include "types/string.h"
#include "utils/map.h"

#define EXPR_ERROR INT_MAX
#define EXPR_NOT_FOUND (INT_MAX - 1)

enum ValueType;
typedef struct Expr Expr;

typedef struct Context Context, *pContext;
typedef struct ContextStack ContextStack, *pContextStack;
typedef struct ContextStackFrame ContextStackFrame, *pContextStackFrame;

enum GCFlags;
typedef struct Pointer Pointer, *pPointer;
typedef struct PointerInfo PointerInfo, *pPointerInfo;
typedef struct Heap Heap, *pHeap;

enum FunctionType;
typedef struct Function Function, *pFunction;
typedef struct UserFunction UserFunction, *pUserFunction;

typedef struct Executor Executor, *pExecutor;

#define BUILTIN_FUNC(NAME) Expr NAME(pExecutor exec, pContext defContext, pContext callContext, Expr *args, int argc)

typedef BUILTIN_FUNC((*pBuiltinFunction));

enum ValueType
{
    VT_NONE = 0,
    VT_ATOM = 1,
    VT_PAIR = 2,
    VT_CHAR = 3,
    VT_INT_VAL = 4,
    VT_REAL = 5,
    VT_STRING_VAL = 6,
    VT_FUNC_VAL = 7,

    VT_POINTER = 1 << 7,
    VT_INT_PTR = VT_INT_VAL | VT_POINTER,
    VT_STRING_PTR = VT_STRING_VAL | VT_POINTER,
    VT_FUNC_PTR = VT_FUNC_VAL | VT_POINTER
};

struct Expr
{
    enum ValueType type;
    union
    {
        size_t val_atom;
        size_t val_pair;
        char val_char;
        pLongInt val_int;
        double val_real;
        pString val_str;
        pFunction val_func;
        pPointer val_ptr;
    };
};

struct Context
{
    int gc_index;
    int links;
    pContext base;
    pMap bindings;
    pMap macros;
};

struct ContextStack
{
    pContextStackFrame head;
};

struct ContextStackFrame
{
    pContext context;
    pContextStackFrame next;
};

enum GCFlags
{
    GC_NONE = 0,
    GC_USED = 1 << 0,
    GC_REFERENCED = 1 << 1
};

struct Pointer
{
    size_t id;
    Expr value;
};

struct PointerInfo
{
    enum GCFlags flags;
    pPointer ptr;
};

struct Heap
{
    size_t size;
    pPointerInfo values;
};

enum FunctionType
{
    FT_NONE,
    FT_BUILTIN,
    FT_USER,
    FT_USER_MACRO
};

struct Function
{
    enum FunctionType type;
    int gc_index;
    pContext context;
    union
    {
        pBuiltinFunction builtin;
        pUserFunction user;
    };
};

struct UserFunction
{
    Expr *args;
    int argc;
    Expr *opt;
    Expr *def;
    int optc;
    Expr rest;
    Expr body;
};

struct Executor
{
    int gc_index;
    pContextStack stack;
    pContext global;
    Expr code;

    size_t atomsCount;
    size_t pairsCount;

    enum GCFlags *atomsFlags;
    char **atoms;
    enum GCFlags *pairsFlags;
    Expr *cars;
    Expr *cdrs;
    pHeap heap;

    Expr nil;
    Expr t;
    Expr quote;
    Expr comma;
    Expr comma_atsign;
};

#endif // LANG_MU_EXEC_TYPES_H_INCLUDED
