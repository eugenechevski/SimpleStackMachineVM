/* $Id: disasm.c,v 1.14 2024/07/28 22:01:51 leavens Exp $ */
#include <stdio.h>
#include "disasm.h"
#include "bof.h"
#include "regname.h"
#include "utilities.h"
#include "instruction.h"

// Disassemble code from bf,
// with output going to the file out
void disasmProgram(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf);
    disasmTextSection(out, bf, bh);
    disasmDataSection(out, bf, bh);
    disasmStackSection(out, bh);
    fprintf(out, ".end");
    newline(out);
}

// Disassemble the text section
// with output going to the file out
void disasmTextSection(FILE *out, BOFFILE bf, BOFHeader bh)
{
    fprintf(out, ".text\t%u", bh.text_start_address);
    newline(out);
    disasmInstrs(out, bf, bh.text_length);
}

// Disassemble length instructions from bf
// with output going to the file out
void disasmInstrs(FILE *out, BOFFILE bf, int length)
{
    for (int i = 0; i < length; i++) {
	disasmInstr(out, instruction_read(bf), i);
    }
}

// Disassemble the binary instruction bi, which would go at address i
// each instruction has a label of the form a%d:, where %d is the value of i
void disasmInstr(FILE *out, bin_instr_t bi, address_type i)
{
    fprintf(out, "a%d:\t%s", i, instruction_assembly_form(i, bi));
    newline(out);
}

// Disassemble the data section from bf, based on the information in bh,
// with output going to out
void disasmDataSection(FILE *out, BOFFILE bf, BOFHeader bh)
{
    fprintf(out, ".data\t%u", bh.data_start_address);
    newline(out);
    disasmStaticDecls(out, bf, bh.data_length);
}

// Disassemble words_to_read static data words from bf,
// with output going to out
void disasmStaticDecls(FILE *out, BOFFILE bf, int words_to_read)
{
    while (words_to_read >= 1) {
	disasmStaticDecl(out, bof_read_word(bf));
	words_to_read--;
    }
}

// count of number of words generated, to get unique names
static int word_count = 0;
// buffer for generated ids
static char id_buf[16];

static const char*new_word_id()
{
    sprintf(id_buf, "w%x", word_count);
    word_count++;
    return id_buf;
}

// Disassemble the the given word as a static data declaration,
// with output going to out
void disasmStaticDecl(FILE *out, word_type w)
{
    fprintf(out, "WORD %s = %d", new_word_id(), w);
    newline(out);
}

// Disassemble the stack section based on the given header information
void disasmStackSection(FILE *out, BOFHeader bh)
{
    fprintf(out, ".stack\t%u", bh.stack_bottom_addr);
    newline(out);
}
