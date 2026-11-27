#ifndef OBJECTS_OBJECT_ALLOCATION_H
#define OBJECTS_OBJECT_ALLOCATION_H

#include "objects/objects.h"
#include "objects/object_broadphase.h"
#include "interface/first_person_weapons.h"
#include "cache/restricted_memory.h"

void adopt_old_object_render_message_queue(void *address);
void set_memory_pool_address(void *address);
void __tls_set_g_collideable_first_data_allocator(void *address);
void __tls_set_g_collideable_data_reference_allocator(void *address);
void __tls_set_g_collideable_cluster_reference_allocator(void *address);
void __tls_set_g_noncollideable_first_data_allocator(void *address);
void __tls_set_g_noncollideable_data_reference_allocator(void *address);
void __tls_set_g_noncollideable_cluster_reference_allocator(void *address);
void __tls_set_g_tracked_object_globals_allocator(void *address);

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager;

template<> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_data_allocator>::allocate(unsigned int size, const char *name);
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_data_allocator>::deallocate(void *address);

typedef t_restricted_allocation_manager<3,
    &adopt_old_object_render_message_queue,
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>
    t_object_render_message_queue_allocation_manager;
extern t_object_render_message_queue_allocation_manager g_object_render_message_queue_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_broadphase_allocator>
    t_object_broadphase_data_allocation_manager;
extern t_object_broadphase_data_allocation_manager g_object_broadphase_data_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_data_allocator>
    t_object_data_allocation_manager;
extern t_object_data_allocation_manager g_object_data_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_name_list_allocator>
    t_object_name_list_allocation_manager;
extern t_object_name_list_allocation_manager g_object_name_list_allocator;
typedef t_restricted_allocation_manager<2,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_render_data_allocator>
    t_object_render_data_allocation_manager;
extern t_object_render_data_allocation_manager g_object_render_data_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &set_memory_pool_address>
    t_object_memory_pool_allocation_manager;
extern t_object_memory_pool_allocation_manager g_object_memory_pool_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_first_data_allocator>
    t_collideable_first_data_allocation_manager;
extern t_collideable_first_data_allocation_manager g_collideable_first_data_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_data_reference_allocator>
    t_collideable_data_reference_allocation_manager;
extern t_collideable_data_reference_allocation_manager g_collideable_data_reference_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_cluster_reference_allocator>
    t_collideable_cluster_reference_allocation_manager;
extern t_collideable_cluster_reference_allocation_manager g_collideable_cluster_reference_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_first_data_allocator>
    t_noncollideable_first_data_allocation_manager;
extern t_noncollideable_first_data_allocation_manager g_noncollideable_first_data_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_data_reference_allocator>
    t_noncollideable_data_reference_allocation_manager;
extern t_noncollideable_data_reference_allocation_manager g_noncollideable_data_reference_allocator;
typedef t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_cluster_reference_allocator>
    t_noncollideable_cluster_reference_allocation_manager;
extern t_noncollideable_cluster_reference_allocation_manager g_noncollideable_cluster_reference_allocator;
typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracked_object_globals_allocator>
    t_tracked_object_globals_allocation_manager;
extern t_tracked_object_globals_allocation_manager g_tracked_object_globals_allocator;

#endif // OBJECTS_OBJECT_ALLOCATION_H
