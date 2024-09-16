/* $Id: bof.c,v 1.19 2024/07/28 22:01:51 leavens Exp $ */
// #include <sys/types.h>
#include <sys/stat.h>
// #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bof.h"
#include "utilities.h"

#define MAGIC "BO32"

// a type for treating bytes as a word
typedef union {
    unsigned char buf[BYTES_PER_WORD];
    word_type w;
} word_pun_t;

// Open filename for reading as a binary file
// Exit the program with an error if this fails,
// otherwise return the BOFFILE struct for the file
BOFFILE bof_read_open(const char *filename) {
    BOFFILE bf;
    bf.fileptr = fopen(filename, "rb");
    bf.filename = filename;

    if (bf.fileptr == NULL) {
	bail_with_error("Error opening file for reading: %s", filename);
    }
    
    return bf;
}

// Return the size (in bytes) of bf
size_t bof_file_bytes(BOFFILE bf)
{
    struct stat st;
    if (stat(bf.filename, &st) < 0) {
	bail_with_error("Cannot stat %s to get its size!", bf.filename);
    }
    ;
    return st.st_size;
}

// Return true just when bf is at its end, false otherwise
bool bof_at_eof(BOFFILE bf) {
    return feof(bf.fileptr);
}

// Requires: bf is open for reading in binary and has
//           at least BYTES_PER_WORD bytes in it
// Return the next word from bf
word_type bof_read_word(BOFFILE bf)
{
    word_pun_t b;
    assert(!bof_at_eof(bf));
    size_t bytes_read = bof_read_bytes(bf, BYTES_PER_WORD, b.buf);
    if (bytes_read == 0) {
	bail_with_error(
	  "Cannot read a word from %s (got %d bytes), at EOF: %d",
	  bf.filename, bytes_read, bof_at_eof(bf));
    }
    return b.w;
}

// Requires: bf.fileptr is open for reading in binary
// and buf is of size at least bytes
// Read the given number of bytes into buf and return the number of bytes read
size_t bof_read_bytes(BOFFILE bf, size_t bytes, void *buf) {
    int elems_read = fread(buf, bytes, 1, bf.fileptr);
    return elems_read;
}

// Requires: bf is open for reading in binary
// Read the header of bf as a BOFHeader and return that header
// If any errors are encountered, exit with an error message.
BOFHeader bof_read_header(BOFFILE bf) {
    BOFHeader ret;
    size_t rd = fread(&ret, sizeof(ret), 1, bf.fileptr);
    if (rd != 1) {
	bail_with_error("Cannot read header from %s", bf.filename);
    }
    if (!bof_has_correct_magic_number(ret)) {
	bail_with_error("Wrong magic number code in file '%s'!",
			bf.filename);
    }
    return ret;
    /*
    bof_read_bytes(bf, MAGIC_BUFFER_SIZE, &ret.magic);
    if (strncmp(ret.magic, MAGIC, MAGIC_BUFFER_SIZE) != 0) {
	bail_with_error("File %s is not a BOF format file, bad magic number!",
			bf.filename);
    }
    bof_read_bytes(bf, BYTES_PER_WORD, &ret.text_start_address);
    bof_read_bytes(bf, BYTES_PER_WORD, &ret.text_length);
    bof_read_bytes(bf, BYTES_PER_WORD, &ret.data_start_address);
    bof_read_bytes(bf, BYTES_PER_WORD, &ret.data_length);
    bof_read_bytes(bf, BYTES_PER_WORD, &ret.stack_bottom_addr);
    return ret;
    */
}

// Requires: f is open for writing
// Write the magic number in hexadecimal notation on f, followed by a newline;
// Note: this is just for help in writing the documentation
/*
void bof_write_magic(FILE *f) {
    fprintf(f, "%s", "0x");
    for (int i = 0; i < MAGIC_BUFFER_SIZE; i++) {
	fprintf(f, "%x", MAGIC[i]);
    }
    fprintf(f, "\n");
}
*/

// Open filename for writing as a binary file
// Exit the program with an error if this fails,
// otherwise return the BOFFILE for it.
BOFFILE bof_write_open(const char *filename) {
    BOFFILE bf;
    bf.fileptr = fopen(filename, "wb");
    bf.filename = filename;

    if (bf.fileptr == NULL) {
	bail_with_error("Error opening file for writing: %s", filename);
    }
    
    return bf;
}

// Requres: bf is open
// Close the given binary file
// Exit the program with an error if this fails.
void bof_close(BOFFILE bf)
{
    if (fclose(bf.fileptr) != 0) {
	bail_with_error("Could not close %s", bf.filename);
    }
}


// Requires: bf is open for writing in binary.
// Write the given word into bf
// Exit the program with an error if this fails.
extern void bof_write_word(BOFFILE bf, word_type w)
{
    word_pun_t b;
    b.w = w;
    bof_write_bytes(bf, BYTES_PER_WORD, b.buf);
}

// Requires: bf is open for writing in binary
//           and the size of buf is at least bytes
// Write the given number of bytes from buf into f.
// Exit the program with an error if this fails.
void bof_write_bytes(BOFFILE bf, size_t bytes,
		     const void *buf) {
    size_t wr = fwrite(buf, bytes, 1, bf.fileptr);
    if (wr != 1) {
	bail_with_error("Cannot write %u bytes to %s", bytes, bf.filename);
    }
}

// Requires: bf is open for writing in binary
// Write the given header to f
// Exit the program with an error if this fails.
void bof_write_header(BOFFILE bf, const BOFHeader hdr) {
    size_t wr = fwrite(&hdr, sizeof(BOFHeader), 1, bf.fileptr);
    if (wr != 1) {
	bail_with_error("Canot write header to %s", bf.filename);
    }
}

// Write the (bits of the) magic number into the header bh.
void bof_write_magic_to_header(BOFHeader *bh)
{
    const char *magic = MAGIC;
    for (int i = 0; i < MAGIC_BUFFER_SIZE; i++) {
	bh->magic[i] = magic[i];
    }
    assert(bof_has_correct_magic_number(*bh));
}

// Does the given header have the appropriate magic number?
bool bof_has_correct_magic_number(BOFHeader bh)
{
    char buf[MAGIC_BUFFER_SIZE+1];
    for (int i = 0; i < MAGIC_BUFFER_SIZE; i++) {
	buf[i] = bh.magic[i];
    }
    buf[MAGIC_BUFFER_SIZE] = '\0';
    return (0 == strncmp(buf, MAGIC, MAGIC_BUFFER_SIZE));
}
