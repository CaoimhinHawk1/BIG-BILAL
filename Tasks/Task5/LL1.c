#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NON_TERMINALS 26
#define MAX_TERMINALS 26
#define MAX_RULES 100
#define MAX_RULE_LENGTH 50
#define MAX_STACK_SIZE 100
#define MAX_INPUT_SIZE 100

// Structure to represent a grammar rule
typedef struct {
    char lhs; // Left-hand side (non-terminal)
    char rhs[MAX_RULE_LENGTH]; // Right-hand side
} Rule;

// Grammar structure
typedef struct {
    Rule rules[MAX_RULES];
    int rule_count;
    bool first_sets[MAX_NON_TERMINALS][MAX_TERMINALS + 1]; // +1 for epsilon
    bool follow_sets[MAX_NON_TERMINALS][MAX_TERMINALS + 1]; // +1 for $
    int parse_table[MAX_NON_TERMINALS][MAX_TERMINALS + 1]; // -1 for error, otherwise rule index
} Grammar;

// Stack for LL(1) parsing
typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} Stack;

Grammar grammar;
Stack stack;

// Constants for special characters
const char EPSILON = '$';
const char END_MARKER = '#';

// Function prototypes
void read_grammar(const char* filename);
void compute_first_sets();
bool compute_first_of_string(char* str, bool* result);
void compute_follow_sets();
void construct_parse_table();
bool parse_input(const char* input);

// Stack operations
void stack_init();
void stack_push(char c);
char stack_pop();
char stack_peek();
bool stack_is_empty();

int non_terminal_to_index(char c);
int terminal_to_index(char c);
bool is_non_terminal(char c);
bool is_terminal(char c);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <grammar_file>\n", argv[0]);
        return 1;
    }
    
    read_grammar(argv[1]);
    compute_first_sets();
    compute_follow_sets();
    construct_parse_table();
    
    char input[MAX_INPUT_SIZE];
    printf("Enter input string (end with %c): ", END_MARKER);
    scanf("%s", input);
    
    if (parse_input(input)) {
        printf("Input accepted!\n");
    } else {
        printf("Input rejected!\n");
    }
    
    return 0;
}

// Convert a non-terminal to its index
int non_terminal_to_index(char c) {
    return c - 'A';
}

// Convert a terminal to its index
int terminal_to_index(char c) {
    if (c == EPSILON) return MAX_TERMINALS; // Epsilon is the last index
    if (c == END_MARKER) return MAX_TERMINALS; // End marker is the last index
    return c - 'a';
}

// Check if a character is a non-terminal
bool is_non_terminal(char c) {
    return c >= 'A' && c <= 'Z';
}

// Check if a character is a terminal
bool is_terminal(char c) {
    return (c >= 'a' && c <= 'z') || c == END_MARKER;
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

// Read grammar rules from a file
void read_grammar(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening grammar file");
        exit(1);
    }
    
    char line[100];
    grammar.rule_count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        // Skip empty lines
        if (strlen(line) == 0) continue;
        
        // Parse the rule
        Rule rule;
        rule.lhs = line[0];
        
        // Skip the "->" part
        char* rhs = strstr(line, "->");
        if (!rhs) {
            printf("Error: Invalid rule format: %s\n", line);
            continue;
        }
        
        // Move past the arrow
        rhs += 2;
        
        // Copy right-hand side
        strcpy(rule.rhs, rhs);
        
        // Add the rule to grammar
        grammar.rules[grammar.rule_count++] = rule;
    }
    
    fclose(file);
    
    // Initialize all sets to false
    memset(grammar.first_sets, 0, sizeof(grammar.first_sets));
    memset(grammar.follow_sets, 0, sizeof(grammar.follow_sets));
    
    // Initialize parse table with -1 (error)
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        for (int j = 0; j <= MAX_TERMINALS; j++) {
            grammar.parse_table[i][j] = -1;
        }
    }
    
    // Set END_MARKER in follow set of start symbol
    grammar.follow_sets[non_terminal_to_index(grammar.rules[0].lhs)][terminal_to_index(END_MARKER)] = true;
}

// Compute the FIRST set for all non-terminals
void compute_first_sets() {
    bool changed;
    do {
        changed = false;
        
        // For each rule
        for (int i = 0; i < grammar.rule_count; i++) {
            char lhs = grammar.rules[i].lhs;
            char* rhs = grammar.rules[i].rhs;
            int lhs_idx = non_terminal_to_index(lhs);
            
            if (strlen(rhs) == 0 || strcmp(rhs, "$") == 0) {
                // Empty production (epsilon)
                if (!grammar.first_sets[lhs_idx][terminal_to_index(EPSILON)]) {
                    grammar.first_sets[lhs_idx][terminal_to_index(EPSILON)] = true;
                    changed = true;
                }
                continue;
            }
            
            if (is_terminal(rhs[0])) {
                // If first symbol is terminal, add to FIRST set
                int term_idx = terminal_to_index(rhs[0]);
                if (!grammar.first_sets[lhs_idx][term_idx]) {
                    grammar.first_sets[lhs_idx][term_idx] = true;
                    changed = true;
                }
            } else if (is_non_terminal(rhs[0])) {
                // If first symbol is non-terminal, add its FIRST set to the current non-terminal
                bool eps_in_first = true;
                
                for (int j = 0; rhs[j] && eps_in_first; j++) {
                    if (!is_non_terminal(rhs[j])) {
                        // If terminal, add to FIRST and break
                        if (is_terminal(rhs[j])) {
                            int term_idx = terminal_to_index(rhs[j]);
                            if (!grammar.first_sets[lhs_idx][term_idx]) {
                                grammar.first_sets[lhs_idx][term_idx] = true;
                                changed = true;
                            }
                        }
                        eps_in_first = false;
                        break;
                    }
                    
                    int nt_idx = non_terminal_to_index(rhs[j]);
                    eps_in_first = false;
                    
                    // Add all terminals from FIRST(X) to FIRST(lhs)
                    for (int k = 0; k < MAX_TERMINALS; k++) {
                        if (grammar.first_sets[nt_idx][k] && !grammar.first_sets[lhs_idx][k]) {
                            grammar.first_sets[lhs_idx][k] = true;
                            changed = true;
                        }
                    }
                    
                    // Check for epsilon in FIRST(X)
                    if (grammar.first_sets[nt_idx][terminal_to_index(EPSILON)]) {
                        eps_in_first = true;
                    } else {
                        break;
                    }
                }
                
                // If all symbols in RHS can derive epsilon, add epsilon to FIRST(lhs)
                if (eps_in_first && !grammar.first_sets[lhs_idx][terminal_to_index(EPSILON)]) {
                    grammar.first_sets[lhs_idx][terminal_to_index(EPSILON)] = true;
                    changed = true;
                }
            }
        }
    } while (changed);
}

// Compute the FIRST set of a string
bool compute_first_of_string(char* str, bool* result) {
    bool contains_epsilon = true;
    
    // Initialize result to all false
    memset(result, 0, (MAX_TERMINALS + 1) * sizeof(bool));
    
    for (int i = 0; str[i]; i++) {
        if (!contains_epsilon) break;
        
        contains_epsilon = false;
        
        if (is_terminal(str[i])) {
            result[terminal_to_index(str[i])] = true;
            break;
        } else if (is_non_terminal(str[i])) {
            int nt_idx = non_terminal_to_index(str[i]);
            
            // Add all terminals from FIRST(X) to result
            for (int j = 0; j < MAX_TERMINALS; j++) {
                if (grammar.first_sets[nt_idx][j]) {
                    result[j] = true;
                }
            }
            
            // Check for epsilon
            if (grammar.first_sets[nt_idx][terminal_to_index(EPSILON)]) {
                contains_epsilon = true;
            }
        }
    }
    
    if (contains_epsilon) {
        result[terminal_to_index(EPSILON)] = true;
    }
    
    return contains_epsilon;
}

// Compute the FOLLOW set for all non-terminals
void compute_follow_sets() {
    bool changed;
    do {
        changed = false;
        
        // For each rule
        for (int i = 0; i < grammar.rule_count; i++) {
            char lhs = grammar.rules[i].lhs;
            char* rhs = grammar.rules[i].rhs;
            int lhs_idx = non_terminal_to_index(lhs);
            
            // For each position in the RHS
            for (int j = 0; rhs[j]; j++) {
                if (is_non_terminal(rhs[j])) {
                    int nt_idx = non_terminal_to_index(rhs[j]);
                    
                    // If this is the last symbol, add FOLLOW(lhs) to FOLLOW(rhs[j])
                    if (rhs[j+1] == '\0') {
                        for (int k = 0; k < MAX_TERMINALS + 1; k++) {
                            if (grammar.follow_sets[lhs_idx][k] && !grammar.follow_sets[nt_idx][k]) {
                                grammar.follow_sets[nt_idx][k] = true;
                                changed = true;
                            }
                        }
                    } else {
                        // Compute FIRST of the string after rhs[j]
                        bool first_result[MAX_TERMINALS + 1];
                        bool has_epsilon = compute_first_of_string(&rhs[j+1], first_result);
                        
                        // Add all terminals from FIRST(beta) to FOLLOW(rhs[j])
                        for (int k = 0; k < MAX_TERMINALS; k++) {
                            if (first_result[k] && !grammar.follow_sets[nt_idx][k]) {
                                grammar.follow_sets[nt_idx][k] = true;
                                changed = true;
                            }
                        }
                        
                        // If epsilon is in FIRST(beta), add FOLLOW(lhs) to FOLLOW(rhs[j])
                        if (has_epsilon) {
                            for (int k = 0; k < MAX_TERMINALS + 1; k++) {
                                if (grammar.follow_sets[lhs_idx][k] && !grammar.follow_sets[nt_idx][k]) {
                                    grammar.follow_sets[nt_idx][k] = true;
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}

// Construct LL(1) parsing table
void construct_parse_table() {
    // For each production rule
    for (int i = 0; i < grammar.rule_count; i++) {
        char lhs = grammar.rules[i].lhs;
        char* rhs = grammar.rules[i].rhs;
        int lhs_idx = non_terminal_to_index(lhs);
        
        // If rhs is epsilon
        if (strlen(rhs) == 0 || strcmp(rhs, "$") == 0) {
            // For each terminal in FOLLOW(lhs), add this rule
            for (int j = 0; j <= MAX_TERMINALS; j++) {
                if (grammar.follow_sets[lhs_idx][j]) {
                    if (grammar.parse_table[lhs_idx][j] != -1) {
                        printf("Warning: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, j + 'a');
                    }
                    grammar.parse_table[lhs_idx][j] = i;
                }
            }
        } else {
            // Compute FIRST(rhs)
            bool first_result[MAX_TERMINALS + 1];
            bool has_epsilon = compute_first_of_string(rhs, first_result);
            
            // For each terminal in FIRST(rhs), add this rule
            for (int j = 0; j < MAX_TERMINALS; j++) {
                if (first_result[j]) {
                    if (grammar.parse_table[lhs_idx][j] != -1) {
                        printf("Warning: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, j + 'a');
                    }
                    grammar.parse_table[lhs_idx][j] = i;
                }
            }
            
            // If epsilon is in FIRST(rhs), add this rule for each terminal in FOLLOW(lhs)
            if (has_epsilon) {
                for (int j = 0; j <= MAX_TERMINALS; j++) {
                    if (grammar.follow_sets[lhs_idx][j]) {
                        if (grammar.parse_table[lhs_idx][j] != -1) {
                            printf("Warning: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, j + 'a');
                        }
                        grammar.parse_table[lhs_idx][j] = i;
                    }
                }
            }
        }
    }
}

// Parse input string using LL(1) parsing table
bool parse_input(const char* input) {
    stack_init();
    
    // Push end marker and start symbol onto stack
    stack_push(END_MARKER);
    stack_push(grammar.rules[0].lhs);  // Start symbol
    
    // Add end marker to input if not already present
    char full_input[MAX_INPUT_SIZE + 2];
    strcpy(full_input, input);
    
    int input_len = strlen(full_input);
    if (full_input[input_len - 1] != END_MARKER) {
        full_input[input_len] = END_MARKER;
        full_input[input_len + 1] = '\0';
    }
    
    int input_pos = 0;
    char current_input = full_input[input_pos];
    
    printf("\nParsing Steps:\n");
    printf("Stack\t\tInput\t\tAction\n");
    printf("----------------------------------------\n");
    
    while (!stack_is_empty()) {
        char top = stack_peek();
        
        // Print current state
        printf("%-10s\t%-10s\t", stack.data, &full_input[input_pos]);
        
        if (top == current_input) {
            // If top matches current input, consume both
            stack_pop();
            input_pos++;
            current_input = full_input[input_pos];
            printf("Match: %c\n", top);
        } else if (is_terminal(top)) {
            // If top is terminal but doesn't match, error
            printf("Error: Expected %c, got %c\n", top, current_input);
            return false;
        } else {
            // Top is non-terminal, consult parsing table
            int nt_idx = non_terminal_to_index(top);
            int term_idx = terminal_to_index(current_input);
            
            int rule_idx = grammar.parse_table[nt_idx][term_idx];
            
            if (rule_idx == -1) {
                // No rule in table, error
                printf("Error: No rule for [%c, %c]\n", top, current_input);
                return false;
            }
            
            // Pop the non-terminal
            stack_pop();
            
            // Push RHS in reverse order
            char* rhs = grammar.rules[rule_idx].rhs;
            int rhs_len = strlen(rhs);
            
            printf("Apply: %c -> %s\n", grammar.rules[rule_idx].lhs, rhs);
            
            // Skip pushing epsilon
            if (!(rhs_len == 1 && rhs[0] == EPSILON)) {
                for (int i = rhs_len - 1; i >= 0; i--) {
                    stack_push(rhs[i]);
                }
            }
        }
    }
    
    // If we've consumed all input, success
    return current_input == END_MARKER;
}