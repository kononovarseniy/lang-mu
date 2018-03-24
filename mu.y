%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strutils.h"

// Prototype of yylex implemented in auto-generated "mu.scanner.c"
int yylex(void);

int yyerror(char *s);

%}

%union
{
    char *name;
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
%token              INT_LITERAL
%token              FLOAT_LITERAL
%token              RAT_LITERAL
%token              CHAR_LITERAL
%token              STRING_LITERAL

%type   <name>      exp
%type   <name>      items

%%

input:  items	{ printf("Result: %s\n", $1); free($1); }
        ;

exp:    NAME    { $$ = $1; }
        | OPEN_PAREN items CLOSE_PAREN	{
            char *s1 = concatstr("(", $items); free($items);
            $$ = concatstr(s1, ")"); free(s1);
        }
        ;
items:  { $$ = copystr(""); }
        | exp items {
            if (strlen($2) != 0)
            {
                char *s1 = concatstr($1, ", "); free($1);
                $$ = concatstr(s1, $2); free(s1); free($2);
            }
            else
            {
                $$ = $1; free($2);
            }
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
