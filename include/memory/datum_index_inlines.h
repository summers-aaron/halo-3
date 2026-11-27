#ifndef MEMORY_DATUM_INDEX_INLINES_H
#define MEMORY_DATUM_INDEX_INLINES_H

#include "memory/data_types.h"

long data_next_absolute_index(const s_data_array *data, long index);

static __forceinline long datum_absolute_index_to_index(const s_data_array *array, long absolute_index)
{
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + array->size * absolute_index) << 16) | absolute_index;
    return result;
}

// Local inline body for inlined data_next_index call sites, named apart from the out-of-line
// function: this build emits bl for cross-unit calls, so inlined sites need this copy.
static __forceinline long inl_data_next_index(s_data_array *data, long index)
{
    long absolute_index = data_next_absolute_index(data, (unsigned short)(index + 1));
    return datum_absolute_index_to_index(data, absolute_index);
}

#endif // MEMORY_DATUM_INDEX_INLINES_H
