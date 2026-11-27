#ifndef AI_SWARM_SPAWNER_DEFINITION_H
#define AI_SWARM_SPAWNER_DEFINITION_H

#include "ai/swarm_spawner_types.h"

// Complete swarm spawner definition passed when spawning swarms.
struct swarms_spawner_definition
{
    s_swarm_spawner_reference reference;
    const real_matrix4x3 *matrix;
    short node_index;
    short count;
    float values[8];
    float minimum_delay;
    float maximum_delay;
};

typedef char swarm_spawner_definition_size[(sizeof(void *) != 4 || sizeof(swarms_spawner_definition) == 0x3c) ? 1 : -1];
typedef char swarm_spawner_definition_values_offset[(sizeof(void *) != 4 || offsetof(swarms_spawner_definition, values) == 0x14) ? 1 : -1];

#endif // AI_SWARM_SPAWNER_DEFINITION_H
