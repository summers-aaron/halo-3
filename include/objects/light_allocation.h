#ifndef OBJECTS_LIGHT_ALLOCATION_H
#define OBJECTS_LIGHT_ALLOCATION_H

#include "objects/lights.h"
#include "cache/restricted_memory_regions.h"

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_render_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment);
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &lights_set_game_globals_address>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment);

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_first_data_allocator> t_light_first_data_allocator;
extern t_light_first_data_allocator g_light_first_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_data_reference_allocator> t_light_data_reference_allocator;
extern t_light_data_reference_allocator g_light_data_reference_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_cluster_reference_allocator> t_light_cluster_reference_allocator;
extern t_light_cluster_reference_allocator g_light_cluster_reference_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_data_allocator> t_light_data_allocator;
extern t_light_data_allocator g_light_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_light_render_data_allocator> t_light_render_data_allocator;
extern t_light_render_data_allocator g_light_render_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &lights_set_game_globals_address> t_lights_game_globals_allocator;
extern t_lights_game_globals_allocator g_lights_game_globals_allocator;

#endif // OBJECTS_LIGHT_ALLOCATION_H
