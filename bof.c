#include "bof.h"
#include "utilities.h"
#include <stdlib.h>

FILE *bof_read_open(const char *filename)
{
    FILE *bf = fopen(filename, "rb");

    // Check if the file was successfully opened
    if (bf == NULL)
    {
        bail_with_error(filename);
        return NULL;
    }

    return bf;
}

BOFHeader *bof_read_header(FILE *file)
{
    // TODO: Implement this function
    return NULL;
}

word_type bof_read_word(FILE *file)
{
    // TODO: Implement this function
    return 0;
}