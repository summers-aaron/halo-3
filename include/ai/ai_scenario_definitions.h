#ifndef AI_AI_SCENARIO_DEFINITIONS_H
#define AI_AI_SCENARIO_DEFINITIONS_H

#include "objects/cluster_reference.h"

// Functions defined in tu/ailibxbox/ai_scenario_definitions.cpp that other units call.

union real_point3d;
union real_vector3d;

s_cluster_reference ai_scenario_find_cluster_index(const real_point3d *point, const real_vector3d *vector);

#endif // AI_AI_SCENARIO_DEFINITIONS_H
