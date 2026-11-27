#ifndef EFFECTS_PARTICLE_H
#define EFFECTS_PARTICLE_H

#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include <stddef.h>
#include "cache/restricted_memory_regions.h"

// Functions defined in tu/blamlibxbox/particle.cpp.

struct s_data_array;
extern "C" { extern __declspec(thread) s_data_array *g_particle_data; }

void __tls_set_g_memory_pool_allocator(void *object);
typedef t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_memory_pool_allocator> t_particle_restricted_allocation_manager;
extern t_particle_restricted_allocation_manager g_memory_pool_allocator;

struct s_particle_instance;
struct s_particle_spawn_parameters {
    real_matrix4x3 matrix;
};

long particle_new_location_from_spawn(unsigned char type, long system_index, s_particle_spawn_parameters *spawn, long seed);
extern "C" void particle_initialize_for_new_map(void);
extern "C" void particle_delete_location_chain(int index);
extern "C" void particle_submit_gpu_state(s_particle_instance *instance);

class c_particle;
union real_vector3d;
struct s_effect_contrail_location;

long particle_instance_new(unsigned char type, long location, long definition, unsigned long seed);
extern "C" void particle_location_set_base_address(void *object);
extern "C" void particle_location_append(s_effect_contrail_location *node, int value);
extern "C" void particle_instance_set_base_address(void *object);
extern "C" void particle_instance_delete(int index);
struct s_particle_collision_entry;
class c_particle_collision_event;
class c_particle_state_list;

extern "C" void particle_handle_collision_event(const c_particle *particle, long type, long definition_index,
    const c_particle_collision_event *collision_event, const c_particle_state_list *state_list);

class c_particle
{
public:
    struct s_property_values
    {
        float m_manual_rotation;
        float m_animation_rate;
        real_vector3d m_self_acceleration;
    };

    short m_identifier;
    unsigned short m_flags;
    long m_next_index;
    long m_emitter_index;
    unsigned long m_gpu_index;
    float m_age;
    float m_inverse_lifespan;
    float m_correlation_1;
    unsigned short m_random_seed[8];
    union {
        real_point3d m_position;
        real_vector3d m_position_vector;
    };
    real_vector3d m_velocity;
    float m_rotational_velocity;
    float m_random_offset;
    real_argb_color m_color;
    real_vector3d m_axis;
    s_property_values m_property_values;

    static __forceinline void initialize_for_map();
    static __forceinline void dispose_from_map();
    __forceinline bool get_run_physics() const { return !(m_flags & 0x48) && m_age <= 1.0f; }

    __forceinline long get_emitter_index() const { return m_emitter_index; }

private:
    void spawn_sound(const s_particle_collision_entry *entry, const c_particle_collision_event *) const;
    friend void particle_handle_collision_event(const c_particle *, long, long, const c_particle_collision_event *,
        const c_particle_state_list *);
};

// Spawn clears the trailing fields and update reads acceleration, so this layout must stay fixed.
typedef char particle_size[(sizeof(c_particle) == 0x7c) ? 1 : -1];
typedef char particle_flags_offset[(offsetof(c_particle, m_flags) == 2) ? 1 : -1];
typedef char particle_position_offset[(offsetof(c_particle, m_position) == 0x2c) ? 1 : -1];
typedef char particle_properties_offset[(offsetof(c_particle, m_property_values) == 0x68) ? 1 : -1];
typedef char particle_animation_offset[(offsetof(c_particle, m_property_values.m_animation_rate) == 0x6c) ? 1 : -1];
typedef char particle_acceleration_offset[(offsetof(c_particle, m_property_values.m_self_acceleration) == 0x70) ? 1 : -1];

#endif // EFFECTS_PARTICLE_H
