%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

extern YYSTYPE yylval;
%}

%%
[0-9]+      { 
              yylval.num = atoi(yytext); 
              return NUMBER; 
            }
[a-zA-Z][a-zA-Z0-9]*  { 
              yylval.id = strdup(yytext);
              return IDENTIFIER; 
            }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIVIDE; }
"="         { return ASSIGN; }
";"         { return SEMICOLON; }
"("         { return LPAREN; }
")"         { return RPAREN; }
[ \t\n]     { /* ignore whitespace */ }
.           { return yytext[0]; }
%%

int yywrap() {
    return 1;
}