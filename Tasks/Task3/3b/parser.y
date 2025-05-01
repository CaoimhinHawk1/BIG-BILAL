%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int yylex();
void yyerror(const char *s);
extern char *yytext;
%}

%token VARIABLE EOF_TOKEN

%%
input:  /* empty */
      | input line
      ;
// line rules
line:   variable '\n'   { 
          printf("Valid variable identifier: %s\n", yytext); 
        }
      | '\n'            { /* allow empty lines */ }
      | error '\n'      { 
          yyerrok; 
          printf("Invalid variable identifier\n"); 
        }
      | EOF_TOKEN       { return 0; }  /* Handle EOF properly */
      ;

variable: VARIABLE      { }
        ;
%%
// Throws an error if the expression is invalid and exits the program
void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}

int main() {
    printf("Variable Identifier Recognizer\n");
    printf("Enter variable identifiers (must start with a letter followed by letters or digits):\n");
    printf("Press Ctrl+C (EOF) to exit\n");
    int result = yyparse();
    printf("Parsing completed.\n");
    return result;
}