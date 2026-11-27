#ifndef PHYSICS_HAVOK_RUNTIME_STORAGE_H
#define PHYSICS_HAVOK_RUNTIME_STORAGE_H

#include "physics/havok_listeners.h"

// Runtime allocation and listener storage owned by havok.obj.
// Address annotations belong to the owning translation unit.
class c_entity_listener;
class c_world_entity_listener;
struct s_havok_runtime_pool_cache;
struct s_havok_simulation_island_cleanup_globals;

extern "C" {
extern unsigned long g_havok_thread_monitor_tls_index;
extern short g_num_entity_iterators_in_use;
extern c_entity_listener *g_global_entity_listener;
extern c_world_entity_listener *g_world_entity_listener;
extern s_havok_runtime_pool_cache g_havok_runtime_pool_cache;
long havok_get_object_bounds_and_group(long object_index, void *buffer);
void havok_update_bodies_in_bounds(long count, void *buffer);
void __tls_set_g_impacts_globals_allocator(void *address);
}

extern s_havok_simulation_island_cleanup_globals g_havok_simulation_island_cleanup_globals;
void __cdecl havok_error_stub(char const *, void *);

#include "cache/restricted_memory_regions.h"
void __tls_set_g_impact_data_allocator(void *object);
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impacts_globals_allocator> g_impacts_globals_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impact_data_allocator> g_impact_data_allocator;

#endif
