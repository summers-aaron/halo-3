#ifndef AI_ACTOR_ALLOCATORS_H
#define AI_ACTOR_ALLOCATORS_H

#include "cache/restricted_memory_regions.h"
#include "ai/actors.h"

void __tls_set_g_ai_reference_frame_data_allocator(void *);

extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_data_allocator> g_actor_data_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_reference_frame_data_allocator> g_ai_reference_frame_data_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_firing_position_hash_allocator> g_actor_firing_position_hash_allocator;

#endif // AI_ACTOR_ALLOCATORS_H
