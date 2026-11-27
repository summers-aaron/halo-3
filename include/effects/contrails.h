#ifndef EFFECTS_CONTRAILS_H
#define EFFECTS_CONTRAILS_H

// Functions defined in tu/blamlibxbox/contrails.cpp.

void __tls_set_g_contrail_location_allocator(void *object);
void __tls_set_g_contrail_profile_allocator(void *object);
void __tls_set_g_particle_emitter_gpu_allocator(void *object);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_emitter_gpu_allocator> g_particle_emitter_gpu_allocator;

class c_contrail_location;
class c_contrail_states;
struct real_matrix4x3;

extern "C" void contrails_initialize();
struct s_game_non_bsp_zone_set;
class c_scenario_resource_registry;
extern "C" void contrail_systems_dispose_unused(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry);
extern "C" void contrail_location_update_matrix(c_contrail_location *location, float delta_time, const real_matrix4x3 *matrix);
extern "C" void contrail_location_pulse(c_contrail_location *location, float delta_time, bool force, c_contrail_states *states);
extern "C" void contrail_owners_frame_advance_all(float delta_time);

struct s_particle_instance;
struct s_object_14;
struct s_effect_contrail_profile;

extern "C" void contrail_evaluate_instance_properties(s_particle_instance *instance, float unused, s_object_14 *states);
extern "C" void contrail_system_destroy(long index);
extern "C" void contrail_set_base_address(void *object);
void __tls_set_g_contrail_allocator(void *object);
void __tls_set_g_contrail_system_allocator(void *object);
extern "C" void contrail_gpu_initialize(void);
extern "C" void contrail_update_system_instances(s_effect_contrail_profile *profile, float delta_time);

#endif // EFFECTS_CONTRAILS_H
