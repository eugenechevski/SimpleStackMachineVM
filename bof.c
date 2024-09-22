#include "bof.h"
#include "utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FILE *bof_read_open(const char *filename)
{
    FILE *bf = fopen(filename, "rb");

    /* Check if the file was successfully opened*/
    if (bf == NULL)
    {
        bail_with_error(filename);
        return NULL;
    }

    /*return bf;*/
    BOFHeader *header = bof_read_header(bf);
    if (!header)
    {
        fclose(bf);
        return NULL;
    }

    /* Now you can use the header information, e.g., print it or store it*/
    /*printf("Magic: %.4s, Data Size: %u, Stack Size: %u\n",
           header->magic, header->data_start_address, header->text_start_address);*/

    /* Don't forget to free the header when done*/
    printf("Succesfully read header: Magic: %.4s\n", header->magic);
    free(header);

    return bf; /* Return the opened file for further reading*/
}

BOFHeader *bof_read_header(FILE *file)
{
    BOFHeader *header = malloc(sizeof(BOFHeader));
    if (!header)
    {
        printf("Failed to allocate memory for BOFHeader\n");
        free(header);
        return NULL;
    }

    /* Read the magic number*/
    if (fread(header->magic, sizeof(char), 4, file) != 4)
    {
        printf("Failed to read magic number\n");
        free(header);
        return NULL;
    }

    /* Read text start address*/
    if (fread(&header->text_start_address, sizeof(word_type), 1, file) != 1)
    {
        printf("Failed to read text start address\n");
        free(header);
        return NULL;
    }

    /* Read text length*/
    if (fread(&header->text_length, sizeof(word_type), 1, file) != 1)
    {
        printf("Failed to read text length\n");
        free(header);
        return NULL;
    }

    /* Read data start address*/
    if (fread(&header->data_start_address, sizeof(word_type), 1, file) != 1)
    {
        printf("Failed to read data start address\n");
        free(header);
        return NULL;
    }

    /*Read data length*/
    if (fread(&header->data_length, sizeof(word_type), 1, file) != 1)
    {
        printf("Failed to read data length\n");
        free(header);
        return NULL;
    }

    /* Read stack bottom address*/
    if (fread(&header->stack_bottom_addr, sizeof(word_type), 1, file) != 1)
    {
        printf("Failed to read stack bottom address\n");
        free(header);
        return NULL;
    }

    /* Validate the magic number*/
    if (strncmp(header->magic, "BO32", 4) != 0)
    {
        printf("Invalid BOF file: incorrect magic number\n");
        free(header);
        return NULL;
    }

    return header;
}
/*return NULL;
}*/

word_type bof_read_word(FILE *file)
{
    /* TODO: Implement this function*/
    return 0;
}