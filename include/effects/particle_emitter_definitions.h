#ifndef EFFECTS_PARTICLE_EMITTER_DEFINITIONS_H
#define EFFECTS_PARTICLE_EMITTER_DEFINITIONS_H

#include "effects/particle_system_definitions.h"
#include "math/real_math_types.h"
#include "effects/editable_property.h"
#include "effects/particle_movement_internals.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

class c_particle_state_list;
class c_particle;
class c_particle_emitter;
class c_particle_system;

class c_particle_emitter_definition
{
public:
    enum e_emission_shape
    {
        _emission_shape_sprayer = 0,
        _emission_shape_disc,
        _emission_shape_globe,
        _emission_shape_implode,
        _emission_shape_tube,
        _emission_shape_halo,
        _emission_shape_impact_contour,
        _emission_shape_impact_area,
        _emission_shape_debris,
        _emission_shape_line,
        _emission_shape_breakable_surface,
        k_number_of_emission_shapes
    };
    enum e_particle_axis
    {
        _particle_axis_emitter = 0,
        _particle_axis_random_cone,
        _particle_axis_random_disc,
        _particle_axis_random_direction,
        k_number_of_particle_axes
    };
    void initialize_particle(const c_particle_state_list *state_list, c_particle *particle, c_particle_emitter *emitter, const c_particle_system *system) const;
    __forceinline bool is_cpu() const { return (m_flags & 2) != 0; }
    __forceinline bool is_gpu() const { return (m_flags & 4) != 0; }
    __forceinline bool is_gpu_when_at_rest() const { return (m_flags & 8) != 0; }
    __forceinline unsigned char get_emission_shape() const { return m_emission_shape; }
    const real_point3d evaluate_emitter_offset(const c_particle_state_list *states) const { return m_emitter_offset.evaluate(states); }
    const real_vector3d evaluate_emitter_direction(const c_particle_state_list *states) const { return m_emitter_direction.evaluate(states); }
    void evaluate_emitter_offset_if_not_constant_over_time(const c_particle_state_list *states, real_point3d *offset) const
    {
        if (!c_editable_property_base::property_value_is_initialized(offset) || !m_emitter_offset.is_constant_over_time())
            *offset = evaluate_emitter_offset(states);
    }
    void evaluate_emitter_direction_if_not_constant_over_time(const c_particle_state_list *states, real_vector3d *direction) const
    {
        if (!c_editable_property_base::property_value_is_initialized(direction) || !m_emitter_direction.is_constant_over_time())
            *direction = evaluate_emitter_direction(states);
    }
private:
    long m_name;
    unsigned char m_emission_shape;
    unsigned char m_flags;
    unsigned char m_particle_axis;
    unsigned char m_particle_reference_axis;
public:
    float m_bounding_sphere_radius;
private:
    float m_bounding_sphere_radius_override;
public:
    real_vector2d uv;
private:
    c_editable_property<real_point3d> m_emitter_offset;
    c_editable_property<real_vector3d> m_emitter_direction;
    c_editable_property<c_particle_state_list> m_emission_radius;
    c_editable_property<c_particle_state_list> m_emission_angle;
    c_editable_property<c_particle_state_list> m_emission_axis_angle;
public:
    c_editable_property_base m_starting_count;
    c_editable_property_base m_max_count;
    c_editable_property_base m_emission_rate;
private:
    c_editable_property<c_particle_state_list> m_particle_lifespan;
    c_particle_movement_definition m_particle_movement;
public:
    __forceinline const c_particle_movement_definition *get_particle_movement() const { return &m_particle_movement; }
    c_editable_property<real_vector3d> m_particle_self_acceleration;
private:
    c_editable_property<c_particle_state_list> m_particle_velocity;
    c_editable_property<c_particle_state_list> m_particle_rotation;
    c_editable_property<c_particle_state_list> m_particle_angular_velocity;
    c_editable_property<c_particle_state_list> m_particle_size;
    c_editable_property<c_particle_state_list> m_particle_scale;
    c_editable_property<c_particle_state_list> m_particle_tint;
    c_editable_property<c_particle_state_list> m_particle_alpha;
    c_editable_property<c_particle_state_list> m_particle_alpha_black_point;
    unsigned long m_constant_per_particle_properties;
    unsigned long m_constant_over_time_properties;
    unsigned long m_used_particle_states;
public:
    unsigned long count_b;
    TAG_POINTER(const void) arg_b;
private:
    TAG_POINTER(const void) m_gpu_properties_definition;
public:
    long value_a;
    TAG_POINTER(const void) value_b;
private:
    TAG_POINTER(const void) m_gpu_functions_definition;
public:
    unsigned long count_a;
    TAG_POINTER(const void) arg_a;
private:
    TAG_POINTER(const void) m_gpu_colors_definition;
public:
    __forceinline unsigned long get_used_particle_states() const { return m_used_particle_states; }
};

__forceinline const c_particle_emitter_definition *c_particle_system_definition::get_emitter(long index) const
{
    return &emitters[index];
}

typedef char particle_emitter_definition_size[(sizeof(void *) != 4 || sizeof(c_particle_emitter_definition) == 0x2f0) ? 1 : -1];

typedef char particle_emitter_starting_count_offset[(offsetof(c_particle_emitter_definition, m_starting_count) == 0xe8) ? 1 : -1];
typedef char particle_emitter_max_count_offset[(offsetof(c_particle_emitter_definition, m_max_count) == 0x108) ? 1 : -1];
typedef char particle_emitter_emission_rate_offset[(offsetof(c_particle_emitter_definition, m_emission_rate) == 0x128) ? 1 : -1];
typedef char particle_emitter_uv_offset[(offsetof(c_particle_emitter_definition, uv) == 0x10) ? 1 : -1];
typedef char particle_emitter_gpu_count_offset[(sizeof(void *) != 4 || offsetof(c_particle_emitter_definition, count_a) == 0x2e4) ? 1 : -1];
typedef char particle_emitter_acceleration_offset[(offsetof(c_particle_emitter_definition, m_particle_self_acceleration) == 0x188) ? 1 : -1];
#endif // EFFECTS_PARTICLE_EMITTER_DEFINITIONS_H
