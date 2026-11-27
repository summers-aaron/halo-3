#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_TRACKER_VIEW_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_TRACKER_VIEW_H
#include <stddef.h>
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_and_assemble_match_internals.h"
// Partial typed view of the life-cycle allocation. Handler offsets are attested
// by the compile-time checks below.
struct s_life_cycle_tracker_view
{
    union { unsigned __int64 initialized_storage; unsigned char initialized; };
    int mode;
    unsigned char manager_and_preceding_handlers[0x9af0 - 12];
    c_life_cycle_state_handler_matchmaking_find_match find_match;
    c_life_cycle_state_handler_matchmaking_find_and_assemble_match find_and_assemble_match;
};
typedef char tracker_find_match_offset[(offsetof(s_life_cycle_tracker_view, find_match) == 0x9af0) ? 1 : -1];
typedef char tracker_find_assemble_offset[(sizeof(void *) != 4 || ((offsetof(s_life_cycle_tracker_view, find_and_assemble_match) == 0xe3f0))) ? 1 : -1];
#endif
