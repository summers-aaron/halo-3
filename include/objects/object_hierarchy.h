#ifndef OBJECTS_OBJECT_HIERARCHY_H
#define OBJECTS_OBJECT_HIERARCHY_H

#include "objects/object_header.h"

static __forceinline bool flags_test(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}

static __forceinline long object_get_root_object(long index)
{
    while (index != -1)
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(index, ~0);
        if (!flags_test(object->flags, 26))
            break;
        index = object->parent_object_index;
    }
    return index;
}

#endif
