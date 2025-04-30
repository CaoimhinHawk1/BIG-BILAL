%{
#include "y.tab.h"
%}

%%
[0-9]+        { return NUMBER; }
[+\-*/]       { return *yytext; }
[ \t\n]       { /* ignore whitespace */ }
.             { return yytext[0]; }
%%