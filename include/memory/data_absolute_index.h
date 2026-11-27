#ifndef MEMORY_DATA_ABSOLUTE_INDEX_H
#define MEMORY_DATA_ABSOLUTE_INDEX_H

#include "memory/data_types.h"

static __forceinline void *datum_try_and_get_absolute(const s_data_array *array, long absolute_index)
{
    void *result = 0;
    if (absolute_index != -1)
    {
        if (absolute_index >= 0 && absolute_index < array->first_unallocated)
        {
            s_datum_header *datum = (s_datum_header *)(array->data + absolute_index * array->size);
            if (datum->identifier != 0)
                result = datum;
        }
    }
    return result;
}
static __forceinline long datum_absolute_index_to_index(const s_data_array *array, long absolute_index)
{
    long result = -1;
    if (absolute_index != -1)
    {
        const s_datum_header *datum = (const s_datum_header *)(array->data + array->size * absolute_index);
        result = ((unsigned short)datum->identifier << 16) | absolute_index;
    }
    return result;
}
#endif
