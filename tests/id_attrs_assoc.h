/* $Id: id_attrs_assoc.h,v 1.3 2024/07/29 02:01:32 leavens Exp $ */
#ifndef _ID_ATTRS_ASSOC_H
#define _ID_ATTRS_ASSOC_H

#include "machine_types.h"
#include "file_location.h"

// Kinds of names in the assembly language
typedef enum {id_label, id_data} id_attr_kind;

// attributes of a name in the assembly language
typedef struct {
    const char *name;
    id_attr_kind kind;
    file_location *file_loc;
    address_type addr;  // word offset from start of text or data section
} id_attrs_assoc;

#endif
