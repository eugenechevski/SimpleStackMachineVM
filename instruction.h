#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H
#include <stdio.h>
#include "machine_types.h"

typedef struct
{
    opcode_type op;
    reg_num_type reg;
    offset_type offset;
    word_type arg;
    func_type func;
} instruction_t;

char *instruction_assembly_form(instruction_t *instr);
instruction_t *instruction_read(FILE *file);

#endif