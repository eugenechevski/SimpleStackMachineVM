#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"
#include <string.h>
#define STACK_SIZE 4096
#define DATA_SIZE 1024
// TODO: Implement VM functions here

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file.asm>\n", argv[0]);
        return 1;
    }
    int *stack = malloc(STACK_SIZE * sizeof(int));
    if (!stack)
    {
        printf("Error: Could not allocate memory for stack\n");
        return 1;
    }
    int sp = STACK_SIZE;
    free(stack);
    return 0;
}