#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 100
#define MAX_VAR_NAME 20
#define MAX_VARS 100

// Structure to store variable information
typedef struct {
    char name[MAX_VAR_NAME];
    int value;
    bool is_constant;
} Variable;

// Global variable table
Variable var_table[MAX_VARS];
int var_count = 0;

// Function prototypes
void add_variable(const char *name, int value, bool is_constant);
int find_variable(const char *name);
bool is_constant_var(const char *name);
int get_constant_value(const char *name);
bool is_numeric(const char *str);
void process_input(FILE *input, FILE *output);
void handle_assignment(char *line, FILE *output);
void handle_operation(char *line, FILE *output);

int main(int argc, char *argv[]) {
    FILE *input, *output;
    
    // Open input file or use stdin
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Error opening input file");
            return 1;
        }
    } else {
        input = stdin;
    }
    
    // Open output file or use stdout
    if (argc > 2) {
        output = fopen(argv[2], "w");
        if (!output) {
            perror("Error opening output file");
            if (argc > 1) fclose(input);
            return 1;
        }
    } else {
        output = stdout;
    }
    
    // Process the input
    process_input(input, output);
    
    // Clean up
    if (argc > 1) fclose(input);
    if (argc > 2) fclose(output);
    
    return 0;
}

// Add a variable to the table
void add_variable(const char *name, int value, bool is_constant) {
    if (var_count >= MAX_VARS) {
        fprintf(stderr, "Error: Variable table full\n");
        return;
    }
    
    // First check if variable already exists
    int idx = find_variable(name);
    if (idx != -1) {
        var_table[idx].value = value;
        var_table[idx].is_constant = is_constant;
        return;
    }
    
    // Add new variable
    strncpy(var_table[var_count].name, name, MAX_VAR_NAME - 1);
    var_table[var_count].name[MAX_VAR_NAME - 1] = '\0';
    var_table[var_count].value = value;
    var_table[var_count].is_constant = is_constant;
    var_count++;
}

// Find a variable in the table
int find_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

// Check if a variable is a constant
bool is_constant_var(const char *name) {
    int idx = find_variable(name);
    if (idx != -1) {
        return var_table[idx].is_constant;
    }
    return false;
}

// Get the value of a constant
int get_constant_value(const char *name) {
    int idx = find_variable(name);
    if (idx != -1) {
        return var_table[idx].value;
    }
    return 0;  // Default to 0 (should not happen if checking is_constant first)
}

// Check if a string is a numeric value
bool is_numeric(const char *str) {
    if (str == NULL || *str == '\0') {
        return false;
    }
    
    // Skip leading whitespace
    while (isspace((unsigned char)*str)) {
        str++;
    }
    
    // Handle optional sign
    if (*str == '+' || *str == '-') {
        str++;
    }
    
    // Check for at least one digit
    bool has_digit = false;
    while (isdigit((unsigned char)*str)) {
        has_digit = true;
        str++;
    }
    
    // Skip trailing whitespace
    while (isspace((unsigned char)*str)) {
        str++;
    }
    
    return has_digit && *str == '\0';
}

// Process the input file
void process_input(FILE *input, FILE *output) {
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), input)) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        // Process based on the first character
        if (line[0] == '=') {
            // Assignment line (e.g., "= 3 - a")
            handle_assignment(line, output);
        } else if (line[0] == '+' || line[0] == '-' || line[0] == '*' || line[0] == '/') {
            // Operation line (e.g., "+ a b t1")
            handle_operation(line, output);
        } else {
            // Just copy the line as-is
            fprintf(output, "%s\n", line);
        }
    }
}

// Handle assignment line (= value var)
void handle_assignment(char *line, FILE *output) {
    char operator;
    char operand1[MAX_VAR_NAME];
    char operand2[MAX_VAR_NAME];
    char result[MAX_VAR_NAME];
    
    // Parse the assignment line
    if (sscanf(line, "= %s", result) == 1) {
        // Handle simple assignment (= value var)
        char value_str[MAX_VAR_NAME];
        if (sscanf(line, "= %s %s", value_str, result) == 2) {
            // Check if value is a numeric constant
            if (is_numeric(value_str)) {
                int value = atoi(value_str);
                add_variable(result, value, true);  // Mark as constant
                fprintf(output, "%s\n", line);  // Keep as-is
                return;
            }
            
            // Check if value is a constant variable
            if (is_constant_var(value_str)) {
                int value = get_constant_value(value_str);
                add_variable(result, value, true);  // Mark as constant
                fprintf(output, "= %d %s\n", value, result);  // Replace with constant value
                return;
            }
            
            // Value is not a constant, so result isn't either
            add_variable(result, 0, false);
            fprintf(output, "%s\n", line);  // Keep as-is
            return;
        }
        
        // Handle operation assignment (= expr op var)
        if (sscanf(line, "= %s %c %s", operand1, &operator, operand2) == 3) {
            // Both operands are constants (numeric or variables)
            bool op1_is_const = is_numeric(operand1) || is_constant_var(operand1);
            bool op2_is_const = is_numeric(operand2) || is_constant_var(operand2);
            
            int val1, val2;
            if (is_numeric(operand1)) {
                val1 = atoi(operand1);
            } else if (is_constant_var(operand1)) {
                val1 = get_constant_value(operand1);
            }
            
            if (is_numeric(operand2)) {
                val2 = atoi(operand2);
            } else if (is_constant_var(operand2)) {
                val2 = get_constant_value(operand2);
            }
            
            // If both operands are constants, compute the result and mark as constant
            if (op1_is_const && op2_is_const) {
                int result_val;
                switch (operator) {
                    case '+': result_val = val1 + val2; break;
                    case '-': result_val = val1 - val2; break;
                    case '*': result_val = val1 * val2; break;
                    case '/': 
                        if (val2 != 0) result_val = val1 / val2; 
                        else {
                            fprintf(stderr, "Warning: Division by zero\n");
                            result_val = 0;
                        }
                        break;
                    default: result_val = 0;
                }
                add_variable(result, result_val, true);
                
                // Output the line with constants replaced
                fprintf(output, "= %d %s\n", result_val, result);
                return;
            }
            
            // Replace individual constants but keep the operation
            char new_op1[MAX_VAR_NAME];
            char new_op2[MAX_VAR_NAME];
            
            if (op1_is_const) {
                sprintf(new_op1, "%d", val1);
            } else {
                strcpy(new_op1, operand1);
            }
            
            if (op2_is_const) {
                sprintf(new_op2, "%d", val2);
            } else {
                strcpy(new_op2, operand2);
            }
            
            fprintf(output, "= %s %c %s %s\n", new_op1, operator, new_op2, result);
            
            // Since at least one operand is not constant, result is not constant
            add_variable(result, 0, false);
            return;
        }
    }
    
    // If we get here, we couldn't parse the line properly
    fprintf(output, "%s\n", line);  // Keep as-is
}

// Handle operation line (op operand1 operand2 result)
void handle_operation(char *line, FILE *output) {
    char operator;
    char operand1[MAX_VAR_NAME];
    char operand2[MAX_VAR_NAME];
    char result[MAX_VAR_NAME];
    
    // Extract the operator
    operator = line[0];
    
    // Parse the operation line
    if (sscanf(line, "%c %s %s %s", &operator, operand1, operand2, result) == 4) {
        // Check if operands are constants
        bool op1_is_const = is_numeric(operand1) || is_constant_var(operand1);
        bool op2_is_const = is_numeric(operand2) || is_constant_var(operand2);
        
        int val1, val2;
        if (is_numeric(operand1)) {
            val1 = atoi(operand1);
        } else if (is_constant_var(operand1)) {
            val1 = get_constant_value(operand1);
        }
        
        if (is_numeric(operand2)) {
            val2 = atoi(operand2);
        } else if (is_constant_var(operand2)) {
            val2 = get_constant_value(operand2);
        }
        
        // If both operands are constants, compute the result and mark as constant
        if (op1_is_const && op2_is_const) {
            int result_val;
            switch (operator) {
                case '+': result_val = val1 + val2; break;
                case '-': result_val = val1 - val2; break;
                case '*': result_val = val1 * val2; break;
                case '/': 
                    if (val2 != 0) result_val = val1 / val2; 
                    else {
                        fprintf(stderr, "Warning: Division by zero\n");
                        result_val = 0;
                    }
                    break;
                default: result_val = 0;
            }
            add_variable(result, result_val, true);
            
            // Output the line with result value
            fprintf(output, "%c %d %d %s\n", operator, val1, val2, result);
            return;
        }
        
        // Replace individual constants but keep the operation
        char new_op1[MAX_VAR_NAME];
        char new_op2[MAX_VAR_NAME];
        
        if (op1_is_const) {
            sprintf(new_op1, "%d", val1);
        } else {
            strcpy(new_op1, operand1);
        }
        
        if (op2_is_const) {
            sprintf(new_op2, "%d", val2);
        } else {
            strcpy(new_op2, operand2);
        }
        
        fprintf(output, "%c %s %s %s\n", operator, new_op1, new_op2, result);
        
        // Since at least one operand is not constant, result is not constant
        add_variable(result, 0, false);
        return;
    }
    
    // If we get here, we couldn't parse the line properly
    fprintf(output, "%s\n", line);  // Keep as-is
}