// $Id: instruction.h,v 1.37 2024/08/29 21:58:39 leavens Exp $
#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H
#include <stdio.h>
#include <stdbool.h>
#include "machine_types.h"
#include "bof.h"

// op codes in binary instructions for the SSM
typedef enum {COMP_O = 0, OTHC_O = 1, ADDI_O = 2, ANDI_O = 3, BORI_O = 4,
              NORI_O = 5, XORI_O = 6, 
	      BEQ_O = 7, BGEZ_O = 8, BGTZ_O = 9, BLEZ_O = 10, BLTZ_O = 11,
	      BNE_O = 12, JMPA_O = 13, CALL_O = 14, RTN_O = 15
             } op_code;

// function codes in binary instructions for the SSM (when opcode is 0)
typedef enum {NOP_F = 0, ADD_F = 1, SUB_F = 2, CPW_F = 3, 
	      AND_F = 5, BOR_F = 6, NOR_F = 7, XOR_F = 8,
	      LWR_F = 9, SWR_F = 10, SCA_F = 11, LWI_F = 12, NEG_F = 13
             } func0_code;

// function codes in binary instructions for the SSM (when opcode is 1)
typedef enum {LIT_F = 1, ARI_F = 2, SRI_F = 3, MUL_F = 4, DIV_F = 5,
	      CFHI_F = 6, CFLO_F = 7, SLL_F = 8, SRL_F = 9, JMP_F = 10,
	      CSI_F = 11, JREL_F = 12, SYS_F = 15
             } func1_code;

// instruction types (each is a binary instruction format)
typedef enum {comp_instr_type, other_comp_instr_type,
	      immed_instr_type, jump_instr_type, syscall_instr_type,
	      error_instr_type
             } instr_type;

// system calls
typedef enum {exit_sc = 1, print_str_sc = 2,
	      print_char_sc = 4, read_char_sc = 5,
	      start_tracing_sc = 2046, stop_tracing_sc = 2047
} syscall_type;

// computational type instructions, with opcode 0
typedef struct {
    opcode_type op : 4;
    reg_num_type rt : 3;  // target register
    offset_type ot : 9;   // offset from target register
    reg_num_type rs : 3;  // source register
    offset_type os : 9;   // offset from source register
    func_type func : 4;
} comp_instr_t;

// other computational type instructions, with opcode 1, except system calls
typedef struct {
    opcode_type op : 4;
    reg_num_type reg : 3;
    offset_type offset : 9;
    arg_type arg : 12;
    func_type func : 4;
} other_comp_instr_t;

// system call instructions, with op field 1 and func field 15
typedef struct {
    opcode_type op : 4;
    reg_num_type reg : 3;
    offset_type offset : 9;
    syscall_type code : 12;
    func_type func : 4;
} syscall_instr_t;

// immediate operand type instructions
// with signed immediate operands
typedef struct {
    opcode_type op : 4;
    reg_num_type reg : 3;
    offset_type offset : 9;
    immediate_type immed : 16;
} immed_instr_t;

// immediate operand type instructions
// with unsigned immediate operands
typedef struct {
    opcode_type op : 4;
    reg_num_type reg : 3;
    offset_type offset : 9;
    uimmed_type uimmed : 16;
} uimmed_instr_t;

// jump type instructions
typedef struct {
    opcode_type op : 4;
    address_type addr : 28;
} jump_instr_t;

// binary instructions of any type
typedef union {
    comp_instr_t comp;
    other_comp_instr_t othc;
    syscall_instr_t syscall;
    immed_instr_t immed;
    uimmed_instr_t uimmed;
    jump_instr_t jump;
} bin_instr_t;

// Return the type of the binary instruction given
extern instr_type instruction_type(bin_instr_t i);

// Requires: bof is open for reading in binary
// Read a single instruction (in binary) from bf and return it,
// but exit with an error if there is a problem.
extern bin_instr_t instruction_read(BOFFILE bf);

// Requires: bof is open for writing in binary
// Write the computational instruction oci to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_compInstr(BOFFILE bf, comp_instr_t oci);

// Requires: bof is open for writing in binary
// Write the other computational instruction oci to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_otherCompInstr(BOFFILE bf, other_comp_instr_t oci);

// Requires: bof is open for writing in binary
// Write the system instruction si to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_syscallInstr(BOFFILE bf, syscall_instr_t si);

// Requires: bof is open for writing in binary
// Write the immediate instruction ii with opcode op to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_immedInstr(BOFFILE bf, immed_instr_t ii);

// Requires: bof is open for writing in binary
// Write the immediate instruction ii with opcode op to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_uimmedInstr(BOFFILE bf, uimmed_instr_t ii);

// Requires: bof is open for writing in binary
// Write the jump instruction ji with opcode op to bf in binary,
// but exit with an error if there is a problem.
extern void instruction_write_jumpInstr(BOFFILE bf, jump_instr_t ji);

// Return the assembly language name (mnemonic) for bi
extern const char *instruction_mnemonic(bin_instr_t bi);

// Return a string containing the assembly language form of instr,
// which is found at address addr
extern const char *instruction_assembly_form(address_type addr,
					     bin_instr_t instr);

// Requires: out is open and writable FILE
// print the header of the instruction output table on out
extern void instruction_print_table_heading(FILE *out);

// Requires: out is an open FILE, and instr is found at address addr.
// Print addr on out, ": ", then the instruction's symbolic
// (assembly language) form, and finally a newline character (all on one line)
extern void instruction_print(FILE *out, address_type addr, bin_instr_t instr);

// Return the mnemonic for the given system call code
extern const char *instruction_syscall_mnemonic(unsigned int code);

// Requires: instr is a SYSCALL instruction
// (i.e., instr.syscall.op == OTHC_O and instr.syscall.func == SYS_F).
// Return the code field that tells what kind of system call is being made
extern syscall_type instruction_syscall_number(bin_instr_t instr);

// Requires: bi is a computational instruction (bi.comp.op == COMP_O).
// Return a string giving the assembly language mnemonic for bi's operation
extern const char *instruction_compFunc2name(bin_instr_t bi);

// Requires: bi is an other computational instruction (bi.othc.op == OTHC_O).
// Return a string giving the assembly language mnemonic for bi's operation
extern const char *instruction_otherCompFunc2name(bin_instr_t bi);

// Return the system call type corresponding to the given token number
extern syscall_type instruction_token2SyscallCode(int toknum);

#endif
