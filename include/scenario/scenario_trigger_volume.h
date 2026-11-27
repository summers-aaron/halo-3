#ifndef SCENARIO_SCENARIO_TRIGGER_VOLUME_H
#define SCENARIO_SCENARIO_TRIGGER_VOLUME_H

#include <stddef.h>
#include "math/real_math_types.h"

struct scenario_trigger_volume
{
    long name;
    unsigned short object_slot;
    long node_name;
    real_vector3d forward;
    real_vector3d up;
    real_point3d position;
    real_point3d extents;
    float radius;
    short kill_trigger_volume_index;
};

typedef char scenario_trigger_volume_size[(sizeof(scenario_trigger_volume) == 68) ? 1 : -1];
typedef char scenario_trigger_volume_object_offset[(offsetof(scenario_trigger_volume, object_slot) == 4) ? 1 : -1];
typedef char scenario_trigger_volume_node_offset[(offsetof(scenario_trigger_volume, node_name) == 8) ? 1 : -1];
typedef char scenario_trigger_volume_position_offset[(offsetof(scenario_trigger_volume, position) == 36) ? 1 : -1];
typedef char scenario_trigger_volume_extents_offset[(offsetof(scenario_trigger_volume, extents) == 48) ? 1 : -1];
typedef char scenario_trigger_volume_radius_offset[(offsetof(scenario_trigger_volume, radius) == 60) ? 1 : -1];
typedef char scenario_trigger_volume_kill_offset[(offsetof(scenario_trigger_volume, kill_trigger_volume_index) == 64) ? 1 : -1];

#endif // SCENARIO_SCENARIO_TRIGGER_VOLUME_H
