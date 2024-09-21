#ifndef _MACHINE_TYPES_H
#define _MACHINE_TYPES_H

#include <stdint.h>

#define BYTES_PER_WORD 4
#define MEMORY_SIZE_IN_WORDS 32768

typedef int32_t word_type;
typedef uint32_t uword_type;
typedef int16_t offset_type;
typedef uint8_t opcode_type;
typedef uint8_t reg_num_type;
typedef uint8_t func_type;

// machine words, assuming ints are 32 bits
typedef int word_type;

// unsigned machine words
typedef unsigned int uword_type;

// addresses
typedef unsigned int address_type;

#endif