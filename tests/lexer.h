/* $Id: lexer.h,v 1.9 2024/07/15 20:31:49 leavens Exp $ */
#ifndef _LEXER_H
#define _LEXER_H
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
#include "parser_types.h"
#include "asm.tab.h"
#include "instruction.h"

// Have any error messages been printed?
extern bool errors_noted;

// The current input file
// extern FILE *yyin;
// The input line number of the current or next input line
// extern int yylineno;
// The value of the current token
// extern YYSTYPE yylval;

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
extern void lexer_init(char *fname);

// Return the next token in the input
extern int yylex();

// Return the name of the current file
extern const char *lexer_filename();

// Return the line number of the next token
extern unsigned int lexer_line();

// On standard output:
// Print a message about the file name of the lexer's input
// and then print a heading for the lexer's output.
extern void lexer_print_output_header();

// Print information about the token t to stdout
// followed by a newline
extern void lexer_print_token(int t, unsigned int tline,
			      const char *txt);

// On standard output, print each token
// using the format in lexer_print_token
extern void lexer_output();

// Requires: toknum is a token number (from asm.tab.h)
//           of an instruction
// Return the opcode corresponding to the given opcode token number
extern op_code lexer_token2opcode(int toknum);

// Return the function code corresponding to the given opcode token number
extern func_type lexer_token2func(int toknum);

// Requires: toknum is a token number (from asm.tab.h)
//           that corresponds to a system call instruction
// Return the system call code that corresponds to that token
extern syscall_type lexer_token2syscall_code(int toknum);

#endif
