#ifndef AI_FLOCK_RUNTIME_TYPES_H
#define AI_FLOCK_RUNTIME_TYPES_H
#include "ai/ai_flocks.h"
#include "ai/flock_instance_types.h"

struct boid_state {
  long flock_index;
  long next_object_index;
  unsigned char dead;
  unsigned char b9;
  bool active;
  unsigned char needs_update;
  unsigned short update_count;
  unsigned short w14;
  short w16;
  short update_timer;
  short target_timer;
  float f24;
  long l28;
  long target_object_index;
  short w36;
  short attack_timer;
  s_perlin_state perlin;
};

struct flock_datum {
  short identifier, definition_index;
  long tag_index;
  long first_boid_index;
  short boid_count;
  short spawn_timer;
  bool active;
  bool check_sources;
  bool spawning;
  short target_boid_count;
  short spawn_source;
  short source_mask;
  short used_source_mask;
  unsigned short danger_mask;
  real_point3d last_death_position;
  float death_scale;
  short death_timer;
  real_rectangle3d bounds;
};

struct boid_flock_definition {
  float speed_factor;
  float sink_factor;
  float perch_weight;
  float fallback_weight;
  float far_weight;
  float max_speed;
  float danger_radius;
  float danger_scale;
  float target_base;
  float attraction_distance;
  float attack_duration;
  float attack_chance;
  float perlin_magnitude;
  float perlin_minimum;
  float perlin_maximum;
  float neighbor_radius;
  float view_angle;
  float separation_multiplier;
  float separation_radius;
  float separation_force;
  float cohesion_weight;
  float cohesion_minimum;
  float cohesion_maximum;
};
#endif
