// ailibxbox/actor_moving_vehicle_nondirectional_flying.obj: from the records 0xb8f80 0xbacc8 0xbae18
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "game/game_time_inline.h"
#include "math/real_math.h"
#include "memory/data.h"
#include <string.h>
#include <math.h>
#include "objects/object_header.h"
#include "cseries/cseries_wrapped.h"
#include "ai/actors.h"
#include "motor/motor_animation.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/ai_direction.h"
#include "ai/actor_datum.h"
#include "ai/ai_direction_mode_types.h"
#include "ai/actor_moving_flying_vehicle_obstacle_avoidance.h"
#include "ai/actor_moving_vehicle_nondirectional_flying.h"




#include "ai/actor_moving_types.h"

enum e_nondirectional_flying_movement_mode
{
    _nondirectional_flying_movement_mode_none = NONE,
    _nondirectional_flying_movement_mode_hold = 1,
    _nondirectional_flying_movement_mode_steer = 2,
    _nondirectional_flying_movement_mode_turn = 3
};



static __forceinline bool actor_moving_vehicle_flags_test(short flags, long bit)
{
    return (flags & (1 << bit)) != 0;
}

// @address 0xbacc8 ?stationary_steering_control@@YAXJJPATreal_vector3d@@00@Z
void stationary_steering_control(long actor_index, long object_index, real_vector3d *forward, real_vector3d *desired_direction, real_vector3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    real_vector3d direction;
    if (actor_moving_vehicle_flags_test(actor->destination_orders.flags.m_storage, 1))
    {
        memcpy(&direction, &actor->destination_orders.m_destination_facing, sizeof(direction));
    }
    else if ((actor->flags & 2) != 0)
    {
        memcpy(&direction, &actor->steering_vector, sizeof(direction));
    }
    else if (actor->look_mode < 3 || !actor->look_direction.get(actor_index, _ai_direction_mode_vector, 0, &direction, 0, 0))
    {
        if (desired_direction)
            direction = *desired_direction;
        else
            memcpy(&direction, &actor->facing_vector, sizeof(direction));
    }
    direction.k = 0.0f;
    if (normalize3d(&direction) == 0.0f)
        memcpy(&direction, &actor->facing_vector, sizeof(direction));
    steering_control(actor_index, object_index, &direction, 1.0f, forward, result);
}


// @address 0xbae18 ?steering_control@@YAXJJPATreal_vector3d@@M00@Z
void steering_control(long actor_index, long object_index, real_vector3d *input, float scale, real_vector3d *forward, real_vector3d *result)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 8195);
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->definition_index);
    real_vector3d direction = *input;
    direction.k = 0.0f;
    float direction_length = normalize3d(&direction);
    real_vector2d forward2d = { forward->i, forward->j };
    real_vector3d flat_forward = *forward;
    flat_forward.k = 0.0f;
    if (normalize3d(&flat_forward) == 0.0f)
        flat_forward = *forward;
    float yaw, pitch;
    if (direction_length > 0.0f && normalize2d(&forward2d) != 0.0f)
    {
        yaw = signed_angle_between_vectors2d(&forward2d, &direction.xy);
        if (properties->steering_angle_scale > 0.0f)
            yaw *= properties->steering_angle_scale;
        float input_pitch = angle_between_vectors3d(&direction, input);
        if (input->k < 0.0f)
            input_pitch = -input_pitch;
        float forward_pitch = angle_between_vectors3d(forward, &flat_forward);
        if (forward->k < 0.0f)
            forward_pitch = -forward_pitch;
        pitch = input_pitch - forward_pitch;
        float limit = properties->maximum_steering_angle > 0.0f ? properties->maximum_steering_angle * 0.017453292f : 1.5707964f;
        yaw = PIN(yaw, -limit, limit);
        pitch = PIN(pitch, -limit, limit);
        float yaw_delta = yaw - actor->previous_yaw;
        float yaw_limit = 6.2831855f;
        if (properties->maximum_steering_delta > 0.0f)
            yaw_limit = properties->maximum_steering_delta * 0.017453292f;
        if (actor->previous_yaw * yaw_delta < 0.0f)
            yaw_delta = PIN(yaw_delta, -yaw_limit, yaw_limit);
        else
        {
            float scaled_limit = yaw_limit * scale;
            yaw_delta = PIN(yaw_delta, -scaled_limit, scaled_limit);
        }
        yaw = actor->previous_yaw + yaw_delta;
        float pitch_delta = pitch - actor->previous_pitch;
        float pitch_limit = 6.2831855f;
        if (properties->maximum_steering_delta > 0.0f)
            pitch_limit = properties->maximum_steering_delta * 0.017453292f;
        if (actor->previous_pitch * pitch_delta < 0.0f)
            pitch_delta = PIN(pitch_delta, -pitch_limit, pitch_limit);
        else
        {
            float scaled_limit = pitch_limit * scale;
            pitch_delta = PIN(pitch_delta, -scaled_limit, scaled_limit);
        }
        pitch = actor->previous_pitch + pitch_delta;
        direction = *forward;
        float cosine = (float)cos(yaw);
        float sine = (float)sin(yaw);
        rotate_vector_about_axis(&direction, global_up3d, sine, cosine);
        real_vector3d axis;
        cross_product3d(&object->forward, &object->up, &axis);
        cosine = (float)cos(pitch);
        sine = (float)sin(pitch);
        rotate_vector_about_axis(&direction, &axis, sine, cosine);
    }
    else
    {
        yaw = 0.0f;
        pitch = 0.0f;
        direction = actor->facing_vector;
    }
    actor->yaw = yaw;
    actor->pitch = pitch;
    *result = direction;
}


extern "C" {
// actor_moving_vehicle_test_destination
// actor_moving_nondirectional_flying_vector_avoidance
}

// @rdata 0x82022dc0
static const float k_nondirectional_flying_maximum_vertical = 0.5f;

// match: 0xb8f80: game_seconds_to_ticks_round with the product in a named local; the canonical one-expression
// form reorders the frame slots of avoidance and direction
static __forceinline long nondirectional_flying_seconds_to_ticks_round(float seconds)
{
    float ticks = seconds * (float)game_tick_rate();
    return (long)(ticks + 0.5f);
}



static __forceinline float nondirectional_flying_cosine(float angle)
{
    return (float)cos(angle);
}

// @address 0xb8f80 ?actor_move_calculate_nondirectional_flying_movement@@YA?AW4e_movement_result@@JJPBTreal_vector3d@@PAT2@1F@Z
e_movement_result actor_move_calculate_nondirectional_flying_movement(long actor_index, long object_index, const real_vector3d *movement,
    real_vector3d *desired_facing, real_vector3d *desired_throttle, short iteration)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 8195);
    character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->definition_index);
    bool facing_destination = true;
    short mode = _nondirectional_flying_movement_mode_none;
    bool last_step = false;
    e_movement_result result = _movement_result_none;

    actor->flags &= ~FLAG(9);
    if (iteration == 0)
    {
        actor->previous_yaw = actor->yaw;
        actor->previous_pitch = actor->pitch;
        actor->previous_throttle = actor->vehicle_throttle;
    }
    if (actor->root_vehicle_index == NONE)
        actor->output_animation_state = actor_get_movement_type(actor_index);

    real_vector3d desired = actor->facing_vector;
    real_vector3d throttle = *global_zero_vector3d;
    real_vector3d avoidance = *global_zero_vector3d;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    real_vector3d direction = *movement;
    float distance = normalize3d(&direction);

    if (!properties)
    {
        actor->turning_in_place = false;
    }
    else if (!TEST_BIT(actor->flags, 0))
    {
        real_vector3d linear_velocity;
        object_get_velocities(object_index, &linear_velocity, NULL);
        actor->turning_in_place = false;
        actor->holding_position = true;
        if (magnitude_squared3d(&linear_velocity) < 0.4f * 0.4f)
        {
            float hover_throttle = 0.0f;
            float hover_urgency = 0.0f;
            real_vector3d hover_avoidance = *global_zero_vector3d;
            real_vector3d hover_direction = *global_down3d;
            actor_moving_flying_vehicle_compute_avoidance(
                actor_index, object_index, &hover_direction, &hover_direction, &hover_avoidance, &hover_throttle, &hover_urgency, false);

            long time = game_time_get();
            float phase_i = (float)(time % nondirectional_flying_seconds_to_ticks_round(7.0f)) / (float)nondirectional_flying_seconds_to_ticks_round(7.0f);
            float phase_j = (float)(time % nondirectional_flying_seconds_to_ticks_round(3.0f)) / (float)nondirectional_flying_seconds_to_ticks_round(3.0f);
            float clamped_urgency = PIN(hover_urgency, 0.0f, 0.9f);
            float hover_radius = (1.0f - clamped_urgency) * 2.0f;
            if (!actor->movement_target_valid)
            {
                actor->movement_target_valid = true;
                actor->movement_target = origin;
            }
            real_vector3d to_hover_point;
            vector_from_points3d(&origin, &actor->movement_target, &to_hover_point);
            float hover_distance = normalize3d(&to_hover_point);
            hover_distance = PIN(hover_distance, 0.0f, hover_radius);

            real_vector3d drift;
            drift.i = nondirectional_flying_cosine(phase_i * k_two_pi);
            drift.j = nondirectional_flying_cosine(phase_j * k_two_pi);
            drift.k = to_hover_point.k;
            normalize3d(&drift);
            scale_vector3d(&drift, 1.0f - hover_distance / hover_radius, &drift);
            scale_vector3d(&to_hover_point, hover_distance / hover_radius, &to_hover_point);
            add_vectors3d(&to_hover_point, &drift, &drift);
            normalize3d(&drift);
            actor_move_calculate_free(actor_index, true, &drift, &desired, &throttle);
            scale_vector3d(&throttle, 0.2f, &throttle);
            if (throttle.k < 0.2f)
                throttle.k = 0.0f;
        }
        stationary_steering_control(actor_index, object_index, &actor->facing_vector, NULL, &desired);
    }
    else
    {
        actor->movement_target_valid = false;
        bool next_step_valid;
        real_vector3d next_step_direction;
        if (actor_moving_has_vehicle_reached_step(actor_index, object_index, &origin, distance, true, &last_step, &next_step_valid, &next_step_direction))
        {
            actor->turning_in_place = false;
            actor->holding_position = true;
            result = _movement_result_advance;
            stationary_steering_control(actor_index, object_index, &actor->facing_vector, NULL, &desired);
        }
        else
        {
            actor_find_pathfinding_location(actor_index);

            real_point3d target;
            point_from_line3d(&origin, movement, 1.0f, &target);
            real_vector2d movement2d = movement->xy;
            float movement_length = normalize2d(&movement2d);
            real_vector2d facing2d = actor->facing_vector.xy;
            normalize2d(&facing2d);
            real_vector2d right2d = { -facing2d.j, facing2d.i };
            float half_deceleration_distance = properties->deceleration_distance / 2.0;
            float radius = properties->pathfinding_radius <= 0.0f ? object->bounding_radius : properties->pathfinding_radius;
            float throttle_scale = 1.0f;
            float avoidance_urgency;

            if (actor->turning_in_place)
            {
                if (dot_product2d(&movement2d, &facing2d) < 0.99 ||
                    !actor->destination_orders.flags.test(_destination_flag_2) && movement_length < half_deceleration_distance)
                {
                    facing_destination = false;
                }
                else
                {
                    facing_destination = true;
                }
            }
            else
            {
                bool right_side = dot_product2d(&right2d, &movement->xy) > 0.0f;
                float turning_radius = properties->turning_radius;
                real_point2d center;
                if (right_side)
                    point_from_line2d(&origin.xy, &right2d, turning_radius, &center);
                else
                    point_from_line2d(&origin.xy, &right2d, -turning_radius, &center);
                if (distance2d(&center, &target.xy) < turning_radius &&
                    (actor->movement_mode != _nondirectional_flying_movement_mode_steer || !actor->destination_orders.flags.test(_destination_flag_2)))
                {
                    facing_destination = false;
                }
                else if (!actor->destination_orders.flags.test(_destination_flag_2) && movement_length < half_deceleration_distance)
                {
                    facing_destination = false;
                }
                else if (dot_product2d(&facing2d, &movement2d) > nondirectional_flying_cosine(properties->destination_behind_angle))
                {
                    facing_destination = true;
                }
                else
                {
                    facing_destination = false;
                }
            }

            desired = *movement;
            if (movement_length < properties->side_slip_distance || !facing_destination ||
                actor->destination_orders.flags.test(_destination_flag_1) || TEST_BIT(actor->flags, 1))
            {
                mode = _nondirectional_flying_movement_mode_turn;
                actor->holding_position = false;
                actor->turning_in_place = true;
                if (movement_length < radius)
                    desired = actor->facing_vector;
                actor_moving_flying_vehicle_compute_avoidance(
                    actor_index, object_index, &desired, movement, &avoidance, &throttle_scale, &avoidance_urgency, false);
            }
            else
            {
                bool avoided = actor_moving_flying_vehicle_compute_avoidance(
                    actor_index, object_index, &desired, movement, &avoidance, &throttle_scale, &avoidance_urgency, true);
                if (!avoided)
                {
                    mode = _nondirectional_flying_movement_mode_turn;
                    actor->holding_position = false;
                    actor->turning_in_place = true;
                }
                else
                {
                    mode = _nondirectional_flying_movement_mode_steer;
                    actor->holding_position = false;
                    if (!actor->destination_orders.flags.test(_destination_flag_2) && movement_length < properties->deceleration_distance)
                    {
                        desired.k *= 1.0f -
                            MIN((properties->deceleration_distance - movement_length) /
                                    (properties->deceleration_distance - half_deceleration_distance),
                                1.0f);
                    }
                    if (normalize3d(&desired) > 0.0f && fabs(desired.k) > k_nondirectional_flying_maximum_vertical)
                    {
                        float horizontal_scale = square_root(1.0f - k_nondirectional_flying_maximum_vertical * k_nondirectional_flying_maximum_vertical);
                        if (normalize2d(&desired.xy) > 0.0f)
                        {
                            scale_vector2d(&desired.xy, horizontal_scale, &desired.xy);
                            desired.k = PIN(desired.k, -k_nondirectional_flying_maximum_vertical, k_nondirectional_flying_maximum_vertical);
                        }
                    }
                }
                if (normalize3d(&desired) == 0.0f)
                    desired = actor->facing_vector;
            }

            float urgency = 0.0f;
            if (mode == _nondirectional_flying_movement_mode_steer)
            {
                if (actor->urgency_override != 0.0f)
                {
                    urgency = actor->urgency_override;
                }
                else
                {
                    float movement_length_squared = movement_length * movement_length;
                    float deceleration_scale = 0.5f *
                        (((properties->deceleration_distance / 2.0f) * (properties->deceleration_distance / 2.0f)) /
                            MAX(movement_length_squared, 0.001f));
                    float facing_scale = 0.5f * (1.0f - MAX(dot_product2d(&movement2d, &facing2d), 0.0f));
                    urgency = 4.0f * deceleration_scale * facing_scale;
                }
                urgency = PIN(urgency, properties->minimum_urgency, 1.0f);
                urgency = MAX(urgency, avoidance_urgency);
            }
            else if (mode == _nondirectional_flying_movement_mode_hold)
            {
                urgency = 1.0f;
            }

            real_vector3d steering_direction = desired;
            if (mode == _nondirectional_flying_movement_mode_turn)
                stationary_steering_control(actor_index, object_index, &actor->facing_vector, &desired, &desired);
            else
                steering_control(actor_index, object_index, &desired, urgency, &actor->facing_vector, &desired);
            actor->urgency = urgency;

            if (mode == _nondirectional_flying_movement_mode_hold)
            {
                actor->turning_in_place = true;
            }
            else
            {
                float speed = properties->throttle_maximum;
                real_vector2d steering_direction2d = steering_direction.xy;
                normalize2d(&steering_direction2d);
                speed *= actor_moving_vehicle_desired_speed(actor_index, object_index, &origin.xy, &movement2d,
                    &steering_direction2d, &facing2d, NULL, distance, mode != _nondirectional_flying_movement_mode_steer);
                actor_move_calculate_free(actor_index, true, &direction, &desired, &throttle);
                if (mode == _nondirectional_flying_movement_mode_steer)
                {
                    actor->turning_in_place = false;
                    if (!TEST_BIT(properties->flags, 2))
                    {
                        throttle.i = 1.0f;
                        throttle.j = 0.0f;
                        if (movement_length < properties->deceleration_distance)
                            throttle.k *= MIN((properties->deceleration_distance - movement_length) /
                                    (properties->deceleration_distance - half_deceleration_distance),
                                1.0f);
                        else
                            throttle.k = 0.0f;
                    }
                }
                speed = speed > throttle_scale ? throttle_scale : speed;
                scale_vector3d(&throttle, speed, &throttle);

                if (properties->theoretical_maximum_speed > 0.0f)
                {
                    real_vector3d forward, left, up;
                    real_vector3d_build_axes_from_vectors(&desired, global_up3d, &forward, &left, &up);
                    real_vector3d speed_error;
                    speed_error.i = dot_product3d(&object->linear_velocity, &desired);
                    speed_error.j = dot_product3d(&object->linear_velocity, &left);
                    speed_error.k = dot_product3d(&object->linear_velocity, &up);
                    speed_error.i = throttle.i - speed_error.i / properties->theoretical_maximum_speed;
                    speed_error.j = throttle.j - speed_error.j / properties->theoretical_maximum_speed;
                    speed_error.k = throttle.k - speed_error.k / properties->theoretical_maximum_speed;
                    throttle.i += square_root(fabsf(speed_error.i)) * (float)(speed_error.i < 0.0f ? -1 : 1) * properties->error_scale;
                    throttle.j += square_root(fabsf(speed_error.j)) * (float)(speed_error.j < 0.0f ? -1 : 1) * properties->error_scale;
                    throttle.k += square_root(fabsf(speed_error.k)) * (float)(speed_error.k < 0.0f ? -1 : 1) * properties->error_scale;
                }

                real_vector3d previous_throttle;
                scale_vector3d(&actor->previous_throttle, properties->throttle_blend, &previous_throttle);
                add_vectors3d(&throttle, &avoidance, &throttle);
                scale_vector3d(&throttle, 1.0f - properties->throttle_blend, &throttle);
                add_vectors3d(&throttle, &previous_throttle, &throttle);
                throttle.i = PIN(throttle.i, -1.0f, 1.0f);
                throttle.j = PIN(throttle.j, -1.0f, 1.0f);
                throttle.k = PIN(throttle.k, -1.0f, 1.0f);
            }

            if (actor->boost_allowed && TEST_BIT(actor->flags, 0) && !motor_animation_busy(object_index) && distance > 0.0f &&
                (mode == _nondirectional_flying_movement_mode_steer || mode == _nondirectional_flying_movement_mode_turn))
            {
                real_vector3d unit_velocity;
                object_get_velocities(actor->unit_index, &unit_velocity, NULL);
                if (normalize3d(&unit_velocity) > 0.0f && dot_product3d(&direction, &unit_velocity) > 0.85f)
                    actor_unit_control_boost(actor_index, true);
            }
            actor->movement_mode = mode;
        }
    }

    if (result == _movement_result_advance && !last_step && actor->destination_orders.type == 3 && actor->destination_orders.flags.test(_destination_flag_5))
        actor->destination_orders.flags.set(_destination_flag_4, true);
    actor->crouch = false;
    actor->moving_crouch = false;
    actor->vehicle_throttle = throttle;
    *desired_facing = desired;
    *desired_throttle = throttle;
    return result;
}
