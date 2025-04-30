#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1000
#define MAX_IDENTIFIER_LENGTH 100
#define MAX_CODE_LINES 1000

// Structure to represent a for-loop
typedef struct {
    char init[MAX_LINE_LENGTH];  // Initialization (e.g., i = 0)
    char cond[MAX_LINE_LENGTH];  // Condition (e.g., i < 10)
    char update[MAX_LINE_LENGTH]; // Update (e.g., i++)
    char var[MAX_IDENTIFIER_LENGTH]; // Loop variable
    int start;    // Start value
    int end;      // End value
    int step;     // Step value
    int body_start; // Line number where body starts
    int body_end;   // Line number where body ends
} ForLoop;

// Function prototypes
int parse_for_loop(const char* line, ForLoop* loop);
int find_matching_brace(char** code, int start_line, int num_lines);
int extract_loop_details(ForLoop* loop);
void unroll_loop(char** code, int* num_lines, ForLoop* loop, int unroll_factor);
void process_file(const char* input_file, const char* output_file, int unroll_factor);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_file> <output_file> <unroll_factor>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    int unroll_factor = atoi(argv[3]);
    
    if (unroll_factor <= 0) {
        printf("Error: Unroll factor must be positive\n");
        return 1;
    }
    
    process_file(input_file, output_file, unroll_factor);
    
    return 0;
}

// Process an input file and perform loop unrolling
void process_file(const char* input_file, const char* output_file, int unroll_factor) {
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
    
    // Look for for-loops and unroll them
    for (int i = 0; i < num_lines; i++) {
        ForLoop loop;
        if (parse_for_loop(code[i], &loop)) {
            // Find the loop body by locating matching braces
            loop.body_start = i + 1;
            loop.body_end = find_matching_brace(code, i, num_lines);
            
            if (loop.body_end != -1 && extract_loop_details(&loop)) {
                printf("Found for-loop from line %d to %d\n", i + 1, loop.body_end + 1);
                printf("  Variable: %s, Start: %d, End: %d, Step: %d\n", 
                       loop.var, loop.start, loop.end, loop.step);
                
                // Unroll the loop
                unroll_loop(code, &num_lines, &loop, unroll_factor);
                
                // Skip past the loop we just processed
                i = loop.body_end;
            }
        }
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
    printf("Loop unrolling completed. Output written to %s\n", output_file);
}

// Parse a for-loop statement
int parse_for_loop(const char* line, ForLoop* loop) {
    // Check if the line contains "for ("
    const char* for_start = strstr(line, "for (");
    if (!for_start) {
        return 0;
    }
    
    // Extract the loop header
    char header[MAX_LINE_LENGTH];
    char* header_start = strstr(line, "(");
    char* header_end = strstr(line, ")");
    
    if (!header_start || !header_end || header_end <= header_start) {
        return 0;
    }
    
    // Extract the content between parentheses
    int header_len = header_end - header_start - 1;
    strncpy(header, header_start + 1, header_len);
    header[header_len] = '\0';
    
    // Split by semicolons
    char* parts[3] = {NULL, NULL, NULL};
    char* token = strtok(header, ";");
    int part_idx = 0;
    
    while (token != NULL && part_idx < 3) {
        // Trim whitespace
        while (*token && isspace(*token)) token++;
        parts[part_idx++] = token;
        token = strtok(NULL, ";");
    }
    
    if (part_idx != 3) {
        return 0;  // Not a well-formed for-loop
    }
    
    // Save the parts
    strcpy(loop->init, parts[0]);
    strcpy(loop->cond, parts[1]);
    strcpy(loop->update, parts[2]);
    
    return 1;
}

// Find the matching closing brace for a loop or block
int find_matching_brace(char** code, int start_line, int num_lines) {
    int brace_count = 0;
    bool found_opening = false;
    
    for (int i = start_line; i < num_lines; i++) {
        for (int j = 0; code[i][j]; j++) {
            if (code[i][j] == '{') {
                found_opening = true;
                brace_count++;
            } else if (code[i][j] == '}') {
                brace_count--;
                if (found_opening && brace_count == 0) {
                    return i;  // Found matching closing brace
                }
            }
        }
    }
    
    return -1;  // No matching brace found
}

// Extract loop details (variable, start, end, step)
int extract_loop_details(ForLoop* loop) {
    // Parse initialization (e.g., "int i = 0" or "i = 0")
    char var[MAX_IDENTIFIER_LENGTH];
    int start_val;
    
    if (sscanf(loop->init, "%*[^=]=%d", &start_val) == 1) {
        // Extract variable name
        char* equals = strchr(loop->init, '=');
        if (!equals) return 0;
        
        // Get the part before '='
        int var_part_len = equals - loop->init;
        strncpy(var, loop->init, var_part_len);
        var[var_part_len] = '\0';
        
        // Trim trailing whitespace and type declarations
        char* p = var + var_part_len - 1;
        while (p >= var && (isspace(*p) || *p == '*')) p--;
        p[1] = '\0';
        
        // Find the start of the identifier
        p = var;
        while (*p && !isalnum(*p) && *p != '_') p++;
        strcpy(loop->var, p);
        
        // Trim trailing whitespace
        p = loop->var + strlen(loop->var) - 1;
        while (p >= loop->var && isspace(*p)) p--;
        p[1] = '\0';
    } else {
        return 0;  // Could not parse initialization
    }
    
    // Parse condition (e.g., "i < 10" or "i <= 10")
    int end_val;
    char relation[3];
    if (sscanf(loop->cond, "%*[^ <>=!]%2[<>=!]%d", relation, &end_val) != 2) {
        return 0;  // Could not parse condition
    }
    
    // Parse update (e.g., "i++" or "i += 1" or "i = i + 1")
    int step_val = 1;  // Default step
    if (strstr(loop->update, "++")) {
        step_val = 1;
    } else if (strstr(loop->update, "--")) {
        step_val = -1;
    } else if (strstr(loop->update, "+=")) {
        sscanf(loop->update, "%*[^+]+=%d", &step_val);
    } else if (strstr(loop->update, "-=")) {
        sscanf(loop->update, "%*[^-]-=%d", &step_val);
        step_val = -step_val;
    } else {
        // Try to handle "i = i + 1" style
        char op;
        int val;
        if (sscanf(loop->update, "%*[^=]=%*[^+-]%c%d", &op, &val) == 2) {
            step_val = (op == '+') ? val : -val;
        } else {
            return 0;  // Could not parse update
        }
    }
    
    // Adjust end value based on relation
    if (strcmp(relation, "<") == 0) {
        end_val--;  // i < 10 is equivalent to i <= 9
    } else if (strcmp(relation, ">") == 0) {
        end_val++;  // i > 0 is equivalent to i >= 1
    } else if (strcmp(relation, "!=") == 0) {
        // Assume loop runs until i == end_val
        if (step_val > 0) {
            end_val--;  // If incrementing, end just before end_val
        } else {
            end_val++;  // If decrementing, end just after end_val
        }
    }
    
    // Set loop properties
    loop->start = start_val;
    loop->end = end_val;
    loop->step = step_val;
    
    return 1;
}

// Unroll a loop by the specified factor
void unroll_loop(char** code, int* num_lines, ForLoop* loop, int unroll_factor) {
    // Calculate the number of iterations
    int num_iterations = 0;
    if (loop->step > 0) {
        num_iterations = (loop->end - loop->start + 1) / loop->step;
    } else {
        num_iterations = (loop->start - loop->end + 1) / (-loop->step);
    }
    
    if (num_iterations <= 0) {
        return;  // No iterations to unroll
    }
    
    // Calculate how many complete unrolled iterations we can do
    int complete_unrolls = num_iterations / unroll_factor;
    int remainder = num_iterations % unroll_factor;
    
    // Prepare the new code
    char** new_code = (char**)malloc(sizeof(char*) * MAX_CODE_LINES);
    if (!new_code) {
        perror("Memory allocation failed");
        exit(1);
    }
    
    int new_line_idx = 0;
    
    // Copy code before the loop
    for (int i = 0; i < loop->body_start - 1; i++) {
        new_code[new_line_idx++] = strdup(code[i]);
    }
    
    // Generate a comment about the unrolling
    char unroll_comment[MAX_LINE_LENGTH];
    sprintf(unroll_comment, "/* Unrolled loop by factor %d */", unroll_factor);
    new_code[new_line_idx++] = strdup(unroll_comment);
    
    // If we have complete unrolls, generate a new loop for them
    if (complete_unrolls > 0) {
        // Create new loop header with step * unroll_factor
        char new_for_header[MAX_LINE_LENGTH];
        sprintf(new_for_header, "for (%s; %s; %s += %d) {", 
                loop->init, loop->cond, loop->var, loop->step * unroll_factor);
        new_code[new_line_idx++] = strdup(new_for_header);
        
        // Generate the unrolled body
        for (int i = 0; i < unroll_factor; i++) {
            // Create a copy of the loop body with the variable replaced
            for (int j = loop->body_start; j < loop->body_end; j++) {
                // Replace the loop variable with its value at this iteration
                char replaced_line[MAX_LINE_LENGTH];
                strcpy(replaced_line, code[j]);
                
                // Add an offset to the loop variable for each unrolled iteration
                if (i > 0) {
                    char var_with_offset[MAX_IDENTIFIER_LENGTH + 20];
                    sprintf(var_with_offset, "%s + %d", loop->var, i * loop->step);
                    
                    // Replace the variable with the offset version
                    char* pos = replaced_line;
                    while ((pos = strstr(pos, loop->var)) != NULL) {
                        // Make sure it's a standalone variable and not a substring
                        if ((pos == replaced_line || !isalnum(*(pos-1))) && 
                            !isalnum(*(pos + strlen(loop->var)))) {
                            
                            // Replace with the offset version
                            char temp[MAX_LINE_LENGTH];
                            strncpy(temp, replaced_line, pos - replaced_line);
                            temp[pos - replaced_line] = '\0';
                            strcat(temp, var_with_offset);
                            strcat(temp, pos + strlen(loop->var));
                            strcpy(replaced_line, temp);
                            
                            // Move past the replacement
                            pos = replaced_line + (pos - replaced_line) + strlen(var_with_offset);
                        } else {
                            // Move past this occurence
                            pos += strlen(loop->var);
                        }
                    }
                }
                
                new_code[new_line_idx++] = strdup(replaced_line);
            }
        }
        
        // Close the new loop
        new_code[new_line_idx++] = strdup("}");
    }
    
    // Handle remaining iterations with a separate loop if needed
    if (remainder > 0) {
        char remainder_comment[MAX_LINE_LENGTH];
        sprintf(remainder_comment, "/* Handle remaining %d iterations */", remainder);
        new_code[new_line_idx++] = strdup(remainder_comment);
        
        // Calculate the start value for the remainder loop
        int remainder_start = loop->start + complete_unrolls * unroll_factor * loop->step;
        
        // Create the remainder loop
        char remainder_init[MAX_LINE_LENGTH];
        sprintf(remainder_init, "for (int %s = %d; ", loop->var, remainder_start);
        
        // Create the appropriate condition
        char remainder_cond[MAX_LINE_LENGTH];
        if (loop->step > 0) {
            sprintf(remainder_cond, "%s <= %d; ", loop->var, loop->end);
        } else {
            sprintf(remainder_cond, "%s >= %d; ", loop->var, loop->end);
        }
        
        // Create the update expression
        char remainder_update[MAX_LINE_LENGTH];
        if (loop->step == 1) {
            sprintf(remainder_update, "%s++) {", loop->var);
        } else if (loop->step == -1) {
            sprintf(remainder_update, "%s--) {", loop->var);
        } else {
            sprintf(remainder_update, "%s += %d) {", loop->var, loop->step);
        }
        
        // Combine into a complete for loop
        char remainder_for[MAX_LINE_LENGTH];
        sprintf(remainder_for, "%s%s%s", remainder_init, remainder_cond, remainder_update);
        new_code[new_line_idx++] = strdup(remainder_for);
        
        // Copy the loop body
        for (int j = loop->body_start; j < loop->body_end; j++) {
            new_code[new_line_idx++] = strdup(code[j]);
        }
        
        // Close the remainder loop
        new_code[new_line_idx++] = strdup("}");
    }
    
    // Copy code after the loop
    for (int i = loop->body_end + 1; i < *num_lines; i++) {
        new_code[new_line_idx++] = strdup(code[i]);
    }
    
    // Replace the old code with the new code
    for (int i = 0; i < *num_lines; i++) {
        free(code[i]);
    }
    
    for (int i = 0; i < new_line_idx; i++) {
        code[i] = new_code[i];
    }
    
    *num_lines = new_line_idx;
    free(new_code);
}