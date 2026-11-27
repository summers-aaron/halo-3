#ifndef HS_OBJECT_LIST_INLINES_H
#define HS_OBJECT_LIST_INLINES_H

#include <ppcintrinsics.h>
#include "hs/hs_runtime_data.h"
#include "memory/data_types.h"
#include "objects/list_reference_types.h"

static __forceinline long object_list_get_next(long object_list_index, long *reference_index)
{
    long result;
    s_data_array *references = g_list_object_reference_data;
    if (*reference_index != -1) {
        s_list_reference *reference = (s_list_reference *)(references->data + (unsigned short)*reference_index * references->size);
        long next_reference = reference->next_index;
        if (next_reference != -1)
            __dcbt(0, references->data + (unsigned short)next_reference * references->size);
        result = reference->value;
        *reference_index = next_reference;
    } else {
        result = -1;
    }
    return result;
}

#endif // HS_OBJECT_LIST_INLINES_H
