%{
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

noncomment  "/"{all}

name_start  {letoth}|{noncomment}|"*"
name_char   {all}|{noncomment}|"*"
name        {name_start}{name_char}*

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

{name}  {
        // create copy of yytext
        char *name = malloc((yyleng + 1) * sizeof(char));
        strcpy(name, yytext);
        // if next character is a slash and not a comment start
        // then append it to the name
        char c = input();
        if (c == '/')
        {
            c = input();
            // Second next char cant be part of the name
            // In this case it would have been eaten before
            unput(c);
            if (c == '/' || c == '*')
            {
                unput('/');
            }
            else
            {
                name = realloc(name, (yyleng + 2) * sizeof(char));
                name[yyleng] = '/';
                name[yyleng + 1] = '\0';
            }
        }
        else
        {
            unput(c);
        }
        yylval.name = name;
        return NAME;
    }
"/"  { yylval.name = copystr(yytext); return NAME; }

[ \t\r]*    { }
[\n]        { yylineno++; }

.           { printf("SCANNER "); yyerror("Unexpected character"); exit(1); }

