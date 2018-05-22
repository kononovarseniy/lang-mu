%code requires {
#include "stree.h"
#include "stdlib/longnum_stub.h"
#include "stdlib/str.h"
}

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stree.h"
#include "strutils.h"

// Prototype of yylex implemented in auto-generated "mu.scanner.c"
int yylex(void);

int yyerror(char *s);

pSTree parsing_result;

pSTree decorate(char *atom, pSTree item);
pSTree parser_create_atom(char *atom);

%}

%union
{
    char *name;
    char char_val;
    pLongNum int_val;
    double real_val;
    pString str_val;
    pSTree tree;
}

%start  input

%token              OPEN_PAREN
%token              CLOSE_PAREN
%token              OPEN_BRACKET
%token              CLOSE_BRACKET
%token              OPEN_BRACE
%token              CLOSE_BRACE

%token              QUOTE
%token              BACKQUOTE
%token              COMMA
%token              COMMA_ATSIGN

%token  <name>      NAME
%token  <char_val>  CHAR_LITERAL
%token  <int_val>   INT_LITERAL
%token  <real_val>  REAL_LITERAL
%token              RAT_LITERAL
%token  <str_val>   STR_LITERAL

%type   <tree>      items
%type   <tree>      list
%type   <tree>      block
%type   <tree>      item
%type   <tree>      quoted
%type   <tree>      literal

%%

input:  items	{ printf("PARSER: success!!!\n"); parsing_result = $1; }
        ;

list:   OPEN_PAREN items CLOSE_PAREN    {
            $$ = $items;
        }
        ;
block:  OPEN_BRACE items CLOSE_BRACE    {
            $$ = $items;
        }
        ;

item:     NAME { $$ = parser_create_atom($1); free($1); }
        | COMMA         { $$ = parser_create_atom("#:comma"); }
        | COMMA_ATSIGN  { $$ = parser_create_atom("#:comma_atsign"); }
        | list {
            $$ = create_stree();
            $$->type = NODE_LIST;
            $$->child = $list;
        }
        | block {
            $$ = create_stree();
            $$->type = NODE_LIST;
            $$->child = parser_create_atom("block");
            stree_append_child($$, $block);
        }
        | quoted
        | literal
        ;

quoted:   QUOTE item          { $$ = decorate("quote", $item); }
        | BACKQUOTE item    { $$ = decorate("backquote", $item); }
        ;
literal:  CHAR_LITERAL {
            $$ = create_stree();
            $$->type = NODE_CHAR;
            $$->val_char = $1;
        }
        | INT_LITERAL {
            $$ = create_stree();
            $$->type = NODE_INT;
            $$->val_int = $1;
        }
        | REAL_LITERAL {
            $$ = create_stree();
            $$->type = NODE_REAL;
            $$->val_real = $1;
        }
        | STR_LITERAL {
            $$ = create_stree();
            $$->type = NODE_STR;
            $$->val_str = $1;
        }
        ;

items:  { $$ = NULL; }
        | item items {
            stree_append($1, $2);
            $$ = $1;
        }
        ;

%%

pSTree decorate(char *atom, pSTree item)
{
    pSTree res = create_stree();
    res->type = NODE_LIST;
    res->child = create_stree();
    res->child->type = NODE_NAME;
    res->child->name = copystr(atom);
    res->child->next = item;
    return res;
}

pSTree parser_create_atom(char *atom)
{
    pSTree res = create_stree();
    res->type = NODE_NAME;
    res->name = copystr(atom);
    return res;
}

int yyerror(char *s)
{
    extern int yylineno;	// defined and maintained in lex.c
    extern char *yytext;	// defined and maintained in lex.c

    printf("ERROR: %s at symbol \"%s\" on line %d\n", s, yytext, yylineno);
    exit(1);
}
