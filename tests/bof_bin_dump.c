#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bof.h"
#include "machine_types.h"
#include "utilities.h"

#define BITS_PER_BYTE 8
#define BUFF_SIZE (BITS_PER_BYTE*sizeof(word_type) + 1)

/* Requires: buff has size at least (BITS_PER_BYTE*sizeof(word_type)+1).
   Put binary digit characters corresponding to val into buff
   Modified from https://stackoverflow.com/questions/699968/display-the-binary-representation-of-a-number-in-c
*/
static const char *binrep (unsigned int val, char *buff) {
    char *pbuff = buff;
    int bit_sz = BITS_PER_BYTE * sizeof(word_type);  // bit size

    /* Work from the end of the buffer back. */
    pbuff += bit_sz;
    *pbuff-- = '\0';

    /* For each bit (going backwards) store character. */
    while (1) {
        if (bit_sz-- < 0) { // when previous bit size was 0, so did 0th bit
	    return buff;
	}
        *pbuff-- = ((val & 1) ? '1' : '0');
        /* Get next bit. */
        val >>= 1;
    }
}


// Print a usage message on stderr and exit with exit code 1.
static void usage(const char *cmdname)
{
    bail_with_error("Usage: %s file.bof", cmdname);
}


int main(int argc, char *argv[]) {
    if (argc >= 2) {
	BOFFILE bf = bof_read_open(argv[1]);
	size_t siz = bof_file_bytes(bf); // size in bytes
	char *suffix = strchr(argv[1], '.');
	if (suffix == NULL || strncmp(suffix, ".bof", 4) != 0) {
	    usage(argv[0]);
	}

	char bits_buff[BUFF_SIZE];
	printf("Addr\t= Binary\t\t\t\t= Unsigned\t= Signed\t= Hexadecimal\n");
	for (address_type a = 0;
	    a*BYTES_PER_WORD < siz && !bof_at_eof(bf); a++) {
	    if (a == sizeof(BOFHeader) / BYTES_PER_WORD) {
		printf("---- end of header ----\n");
	    }
	    word_type w = bof_read_word(bf);
	    printf("%hu:", a / BYTES_PER_WORD);
	    printf("\t= %s", binrep((unsigned int) w,
				      bits_buff));
	    printf("\t= %10u", w);
	    printf("\t= %10d", w);
	    printf("\t= 0x%x\n", w);
	}
    } else {
	bail_with_error("Cannot read %s!", argv[1]);
    }
    return EXIT_SUCCESS;
}
