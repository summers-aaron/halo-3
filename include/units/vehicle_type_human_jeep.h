#ifndef UNITS_VEHICLE_TYPE_HUMAN_JEEP_H
#define UNITS_VEHICLE_TYPE_HUMAN_JEEP_H

#include "math/real_math_types.h"
#include "animations/animation_id.h"
#include "units/vehicle_engine.h"
#include "units/vehicle_counters.h"
#include "units/vehicle_utilities.h"
#include "tag_files/string_ids.h"

class c_animation_channel;
struct render_model_definition;
struct s_havok_vehicle_physics_instance;

// The human jeep's physics type block element (the vehicle definition's human jeep block): the steering and
// turning controls passed to vehicle_update_control_classic, the engine, and the distance after which the
// wheel animation positions wrap.
struct s_vehicle_human_jeep_definition
{
    s_vehicle_steering_control_definition steering_control;
    s_vehicle_turning_control_definition turning_control;
    s_vehicle_engine_definition engine;
    float wheel_circumference;
    float gravity_adjust;
};

typedef char vehicle_human_jeep_gravity_offset[(offsetof(s_vehicle_human_jeep_definition, gravity_adjust) == 0x3c) ? 1 : -1];

typedef char vehicle_human_jeep_definition_engine_offset[(offsetof(s_vehicle_human_jeep_definition, engine) == 20) ? 1 : -1];

struct c_vehicle_type_human_jeep {
public:
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    __forceinline void adjust_gravity(long vehicle_index, real_vector3d *gravity);

    void process_animation_channels(long vehicle_index,
        void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
        void *context, c_animation_channel *channel, bool refresh);
 s_vehicle_engine engine;
 c_animation_id steering, velocity, turning;
 real_vector3d up;
 float steering_value, velocity_value, turning_value;
 c_vehicle_stop_counter stop_counter;
 c_vehicle_braking_counter braking_counter;
 bool e_braking;
 void update_control(long vehicle_index);
 bool compute_function_value(long vehicle_index, long function_name, float *value, bool *active);
 void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
};

#endif // UNITS_VEHICLE_TYPE_HUMAN_JEEP_H
