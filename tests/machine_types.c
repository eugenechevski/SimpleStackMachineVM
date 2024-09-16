// $Id: machine_types.c,v 1.12 2024/08/22 11:36:02 leavens Exp $
#include "machine_types.h"
#include "utilities.h"

// Return the sign-extended equivalent of i
word_type machine_types_sgnExt(immediate_type i) {
    return i; // preserves the value
}

// Return the zero-extended equivalent of i
uword_type machine_types_zeroExt(immediate_type i) {
    return (uword_type) i;
}

// Return the offset given by o,
// which is the sign extension of o
word_type machine_types_formOffset(immediate_type o) {
    return machine_types_sgnExt(o);
}

// Return an address formed by concatenating the instruction address, a,
// with the high-order (4) bits of the PC.
address_type machine_types_formAddress(address_type PC, address_type a) {
    return ((0xF0000000 & PC) | (0x0FFFFFF & a));
}

// Check that o can be represented as an offset field in an instruction
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_offset(word_type o)
{
    if (o > NINEBITSMAXSIGNED) {
	bail_with_error("Offset is too large: %d", o);
    } else if (o < NINEBITSMINSIGNED) {
	bail_with_error("Offset is too small: %d", o);
    }
}

// Check that arg can be represented as an arg field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_arg(word_type arg)
{
    if (arg > TWELVEBITSMAXSIGNED) {
	bail_with_error("12 bit argument is too large: %d", arg);
    } else if (arg < TWELVEBITSMINSIGNED) {
	bail_with_error("12 bit argument is too small: %d", arg);
    }
}

// Check that s can be represented as a shift field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_shift(word_type s)
{
    if (s > TWELVEBITSMAXUNSIGNED) {
	bail_with_error("Shift is too large: %u", s);
    }
}

// Check that immed can be represented as a signed immediate field,
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_immed(word_type immed)
{
    if (immed > SIXTEENBITSMAXSIGNED) {
	bail_with_error("Immediate argument is too large: %d",
			immed);
    } else if (immed < SIXTEENBITSMINSIGNED) {
	bail_with_error("Immediate argument is too small: %d",
			immed);
    }
}

// Check that arg can be represented as unsigned immediate field,
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_uimmed(word_type arg)
{
    if (arg > SIXTEENBITSMAXUNSIGNED) {
	bail_with_error("Unsigned immediate argument is too large: %u", arg);
    }
}

// Check that addr can be represented as an address field in an instruction,
// bail with an error message if not (so doesn't return if wrong)
void machine_types_check_fits_in_addr(address_type addr)
{
    if (addr > TWENTYEIGHTBITSMAXUNSIGNED) {
	bail_with_error("Address is too large: %u", addr);
    }
}

// Return the nearest multiple of BYTES_PER_WORD
// that is greater than or equal to n
int machine_types_round_up_to_wordsize(unsigned int n)
{
    int rem = n % BYTES_PER_WORD;
    if (rem == 0) {
	return n;
    } else {
	// assert(((n + (BYTES_PER_WORD - rem)) % BYTES_PER_WORD) == 0);
	return n + (BYTES_PER_WORD - rem);
    }
}
