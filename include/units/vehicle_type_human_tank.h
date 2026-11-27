#ifndef UNITS_VEHICLE_TYPE_HUMAN_TANK_H
#define UNITS_VEHICLE_TYPE_HUMAN_TANK_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "animations/animation_id.h"
#include "physics/physics_variable_types.h"
#include "units/vehicle_counters.h"
#include "units/vehicle_engine.h"

struct s_havok_vehicle_physics_instance;

struct s_vehicle_human_tank_definition
{
    float maximum_angle;
    float forward_turn_scale;
    float reverse_turn_scale;
    physics_variable_speed forward_differential;
    physics_variable_speed reverse_differential;
    s_vehicle_engine_definition engine;
    float wheel_circumference;
    float gravity_adjust;
};

typedef char vehicle_human_tank_definition_size[(sizeof(s_vehicle_human_tank_definition) == 0x58) ? 1 : -1];

class c_vehicle_type_human_tank
{
    friend class c_vehicle_type_component;
public:
    __forceinline c_vehicle_stop_counter *get_stop_counter() { return &m_stop_counter; }
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    __forceinline void adjust_gravity(long vehicle_index, real_vector3d *gravity);

    void update_control(long vehicle_index);
    void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
    bool compute_function_value(long vehicle_index, long function_name, float *value, bool *active);

private:
    void compute_tread_speeds(long vehicle_index, float *left_tread_speed, float *right_tread_speed);

    s_vehicle_engine m_engine;
    c_animation_id m_ground_speed_animation;
    float m_differential;
    float m_left_tread_position;
    float m_right_tread_position;
    c_vehicle_stop_counter m_stop_counter;
};

typedef char vehicle_type_human_tank_size[(sizeof(c_vehicle_type_human_tank) == 0x28) ? 1 : -1];

#endif // UNITS_VEHICLE_TYPE_HUMAN_TANK_H
