%{
#include <stdio.h>
%}

%token NUMBER

%%
expr: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | NUMBER
    ;
%%

int main() {
    return yyparse();
}

int yyerror(char *s) {
    printf("Parse error: %s\n", s);
    return 0;
}
