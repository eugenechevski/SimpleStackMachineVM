// $Id: pass1.h,v 1.4 2024/07/26 12:44:46 leavens Exp $
#ifndef _PASS1_H
#define _PASS1_H
#include <stdio.h>
#include "ast.h"
#include "machine_types.h"

// Build the symbol table and check for duplicate declarations in the given AST
extern void pass1(ast_program_t progast);

// Check the given AST and put its declarations in the symbol table
extern void pass1TextSection(ast_text_section_t ts);

// Check the given AST and put its declarations in the symbol table
extern void pass1AsmInstrs(ast_asm_instrs_t instrs);

// Check the given AST and put its declarations in the symbol table
extern void pass1AsmInstr(ast_asm_instr_t instr, address_type count);

// Check the given AST and put its declarations in the symbol table
extern void pass1LabelOpt(ast_label_opt_t lopt, address_type count);

// Check the given AST and put its declarations in the symbol table
extern void pass1DataSection(ast_data_section_t ds);

// Check the given AST and put its declarations in the symbol table
extern void pass1StaticDecls(ast_static_decls_t sds);

// Check the given AST and put its declarations in the symbol table
extern void pass1StaticDecl(ast_static_decl_t dcl, address_type offset);

// Check the given AST and put its declarations in the symbol table
extern void pass1DeclareIdent(ast_ident_t id, address_type offset);

// Requires: out is a character file (like stdout)
//           that is open for writing
// Print debugging information about the symbol table on out
void pass1_print(FILE *out);

#endif
