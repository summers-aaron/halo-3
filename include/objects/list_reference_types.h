#ifndef OBJECTS_LIST_REFERENCE_TYPES_H
#define OBJECTS_LIST_REFERENCE_TYPES_H

#include "memory/data_types.h"

struct s_list_reference {
    short identifier;
    short padding;
    long value;
    long next_index;
};

#endif // OBJECTS_LIST_REFERENCE_TYPES_H
