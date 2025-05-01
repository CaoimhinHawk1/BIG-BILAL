#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NON_TERMINALS 26
#define MAX_TERMINALS 26
#define MAX_RULES 100
#define MAX_RULE_LENGTH 50

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
} Grammar;

Grammar grammar;

// Constants for special characters
const char EPSILON = '$';
const char END_MARKER = '#';

// Function prototypes
void read_grammar(const char* filename);
void compute_first_sets();
bool compute_first_of_string(char* str, bool* result);
void compute_follow_sets();
void print_first_sets();
void print_follow_sets();
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
    
    printf("\nFirst Sets:\n");
    print_first_sets();
    
    printf("\nFollow Sets:\n");
    print_follow_sets();
    
    return 0;
}

// Convert a non-terminal to its index
int non_terminal_to_index(char c) {
    return c - 'A';
}

// Convert a terminal to its index
int terminal_to_index(char c) {
    if (c == EPSILON) return MAX_TERMINALS; // Epsilon is the last index
    return c - 'a';
}

// Check if a character is a non-terminal
bool is_non_terminal(char c) {
    return c >= 'A' && c <= 'Z';
}

// Check if a character is a terminal
bool is_terminal(char c) {
    return (c >= 'a' && c <= 'z') || c == EPSILON;
}

// Read grammar rules from a file
// The grammar file should contain rules in the format: A -> aB | cD | ε
// where A is a non-terminal, a, c are terminals, and ε represents epsilon (empty string).
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
    
    // Set END_MARKER in follow set of start symbol
    grammar.follow_sets[non_terminal_to_index(grammar.rules[0].lhs)][terminal_to_index(END_MARKER)] = true;
}

// Compute the FIRST set for all non-terminals
// This function iterates through each rule and checks if the non-terminal can derive a terminal or epsilon.
// It updates the FIRST set accordingly.
// It uses a boolean flag to track if any changes were made in the FIRST sets during the iteration.
void compute_first_sets() {
    bool changed;
    do {
        changed = false;
        
        // For each rule
        for (int i = 0; i < grammar.rule_count; i++) {
            char lhs = grammar.rules[i].lhs;
            char* rhs = grammar.rules[i].rhs;
            int lhs_idx = non_terminal_to_index(lhs);
            
            if (strlen(rhs) == 0) {
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

// This function computes the FIRST set of a string (sequence of symbols)
// It iterates through each symbol in the string and checks if it is a terminal or non-terminal.
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

// This function computes the FOLLOW set for all non-terminals in the grammar
// It iterates through each rule and checks if the non-terminal is followed by another non-terminal or terminal.
// If it is followed by a terminal, it adds that terminal to the FOLLOW set of the non-terminal.
void compute_follow_sets() {
    // Clear all follow sets first
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        for (int j = 0; j <= MAX_TERMINALS; j++) {
            grammar.follow_sets[i][j] = false;
        }
    }
    
    // Set END_MARKER in follow set of start symbol
    int start_idx = non_terminal_to_index(grammar.rules[0].lhs);
    grammar.follow_sets[start_idx][terminal_to_index(END_MARKER)] = true;
    bool changed;
    do {
        changed = false;
        
        // For each rule in the grammar
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

// Print the FIRST sets for all non-terminals
// This function iterates through each non-terminal and checks if it has any elements in its FIRST set.
// If it does, it prints the non-terminal and its FIRST set.
void print_first_sets() {
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        bool non_empty = false;
        
        // Check if this non-terminal has any elements in its FIRST set
        for (int j = 0; j <= MAX_TERMINALS; j++) {
            if (grammar.first_sets[i][j]) {
                non_empty = true;
                break;
            }
        }
        
        if (non_empty) {
            printf("FIRST(%c) = { ", i + 'A');
            
            bool first = true;
            for (int j = 0; j < MAX_TERMINALS; j++) {
                if (grammar.first_sets[i][j]) {
                    if (!first) printf(", ");
                    printf("%c", j + 'a');
                    first = false;
                }
            }
            
            if (grammar.first_sets[i][MAX_TERMINALS]) {
                if (!first) printf(", ");
                printf("ε");
            }
            
            printf(" }\n");
        }
    }
}

// Print the FOLLOW sets
// This function prints the FOLLOW sets for all non-terminals in the grammar.
// It iterates through each non-terminal and checks if it has any elements in its FOLLOW set.
void print_follow_sets() {
    // Create a set of non-terminals that appear in the grammar
    bool non_terminal_exists[MAX_NON_TERMINALS] = {false};
    
    for (int i = 0; i < grammar.rule_count; i++) {
        non_terminal_exists[non_terminal_to_index(grammar.rules[i].lhs)] = true;
    }
    
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        // Skip if this non-terminal doesn't exist in the grammar
        if (!non_terminal_exists[i]) continue;
        
        bool non_empty = false;
        
        // Check if this non-terminal has any elements in its FOLLOW set
        for (int j = 0; j <= MAX_TERMINALS; j++) {
            if (grammar.follow_sets[i][j]) {
                non_empty = true;
                break;
            }
        }
        
        if (non_empty) {
            printf("FOLLOW(%c) = { ", i + 'A');
            
            bool first = true;
            for (int j = 0; j < MAX_TERMINALS; j++) {
                if (grammar.follow_sets[i][j]) {
                    if (!first) printf(", ");
                    printf("%c", j + 'a');
                    first = false;
                }
            }
            
            if (grammar.follow_sets[i][MAX_TERMINALS]) {
                if (!first) printf(", ");
                printf("#");  // End marker
            }
            
            printf(" }\n");
        }
    }
}