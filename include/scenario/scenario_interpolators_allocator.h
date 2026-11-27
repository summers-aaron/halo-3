#ifndef SCENARIO_SCENARIO_INTERPOLATORS_ALLOCATOR_H
#define SCENARIO_SCENARIO_INTERPOLATORS_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "scenario/scenario_interpolators.h"

float scenario_interpolator_get_current_value(long index);

typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_interpolator_globals_allocator> c_scenario_interpolator_allocation_manager;

template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_scenario_interpolator_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_scenario_interpolator_globals_allocator>::allocate(
    unsigned int size, const char *name);

template <>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_scenario_interpolator_globals_allocator>::deallocate(
    void *memory);

#endif // SCENARIO_SCENARIO_INTERPOLATORS_ALLOCATOR_H
