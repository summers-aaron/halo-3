// objectslibxbox/vehicle_type_vtol.obj: from the records 0x375980 0x375b18 0x3761e0 0x3768d8 0x376a28 0x376c58 0x376da0 0x376f48 0x377030 0x377118 0x3772d0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "objects/objects.h"
#include "models/render_model_definitions.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "math/matrix_math.h"
#include "physics/havok_vehicle_physics.h"
#include "units/vehicle_type_vtol.h"
#include "units/vehicle_animation.h"
#include "animations/animation_manager.h"
#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "models/model_types.h"
#include "physics/physics_constants_globals.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "animations/animation_id.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "math/real_math_trigonometry.h"
#include "cseries/cseries.h"
#include "tag_files/string_ids.h"
// @address 0x375980 ?compute_throttle@c_vehicle_type_vtol@@CAXJPIATreal_vector3d@@@Z
// @address 0x3768d8 ?interpolate_lift_angles@c_vehicle_type_vtol@@CAXJPAM0PBM_N@Z
// @address 0x376c58 ?update_rotor_damping@c_vehicle_type_vtol@@CAXJ@Z
// @address 0x376da0 ?process_animation_channels@c_vehicle_type_vtol@@QAAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z21_N@Z
// @address 0x376f48 ?compute_function_value@c_vehicle_type_vtol@@QAA_NJJPAMPA_N@Z
#include <math.h>
#include <float.h>
#include "animations/mixing_board/channels/animation_channel.h"
#include "math/real_math_types.h"





namespace {
struct vehicle_type_vtol_definition {
    s_vehicle_turning_control_definition turning_control;
    long left_marker, right_marker;
    long thrust_marker;
    float lift_angle;
    float lift_dead_angle;
    float drop_angle;
    float drop_dead_angle;
    float altitude_range, active_altitude_range;
    float minimum_acceleration, maximum_acceleration;
    float turn_acceleration, turn_scale;
    float damping;
    float lateral_acceleration, forward_acceleration, marker_offset;
    float drag, constant_drag;
    float upright_exponent, upright_scale, upright_acceleration;
    float maximum_lift_acceleration;
    float maximum_lift_acceleration_boosted;
    float minimum_lift_speed, maximum_lift_speed;
};
static __forceinline bool vehicle_type_vtol_driven_by_ai(long object_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(object_index, 8195);
    if (unit->unit.driver_object_index != -1)
        unit = (unit_datum *)object_get_and_verify_type(unit->unit.driver_object_index, 8195);
    return unit->unit.actor_index != -1;
}
static __forceinline float vehicle_type_vtol_desired_acceleration(float velocity, float minimum, float maximum, float distance)
{
    float maximum_acceleration = velocity > 0.0f ? minimum : maximum;
    float sign = distance > 0.0f ? 1.0f : -1.0f;
    float desired_velocity = square_root(fabsf(2.0f * distance * maximum_acceleration)) * sign;
    return MIN(MAX((desired_velocity - velocity) / game_tick_length(), minimum), maximum);
}
}

// match: 0x375980: without restrict the *throttle store forces a second datum-table walk reloading vehicle instead of reusing r11
void c_vehicle_type_vtol::compute_throttle(long vehicle_index, real_vector3d *__restrict throttle)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, 2);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index)->vtol.address;
    *throttle = vehicle->unit.throttle;
    float vertical_throttle;
    if (vehicle_type_vtol_driven_by_ai(vehicle_index)) {
        vertical_throttle = vehicle->unit.throttle.k;
    } else {
        float angle = arctangent(vehicle->unit.facing_vector.k,
            square_root(vehicle->unit.facing_vector.i * vehicle->unit.facing_vector.i + vehicle->unit.facing_vector.j * vehicle->unit.facing_vector.j));
        if (angle > definition->lift_dead_angle) {
            if (definition->lift_angle - definition->lift_dead_angle > 0.001f)
                vertical_throttle = (angle - definition->lift_dead_angle) / (definition->lift_angle - definition->lift_dead_angle);
            else
                vertical_throttle = 1.0f;
        } else if (angle < definition->drop_dead_angle) {
            if (definition->drop_angle - definition->drop_dead_angle > 0.001f)
                vertical_throttle = (angle - definition->drop_dead_angle) / (definition->drop_angle - definition->drop_dead_angle);
            else
                vertical_throttle = -1.0f;
        } else {
            vertical_throttle = 0.0f;
        }
    }
    throttle->k = (vertical_throttle > -1.0f ? vertical_throttle : -1.0f) < 1.0f
        ? (vertical_throttle > -1.0f ? vertical_throttle : -1.0f) : 1.0f;
}

void c_vehicle_type_vtol::interpolate_lift_angles(long object_index, float *angles, float *velocity, const float *desired_angles, bool boosted)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(object_index, 0);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index)->vtol.address;
    float maximum_acceleration = boosted ? definition->maximum_lift_acceleration_boosted : definition->maximum_lift_acceleration;
    for (unsigned long i = 0; i < 2; i++)
    {
        float acceleration = vehicle_type_vtol_desired_acceleration(velocity[i], -maximum_acceleration, maximum_acceleration, desired_angles[i] - angles[i]);
        velocity[i] += acceleration * game_tick_length();
        angles[i] += velocity[i] * game_tick_length();
    }
}


// @rdata 0x82021694
static const float k_vehicle_type_vtol_turn_max_velocity = FLT_MAX;

// @address 0x376a28 vehicle_type_vtol_compute_turn_output
extern "C" float vehicle_type_vtol_compute_turn_output(long object_index, const s_havok_vehicle_physics_instance *input)
{
    vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, unit->object.definition_index)->vtol.address;
    real_vector3d linear_velocity, angular_velocity;
    object_get_localized_velocities(object_index, &linear_velocity, &angular_velocity, 0, 0);

    real_matrix3x3 basis0;
    basis0.forward = unit->unit.facing_vector;
    basis0.forward.k = 0.0f;
    normalize3d_with_default(&basis0.forward, global_forward3d);
    basis0.up = *global_up3d;
    cross_product3d(&basis0.up, &basis0.forward, &basis0.left);

    real_matrix3x3 basis1;
    basis1.forward = input->world_matrix.forward;
    basis1.forward.k = 0.0f;
    normalize3d_with_default(&basis1.forward, global_forward3d);
    basis1.up = *global_up3d;
    cross_product3d(&basis1.up, &basis1.forward, &basis1.left);

    matrix3x3_transpose(&basis1, &basis1);
    real_matrix3x3 product;
    matrix3x3_multiply(&basis0, &basis1, &product);
    real_quaternion rotation;
    matrix3x3_rotation_to_quaternion(&product, &rotation);
    float angle;
    real_vector3d axis;
    quaternion_to_angle_and_vector(&rotation, &angle, &axis);
    if (!(axis.k >= 0.0f)) {
        angle *= -1.0f;
        scale_vector3d(&axis, -1.0f, &axis);
    }
    float velocity = dot_product3d(&angular_velocity, &axis);
    float result = vehicle_type_vtol_compute_damped_response(true, false, input->mass, angle, definition->turn_acceleration, velocity,
        k_vehicle_type_vtol_turn_max_velocity, 0.0f, 0.0f, false, &angle);
    return definition->turn_scale * result;
}

void c_vehicle_type_vtol::update_rotor_damping(long object_index)
{
    vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, unit->object.definition_index)->vtol.address;
    if (unit->object.parent_object_index == -1) {
        real_vector3d linear, angular, linear_parent, angular_parent;
        object_get_localized_velocities(object_index, &linear, &angular, &linear_parent, &angular_parent);
        const real_vector3d up = {0.0f, 0.0f, 1.0f};
        real_vector3d vertical, horizontal;
        scale_vector3d(&up, dot_product3d(&up, &angular), &vertical);
        subtract_vectors3d(&angular, &vertical, &horizontal);
        float damping = 1.0f - MIN(MAX(unit->unit.throttle_scale * definition->damping * game_tick_length(), 0.0f), 1.0f);
        scale_vector3d(&horizontal, damping, &horizontal);
        add_vectors3d(&vertical, &horizontal, &angular);
        add_vectors3d(&angular, &angular_parent, &angular);
        ::object_set_velocities(object_index, &linear, &angular);
    }
}

void c_vehicle_type_vtol::process_animation_channels(long object_index,
    void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
    void *user, c_animation_channel *channel, bool refresh)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    vehicle_definition *definition = TAG_GET('vehi', vehicle_definition, object->definition_index);
    s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
    render_model_definition *render_model = TAG_GET('mode', render_model_definition, model->render_model_index);
    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->animation.offset);
    if (refresh) {
        animation_ids[0] = manager->find_parallel_impulse_animation_id(226, 226, 90);
        animation_ids[1] = manager->find_parallel_impulse_animation_id(226, 226, 91);
    }
    if (animation_ids[0].valid() && manager->setup_animation_channel_by_index(channel, animation_ids[0], 0))
        vehicle_impulse_channel_node_preprocess_callback(object_index, render_model, channel, -lift_i, -lift_j, 1.0f, user);
    if (animation_ids[1].valid() && manager->setup_animation_channel_by_index(channel, animation_ids[1], 0))
        vehicle_impulse_channel_node_preprocess_callback(object_index, render_model, channel, -lower_i, -lower_j, 1.0f, user);
}


bool c_vehicle_type_vtol::compute_function_value(long vehicle_index, long function, float *value, bool *active)
{
    bool result = true;
    float left, right;
    switch (function) {
    case _string_id_engine_power_left:
        this->compute_engine_scales((float *)(&left), (float *)(&right));
        *value = left;
        break;
    case _string_id_engine_power_right:
        this->compute_engine_scales((float *)(&left), (float *)(&right));
        *value = right;
        break;
    case _string_id_engine_power:
        this->compute_engine_scales((float *)(&left), (float *)(&right));
        *value = 0.5f * (left + right);
        break;
    case _string_id_engine_rpm:
        this->compute_engine_scales((float *)(&left), (float *)(&right));
        *value = left > right ? left : right;
        break;
    case _string_id_gear_position:
        *value = lift;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

// @address 0x377030 ?compute_engine_scales@c_vehicle_type_vtol@@ABAXPAM0@Z
void c_vehicle_type_vtol::compute_engine_scales(float * scale_a, float * scale_b) const
{
    float a = 0.0f;
    float b = 0.0f;
    for (unsigned long i = 0; i < 4; i++)
    {
        a += (float)this->engine_a[i];
        b += (float)this->engine_b[i];
    }
    *scale_a = 0.0009803922f * a;
    *scale_b = 0.0009803922f * b;
}

#define VEHICLE_TYPE_VTOL_SIGN(x) ((x) != 0.0f ? ((x) < 0.0f ? -1 : 1) : 0)
const float k_real_max = FLT_MAX;
const float k_real_min = -FLT_MAX;

// @address 0x377118 vehicle_type_vtol_compute_altitude_acceleration
extern "C" float vehicle_type_vtol_compute_altitude_acceleration(float velocity, float acceleration, float minimum, float maximum, float distance)
{
    float tick_length = game_tick_length();
    float desired_velocity = -(tick_length * acceleration - distance / game_tick_length());
    float braking_acceleration;
    if (velocity > 0.0f)
        braking_acceleration = acceleration;
    else
        braking_acceleration = acceleration + maximum;
    braking_acceleration = fabsf(braking_acceleration);
    float stopping_ticks;
    if (fabsf(braking_acceleration) < 0.0001f)
        stopping_ticks = 1000.0f;
    else
        stopping_ticks = fabsf(velocity / braking_acceleration) / game_tick_length();
    float stopping_distance = fabsf(game_tick_length() * stopping_ticks * velocity * 0.5f);
    if (fabsf(stopping_distance) - 0.1f >= fabsf(distance))
    {
        desired_velocity = square_root(fabsf(braking_acceleration * distance / (tick_length * 0.5f))) * (float)VEHICLE_TYPE_VTOL_SIGN(distance)
            - game_tick_length() * acceleration;
    }
    float limit = square_root(fabsf(braking_acceleration) * fabsf(distance) * 2.0f);
    float upper_limit = limit < 0.0f ? k_real_max : limit;
    float lower_limit = limit < 0.0f ? limit : k_real_min;
    desired_velocity = MIN(MAX(desired_velocity, lower_limit), upper_limit);
    return MIN(MAX((desired_velocity - velocity) / game_tick_length(), minimum), maximum);
}
// @address 0x3772d0 vehicle_type_vtol_compute_damped_response
extern "C" float vehicle_type_vtol_compute_damped_response(bool clamp_to_infinity, bool scale_by_time, float time, float target,
    float acceleration, float velocity, float max_velocity, float friction, float exponent, bool instant, float *scale)
{
    float target_magnitude = (float)fabs(target);
    float acceleration_tick = game_tick_length() * acceleration;
    float acceleration_magnitude = acceleration_tick >= 0.0f ? acceleration_tick : -acceleration_tick;
    float velocity_tick = game_tick_length() * velocity;
    float result;
    target -= VEHICLE_TYPE_VTOL_SIGN(target) * 0.0f;
    if (acceleration_tick > 0.001f)
    {
        float power = target_magnitude * 5.0f;
        float ticks = (float)fabs(velocity) / acceleration_magnitude + 1.0f;
        float stopping_distance = (float)fabs(ticks * velocity_tick * 0.5f);
        power = MIN(MAX(power, 0.0f), 1.0f);
        power = (float)pow((double)power, 0.5);
        if (stopping_distance < target_magnitude)
            result = (float)VEHICLE_TYPE_VTOL_SIGN(target) * power * acceleration_magnitude + velocity;
        else
            result = square_root(acceleration_magnitude * target_magnitude * 2.0f) * (float)-VEHICLE_TYPE_VTOL_SIGN(velocity);
        result = MIN(MAX(result, k_real_min), k_real_max);
    }
    else
    {
        result = MIN(MAX(acceleration_tick + velocity, k_real_min), k_real_max);
    }
    float scale_value = velocity * 2.938736e-39f;
    if (scale_value < 0.0f)
        scale_value = -scale_value;
    *scale = scale_value;
    return MIN(MAX(result - velocity, -acceleration_magnitude), acceleration_magnitude);
}


// @rdata 0x8202490c
static const float k_vehicle_type_vtol_normalization_epsilon = 0.0001f;
// match: 0x3761e0: the lift-direction normalize is expanded in place; the shared normalize3d_with_default stays a call there
static __forceinline float vehicle_type_vtol_normalize3d_with_default(real_vector3d *vector, const real_vector3d *fallback)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_vehicle_type_vtol_normalization_epsilon)) {
        scale_vector3d(vector, 1.0f / length, vector);
        return length;
    } else {
        *vector = *fallback;
        return 0.0f;
    }
}

// @address 0x3761e0 ?update_lift_vector@c_vehicle_type_vtol@@AAAXJPBUs_havok_vehicle_physics_instance@@PBTreal_vector3d@@MJPAT3@PATreal_point3d@@PATreal_euler_angles2d@@4PATreal_vector2d@@5PAM@Z
void c_vehicle_type_vtol::update_lift_vector(long object_index, const s_havok_vehicle_physics_instance *havok,
    const real_vector3d *input_vector, float scale, long marker_name, real_vector3d *lift_vector, real_point3d *position,
    real_euler_angles2d *angles, real_euler_angles2d *angles2, real_vector2d *velocity, real_vector2d *velocity2, float *out_value)
{
    vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, unit->object.definition_index)->vtol.address;
    *position = havok->center_of_mass;
    *lift_vector = *global_zero_vector3d;
    object_marker marker;
    if (internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false) != 1)
        return;

    float weights[4];
    weights[0] = scale;
    weights[1] = -(definition->forward_acceleration * input_vector->i);
    weights[2] = -(input_vector->j * definition->lateral_acceleration);
    weights[3] = vehicle_type_vtol_compute_turn_output(object_index, havok);

    real_vector3d forward = unit->unit.aiming_vector;
    forward.k = 0.0f;
    normalize3d_with_default(&forward, global_forward3d);
    real_matrix4x3 basis;
    matrix4x3_from_point_and_vectors(&basis, &havok->center_of_mass, &forward, global_up3d);
    real_vector3d rows[4];
    rows[0] = *global_down3d;
    rows[1] = basis.forward;
    rows[2] = basis.left;
    real_vector3d to_marker;
    vector_from_points3d(&basis.position, &marker.matrix.position, &to_marker);
    normalize3d_with_default(&to_marker, global_left3d);
    cross_product3d(&to_marker, global_up3d, &rows[3]);
    normalize3d_with_default(&rows[3], global_down3d);

    for (long i = 0; i < 4; i++) {
        float weight = weights[i];
        lift_vector->i += rows[i].i * weight;
        lift_vector->j += rows[i].j * weight;
        lift_vector->k += rows[i].k * weight;
    }
    real_vector3d local_direction = *lift_vector;
    local_direction.k -= 0.3f;
    real_matrix3x3 inverse;
    inverse.up = havok->world_matrix.rotation.forward;
    scale_vector3d(&havok->world_matrix.rotation.up, -1.0f, &inverse.forward);
    inverse.left = havok->world_matrix.rotation.left;
    normalize3d_with_default(&local_direction, global_down3d);
    matrix3x3_inverse_transform_vector(&inverse, &local_direction, &local_direction);
    real_euler_angles2d desired;
    euler_angles2d_from_vector3d(&desired, &local_direction);
    desired.pitch = unit->unit.throttle_scale * desired.pitch;
    desired.yaw = unit->unit.throttle_scale * desired.yaw;
    interpolate_lift_angles(object_index, &angles->yaw, &velocity->i, &desired.yaw, false);
    interpolate_lift_angles(object_index, &angles2->yaw, &velocity2->i, &angles->yaw, true);

    float lift_weights[4];
    for (long i = 0; i < 4; i++) {
        if (i != 3) {
            float weight = weights[i];
            long sign;
            if (weight != 0.0f)
                sign = weight < 0.0f ? -1 : 1;
            else
                sign = 0;
            double sign_value = (double)sign;
            float sign_scale = (float)sign_value;
            const real_vector3d *row = &rows[i];
            lift_weights[i] = dot_product3d(lift_vector, row) * sign_scale;
            lift_weights[i] = MIN(MAX(lift_weights[i], 0.0f), 1.0f);
        } else {
            lift_weights[3] = 1.0f;
        }
    }

    *lift_vector = *global_zero_vector3d;
    for (long i = 0; i < 4; i++)
        point_from_line3d(lift_vector, &rows[i], lift_weights[i] * weights[i], lift_vector);
    float damping = unit->unit.throttle_scale;
    scale_vector3d(lift_vector, damping, lift_vector);

    float bounded = MAX(definition->turn_acceleration, MAX(definition->forward_acceleration, definition->lateral_acceleration));
    if (bounded > 0.001f) {
        *out_value = sqrtf(lift_vector->j * lift_vector->j + (lift_vector->i * lift_vector->i + lift_vector->k * lift_vector->k)) / bounded;
        *out_value = MIN(MAX(*out_value, 0.0f), 1.0f);
    }

    real_vector3d direction = *lift_vector;
    vehicle_type_vtol_normalize3d_with_default(&direction, global_down3d);
    real_point3d offset_position;
    point_from_line3d(&marker.matrix.position, &havok->world_matrix.rotation.up, -definition->marker_offset, &offset_position);
    point_from_line3d(&offset_position, &direction, -definition->marker_offset, position);
}

static __forceinline void vehicle_type_vtol_apply_thrust(
    const s_havok_vehicle_physics_instance *instance,
    const real_vector3d *lift, const real_point3d *position, float magnitude,
    real_vector3d *force, real_vector3d *torque)
{
    real_vector3d offset, thrust, moment;
    vector_from_points3d(&instance->center_of_mass, position, &offset);
    scale_vector3d(lift, -magnitude, &thrust);
    cross_product3d(&offset, &thrust, &moment);
    add_vectors3d(force, &thrust, force);
    add_vectors3d(torque, &moment, torque);
}


static __forceinline void vehicle_type_vtol_apply_upright_torque(
    const vehicle_type_vtol_definition *definition, const vehicle_datum *unit,
    const s_havok_vehicle_physics_instance *instance, float angle,
    const real_vector3d *axis, real_vector3d *torque)
{
    float magnitude = definition->upright_acceleration * unit->unit.throttle_scale *
        (float)pow(definition->upright_scale * angle, definition->upright_exponent) * instance->mass;
    point_from_line3d(torque,
        axis, magnitude, torque);
}

// @address 0x375b18 ?update_physics@c_vehicle_type_vtol@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_vtol::update_physics(long object_index, s_havok_vehicle_physics_instance *instance)
{
    vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(object_index, 2);
    const vehicle_type_vtol_definition *definition = (const vehicle_type_vtol_definition *)
        TAG_GET('vehi', vehicle_definition, unit->object.definition_index)->vtol.address;
    real_vector3d torque = *global_zero_vector3d;
    real_vector3d force = *global_zero_vector3d;
    float left_scale = 0.0f;
    float right_scale = 0.0f;
    real_vector3d throttle;
    compute_throttle(object_index, &throttle);

    if (!(fabs(throttle.k) < k_vehicle_type_vtol_normalization_epsilon)) {
        target_altitude = instance->center_of_mass.z + throttle.k * definition->active_altitude_range;
        target_altitude = MIN(MAX(target_altitude,
            instance->center_of_mass.z - definition->active_altitude_range),
            instance->center_of_mass.z + definition->active_altitude_range);
    } else {
        target_altitude = MIN(MAX(target_altitude,
            instance->center_of_mass.z - definition->altitude_range),
            instance->center_of_mass.z + definition->altitude_range);
    }

    float altitude_delta = target_altitude - instance->center_of_mass.z;
    real_vector3d linear_velocity, angular_velocity;
    object_get_velocities(object_index, &linear_velocity, &angular_velocity);
    float vertical_velocity = linear_velocity.k;
    float drag_scale = 0.0f;
    // match: 0x375b18: value range assumption guides branch lowering
    __assume(vertical_velocity == vertical_velocity &&
        linear_velocity.i == linear_velocity.i &&
        linear_velocity.j == linear_velocity.j);
    // match: 0x375b18: value range assumption guides branch lowering
    __assume(instance != 0);
    float acceleration = vehicle_type_vtol_compute_altitude_acceleration(vertical_velocity, -global_gravity_get(),
        definition->minimum_acceleration, definition->maximum_acceleration, altitude_delta);
    real_vector3d left_lift, right_lift;
    real_point3d left_position, right_position;
    update_lift_vector(object_index, instance, &throttle, acceleration, definition->left_marker,
        &left_lift, &left_position, &left_angles, &left_angles2, &left_velocity, &left_velocity2, &left_scale);
    update_lift_vector(object_index, instance, &throttle, acceleration, definition->right_marker,
        &right_lift, &right_position, &right_angles, &right_angles2, &right_velocity, &right_velocity2, &right_scale);

    vehicle_type_vtol_apply_thrust(instance, &left_lift, &left_position,
        unit->unit.throttle_scale * 0.5f * instance->mass, &force, &torque);
    vehicle_type_vtol_apply_thrust(instance, &right_lift, &right_position,
        unit->unit.throttle_scale * 0.5f * instance->mass, &force, &torque);
    const float *drag_coefficient = &definition->drag;
    for (long i = 0; i < instance->definition->anti_gravity_points.count; i++)
        instance->anti_gravity_points[i].strength = unit->unit.throttle_scale;

    if (definition->drag > 0.0f || definition->constant_drag > 0.0f) {
        real_vector3d direction = linear_velocity;
        float speed = normalize3d(&direction);
        float drag = *drag_coefficient * speed;
        drag *= drag;
        if (speed != 0.0f) {
            float constant_drag = tls_game_time_globals()->ticks_per_second * speed;
            float maximum_drag = definition->constant_drag - drag;
            if (constant_drag > maximum_drag)
                constant_drag = maximum_drag;
            constant_drag = MAX(constant_drag, 0.0f);
            real_vector3d drag_acceleration;
            drag_scale = -(constant_drag + drag);
            scale_vector3d(&linear_velocity, drag_scale, &drag_acceleration);
            point_from_line3d(&force,
                &drag_acceleration, instance->mass, &force);
        }
    }

    float angle = fabsf(angle_between_vectors3d(global_up3d, &instance->world_matrix.rotation.up));
    real_vector3d upright_axis;
    cross_product3d(&instance->world_matrix.rotation.up, global_up3d, &upright_axis);
    if (normalize3d(&upright_axis) != 0.0f) {
        vehicle_type_vtol_apply_upright_torque(definition, unit, instance, angle, &upright_axis, &torque);
    }
    havok_vehicle_physics_update(object_index, instance, 0.0f, false, &force, &torque, 0);

    long history_index = tls_game_time_globals()->time & 3;
    long left_history = (long)(left_scale * 255.0f);
    long right_history = (long)(right_scale * 255.0f);
    engine_a[history_index] = (unsigned char)MIN(MAX(left_history, 0), 255);
    engine_b[history_index] = (unsigned char)MIN(MAX(right_history, 0), 255);
    update_rotor_damping(object_index);

    float function_value;
    bool active;
    if (!object_get_function_value(object_index, 0x4a4, unit->object.definition_index, &function_value, &active))
        function_value = 0.0f;
    float lift_speed = definition->minimum_lift_speed +
        fabsf(function_value) * (definition->maximum_lift_speed - definition->minimum_lift_speed);
    float rotor_scale = 1.0f;
    if (object_get_function_value(object_index, 0x4a5, unit->object.definition_index, &rotor_scale, &active)) {
        float value = rotor_scale;
        float scale = MIN(MAX(value, 0.0f), 1.0f);
        if (!(fabsf(scale - 1.0f) < k_vehicle_type_vtol_normalization_epsilon)) {
            float minimum = definition->minimum_lift_speed * scale;
            lift_speed = (lift_speed - minimum) * scale + minimum;
        }
    }
    lift += game_tick_length() * lift_speed;
    lift = (float)fmod((double)lift, 1.0);
}
