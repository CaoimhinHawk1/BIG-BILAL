%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
// * Define the data types for the tokens */
%token NUMBER PLUS MINUS TIMES DIVIDE LPAREN RPAREN

// * Define the precedence and associativity of operators */
%left PLUS MINUS
%left TIMES DIVIDE
%precedence NEG   /* negation--unary minus */

%%
input:  /* empty */
      | input line
      ;
// line rules
line:   expr '\n'   { printf("Valid arithmetic expression\n"); }
      | '\n'        { /* allow empty lines */ }
      | error '\n'  { yyerrok; }
      ;
// expression rules
expr:   NUMBER            { $$ = $1; }
      | expr PLUS expr    { $$ = $1 + $3; }
      | expr MINUS expr   { $$ = $1 - $3; }
      | expr TIMES expr   { $$ = $1 * $3; }
      | expr DIVIDE expr  { 
          if ($3 == 0) {
              yyerror("Division by zero");
          } else {
              $$ = $1 / $3;
          }
        }
      | MINUS expr %prec NEG { $$ = -$2; }
      | LPAREN expr RPAREN { $$ = $2; }
      ;
%%

//Throws an error if the expression is invalid and exits the program
void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}

int main() {
    printf("Arithmetic Expression Recognizer\n");
    printf("Enter arithmetic expressions (use operators +, -, *, /):\n");
    printf("Press Ctrl+C (EOF) to exit\n");
    int result = yyparse();
    printf("Parsing completed.\n");
    return result;
}