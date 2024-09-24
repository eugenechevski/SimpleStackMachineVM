/* $Id: asm_unparser.h,v 1.9 2024/07/29 16:17:19 leavens Exp $ */
#ifndef _ASM_UNPARSER_H
#define _ASM_UNPARSER_H
#include <stdio.h>
#include "ast.h"

// Unparse prog, with output going to the file out
extern void unparseProgram(FILE *out, ast_program_t prog);

// Unparse the given AST, with output going to out
extern void unparseTextSection(FILE *out, ast_text_section_t ts);

// Unparse the given AST, with output going to out
extern void unparseEntryPoint(FILE *out, ast_addr_t ep);

// Unparse the given AST, with output going to out
extern void unparseAddr(FILE *out, ast_addr_t l);

// Unparse the given AST, with output going to out
extern void unparseAsmInstrs(FILE *out, ast_asm_instrs_t instrs);

// Unparse the given AST, with output going to out
extern void unparseAsmInstr(FILE *out, ast_asm_instr_t instr);

// Unparse the given AST, with output going to out
extern void unparseLabelOpt(FILE *out, ast_label_opt_t lopt);

// Unparse the given AST, with output going to out
extern void unparseInstr(FILE *out, ast_instr_t instr);

// Unparse the given AST, with output going to out
extern void unparseImmediateAddress(FILE *out, ast_addr_t addr);
    
// Unparse a register, returning a string for the register's name
extern const char *unparseReg(reg_num_type n);

// Unparse the given AST, with output going to out
extern void unparseDataSection(FILE *out, ast_data_section_t ds);

// Unparse the given AST, with output going to out
extern void unparseStaticDecls(FILE *out, ast_static_decls_t sds);

// Unparse the given AST, with output going to out
extern void unparseStaticDecl(FILE *out, ast_static_decl_t dcl);

// Unparse the data size declartion, with output going to out
void unparseDataSize(FILE *out, ast_static_decl_t dcl);

// Unparse the given AST, with output going to out
extern void unparseIdent(FILE *out, ast_ident_t id);

// Unparse the given AST, with output going to out
extern void unparseInitializer(FILE *out, ast_initializer_opt_t init);

// Unparse the given AST, with output going to out
extern void unparseStackSection(FILE *out, ast_stack_section_t ss);

#endif
