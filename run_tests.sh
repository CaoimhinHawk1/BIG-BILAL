#!/bin/bash
# Improved script to compile and run all tasks

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to check if the last command succeeded
check_status() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Success${NC}"
    else
        echo -e "${RED}✗ Failed${NC}"
        # Don't exit immediately, try to run other tests
    fi
}

# Function to display task header
task_header() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Create tests directory if it doesn't exist
mkdir -p tests

# Copy test files to tests directory
echo -e "${YELLOW}Copying test files...${NC}"
cp test_input.c tests/ 2>/dev/null || echo -e "${RED}Warning: test_input.c not found${NC}"
cp loop_test.c tests/ 2>/dev/null || echo -e "${RED}Warning: loop_test.c not found${NC}"
cp const_test.c tests/ 2>/dev/null || echo -e "${RED}Warning: const_test.c not found${NC}"
cp Tasks/Task4/grammar.txt tests/ 2>/dev/null || echo -e "${RED}Warning: grammar.txt not found${NC}"

# Compile all tasks
task_header "Compiling all tasks"
echo -e "${YELLOW}Running make all...${NC}"
make all
check_status

# Task 1: Lexical Analyzer in C
task_header "Task 1: Lexical Analyzer in C"
if [ -f "Tasks/Task1/lexer" ]; then
    echo -e "${YELLOW}Running lexer on test_input.c...${NC}"
    Tasks/Task1/lexer tests/test_input.c
    check_status
else
    echo -e "${RED}Error: Tasks/Task1/lexer not found${NC}"
fi

# Task 2: Lexical Analyzer using Lex
task_header "Task 2: Lexical Analyzer using Lex"
if [ -f "Tasks/Task2/lexer" ]; then
    echo -e "${YELLOW}Running lexer on test_input.c...${NC}"
    Tasks/Task2/lexer tests/test_input.c
    check_status
else
    echo -e "${RED}Error: Tasks/Task2/lexer not found${NC}"
fi

# Task 3a: Arithmetic Expression Recognizer
task_header "Task 3a: Arithmetic Expression Recognizer"
if [ -f "Tasks/Task3/3a/parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3a/parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  2+3*4${NC}"
    echo -e "${GREEN}  (10-5)/(2+1)${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3a/parser not found${NC}"
fi

# Task 3b: Variable Recognizer
task_header "Task 3b: Variable Recognizer"
if [ -f "Tasks/Task3/3b/parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3b/parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  validVar${NC}"
    echo -e "${GREEN}  _privateVar${NC}"
    echo -e "${RED}  123invalidVar (should be rejected)${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3b/parser not found${NC}"
fi

# Task 3c: Calculator
task_header "Task 3c: Calculator using LEX and YACC"
if [ -f "Tasks/Task3/3c/calculator" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3c/calculator${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  2+3*4${NC}"
    echo -e "${GREEN}  (20-5)*3${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3c/calculator not found${NC}"
fi

# Task 3d: Abstract Syntax Tree
task_header "Task 3d: Abstract Syntax Tree Generator"
if [ -f "Tasks/Task3/3d/ast_gen" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3d/ast_gen${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  x = 10 + 5;${NC}"
    echo -e "${GREEN}  y = x * 2;${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3d/ast_gen not found${NC}"
fi

# Task 4: First and Follow Sets
task_header "Task 4: First and Follow Sets"
if [ -f "Tasks/Task4/ffsets" ] && [ -f "tests/grammar.txt" ]; then
    echo -e "${YELLOW}Running First and Follow Sets calculation...${NC}"
    Tasks/Task4/ffsets tests/grammar.txt
    check_status
else
    echo -e "${RED}Error: Tasks/Task4/ffsets or tests/grammar.txt not found${NC}"
fi

# Task 5: LL(1) Parser
task_header "Task 5: LL(1) Parser"
if [ -f "Tasks/Task5/ll1parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task5/ll1parser tests/grammar.txt${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  ab#${NC}"
    echo -e "${GREEN}  a#${NC}"
else
    echo -e "${RED}Error: Tasks/Task5/ll1parser not found${NC}"
fi

# Task 6: Operator Precedence Parser
task_header "Task 6: Operator Precedence Parser"
if [ -f "Tasks/Task6/op_parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task6/op_parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  id+id*id${NC}"
    echo -e "${GREEN}  id*(id+id)${NC}"
else
    echo -e "${RED}Error: Tasks/Task6/op_parser not found${NC}"
fi

# Task 7: Intermediate Code Generation
task_header "Task 7: Intermediate Code Generation"
if [ -f "Tasks/Task7/icg" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task7/icg${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  a+b*c${NC}"
    echo -e "${GREEN}  x=(a+b)*(c-d)${NC}"
else
    echo -e "${RED}Error: Tasks/Task7/icg not found${NC}"
fi

# Task 8: Loop Unrolling
task_header "Task 8: Loop Unrolling"
if [ -f "Tasks/Task8/loop_unroll" ] && [ -f "tests/loop_test.c" ]; then
    echo -e "${YELLOW}Running loop unrolling with factor 4...${NC}"
    Tasks/Task8/loop_unroll tests/loop_test.c tests/loop_test_unrolled.c 4
    check_status
    
    # Show diff if successful
    if [ $? -eq 0 ]; then
        echo -e "${YELLOW}Differences between original and unrolled code:${NC}"
        echo -e "${BLUE}-----------------------------------------------${NC}"
        diff -u tests/loop_test.c tests/loop_test_unrolled.c | grep -E '^\+|^\-' | grep -v '+++' | grep -v '\-\-\-'
        echo -e "${BLUE}-----------------------------------------------${NC}"
        echo -e "${YELLOW}Complete output in tests/loop_test_unrolled.c${NC}"
    fi
else
    echo -e "${RED}Error: Tasks/Task8/loop_unroll or tests/loop_test.c not found${NC}"
fi

# Task 9: Constant Propagation
task_header "Task 9: Constant Propagation"
if [ -f "Tasks/Task9/const_prop" ] && [ -f "tests/const_test.c" ]; then
    echo -e "${YELLOW}Running constant propagation...${NC}"
    Tasks/Task9/const_prop tests/const_test.c tests/const_test_optimized.c
    check_status
    
    # Show diff if successful
    if [ $? -eq 0 ]; then
        echo -e "${YELLOW}Differences between original and optimized code:${NC}"
        echo -e "${BLUE}-----------------------------------------------${NC}"
        diff -u tests/const_test.c tests/const_test_optimized.c | grep -E '^\+|^\-' | grep -v '+++' | grep -v '\-\-\-'
        echo -e "${BLUE}-----------------------------------------------${NC}"
        echo -e "${YELLOW}Complete output in tests/const_test_optimized.c${NC}"
    fi
else
    echo -e "${RED}Error: Tasks/Task9/const_prop or tests/const_test.c not found${NC}"
fi

# Summary
task_header "Test Summary"
echo -e "${GREEN}All tests completed!${NC}"
echo -e "${YELLOW}Note: Some tasks require manual testing as they are interactive.${NC}"
echo -e "${YELLOW}To manually test a task, run the corresponding executable and follow the instructions.${NC}"
echo -e "\n${BLUE}Thank you for using the compiler design test suite!${NC}"#!/bin/bash
# Improved script to compile and run all tasks

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to check if the last command succeeded
check_status() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Success${NC}"
    else
        echo -e "${RED}✗ Failed${NC}"
        # Don't exit immediately, try to run other tests
    fi
}

# Function to display task header
task_header() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Create tests directory if it doesn't exist
mkdir -p tests

# Copy test files to tests directory
echo -e "${YELLOW}Copying test files...${NC}"
cp test_input.c tests/ 2>/dev/null || echo -e "${RED}Warning: test_input.c not found${NC}"
cp loop_test.c tests/ 2>/dev/null || echo -e "${RED}Warning: loop_test.c not found${NC}"
cp const_test.c tests/ 2>/dev/null || echo -e "${RED}Warning: const_test.c not found${NC}"
cp Tasks/Task4/grammar.txt tests/ 2>/dev/null || echo -e "${RED}Warning: grammar.txt not found${NC}"

# Compile all tasks
task_header "Compiling all tasks"
echo -e "${YELLOW}Running make all...${NC}"
make all
check_status

# Task 1: Lexical Analyzer in C
task_header "Task 1: Lexical Analyzer in C"
if [ -f "Tasks/Task1/lexer" ]; then
    echo -e "${YELLOW}Running lexer on test_input.c...${NC}"
    Tasks/Task1/lexer tests/test_input.c
    check_status
else
    echo -e "${RED}Error: Tasks/Task1/lexer not found${NC}"
fi

# Task 2: Lexical Analyzer using Lex
task_header "Task 2: Lexical Analyzer using Lex"
if [ -f "Tasks/Task2/lexer" ]; then
    echo -e "${YELLOW}Running lexer on test_input.c...${NC}"
    Tasks/Task2/lexer tests/test_input.c
    check_status
else
    echo -e "${RED}Error: Tasks/Task2/lexer not found${NC}"
fi

# Task 3a: Arithmetic Expression Recognizer
task_header "Task 3a: Arithmetic Expression Recognizer"
if [ -f "Tasks/Task3/3a/parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3a/parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  2+3*4${NC}"
    echo -e "${GREEN}  (10-5)/(2+1)${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3a/parser not found${NC}"
fi

# Task 3b: Variable Recognizer
task_header "Task 3b: Variable Recognizer"
if [ -f "Tasks/Task3/3b/parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3b/parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  validVar${NC}"
    echo -e "${GREEN}  _privateVar${NC}"
    echo -e "${RED}  123invalidVar (should be rejected)${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3b/parser not found${NC}"
fi

# Task 3c: Calculator
task_header "Task 3c: Calculator using LEX and YACC"
if [ -f "Tasks/Task3/3c/calculator" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3c/calculator${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  2+3*4${NC}"
    echo -e "${GREEN}  (20-5)*3${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3c/calculator not found${NC}"
fi

# Task 3d: Abstract Syntax Tree
task_header "Task 3d: Abstract Syntax Tree Generator"
if [ -f "Tasks/Task3/3d/ast_gen" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task3/3d/ast_gen${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  x = 10 + 5;${NC}"
    echo -e "${GREEN}  y = x * 2;${NC}"
else
    echo -e "${RED}Error: Tasks/Task3/3d/ast_gen not found${NC}"
fi

# Task 4: First and Follow Sets
task_header "Task 4: First and Follow Sets"
if [ -f "Tasks/Task4/ffsets" ] && [ -f "tests/grammar.txt" ]; then
    echo -e "${YELLOW}Running First and Follow Sets calculation...${NC}"
    Tasks/Task4/ffsets tests/grammar.txt
    check_status
else
    echo -e "${RED}Error: Tasks/Task4/ffsets or tests/grammar.txt not found${NC}"
fi

# Task 5: LL(1) Parser
task_header "Task 5: LL(1) Parser"
if [ -f "Tasks/Task5/ll1parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task5/ll1parser tests/grammar.txt${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  ab#${NC}"
    echo -e "${GREEN}  a#${NC}"
else
    echo -e "${RED}Error: Tasks/Task5/ll1parser not found${NC}"
fi

# Task 6: Operator Precedence Parser
task_header "Task 6: Operator Precedence Parser"
if [ -f "Tasks/Task6/op_parser" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task6/op_parser${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  id+id*id${NC}"
    echo -e "${GREEN}  id*(id+id)${NC}"
else
    echo -e "${RED}Error: Tasks/Task6/op_parser not found${NC}"
fi

# Task 7: Intermediate Code Generation
task_header "Task 7: Intermediate Code Generation"
if [ -f "Tasks/Task7/icg" ]; then
    echo -e "${YELLOW}Manual testing required with: Tasks/Task7/icg${NC}"
    echo -e "${YELLOW}Example inputs:${NC}"
    echo -e "${GREEN}  a+b*c${NC}"
    echo -e "${GREEN}  x=(a+b)*(c-d)${NC}"
else
    echo -e "${RED}Error: Tasks/Task7/icg not found${NC}"
fi

# Task 8: Loop Unrolling
task_header "Task 8: Loop Unrolling"
if [ -f "Tasks/Task8/loop_unroll" ] && [ -f "tests/loop_test.c" ]; then
    echo -e "${YELLOW}Running loop unrolling with factor 4...${NC}"
    Tasks/Task8/loop_unroll tests/loop_test.c tests/loop_test_unrolled.c 4
    check_status
    
    # Show diff if successful
    if [ $? -eq 0 ]; then
        echo -e "${YELLOW}Differences between original and unrolled code:${NC}"
        echo -e "${BLUE}-----------------------------------------------${NC}"
        diff -u tests/loop_test.c tests/loop_test_unrolled.c | grep -E '^\+|^\-' | grep -v '+++' | grep -v '\-\-\-'
        echo -e "${BLUE}-----------------------------------------------${NC}"
        echo -e "${YELLOW}Complete output in tests/loop_test_unrolled.c${NC}"
    fi
else
    echo -e "${RED}Error: Tasks/Task8/loop_unroll or tests/loop_test.c not found${NC}"
fi

# Task 9: Constant Propagation
task_header "Task 9: Constant Propagation"
if [ -f "Tasks/Task9/const_prop" ] && [ -f "tests/const_test.c" ]; then
    echo -e "${YELLOW}Running constant propagation...${NC}"
    Tasks/Task9/const_prop tests/const_test.c tests/const_test_optimized.c
    check_status
    
    # Show diff if successful
    if [ $? -eq 0 ]; then
        echo -e "${YELLOW}Differences between original and optimized code:${NC}"
        echo -e "${BLUE}-----------------------------------------------${NC}"
        diff -u tests/const_test.c tests/const_test_optimized.c | grep -E '^\+|^\-' | grep -v '+++' | grep -v '\-\-\-'
        echo -e "${BLUE}-----------------------------------------------${NC}"
        echo -e "${YELLOW}Complete output in tests/const_test_optimized.c${NC}"
    fi
else
    echo -e "${RED}Error: Tasks/Task9/const_prop or tests/const_test.c not found${NC}"
fi

# Summary
task_header "Test Summary"
echo -e "${GREEN}All tests completed!${NC}"
echo -e "${YELLOW}Note: Some tasks require manual testing as they are interactive.${NC}"
echo -e "${YELLOW}To manually test a task, run the corresponding executable and follow the instructions.${NC}"
echo -e "\n${BLUE}Thank you for using the compiler design test suite!${NC}"