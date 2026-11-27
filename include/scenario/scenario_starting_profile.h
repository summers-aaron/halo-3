#ifndef SCENARIO_SCENARIO_STARTING_PROFILE_H
#define SCENARIO_SCENARIO_STARTING_PROFILE_H

#include "tag_files/tag_types.h"
#include <stddef.h>

// A starting weapon entry with loaded and reserve round counts.
struct s_scenario_starting_profile_weapon
{
    s_tag_reference weapon;
    unsigned short rounds_loaded;
    unsigned short rounds_inventory;
};
struct s_scenario_starting_profile
{
    char name[32];
    float body_vitality;
    float shield_vitality;
    s_scenario_starting_profile_weapon weapons[2];
    unsigned char grenade_counts[4];
};
typedef char scenario_starting_profile_grenades_offset[(offsetof(s_scenario_starting_profile, grenade_counts) == 0x50) ? 1 : -1];
typedef char scenario_starting_profile_size[(sizeof(s_scenario_starting_profile) == 0x54) ? 1 : -1];

#endif
