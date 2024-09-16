/* $Id: asm_unparser.c,v 1.30 2024/08/28 21:25:00 leavens Exp $ */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "utilities.h"
#include "char_utilities.h"
#include "regname.h"
#include "asm_unparser.h"

// Note: newlines and indentation are only printed
// by a function which deals with a grammar that sensibly needs one printed,
// as this makes them those functions more reusable.

// Unparse prog, with output going to the file out
void unparseProgram(FILE *out, ast_program_t prog)
{
    unparseTextSection(out, prog.textSection);
    unparseDataSection(out, prog.dataSection);
    unparseStackSection(out, prog.stackSection);
    fprintf(out, ".end");
    newline(out);
}

// Unparse the given AST, with output going to out
void unparseTextSection(FILE *out, ast_text_section_t ts)
{
    fprintf(out, ".text ");
    unparseEntryPoint(out, ts.entryPoint);
    newline(out);
    unparseAsmInstrs(out, ts.instrs);
}

// Unparse the given AST, with output going to out
void unparseEntryPoint(FILE *out, ast_addr_t ep)
{
    unparseAddr(out, ep);
}

// Unparse the given AST, with output going to out
void unparseAddr(FILE *out, ast_addr_t addr)
{
    if (addr.label != NULL) {
	fprintf(out, "%s", addr.label);
    } else {
	fprintf(out, "%u", addr.addr);
    }
}

// Unparse the given AST, with output going to out
void unparseAsmInstrs(FILE *out, ast_asm_instrs_t instrs)
{
    ast_asm_instr_t *ip = instrs.instrs;
    while (ip != NULL) {
	unparseAsmInstr(out, *ip);
	ip = ip->next;
    }
}

// Unparse the given AST, with output going to out
void unparseAsmInstr(FILE *out, ast_asm_instr_t instr)
{
    unparseLabelOpt(out, instr.label_opt);
    unparseInstr(out, instr.instr);
    newline(out);
}

// Unparse the given AST, with output going to out
void unparseLabelOpt(FILE *out, ast_label_opt_t lopt)
{
    if (lopt.name == NULL) {
	fprintf(out, "\t");
    } else {
	fprintf(out, "%s:\t", lopt.name);
    }
}

// Unparse the given AST, with output going to out
void unparseInstr(FILE *out, ast_instr_t instr)
{
    fprintf(out, "%s ", instr.opname);

    // print any registers used
    switch (instr.opcode) {
    case COMP_O:
	switch (instr.func) {
	case NOP_F:
	    // no arguments, so nothing to print!
	    break;
	case LWR_F: 
	    fprintf(out, "%s, %s, %hd",
		    unparseReg(instr.reg), unparseReg(instr.reg2),
		    instr.offset2);
	    break;
	case SWR_F: 
	    fprintf(out, "%s, %hd, %s",
		    unparseReg(instr.reg), instr.offset,
		    unparseReg(instr.reg2));
	    break;
	default:    // all the rest of these instructions
	    fprintf(out, "%s, %hd, %s, %hd",
		    unparseReg(instr.reg), instr.offset,
		    unparseReg(instr.reg2), instr.offset2);
	    break;
	}
	break;
    case OTHC_O:
	switch (instr.func) {
	case LIT_F:
	    fprintf(out, "%s, %hd, %hd",
		    unparseReg(instr.reg), instr.offset,
		    instr.immed_data.data.immed);
	    break;
	case ARI_F: case SRI_F: case MUL_F: case DIV_F:
	case CFHI_F: case CFLO_F: case JMP_F: case CSI_F:
	    fprintf(out, "%s, %hd",
		    unparseReg(instr.reg), instr.offset);
	    break;
	case SLL_F: case SRL_F:
	    fprintf(out, "%s, %hd, %hu",
		    unparseReg(instr.reg), instr.offset,
		    instr.immed_data.data.uimmed);
	    break;
	case JREL_F:
	    fprintf(out, "%hd", instr.immed_data.data.immed);
	    break;
	case SYS_F:
	    switch (instr.immed_data.data.syscall_code) {
	    case exit_sc:
		fprintf(out, "%hd", instr.offset);
		break;
	    case print_str_sc: case print_char_sc: case read_char_sc:
		fprintf(out, "%s, %hd", unparseReg(instr.reg), instr.offset);
		break;
	    case start_tracing_sc: case stop_tracing_sc:
		// no arguments!
		break;
	    default:
		bail_with_error("Unknown syscall_code in SYS_F case of unparseInstr!");
		break;
	    }
	    break;
	default:
	    bail_with_error("Unknown other computational opcode in unparseInstr %d", instr.opcode);
	    break;
	}
	break;
    case ADDI_O:
	fprintf(out, "%s, %hd, %hd",
		unparseReg(instr.reg), instr.offset, instr.immed_data.data.immed);
	break;
    case ANDI_O: case BORI_O: case NORI_O: case XORI_O:
	fprintf(out, "%s, %hd, %hx",
		unparseReg(instr.reg), instr.offset, instr.immed_data.data.uimmed);
	break;
    case BEQ_O: case BGEZ_O: case BGTZ_O: case BLEZ_O: case BLTZ_O: case BNE_O:
	fprintf(out, "%s, %hd, %hd",
		unparseReg(instr.reg), instr.offset, instr.immed_data.data.immed);
	break;
    case JMPA_O: case CALL_O:
	unparseImmediateAddress(out, instr.immed_data.data.addr);
	break;
    case RTN_O:
	// no arguments, so nothing to print!
	break;
    default:
	bail_with_error("Unknown opcode (%d) in unparseInstr!", instr.opcode);
	break;
    }
}

// Unparse the given AST, with output going to out
void unparseImmediateAddress(FILE *out, ast_addr_t addr)
{
    if (addr.address_defined) {
	fprintf(out, "%u", addr.addr);
    } else {
	fprintf(out, "%s", addr.label);
    }
}

// Unparse a register, returning a string for the register's name
const char *unparseReg(reg_num_type n)
{
    if (0 <= n && n < NUM_REGISTERS) {
	return regname_get(n);
    } else {
	bail_with_error("Bad register number (%d)!", n);
	return NULL;  // never happens
    }
}

// Unparse the given AST, with output going to out
void unparseDataSection(FILE *out, ast_data_section_t ds)
{
    fprintf(out, ".data %u", ds.static_start_addr);
    newline(out);
    unparseStaticDecls(out, ds.staticDecls);
}

// Unparse the given AST, with output going to out
void unparseStaticDecls(FILE *out, ast_static_decls_t sds)
{
    ast_static_decl_t *dcl = sds.decls;
    while (dcl != NULL) {
	unparseStaticDecl(out, *dcl);
	dcl = dcl->next;
    }
}

// Unparse the given AST, with output going to out
void unparseStaticDecl(FILE *out, ast_static_decl_t dcl)
{
    unparseDataSize(out, dcl);
    fprintf(out, "\t");
    unparseIdent(out, dcl.ident);
    fprintf(out, " ");
    unparseInitializer(out, dcl.initializer);
    newline(out);
}

// Unparse the data size declaration, with output going to out
void unparseDataSize(FILE *out, ast_static_decl_t dcl)
{
    fprintf(out, "%s ", dcl.size_name);
    if (dcl.dse == ds_string) {
	fprintf(out, "[%u]", dcl.size_in_words);
    }
}

// Unparse the given AST, with output going to out
void unparseIdent(FILE *out, ast_ident_t id)
{
    fprintf(out, "%s", id.name);
}


// Unparse the given AST, with output going to out
void unparseInitializer(FILE *out, ast_initializer_opt_t init)
{
    switch (init.kind) {
    case initzlr_k_number:
	fprintf(out, "= %d", init.num_value);
	break;
    case initzlr_k_char:
	fprintf(out, "= '%s'", char_utilities_unescape_char(init.num_value));
	break;
    case initzlr_k_string:
	fprintf(out, "= \"");
	fprintf(out, "%s", char_utilities_unescape_string(init.str_value));
	fprintf(out, "\"");
	break;
    case initzlr_k_none:
	// don't print anything!
	break;
    default:
	bail_with_error("Unknown initializer kind in unparseInitializer!");
	break;
    }
}


// Unparse the given AST, with output going to out
void unparseStackSection(FILE *out, ast_stack_section_t ss)
{
    fprintf(out, ".stack %u", ss.stack_bottom_addr);
    newline(out);
}
