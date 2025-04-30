%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node types for AST
typedef enum {
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_STATEMENT
} NodeType;

// Node structure for AST
typedef struct node {
    NodeType type;
    union {
        int value;              // For numbers
        char *id;               // For identifiers
        struct {                // For binary operations
            struct node *left;
            struct node *right;
            char op;
        } binary_op;
        struct {                // For assignments
            char *id;
            struct node *expr;
        } assign;
        struct {                // For statements
            struct node *stmt;
            struct node *next;
        } stmt;
    };
} Node;

// Function prototypes
Node *create_number_node(int value);
Node *create_identifier_node(char *id);
Node *create_binary_op_node(Node *left, Node *right, char op);
Node *create_assign_node(char *id, Node *expr);
Node *create_statement_node(Node *stmt, Node *next);
void print_ast(Node *node, int indent);

int yylex();
void yyerror(const char *s);
%}

%union {
    int num;
    char *id;
    struct node *ast;
}

%token <num> NUMBER
%token <id> IDENTIFIER
%token PLUS MINUS TIMES DIVIDE ASSIGN SEMICOLON LPAREN RPAREN

%type <ast> expr term factor statement statement_list

%left PLUS MINUS
%left TIMES DIVIDE
%right ASSIGN
%precedence NEG

%%
program: statement_list {
           printf("\nAbstract Syntax Tree:\n");
           print_ast($1, 0);
         }
       ;

statement_list: statement { $$ = $1; }
              | statement_list statement {
                  $$ = create_statement_node($1, $2);
                }
              ;

statement: expr SEMICOLON { $$ = $1; }
         | IDENTIFIER ASSIGN expr SEMICOLON {
             $$ = create_assign_node($1, $3);
           }
         ;

expr: term { $$ = $1; }
    | expr PLUS term {
        $$ = create_binary_op_node($1, $3, '+');
      }
    | expr MINUS term {
        $$ = create_binary_op_node($1, $3, '-');
      }
    ;

term: factor { $$ = $1; }
    | term TIMES factor {
        $$ = create_binary_op_node($1, $3, '*');
      }
    | term DIVIDE factor {
        $$ = create_binary_op_node($1, $3, '/');
      }
    ;

factor: NUMBER {
          $$ = create_number_node($1);
        }
      | IDENTIFIER {
          $$ = create_identifier_node($1);
        }
      | MINUS factor %prec NEG {
          $$ = create_binary_op_node(create_number_node(0), $2, '-');
        }
      | LPAREN expr RPAREN { $$ = $2; }
      ;
%%

// Create a number node
Node *create_number_node(int value) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->type = NODE_NUMBER;
    node->value = value;
    return node;
}

// Create an identifier node
Node *create_identifier_node(char *id) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->type = NODE_IDENTIFIER;
    node->id = id;
    return node;
}

// Create a binary operation node
Node *create_binary_op_node(Node *left, Node *right, char op) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->type = NODE_BINARY_OP;
    node->binary_op.left = left;
    node->binary_op.right = right;
    node->binary_op.op = op;
    return node;
}

// Create an assignment node
Node *create_assign_node(char *id, Node *expr) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->type = NODE_ASSIGN;
    node->assign.id = id;
    node->assign.expr = expr;
    return node;
}

// Create a statement node
Node *create_statement_node(Node *stmt, Node *next) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->type = NODE_STATEMENT;
    node->stmt.stmt = stmt;
    node->stmt.next = next;
    return node;
}

// Print the AST with proper indentation
void print_ast(Node *node, int indent) {
    if (node == NULL) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    // Print node based on its type
    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %d\n", node->value);
            break;
        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->id);
            break;
        case NODE_BINARY_OP:
            printf("BINARY_OP: %c\n", node->binary_op.op);
            print_ast(node->binary_op.left, indent + 1);
            print_ast(node->binary_op.right, indent + 1);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN: %s\n", node->assign.id);
            print_ast(node->assign.expr, indent + 1);
            break;
        case NODE_STATEMENT:
            printf("STATEMENT:\n");
            print_ast(node->stmt.stmt, indent + 1);
            print_ast(node->stmt.next, indent);
            break;
    }
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int main() {
    printf("Enter expressions or assignments (end with semicolons):\n");
    printf("Example: x = 10 + 5; y = x * 2;\n");
    printf("Press Ctrl+D (EOF) to exit\n");
    yyparse();
    return 0;
}