#ifndef ITEMS_WEAPON_INTERFACE_STATE_TYPES_H
#define ITEMS_WEAPON_INTERFACE_STATE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "items/weapon_interface_magazine_state_types.h"

// Layout produced by weapon_build_weapon_interface_state in weapons.cpp.
struct weapon_interface_state
{
    float heat;
    float age;
    float charge;
    bool overheated;
    bool control_flag_1;
    float analog_value;
    bool charged_plasma_pistol;
    bool tracking;
    bool locking;
    bool locked;
    float lock_fraction;
    real_point3d target_position;
    short magazine_count;
    weapon_interface_magazine_state magazines[2];
};

typedef char weapon_interface_state_size[(sizeof(weapon_interface_state) == 0x40) ? 1 : -1];
typedef char weapon_interface_state_target_offset[(offsetof(weapon_interface_state, target_position) == 0x1c) ? 1 : -1];
typedef char weapon_interface_state_magazines_offset[(offsetof(weapon_interface_state, magazines) == 0x2a) ? 1 : -1];

#endif // ITEMS_WEAPON_INTERFACE_STATE_TYPES_H
