%{
#include "y.tab.h"
#include <stdio.h>
%}

%%
[a-zA-Z][a-zA-Z0-9]*   { return VARIABLE; }
\n                      { return '\n'; }
[ \t]                   { /* ignore whitespace */ }
<<EOF>>                 { return EOF_TOKEN; }  /* Return specific token for EOF */
.                       { return yytext[0]; }
%%

int yywrap() {
    return 1;
}