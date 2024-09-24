// $Id: instruction.c,v 1.57 2024/08/29 21:58:39 leavens Exp $
#include <errno.h>
#include <string.h>
#include "bof.h"
#include "instruction.h"
#include "regname.h"
#include "utilities.h"
#include "machine_types.h"
#include "asm.tab.h"

#define INSTR_BUF_SIZE 512

// the following declaration isn't in <string.h> everywhere ...
extern char *strdup(const char *s);

// space to hold one instruction's assembly language form
static char instr_buf[INSTR_BUF_SIZE];

// Return the instruction type of the given opcode 
instr_type instruction_type(bin_instr_t i) {
    switch (i.comp.op) { // need to pretend a type to read the op field
    case COMP_O:
	    return comp_instr_type;
	break;
    case OTHC_O:
	if (i.othc.func == SYS_F) {
	    return syscall_instr_type;
	} else {
	    assert(i.othc.op != COMP_O);
	    assert(i.othc.func != NOP_F);
	    return other_comp_instr_type;
	}
	break;
    case ADDI_O: case ANDI_O: case BORI_O: case NORI_O: case XORI_O:
    case BEQ_O: case BGEZ_O: case BGTZ_O: case BLEZ_O: case BLTZ_O:
    case BNE_O:
	return immed_instr_type;
	break;
    case JMPA_O: case CALL_O: case RTN_O:
	return jump_instr_type;
	break;
    default:
	return error_instr_type;
	break;
    }
}

// Requires: bof is open for reading in binary
// Read a single instruction (in binary) from bf and return it,
// but exit with an error if there is a problem.
bin_instr_t instruction_read(BOFFILE bf)
{
    bin_instr_t bi;
    size_t rd = fread(&bi, sizeof(bi), 1, bf.fileptr);
    if (rd != 1) {
	bail_with_error("Cannot read instruction from %s (read %d instrs)",
			bf.filename, rd);
    }
    return bi;
}

// Requires: bf is open for writing in binary
// Write the given binary instruction to bf,
// but exit with an error if there is a problem.
static void write_bin_instr(BOFFILE bf, bin_instr_t i)
{
    size_t wr = fwrite(&i, sizeof(i), 1, bf.fileptr);
    if (wr != 1) {
	bail_with_error("Cannot write binary instr to %s", bf.filename);
    }
}

// Requires: bof is open for writing in binary
// Write the computational instruction ci to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_compInstr(BOFFILE bf, comp_instr_t ci)
{
    bin_instr_t bi;
    bi.comp = ci;
    assert(bi.comp.op == ci.op);
    assert(bi.comp.rt == ci.rt);
    assert(bi.comp.ot == ci.ot);
    assert(bi.comp.rs == ci.rs);
    assert(bi.comp.os == ci.os);
    assert(bi.comp.func == ci.func);
    assert(bi.comp.func != NOP_F || bi.comp.op == COMP_O);
    write_bin_instr(bf, bi);
}

// Requires: bof is open for writing in binary
// Write the other computational instruction oci to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_otherCompInstr(BOFFILE bf, other_comp_instr_t oci)
{
    bin_instr_t bi;
    bi.othc = oci;

    assert(bi.othc.op == oci.op);
    assert(bi.othc.reg == oci.reg);
    assert(bi.othc.offset == oci.offset);
    assert(bi.othc.arg == oci.arg);
    assert(bi.othc.func == oci.func);

    write_bin_instr(bf, bi);
}

// Requires: bof is open for writing in binary
// Write the system instruction si to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_syscallInstr(BOFFILE bf, syscall_instr_t si)
{
    bin_instr_t bi;
    bi.syscall = si;
    /*
    assert(bi.syscall.op == OTHC_O);
    assert(bi.syscall.reg == si.reg);
    assert(bi.syscall.offset == si.offset);
    assert(bi.syscall.code == si.code);
    assert(bi.syscall.func == SYS_F);
    */
    write_bin_instr(bf, bi);
}

// Requires: bof is open for writing in binary
// Write the immediate instruction ii to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_uimmedInstr(BOFFILE bf, uimmed_instr_t ui)
{
    bin_instr_t bi;
    bi.uimmed = ui;
    /*
    assert(bi.uimmed.op == op);
    assert(bi.uimmed.reg == ui.reg);
    assert(bi.uimmed.offset == ui.offset);
    assert(bi.uimmed.uimmed == ui.uimmed);
    */
    write_bin_instr(bf, bi);
}

// Requires: bof is open for writing in binary
// Write the immediate instruction ii to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_immedInstr(BOFFILE bf, immed_instr_t ii)
{
    bin_instr_t bi;
    bi.immed = ii;
    /*
    assert(bi.immed.op == op);
    assert(bi.immed.reg == ii.reg);
    assert(bi.immed.offset == ii.offset);
    assert(bi.immed.immed == ii.immed);
    */
    write_bin_instr(bf, bi);
}

// Requires: bof is open for writing in binary
// Write the jump instruction ji with opcode op to bf in binary,
// but exit with an error if there is a problem.
void instruction_write_jumpInstr(BOFFILE bf, jump_instr_t ji)
{
    bin_instr_t bi;
    bi.jump = ji;
    /*
    assert(bi.jump.op == op);
    assert(bi.jump.addr == ji.addr);
    */
    write_bin_instr(bf, bi);
}

// Return the assembly language name (mnemonic) for bi
const char *instruction_mnemonic(bin_instr_t bi) {
    switch (bi.comp.op) { // pretending a type is needed to get the op field
    case COMP_O:
	return instruction_compFunc2name(bi);
	break;
    case OTHC_O:
	return instruction_otherCompFunc2name(bi);
	break;
    case ADDI_O:
	return "ADDI";
	break;
    case ANDI_O:
	return "ANDI";
	break;
    case BORI_O:
	return "BORI";
	break;
    case NORI_O:
	return "NORI";
	break;
    case XORI_O:
	return "XORI";
	break;
    case BEQ_O:
	return "BEQ";
	break;
    case BGEZ_O:
	return "BGEZ";
	break;
    case BGTZ_O:
	return "BGTZ";
	break;
    case BLEZ_O:
	return "BLEZ";
	break;
    case BLTZ_O:
	return "BLTZ";
	break;
    case BNE_O:
	return "BNE";
	break;
    case JMPA_O:
	return "JMPA";
	break;
    case CALL_O:
	return "CALL";
	break;
    case RTN_O:
	return "RTN";
	break;
    default:
	bail_with_error("Unknown op code (%d) in instruction_mnemonic!",
			bi.immed.op);
	return NULL;   // should never happen
    }
    return NULL;  // should never happen
}

static char address_comment_buf[512];

// return a comment string of the form
// "# target is word address %u"
// that uses the formAddress function to get the proper address
static const char *instruction_formAddress_comment(address_type addr,
						   address_type a)
{
    address_type actual = machine_types_formAddress(addr, a);
    sprintf(address_comment_buf, "# target is word address %u",
	    actual);
    return strdup(address_comment_buf);
}


// Return a string containing the assembly language form of instr,
// which is found at address addr
const char *instruction_assembly_form(address_type addr,
				      bin_instr_t instr)
{
    char *buf = instr_buf;

    // put in the mnemonic for the instruction
    int cwr = sprintf(buf, "%s ", instruction_mnemonic(instr));
    // point buf to the null char that was printed into instr_buf
    buf += cwr;

    instr_type it = instruction_type(instr);
    switch (it) {
    case comp_instr_type:
	switch (instr.comp.func) {
	case NOP_F:
	    // no arguments for NOP!
	    break;
	case ADD_F: case SUB_F: case CPW_F:
	case AND_F: case BOR_F: case NOR_F: case XOR_F:
	case SCA_F: case LWI_F: case NEG_F:
	    sprintf(buf, "%s, %hd, %s, %hd",
		    regname_get(instr.comp.rt),
		    instr.comp.ot,
		    regname_get(instr.comp.rs),
		    instr.comp.os);
	    break;
	case LWR_F: 
	    sprintf(buf, "%s, %s, %hd",
		    regname_get(instr.comp.rt),
		    regname_get(instr.comp.rs),
		    instr.comp.os);
	    break;
	case SWR_F: 
	    sprintf(buf, "%s, %hd, %s",
		    regname_get(instr.comp.rt),
		    instr.comp.ot,
		    regname_get(instr.comp.rs));
	    break;
	default:
	    bail_with_error("Unknown computational instruction function (%d) for mnemonic %s!",
			    instr.comp.func, buf);
	    break;
	}
	break;
    case other_comp_instr_type:
	assert(instr.othc.op == OTHC_O);
	switch (instr.othc.func) {
	case LIT_F:
	    sprintf(buf, "%s, %hd, %hd", regname_get(instr.othc.reg),
		    instr.othc.offset, instr.othc.arg);
	    break;
	case ARI_F: case SRI_F:
	    sprintf(buf, "%s, %hd", regname_get(instr.othc.reg),
		    instr.othc.arg);
	    break;
	case MUL_F: case DIV_F: case CFHI_F: case CFLO_F: case JMP_F:
	case CSI_F:
	    sprintf(buf, "%s, %hd", regname_get(instr.othc.reg),
		    instr.othc.offset);
	    break;
	case SLL_F: case SRL_F:
	    sprintf(buf, "%s, %hd, %hu", regname_get(instr.othc.reg),
		    instr.othc.offset, instr.othc.arg);
	    break;
	case JREL_F:
	    sprintf(buf, "%hd\t%s", instr.othc.arg,
		    instruction_formAddress_comment(addr,
						    addr+instr.othc.arg));
	    break;  
	default:
	    bail_with_error("Unknown other computational instruction function (%d)!",
			    instr.comp.func);
	    break;
	}
	break;
    case immed_instr_type:
	switch (instr.immed.op) {
	case ADDI_O:
	    sprintf(buf, "%s, %hd, %hd", regname_get(instr.immed.reg),
		    instr.immed.offset, instr.immed.immed);
	    break;
	case ANDI_O: case BORI_O: case NORI_O: case XORI_O: 
	    sprintf(buf, "%s, %hd, 0x%hx", regname_get(instr.immed.reg),
		    instr.immed.offset, instr.immed.immed);
	    break;
	case BEQ_O: case BGEZ_O: case BGTZ_O:
	case BLEZ_O: case BLTZ_O: case BNE_O:
	    sprintf(buf, "%s, %hd, %hd\t%s", regname_get(instr.immed.reg),
		    instr.immed.offset, instr.immed.immed,
		    instruction_formAddress_comment(addr,
						    addr+instr.immed.immed));
	    break;
	default:
	    bail_with_error("Unknown immediate type instruction opcode (%d)!",
			    instr.immed.op);
	    break;
	}
	break;
    case jump_instr_type:
	switch (instr.jump.op) {
	case JMPA_O: case CALL_O:
	    sprintf(buf, "%u\t%s", instr.jump.addr,
		    instruction_formAddress_comment(addr, instr.jump.addr));
	    break;
	case RTN_O:
	    // no arguments in this case
	    break;
	default:
	    bail_with_error("Unknown jump type instruction opcode (%d)!",
			    instr.jump.op);
	    break;
	}
	break;
    case syscall_instr_type:
	switch (instr.syscall.code) {
	case exit_sc:
	    sprintf(buf, "%hd", instr.syscall.offset);
	    break;
	case print_str_sc: case print_char_sc: case read_char_sc:
	    sprintf(buf, "%s, %hd", regname_get(instr.syscall.reg),
		    instr.syscall.offset);
	    break;
	case start_tracing_sc: case stop_tracing_sc:
	    // no arguments, so nothing to do!
	    break;
	}
	break;
    default:
	bail_with_error("Unknown instruction type (%d) in instruction_assembly_form!",
			it);
	break;
    }

    return instr_buf;
}

// Requires: out is open and writable FILE
// print the header of the instruction output table on out
void instruction_print_table_heading(FILE *out) {
    fprintf(out, "%s %s\n", "Address", "Instruction");
}

// Requires: out is an open FILE, and instr is found at address addr.
// Print addr on out, ": ", then the instruction's symbolic
// (assembly language) form, and finally a newline character (all on one line)
void instruction_print(FILE *out, address_type addr, bin_instr_t instr) {
    fprintf(out, "%8u: %s\n", addr, instruction_assembly_form(addr, instr));
}

// Return the mnemonic for the given system call code
const char *instruction_syscall_mnemonic(syscall_type code)
{
    switch (code) {
    case exit_sc:
	return "EXIT";
	break;
    case print_str_sc:
	return "PSTR";
	break;
    case print_char_sc:
	return "PCH";
	break;
    case read_char_sc:
	return "RCH";
	break;
    case start_tracing_sc:
	return "STRA";
	break;
    case stop_tracing_sc:
	return "NOTR";
	break;
    default:
	bail_with_error("Unknown code (%u) in instruction_syscall_mnemonic",
			code);
	return "NEVERHAPPENS";
	break;
    }
}

// Requires: instr is a SYSCALL instruction
// (i.e., instr.syscall.op == OTHC_O and instr.syscall.func == SYS_F).
// Return the code field that tells what kind of system call is being made
syscall_type instruction_syscall_number(bin_instr_t instr) {
    assert(instr.syscall.op == OTHC_O && instr.syscall.func == SYS_F);
    return instr.syscall.code;
}

// Requires: bi is a computational instruction (bi.comp.op == COMP_O).
// Return a string giving the assembly language mnemonic for bi's operation
const char *instruction_compFunc2name(bin_instr_t bi) {
    assert(bi.comp.op == COMP_O);
    switch (bi.comp.func) {
    case NOP_F:
	return "NOP";
	break;
    case ADD_F:
	return "ADD";
	break;
    case SUB_F:
	return "SUB";
	break;
    case CPW_F:
	return "CPW";
	break;
    case AND_F:
	return "AND";
	break;
    case BOR_F:
	return "BOR";
	break;
    case NOR_F:
	return "NOR";
	break;
    case XOR_F:
	return "XOR";
	break;
    case LWR_F:
	return "LWR";
	break;
    case SWR_F:
	return "SWR";
	break;
    case SCA_F:
	return "SCA";
	break;
    case LWI_F:
	return "LWI";
	break;
    case NEG_F:
	return "NEG";
	break;
    default:
	bail_with_error("Unknown function code (%d) in instruction_compFunc2name",
			bi.comp.func);
	break;
    }
    return NULL; // should never happen
}

// Requires: bi is an other computational instruction (bi.othc.op == OTHC_O).
// Return a string giving the assembly language mnemonic for bi's operation
const char *instruction_otherCompFunc2name(bin_instr_t bi) {
    assert(bi.othc.op == OTHC_O);
    switch (bi.othc.func) {
    case LIT_F:
	return "LIT";
	break;
    case ARI_F:
	return "ARI";
	break;
    case SRI_F:
	return "SRI";
	break;
    case MUL_F:
	return "MUL";
	break;
    case DIV_F:
	return "DIV";
	break;
    case CFHI_F:
	return "CFHI";
	break;
    case CFLO_F:
	return "CFLO";
	break;
    case SLL_F:
	return "SLL";
	break;
    case SRL_F:
	return "SRL";
	break;
    case JMP_F:
	return "JMP";
	break;
    case CSI_F:
	return "CSI";
	break;
    case JREL_F:
	return "JREL";
	break;
    case SYS_F:
	return instruction_syscall_mnemonic(instruction_syscall_number(bi));
	break;
    default:
	bail_with_error("Unknown function code (%d) in instruction_otherCompFunc2name",
			bi.comp.func);
	break;
    }
    return NULL; // should never happen
}

// Requires: toknum is the token number of a system call operation
// Return the system call type corresponding to the given token number
syscall_type instruction_token2SyscallCode(int toknum)
{
    switch (toknum) {
    case exitopsym:
	return exit_sc;
	break;
    case pstropsym:
	return print_str_sc;
	break;
    case pchopsym:
	return print_char_sc;
	break;
    case rchopsym:
	return read_char_sc;
	break;
    case straopsym:
	return start_tracing_sc;
	break;
    case notropsym:
	return stop_tracing_sc;
	break;
    default:
	bail_with_error("Unlnown token type in instruction_token2SyscallCode: %d",
			toknum);
	return 0; // should never happen
	break;
    }
}
