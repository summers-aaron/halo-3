#ifndef MEMORY_DATA_TYPED_ACCESS_H
#define MEMORY_DATA_TYPED_ACCESS_H

#include "memory/data_types.h"

// March data.cpp:1200: absolute indices already exclude the datum identifier.
static __forceinline void *datum_get_absolute(s_data_array *data, long absolute_index)
{
    return data->data + absolute_index * data->size;
}

template<typename t_datum>
static __forceinline t_datum *datum_get_absolute(s_data_array *data, long absolute_index)
{
    return &((t_datum *)data->data)[absolute_index];
}

template<typename t_datum>
static __forceinline t_datum *datum_get_typed(s_data_array *data, long index)
{
    return &((t_datum *)data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}

#endif
