#ifndef SCENARIO_SCENARIO_CUTSCENE_FLAG_H
#define SCENARIO_SCENARIO_CUTSCENE_FLAG_H

#include "math/real_math_types.h"
#include <stddef.h>

// Named position and facing used to place cutscene cameras.
struct scenario_cutscene_flag
{
    long pad0;
    long name;
    real_point3d position;
    real_euler_angles2d facing;
};

typedef char scenario_cutscene_flag_size[(sizeof(scenario_cutscene_flag) == 28) ? 1 : -1];
typedef char scenario_cutscene_flag_position_offset[(offsetof(scenario_cutscene_flag, position) == 8) ? 1 : -1];
typedef char scenario_cutscene_flag_facing_offset[(offsetof(scenario_cutscene_flag, facing) == 20) ? 1 : -1];

#endif // SCENARIO_SCENARIO_CUTSCENE_FLAG_H
