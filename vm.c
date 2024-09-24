#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "utilities.h"

void vm_init(VM *vm) {
    // Initialize memory and registers to 0
    for (int i = 0; i < MEMORY_SIZE_IN_WORDS; i++) {
        vm->memory[i] = 0;
    }
    for (int i = 0; i < NUM_REGISTERS; i++) {
        vm->registers[i] = 0;
    }
    vm->pc = 0;
    vm->hi = 0;
    vm->lo = 0;
}

void vm_load_program(VM *vm, BOFFILE bf) {
    BOFHeader header = bof_read_header(bf);
    
    // Load instructions
    for (int i = 0; i < header.text_length; i++) {
        vm->memory[i] = bof_read_word(bf);
    }
    
    // Load data
    for (int i = 0; i < header.data_length; i++) {
        vm->memory[header.data_start_address + i] = bof_read_word(bf);
    }
    
    // Set initial register values
    vm->pc = header.text_start_address;
    vm->registers[0] = header.data_start_address; // $gp
    vm->registers[1] = header.stack_bottom_addr;  // $sp
    vm->registers[2] = header.stack_bottom_addr;  // $fp
}

void vm_run(VM *vm) {
    while (1) {
        // Fetch instruction
        bin_instr_t instr = *(bin_instr_t*)&vm->memory[vm->pc];
        
        // Increment PC
        vm->pc++;
        
        // Decode and execute instruction
        switch (instruction_type(instr)) {
            case comp_instr_type:
                // Handle computational instructions
                break;
            case other_comp_instr_type:
                // Handle other computational instructions
                break;
            case immed_instr_type:
                // Handle immediate instructions
                break;
            case jump_instr_type:
                // Handle jump instructions
                break;
            case syscall_instr_type:
                // Handle system call instructions
                if (instr.syscall.code == exit_sc) {
                    return; // Exit the VM
                }
                break;
            default:
                bail_with_error("Unknown instruction type");
        }
    }
}

void vm_print_program(VM *vm) {
    // Print the loaded program
    printf("Address Instruction\n");
    for (int i = 0; vm->memory[i] != 0; i++) {
        bin_instr_t instr = *(bin_instr_t*)&vm->memory[i];
        printf("%d: %s\n", i, instruction_assembly_form(i, instr));
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        bail_with_error("Usage: %s [-p] <bof_file>", argv[0]);
    }

    bool print_mode = false;
    char *filename = argv[1];

    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        print_mode = true;
        filename = argv[2];
    }

    BOFFILE bf = bof_read_open(filename);
    VM vm;
    vm_init(&vm);
    vm_load_program(&vm, bf);

    if (print_mode) {
        vm_print_program(&vm);
    } else {
        vm_run(&vm);
    }

    bof_close(bf);
    return 0;
}