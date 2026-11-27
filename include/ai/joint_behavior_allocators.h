#ifndef AI_JOINT_BEHAVIOR_ALLOCATORS_H
#define AI_JOINT_BEHAVIOR_ALLOCATORS_H
#include "cache/restricted_memory_regions.h"
#include "ai/joint_behavior_functions.h"
void __tls_set_g_clump_data_allocator(void *allocator);
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_clump_data_allocator>::deallocate(void *memory);

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_joint_data_allocator> joint_clump_allocation_manager;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_clump_data_allocator> joint_state_allocation_manager;
#endif
