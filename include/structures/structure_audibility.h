#ifndef STRUCTURES_STRUCTURE_AUDIBILITY_H
#define STRUCTURES_STRUCTURE_AUDIBILITY_H

// Functions defined in tu/blamlibxbox/structure_audibility.cpp.

#include "objects/cluster_reference.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "scenario/scenario_zone_set_audibility.h"

// Element of structure_bsp::audibility, read up to the cluster distance table. The block order follows
// scenario_zone_set_audibility (March names its readers structure_get_door_portal_pas,
// structure_get_cluster_ai_deafening and structure_get_cluster_distance).
struct structure_audibility
{
    long door_portal_count;
    real_bounds cluster_distance_bounds;
    s_tag_block encoded_door_pas;
    s_tag_block cluster_door_portal_encoded_pas;
    s_tag_block ai_deafening_pas;
    s_tag_block cluster_distances;
};

// Per-zone-set cluster mapping built by actor_perception_initialize_audibility_table.
struct s_structure_cluster_mapping
{
    scenario_zone_set_audibility *audibility;
    long cluster_count;
    struct { short *values; long count; } bsp_clusters[1];
};
struct structure_bsp;


extern "C" unsigned char structure_get_cluster_ai_deafening(s_structure_cluster_mapping *table, s_cluster_reference a, s_cluster_reference b);

extern "C" float structure_audibility_get_cluster_distance(const structure_bsp *structure, int a, int b);
extern "C" float structure_audibility_get_distance(s_structure_cluster_mapping *mapping, unsigned short first_reference, unsigned short second_reference);
bool structure_audibility_is_valid(const structure_bsp *structure);

#endif // STRUCTURES_STRUCTURE_AUDIBILITY_H
