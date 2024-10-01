/* $Id: ast.h,v 1.35 2024/09/25 15:45:15 leavens Exp $ */
#ifndef _AST_H
#define _AST_H
#include <stdbool.h>
#include "machine_types.h"
#include "file_location.h"
#include "instruction.h"

// types of ASTs (type tags)
typedef enum {
    program_ast, text_section_ast, addr_ast, asm_instr_ast,
    asm_instrs_ast, label_opt_ast, empty_ast, instr_ast,
    data_section_ast, data_size_ast,
    static_decls_ast, static_decl_ast, initializer_opt_ast,
    stack_section_ast, ident_ast, number_ast, unsignednum_ast,
    reg_ast, token_ast, char_literal_ast, string_literal_ast
} AST_type;

// forward declaration, so can use the type AST* below
typedef union AST_u AST;

// The following types for structs named N_t
// are used in the declaration of the AST_u union below.
// The struct N_t is the type of information kept in the AST
// that is related to the nonterminal N in the abstract syntax.

// The generic struct type (ast_generic_t) has the fields that
// should be in all alternatives of the AST union type (AST_u)
// (although the next field is only needed for those that can be in lists).
typedef struct {
    file_location *file_loc;
    AST_type type_tag; // says what field of the union is active
    void *next; // for lists
} ast_generic_t;

// empty ::=
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
} ast_empty_t;

// label ::= ident
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *name;
} ast_ident_t;

// (signed) numbers (and offsets)
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    word_type value;
} ast_number_t;

// unsigned numbers
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *text;
    uword_type value;
} ast_unsignednum_t;

// registers
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *text;
    unsigned short number;
} ast_reg_t;

typedef enum { ds_word, ds_char, ds_string } data_size_e;

// dataSize ::= WORD | CHAR | STRING [ <number> ]
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    data_size_e dse;
    const char *size_name;
    unsigned short size_in_words;
} ast_data_size_t;

// tokens as ASTs
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *text;
    int toknum;
} ast_token_t;

// char literals as ASTs
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    char value;
} ast_charliteral_t;

// string literals as ASTs
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *pointer;
} ast_stringliteral_t;


// label-opt ::= label | empty
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *name;  // NULL if there was no label
} ast_label_opt_t;

// addr ::= label | unsigned-number
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    bool address_defined; // has this address gotten a value?
    const char *label; // NULL if address_defined (is true)
    address_type addr; // 0 if !address_defined
} ast_addr_t;

// kinds of immediate data
typedef enum { id_number, id_unsigned, id_addr, id_syscall_code, id_empty
} immed_data_kind_t;

// immed-data ::= number | unsignednum | syscall-code | addr | empty
typedef struct {
    immed_data_kind_t id_data_kind;
    union {
	immediate_type immed; // immediate arguments (not offsets!)
	shift_type uimmed; // shifts
	syscall_type syscall_code;
	ast_addr_t addr;
    } data;
} ast_immedData_t;

// instr ::= no-arg-instr | two-reg-comp-instr | no-target-offset-instr
//         | no-source-offset-instr | one-reg-arg-instr | one-reg-offset-instr
//         | shift-instr | arg-only-instr | immed-arith-instr
//         | immed-bool-astr | branch-test-instr | jump-instr | syscall-instr
typedef struct ast_instr_s {
    file_location *file_loc;
    AST_type type_tag;
    struct ast_instr_s * next;
    instr_type itype;
    const char *opname;
    op_code opcode;
    func_type func; // function code for computational instructions
    unsigned short regs_used;
    reg_num_type reg;
    offset_type offset;
    reg_num_type reg2;  // if there are two of registers
    offset_type offset2;
    ast_immedData_t immed_data; // includes arg (the number alternative)
} ast_instr_t;

// asmInstr ::= label-opt instr
typedef struct asm_instr_s {
    file_location *file_loc;
    AST_type type_tag;
    struct asm_instr_s *next;  // for lists
    ast_label_opt_t label_opt;
    ast_instr_t instr;
} ast_asm_instr_t;

// asmInstrs ::= asmInstr | asmInstrs asmInstr
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    ast_asm_instr_t *instrs;
} ast_asm_instrs_t;

// initializer kinds
typedef enum {initzlr_k_number, initzlr_k_char, initzlr_k_string,
	      initzlr_k_none
             } initzlr_k;

// initializer-opt ::= initializer | empty
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    initzlr_k kind;
    word_type num_value; // for numbers and char-literals
    word_type byte_size; // for strings
    const char *str_value; // for strings
} ast_initializer_opt_t;

// staticDecl ::= dataSize ident initializer-opt
typedef struct static_decl_s {
    file_location *file_loc;
    AST_type type_tag;
    struct static_decl_s *next;  // for lists
    unsigned short size_in_words;
    data_size_e dse;
    const char *size_name;
    ast_ident_t ident;
    ast_initializer_opt_t initializer;
} ast_static_decl_t;

// staticDecls ::= staticDecl*
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    ast_static_decl_t *decls;
} ast_static_decls_t;

// text-section ::= entry-point asmInstr*
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    ast_addr_t entryPoint;
    ast_asm_instrs_t instrs;
} ast_text_section_t;

// data-section ::= static-decl*
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    address_type static_start_addr;
    ast_static_decls_t staticDecls;
} ast_data_section_t;

// stack-section ::= .stack stack-bottom-addr
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    address_type stack_bottom_addr;
} ast_stack_section_t;

// program ::= text-section data-section
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    ast_text_section_t textSection;
    ast_data_section_t dataSection;
    ast_stack_section_t stackSection;
} ast_program_t;

// The actual AST definition:
typedef union AST_u {
    ast_generic_t generic;
    ast_program_t program;
    ast_text_section_t text_section;
    ast_addr_t addr;
    ast_asm_instrs_t asm_instrs;
    ast_asm_instr_t asm_instr;
    ast_label_opt_t label_opt;
    ast_empty_t empty;
    ast_instr_t instr;
    ast_data_section_t data_section;
    ast_static_decls_t static_decls;
    ast_static_decl_t static_decl;
    ast_data_size_t data_size;
    ast_initializer_opt_t initializer;
    ast_stack_section_t stack_section;
    ast_ident_t ident;
    ast_number_t number;
    ast_unsignednum_t unsignednum;
    ast_immedData_t immed;
    ast_reg_t reg;
    ast_token_t token;
    ast_charliteral_t charlit;
    ast_stringliteral_t stringlit;
} AST;

// Return (a pointer to) the file location from an AST
extern const file_location *ast_file_loc(AST t);

// Return the filename from the AST t
extern const char *ast_filename(AST t);

// Return the line number from the AST t
extern unsigned int ast_line(AST t);

// Return the type tag of the AST t
extern AST_type ast_type_tag(AST t);

// Return an AST for a program,
// which contains the given ASTs.
extern ast_program_t ast_program(ast_text_section_t textSec,
				 ast_data_section_t dataSec,
				 ast_stack_section_t stackSec);

// Return an AST for the text section
// with the given entry point and instructions.
extern ast_text_section_t ast_text_section(ast_token_t tok, ast_addr_t entryPoint,
					   ast_asm_instrs_t instrs);

// Return an AST for an entry point
// that represents the label given
extern ast_addr_t ast_addr_label(ast_ident_t label);

// Return an AST for an entry point
// that represents the address given
extern ast_addr_t ast_entry_addr(ast_unsignednum_t addr);

// Return an AST for an asm instr AST
// with the given label and instruction
extern ast_asm_instr_t ast_asm_instr(ast_label_opt_t label, ast_instr_t instr);

// Return an AST for a singleton asm instrs AST
// with the given instruction
extern ast_asm_instrs_t ast_asm_instrs_singleton(ast_asm_instr_t asminstr);

// Return an AST made from adding the given asm instr to the end of lst
extern ast_asm_instrs_t ast_asm_instrs_add(ast_asm_instrs_t lst, ast_asm_instr_t asminstr);

// Requires: id.name != NULL.
// Return an AST for a label_opt AST
// with the given identifier
extern ast_label_opt_t ast_label_opt_label(ast_ident_t id);

// Return an AST for a label_opt AST
// with the empty AST (t)
extern ast_label_opt_t ast_label_opt_empty(ast_empty_t t);

// Return an immediate data holding a number
extern ast_immedData_t ast_immed_number(word_type n);

// Return an immediate data holding an unsigned int
extern ast_immedData_t ast_immed_unsigned(unsigned int u);

// Requires: toknum is the token number of a system call operation.
// Return an immedidate data holding a system call code for t
extern ast_immedData_t ast_syscall_code_for(int toknum);

// Return an immediate data holding an address
extern ast_immedData_t ast_immed_addr(ast_addr_t a);

// Return an immediate data that is nothing
extern ast_immedData_t ast_immed_none();

// Return an instruction AST for a (no-argument) instruction
extern ast_instr_t ast_0arg_instr(ast_token_t op);

// Return an instruction AST
// for a (2-register) computational instruction
extern ast_instr_t ast_2reg_instr(ast_token_t op,
				  unsigned short rt, offset_type ot,
				  unsigned short rs, offset_type os,
				  func_type func);

// Return an AST for an instruction
// with the given information
extern ast_instr_t ast_1reg_instr(ast_token_t op, instr_type itype,
				  unsigned short num_regs_used,
				  unsigned short reg, offset_type offset,
				  func_type func, ast_immedData_t im);

// Return an AST for the data section AST
// with the given list of static declarations.
extern ast_data_section_t ast_data_section(ast_token_t kw, unsigned int static_start, ast_static_decls_t staticDecls);

// Return an AST for an empty list of static declarations
extern ast_static_decls_t ast_static_decls_empty(ast_empty_t e);

// Return an AST for a list of static declarations
// with sd added to the end of sds
extern ast_static_decls_t ast_static_decls_add(ast_static_decls_t sds,
					   ast_static_decl_t sd);

// Return an AST for a data size delclarator
// for the given number of words
extern ast_data_size_t ast_data_size(ast_token_t kw, data_size_e dse,
				     unsigned short words);

// Requires: init.byte_size <= (ds.size_in_words * BYTES_PER_WORD).
// Return an AST for a static declaration
// with the given data size, identifier, and initializer.
// Note that the size is taken from the ds argument, not the initializer.
extern ast_static_decl_t ast_static_decl(ast_data_size_t ds,
					 ast_ident_t ident,
					 ast_initializer_opt_t init);

// Return an AST for an initializer with the given number
extern ast_initializer_opt_t ast_initializer_number(ast_token_t eqs,
						    word_type value);

// Return an AST for an initializer with the given char literal
extern ast_initializer_opt_t ast_initializer_char(ast_token_t eqs,
						    char c);

// Return an AST for an initializer with the given string literal
extern ast_initializer_opt_t ast_initializer_string(ast_token_t eqs,
						    const char *s);

// Return an AST for an empty initializer 
extern ast_initializer_opt_t ast_initializer_empty(ast_empty_t e);

// Return an AST for empty found in the file named fn, on line ln
extern ast_empty_t ast_empty(const char *fn, unsigned int ln);

// Return an AST for a stack section AST
// with the given keyword and stack bottom address.
extern ast_stack_section_t ast_stack_section(ast_token_t kw,
					     unsigned int stack_bottom);

// found in the file named fn, on line ln, with the given name.
extern ast_ident_t ast_ident(const char *fn, unsigned int ln, const char *name);

// Return an AST for a (signed) number with the given value
extern ast_number_t ast_number(ast_token_t sgn, word_type value);

// Return an AST for a (signed) number with the given value
extern ast_unsignednum_t ast_unsigned_number(const char *fn, unsigned int ln,
					 unsigned int value);

// Return an AST for a token
extern ast_token_t ast_token(const char *fn, unsigned int ln, int token_code);

// Requires: lst is a pointer to a non-circular linked list with next pointers
//           in the same relative position as ast_generic_t
// Return a pointer to the last element in lst.
// This only returns NULL if lst == NULL.
extern void *ast_last_list_elem(void *lst);

// Requires: lst is a pointer to a non-circular linked list with next pointers
//           in the same relative position as ast_generic_t
// Return the number of elements in the linked list lst
extern int ast_list_length(void *lst);

#endif
