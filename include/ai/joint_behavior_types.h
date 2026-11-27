#ifndef AI_JOINT_BEHAVIOR_TYPES_H
#define AI_JOINT_BEHAVIOR_TYPES_H

#include <stddef.h>

// Actor invitation entry. The type field uses 0xffff for none.
struct invitation_data
{
    short type;
    short slot;
    long mode;
    long handle;
    long time;
};

typedef char invitation_data_size[(sizeof(invitation_data) == 0x10) ? 1 : -1];
typedef char invitation_data_slot_offset[((offsetof(invitation_data, slot)) == 2) ? 1 : -1];
typedef char invitation_data_handle_offset[((offsetof(invitation_data, handle)) == 8) ? 1 : -1];
typedef char invitation_data_time_offset[((offsetof(invitation_data, time)) == 0xc) ? 1 : -1];

#endif // AI_JOINT_BEHAVIOR_TYPES_H
