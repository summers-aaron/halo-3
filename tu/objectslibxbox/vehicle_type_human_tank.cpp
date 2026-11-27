// objectslibxbox/vehicle_type_human_tank.obj: from the records 0x36f818 0x36fb80 0x36fe10 0x36ff40
// @flags /TP /O2 /Oc
#include "units/vehicle_type_human_tank.h"
#include "units/vehicle_definitions.h"
#include "units/vehicle_datum.h"
#include "physics/havok_vehicle_physics.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include <math.h>
#include "math/real_math.h"
#include "units/vehicle_engine.h"
#include "units/vehicle_utilities.h"
#include "physics/physics_variables.h"
#include "units/vehicle_counters.h"
#include "physics/physics_variable_types.h"

// @address 0x36fe10 ?compute_tread_speeds@c_vehicle_type_human_tank@@AAAXJPAM0@Z
// @address 0x36ff40 ?compute_function_value@c_vehicle_type_human_tank@@QAA_NJJPAMPA_N@Z

// Function names (string_ids) the tank answers in compute_function_value.
enum
{
    _string_id_left_tread_position = 542,
    _string_id_right_tread_position = 543,
    _string_id_left_tread_velocity = 544,
    _string_id_right_tread_velocity = 545,
    _string_id_engine_rpm = 558,
    _string_id_engine_power = 571,
    _string_id_tread_grind = 763
};

static __forceinline vehicle_datum *human_tank_vehicle_get(long object_index)
{
    return (vehicle_datum *)object_get_and_verify_type(object_index, _object_mask_vehicle);
}
static __forceinline const s_vehicle_human_tank_definition *human_tank_get_definition(long definition_index)
{
    vehicle_definition *definition = TAG_GET('vehi', vehicle_definition, definition_index);
    return (const s_vehicle_human_tank_definition *)definition->human_tank.address;
}
static __forceinline float human_tank_get_gear_ratio(const s_vehicle_engine_definition *definition, short gear)
{
    float ratio = 0.0f;
    if (gear != -1)
    {
        if (gear >= 0 && gear < definition->gears.count)
            ratio = ((const s_vehicle_gear_definition *)definition->gears.address)[gear].ratio;
    }
    else
        ratio = 0.0f;
    return ratio;
}
static __forceinline float human_tank_engine_get_wheel_ground_speed(const s_vehicle_engine *engine, const s_vehicle_engine_definition *definition)
{
    return engine->rpm * human_tank_get_gear_ratio(definition, engine->gear);
}
static __forceinline float human_tank_safe_divide(float numerator, float denominator)
{
    float result = 0.0f;
    if (fabsf(denominator) > 0.001f)
        result = numerator / denominator;
    return result;
}

void c_vehicle_type_human_tank::compute_tread_speeds(long vehicle_index, float *left_tread_speed, float *right_tread_speed)
{
    vehicle_datum *vehicle = human_tank_vehicle_get(vehicle_index);
    const s_vehicle_human_tank_definition *definition = human_tank_get_definition(vehicle->object.definition_index);
    float wheel_speed = human_tank_engine_get_wheel_ground_speed(&m_engine, &definition->engine);
    float tread_speed = m_engine.gear == 0 ? definition->reverse_differential.positive_limit : definition->forward_differential.positive_limit;
    float maximum_speed = tread_speed > 0.001f ? tread_speed : 0.001f;
    float turn = m_differential / maximum_speed;
    float speed = wheel_speed < 0.0f ? -wheel_speed : wheel_speed;
    *left_tread_speed = (1.0f - (turn < 0.0f ? -turn : turn)) * wheel_speed - speed * turn;
    *right_tread_speed = (1.0f - (turn < 0.0f ? -turn : turn)) * wheel_speed + speed * turn;
}

bool c_vehicle_type_human_tank::compute_function_value(long vehicle_index, long function_name, float *value, bool *active)
{
    // match: 0x36ff40: restrict hoists the throttle_scale load into fr13 above the *value store; plain sinks it below into fr0.
    vehicle_datum *__restrict vehicle = human_tank_vehicle_get(vehicle_index);
    // match: 0x36ff40: restrict sinks the tread_grind *value store below the rpm reloads; plain keeps it above them.
    const s_vehicle_human_tank_definition *__restrict definition = human_tank_get_definition(vehicle->object.definition_index);
    bool result = true;
    switch (function_name) {
    case _string_id_engine_power:
        *value = human_tank_safe_divide(m_engine.rpm, definition->engine.maximum_rpm);
        *value = *value * vehicle_engine_get_rpm_function_scale(&m_engine);
        if (vehicle->unit.throttle_scale <= 0.0f) {
            *value = 0.0f;
            *active = false;
        } else {
            *active = true;
        }
        break;
    case _string_id_engine_rpm:
        *value = human_tank_safe_divide(m_engine.rpm, definition->engine.maximum_rpm);
        *value = *value * vehicle_engine_get_rpm_function_scale(&m_engine);
        break;
    case _string_id_tread_grind:
        if (m_engine.gear != -2 && m_engine.gear != -1) {
            float tread_speed = m_engine.gear == 0 ? definition->reverse_differential.positive_limit : definition->forward_differential.positive_limit;
            *value = human_tank_safe_divide(fabsf(m_differential), fabsf(tread_speed));
            *value = 0.5f * (*value + human_tank_safe_divide(m_engine.rpm, definition->engine.maximum_rpm));
        }
        break;
    case _string_id_left_tread_position:
        *value = human_tank_safe_divide(m_left_tread_position, definition->wheel_circumference);
        break;
    case _string_id_right_tread_position:
        *value = human_tank_safe_divide(m_right_tread_position, definition->wheel_circumference);
        break;
    case _string_id_left_tread_velocity:
    case _string_id_right_tread_velocity: {
        float left_tread_speed;
        float right_tread_speed;
        compute_tread_speeds(vehicle_index, &left_tread_speed, &right_tread_speed);
        float maximum_ground_speed = definition->engine.maximum_rpm * human_tank_get_gear_ratio(&definition->engine, m_engine.gear);
        float tread_speed;
        if (function_name == _string_id_left_tread_velocity)
            tread_speed = left_tread_speed;
        else
            tread_speed = right_tread_speed;
        *value = human_tank_safe_divide(fabsf(tread_speed), fabsf(maximum_ground_speed));
        break;
    }
    default:
        result = false;
        break;
    }
    return result;
}

static inline real_vector2d *human_tank_control_set(real_vector2d *vector, float i, float j)
{
    vector->i = i;
    vector->j = j;
    return vector;
}
static inline real_vector2d *human_tank_control_scale(const real_vector2d *vector, float scale, real_vector2d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    return result;
}
static inline real_vector2d *human_tank_control_add(const real_vector2d *a, const real_vector2d *b, real_vector2d *result)
{
    result->i = a->i + b->i;
    result->j = a->j + b->j;
    return result;
}
static inline real_vector2d *human_tank_control_perpendicular(const real_vector2d *vector, real_vector2d *result)
{
    float i = -vector->j;
    result->j = vector->i;
    result->i = i;
    return result;
}
static inline float human_tank_control_normalize(real_vector2d *vector)
{
    float length = sqrtf(magnitude_squared2d(vector));
    if (fabsf(length - 0.0f) < k_real_epsilon)
        length = 0.0f;
    else {
        human_tank_control_scale(vector, 1.0f / length, vector);
    }
    return length;
}

static inline real_vector3d *human_tank_control_get_throttle(long vehicle_index, real_vector3d *throttle)
{
    const vehicle_datum *vehicle = (const vehicle_datum *)human_tank_vehicle_get(vehicle_index);
    throttle->i = vehicle->unit.throttle.i * vehicle->unit.throttle_scale;
    throttle->j = vehicle->unit.throttle.j * vehicle->unit.throttle_scale;
    throttle->k = 0.0f;
    return throttle;
}

// @address 0x36f818 ?update_control@c_vehicle_type_human_tank@@QAAXJ@Z
void c_vehicle_type_human_tank::update_control(long vehicle_index)
{
    bool reverse = m_engine.gear == 0;
    vehicle_datum *vehicle = (vehicle_datum *)human_tank_vehicle_get(vehicle_index);
    real_vector3d throttle;
    human_tank_control_get_throttle(vehicle_index, &throttle);
    const s_vehicle_human_tank_definition *definition = human_tank_get_definition(vehicle->object.definition_index);
    vehicle_update_boost(vehicle_index, 0);
    vehicle->vehicle.m_flags.set(_vehicle_flag_3, (vehicle->unit.control_flags & (1 << 1)) != 0);
    real_vector2d direction;
    human_tank_control_set(&direction, vehicle->unit.facing_vector.i, vehicle->unit.facing_vector.j);
    human_tank_control_normalize(&direction);
    real_vector2d motion;
    human_tank_control_set(&motion, throttle.i * direction.i - throttle.j * direction.j,
        throttle.i * direction.j + throttle.j * direction.i);
    real_vector2d forward;
    human_tank_control_set(&forward, vehicle->object.forward.i, vehicle->object.forward.j);
    if (human_tank_control_normalize(&forward) == 0.0f)
        forward = *global_forward2d;
    real_vector2d perpendicular;
    human_tank_control_perpendicular(&forward, &perpendicular);
    float angle_limit = PIN(definition->maximum_angle, 0.0f, k_pi);
    float turn_limit = reverse ? k_pi - angle_limit : angle_limit;
    real_vector2d blend_direction;
    human_tank_control_set(&blend_direction, vehicle->unit.facing_vector.i, vehicle->unit.facing_vector.j);
    if (human_tank_control_normalize(&blend_direction) == 0.0f)
        blend_direction = *global_forward2d;
    float complement = 1.0f - dot_product2d(&forward, &blend_direction);
    real_vector2d scaled_forward;
    human_tank_control_scale(&forward, complement, &scaled_forward);
    real_vector2d combined;
    human_tank_control_add(&blend_direction, &scaled_forward, &combined);
    if (human_tank_control_normalize(&combined) == 0.0f)
        combined = forward;
    real_vector2d combined_perpendicular;
    human_tank_control_perpendicular(&combined, &combined_perpendicular);
    float direction_angle = (float)atan2(dot_product2d(&motion, &combined_perpendicular), dot_product2d(&motion, &combined));
    bool exceeds = fabsf(direction_angle) >= turn_limit;
    float steer_angle = (float)atan2(dot_product2d(&motion, &perpendicular), dot_product2d(&motion, &forward));
    float angle_adjust = exceeds ? (steer_angle > 0.0f ? -k_pi : k_pi) : 0.0f;
    float target_angle = angle_adjust + steer_angle;
    short gear;
    if (magnitude_squared3d(&throttle) != 0.0f)
        gear = !exceeds;
    else
        gear = m_engine.gear;
    m_engine.gear = gear;
    float turn_scale;
    const physics_variable_speed *speed;
    turn_scale = exceeds ? definition->reverse_turn_scale : definition->forward_turn_scale;
    speed = exceeds ? &definition->reverse_differential : &definition->forward_differential;
    float seek = PIN(turn_scale * target_angle, -0.7853982f, 0.7853982f);
    float fraction = seek * 1.2732395f;
    float limit = fraction < 0.0f ? speed->negative_limit : speed->positive_limit;
    physics_variable_speed_update_seek(&m_differential, speed, limit * fraction, 1.0f, tls_game_time_globals()->tick_length);
}

static __forceinline void human_tank_engine_post_physics_update(
    s_vehicle_engine *engine, const s_vehicle_engine_definition *definition, s_havok_vehicle_physics_instance *instance)
{
    engine->load_torque = instance->load_torque;
    bool any_grounded = false;
    for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++) {
        if (instance->friction_points[point_index].grounded) {
            any_grounded = true;
            break;
        }
    }
    if (!any_grounded)
        engine->up_ticks = 0;
}

static __forceinline real_vector3d *human_tank_physics_scale_throttle(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    return result;
}

static __forceinline real_vector3d *human_tank_physics_get_throttle(long vehicle_index, real_vector3d *throttle)
{
    const vehicle_datum *vehicle = (const vehicle_datum *)human_tank_vehicle_get(vehicle_index);
    human_tank_physics_scale_throttle(&vehicle->unit.throttle, vehicle->unit.throttle_scale, throttle);
    throttle->k = 0.0f;
    return throttle;
}

// @address 0x36fb80
void c_vehicle_type_human_tank::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    float tick_length = tls_game_time_globals()->tick_length;
    const vehicle_datum *vehicle = (const vehicle_datum *)human_tank_vehicle_get(vehicle_index);
    const s_vehicle_human_tank_definition *definition = human_tank_get_definition(vehicle->object.definition_index);
    real_vector3d throttle;
    human_tank_physics_get_throttle(vehicle_index, &throttle);
    float throttle_magnitude = sqrtf(magnitude_squared3d(&throttle));
    vehicle_engine_update(vehicle_index, &m_engine, &definition->engine,
        MIN(throttle_magnitude, 1.0f),
        vehicle_engine_get_desired_gear(&m_engine, &definition->engine,
            MIN(throttle_magnitude, 1.0f), false, vehicle->unit.driver_object_index != -1, true), false);
    float left_tread_speed, right_tread_speed;
    compute_tread_speeds(vehicle_index, &left_tread_speed, &right_tread_speed);
    for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++)
        instance->friction_points[point_index].forward_velocity = TEST_BIT(point_index, 0) ? right_tread_speed : left_tread_speed;
    m_left_tread_position += left_tread_speed * tick_length;
    m_left_tread_position = definition->wheel_circumference > 0.001f
        ? (float)fmod(m_left_tread_position > 0.0f ? m_left_tread_position : definition->wheel_circumference + m_left_tread_position,
        definition->wheel_circumference)
        : 0.0f;
    m_right_tread_position += right_tread_speed * tick_length;
    m_right_tread_position = definition->wheel_circumference > 0.001f
        ? (float)fmod(m_right_tread_position > 0.0f ? m_right_tread_position : definition->wheel_circumference + m_right_tread_position,
        definition->wheel_circumference)
        : 0.0f;
    havok_vehicle_physics_update(vehicle_index, instance, m_stop_counter.calculate_fraction(vehicle_index), true, 0, 0, 0);
    human_tank_engine_post_physics_update(&m_engine, &definition->engine, instance);
    m_stop_counter.update(vehicle_index, throttle_magnitude, false);
}
