#ifndef OBJECTS_OBJECT_LIST_HEADER_TYPES_H
#define OBJECTS_OBJECT_LIST_HEADER_TYPES_H

#include "memory/data_types.h"

// count and reference_count from object_lists.cpp: object_lists_dispose_empty_lists deletes lists whose count is 0,
// object_list_add and object_list_delete_object step reference_count.
struct s_object_list_header {
    short identifier;
    short padding;
    unsigned short count;
    short reference_count;
    long first_list_reference;
};

typedef char object_list_header_size[(sizeof(s_object_list_header) == 12) ? 1 : -1];

#endif // OBJECTS_OBJECT_LIST_HEADER_TYPES_H
