// $Id: regname.c,v 1.3 2024/07/29 12:38:50 leavens Exp $
#include "regname.h"

static const char *regnames[NUM_REGISTERS] = {
    "$gp", "$sp", "$fp", "$r3", "$r4", "$r5", "$r6", "$ra" };

// requires 0 <= n && n < NUM_REGISTERS
// Return the standard symbolic name for n
const char *regname_get(reg_num_type n)
{
    return regnames[n];
}
