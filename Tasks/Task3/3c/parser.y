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

%token <dval> NUMBER
%token PLUS MINUS TIMES DIVIDE LPAREN RPAREN EOL
%type <dval> expr factor term

%left PLUS MINUS
%left TIMES DIVIDE
%precedence NEG   /* negation--unary minus */

%%
calclist: /* nothing */
        | calclist expr EOL { printf("= %.10g\n", $2); printf("> "); }
        | calclist EOL      { printf("> "); } /* blank line or a comment */
        ;

expr:   factor              { $$ = $1; }
      | expr PLUS factor    { $$ = $1 + $3; }
      | expr MINUS factor   { $$ = $1 - $3; }
      ;

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

term:   NUMBER              { $ = $1; }
      | MINUS term %prec NEG { $ = -$2; }
      | LPAREN expr RPAREN  { $ = $2; }