#ifndef _BOF_H
#define _BOF_H

#include <stdio.h>
#include <stdint.h>
#include "machine_types.h"

typedef struct
{
    char magic[4];
    word_type text_start_address;
    word_type text_length;
    word_type data_start_address;
    word_type data_length;
    word_type stack_bottom_addr;
} BOFHeader;

/*typedef struct
{
    FILE *fileptr;
    const char *filename;
} BOFFILE;*/

FILE *bof_read_open(const char *filename);
BOFHeader *bof_read_header(FILE *file);
word_type bof_read_word(FILE *file);

#endif