/* $Id: assemble.c,v 1.36 2024/08/29 21:58:39 leavens Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "utilities.h"
#include "assemble.h"
#include "bof.h"
#include "symtab.h"
#include "id_attrs_assoc.h"
#include "regname.h"

// Return the address associated with the addr l
static address_type addr2address(ast_addr_t addr)
{
    address_type ret = addr.addr;
    if (addr.address_defined) {
	return ret;
    } else {
	// the address is not defined yet
	id_attrs_assoc *ida = symtab_lookup(addr.label);
	if (ida == NULL) {
	    bail_with_prog_error(*(addr.file_loc),
				 "Label \"%s\" was never defined!",
				 addr.label);
	}
	return ida->addr;
    }
}

// Assemble the code for prog, with output going to bf
void assembleProgram(BOFFILE bf, ast_program_t prog)
{
    BOFHeader bh;
    bof_write_magic_to_header(&bh);
    bh.text_start_address = addr2address(prog.textSection.entryPoint);
    bh.text_length = ast_list_length(prog.textSection.instrs.instrs);
    bh.data_start_address = prog.dataSection.static_start_addr;
    // have to write the header first, so need to know the true size
    // of the data section before writing the header
    bh.data_length = assemble_dataSection_words(prog.dataSection);
    bh.stack_bottom_addr = prog.stackSection.stack_bottom_addr;
    bof_write_header(bf, bh);
    assembleTextSection(bf, prog.textSection);
    assembleDataSection(bf, prog.dataSection);
    // nothing to do for the stack section, as it's all in the header
}

// Assemble the code for the given AST, with output going to bf
void assembleTextSection(BOFFILE bf, ast_text_section_t ts)
{
    assembleAsmInstrs(bf, ts.instrs);
}

// Assemble the code for the given AST, with output going to bf
void assembleAsmInstrs(BOFFILE bf, ast_asm_instrs_t instrs)
{
    ast_asm_instr_t *ip = instrs.instrs;
    while (ip != NULL) {
	assembleAsmInstr(bf, *ip);
	ip = ip->next;
    }
}

// Assemble the code for the given AST, with output going to bf
void assembleAsmInstr(BOFFILE bf, ast_asm_instr_t instr)
{
    assembleInstr(bf, instr.instr);
}

// Return the value of the immedidate data AST immed
static unsigned int immedData_value(ast_immedData_t immed)
{
    int ret = 0;
    switch (immed.id_data_kind) {
    case id_number:
	ret = immed.data.immed;
	break;
    case id_syscall_code:
	ret = immed.data.syscall_code;
	break;
    case id_unsigned:
	ret = immed.data.uimmed;
	break;
    case id_addr:
	return addr2address(immed.data.addr);
	break;
    case id_empty:
	ret = 0;
	break;
    default:
	bail_with_error("Unknown immed_data_kind_t (%d) in immedData_value",
			immed.id_data_kind);
	break;
    }
    return ret;
}

// Assemble the code for the given AST, with output going to bf
void assembleInstr(BOFFILE bf, ast_instr_t instr)
{
    switch (instr.itype) {
    case comp_instr_type:
	comp_instr_t ci;
	ci.op = instr.opcode;
	assert(ci.op == COMP_O);
	ci.rt = instr.reg;
	ci.ot = instr.offset;
	ci.rs = instr.reg2;
	ci.os = instr.offset2;
	ci.func = instr.func;
	instruction_write_compInstr(bf, ci);
	break;
    case other_comp_instr_type:
	other_comp_instr_t oci;
	oci.op = instr.opcode;
	assert(oci.op == OTHC_O);
	oci.reg = instr.reg;
	oci.offset = instr.offset;
	oci.arg = instr.immed_data.data.uimmed;
	oci.func = instr.func;
	instruction_write_otherCompInstr(bf, oci);
	break;
    case syscall_instr_type:
	syscall_instr_t si;
	si.op = OTHC_O;
	si.reg = instr.reg;
	si.offset = instr.offset;
	si.code = (syscall_type) immedData_value(instr.immed_data);
	si.func = SYS_F;
	instruction_write_syscallInstr(bf, si);
	break;
    case immed_instr_type:
	switch (instr.opcode) {
	case ANDI_O: case BORI_O: case NORI_O: case XORI_O:
	    uimmed_instr_t ui;
	    ui.op = instr.opcode;
	    ui.reg = instr.reg;
	    ui.offset = instr.offset;
	    ui.uimmed = immedData_value(instr.immed_data);
	    instruction_write_uimmedInstr(bf, ui);
	    break;
	default:
	    immed_instr_t ii;
	    ii.op = instr.opcode;
	    ii.reg = instr.reg;
	    ii.offset = instr.offset;
	    ii.immed = (immediate_type) immedData_value(instr.immed_data);
	    instruction_write_immedInstr(bf, ii);
	    break;
	}
	break;
    case jump_instr_type:
	jump_instr_t ji;
	ji.op = instr.opcode;
	ji.addr = (address_type) immedData_value(instr.immed_data);
	instruction_write_jumpInstr(bf, ji);
	break;
    default:
	bail_with_error("Bad instr_type in assembleInstr (%d)!", instr.itype);
	break;
    }
}

// return the size (in words) of the data section's declarations
unsigned int assemble_dataSection_words(ast_data_section_t ds)
{
    return assemble_staticDecls_words(ds.staticDecls);
}

// return the size (in words) of the given static declarations
unsigned int assemble_staticDecls_words(ast_static_decls_t sds)
{
    int ret = 0;
    ast_static_decl_t *dcl = sds.decls;
    while (dcl != NULL) {
	ret += assemble_staticDecl_words(*dcl);
	dcl = dcl->next;
    }
    return ret;
}

// return the size (in words) of the given static declarations
unsigned int assemble_staticDecl_words(ast_static_decl_t sd)
{
    return sd.size_in_words;
}

// Assemble the code for the given AST, with output going to bf
void assembleDataSection(BOFFILE bf, ast_data_section_t ds)
{
    assembleStaticDecls(bf, ds.staticDecls);
}

// Assemble the code for the given AST, with output going to bf
void assembleStaticDecls(BOFFILE bf, ast_static_decls_t sds)
{
    ast_static_decl_t *dcl = sds.decls;
    while (dcl != NULL) {
	assembleStaticDecl(bf, *dcl);
	dcl = dcl->next;
    }
}

// Assemble the code for the given AST, with output going to bf
void assembleStaticDecl(BOFFILE bf, ast_static_decl_t dcl)
{
    switch (dcl.initializer.kind) {
    case initzlr_k_number: case initzlr_k_char:
	bof_write_word(bf, dcl.initializer.num_value);
	break;
    case initzlr_k_string:
	bof_write_bytes(bf, dcl.initializer.byte_size,
			dcl.initializer.str_value);
	// pad to word-align the space for the string
	int padding_needed = ((dcl.size_in_words * BYTES_PER_WORD)
			      - dcl.initializer.byte_size);
	assert(padding_needed >= 0);
	assert(((dcl.initializer.byte_size + padding_needed)
		% BYTES_PER_WORD) == 0);
	int zeros[2] = {0, 0};
	for (int i = 0; i < padding_needed; i++) {
	    bof_write_bytes(bf, 1, zeros);
	}
	break;
    case initzlr_k_none:
	for (int i = 0; i < (dcl.size_in_words); i++) {
	    bof_write_word(bf, 0);
	}
	break;
    default:
	bail_with_error("Unknown declaration kind (%d) in assembleStaticDecl!",
			dcl.initializer.kind);
	break;
    }
}
