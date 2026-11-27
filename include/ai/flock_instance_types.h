#ifndef AI_FLOCK_INSTANCE_TYPES_H
#define AI_FLOCK_INSTANCE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/cluster_reference.h"
#include "tag_files/tag_types.h"

struct ai_flocks_spawn_source {
  long flags;
  real_point3d position;
  float orientation;
  float cone_angle;
  float radius;
  float weight;
  s_cluster_reference cluster;
};
struct boid_perch_entry_b;

struct s_flock_instance {
  long name;
  short definition;
  short bsp_index;
  short trigger_volume_index;
  unsigned short flags;
  float boundary_distance;
  long source_count;
  TAG_POINTER(ai_flocks_spawn_source) sources;
  long source_definition;
  long random_count;
  TAG_POINTER(boid_perch_entry_b) perch_b;
  long destination_definition;
  float minimum_period;
  float maximum_period;
  float minimum_scale;
  float maximum_scale;
  float scale_a;
  float scale_b;
  short creature;
  short minimum_boid_count;
  short maximum_boid_count;
  short target_flock;
};
typedef char ai_flocks_instance_size[(sizeof(s_flock_instance) == 0x48) ? 1 : -1];
typedef char ai_flocks_instance_sources[(offsetof(s_flock_instance, sources) == 0x14) ? 1 : -1];
typedef char ai_flocks_instance_minimum_count[(offsetof(s_flock_instance, minimum_boid_count) == 0x42) ? 1 : -1];
typedef char ai_flocks_instance_target[(offsetof(s_flock_instance, target_flock) == 0x46) ? 1 : -1];

#endif
