#ifndef UNITS_VEHICLE_ALIEN_FIGHTER_DEFINITION_H
#define UNITS_VEHICLE_ALIEN_FIGHTER_DEFINITION_H

#include "units/vehicle_definitions.h"
#include "physics/physics_variable_types.h"

// Alien fighter handling parameters.
struct s_vehicle_alien_fighter_definition {
  s_vehicle_steering_control_definition steering_control;
  s_vehicle_turning_control_definition turning_control;
  physics_variable_speed forward_speed;
  physics_variable_speed vertical_speed;
  float flying_torque_scale;
  float fixed_gun_yaw;
  float fixed_gun_pitch;
  float maximum_trick_frequency;
  float loop_trick_duration;
  float roll_trick_duration;
  float zero_gravity_speed;
  float full_gravity_speed;
  float strafe_boost_scale;
  float off_stick_decel_scale;
  float cruising_throttle;
  float dive_speed_scale;
};
typedef char alien_fighter_definition_size[(sizeof(s_vehicle_alien_fighter_definition) == 0x64) ? 1 : -1];

#endif
