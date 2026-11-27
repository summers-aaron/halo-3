#ifndef SCENARIO_SCENARIO_SOFT_CEILINGS_ALLOCATOR_H
#define SCENARIO_SCENARIO_SOFT_CEILINGS_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "scenario/scenario_soft_ceilings.h"

void scenario_soft_ceilings_initialize(void);
void scenario_soft_ceilings_dispose(void);
void scenario_soft_ceilings_initialize_for_new_map(void);
void scenario_soft_ceilings_dispose_from_old_map(void);

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator> c_scenario_soft_ceilings_allocation_manager;

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::allocate(
    unsigned int size, const char *name);

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::deallocate(
    void *memory);

#endif // SCENARIO_SCENARIO_SOFT_CEILINGS_ALLOCATOR_H
