#ifndef UNITS_VEHICLE_MANTIS_DEFINITIONS_H
#define UNITS_VEHICLE_MANTIS_DEFINITIONS_H

#include "units/vehicle_utilities.h"
#include "physics/walker_physics_definitions.h"

struct s_vehicle_mantis_definition
{
    s_vehicle_steering_control_definition steering_control;
    s_vehicle_turning_control_definition turning_control;
    physics_variable_speed forward_speed;
    physics_variable_speed lateral_speed;
    s_walker_physics_definition walker_physics;
    short minimum_stuck_leg_count;
    float stabilization_time;
    float gravity_scale;
    float maximum_forward_correction;
    float forward_correction_scale;
    float maximum_lateral_correction;
    float lateral_correction_scale;
    float maximum_angular_correction;
    float angular_correction_scale;
    float jump_crouch_time;
    float jump_crouch_interpolate_fraction;
    float jump_leap_time;
    float jump_recovery_time;
    float jump_recovery_interpolate_fraction;
    float jump_crouch_depth;
    float jump_leap_height;
};

typedef char mantis_gravity_scale_offset[(offsetof(s_vehicle_mantis_definition, gravity_scale) == 0x74) ? 1 : -1];

typedef char mantis_stabilization_time_offset[(offsetof(s_vehicle_mantis_definition, stabilization_time) == 0x70) ? 1 : -1];
typedef char mantis_definition_size[(sizeof(s_vehicle_mantis_definition) == 0xac) ? 1 : -1];

#endif
