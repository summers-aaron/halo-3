#ifndef INTERFACE_CUSTOMIZATION_TYPES_H
#define INTERFACE_CUSTOMIZATION_TYPES_H

#include "memory/data_types.h"
#include "tag_files/tag_types.h"

struct s_customization {
    long name;
    long count;
    TAG_POINTER(void) address;
    long definition;
};

#endif // INTERFACE_CUSTOMIZATION_TYPES_H
