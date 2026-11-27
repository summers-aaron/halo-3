#ifndef UNITS_VEHICLE_TYPE_ALIEN_FIGHTER_H
#define UNITS_VEHICLE_TYPE_ALIEN_FIGHTER_H

#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "animations/animation_id.h"

class c_animation_channel;
struct render_model_definition;
struct s_havok_vehicle_physics_instance;

enum e_vehicle_trick_type {
  _vehicle_trick_type_none = 0,
  _vehicle_trick_type_roll_left = 1,
  _vehicle_trick_type_roll_right = 2,
  _vehicle_trick_type_backward = 3,
  k_vehicle_trick_type_count = 4,
};

class c_vehicle_type_alien_fighter {
    friend class c_vehicle_type_component;
public:
    __forceinline bool kills_riders_at_terminal_velocity(long object_index);
    __forceinline bool should_override_deactivation(long object_index);
    __forceinline void adjust_gravity(long vehicle_index, real_vector3d *gravity);

    void process_animation_channels(long vehicle_index,
        void (*callback)(long, const render_model_definition *, c_animation_channel *, float, float, float, void *),
        void *context, c_animation_channel *channel, bool refresh);
private:
  float trick_duration(long object_index);
  // match: 0x373f00: static, so no unused this parameter is passed (March keeps one).
  static e_vehicle_trick_type compute_new_trick_type(long object_index);
  bool compute_reference_and_trick_rotation(long object_index, const s_havok_vehicle_physics_instance *instance,
      real_matrix3x3 *reference_rotation, real_matrix3x3 *trick_rotation);
public:
  bool compute_function_value(long object_index, long function, float *value, bool *force_active);
  void start_trick(long object_index, e_vehicle_trick_type trick);
  void update_control(long object_index);
  void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
private:
  c_animation_id animation_id;
  float thrust;
  float forward;
  union { float turn; float animation_weight; };
  float vertical;
  float lift_scale;
public:
  signed char type;
  unsigned char count;
private:
  float compute_trick_t(long object_index);
public:
  bool is_running_trick(long object_index);
};

#endif // UNITS_VEHICLE_TYPE_ALIEN_FIGHTER_H
