#ifndef OBJECTS_OBJECT_TYPE_ACCESS_H
#define OBJECTS_OBJECT_TYPE_ACCESS_H

#include "objects/object_header.h"
#include "memory/data_types.h"

static __forceinline long object_get_type(long index)
{
    return ((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)].object_type;
}

#endif
