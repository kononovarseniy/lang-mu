#ifndef EXEC_TYPES_H_INCLUDED
#define EXEC_TYPES_H_INCLUDED

#include "stdlib/longnum_stub.h"
#include "stdlib/str.h"

#include "context.h"

typedef struct Context Context, *pContext; // Defined in context.h
typedef struct ContextStack ContextStack, *pContextStack; // Defined in context.h

enum ValueType;
typedef struct Expr Expr;

enum GCFlags;
typedef struct GCPointer GCPointer, *pGCPointer;
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
        pLongNum val_int;
        double val_real;
        pString val_str;
        pFunction val_func;
        pGCPointer val_ptr;
    };
};

enum GCFlags
{
    GC_NONE = 0,
    GC_USED = 1 << 0,
    GC_REFERENCED = 1 << 1,
    GC_PROTECTED = 1 << 2
};

struct GCPointer
{
    enum GCFlags flags;
    Expr value;
};

struct Heap
{
    size_t size;
    pGCPointer values;
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

#endif // EXEC_TYPES_H_INCLUDED
