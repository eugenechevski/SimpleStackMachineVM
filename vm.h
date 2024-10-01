#ifndef VM_H
#define VM_H

#include "machine_types.h"
#include "instruction.h"
#include "regname.h"
#include "bof.h"
#include "utilities.h"

#define MEMORY_SIZE_IN_WORDS 32768
#define NUM_REGISTERS 8

#define GP 0  // Global Pointer
#define SP 1  // Stack Pointer
#define FP 2  // Frame Pointer
#define RA 7  // Return Address
typedef struct {
    union {
        word_type words[MEMORY_SIZE_IN_WORDS];
        uword_type uwords[MEMORY_SIZE_IN_WORDS];
        bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
    } memory;
    word_type registers[NUM_REGISTERS];
    word_type pc;
    word_type hi;
    word_type lo;
    word_type data_length;
    word_type text_length;
} VM;


void vm_init(VM *vm);
void vm_load_program(VM *vm, BOFFILE bf);
void vm_run(VM *vm);
void vm_print_program(VM *vm);

#endif