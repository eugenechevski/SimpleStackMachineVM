// $Id: machine_types.h,v 1.29 2024/08/29 21:58:39 leavens Exp $
// Machine Types for the Simplified Stack Machine (SSM)
#ifndef _MACHINE_TYPES_H
#define _MACHINE_TYPES_H

#define BYTES_PER_WORD 4

// operation codes
typedef unsigned short opcode_type;

// registers encoded in instructions
typedef unsigned short reg_num_type;

// offset values (which are signed) encoded in instructions
typedef short offset_type;

// function codes in computational instructions
typedef unsigned short func_type;

// argument values (signed) encoded in instructions
typedef short arg_type;

// shift values encoded in other computational instructions
typedef unsigned short shift_type;

// immediate operands encoded in instructions
typedef int immediate_type;

// unsigned immediate operands in instructions
typedef unsigned int uimmed_type;

// addresses, assuming ints are 32 bits
typedef unsigned int address_type;

// machine words, assuming ints are 32 bits
typedef int word_type;

// machine words, unsigned
typedef unsigned int uword_type;

// bytes, assuming chars are 8 bits
typedef unsigned char byte_type;

#define NINEBITSMAXSIGNED 0xff
#define NINEBITSMINSIGNED -512
#define TWELVEBITSMAXSIGNED 0x7ff
#define TWELVEBITSMINSIGNED -0x800
#define TWELVEBITSMAXUNSIGNED 0xfff
#define SIXTEENBITSMAXSIGNED 0777
#define SIXTEENBITSMINSIGNED -01000
#define SIXTEENBITSMAXUNSIGNED 0xffff
#define TWENTYEIGHTBITSMAXUNSIGNED 0xfffffff

// Return the sign-extended equivalent of i
extern word_type machine_types_sgnExt(immediate_type i);

// Return the zero-extended equivalent of i
extern uword_type machine_types_zeroExt(immediate_type i);

// Return the offset given by o,
// which is the sign extension of o
extern word_type machine_types_formOffset(immediate_type o);

// Return an address formed by concatenating the instruction address, a,
// with the high-order (4) bits of the PC.
extern address_type machine_types_formAddress(address_type PC, address_type a);

// Check that o can be represented as an offset field in an instruction
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_offset(word_type o);

// Check that arg can be represented as an arg field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_arg(word_type arg);

// Check that arg can be represented as a shift field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_shift(word_type arg);

// Check that immed can be represented as a signed immediate field,
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_immed(word_type immed);

// Check that arg can be represented as unsigned immediate field,
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_uimmed(word_type arg);

// Check that addr can be represented as an address field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
extern void machine_types_check_fits_in_addr(address_type addr);

// the following line is for the documentation
// ...
#endif
