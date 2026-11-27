#ifndef AI_BEHAVIOR_POSTURE_INFO_TYPES_H
#define AI_BEHAVIOR_POSTURE_INFO_TYPES_H

#include <stddef.h>

// behavior.cpp owns the 20-byte posture records. actor_get_posture searches the
// name at +4; unit_update_aiming reads peek_type at +8 from the same table.
struct behavior_posture_info
{
    long index;
    long name;
    long peek_type;
    short duration;
    bool locked;
    union {
        char raw[5];
        struct {
            char unaware;
            union { unsigned char disallow; bool automatic_exit_blocked; };
            char spare[3];
        };
    };
};

typedef char behavior_posture_info_size[(sizeof(behavior_posture_info) == 0x14) ? 1 : -1];
typedef char behavior_posture_info_name_offset[(offsetof(behavior_posture_info, name) == 4) ? 1 : -1];
typedef char behavior_posture_info_peek_type_offset[(offsetof(behavior_posture_info, peek_type) == 8) ? 1 : -1];
typedef char behavior_posture_info_disallow_offset[(offsetof(behavior_posture_info, disallow) == 0x10) ? 1 : -1];
typedef char behavior_posture_info_locked_offset[(offsetof(behavior_posture_info, locked) == 0xe) ? 1 : -1];

extern "C" behavior_posture_info k_behavior_posture_info[];

#endif // AI_BEHAVIOR_POSTURE_INFO_TYPES_H
