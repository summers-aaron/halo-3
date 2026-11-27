#ifndef AI_GRAPH_TYPES_H
#define AI_GRAPH_TYPES_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Head of scenario_zone_set_audibility viewed as a room bit-matrix:
// structure_get_cluster_ai_deafening reads room_count as count and the
// ai_deafening_pas block address as bits. Order matches
// include/scenario/scenario_zone_set_audibility.h.
typedef struct {
    int door_portal_count;
    int count;
    real_bounds room_distance_bounds;
    s_tag_block encoded_door_pas;
    s_tag_block room_door_portal_encoded_pas;
    int ai_deafening_pas_count;
    int *bits;
} s_graph;

#endif // AI_GRAPH_TYPES_H
