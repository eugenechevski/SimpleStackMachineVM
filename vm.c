#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "utilities.h"
#include "instruction.h"
#include "bof.h"

void vm_init(VM *vm)
{
    memset(vm->memory, 0, sizeof(vm->memory));
    memset(vm->registers, 0, sizeof(vm->registers));
    vm->pc = 0;
    vm->hi = 0;
    vm->lo = 0;
}

void vm_load_program(VM *vm, BOFFILE bf)
{
    BOFHeader header = bof_read_header(bf);

    // Load instructions
    for (int i = 0; i < header.text_length; i++)
    {
        vm->memory[header.text_start_address + i] = bof_read_word(bf);
    }

    // Load data
    for (int i = 0; i < header.data_length; i++)
    {
        vm->memory[header.data_start_address + i] = bof_read_word(bf);
    }

    // Set initial register values
    vm->pc = header.text_start_address;
    vm->registers[0] = header.data_start_address; // $gp
    vm->registers[1] = header.stack_bottom_addr;  // $sp
    vm->registers[2] = header.stack_bottom_addr;  // $fp
}

void vm_print_state(VM *vm)
{
    printf("PC: %u\n", vm->pc);
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("GPR[$%s]: %d ", regname_get(i), vm->registers[i]);
        if ((i + 1) % 4 == 0)
            printf("\n");
    }
    printf("\n");
    // Print memory contents (you may want to limit this to relevant sections)
    // ...
}

void vm_run(VM *vm)
{
    bool tracing = true;
    while (1)
    {
        if (tracing)
            vm_print_state(vm);

        // Fetch instruction
        bin_instr_t instr = *(bin_instr_t *)&vm->memory[vm->pc];

        // Print the current instruction
        printf("==> %u: %s\n", vm->pc, instruction_assembly_form(vm->pc, instr));

        // Increment PC
        vm->pc++;

        // Decode and execute instruction
        switch (instruction_type(instr))
        {
        case comp_instr_type:
            vm_execute_comp_instr(vm, instr.comp);
            break;
        case other_comp_instr_type:
            vm_execute_other_comp_instr(vm, instr.othc);
            break;
        case immed_instr_type:
            vm_execute_immed_instr(vm, instr.immed);
            break;
        case jump_instr_type:
            vm_execute_jump_instr(vm, instr.jump);
            break;
        case syscall_instr_type:
            if (vm_execute_syscall(vm, instr.syscall) == 1)
            {
                return; // Exit the VM
            }
            break;
        default:
            bail_with_error("Unknown instruction type");
        }
    }
}

// Helper function to get memory address
static inline word_type *get_memory_address(VM *vm, reg_num_type reg, offset_type offset)
{
    return &vm->memory[vm->registers[reg] + machine_types_formOffset(offset)];
}

// Implement these functions next
void vm_execute_comp_instr(VM *vm, comp_instr_t instr)
{
    // TODO: Implement computational instructions
}

void vm_execute_other_comp_instr(VM *vm, other_comp_instr_t instr)
{
    // TODO: Implement other computational instructions
}

void vm_execute_immed_instr(VM *vm, immed_instr_t instr)
{
    word_type *target = get_memory_address(vm, instr.reg, instr.offset);

    switch (instr.op)
    {
    case ADDI_O:
        *target += machine_types_sgnExt(instr.immed);
        break;
    case ANDI_O:
        *target &= machine_types_zeroExt(instr.immed);
        break;
    case BORI_O:
        *target |= machine_types_zeroExt(instr.immed);
        break;
    case NORI_O:
        *target = ~(*target | machine_types_zeroExt(instr.immed));
        break;
    case XORI_O:
        *target ^= machine_types_zeroExt(instr.immed);
        break;
    case BEQ_O:
        if (*get_memory_address(vm, SP, 0) == *target)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    case BGEZ_O:
        if ((int)*target >= 0)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    case BGTZ_O:
        if ((int)*target > 0)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    case BLEZ_O:
        if ((int)*target <= 0)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    case BLTZ_O:
        if ((int)*target < 0)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    case BNE_O:
        if (*get_memory_address(vm, SP, 0) != *target)
        {
            vm->pc += machine_types_formOffset(instr.immed);
        }
        break;
    default:
        bail_with_error("Unknown immediate instruction opcode: %d", instr.op);
    }
}

void vm_execute_jump_instr(VM *vm, jump_instr_t instr)
{
    switch (instr.op)
    {
    case JMPA_O:
        vm->pc += machine_types_formAddress(vm->pc, instr.op);
        break;
    case CALL_O:
        vm->registers[RA] = vm->pc;
        vm->pc += machine_types_formAddress(vm->pc, instr.op);
        break;
    case RTN_O:
        vm->pc = vm->registers[RA];
        break;
    default:
        bail_with_error("Unknown immediate instruction opcode: %d", instr.op);
    }

    // TODO: Implement jump instructions
}

int vm_execute_syscall(VM *vm, syscall_instr_t instr)
{
    // TODO: Implement system calls
    return 0;
}

void vm_print_program(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = 0; i < MEMORY_SIZE_IN_WORDS; i++)
    {
        bin_instr_t instr = *(bin_instr_t *)&vm->memory[i];
        if (instruction_type(instr) == error_instr_type)
            break;
        printf("%u: %s\n", i, instruction_assembly_form(i, instr));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        bail_with_error("Usage: %s [-p] <bof_file>", argv[0]);
    }

    bool print_mode = false;
    char *filename = argv[1];

    if (argc == 3 && strcmp(argv[1], "-p") == 0)
    {
        print_mode = true;
        filename = argv[2];
    }

    BOFFILE bf = bof_read_open(filename);
    VM vm;
    vm_init(&vm);
    vm_load_program(&vm, bf);

    if (print_mode)
    {
        vm_print_program(&vm);
    }
    else
    {
        vm_run(&vm);
    }

    bof_close(bf);
    return 0;
}