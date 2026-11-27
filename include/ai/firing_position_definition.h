#ifndef AI_FIRING_POSITION_DEFINITION_H
#define AI_FIRING_POSITION_DEFINITION_H

#include <stddef.h>

#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"

// Firing-position tag entry.
// reads flags +0x10, the normal +0x1c and point.reference_frame().
struct firing_position_definition
{
    union
    {
        c_ai_point3d point;
        struct
        {
            real_point3d position;
            short attachment_index;
            short structure_bsp_index;
        };
    };
    short flags;
    union { unsigned short cornering_flags; short unknown; };
    union { short area; short area_index; };
    union { short cluster; short cluster_index; };
    union { c_sector_ref sector; c_sector_ref sector_ref; };
    real_euler_angles2d normal;
    float facing;
};

typedef char firing_position_definition_size[(sizeof(firing_position_definition) == 0x28) ? 1 : -1];
typedef char firing_position_definition_flags_offset[(offsetof(firing_position_definition, flags) == 0x10) ? 1 : -1];
typedef char firing_position_definition_sector_offset[(offsetof(firing_position_definition, sector) == 0x18) ? 1 : -1];
typedef char firing_position_definition_normal_offset[(offsetof(firing_position_definition, normal) == 0x1c) ? 1 : -1];
typedef char firing_position_definition_facing_offset[(offsetof(firing_position_definition, facing) == 0x24) ? 1 : -1];

#endif // AI_FIRING_POSITION_DEFINITION_H
