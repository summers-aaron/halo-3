#ifndef PHYSICS_HAVOK_COMPONENT_DATUM_ACCESS_H
#define PHYSICS_HAVOK_COMPONENT_DATUM_ACCESS_H

#include "memory/data_types.h"
#include "memory/data.h"
#include "physics/havok_component.h"

// Include after the complete component definition.
static __forceinline c_havok_component *havok_component_get(long index)
{
    return datum_get<c_havok_component>(g_havok_component_data, index);
}

#endif
