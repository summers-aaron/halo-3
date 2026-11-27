#ifndef AI_OBJECTIVE_RUNTIME_H
#define AI_OBJECTIVE_RUNTIME_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "cseries/cseries_wrapped.h"
#include "ai/task_group_types.h"

enum e_task_record_flags
{
    _task_record_active_latched = 0,
    _task_record_exhausted = 1,
    _task_record_broken = 2,
    _task_record_script_executed = 3,
    _task_record_script_pending = 4,
    _task_record_activated = 5,
    _task_record_occupied = 6,
    _task_record_dialogue_spoken = 7,
    _task_record_dialogue_pending = 8,
    _task_record_search_allowed = 9,
    _task_record_search_requested = 10,
    _task_record_engage_allowed = 11
};

// Runtime records, distinct from the scenario s_objective and s_task tag data.
// Objective records are 10 bytes and task records are 56 bytes.
struct s_objective_record
{
    unsigned short identifier;
    short first_task;
    short first_squad_index;
    short first_unassigned_squad_index;
    union
    {
        short last;
        struct { bool changed; };
    };
};

struct s_task_record
{
    long leader;
    short deaths;
    short lowest_rank;
    short highest_rank;
    c_flags_no_init<e_task_record_flags, short, 12> flags;
    long break_time;
    long last_search_time; // behavior search cooldown.
    long engage_time;
    long grenade_time;
    short count;
    short body_count;
    long position_time;
    real_point3d position;
    s_task_group group;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char objective_record_size[(sizeof(s_objective_record) == 10) ? 1 : -1];
typedef char task_record_size[(sizeof(s_task_record) == 56) ? 1 : -1];
typedef char task_record_group_offset[(offsetof(s_task_record, group) == 0x30) ? 1 : -1];
#endif

#endif // AI_OBJECTIVE_RUNTIME_H
