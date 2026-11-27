#ifndef UNITS_VEHICLE_TYPE_MANTIS_H
#define UNITS_VEHICLE_TYPE_MANTIS_H

#include "math/real_math_types.h"
#include <stddef.h>

struct s_vehicle_mantis_definition;
struct s_havok_vehicle_physics_instance;

class c_vehicle_type_mantis {
private:
  __forceinline s_vehicle_mantis_definition *get_type_definition(long vehicle_index);
  void update_movement_leg_turn_and_displacment(long vehicle_index, float *final_turn, real_vector3d *final_leg_displacement);
  void update_leg_turn_and_displacment(long vehicle_index, float *turn, real_vector3d *movement, real_vector3d *jump_displacement, bool *jumping);
  void calculate_throttle(long vehicle_index, real_vector3d *throttle);
public:
  __forceinline bool compute_function_value(long vehicle_index, long function, float *value, bool *active);
  __forceinline bool kills_riders_at_terminal_velocity(long object_index);
  __forceinline bool should_override_deactivation(long object_index);
  void update_control(long vehicle_index);
  unsigned int : 32;
  real_vector3d m_foot_up;
  float m_forward_position;
  float m_lateral_position;
  float m_turn_position;
  float m_facing_angle;
  float m_forward_throttle;
  float m_lateral_throttle;
  signed char m_foot_group;
  signed char m_stabilization_ticks;
  long m_jump_start_time;
public:
  void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *physics);
};

typedef char mantis_foot_up_offset[(offsetof(c_vehicle_type_mantis, m_foot_up) == 0x04) ? 1 : -1];
typedef char mantis_jump_start_offset[(offsetof(c_vehicle_type_mantis, m_jump_start_time) == 0x2c) ? 1 : -1];

#endif
