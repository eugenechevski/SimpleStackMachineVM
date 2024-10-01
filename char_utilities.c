// $Id: char_utilities.c,v 1.4 2024/07/26 02:31:21 leavens Exp $
#include <ctype.h>
#include <string.h>
#include "char_utilities.h"
#include "utilities.h"

// strdup seems to be in the string library but not in the header...
extern char *strdup(const char *s);

// return the value of the given character literal
// and set *charlit_len to the number of characters in the literal read
unsigned char char_utilities_char_value(const char *lit,
					int *charlit_len)
{
    int len = strlen(lit);
    assert(len > 0);
    if (len < 2 || lit[0] != '\\') {
	*charlit_len = 1;
	return lit[0];
    }
    // now we know that lit[0] is a backslash
    *charlit_len = 2; // adjusted below for octal and hex literals
    // lit is an escape sequence, with lit[0] == '\\'
    switch (lit[1]) {
    case 'n':
	return '\n';
	break;
    case 'r':
	return '\r';
	break;
    case 'f':
	return '\f';
	break;
    case 't':
	return '\t';
	break;
    case 'v':
	return '\v';
	break;
    case '0':
	if (len == 2 || (len > 2 && !is_octal_digit(lit[3]))) {
	    return '\0';
	} else {
	    // octal escape sequence
	    unsigned int val;
	    sscanf(lit+2, "%o", &val);
	    int digits_after_0 = 0;
	    while (is_octal_digit(lit[2+digits_after_0])) {
		digits_after_0++;
	    }
	    *charlit_len = 2 + digits_after_0;
	    return (unsigned char)val;
	}
	break;
    case 'a':
	return '\a';
	break;
    case 'b':
	return '\b';
	break;
    case '\\':
	return '\\';
	break;
    case '\'':
	return '\'';
	break;
    case '\"':
	return '\"';
	break;
    case 'x':
	// hex escape sequence
	unsigned int val;
	sscanf(lit+2, "%x", &val);
	int digits_after_x = 0;
	while (isxdigit((int)lit[2+digits_after_x])) {
	    digits_after_x++;
	}
	*charlit_len = 2 + digits_after_x;
	return (unsigned char)val;
	break;
    default:
	return (unsigned char)lit[1];
	break;
    }
}

// Return the given char as a string
// or, if it's not printable, then return a literal string for it
const char *char_utilities_unescape_char(unsigned char c)
{
    char unescape_buf[20]; // allow for longer octal escapes (for errors)
    if (isprint(c) && c != '\\' && c != '\n' && c != '\'' && c != '\"') {
	sprintf(unescape_buf, "%c", c);
    } else {
	switch (c) {
	case '\n':
	    sprintf(unescape_buf, "\\n");
	    break;
	case '\r':
	    sprintf(unescape_buf, "\\r");
	    break;
	case '\f':
	    sprintf(unescape_buf, "\\f");
	    break;
	case '\t':
	    sprintf(unescape_buf, "\\t");
	    break;
	case '\v':
	    sprintf(unescape_buf, "\\v");
	    break;
	case '\0':
	    sprintf(unescape_buf, "\\0");
	    break;
	case '\a':
	    sprintf(unescape_buf, "\\a");
	    break;
	case '\b':
	    sprintf(unescape_buf, "\\b");
	    break;
	case '\\':
	    sprintf(unescape_buf, "\\\\");
	    break;
	case '\'':
	    sprintf(unescape_buf, "\\'");
	    break;
	case '\"':
	    sprintf(unescape_buf, "\\\"");
	    break;
	default:
	    sprintf(unescape_buf, "\\0%o", c);
	    break;
	}
    }
    return strdup(unescape_buf);
}

#define UNESTRBUFSIZE 4096

// Return the given string as a string with only
// printable characters and escape sequences
const char *char_utilities_unescape_string(const char *s)
{
    char buf[UNESTRBUFSIZE];
    int len = strlen(s);
    int buff_index = 0;
    for (int i = 0; i < len; i++) {
	buff_index
	    += sprintf(buf+buff_index, "%s",
		       char_utilities_unescape_char((unsigned char) s[i]));
    }
    return strdup(buf);
}

// Is the given character an octal digit (0-7)?
bool is_octal_digit(char c)
{
    return ('0' <= c && c <= '7');
}
