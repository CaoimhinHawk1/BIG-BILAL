%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int yylex();
void yyerror(const char *s);
%}

%union {
    double dval;
}
/* Define the data types for the tokens */
%token <dval> NUMBER
%token PLUS MINUS TIMES DIVIDE LPAREN RPAREN EOL
%type <dval> expr factor term

/* Define the precedence and associativity of operators */
%left PLUS MINUS
%left TIMES DIVIDE
%precedence NEG   /* negation--unary minus */

%%
/* The main rule for the calculator */
calclist: /* nothing */
        | calclist expr EOL { printf("= %.10g\n", $2); printf("> "); }
        | calclist EOL      { printf("> "); } /* blank line or a comment */
        ;
/* The rules for the expressions */
expr:   factor              { $$ = $1; }
      | expr PLUS factor    { $$ = $1 + $3; }
      | expr MINUS factor   { $$ = $1 - $3; }
      ;
//* The rules for the factors and terms */
factor: term                { $$ = $1; }
      | factor TIMES term   { $$ = $1 * $3; }
      | factor DIVIDE term  { 
          if ($3 == 0.0) {
              yyerror("Division by zero");
              $$ = 0.0;
          } else {
              $$ = $1 / $3;
          }
        }
      ;

term:   NUMBER              { $$ = $1; }
      | MINUS term %prec NEG { $$ = -$2; }
      | LPAREN expr RPAREN  { $$ = $2; }
      ;
%%

// Throws an error if the expression is invalid and exits the program
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(1);
}

int main() {
    printf("Calculator\n");
    printf("Enter expressions, exit execution with Ctrl+C \n");
    printf("> ");
    int result = yyparse();
    printf("\nCalculator exiting.\n");
    return result;
}