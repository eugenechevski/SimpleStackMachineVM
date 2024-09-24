#ifndef VM_H
#define VM_H

#include "machine_types.h"
#include "instruction.h"
#include "bof.h"

#define MEMORY_SIZE_IN_WORDS 32768
#define NUM_REGISTERS 8

#define GP 0  // Global Pointer
#define SP 1  // Stack Pointer
#define FP 2  // Frame Pointer
#define RA 7  // Return Address

typedef struct {
    word_type memory[MEMORY_SIZE_IN_WORDS];
    word_type registers[NUM_REGISTERS];
    word_type pc;
    word_type hi;
    word_type lo;
} VM;

void vm_init(VM *vm);
void vm_load_program(VM *vm, BOFFILE bf);
void vm_run(VM *vm);
void vm_print_program(VM *vm);

#endif