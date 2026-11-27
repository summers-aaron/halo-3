#ifndef UNITS_VEHICLE_TYPE_GUARDIAN_H
#define UNITS_VEHICLE_TYPE_GUARDIAN_H

#include <stddef.h>
#include "units/vehicle_auto_control.h"
#include "units/vehicle_utilities.h"

struct s_havok_vehicle_physics_instance;

struct s_vehicle_guardian_definition
{
    s_vehicle_steering_control_definition steering_control;
    float maximum_forward_speed;
    float maximum_reverse_speed;
    float speed_acceleration;
    float speed_deceleration;
    float maximum_left_slide;
    float maximum_right_slide;
    float slide_acceleration;
    float slide_deceleration;
    float torque_scale;
    float anti_gravity_force_z_offset;
};

typedef char vehicle_guardian_definition_size[(sizeof(s_vehicle_guardian_definition) == 0x30) ? 1 : -1];

class c_vehicle_type_guardian
{
public:
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    __forceinline void update_control(long vehicle_index);
    __forceinline bool compute_function_value(long vehicle_index, long function, float *value, bool *active);

    float m_steering;
    float m_hover;
    s_vehicle_auto_turret_control m_auto_control;

    void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
    const s_vehicle_guardian_definition *get_type_definition(long vehicle_index);
};

typedef char vehicle_type_guardian_volume_offset[(offsetof(c_vehicle_type_guardian, m_auto_control.m_volume_index) == 0x28) ? 1 : -1];

typedef char vehicle_type_guardian_auto_control_offset[(offsetof(c_vehicle_type_guardian, m_auto_control) == 0x08) ? 1 : -1];

#endif // UNITS_VEHICLE_TYPE_GUARDIAN_H
