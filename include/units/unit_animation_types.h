#ifndef UNITS_UNIT_ANIMATION_TYPES_H
#define UNITS_UNIT_ANIMATION_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"

// Unit animation state holding aiming bounds, root index, and saved weapon info.
enum e_unit_animation_flags {
    _unit_animation_flag_bit0 = 0,
    _unit_animation_flag_weapon_ik = 0,
    _unit_animation_auxilary_animations_bit = 1,
    _unit_animation_disabled_bit = 2,
    _unit_animation_primary_aiming_screen_bounds_bit = 5,
    _unit_animation_aiming_euler_bit = 6,
    _unit_animation_secondary_aiming_screen_bounds_bit = 7,
    _unit_animation_looking_euler_bit = 8,
    _unit_animation_flag_constant_overlay_unknown = 9,   /* dev 7 */
    k_unit_animation_flags_count = 9
};
struct unit_aiming_screen_bounds { real_bounds yaw; real_bounds pitch; };

struct s_unit_animation {
    c_flags_no_init<e_unit_animation_flags, unsigned short, 9> flags;
    short pad2;
    long pad4;
    union { float aim_bounds[4]; unit_aiming_screen_bounds primary_aiming_screen_bounds; };
    union { float look_bounds[4]; unit_aiming_screen_bounds secondary_aiming_screen_bounds; };
    long pad40;
    long root_index;
    long pad48;
    short node_index_1;
    short node_index_2;
    long saved_weapon_class; // Weapon class saved while holstered.
    long saved_weapon_type; // Weapon type used by the holstered weapon channel.
};

typedef char unit_animation_saved_weapon_class_offset[(offsetof(s_unit_animation, saved_weapon_class) == 0x38) ? 1 : -1];
typedef char unit_animation_saved_weapon_type_offset[(offsetof(s_unit_animation, saved_weapon_type) == 0x3c) ? 1 : -1];

typedef char unit_animation_size[(sizeof(s_unit_animation) == 0x40) ? 1 : -1];
typedef char unit_animation_root_offset[(offsetof(s_unit_animation, root_index) == 0x2c) ? 1 : -1];
typedef char unit_animation_aim_bounds_offset[(offsetof(s_unit_animation, aim_bounds) == 8) ? 1 : -1];
typedef char unit_animation_look_bounds_offset[(offsetof(s_unit_animation, look_bounds) == 24) ? 1 : -1];

typedef char unit_animation_secondary_bounds_offset[(offsetof(s_unit_animation, secondary_aiming_screen_bounds) == 0x18) ? 1 : -1];

#endif // UNITS_UNIT_ANIMATION_TYPES_H
