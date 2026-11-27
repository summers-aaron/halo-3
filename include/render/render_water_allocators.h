#ifndef RENDER_RENDER_WATER_ALLOCATORS_H
#define RENDER_RENDER_WATER_ALLOCATORS_H

#include "cache/restricted_memory.h"
#include "render/render_water.h"

typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator> water_interaction_allocation_manager;
template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);
template<>
void t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::deallocate(void *memory);
extern water_interaction_allocation_manager g_water_interaction_event_allocator;

typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator> water_allocation_manager;
extern water_allocation_manager g_water_interaction_events_allocator;

#endif
