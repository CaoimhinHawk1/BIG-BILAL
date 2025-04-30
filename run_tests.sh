#!/bin/bash
# Script to compile and run all tasks

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compiling all tasks...${NC}"
make all

# Create test directory if it doesn't exist
mkdir -p tests

# Copy test files to tests directory
cp test_input.c tests/
cp loop_test.c tests/
cp const_test.c tests/
cp Tasks/Task4/grammar.txt tests/

# Task 1: Lexical Analyzer in C
echo -e "\n${GREEN}Testing Task 1: Lexical Analyzer in C${NC}"
Tasks/Task1/lexer tests/test_input.c

# Task 2: Lexical Analyzer using Lex
echo -e "\n${GREEN}Testing Task 2: Lexical Analyzer using Lex${NC}"
Tasks/Task2/lexer tests/test_input.c

# Task 3a: Arithmetic Expression Recognizer
echo -e "\n${GREEN}Testing Task 3a: Arithmetic Expression Recognizer${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task3/3a/parser${NC}"
echo -e "${YELLOW}And enter expressions like: 2+3*4${NC}"

# Task 3b: Variable Recognizer
echo -e "\n${GREEN}Testing Task 3b: Variable Recognizer${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task3/3b/parser${NC}"
echo -e "${YELLOW}And enter variable names like: validVar123${NC}"

# Task 3c: Calculator
echo -e "\n${GREEN}Testing Task 3c: Calculator using LEX and YACC${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task3/3c/calculator${NC}"
echo -e "${YELLOW}And enter expressions like: 2+3*4${NC}"

# Task 3d: Abstract Syntax Tree
echo -e "\n${GREEN}Testing Task 3d: Abstract Syntax Tree Generator${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task3/3d/ast_gen${NC}"
echo -e "${YELLOW}And enter assignments like: x = 10 + 5;${NC}"

# Task 4: First and Follow Sets
echo -e "\n${GREEN}Testing Task 4: First and Follow Sets${NC}"
Tasks/Task4/ffsets tests/grammar.txt

# Task 5: LL(1) Parser
echo -e "\n${GREEN}Testing Task 5: LL(1) Parser${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task5/ll1parser tests/grammar.txt${NC}"
echo -e "${YELLOW}And enter an input string like: ab#${NC}"

# Task 6: Operator Precedence Parser
echo -e "\n${GREEN}Testing Task 6: Operator Precedence Parser${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task6/op_parser${NC}"
echo -e "${YELLOW}And enter expressions like: id+id*id${NC}"

# Task 7: Intermediate Code Generation
echo -e "\n${GREEN}Testing Task 7: Intermediate Code Generation${NC}"
echo -e "${YELLOW}Run this manually with: Tasks/Task7/icg${NC}"
echo -e "${YELLOW}And enter an expression like: a+b*c${NC}"

# Task 8: Loop Unrolling
echo -e "\n${GREEN}Testing Task 8: Loop Unrolling${NC}"
Tasks/Task8/loop_unroll tests/loop_test.c tests/loop_test_unrolled.c 4
echo -e "${YELLOW}Output written to tests/loop_test_unrolled.c${NC}"

# Task 9: Constant Propagation
echo -e "\n${GREEN}Testing Task 9: Constant Propagation${NC}"
Tasks/Task9/const_prop tests/const_test.c tests/const_test_optimized.c
echo -e "${YELLOW}Output written to tests/const_test_optimized.c${NC}"

echo -e "\n${GREEN}All tests completed!${NC}"