#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vm.h"
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include <string.h>
#define STACK_SIZE 4096
#define DATA_SIZE 1024
// TODO: Implement VM functions here
static void usage(const char *cmdname)
{
    fprintf(stderr, "Usage: %s file.bof\n", cmdname);
    fprintf(stderr, "   or: %s -p file.bof\n", cmdname);
    bail_with_error(cmdname);
}

int main(int argc, char *argv[])
{
    const char *cmdname = argv[0];
    argc--;
    argv++;

    bool print_program = false;
    if (argc == 2 && strcmp(argv[0], "-p") == 0)
    {
        print_program = true;
        argc--;
        argv++;
    }

    /*if (print_program && should_trace)
{
    bail_with_error("Cannot both print the program (with -p) and trace it (with -t)!");
}*/

    // now there should be exactly 1 file argument
    if (argc != 1 || argv[0][0] == '-')
    {
        usage(cmdname);
    }

    char *suffix = strchr(argv[0], '.');
    if (suffix == NULL || strcmp(suffix, ".bof") != 0)
    {
        usage(cmdname);
    }
    printf("Something??\n");

    FILE *bf;
    bf = bof_read_open(argv[0]);
    // BOFHeader bf = bof_read_open(argv[0]);

    /*machine_load(bf);

    // if printing, don't run the program
    if (print_program)
    {
        machine_print_loaded_program(stdout);
        return EXIT_SUCCESS;
    }

    machine_run(should_trace);*/
}