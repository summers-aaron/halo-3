#ifndef OBJECTS_SCENERY_H
#define OBJECTS_SCENERY_H

#include "objects/object_definitions.h"
#include "objects/object_datum.h"

struct s_scenery_object {
  object_datum object;
  long flags;
  unsigned short pathfinding_policy;
  long instance_index;
  long linked_object_index;
};

typedef char scenery_pathfinding_policy_offset[
    (offsetof(s_scenery_object, pathfinding_policy) == 0x164) ? 1 : -1];

typedef char scenery_object_named_flags_offset[(offsetof(s_scenery_object, flags) == 0x160) ? 1 : -1];

typedef char scenery_instance_index_offset[(offsetof(s_scenery_object, instance_index) == 0x168) ? 1 : -1];

struct s_scenery_definition : s_object_definition
{
    short pathfinding_policy;
    unsigned short scenery_flags;
    short lightmapping_policy;
};

typedef char scenery_definition_flags_offset[(offsetof(s_scenery_definition, scenery_flags) == 0xfa) ? 1 : -1];

// Functions defined in tu/objectslibxbox/scenery.cpp that other units call.

struct s_scenario_scenery;
void scenery_place(long scenery_index, s_scenario_scenery *scenario_scenery);

short scenery_animation_get_ticks_remaining_for_scripting(long scenery_index);
void scenery_animation_start_idle(long scenery_index);
bool scenery_animation_start_private(
    long scenery_index, long graph_index, long animation_name, short frame_index,
    long linked_object_index, bool repeat, bool interpolate);

#endif // OBJECTS_SCENERY_H
