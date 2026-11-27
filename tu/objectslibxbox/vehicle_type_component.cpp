// objectslibxbox/vehicle_type_component.obj: from the records 0xdc348 0xdc5b8 0xdc638 0xf0bd0 0xf12a0 0x195998 0x195c50 0x3535b8 0x3538e0 0x353ac8 0x371320 0xf1b88
// @flags /TP /O2
#include "objects/object_types.h"
#include "models/render_model_definitions.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "objects/object_datum.h"
#include "units/vehicle_types.h"
#include "cseries/cseries.h"
// @address 0xf12a0 ?is_e_braking@c_vehicle_type_component@@QAA_NJ@Z
// @address 0x195998 ?should_override_deactivation@c_vehicle_type_component@@QAA_NJ@Z
// @address 0x353ac8 vehicle_get_auto_control
// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
// @address 0x366800 ?vehicle_compute_throttle1d@@YAMJ@Z
// @address 0x3665f0 ?vehicle_scale_gravity_for_friction_point_vehicle@@YAXJPBTreal_vector3d@@PAT1@M@Z
#include "math/real_math.h"
#include "objects/objects.h"
#include "units/vehicle_type_component.h"
#include "units/vehicle_auto_control.h"
#include "units/vehicle_utilities.h"
#include "animations/animation_manager.h"
#include "units/vehicles.h"
#include "units/vehicle_datum.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "math/real_math_types.h"
#include "physics/havok_vehicle_physics.h"
#include "units/vehicle_type_component_internals.h"

namespace {
static __forceinline vehicle_datum *vehicle_type_component_get_object(long object_index)
{
    return (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
}
static __forceinline float vehicle_type_component_magnitude_squared(const real_vector2d *a)
{
    return a->i * a->i + a->j * a->j;
}
static __forceinline float vehicle_type_component_velocity_squared(long object_index)
{
    vehicle_datum *object = vehicle_type_component_get_object(object_index);
    real_vector2d velocity;
    float speed = object->unit.throttle_scale;
    velocity.i = object->unit.throttle.i;
    velocity.j = object->unit.throttle.j;
    velocity.i = velocity.i * speed;
    velocity.j = velocity.j * speed;
    return vehicle_type_component_magnitude_squared(&velocity);
}
}

__forceinline bool c_vehicle_type_human_tank::kills_riders_at_terminal_velocity(long object_index) { return true; }
__forceinline bool c_vehicle_type_human_tank::should_override_deactivation(long object_index)
{
    return (unsigned char)m_engine.gear != 255 || vehicle_type_component_velocity_squared(object_index) != 0.0f;
}
__forceinline bool c_vehicle_type_human_jeep::kills_riders_at_terminal_velocity(long object_index) { return true; }
__forceinline bool c_vehicle_type_human_jeep::should_override_deactivation(long object_index)
{
    return (unsigned char)engine.gear != 255 || vehicle_compute_throttle1d(object_index) != 0.0f;
}
__forceinline bool c_vehicle_type_human_plane::kills_riders_at_terminal_velocity(long object_index) { return false; }
__forceinline bool c_vehicle_type_human_plane::should_override_deactivation(long object_index)
{
    return m_forward_speed != 0.0f || m_left_speed != 0.0f || m_vertical_speed != 0.0f
        || vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_alien_scout::kills_riders_at_terminal_velocity(long object_index) { return true; }
__forceinline bool c_vehicle_type_alien_scout::should_override_deactivation(long object_index)
{
    return m_a != 0.0f || vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_alien_fighter::kills_riders_at_terminal_velocity(long object_index) { return false; }
__forceinline bool c_vehicle_type_alien_fighter::should_override_deactivation(long object_index)
{
    return forward != 0.0f || vertical != 0.0f || vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_mantis::compute_function_value(long vehicle_index, long function, float *value, bool *active) { return false; }
__forceinline bool c_vehicle_type_mantis::kills_riders_at_terminal_velocity(long object_index) { return true; }
__forceinline bool c_vehicle_type_mantis::should_override_deactivation(long object_index)
{
    return m_forward_throttle != 0.0f || m_lateral_throttle != 0.0f || m_turn_position != 0.0f
        || vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_guardian::compute_function_value(long vehicle_index, long function, float *value, bool *active) { return false; }
__forceinline bool c_vehicle_type_guardian::kills_riders_at_terminal_velocity(long object_index) { return false; }
__forceinline bool c_vehicle_type_guardian::should_override_deactivation(long object_index)
{
    return m_hover != 0.0f || vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_vtol::kills_riders_at_terminal_velocity(long object_index) { return false; }
__forceinline bool c_vehicle_type_vtol::should_override_deactivation(long object_index)
{
    return vehicle_type_component_get_object(object_index)->unit.throttle_scale != 0.0f;
}
__forceinline bool c_vehicle_type_chopper::kills_riders_at_terminal_velocity(long object_index) { return true; }
__forceinline bool c_vehicle_type_chopper::should_override_deactivation(long object_index)
{
    return (unsigned char)engine.gear != 255 || vehicle_compute_throttle1d(object_index) != 0.0f;
}

// @address 0xdc5b8 ?kills_riders_at_terminal_velocity@c_vehicle_type_component@@QAA_NJ@Z
bool c_vehicle_type_component::kills_riders_at_terminal_velocity(long object_index)
{
    bool kills_riders = false;
    e_vehicle_type type = vehicle_get_type(object_index);
    switch (type) {
    case 0:
        kills_riders = get_type_human_tank()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 1:
        kills_riders = get_type_human_jeep()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 2:
        kills_riders = get_type_human_plane()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 3:
        kills_riders = get_type_alien_scout()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 4:
        kills_riders = get_type_alien_fighter()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 5:
        kills_riders = get_type_turret()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 6:
        kills_riders = get_type_mantis()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 9:
        kills_riders = get_type_guardian()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 7:
        kills_riders = get_type_vtol()->kills_riders_at_terminal_velocity(object_index);
        break;
    case 8:
        kills_riders = get_type_chopper()->kills_riders_at_terminal_velocity(object_index);
        break;
    default:
        // match: 0xdc5b8: unreachable hint omits dead branch check
        __assume(0);
    }
    return kills_riders;
}

// @address 0xf1b88 ?update_physics@c_vehicle_type_component@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_component::update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance)
{
    e_vehicle_type type = vehicle_get_type(vehicle_index);
    switch (type) {
    case 0:
        get_type_human_tank()->update_physics(vehicle_index, instance);
        break;
    case 1:
        get_type_human_jeep()->update_physics(vehicle_index, instance);
        break;
    case 2:
        get_type_human_plane()->update_physics(vehicle_index, instance);
        break;
    case 3:
        get_type_alien_scout()->update_physics(vehicle_index, instance);
        break;
    case 4:
        get_type_alien_fighter()->update_physics(vehicle_index, instance);
        break;
    case 5:
        break;
    case 6:
        get_type_mantis()->update_physics(vehicle_index, instance);
        break;
    case 9:
        get_type_guardian()->update_physics(vehicle_index, instance);
        break;
    case 7:
        get_type_vtol()->update_physics(vehicle_index, instance);
        break;
    case 8:
        get_type_chopper()->update_physics(vehicle_index, instance);
        break;
    default:
        // match: 0xf1b88: unreachable hint omits dead branch check
        __assume(0);
    }
    vehicle_try_to_join_local_physics(vehicle_index, instance);
}


bool c_vehicle_type_component::compute_function_value(long vehicle_index, long function, float *value, bool *active)
{
    bool result = false;
    e_vehicle_type type = vehicle_get_type(vehicle_index);
    switch (type) {
    case 0:
        result = get_type_human_tank()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 1:
        result = get_type_human_jeep()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 2:
        result = get_type_human_plane()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 3:
        result = get_type_alien_scout()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 4:
        result = get_type_alien_fighter()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 5:
        result = get_type_turret()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 6:
        result = get_type_mantis()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 9:
        result = get_type_guardian()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 7:
        result = get_type_vtol()->compute_function_value(vehicle_index, function, value, active);
        break;
    case 8:
        result = get_type_chopper()->compute_function_value(vehicle_index, function, value, active);
        break;
    default:
        // match: 0xdc638: __assume(0) omits the cmplwi-9/bgt bounds check before the jump table
        __assume(0);
    }
    return result;
}

// Reports the emergency brake flag for the supported vehicle type.
bool c_vehicle_type_component::is_e_braking(long object_index)
{
  if (vehicle_get_type(object_index) != 1)
    return false;
  vehicle_datum *object =
    (vehicle_datum *)object_get_and_verify_type(object_index, NONE);
  return (object->unit.control_flags >> 11) & 1;
}

bool c_vehicle_type_component::should_override_deactivation(long object_index)
{
    bool should_override;
    e_vehicle_type type = vehicle_get_type(object_index);
    switch (type) {
    case 0:
        should_override = get_type_human_tank()->should_override_deactivation(object_index);
        break;
    case 1:
        should_override = get_type_human_jeep()->should_override_deactivation(object_index);
        break;
    case 2:
        should_override = get_type_human_plane()->should_override_deactivation(object_index);
        break;
    case 3:
        should_override = get_type_alien_scout()->should_override_deactivation(object_index);
        break;
    case 4:
        should_override = get_type_alien_fighter()->should_override_deactivation(object_index);
        break;
    case 5:
        should_override = get_type_turret()->should_override_deactivation(object_index);
        break;
    case 6:
        should_override = get_type_mantis()->should_override_deactivation(object_index);
        break;
    case 9:
        should_override = get_type_guardian()->should_override_deactivation(object_index);
        break;
    case 7:
        should_override = get_type_vtol()->should_override_deactivation(object_index);
        break;
    case 8:
        should_override = get_type_chopper()->should_override_deactivation(object_index);
        break;
    default:
        // match: 0x195998: __assume(0) omits the cmplwi-9/bgt bounds check and the should_override stack spill
        __assume(0);
    }
    return should_override;
}

__forceinline void c_vehicle_type_human_tank::adjust_gravity(long vehicle_index, real_vector3d *gravity)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_human_tank_definition *definition
        = (s_vehicle_human_tank_definition *)TAG_GET('vehi', vehicle_definition, vehicle->object.definition)->human_tank.address;
    vehicle_scale_gravity_for_friction_point_vehicle(vehicle_index, &vehicle->object.up, gravity, definition->gravity_adjust);
}
__forceinline void c_vehicle_type_human_jeep::adjust_gravity(long vehicle_index, real_vector3d *gravity)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    const s_vehicle_human_jeep_definition *definition
        = (s_vehicle_human_jeep_definition *)TAG_GET('vehi', vehicle_definition, vehicle->object.definition)->jeep;
    vehicle_scale_gravity_for_friction_point_vehicle(vehicle_index, &up, gravity, definition->gravity_adjust);
}
__forceinline void c_vehicle_type_alien_fighter::adjust_gravity(long vehicle_index, real_vector3d *gravity)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    scale_vector3d(gravity, 1.0f - vehicle->unit.throttle_scale * lift_scale, gravity);
}
static __forceinline void vehicle_type_component_mantis_adjust_gravity(c_vehicle_type_mantis *self, long vehicle_index, real_vector3d *gravity)
{
    if (self->m_stabilization_ticks > 0) {
        const s_vehicle_mantis_definition *definition = (s_vehicle_mantis_definition *)TAG_GET('vehi', vehicle_definition,
            ((vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle))->object.definition)->mantis.address;
        real_vector3d parallel, perpendicular;
        float dot = dot_product3d(gravity, &self->m_foot_up);
        scale_vector3d(&self->m_foot_up, dot, &parallel);
        subtract_vectors3d(gravity, &parallel, &perpendicular);
        scale_vector3d(&perpendicular, definition->gravity_scale, &perpendicular);
        add_vectors3d(&perpendicular, &parallel, gravity);
    }
}
// @address 0x195c50 ?adjust_gravity@c_vehicle_type_component@@QAAXJPATreal_vector3d@@@Z
void c_vehicle_type_component::adjust_gravity(long index, real_vector3d *gravity)
{
    switch (vehicle_get_type(index)) {
    case _vehicle_type_human_tank: get_type_human_tank()->adjust_gravity(index, gravity); break;
    case _vehicle_type_human_jeep: get_type_human_jeep()->adjust_gravity(index, gravity); break;
    case _vehicle_type_human_boat: case _vehicle_type_alien_scout: break;
    case _vehicle_type_alien_fighter: get_type_alien_fighter()->adjust_gravity(index, gravity); break;
    case _vehicle_type_turret: break;
    case _vehicle_type_mantis: vehicle_type_component_mantis_adjust_gravity(get_type_mantis(), index, gravity); break;
    case _vehicle_type_vtol: case _vehicle_type_chopper: case _vehicle_type_guardian: break;
    // match: 0x195c50: __assume(0) omits the cmplwi-6/bgt bounds check and keeps the 10-entry jump table
    default: __assume(0);
    }
}

// @address 0x3538e0 ?deplete_function_variables@c_vehicle_type_component@@QAAXJ@Z
void c_vehicle_type_component::deplete_function_variables(long object_index)
{
    e_vehicle_type vehicle_type;
    unsigned char engine_history_1;
    unsigned char engine_history_2;
    unsigned char engine_history_3;
    vehicle_type = vehicle_get_type(object_index);
    switch (vehicle_type) {
    case _vehicle_type_human_tank:
        get_type_human_tank()->m_engine.gear = -1;
        vehicle_degrade_variable(&get_type_human_tank()->m_engine.rpm);
    case _vehicle_type_turret:
        return;
    case _vehicle_type_human_jeep:
        get_type_human_jeep()->e_braking = false;
        get_type_human_jeep()->engine.gear = -1;
        vehicle_degrade_variable(&get_type_human_jeep()->engine.rpm);
        vehicle_degrade_variable(&get_type_human_jeep()->turning_value);
        return;
    case _vehicle_type_human_boat:
        vehicle_degrade_variable(&get_type_human_plane()->m_forward_speed);
        vehicle_degrade_variable(&get_type_human_plane()->m_left_speed);
        vehicle_degrade_variable(&get_type_human_plane()->m_vertical_speed);
        vehicle_degrade_variable(&get_type_human_plane()->m_smoothed_control);
        return;
    case _vehicle_type_alien_scout:
        vehicle_degrade_variable(&get_type_alien_scout()->m_a);
        vehicle_degrade_variable(&get_type_alien_scout()->m_throttle.j);
        vehicle_degrade_variable(&get_type_alien_scout()->m_throttle.k);
        vehicle_degrade_variable(&get_type_alien_scout()->scale);
        return;
    case _vehicle_type_alien_fighter:
        vehicle_degrade_variable(&get_type_alien_fighter()->thrust);
        vehicle_degrade_variable(&get_type_alien_fighter()->forward);
        vehicle_degrade_variable(&get_type_alien_fighter()->turn);
        vehicle_degrade_variable(&get_type_alien_fighter()->vertical);
        vehicle_degrade_variable(&get_type_alien_fighter()->lift_scale);
        return;
    case _vehicle_type_mantis:
        vehicle_degrade_variable(&get_type_mantis()->m_forward_throttle);
        vehicle_degrade_variable(&get_type_mantis()->m_lateral_throttle);
        return;
    case _vehicle_type_guardian:
        vehicle_degrade_variable(&get_type_guardian()->m_hover);
        return;
    case _vehicle_type_vtol:
        engine_history_1 = get_type_vtol()->engine_a[1];
        engine_history_2 = get_type_vtol()->engine_a[2];
        engine_history_3 = get_type_vtol()->engine_a[3];
        get_type_vtol()->engine_a[0] = get_type_vtol()->engine_a[0] >> 1;
        get_type_vtol()->engine_a[1] = engine_history_1 >> 1;
        get_type_vtol()->engine_a[2] = engine_history_2 >> 1;
        get_type_vtol()->engine_a[3] = engine_history_3 >> 1;
        return;
    case _vehicle_type_chopper:
        get_type_chopper()->e_braking = false;
        get_type_chopper()->engine.gear = -1;
        vehicle_degrade_variable(&get_type_chopper()->engine.rpm);
        vehicle_degrade_variable(&get_type_chopper()->wheel_position);
        return;
    // match: 0x3538e0: unreachable hint omits dead branch check
    default: __assume(0);
    }
}
extern "C" s_vehicle_auto_control *vehicle_get_auto_control(void *storage, long object_index)
{
    s_vehicle_auto_control *result = 0;
    switch (vehicle_get_type(object_index)) {
    case 5:
        result = &((c_vehicle_type_turret *)storage)->m_auto_control;
        break;
    case 9:
        result = &((c_vehicle_type_guardian *)storage)->m_auto_control;
        break;
    }
    return result;
}
// @address 0x79dcf0 k_human_plane_control_limit_fallback
// The two fallback initializers must be spelled differently: identical
// spellings merge into one symbol, and a plain literal lands in .rdata.
extern "C" {
const float k_human_plane_control_limit_fallback = 2.0f * k_pi;
}
// @address 0xf0bd0 ?update_control@c_vehicle_type_human_plane@@QAAXJ@Z
void c_vehicle_type_human_plane::update_control(long index)
{
    vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(index, _object_mask_vehicle));
    const s_vehicle_human_plane_definition *definition
        = (const s_vehicle_human_plane_definition *)TAG_GET('vehi', vehicle_definition, vehicle->object.definition)->human_plane.address;
    vehicle_update_boost(index, 0);
    m_control = vehicle_update_control_classic(index, &vehicle->object.forward, &vehicle->object.up, vehicle->unit.throttle.i, vehicle->unit.throttle.j,
        0, 0, &definition->forward_speed, &definition->left_speed, false, false, 0, &m_forward_speed, &m_left_speed);
    float scale = PIN(1.0f - definition->steering_animation.interpolation_scale, 0.1f, 1.0f);
    float limit = definition->steering_animation.maximum_angle - 0.0001f < 0.0f
        ? k_human_plane_control_limit_fallback : definition->steering_animation.maximum_angle;
    float step = PIN(m_control - m_smoothed_control, -limit, limit);
    m_smoothed_control = step * scale + m_smoothed_control;
}
// @address 0x79dcf4 k_alien_scout_control_limit_fallback
// Spelling differs from k_human_plane_control_limit_fallback on purpose (see above).
extern "C" {
const float k_alien_scout_control_limit_fallback = k_pi * 2.0f;
}
// @address 0x371320 ?update_control@c_vehicle_type_alien_scout@@QAAXJ@Z
void c_vehicle_type_alien_scout::update_control(long index)
{
    vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(index, _object_mask_vehicle));
    const vehicle_type_component_control_definition *control
        = (vehicle_type_component_control_definition *)TAG_GET('vehi', vehicle_definition, vehicle->object.definition)->alien_scout.address;
    vehicle_update_boost(index, 0);
    m_steering = vehicle_update_control_classic(index, &vehicle->object.forward, &vehicle->object.up, vehicle->unit.throttle.i, vehicle->unit.throttle.j,
        &control->steering, 0, 0, 0, false, false, 0, 0, 0);
    float scale = PIN(1.0f - control->steering_animation.interpolation_scale, 0.1f, 1.0f);
    float limit = control->steering_animation.maximum_angle - 0.0001f < 0.0f
        ? k_alien_scout_control_limit_fallback : control->steering_animation.maximum_angle;
    float step = PIN(m_steering - this->scale, -limit, limit);
    this->scale = step * scale + this->scale;
    vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(index, _object_mask_vehicle);
    c_animation_manager *animation = (c_animation_manager *)((char *)object + object->object.animation.offset);
    float factor = 1.0f;
    if (animation->m_current_goal.state == 96)
        factor = 1.0f - animation_channel_get_playback_ratio((c_animation_channel *)animation);
    else if (animation->m_current_goal.state == 97)
        factor = animation_channel_get_playback_ratio((c_animation_channel *)animation);
    this->scale *= factor;
}
__forceinline void c_vehicle_type_guardian::update_control(long vehicle_index)
{
    const s_vehicle_steering_control_definition *steering = (s_vehicle_steering_control_definition *)TAG_GET('vehi', vehicle_definition,
        ((vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle))->object.definition)->guardian.address;
    vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle));
    m_steering = vehicle_update_control_classic(vehicle_index, &vehicle->object.forward, &vehicle->object.up, vehicle->unit.throttle.i,
        vehicle->unit.throttle.j,
        steering, 0, 0, 0, false, false, 0, 0, 0);
}
__forceinline void c_vehicle_type_vtol::update_control(long vehicle_index)
{
    vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle));
    const s_vehicle_turning_control_definition *turning
        = (s_vehicle_turning_control_definition *)TAG_GET('vehi', vehicle_definition, vehicle->object.definition)->vtol.address;
    real_vector3d throttle;
    compute_throttle(vehicle_index, &throttle);
    s_vehicle_steering_control_definition steering = { 0.17453292f, 1.2f };
    m_steering = vehicle_update_control_classic(vehicle_index, &vehicle->object.forward, &vehicle->object.up, throttle.i, throttle.j,
        &steering, turning, 0, 0, false, false, &m_turn_position, 0, 0);
}
// @address 0xdc348 ?update_control@c_vehicle_type_component@@QAAXJ@Z
void c_vehicle_type_component::update_control(long object_index)
{
    e_vehicle_type type = vehicle_get_type(object_index);
    switch (type) {
    case 0:
        get_type_human_tank()->update_control(object_index);
        break;
    case 1:
        get_type_human_jeep()->update_control(object_index);
        break;
    case 2:
        get_type_human_plane()->update_control(object_index);
        break;
    case 3:
        get_type_alien_scout()->update_control(object_index);
        break;
    case 4:
        get_type_alien_fighter()->update_control(object_index);
        break;
    case 5:
        get_type_turret()->update_control(object_index);
        break;
    case 6:
        get_type_mantis()->update_control(object_index);
        break;
    case 9:
        get_type_guardian()->update_control(object_index);
        break;
    case 7:
        get_type_vtol()->update_control(object_index);
        break;
    case 8:
        get_type_chopper()->update_control(object_index);
        break;
    default:
        // match: 0xdc348: unreachable hint omits dead branch check
        __assume(0);
    }
}

// @rdata 0x8203ac90..0x8203aca0
static const float vehicle_type_component_argb_white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
// @rdata-end
const real_argb_color *objects_get_default_argb_white()
{
    return (const real_argb_color *)vehicle_type_component_argb_white;
}
// @rdata 0x8202251c
static const float k_vehicle_type_component_zero = 0.0f;
static __forceinline const float &vehicle_type_component_reset_zero() { return k_vehicle_type_component_zero; }
// @address 0x3535b8 ?reset@c_vehicle_type_component@@QAAXJ@Z
void c_vehicle_type_component::reset(long object_index)
{
    m_type = (signed char)vehicle_get_type(object_index);
    memset(m_storage, 0, sizeof(m_storage));
    switch (vehicle_get_type(object_index))
    {
    case 0:
    {
        c_vehicle_type_human_tank *w = get_type_human_tank();
        memset(&w->m_engine, 0, sizeof(w->m_engine));
        w->m_engine.gear = -1;
        w->m_engine.desired_gear = -2;
        w->m_ground_speed_animation.clear();
        w->m_stop_counter.m_ticks = 254;
        return;
    }
    case 1:
    {
        c_vehicle_type_human_jeep *w = get_type_human_jeep();
        memset(&w->engine, 0, sizeof(w->engine));
        w->engine.gear = -1;
        w->engine.desired_gear = -2;
        w->steering.clear();
        w->velocity.clear();
        w->turning.clear();
        w->stop_counter.m_ticks = 254;
        w->braking_counter.m_ticks = 0;
        w->up = *global_up3d;
        return;
    }
    case 2:
    {
        float zero = vehicle_type_component_reset_zero();
        get_type_human_plane()->m_control = zero;
        get_type_human_plane()->m_smoothed_control = zero;
        get_type_human_plane()->m_animation.clear();
        return;
    }
    case 3:
    {
        float zero = vehicle_type_component_reset_zero();
        get_type_alien_scout()->marker.clear();
        get_type_alien_scout()->m_b = zero;
        get_type_alien_scout()->m_steering = zero;
        get_type_alien_scout()->scale = zero;
        return;
    }
    case 4:
    {
        float zero = vehicle_type_component_reset_zero();
        get_type_alien_fighter()->lift_scale = zero;
        get_type_alien_fighter()->animation_id.clear();
        get_type_alien_fighter()->type = 0;
        get_type_alien_fighter()->count = 255;
        return;
    }
    case 5:
    {
        float zero = vehicle_type_component_reset_zero();
        s_vehicle_auto_control *d = &get_type_turret()->m_auto_control;
        d->active = 0;
        const s_damage_owner *owner = damage_owner_unknown();
        d->damage_owner = *owner;
        get_type_turret()->m_volume_index = -1;
        get_type_turret()->m_min_range = zero;
        get_type_turret()->m_max_range = zero;
        get_type_turret()->m_lost_seconds = zero;
        d->target_object_index = -1;
        d->target_time = -1;
        d->minimum_range_start_time = -1;
        d->target_lost_time = -1;
        return;
    }
    case 6:
        get_type_mantis()->m_jump_start_time = -1;
        get_type_mantis()->m_foot_group = false;
        get_type_mantis()->m_foot_up = *global_up3d;
        return;
    case 9:
    {
        float zero = vehicle_type_component_reset_zero();
        s_vehicle_auto_turret_control *d = &get_type_guardian()->m_auto_control;
        get_type_guardian()->m_steering = zero;
        d->active = 0;
        const s_damage_owner *owner = damage_owner_unknown();
        d->damage_owner = *owner;
        d->m_volume_index = -1;
        d->m_min_range = zero;
        d->m_max_range = zero;
        d->m_lost_seconds = zero;
        d->target_object_index = -1;
        d->target_time = -1;
        d->minimum_range_start_time = -1;
        d->target_lost_time = -1;
        return;
    }
    case 7:
    {
        float zero = vehicle_type_component_reset_zero();
        get_type_vtol()->animation_ids[0].clear();
        get_type_vtol()->animation_ids[1].clear();
        get_type_vtol()->lift = zero;
        return;
    }
    case 8:
    {
        c_vehicle_type_chopper *w = get_type_chopper();
        memset(&w->engine, 0, sizeof(w->engine));
        w->engine.gear = -1;
        w->engine.desired_gear = -2;
        w->turn.clear();
        w->wheel.clear();
        w->stop_counter.m_ticks = 254;
        w->braking_counter.m_ticks = 0;
        return;
    }
    default:
        // match: 0x3535b8: unreachable hint omits dead branch check
        __assume(0);
    }
}
