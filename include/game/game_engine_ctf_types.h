#ifndef GAME_GAME_ENGINE_CTF_TYPES_H
#define GAME_GAME_ENGINE_CTF_TYPES_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"

// Element of the scenario spawn data's static respawn/initial spawn zone blocks. Field names follow the tag layout
// (https://github.com/TheGuardians/TagTool/blob/master/TagTool/Tags/Definitions/Scenario.cs, StaticSpawnZone);
// March c_ctf_engine::static_spawn_zone_is_valid_for_player reads flags.
struct s_static_spawn_zone
{
    string_id name;
    unsigned long relevant_team;
    unsigned long relevant_games;
    long flags;
    real_point3d position;
    float lower_height;
    float upper_height;
    float inner_radius;
    float outer_radius;
    float weight;
};

typedef char static_spawn_zone_flags_offset[(offsetof(s_static_spawn_zone, flags) == 0xc) ? 1 : -1];
typedef char static_spawn_zone_size[(sizeof(s_static_spawn_zone) == 0x30) ? 1 : -1];

#endif // GAME_GAME_ENGINE_CTF_TYPES_H
