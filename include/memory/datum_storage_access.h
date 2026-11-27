#ifndef MEMORY_DATUM_STORAGE_ACCESS_H
#define MEMORY_DATUM_STORAGE_ACCESS_H

#include "memory/data.h"

// A cached storage base retains its owning data array's compile-time stride.
template<typename t_datum>
static __forceinline t_datum *datum_get(t_datum *datums, long index)
{
    return &datums[(unsigned short)index];
}

#endif // MEMORY_DATUM_STORAGE_ACCESS_H
