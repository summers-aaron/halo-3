#ifndef INTERFACE_CHUD_CHUD_NAVPOINT_TYPES_H
#define INTERFACE_CHUD_CHUD_NAVPOINT_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

// March c_chud_navpoint_manager::submit_navpoint (836a1938) copies 44 bytes.
// The
// identifier may name a player, an object, or a territory; state selects color.
struct s_chud_navpoint
{
    long object_index;
    long type;
    long icon;
    long state;
    unsigned long flags;
    float scale;
    wchar_t name[4];
    real_point3d position;
};

typedef char chud_navpoint_size[(sizeof(s_chud_navpoint) == 0x2c) ? 1 : -1];
typedef char chud_navpoint_flags_offset[(offsetof(s_chud_navpoint, flags) == 0x10) ? 1 : -1];
typedef char chud_navpoint_name_offset[(offsetof(s_chud_navpoint, name) == 0x18) ? 1 : -1];
typedef char chud_navpoint_position_offset[(offsetof(s_chud_navpoint, position) == 0x20) ? 1 : -1];

#endif // INTERFACE_CHUD_CHUD_NAVPOINT_TYPES_H
