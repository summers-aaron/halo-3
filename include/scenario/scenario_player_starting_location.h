#ifndef SCENARIO_PLAYER_STARTING_LOCATION_H
#define SCENARIO_PLAYER_STARTING_LOCATION_H

#include "math/real_math_types.h"
#include <stddef.h>

// A player starting location holds spawn position, facing, and pitch. The
// player_find_best_spawn_location reads position, facing, and pitch at 0, 12,
// and 16. Neither consumer names the final two shorts; their meanings are open.
struct s_scenario_player_starting_location
{
    real_point3d position;
    float facing;
    float pitch;
    short pad14;
    short pad16;
};

typedef char scenario_player_starting_location_size[(sizeof(s_scenario_player_starting_location) == 24) ? 1 : -1];
typedef char scenario_player_starting_location_facing_offset[(offsetof(s_scenario_player_starting_location, facing) == 12) ? 1 : -1];
typedef char scenario_player_starting_location_pitch_offset[(offsetof(s_scenario_player_starting_location, pitch) == 16) ? 1 : -1];

#endif // SCENARIO_PLAYER_STARTING_LOCATION_H
