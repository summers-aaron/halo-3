#ifndef EFFECTS_PARTICLE_EMITTER_ALLOCATOR_H
#define EFFECTS_PARTICLE_EMITTER_ALLOCATOR_H
#include "cache/restricted_memory_regions.h"

void particle_emitter_set_array_address(void *object);
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_set_array_address>::deallocate(void *allocation);

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_set_array_address> c_particle_emitter_allocation_manager;
extern c_particle_emitter_allocation_manager g_particle_emitter_allocator;

#endif
