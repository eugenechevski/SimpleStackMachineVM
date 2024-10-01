// $Id: char_utilities.h,v 1.3 2024/07/26 02:31:21 leavens Exp $
#ifndef CHAR_UTILTIES_H
#define CHAR_UTILTIES_H

#include <stdbool.h>

// return the value of the given character literal
// and set *charlit_len to the number of characters in the literal read
extern unsigned char char_utilities_char_value(const char *lit,
					       int *charlit_len);

// Return a the given char as a string
// or, if it's not printable, then return a literal string for it
extern const char *char_utilities_unescape_char(unsigned char c);

// Return the given string as a string with only
// printable characters and escape sequences
extern const char *char_utilities_unescape_string(const char *s);

// Is the given character an octal digit (0-7)?
extern bool is_octal_digit(char c);

#endif
