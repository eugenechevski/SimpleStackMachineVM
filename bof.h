/* $Id: bof.h,v 1.22 2024/08/20 00:46:41 leavens Exp $ */
// Binary Object File Format (for the SSM)
#ifndef _BOF_H
#define _BOF_H
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "machine_types.h"

#define MAGIC_BUFFER_SIZE 4

typedef struct { // Field magic should hold value of MAGIC (with no null char)
    char      magic[MAGIC_BUFFER_SIZE];
    word_type text_start_address;  // word address to start running (PC)
    word_type text_length;         // size of the text section in words
    word_type data_start_address;  // word address of static data (GP)
    word_type data_length;         // size of data section in words
    word_type stack_bottom_addr;   // word address of stack "bottom" (FP)
} BOFHeader;

// a type for Binary Output Files
typedef struct {
    FILE *fileptr;
    const char *filename;
} BOFFILE;

// Open filename for reading as a binary file
// Exit the program with an error if this fails,
// otherwise return the FILE pointer to the open file.
extern BOFFILE bof_read_open(const char *filename);

// Return the size (in bytes) of bf
extern size_t bof_file_bytes(BOFFILE bf);

// Return true just when bf is at its end, false otherwise
extern bool bof_at_eof(BOFFILE bf);

// Requires: bf is open for reading in binary and has
//           at least 1 word (2 bytes) available to read
// Return the next word from bf
extern word_type bof_read_word(BOFFILE bf);

// Requires: bf is open for reading in binary and
// buf is of size at least bytes
// Read the given number of bytes into buf and return the number of bytes read
size_t bof_read_bytes(BOFFILE bf, size_t bytes, void *buf);

// Requires: bf is open for reading in binary
// Read the header of bf as a BOFHeader and return that header
// If any errors are encountered, exit with an error message.
extern BOFHeader bof_read_header(BOFFILE);

// Open filename for writing as a binary file
// Exit the program with an error if this fails,
// otherwise return the BOFFILE for it.
extern BOFFILE bof_write_open(const char *filename);

// Requres: bf is open
// Close the given binary file
// Exit the program with an error if this fails.
extern void bof_close(BOFFILE bf);

// Requires: bf is open for writing in binary
// Write the given word into bf.
// Exit the program with an error if this fails.
extern void bof_write_word(BOFFILE bf, word_type w);

// Requires: bf is open for writing in binary
//           and the size of buf is at least bytes
// Write the given number of bytes from buf into f.
// Exit the program with an error if this fails.
extern void bof_write_bytes(BOFFILE bf, size_t bytes,
			    const void *buf);

// Requires: bf is open for writing in binary
// Write the given header to f
// Exit the program with an error if this fails.
extern void bof_write_header(BOFFILE bf, const BOFHeader hdr);

// Write the (bits of the) magic number into the header *bh.
extern void bof_write_magic_to_header(BOFHeader *bh);

// Does the given header have the appropriate magic number?
bool bof_has_correct_magic_number(BOFHeader bh);

// The following line is for the manual (i.e., the document itself)y
// ...
#endif
