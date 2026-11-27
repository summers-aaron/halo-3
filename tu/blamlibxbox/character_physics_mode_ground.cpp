// blamlibxbox/character_physics_mode_ground.obj: from the records 0xe7118 0xe7498 0xe7a38 0x10baa8 0x10c518 0x10d248 0x10d6e0 0x2b1998 0x2b19f8 0x2b1b38
// @compiler xdk6130
#include "memory/data.h"
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "physics/character_physics_globals.h"
// @address 0x11df6bc game_time_globals
// @address 0x11df6a8 object_header_data
// @address 0x3aa90 k_real_math_zero_vector3d_storage
#include <stddef.h>
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "physics/havok_contact_point_reference.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "units/vehicle_types.h"
#include "game/game_time_types.h"
#include "physics/physics_constants.h"
#include "objects/object_early_movers.h"
#include "math/matrix_math.h"
#include "units/vehicles.h"
#include "game/materials.h"
#include "physics/havok_contact_points.h"
#include "physics/havok_component.h"
#include "physics/character_physics_definitions.h"

#include "physics/physics_constants_globals.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "math/real_math_globals.h"
#include "math/real_math_types.h"
#include "memory/tls_globals.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "units/vehicle_counters.h"
#include "math/real_math_constants.h"
#include "physics/character_physics_ground_helpers.h"
// @address 0x2251c k_real_zero_storage


class phys_shape;
class c_animation_manager;

#include "physics/havok_contact_point.h"
#include "physics/havok_component_class.h"

struct s_character_physics_move_input_datum;
struct s_character_physics_move_output_datum;

#include "physics/character_physics_move_input.h"
#include "physics/character_physics_move_output.h"
#include "physics/character_physics_object_access.h"
#include "physics/havok_contact_point_iterator.h"
#include "units/vehicle_type_human_tank.h"
#include "units/vehicle_type_human_jeep.h"
#include "units/vehicle_type_chopper.h"
#include "units/vehicle_datum.h"

namespace {
static __forceinline game_time_globals_definition *character_physics_mode_ground_game_time_globals_get()
{
    return game_time_globals;
}
static __forceinline float character_physics_mode_ground_game_tick_length()
{
    game_time_globals_definition *globals = character_physics_mode_ground_game_time_globals_get();
    return globals->tick_length;
}
static __forceinline real_vector3d *character_physics_mode_ground_cross_product3d_ordered(const real_vector3d *a, const real_vector3d *b,
    real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}
static __forceinline bool character_physics_mode_ground_adhesion_forces_disabled()
{
    return g_physics_constants->character_ground_adhesion_forces_disable_time - game_time_globals->time > 0;
}
static __forceinline float character_physics_mode_ground_sine(float angle) { return (float)sin(angle); }
}

namespace {
static __forceinline float character_physics_mode_ground_adhesion(const s_character_physics_update_input_datum *input, const real_vector3d *vector)
{
    float minimum = (input->flags & (1 << 3)) != 0 ? 0.12f : 0.24f;
    float maximum = (input->flags & (1 << 3)) != 0 ? 0.234f : 0.468f;
    float speed = magnitude3d(vector);
    float adhesion = maximum * (speed * 0.45454544f);
    adhesion = PIN(adhesion, minimum, maximum);
    if (input->ground_adhesion > 0.0f)
        adhesion *= input->ground_adhesion;
    return adhesion;
}
}

// @address 0x2b1998 ?initialize_internal@c_character_physics_mode_ground_datum@@AAAX_N@Z
void c_character_physics_mode_ground_datum::initialize_internal(bool reset)
{
    m_contact_material = k_global_material_type_invalid;
    m_ground_index = -1;
    m_landing_velocity = k_real_zero_storage;
    m_contact_flags &= ~1;
    if (reset) {
        m_ground_object_index = -1;
        m_ground_component_index = -1;
        m_ground_body_index = -1;
        memcpy(&m_ground_transform, global_identity4x3, sizeof(m_ground_transform));
    }
}

// @address 0x2b19f8 character_physics_mode_ground_clamp_velocity_change
extern "C" bool character_physics_mode_ground_clamp_velocity_change(const real_vector3d *normal, const real_vector3d *velocity, real_vector3d *result,
    float speed, bool flag, bool *started)
{
    real_vector3d direction = *velocity;
    float magnitude = ::normalize3d(&direction);
    bool output = false;
    float tick_length = game_time_globals->tick_length;
    float max_distance = tick_length * speed;
    if (flag)
    {
        float dot = normal->i * direction.i + (normal->k * direction.k + normal->j * direction.j);
        if (dot < 0.0f && magnitude > 0.8f)
        {
            float boost = (magnitude - 0.8f) * 0.23809524f;
            if (boost >= 1.0f)
                boost = 1.0f;
            *started = true;
            max_distance += (boost * boost * 20.0f) * tick_length;
        }
    }
    if (magnitude > max_distance)
    {
        result->i = direction.i * max_distance;
        result->j = direction.j * max_distance;
        result->k = direction.k * max_distance;
        output = true;
    }
    else
        *result = *velocity;
    return output;
}
// @address 0x10d6e0 ?set_contact@c_character_physics_mode_ground_datum@@AAAXJPBUs_havok_contact_point_reference@@PBTreal_vector3d@@@Z
void c_character_physics_mode_ground_datum::set_contact(
    long havok_component_index, const s_havok_contact_point_reference *contact_point_reference, const real_vector3d *normal)
{
    c_havok_component *component =
        datum_get<c_havok_component>(g_havok_component_data, havok_component_index);
    short rigid_body_index = contact_point_reference->rigid_body_index;
    m_ground_plane = *normal;
    if (rigid_body_index != -1)
    {
        c_havok_contact_point *contact_point
           = component->m_rigid_bodies[rigid_body_index].get_contact_point(contact_point_reference->contact_point_index);
        m_contact_material = contact_point->calculate_global_material_type_b();
        if (contact_point->m_surface_index != -1)
            m_contact_flags |= 1;
        else
            m_contact_flags &= ~1;
    }
    else
    {
        m_contact_material = k_global_material_type_default;
        m_contact_flags &= ~1;
    }
}

// @address 0x2b1b38 ?snap_facing@c_character_physics_mode_ground_datum@@QAAXPATreal_vector3d@@0PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_ground_datum::snap_facing(real_vector3d *forward, real_vector3d *up,
    const s_character_physics_update_input_datum *input)
{
    bool snap_to_ground = (input->flags & (1 << 6)) != 0;
    if (snap_to_ground)
    {
        bool aligned = false;
        real_vector3d ground_plane = m_ground_plane;
        {
        real_vector3d axis;
        cross_product3d(&input->up, &ground_plane, &axis);
        if (::normalize3d(&axis) == 0.0f)
        {
            if (dot_product3d(&input->up, &ground_plane) > 0.0f)
                aligned = true;
            else
                axis = input->up;
        }
        if (!aligned)
        {
            real_vector3d rotated_up = input->up;
            float angle = 5.2359877f * character_physics_mode_ground_game_tick_length();
            float sine = character_physics_mode_ground_sine(angle);
            float cosine = (float)cos(angle);
            rotate_vector_about_axis(&rotated_up, &axis, sine, cosine);
            real_vector3d cross;
            cross_product3d(&rotated_up, &ground_plane, &cross);
            if (dot_product3d(&axis, &cross) > 0.0f)
                ground_plane = rotated_up;
        }
        }
        real_vector3d left;
        real_vector3d ground_forward;
        cross_product3d(&input->forward, &ground_plane, &left);
        cross_product3d(&ground_plane, &left, &ground_forward);
        if (::normalize3d(&ground_forward) == 0.0f)
        {
            cross_product3d(&ground_plane, &input->up, &left);
            cross_product3d(&ground_plane, &left, &ground_forward);
            if (::normalize3d(&ground_forward) == 0.0f)
            {
                ground_plane = global_identity4x3->up;
                ground_forward = global_identity4x3->forward;
            }
        }
        *forward = ground_forward;
        *up = ground_plane;
    }
    else
    {
        memcpy(forward, &input->forward, sizeof(*forward));
        memcpy(up, &global_identity4x3->up, sizeof(*up));
        forward->k = 0.0f;
        if (::normalize3d(forward) == 0.0f)
            memcpy(forward, &global_identity4x3->forward, sizeof(*forward));
    }
}

// @rdata 0x82022dc0
static const float k_character_physics_mode_ground_acceleration_limit_padding = 0.5f;

// @address 0xe7a38 ?update_internal@c_character_physics_mode_ground_datum@@AAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@_NPBTreal_vector3d@@@Z
void c_character_physics_mode_ground_datum::update_internal(
    s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input, bool, const real_vector3d *vector)
{
    bool snap_to_ground = (input->flags & (1 << 6)) != 0;
    bool airborne = (input->flags & (1 << 2)) != 0;
    bool apply_friction = (input->flags & (1 << 5)) != 0;
    bool limit_upward = (input->flags & (1 << 11)) != 0;
    bool input_gravity = (input->flags & (1 << 4)) != 0;
    bool apply_gravity = input_gravity;
    // written but never read, as in the March source; c2 numbers these symbols
    float ground_i = 0.0f;
    float ground_j = 0.0f;
    bool started = false;
    if ((input->flags & 1) == 0)
    {
        real_vector3d left;
        real_vector3d scaled;
        character_physics_mode_ground_cross_product3d_ordered(&input->up, &input->desired_velocity, &left);
        scale_vector3d(&input->acceleration, input->scale, &scaled);
        scale_vector3d(&input->desired_velocity, scaled.i, &output->velocity);
        point_from_line3d(&output->velocity, &left, scaled.j, &output->velocity);
        point_from_line3d(&output->velocity, &input->up, scaled.k, &output->velocity);
        apply_gravity = false;
    }
    else if (airborne)
    {
        real_vector2d desired;
        real_vector3d delta;
        real_vector3d clipped;
        desired.i = input->acceleration.i * input->desired_velocity.i - input->acceleration.j * input->desired_velocity.j;
        desired.j = input->acceleration.i * input->desired_velocity.j + input->acceleration.j * input->desired_velocity.i;
        scale_vector2d(&desired, input->scale, &desired);
        delta.i = desired.i - vector->i;
        delta.j = desired.j - vector->j;
        if (limit_upward)
        {
            float k = -vector->k;
            delta.k = 0.0f > k ? k : 0.0f;
        }
        else
        {
            delta.k = 0.0f;
        }
        character_physics_mode_ground_clamp_velocity_change(vector, &delta, &clipped, input->maximum_slope, limit_upward, &started);
        add_vectors3d(vector, &clipped, &output->velocity);
        output->velocity.k = output->velocity.k - character_physics_mode_ground_game_tick_length() * input->movement_scale;
    }
    else
    {
        bool clipped_speed = false;
        float magnitude = magnitude3d(&input->acceleration);
        real_vector3d direction;
        if (snap_to_ground)
        {
            real_vector3d forward;
            real_vector3d left;
            memcpy(&forward, &input->desired_velocity, sizeof(forward));
            character_physics_mode_ground_cross_product3d_ordered(&input->ground_plane, &forward, &left);
            if (::normalize3d(&left) == 0.0f)
            {
                character_physics_mode_ground_cross_product3d_ordered(&input->ground_plane, global_up3d, &left);
                if (::normalize3d(&left) == 0.0f)
                {
                    character_physics_mode_ground_cross_product3d_ordered(&input->ground_plane, global_forward3d, &left);
                    ::normalize3d(&left);
                }
            }
            character_physics_mode_ground_cross_product3d_ordered(&left, &input->ground_plane, &forward);
            ::normalize3d(&forward);
            direction.i = input->acceleration.i * forward.i + input->acceleration.j * left.i;
            direction.j = input->acceleration.i * forward.j + input->acceleration.j * left.j;
            direction.k = input->acceleration.i * forward.k + input->acceleration.j * left.k;
            ::normalize3d(&direction);
        }
        else if (input->ground_plane.k > 0.001f)
        {
            direction.i = input->acceleration.i * input->desired_velocity.i - input->acceleration.j * input->desired_velocity.j;
            ground_i = direction.i;
            direction.j = input->acceleration.i * input->desired_velocity.j + input->acceleration.j * input->desired_velocity.i;
            ground_j = direction.j;
            direction.k = input->acceleration.k - (direction.i * input->ground_plane.i + direction.j * input->ground_plane.j) / input->ground_plane.k;
            ::normalize3d(&direction);
        }
        else
        {
            float angle = input->definition->ground.climb_inflection_angle;
            real_vector3d forward = input->aiming;
            real_vector3d left;
            character_physics_mode_ground_cross_product3d_ordered(global_up3d, &input->aiming, &left);
            if (::normalize3d(&left) != 0.0f)
            {
                rotate_vector_about_axis(&forward, &left, character_physics_mode_ground_sine(angle), (float)cos(angle));
            }
            point_from_line3d(&forward, &input->ground_plane, -dot_product3d(&forward, &input->ground_plane), &forward);
            point_from_line3d(&left, &input->ground_plane, -dot_product3d(&left, &input->ground_plane), &left);
            direction.i = input->acceleration.i * forward.i + input->acceleration.j * left.i;
            direction.j = input->acceleration.i * forward.j + input->acceleration.j * left.j;
            direction.k = input->acceleration.i * forward.k + input->acceleration.j * left.k + input->acceleration.k;
            if (!snap_to_ground)
                direction.k *= 5.0f;
            ::normalize3d(&direction);
        }
        if (!snap_to_ground)
        {
            if (direction.k <= input->definition->ground.downhill_k0)
            {
                magnitude *= input->definition->ground.downhill_velocity_scale;
            }
            else if (direction.k < input->definition->ground.downhill_k0)
            {
                magnitude *= 1.0f +
                    (input->definition->ground.downhill_velocity_scale - 1.0f) * (direction.k - input->definition->ground.downhill_k0) /
                        (input->definition->ground.downhill_k1 - input->definition->ground.downhill_k0);
            }
            else if (direction.k >= input->definition->ground.uphill_k1)
            {
                magnitude *= input->definition->ground.uphill_velocity_scale;
            }
            else if (direction.k > input->definition->ground.uphill_k0)
            {
                magnitude *= 1.0f +
                    (input->definition->ground.uphill_velocity_scale - 1.0f) * (direction.k - input->definition->ground.uphill_k0) /
                        (input->definition->ground.uphill_k1 - input->definition->ground.uphill_k0);
            }
        }
        magnitude *= input->scale;
        real_vector3d desired;
        real_vector3d delta;
        real_vector3d result;
        scale_vector3d(&direction, magnitude, &desired);
        subtract_vectors3d(&desired, vector, &delta);
        clipped_speed = character_physics_mode_ground_clamp_velocity_change(vector, &delta, &result, input->maximum_velocity, limit_upward,
            &started) != 0;
        if (!clipped_speed)
            apply_gravity = false;
        else if (snap_to_ground)
            apply_gravity = false;
        if (!input->ground_contact && !character_physics_mode_ground_adhesion_forces_disabled())
        {
            point_from_line3d(&result, &input->ground_plane, -character_physics_mode_ground_adhesion(input, vector), &result);
        }
        if (apply_friction)
        {
            result.i *= 1.0f - input->ground_friction;
            result.j *= 1.0f - input->ground_friction;
        }
        if (apply_gravity)
            result.k -= input->movement_scale * character_physics_mode_ground_game_tick_length();
        add_vectors3d(vector, &result, &output->velocity);
    }
    bool update_limit = !character_physics_definition_test_flag(input->definition, 2) && (input->flags & (1 << 2)) == 0;
    if (update_limit)
    {
        if (last_update_time != -1 && game_time_globals->time - 1 == last_update_time)
        {
            real_vector3d change;
            real_vector3d external;
            real_vector3d limited;
            subtract_vectors3d(&input->acceleration, &last_acceleration, &change);
            add_vectors3d(&change, &output->previous_external_velocity, &change);
            float maximum_change = magnitude3d(&change);
            subtract_vectors3d(&input->external_velocity, &output->previous_external_velocity, &external);
            subtract_vectors3d(&output->velocity, &external, &limited);
            limit3d(&limited, maximum_change + k_character_physics_mode_ground_acceleration_limit_padding);
            add_vectors3d(&external, &limited, &output->velocity);
        }
        last_acceleration = input->acceleration;
        last_update_time = game_time_globals->time;
    }
    if (apply_gravity)
        output->flags |= 1;
    else
        output->flags &= ~1;
    if (limit_upward)
        output->flags |= 16;
    else
        output->flags &= ~16;
    if (started)
        output->flags |= 32;
    else
        output->flags &= ~32;
}


namespace {
static __forceinline const c_havok_contact_point *character_physics_mode_ground_get_contact(
    const c_havok_component *component, const s_havok_contact_point_reference *reference)
{
    return (const c_havok_contact_point *)&component->m_rigid_bodies[reference->rigid_body_index].m_contact_points[reference->contact_point_index];
}
static __forceinline void character_physics_mode_ground_get_normal(const c_havok_contact_point *contact, real_vector3d *normal)
{
    *normal = contact->m_normal;
}
// @rdata 0x820328f4 k_character_physics_mode_ground_minimum_surface_normal_dot
static const float k_character_physics_mode_ground_minimum_surface_normal_dot = -0.002f;
// @rdata 0x8202490c character_physics_mode_ground_epsilon
static const float character_physics_mode_ground_epsilon = 0.0001f;
// @rdata 0x820221fc character_physics_mode_ground_one
static const float character_physics_mode_ground_one = 1.0f;
// @rdata 0x8202251c character_physics_mode_ground_zero
static const float character_physics_mode_ground_zero = 0.0f;
static __forceinline void character_physics_mode_ground_project_up(const real_vector3d *normal, float distance, real_vector3d *plane)
{
    plane->i = normal->i * distance;
    plane->j = normal->j * distance;
    plane->k = character_physics_mode_ground_one + distance * normal->k;
}
static __forceinline void character_physics_mode_ground_add_normals(const real_vector3d &first, const real_vector3d *second, real_vector3d *result)
{
    result->i = first.i + second->i;
    result->j = first.j + second->j;
    result->k = first.k + second->k;
}
static __forceinline float character_physics_mode_ground_project_normal(const real_vector3d &normal, const real_vector3d *velocity)
{
    return normal.i * velocity->i + normal.j * velocity->j + normal.k * velocity->k;
}
static __forceinline unsigned long character_physics_mode_ground_first_projection_is_lower(
    float fi, float fj, float fk, float ai, float aj, float ak, const real_vector3d *velocity)
{
    float vk = velocity->k;
    float vj = velocity->j;
    float vi = velocity->i;
    float first_dot = (fi * vi + fk * vk) + fj * vj;
    float average_dot = (ak * vk + aj * vj) + ai * vi;
    return first_dot < average_dot;
}
static __forceinline void character_physics_mode_ground_choose_normal(
    const real_vector3d *first, const real_vector3d *second, real_vector3d *average, const real_vector3d *velocity, real_vector3d *plane)
{
    character_physics_mode_ground_add_normals(*first, second, average);
    normalize3d(average);
    if (character_physics_mode_ground_first_projection_is_lower(first->i, first->j, first->k, average->i, average->j, average->k, velocity))
        *plane = *first;
    else
        *plane = *average;
}
static __forceinline real_vector3d *character_physics_mode_ground_contact_edge(
    const real_vector3d *a, const real_vector3d *b, float ai, float negative_k, float bj, real_vector3d *result)
{
    float k = ai * bj - negative_k;
    float j = a->k * b->i - ai * b->k;
    result->i = a->j * b->k - a->k * bj;
    result->j = j;
    result->k = k;
    return result;
}
}

// @address 0x10d248 ?build_composite_ground_plane@c_character_physics_mode_ground_datum@@AAAXPBUs_character_physics_move_input_datum@@PBUreal_matrix3x3@@_NPBTreal_vector3d@@PBUs_havok_contact_point_reference@@JPAT4@@Z
void c_character_physics_mode_ground_datum::build_composite_ground_plane(const s_character_physics_move_input_datum *input,
    const real_matrix3x3 *rotation, bool, const real_vector3d *velocity, const s_havok_contact_point_reference *references, long count,
    real_vector3d *plane)
{
    const c_havok_component *component = &(
        (const c_havok_component *)g_havok_component_data->data)[(unsigned short)input->havok_component_index];
    float lowest = character_physics_mode_ground_zero;
    long lowest_index = -1;
    long index = 0;
    if (index < count)
    {
        unsigned long ignore_bipeds = (input->definition->flags >> 7) & 1;
        do
        {
            const c_havok_contact_point *contact = character_physics_mode_ground_get_contact(component, &references[index]);
            if (ignore_bipeds == 0 || contact->m_other_object_index == -1 ||
                ((1 << character_physics_mode_ground_object_type(contact->m_other_object_index)) & 1) == 0)
            {
                real_vector3d normal;
                character_physics_mode_ground_get_normal(contact, &normal);
                if (rotation)
                    matrix3x3_inverse_transform_vector(rotation, &normal, &normal);
                if (lowest > normal.k && normal.k > -0.999f)
                {
                    lowest = normal.k;
                    lowest_index = index;
                }
            }
            ++index;
        } while (index < count);
    }
    *plane = global_identity4x3->up;
    switch (count)
    {
    case 1:
        character_physics_mode_ground_get_normal(character_physics_mode_ground_get_contact(component, references), plane);
        break;
    case 2:
        {
            real_vector3d first;
            real_vector3d second;
            real_vector3d temporary;
            character_physics_mode_ground_get_normal(character_physics_mode_ground_get_contact(component, references), &first);
            character_physics_mode_ground_get_normal(character_physics_mode_ground_get_contact(component, references + 1), &second);
            if (lowest_index != -1)
            {
                character_physics_mode_ground_get_normal(character_physics_mode_ground_get_contact(component, &references[lowest_index]), &temporary);
                real_vector3d edge;
                character_physics_mode_ground_contact_edge(&first, &second, first.i, first.j * second.i, second.j, &edge);
                if (lowest_index == 0)
                    character_physics_mode_ground_cross_product3d_ordered(&edge, &temporary, plane);
                else
                    character_physics_mode_ground_cross_product3d_ordered(&temporary, &edge, plane);
            }
            else
            {
                character_physics_mode_ground_choose_normal(&first, &second, &temporary, velocity, plane);
            }
        }
        break;
    case 3:
        if (lowest_index != -1)
        {
            real_vector3d normal;
            character_physics_mode_ground_get_normal(character_physics_mode_ground_get_contact(component, &references[lowest_index]), &normal);
            character_physics_mode_ground_project_up(&normal, -normal.k, plane);
        }
        else
            *plane = global_identity4x3->up;
        break;
    }
    normalize3d_with_default(plane, &global_identity4x3->up);
}

static __forceinline c_havok_component *character_physics_mode_ground_get_component(long index)
{
    return datum_get<c_havok_component>(g_havok_component_data, index);
}

static __forceinline bool character_physics_mode_ground_vehicle_is_stopped(void *state, long index)
{
    switch (vehicle_get_type(index)) {
    case _vehicle_type_human_tank:
        return ((c_vehicle_type_human_tank *)state)->get_stop_counter()->calculate_fraction(index) == 1.0f;
    case _vehicle_type_human_boat:
        return false;
    case _vehicle_type_alien_scout:
        return false;
    case _vehicle_type_alien_fighter:
        return false;
    case _vehicle_type_turret:
        return false;
    case _vehicle_type_mantis:
        return false;
    case _vehicle_type_vtol:
        return false;
    case _vehicle_type_guardian:
        return false;
    case _vehicle_type_human_jeep:
        return ((c_vehicle_type_human_jeep *)state)->stop_counter.calculate_fraction(index) == 1.0f;
    case _vehicle_type_chopper:
        return ((c_vehicle_type_chopper *)state)->stop_counter.calculate_fraction(index) == 1.0f;
    default:
        // match: 0xe7498: unreachable hint omits the cmplwi-9 vehicle-type range check and default epilog before the stopped-vehicle jump table.
        __assume(0);
    }
}
static __forceinline unsigned long character_physics_mode_ground_actor_controls_support(long index, const c_havok_component *component)
{
    if (index != -1 && ((1 << character_physics_mode_ground_object_type(index)) & 1) != 0) {
        s_object_header *own_header = character_physics_mode_ground_get_object_header(component->m_object_index);
        if (((1 << own_header->object_type) & 1) != 0 && ((unit_datum *)own_header->datum)->unit.actor_index != -1)
            return true;
    }
    return false;
}
static __forceinline void character_physics_mode_ground_copy_contact_normal(const c_havok_contact_point *contact, real_vector3d *normal)
{
    *normal = contact->m_normal;
}
static __forceinline float character_physics_mode_ground_contact_alignment(const real_vector3d *normal, const real_vector3d *up)
{
    return up->i * normal->i + up->j * normal->j + up->k * normal->k;
}


static __forceinline real_matrix3x3 *character_physics_mode_ground_output_rotation(s_character_physics_update_output_datum *output)
{
    return &output->localized_physics_space_rotation;
}
inline bool c_character_physics_mode_ground_datum::update_localized_physics_space_linear_velocity(
    const s_character_physics_update_input_datum *input, real_vector3d *linear_velocity, real_vector3d *linear_velocity_delta,
    real_matrix3x3 *rotation)
{
    bool result = false;
    long object_index = input->accepted_physics;
    if (object_index != -1 && object_early_mover_is_owner(object_index, false)) {
        object_datum *object = character_physics_mode_ground_get_object_header(object_index)->datum;
        result = calculate_localized_physics_space_linear_velocity(object->havok_component_index, -1, &input->current_position,
            &input->support_matrix, &m_ground_transform, &m_localized_origin, linear_velocity, linear_velocity_delta, rotation);
    }
    if (!result)
        m_localized_origin = *global_zero_vector3d;
    return result;
}
// @address 0xe7118 ?update@c_character_physics_mode_ground_datum@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_ground_datum::update(s_character_physics_update_output_datum *output,
    const s_character_physics_update_input_datum *input)
{
    real_vector3d external_velocity = input->external_velocity;
    m_ground_component_index = -1;
    m_ground_object_index = -1;
    m_ground_body_index = -1;
    real_matrix3x3 rotation = global_identity4x3->rotation;
    real_vector3d ground_velocity = *global_zero_vector3d;
    real_vector3d localized_velocity;
    real_vector3d localized_velocity_delta;
    bool localized = update_localized_physics_space_linear_velocity(input, &localized_velocity, &localized_velocity_delta, &rotation);
    bool grounded = !localized && update_ground_velocity(input, &ground_velocity, &rotation) ? true : false;
    if (localized) {
        ground_velocity = localized_velocity;
        output->flags |= 8;
        *character_physics_mode_ground_output_rotation(output) = rotation;
    }
    if (grounded || localized) {
        output->previous_external_velocity = ground_velocity;
        subtract_vectors3d(&external_velocity, &ground_velocity, &external_velocity);
        *character_physics_mode_ground_output_rotation(output) = rotation;
        if (localized && m_update_count > 1)
            add_vectors3d(&external_velocity, &localized_velocity_delta, &external_velocity);
    }
    update_internal(output, input, localized, &external_velocity);
    if (grounded || localized) {
        rotation.forward.k = 0.0f;
        rotation.up = *global_up3d;
        normalize3d_with_default(&rotation.forward, global_forward3d);
        character_physics_mode_ground_cross_product3d_ordered(&rotation.basis[2], &rotation.basis[0], &rotation.basis[1]);
        add_vectors3d(&output->velocity, &ground_velocity, &output->velocity);
        matrix3x3_transform_vector(&rotation, &output->facing, &output->facing);
        matrix3x3_transform_vector(&rotation, &output->up, &output->up);
    }
    m_update_count = (unsigned char)MIN(m_update_count + 1, 254);
}

// @address 0xe7498 ?update_ground_velocity@c_character_physics_mode_ground_datum@@AAA_NPBUs_character_physics_update_input_datum@@PATreal_vector3d@@PAUreal_matrix3x3@@@Z
bool c_character_physics_mode_ground_datum::update_ground_velocity(
    const s_character_physics_update_input_datum *input, real_vector3d *linear_velocity, real_matrix3x3 *rotation)
{
    c_havok_component *component = character_physics_mode_ground_get_component(input->havok_component_index);
    float best_alignment = -3.402823466e38f;
    s_havok_contact_point_reference best_reference = k_invalid_contact_point_reference;
    float height = input->definition->radius + input->current_position.z;
    c_havok_contact_point_iterator iterator(component->m_object_index, false);
    while (iterator.valid()) {
        c_havok_contact_point *contact = iterator.get_contact_point();
        if ((contact->m_position.z >= height - 0.001f) == false) {
            real_vector3d normal;
            character_physics_mode_ground_copy_contact_normal(contact, &normal);
            float alignment = character_physics_mode_ground_contact_alignment(&normal, &input->ground_plane);
            if ((alignment <= 0.8660254f) == false && (alignment <= best_alignment) == false) {
                best_alignment = alignment;
                best_reference = *iterator.get_reference();
            }
        }
        iterator.next();
    }
    bool result = false;
    if (best_reference.rigid_body_index != -1) {
        c_havok_contact_point *contact = component->m_rigid_bodies[best_reference.rigid_body_index].get_contact_point(best_reference.contact_point_index);
        signed char body_index_byte = contact->m_other_rigid_body_index;
        long body_index = body_index_byte;
        long contact_component_index = contact->m_other_havok_component_index;
        if (contact_component_index != -1) {
            real_matrix4x3 transform;
            c_havok_component *contact_component = character_physics_mode_ground_get_component(contact_component_index);
            contact_component->get_rigid_body_transform(body_index_byte, &transform);
            if (contact_component_index == input->support_component && body_index == input->support_body) {
                long object_index = contact->m_other_object_index;
                if ((object_index == -1 || ((1 << character_physics_mode_ground_contact_object_type(object_index)) & 2) == 0 ||
                    !character_physics_mode_ground_vehicle_is_stopped(
                        ((vehicle_datum *)character_physics_mode_ground_get_object_header(object_index)->datum)->vehicle.type_state,
                        object_index)) &&
                    !character_physics_mode_ground_actor_controls_support(contact->m_other_object_index, component)) {
                    real_matrix4x3 inverse_transform;
                    real_matrix4x3 relative_transform;
                    matrix4x3_inverse(&transform, &inverse_transform);
                    matrix4x3_compute_product(&input->support_matrix, &inverse_transform, &relative_transform);
                    contact_component->get_rigid_body_linear_velocity_at_point(contact->m_other_rigid_body_index, linear_velocity,
                        &input->current_position);
                    matrix4x3_inverse(&relative_transform, &relative_transform);
                    *rotation = relative_transform.rotation;
                    result = true;
                }
            }
            memcpy(&m_ground_transform, &transform, sizeof(transform));
            m_ground_component_index = contact_component_index;
            m_ground_object_index = contact->m_other_object_index;
            m_ground_body_index = contact->m_other_rigid_body_index;
        }
    }
    return result;
}

static __forceinline float character_physics_mode_ground_dot_product3d_k_ij(const real_vector3d *a, const real_vector3d *b)
{
    return a->k * b->k + (a->i * b->i + a->j * b->j);
}
static __forceinline float character_physics_mode_ground_dot_product3d_i_jk(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}
static __forceinline float character_physics_mode_ground_dot_product3d_current(const real_vector3d *a, const real_vector3d *b)
{
    return b->i * a->i + a->j * b->j + a->k * b->k;
}
static __forceinline float character_physics_mode_ground_normalize3d(real_vector3d *vector)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    float length = magnitude3d(vector);
    if ((float)fabs(length) >= character_physics_mode_ground_epsilon)
    {
        scale_vector3d(vector, character_physics_mode_ground_one / length, vector);
        return length;
    }
    return character_physics_mode_ground_zero;
}
static __forceinline float character_physics_mode_ground_normalize3d_plane(real_vector3d *vector)
{
    float length = (float)sqrt(character_physics_mode_ground_dot_product3d_k_ij(vector, vector));
    if ((float)fabs(length) >= character_physics_mode_ground_epsilon)
    {
        float inverse = character_physics_mode_ground_one / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
        return length;
    }
    return character_physics_mode_ground_zero;
}

// @rdata 0x8202af7c k_character_physics_mode_ground_airborne_k_scale
static const float k_character_physics_mode_ground_airborne_k_scale = 30.0f;
// @rdata 0x82021694 k_character_physics_mode_ground_real_max
static const float k_character_physics_mode_ground_real_max = 3.402823466e38f;

static bool character_physics_mode_ground_verify_contact_point(
    const real_point3d *position,
    float height,
    bool localized_physics_space,
    const c_character_physics_mode_ground_datum *ground_datum,
    bool airborne,
    const c_havok_contact_point *contact_point)
{
    return true;
}

// @address 0x10c518 ?calculate_ground_contact@c_character_physics_mode_ground_datum@@AAA_NPBUs_character_physics_move_input_datum@@_NPBTreal_vector3d@@PAT3@PAUs_havok_contact_point_reference@@PAMPAJ@Z
bool c_character_physics_mode_ground_datum::calculate_ground_contact(
    const s_character_physics_move_input_datum *physics_input,
    bool localized_physics_space,
    const real_vector3d *linear_velocity_input,
    real_vector3d *ground_plane,
    // match: 0x10c518: pointer __restrict qualifier prevents alias reload
    s_havok_contact_point_reference * __restrict contact_point_reference,
    float *landing_velocity,
    long *ground_object_index)
{
    c_havok_component *havok_component = character_physics_mode_ground_get_component(physics_input->havok_component_index);
    bool force_ground = (physics_input->flags & (1 << 5)) != 0;
    bool snap_to_ground = (physics_input->flags & (1 << 6)) != 0;
    bool airborne = (physics_input->flags & (1 << 1)) != 0;
    s_havok_contact_point_reference plane_references[3];
    long plane_count = 0;
    real_vector3d linear_velocity = *linear_velocity_input;
    float minimum_ground_plane_k = physics_input->definition->ground.minimum_normal_k;
    real_matrix3x3 ground_plane_matrix;
    if (snap_to_ground)
    {
        ground_plane_matrix.up = m_ground_plane;
        generate_up_vector3d(&ground_plane_matrix.up, &ground_plane_matrix.forward);
        cross_product3d(&ground_plane_matrix.up, &ground_plane_matrix.forward, &ground_plane_matrix.left);
    }
    *landing_velocity = character_physics_mode_ground_zero;
    *ground_plane = *global_up3d;
    *contact_point_reference = k_invalid_contact_point_reference;
    *ground_object_index = -1;
    if (localized_physics_space || physics_input->accepted_physics != -1)
    {
        if (!localized_physics_space)
        {
            if ((physics_input->flags & (1 << 7)) != 0)
            {
                subtract_vectors3d(&linear_velocity, &physics_input->early_velocity, &linear_velocity);
            }
        }
        else
        {
            subtract_vectors3d(&linear_velocity, &m_localized_origin, &linear_velocity);
        }
    }
    real_vector3d original_desired_ground_plane = linear_velocity;
    if (!snap_to_ground)
    {
        c_havok_contact_point_iterator contact_point_iterator(havok_component->m_object_index, false);
        while (contact_point_iterator.valid())
        {
            const c_havok_contact_point *contact_point = contact_point_iterator.get_contact_point();
            if ((contact_point->m_flags & (1 << 6)) != 0 || (m_contact_flags & (1 << 1)) != 0)
            {
                real_vector3d surface_normal;
                contact_point->get_surface_normal(&surface_normal);
                if (surface_normal.k < global_slip_surface_maximum_k_get())
                {
                    minimum_ground_plane_k = MAX(minimum_ground_plane_k, surface_normal.k + 0.001f);
                }
            }
            contact_point_iterator.next();
        }
    }
    if (physics_input->ground_havok_component_index != -1)
    {
        c_havok_contact_point_iterator contact_point_iterator(havok_component->m_object_index, false);
        while (contact_point_iterator.valid())
        {
            const c_havok_contact_point *contact_point = contact_point_iterator.get_contact_point();
            if (!character_physics_definition_test_flag(physics_input->definition, 7) ||
                contact_point->m_other_object_index == -1 ||
                ((1 << character_physics_mode_ground_object_type(contact_point->m_other_object_index)) & 1) == 0)
            {
                bool contact_matches_ground =
                    (physics_input->ground_havok_component_index == contact_point->m_other_havok_component_index &&
                    physics_input->ground_rigid_body_index == contact_point->m_other_rigid_body_index) ? true : false;
                real_vector3d contact_normal;
                character_physics_mode_ground_copy_contact_normal(contact_point, &contact_normal);
                if (snap_to_ground)
                {
                    matrix3x3_inverse_transform_vector(&ground_plane_matrix, &contact_normal, &contact_normal);
                }
                if (contact_matches_ground && ((force_ground && !snap_to_ground) || contact_normal.k > minimum_ground_plane_k))
                {
                    if (character_physics_mode_ground_verify_contact_point(
                        &physics_input->position,
                        physics_input->definition->radius,
                        localized_physics_space,
                        this,
                        (physics_input->flags & (1 << 1)) != 0,
                        contact_point))
                    {
                        scale_vector3d(&contact_normal, -1.0f, &original_desired_ground_plane);
                        break;
                    }
                }
            }
            contact_point_iterator.next();
        }
    }
    if (force_ground)
    {
        if ((physics_input->flags & (1 << 1)) != 0)
        {
            subtract_vectors3d(&original_desired_ground_plane, &m_ground_plane, &original_desired_ground_plane);
        }
    }
    else
    {
        if ((physics_input->flags & (1 << 1)) == 0)
        {
            if (physics_input->accepted_physics != 0)
            {
                original_desired_ground_plane = *global_down3d;
            }
            else if (original_desired_ground_plane.k < character_physics_mode_ground_zero)
            {
                original_desired_ground_plane.k *= k_character_physics_mode_ground_airborne_k_scale;
            }
        }
        if ((physics_input->flags & (1 << 1)) != 0)
        {
            subtract_vectors3d(&original_desired_ground_plane, &m_ground_plane, &original_desired_ground_plane);
        }
        else if (magnitude_squared3d(&physics_input->up) == character_physics_mode_ground_zero)
        {
            original_desired_ground_plane = *global_down3d;
        }
    }
    if (character_physics_mode_ground_normalize3d_plane(&original_desired_ground_plane) != character_physics_mode_ground_zero)
    {
        real_vector3d current_desired_ground_plane = original_desired_ground_plane;
        for (long iteration = 0; iteration < 3; ++iteration)
        {
            float best_dot_product = k_character_physics_mode_ground_real_max;
            s_havok_contact_point_reference best_contact_point_reference = k_invalid_contact_point_reference;
            c_havok_contact_point_iterator contact_point_iterator(havok_component->m_object_index, false);
            while (contact_point_iterator.valid())
            {
                const c_havok_contact_point *contact_point = contact_point_iterator.get_contact_point();
                if (character_physics_mode_ground_verify_contact_point(
                    &physics_input->position,
                    physics_input->definition->radius,
                    localized_physics_space,
                    this,
                    (physics_input->flags & (1 << 1)) != 0,
                    contact_point))
                {
                    if (!character_physics_definition_test_flag(physics_input->definition, 7) ||
                        contact_point->m_other_object_index == -1 ||
                        ((1 << character_physics_mode_ground_object_type(contact_point->m_other_object_index)) & 1) == 0)
                    {
                        if ((physics_input->flags & (1 << 1)) == 0 ||
                            physics_input->ground_havok_component_index == -1 ||
                            physics_input->ground_havok_component_index == contact_point->m_other_havok_component_index)
                        {
                            real_vector3d contact_normal;
                            character_physics_mode_ground_copy_contact_normal(contact_point, &contact_normal);
                            float current_dot_product = character_physics_mode_ground_dot_product3d_current(&current_desired_ground_plane,
                                &contact_normal);
                            float original_dot_product = dot_product3d(&original_desired_ground_plane, &contact_normal);
                            if (snap_to_ground)
                            {
                                matrix3x3_inverse_transform_vector(&ground_plane_matrix, &contact_normal, &contact_normal);
                            }
                            if ((airborne || !snap_to_ground || contact_normal.k > minimum_ground_plane_k) &&
                                current_dot_product < -0.025f &&
                                current_dot_product < best_dot_product &&
                                original_dot_product < -0.025f)
                            {
                                best_dot_product = current_dot_product;
                                best_contact_point_reference = *contact_point_iterator.get_reference();
                            }
                        }
                    }
                }
                contact_point_iterator.next();
            }
            if (best_contact_point_reference.rigid_body_index == -1 && plane_count == 0 && force_ground)
            {
                c_havok_contact_point_iterator fallback_iterator(havok_component->m_object_index, false);
                while (fallback_iterator.valid())
                {
                    const c_havok_contact_point *contact_point = fallback_iterator.get_contact_point();
                    if (!character_physics_definition_test_flag(physics_input->definition, 7) ||
                        contact_point->m_other_object_index == -1 ||
                        ((1 << character_physics_mode_ground_object_type(contact_point->m_other_object_index)) & 1) == 0)
                    {
                        real_vector3d contact_normal;
                        character_physics_mode_ground_copy_contact_normal(contact_point, &contact_normal);
                        best_dot_product = -1.0f;
                        best_contact_point_reference = *fallback_iterator.get_reference();
                        scale_vector3d(&contact_normal, -1.0f, &original_desired_ground_plane);
                        current_desired_ground_plane = original_desired_ground_plane;
                        break;
                    }
                    fallback_iterator.next();
                }
            }
            if (best_contact_point_reference.rigid_body_index == -1)
            {
                break;
            }
                c_havok_contact_point *contact_point =
                havok_component->m_rigid_bodies[best_contact_point_reference.rigid_body_index].get_contact_point(
                    best_contact_point_reference.contact_point_index);
            *contact_point_reference = best_contact_point_reference;
            plane_references[plane_count] = best_contact_point_reference;
            ++plane_count;
            switch (plane_count)
            {
            case 1:
                {
                    real_vector3d plane_normal;
                    real_vector3d normal;
                    character_physics_mode_ground_copy_contact_normal(contact_point, &normal);
                    scale_vector3d(&normal, best_dot_product, &plane_normal);
                    subtract_vectors3d(&original_desired_ground_plane, &plane_normal, &current_desired_ground_plane);
                }
                break;
            case 2:
                {
                    real_vector3d plane0;
                    real_vector3d plane1;
                    real_vector3d clip_line_vector;
                    character_physics_mode_ground_copy_contact_normal(
                        havok_component->m_rigid_bodies[plane_references[0].rigid_body_index].get_contact_point(plane_references[0].contact_point_index),
                        &plane0);
                    character_physics_mode_ground_copy_contact_normal(
                        havok_component->m_rigid_bodies[plane_references[1].rigid_body_index].get_contact_point(plane_references[1].contact_point_index),
                        &plane1);
                    cross_product3d(&plane0, &plane1, &clip_line_vector);
                    character_physics_mode_ground_normalize3d(&clip_line_vector);
                    float scale = character_physics_mode_ground_dot_product3d_i_jk(&original_desired_ground_plane, &clip_line_vector);
                    scale_vector3d(&clip_line_vector, scale, &current_desired_ground_plane);
                }
                break;
            case 3:
                break;
            }
        }
        build_composite_ground_plane(
            physics_input,
            snap_to_ground ? &ground_plane_matrix : NULL,
            localized_physics_space,
            &original_desired_ground_plane,
            plane_references,
            plane_count,
            ground_plane);
    }
    bool airborne_for_landing = (physics_input->flags & (1 << 1)) != 0;
    float head_z = physics_input->position.z + physics_input->definition->radius;
    c_havok_contact_point_iterator contact_point_iterator(havok_component->m_object_index, false);
    while (contact_point_iterator.valid())
    {
        const c_havok_contact_point *contact_point = contact_point_iterator.get_contact_point();
        if (character_physics_mode_ground_verify_contact_point(
            &physics_input->position,
            physics_input->definition->radius,
            localized_physics_space,
            this,
            (physics_input->flags & (1 << 1)) != 0,
            contact_point))
        {
            real_vector3d contact_normal;
            character_physics_mode_ground_copy_contact_normal(contact_point, &contact_normal);
            float normal_velocity = -character_physics_mode_ground_dot_product3d_i_jk(&linear_velocity, &contact_normal);
            if (airborne_for_landing &&
                contact_point->m_position.z < head_z - 0.001f &&
                normal_velocity > *landing_velocity)
            {
                *landing_velocity = normal_velocity;
            }
        }
        if (contact_point->m_other_object_index != -1 &&
            (*ground_object_index == -1 || ((1 << character_physics_mode_ground_object_type(contact_point->m_other_object_index)) & 1) != 0))
        {
            *ground_object_index = contact_point->m_other_object_index;
        }
        contact_point_iterator.next();
    }
    return plane_count != 0 && (force_ground || ground_plane->k > minimum_ground_plane_k);
}


static __forceinline bool havok_contact_point_reference_valid(const s_havok_contact_point_reference *reference)
{
    return reference->rigid_body_index != -1;
}

// @address 0x10baa8 ?move@c_character_physics_mode_ground_datum@@QAAXPAUs_character_physics_move_output_datum@@PBUs_character_physics_move_input_datum@@@Z
void c_character_physics_mode_ground_datum::move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input)
{
    c_havok_component *havok_component = character_physics_mode_ground_get_component(input->havok_component_index);
    bool disable_ground =
        (input->flags & (1 << 2)) != 0 ||
        (input->flags_test(0) && (input->flags & (1 << 1)) == 0) ||
        (input->flags & (1 << 3)) != 0 ||
        (input->flags & (1 << 4)) != 0 ? true : false;
    bool localized_physics_space = magnitude3d(&m_localized_origin) != character_physics_mode_ground_zero ? true : false;
    bool touching_physics_only_surface = false;
    output->flags &= ~(1 << 0);
    if (!disable_ground)
    {
        const real_vector3d *linear_velocity = havok_component->get_rigid_body_pre_simulation_linear_velocity(0);
        bool force_ground = input->flags_test(5);
        bool stick_to_ground = force_ground && (!input->flags_test(6) || input->flags_test(1)) ? true : false;
        bool ignore_climbable_surfaces = input->flags_test(8);
        s_havok_contact_point_reference best_contact_point_reference = k_invalid_contact_point_reference;
        bool best_is_ground = false;
        bool best_has_object = false;
        float best_velocity_dot = -k_character_physics_mode_ground_real_max;
        float best_linear_velocity_dot = -k_character_physics_mode_ground_real_max;
        float best_distance = k_character_physics_mode_ground_real_max;
        s_havok_contact_point_reference fastest_contact_point_reference = k_invalid_contact_point_reference;
        float fastest_velocity_dot = -k_character_physics_mode_ground_real_max;
        initialize_internal(false);
        c_havok_contact_point_iterator contact_point_iterator(havok_component->get_object_index(), false);
        while (contact_point_iterator.valid())
        {
            const c_havok_contact_point *contact_point = contact_point_iterator.get_contact_point();
            float distance = contact_point->m_distance;
            bool physics_only = contact_point->is_physics_only_surface();
            bool climbable = contact_point->is_climbable_surface() && !ignore_climbable_surfaces;
            bool has_object = contact_point->m_other_object_index != -1;
            real_vector3d contact_normal = contact_point->m_normal;
            real_vector3d surface_normal;
            (void)&surface_normal.j;
            contact_point->get_surface_normal(&surface_normal);
            float velocity_dot = dot_product3d(&input->velocity, &contact_normal);
            float linear_velocity_dot = -dot_product3d(linear_velocity, &contact_normal);
            float forward_dot = -dot_product3d(&input->forward, &contact_normal) * input->up.i;
            bool ground = ((stick_to_ground && input->ground_enabled) || climbable) && !(forward_dot < -0.001f && !stick_to_ground);
            if (velocity_dot > fastest_velocity_dot)
            {
                fastest_contact_point_reference = *contact_point_iterator.get_reference();
                fastest_velocity_dot = velocity_dot;
            }
            if (dot_product3d(&contact_normal, &surface_normal) > k_character_physics_mode_ground_minimum_surface_normal_dot &&
                (contact_point->m_other_havok_component_index != -1 || linear_velocity_dot > character_physics_mode_ground_zero))
            {
                bool select = false;
                if (best_has_object && !has_object)
                    select = true;
                else if (ground && !force_ground && linear_velocity->i * contact_normal.i + linear_velocity->j * contact_normal.j > 0.5f)
                    select = false;
                else if (ground && !best_is_ground)
                    select = true;
                else if (ground)
                    select = linear_velocity_dot > best_linear_velocity_dot;
                else if (!best_is_ground)
                {
                    if (contact_normal.k >= input->definition->ground.minimum_normal_k)
                        select = distance < best_distance;
                    else
                        select = velocity_dot >= best_velocity_dot + character_physics_mode_ground_epsilon ||
                            (velocity_dot > best_velocity_dot - character_physics_mode_ground_epsilon && distance < best_distance);
                }
                touching_physics_only_surface |= physics_only;
                if (select)
                {
                    best_contact_point_reference = *contact_point_iterator.get_reference();
                    best_is_ground = ground;
                    best_velocity_dot = MAX(best_velocity_dot, velocity_dot);
                    best_linear_velocity_dot = linear_velocity_dot;
                    best_has_object = has_object;
                    best_distance = distance;
                }
            }
            contact_point_iterator.next();
        }
        if (havok_contact_point_reference_valid(&best_contact_point_reference) && best_is_ground)
        {
            real_vector3d ground_normal;
            character_physics_mode_ground_copy_contact_normal(havok_component->get_contact_point(&best_contact_point_reference), &ground_normal);
            normalize3d_with_default(&ground_normal, global_up3d);
            set_contact(input->havok_component_index, &best_contact_point_reference, &ground_normal);
            output->set_flag(1, true);
        }
        else
        {
            real_vector3d ground_plane;
            s_havok_contact_point_reference contact_point_reference;
            if (calculate_ground_contact(input, localized_physics_space, linear_velocity, &ground_plane, &contact_point_reference,
                    &m_landing_velocity, &output->material))
            {
                set_contact(input->havok_component_index, &fastest_contact_point_reference, &ground_plane);
                if (havok_contact_point_reference_valid(&best_contact_point_reference))
                {
                    output->set_flag(1, true);
                    if (!best_is_ground)
                        output->contact = best_contact_point_reference;
                }
            }
            else
            {
                output->set_flag(0, true);
            }
        }
    }
    else
    {
        bool reset_ground = (input->flags & (1 << 4)) != 0 || (input->flags & (1 << 3)) != 0;
        if ((input->flags & (1 << 1)) != 0 || reset_ground)
            initialize_internal(false);
        if (reset_ground)
            output->set_flag(0, true);
    }
    if (!havok_component_has_active_entity(havok_component))
        output->set_flag(0, false);
    if (input->local_physics != -1 && input->accepted_physics == -1)
    {
        m_localized_origin = *global_zero_vector3d;
        if (!output->flags_test(0) && havok_contact_point_reference_valid(&output->contact))
        {
            if (havok_component->get_contact_point(&output->contact)->m_other_object_index == input->local_physics)
            {
                output->object_index = input->local_physics;
                m_localized_origin = *global_zero_vector3d;
                m_update_count = 0;
            }
        }
    }
    if (touching_physics_only_surface)
        output->contact = k_invalid_contact_point_reference;
    m_contact_flags &= ~(1 << 1);
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char contact_iterator_object_offset[(offsetof(c_havok_contact_point_iterator, object_index) == 4) ? 1 : -1];
typedef char contact_iterator_size[(sizeof(c_havok_contact_point_iterator) == 12) ? 1 : -1];
#endif
