%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token VARIABLE

%%
input:  /* empty */
      | input line
      ;

line:   variable '\n'   { printf("Valid variable identifier\n"); }
      | error '\n'      { yyerrok; }
      ;

variable: VARIABLE      { }
        ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int main() {
    printf("Enter variable identifiers (must start with a letter followed by letters or digits):\n");
    printf("Press Ctrl+D (EOF) to exit\n");
    yyparse();
    printf("Parsing completed.\n");
    return 0;
}