#ifndef UNITS_VEHICLE_ALIEN_SCOUT_DEFINITIONS_H
#define UNITS_VEHICLE_ALIEN_SCOUT_DEFINITIONS_H

#include <stddef.h>

struct s_scout_speed_definition { float unused, minimum, maximum, response, speed_scale; };
struct vehicle_type_alien_scout_handling
{
  long flags;
  float gravity_scale;
  float forward_speed;
  float reverse_speed;
  float acceleration;
  float deceleration;
  float left_speed;
  float right_speed;
  float lateral_acceleration;
  float lateral_deceleration;
  signed char control_type;
  unsigned char camera_flags;
  unsigned short : 16;
  float drag;
  float minimum_deceleration;
  float turning_scale;
  s_scout_speed_definition engine;
  s_scout_speed_definition contrail;
  float minimum_effect_rate;
  float maximum_effect_rate;
};

typedef char scout_control_offset[(offsetof(vehicle_type_alien_scout_handling, control_type) == 0x28) ? 1 : -1];
typedef char scout_engine_minimum_offset[(offsetof(vehicle_type_alien_scout_handling, engine.minimum) == 0x3c) ? 1 : -1];
typedef char scout_engine_maximum_offset[(offsetof(vehicle_type_alien_scout_handling, engine.maximum) == 0x40) ? 1 : -1];

#endif
