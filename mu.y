%code requires {
#include "stree.h"
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

%}

%union
{
    char *name;
    char *str_val;
    char char_val;
    long int_val;
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

%token  <name>      NAME
%token  <int_val>   INT_LITERAL
%token              FLOAT_LITERAL
%token              RAT_LITERAL
%token  <char_val>  CHAR_LITERAL
%token  <str_val>   STR_LITERAL

%type   <tree>      list
%type   <tree>      items
%type   <tree>      item

%%

input:  items	{ printf("PARSER: success!!!\n"); parsing_result = $1; }
        ;

list:   OPEN_PAREN items CLOSE_PAREN    {
            $$ = $items;
        }
        ;

item:   NAME {
            $$ = create_stree();
            $$->type = NODE_NAME;
            $$->name = copystr($1);
        }
        | INT_LITERAL {
            $$ = create_stree();
            $$->type = NODE_INT;
            $$->int_val = $1;
        }
        | STR_LITERAL {
            $$ = create_stree();
            $$->type = NODE_STR;
            $$->str_val = $1;
        }
        | CHAR_LITERAL {
            $$ = create_stree();
            $$->type = NODE_CHAR;
            $$->char_val = $1;
        }
        | list {
            $$ = create_stree();
            $$->type = NODE_LIST;
            $$->child = $list;
        }
        ;

items:  { $$ = NULL; }
        | item items {
            stree_append($1, $2);
            $$ = $1;
        }
        ;

%%

int yyerror(char *s)
{
    extern int yylineno;	// defined and maintained in lex.c
    extern char *yytext;	// defined and maintained in lex.c

    printf("ERROR: %s at symbol \"%s\" on line %d\n", s, yytext, yylineno);
    exit(1);
}
