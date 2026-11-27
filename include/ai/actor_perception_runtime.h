#ifndef AI_ACTOR_PERCEPTION_RUNTIME_H
#define AI_ACTOR_PERCEPTION_RUNTIME_H
#include "objects/cluster_reference.h"
#include "ai/s12_types.h"
#include "ai/command_script_data.h"
#include "math/real_constant_pool.h"
#include "math/real_constant_pool_access.h"
struct s_structure_cluster_mapping;
extern "C" {
unsigned char structure_get_cluster_ai_deafening(s_structure_cluster_mapping *table, s_cluster_reference a, s_cluster_reference b);
float structure_audibility_get_distance(s_structure_cluster_mapping *table, unsigned short a, unsigned short b);
extern const float k_creature_ground_airborne_delay[];
extern const float literal_0_66f;
extern const float k_observer_default_aspect_ratio;
extern const float k_real_constant_pool[];
}
#define ACTOR_PERCEPTION_REAL_CONSTANTS k_creature_ground_airborne_delay
#define AI_AUDIBILITY_DISTANCE_SCALE k_observer_default_aspect_ratio
#define AI_PERCEPTION_REAL_CONSTANTS k_real_constant_pool
#endif
