#ifndef MEMORY_DATA_HANDLE_ACCESS_H
#define MEMORY_DATA_HANDLE_ACCESS_H
#include "memory/data_types.h"
// Inline checked datum lookup that returns null for invalid handles.
static __forceinline void *datum_try_and_get_inline(const s_data_array *data, long datum_index)
{
    void *result = 0;
    if (datum_index != -1) {
        long absolute_index = datum_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            void *datum = data->data + data->size * absolute_index;
            short identifier = *(short *)datum;
            if (identifier != 0 && identifier == (datum_index >> 16))
                result = datum;
        }
    }
    return result;
}

#endif
