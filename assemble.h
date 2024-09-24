/* $Id: assemble.h,v 1.10 2024/07/28 22:01:51 leavens Exp $ */
#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H
#include <stdio.h>
#include "ast.h"
#include "bof.h"

// Generate code for prog, with output going to bf
extern void assembleProgram(BOFFILE bf, ast_program_t prog);

// Generate code for the given AST, with output going to bf
extern void assembleTextSection(BOFFILE bf, ast_text_section_t ts);

// Generate code for the given AST, with output going to bf
extern void assembleAsmInstrs(BOFFILE bf, ast_asm_instrs_t instrs);

// Generate code for the given AST, with output going to bf
extern void assembleAsmInstr(BOFFILE bf, ast_asm_instr_t instr);

// Generate code for the given AST, with output going to bf
extern void assembleInstr(BOFFILE bf, ast_instr_t instr);

// return the size (in words) of the data section's declarations
extern unsigned int assemble_dataSection_words(ast_data_section_t ds);

// return the size (in words) of the given static declarations
extern unsigned int assemble_staticDecls_words(ast_static_decls_t sds);

// return the size (in words) of the given static declaration
extern unsigned int assemble_staticDecl_words(ast_static_decl_t sd);

// Generate code for the given AST, with output going to bf
extern void assembleDataSection(BOFFILE bf, ast_data_section_t ds);

// Generate code for the given AST, with output going to bf
extern void assembleStaticDecls(BOFFILE bf, ast_static_decls_t sds);

// Generate code for the given AST, with output going to bf
extern void assembleStaticDecl(BOFFILE bf, ast_static_decl_t dcl);

#endif
