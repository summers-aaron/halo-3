#ifndef UNITS_VEHICLE_TYPE_HUMAN_PLANE_H
#define UNITS_VEHICLE_TYPE_HUMAN_PLANE_H
#include <stddef.h>
#include "animations/animation_id.h"
#include "physics/physics_variable_types.h"
#include "units/vehicle_utilities.h"

struct s_vehicle_human_plane_definition {
    physics_variable_speed forward_speed;
    physics_variable_speed left_speed;
    physics_variable_speed vertical_speed;
    float angular_acceleration;
    float drag;
    float velocity_scale;
    float boosted_turn_scale;
    float maximum_bank_angle;
    s_vehicle_steering_animation_definition steering_animation;
};

struct s_havok_vehicle_physics_instance;

class c_vehicle_type_human_plane {
public:
    void update_control(long vehicle_index);
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    bool compute_function_value(long vehicle_index, long function, float *value, bool *active);
    void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
    float m_forward_speed;
    float m_left_speed;
    float m_vertical_speed;
    float m_control;
    float m_smoothed_control;
    c_animation_id m_animation;
};

typedef char human_plane_animation_offset[(offsetof(c_vehicle_type_human_plane, m_animation) == 0x14) ? 1 : -1];

typedef char human_plane_vertical_speed_offset[(offsetof(s_vehicle_human_plane_definition, vertical_speed) == 0x20) ? 1 : -1];
typedef char human_plane_steering_animation_offset[(offsetof(s_vehicle_human_plane_definition, steering_animation) == 0x44) ? 1 : -1];

// Functions defined in tu/objectslibxbox/vehicle_type_human_plane.cpp that other units call.

extern "C" void vehicle_type_human_plane_create_effects(long vehicle_index, float first_scale, float second_scale);

extern "C" {
extern float g_human_plane_drag_scale;
extern unsigned long g_human_plane_init_flags;
}

extern "C" const float k_human_plane_control_limit_fallback;

#endif // UNITS_VEHICLE_TYPE_HUMAN_PLANE_H
