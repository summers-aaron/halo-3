#ifndef SCENARIO_SCENARIO_WEAPON_TYPES_H
#define SCENARIO_SCENARIO_WEAPON_TYPES_H

#include "scenario/scenario_map_variant_types.h"

struct s_scenario_weapon
{
    s_scenario_object_permutation permutation;
    short rounds_total;
    short rounds_loaded;
    unsigned long flags;                // c_flags_no_init<e_scenario_weapon_flags, unsigned long, 3> (March)
};

typedef char scenario_weapon_rounds_offset[(offsetof(s_scenario_weapon, rounds_total) == 0x6c) ? 1 : -1];
typedef char scenario_weapon_flags_offset[(offsetof(s_scenario_weapon, flags) == 0x70) ? 1 : -1];

#endif
