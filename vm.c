#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"

// Helper function to get memory address
static inline word_type *get_memory_address(VM *vm, reg_num_type reg, offset_type offset)
{
    return &vm->memory[vm->registers[reg] + machine_types_formOffset(offset)];
}

static inline word_type *get_p(VM *vm, reg_num_type reg)
{
    return &vm->memory[vm->registers[reg]];
}

void vm_init(VM *vm)
{
    memset(vm->memory, 0, sizeof(vm->memory));
    memset(vm->registers, 0, sizeof(vm->registers));
    memset(vm->instructions, 0, sizeof(vm->instructions));
    vm->pc = 0;
    vm->hi = 0;
    vm->lo = 0;
    vm->data_length = 0;
    vm->text_length = 0;
    vm->memory_index = 0;
}

void vm_load_program(VM *vm, BOFFILE bf)
{
    BOFHeader header = bof_read_header(bf);
    vm->text_length = header.text_length;
    vm->data_length = header.data_length;

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

void vm_print_tracing_words(VM *vm) {
    word_type address = vm->registers[0];  // Correct access to registers array
    int count = 0;  // Use a counter for formatting
    printf("    ");

    // Loop through memory up to the stack pointer (registers[1] = $sp)
    while (address < vm->registers[2]) {
        // Print memory address and value
        printf("%3d: %d         ", address, vm->memory[address]);
        count++;
        
        // Print a newline every 4 values for readability
        if (count % 4 == 0)
        {
            printf("\n");
            printf("    ");
        }
        
        // Detect blocks of zeros and print "..."
        if (address < vm-> registers[2] - 2 && vm->memory[address] == 0 && vm->memory[address + 1] == 0) {
            printf("...         ");
            // Skip over consecutive zeros
            while (vm->memory[address] == 0 && vm->memory[address + 1] == 0 && address < vm->registers[2] - 1) {
                address++;
            }
        }
        
        // Move to the next memory address
        address++;
    }

    // print the last memory address on the newline
    printf("\n");
    printf("    ");
    printf("%3d: %d\n\n", address, vm->memory[address]);
}

void vm_print_state(VM *vm)
{
    printf("      PC: %u\n", vm->pc);
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("GPR[%s]: %-6d", regname_get(i), vm->registers[i]);
        if ((i + 1) % 5 == 0)
            printf("\n");
    }
    printf("\n");

    // Print memory contents (you may want to limit this to relevant sections)
    vm_print_tracing_words(vm);
}

void vm_execute_comp_instr(VM *vm, comp_instr_t instr)
{
    word_type *target = get_memory_address(vm, instr.rt, instr.ot);
    word_type *sp = get_p(vm, 1);
    word_type *s_os = get_memory_address(vm, instr.rs, instr.os);

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
}

void vm_execute_other_comp_instr(VM *vm, other_comp_instr_t instr)
{
    word_type *target = get_memory_address(vm, instr.reg, instr.offset);

    word_type *sp = get_p(vm, 1);

    union {
        int64_t long_val;
        struct {
            int32_t low;
            int32_t high;
        } parts;
    } result;

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

        result.long_val = (int64_t)(*sp) * (int64_t)(*target);
        vm->hi = result.parts.high;
        vm->lo = result.parts.low;
        break;

    case DIV_F:
        vm->lo = *sp / *target;
        vm->hi = *sp % *target;

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

int vm_execute_syscall(VM *vm, bool *tracing, syscall_instr_t instr)
{
    if (instr.code == 1)
    {
        return 1; // Exit the VM
    }
    else if (instr.code == 2)
    {
        word_type *sp = get_p(vm, 1);
        char *str = (char *)get_memory_address(vm, instr.reg, instr.offset);
        *sp = printf("%s", str);
    }
    else if (instr.code == 3) // PINT instruction
    {
        word_type *sp = get_p(vm, 1);
        int value = *(get_memory_address(vm, instr.reg, instr.offset));
        *sp = printf("%d", value);
    }
    else if (instr.code == 4)
    {
        word_type *sp = get_p(vm, 1);
        int ch = *(get_memory_address(vm, instr.reg, instr.offset));
        *sp = fputc(ch, stdout);
    }
    else if (instr.code == 5)
    {
        word_type *target = get_memory_address(vm, instr.reg, instr.offset);
        *target = getc(stdin);
    }
    else if (instr.code == 2046)
    {
        *tracing = true;
    }
    else if (instr.code == 2047)
    {
        *tracing = false;
    }

    return 0;
}

void vm_run(VM *vm)
{
    bool tracing = true;
    while (1)
    {
        // Fetch instruction
        bin_instr_t instr = *(bin_instr_t *)&vm->memory[vm->pc];


        // Print the current state of the VM and assembly form of the instruction
        // if tracing is enabled and the instruction opcode is not NOP_F
        if (tracing)
        {
            vm_print_state(vm);
        }
        
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
            if (vm_execute_syscall(vm, &tracing, instr.syscall) == 1)
            {
                return; // Exit the VM
            }
            break;
        default:
            bail_with_error("Unknown instruction type");
        }
    }
}

void vm_print_instructions(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = 0; i < vm->text_length; i++)
    {
        bin_instr_t instr = *(bin_instr_t *)&vm->instructions[i];

        if (instruction_type(instr) == error_instr_type)
            break;

        // if statements to help provide correct spacing
        if (i < 10)
        {
            printf("     %u: %s\n", i, instruction_assembly_form(i, instr));
        }
        else
        {
            printf("    %u: %s\n", i, instruction_assembly_form(i, instr));
        }
    }
}

void vm_print_ls_words(VM *vm)
{
    // Almost like vm_print_tracing_words, but doesn't print the last memory address

    word_type address = vm->registers[0];  // Correct access to registers array
    int count = 0;  // Use a counter for formatting
    printf("    ");

    // Loop through memory up to the stack pointer (registers[1] = $sp)
    while (address < vm->registers[2]) {
        // Print memory address and value
        printf("%d: %d\t", address, vm->memory[address]);
        count++;
        
        // Print a newline every 4 values for readability
        if (count % 5 == 0)
        {
            printf("\n");
            printf("    ");
        }
        
        // Detect blocks of zeros and print "..."
        if (address < vm-> registers[2] - 2 && vm->memory[address] == 0 && vm->memory[address + 1] == 0) {
            printf("...         ");
            // Skip over consecutive zeros
            while (vm->memory[address] == 0 && vm->memory[address + 1] == 0 && address < vm->registers[2] - 1) {
                address++;
            }
        }
        
        // Move to the next memory address
        address++;
    }

    // print new line
    printf("\n");
}

void vm_print_program(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = vm->pc; i < vm->text_length; i++)
    {
        bin_instr_t instr = *(bin_instr_t *)&vm->memory[i];

        if (instruction_type(instr) == error_instr_type)
            break;

        // if statements to help provide correct spacing
        if (i < 10)
        {
            printf("     %u: %s\n", i, instruction_assembly_form(i, instr));
        }
        else
        {
            printf("    %u: %s\n", i, instruction_assembly_form(i, instr));
        }
    }

    vm_print_ls_words(vm);
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