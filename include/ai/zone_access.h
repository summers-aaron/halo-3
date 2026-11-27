#ifndef AI_ZONE_ACCESS_H
#define AI_ZONE_ACCESS_H

#include "ai/Zone.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_access.h"

inline zone_definition *zone_definition_get(long index)
{
    return (zone_definition *)tag_block_get_element_with_size(
        &global_scenario_get()->zones, index, sizeof(zone_definition));
}

#endif // AI_ZONE_ACCESS_H
