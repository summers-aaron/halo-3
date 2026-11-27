#ifndef PHYSICS_BREAKABLE_SURFACE_ALLOCATORS_H
#define PHYSICS_BREAKABLE_SURFACE_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"
#include "physics/breakable_surfaces.h"

typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_breakable_surface_globals_allocator> t_breakable_surface_globals_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_simulation_gamestate_entity_data_allocator> t_breakable_surface_events_allocator;

extern t_breakable_surface_globals_allocator g_breakable_surface_globals_allocator;
extern t_breakable_surface_events_allocator g_breakable_surface_events_allocator;

template <> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_breakable_surface_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment);
template <> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_simulation_gamestate_entity_data_allocator>::allocate(
    unsigned int size, const char *name);
template <> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    __tls_set_g_simulation_gamestate_entity_data_allocator>::deallocate(void *memory);

#endif // PHYSICS_BREAKABLE_SURFACE_ALLOCATORS_H
