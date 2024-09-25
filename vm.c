#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
    vm->text_length = header.text_length;
    vm->data_length = header.data_length;
    // Load instructions
    for (int i = 0; i < header.text_length; i++)
    {
        vm->instructions[i] = instruction_read(bf);
        // printf(" \n 1 \n");
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

void vm_print_words(VM *vm)
{
    word_type address = vm->registers[0]; // Correct access to registers array
    int count = 0;                        // Use a counter for formatting
    printf("    ");

    // Loop through memory up to the stack pointer (registers[1] = $sp)
    while (address <= vm->registers[2])
    {
        // Print memory address and value
        printf("%6d: %d     ", address, vm->memory[address]);
        count++;

        // Print a newline every 4 values for readability
        if (count % 5 == 0)
            printf("\n");

        // Detect blocks of zeros and print "..."
        if (address < vm->registers[2] - 2 && vm->memory[address] == 0 && vm->memory[address + 1] == 0)
        {
            printf("\t...\t");
            // Skip over consecutive zeros
            while (vm->memory[address] == 0 && vm->memory[address + 1] == 0 && address < vm->registers[2] - 1)
            {
                address++;
            }
        }

        // Move to the next memory address
        address++;
    }
    printf("\n");
}

void vm_print_state(VM *vm)
{

    printf("     PC: %u\n", vm->pc);
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("GPR[$%s]: %-6d\t", regname_get(i), vm->registers[i]);
        if ((i + 1) % 5 == 0)
            printf("\n");
    }
    printf("\n");
    // Print memory contents (you may want to limit this to relevant sections)
    // ...
    vm_print_words(vm);
}

static inline word_type *get_memory_address(VM *vm, reg_num_type reg, offset_type offset)
{
    return &vm->memory[vm->registers[reg] + machine_types_formOffset(offset)];
}

static inline word_type *get_p(VM *vm, reg_num_type reg)
{
    return &vm->memory[vm->registers[reg]];
}

void vm_execute_comp_instr(VM *vm, comp_instr_t instr)
{
    word_type *target = get_memory_address(vm, instr.rt, instr.ot);
    word_type *sp = get_p(vm, 1);
    word_type *s_os = get_memory_address(vm, instr.rs, instr.os);
    word_type *rt = get_p(vm, instr.rt);
    word_type *rs = get_p(vm, instr.rs);

    switch (instr.func)
    {
    case NOP_F:
        break;
    case ADD_F:
        *target = *sp + *s_os;
        break;
    case SUB_F:
        *target = *sp - *s_os;
        break;
    case CPW_F:
        *target = *s_os;
        break;
    case AND_F:
        *target = *sp & *s_os;
        break;

    case BOR_F:
        *target = *sp | *s_os;
        break;
    case NOR_F:
        *target = ~(*sp | *s_os);
        break;
    case XOR_F:
        *target = *sp ^ *s_os;
        break;
    case LWR_F:
        vm->registers[instr.rt] = *s_os;
        break;

    case SWR_F:
        *target = vm->registers[instr.rs];
        break;
    case SCA_F:
        *target = vm->registers[instr.rs] + machine_types_formOffset(instr.os);
        break;
    case LWI_F:
        *target = vm->memory[*s_os];
        break;

    case NEG_F:
        *target = -(*s_os);
        break;

    default:
        bail_with_error("Unknown immediate instruction opcode: %d", instr.op);
    }

    // TODO: Implement computational instructions
}
// Implement these functions next

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

void vm_execute_other_comp_instr(VM *vm, other_comp_instr_t instr)
{
    // TODO: Implement other computational instructions
    word_type *target = get_memory_address(vm, instr.reg, instr.offset);

    word_type *sp = get_p(vm, 1);
    word_type *ra = get_p(vm, 7);
    // word_type *s_os = get_memory_address(vm, instr.rs, instr.os);
    word_type *r = get_p(vm, instr.reg);
    int64_t temp;
    // printf("\n\n\n\\n\n\n %d \n\n\n\n\n\n", instr.op);
    switch (instr.func)
    {
    case LIT_F:
        *target = machine_types_sgnExt(instr.arg);
        break;

    case ARI_F:
        vm->registers[instr.reg] += machine_types_sgnExt(instr.arg);
        break;

    case SRI_F:
        // printf("    I am here    \n\n\n");
        vm->registers[instr.reg] -= machine_types_sgnExt(instr.arg);
        break;

    case MUL_F:

        temp = (int32_t)*sp * (int32_t)*target;

        vm->hi = ((temp >> 32) & 0xFFFFFFFF);
        vm->lo = (temp & 0xFFFFFFFF);

        break;

    case DIV_F:
        vm->hi = *sp % *target;
        vm->lo = *sp / *target;
        break;

    case CFHI_F:
        *target = vm->hi;
        break;

    case CFLO_F:
        *target = vm->lo;
        break;

    case SLL_F:
        *target = *sp << instr.arg;
        break;

    case SRL_F:
        *target = *sp >> instr.arg;
        break;

    case JMP_F:
        vm->pc = *target;
        break;

    case CSI_F:
        vm->registers[7] = vm->pc;
        vm->pc = *target;
        break;

    case JREL_F:
        vm->pc = ((vm->pc - 1) + machine_types_formOffset(instr.arg));
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
        vm->pc = machine_types_formAddress(vm->pc, instr.op);
        break;
    case CALL_O:
        vm->registers[RA] = vm->pc;
        vm->pc = machine_types_formAddress(vm->pc, instr.op);
        break;
    case RTN_O:
        vm->pc = vm->registers[RA];
        break;
    default:
        bail_with_error("Unknown immediate instruction opcode: %d", instr.op);
    }

    // TODO: Implement jump instructions
}

void vm_run(VM *vm)
{
    bool tracing;
    bool execute = true;
    vm_print_state(vm);
    while (execute == true)
    {
        // Increment PC
        vm->pc++;
        bin_instr_t instr = *(bin_instr_t *)&vm->instructions[vm->pc];

        if (tracing)
        {
            printf("\n==>      %u: %s\n", vm->pc, instruction_assembly_form(vm->pc, instr));
            vm_print_state(vm);
        }

        if (instruction_type(instr) == syscall_instr_type && instr.syscall.code == 1)
        {
            // Terminate VM if it's an exit syscall (code == 1)
            execute = false;
            return; // Exit the VM
        }
        // Fetch instruction

        // Print the current instruction

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

            // printf("\n\n\n\n   %d \n\n\n\n\n", instr.syscall.code);

            if (instr.syscall.code == 1)
            {

                execute = false;
                return; // Exit the VM
            }
            else if (instr.syscall.code == 2)
            {
                word_type *sp = get_p(vm, 1);
                char *str = (char *)get_memory_address(vm, instr.syscall.reg, instr.syscall.offset);
                *sp = printf("%s", str);
            }
            else if (instr.syscall.code == 4)
            {

                word_type *sp = get_p(vm, 1);
                int ch = *(get_memory_address(vm, instr.syscall.reg, instr.syscall.offset));
                *sp = fputc(ch, stdout);
            }
            else if (instr.syscall.code == 5)
            {
                word_type *target = get_memory_address(vm, instr.syscall.reg, instr.syscall.offset);
                *target = getc(stdin);
            }
            else if (instr.syscall.code == 2046)
            {
                tracing = true;
            }
            else if (instr.syscall.code == 2047)
            {
                tracing = false;
            }
            break;
        default:
            bail_with_error("Unknown instruction type");
        }
    }
}

// Helper function to get memory address

void vm_print_program(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = 0; i < vm->text_length; i++)
    {
        bin_instr_t instr = *(bin_instr_t *)&vm->instructions[i];

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