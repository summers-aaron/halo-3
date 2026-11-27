#ifndef SCENARIO_SCENARIO_MAP_VARIANT_TYPES_H
#define SCENARIO_SCENARIO_MAP_VARIANT_TYPES_H

#include "scenario/scenario_objects.h"
#include "objects/multiplayer_object_types.h"

struct s_scenario_object_placement {
    long placement_mode;
    unsigned short engine_flags;
    unsigned short team;
    unsigned char spare_clips;
    unsigned char minimum_count;
    signed char maximum_count;
    signed char placement_flags;
    unsigned short spawn_rate;
    unsigned short abandon_time;
    signed char remapping_policy;
    unsigned char boundary_shape;
    unsigned char spare_clips_copy;
    char spare_before_parent_name_index[3];
    short parent_name_index;
    char spare_before_boundary_width[8];
    float boundary_width;
    float boundary_box_length;
    float boundary_positive_height;
    float boundary_negative_height;
    float boundary_size;
    s_scenario_object_placement() : engine_flags(0), placement_flags(0) {}
};

// Placement tails shared by object_placement and scenario_map_variant.
// Each tail ends with shared multiplayer properties for units, equipment,
// scenery 0x80, crates 0x7c.
struct s_scenario_unit_placement
{
    s_scenario_object_permutation permutation;
    s_scenario_unit_datum unit;
    s_scenario_object_placement multiplayer;
};
struct s_scenario_equipment
{
    s_scenario_object object;
    long equipment_flags;
    s_scenario_object_placement multiplayer;
};
struct s_scenario_scenery_placement
{
    s_scenario_scenery scenery;
    s_scenario_object_placement multiplayer;
};
struct s_scenario_crate_placement
{
    s_scenario_crate crate;
    s_scenario_object_placement multiplayer;
};
typedef char scenario_unit_multiplayer_offset[(offsetof(s_scenario_unit_placement, multiplayer) == 0x74) ? 1 : -1];
typedef char scenario_equipment_multiplayer_offset[(offsetof(s_scenario_equipment, multiplayer) == 0x58) ? 1 : -1];
typedef char scenario_scenery_multiplayer_offset[(offsetof(s_scenario_scenery_placement, multiplayer) == 0x80) ? 1 : -1];
typedef char scenario_crate_multiplayer_offset[(offsetof(s_scenario_crate_placement, multiplayer) == 0x7c) ? 1 : -1];
#endif
