#ifndef PHYSICS_HAVOK_RUNTIME_POOL_H
#define PHYSICS_HAVOK_RUNTIME_POOL_H

#include "physics/havok_memory.h"

// Functions defined in tu/blamlibxbox/havok_runtime_pool.cpp.

extern "C" bool havok_runtime_pool_is_surface_enabled(long structure_bsp_index, long, long, long surface_key);

extern "C" void *havok_runtime_pool_cache_allocate_entry(long type, unsigned long key);

struct s_havok_runtime_pool_cache;
extern "C" {
extern s_havok_runtime_pool_cache g_havok_runtime_pool_cache;
}

extern "C" bool havok_runtime_pool_cache_try_and_get(long type, unsigned long key, void **storage);
#endif // PHYSICS_HAVOK_RUNTIME_POOL_H
