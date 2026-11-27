#ifndef PHYSICS_HAVOK_COMPONENT_INLINE_H
#define PHYSICS_HAVOK_COMPONENT_INLINE_H

#include "physics/havok_component.h"
#include "memory/data_types.h"

static __forceinline c_havok_component *objects_havok_component_get(long havok_component_index)
{
#ifdef HALO3_64BIT
    return (c_havok_component *)(g_havok_component_data->data + (havok_component_index & 0xffff) * g_havok_component_data->size);
#else
    return (c_havok_component *)(g_havok_component_data->data + (havok_component_index & 0xffff) * 144);
#endif
}

#endif
