#ifndef AI_RESERVED_HINTS_H
#define AI_RESERVED_HINTS_H

#include <stddef.h>

// actor_moving_compute_reserved_hints fills four hint reservations.
// path_state_new copies this 18-byte list to path_state +0x280d8; the separate
// 242-byte flying-area evaluation cache starts at +0x280ea.
struct actor_moving_reserved_hint
{
    short hint_index;
    signed char structure_bsp_index;
    signed char count;
};

struct actor_moving_reserved_hints
{
    actor_moving_reserved_hint hints[4];
    short count;
};

typedef char reserved_hint_size[(sizeof(actor_moving_reserved_hint) == 4) ? 1 : -1];
typedef char reserved_hints_size[(sizeof(actor_moving_reserved_hints) == 18) ? 1 : -1];
typedef char reserved_hints_alignment[(__alignof(actor_moving_reserved_hints) == 2) ? 1 : -1];
typedef char reserved_hints_count_offset[((offsetof(actor_moving_reserved_hints, count)) == 16) ? 1 : -1];

#endif // AI_RESERVED_HINTS_H
