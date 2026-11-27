#ifndef SCENARIO_SCENARIO_OBJECT_NAMES_H
#define SCENARIO_SCENARIO_OBJECT_NAMES_H

#include "objects/object_name_list.h"
// Per-thread object-name table, indexed by scenario object-name slot.
static __forceinline long scenario_object_name_get_object_index(short slot)
{
    return g_object_name_list[slot];
}
#endif
