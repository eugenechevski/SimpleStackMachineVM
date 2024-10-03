// Libraries that will be used in the main c file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"

// The declaration of helper functions to get memory address
static inline word_type *get_memory_address_signed(VM *vm, reg_num_type reg, offset_type offset)
{
    return &vm->memory.words[vm->registers[reg] + machine_types_formOffset(offset)];
}

static inline uword_type *get_memory_address_unsigned(VM *vm, reg_num_type reg, offset_type offset)
{
    return &vm->memory.uwords[vm->registers[reg] + machine_types_formOffset(offset)];
}

static inline word_type *get_p_signed(VM *vm, reg_num_type reg)
{
    return &vm->memory.words[vm->registers[reg]];
}

static inline uword_type *get_p_unsigned(VM *vm, reg_num_type reg)
{
    return &vm->memory.uwords[vm->registers[reg]];
}

// Definition of the main struc object that defines how the Virtual machine will store its elements
void vm_init(VM *vm)
{
    memset(&vm->memory, 0, sizeof(vm->memory));
    memset(vm->registers, 0, sizeof(vm->registers));
    vm->pc = 0;
    vm->hi = 0;
    vm->lo = 0;
    vm->data_length = 0;
    vm->text_length = 0;
}

// Definition and  description of the process of  loading a program into a virtual machine (VM) from a specified binary object file (BOF)
void vm_load_program(VM *vm, BOFFILE bf)
{
    // First of it reads the header of the binary object file using the header below
    // the header will provide the VM lengths and starting addresses of the text and data segments
    BOFHeader header = bof_read_header(bf);
    vm->text_length = header.text_length;
    vm->data_length = header.data_length;

    // Load instructions
    // For loop that iterates over the range defined by header.text_length, reading each instruction from the binary file  and storing it in the VM's memory
    for (int i = 0; i < header.text_length; i++)
    {
        vm->memory.words[header.text_start_address + i] = bof_read_word(bf);
    }

    // Load data
    // Same concept with the for loop above, but it reads the data from the binary file and stores it in memory starting
    for (int i = 0; i < header.data_length; i++)
    {
        vm->memory.words[header.data_start_address + i] = bof_read_word(bf);
    }

    // Set initial register values
    vm->pc = header.text_start_address;
    vm->registers[0] = header.data_start_address; // $gp
    vm->registers[1] = header.stack_bottom_addr;  // $sp
    vm->registers[2] = header.stack_bottom_addr;  // $fp
}

// Function below is designed to print the contents of memory words starting from the stack pointer ($sp) register of a virtual machine (VM) up to the frame pointer ($fp) register
void print_words_from_sp(VM *vm)
{
    int printed =0;
    word_type address = vm->registers[1];
    int count = 0;
    printed += printf("   ");

    /// While loop that continues to iterate, until the address reached is above the frame pointer
    while (address <= vm->registers[2])
    {
        // Printing the current address and its corresponsive memory value
        printed += printf("%3d: %-10d", address, vm->memory.words[address]);
        count++;

        // If statement to set the format that the information is been display to the user
        if (printed>70)
        {
           printed = printf("\n   ");
        }
        // If statement   that checks for  zeros in memory. If two consecutive words are zero and the current address is not at the end of the memory section, it prints ellipsis ("...")
        if (address < vm->registers[2] - 1 && vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0)
        {
           printed += printf("...         ");
            // The use of the while loop is to check for two consecutive words are zero and if the current addres is not at the end.Skips if true by just adding 1 to address
            while (vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0 && address <= vm->registers[2])
            {
                address++;
            }
        }

        address++;
    }

    // printf("%3d: %-10d", address, vm->memory.words[address]);

    printf("\n");
}
// The function belows prints the contents of a specified range of memory words in a virtual machine (VM)
//  A note to point out is that, this funciton Starts from the address in the global pointer register (`$gp`)
//  The while loops and condition works similar to the previous function
void vm_print_tracing_words(VM *vm)
{
    word_type address = vm->registers[0];
    int printed = 0;
    int count = 0;
    printed += printf("   ");

    while (address < vm->registers[1])
    {
        printed += printf("%3d: %-10d", address, vm->memory.words[address]);
        count++;

        if (printed>70)
        {
           printed = printf("\n   ");
        }

        if (address < vm->registers[1] - 2 && vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0)
        {
           printed+= printf("...         ");
            while (vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0 && address < vm->registers[2] - 1)
            {
                address++;
            }
        }

        address++;
    }

    printf("\n");
    // printf("%3d: %d\n\n", address, vm->memory.words[address]);
    print_words_from_sp(vm);
}

// This function works to print the current state of the VM
void vm_print_state(VM *vm)
{

    // If statement to checks the values of the HI and LO registers
    if (vm->hi != 0 || vm->lo != 0)
    {
        printf("      PC: %u      HI: %d\tLO: %d", vm->pc, vm->hi, vm->lo);
    }
    // only prints the program counter
    else
    {
        printf("      PC: %u", vm->pc);
    }
    printf("\n");
    // For loop to print name and value of each register
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("GPR[%s]: %-6d", regname_get(i), vm->registers[i]);
        if ((i + 1) % 5 == 0)
            printf("\n");
    }
    printf("\n");

    // Function call to print the from the GP memory and the rest of the state of the VM
    vm_print_tracing_words(vm);
}
// Function to do the math calculation called, two's complement
uword_type twosComplement(uword_type target)
{
    uword_type toConvert = target;
    target = ~toConvert + 1;
    return target;
}
// Funciton executes a computational instruction for the VM
// It performs a variety of arithmetic and logical operations based on the provided instruction instr
void vm_execute_comp_instr(VM *vm, comp_instr_t instr)
{
    // Target Memory Address for Signed Value
    word_type *target_signed = get_memory_address_signed(vm, instr.rt, instr.ot);
    // Target Memory Address for Unsigned Value
    uword_type *target_unsigned = get_memory_address_unsigned(vm, instr.rt, instr.ot);
    // Stack Pointer Address for Signed Value
    word_type *sp_signed = get_p_signed(vm, 1);
    // Stack Pointer Address for Unsigned Value
    uword_type *sp_unsigned = get_p_unsigned(vm, 1);
    // Source Operand with Offset for Signed Value
    word_type *s_os_signed = get_memory_address_signed(vm, instr.rs, instr.os);
    // Source Operand with Offset for Unsigned Value
    uword_type *s_os_unsigned = get_memory_address_unsigned(vm, instr.rs, instr.os);

    switch (instr.func)
    {
        // Performs no operation and simply breaks out of the switch.
    case NOP_F:
        break;
        // Adds the value at the stack pointer to the source operand with offset and stores the result in the target address
    case ADD_F:
        *target_signed = *sp_signed + *s_os_signed;
        break;
        // Subtracts the value from the source operand with offset from the value at the stack pointer and stores the result in the target address.
    case SUB_F:
        *target_signed = *sp_signed - *s_os_signed;
        break;
        // Copies the value from the source operand with offset to the target address.
    case CPW_F:
        *target_signed = *s_os_signed;
        break;
        // Performs a bitwise AND operation between the unsigned values at the stack pointer and the source operand
    case AND_F:
        *target_unsigned = *sp_unsigned & *s_os_unsigned;
        break;
    case BOR_F:
        *target_unsigned = *sp_unsigned | *s_os_unsigned;
        break;
        // Performs a bitwise NOR operation
    case NOR_F:
        *target_unsigned = ~(*sp_unsigned | *s_os_unsigned);
        break;
        // Performs a bitwise XOR operation
    case XOR_F:
        *target_unsigned = *sp_unsigned ^ *s_os_unsigned;
        break;
        // Loads rt
    case LWR_F:
        vm->registers[instr.rt] = *s_os_signed;
        break;
        // Stores the value from the register specified by rs into the target address
    case SWR_F:
        *target_signed = vm->registers[instr.rs];
        break;
        // Computes an address by adding the value in the register rs to an offset and stores the result in the target address.
    case SCA_F:
        *target_signed = vm->registers[instr.rs] + machine_types_formOffset(instr.os);
        break;
        // It assigns the value stored in memory at the address of `*s_os_signed` to the location pointed to by `*target_signed`
    case LWI_F:
        *target_signed = vm->memory.words[*s_os_signed];
        break;
        // Negates the value from the source operand with offset and stores the result in the target address
    case NEG_F:
        *target_signed = -(*s_os_signed);
        break;
        // If the instruction is not recognized
        // It is used as our error message
    default:
        bail_with_error("Unknown computational instruction function: %d", instr.func);
    }
}
// The function below, it executes additional computational instructions for the VM.
// It handles a variety of operations, including arithmetic, logical shifts, jumps, and managing special registers for high and low values
void vm_execute_other_comp_instr(VM *vm, other_comp_instr_t instr)
{
    word_type *target_signed = get_memory_address_signed(vm, instr.reg, instr.offset);
    uword_type *target_unsigned = get_memory_address_unsigned(vm, instr.reg, instr.offset);
    word_type *sp_signed = get_p_signed(vm, 1);
    uword_type *sp_unsigned = get_p_unsigned(vm, 1);

    union
    {
        int64_t long_val;
        struct
        {
            int32_t low;
            int32_t high;
        } parts;
    } result;

    // Use of switch cases to do some instructions
    // Multiplication, Divition
    switch (instr.func)
    {
        // loads an immediate (literal) value into the memory location pointed to by target_signed
    case LIT_F:
        *target_signed = machine_types_sgnExt(instr.arg);
        break;
        // performs an addition on the register indexed
    case ARI_F:
        vm->registers[instr.reg] += machine_types_sgnExt(instr.arg);
        break;
        // his case performs a subtraction on the register indexed by `instr.reg`(register field in the instruction)
    case SRI_F:
        vm->registers[instr.reg] -= machine_types_sgnExt(instr.arg);
        break;
        // case multiplies the values in `*sp_signed` and `*target_signed`, storing the result in a 64-bit temporary
    case MUL_F:
        result.long_val = (int64_t)(*sp_signed) * (int64_t)(*target_signed);
        vm->hi = result.parts.high;
        vm->lo = result.parts.low;
        break;
        // This case performs a division of `*sp_signed` by `*target_signed`
    case DIV_F:
        vm->lo = *sp_signed / *target_signed;
        vm->hi = *sp_signed % *target_signed;
        break;
        // case copies the value of `vm->hi` to the memory location pointing into target_signed
    case CFHI_F:
        *target_signed = vm->hi;
        break;
        // case copies the value of `vm->lo` to the memory location pointing into target_signed
    case CFLO_F:
        *target_signed = vm->lo;
        break;
        // case performs a logical left shift on `*sp_unsigned` by the number of bits specified by the instruciton argument
    case SLL_F:
        *target_unsigned = *sp_unsigned << instr.arg;
        break;
        // //case performs a logical right shift on `*sp_unsigned` by the number of bits specified by the instruciton argument
    case SRL_F:
        *target_unsigned = *sp_unsigned >> instr.arg;
        break;
        // This case jumps to the address stored in unsigned target
    case JMP_F:
        vm->pc = *target_unsigned;
        break;
        // call instruction, storing the current program counter in register 7(text_length) before jumping to the target address.
    case CSI_F:
        vm->registers[7] = vm->pc;
        vm->pc = *target_signed;
        break;
        // case performs a relative jump, adjusting the program counter based on the immediate offset(the instruction argument)
    case JREL_F:
        vm->pc = ((vm->pc - 1) + machine_types_formOffset(instr.arg));
        break;
        // Deffault error message when an instr is an invalid option
    default:
        bail_with_error("Unknown other computational instruction function: %d", instr.func);
    }
}
// The function below responsible for handling jump and call instructions in the VM
// It updates the PC based on the specified jump operation, facilitating control flow changes with the CALL_O and returning from them using RTN_O
void vm_execute_jump_instr(VM *vm, jump_instr_t instr)
{
    switch (instr.op)
    {
    case JMPA_O:
        vm->pc = machine_types_formAddress(vm->pc - 1, instr.addr);
        break;
    case CALL_O:
        vm->registers[RA] = vm->pc;
        vm->pc = machine_types_formAddress(vm->pc - 1, instr.addr);
        break;
    case RTN_O:
        vm->pc = vm->registers[RA];
        break;
    default:
        bail_with_error("Unknown jump instruction opcode: %d", instr.op);
    }
}
// The instruction below is designed to execute immediate instructions within the VM
// instructions  operations that involve immediate values, allowing for arithmetic operations, logical operations, and conditional branching based on immediate data.
void vm_execute_immed_instr(VM *vm, immed_instr_t instr)
{
    word_type *target_signed = get_memory_address_signed(vm, instr.reg, instr.offset);
    uword_type *target_unsigned = get_memory_address_unsigned(vm, instr.reg, instr.offset);

    switch (instr.op)
    {
        // Adds the sign-extended immediate value (`instr.immed`) store in the target_signed
    case ADDI_O:
        *target_signed += machine_types_sgnExt(instr.immed);
        break;
        // Performs a bitwise AND between the zero-extended immediate value and the unsigned target
    case ANDI_O:
        *target_unsigned &= machine_types_zeroExt(instr.immed);
        break;
        // Performs a bitwise OR between like the one above.  `0xFFFF` to limit the immediate to 16 bits. This simulates an OR operation with a constant value
    case BORI_O:
        *target_unsigned |= machine_types_zeroExt(instr.immed) & 0xFFFF;
        break;
        // Computes the bitwise NOR effectively flipping the bits of the OR result
    case NORI_O:
        *target_unsigned = ~(*target_unsigned | machine_types_zeroExt(instr.immed));
        break;
        // Performs a bitwise XOR
    case XORI_O:
        *target_unsigned ^= machine_types_zeroExt(instr.immed) & 0xFFFF;
        break;
        // it updates the program counter (`vm->pc`) to branch to the instruction offset.This creates conditional jump
    case BEQ_O:
        if (*get_p_signed(vm, SP) == *target_signed)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;

        // the program counter  is updated to branch to the immediate offset. This tests for non-negative values and conditionally jumps
    case BGEZ_O:
        if (*target_signed >= 0)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;
        // the program counter  is updated to branch to the immediate offset. This is a conditional jump based on the positive value of the target
    case BGTZ_O:
        if (*target_signed > 0)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;
        // program counter  is updated by the immediate offset value, causing a branch
    case BLEZ_O:
        if (*target_signed <= 0)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;
        // Similar to the option above, but this creates a conditional jump for negative values
    case BLTZ_O:
        if (*target_signed < 0)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;
        // This performs a conditional jump if the values differ
    case BNE_O:
        if (*get_p_signed(vm, SP) != *target_signed)
        {
            vm->pc = (vm->pc - 1) + machine_types_formOffset(instr.immed);
        }
        break;
    default:
        bail_with_error("Unknown immediate instruction opcode: %d", instr.op);
    }
}
// The function below is designed to do the terminating instructions/commands from the VM
// Terminating the execution of the program or stopping the VM from tracing( printing the state of the VM)
int vm_execute_syscall(VM *vm, bool *tracing, syscall_instr_t instr)
{
    // This case causes the virtual machine to exit
    if (instr.code == 1)
    {
        return 1; // Exit the VM
    }
    // This case prints a string to the console
    else if (instr.code == 2)
    {
        word_type *sp = get_p_signed(vm, 1);
        char *str = (char *)get_memory_address_signed(vm, instr.reg, instr.offset);
        *sp = printf("%s", str);
    }
    // This case prints an integer  value; it is retrieved from memory using the register and offset
    else if (instr.code == 3) // PINT instruction
    {
        word_type *sp = get_p_signed(vm, 1);
        int value = *(get_memory_address_signed(vm, instr.reg, instr.offset));
        *sp = printf("%d", value);
    }
    // case prints a single character to the console, it fetches a character from memory and the character is the output
    else if (instr.code == 4)
    {
        word_type *sp = get_p_signed(vm, 1);
        int ch = *(get_memory_address_signed(vm, instr.reg, instr.offset));
        *sp = fputc(ch, stdout);
    }
    // case reads a character from standard input
    else if (instr.code == 5)
    {
        word_type *target = get_memory_address_signed(vm, instr.reg, instr.offset);
        *target = getc(stdin);
    }
    // VM will print debugging information such as the current state or instruction being executed
    else if (instr.code == 2046)
    {
        *tracing = true;
    } // Instruction option that will hide the current state or instruction being executed. Until the flag gets changed again
    else if (instr.code == 2047)
    {
        *tracing = false;
    }

    return 0;
}
// This is the menu function of the VM
// It manages and executes the instruction cycle of the VM
// The function  fetches, decodes, and executes instructions of the memory of the VM until a syscall/EXIT instruction gets called
void vm_run(VM *vm)
{
    bool tracing = true;
    // All of the VM running process is based and enclosed in a while loop
    while (1)
    {
        bin_instr_t instr = vm->memory.instrs[vm->pc];
        /*If statement where if true we print the current state of the virtual machine
        if false we don't print the current state of the VM to the user
        The tracing variable can get change as it reads instructions*/
        if (tracing)
        {
            vm_print_state(vm);
            printf("==>      %u: %s\n", vm->pc, instruction_assembly_form(vm->pc, instr));
        }
        // Increase the program counter BEFORE we execute the instruction
        vm->pc++;

        // Switch statement that encloses and calls the right function, based on the type of the instruction  it is reading
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

/*Void function to print the address of the current instruction.It basically  prints user friendly  list of the instructions loaded in the VM's memory*/
void vm_print_instructions(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = 0; i < vm->text_length; i++)
    {
        bin_instr_t instr = vm->memory.instrs[i];

        if (instruction_type(instr) == error_instr_type)
            break;
        // extra padding for single-digit addresses for consistent formatting
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
    word_type address = vm->registers[0];
    int count = 0;
    printf("    ");
    // While loop to encapsulate the entire function
    // Stop iterating before reaching the FP
    while (address < vm->registers[2])
    {
        // Printing each memory word at the current address, it prints the address and its associated
        printf("%d: %d\t", address, vm->memory.words[address]);
        count++;

        if (count % 5 == 0)
        {
            printf("\n");
            printf("    ");
        }
        // If statement to handle zero sequences , the function moves to the next memory address by incrementing if the statement is true and while the true statement in the while loop is true too
        if (address < vm->registers[2] - 2 && vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0)
        {
            printf("...         ");
            while (vm->memory.words[address] == 0 && vm->memory.words[address + 1] == 0 && address < vm->registers[2] - 1)
            {
                address++;
            }
        }

        address++;
    }

    printf("\n");
}

void vm_print_program(VM *vm)
{
    printf("Address Instruction\n");
    for (unsigned int i = vm->pc; i < vm->text_length; i++)
    {
        bin_instr_t instr = vm->memory.instrs[i];

        if (instruction_type(instr) == error_instr_type)
            break;

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
    // function first checks if at least one argument is provided  is the BOF file
    if (argc < 2)
    {
        bail_with_error("Usage: %s [-p] <bof_file>", argv[0]);
    }

    bool print_mode = false;
    char *filename = argv[1];
    // If statement to where the second argument is -p, the program is set to print mode
    if (argc == 3 && strcmp(argv[1], "-p") == 0)
    {
        print_mode = true;
        filename = argv[2];
    }
    // BOF file is opened for reading using the function below
    BOFFILE bf = bof_read_open(filename);

    VM vm;
    // Initialization of the Virtual machine
    vm_init(&vm);
    // binary program from the BOF file is loaded into the VM’s memory using
    vm_load_program(&vm, bf);

    // If statement(where if true), the program prints the program instructions) if the user invoked the -p option.
    if (print_mode)
    {
        vm_print_program(&vm);
    }
    // Else executes the program
    else
    {
        vm_run(&vm);
    }
    // Close the BOF file
    bof_close(bf);
    return 0;
} // End of code    