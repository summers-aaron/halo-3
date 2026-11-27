#ifndef SCENARIO_SCENARIO_SPAWN_DATA_H
#define SCENARIO_SCENARIO_SPAWN_DATA_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

// Dynamic and static respawn parameters stored in each
// spawn-data element. March uses the same element size and reset-height offset.
// TagTool Scenario.SpawnDatum identifies the bounds, reserved bytes and blocks.
struct s_scenario_spawn_data
{
    real_bounds dynamic_spawn_height_bounds;
    float game_object_reset_height;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    s_tag_block dynamic_spawn_overloads;
    s_tag_block static_respawn_zones;
    s_tag_block static_initial_spawn_zones;
};

typedef char scenario_spawn_data_size[(sizeof(s_scenario_spawn_data) == 0x6c) ? 1 : -1];
typedef char scenario_spawn_reset_height_offset[(offsetof(s_scenario_spawn_data, game_object_reset_height) == 8) ? 1 : -1];
typedef char scenario_spawn_overloads_offset[(offsetof(s_scenario_spawn_data, dynamic_spawn_overloads) == 0x48) ? 1 : -1];

#endif
