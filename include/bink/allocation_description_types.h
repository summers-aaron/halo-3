#ifndef BINK_ALLOCATION_DESCRIPTION_TYPES_H
#define BINK_ALLOCATION_DESCRIPTION_TYPES_H

#include "memory/data_types.h"

struct s_allocation_description {
    void **destination;
    unsigned long element_size;
    unsigned long element_count;
    unsigned long size;
    long alignment_bits;
};

#endif // BINK_ALLOCATION_DESCRIPTION_TYPES_H
