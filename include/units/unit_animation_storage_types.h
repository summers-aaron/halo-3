#ifndef UNITS_UNIT_ANIMATION_STORAGE_TYPES_H
#define UNITS_UNIT_ANIMATION_STORAGE_TYPES_H
#include <stddef.h>
#include "cseries/cseries_wrapped.h"
enum e_motor_animation_unit_flag_bits {
    _motor_animation_unit_flag_overlay = 9,
    _motor_animation_unit_flag_custom = 10,
};
// Shared block read by motor_animation and the custom-animation motor program.
struct s_unit_animation_storage {
    union {
        unsigned short flags;
        c_flags_no_init<e_motor_animation_unit_flag_bits, unsigned short, 16> flag_bits;
    };
    // Middle bytes mirror the s_unit_animation view of this block (aim/look bounds).
    short pad2;
    long pad4;
    float aim_bounds[4];
    float look_bounds[4];
    long pad40;
    long custom_animation_object_index;
    long desired_mode;
    // Model node indices shared with the s_unit_animation view of this block (March unit_animation_reset stores
    // model_find_node results here as halfwords).
    short node_index_1;
    short node_index_2;
    long weapon_class;
    long weapon_type;
    __forceinline bool test_flag(long bit) const { return ((flags >> bit) & 1) != 0; }
};
typedef char unit_animation_storage_object_offset[(offsetof(s_unit_animation_storage, custom_animation_object_index) == 0x2c) ? 1 : -1];
typedef char unit_animation_storage_mode_offset[(offsetof(s_unit_animation_storage, desired_mode) == 0x30) ? 1 : -1];
typedef char unit_animation_storage_weapon_offset[(offsetof(s_unit_animation_storage, weapon_type) == 0x3c) ? 1 : -1];
#endif
