#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TERMINALS 12
#define MAX_STACK_SIZE 100
#define MAX_INPUT_SIZE 100

// Precedence relations
typedef enum {
    LESS_THAN,        // <
    GREATER_THAN,     // >
    EQUAL,            // =
    NONE              // No relation
} Precedence;

// Terminal symbols
char terminals[MAX_TERMINALS];
int terminal_count = 0;

// Precedence table
Precedence precedence_table[MAX_TERMINALS][MAX_TERMINALS];

// Stack for parsing
typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} Stack;

Stack stack;

// Function prototypes
void initialize_terminals();
void initialize_precedence_table();
int get_terminal_index(char c);
Precedence get_precedence(char a, char b);
void stack_init();
void stack_push(char c);
char stack_pop();
char stack_peek();
bool stack_is_empty();
bool parse_input(const char* input);
void print_precedence_table();
char* preprocess_input(const char* input);

int main() {
    initialize_terminals();
    initialize_precedence_table();
    print_precedence_table();
    
    while (1) {
        char input[MAX_INPUT_SIZE];
        printf("\nEnter expression (e.g., id+id*id): ");
        scanf("%s", input);
        
        // Check for exit condition
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }
        
        // Preprocess input to standardize it
        char* processed_input = preprocess_input(input);
        printf("\nProcessed input: %s\n", processed_input);
        
        if (parse_input(processed_input)) {
            printf("Input accepted!\n");
        } else {
            printf("Input rejected!\n");
        }
        
        free(processed_input);
    }
    
    return 0;
}

// Initialize terminal symbols
void initialize_terminals() {
    // For expression grammar with +, -, *, /, (, ), id
    terminals[0] = '+';
    terminals[1] = '-';
    terminals[2] = '*';
    terminals[3] = '/';
    terminals[4] = '(';
    terminals[5] = ')';
    terminals[6] = 'i';  // Using 'i' to represent identifier
    terminals[7] = '$';  // End marker
    terminal_count = 8;
}

// Initialize precedence table based on grammar rules
void initialize_precedence_table() {
    // Initialize all relations to NONE
    for (int i = 0; i < terminal_count; i++) {
        for (int j = 0; j < terminal_count; j++) {
            precedence_table[i][j] = NONE;
        }
    }
    
    // For a grammar with rules:
    // E -> E+T | E-T | T
    // T -> T*F | T/F | F
    // F -> (E) | id
    
    // Set precedence relations
    // Get indices for each terminal
    int plus_idx = get_terminal_index('+');
    int minus_idx = get_terminal_index('-');
    int mult_idx = get_terminal_index('*');
    int div_idx = get_terminal_index('/');
    int lparen_idx = get_terminal_index('(');
    int rparen_idx = get_terminal_index(')');
    int id_idx = get_terminal_index('i');
    int end_idx = get_terminal_index('$');
    
    // id relations - id has higher precedence than operators
    precedence_table[id_idx][plus_idx] = GREATER_THAN;
    precedence_table[id_idx][minus_idx] = GREATER_THAN;
    precedence_table[id_idx][mult_idx] = GREATER_THAN;
    precedence_table[id_idx][div_idx] = GREATER_THAN;
    precedence_table[id_idx][rparen_idx] = GREATER_THAN;
    precedence_table[id_idx][end_idx] = GREATER_THAN;
    
    // + relations
    precedence_table[plus_idx][id_idx] = LESS_THAN;
    precedence_table[plus_idx][lparen_idx] = LESS_THAN;
    precedence_table[plus_idx][mult_idx] = LESS_THAN;
    precedence_table[plus_idx][div_idx] = LESS_THAN;
    precedence_table[plus_idx][minus_idx] = GREATER_THAN;
    precedence_table[plus_idx][plus_idx] = GREATER_THAN;
    precedence_table[plus_idx][rparen_idx] = GREATER_THAN;
    precedence_table[plus_idx][end_idx] = GREATER_THAN;
    
    // - relations (same as +)
    precedence_table[minus_idx][id_idx] = LESS_THAN;
    precedence_table[minus_idx][lparen_idx] = LESS_THAN;
    precedence_table[minus_idx][mult_idx] = LESS_THAN;
    precedence_table[minus_idx][div_idx] = LESS_THAN;
    precedence_table[minus_idx][minus_idx] = GREATER_THAN;
    precedence_table[minus_idx][plus_idx] = GREATER_THAN;
    precedence_table[minus_idx][rparen_idx] = GREATER_THAN;
    precedence_table[minus_idx][end_idx] = GREATER_THAN;
    
    // * relations
    precedence_table[mult_idx][id_idx] = LESS_THAN;
    precedence_table[mult_idx][lparen_idx] = LESS_THAN;
    precedence_table[mult_idx][plus_idx] = GREATER_THAN;
    precedence_table[mult_idx][minus_idx] = GREATER_THAN;
    precedence_table[mult_idx][mult_idx] = GREATER_THAN;
    precedence_table[mult_idx][div_idx] = GREATER_THAN;
    precedence_table[mult_idx][rparen_idx] = GREATER_THAN;
    precedence_table[mult_idx][end_idx] = GREATER_THAN;
    
    // / relations (same as *)
    precedence_table[div_idx][id_idx] = LESS_THAN;
    precedence_table[div_idx][lparen_idx] = LESS_THAN;
    precedence_table[div_idx][plus_idx] = GREATER_THAN;
    precedence_table[div_idx][minus_idx] = GREATER_THAN;
    precedence_table[div_idx][mult_idx] = GREATER_THAN;
    precedence_table[div_idx][div_idx] = GREATER_THAN;
    precedence_table[div_idx][rparen_idx] = GREATER_THAN;
    precedence_table[div_idx][end_idx] = GREATER_THAN;
    
    // ( relations
    precedence_table[lparen_idx][id_idx] = LESS_THAN;
    precedence_table[lparen_idx][lparen_idx] = LESS_THAN;
    precedence_table[lparen_idx][plus_idx] = LESS_THAN;
    precedence_table[lparen_idx][minus_idx] = LESS_THAN;
    precedence_table[lparen_idx][mult_idx] = LESS_THAN;
    precedence_table[lparen_idx][div_idx] = LESS_THAN;
    precedence_table[lparen_idx][rparen_idx] = EQUAL;
    
    // ) relations
    precedence_table[rparen_idx][plus_idx] = GREATER_THAN;
    precedence_table[rparen_idx][minus_idx] = GREATER_THAN;
    precedence_table[rparen_idx][mult_idx] = GREATER_THAN;
    precedence_table[rparen_idx][div_idx] = GREATER_THAN;
    precedence_table[rparen_idx][rparen_idx] = GREATER_THAN;
    precedence_table[rparen_idx][end_idx] = GREATER_THAN;
    
    // $ relations
    precedence_table[end_idx][id_idx] = LESS_THAN;
    precedence_table[end_idx][lparen_idx] = LESS_THAN;
    precedence_table[end_idx][plus_idx] = LESS_THAN;
    precedence_table[end_idx][minus_idx] = LESS_THAN;
    precedence_table[end_idx][mult_idx] = LESS_THAN;
    precedence_table[end_idx][div_idx] = LESS_THAN;
}

// Get index of terminal in the terminals array
int get_terminal_index(char c) {
    for (int i = 0; i < terminal_count; i++) {
        if (terminals[i] == c) {
            return i;
        }
    }
    return -1;  // Terminal not found
}

// Get precedence relation between two terminals
Precedence get_precedence(char a, char b) {
    int a_idx = get_terminal_index(a);
    int b_idx = get_terminal_index(b);
    
    if (a_idx == -1 || b_idx == -1) {
        return NONE;
    }
    
    return precedence_table[a_idx][b_idx];
}

// Initialize the stack
void stack_init() {
    stack.top = -1;
}

// Push an element onto the stack
void stack_push(char c) {
    if (stack.top == MAX_STACK_SIZE - 1) {
        printf("Stack overflow!\n");
        exit(1);
    }
    stack.data[++stack.top] = c;
}

// Pop an element from the stack
char stack_pop() {
    if (stack.top == -1) {
        printf("Stack underflow!\n");
        exit(1);
    }
    return stack.data[stack.top--];
}

// Peek at the top element of the stack
char stack_peek() {
    if (stack.top == -1) {
        printf("Stack is empty!\n");
        exit(1);
    }
    return stack.data[stack.top];
}

// Check if the stack is empty
bool stack_is_empty() {
    return stack.top == -1;
}

// Print the precedence table
void print_precedence_table() {
    printf("Precedence Table:\n");
    printf("   | ");
    for (int i = 0; i < terminal_count; i++) {
        printf("%c  ", terminals[i]);
    }
    printf("\n---+");
    for (int i = 0; i < terminal_count; i++) {
        printf("---");
    }
    printf("\n");
    
    for (int i = 0; i < terminal_count; i++) {
        printf(" %c | ", terminals[i]);
        for (int j = 0; j < terminal_count; j++) {
            switch (precedence_table[i][j]) {
                case LESS_THAN:
                    printf("<  ");
                    break;
                case GREATER_THAN:
                    printf(">  ");
                    break;
                case EQUAL:
                    printf("=  ");
                    break;
                case NONE:
                    printf("   ");
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Preprocess input to convert operands to 'i' and ensure end marker
// This function replaces all identifiers and numbers with 'i' and adds a '$' at the end.
// It also handles the case where identifiers may contain digits or letters.
// The function returns a dynamically allocated string that should be freed by the caller.
char* preprocess_input(const char* input) {
    // Allocate space for processed input
    char* processed = (char*)malloc(strlen(input) * 2 + 2); // Extra space for $ and potential i replacements
    int j = 0;
    
    // Process each character
    for (int i = 0; input[i]; i++) {
        // Handle identifiers, variables and numbers
        if (isalnum(input[i]) && 
            input[i] != '+' && input[i] != '-' && 
            input[i] != '*' && input[i] != '/' && 
            input[i] != '(' && input[i] != ')' && 
            input[i] != '$') {
            
            processed[j++] = 'i'; // Replace all variables/numbers with 'i'
            
            // Skip the rest of this identifier/number
            while (isalnum(input[i+1]) || input[i+1] == '.') {
                i++;
            }
        } else {
            processed[j++] = input[i];
        }
    }
    
    // Add end marker if not already present
    if (j == 0 || processed[j-1] != '$') {
        processed[j++] = '$';
    }
    
    processed[j] = '\0';
    return processed;
}

// Parse input using operator precedence parsing
// This function uses a stack to parse the input string based on operator precedence rules.
// It prints the parsing steps and returns true if the input is accepted, false otherwise.
// The function assumes the input is preprocessed to replace identifiers with 'i' and ends with '$'.
bool parse_input(const char* input) {
    stack_init();
    stack_push('$');  // Bottom of stack marker
    
    char input_buffer[MAX_INPUT_SIZE + 2];
    strcpy(input_buffer, input);
    
    // Preprocess input: replace 'id' with 'i'
    char processed_input[MAX_INPUT_SIZE * 2];
    int j = 0;
    
    for (int i = 0; input_buffer[i]; i++) {
        if (input_buffer[i] == 'i' && input_buffer[i+1] == 'd') {
            processed_input[j++] = 'i';
            i++;  // Skip 'd'
        } else {
            processed_input[j++] = input_buffer[i];
        }
    }
    processed_input[j] = '\0';
    
    // Add end marker
    strcat(processed_input, "$");
    
    printf("\nProcessed input: %s\n", processed_input);
    printf("\nParsing Steps:\n");
    printf("Stack\t\tInput\t\tAction\n");
    printf("----------------------------------------\n");
    
    int input_pos = 0;
    char current_input = processed_input[input_pos];
    
    while (true) {
        // Print current state
        printf("%-10s\t%-10s\t", stack.data, &processed_input[input_pos]);
        
        char stack_top = stack_peek();
        
        // Get precedence relation
        Precedence relation = get_precedence(stack_top, current_input);
        
        if (relation == LESS_THAN || relation == EQUAL) {
            // Shift: Push current input onto stack
            stack_push(current_input);
            input_pos++;
            current_input = processed_input[input_pos];
            printf("Shift: %c\n", stack.data[stack.top]);
        } else if (relation == GREATER_THAN) {
            // Reduce: Pop symbols from stack
            char handle[MAX_STACK_SIZE];
            int handle_idx = 0;
            
            // Pop the top element
            char popped = stack_pop();
            handle[handle_idx++] = popped;
            
            // Keep popping until we find a precedence relation that is < or =
            while (true) {
                // Check if stack is empty (shouldn't happen in valid parsing)
                if (stack_is_empty()) {
                    printf("Error: Stack underflow during reduction\n");
                    return false;
                }
                
                stack_top = stack_peek();
                relation = get_precedence(stack_top, handle[0]);
                
                if (relation != GREATER_THAN) {
                    break;
                }
                
                popped = stack_pop();
                handle[handle_idx++] = popped;
            }
            
            // Reverse the handle for clearer output
            for (int i = 0; i < handle_idx / 2; i++) {
                char temp = handle[i];
                handle[i] = handle[handle_idx - 1 - i];
                handle[handle_idx - 1 - i] = temp;
            }
            handle[handle_idx] = '\0';
            
            printf("Reduce: %s\n", handle);
        } else if (stack_top == '$' && current_input == '$') {
            // Acceptance
            printf("Accept\n");
            return true;
        } else {
            // Error: No relation found
            printf("Error: No precedence relation between %c and %c\n", stack_top, current_input);
            return false;
        }
    }
    
    return false; 
}