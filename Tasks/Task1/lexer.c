#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_KEYWORD,
    TOKEN_STRING
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char lexeme[100];
    int line;
    int column;
} Token;

// Keywords array
const char *keywords[] = {
    "if", "else", "while", "for", "int", "float", "char", "void", "return", NULL
};

// Function to check if a string is a keyword
int is_keyword(const char *str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to get the next token from input
Token get_token(FILE *fp, int *line, int *column) {
    Token token;
    char c;
    
    // Skip whitespace characters (spaces, tabs, newlines)
    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\t') {
            (*column)++;
            continue;
        } else if (c == '\n') {
            (*line)++;
            *column = 1;
            continue;
        } else {
            ungetc(c, fp);
            break;
        }
    }
    
    // Initialize token
    token.line = *line;
    token.column = *column;
    
    // Read the next character
    c = fgetc(fp);
    
    // Check for EOF
    if (c == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    // Update column position
    (*column)++;
    
    // Check for identifiers (starting with a letter or underscore)
    if (isalpha(c) || c == '_') {
        int i = 0;
        token.lexeme[i++] = c;
        
        // Continue reading until we hit a non-identifier character
        while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
            token.lexeme[i++] = c;
            (*column)++;
        }
        
        // Put back the last character read
        if (c != EOF) {
            ungetc(c, fp);
        }
        
        token.lexeme[i] = '\0';
        
        // Check if the identifier is a keyword
        if (is_keyword(token.lexeme)) {
            token.type = TOKEN_KEYWORD;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        
        return token;
    }
    
    // Check for numbers
    if (isdigit(c)) {
        int i = 0;
        token.lexeme[i++] = c;
        
        // Continue reading digits
        while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.')) {
            token.lexeme[i++] = c;
            (*column)++;
        }
        
        // Put back the last character read
        if (c != EOF) {
            ungetc(c, fp);
        }
        
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        
        return token;
    }
    
    // Check for operators
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || c == '!') {
        int i = 0;
        token.lexeme[i++] = c;
        
        // Check for double operators like ==, <=, >=, !=
        char nextc = fgetc(fp);
        if (nextc == '=' && (c == '=' || c == '<' || c == '>' || c == '!')) {
            token.lexeme[i++] = nextc;
            (*column)++;
        } else if (nextc != EOF) {
            ungetc(nextc, fp);
        }
        
        token.lexeme[i] = '\0';
        token.type = TOKEN_OPERATOR;
        
        return token;
    }
    
    // Check for delimiters
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '.') {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_DELIMITER;
        
        return token;
    }
    
    // Check for string literals
    if (c == '"') {
        int i = 0;
        token.lexeme[i++] = c;
        
        // Read until closing quote or EOF
        while ((c = fgetc(fp)) != EOF && c != '"') {
            token.lexeme[i++] = c;
            (*column)++;
            
            // Handle escape sequences
            if (c == '\\') {
                c = fgetc(fp);
                if (c != EOF) {
                    token.lexeme[i++] = c;
                    (*column)++;
                }
            }
        }
        
        if (c == '"') {
            token.lexeme[i++] = c;
            (*column)++;
        }
        
        token.lexeme[i] = '\0';
        token.type = TOKEN_STRING;
        
        return token;
    }
    
    // Unrecognized character
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    token.type = TOKEN_OPERATOR;  // Default to operator for unrecognized chars
    
    return token;
}

// Function to get token type as string
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_DELIMITER: return "DELIMITER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_STRING: return "STRING";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    int line = 1;
    int column = 1;
    Token token;
    
    printf("TOKEN\t\tLEXEME\t\tLINE\tCOLUMN\n");
    printf("----------------------------------------------\n");
    
    do {
        token = get_token(fp, &line, &column);
        printf("%-10s\t%-10s\t%d\t%d\n", 
               token_type_to_string(token.type), 
               token.lexeme, 
               token.line, 
               token.column);
    } while (token.type != TOKEN_EOF);
    
    fclose(fp);
    return 0;
}