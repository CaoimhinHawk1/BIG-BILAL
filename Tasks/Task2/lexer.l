%{
#include <stdio.h>
#include <string.h>

int line_number = 1;
int column_number = 1;

void update_position();
void update_position_token(int length);
%}

/* Regular definitions */
/* The following regular definitions are used to match different tokens in the input. */
/* Each token type is associated with a specific pattern. */
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER      {DIGIT}+(\.{DIGIT}+)?([eE][+-]?{DIGIT}+)?
WHITESPACE  [ \t]+
NEWLINE     \n
STRING      \"([^\"\n]|\\.)*\"
COMMENT     \/\/.*\n

%%

{WHITESPACE}    { update_position(); /* Ignore whitespace */ }
{NEWLINE}       { line_number++; column_number = 1; /* Handle newlines */ }

"if"        { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"else"      { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"while"     { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"for"       { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"int"       { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"float"     { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"char"      { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"void"      { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"return"    { update_position_token(yyleng); printf("KEYWORD\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

{ID}        { update_position_token(yyleng); printf("IDENTIFIER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
{NUMBER}    { update_position_token(yyleng); printf("NUMBER\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

"+"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"-"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"*"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"/"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"="         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"=="        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"!="        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"<"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
">"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"<="        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
">="        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"&&"        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"||"        { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"!"         { update_position_token(yyleng); printf("OPERATOR\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

"("         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
")"         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"{"         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"}"         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"["         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"]"         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
";"         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
","         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }
"."         { update_position_token(yyleng); printf("DELIMITER\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

{STRING}    { update_position_token(yyleng); printf("STRING\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

{COMMENT}   { line_number++; column_number = 1; /* Ignore comments */ }

.           { update_position_token(yyleng); printf("UNKNOWN\t\t%s\t\t%d\t%d\n", yytext, line_number, column_number - yyleng); }

%%
/* The above rules define how to handle different tokens. Each rule specifies a pattern and the action to take when that pattern is matched. 
   The action typically involves printing the token type, lexeme, line number, and column number. */

/* The following functions are used to update the line and column numbers. 
   They are called whenever a token is matched or whitespace is encountered. */
void update_position() {
    column_number += yyleng;
}

void update_position_token(int length) {
    column_number += length;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    yyin = file;
    
    printf("TOKEN\t\tLEXEME\t\tLINE\tCOLUMN\n");
    printf("----------------------------------------------\n");
    
    yylex();
    
    fclose(file);
    return 0;
}

int yywrap() {
    return 1;
}