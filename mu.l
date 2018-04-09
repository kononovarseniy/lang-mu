%{
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "mu.parser.h"
#include "strutils.h"

int yyerror(char *s);

int hextodec(char ch);

char *unescape_string(char *str, int len);

void scanner_error(const char *format, ...);
%}

%option noyywrap

quote           "'"
backquote       "`"
comma           ","
comma_atsign    ",@"

digit       [0-9]
hex_digit   [0-9a-f]
oct_digit   [0-7]
bin_digit   [0-1]

    // Symbols / * , have spacial meaning use it carefully
    // This symbols do not apear in named categories

letter  [a-zA-Z]
other   [_.!?:;~@#$%^&+=|<>\\\-]

letdig  {letter}|{digit}
letoth  {letter}|{other}
all     {letter}|{other}|{digit}

noncomment  "/"{all}

name_start  {letoth}|{noncomment}|"*"
name_char   {all}|{noncomment}|"*"
name        {name_start}{name_char}*

dec_int     [+-]?{digit}+
hex_int     (?i:"0x"{hex_digit}+)
oct_int     (?i:"0o"{oct_digit}+)
bin_int     (?i:"0b"{bin_digit}+)

str_char    ([^"\n\\]|"\\"[^\n])
string      "\""{str_char}*"\""

char        "\'"{str_char}"\'"

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

{dec_int}   { yylval.int_val = strtol(yytext, NULL, 10); return INT_LITERAL; }
{hex_int}   { yylval.int_val = strtol(yytext + 2, NULL, 16); return INT_LITERAL; }
{oct_int}   { yylval.int_val = strtol(yytext + 2, NULL, 8); return INT_LITERAL; }
{bin_int}   { yylval.int_val = strtol(yytext + 2, NULL, 2); return INT_LITERAL; }

{char}      {
        char *str = unescape_string(yytext + 1, yyleng - 2);
        yylval.char_val = str[0];
        free(str);
        return CHAR_LITERAL;
    }
    
{quote}         { return QUOTE; }
{backquote}     { return BACKQUOTE; }
{comma}         { return COMMA; }
{comma_atsign}  { return COMMA_ATSIGN; }

{string}    { yylval.str_val = unescape_string(yytext + 1, yyleng - 2); return STR_LITERAL; }

{name}  {
        // create copy of yytext
        char *name = malloc((yyleng + 1) * sizeof(char));
        if (name == NULL)
        {
            perror("SCANNER name: malloc failed");
            exit(1);
        }
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
                if (name == NULL)
                {
                    perror("SCANNER name: realloc failed");
                    exit(1);
                }
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

.           { scanner_error("Unexpected character"); exit(1); }

%%

void scanner_error(const char *format, ...)
{
    char buffer[256];
    
    va_list args;
    va_start(args, format);

    vsprintf(buffer, format, args);

    printf("SCANNER ");
    yyerror(buffer);
    
    va_end(args);
}
