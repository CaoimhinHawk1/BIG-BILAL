%{
#include "y.tab.h"
#include <stdlib.h>
%}

%%
[0-9]+(\.[0-9]+)?      { yylval.dval = atof(yytext); return NUMBER; }
[ \t]                  { /* ignore whitespace */ }
\n                     { return EOL; }
"+"                    { return PLUS; }
"-"                    { return MINUS; }
"*"                    { return TIMES; }
"/"                    { return DIVIDE; }
"("                    { return LPAREN; }
")"                    { return RPAREN; }
.                      { return yytext[0]; }
%%

int yywrap() {
    return 1;
}