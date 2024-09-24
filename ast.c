/* $Id: ast.c,v 1.44 2024/08/29 00:06:21 leavens Exp $ */
#include <string.h>
#include <stdlib.h>
#include "utilities.h"
#include "ast.h"
#include "lexer.h"

// Return (a pointer to) the file location from an AST
const file_location *ast_file_loc(AST t) {
    return t.generic.file_loc;
}

// Return the filename from the AST t
const char *ast_filename(AST t) {
    return ast_file_loc(t)->filename;
}

// Return the line number from the AST t
unsigned int ast_line(AST t) {
    return ast_file_loc(t)->line;
}

// Return the type tag of the AST t
AST_type ast_type_tag(AST t) {
    return t.generic.type_tag;
}

// Return an AST for a program,
// which contains the given ASTs.
ast_program_t ast_program(ast_text_section_t textSec,
			  ast_data_section_t dataSec,
			  ast_stack_section_t stackSec)
{
    ast_program_t ret;
    ret.file_loc = textSec.file_loc;
    ret.type_tag = program_ast;
    ret.textSection = textSec;
    ret.dataSection = dataSec;
    ret.stackSection = stackSec;
    return ret;
}

// Return an AST for the text section
// with the given entry point and instructions.
ast_text_section_t ast_text_section(ast_token_t tok, ast_addr_t entryPoint,
				    ast_asm_instrs_t instrs)
{
    ast_text_section_t ret;
    ret.file_loc = file_location_copy(tok.file_loc);
    ret.type_tag = text_section_ast;
    ret.entryPoint = entryPoint;
    ret.instrs = instrs;
    return ret;
}

// Return an AST for an entry point
// that represents the label given
ast_addr_t ast_addr_label(ast_ident_t label)
{
    ast_addr_t ret;
    ret.file_loc = file_location_copy(label.file_loc);
    ret.type_tag = addr_ast;
    ret.address_defined = false;
    ret.label = label.name;
    ret.addr = 0;
    return ret;
}

// Return an AST for an entry point
// that represents the address given
ast_addr_t ast_entry_addr(ast_unsignednum_t addr)
{
    ast_addr_t ret;
    ret.file_loc = file_location_copy(addr.file_loc);
    ret.type_tag = addr_ast;
    ret.address_defined = true;
    ret.label = NULL;
    ret.addr = addr.value;
    return ret;
}

// Return an AST for an asm instr AST
// with the given label-opt and instruction
ast_asm_instr_t ast_asm_instr(ast_label_opt_t labelOpt, ast_instr_t instr)
{
    ast_asm_instr_t ret;
    ret.file_loc = labelOpt.file_loc;
    ret.type_tag = asm_instr_ast;
    ret.next = NULL;
    ret.label_opt = labelOpt;
    ret.instr = instr;
    return ret;
}

// Return an AST for a singleton asm instrs AST
// with the given instruction
ast_asm_instrs_t ast_asm_instrs_singleton(ast_asm_instr_t asminstr)
{
    ast_asm_instrs_t ret;
    ret.file_loc = asminstr.file_loc;
    ret.type_tag = asm_instrs_ast;
    ast_asm_instr_t *p = (ast_asm_instr_t *)malloc(sizeof(ast_asm_instr_t));
    if (p == NULL) {
	bail_with_error("Cannot allocate space for an asm_instr!");
    }
    *p = asminstr;
    p->next = NULL;
    ret.instrs = p;
    return ret;
}

// Return an AST made from adding instr to the end of lst
ast_asm_instrs_t ast_asm_instrs_add(ast_asm_instrs_t lst, ast_asm_instr_t asminstr)
{
    ast_asm_instrs_t ret = lst;
    ast_asm_instr_t *p = (ast_asm_instr_t *)malloc(sizeof(ast_asm_instr_t));
    if (p == NULL) {
	bail_with_error("Cannot allocate space for an asm_instr!");
    }
    *p = asminstr;
    p->next = NULL;
    // splice p onto the end of lst.instrs
    ast_asm_instr_t *last = ast_last_list_elem(lst.instrs);
    if (last == NULL) {
	ret.instrs = p;
    } else {
	last->next = p;
    }
    return ret;
}

// Requires: id.name != NULL.
// Return an AST for a label_opt AST
// with the given identifier
ast_label_opt_t ast_label_opt_label(ast_ident_t id)
{
    ast_label_opt_t ret;
    ret.file_loc = file_location_copy(id.file_loc);
    ret.type_tag = label_opt_ast;
    ret.name = id.name;
    return ret;
}

// Return an AST for a label_opt AST
// with the empty AST (t)
ast_label_opt_t ast_label_opt_empty(ast_empty_t t)
{
    ast_label_opt_t ret;
    ret.file_loc = file_location_copy(t.file_loc);
    ret.type_tag = label_opt_ast;
    ret.name = NULL;
    return ret;
}

// Return an immediate data holding a number
ast_immedData_t ast_immed_number(word_type n) {
    ast_immedData_t ret;
    ret.id_data_kind = id_number;
    ret.data.immed = n;
    return ret;
}

// Return an immediate data holding an unsigned int
ast_immedData_t ast_immed_unsigned(unsigned int u) {
    ast_immedData_t ret;
    ret.id_data_kind = id_unsigned;
    ret.data.uimmed = u;
    return ret;
}

// Requires: toknum is the token number of a system call operation.
// Return an immedidate data holding a system call code for t
extern ast_immedData_t ast_syscall_code_for(int toknum)
{
    ast_immedData_t ret;
    ret.id_data_kind = id_syscall_code;
    ret.data.syscall_code
	= (syscall_type) instruction_token2SyscallCode(toknum);
    return ret;
}

// Return an immediate data holding an address
ast_immedData_t ast_immed_addr(ast_addr_t a) {
    ast_immedData_t ret;
    ret.id_data_kind = id_addr;
    ret.data.addr = a;
    return ret;
}

// Return an immediate data that is nothing
ast_immedData_t ast_immed_none() {
    ast_immedData_t ret;
    ret.id_data_kind = id_empty;
    return ret;
}

// Return an instruction AST for a (no-argument) instruction
ast_instr_t ast_0arg_instr(ast_token_t op)
{
    ast_instr_t ret;
    ret.file_loc = file_location_copy(op.file_loc);
    ret.type_tag = instr_ast;
    ret.next = NULL;
    ret.opname = op.text;
    ret.opcode = lexer_token2opcode(op.toknum);
    assert(op.toknum != noopsym || ret.opcode == COMP_O);
    if (ret.opcode == COMP_O) {
	ret.itype = comp_instr_type;
	ret.func = lexer_token2func(op.toknum);
    } else if (ret.opcode == OTHC_O) {
	ret.itype = other_comp_instr_type;
	ret.func = lexer_token2func(op.toknum);
    } else {
	// the func code is not used with other instruction types
	ret.itype = jump_instr_type;
	ret.func = 0;
    }
    ret.regs_used = 0;
    ret.reg = 0;        // not really needed
    ret.offset = 0;     // not really needed
    ret.reg2 = 0;       // not really needed
    ret.offset2 = 0;    // not really needed
    ret.immed_data = ast_immed_none();
    return ret;
}
    
// Return an instruction AST
// for a (2-register) computational instruction
ast_instr_t ast_2reg_instr(ast_token_t op,
			   unsigned short rt, offset_type ot,
			   unsigned short rs, offset_type os,
			   func_type func)
{
    ast_instr_t ret;
    ret.file_loc = file_location_copy(op.file_loc);
    ret.type_tag = instr_ast;
    ret.next = NULL;
    ret.itype = comp_instr_type; // only this type has 2 registers & 2 offsets
    ret.opname = op.text;
    ret.opcode = lexer_token2opcode(op.toknum);
    ret.func = func;
    ret.regs_used = 2;
    ret.reg = rt;
    ret.offset = ot;
    ret.reg2 = rs;
    ret.offset2 = os;
    ret.immed_data = ast_immed_none();
    return ret;
}

// Return an AST for an instruction
// with the given information
ast_instr_t ast_1reg_instr(ast_token_t op, instr_type itype,
			   unsigned short num_regs_used,
			   unsigned short reg, offset_type offset,
			   func_type func, ast_immedData_t im)
{
    ast_instr_t ret;
    ret.file_loc = file_location_copy(op.file_loc);
    ret.type_tag = instr_ast;
    ret.next = NULL;
    ret.itype = itype;
    ret.opname = op.text;
    ret.opcode = lexer_token2opcode(op.toknum);
    ret.func = func;
    ret.regs_used = num_regs_used;
    switch (num_regs_used) {
    case 0:
	ret.reg = 0;
	ret.offset = 0;
	break;
    case 1:
	ret.reg = reg;
	ret.offset = offset;
	break;
    default:
	bail_with_error("ast_1reg_instr called with unknown number of registers!");
	break;
    }
    ret.immed_data = im;
    ret.reg2 = 0;    // reg2 is not used
    ret.offset2 = 0; // offset2 is not used
    return ret;
}

// Return an AST for the data section AST
// with the given list of static declarations.
ast_data_section_t ast_data_section(ast_token_t kw, unsigned int static_start,
				    ast_static_decls_t staticDecls)
{
    ast_data_section_t ret;
    ret.file_loc = file_location_copy(kw.file_loc);
    ret.type_tag = data_section_ast;
    ret.static_start_addr = static_start;
    ret.staticDecls = staticDecls;
    return ret;
}

// Return an AST for an empty list of static decls
ast_static_decls_t ast_static_decls_empty(ast_empty_t e)
{
    ast_static_decls_t ret;
    ret.file_loc = file_location_copy(e.file_loc);
    ret.type_tag = static_decls_ast;
    ret.decls = NULL;
    return ret;
}

// Return an AST for a list of static declarations
// with sd added to the end of sds
ast_static_decls_t ast_static_decls_add(ast_static_decls_t sds,
				    ast_static_decl_t sd)
{
    ast_static_decls_t ret = sds;
    ast_static_decl_t *p = (ast_static_decl_t *)malloc(sizeof(ast_static_decl_t));
    if (p == NULL) {
	bail_with_error("Cannot allocate space for a static_decl!");
    }
    *p = sd;
    p->next = NULL;
    // splice p onto the end of sds.decls
    ast_static_decl_t *last = ast_last_list_elem(sds.decls);
    if (last == NULL) {
	ret.decls = p;
    } else {
	last->next = p;
    }
    return ret;
}

// Return an AST for a data size delclarator
// for the given number of words
ast_data_size_t ast_data_size(ast_token_t kw, data_size_e dse,
			      unsigned short words)
{
    extern char *strdup(const char *s);
    ast_data_size_t ret;
    ret.file_loc = file_location_copy(kw.file_loc);
    ret.type_tag = data_size_ast;
    ret.dse = dse;
    ret.size_name = strdup(kw.text);
    ret.size_in_words = words;
    return ret;
}

// Requires: init.byte_size <= (ds.size_in_words * BYTES_PER_WORD).
// Return an AST for a static declaration
// with the given data size, identifier, and initializer.
// Note that the size is taken from the ds argument, not the initializer.
ast_static_decl_t ast_static_decl(ast_data_size_t ds,
			      ast_ident_t ident,
			      ast_initializer_opt_t init)
{
    ast_static_decl_t ret;
    ret.file_loc = file_location_copy(ds.file_loc);
    ret.type_tag = static_decl_ast;
    ret.next = NULL;
    ret.size_in_words = ds.size_in_words;
    if (init.kind != initzlr_k_none) {
	// give an error if the declared size and initializer don't match
	// TODO: move these checks to a static checking pass
	if (init.byte_size > (ds.size_in_words * BYTES_PER_WORD)) {
	    // assert(init.file_loc != NULL);
	    fprintf(stderr, "Initializer size (%u bytes) is too big for declared data size (%u bytes)!\n",
		    init.byte_size, (ds.size_in_words * BYTES_PER_WORD));
	    if (init.str_value != NULL) {
		fprintf(stderr, "Note: initializer string is %s\n", init.str_value);
	    }
	}
    }
    ret.dse = ds.dse;
    switch (ds.dse) {
    case ds_word:
	if (init.kind == initzlr_k_string) {
	    bail_with_prog_error(*(init.file_loc),
				 "Using a string initializer for word data!");
	}
	break;
    case ds_char:
	if (init.kind == initzlr_k_string) {
	    bail_with_prog_error(*(init.file_loc),
				 "Using a string initializer for char data!");
	} else if (init.kind == initzlr_k_number) {
	    bail_with_prog_error(*(init.file_loc),
				 "Using a word initializer for char data!");
	}
	break;
    case ds_string:
	if (init.kind != initzlr_k_string) {
	    bail_with_prog_error(*(init.file_loc),
				 "String data requires a string initializer!");
	}
	break;
    }
    ret.size_name = ds.size_name;
    ret.ident = ident;
    ret.initializer = init;
    return ret;
}

// Return an AST for an initializer with the given number
ast_initializer_opt_t ast_initializer_number(ast_token_t eqs,
					     word_type value)
{
    ast_initializer_opt_t ret;
    ret.file_loc = file_location_copy(eqs.file_loc);
    ret.type_tag = initializer_opt_ast;
    ret.kind = initzlr_k_number;
    ret.byte_size = BYTES_PER_WORD;
    ret.num_value = value;
    return ret;
}

// Return an AST for an initializer with the given char literal
ast_initializer_opt_t ast_initializer_char(ast_token_t eqs,
					   char c)
{
    ast_initializer_opt_t ret;
    ret.file_loc = file_location_copy(eqs.file_loc);
    ret.type_tag = initializer_opt_ast;
    ret.kind = initzlr_k_char;
    ret.byte_size = BYTES_PER_WORD; // make it word aligned
    ret.num_value = c;
    return ret;
}

// Return an AST for an initializer with the given string literal
ast_initializer_opt_t ast_initializer_string(ast_token_t eqs,
					     const char *s)
{
    ast_initializer_opt_t ret;
    ret.file_loc = file_location_copy(eqs.file_loc);
    ret.type_tag = initializer_opt_ast;
    ret.kind = initzlr_k_string;
    ret.byte_size = strlen(s) + 1; // add 1 for null char at end
    ret.str_value = s;
    return ret;
}


// Return an AST for an empty initializer 
ast_initializer_opt_t ast_initializer_empty(ast_empty_t e)
{
    ast_initializer_opt_t ret;
    ret.file_loc = file_location_copy(e.file_loc);
    ret.type_tag = initializer_opt_ast;
    ret.kind = initzlr_k_none;
    // the other fields should not be used when the kind is as above
    return ret;
}

// Return an AST for empty found in the file named fn, on line ln
ast_empty_t ast_empty(const char *fn, unsigned int ln)
{
    ast_empty_t ret;
    ret.file_loc = file_location_make(fn, ln);
    ret.type_tag = empty_ast;
    return ret;
}

// Return an AST for a stack section AST
// with the given keyword and stack bottom address.
ast_stack_section_t ast_stack_section(ast_token_t kw,
				      unsigned int stack_bottom)
{
    ast_stack_section_t ret;
    ret.file_loc = file_location_copy(kw.file_loc);
    ret.type_tag = stack_section_ast;
    ret.stack_bottom_addr = stack_bottom;
    return ret;
}

// Return an AST for an identifier
// found in the file named fn, on line ln, with the given name.
ast_ident_t ast_ident(const char *fn, unsigned int ln, const char *name)
{
    ast_ident_t ret;
    ret.file_loc = file_location_make(fn, ln);
    ret.type_tag = ident_ast;
    ret.name = name;
    return ret;
}

// Return an AST for a (signed) number with the given value
ast_number_t ast_number(ast_token_t sgn, word_type value)
{
    ast_number_t ret;
    ret.file_loc = file_location_copy(sgn.file_loc);
    ret.type_tag = number_ast;
    ret.value = value;
    return ret;
}

// Return an AST for an (unsigned) number
// with the given value
ast_unsignednum_t ast_unsignednum(const char *fn, unsigned int ln,
			          unsigned int value)
{
    ast_unsignednum_t ret;
    ret.file_loc = file_location_make(fn, ln);
    ret.type_tag = unsignednum_ast;
    ret.value = value;
    return ret;
}

// Return an AST for a token
ast_token_t ast_token(const char *fn, unsigned int ln, int token_code)
{
    ast_token_t ret;
    ret.file_loc = file_location_make(fn, ln);
    ret.type_tag = token_ast;
    ret.toknum = token_code;
    return ret;
}    

// Requires: lst is a pointer to a non-circular linked list with next pointers
//           in the same relative position as ast_generic_t
// Return a pointer to the last element in lst.
// This only returns NULL if lst == NULL.
void *ast_last_list_elem(void *lst)
{
    if (lst == NULL) {
	return lst;
    }
    // assert lst is not NULL
    void *prev = NULL;
    while (lst != NULL) {
	prev = lst;
	lst = ((ast_generic_t *)lst)->next;
    }
    // here lst == NULL;
    return prev;
}

// Requires: lst is a pointer to a non-circular linked list with next pointers
//           in the same relative position as ast_generic_t
// Return the number of elements in the linked list lst
extern int ast_list_length(void *lst)
{
    int ret = 0;
    ast_generic_t *p = (ast_generic_t *) lst;
    while (p != NULL) {
	p = p->next;
	ret++;
    }
    return ret;
}
