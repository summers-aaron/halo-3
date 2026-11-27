// objectslibxbox/vehicle_type_mantis.obj: from the records 0x3743c0 0x374528 0x3749f0 0x374a88 0x374eb0
// @flags /TP /O2
#include "objects/objects.h"
#include "physics/physics_variables.h"
#include "units/vehicle_utilities.h"
#include "physics/havok_component.h"
#include "objects/object_datum.h"
// @address 0x3749f0 ?calculate_throttle@c_vehicle_type_mantis@@AAAXJPATreal_vector3d@@@Z
#include <math.h>
#include "math/matrix_math.h"
#include "math/real_math_limit.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"

#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "physics/walker_physics.h"
#include "math/real_math_trigonometry.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/walker_physics_definitions.h"
#include "units/vehicle_mantis_definitions.h"

#include "units/vehicle_type_mantis.h"



#include "math/rotate_vector2d.h"
#include "physics/physics_variable_types.h"
#include "physics/havok_component_class.h"
#include "math/integer_math_templates.h"

void c_vehicle_type_mantis::calculate_throttle(long vehicle_index, real_vector3d *throttle)
{
  vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
  real_vector2d planar_throttle;
  set_real_vector2d(&planar_throttle, vehicle->unit.throttle.i, vehicle->unit.throttle.j);
  rotate_vector2d(&planar_throttle, sine(m_facing_angle), cosine(m_facing_angle), &planar_throttle);
  set_real_vector3d(throttle, planar_throttle.i, planar_throttle.j, vehicle->unit.throttle.k);
}

__forceinline s_vehicle_mantis_definition *c_vehicle_type_mantis::get_type_definition(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    return (s_vehicle_mantis_definition *)((const vehicle_definition *)tag_get(0, vehicle->object.definition_index))->mantis.address;
}
static __forceinline float mantis_tick_length()
{ return tls_game_time_globals()->tick_length; }
static __forceinline long mantis_sign(float value)
{
    return value == 0.0f ? 0 : (value < 0.0f ? -1 : 1);
}
// @address 0x3743c0 ?update_control@c_vehicle_type_mantis@@QAAXJ@Z
void c_vehicle_type_mantis::update_control(long vehicle_index)
{
    s_vehicle_mantis_definition *definition = get_type_definition(vehicle_index);
    real_matrix4x3 matrix;
    if (!c_walker_physics::calculate_foot_group_center_matrix(&definition->walker_physics, vehicle_index, m_foot_group, &matrix))
        object_get_world_matrix(vehicle_index, &matrix);
    real_vector3d throttle;
    calculate_throttle(vehicle_index, &throttle);
    m_facing_angle = vehicle_update_control_classic(vehicle_index, &matrix.forward, &matrix.up, throttle.i, throttle.j,
        &definition->steering_control, NULL, &definition->forward_speed, &definition->lateral_speed,
        false, false, NULL, &m_forward_throttle, &m_lateral_throttle);
    if (definition->walker_physics.maximum_turn > 0.001f)
    {
        float facing_angle = m_facing_angle;
        long direction = mantis_sign(facing_angle);
        facing_angle -= (float)fmod(fabs(facing_angle), definition->walker_physics.maximum_turn) * direction;
        physics_variable_position_update_seek(&m_turn_position, &definition->turning_control.turn_bounds, false,
            facing_angle, definition->turning_control.turn_rate, mantis_tick_length());
    }
}
// @address 0x374eb0 ?update_movement_leg_turn_and_displacment@c_vehicle_type_mantis@@AAAXJPAMPATreal_vector3d@@@Z
void c_vehicle_type_mantis::update_movement_leg_turn_and_displacment(long vehicle_index,
    // match: 0x374eb0: pointer __restrict qualifier prevents alias reload
    float *__restrict final_turn, real_vector3d *__restrict final_leg_displacement)
{
    float zero = 0.0f;
    s_vehicle_mantis_definition *definition = get_type_definition(vehicle_index);
    float tick_length = mantis_tick_length();
    unsigned char forward_overtravel = (!(m_forward_position < zero) ? m_forward_position : -m_forward_position) > definition->walker_physics.overtravel + 1.0f;
    unsigned char lateral_overtravel = (!(m_lateral_position < zero) ? m_lateral_position : -m_lateral_position) > definition->walker_physics.overtravel + 1.0f;
    unsigned char turn_overtravel = (!(m_turn_position < zero) ? m_turn_position : -m_turn_position) >
        (definition->walker_physics.overtravel + 1.0f) * definition->walker_physics.maximum_turn;
    if (forward_overtravel || lateral_overtravel || turn_overtravel)
    {
        m_foot_group = m_foot_group != 1;
        m_forward_position = PIN(m_forward_position, -1.0f, 1.0f) * -1.0f;
        m_lateral_position = PIN(m_lateral_position, -1.0f, 1.0f) * -1.0f;
        m_turn_position = PIN(m_turn_position, -definition->walker_physics.maximum_turn, definition->walker_physics.maximum_turn) * -1.0f;
    }
    real_vector3d throttle;
    calculate_throttle(vehicle_index, &throttle);
    if ((!(throttle.i < 0.0f) ? throttle.i : -throttle.i) < 0.25f)
        m_forward_position *= 0.9f;
    if ((!(throttle.j < 0.0f) ? throttle.j : -throttle.j) < 0.25f)
        m_lateral_position *= 0.9f;
    if (m_forward_throttle > 0.0f && definition->forward_speed.positive_limit > 0.001f)
    { m_forward_position += m_forward_throttle * tick_length / (definition->walker_physics.forward_stride * 0.5f); }
    else if (m_forward_throttle < 0.0f && definition->forward_speed.negative_limit > 0.001f)
    { m_forward_position += m_forward_throttle * tick_length / (definition->walker_physics.forward_stride * 0.5f); }
    if (m_lateral_throttle > 0.0f && definition->lateral_speed.positive_limit > 0.001f)
    { m_lateral_position += m_lateral_throttle * tick_length / (definition->walker_physics.lateral_stride * 0.5f); }
    else if (m_lateral_throttle < 0.0f && definition->lateral_speed.negative_limit > 0.001f)
    { m_lateral_position += m_lateral_throttle * tick_length / (definition->walker_physics.lateral_stride * 0.5f); }
    float turn = PIN(m_turn_position, -definition->walker_physics.maximum_turn, definition->walker_physics.maximum_turn);
    float normalized_turn = definition->walker_physics.maximum_turn > 0.001f ? turn / definition->walker_physics.maximum_turn : 0.0f;
    real_vector2d leg_displacement;
    set_real_vector2d(&leg_displacement, PIN(m_forward_position, -1.0f, 1.0f), PIN(m_lateral_position, -1.0f, 1.0f));
    *final_turn = turn;
    final_leg_displacement->i = leg_displacement.i * definition->walker_physics.forward_stride;
    final_leg_displacement->j = leg_displacement.j * definition->walker_physics.lateral_stride;
    float phase;
    if ((!(leg_displacement.i < 0.0f) ? leg_displacement.i : -leg_displacement.i)
        >= (!(leg_displacement.j < 0.0f) ? leg_displacement.j : -leg_displacement.j) &&
        (!(leg_displacement.i < 0.0f) ? leg_displacement.i : -leg_displacement.i) >= (!(normalized_turn < 0.0f) ? normalized_turn : -normalized_turn))
        phase = leg_displacement.i * 0.5f;
    else if ((!(leg_displacement.j < 0.0f) ? leg_displacement.j : -leg_displacement.j) >= (!(normalized_turn < 0.0f) ? normalized_turn : -normalized_turn))
        phase = leg_displacement.j * 0.5f;
    else
        phase = normalized_turn * 0.5f;
    float cycle = (float)fmod(PIN(phase, -0.5f, 0.5f) + 0.5f, 1.0f);
    float cosine_value = cosine(cycle * k_two_pi);
    float lift_fraction = cosine_value * -0.5f + 0.5f;
    final_leg_displacement->k = lift_fraction * definition->walker_physics.leg_lift;
}
static __forceinline long mantis_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
}
static __forceinline long mantis_game_time_get()
{
    return tls_game_time_globals()->time;
}
// @address 0x374a88 ?update_leg_turn_and_displacment@c_vehicle_type_mantis@@AAAXJPAMPATreal_vector3d@@1PA_N@Z
void c_vehicle_type_mantis::update_leg_turn_and_displacment(long vehicle_index, float * turn,
    // match: 0x374a88: pointer __restrict qualifier prevents alias reload
    real_vector3d * movement, real_vector3d *__restrict jump_displacement, bool *__restrict jumping)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    s_vehicle_mantis_definition *definition = get_type_definition(vehicle_index);
    bool jump_requested = ((vehicle->unit.control_flags & 1) != 0) || vehicle->unit.seat_acceleration_fractions[0] == 0.0f;
    const long crouch_ticks = mantis_seconds_to_ticks_round(definition->jump_crouch_time);
    const long crouch_interpolate_ticks = mantis_seconds_to_ticks_round(definition->jump_crouch_time * definition->jump_crouch_interpolate_fraction);
    const long leap_ticks = mantis_seconds_to_ticks_round(definition->jump_leap_time);
    const long recovery_ticks = mantis_seconds_to_ticks_round(definition->jump_recovery_time);
    const long recovery_interpolate_ticks = mantis_seconds_to_ticks_round(definition->jump_recovery_time * definition->jump_recovery_interpolate_fraction);
    bool override_displacement = false;
    *jumping = override_displacement;
    bool success = override_displacement;
    update_movement_leg_turn_and_displacment(vehicle_index, turn, movement);
    *jump_displacement = *global_zero_vector3d;
    if (jump_requested && (m_jump_start_time == -1 || mantis_game_time_get() - m_jump_start_time >= crouch_ticks))
    {
        if (m_jump_start_time != -1 && mantis_game_time_get() - m_jump_start_time == crouch_ticks)
            ++m_jump_start_time;
        else if (m_jump_start_time == -1 || mantis_game_time_get() - m_jump_start_time > crouch_ticks + leap_ticks + recovery_ticks)
            m_jump_start_time = mantis_game_time_get();
    }
    real_vector3d offset;
    if (m_jump_start_time != -1)
    {
        float frame = (float)(mantis_game_time_get() - m_jump_start_time);
        if (frame >= 0.0f && frame <= (float)crouch_ticks)
        {
            if (frame >= 0.0f && frame <= (float)crouch_interpolate_ticks)
            {
                success = true;
                offset = *global_zero_vector3d;
            }
            else
            {
                success = true;
                float fraction = (frame - (float)crouch_interpolate_ticks) / (float)(crouch_ticks - crouch_interpolate_ticks);
                point_from_line3d(global_zero_vector3d, global_up3d, fraction * definition->jump_crouch_depth, &offset);
            }
        }
        else if (frame >= (float)crouch_ticks && frame <= (float)(crouch_ticks + leap_ticks - 1))
        {
            success = true;
            float fraction = (frame - (float)crouch_ticks) / (float)leap_ticks;
            scale_vector3d(global_up3d, definition->jump_crouch_depth, &offset);
            point_from_line3d(&offset, global_down3d, fraction * (definition->jump_crouch_depth + definition->jump_leap_height), &offset);
        }
        else if (frame >= (float)(crouch_ticks + leap_ticks) && frame <= (float)(crouch_ticks + leap_ticks + recovery_ticks - 1))
        {
            if (frame >= (float)(crouch_ticks + leap_ticks) && frame <= (float)(crouch_ticks + leap_ticks + (recovery_ticks - recovery_interpolate_ticks) - 1))
            {
                success = true;
                float fraction = (frame - (float)(crouch_ticks + leap_ticks)) / (float)(recovery_ticks - recovery_interpolate_ticks);
                point_from_line3d(global_zero_vector3d, global_down3d, (1.0f - fraction) * definition->jump_leap_height, &offset);
            }
            else
            {
                success = true;
                offset = *global_zero_vector3d;
            }
        }
    }
    if (success)
        *jump_displacement = offset;
}

static __forceinline float mantis_absolute(float value)
{
    return value < 0.0f ? -value : value;
}
static __forceinline long mantis_game_seconds_to_ticks_round(float seconds)
{ return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + 0.5f); }
// @address 0x374528 ?update_physics@c_vehicle_type_mantis@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_mantis::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *)
{
    s_vehicle_mantis_definition *definition = get_type_definition(vehicle_index);
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    c_havok_component *havok = &((c_havok_component *)g_havok_component_data->data)[(unsigned short)vehicle->object.havok_component_index];
    long controlling_body = havok->m_object_postion_controlling_rigid_body_index;
    long rigid_body_index = (short)((controlling_body >= 0 && controlling_body < havok->get_rigid_body_count()) ? controlling_body : -1);
    if (vehicle->unit.seat_acceleration_fractions[0] != 0.0f)
        object_set_at_rest(vehicle_index, false);
    float turn;
    real_vector3d displacement;
    real_vector3d jump_displacement;
    bool jumping;
    update_leg_turn_and_displacment(vehicle_index, &turn, &displacement, &jump_displacement, &jumping);
    real_matrix4x3 foot_matrix;
    if (!c_walker_physics::calculate_foot_group_center_matrix(&definition->walker_physics, vehicle_index, m_foot_group, &foot_matrix))
        return;
    long support_group = m_foot_group == 0;
    m_foot_up = foot_matrix.up;
    float support_turn = turn * 0.5f;
    float stepping_turn = turn * -0.5f;
    real_vector3d support_displacement;
    scale_vector3d(&displacement, 0.5f, &support_displacement);
    real_vector3d stepping_displacement;
    scale_vector3d(&displacement, jumping ? 0.5f : -0.5f, &stepping_displacement);
    if (!jumping)
        stepping_displacement.k = 0.0f;
    long support_stuck_legs = c_walker_physics::change_leg_group_stuck_state(&definition->walker_physics, vehicle_index, support_group, false);
    long stepping_stuck_legs = c_walker_physics::change_leg_group_stuck_state(&definition->walker_physics, vehicle_index, m_foot_group, true);
    add_vectors3d(&support_displacement, &jump_displacement, &support_displacement);
    add_vectors3d(&stepping_displacement, &jump_displacement, &stepping_displacement);
    c_walker_physics::keyframe_leg_group(&definition->walker_physics, &foot_matrix.rotation, vehicle_index, support_group, m_foot_group,
        &support_stuck_legs, &support_displacement, support_turn);
    c_walker_physics::keyframe_leg_group(&definition->walker_physics, &foot_matrix.rotation, vehicle_index, m_foot_group, m_foot_group,
        &stepping_stuck_legs, &stepping_displacement, stepping_turn);
    signed char stabilization_ticks = (signed char)mantis_game_seconds_to_ticks_round(definition->stabilization_time);
    m_stabilization_ticks = count_bits<unsigned long>(stepping_stuck_legs) >= definition->minimum_stuck_leg_count
        ? stabilization_ticks
        : MAX(m_stabilization_ticks - 1, 0);
    if (rigid_body_index == -1 || m_stabilization_ticks <= 0)
        return;
    float tick_length = mantis_tick_length();
    c_havok_component *havok_component = (c_havok_component *)havok;
    real_vector3d linear_velocity;
    havok_component->get_rigid_body_linear_velocity(rigid_body_index, &linear_velocity);
    real_vector3d angular_velocity;
    havok_component->get_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
    real_vector3d local_linear_velocity;
    matrix4x3_inverse_transform_vector(&foot_matrix, &linear_velocity, &local_linear_velocity);
    real_vector3d local_angular_velocity;
    matrix4x3_inverse_transform_vector(&foot_matrix, &angular_velocity, &local_angular_velocity);
    real_vector3d desired_linear_velocity;
    set_real_vector3d(&desired_linear_velocity, m_forward_throttle * definition->forward_correction_scale,
        m_lateral_throttle * definition->lateral_correction_scale, 0.0f);
    real_vector3d desired_angular_velocity;
    scale_vector3d(global_up3d, m_facing_angle, &desired_angular_velocity);
    real_vector3d linear_correction;
    subtract_vectors3d(&desired_linear_velocity, &local_linear_velocity, &linear_correction);
    scale_vector3d(&linear_correction, 1.0f / tick_length, &linear_correction);
    real_vector3d angular_correction;
    subtract_vectors3d(&desired_angular_velocity, &local_angular_velocity, &angular_correction);
    scale_vector3d(&angular_correction, 1.0f / tick_length, &angular_correction);
    linear_correction.i = PIN(linear_correction.i, -definition->maximum_forward_correction, definition->maximum_forward_correction);
    linear_correction.j = PIN(linear_correction.j, -definition->maximum_lateral_correction, definition->maximum_lateral_correction);
    linear_correction.k = 0.0f;
    scale_vector3d(&angular_correction, definition->angular_correction_scale, &angular_correction);
    limit3d(&angular_correction, definition->maximum_angular_correction);
    matrix4x3_transform_vector(&foot_matrix, &linear_correction, &linear_correction);
    matrix4x3_transform_vector(&foot_matrix, &angular_correction, &angular_correction);
    point_from_line3d(&linear_velocity, &linear_correction, tick_length, &linear_velocity);
    point_from_line3d(&angular_velocity, &angular_correction, tick_length, &angular_velocity);
    havok_component->set_rigid_body_linear_velocity(rigid_body_index, &linear_velocity);
    havok_component->set_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
}
