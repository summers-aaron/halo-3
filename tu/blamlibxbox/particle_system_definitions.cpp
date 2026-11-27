// blamlibxbox/particle_system_definitions.obj: from the records 0x143df0 0x144688
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "math/real_math.h"
#include "memory/data.h"
#include "physics/breakable_surfaces.h"
#include "effects/particle_effect_data_array_types.h"
#include "math/function_definitions.h"
#include "effects/particle_system.h"
#include "effects/particle_emitter.h"
#include "effects/particle_states.h"
#include "effects/editable_property.h"
#include "effects/effect_datum.h"
#include "effects/particle.h"
#include "effects/particle_definitions.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "math/real_math_trigonometry.h"
#include "effects/particle_emitter_definitions.h"

static __forceinline bool particle_system_definitions_is_constant(unsigned char flags)
{
    return (flags & 0x20) != 0;
}

template<> float c_editable_property_base::evaluate<c_particle_state_list>(const c_particle_state_list *states) const
{
    if (particle_system_definitions_is_constant(m_flags))
        return m_constant;
    else
    {
        float range = states->get_state_value(m_range);
        float input = states->get_state_value(m_input);
        float value = m_function.evaluate_scalar(input, range);
        long modifier_input = m_modifier_input;
        if (modifier_input != -1)
        {
            switch (m_modifier)
            {
            case 1: value += states->get_state_value(modifier_input); break;
            case 2: value *= states->get_state_value(modifier_input); break;
            }
        }
        return value;
    }
}

static __forceinline const real_point2d *effect_get_impact_size(long effect_index)
{
    return &((const effect_datum *)effect_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(effect_index)].impact_coordinates;
}

__forceinline void c_particle_system::initialize_particle(const c_particle_state_list *state_list, c_particle *particle) const
{
    const c_particle_definition *particle_definition = get_particle_definition();
    particle_definition->initialize_particle(state_list, particle, m_lightprobe_index);
}
__forceinline unsigned short c_particle_system::internal_random() const
{
    return _random(&m_random_seed, __FILE__, __FUNCTION__, __LINE__);
}
__forceinline float c_particle_system::real_internal_random_range(float lower, float upper) const
{
    return _real_random_range(&m_random_seed, __FILE__, __FUNCTION__, __LINE__, lower, upper);
}
__forceinline void c_particle_system::internal_random_direction3d(real_vector3d *direction) const
{
    _random_direction3d(&m_random_seed, __FILE__, __FUNCTION__, __LINE__, direction);
}

// @address 0x144688 ?initialize_particle@c_particle_emitter_definition@@QBAXPBVc_particle_state_list@@PAVc_particle@@PAVc_particle_emitter@@PBVc_particle_system@@@Z
void c_particle_emitter_definition::initialize_particle(const c_particle_state_list *state_list, c_particle *particle,
    c_particle_emitter *particle_emitter, const c_particle_system *particle_system) const
{
    particle_emitter->get_property_values()->particle_lifespan = m_particle_lifespan.evaluate(state_list);
    particle->m_inverse_lifespan = 1.0f / MAX(particle_emitter->get_property_values()->particle_lifespan, 0.01f);
    particle->m_random_offset = 0.0f;
    particle_system->initialize_particle(state_list, particle);

    particle_emitter->get_property_values()->emission_angle = m_emission_angle.evaluate(state_list);
    particle_emitter->get_property_values()->emission_radius = m_emission_radius.evaluate(state_list);

    switch (m_emission_shape)
    {
    case _emission_shape_sprayer:
    {
        float phi = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float theta = particle_system->real_internal_random_range(0.0f, particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians);
        float sin_theta = sine(theta);
        particle->m_position.x = 0.0f;
        particle->m_position.y = 0.0f;
        particle->m_position.z = 0.0f;
        particle->m_velocity.i = cosine(theta);
        particle->m_velocity.j = sine(phi) * sin_theta;
        particle->m_velocity.k = cosine(phi) * sin_theta;
    }
    break;
    case _emission_shape_disc:
    {
        float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        particle->m_velocity.i = 0.0f;
        particle->m_velocity.j = sine(angle);
        particle->m_velocity.k = cosine(angle);
        particle->m_position.x = 0.0f;
        particle->m_position.y = particle->m_velocity.j * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = particle->m_velocity.k * particle_emitter->get_property_values()->emission_radius;
    }
    break;
    case _emission_shape_globe:
    {
        particle_system->internal_random_direction3d(&particle->m_velocity);
        particle->m_position.x = particle->m_velocity.i * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.y = particle->m_velocity.j * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = particle->m_velocity.k * particle_emitter->get_property_values()->emission_radius;
    }
    break;
    case _emission_shape_implode:
    {
        real_vector3d direction;
        particle_system->internal_random_direction3d(&direction);
        particle->m_position.x = direction.i * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.y = direction.j * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = direction.k * particle_emitter->get_property_values()->emission_radius;
        particle->m_velocity.i = -direction.i;
        particle->m_velocity.j = -direction.j;
        particle->m_velocity.k = -direction.k;
    }
    break;
    case _emission_shape_tube:
    {
        float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float cone_angle = particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians;
        float sin_angle = sine(angle);
        float cos_angle = cosine(angle);
        float sin_cone_angle = sine(cone_angle);
        particle->m_position.x = 0.0f;
        particle->m_position.y = sin_angle * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = cos_angle * particle_emitter->get_property_values()->emission_radius;
        particle->m_velocity.i = cosine(cone_angle);
        particle->m_velocity.j = sin_angle * sin_cone_angle;
        particle->m_velocity.k = cos_angle * sin_cone_angle;
    }
    break;
    case _emission_shape_halo:
    {
        float phi = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float theta = particle_system->real_internal_random_range(0.0f, particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians);
        float sin_theta = sine(theta);
        particle->m_velocity.i = cosine(theta);
        particle->m_velocity.j = sine(phi) * sin_theta;
        particle->m_velocity.k = cosine(phi) * sin_theta;
        real_vector3d direction;
        particle_system->internal_random_direction3d(&direction);
        particle->m_position.x = direction.i * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.y = direction.j * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = direction.k * particle_emitter->get_property_values()->emission_radius;
    }
    break;
    case _emission_shape_impact_contour:
    {
        long effect_index = particle_system->get_effect_index();
        const real_point2d *impact_size = effect_index != NONE ? effect_get_impact_size(effect_index) : global_origin2d;
        float cone_angle = particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians;
        bool horizontal = (float)(particle_system->internal_random() & 1) == 0.0f;
        particle->m_position.x = 0.0f;
        particle->m_velocity.i = cosine(cone_angle);
        if (horizontal)
        {
            float sign = (particle_system->internal_random() & 1) ? -1.0f : 1.0f;
            particle->m_position.y = sign * impact_size->x;
            particle->m_position.z = particle_system->real_internal_random_range(-impact_size->y, impact_size->y);
            particle->m_velocity.j = sign * sine(cone_angle);
            particle->m_velocity.k = 0.0f;
        }
        else
        {
            float sign = (particle_system->internal_random() & 1) ? -1.0f : 1.0f;
            particle->m_position.y = particle_system->real_internal_random_range(-impact_size->x, impact_size->x);
            particle->m_position.z = sign * impact_size->y;
            particle->m_velocity.j = 0.0f;
            particle->m_velocity.k = sign * sine(cone_angle);
        }
    }
    break;
    case _emission_shape_impact_area:
    {
        long effect_index = particle_system->get_effect_index();
        const real_point2d *impact_size = effect_index != NONE ? effect_get_impact_size(effect_index) : global_origin2d;
        float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float cone_angle = particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians;
        float sin_angle = sine(angle);
        float cos_angle = cosine(angle);
        float sin_cone_angle = sine(cone_angle);
        particle->m_position.x = 0.0f;
        particle->m_position.y = particle_system->real_internal_random_range(-impact_size->x, impact_size->x);
        particle->m_position.z = particle_system->real_internal_random_range(-impact_size->y, impact_size->y);
        particle->m_velocity.i = cosine(cone_angle);
        particle->m_velocity.j = sin_angle * sin_cone_angle;
        particle->m_velocity.k = cos_angle * sin_cone_angle;
    }
    break;
    case _emission_shape_debris:
    {
        float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float sin_angle = sine(angle);
        float cos_angle = cosine(angle);
        particle->m_position.x = 0.0f;
        particle->m_position.y = sin_angle * particle_emitter->get_property_values()->emission_radius;
        particle->m_position.z = cos_angle * particle_emitter->get_property_values()->emission_radius;
        particle->m_velocity.i = 0.0f;
        particle->m_velocity.j = 0.0f;
        particle->m_velocity.k = 1.0f;
    }
    break;
    case _emission_shape_line:
    {
        float phi = particle_system->real_internal_random_range(0.0f, 6.28318531f);
        float theta = particle_system->real_internal_random_range(0.0f, particle_emitter->get_property_values()->emission_angle * k_degrees_to_radians);
        float sin_theta = sine(theta);
        particle->m_position.x = 0.0f;
        particle->m_position.y = particle_system->real_internal_random_range(
            -particle_emitter->get_property_values()->emission_radius, particle_emitter->get_property_values()->emission_radius);
        particle->m_position.z = 0.0f;
        particle->m_velocity.i = cosine(theta);
        particle->m_velocity.j = sine(phi) * sin_theta;
        particle->m_velocity.k = cosine(phi) * sin_theta;
    }
    break;
    case _emission_shape_breakable_surface:
    {
        particle->m_position = *global_origin3d;
        particle->m_velocity = *global_zero_vector3d;
        if (particle_system->m_breakable_surface_event_index != NONE)
            breakable_surfaces_get_particle_emission(particle_system->m_breakable_surface_event_index, &particle->m_position, &particle->m_velocity);
    }
    break;
    default:
        UNREACHABLE();
    }

    if (particle_system->get_particle_definition()->get_particle_is_particle_model())
    {
        switch (m_particle_axis)
        {
        case _particle_axis_emitter:
            particle->m_axis.i = 1.0f;
            particle->m_axis.j = 0.0f;
            particle->m_axis.k = 0.0f;
            break;
        case _particle_axis_random_cone:
        {
            float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
            particle_emitter->get_property_values()->emission_axis_angle = m_emission_axis_angle.evaluate(state_list);
            float theta = particle_system->real_internal_random_range(
                0.0f, particle_emitter->get_property_values()->emission_axis_angle *k_degrees_to_radians);
            float sin_theta = sine(theta);
            particle->m_axis.i = cosine(theta);
            particle->m_axis.j = sine(angle) * sin_theta;
            particle->m_axis.k = cosine(angle) * sin_theta;
        }
        break;
        case _particle_axis_random_disc:
        {
            float angle = particle_system->real_internal_random_range(0.0f, 6.28318531f);
            particle->m_axis.i = 0.0f;
            particle->m_axis.j = sine(angle);
            particle->m_axis.k = cosine(angle);
        }
        break;
        case _particle_axis_random_direction:
            particle_system->internal_random_direction3d(&particle->m_axis);
            break;
        }

        switch (m_particle_reference_axis)
        {
        case 1:
        {
            real_vector3d axis;
            axis.i = particle->m_axis.k;
            axis.j = particle->m_axis.i;
            axis.k = particle->m_axis.j;
            particle->m_axis = axis;
        }
        break;
        case 2:
        {
            real_vector3d axis;
            axis.i = particle->m_axis.j;
            axis.j = particle->m_axis.k;
            axis.k = particle->m_axis.i;
            particle->m_axis = axis;
        }
        break;
        }
    }

    normalize3d(&particle->m_velocity);
    particle_emitter->get_property_values()->particle_velocity = m_particle_velocity.evaluate(state_list);
    particle_emitter->get_property_values()->particle_angular_velocity = m_particle_angular_velocity.evaluate(state_list);
    if (particle_emitter->get_property_values()->particle_velocity == 0.0f)
        particle_emitter->get_property_values()->particle_velocity = 0.001f;
    scale_vector3d(&particle->m_velocity, particle_emitter->get_property_values()->particle_velocity, &particle->m_velocity);
    particle->m_rotational_velocity = particle_emitter->get_property_values()->particle_angular_velocity;
}

// @address 0xea4f8 ?evaluate_scalar@c_function_definition@@QBAMMM@Z
// @address 0x143df0 ??$evaluate@Vc_particle_state_list@@@c_editable_property_base@@QBAMPBVc_particle_state_list@@@Z
