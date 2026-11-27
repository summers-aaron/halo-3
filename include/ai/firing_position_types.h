#ifndef AI_FIRING_POSITION_TYPES_H
#define AI_FIRING_POSITION_TYPES_H

#include <stddef.h>
#include "ai/firing_position_definition.h"
#include "ai/firing_position_ref.h"

struct firing_position {
 firing_position_definition *definition;
 union {
  firing_position_ref original_index;
  long reference;
  struct { short area; unsigned short flags; };
 };
 short type;
 short pad0a;
 real_point3d position;
 float target_distance;
 real_vector3d target_direction;
 float evaluation_distance;
 float distance;
 float danger_distance_squared;
 real_vector3d firing_direction;
 real_vector3d second;
 bool valid;
 union {
  bool preferred;
  bool rejected;
 };
 bool forced;
 bool evaluated;
 float evaluated_penalty;
 float evaluated_hide_score;
 union {
  float penalty;
  float score;
  float base_score;
 };
 union {
  float hide_score;
  float secondary_score;
 };
 bool concealed;
 bool reachable;
 bool enabled;
 bool primary;
 bool special;
 bool transparent_line_of_sight;
 short cornering;
 bool cornering_left;
 char pad69;
 short pad6a;
 c_ai_point3d cornering_point;
 real_vector3d cornering_normal;
};

typedef char firing_position_original_index_offset[(sizeof(void *) != 4 || ((offsetof(firing_position, original_index) == 4))) ? 1 : -1];
typedef char firing_position_line_of_sight_offset[(sizeof(void *) != 4 || ((offsetof(firing_position, transparent_line_of_sight) == 0x65))) ? 1 : -1];
typedef char firing_position_layout_size[(sizeof(void *) != 4 || sizeof(firing_position) == 0x88) ? 1 : -1];

#endif // AI_FIRING_POSITION_TYPES_H
