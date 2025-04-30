CC=gcc
CFLAGS=-Wall -std=c99
FLEX=flex
BISON=bison

all: task1 task2 task3a task3b task3c task3d task4 task5 task6 task7 task8 task9

# Task 1: Lexical Analyzer in C
task1: 
	$(CC) $(CFLAGS) -o Tasks/Task1/lexer Tasks/Task1/lexer.c

# Task 2: Lexical Analyzer using Lex
task2:
	cd Tasks/Task2 && $(FLEX) -o lexer.yy.c lexer.l
	$(CC) $(CFLAGS) -o Tasks/Task2/lexer Tasks/Task2/lexer.yy.c 

# Task 3a: Arithmetic Expression Recognizer
task3a:
	cd Tasks/Task3/3a && $(BISON) -d -y parser.y
	cd Tasks/Task3/3a && $(FLEX) -o lex.yy.c lexer.l
	$(CC) $(CFLAGS) -o Tasks/Task3/3a/parser Tasks/Task3/3a/y.tab.c Tasks/Task3/3a/lex.yy.c 

# Task 3b: Variable Recognizer
task3b:
	cd Tasks/Task3/3b && $(BISON) -d -y parser.y
	cd Tasks/Task3/3b && $(FLEX) -o lex.yy.c lexer.l
	$(CC) $(CFLAGS) -o Tasks/Task3/3b/parser Tasks/Task3/3b/y.tab.c Tasks/Task3/3b/lex.yy.c 

# Task 3c: Calculator using LEX and YACC
task3c:
	cd Tasks/Task3/3c && $(BISON) -d -y parser.y
	cd Tasks/Task3/3c && $(FLEX) -o lex.yy.c lexer.l
	$(CC) $(CFLAGS) -o Tasks/Task3/3c/calculator Tasks/Task3/3c/y.tab.c Tasks/Task3/3c/lex.yy.c  -lm

# Task 3d: Abstract Syntax Tree Generator
task3d:
	cd Tasks/Task3/3d && $(BISON) -d -y parser.y
	cd Tasks/Task3/3d && $(FLEX) -o lex.yy.c lexer.l
	$(CC) $(CFLAGS) -o Tasks/Task3/3d/ast_gen Tasks/Task3/3d/y.tab.c Tasks/Task3/3d/lex.yy.c 

# Task 4: First and Follow Sets
task4:
	$(CC) $(CFLAGS) -o Tasks/Task4/ffsets Tasks/Task4/FFSets.c

# Task 5: LL(1) Parser
task5:
	$(CC) $(CFLAGS) -o Tasks/Task5/ll1parser Tasks/Task5/LL1.c

# Task 6: Operator Precedence Parser
task6:
	$(CC) $(CFLAGS) -o Tasks/Task6/op_parser Tasks/Task6/precedence.c

# Task 7: Intermediate Code Generation
task7:
	$(CC) $(CFLAGS) -o Tasks/Task7/icg Tasks/Task7/intermediate.c

# Task 8: Loop Unrolling
task8:
	$(CC) $(CFLAGS) -o Tasks/Task8/loop_unroll Tasks/Task8/loop.c 

# Task 9: Constant Propagation
task9:
	$(CC) $(CFLAGS) -o Tasks/Task9/const_prop Tasks/Task9/propagation.c 
clean:
	rm -f Tasks/Task1/lexer
	rm -f Tasks/Task2/lexer Tasks/Task2/lexer.yy.c
	rm -f Tasks/Task3/3a/parser Tasks/Task3/3a/y.tab.c Tasks/Task3/3a/y.tab.h Tasks/Task3/3a/lex.yy.c
	rm -f Tasks/Task3/3b/parser Tasks/Task3/3b/y.tab.c Tasks/Task3/3b/y.tab.h Tasks/Task3/3b/lex.yy.c
	rm -f Tasks/Task3/3c/calculator Tasks/Task3/3c/y.tab.c Tasks/Task3/3c/y.tab.h Tasks/Task3/3c/lex.yy.c
	rm -f Tasks/Task3/3d/ast_gen Tasks/Task3/3d/y.tab.c Tasks/Task3/3d/y.tab.h Tasks/Task3/3d/lex.yy.c
	rm -f Tasks/Task4/ffsets
	rm -f Tasks/Task5/ll1parser
	rm -f Tasks/Task6/op_parser
	rm -f Tasks/Task7/icg
	rm -f Tasks/Task8/loop_unroll
	rm -f Tasks/Task9/const_prop

.PHONY: all task1 task2 task3a task3b task3c task3d task4 task5 task6 task7 task8 task9 clean