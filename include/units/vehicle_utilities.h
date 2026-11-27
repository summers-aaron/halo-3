#ifndef UNITS_VEHICLE_UTILITIES_H
#define UNITS_VEHICLE_UTILITIES_H

#include <stddef.h>
#include "physics/physics_variable_types.h"

struct s_vehicle_steering_control_definition
{
    float overdampen_cusp_angle;
    float overdampen_exponent;
};

struct s_vehicle_turning_control_definition
{
    physics_variable_position turn_bounds;
    float turn_rate;
};

struct s_vehicle_steering_animation_definition
{
    float interpolation_scale;
    float maximum_angle;
};

typedef char vehicle_steering_control_size[(sizeof(s_vehicle_steering_control_definition) == 8) ? 1 : -1];
typedef char vehicle_turning_control_size[(sizeof(s_vehicle_turning_control_definition) == 12) ? 1 : -1];
typedef char vehicle_turning_control_rate_offset[(offsetof(s_vehicle_turning_control_definition, turn_rate) == 8) ? 1 : -1];
typedef char vehicle_steering_animation_size[(sizeof(s_vehicle_steering_animation_definition) == 8) ? 1 : -1];

// Functions defined in tu/objectslibxbox/vehicle_utilities.cpp.

struct physics_variable_speed;
union real_vector3d;
struct s_havok_vehicle_physics_instance;
struct s_vehicle_steering_control_definition;
struct s_vehicle_turning_control_definition;

float vehicle_update_control_classic(long vehicle_index, const real_vector3d *forward, const real_vector3d *up,
    float throttle_forward, float throttle_left, const s_vehicle_steering_control_definition *steering_control,
    const s_vehicle_turning_control_definition *turning_control, const physics_variable_speed *forward_speed,
    const physics_variable_speed *left_speed, bool reverse, bool force, float *turn_position, float *forward_value,
    float *left_value);
void vehicle_try_to_join_local_physics(long vehicle_index, const s_havok_vehicle_physics_instance *instance);
void vehicle_degrade_variable(float *variable);
void vehicle_scale_gravity_for_friction_point_vehicle(long vehicle_index, const real_vector3d *normal, real_vector3d *gravity, float scale);
real_vector3d *vehicle_compute_acceleration(const real_vector3d *a, const real_vector3d *b, float maximum, float minimum, real_vector3d *result, float dt);
float vehicle_compute_throttle1d(long vehicle_index);
void vehicle_update_antigravity_hover(const s_havok_vehicle_physics_instance *instance, float *hover_height);
void vehicle_compute_relative_throttle(long vehicle_index, real_vector3d *relative_throttle);
void vehicle_calculate_turning_torque(const s_havok_vehicle_physics_instance *physics, const real_vector3d *direction,
    real_vector3d *torque, bool airborne, float scale, float throttle, float turning);
extern "C" unsigned char vehicle_update_boost(long object_index, unsigned char force);
extern "C" bool vehicle_has_element_value(long object_index, long element_value);

class c_vehicle_type_human_plane;
typedef c_vehicle_type_human_plane vehicle_utilities_engine_input;

extern "C" void vehicle_update_engine_scales(vehicle_utilities_engine_input *input, long object_index);

enum e_vehicle_type;
e_vehicle_type vehicle_get_type(long object_index);

#endif // UNITS_VEHICLE_UTILITIES_H
