// objectslibxbox/vehicle_type_chopper.obj: from the records 0x377540 0x377618 0x377818 0x3784d0 0x378690 0x378a28 0x378af8
// @flags /TP /O2 /Oc
#include "units/vehicle_type_chopper.h"
#include "tag_files/string_ids.h"
#include "models/model_types.h"
#include "memory/tls_globals.h"
#include "models/render_model_definitions.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
// @address 0x378690 ?compute_function_value@c_vehicle_type_chopper@@QAA_NJJPAMPA_N@Z
// @address 0x378a28 ?update_boost@c_vehicle_type_chopper@@CAXJPAUs_havok_vehicle_physics_instance@@@Z
// @address 0x378af8 ?update_damping@c_vehicle_type_chopper@@AAAXJPAUs_havok_vehicle_physics_instance@@@Z
// @address 0x366be8 ?is_braking@c_vehicle_braking_counter@@QAA_NJ@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
#include <math.h>
#include "animations/animation_id.h"
#define CHOPPER_MIN(a, b) ((a) < (b) ? (a) : (b))
#define CHOPPER_MAX(a, b) ((a) > (b) ? (a) : (b))
#define CHOPPER_PIN(a, low, high) CHOPPER_MIN(CHOPPER_MAX(a, low), high)
#include "objects/objects.h"
#include "units/units.h"
#include "physics/havok_vehicle_physics.h"
#include "units/vehicle_engine.h"
#include "units/vehicle_utilities.h"
#include "physics/physics_variables.h"
#include "math/matrix_math.h"
#include "objects/object_early_movers.h"
#include "animations/animation_manager.h"
#include "physics/havok_component.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "physics/physics_variable_types.h"






#include "units/vehicle_animation.h"
#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "physics/havok_component_class.h"

struct s_vehicle_chopper_definition {
    s_vehicle_steering_control_definition steering_control;
    s_vehicle_turning_control_definition turning_control;
    s_vehicle_engine_definition engine;
    float maximum_turn_angle;
    long marker_name;
    float lateral_force_scale;
    float lateral_acceleration;
    float maximum_lateral_velocity;
    float lateral_acceleration_exponent;
    float lateral_slip_velocity;
    float lateral_slip_exponent;
    float forward_slip_velocity;
    float forward_slip_exponent;
    float minimum_radius_scale;
    float steering_response_rate;
    float wheel_acceleration;
    float damping;
};



namespace {
static __forceinline const s_vehicle_chopper_definition *vehicle_get_chopper_definition(long definition_index)
{
    const vehicle_definition *definition = TAG_GET('vehi', const vehicle_definition, definition_index);
    return (const s_vehicle_chopper_definition *)definition->chopper.address;
}
static __forceinline float vehicle_type_chopper_safe_divide(float numerator, float denominator)
{
    float result = 0.0f;
    if (fabsf(denominator) > 0.001f)
        result = numerator / denominator;
    return result;
}
static __forceinline float vehicle_type_chopper_gear_ratio(const s_vehicle_engine *engine, const s_vehicle_engine_definition *definition, short gear_index)
{
    float result = 0.0f;
    if (gear_index != -1) {
        if (gear_index >= 0 && gear_index < definition->gears.count) {
            const s_vehicle_gear_definition *gear_definition = &((const s_vehicle_gear_definition *)definition->gears.address)[gear_index];
            result = gear_definition->ratio;
        }
    } else {
        result = 0.0f;
    }
    return result;
}
static __forceinline float vehicle_type_chopper_wheel_ground_speed(const s_vehicle_engine *engine, const s_vehicle_engine_definition *definition)
{
    float result = engine->rpm * vehicle_type_chopper_gear_ratio(engine, definition, engine->gear);
    return result;
}
static __forceinline c_animation_manager *vehicle_type_chopper_get_animation_manager(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_vehicle);
    return (c_animation_manager *)((char *)object + object->animation.offset);
}
}

bool c_vehicle_type_chopper::compute_function_value(long vehicle_index, long function_name, float *value, bool *active)
{
    // match: 0x378690: __restrict hoists the throttle_scale load above the *value store; without it the lfs sinks one slot below the stfs.
    vehicle_datum *__restrict vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_chopper_definition *definition = vehicle_get_chopper_definition(vehicle->object.definition_index);
    bool result = true;
    switch (function_name) {
    case _string_id_engine_power:
        *value = vehicle_type_chopper_safe_divide(engine.rpm, definition->engine.maximum_rpm);
        if (vehicle->unit.throttle_scale <= 0.0f) {
            *value = 0.0f;
            *active = false;
        } else {
            *active = true;
        }
        break;
    case _string_id_speed_absolute: {
        float maximum_ground_speed = vehicle_type_chopper_gear_ratio(
                                         &engine, &definition->engine, (short)(definition->engine.gears.count - 1)) *
            definition->engine.maximum_rpm;
        float ground_speed = vehicle_type_chopper_wheel_ground_speed(&engine, &definition->engine);
        *value = vehicle_type_chopper_safe_divide(fabsf(ground_speed), fabsf(maximum_ground_speed));
        break;
    }
    case _string_id_engine_rpm:
        *value = vehicle_type_chopper_safe_divide(engine.rpm, definition->engine.maximum_rpm);
        break;
    case _string_id_engine_cruising:
        *value = CHOPPER_PIN(1.0f - engine.throttle, 0.0f, 1.0f) *
            vehicle_type_chopper_safe_divide(engine.rpm, definition->engine.maximum_rpm);
        break;
    case _string_id_jump:
        *value = braking_counter.is_braking(vehicle_index) || (vehicle->unit.control_flags & 2) != 0 || e_braking ? 1.0f : 0.0f;
        break;
    case _string_id_front_left_tire_position: case _string_id_front_right_tire_position:
    case _string_id_back_left_tire_position: case _string_id_back_right_tire_position:
        *value = vehicle_type_chopper_safe_divide(turn_angle, definition->maximum_turn_angle);
        break;
    case _string_id_front_left_tire_velocity: case _string_id_front_right_tire_velocity:
    case _string_id_back_left_tire_velocity: case _string_id_back_right_tire_velocity:
        *value = vehicle_type_chopper_safe_divide(fabsf(engine.rpm), fabsf(definition->engine.maximum_rpm));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void c_vehicle_type_chopper::update_boost(long vehicle_index, s_havok_vehicle_physics_instance *havok_vehicle_physics_instance)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    if ((bool)((vehicle->vehicle.flags >> 4) & 1)) {
        float fraction = ::unit_compute_boost_fraction(vehicle_index);
        const vehicle_definition *definition = TAG_GET('vehi', const vehicle_definition, vehicle->object.definition_index);
        float boost = fraction * definition->boost_peak_power;
        real_vector3d linear_velocity;
        real_vector3d angular_velocity;
        object_get_velocities(vehicle_index, &linear_velocity, &angular_velocity);
        float scale = TLS_GAME_TIME_GLOBALS->tick_length * boost;
        point_from_line3d(&linear_velocity, &havok_vehicle_physics_instance->world_matrix.forward, scale, &linear_velocity);
        object_set_velocities(vehicle_index, &linear_velocity, &angular_velocity);
    }
}

// @address 0x3784d0 ?process_animation_channels@c_vehicle_type_chopper@@QAAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z21_N@Z
void c_vehicle_type_chopper::process_animation_channels(long vehicle_index,
    void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
    void *user_data, c_animation_channel *channel, bool initialize)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const vehicle_definition *definition = TAG_GET('vehi', const vehicle_definition, vehicle->object.definition_index);
    const s_vehicle_chopper_definition *chopper = vehicle_get_chopper_definition(vehicle->object.definition_index);
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
    const render_model_definition *render_model = TAG_GET('mode', const render_model_definition, model->render_model_index);
    c_animation_manager *animation = vehicle_type_chopper_get_animation_manager(vehicle_index);
    if (initialize) {
        turn = animation->find_parallel_impulse_animation_id(226, 226, 129);
        wheel = animation->find_parallel_impulse_animation_id(226, 226, 124);
    }
    if (turn.valid() && animation->setup_animation_channel_by_index(channel, turn, 0)) {
        float maximum_turn_angle = chopper->maximum_turn_angle;
        float fraction = maximum_turn_angle > 0.0f ? turn_angle / maximum_turn_angle : 0.0f;
        vehicle_impulse_channel_node_preprocess_callback(vehicle_index, render_model, channel, fraction, 0.0f, 1.0f, user_data);
    }
    if (wheel.valid() && animation->setup_animation_channel_by_index(channel, wheel, 0)) {
        vehicle_impulse_channel_node_preprocess_callback(vehicle_index, render_model, channel, wheel_position, 0.0f, 1.0f, user_data);
    }
}

void c_vehicle_type_chopper::update_damping(long vehicle_index, struct s_havok_vehicle_physics_instance * havok_vehicle_physics_instance)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_chopper_definition *chopper = vehicle_get_chopper_definition(vehicle->object.definition_index);
    if (fabsf(chopper->engine.maximum_rpm) > 0.001f) {
        float factor = 1.0f -
            fabsf(this->engine.rpm) * (TLS_GAME_TIME_GLOBALS->tick_length / chopper->engine.maximum_rpm) *
                chopper->damping;
        real_vector3d angular_velocity;
        object_get_velocities(vehicle_index, 0, &angular_velocity);
        matrix4x3_inverse_transform_vector(&havok_vehicle_physics_instance->world_matrix, &angular_velocity, &angular_velocity);
        angular_velocity.i *= factor;
        matrix4x3_transform_vector(&havok_vehicle_physics_instance->world_matrix, &angular_velocity, &angular_velocity);
        object_set_velocities(vehicle_index, 0, &angular_velocity);
    }
}

// @address 0x377540 ?update_control@c_vehicle_type_chopper@@QAAXJ@Z
void c_vehicle_type_chopper::update_control(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_chopper_definition *definition = vehicle_get_chopper_definition(vehicle->object.definition_index);
    vehicle_update_boost(vehicle_index, false);
    vehicle_update_control_classic(vehicle_index, &vehicle->object.forward, &vehicle->object.up, vehicle_compute_throttle1d(vehicle_index), 0.0f,
        &definition->steering_control, &definition->turning_control, 0, 0, engine.gear == 0, false,
        &wheel_position, 0, 0);
    e_braking = (((vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle))->unit.control_flags >> 11) & 1;
}

static __forceinline float vehicle_type_chopper_power(float base, float exponent)
{
    return (float)pow(base, exponent);
}
static __forceinline long vehicle_type_chopper_sign(float value)
{
    return value != 0.0f ? (value < 0.0f ? -1 : 1) : 0;
}

#define VEHICLE_TYPE_CHOPPER_ABS(value) ((value) >= 0.0f ? (value) : -(value))

// @address 0x377618 ?chopper_calculate_acceleration_magnitude@@YAMMMMMMMPAM@Z
float chopper_calculate_acceleration_magnitude(float velocity, float acceleration,
    float deceleration, float maximum_velocity, float target_velocity,
    float exponent, float *scale)
{
    float acceleration_magnitude = acceleration * vehicle_type_chopper_power(VEHICLE_TYPE_CHOPPER_ABS(velocity), exponent);
    float acceleration_per_tick = acceleration_magnitude * game_tick_length();
    float maximum_acceleration = VEHICLE_TYPE_CHOPPER_ABS(acceleration_per_tick);
    float deceleration_per_tick = deceleration * game_tick_length();
    float speed = (float)fabs(velocity);
    float result = 0.0f;

    if (acceleration_per_tick > 0.001f)
    {
        float stopping_ticks = 1.0f + (float)fabs(deceleration) / maximum_acceleration;
        float stopping_distance = (float)fabs(0.5f * stopping_ticks * deceleration_per_tick);
        if (stopping_distance < speed)
            result = maximum_acceleration * (float)-vehicle_type_chopper_sign(velocity);
        else
            result = (float)-vehicle_type_chopper_sign(velocity) * square_root(2.0f * speed * maximum_acceleration);
    }

    *scale = VEHICLE_TYPE_CHOPPER_ABS(maximum_velocity) > 0.001f
        ? VEHICLE_TYPE_CHOPPER_ABS(deceleration / maximum_velocity) : 0.0f;
    return (result > -maximum_acceleration ? result : -maximum_acceleration) < maximum_acceleration
        ? (result > -maximum_acceleration ? result : -maximum_acceleration) : maximum_acceleration;
}



namespace {
static __forceinline c_havok_component *vehicle_type_chopper_get_component(long index)
{
    s_data_array *data = (s_data_array *)g_havok_component_data;
    return (c_havok_component *)(data->data + (index & 0xffff) * sizeof(c_havok_component));
}
static __forceinline void *chopper_friction_point_get(void *array, long index, long element_size)
{
    return (char *)array + index * element_size;
}
}

static __forceinline float vehicle_type_chopper_normalize2d(real_vector2d *vector);

// @address 0x377818 ?update_physics@c_vehicle_type_chopper@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_chopper::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    float half_angle = wheel_position * 0.5f;
    float tick_length = game_tick_length();
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_chopper_definition *chopper_definition = vehicle_get_chopper_definition(vehicle->object.definition_index);
    float cos_half_angle = (float)cos((double)half_angle);
    float sin_half_angle = (float)sin((double)half_angle);
    float throttle = vehicle_compute_throttle1d(vehicle_index);
    float lateral_speed = 0.0f;
    float forward_speed = 0.0f;
    real_vector3d force = *global_zero_vector3d;
    real_vector3d torque = *global_zero_vector3d;
    float absolute_throttle = throttle < 0.0f ? -throttle : throttle;
    braking_counter.update(vehicle_index);
    bool braking = braking_counter.is_braking(vehicle_index);
    const s_vehicle_engine_definition *engine_definition = &chopper_definition->engine;
    short desired_gear = vehicle_engine_get_desired_gear(&engine, engine_definition, throttle, braking, vehicle->unit.driver_object_index != -1, false);
    vehicle_engine_update(vehicle_index, &engine, engine_definition, absolute_throttle, desired_gear, braking);
    float ground_speed;
    if (braking)
        ground_speed = 0.0f;
    else
        ground_speed = vehicle_type_chopper_wheel_ground_speed(&engine, engine_definition);
    for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++) {
            s_havok_vehicle_friction_point_definition *point_name = 
                &((s_havok_vehicle_friction_point_definition *)instance->definition->friction_points.address)[point_index];
            unsigned long point_flags = point_name->flags;
            bool turns_forward = ((point_flags >> 2) & 1) != 0;
            bool turns_reverse = ((point_flags >> 3) & 1) != 0;
            real_quaternion rotation;
            instance->friction_points[point_index].forward_velocity = ground_speed;
            if (braking)
                instance->friction_points[point_index].braking = 1;
            if (turns_forward) {
                rotation.v.i = 0.0f;
                rotation.v.j = 0.0f;
                rotation.v.k = sin_half_angle;
                rotation.w = cos_half_angle;
                matrix3x3_rotation_from_quaternion(&instance->friction_points[point_index].matrix.rotation, &rotation);
                instance->friction_points[point_index].matrix.position.x = 0.0f;
                instance->friction_points[point_index].matrix.position.y = 0.0f;
                instance->friction_points[point_index].matrix.position.z = 0.0f;
                instance->friction_points[point_index].matrix.scale = 1.0f;
                matrix4x3_compute_product(&instance->world_matrix, &instance->friction_points[point_index].matrix,
                    &instance->friction_points[point_index].matrix);
            }
            else if (turns_reverse) {
                rotation.v.i = 0.0f;
                rotation.v.j = 0.0f;
                rotation.v.k = -sin_half_angle;
                rotation.w = cos_half_angle;
                matrix3x3_rotation_from_quaternion(&instance->friction_points[point_index].matrix.rotation, &rotation);
                instance->friction_points[point_index].matrix.position.x = 0.0f;
                instance->friction_points[point_index].matrix.position.y = 0.0f;
                instance->friction_points[point_index].matrix.position.z = 0.0f;
                instance->friction_points[point_index].matrix.scale = 1.0f;
                matrix4x3_compute_product(&instance->world_matrix, &instance->friction_points[point_index].matrix,
                    &instance->friction_points[point_index].matrix);
            }
            else {
                instance->friction_points[point_index].matrix.rotation = instance->world_matrix.rotation;
            }
        }
    for (long wheel_index = 0; wheel_index < instance->definition->anti_gravity_points.count; wheel_index++)
        instance->anti_gravity_points[wheel_index].strength = vehicle->unit.throttle_scale;
    object_marker marker;
    if (internal_object_get_markers_by_string_id(vehicle_index, chopper_definition->marker_name, &marker, 1, false) != 0) {
        long havok_index = vehicle->object.havok_component_index;
        if (havok_index != -1) {
            if (vehicle_type_chopper_get_component(havok_index)->get_object_postion_controlling_rigid_body_index() != -1) {
                c_havok_component *havok_component = vehicle_type_chopper_get_component(havok_index);
                real_vector3d rigid_velocity = *global_zero_vector3d;
                real_vector3d early_offset;
                real_vector3d early_axis;
                if (instance->definition->friction_points.count > 0) {
                    float inverse_count = 1.0f / (float)instance->definition->friction_points.count;
                    real_point3d average_point = *global_origin3d;
                    for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++) {
                        average_point.x += instance->friction_points[point_index].marker_position.x;
                        average_point.y += instance->friction_points[point_index].marker_position.y;
                        average_point.z += instance->friction_points[point_index].marker_position.z;
                    }
                    average_point.x *= inverse_count;
                    average_point.y *= inverse_count;
                    average_point.z *= inverse_count;
                    havok_component->get_rigid_body_linear_velocity_at_point(
                        havok_component->get_object_postion_controlling_rigid_body_index(), &rigid_velocity, &average_point);
                    if (object_get_early_mover_local_space_velocity(vehicle_index, &average_point, &early_offset, &early_axis, true, false))
                        subtract_vectors3d(&rigid_velocity, &early_offset, &rigid_velocity);
                }
                real_vector2d heading = { vehicle->unit.desired_aiming_vector.i, vehicle->unit.desired_aiming_vector.j };
                real_vector2d forward = { instance->world_matrix.rotation.forward.i, instance->world_matrix.rotation.forward.j };
                if (vehicle_type_chopper_normalize2d(&heading) != 0.0f && vehicle_type_chopper_normalize2d(&forward) != 0.0f) {
                                float first_angle = (float)atan2((double)heading.j, (double)heading.i);
                                float second_angle = (float)atan2((double)forward.j, (double)forward.i);
                                float angle_difference = second_angle - first_angle;
                                if (!(fabsf(angle_difference) <= 3.1415927f)) {
                                    do {
                                        float wrap;
                                        if (!(angle_difference <= 0.0f))
                                            wrap = -k_two_pi;
                                        else
                                            wrap = k_two_pi;
                                        angle_difference = wrap + angle_difference;
                                    } while ((float)fabsf(angle_difference) > 3.1415927f);
                                }
                }
                real_vector3d linear_velocity;
                real_point3d center_of_mass;
                object_get_velocities(vehicle_index, 0, &linear_velocity);
                object_get_center_of_mass(vehicle_index, &center_of_mass);
                if (object_get_early_mover_local_space_velocity(vehicle_index, &center_of_mass, &early_offset, &early_axis, true, false)) {
                    subtract_vectors3d(&linear_velocity, &early_axis, &linear_velocity);
                }
                forward_speed = dot_product3d(&marker.matrix.rotation.up, &linear_velocity);
                real_quaternion relative_rotation;
                relative_rotation.v.i = 0.0f;
                relative_rotation.v.j = 0.0f;
                relative_rotation.v.k = sin_half_angle;
                relative_rotation.w = cos_half_angle;
                real_matrix4x3 relative_matrix;
                matrix3x3_rotation_from_quaternion(&relative_matrix.rotation, &relative_rotation);
                relative_matrix.position.x = 0.0f;
                relative_matrix.position.y = 0.0f;
                relative_matrix.position.z = 0.0f;
                relative_matrix.scale = 1.0f;
                matrix4x3_compute_product(&instance->world_matrix, &relative_matrix, &relative_matrix);
                real_vector3d point_velocity;
                havok_component->get_rigid_body_linear_velocity_at_point(
                    havok_component->get_object_postion_controlling_rigid_body_index(), &point_velocity, &marker.matrix.position);
                if (object_get_early_mover_local_space_velocity(vehicle_index, &marker.matrix.position, &early_offset, &early_axis, true, false)) {
                    subtract_vectors3d(&point_velocity, &early_offset, &point_velocity);
                }
                lateral_speed = -dot_product3d(&rigid_velocity, &relative_matrix.rotation.left);
                float point_lateral_speed = dot_product3d(&point_velocity, &instance->world_matrix.rotation.left);
                float clamped_lateral = point_lateral_speed <= -20.0f ? -20.0f : (point_lateral_speed >= 20.0f ? 20.0f : point_lateral_speed);
                float acceleration_scale;
                float acceleration = chopper_calculate_acceleration_magnitude(clamped_lateral, chopper_definition->lateral_acceleration,
                    clamped_lateral, chopper_definition->maximum_lateral_velocity, 0.0f,
                    chopper_definition->lateral_acceleration_exponent, &acceleration_scale);
                real_vector3d to_marker;
                vector_from_points3d(&instance->center_of_mass, &marker.matrix.position, &to_marker);
                float push_scale = chopper_definition->lateral_force_scale * instance->mass * acceleration;
                real_vector3d push;
                real_vector3d twist;
                scale_vector3d(&marker.matrix.rotation.left, push_scale, &push);
                cross_product3d(&to_marker, &push, &twist);
                const float &twist_scale = dot_product3d(&instance->world_matrix.rotation.up, &twist);
                scale_vector3d(&instance->world_matrix.rotation.up, twist_scale, &twist);
                add_vectors3d(&force, &push, &force);
                add_vectors3d(&torque, &twist, &torque);
            }
        }
    }
    float turn_absolute = VEHICLE_TYPE_CHOPPER_ABS(chopper_definition->lateral_slip_velocity);
    physics_variable_position seek_bounds;
    seek_bounds.upper_bound = 1.0f;
    seek_bounds.lower_bound = -1.0f;
    float turn_ratio = VEHICLE_TYPE_CHOPPER_ABS(chopper_definition->forward_slip_velocity) > 0.001f
        ? CHOPPER_PIN((ground_speed < 0.0f ? -1.0f : 1.0f) * forward_speed,
            -chopper_definition->forward_slip_velocity, chopper_definition->forward_slip_velocity) / chopper_definition->forward_slip_velocity : 0.0f;
    turn_ratio = (float)vehicle_type_chopper_sign(turn_ratio) * fabsf(vehicle_type_chopper_power(fabsf(turn_ratio), chopper_definition->forward_slip_exponent));
    float steer_ratio = VEHICLE_TYPE_CHOPPER_ABS(chopper_definition->lateral_slip_velocity) > 0.001f
        ? CHOPPER_PIN(lateral_speed / chopper_definition->lateral_slip_velocity, -turn_absolute, turn_absolute) : 0.0f;
    steer_ratio = (float)vehicle_type_chopper_sign(steer_ratio) *
        fabsf(vehicle_type_chopper_power(fabsf(steer_ratio), chopper_definition->lateral_slip_exponent));
    float steering_target = VEHICLE_TYPE_CHOPPER_ABS(turn_ratio) > VEHICLE_TYPE_CHOPPER_ABS(steer_ratio)
        ? turn_ratio : steer_ratio;
    physics_variable_position_update_seek(&steering_fraction, &seek_bounds, false, steering_target, chopper_definition->steering_response_rate, tick_length);
    for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++) {
        {
            s_havok_vehicle_friction_point_definition *point_name = (s_havok_vehicle_friction_point_definition *)chopper_friction_point_get(
                instance->definition->friction_points.address, point_index, 76);
            s_friction_vehicle_physics_point_datum *point = &instance->friction_points[point_index];
            internal_object_get_markers_by_string_id(vehicle_index, point_name->marker_name, &marker, 1, false);
            long marker_negative = marker.node_to_marker.position.y < 0.0f ? 1 : 0;
            long value_negative = steering_fraction < 0.0f ? 1 : 0;
            if (marker_negative == value_negative) {
                float grip = fabsf(steering_fraction);
                grip = CHOPPER_PIN(grip, 0.0f, 1.0f);
                point->current_radius = point->current_radius - (1.0f - chopper_definition->minimum_radius_scale) * (grip * grip) * point->current_radius;
            }
        }
    }
    instance->upside_down = instance->world_matrix.rotation.up.k < 0.0f;
    float stop_fraction = stop_counter.calculate_fraction(vehicle_index);
    havok_vehicle_physics_update(vehicle_index, instance, stop_fraction, true, &force, &torque, 0);
    engine.load_torque = instance->load_torque;
    bool any_active = false;
    {
        for (long point_index = 0; point_index < instance->definition->friction_points.count; point_index++) {
            if (instance->friction_points[point_index].grounded != 0) {
                any_active = true;
                break;
            }
        }
    }
    if (!any_active)
        engine.up_ticks = 0;
    stop_counter.update(vehicle_index, absolute_throttle, braking);
    float current_speed = wheel_speed;
    float speed_error = ground_speed - current_speed;
    float max_step = chopper_definition->wheel_acceleration * tick_length;
    if (speed_error > max_step)
        wheel_speed = current_speed + max_step;
    else if (speed_error < -max_step)
        wheel_speed = current_speed - max_step;
    else
        wheel_speed = ground_speed;
    turn_angle = turn_angle + wheel_speed * tick_length;
    float maximum_turn_angle = chopper_definition->maximum_turn_angle;
    float wrapped_angle;
    if (!(maximum_turn_angle <= 0.001f))
        wrapped_angle = (float)fmod((double)turn_angle, (double)maximum_turn_angle);
    else
        wrapped_angle = 0.0f;
    turn_angle = wrapped_angle;
    if (!(wrapped_angle >= 0.0f))
        turn_angle = wrapped_angle + chopper_definition->maximum_turn_angle;
    update_boost(vehicle_index, instance);
    update_damping(vehicle_index, instance);
}

static __forceinline float vehicle_type_chopper_normalize2d(real_vector2d *vector)
{
    float magnitude = sqrtf(vector->i * vector->i + vector->j * vector->j);
    if (fabsf(magnitude) < 0.0001f)
        return 0.0f;
    float scale = 1.0f / magnitude;
    vector->i *= scale;
    vector->j *= scale;
    return magnitude;
}
