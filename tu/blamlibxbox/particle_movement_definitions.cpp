// blamlibxbox/particle_movement_definitions.obj: from the records 0x14ab60 0x14ac58 0x14aee0 0x150d38 0x313d70 0x313ec0 0x313fd0 0x3145a8 0x314810 0x3148b8 0x314900
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "tag_files/tag_types.h"
#include "memory/data_types.h"
#include "effects/particle.h"
#include "physics/collision_types.h"
#include "objects/object_reference_types.h"
// @address 0x150d38 ?get_property_by_index@c_particle_movement_definition@@QBAPBV?$c_editable_property@Vc_particle_state_list@@@@JJ@Z
// @address 0x314810 ?initialize@c_particle_collision_event@@QAAXPBTreal_point3d@@PBTreal_vector3d@@1Vc_global_material_type@@@Z

#include "math/matrix_math.h"
#include "memory/tls_globals.h"
#include "math/periodic_functions.h"
#include "effects/particle_movement_definitions.h"
#include "effects/particle_system.h"
#include "math/real_math_globals.h"
#include "physics/collision_bsp_types.h"
#include "animations/animation_codec_types.h"
#include "effects/particle_movement_internals.h"
#include <math.h>

struct s_particle_system_state;
#include "effects/particle_states.h"
#include "physics/physics_constants_globals.h"
#include "effects/particle_location_runtime_types.h"
#include "physics/collision_test_vector.h"
#include "tag_files/tag_groups.h"
#include "effects/particle_definitions.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_definitions.h"

#include "cseries/bit_operations.h"

// c_particle_movement_definition::update_particles

// @address 0x14ab60 ?update_particles@c_particle_movement_definition@@QBAXPBVc_particle_system@@PBVc_particle_location@@JM@Z
void c_particle_movement_definition::update_particles(
    class c_particle_system const *system, class c_particle_location const *location, long particle_count, float dt) const
{
    for (long controller_index = 0; controller_index < this->m_controllers.count; controller_index++)
    {
        const c_particle_controller *controller = this->get_controller_by_index(controller_index);
        controller->update_particles(system, location, particle_count, dt, this->m_flags);
    }
}

const c_editable_property<c_particle_state_list> *c_particle_movement_definition::get_property_by_index(long type, long index) const
{
    // match: 0x150d38: all callers pass type 0; without this the controller-type loop compares cmpw r8,r4 instead of cmplwi r8,0
    __assume(type == 0);
    const c_editable_property<c_particle_state_list> *property = 0;
    const c_particle_controller *controller = get_controller_by_type(type);
    if (controller)
    {
        const c_particle_controller_parameter *parameter = &controller->get_parameter_by_index(index);
        if (parameter)
            property = &parameter->get_property();
    }
    return property;
}

void c_particle_collision_event::initialize(
    const real_point3d *position, const real_vector3d *direction, const real_vector3d *normal, c_global_material_type material_type)
{
    real_vector3d forward = *direction;
    normalize3d_with_default(&forward, global_down3d);
    m_material_type = material_type;
    m_position = *position;
    m_forward = forward;
    m_normal = *normal;
}


// @address 0x3148b8 animation_fullframe_get_translation
void animation_fullframe_get_translation(
    // match: 0x3148b8: __restrict on out lets the translation j/k loads issue before the first store
    s_fullframe_codec_header *data, long channel, int time, int, int, int, real_orientation *__restrict out)
{
    out->translation = data->translation_offset.get(data, data->translation_stride * channel)[time];
}

// @address 0x314900 animation_fullframe_get_scale
void animation_fullframe_get_scale(
    s_fullframe_codec_header *data, long channel, int time, int, int, int, real_orientation *out)
{
    out->scale = data->scale_offset.get(data, data->scale_stride * channel)[time];
}

// match: 0x313d70, 0x313fd0: the real_math.h add_vectors3d form swaps the fadds/fmuls operands here
static __forceinline real_vector3d *particle_movement_definitions_add_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->i;
    result->i = i + b->i;
    result->j = a->j + b->j;
    result->k = a->k + b->k;
    return result;
}

static __forceinline float particle_movement_definitions_ratio(float distance, const real_point3d *a, const real_point3d *b)
{
    real_vector3d displacement;
    real_vector3d vector = *vector_from_points3d(a, b, &displacement);
    float squared = vector.i * vector.i + vector.j * vector.j + vector.k * vector.k;
    return sqrtf(squared) / MAX(distance, 0.1f);
}
static __forceinline void particle_movement_definitions_separation_direction(real_vector3d *direction, float ratio, float strength)
{
    if (ratio < 1.0f)
    {
        scale_vector3d(direction, -(ratio * strength), direction);
    }
    else
    {
        scale_vector3d(direction, strength, direction);
    }
}
// @address 0x313d70 ?accumulate_separation_force@@YAXPBVc_particle@@0MMPATreal_vector3d@@@Z
void accumulate_separation_force(
    const c_particle *particle, const c_particle *neighbor, float separation_distance, float separation_strength, real_vector3d *force)
{
    if (fabsf(separation_strength) >= 0.0001f)
    {
        float ratio = particle_movement_definitions_ratio(separation_distance, &neighbor->m_position, &particle->m_position);
        real_vector3d direction;
        vector_from_points3d(&particle->m_position, &neighbor->m_position, &direction);
        normalize3d(&direction);
        particle_movement_definitions_separation_direction(&direction, ratio, separation_strength);
        particle_movement_definitions_add_vectors3d(force, &direction, force);
    }
}

// @address 0x14ac58 ?update_particles_physics@c_particle_controller@@ABAXPBVc_particle_system@@PBVc_particle_location@@JM@Z
void c_particle_controller::update_particles_physics(
    const c_particle_system *system, const c_particle_location *location, long first_particle_index, float dt) const
{
    const c_particle_controller_parameter *parameter_list = (const c_particle_controller_parameter *)m_parameters.address;
    long particle_index = first_particle_index;
    c_particle_state_list state_list;
    float parameters[3] = {0.0f, 0.0f, 0.0f};
    unsigned long constant_parameters = m_runtime_data[0];
    state_list.set_particle_system(system);
    for (unsigned long parameter_index = 0; parameter_index < 3 && constant_parameters; ++parameter_index) {
        unsigned long mask = 1 << parameter_index;
        if (constant_parameters & mask) {
            parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
            constant_parameters &= ~mask;
        }
    }
    while (particle_index != -1) {
        c_particle *particle = &((c_particle *)g_particle_data->data)[(unsigned short)particle_index];
        particle_index = particle->m_next_index;
        if (particle->get_run_physics()) {
            state_list.set_particle(particle);
            for (unsigned long parameter_index = 0; parameter_index < 3; ++parameter_index) {
                if (!((unsigned long)m_runtime_data[0] & (1 << parameter_index)))
                    parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
            }
            float drag = parameters[1] * dt;
            float rotational_drag = parameters[2] * dt;
            drag = MIN(MAX(drag, 0.0f), 1.0f);
            rotational_drag = MIN(MAX(rotational_drag, 0.0f), 1.0f);
            particle->m_velocity.k -= g_physics_constants->gravity * parameters[0] * dt;
            scale_vector3d(&particle->m_velocity, 1.0f - drag, &particle->m_velocity);
            particle->m_rotational_velocity *= 1.0f - rotational_drag;
        }
    }
}

// @rdata 0x8202490c
static const float particle_movement_definitions_normalization_epsilon = 0.0001f;

static __forceinline float particle_movement_definitions_normalize3d(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < particle_movement_definitions_normalization_epsilon)) {
        scale_vector3d(vector, 1.0f / length, vector);
        return length;
    }
    return 0.0f;
}

// match: 0x313fd0: the extra inline boundary orders the minimum-speed fmuls operands
static __forceinline float particle_movement_definitions_normalize3d_ptr(real_vector3d *vector)
{
    return particle_movement_definitions_normalize3d(vector);
}

static __forceinline c_particle *particle_movement_definitions_get_particle(long index)
{
    return &((c_particle *)g_particle_data->data)[(unsigned short)index];
}

static __forceinline const c_particle *particle_movement_definitions_find_nearest_particle(
    const c_particle *particle, long first_particle_index, const c_particle_system *system)
{
    long particle_index = first_particle_index;
    float nearest_distance_squared = 0.0f;
    const c_particle *nearest = 0;
    real_vector3d direction = particle->m_velocity;
    particle_movement_definitions_normalize3d(&direction);
    while (particle_index != -1) {
        const c_particle *other = particle_movement_definitions_get_particle(particle_index);
        particle_index = other->m_next_index;
        if (other != particle) {
            real_vector3d displacement;
            vector_from_points3d(&other->m_position, &particle->m_position, &displacement);
            float distance_squared = dot_product3d(&displacement, &displacement);
            if (distance_squared < nearest_distance_squared || !nearest) {
                nearest = other;
                nearest_distance_squared = distance_squared;
            }
        }
    }
    return nearest;
}

static __forceinline void particle_movement_definitions_accumulate_alignment_force(
    const c_particle *particle, const c_particle *neighbor, float strength, real_vector3d *force)
{
    real_vector3d direction = particle->m_velocity;
    real_vector3d neighbor_direction = neighbor->m_velocity;
    particle_movement_definitions_normalize3d(&direction);
    particle_movement_definitions_normalize3d(&neighbor_direction);
    real_vector3d difference;
    subtract_vectors3d(&neighbor_direction, &direction, &difference);
    scale_vector3d(&difference, strength, &difference);
    particle_movement_definitions_add_vectors3d(force, &difference, force);
}

static __forceinline void particle_movement_definitions_accumulate_cohesion_force(
    const c_particle *particle, const real_point3d *center, float distance, float strength, real_vector3d *force)
{
    if (distance > 0.0f) {
        real_vector3d direction;
        vector_from_points3d(&particle->m_position, center, &direction);
        float magnitude = dot_product3d(&direction, &direction);
        magnitude = sqrtf(magnitude);
        strength *= magnitude / distance;
        particle_movement_definitions_normalize3d(&direction);
        scale_vector3d(&direction, strength, &direction);
        particle_movement_definitions_add_vectors3d(force, &direction, force);
    }
}


// @address 0x313fd0 ?update_particles_flocking@c_particle_controller@@ABAXPBVc_particle_system@@PBVc_particle_location@@JM@Z
void c_particle_controller::update_particles_flocking(
    const c_particle_system *system, const c_particle_location *location, long first_particle_index, float dt) const
{
    const long separation_distance_parameter = 0;
    const long separation_strength_parameter = 1;
    const long cohesion_distance_parameter = 2;
    const long cohesion_strength_parameter = 3;
    const c_particle_controller_parameter *parameter_list = (const c_particle_controller_parameter *)m_parameters.address;
    long particle_index = first_particle_index;
    c_particle_state_list state_list;
    state_list.set_particle_system(system);
    while (particle_index != -1) {
        c_particle *particle = particle_movement_definitions_get_particle(particle_index);
        state_list.set_particle(particle);
        particle_index = particle->m_next_index;
        const c_particle *neighbor = particle_movement_definitions_find_nearest_particle(particle, first_particle_index, system);
        const float minimum_speed = parameter_list[6].get_property().evaluate(&state_list);
        float maximum_speed = parameter_list[7].get_property().evaluate(&state_list);
        float maximum_acceleration = parameter_list[8].get_property().evaluate(&state_list);
        real_vector3d force = {0.0f, 0.0f, 0.0f};
        if (neighbor) {
            float separation_strength = parameter_list[separation_strength_parameter].get_property().evaluate(&state_list);
            accumulate_separation_force(particle, neighbor,
                parameter_list[separation_distance_parameter].get_property().evaluate(&state_list), separation_strength, &force);
            particle_movement_definitions_accumulate_alignment_force(particle, neighbor, parameter_list[4].get_property().evaluate(&state_list), &force);
        }
        particle_movement_definitions_accumulate_cohesion_force(particle, location->get_position(),
            parameter_list[cohesion_distance_parameter].get_property().evaluate(&state_list),
            parameter_list[cohesion_strength_parameter].get_property().evaluate(&state_list), &force);
        accumulate_avoidance_force(particle, parameter_list[5].get_property().evaluate(&state_list), &force);
        scale_vector3d(&force, dt, &force);
        float magnitude = sqrtf(dot_product3d(&force, &force));
        if (magnitude > maximum_acceleration) {
            particle_movement_definitions_normalize3d(&force);
            scale_vector3d(&force, maximum_acceleration, &force);
        }
        real_vector3d old_velocity = particle->m_velocity;
        scale_vector3d(&old_velocity, 0.75f, &old_velocity);
        particle_movement_definitions_add_vectors3d(&particle->m_velocity, &force, &particle->m_velocity);
        scale_vector3d(&particle->m_velocity, 0.25f, &particle->m_velocity);
        particle_movement_definitions_add_vectors3d(&particle->m_velocity, &old_velocity, &particle->m_velocity);
        float speed_squared = dot_product3d(&particle->m_velocity, &particle->m_velocity);
        if (speed_squared > maximum_speed * maximum_speed) {
            particle_movement_definitions_normalize3d(&particle->m_velocity);
            scale_vector3d(&particle->m_velocity, maximum_speed, &particle->m_velocity);
        } else if (speed_squared < minimum_speed * minimum_speed) {
            particle_movement_definitions_normalize3d_ptr(&particle->m_velocity);
            scale_vector3d(&particle->m_velocity, minimum_speed, &particle->m_velocity);
        }
    }
}


// @address 0x3145a8 ?update_particles_wind@c_particle_controller@@ABAXPBVc_particle_system@@PBVc_particle_location@@JM@Z
void c_particle_controller::update_particles_wind(
    const c_particle_system *system, const c_particle_location *location, long first_particle_index, float dt) const
{
    const c_particle_controller_parameter *parameter_list = (const c_particle_controller_parameter *)m_parameters.address;
    long particle_index = first_particle_index;
    c_particle_state_list state_list;
    float parameters[3] = {0.0f, 0.0f, 0.0f};
    unsigned long constant_parameters = m_runtime_data[0];
    state_list.set_particle_system(system);
    for (unsigned long parameter_index = 0; parameter_index < 3 && constant_parameters; ++parameter_index) {
        unsigned long mask = 1 << parameter_index;
        if (constant_parameters & mask) {
            parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
            constant_parameters &= ~mask;
        }
    }
    while (particle_index != -1) {
        c_particle *particle = &((c_particle *)g_particle_data->data)[(unsigned short)particle_index];
        particle_index = particle->m_next_index;
        if (particle->get_run_physics()) {
            state_list.set_particle(particle);
            for (unsigned long parameter_index = 0; parameter_index < 3; ++parameter_index) {
                if (!((unsigned long)m_runtime_data[0] & (1 << parameter_index)))
                    parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
            }
            float wind = periodic_function_evaluate(10, (float)tls_game_time_globals()->time * tls_game_time_globals()->tick_length + dt * parameters[1]);
            wind -= 0.5f;
            wind *= dt;
            particle->m_rotational_velocity += wind * parameters[2];
            real_vector3d wind_direction;
            real_vector3d velocity_change;
            scale_vector3d(&wind_direction, wind * parameters[0], &velocity_change);
            add_vectors3d(&particle->m_velocity, &velocity_change, &particle->m_velocity);
        }
    }
}

// @address 0x313ec0 ?accumulate_avoidance_force@@YAXPBVc_particle@@MPATreal_vector3d@@@Z
void accumulate_avoidance_force(const c_particle *particle, float distance, real_vector3d *force)
{
    s_collision_test_flags flags;
    flags.collision_flags = 0x800;
    flags.object_flags = 0;
    s_collision_test_flags test_flags = flags;
    real_vector3d direction = particle->m_velocity;
    test_flags.collision_flags |= 9;
    collision_result collision;
    normalize3d(&direction);
    scale_vector3d(&direction, distance, &direction);
    if (collision_test_vector(test_flags, false, &particle->m_position, &direction, -1, -1, -1, &collision))
    {
        force->i = force->i + collision.plane.n.i;
        force->j = collision.plane.n.j + force->j;
        force->k = collision.plane.n.k + force->k;
    }
}

static __forceinline bool particle_movement_test_bit(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}

// @address 0x14aee0 ?update_particles_collider@c_particle_controller@@ABAXPBVc_particle_system@@PBVc_particle_location@@JMK@Z
void c_particle_controller::update_particles_collider(
    const c_particle_system *system, const c_particle_location *location, long first_particle_index, float dt, unsigned long flags) const
{
    const c_particle_controller_parameter *parameter_list = (const c_particle_controller_parameter *)m_parameters.address;
    const c_particle_definition *definition = system->get_particle_definition();
    long particle_index = first_particle_index;
    c_particle_state_list state_list;
    float parameters[2] = {0.0f, 0.0f};
    unsigned long constant_parameters = m_runtime_data[0];
    real_vector3d delta;
    state_list.set_particle_system(system);
    for (unsigned long parameter_index = 0; parameter_index < 2 && constant_parameters; ++parameter_index) {
        unsigned long mask = 1 << parameter_index;
        if (constant_parameters & mask) {
            parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
            constant_parameters &= ~mask;
        }
    }
    while (particle_index != -1) {
        c_particle *particle = &((c_particle *)g_particle_data->data)[(unsigned short)particle_index];
        particle_index = particle->m_next_index;
        if (particle->get_run_physics()) {
            if (!(particle->m_flags & 7)) {
                s_collision_test_flags collision_test_flags;
                collision_test_flags.collision_flags = 0x4800;
                collision_test_flags.object_flags = 0;
                s_collision_test_flags test_flags = collision_test_flags;
                SET_BIT(test_flags.collision_flags, 0, particle_movement_test_bit(flags, 1));
                SET_BIT(test_flags.collision_flags, 3, particle_movement_test_bit(flags, 1));
                SET_BIT(test_flags.collision_flags, 1, particle_movement_test_bit(flags, 2));
                SET_BIT(test_flags.object_flags, 2, particle_movement_test_bit(flags, 4));
                SET_BIT(test_flags.object_flags, 7, particle_movement_test_bit(flags, 3));
                SET_BIT(test_flags.object_flags, 1, particle_movement_test_bit(flags, 5));
                if (test_flags.object_flags & 0x86)
                    test_flags.object_flags |= FLAG(0);
                if (particle_movement_test_bit(test_flags.collision_flags, 0) ||
                    particle_movement_test_bit(test_flags.collision_flags, 1) ||
                    particle_movement_test_bit(test_flags.collision_flags, 3) || particle_movement_test_bit(test_flags.object_flags, 0))
                {
                    collision_result collision;
                    delta.i = dt * particle->m_velocity.i;
                    delta.j = dt * particle->m_velocity.j;
                    delta.k = dt * particle->m_velocity.k;
                    if (collision_test_vector(test_flags, false, &particle->m_position, &delta, -1, -1, -1, &collision)) {
                        if (collision.fraction < 1.0f) {
                            state_list.set_particle(particle);
                            for (unsigned long parameter_index = 0; parameter_index < 2; ++parameter_index) {
                                if (!((unsigned long)m_runtime_data[0] & (1 << parameter_index)))
                                    parameters[parameter_index] = parameter_list[parameter_index].get_property().evaluate(&state_list);
                            }
                            float elasticity = 1.0f - parameters[0];
                            float friction = parameters[1];
                            c_particle_collision_event collision_event;
                            real_vector3d parallel;
                            real_vector3d perpendicular;
                            component_vectors_from_normal3d(&particle->m_velocity, &collision.plane.n, &parallel, &perpendicular);
                            particle->m_position.x = collision.point.x + collision.plane.n.i * 0.005f;
                            particle->m_position.y = collision.point.y + collision.plane.n.j * 0.005f;
                            particle->m_position.z = collision.point.z + collision.plane.n.k * 0.005f;
                            collision_event.initialize(&particle->m_position, &particle->m_velocity, &collision.plane.n, collision.global_material);
                            particle->m_velocity.i = perpendicular.i * elasticity - parallel.i * friction;
                            particle->m_velocity.j = perpendicular.j * elasticity - parallel.j * friction;
                            particle->m_velocity.k = perpendicular.k * elasticity - parallel.k * friction;
                            particle->m_rotational_velocity *= elasticity;
                            if (!TEST_BIT(particle->m_flags, 4)) {
                                if (system->get_particle_system_definition()->get_camera_mode() != 1 && definition->get_has_collision_effect())
                                    particle_handle_collision_event(particle, 1, -1, &collision_event, &state_list);
                                particle->m_flags |= FLAG(4);
                            }
                            SET_BIT(particle->m_flags, 7, collision.plane.n.k > 0.8f);
                            if ((collision.type == 1 && definition->get_particle_dies_on_structure_collision()) ||
                                (collision.type == 3 && definition->get_particle_dies_on_structure_collision()) ||
                                (collision.type == 2 && definition->get_particle_dies_on_media_collision()))
                                particle->m_flags |= FLAG(3);
                            particle->m_flags = (particle->m_flags & ~7) | 1;
                        } else {
                            particle->m_flags = (particle->m_flags & ~7) | 1;
                        }
                    } else {
                        particle->m_flags = (particle->m_flags & ~7) | 1;
                        particle->m_flags &= ~FLAG(4);
                    }
                } else {
                    particle->m_flags &= ~FLAG(4);
                }
            }
            particle->m_flags = (particle->m_flags & ~7) | ((particle->m_flags - 1) & 7);
        }
    }
}
