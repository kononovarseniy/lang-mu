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


digit       [0-9]
hex_digit   [0-9a-f]
oct_digit   [0-7]
bin_digit   [0-1]

letter  [a-zA-Z]
other   [_,.!?:;~@#$%^&+=|<>\\\-]

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

char        "'"{str_char}"'"

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

int hextodec(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

char *unescape_string(char *str, int len)
{
    char *res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
    {
        perror("SCANNER unescape_string: malloc failed");
        exit(1);
    }
    int pos = 0;
    int i = 0 ;
    while (i < len)
    {
        char c = str[i++];
        if (c != '\\')
            res[pos++] = c;
        else
        {
            c = str[i++];
            switch (tolower(c))
            {
            case '\'': res[pos++] = '\''; break;
            case '\"': res[pos++] = '\"'; break;
            case '\\': res[pos++] = '\\'; break;
            case '0': res[pos++] = '\0'; break;
            case 'a': res[pos++] = '\a'; break;
            case 'b': res[pos++] = '\b'; break;
            case 'f': res[pos++] = '\f'; break;
            case 'n': res[pos++] = '\n'; break;
            case 'r': res[pos++] = '\r'; break;
            case 't': res[pos++] = '\t'; break;
            case 'v': res[pos++] = '\v'; break;
            case 'x': 
                {
                    char d1, d2;
                    if (len - i < 2 ||
                        (d1 = hextodec(str[i++])) == -1 ||
                        (d2 = hextodec(str[i++])) == -1)
                    {
                        scanner_error("Unrecognized escape sequence \\xnn");
                    }
                    res[pos++] = d1 * 16 + d2;
                }
                break;
            default:
                scanner_error("Unrecognized escape sequence \\%c", c);
                break;
            }
        }
    }
    res[pos] = '\0';
    return res;
}

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
