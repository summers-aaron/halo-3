#ifndef UNITS_VEHICLE_TYPE_VTOL_H
#define UNITS_VEHICLE_TYPE_VTOL_H

#include "math/real_math_types.h"
#include "animations/animation_id.h"

#include "units/vehicle_utilities.h"
struct s_havok_vehicle_physics_instance;
struct render_model_definition;
class c_animation_channel;

class c_vehicle_type_vtol {
    friend void vehicle_update_engine_scales(vehicle_utilities_engine_input *input, long object_index);
private:
    // match: 0x375980: restrict is mangled into the emitted symbol (CAXJPIAT); without it the definition emits CAXJPAT and misses its address tag
    static void compute_throttle(long vehicle_index, real_vector3d *__restrict throttle);
    static void interpolate_lift_angles(long object_index, float *angles, float *velocity, const float *desired_angles, bool boosted);
    static void update_rotor_damping(long object_index);
public:
    __forceinline void update_control(long vehicle_index);
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    void update_physics(long object_index, s_havok_vehicle_physics_instance *instance);
    void process_animation_channels(long object_index,
        void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
        void *user, c_animation_channel *channel, bool refresh);
    bool compute_function_value(long vehicle_index, long function, float *value, bool *active);
    float m_steering;
    float m_turn_position;
    float target_altitude;
    unsigned char engine_a[4];
    unsigned char engine_b[4];
    real_euler_angles2d left_angles;
    union { struct { float lift_i, lift_j; }; real_euler_angles2d left_angles2; };
    real_euler_angles2d right_angles;
    union { struct { float lower_i, lower_j; }; real_euler_angles2d right_angles2; };
    real_vector2d left_velocity, left_velocity2;
    real_vector2d right_velocity, right_velocity2;
    c_animation_id animation_ids[2];
    float lift;
private:
    void compute_engine_scales(float *scale_a, float *scale_b) const;
  void update_lift_vector(long object_index, const s_havok_vehicle_physics_instance *havok, const real_vector3d *input_vector, float scale,
      long marker_name, real_vector3d *lift_vector, real_point3d *position, real_euler_angles2d *angles, real_euler_angles2d *angles2,
      real_vector2d *velocity, real_vector2d *velocity2, float *out_value);
};


// Functions defined in tu/objectslibxbox/vehicle_type_vtol.cpp.

extern "C" float vehicle_type_vtol_compute_damped_response(bool clamp_to_infinity, bool scale_by_time, float time, float target,
    float acceleration, float velocity, float max_velocity, float friction, float exponent, bool instant, float *scale);

#endif // UNITS_VEHICLE_TYPE_VTOL_H
