%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token NUMBER PLUS MINUS TIMES DIVIDE LPAREN RPAREN

%left PLUS MINUS
%left TIMES DIVIDE
%precedence NEG   /* negation--unary minus */
%right POWER

%%
input:  /* empty */
      | input line
      ;

line:   expr '\n'   { printf("Valid arithmetic expression\n"); }
      | error '\n'  { yyerrok; }
      ;

expr:   NUMBER            { $$ = $1; }
      | expr PLUS expr    { $$ = $1 + $3; }
      | expr MINUS expr   { $$ = $1 - $3; }
      | expr TIMES expr   { $$ = $1 * $3; }
      | expr DIVIDE expr  { $$ = $1 / $3; }
      | MINUS expr %prec NEG { $$ = -$2; }
      | LPAREN expr RPAREN { $$ = $2; }
      ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int main() {
    printf("Enter arithmetic expressions (use operators +, -, *, /):\n");
    printf("Press Ctrl+D (EOF) to exit\n");
    yyparse();
    printf("Parsing completed.\n");
    return 0;
}