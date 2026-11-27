#ifndef AI_PROP_TYPE_INFO_H
#define AI_PROP_TYPE_INFO_H

#include <stddef.h>

// Prop type table entry: refresh rate, kind, state-update policy, and tracking policy.
struct prop_type {
    short pad0;
    short refresh_rate;
    short kind;
    unsigned short state_update_mode;
    short tracking_mode;
    short pad0a;
    long pad0c;
};
extern "C" const prop_type k_prop_type_info[];

#if defined(_XBOX)
typedef char prop_type_refresh_rate_check[(offsetof(prop_type, refresh_rate) == 2) ? 1 : -1];
typedef char prop_type_kind_check[(offsetof(prop_type, kind) == 4) ? 1 : -1];
typedef char prop_type_state_update_mode_check[(offsetof(prop_type, state_update_mode) == 6) ? 1 : -1];
typedef char prop_type_tracking_mode_check[(offsetof(prop_type, tracking_mode) == 8) ? 1 : -1];
typedef char prop_type_size_check[(sizeof(prop_type) == 0x10) ? 1 : -1];
#endif

#endif // AI_PROP_TYPE_INFO_H
