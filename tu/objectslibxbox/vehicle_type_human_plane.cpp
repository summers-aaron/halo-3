// objectslibxbox/vehicle_type_human_plane.obj: from the records 0xeeb00 0xf1cc8 0xf3368 0x35a450
// @flags /TP /O2 /Oc
#include "objects/objects.h"
#include "physics/havok_group_types.h"
#include "tag_files/tag_index.h"
#include "units/units.h"
#include "effects/effects.h"
#include "math/random_math.h"
#include "units/vehicle_type_human_plane.h"
#include "math/matrix_math.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/physics_variables.h"
#include "units/vehicle_datum.h"
#include "effects/effect_vector_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "units/unit_types.h"
#include <math.h>
#include "physics/physics_variable_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_types.h"
#include "units/vehicle_definitions.h"
#include "physics/havok_utilities.h"
#include "effects/effect_creation_data.h"
#include "physics/collision_test_vector.h"
#include "effects/decals_globals.h"
#include "tag_files/string_ids.h"

// @address 0xeeb00 ?compute_function_value@c_vehicle_type_human_plane@@QAA_NJJPAMPA_N@Z
bool c_vehicle_type_human_plane::compute_function_value(long vehicle_index, long function, float *value, bool *)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_human_plane_definition *definition = (const s_vehicle_human_plane_definition *)
        TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index)->human_plane.address;
    bool result = true;
    switch (function) {
    case _string_id_hover:
        if (definition->vertical_speed.positive_limit > 0.0001f)
            *value = m_vertical_speed / definition->vertical_speed.positive_limit;
        break;
    case _string_id_turn_absolute: {
        real_vector3d velocities;
        object_get_localized_velocities(vehicle_index, 0, &velocities, 0, 0);
        float xx = velocities.i * velocities.i;
        float yy = velocities.j * velocities.j;
        *value = sqrtf(xx + yy + velocities.k * velocities.k) * 0.63661975f;
        break;
    }
    case _string_id_thrust:
        if (definition->forward_speed.positive_limit > 0.0001f)
            *value = m_forward_speed / definition->forward_speed.positive_limit;
        break;
    case _string_id_engine_rpm: {
        float max_high;
        if (fabsf(definition->vertical_speed.positive_limit) > fabsf(definition->vertical_speed.negative_limit))
            max_high = fabsf(definition->vertical_speed.positive_limit);
        else
            max_high = fabsf(definition->vertical_speed.negative_limit);
        float max_mid;
        if (fabsf(definition->left_speed.positive_limit) > fabsf(definition->left_speed.negative_limit))
            max_mid = fabsf(definition->left_speed.positive_limit);
        else
            max_mid = fabsf(definition->left_speed.negative_limit);
        float max_low;
        if (fabsf(definition->forward_speed.positive_limit) > fabsf(definition->forward_speed.negative_limit))
            max_low = fabsf(definition->forward_speed.positive_limit);
        else
            max_low = fabsf(definition->forward_speed.negative_limit);
        float max_paired;
        if (max_mid > max_low) {
            if (fabsf(definition->left_speed.positive_limit) > fabsf(definition->left_speed.negative_limit))
                max_paired = fabsf(definition->left_speed.positive_limit);
            else
                max_paired = fabsf(definition->left_speed.negative_limit);
        } else {
            if (fabsf(definition->forward_speed.positive_limit) > fabsf(definition->forward_speed.negative_limit))
                max_paired = fabsf(definition->forward_speed.positive_limit);
            else
                max_paired = fabsf(definition->forward_speed.negative_limit);
        }
        float max_limit;
        if (max_high > max_paired) {
            if (fabsf(definition->vertical_speed.positive_limit) > fabsf(definition->vertical_speed.negative_limit))
                max_limit = fabsf(definition->vertical_speed.positive_limit);
            else
                max_limit = fabsf(definition->vertical_speed.negative_limit);
        } else {
            float max_first;
            if (fabsf(definition->left_speed.positive_limit) > fabsf(definition->left_speed.negative_limit))
                max_first = fabsf(definition->left_speed.positive_limit);
            else
                max_first = fabsf(definition->left_speed.negative_limit);
            float max_second;
            if (fabsf(definition->forward_speed.positive_limit) > fabsf(definition->forward_speed.negative_limit))
                max_second = fabsf(definition->forward_speed.positive_limit);
            else
                max_second = fabsf(definition->forward_speed.negative_limit);
            if (max_first > max_second) {
                if (fabsf(definition->left_speed.positive_limit) > fabsf(definition->left_speed.negative_limit))
                    max_limit = fabsf(definition->left_speed.positive_limit);
                else
                    max_limit = fabsf(definition->left_speed.negative_limit);
            } else {
                if (fabsf(definition->forward_speed.positive_limit) > fabsf(definition->forward_speed.negative_limit))
                    max_limit = fabsf(definition->forward_speed.positive_limit);
                else
                    max_limit = fabsf(definition->forward_speed.negative_limit);
            }
        }
        if (max_limit > 0.001f) {
            float max_ab;
            if (fabsf(m_left_speed) > fabsf(m_vertical_speed))
                max_ab = fabsf(m_left_speed);
            else
                max_ab = fabsf(m_vertical_speed);
            float max_abc;
            if (fabsf(m_forward_speed) > max_ab) {
                max_abc = fabsf(m_forward_speed);
            } else {
                if (fabsf(m_left_speed) > fabsf(m_vertical_speed))
                    max_abc = fabsf(m_left_speed);
                else
                    max_abc = fabsf(m_vertical_speed);
            }
            *value = max_abc / max_limit;
            float clamped = PIN(*value, 0.0f, 1.0f);
            *value = clamped;
        }
        break;
    }
    default:
        result = false;
        break;
    }
    float clamped = PIN(*value, 0.0f, 1.0f);
    *value = clamped;
    return result;
}
extern "C" {
// @address 0x11db300 g_human_plane_drag_scale
float g_human_plane_drag_scale;
// @address 0x11db304 g_human_plane_init_flags
unsigned long g_human_plane_init_flags;
}

#define vehicle_type_human_plane_abs(value) (((value) >= 0.0f) ? (value) : -(value))
static __forceinline bool vehicle_type_human_plane_boosting(const vehicle_datum *vehicle)
{
    return TEST_BIT(vehicle->vehicle.flags, 4);
}
static __forceinline float vehicle_type_human_plane_speed_from_fraction(const physics_variable_speed *speed, float fraction)
{
    return (fraction < 0.0f ? speed->negative_limit : speed->positive_limit) * fraction;
}
static __forceinline void vehicle_type_human_plane_yaw_vectors(real_vector3d *up,
    const real_vector3d *forward, float sine, float cosine)
{
    real_vector3d left;
    left.i = (forward->j * up->k - forward->k * up->j) * sine;
    left.j = (forward->k * up->i - forward->i * up->k) * sine;
    left.k = (forward->i * up->j - forward->j * up->i) * sine;
    up->i = cosine * up->i + left.i;
    up->j = cosine * up->j + left.j;
    up->k = cosine * up->k + left.k;
}

// @address 0xf1cc8
void c_vehicle_type_human_plane::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    float delta_time = tls_game_time_globals()->tick_length;
    vehicle_datum *vehicle =
        (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    const vehicle_definition *tag_definition =
        TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index);
    const s_vehicle_human_plane_definition *definition = (const s_vehicle_human_plane_definition *)
        TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index)->human_plane.address;
    float velocity_scale = definition->velocity_scale;
    float boost_scale = vehicle_type_human_plane_boosting(vehicle)
        ? 1.0f + unit_compute_boost_fraction(vehicle_index) * (tag_definition->boost_peak_power - 1.0f)
        : 1.0f;
    float maximum_speed = definition->forward_speed.positive_limit * boost_scale;
    const real_vector3d *forward = &vehicle->unit.facing_vector;
    real_vector3d linear_velocity, angular_velocity;
    object_get_velocities(vehicle_index, &linear_velocity, &angular_velocity);
    if (vehicle->unit.m_unit_flags.test(_unit_flag_crouching))
        vehicle->unit.throttle.k = -1.0f;
    // match: 0xf1cc8: zero-bit rotate preserves 32-bit GPR value
    else if (TEST_BIT(((((_rotl(_rotl(vehicle->unit.control_flags, 0), 0))))), 1))
        vehicle->unit.throttle.k = 1.0f;
    const physics_variable_speed *vertical_speed = &definition->vertical_speed;
    physics_variable_speed_update_seek(&m_vertical_speed, vertical_speed,
        vehicle_type_human_plane_speed_from_fraction(vertical_speed, vehicle->unit.throttle.k), 1.0f, delta_time);
    real_vector3d acceleration_scale;
    acceleration_scale.i = PIN(m_forward_speed * boost_scale, -maximum_speed, maximum_speed) / maximum_speed;
    acceleration_scale.j = PIN(m_left_speed, -maximum_speed, maximum_speed) / maximum_speed;
    acceleration_scale.k = PIN(m_vertical_speed, -maximum_speed, maximum_speed) / maximum_speed;
    multiply_vectors3d(&acceleration_scale, &acceleration_scale, &acceleration_scale);
    scale_vector3d(&acceleration_scale, vehicle->unit.seat_acceleration_fractions[0], &acceleration_scale);
    real_vector3d up;
    set_real_vector3d(&up, -forward->i * forward->k, -forward->j * forward->k,
        1.0f - forward->k * forward->k);
    if (normalize3d(&up) == 0.0f)
    {
        set_real_vector3d(&up, 1.0f, 0.0f, 0.0f);
    }
    real_vector3d left, object_left;
    cross_product3d(&up, forward, &left);
    cross_product3d(&vehicle->object.up, &vehicle->object.forward, &object_left);
    float forward_velocity = dot_product3d(&linear_velocity, forward);
    float vertical_velocity = dot_product3d(&linear_velocity, &up);
    float left_velocity = dot_product3d(&linear_velocity, &left);
    float forward_force = acceleration_scale.i * velocity_scale * instance->mass * (m_forward_speed * boost_scale - forward_velocity);
    float left_force = acceleration_scale.j * velocity_scale * instance->mass * (m_left_speed - left_velocity);
    float vertical_force = acceleration_scale.k * velocity_scale * instance->mass * (m_vertical_speed - vertical_velocity);
    real_vector3d force;
    force.i = (forward_force * forward->i + vertical_force * up.i) + left_force * left.i;
    force.j = (forward_force * forward->j + vertical_force * up.j) + left_force * left.j;
    force.k = (forward_force * forward->k + vertical_force * up.k) + left_force * left.k;
    if (!TEST_BIT(g_human_plane_init_flags, 0))
    {
        g_human_plane_init_flags |= 1;
        g_human_plane_drag_scale = (float)tls_game_time_globals()->ticks_per_second * 0.1f;
    }
    if (m_forward_speed == 0.0f)
    {
        float sign = forward_velocity > 0.0f ? 1.0f : -1.0f;
        float speed = vehicle_type_human_plane_abs(forward_velocity);
        float drag = -sign * definition->drag * velocity_scale * vehicle_type_human_plane_abs(definition->forward_speed.toward_rate);
        drag = PIN(drag, -speed * g_human_plane_drag_scale, speed * g_human_plane_drag_scale);
        real_vector3d drag_force;
        scale_vector3d(forward, drag * instance->mass, &drag_force);
        add_vectors3d(&force, &drag_force, &force);
    }
    if (m_left_speed == 0.0f)
    {
        float sign = left_velocity > 0.0f ? 1.0f : -1.0f;
        float speed = vehicle_type_human_plane_abs(left_velocity);
        float drag = -sign * definition->drag * vehicle_type_human_plane_abs(definition->left_speed.toward_rate);
        drag = PIN(drag, -speed * g_human_plane_drag_scale, speed * g_human_plane_drag_scale);
        real_vector3d drag_force;
        scale_vector3d(&left, drag * instance->mass, &drag_force);
        add_vectors3d(&force, &drag_force, &force);
    }
    if (m_vertical_speed == 0.0f)
    {
        float sign = vertical_velocity > 0.0f ? 1.0f : -1.0f;
        float speed = vehicle_type_human_plane_abs(vertical_velocity);
        float drag = -sign * definition->drag * vehicle_type_human_plane_abs(definition->vertical_speed.toward_rate);
        drag = PIN(drag, -speed * g_human_plane_drag_scale, speed * g_human_plane_drag_scale);
        real_vector3d drag_force;
        scale_vector3d(&up, drag * instance->mass, &drag_force);
        add_vectors3d(&force, &drag_force, &force);
    }
    float angular_acceleration = definition->angular_acceleration * 15.0f;
    float bank_threshold = 0.075f;
    float bank_scale = -0.25f;
    float maximum_bank = definition->maximum_bank_angle;
    float maximum_turn = 2.0943952f;
    if (TEST_BIT(vehicle->unit.control_flags, 12))
        maximum_turn *= definition->boosted_turn_scale;
    real_vector2d forward2d = forward->xy;
    real_vector2d velocity2d = linear_velocity.xy;
    float lateral_fraction = definition->left_speed.positive_limit < 0.001f ? 0.0f : m_left_speed / fabs(definition->left_speed.positive_limit);
    float forward_fraction = maximum_speed < 0.001f ? 0.0f : m_forward_speed * boost_scale / maximum_speed;
    float velocity_bank = MAX(vehicle_type_human_plane_abs(forward_fraction) - vehicle_type_human_plane_abs(lateral_fraction), 0.0f)
        * maximum_bank * cross_product2d(&forward2d, &velocity2d) / fabs(maximum_speed);
    float control_bank = bank_scale * lateral_fraction * maximum_bank;
    float bank = vehicle_type_human_plane_abs(control_bank) > maximum_bank * bank_threshold
        ? (control_bank < 0.0f ? (control_bank > velocity_bank ? velocity_bank : control_bank) : MAX(control_bank, velocity_bank))
        : (vehicle_type_human_plane_abs(velocity_bank) > vehicle_type_human_plane_abs(control_bank) ? velocity_bank : control_bank);
    bank = PIN(bank, -maximum_bank, maximum_bank);
    float bank_cosine = (float)cos(bank);
    float bank_sine = (float)sin(bank);
    vehicle_type_human_plane_yaw_vectors(&up, forward, bank_sine, bank_cosine);
    real_matrix4x3 object_rotation, desired_rotation, relative_rotation;
    matrix4x3_rotation_from_vectors(&object_rotation, &vehicle->object.forward, &vehicle->object.up);
    matrix4x3_rotation_from_vectors(&desired_rotation, forward, &up);
    matrix4x3_inverse(&object_rotation, &object_rotation);
    matrix4x3_compute_product(&desired_rotation, &object_rotation, &relative_rotation);
    real_quaternion rotation;
    matrix3x3_rotation_to_quaternion(&relative_rotation.rotation, &rotation);
    real_vector3d axis;
    float angle;
    quaternion_to_angle_and_vector(&rotation, &angle, &axis);
    float turn_speed = 3.0f * angle / k_pi * maximum_turn;
    turn_speed = PIN(turn_speed, -maximum_turn, maximum_turn);
    real_vector3d desired_angular_velocity;
    scale_vector3d(&axis, turn_speed, &desired_angular_velocity);
    float torque_scale = angular_acceleration * instance->mass;
    real_vector3d torque;
    torque.i = torque_scale * (desired_angular_velocity.i - vehicle->object.angular_velocity.i);
    torque.j = torque_scale * (desired_angular_velocity.j - vehicle->object.angular_velocity.j);
    torque.k = torque_scale * (desired_angular_velocity.k - vehicle->object.angular_velocity.k);
    scale_vector3d(&force, vehicle->unit.seat_acceleration_fractions[0], &force);
    scale_vector3d(&torque, vehicle->unit.seat_acceleration_fractions[0], &torque);
    havok_vehicle_physics_update(vehicle_index, instance, 0.0f, false, &force, &torque, 0);
}

// @address 0x35a450 vehicle_type_human_plane_find_exit_point
extern "C" void vehicle_type_human_plane_find_exit_point(long vehicle_index, float distance, float height, float radius, short seat_type, real_point3d *point)
{
    long type = seat_type;
    if (!(radius > 0.0f))
        radius = 0.1f;
    real_point3d origin;
    if (type == 0 || type == 1)
        unit_get_camera_position(vehicle_index, &origin);
    else
        object_get_origin(vehicle_index, &origin);
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    real_vector3d forward = vehicle->unit.aiming_vector;
    if (type == 1 || type == 2)
        forward.k = 0.0f;
    normalize3d_with_default(&forward, global_forward3d);
    real_point3d test_point;
    test_point.x = forward.i * distance + origin.x;
    test_point.y = forward.j * distance + origin.y;
    test_point.z = forward.k * distance + origin.z + height;
    real_point3d center;
    object_get_center_of_mass(vehicle_index, &center);
    real_point3d fixed_point;
    real_point3d result;
    if (havok_try_and_get_sphere_fixed_position(radius, radius * 3.0f, _havok_group_vehicle, 10, -1, &test_point, &center, vehicle_index, &fixed_point, true))
        result = fixed_point;
    else
        result = center;
    if (point)
        *point = result;
}

static __forceinline void vehicle_type_human_plane_create_effect(
    c_tag_index effect_index, const s_effect_vector *effect_vectors, const s_location *hint, float scale)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);
    creation_data.definition_index = effect_index;
    creation_data.effect_vector_count = 7;
    creation_data.plane = 0;
    creation_data.flags = 4;
    creation_data.effect_vectors = effect_vectors;
    if (hint)
        creation_data.location_hint = *hint;
    creation_data.scale_a = scale;
    creation_data.scale_b = scale;
    creation_data.structure_bsp_index = -1;
    effect_new_from_creation_data(&creation_data);
}
// @address 0xf3368 vehicle_type_human_plane_create_effects
extern "C" void vehicle_type_human_plane_create_effects(long vehicle_index, float first_scale, float second_scale)
{
    object_datum *vehicle = (object_datum *)object_get_and_verify_type(vehicle_index, 2);
    const vehicle_definition *tag_definition =
        TAG_GET('vehi', vehicle_definition, vehicle->definition_index);
    if (tag_definition->effect.index != -1)
    {
        object_marker markers[16];
        short first_count = internal_object_get_markers_by_string_id(vehicle_index, 175, markers, 15, false);
        short second_count = internal_object_get_markers_by_string_id(vehicle_index, 176, &markers[first_count], (short)(16 - first_count), false);
        if (first_count + second_count > 0)
        {
            short marker_index = 0;
            do
            {
                object_marker *marker = &markers[marker_index];
                collision_result collision;
                real_vector3d random_direction;
                _random_vector_in_cone3d(get_local_random_seed_address(), 0, "..\\units\\vehicle_utilities.cpp", 0x252,
                    &marker->matrix.forward, 0.0f, 0.2617994f, &random_direction);
                float magnitude = marker_index < first_count ? first_scale : second_scale;
                float lateral_direction = random_direction.j;
                float speed = magnitude * 6.0f + 2.0f;
                real_vector3d velocity;
                velocity.i = random_direction.i * speed;
                velocity.j = lateral_direction * speed;
                velocity.k = random_direction.k * speed;
                if (collision_test_vector(k_collision_test_structure_and_water, false, &marker->matrix.position, &velocity, vehicle_index, -1, -1, &collision))
                {
                    real_vector3d forward = random_direction;
                    normalize3d_with_default(&forward, global_down3d);
                    s_effect_vector effect_vectors[7];
                    build_common_effect_vectors(&collision.point, &forward, &collision.plane.normal, effect_vectors);
                    vehicle_type_human_plane_create_effect(
                        c_tag_index(tag_definition->effect.index), effect_vectors, &vehicle->location, 1.0f - collision.fraction);
                }
                ++marker_index;
            } while (marker_index < first_count + second_count);
        }
    }
}
