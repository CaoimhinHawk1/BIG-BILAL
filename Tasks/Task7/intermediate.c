#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR_LEN 100
#define MAX_TEMP_VARS 100
#define MAX_QUAD_SIZE 100

// Structure for quadruple
typedef struct {
    char op;         // Operator: +, -, *, /
    char arg1[10];   // Argument 1
    char arg2[10];   // Argument 2
    char result[10]; // Result
} Quadruple;

// Global variables
int temp_count = 0;
Quadruple quads[MAX_QUAD_SIZE];
int quad_count = 0;

// Function prototypes
void generate_three_address_code(const char* expr);
char* generate_code_for_expr(const char* expr, int start, int end);
int find_last_operator(const char* expr, int start, int end);
char* new_temp();
void add_quadruple(char op, const char* arg1, const char* arg2, const char* result);
void print_quadruples();
int is_operand(char c);
int is_operator(char c);
int get_precedence(char op);

int main() {
    char expr[MAX_EXPR_LEN];
    
    printf("Enter an arithmetic expression: ");
    fgets(expr, MAX_EXPR_LEN, stdin);
    
    // Remove newline character
    size_t len = strlen(expr);
    if (len > 0 && expr[len-1] == '\n') {
        expr[len-1] = '\0';
    }
    
    generate_three_address_code(expr);
    print_quadruples();
    
    return 0;
}

// Create a new temporary variable
char* new_temp() {
    static char temp[10];
    sprintf(temp, "t%d", temp_count++);
    return strdup(temp);
}

// Add a quadruple to the list
void add_quadruple(char op, const char* arg1, const char* arg2, const char* result) {
    if (quad_count >= MAX_QUAD_SIZE) {
        printf("Error: Quadruple array full\n");
        return;
    }
    
    quads[quad_count].op = op;
    strcpy(quads[quad_count].arg1, arg1);
    strcpy(quads[quad_count].arg2, arg2);
    strcpy(quads[quad_count].result, result);
    
    quad_count++;
}

// Print all generated quadruples
void print_quadruples() {
    printf("\nGenerated Three-Address Code (Quadruples):\n");
    printf("------------------------------------------\n");
    printf("No.\tOperator\tArg1\tArg2\tResult\n");
    
    for (int i = 0; i < quad_count; i++) {
        printf("%d\t%c\t\t%s\t%s\t%s\n", 
               i + 1, quads[i].op, quads[i].arg1, quads[i].arg2, quads[i].result);
    }
}

// Check if a character is an operand (variable or digit)
int is_operand(char c) {
    return isalnum(c);
}

// Check if a character is an operator
int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Get precedence of an operator
int get_precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

// Find the last operator with lowest precedence
int find_last_operator(const char* expr, int start, int end) {
    int paren_count = 0;
    int min_precedence = 3;  // Higher than any operator precedence
    int min_precedence_pos = -1;
    
    for (int i = end; i >= start; i--) {
        if (expr[i] == ')') {
            paren_count++;
        } else if (expr[i] == '(') {
            paren_count--;
        } else if (paren_count == 0 && is_operator(expr[i])) {
            int precedence = get_precedence(expr[i]);
            
            // For operators with same precedence, choose the leftmost (last in right-to-left scan)
            if (precedence <= min_precedence) {
                min_precedence = precedence;
                min_precedence_pos = i;
            }
        }
    }
    
    return min_precedence_pos;
}

// Generate code for an expression (recursive)
char* generate_code_for_expr(const char* expr, int start, int end) {
    // Skip leading and trailing whitespace
    while (start <= end && isspace(expr[start])) start++;
    while (end >= start && isspace(expr[end])) end--;
    
    if (start > end) {
        return strdup("");  // Empty expression
    }
    
    // Handle parenthesized expressions
    if (expr[start] == '(' && expr[end] == ')') {
        // Check if the parentheses are balanced and match
        int paren_count = 1;
        int i;
        for (i = start + 1; i < end; i++) {
            if (expr[i] == '(') paren_count++;
            if (expr[i] == ')') paren_count--;
            if (paren_count == 0) break;  // Found matching closing parenthesis before end
        }
        
        // If parentheses are balanced and match start/end, remove them
        if (paren_count == 1 && i == end) {
            return generate_code_for_expr(expr, start + 1, end - 1);
        }
    }
    
    // Find the last operator with lowest precedence
    int op_pos = find_last_operator(expr, start, end);
    
    if (op_pos == -1) {
        // No operator found, it's a single operand
        char* operand = (char*)malloc(end - start + 2);
        strncpy(operand, &expr[start], end - start + 1);
        operand[end - start + 1] = '\0';
        return operand;
    }
    
    // Generate code for left and right subexpressions
    char* left = generate_code_for_expr(expr, start, op_pos - 1);
    char* right = generate_code_for_expr(expr, op_pos + 1, end);
    
    // Create a new temporary and add the quadruple
    char* temp = new_temp();
    add_quadruple(expr[op_pos], left, right, temp);
    
    // Clean up
    free(left);
    free(right);
    
    return temp;
}

// Generate three-address code for the whole expression
void generate_three_address_code(const char* expr) {
    // Reset counters
    temp_count = 0;
    quad_count = 0;
    
    // Generate code for the expression
    char* result = generate_code_for_expr(expr, 0, strlen(expr) - 1);
    
    // Clean up
    free(result);
}