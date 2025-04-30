#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1000
#define MAX_IDENTIFIER_LENGTH 100
#define MAX_CODE_LINES 1000
#define MAX_VARIABLES 100

// Structure to store constant values
typedef struct {
    char name[MAX_IDENTIFIER_LENGTH];
    int value;
    bool is_constant;
} Variable;

// Global variable table
Variable variables[MAX_VARIABLES];
int variable_count = 0;

// Function prototypes
void process_file(const char* input_file, const char* output_file);
int find_variable(const char* name);
void add_variable(const char* name, int value, bool is_constant);
void update_variable(int index, int value, bool is_constant);
void parse_assignment(char* line);
char* replace_constants(char* line);
bool evaluate_constant_expression(char* expr, int* result);
int parse_expression(char* expr);
bool is_numeric_literal(const char* str);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    process_file(input_file, output_file);
    
    return 0;
}

// Process the input file and perform constant propagation
void process_file(const char* input_file, const char* output_file) {
    FILE* in_fp = fopen(input_file, "r");
    if (!in_fp) {
        perror("Error opening input file");
        exit(1);
    }
    
    // Read the entire file
    char* code[MAX_CODE_LINES];
    int num_lines = 0;
    char buffer[MAX_LINE_LENGTH];
    
    while (fgets(buffer, MAX_LINE_LENGTH, in_fp) && num_lines < MAX_CODE_LINES) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        code[num_lines] = strdup(buffer);
        num_lines++;
    }
    
    fclose(in_fp);
    
    // First pass: identify constant variables and their values
    for (int i = 0; i < num_lines; i++) {
        parse_assignment(code[i]);
    }
    
    // Print the variables found
    printf("Variables identified:\n");
    for (int i = 0; i < variable_count; i++) {
        printf("  %s = %d (constant: %s)\n", 
               variables[i].name, variables[i].value, 
               variables[i].is_constant ? "yes" : "no");
    }
    
    // Second pass: replace constants in expressions
    for (int i = 0; i < num_lines; i++) {
        char* modified_line = replace_constants(code[i]);
        free(code[i]);
        code[i] = modified_line;
    }
    
    // Write the output file
    FILE* out_fp = fopen(output_file, "w");
    if (!out_fp) {
        perror("Error opening output file");
        exit(1);
    }
    
    for (int i = 0; i < num_lines; i++) {
        fprintf(out_fp, "%s\n", code[i]);
        free(code[i]);  // Free memory
    }
    
    fclose(out_fp);
    printf("Constant propagation completed. Output written to %s\n", output_file);
}

// Find a variable in the table by name
int find_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

// Add a new variable to the table
void add_variable(const char* name, int value, bool is_constant) {
    if (variable_count >= MAX_VARIABLES) {
        printf("Error: Variable table full\n");
        return;
    }
    
    strcpy(variables[variable_count].name, name);
    variables[variable_count].value = value;
    variables[variable_count].is_constant = is_constant;
    variable_count++;
}

// Update an existing variable
void update_variable(int index, int value, bool is_constant) {
    variables[index].value = value;
    variables[index].is_constant = is_constant;
}

// Parse an assignment statement
void parse_assignment(char* line) {
    // Skip whitespace and preprocessor directives
    if (line[0] == '#' || line[0] == '\0') {
        return;
    }
    
    // Look for assignment operator
    char* assign_pos = strstr(line, "=");
    if (!assign_pos) {
        return;  // No assignment found
    }
    
    // Extract the left-hand side (variable name)
    char var_name[MAX_IDENTIFIER_LENGTH];
    int name_len = assign_pos - line;
    if (name_len >= MAX_IDENTIFIER_LENGTH) {
        return;  // Variable name too long
    }
    
    strncpy(var_name, line, name_len);
    var_name[name_len] = '\0';
    
    // Trim trailing whitespace from variable name
    char* p = var_name + name_len - 1;
    while (p >= var_name && isspace(*p)) p--;
    p[1] = '\0';
    
    // Skip keywords and declarations
    if (strstr(var_name, "int ") || strstr(var_name, "float ") || 
        strstr(var_name, "char ") || strstr(var_name, "double ")) {
        // Extract just the variable name after the type
        char* start = var_name;
        while (*start && !isalpha(*start) && *start != '_') start++;
        
        if (!*start) return;  // No valid identifier found
        
        p = start;
        while (*p && (isalnum(*p) || *p == '_')) p++;
        *p = '\0';
        
        strcpy(var_name, start);
    } else {
        // Trim leading whitespace
        char* start = var_name;
        while (*start && isspace(*start)) start++;
        
        if (!*start) return;  // No valid identifier found
        
        // Check if it's a valid identifier
        if (!isalpha(*start) && *start != '_') {
            return;  // Not a valid identifier
        }
        
        strcpy(var_name, start);
    }
    
    // Extract the right-hand side (expression)
    char* expr = assign_pos + 1;
    
    // Trim leading whitespace from expression
    while (*expr && isspace(*expr)) expr++;
    
    // Check if this is a constant assignment
    bool is_constant = false;
    int value = 0;
    
    if (evaluate_constant_expression(expr, &value)) {
        is_constant = true;
    }
    
    // Update the variable table
    int var_index = find_variable(var_name);
    if (var_index == -1) {
        add_variable(var_name, value, is_constant);
    } else {
        update_variable(var_index, value, is_constant);
    }
}

// Replace constants in a line of code
char* replace_constants(char* line) {
    char result[MAX_LINE_LENGTH];
    strcpy(result, line);
    
    // Don't modify preprocessor directives, comments, or string literals
    if (line[0] == '#' || strstr(line, "//") == line || strstr(line, "/*") == line ||
        strstr(line, "\"") != NULL) {
        return strdup(result);
    }
    
    // Don't modify the left side of assignments
    char* assign_pos = strstr(result, "=");
    if (assign_pos) {
        // Only process the right-hand side
        char* expr = assign_pos + 1;
        
        // Look for constant variables to replace
        for (int i = 0; i < variable_count; i++) {
            if (!variables[i].is_constant) continue;
            
            char* pos = expr;
            while ((pos = strstr(pos, variables[i].name)) != NULL) {
                // Check if it's a standalone variable (not part of another identifier)
                if ((pos == expr || !isalnum(*(pos-1))) && 
                    !isalnum(*(pos + strlen(variables[i].name)))) {
                    
                    // Replace with the constant value
                    char value_str[20];
                    sprintf(value_str, "%d", variables[i].value);
                    
                    char temp[MAX_LINE_LENGTH];
                    strncpy(temp, result, pos - result);
                    temp[pos - result] = '\0';
                    strcat(temp, value_str);
                    strcat(temp, pos + strlen(variables[i].name));
                    strcpy(result, temp);
                    
                    // Start from the beginning of the replaced text
                    expr = assign_pos + 1;
                    pos = expr;
                } else {
                    // Move past this occurrence
                    pos += strlen(variables[i].name);
                }
            }
        }
    } else {
        // No assignment, just replace all constants
        for (int i = 0; i < variable_count; i++) {
            if (!variables[i].is_constant) continue;
            
            char* pos = result;
            while ((pos = strstr(pos, variables[i].name)) != NULL) {
                // Check if it's a standalone variable (not part of another identifier)
                if ((pos == result || !isalnum(*(pos-1))) && 
                    !isalnum(*(pos + strlen(variables[i].name)))) {
                    
                    // Replace with the constant value
                    char value_str[20];
                    sprintf(value_str, "%d", variables[i].value);
                    
                    char temp[MAX_LINE_LENGTH];
                    strncpy(temp, result, pos - result);
                    temp[pos - result] = '\0';
                    strcat(temp, value_str);
                    strcat(temp, pos + strlen(variables[i].name));
                    strcpy(result, temp);
                    
                    // Start from the beginning of the result
                    pos = result;
                } else {
                    // Move past this occurrence
                    pos += strlen(variables[i].name);
                }
            }
        }
    }
    
    return strdup(result);
}

// Check if a string is a numeric literal
bool is_numeric_literal(const char* str) {
    // Skip leading whitespace
    while (*str && isspace(*str)) str++;
    
    // Check for optional sign
    if (*str == '+' || *str == '-') str++;
    
    // Must have at least one digit
    if (!isdigit(*str)) return false;
    
    // Check the rest of the string
    while (*str && isdigit(*str)) str++;
    
    // Skip trailing whitespace
    while (*str && isspace(*str)) str++;
    
    // If we reached the end, it's a valid numeric literal
    return *str == '\0';
}

// Evaluate a constant expression
bool evaluate_constant_expression(char* expr, int* result) {
    // Simple case: numeric literal
    if (is_numeric_literal(expr)) {
        *result = atoi(expr);
        return true;
    }
    
    // Check for simple variable reference
    for (int i = 0; i < variable_count; i++) {
        if (variables[i].is_constant && strcmp(expr, variables[i].name) == 0) {
            *result = variables[i].value;
            return true;
        }
    }
    
    // TODO: For a complete solution, implement a full expression evaluator
    // that can handle complex expressions with operators and parentheses
    
    return false;  // Not a constant expression
}