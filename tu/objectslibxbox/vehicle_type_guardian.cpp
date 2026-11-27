// objectslibxbox/vehicle_type_guardian.obj: from the records 0x375370
// @flags /TP /O2 /Oc
#include "units/vehicle_type_guardian.h"
#include "units/vehicle_definitions.h"
#include "units/vehicle_datum.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_types.h"
#include <math.h>
#include "math/real_math.h"
#include "objects/objects.h"
#include "units/units.h"
#include "physics/havok_utilities.h"
#include "units/vehicle_utilities.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"

__forceinline const s_vehicle_guardian_definition *c_vehicle_type_guardian::get_type_definition(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    return (const s_vehicle_guardian_definition *)((const vehicle_definition *)
        global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(vehicle->object.definition_index)].definition)->guardian.address;
}

// @address 0x375370 ?update_physics@c_vehicle_type_guardian@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_guardian::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    const s_vehicle_guardian_definition *definition = get_type_definition(vehicle_index);
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    const vehicle_definition *vehicle_def = TAG_GET('vehi', const vehicle_definition, vehicle->object.definition_index);
    float throttle_scale = vehicle->unit.seat_acceleration_fractions[0];
    float acceleration_fraction = vehicle->unit.seat_acceleration_fractions[0];
    float tick_length = tls_game_time_globals()->tick_length;
    real_vector3d linear_impulse = *global_zero_vector3d;
    real_vector3d angular_impulse = *global_zero_vector3d;
    real_vector3d linear_velocity;
    real_vector3d angular_velocity, local_velocity, reference_angular_velocity;
    object_get_localized_velocities(vehicle_index, &linear_velocity, &angular_velocity, &local_velocity, &reference_angular_velocity);
    if (acceleration_fraction == 1.0f && vehicle->object.havok_component_index != -1)
    {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, vehicle->object.havok_component_index);
        long controlling_body = component->m_object_postion_controlling_rigid_body_index;
        if (controlling_body < 0 || controlling_body >= component->get_rigid_body_count())
            controlling_body = -1;
        short body_index = (short)controlling_body;
        if (body_index != -1)
        {
            float maximum_angular_velocity = 6.36f * k_pi;
            float maximum_angular_acceleration = 2.0f * k_pi;
            real_matrix3x3 rotation;
            rotation.forward = vehicle->unit.facing_vector;
            rotation.forward.k = 0.0f;
            normalize3d_with_default(&rotation.forward, global_forward3d);
            rotation.up = *global_up3d;
            cross_product3d(&rotation.up, &rotation.forward, &rotation.left);
            real_matrix3x3 inverse_rotation, relative_rotation;
            matrix3x3_transpose(&instance->world_matrix.rotation, &inverse_rotation);
            matrix3x3_multiply(&rotation, &inverse_rotation, &relative_rotation);
            real_quaternion quaternion;
            matrix3x3_rotation_to_quaternion(&relative_rotation, &quaternion);
            float angle;
            real_vector3d axis;
            quaternion_to_angle_and_vector(&quaternion, &angle, &axis);
            float angular_speed = dot_product3d(&angular_velocity, &axis);
            float scale;
            float acceleration = calculate_acceleration_magnitude(false, false, instance->mass, angle,
                maximum_angular_velocity, angular_speed, maximum_angular_acceleration,
                0.0f, 0.0f, false, &scale);
            add_vectors3d(&angular_velocity, &reference_angular_velocity, &angular_velocity);
            float angular_scale = definition->torque_scale * acceleration;
            point_from_line3d(&angular_velocity, &axis, angular_scale, &angular_velocity);
            component->set_rigid_body_angular_velocity(body_index, &angular_velocity);
            subtract_vectors3d(&angular_velocity, &reference_angular_velocity, &angular_velocity);
        }
    }
    real_vector3d throttle;
    vehicle_compute_relative_throttle(vehicle_index, &throttle);
    real_vector3d relative_velocity;
    matrix4x3_inverse_transform_vector(&instance->world_matrix, &linear_velocity, &relative_velocity);
    float boost = 1.0f;
    if (vehicle->vehicle.m_flags.test(_guardian_vehicle_boost))
        boost = unit_compute_boost_fraction(vehicle_index) * (vehicle_def->boost_peak_power - 1.0f) + 1.0f;
    float angle = (float)atan2(throttle.j, throttle.i);
    float forward_fraction = (1.5707964f - (float)fabs(angle)) / 1.5707964f;
    float forward_weight = (float)fabs(forward_fraction);
    float lateral_fraction = (1.0f - forward_weight) * (angle > 0.0f ? 1.0f : -1.0f);
    float lateral_weight = (float)fabs(lateral_fraction);
    bool forward = true;
    if (!(forward_fraction > 0.0f))
        forward = false;
    bool left;
    if (lateral_fraction > 0.0f)
        left = true;
    else
        left = false;
    float forward_speed = forward ? definition->maximum_forward_speed : definition->maximum_reverse_speed;
    float lateral_speed = left ? definition->maximum_left_slide : definition->maximum_right_slide;
    float speed = (forward_speed * forward_weight + lateral_speed * lateral_weight) * boost;
    real_vector3d desired_velocity, acceleration;
    scale_vector3d(&throttle, speed, &desired_velocity);
    subtract_vectors3d(&desired_velocity, &relative_velocity, &acceleration);
    scale_vector3d(&acceleration, 1.0f / tick_length, &acceleration);
    acceleration.k = 0.0f;
    bool accelerating = vehicle->unit.throttle.i != 0.0f || vehicle->unit.throttle.j != 0.0f;
    angle = accelerating ? (float)atan2(desired_velocity.j, desired_velocity.i) : (float)atan2(-acceleration.j, -acceleration.i);
    forward_fraction = (1.5707964f - (float)fabs(angle)) / 1.5707964f;
    forward_weight = (float)fabs(forward_fraction);
    lateral_fraction = (1.0f - forward_weight) * (angle > 0.0f ? 1.0f : -1.0f);
    lateral_weight = (float)fabs(lateral_fraction);
    float forward_acceleration = accelerating ? definition->speed_acceleration : definition->speed_deceleration;
    float lateral_acceleration = accelerating ? definition->slide_acceleration : definition->slide_deceleration;
    float maximum_acceleration = (forward_acceleration * forward_weight + lateral_acceleration * lateral_weight) * boost;
    float magnitude_squared = magnitude_squared3d(&acceleration);
    if (magnitude_squared > maximum_acceleration * maximum_acceleration)
        scale_vector3d(&acceleration, maximum_acceleration / sqrtf(magnitude_squared), &acceleration);
    matrix4x3_transform_vector(&instance->world_matrix, &acceleration, &acceleration);
    point_from_line3d(&linear_impulse, &acceleration, instance->mass, &linear_impulse);
    long region_count;
    char *region_permutations;
    object_region_information *regions;
    object_get_region_information(vehicle_index, &region_count, &region_permutations, 0, &regions);
    for (long index = 0; index < instance->definition->anti_gravity_points.count; ++index)
    {
        s_havok_vehicle_anti_gravity_point *wheel = &instance->anti_gravity_points[index];
        const s_havok_vehicle_anti_gravity_point_definition *wheel_definition =
            &((const s_havok_vehicle_anti_gravity_point_definition *)instance->definition->anti_gravity_points.address)[index];
        wheel->strength = throttle_scale;
        if (TEST_BIT(wheel_definition->flags, 0) && VALID_INDEX(wheel_definition->damage_source_region_index, region_count))
            wheel->oscillation = wheel_definition->runtime_damage_state_errors[regions[wheel_definition->damage_source_region_index].state];
        else
            wheel->oscillation = wheel_definition->runtime_damage_state_errors[0];
    }
    instance->steering = m_steering;
    real_point3d hover_position;
    point_from_line3d(&instance->center_of_mass, &instance->world_matrix.up, definition->anti_gravity_force_z_offset, &hover_position);
    havok_vehicle_physics_update(vehicle_index, instance, 0.0f, true, &linear_impulse, &angular_impulse, &hover_position);
    vehicle_update_antigravity_hover(instance, &m_hover);
}
