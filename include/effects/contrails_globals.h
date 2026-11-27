#ifndef EFFECTS_CONTRAILS_GLOBALS_H
#define EFFECTS_CONTRAILS_GLOBALS_H

#include "effects/contrail_profile_data.h"

#include "cache/restricted_memory_regions.h"
#include "effects/contrails.h"
#include "math/real_math_types.h"

class c_contrail;

extern "C" void contrail_create_profiles(c_contrail *contrail, unsigned long queue_context, c_contrail_states *states, bool temporary,
    const real_point3d *direction, float delta_distance);

// Address tags stay in contrails.cpp.
extern "C" {
extern const real_argb_color k_contrail_profile_default_tint;
}

extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator> g_contrail_system_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_location_allocator> g_contrail_location_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_allocator> g_contrail_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_profile_allocator> g_contrail_profile_allocator;

extern "C" {
extern bool g_contrail_profile_creation_enabled;
extern bool g_contrail_creation_enabled[3];
extern bool g_contrail_system_creation_enabled[3];
extern bool g_contrail_location_creation_enabled[3];
void contrail_states_update_values(void *object, unsigned long mask);
void contrail_states_set_definition(char *, char *);
}

#endif // EFFECTS_CONTRAILS_GLOBALS_H
