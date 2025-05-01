%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

extern YYSTYPE yylval;
%}

%%
"int"       { return INT; }
"main"      { return MAIN; }
"return"    { return RETURN; }
[a-zA-Z][a-zA-Z0-9]* { 
                yylval.id = strdup(yytext); 
                return IDENTIFIER; 
              }
[0-9]+      { 
                yylval.num = atoi(yytext); 
                return NUMBER; 
             }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIVIDE; }
"="         { return ASSIGN; }
";"         { return SEMICOLON; }
","         { return COMMA; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
[ \t\n]     { /* Skip whitespace */ }
.           { return yytext[0]; }
%%

int yywrap() {
    return 1;
}