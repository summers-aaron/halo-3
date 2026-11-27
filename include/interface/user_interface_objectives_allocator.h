#ifndef INTERFACE_USER_INTERFACE_OBJECTIVES_ALLOCATOR_H
#define INTERFACE_USER_INTERFACE_OBJECTIVES_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "interface/user_interface_objectives.h"

typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_allocator> c_objectives_allocation_manager;

template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_objectives_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

template <>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_objectives_allocator>::deallocate(
    void *memory);

template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_objectives_allocator>::allocate(
    unsigned int size, const char *name);

#endif // INTERFACE_USER_INTERFACE_OBJECTIVES_ALLOCATOR_H
