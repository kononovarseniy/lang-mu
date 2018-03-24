%{
#define YY_NO_UNPUT
#define YY_NO_INPUT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mu.parser.h"
#include "strutils.h"

int yyerror(char *s);
%}

%option noyywrap


digit   [0-9]
letter  [a-zA-Z]
other   [_,.!?:;~@#$%^&+=|<>\\\-]

letdig  {letter}|{digit}
letoth  {letter}|{other}
all     {letter}|{other}|{digit}

noncomment  "/"{other}

name_start  {letoth}|("/"{all})|"*"
name_char   {all}|("/"{all})|"*"
name_end    ({all}|[/*])?
name        {name_start}{name_char}*{name_end}

%x slcomment mlcomment

%%

"("     return OPEN_PAREN;
")"     return CLOSE_PAREN;
"["     return OPEN_BRACKET;
"]"     return CLOSE_BRACKET;
"{"     return OPEN_BRACE;
"}"     return CLOSE_BRACE;

"//"                BEGIN(slcomment);
<slcomment>[^\n]*   {}
<slcomment>\n       { yylineno++; BEGIN(INITIAL); }

"/*"                    BEGIN(mlcomment);
<mlcomment>[^*\n]*      {}
<mlcomment>"*"+[^*/\n]  {}
<mlcomment>\n           yylineno++;
<mlcomment>"*"+"/"      BEGIN(INITIAL);

{name}  { yylval.name = copystr(yytext); return NAME; }

[ \t\r]*  {}
[\n]        { yylineno++; }

.           { printf("SCANNER "); yyerror("Unexpected character"); exit(1); }

