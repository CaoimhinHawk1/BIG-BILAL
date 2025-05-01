%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Temporary variables counter
int temp_counter = 0;
int pos_counter = 0;

// Structure to represent a 3-address code instruction
typedef struct {
    char op;        // Operator: +, -, *, /, =
    char arg1[10];  // First argument
    char arg2[10];  // Second argument
    char result[10]; // Result
} ThreeAddressCode;

// Array to store generated code
ThreeAddressCode code[100];
int code_count = 0;

// Function prototypes
char* new_temp();
void add_code(char op, const char* arg1, const char* arg2, const char* result);
void print_code();

int yylex();
void yyerror(const char *s);
%}

%union {
    int num;
    char *id;
    struct {
        char place[10];
        int value;
    } expr;
}

%token <num> NUMBER
%token <id> IDENTIFIER
%token INT MAIN RETURN
%token PLUS MINUS TIMES DIVIDE
%token ASSIGN SEMICOLON COMMA
%token LPAREN RPAREN LBRACE RBRACE

%type <expr> expr term factor

%left PLUS MINUS
%left TIMES DIVIDE
%right ASSIGN

%%
program:
    MAIN LPAREN RPAREN LBRACE declarations statements RBRACE
    {
        print_code();
    }
    ;

declarations:
    /* empty */ 
    | INT declaration_list SEMICOLON declarations
    ;

declaration_list:
    IDENTIFIER
    {
        /* No code for declarations */
    }
    | declaration_list COMMA IDENTIFIER
    {
        /* No code for declarations */
    }
    ;

statements:
    /* empty */
    | statement statements
    ;

statement:
    IDENTIFIER ASSIGN expr SEMICOLON
    {
        add_code('=', $3.place, "", $1);
    }
    ;

expr:
    term
    {
        strcpy($$.place, $1.place);
    }
    | expr PLUS term
    {
        char* temp = new_temp();
        add_code('+', $1.place, $3.place, temp);
        strcpy($$.place, temp);
    }
    | expr MINUS term
    {
        char* temp = new_temp();
        add_code('-', $1.place, $3.place, temp);
        strcpy($$.place, temp);
    }
    ;

term:
    factor
    {
        strcpy($$.place, $1.place);
    }
    | term TIMES factor
    {
        char* temp = new_temp();
        add_code('*', $1.place, $3.place, temp);
        strcpy($$.place, temp);
    }
    | term DIVIDE factor
    {
        char* temp = new_temp();
        add_code('/', $1.place, $3.place, temp);
        strcpy($$.place, temp);
    }
    ;

factor:
    IDENTIFIER
    {
        strcpy($$.place, $1);
    }
    | NUMBER
    {
        sprintf($$.place, "%d", $1);
    }
    | LPAREN expr RPAREN
    {
        strcpy($$.place, $2.place);
    }
    ;

%%

// Create a new temporary variable
char* new_temp() {
    static char temp[10];
    sprintf(temp, "t%d", temp_counter++);
    return temp;
}

// Add a three-address code instruction
void add_code(char op, const char* arg1, const char* arg2, const char* result) {
    if (code_count >= 100) {
        printf("Error: Code array full\n");
        return;
    }
    
    code[code_count].op = op;
    strcpy(code[code_count].arg1, arg1);
    strcpy(code[code_count].arg2, arg2);
    strcpy(code[code_count].result, result);
    
    code_count++;
}

// Print the generated three-address code
void print_code() {
    printf("------------------------------------\n");
    printf("Pos Operator Arg1 Arg2 Result\n");
    printf("------------------------------------\n");
    
    for (int i = 0; i < code_count; i++) {
        // Convert operator to string for better display
        char op_str[5] = "";
        op_str[0] = code[i].op;
        op_str[1] = '\0';
        
        printf("%-4d %-8s %-4s %-4s %-4s\n", 
               i, op_str, 
               code[i].arg1, 
               code[i].arg2, 
               code[i].result);
    }
    
    printf("------------------------------------\n");
}

int main() {
    printf("Enter C code (end with Ctrl+D):\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}