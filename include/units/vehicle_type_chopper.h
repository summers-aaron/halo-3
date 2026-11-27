#ifndef UNITS_VEHICLE_TYPE_CHOPPER_H
#define UNITS_VEHICLE_TYPE_CHOPPER_H

#include "math/real_math_types.h"
#include "animations/animation_id.h"
#include "units/vehicle_engine.h"
#include "units/vehicle_counters.h"

class c_animation_channel;
struct render_model_definition;
struct s_havok_vehicle_physics_instance;

class c_vehicle_type_chopper {
public:
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);

    void process_animation_channels(long vehicle_index,
        void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
        void *context, c_animation_channel *channel, bool refresh);
public:
    s_vehicle_engine engine;
    c_animation_id turn;
    c_animation_id wheel;
    char spare_before_turn_angle[0x28 - 0x1c];
    float turn_angle;
    float wheel_speed;
    float wheel_position;
    float steering_fraction;
    c_vehicle_stop_counter stop_counter;
    c_vehicle_braking_counter braking_counter;
    bool e_braking;
    bool compute_function_value(long vehicle_index, long function_name, float *value, bool *active);
    void update_control(long vehicle_index);
    void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
    private:
    void update_damping(long vehicle_index, struct s_havok_vehicle_physics_instance *instance);
    // match: 0x378a28: static form drops the unused this parameter; March declares it non-static.
    static void update_boost(long vehicle_index, s_havok_vehicle_physics_instance *instance);
};

#endif // UNITS_VEHICLE_TYPE_CHOPPER_H
