// objectslibxbox/vehicle_utilities.obj: from the records 0xf0d58 0xf0e68 0xf30f8 0xf31e0 0x366590 0x3665f0 0x366700 0x366800 0x366898 0x366998 0x366a98 0x366be8 0x366c70 0x366da0 0x367060 0x367120
// @flags /TP /O2 /J /Oc
#include "math/real_math_types.h"
#include "physics/havok_vehicle_physics.h"
#include "game/game_time_types.h"
#include "objects/object_datum.h"
// @address 0xf0d58 vehicle_update_boost
// @address 0xf0e68 ?vehicle_update_control_classic@@YAMJPBTreal_vector3d@@0MMPBUs_vehicle_steering_control_definition@@PBUs_vehicle_turning_control_definition@@PBUphysics_variable_speed@@3_N4PAM55@Z
// @address 0xf30f8 ?vehicle_try_to_join_local_physics@@YAXJPBUs_havok_vehicle_physics_instance@@@Z
// @address 0xf31e0 vehicle_update_engine_scales
// @address 0x366590 ?vehicle_degrade_variable@@YAXPAM@Z
// @address 0x3665f0 ?vehicle_scale_gravity_for_friction_point_vehicle@@YAXJPBTreal_vector3d@@PAT1@M@Z
// @address 0x366700 ?vehicle_compute_acceleration@@YAPATreal_vector3d@@PBT1@0MMPAT1@M@Z
// @address 0x366800 ?vehicle_compute_throttle1d@@YAMJ@Z
// @address 0x366898 ?update@c_vehicle_stop_counter@@QAAXJM_N@Z
// @address 0x366998 ?calculate_fraction@c_vehicle_stop_counter@@QAAMJ@Z
// @address 0x366a98 ?update@c_vehicle_braking_counter@@QAAXJ@Z
// @address 0x366be8 ?is_braking@c_vehicle_braking_counter@@QAA_NJ@Z
// @address 0x366c70 ?vehicle_update_antigravity_hover@@YAXPBUs_havok_vehicle_physics_instance@@PAM@Z
// @address 0x367060 ?vehicle_compute_relative_throttle@@YAXJPATreal_vector3d@@@Z
// @address 0x367120 vehicle_has_element_value
#include <math.h>
#include "objects/objects.h"
#include "units/units.h"
#include "units/vehicles.h"
#include "units/vehicle_utilities.h"

#include "physics/physics_variables.h"
#include "units/vehicle_type_human_plane.h"
#include "units/vehicle_datum.h"
#include "cache/cache_files.h"
#include "units/vehicle_counters.h"
#include "objects/object_header.h"
#include "game/game_time_inline.h"
#include "math/real_math.h"
#include "math/real_math_limit.h"
#include "tag_files/tag_groups.h"
#include "units/vehicle_type_alien_scout.h"
#include "tag_files/tag_types.h"
#include "physics/physics_variable_types.h"
#include "units/vehicle_types.h"
#include "cseries/crt_xdk.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "rasterizer/rasterizer_depth_bias.h"
#include "math/real_math_trigonometry.h"
#include "objects/object_ai_properties.h"







#include "units/vehicle_type_vtol.h"

__forceinline long object_local_physics_object_get(long index)
{
    vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(index, ~0u);
    long result = -1;
    if (object->object.m_physics_flags.test(_object_physics_flag_1) || object->object.m_physics_flags.test(_object_physics_flag_2))
        result = object->object.local_physics_object_index;
    return result;
}

extern "C" unsigned char vehicle_update_boost(long object_index, unsigned char force)
{
    vehicle_datum *object = (vehicle_datum *)object_get(object_index);
    unit_definition *definition = (unit_definition *)tag_get('vehi', object->object.definition_index);
    unsigned char active;
    unsigned char result;
    unsigned char bit;
    unsigned char rising;
    if (force || (object->unit.control_flags & 0x800))
        active = 1;
    else
        active = 0;
    result = 0;
    ((volatile unsigned short &)object->vehicle.flags) = ((volatile unsigned short &)object->vehicle.flags) & 0xffef;
    bit = (definition->flags >> 28) & 1;
    if (bit != 0 && (active || object->unit.boost_time > 0.0f)) {
        rising = object->unit.boost_time > object->unit.boost_decay_time;
        if (rising != 0) {
            ((volatile unsigned short &)object->vehicle.flags) = ((volatile unsigned short &)object->vehicle.flags) | 16;
        } else {
            ((volatile unsigned short &)object->vehicle.flags) = ((volatile unsigned short &)object->vehicle.flags) & 0xffef;
        }
        if (active != 0) {
            const real_vector3d *default_vector = global_forward3d;
            object->unit.throttle = *default_vector;
            result = 1;
        }
    }
    return result;
}




static __forceinline bool vehicle_utilities_unit_is_boosting(long unit_index)
{
    bool boosting = false;
    if (::unit_compute_boost_fraction(unit_index) > 0.0f)
        boosting = true;
    return boosting;
}
static __forceinline float vehicle_utilities_physics_variable_speed_from_fraction(const physics_variable_speed *speed, float fraction)
{
    return fraction * (fraction < 0.0f ? speed->negative_limit : speed->positive_limit);
}

// 0xf0e68
float vehicle_update_control_classic(long vehicle_index, const real_vector3d *forward, const real_vector3d *up,
    float throttle_forward, float throttle_left,
    const s_vehicle_steering_control_definition *steering_control,
    const s_vehicle_turning_control_definition *turning_control,
    const physics_variable_speed *forward_speed, const physics_variable_speed *left_speed,
    bool reverse, bool force, float *turn_position, float *forward_value, float *left_value)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    float dt = game_tick_length();
    bool stopped;
    if (vehicle->vehicle.m_flags.test(_vehicle_flag_3) && !::vehicle_is_running_trick(vehicle_index) && !vehicle_utilities_unit_is_boosting(vehicle_index))
        stopped = true;
    else
        stopped = false;
    float steering_angle = 0.0f;
    vehicle->vehicle.m_flags.set(_vehicle_flag_3, (vehicle->unit.control_flags & 2) != 0);

    real_vector3d left;
    cross_product3d(up, forward, &left);
    float forward_dot = dot_product3d(&vehicle->unit.facing_vector, forward);
    steering_angle = arctangent(dot_product3d(&vehicle->unit.facing_vector, &left), forward_dot);

    if (forward_speed)
    {
        if (stopped)
            physics_variable_speed_update_seek(forward_value, forward_speed, 0.0f, 1.0f, dt);
        else
            physics_variable_speed_update_seek(forward_value, forward_speed,
                vehicle_utilities_physics_variable_speed_from_fraction(forward_speed, throttle_forward), 1.0f, dt);
    }
    if (left_speed)
    {
        if (stopped)
            physics_variable_speed_update_seek(left_value, left_speed, 0.0f, 1.0f, dt);
        else
            physics_variable_speed_update_seek(
                left_value, left_speed, vehicle_utilities_physics_variable_speed_from_fraction(left_speed, throttle_left), 1.0f, dt);
    }
    if (steering_control)
    {
        // match: 0xf0e68: pi spelled as a literal; the real_math.h k_pi constant reloads -pi at each PIN instead of holding it
        steering_angle = PIN(steering_angle, -3.14159265359f, 3.14159265359f);
        float fraction = fabs(steering_angle / steering_control->overdampen_cusp_angle);
        float sign = steering_angle >= 0.0f ? 1.0f : -1.0f;
        steering_angle = sign * steering_control->overdampen_cusp_angle * pow(fraction, steering_control->overdampen_exponent);
        // match: 0xf0e68: as above
        steering_angle = PIN(steering_angle, -3.14159265359f, 3.14159265359f);
    }
    if (turning_control)
    {
        float target = reverse ? -steering_angle : steering_angle;
        physics_variable_position_update_seek(turn_position, &turning_control->turn_bounds, false, target, turning_control->turn_rate, dt);
    }
    return steering_angle;
}

void vehicle_try_to_join_local_physics(long vehicle_index, const s_havok_vehicle_physics_instance *instance)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    long local_physics_object_index = object_local_physics_object_get(vehicle_index);
    if (local_physics_object_index != -1 && !vehicle->object.m_physics_flags.test(_object_physics_joined_local_physics_bit))
    {
        long count = 0;
        for (long wheel_index = 0; wheel_index < instance->definition->anti_gravity_points.count; wheel_index++)
        {
            if (instance->anti_gravity_points[wheel_index].local_physics_object_index == local_physics_object_index)
                count++;
        }
        for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++)
        {
            if (instance->friction_points[point_index].object_index == local_physics_object_index)
                count++;
        }
        if (count * 2 > instance->definition->anti_gravity_points.count + instance->definition->friction_points.count)
            vehicle->object.m_physics_flags.set(_object_physics_joined_local_physics_bit, true);
    }
}



// @address 0xf31e0
extern "C" void vehicle_update_engine_scales(vehicle_utilities_engine_input *input, long object_index)
{
    float first_scale;
    float second_scale;
    switch (vehicle_get_type(object_index)) {
    case 0:
    case 1:
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
        return;
    case 2: {
        vehicle_datum *vehicle = (vehicle_datum *)object_get(object_index);
        vehicle_definition *definition =
            (vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
        const s_vehicle_human_plane_definition *gears = (const s_vehicle_human_plane_definition *)definition->human_plane.address;
        first_scale = 0.0f;
        float high = gears->vertical_speed.positive_limit;
        if (high > 0.0001f)
            first_scale = input->m_vertical_speed / high;
        float low = gears->forward_speed.positive_limit;
        second_scale = 0.0f;
        if (low > 0.0001f)
            second_scale = input->m_forward_speed / low;
        vehicle_type_human_plane_create_effects(object_index,
            PIN(first_scale, 0.0f, 1.0f),
            PIN(second_scale, 0.0f, 1.0f));
        return;
    }
    case 3:
        c_vehicle_type_alien_scout::create_effects(object_index);
        return;
    case 7: {
        float scales[2];
        ((const c_vehicle_type_vtol *)input)->compute_engine_scales(&scales[0], &scales[1]);
        first_scale = MAX(scales[0], scales[1]);
        second_scale = 0.0f;
        vehicle_type_human_plane_create_effects(object_index, first_scale, second_scale);
        break;
    }
    default:
        // match: 0xf31e0: unreachable hint omits dead branch check
        __assume(0);
        break;
    }
}

// 0x366590
void vehicle_degrade_variable(float *variable)
{
    float decay = 10.0f * game_tick_length();
    float threshold = 0.05f;
    if ((*variable >= 0.0f ? *variable : -*variable) < threshold)
        *variable = 0.0f;
    else
        *variable *= decay;
}

void vehicle_scale_gravity_for_friction_point_vehicle(long vehicle_index, const real_vector3d *normal, real_vector3d *gravity, float scale)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    vehicle_definition *definition = (vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
    if (definition->havok_vehicle_physics.friction_points.count > 0 && scale > 0.0f)
    {
        long count = 0;
        for (long friction_index = 0; friction_index < definition->havok_vehicle_physics.friction_points.count; friction_index++)
        {
            if ((vehicle->vehicle.grounded & (1 << friction_index)) == 0)
                count++;
        }
        if (count > definition->havok_vehicle_physics.friction_points.count >> 1)
        {
            float dot = dot_product3d(gravity, normal);
            if (dot < 0.0f)
            {
                real_vector3d vertical;
                real_vector3d horizontal;
                scale_vector3d(&vehicle->object.up, dot, &vertical);
                subtract_vectors3d(gravity, &vertical, &horizontal);
                scale_vector3d(&horizontal, scale, &horizontal);
                add_vectors3d(&horizontal, &vertical, gravity);
            }
        }
    }
}

real_vector3d *vehicle_compute_acceleration(const real_vector3d *a, const real_vector3d *b, float maximum, float minimum, real_vector3d *result, float dt)
{
    subtract_vectors3d(b, a, result);
    scale_vector3d(result, 1.0f / dt, result);
    float dot = dot_product3d(result, b);
    if (dot > 0.0001f)
    {
        float t = dot * dot / magnitude_squared3d(result) / magnitude_squared3d(b);
        maximum = minimum + t * (maximum - minimum);
    }
    else
        maximum = minimum;
    limit3d(result, maximum);
    return result;
}

float vehicle_compute_throttle1d(long vehicle_index)
{
  vehicle_datum *vehicle = (vehicle_datum *)object_get(vehicle_index);
  const real_vector2d *throttle = &vehicle->unit.throttle.xy;
  float i = throttle->i;
  float throttle1d = magnitude2d(throttle);
  throttle1d = MIN(throttle1d, 1.0f) * (i != 0.0f ? (i < 0.0f ? -1 : 1) : 0);
  throttle1d *= vehicle->unit.throttle_scale;
  return throttle1d;
}

void c_vehicle_stop_counter::update(long vehicle_index, float throttle, bool force)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    vehicle_definition *definition =
        (vehicle_definition *)tag_get('vehi', vehicle->object.definition_index);
    bool throttling = (throttle < 0.0f ? -throttle : throttle) > 0.001f;
    if ((!throttling || force) && vehicle->vehicle.grounded != (1 << definition->havok_vehicle_physics.friction_points.count) - 1
        && magnitude_squared3d(&vehicle->object.translational_velocity) <= 3.0625f)
        m_ticks = (unsigned char)MIN(m_ticks + 1, 254);
    else
        m_ticks = 0;
}

float c_vehicle_stop_counter::calculate_fraction(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get(vehicle_index);
    long ticks = game_seconds_to_ticks_round(0.45f);
    float fraction = 0.0f;
    if (m_ticks > ticks)
    {
        if (vehicle->unit.throttle_scale == 0.0f || magnitude_squared3d(&vehicle->object.translational_velocity) < 3.0625f)
            fraction = ((float)m_ticks * game_tick_length() - 0.45f) / 0.8f;
    }
    fraction = MIN(MAX(fraction, 0.0f), 1.0f);
    return fraction;
}

void c_vehicle_braking_counter::update(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    bool braking = false;
    if ((vehicle->unit.control_flags & 2) != 0)
    {
        braking = true;
    }
    else if (vehicle->unit.throttle.i != 0.0f)
    {
        real_vector3d velocity;
        object_get_velocities(vehicle_index, &velocity, 0);
        float speed = normalize3d(&velocity);
        if (speed > 1.0f)
        {
            if (vehicle->unit.throttle.i < 0.0f)
            {
                braking = dot_product3d(&velocity, &vehicle->object.forward) > 0.0f;
            }
            else if (vehicle->unit.throttle.i > 0.0f)
            {
                braking = dot_product3d(&velocity, &vehicle->object.forward) < 0.0f;
            }
        }
    }
    if (braking)
        m_ticks = MIN(m_ticks + 1, 254);
    else
        m_ticks = 0;
}

bool c_vehicle_braking_counter::is_braking(long vehicle_index)
{
    vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    return m_ticks != 0 && (game_tick_length() * (float)m_ticks < 0.5f || (object->unit.control_flags & 2) != 0);
}

// 0x366c70
void vehicle_update_antigravity_hover(const s_havok_vehicle_physics_instance *instance, float *hover_height)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(instance->object_index, _object_mask_vehicle);
    const s_havok_vehicle_physics_definition *definition = instance->definition;
    float maximum_height = vehicle->unit.throttle_scale;
    float base_height = MAX(0.4f, vehicle->object.up.k);
    float fraction = 0.0f;
    long wheel_count = 0;
    long active_count = 0;
    for (long wheel_index = 0; wheel_index < definition->anti_gravity_points.count; wheel_index++)
    {
        wheel_count++;
        if (instance->anti_gravity_points[wheel_index].active)
            active_count++;
    }
    if (wheel_count > 0)
        fraction = (float)active_count / (float)wheel_count;
    float target_height = MIN(MAX(base_height * fraction, 0.0f), maximum_height);
    float step = 0.1f;
    float delta = target_height - *hover_height;
    if (delta > step)
        *hover_height += step;
    else if (delta < rasterizer_states_mode_5_slope_scale_bias)
        *hover_height -= step;
    else
        *hover_height = target_height;
}


// Dropped 0x366da0: compiler-generated absolute-value sign table relocates to unowned data.

void vehicle_compute_relative_throttle(long vehicle_index, real_vector3d *relative_throttle)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    real_vector2d facing2d;
    real_vector2d forward2d;
    real_vector2d rotation;

    set_real_vector2d(&facing2d, vehicle->unit.facing_vector.i, vehicle->unit.facing_vector.j);
    normalize2d(&facing2d);
    set_real_vector2d(&forward2d, vehicle->object.forward.i, vehicle->object.forward.j);
    normalize2d(&forward2d);
    set_real_vector2d(&rotation, dot_product2d(&forward2d, &facing2d), cross_product2d(&forward2d, &facing2d));
    set_real_vector3d(relative_throttle,
        vehicle->unit.throttle.i * rotation.i - vehicle->unit.throttle.j * rotation.j,
        vehicle->unit.throttle.i * rotation.j + vehicle->unit.throttle.j * rotation.i,
        vehicle->unit.throttle.k);
}

extern "C" bool vehicle_has_element_value(long object_index, long element_value)
{
    bool result = false;
    if (object_index != -1)
    {
        s_object_header *header = object_header_get(object_index);
        if ((1 << header->object_type) & _object_mask_vehicle)
        {
            vehicle_datum *object = (vehicle_datum *)header->datum;
            vehicle_definition *definition = (vehicle_definition *)tag_get('vehi', object->object.definition_index);
            const object_ai_properties *ai_properties = TAG_BLOCK_GET_ELEMENT(&definition->ai_properties, 0, object_ai_properties);
            if (definition->ai_properties.count > 0 && ai_properties->ai_type_name == element_value)
                result = true;
        }
    }
    return result;
}

// @address 0x366da0 ?vehicle_calculate_turning_torque@@YAXPBUs_havok_vehicle_physics_instance@@PBTreal_vector3d@@PAT2@_NMMM@Z
void vehicle_calculate_turning_torque(const s_havok_vehicle_physics_instance *physics, const real_vector3d *direction,
    real_vector3d *torque, bool airborne, float scale, float throttle, float turning)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(physics->object_index, 2);
    float dt = game_tick_length();
    if (throttle > 0.0f) {
        float angular_speed;
        float angular_acceleration;
        float turning_amount = turning;
        if (!airborne) {
            angular_acceleration = 4.712389f;
            angular_speed = (dot_product3d(direction, &vehicle->object.up) + direction->k) * 0.5f;
            if ((turning_amount > 0.0f) != (angular_speed > 0.0f)) {
                float multiplier = 8.0f;
                float min_speed = 0.7853982f;
                float max_speed = 3.1415927f;
                float speed = angular_speed >= 0.0f ? angular_speed : -angular_speed;
                speed = PIN(speed, min_speed, max_speed);
                angular_acceleration *= 1.0f + (multiplier - 1.0f) * (speed - min_speed) / (max_speed - min_speed);
            }
        } else {
            angular_speed = dot_product3d(&vehicle->object.angular_velocity, &vehicle->object.up);
            angular_acceleration = 3.1415927f;
        }
        float desired_speed = (float)(turning_amount != 0.0f ? (turning_amount < 0.0f ? -1 : 1) : 0)
            * sqrtf(2.0f * angular_acceleration * (float)fabs(turning_amount));
        if ((turning_amount != 0.0f ? (turning_amount < 0.0f ? -1 : 1) : 0)
            == (desired_speed != 0.0f ? (desired_speed < 0.0f ? -1 : 1) : 0)) {
            if ((turning_amount >= 0.0f ? turning_amount : -turning_amount)
                < (desired_speed >= 0.0f ? desired_speed : -desired_speed) * 2.0f * dt)
                desired_speed = 0.5f * turning_amount / dt;
        }
        float acceleration = PIN((desired_speed - angular_speed) / dt, -angular_acceleration, angular_acceleration);
        float force = physics->inertia_tensor.up.k * acceleration;
        point_from_line3d(torque, &vehicle->object.up, scale * force * throttle, torque);
    }
}
