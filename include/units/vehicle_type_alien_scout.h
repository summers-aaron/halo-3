#ifndef UNITS_VEHICLE_TYPE_ALIEN_SCOUT_H
#define UNITS_VEHICLE_TYPE_ALIEN_SCOUT_H

#include "math/real_math_types.h"
#include "animations/animation_id.h"

class c_animation_channel;
struct render_model_definition;
struct s_havok_vehicle_physics_instance;

class c_vehicle_type_alien_scout {
    friend class c_vehicle_type_component;
public:
    void update_control(long vehicle_index);
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);

    void process_animation_channels(long vehicle_index,
        void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
        void *context, c_animation_channel *channel, bool refresh);
private:
  static void compute_throttle(long object_index, real_vector3d *throttle);
public:
  void update_physics(long vehicle_index, struct s_havok_vehicle_physics_instance *instance);
  bool compute_function_value(long vehicle_index, long function, float *value, bool *active);
  static void create_effects(long vehicle_index);
private:
  real_vector3d m_throttle;
private:
  float m_steering;
private:
  float m_a;
private:
  float m_b;
  float scale;
  c_animation_id marker;
};

extern "C" const float k_alien_scout_control_limit_fallback;

#endif // UNITS_VEHICLE_TYPE_ALIEN_SCOUT_H
