%{
#include "y.tab.h"
%}

%%
[a-zA-Z][a-zA-Z0-9]*   { return VARIABLE; }
[ \t\n]                { /* ignore whitespace */ }
.                      { return yytext[0]; }
%%

int yywrap() {
    return 1;
}