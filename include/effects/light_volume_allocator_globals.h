#ifndef EFFECTS_LIGHT_VOLUME_ALLOCATOR_GLOBALS_H
#define EFFECTS_LIGHT_VOLUME_ALLOCATOR_GLOBALS_H

#include "cache/restricted_memory_regions.h"
#include "effects/light_volumes.h"
#include "effects/light_volume_allocators.h"


typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_allocator> t_light_volume_location_allocator;
extern t_light_volume_location_allocator g_light_volume_location_allocator;

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_allocator> t_light_volume_allocator;
extern t_light_volume_allocator g_light_volume_allocator;

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_system_allocator> t_light_volume_system_allocator;
extern t_light_volume_system_allocator g_light_volume_system_allocator;

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_location_data_allocator> t_light_volume_location_data_allocator;
extern t_light_volume_location_data_allocator g_light_volume_location_data_allocator;

typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_volume_data_allocator> t_light_volume_data_allocator;
extern t_light_volume_data_allocator g_light_volume_data_allocator;

#endif // EFFECTS_LIGHT_VOLUME_ALLOCATOR_GLOBALS_H
