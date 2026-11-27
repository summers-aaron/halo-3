#ifndef NETWORKING_ONLINE_FILE_SHARE_SLOT_TYPES_H
#define NETWORKING_ONLINE_FILE_SHARE_SLOT_TYPES_H
#include "networking/online/online_file_work_item_types.h"
#include <stddef.h>

struct s_file_share_slot {
    long state;
    char filename[33];
    s_online_file_description description;
};

typedef char file_share_slot_layout_size[(sizeof(s_file_share_slot) == 0x130) ? 1 : -1];
typedef char file_share_slot_layout_description[(offsetof(s_file_share_slot, description) == 0x28) ? 1 : -1];
#endif
