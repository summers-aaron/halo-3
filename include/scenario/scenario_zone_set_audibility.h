#ifndef SCENARIO_SCENARIO_ZONE_SET_AUDIBILITY_H
#define SCENARIO_SCENARIO_ZONE_SET_AUDIBILITY_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "cache/cache_file_allocation_range_types.h"

struct scenario_audibility_room_bounds
{
    long first_room_index;
    long room_index_count;
};

// Zone-set audibility tag element. The perception
// initializer reads the bounds mapping and room indices.
struct scenario_zone_set_audibility
{
    long door_portal_count;
    long room_count;
    real_bounds room_distance_bounds;
    c_typed_tag_block<unsigned long> encoded_door_pas;
    c_typed_tag_block<unsigned long> room_door_portal_encoded_pas;
    s_tag_block ai_deafening_pas;
    s_tag_block room_distances;
    c_typed_tag_block<cache_file_allocation_range> game_portal_to_door_occluder_mappings;
    s_tag_block bsp_cluster_to_room_bounds_mappings;
    s_tag_block bsp_cluster_to_room_indices;
};

typedef char scenario_zone_set_audibility_size[(sizeof(scenario_zone_set_audibility) == 0x64) ? 1 : -1];
typedef char scenario_zone_set_audibility_bounds_offset[(offsetof(scenario_zone_set_audibility, bsp_cluster_to_room_bounds_mappings) == 0x4c) ? 1 : -1];
typedef char scenario_zone_set_audibility_indices_offset[(offsetof(scenario_zone_set_audibility, bsp_cluster_to_room_indices) == 0x58) ? 1 : -1];

#endif
