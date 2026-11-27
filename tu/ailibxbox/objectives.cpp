// ailibxbox/objectives.obj: from the records 0xa9b38 0xa9c00 0xa9d08 0xa9dd0 0xaa188 0xaa268 0xaa338 0xaa420 0xaa500 0xaa658 0xaaa70 0xaab70 0xb0178 0x1ba860 0x1be748 0x1be800 0x50d000 0x50d018 0x50d028 0x50d160 0x50d248 0x50d5a8 0x50d890 0x50dbe0 0x50ddb8 0x50dec8 0x50df90 0x50e038 0x50ea98 0x50eb78 0x50ec78 0x50eef0 0x50f640 0x50f880 0x50f900 0x50fa20 0x5103b8 0x510700 0x5107e8 0x5109f8 0x510ad0 0x510c10 0x510e20 0x510f20 0x510f88 0x511550 0x511808 0x511920 0x511af8 0x511c50 0x511d50 0x511e60 0x511fb0 0x512088 0x512108 0x512190 0x512230 0x512250 0x5122f0 0x512308
// @flags /TP /O2
#include "ai/squad_data.h"
#include "ai/ai_information_data.h"
#include "units/vehicle_definitions.h"
#include "ai/actor_type_flood_types.h"
#include "game/game_engine_team.h"
#include "ai/behavior_stimulus_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "ai/squad_types.h"
#include "ai/actor_globals.h"
#include "ai/actor_datum.h"
#include "scenario/scenario_tag.h"
#include "memory/tls_data_arrays.h"
#include "ai/ai_point3d.h"
#include "cache/restricted_memory.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ai/actors.h"
#include "ai/ai_dialogue.h"
#include "ai/actor_stimulus.h"
#include "cache/restricted_memory_regions.h"
#include "motor/motor_animation.h"
#include "ai/actor_type_pureform.h"
#include "ai/ai_script.h"
#include "ai/cl_engine.h"
#include "ai/clump.h"
#include "ai/objectives.h"
#include "ai/prop_dialogue.h"
#include "ai/squads.h"
#include "hs/hs_runtime.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/data_typed_access.h"
#include "game/game_allegiance.h"
#include "ai/task_actor_iterator_types.h"
#include "ai/squad_array_types.h"
#include "cache/cache_files.h"
#include "ai/squad_iterator_types.h"
#include "ai/ai_reference_frame.h"
#include "ai/Zone.h"
#include "ai/objective_definitions.h"
#include "ai/objective_storage.h"
// @address 0x11df770 g_objectives_data
__declspec(thread) s_data_array *g_objectives_data;
// @address 0x11df8dc g_tasks_records
__declspec(thread) s_task_record *g_tasks_records;
#include "game/game_time_types.h"
#include "cache/restricted_memory_layout.h"
#include "cseries/cseries.h"
#include "game/game_allegiance_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
// @address 0xa9dd0 ?task_update_group_state@@YAXFF@Z
// @address 0x5107e8 ?objective_task_break_nearby@@YAXFFFF@Z
// @address 0x1be748 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x512190 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_objectives_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x512230 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_objectives_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x512250 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_records_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x5122f0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_records_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x512308 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_records_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0xaa188 ?task_actor_iterator_new@@YAXFFPAUtask_actor_iterator@@@Z
// @address 0xaa268 ?task_squad_iterator_next@@YAPAUsquad_datum@@PAUs_task_squad_iterator@@@Z
// @address 0xaa338 ?task_actor_iterator_next@@YAPAUactor_datum@@PAUtask_actor_iterator@@@Z
// @address 0xaaa70 ?task_iterator_next@@YAPAUs_task@@PAUs_task_iterator@@@Z
// @address 0xaab70 ?task_squad_iterator_new@@YAXPAUs_task_squad_iterator@@FF_N1@Z
// @address 0xb0178 ?squad_valid_task@@YA_NPAUsquad_datum@@@Z
// @address 0x1ba860 ?__tls_set_g_tasks_data_allocator@@YAXPAX@Z
// @address 0x1be800 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x50d160 objective_handle_death
// @address 0x50ddb8 ?pick_a_squad_to_eject@@YAFFFF@Z
// @address 0x50df90 ?squad_list_add@@YA_NFW4e_squad_source@@PAUs_task_info@@PAUs_squad_info@@PAF@Z
// @address 0x5109f8 ?objective_task_request_engage@@YAXFF_N@Z
// @address 0x511c50 ?ai_scripting_set_task_condition@@YA_NJ_N@Z
// @address 0x511d50 ?ai_scripting_leadership@@YA_NJ@Z
// @address 0x511e60 ?ai_scripting_leadership_all@@YA_NJ@Z
// @address 0x512088 task_actor_iterator_get_next_squad
// @address 0x50dbe0 ?decide_task_group@@YA_NFFPAUs_task_info@@PAMPAJ@Z
// @address 0x50dec8 ?eject_squad@@YAXFFFPAUs_task_info@@@Z
// @address 0x50eb78 ?disallowed_tasks_unassign_squads@@YAXFFPAUs_task_info@@PAKPAUs_squad_info@@PAF@Z
// @address 0x50f880 ?task_get_score@@YAMFFFFPAUs_task_info@@PAMPAJ@Z
// @address 0x50f900 ?task_get_current_count@@YAFFFFMPAUs_squad_info@@FPAUs_task_info@@PAMPAJ@Z
// @address 0xa9b38 ?objectives_update@@YAXXZ
struct s_task_info;
enum e_squad_source;
struct s_squad_info;

struct s_task_info;


struct s_task_squad_iterator
{
    short squad_index;                 // 0
    short next_squad_index;            // 2
    short task_index;                  // 4
    union { bool current_tasks_only; bool include_children; };   // 6
    unsigned long task_flags[4];       // 8
};


struct s_task_iterator
{
    short objective_index;             // 0
    short depth;                       // 2
    short next_task_index;             // 4
    short task_index;                  // 6
};


enum e_squad_source { _squad_source_dummy = 0, _squad_source_disallowed_task = 1, _squad_source_lower_priority = 2 };
struct s_task_info { short squad_count; short assignment_count; short squad_spawn_count; bool full; };
struct s_squad_info
{
    short squad_index;
    short assigned_task_index;
    short task_index;
    short squad_spawn_count;
    e_squad_source source;
    bool unassigned;
};


// @rdata 0x82024078
static const float objectives_task_initial_score = 100000.0f;
// @rdata 0x8202407c
static const float objectives_task_filter_penalty = 24.0f;
namespace { static __forceinline bool task_has_vehicle_filter(const s_task *task); }
#include "ai/objectives_runtime.h"


#include "ai/objective_access.h"
#include "ai/ai_math_constants.h"
#include "ai/dialogue_event.h"
#include "ai/task_group_state.h"
#include "math/real_math_constants.h"
#include "cache/restricted_memory_registration.h"
#include "objects/object_ai_properties.h"
#include "objects/object_hierarchy.h"
#include "ai/squad_hierarchy.h"
#include "objects/damage_globals.h"
#include "hs/hs.h"
#include "hs/hs_value_types.h"
#include "ai/objective_task_filters.h"
static __forceinline s_objective_record *objective_record_get_absolute(s_data_array *data,
    long absolute_index) { return &((s_objective_record *)data->data)[absolute_index]; }


static __forceinline s_task *objective_get_task_direct(short objective_index, short task_index)
{
    s_objective *objective = (s_objective *)global_scenario->objectives.address + objective_index;
    return (s_task *)objective->tasks.address + task_index;
}

static __forceinline void task_iterator_new(s_task_iterator *iterator, short objective_index, short task_index)
{
    s_task *task = objective_get_task_direct(objective_index, task_index);
    iterator->objective_index = objective_index;
    iterator->next_task_index = task->first_child_index;
    iterator->task_index = -1;
    iterator->depth = 0;
}


static __forceinline long objective_task_get_leader(short objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    return record->leader;
}

static __forceinline s_objective *objective_get_e60(short index)
{
    return (s_objective *)tag_block_get_element_with_size(&global_scenario->objectives, index, sizeof(s_objective));
}

static __forceinline s_task *objective_get_task_e60(short objective_index, short task_index)
{
    s_objective *o = objective_get_e60(objective_index);
    return (s_task *)tag_block_get_element_with_size(&o->tasks, task_index, sizeof(s_task));
}

static __forceinline s_task_record *objective_get_task_record_e60(short objective_index, short task_index)
{
    s_objective_record *o = &((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index];
    short absolute = o->first_task + task_index;
    return &((s_task_record *)g_tasks_records)[absolute];
}

static __forceinline long objective_task_get_leader_e60(short objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record_e60(objective_index, task_index);
    return record->leader;
}

static __forceinline void task_iterator_new_e60(s_task_iterator *iterator, short objective_index, short task_index)
{
    s_task *t = objective_get_task_e60(objective_index, task_index);
    iterator->objective_index = objective_index;
    iterator->depth = 0;
    iterator->next_task_index = t->first_child_index;
    iterator->task_index = -1;
}

void objectives_update(void)
{
    scenario *scenario_data = global_scenario;
    if (scenario_data->objectives.count > 0)
    {
        short index = 0;
        do
        {
            objective_update(index);
            index = index + 1;
        } while (index < scenario_data->objectives.count);
        short remaining = (short)scenario_data->objectives.count;
        bool decided = false;
        while (remaining > 0 && !decided)
        {
            ai_globals_type *globals = ai_globals;
            short current = globals->next_decision_index;
            if (current < 0 || current >= scenario_data->objectives.count)
                globals->next_decision_index = 0;
            decided = objective_decide(ai_globals->next_decision_index);
            remaining = remaining - 1;
            ++ai_globals->next_decision_index;
        }
    }
}

// 0xa9d08

// match: 0xa9d08: shared game_team_is_enemy stays out of line and resizes task_get_movement.
static __forceinline bool objectives_game_team_is_enemy(long team_a, long team_b)
{
    bool result = true;
    long mode = game_globals->options.game_mode;
    if (mode == 2) {
        result = ::game_engine_team_is_enemy((e_game_team)team_a, (e_game_team)team_b);
    } else if (team_a == team_b) {
        result = false;
    } else if (team_a != NONE && team_b != NONE && mode == 1 && (unsigned long)team_a <= 15 && (unsigned long)team_b <= 15) {
        long bit = team_a * 16 + team_b;
        result = !TEST_BIT(tls_game_allegiance_globals()->enemy_bits[bit >> 5], bit & 31);
    }
    return result;
}

// @address 0xa9d08 task_get_movement
extern "C" long task_get_movement(short team, s_task *task)
{
    long movement = task->movement;
    if (movement == 0)
        movement = objectives_game_team_is_enemy(1, team) ? 2 : 1;
    return movement;
}

static __forceinline long task_update_group_state_game_time_get()
{
    return ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time;
}
void task_update_group_state(short objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    unsigned char all_actors_changed = true;
    short actor_count = 0;
    float state_fractions[14];
    float cumulative_fractions[14];
    for (long state_index = 0; state_index < 14; ++state_index)
    {
        state_fractions[state_index] = 0.0f;
        cumulative_fractions[state_index] = 0.0f;
    }
    task_actor_iterator iterator;
    task_actor_iterator_new(objective_index, task_index, &iterator);
    actor_datum *actor;
    while ((actor = task_actor_iterator_next(&iterator)) != NULL)
    {
        short state = actor->situation.best_prop_class;
        for (long state_index = 0; state_index <= state; ++state_index)
            cumulative_fractions[state_index] += 1.0;
        state_fractions[state] += 1.0;
        ++actor_count;
        all_actors_changed &= actor->suppress_combat;
    }
    if (actor_count > 0)
    {
        for (long state_index = 0; state_index < 14; ++state_index)
        {
            state_fractions[state_index] /= (float)actor_count;
            cumulative_fractions[state_index] /= (float)actor_count;
        }
    }
    if (!(record->group.flags & 1) && cumulative_fractions[7] > 0.0f && !all_actors_changed)
        record->group.flags |= 1;
    s_task_group *group = &record->group;
    short new_state = task_group_compute_next_state(group, state_fractions, cumulative_fractions);
    if (new_state != NONE && (new_state >= group->state || record->count <= 0 || group->state != 1))
    {
        group->state = new_state;
        group->time = task_update_group_state_game_time_get();
        if (new_state == 0)
            group->flags &= ~1;
    }
}

struct s_dialogue_object;
// @address 0x50d5a8 ?task_update_behaviors@@YAXJF@Z
void task_update_behaviors(long objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record((short)objective_index, task_index);
    if (record->leader != NONE)
    {
        if (record->flags.test(_task_record_engage_allowed))
        {
            if (record->group.state <= 1)
            {
                record->flags.set(_task_record_engage_allowed, false);
                record->engage_time = NONE;
                task_actor_iterator iterator;
                task_actor_iterator_new((short)objective_index, task_index, &iterator);
                if (task_actor_iterator_next(&iterator) != NULL)
                {
                    do
                    {
                        datum_get<actor_datum>(actor_data, iterator.actor_index)->suppress_combat = true;
                    } while (task_actor_iterator_next(&iterator) != NULL);
                }
            }
        }
        else
        {
            long engage_time = record->engage_time;
            if (engage_time != NONE && TLS_GAME_TIME_GLOBALS->time - engage_time >
#if defined(_XBOX) && !defined(__clang__)
                (long)((float)(double)(int)(short)TLS_GAME_TIME_GLOBALS->ticks_per_second * AI_REAL_CONSTANTS[1350] + AI_REAL_CONSTANTS[0]))
#else
                (long)((float)(double)(int)(short)TLS_GAME_TIME_GLOBALS->ticks_per_second * 2.0f + AI_REAL_CONSTANTS[0]))
#endif
            {
                record->flags.set(_task_record_engage_allowed, true);
                unsigned short state = (unsigned short)record->group.state;
                record->engage_time = NONE;
                if (state == 3)
                {
                    long leader = record->leader;
                    actor_datum *actor = datum_get<actor_datum>(actor_data, leader);
                    long unit_index = actor->unit_index;
                    if (unit_index != NONE)
                    {
                        long dialogue_unit_index = datum_get<actor_datum>(actor_data, leader)->unit_index;
                        if (dialogue_unit_index != NONE)
                        {
                            c_dialogue_event event(82, dialogue_unit_index, NONE, NONE, NONE, 0);
                            event.clump_index = 0;
                            ai_dialogue_event(&event);
                        }
                        if (!motor_animation_busy(actor->unit_index))
                            ::actor_impulse_submit(leader, 48, 0, 0);
                    }
                }
            }
        }

        {
            if (record->flags.test(_task_record_search_allowed))
            {
                if (record->group.state != 2)
                {
                    record->flags.set(_task_record_search_allowed, false);
                    record->flags.set(_task_record_search_requested, false);
                }
            }
            else if (record->flags.test(_task_record_search_requested) && record->group.state <= 2)
            {
                record->flags.set(_task_record_search_allowed, true);
                record->flags.set(_task_record_search_requested, false);
                long leader = record->leader;
                long unit_index = datum_get<actor_datum>(actor_data, leader)->unit_index;
                if (unit_index != NONE && !motor_animation_busy(unit_index))
                    ::actor_impulse_submit(leader, 47, 0, 0);
            }
        }

    }
    if (record->count > 0 || record->group.state > 0)
        task_update_group_state((short)objective_index, task_index);
}

// 0xaa188
// match: 0xaa188: __restrict on iterator preserves task_actor_iterator_new's scheduling
void task_actor_iterator_new(short objective_index, short task_index, task_actor_iterator *__restrict iterator)
{
    bool valid = false;
    iterator->done = false;
    if (ai_globals->enabled)
    {
        s_objective_record *objective = objective_record_get_absolute(g_objectives_data, (unsigned short)objective_index);
        iterator->objective_index = objective_index;
        iterator->task_index = task_index;
        iterator->actor_index = NONE;
        iterator->squad_index = NONE;
        iterator->next_squad_index = objective->first_squad_index;
        if (task_actor_iterator_get_next_squad(iterator))
        {
            squad_datum *squad = datum_get<squad_datum>(squad_data, iterator->squad_index);
            iterator->next_actor_index = squad->first_actor_index;
            valid = true;
        }
    }
    if (!valid)
        iterator->done = true;
}

// match: 0xaa268: __restrict on iterator preserves task_squad_iterator_next's register allocation
squad_datum *task_squad_iterator_next(s_task_squad_iterator *__restrict iterator)
{
    squad_datum *squad = NULL;
    iterator->squad_index = NONE;
    while (iterator->next_squad_index != NONE)
    {
        squad = datum_get<squad_datum>(squad_data, iterator->next_squad_index);
        long task_index = squad->task_index;
        if (!iterator->current_tasks_only && squad->temporary)
            task_index = squad->previous_task_index;
        if (task_index == iterator->task_index || task_index != NONE && TEST_BIT(iterator->task_flags[task_index >> 5], task_index & 31))
        {
            iterator->squad_index = iterator->next_squad_index;
            iterator->next_squad_index = squad->next_index;
            break;
        }
        else
        {
            iterator->next_squad_index = squad->next_index;
            squad = NULL;
        }
    }
    return squad;
}

// 0xaa338
// match: 0xaa338: __restrict on iterator preserves task_actor_iterator_next's register allocation
actor_datum *task_actor_iterator_next(task_actor_iterator *__restrict iterator)
{
    actor_datum *actor = NULL;
    if (iterator->done)
    {
        iterator->actor_index = NONE;
    }
    else
    {
        while (!actor)
        {
            if (iterator->next_actor_index != NONE)
            {
                iterator->actor_index = iterator->next_actor_index;
                actor = datum_get<actor_datum>(actor_data, iterator->actor_index);
                iterator->next_actor_index = actor->next_squad_actor_index;
            }
            else if (task_actor_iterator_get_next_squad(iterator))
            {
                squad_datum *squad = datum_get<squad_datum>(squad_data, iterator->squad_index);
                iterator->next_actor_index = squad->first_actor_index;
            }
            else
            {
                iterator->done = true;
                iterator->actor_index = NONE;
                break;
            }
        }
    }
    return actor;
}

// @address 0x72d288 k_task_group_state_definitions
s_group_state_definition k_task_group_state_definitions[] =
{
    { 0, 0, 0 },
    { 1, 0, clump_timed_entry_is_recent },
    { 2, group_state_searching_override, clump_group_state_get_search_flag },
    { 3, group_state_attacking_override, group_state_attacking_next_state },
};

// 0xaa420
// @address 0xaa420 task_group_compute_next_state
extern "C" short task_group_compute_next_state(s_task_group *group, float *a, float *b)
{
    short new_state = NONE;
    for (long state = 3; state > group->state; state--) {
        if (k_task_group_state_definitions[state].override && k_task_group_state_definitions[state].override(a, b)) {
            new_state = (short)state;
            break;
        }
    }
    if (new_state == NONE && k_task_group_state_definitions[group->state].next_state)
        return k_task_group_state_definitions[group->state].next_state(group, a, b);
    return new_state;
}

// match: 0xaaa70: __restrict on iterator preserves task_iterator_next's scheduling
s_task *task_iterator_next(s_task_iterator *__restrict iterator)
{
    s_task *task = NULL;
    iterator->task_index = NONE;
    if (iterator->next_task_index != NONE)
    {
        task = objective_get_task(iterator->objective_index, iterator->next_task_index);
        iterator->task_index = iterator->next_task_index;
        if (task->first_child_index != NONE)
        {
            iterator->next_task_index = task->first_child_index;
            iterator->depth++;
        }
        else if (task->next_sibling_index != NONE)
        {
            iterator->next_task_index = task->next_sibling_index;
        }
        else
        {
            short parent_index = task->parent_index;
            while (true)
            {
                iterator->depth--;
                if (iterator->depth >= 0)
                {
                    s_task *parent = objective_get_task(iterator->objective_index, parent_index);
                    if (parent->next_sibling_index != NONE)
                    {
                        iterator->next_task_index = parent->next_sibling_index;
                        break;
                    }
                    else
                    {
                        parent_index = parent->parent_index;
                    }
                }
                else
                {
                    iterator->next_task_index = NONE;
                    break;
                }
            }
        }
    }
    return task;
}

void task_squad_iterator_new(
    // match: 0xaab70: __restrict on iterator preserves task_squad_iterator_new's store order
    s_task_squad_iterator *__restrict iterator, short objective_index, short task_index, bool include_child_tasks, bool include_something)
{
    s_objective_record *objective = objective_record_get_absolute(g_objectives_data, (unsigned short)objective_index);
    iterator->squad_index = -1;
    iterator->task_index = task_index;
    iterator->include_children = include_something;
    iterator->next_squad_index = task_index == -1 ? objective->first_unassigned_squad_index : objective->first_squad_index;
    memset(iterator->task_flags, 0, sizeof(iterator->task_flags));
    if (task_index != -1 && include_child_tasks) {
        s_task_iterator task_iterator;
        task_iterator_new(&task_iterator, objective_index, task_index);
        while (task_iterator_next(&task_iterator)) {
            iterator->task_flags[task_iterator.task_index >> 5] |= 1 << (task_iterator.task_index & 31);
        }
    }
}

bool squad_valid_task(squad_datum *squad)
{
    bool valid = false;
    short block_index = squad->objective_index;
    if (block_index >= 0 && block_index < global_scenario->objectives.count) {
        int entry_index = squad->task_index;
        if (entry_index != -1) {
            valid = entry_index >= 0 && entry_index < ((s_objective *)global_scenario->objectives.address)[block_index].tasks.count;
        }
    }
    return valid;
}

void __tls_set_g_tasks_data_allocator(void *value)
{
    g_tasks_data_tls_address = (long *)value;
}


// @address 0x50d000 ?__tls_set_g_objectives_data_allocator@@YAXPAX@Z
void __tls_set_g_objectives_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_objectives_data, (s_data_array *)object);
}

// @address 0x50d018 ?__tls_set_g_tasks_records_allocator@@YAXPAX@Z
void __tls_set_g_tasks_records_allocator(void *address)
{
    g_tasks_records = (s_task_record *)address;
}

extern "C" void objective_handle_death(short objective_index,short task_index,long actor_index) {
 actor_datum *actor=datum_get<actor_datum>(actor_data,actor_index);
 if(!actor->swarm) {
  scenario *scenario=global_scenario;
  if(objective_index<scenario->objectives.count) {
   s_objective *objective=(s_objective*)tag_block_get_element_with_size(&scenario->objectives,objective_index,sizeof(s_objective));
   while(task_index!=-1) {
    if(task_index<objective->tasks.count) {
     s_task *task=objective_get_task(objective_index,task_index);
     s_task_record *record=objective_get_task_record(objective_index,task_index);
     record->deaths++;
     task_index=task->parent_index;
    } else break;
   }
  }
 }
}

// @address 0x50d890 ?objective_decide@@YA_NJ@Z
bool objective_decide(long objective_index)
{
    s_objective *objective = (s_objective *)tag_block_get_element_with_size(&global_scenario->objectives, objective_index, sizeof(s_objective));
    s_objective_record *record = objective_record_get_absolute(g_objectives_data, objective_index & 0xffff);
    bool result = false;
    if (record->changed)
    {
        if (record->first_squad_index != NONE || record->first_unassigned_squad_index != NONE)
        {
            result = true;
            long valid[1047];
            float scores[33500];
            s_task_info task_info[100];
            memset(valid, 0, sizeof(valid));
            memset(task_info, 0, sizeof(task_info));
            long task_count = objective->tasks.count;
            for (short task_index = 0; task_index < task_count; ++task_index)
            {
                s_task_record *task_record = objective_get_task_record((short)objective_index, task_index);
                task_info[task_index].squad_spawn_count = task_record->highest_rank;
                task_info[task_index].full = task_record->flags.test(_task_record_broken);
            }
            for (short task_index = 0; task_index < task_count; ++task_index)
            {
                if (task_info[task_index].squad_spawn_count != NONE)
                {
                    long parent_index = objective_get_task((short)objective_index, task_index)->parent_index;
                    short priority = task_info[task_index].squad_spawn_count;
                    while (parent_index != NONE)
                    {
                        s_task *task = objective_get_task((short)objective_index, parent_index);
                        if (task_info[parent_index].squad_spawn_count != NONE && task_info[parent_index].squad_spawn_count >= priority)
                            break;
                        task_info[parent_index].squad_spawn_count = priority;
                        parent_index = task->parent_index;
                    }
                }
            }
            short squad_index = record->first_squad_index;
            while (squad_index != NONE)
            {
                squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
                short task_index = squad->task_index;
                while (task_index != NONE)
                {
                    s_task *task = objective_get_task(objective, task_index);
                    objective_get_task_record((short)objective_index, task_index)->flags.set(_task_record_occupied, true);
                    task_info[task_index].squad_count += squad->living_count;
                    task_index = task->parent_index;
                }
                squad_index = squad->next_index;
            }
            decide_task_group((short)objective_index, NONE, task_info, scores, valid);
            squad_index = record->first_squad_index;
            while (squad_index != NONE)
            {
                squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
                short next_squad_index = squad->next_index;
                if (squad->temporary)
                    finalize_task_change(squad_index);
                squad_index = next_squad_index;
            }
            squad_index = record->first_unassigned_squad_index;
            while (squad_index != NONE)
            {
                squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
                short next_squad_index = squad->next_index;
                if (squad->temporary)
                    finalize_task_change(squad_index);
                squad_index = next_squad_index;
            }
        }
    }
    return result;
}

bool decide_task_group(short objective_index, short parent_index, s_task_info *info, float *scores, long *valid)
{
    short current_priority_index = NONE;
    short current_priority = NONE;
    short current_count = 0;
    short first_task;
    if (parent_index == NONE)
        first_task = objective_get_e60(objective_index)->first_task_index;
    else
        first_task = objective_get_task(objective_index, parent_index)->first_child_index;
    short index = first_task;
    while (true)
    {
        s_task *task = NULL;
        short priority = -2;
        if (index != NONE)
        {
            task = objective_get_task(objective_index, index);
            priority = task->priority;
        }
        if (current_priority == NONE || current_priority != priority)
        {
            if (current_count > 0)
                decide_tasks_priority_group(objective_index, parent_index, current_priority_index, current_count, index, info, scores, valid);
            current_priority_index = index;
            current_priority = priority;
            current_count = 1;
        }
        else
            current_count++;
        if (task == NULL)
            break;
        index = task->next_sibling_index;
    }
    if (parent_index != NONE)
    {
        s_task *parent = objective_get_task(objective_index, parent_index);
        if ((_rotl((short)parent->flags, 30) & 1) != 0)
        {
            s_objective_record *objective = objective_record_get_absolute(g_objectives_data, (unsigned short)objective_index);
            long squad_index = objective->first_squad_index;
            while (squad_index != NONE)
            {
                squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
                short next_index = squad->next_index;
                if (squad->task_index == parent_index)
                    ::squad_set_objective(squad_index, objective_index, parent->parent_index, true);
                squad_index = next_index;
            }
        }
    }
    return true;
}

short pick_a_squad_to_eject(short objective_index, short task_index, short total)
{
    s_task *task = objective_get_task(objective_index, task_index);
    short chosen = NONE;
    short minimum = 32767;
    short largest = NONE;
    short maximum = 0;
    s_task_squad_iterator iterator;
    task_squad_iterator_new(&iterator, objective_index, task_index, true, true);
    squad_datum *squad;
    while ((squad = task_squad_iterator_next(&iterator)) != NULL)
    {
        if (squad->living_count > maximum)
        {
            largest = iterator.squad_index;
            maximum = squad->living_count;
        }
        if (squad->living_count > task->capacity)
        {
            chosen = iterator.squad_index;
            break;
        }
        else if (total - squad->living_count <= task->capacity && squad->living_count < minimum)
        {
            chosen = iterator.squad_index;
            minimum = squad->living_count;
        }
    }
    if (chosen == NONE && largest != NONE)
        chosen = largest;
    return chosen;
}

void eject_squad(short squad_index, short objective_index, short root_task_index, s_task_info *task_info)
{
    s_objective *objective = objective_get_e60(objective_index);
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    short task_index = squad->task_index;
    while (task_index != NONE && task_index != root_task_index)
    {
        s_task *task = objective_get_task(objective, task_index);
        task_info[task_index].squad_count = task_info[task_index].squad_count - squad->living_count;
        task_index = task->parent_index;
    }
    ::squad_set_objective(squad_index, objective_index, root_task_index, true);
}

bool squad_list_add(short squad_index, enum e_squad_source source, struct s_task_info *task_info, struct s_squad_info *squad_list,
    // match: 0x50df90: __restrict on squad_count preserves squad_list_add's register allocation
    short *__restrict squad_count)
{
    s_squad_info *squad_info = &squad_list[*squad_count];
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    bool added = false;
    if (squad->living_count > 0 && (squad->task_index == NONE || !task_info[squad->task_index].full))
    {
        squad_info->squad_index = squad_index;
        squad_info->assigned_task_index = NONE;
        squad_info->task_index = squad->task_index;
        squad_info->source = source;
        squad_info->squad_spawn_count = squad->squad_spawn_count;
        (*squad_count)++;
        added = true;
    }
    return added;
}


static __forceinline void objectives_priority_union_task_flags(const unsigned long *left, const unsigned long *right, unsigned long *result)
{
    for (long word_index = 2; word_index >= 0; --word_index)
        result[word_index] = left[word_index] | right[word_index];
}
static __forceinline void objectives_priority_remove_squad(s_task_info *info, short task_index, const long &living_count)
{
    info[task_index].squad_count -= living_count;
}

// @address 0x50e038 ?decide_tasks_priority_group@@YA_NFFFFFPAUs_task_info@@PAMPAJ@Z
bool decide_tasks_priority_group(short objective_index, short parent_index, short current_priority_index, short current_count, short index,
    s_task_info *info, float *scores, long *valid)
{
    short squad_count = 0;
    short current_squad_count = 0;
    bool result = true;
    unsigned long disallowed_tasks[4];
    unsigned long reassigned_squads[11];
    s_squad_info squad_list[335];
    c_static_flags_no_init<100> exhausted_snapshot;
    {
    unsigned long exhausted_tasks[4];
    memset(disallowed_tasks, 0, sizeof(disallowed_tasks));
    memset(exhausted_tasks, 0, sizeof(exhausted_tasks));
    memset(reassigned_squads, 0, sizeof(reassigned_squads));
    short task_index = current_priority_index;
    for (short task_count = 0; task_index != NONE && task_count < current_count; ++task_count)
    {
        s_task *task = objective_get_task(objective_index, task_index);
        s_task_info *task_info = &info[task_index];
        if (objective_task_test_activation(objective_index, task_index, task_info, true, NULL) && !task_info->full)
        {
            if (task->filter > 0 && ((unsigned short)task->filter_flags & 1U))
            {
                s_task_record *record = &g_tasks_records[(short)(
        ((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index].first_task + task_index)];
                s_task_squad_iterator iterator;
                task_squad_iterator_new(&iterator, objective_index, task_index, true, true);
                while (task_squad_iterator_next(&iterator))
                {
                    short squad_index = iterator.squad_index;
                    if (!test_filter(squad_index, task, task_info, record, NONE, true, NULL))
                        eject_squad(squad_index, objective_index, parent_index, info);
                }
            }
            while (task->capacity > 0 && task_info->squad_count > task->capacity)
            {
                short squad_index = pick_a_squad_to_eject(objective_index, task_index, task_info->squad_count);
                if (squad_index == NONE)
                    break;
                eject_squad(squad_index, objective_index, parent_index, info);
            }
            current_squad_count += task_info->squad_count;
        }
        else if (!task_info->full)
            disallow_task(objective_index, task_index, disallowed_tasks);
        task_index = task->next_sibling_index;
    }
    {
        s_task_squad_iterator iterator;
        task_squad_iterator_new(&iterator, objective_index, parent_index, false, true);
        while (task_squad_iterator_next(&iterator))
            squad_list_add(iterator.squad_index, _squad_source_dummy, info, squad_list, &squad_count);
    }
    disallowed_tasks_unassign_squads(objective_index, parent_index, info, disallowed_tasks, squad_list, &squad_count);
    task_index = current_priority_index;
    for (short task_count = 0; task_index != NONE && task_count < current_count; ++task_count)
    {
        s_task *task = objective_get_task(objective_index, task_index);
        s_task_record *record = &g_tasks_records[(short)(
            ((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index].first_task + task_index)];
        if (info[task_index].full)
            disallow_task(objective_index, task_index, disallowed_tasks);
        else if (((_rotl(task->signed_flags, 29) & 1) != 0 && record->flags.test(_task_record_occupied)) ||
            (task->maximum_body_count > 0 && record->body_count >= task->maximum_body_count))
            disallow_task(objective_index, task_index, exhausted_tasks);
        task_index = task->next_sibling_index;
    }
    short lower_priority_task_index = index;
    while (lower_priority_task_index != NONE)
    {
        s_task *task = objective_get_task(objective_index, lower_priority_task_index);
        s_task_squad_iterator iterator;
        task_squad_iterator_new(&iterator, objective_index, lower_priority_task_index, true, true);
        while (task_squad_iterator_next(&iterator))
            squad_list_add(iterator.squad_index, _squad_source_lower_priority, info, squad_list, &squad_count);
        lower_priority_task_index = task->next_sibling_index;
    }
    exhausted_snapshot.m_storage[3] = exhausted_tasks[3];
    memcpy(exhausted_snapshot.m_storage, exhausted_tasks, 3 * sizeof(unsigned long));
    }
    if (squad_count > 0 || current_squad_count > 0)
    {
        short maximum_spawn_count = 1;
        bool done = false;
        for (short squad_index = 0; squad_index < squad_count; ++squad_index)
        {
            squad_datum *squad = datum_get<squad_datum>(squad_data, squad_list[squad_index].squad_index);
            if (squad->squad_spawn_count > maximum_spawn_count)
                maximum_spawn_count = squad->squad_spawn_count;
        }
        unsigned long full_tasks[4];
        memset(full_tasks, 0, sizeof(full_tasks));
        do
        {
            unsigned long unavailable_tasks[4];
            unsigned long full_top = full_tasks[3];
            unsigned long merged_top = disallowed_tasks[3];
            merged_top |= full_top;
            disallowed_tasks[3] = merged_top;
            objectives_priority_union_task_flags(disallowed_tasks, full_tasks, disallowed_tasks);
            unavailable_tasks[3] = disallowed_tasks[3] | exhausted_snapshot.m_storage[3];
            unsigned long unavailable_word2 = exhausted_snapshot.m_storage[2];
            unavailable_word2 |= disallowed_tasks[2];
            unavailable_tasks[2] = unavailable_word2;
            unsigned long unavailable_word1 = exhausted_snapshot.m_storage[1];
            unavailable_word1 |= disallowed_tasks[1];
            unavailable_tasks[1] = unavailable_word1;
            unsigned long unavailable_word0 = exhausted_snapshot.m_storage[0];
            unavailable_word0 |= disallowed_tasks[0];
            unavailable_tasks[0] = unavailable_word0;
            for (short squad_index = 0; squad_index < squad_count; ++squad_index)
                squad_list[squad_index].assigned_task_index = NONE;
            short task_index = current_priority_index;
            for (short task_count = 0; task_count < current_count; ++task_count)
            {
                s_task *task = objective_get_task(objective_index, task_index);
                info[task_index].assignment_count = 0;
                task_index = task->next_sibling_index;
            }
            if (find_assignment(objective_index, current_priority_index, current_count, maximum_spawn_count, squad_list, squad_count, info,
                    unavailable_tasks, scores, valid))
            {
                short task_count = 0;
                short search_task_index = current_priority_index;
                short most_empty_task_index = NONE;
                short largest_shortfall = 0;
                for (; search_task_index != NONE && task_count < current_count; ++task_count)
                {
                    s_task *task = objective_get_task(objective_index, search_task_index);
                    if (!TEST_BIT(disallowed_tasks[search_task_index >> 5], search_task_index & 31))
                    {
                        short shortfall = task->minimum_squad_count - info[search_task_index].assignment_count - info[search_task_index].squad_count;
                        if (shortfall > largest_shortfall)
                        {
                            largest_shortfall = shortfall;
                            most_empty_task_index = search_task_index;
                        }
                    }
                    search_task_index = task->next_sibling_index;
                }
                if (most_empty_task_index != NONE)
                {
                    disallow_task(objective_index, most_empty_task_index, full_tasks);
                    disallowed_tasks_unassign_squads(objective_index, parent_index, info, full_tasks, squad_list, &squad_count);
                }
                else
                {
                    for (short squad_index = 0; squad_index < squad_count; ++squad_index)
                    {
                        s_squad_info *squad_info = &squad_list[squad_index];
                        short datum_index = squad_info->squad_index;
                        squad_datum *squad = datum_get<squad_datum>(squad_data, datum_index);
                        if (squad_info->assigned_task_index != NONE)
                        {
                            if ((unsigned short)squad_info->task_index != (unsigned short)NONE)
                            {
                                short parent = squad_info->task_index;
                                while (parent != NONE)
                                {
                                    s_task *task = objective_get_task(objective_index, parent);
                                    objectives_priority_remove_squad(info, parent, squad->living_count);
                                    parent = task->parent_index;
                                }
                            }
                            if (squad_info->source == _squad_source_lower_priority)
                                reassigned_squads[datum_index >> 5] |= 1UL << (datum_index & 31);
                            squad_set_objective(datum_index, objective_index, squad_info->assigned_task_index, true);
                        }
                        else if (squad_info->source == _squad_source_disallowed_task)
                            squad_set_objective(datum_index, squad->objective_index, squad->task_index, true);
                    }
                    done = true;
                }
            }
            else
            {
                result = false;
                done = true;
            }
        } while (!done);
    }
    if (result)
    {
        short task_index = current_priority_index;
        for (short task_count = 0; task_index != NONE && task_count < current_count; ++task_count)
        {
            s_task *task = objective_get_task(objective_index, task_index);
            decide_task_group(objective_index, task_index, info, scores, valid);
            task_index = task->next_sibling_index;
        }
    }
    {
        s_task_squad_iterator iterator;
        task_squad_iterator_new(&iterator, objective_index, parent_index, false, true);
        squad_datum *squad;
        while ((squad = task_squad_iterator_next(&iterator)) != NULL)
        {
            short squad_index = iterator.squad_index;
            if (TEST_BIT(reassigned_squads[squad_index >> 5], squad_index & 31))
            {
                short previous_task_index = squad->previous_task_index;
                squad_set_objective(squad_index, squad->previous_objective_index, previous_task_index, true);
                for (short parent = previous_task_index; parent != NONE && parent != parent_index; )
                {
                    s_task *task = objective_get_task(objective_index, parent);
                    info[parent].squad_count += squad->living_count;
                    parent = task->parent_index;
                }
            }
        }
    }
    return result;
}

void disallowed_tasks_unassign_squads(short objective_index, short root_task_index, s_task_info *task_info,
    unsigned long *disallowed_task_flags, s_squad_info *squad_list, short *squad_count)
{
    s_objective_record *objective = objective_record_get_absolute(g_objectives_data, DATUM_INDEX_TO_ABSOLUTE_INDEX(objective_index));
    long squad_index = objective->first_squad_index;
    while (squad_index != NONE)
    {
        squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
        short next_squad_index = squad->next_index;
        if (TEST_BIT(disallowed_task_flags[squad->task_index >> 5], squad->task_index & 31) && !task_info[squad->task_index].full)
        {
            ::squad_set_objective(squad_index, objective_index, root_task_index, true);
            ::squad_list_add((short)squad_index, _squad_source_disallowed_task, task_info,
                squad_list, (short *)(squad_count));
        }
        squad_index = next_squad_index;
    }
}

float task_get_score(short squad_index, short task_index, short objective_index, short group, s_task_info *info, float *scores, long *valid)
{
    unsigned short index = (unsigned short)objective_index * 100 + (unsigned short)task_index;
    float result;
    if ((valid[index >> 5] & (1u << (index & 31))) != 0)
        result = scores[index];
    else
    {
        result = ::task_score(squad_index, task_index, objective_index, group, info, (float *)(scores), (long *)(valid));
        scores[index] = result;
        valid[index >> 5] |= 1u << (index & 31);
    }
    return result;
}

static __forceinline s_task *objectives3_objective_get_task(s_objective *objective, short task_index)
{
    return (s_task *)tag_block_get_element_with_size(&objective->tasks, task_index, sizeof(s_task));
}
static __forceinline s_task *objectives3_objective_get_task(short objective_index, short task_index)
{
    return objectives3_objective_get_task(
        (s_objective *)tag_block_get_element_with_size(&global_scenario->objectives, objective_index, sizeof(s_objective)), task_index);
}
static __forceinline zone_definition *objectives3_zone_definition_get(scenario *scenario_data, long zone_index)
{
    zone_definition *zone = (zone_definition *)((char *)scenario_data->zones.address + (((unsigned long)zone_index << 6) & 0x3fffc0));
    return zone;
}

static __forceinline float objectives3_child_score(short objective_index, short task_index, short squad_index, short group,
    s_task_info *task_info, float *scores, long *valid, s_task *task, const float &area_score, bool passes)
{
    float result = area_score;
    s_task_iterator iterator;
    iterator.objective_index = objective_index;
    iterator.depth = 0;
    iterator.next_task_index = task->first_child_index;
    iterator.task_index = NONE;
    while (task_iterator_next(&iterator) != NULL)
    {
        float score = task_get_score(objective_index, iterator.task_index, squad_index, group, task_info, scores, valid);
        if (score < result)
            result = score;
    }
    if (!passes)
        return result + objectives_task_filter_penalty;
    return result;
}
// @address 0x50f640 ?task_score@@YAMFFFFPAUs_task_info@@PAMPAJ@Z
float task_score(short objective_index, short task_index, short squad_index, short group, s_task_info *task_info, float *scores, long *valid)
{
    scenario *scenario_data = global_scenario;
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    s_task *task = objectives3_objective_get_task(objective_index, task_index);
    s_task_info *info = &task_info[task_index];
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    float result = objectives_task_initial_score;
    float closest = result;
    bool valid_flag = false;
    bool passes = test_filter(squad_index, task, info, record, group, false, &valid_flag);
    if (!passes && (!valid_flag || task_has_vehicle_filter(objectives3_objective_get_task(objective_index, task_index))))
        return result;
    if (task->areas.count > 0)
    {
        for (short area_index = 0; area_index < task->areas.count; ++area_index)
        {
            squads_area_zone_set_entry *task_area = (squads_area_zone_set_entry *)
                tag_block_get_element_with_size(&task->areas, area_index, sizeof(squads_area_zone_set_entry));
            if ((unsigned short)task_area->zone_index != (unsigned short)NONE)
            {
                zone_definition *zone = objectives3_zone_definition_get(scenario_data, task_area->zone_index);
                if (task_area->area_index >= 0 && task_area->area_index < zone->areas.count)
                {
                    area_definition *area = (area_definition *)tag_block_get_element_with_size(&zone->areas, task_area->area_index, sizeof(area_definition));
                    float distance = ::ai_distance_squared3d(&area->point, &squad->mean_position);
                    if (distance < closest)
                        closest = distance;
                }
            }
        }
    }
    if (closest > 0.0f)
        result = sqrtf(closest);
    return objectives3_child_score(objective_index, task_index, squad_index, group, task_info, scores, valid, task, result, passes);
}
short task_get_current_count(short objective_index, short task_index, short group, float threshold, s_squad_info *squad_info,
    short squad_count, s_task_info *task_info, float *scores, long *valid)
{
    short count = 0;
    for (short index = 0; index < squad_count; ++index)
    {
        s_squad_info *info = &squad_info[index];
        if (info->assigned_task_index == task_index)
        {
            unsigned short score_index = (unsigned short)info->squad_index * 100 + (unsigned short)info->assigned_task_index;
            float score;
            if ((valid[score_index >> 5] & (1u << (score_index & 31))) != 0)
                score = scores[score_index];
            else
            {
                score = task_score(objective_index, info->assigned_task_index, info->squad_index, group, task_info, scores, valid);
                scores[score_index] = score;
                valid[score_index >> 5] |= 1u << (score_index & 31);
            }
            if (score < threshold)
                count = (short)(datum_get<squad_datum>(squad_data, info->squad_index)->living_count + count);
        }
    }
    return count;
}

static const float objectives_task_maximum_score = 3.402823466e+38f;
static __forceinline float find_assignment_get_score(
    short objective_index, short task_index, short squad_index, short group, s_task_info *task_info, float *scores, long *valid)
{
    unsigned short score_index = (unsigned short)squad_index * 100 + (unsigned short)task_index;
    unsigned long mask = 1u << (score_index & 31);
    float score;
    if ((valid[score_index >> 5] & mask) != 0)
        score = scores[score_index];
    else
    {
        score = task_score(objective_index, task_index, squad_index, group, task_info, scores, valid);
        scores[score_index] = score;
        valid[score_index >> 5] |= mask;
    }
    return score;
}
// @address 0x50fa20
bool find_assignment(short objective_index, short first_task_index, short task_count, short group, s_squad_info *squad_list,
    short squad_count, s_task_info *task_info, unsigned long *disallowed_tasks, float *scores, long *valid)
{
    // match: 0x50fa20: value range assumption guides branch lowering
    __assume(squad_list != NULL);
    // match: 0x50fa20: value range assumption guides branch lowering
    __assume(task_info != NULL);
    for (short squad_list_index = 0; squad_list_index < squad_count; ++squad_list_index)
        squad_list[squad_list_index].unassigned = true;

    bool changed;
    do
    {
        changed = false;
        for (short squad_list_index = 0; squad_list_index < squad_count; ++squad_list_index)
        {
            s_squad_info *squad_info = &squad_list[squad_list_index];
            if (!squad_info->unassigned)
                continue;

            float best_score = objectives_task_maximum_score;
            short best_task_index = NONE;
            squad_datum *squad = datum_get<squad_datum>(squad_data, squad_info->squad_index);
            short task_index = first_task_index;
            for (short count = task_count; task_index != NONE && count > 0; --count)
            {
                s_task *task = objective_get_task(objective_index, task_index);
                if (!TEST_BIT(disallowed_tasks[task_index >> 5], task_index & 31))
                {
                    s_task_info *info = &task_info[task_index];
                    bool valid_task = true;
                    float score = find_assignment_get_score(objective_index, task_index, squad_info->squad_index, group, task_info, scores, valid);
                    short current_count = task_get_current_count(objective_index, task_index, group, score, squad_list, squad_count, task_info, scores, valid);
                    if (score >= objectives_task_initial_score)
                        valid_task = false;
                    else if (task->capacity > 0 && info->squad_count + current_count + squad->living_count > task->capacity)
                    {
                        if (task->filter == 1 && ((unsigned short)task->filter_flags & 1U) &&
                            squad->squad_spawn_count > info->squad_spawn_count && current_count + squad->living_count <= task->capacity)
                            valid_task = true;
                        else
                            valid_task = false;
                    }
                    if (valid_task && (best_task_index == NONE || score < best_score))
                    {
                        best_task_index = task_index;
                        best_score = score;
                    }
                }
                task_index = task->next_sibling_index;
            }

            if (best_task_index != NONE)
            {
                s_task_info *best_info = &task_info[best_task_index];
                float score = find_assignment_get_score(objective_index, best_task_index, squad_info->squad_index, group, task_info, scores, valid);
                squad_info->assigned_task_index = best_task_index;
                best_info->assignment_count += squad->living_count;
                for (short other_index = 0; other_index < squad_count; ++other_index)
                {
                    if (other_index == squad_list_index)
                        continue;
                    s_squad_info *other = &squad_list[other_index];
                    if (other->assigned_task_index != best_task_index)
                        continue;
                    float other_score = find_assignment_get_score(objective_index, best_task_index, other->squad_index, group, task_info, scores, valid);
                    if (other_score > score)
                    {
                        changed = true;
                        squad_datum *other_squad = datum_get<squad_datum>(squad_data, other->squad_index);
                        other->assigned_task_index = NONE;
                        best_info->assignment_count -= other_squad->living_count;
                        other->unassigned = true;
                    }
                }
            }
            squad_info->unassigned = false;
        }
    } while (changed);

    for (short squad_list_index = 0; squad_list_index < squad_count; ++squad_list_index)
    {
        s_squad_info *squad_info = &squad_list[squad_list_index];
        s_task_info *info = NULL;
        float best_improvement = 0.0f;
        short best_swap_index = NONE;
        s_task *task = NULL;
        squad_datum *squad = datum_get<squad_datum>(squad_data, squad_info->squad_index);
        if (squad_info->assigned_task_index != NONE)
        {
            task = objective_get_task(objective_index, squad_info->assigned_task_index);
            info = &task_info[squad_info->assigned_task_index];
        }
        for (short other_index = 0; other_index < squad_count; ++other_index)
        {
            if (squad_list_index == other_index)
                continue;
            s_squad_info *other = &squad_list[other_index];
            s_task *other_task = NULL;
            bool valid_swap = true;
            squad_datum *other_squad = datum_get<squad_datum>(squad_data, other->squad_index);
            if (squad_info->assigned_task_index != NONE &&
                (short)(other_squad->living_count - squad->living_count + info->assignment_count + info->squad_count) > task->capacity)
                valid_swap = false;
            if (valid_swap && other->assigned_task_index != NONE)
            {
                other_task = objective_get_task(objective_index, other->assigned_task_index);
                s_task_info *other_info = &task_info[other->assigned_task_index];
                if ((short)(other_info->assignment_count - other_squad->living_count + squad->living_count + other_info->squad_count) > other_task->capacity)
                    valid_swap = false;
            }
            if (valid_swap)
            {
                float current_score = 0.0f;
                float swapped_score = 0.0f;
                if (task)
                    current_score = find_assignment_get_score(
                        objective_index, squad_info->assigned_task_index, squad_info->squad_index, group, task_info, scores, valid);
                if (other_task)
                    current_score += find_assignment_get_score(
                        objective_index, other->assigned_task_index, other->squad_index, group, task_info, scores, valid);
                if (task)
                    swapped_score = find_assignment_get_score(
                        objective_index, squad_info->assigned_task_index, other->squad_index, group, task_info, scores, valid);
                if (other_task)
                    swapped_score += find_assignment_get_score(
                        objective_index, other->assigned_task_index, squad_info->squad_index, group, task_info, scores, valid);
                float improvement = current_score - swapped_score;
                if (improvement > best_improvement)
                {
                    best_swap_index = other_index;
                    best_improvement = improvement;
                }
            }
        }
        if (best_swap_index != NONE)
        {
            s_squad_info *other = &squad_list[best_swap_index];
            s_task *other_task = NULL;
            short assigned_task_index = squad_info->assigned_task_index;
            s_task_info *other_info = NULL;
            short other_task_index = other->assigned_task_index;
            squad_datum *other_squad = datum_get<squad_datum>(squad_data, other->squad_index);
            if (other_task_index != NONE)
            {
                other_task = objective_get_task(objective_index, other_task_index);
                other_info = &task_info[other_task_index];
            }
            if (task)
            {
                info->assignment_count -= squad->living_count;
                squad_info->assigned_task_index = NONE;
            }
            if (other_task)
            {
                other_info->assignment_count -= other_squad->living_count;
                other->assigned_task_index = NONE;
            }
            if (task)
            {
                info->assignment_count += other_squad->living_count;
                other->assigned_task_index = assigned_task_index;
            }
            if (other_task)
            {
                other_info->assignment_count += squad->living_count;
                squad_info->assigned_task_index = other_task_index;
            }
        }
    }
    return true;
}

// @address 0x3aa90 k_real_math_origin3d
namespace {
static __forceinline long objective_game_time_get() { return ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time; }
static __forceinline void objective_add_vectors(const real_point3d *a, const real_point3d *b, real_point3d *result) {
    result->x = a->x + b->x; result->y = a->y + b->y; result->z = a->z + b->z;
}
static __forceinline void objective_scale_vector(const real_point3d *vector, float scale, real_point3d *result) {
    result->x = scale * vector->x; result->y = scale * vector->y; result->z = scale * vector->z;
}
static __forceinline void objective_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result) {
    result->i = b->x - a->x; result->j = b->y - a->y; result->k = b->z - a->z;
}
}
// 0x510700
// match: 0x510700: shared actor_break stays out of line and resizes objective_task_break.
static __forceinline void objectives_actor_break(long actor_index, bool broken)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (broken)
        stimulus_register(actor_index, _behavior_stimulus_broken, 1, 3);
    actor->broken = broken;
}

// @address 0x5103b8 ?objective_update_leadership@@YAXFFFJ@Z
void objective_update_leadership(short objective_index, short task_index, short mode, long actor_index)
{
    s_task_record *record = objective_get_task_record_e60(objective_index, task_index);
    short highest_rank = 0;
    short lowest_rank = 32767;
    bool leader_found = false;
    short actor_count = 0;
    long leader_index = record->leader;
    long highest_actor_index = NONE;

    if (actor_index != NONE)
    {
        short rank = ::actor_get_rank(actor_index);
        switch (mode)
        {
        case 0:
            if (record->highest_rank != NONE)
            {
                if (rank > record->highest_rank)
                {
                    record->leader = actor_index;
                    record->highest_rank = rank;
                    return;
                }
                else if (rank == record->highest_rank || rank == record->lowest_rank)
                {
                    return;
                }
                else if (rank < record->lowest_rank)
                {
                    if (record->leader != NONE)
                    {
                        record->lowest_rank = rank;
                        return;
                    }
                }
                else
                {
                    return;
                }
            }
            else
            {
                record->highest_rank = rank;
                record->lowest_rank = record->highest_rank;
                return;
            }
            break;
        case 1:
        case 2:
            if (actor_index != record->leader && rank > record->lowest_rank)
                return;
            break;
        default:
            // match: 0x5103b8: unreachable hint omits dead branch check
            __assume(0);
        }
    }

    task_actor_iterator iterator;
    task_actor_iterator_new(objective_index, task_index, &iterator);
    if (task_actor_iterator_next(&iterator))
    {
        actor_datum *actors = (actor_datum *)actor_data->data;
        do
        {
            short rank = 0;
            character_general_properties *properties = character_definition_general_properties_get(datum_get<actor_datum>(actor_data,
                iterator.actor_index)->character_index);
            if (properties)
                rank = properties->rank;
            if (iterator.actor_index == leader_index)
                leader_found = true;
            else if (rank > highest_rank)
                highest_actor_index = iterator.actor_index;
            highest_rank = highest_rank > rank ? highest_rank : rank;
            lowest_rank = lowest_rank > rank ? rank : lowest_rank;
            actor_count++;
        } while (task_actor_iterator_next(&iterator));
    }

    if (actor_count == 0)
    {
        if (mode == 2)
            objective_task_break_nearby(objective_index, task_index, datum_get<actor_datum>(actor_data, actor_index)->team, ::actor_get_rank(actor_index));
        record->leader = NONE;
        record->lowest_rank = NONE;
        record->highest_rank = NONE;
        return;
    }

    if (highest_rank > lowest_rank && mode != 2)
    {
        if (leader_index == NONE)
        {
            record->leader = highest_actor_index;
        }
        else
        {
            short leader_rank = ::actor_get_rank(leader_index);
            if (leader_found)
            {
                if (leader_rank != highest_rank)
                {
                    if (highest_rank > leader_rank)
                {
                    record->leader = highest_actor_index;
                }
                else
                {
                    // match: 0x5103b8: unreachable hint omits dead branch check
                    __assume(0);
                }
                }
            }
            else if (leader_rank == highest_rank)
            {
                record->leader = highest_actor_index;
            }
            else
            {
                record->leader = NONE;
            }
        }
    }
    else if (!leader_found && leader_index != NONE)
    {
        if (highest_rank == record->highest_rank)
            record->leader = highest_actor_index;
        else
            record->leader = NONE;
    }

    record->highest_rank = highest_rank;
    record->lowest_rank = lowest_rank;

    if (mode == 2)
    {
        if (record->leader == NONE)
        {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            if (actor_index == leader_index)
                objective_task_break(objective_index, task_index);
            objective_task_break_nearby(objective_index, task_index, actor->team, ::actor_get_rank(actor_index));
        }
        else if (actor_count == 1)
        {
            stimulus_register(record->leader, _behavior_stimulus_leader_abandoned, 1, 3);
        }
    }
}
// @address 0x510700 ?objective_task_break@@YAXFF@Z
void objective_task_break(short objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    record->break_time = objective_game_time_get();
    record->flags.set(_task_record_broken, true);
    record->flags.set(_task_record_engage_allowed, true);
    record->flags.set(_task_record_search_allowed, true);
    task_actor_iterator iterator;
    task_actor_iterator_new(objective_index, task_index, &iterator);
    while (task_actor_iterator_next(&iterator))
        objectives_actor_break(iterator.actor_index, true);
}

namespace {
}
void objective_task_break_nearby(short objective_index, short task_index, short team, short highest_rank)
{
    const real_point3d *position = objective_task_get_position(objective_index, task_index);
    if (position)
    {
        scenario *scenario_data = global_scenario;
        for (short nearby_objective_index = 0; nearby_objective_index < scenario_data->objectives.count; ++nearby_objective_index)
        {
            s_objective *objective = (s_objective *)tag_block_get_element_with_size(
                &global_scenario->objectives, nearby_objective_index, sizeof(s_objective));
            for (short nearby_task_index = 0; nearby_task_index < objective->tasks.count; ++nearby_task_index)
            {
                bool friendly = false;
                s_task_record *record = objective_get_task_record(nearby_objective_index, nearby_task_index);
                const real_point3d *nearby_position = objective_task_get_position(nearby_objective_index, nearby_task_index);
                if (!nearby_position || record->leader != NONE || record->count <= 0 || record->highest_rank >= highest_rank ||
                    record->flags.test(_task_record_broken))
                    continue;
                s_task_squad_iterator iterator;
                task_squad_iterator_new(&iterator, nearby_objective_index, nearby_task_index, true, true);
                squad_datum *squad;
                while ((squad = task_squad_iterator_next(&iterator)) != NULL)
                {
                    if (squad->team == team || game_team_is_ally((e_game_team)squad->team, (e_game_team)team))
                    {
                        friendly = true;
                        break;
                    }
                }
                if (friendly)
                {
                    float distance = distance_squared3d(position, nearby_position);
                    if (distance < 10.0f * 10.0f)
                        objective_task_break(nearby_objective_index, nearby_task_index);
                }
            }
        }
    }
}

void objective_task_request_engage(short objective_index, short task_index, bool force)
{
    s_task *task = objective_get_task(objective_index, task_index);
    if (force || (_rotl(task->flags, 28) & 1) == 0)
    {
        s_task_record *record = objective_get_task_record(objective_index, task_index);
        if (record->leader != NONE)
        {
            if (!record->flags.test(_task_record_engage_allowed) && record->engage_time == NONE)
                record->engage_time = ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time;
        }
        else
        {
            record->flags.set(_task_record_engage_allowed, true);
            record->engage_time = NONE;
        }
    }
}

// @address 0x510ad0 ?objective_task_request_group_grenade@@YAXFF@Z

void objective_task_request_group_grenade(short objective_index, short task_index)
{
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    long leader = record->leader;
    if (leader != NONE) {
        if (record->grenade_time == NONE ||
#if defined(_XBOX) && !defined(__clang__)
            ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time - record->grenade_time >
                (long)((float)((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->ticks_per_second * AI_REAL_CONSTANTS[3504] + AI_REAL_CONSTANTS[0]))
#else
            ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time - record->grenade_time >
                (long)((float)((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->ticks_per_second * 8.0f + AI_REAL_CONSTANTS[0]))
#endif
        {
            bool requested = false;
            long unit_index = datum_get<actor_datum>(actor_data, leader)->unit_index;
            if (unit_index != NONE) {
                if (!motor_animation_busy(unit_index)) {
                    ::actor_impulse_submit(leader, 48, 0, 0);
                    requested = true;
                }
                actor_dialogue_event(leader, 14, NONE, 0);
            }
            if (requested) record->grenade_time = ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time;
        }
    }
}

// @address 0x510c10 ?objective_task_get_position@@YAPBTreal_point3d@@FF@Z
const real_point3d *objective_task_get_position(short objective_index, short task_index)
{
    s_task *task = objective_get_task(objective_index, task_index);
    s_task_record *record = objective_get_task_record(objective_index, task_index);
    bool result = false;
    if (task->areas.count > 0) {
        if (record->position_time == objective_game_time_get()) result = true;
        else {
            scenario *scenario_data = global_scenario;
            real_point3d position = k_real_math_origin3d;
            short count = 0;
            for (short area_index = 0; area_index < task->areas.count; ++area_index) {
                squads_area_zone_set_entry *task_area = (squads_area_zone_set_entry *)
                    tag_block_get_element_with_size(&task->areas, area_index, sizeof(squads_area_zone_set_entry));
                if (task_area->zone_index >= 0 && task_area->zone_index < scenario_data->zones.count) {
                    zone_definition *zone = (zone_definition *)scenario_data->zones.address + (unsigned short)task_area->zone_index;
                    if (task_area->area_index >= 0 && task_area->area_index < zone->areas.count) {
                        area_definition *area = (area_definition *)tag_block_get_element_with_size(
                            &zone->areas, task_area->area_index, sizeof(area_definition));
                        real_point3d point;
                        ::ai_point_get_position(&area->point, &point);
                        objective_add_vectors(&point, &position, &position);
                        ++count;
                    }
                }
            }
            if (count > 0) {
                objective_scale_vector(&position, 1.0f / count, &position);
                record->position = position;
                record->position_time = objective_game_time_get();
                result = true;
            }
        }
    }
    return result ? &record->position : NULL;
}
// @address 0x510e20 ?objective_task_get_direction@@YA_NFFPATreal_vector3d@@@Z
bool objective_task_get_direction(short objective_index, short task_index, real_vector3d *direction)
{
    bool result = false;
    real_point3d end;
    real_point3d start;
    while (task_index != NONE) {
        s_task *task = objective_get_task(objective_index, task_index);
        if (task->points.count > 0) {
            c_ai_point3d *points = (c_ai_point3d *)tag_block_get_element_with_size(&task->points, 0, sizeof(c_ai_point3d));
            ::ai_point_get_position(points, &start);
            ::ai_point_get_position(points + 1, &end);
            objective_vector_from_points(&start, &end, direction);
            result = normalize3d(direction) > 0.0f;
            break;
        } else task_index = task->parent_index;
    }
    return result;
}

// 0x510f20
// @address 0x510f20 ?remove_squad@@YA_NPAFF@Z
bool remove_squad(short *first_squad_index, short squad_index)
{
    bool removed = false;
    short *squad_index_reference = first_squad_index;
    while (*squad_index_reference != NONE)
    {
        squad_datum *squad = datum_get<squad_datum>(squad_data, *squad_index_reference);
        if (*squad_index_reference == squad_index)
        {
            *squad_index_reference = squad->next_index;
            removed = true;
            break;
        }
        squad_index_reference = &squad->next_index;
    }
    return removed;
}

// 0x510f88
struct s_pureform_distribution;

// @address 0x510f88 ?finalize_task_change@@YAXJ@Z
void finalize_task_change(long squad_index)
{
    s_task *task = NULL;
    short living_count = 0;
    squad_datum *squad = datum_get_absolute<squad_datum>(squad_data, (unsigned short)squad_index);
    unsigned short objective_index_unsigned = squad->objective_index;
    unsigned short task_index_unsigned = squad->task_index;
    short objective_index = (short)objective_index_unsigned;
    short task_index = (short)task_index_unsigned;
    if (objective_index != NONE)
    {
        if (task_index != NONE)
            task = objective_get_task(objective_index, task_index);
    }

    if (squad_valid_task(squad))
        squad->last_task_change_time = objective_game_time_get();
    else
        squad->last_task_change_time = NONE;

    squad_actor_iterator iterator;
    squad_actor_iterator_new(&iterator, squad_index, false);
    actor_datum *actor = squad_actor_iterator_next(&iterator);
    while (actor != NULL)
    {
        bool changed = false;
        bool camo = false;
        if (task != NULL)
        {
            changed = true;
            camo = (bool)(_rotl(task->signed_flags, 27) & 1);
        }
        actor_handle_change_internal(iterator.index, changed, camo);
        if (task != NULL && task->command_script_index != NONE)
            cs_attach_command_script(iterator.index, task->command_script_index, -1);
        if (!actor->swarm)
            living_count = (short)(living_count + 1);
        actor = squad_actor_iterator_next(&iterator);
    }

    bool task_frozen = false;
    bool task_vehicle = false;
    bool is_vehicle_task = false;
    if (task != NULL)
    {
        s_task_record *record = objective_get_task_record(objective_index_unsigned, task_index_unsigned);
        squad->follow_object_index = NONE;
        if (task->type == 1)
            calculate_dynamic_areas(squad_index, &task->areas, task->distance);

        s_pureform_distribution *distribution = NULL;
        s_tag_block *areas = NULL;
        if (task->pureform_distributions.count > 0)
        {
            distribution = (s_pureform_distribution *)task->pureform_distributions.address;
            areas = &task->areas;
        }
        squad_handle_morph_redistribution(squad_index, distribution, areas);

        if (!record->flags.test(_task_record_script_executed))
            record->flags.set(_task_record_script_pending, true);
        if (!record->flags.test(_task_record_dialogue_spoken) && task->dialogue != 0)
            record->flags.set(_task_record_dialogue_pending, true);

        task_frozen = (_rotl((short)task->signed_flags, 24) & 1) != 0;
        task_vehicle = (_rotl((short)task->signed_flags, 23) & 1) != 0;
        is_vehicle_task = objective_task_is_vehicle_task(objective_index, (short)task_index_unsigned);
    }

    squad_actor_iterator_new(&iterator, squad_index, false);
    actor = squad_actor_iterator_next(&iterator);
    while (actor != NULL)
    {
        actor_datum *current_actor = datum_get<actor_datum>(actor_data, iterator.index);
        if (task_frozen)
        {
            current_actor->behavior_mode = 0;
            actor_freeze(iterator.index);
        }
        else if (current_actor->behavior_mode == 0)
        {
            current_actor->behavior_mode = 3;
        }
        actor = squad_actor_iterator_next(&iterator);
    }

    if (ai_globals->enabled)
    {
        squad_datum *current_squad = datum_get_absolute<squad_datum>(squad_data, (unsigned short)squad_index);
        if (is_vehicle_task)
            current_squad->flags |= 0x800;
        else
            current_squad->flags &= ~0x800;
    }
    if (task_vehicle)
        squad->flags |= 0x400;
    else
        squad->flags &= ~0x400;

    short style_index = task != NULL ? task->style_index : NONE;
    squad_update_style(squad_index, style_index);

    if (squad->previous_objective_index != NONE && squad->previous_task_index != NONE && objective_index != NONE && task_index != NONE)
    {
        s_task_record *previous_record = objective_get_task_record(squad->previous_objective_index, squad->previous_task_index);
        s_task_record *current_record = objective_get_task_record(objective_index, task_index);
        current_record->engage_time = previous_record->engage_time;
        current_record->grenade_time = previous_record->grenade_time;
        bool previous_800 = previous_record->flags.test(_task_record_engage_allowed);
        bool previous_200 = previous_record->flags.test(_task_record_search_allowed);
        current_record->flags.set(_task_record_engage_allowed, previous_800);
        current_record->flags.set(_task_record_search_allowed, previous_200);
    }

    if (objective_index != NONE && task_index != NONE)
        objective_update_leadership(objective_index, task_index, NONE, NONE);

    if (squad->previous_objective_index != NONE && squad->previous_task_index != NONE)
        objective_update_leadership(squad->previous_objective_index, squad->previous_task_index, NONE, NONE);

    if (squad->objective_index != NONE && squad->task_index != NONE)
    {
        short previous_task_index = NONE;
        short previous_parent_index = NONE;
        if (squad->objective_index == squad->previous_objective_index && squad->previous_task_index != NONE)
        {
            s_task *previous_task = objective_get_task(squad->objective_index, squad->previous_task_index);
            previous_task_index = squad->previous_task_index;
            previous_parent_index = previous_task->parent_index;
        }
        short current_task_index = squad->task_index;
        while (current_task_index != NONE)
        {
            s_task *current_task = objective_get_task(squad->objective_index, current_task_index);
            if (current_task_index == previous_task_index || current_task_index == previous_parent_index)
                break;
            s_task_record *record = objective_get_task_record(squad->objective_index, current_task_index);
            record->body_count += living_count;
            current_task_index = current_task->parent_index;
        }
        task_update_group_state(squad->objective_index, squad->task_index);
    }

    squad->temporary = false;
    squad->previous_objective_index = squad->objective_index;
    squad->previous_task_index = squad->task_index;
}

bool ai_scripting_set_task_condition(long ai_index, bool condition)
{
 scenario *sc=global_scenario;
 bool success=false;
 if(((unsigned long)ai_index>>29)==5) {
   short objective_index=(short)ai_index;
   if(objective_index>=0 && objective_index<sc->objectives.count) {
     s_objective *objective=(s_objective *)tag_block_get_element_with_size(&global_scenario->objectives,objective_index,sizeof(s_objective));
     short task_index=(ai_index>>16)&0x1fff;
     if(task_index>=0 && task_index<objective->tasks.count) {
       s_task_record *record=objective_get_task_record(objective_index,task_index);
       if(condition) { record->flags.set(_task_record_active_latched,true); record->flags.set(_task_record_exhausted,false); }
       else { record->flags.set(_task_record_active_latched,false); record->flags.set(_task_record_exhausted,true); }
       success=true;
     }
   }
 }
 return success;
}

bool ai_scripting_leadership(long ai_index) {
scenario *scenario_data=global_scenario;
bool result=false;
if(((unsigned long)ai_index >> 29)==5) {
short objective_index=(short)ai_index;
if(objective_index>=0 && objective_index<scenario_data->objectives.count) {
s_objective *objective=(s_objective *)tag_block_get_element_with_size(&global_scenario->objectives,objective_index,sizeof(s_objective));
short task_index=(short)((ai_index >> 16) & 0x1fff);
if(task_index>=0 && task_index<objective->tasks.count)
result=objective_task_get_leader(objective_index,task_index)!=-1;
}
} else if(((unsigned long)ai_index >> 29)==1) {
short squad_index=(short)ai_index;
if(squad_index>=0 && squad_index<scenario_data->squads.count)
result=squad_get_leader(squad_index)!=-1;
}
return result;
}

bool ai_scripting_leadership_all(long ai_index){
 scenario *s=global_scenario;
 bool result=false;
 if(((unsigned long)ai_index>>29)==5){
 short objective_index=(short)ai_index;
 if(objective_index>=0 && objective_index<s->objectives.count){
 s_objective *o=objective_get_e60(objective_index);
 short task_index=(short)((ai_index>>16)&0x1fff);
 if(task_index>=0 && task_index<o->tasks.count){
 if(objective_task_get_leader_e60(objective_index,task_index)!=-1)result=true;
 else{
 s_task_iterator iterator;
 task_iterator_new_e60(&iterator,objective_index,task_index);
 while(task_iterator_next(&iterator)){
 if(objective_task_get_leader_e60(objective_index,iterator.task_index)!=-1){result=true;break;}
 }
 }
 }
 }
 }
 return result;
}

// 0x511fb0
short ai_task_count(long ai_index)
{
    short count = 0;
    if (((unsigned long)ai_index & 0xe0000000) == 0xa0000000) {
        scenario *scenario_data = global_scenario;
        short objective_index = (short)ai_index;
        short task_index = (short)((ai_index >> 16) & 0x1fff);
        if (task_index != 0x1fff) {
            if (objective_index >= 0 && objective_index < scenario_data->objectives.count) {
                s_objective *objective = (s_objective *)tag_block_get_element_with_size(
                    &global_scenario->objectives, objective_index, sizeof(s_objective));
                if (task_index >= 0 && task_index < objective->tasks.count) {
                    s_task_squad_iterator iterator;
                    task_squad_iterator_new(&iterator, objective_index, task_index, true, false);
                    squad_datum *squad;
                    while ((squad = task_squad_iterator_next(&iterator)) != 0)
                        count += squad->living_count;
                }
            }
        }
    }
    return count;
}

namespace {
static __forceinline bool task_has_vehicle_filter(const s_task *task)
{
    bool result = (task->filter_flags & 1) != 0;
    return result;
}
}

// match: 0x512088: __restrict on iterator preserves task_actor_iterator_get_next_squad's loads
extern "C" squad_datum *task_actor_iterator_get_next_squad(task_actor_iterator *__restrict iterator)
{
    squad_datum *squad = NULL;
    iterator->squad_index = NONE;
    if (iterator->next_squad_index != NONE)
    {
        squad_datum *squads = (squad_datum *)squad_data->data;
        while (iterator->next_squad_index != NONE)
        {
            iterator->squad_index = iterator->next_squad_index;
            squad = &squads[DATUM_INDEX_TO_ABSOLUTE_INDEX(iterator->squad_index)];
            iterator->next_squad_index = squad->next_index;
            if (squad->task_index == iterator->task_index)
                break;
            iterator->squad_index = NONE;
            squad = NULL;
        }
    }
    return squad;
}

// 0x512108
bool objective_task_is_vehicle_task(long objective_index, long task_index)
{
    bool result = false;
    if (objective_index != -1 && task_index != -1) {
        s_task *task = objective_get_task((short)objective_index, (short)task_index);
        result = task->filter >= 34 && task->filter <= 46 && task_has_vehicle_filter(task);
    }
    return result;
}


namespace {
static __forceinline void *restricted_region_get_task_member_address(long member_index)
{
    char *base_address = g_restricted_address[1];
    long offset = g_restricted_regions[1].get_member_offset(member_index);
    return base_address + offset;
}

static __forceinline long restricted_region_add_task_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, alignment_bits,
        __tls_set_g_tasks_records_allocator, 0, 0);
    void *address = restricted_region_get_task_member_address(member_index);
    g_tasks_records = (s_task_record *)address;
    return member_index;
}

}


// 0x1be800
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_data_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    g_tasks_data_tls_address = 0;
    restricted_region_free_member(1, value);
}

namespace {
static __forceinline long restricted_region_add_tasks_data_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(
        &g_restricted_regions[1], name, type, size, alignment_bits, __tls_set_g_tasks_data_allocator, 0, 0);
    void *address = restricted_region_get_task_member_address(member_index);
    g_tasks_data_tls_address = (long *)address;
    return member_index;
}

static __forceinline long restricted_region_add_objectives_data_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, alignment_bits,
        __tls_set_g_objectives_data_allocator, 0, 0);
    void *address = restricted_region_get_task_member_address(member_index);
    data_set_new_base_address((s_data_array **)&g_objectives_data, (s_data_array *)address);
    return member_index;
}
}

// 0x1be748
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_tasks_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x1be748: assume materializes the alignment_bits argument as li r7,0 for add_member
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_tasks_data_member(name, type, size, alignment_bits);
    m_thread_id = tls_current_thread_index();
    return restricted_region_get_task_member_address(m_member_index);
}

// 0x512190
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_objectives_data_member(name, "", size, 0);
    m_thread_id = tls_current_thread_index();
    return restricted_region_get_task_member_address(m_member_index);
}

// 0x512230
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_data_allocator>::deallocate(void *memory)
{
    g_objectives_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// 0x512250
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_records_allocator>::reserve_memory(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    // match: 0x512250: assume materializes the alignment_bits argument as li r7,0 for add_member
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_task_member(name, type, size, alignment_bits);
    m_thread_id = tls_current_thread_index();
    return restricted_region_get_task_member_address(m_member_index);
}

// 0x5122f0
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_records_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x512308
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_records_allocator>::deallocate(void *memory)
{
    g_tasks_records = 0;
    restricted_region_free_member(1, m_member_index);
}


// @address 0x50ea98 ?disallow_task@@YAXFFPAK@Z
void disallow_task(short objective_index, short task_index, unsigned long *disallowed_tasks)
{
    s_task_iterator iterator;
    iterator.objective_index = objective_index;
    iterator.task_index = -1;
    iterator.depth = 0;
    iterator.next_task_index = objective_get_task_direct(objective_index, task_index)->first_child_index;
    disallowed_tasks[task_index >> 5] |= 1 << (task_index & 31);
    while (task_iterator_next(&iterator))
        disallowed_tasks[iterator.task_index >> 5] |= 1 << (iterator.task_index & 31);
}

// @address 0x50ec78 test_vehicle_filter
extern "C" bool test_vehicle_filter(long squad,s_task *task,void *info,void *record,short unused,bool allow) {
 actor_datum *actor=0;
 float total=0.0f;
 float matches=0.0f;
 squad_actor_iterator iterator;
 squad_actor_iterator_new(&iterator,squad,false);
 while((actor=::squad_actor_iterator_next(&iterator))!=0) {
  bool match=false;
  long vehicle=-1;
  if(actor->attached_vehicle_index!=-1 && actor->vehicle_desire>=2) vehicle=actor->attached_vehicle_index;
  if(vehicle==-1 && actor->type==15 && actor->unit_index!=-1) vehicle=object_get_root_object(actor->unit_index);
  if(vehicle==-1) {
   if(allow) match=actor->seeking_squad;
  } else {
   object_datum *object=(object_datum *)object_get_and_verify_type(vehicle,2);
   vehicle_definition *definition=(vehicle_definition *)tag_get('vehi',object->definition);
   if(definition->ai_properties.count>0) {
    object_ai_properties *properties=(object_ai_properties *)tag_block_get_element_with_size(&definition->ai_properties,0,sizeof(object_ai_properties));
    switch(task->filter) {
    case _task_filter_vehicle: match=true; break;
    case _task_filter_scorpion: match=properties->ai_type_name==_string_id_scorpion; break;
    case _task_filter_ghost: match=properties->ai_type_name==_string_id_ghost; break;
    case _task_filter_warthog: match=properties->ai_type_name==_string_id_warthog; break;
    case _task_filter_spectre: match=properties->ai_type_name==_string_id_spectre; break;
    case _task_filter_wraith: match=properties->ai_type_name==_string_id_wraith; break;
    case _task_filter_phantom: match=properties->ai_type_name==_string_id_phantom; break;
    case _task_filter_pelican: match=properties->ai_type_name==_string_id_pelican; break;
    case _task_filter_banshee: match=properties->ai_type_name==_string_id_banshee; break;
    case _task_filter_hornet: match=properties->ai_type_name==_string_id_hornet; break;
    case _task_filter_chopper: match=properties->ai_type_name==_string_id_chopper; break;
    case _task_filter_mauler: match=properties->ai_type_name==_string_id_mauler; break;
    case _task_filter_mongoose: match=properties->ai_type_name==_string_id_mongoose; break;
    }
   }
  }
  if(match) matches+=1.0f;
  total+=1.0f;
 }
 if(total>0.0f) return matches/total>0.49f;
 return false;
}

// @address 0x50eef0 ?test_filter@@YA_NJPAUs_task@@PAUs_task_info@@PAUs_task_record@@F_NPA_N@Z
bool test_filter(long squad_index, s_task *task, s_task_info *task_info, s_task_record *task_record, short threshold, bool allow, bool *valid)
{
 bool result=true;
 if(valid!=0) *valid=true;
 if(task->group_squad_index!=-1 && !squad_ancestor_of_squad_group(squad_index,task->group_squad_index)) {
  result=false;
  if(valid!=0) *valid=false;
 }
 if(result && task->filter>=_task_filter_vehicle && task->filter<=_task_filter_mongoose) {
  result=test_vehicle_filter(squad_index,task,task_info,task_record,threshold,allow);
 } else if(result && task->filter!=0) {
  squad_datum *squad=&((squad_datum *)squad_data->data)[(unsigned short)squad_index];
  switch(task->filter) {
  case _task_filter_highest_spawn_count:
   if(squad->squad_spawn_count==0) {
    result=task_info->squad_spawn_count>0;
   } else {
    result = squad->squad_spawn_count >= task_info->squad_spawn_count && squad->squad_spawn_count > 0 &&
        (threshold == -1 || !task_record->flags.test(_task_record_broken) && squad->squad_spawn_count >= threshold);
   }
   break;
  case _task_filter_below_spawn_threshold:
   result=threshold==-1 || squad->squad_spawn_count<threshold;
   break;
  case 5:
   result=(squad->flags & 8)!=0;
   break;
  case 6:
   result=(squad->flags & 16)!=0;
   break;
  case _task_filter_health_above_threshold:
   result=squad->health>AI_SQUAD_HEALTH_THRESHOLD;
   break;
  case _task_filter_health_above_half:
   result=squad->health>0.5f;
   break;
  case _task_filter_health_above_three_quarters:
   result=squad->health>0.75f;
   break;
  case _task_filter_health_below_threshold:
   result=squad->health<AI_SQUAD_HEALTH_THRESHOLD;
   break;
  case _task_filter_health_below_half:
   result=squad->health<0.5f;
   break;
  case _task_filter_health_below_three_quarters:
   result=squad->health<0.75f;
   break;
  case _task_filter_team_human:
   result=squad->team==2;
   break;
  case _task_filter_team_covenant:
   result=squad->team==3;
   break;
  case _task_filter_team_flood:
   result=squad->team==4;
   break;
  case _task_filter_team_sentinel:
   result=squad->team==5;
   break;
  case _task_filter_team_heretic:
   result=squad->team==6;
   break;
  case _task_filter_team_prophet:
   result=squad->team==7;
   break;
  default: {
   bool break_on_match=false;
   squad_actor_iterator iterator;
   squad_actor_iterator_new(&iterator,squad_index,false);
   while(break_on_match || result) {
    actor_datum *actor=::squad_actor_iterator_next(&iterator);
    if(actor!=0) {
     bool match=false;
     switch(task->filter) {
     case _task_filter_infantry:
      match=actor->attached_vehicle_index==-1;
      break;
     case _task_filter_arbiter:
      if(actor->unit_index!=-1) {
       object_datum *object=(object_datum *)object_get_and_verify_type(actor->unit_index,8195);
       vehicle_definition *definition=(vehicle_definition *)tag_get('unit',object->definition);
       if(definition->ai_properties.count>0) {
        object_ai_properties *properties=(object_ai_properties *)tag_block_get_element_with_size(&definition->ai_properties,0,sizeof(object_ai_properties));
        match=properties->ai_type_name==_string_id_arbiter;
       }
       break_on_match=true;
      }
      break;
     case _task_filter_player_in_vehicle:
      match=actor->friendly_player_in_vehicle;
      break_on_match=true;
      break;
     case _task_filter_elite:
      match=actor->type==0;
      break;
     case _task_filter_jackal:
      match=(short)actor->type==1;
      break;
     case _task_filter_grunt:
      match=(short)actor->type==2;
      break;
     case _task_filter_hunter:
      match=(short)actor->type==3;
      break;
     case _task_filter_marine:
      match=(short)actor->type==7;
      break;
     case _task_filter_flood_combat:
      match=(short)actor->type==9;
      break;
     case _task_filter_flood_carrier:
      match=(short)actor->type==11;
      break;
     case _task_filter_sentinel:
      match=(short)actor->type==13;
      break;
     case _task_filter_brute:
      match=(short)actor->type==16;
      break;
     case _task_filter_prophet:
      match=(short)actor->type==17;
      break;
     case _task_filter_bugger:
      match=(short)actor->type==18;
      break;
     case _task_filter_flood_pureform: {
      short actor_type=(short)actor->type;
      match=actor_type==20 || actor_type==21 || actor_type==22;
      break;
     }
     case _task_filter_guardian:
      match=(short)actor->type==24;
      break;
     case _task_filter_sniper: {
      long weapon=actor_get_weapon(iterator.index);
      break_on_match=true;
      if(weapon!=-1) {
       object_datum *object=(object_datum *)object_get_and_verify_type(weapon,4);
       vehicle_definition *definition=(vehicle_definition *)tag_get('weap',object->definition);
       if(definition->ai_properties.count>0) {
        object_ai_properties *properties=(object_ai_properties *)tag_block_get_element_with_size(&definition->ai_properties,0,sizeof(object_ai_properties));
        match=properties->ai_type_name==_string_id_sniper;
       }
      }
      break;
     }
     default:
      result=true;
      break;
     }
     if(break_on_match) {
      if(match) {
       result=true;
       break;
      } else {
       result=false;
       continue;
      }
     } else {
      result=result & match;
      continue;
     }
    } else {
     break;
    }
   }
   break;
  }
  }
 }
 return result;
}

struct squad_group_datum;
struct ai_script_context;
static __forceinline void objectives_cs_global_script_context_clear()
{
    global_script_context.ai_index = 0;
    global_script_context.object_index = 0;
}
static __forceinline squad_datum *objectives_inl_squad_get(long index)
{
    return datum_get<squad_datum>(squad_data, index);
}
static __forceinline s_objective_record *objectives_inl_objective_get(short index)
{
    return datum_get<s_objective_record>(g_objectives_data, index);
}
static __forceinline void objectives_inl_list_add(short *head, short index) {
    squad_datum *squad = objectives_inl_squad_get(index);
    squad->next_index = *head;
    *head = index;
}

// @address 0x511550 ?squad_set_objective@@YA_NJFF_N@Z
bool squad_set_objective(long squad_index, short objective_index, short task_index, bool temporary)
{
    squad_datum *squad = objectives_inl_squad_get(squad_index);
    bool changed = false;
    bool finalize = false;
    short previous_objective_index = squad->objective_index;
    short previous_task_index = squad->task_index;
    finalize = objective_index != squad->previous_objective_index || task_index != squad->previous_task_index;
    changed = objective_index != previous_objective_index || task_index != previous_task_index;
    squad->temporary = temporary && finalize;
    squad->objective_index = objective_index;
    squad->task_index = task_index;
    if (changed) {
        if (objective_index == previous_objective_index) {
            if (previous_task_index == -1 || task_index == -1) {
                if (previous_task_index == -1) {
                    remove_squad(&objectives_inl_objective_get(previous_objective_index)->first_unassigned_squad_index, (short)squad_index);
                    s_objective_record *objective = objectives_inl_objective_get(previous_objective_index);
                    objectives_inl_list_add(&objective->first_squad_index, (short)squad_index);
                    objective->changed = true;
                } else {
                    remove_squad(&objectives_inl_objective_get(previous_objective_index)->first_squad_index, (short)squad_index);
                    s_objective_record *objective = objectives_inl_objective_get(previous_objective_index);
                    objectives_inl_list_add(&objective->first_unassigned_squad_index, (short)squad_index);
                    objective->changed = true;
                }
            }
        } else {
            if (previous_objective_index != -1) {
                if (previous_task_index == -1) {
                    remove_squad(&objectives_inl_objective_get(previous_objective_index)->first_unassigned_squad_index, (short)squad_index);
                } else {
                    remove_squad(&objectives_inl_objective_get(previous_objective_index)->first_squad_index, (short)squad_index);
                }
            }
            if (objective_index != -1) {
                if (task_index == -1) {
                    s_objective_record *objective = objectives_inl_objective_get(objective_index);
                    objectives_inl_list_add(&objective->first_unassigned_squad_index, (short)squad_index);
                    objective->changed = true;
                } else {
                    s_objective_record *objective = objectives_inl_objective_get(objective_index);
                    objectives_inl_list_add(&objective->first_squad_index, (short)squad_index);
                    objective->changed = true;
                }
            }
        }
    }
    if (!squad->temporary && finalize) {
        finalize_task_change(squad_index);
    }
    return true;
}

// @address 0x511808 ?task_exhaust@@YAXFF@Z
void task_exhaust(short objective_index, short task_index) {
 s_task *task = objective_get_task(objective_index, task_index);
 s_task_record *record = objective_get_task_record(objective_index, task_index);
 if (!record->flags.test(_task_record_exhausted)) {
  if (task->exhaust_script_index != NONE) {
   global_script_context.ai_index = 0;
   global_script_context.object_index = ((0xA000 | (task_index & 0x1fff)) << 16) | (unsigned short)objective_index;
   s_hs_script_definition *script = (s_hs_script_definition *)tag_block_get_element_with_size(&global_scenario->scripts,
       task->exhaust_script_index, sizeof(s_hs_script_definition));
   hs_runtime_evaluate(script->root_expression, false, true);
   objectives_cs_global_script_context_clear();
  }
  record->flags.set(_task_record_exhausted, true);
 }
}

// @address 0x511920 ?task_update@@YAXFF@Z
void task_update(short objective_index, short task_index) {
 s_task *task=objective_get_task(objective_index,task_index);
 s_task_record *record=objective_get_task_record(objective_index,task_index);
 if(record->flags.test(_task_record_script_pending) && !record->flags.test(_task_record_script_executed)) {
  if(task->script_index!=-1) {
   unsigned long encoded=0xa0000000UL|((task_index&8191)<<16)|(unsigned short)objective_index;
   global_script_context.ai_index=0;
   global_script_context.object_index=encoded;
   s_hs_script_definition *script=(s_hs_script_definition *)tag_block_get_element_with_size(
       &global_scenario->scripts,task->script_index,sizeof(s_hs_script_definition));
   hs_runtime_evaluate(script->root_expression,false,true);
   objectives_cs_global_script_context_clear();
  }
  record->flags.set(_task_record_script_executed,true);
  record->flags.set(_task_record_script_pending,false);
 }
 unsigned short dialogue_index;
 if(record->flags.test(_task_record_dialogue_pending) && !record->flags.test(_task_record_dialogue_spoken) && (dialogue_index=task->dialogue)!=0) {
  task_actor_iterator iterator;
  task_actor_iterator_new(objective_index,task_index,&iterator);
  actor_datum *actor;
  while((actor=task_actor_iterator_next(&iterator))!=0) {
   if(actor->unit_index!=-1) {
    short dialogue=(short)(dialogue_index+65-1);
    c_dialogue_event event(dialogue,actor->unit_index,-1,-1,-1,0);
    ai_dialogue_event(&event);
    record->flags.set(_task_record_dialogue_pending,false);
    record->flags.set(_task_record_dialogue_spoken,true);
    break;
   }
  }
 }
}

static __forceinline bool objectives_task_flag_test(s_task *task,long bit) { return (task->signed_flags & (1 << bit)) != 0; }
// match: 0xaa658: shared game_difficulty_level_get stays out of line and resizes objective_task_test_activation.
static __forceinline short objectives_game_difficulty_level_get()
{
    long difficulty;
    if (game_globals->options.game_mode == 1)
        difficulty = game_globals->options.campaign_difficulty;
    else
        difficulty = 1;
    return (short)difficulty;
}

static float objectives_objective_task_get_strength(short objective_index, short task_index)
{
    s_objective_record *objective = &((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index];
    long squad_index = objective->first_squad_index;
    float strength = 0.0f;
    float total_weight = 0.0f;
    bool found = false;
    while (squad_index != NONE)
    {
        squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)squad_index];
        if (squad->task_index == task_index)
        {
            strength = strength + squad->health * squad->strength_weight;
            total_weight = total_weight + squad->strength_weight;
            found = true;
        }
        squad_index = squad->next_index;
    }
    if (!found)
        strength = 1.0f;
    else if (total_weight > 0.0f)
        strength = strength / total_weight;
    return strength;
}

inline bool objectives_objective_task_test_script_activation(short objective_index, short task_index)
{
    s_task *task = objective_get_task(objective_index, task_index);
    scenario *scenario = global_scenario;
    bool result = true;
    if (task->activation_script_index >= 0 && task->activation_script_index < scenario->scripts.count)
    {
        s_hs_script_definition *script = (s_hs_script_definition *)tag_block_get_element_with_size(&scenario->scripts, task->activation_script_index,
            sizeof(s_hs_script_definition));
        hs_value value;
        value.long_value = hs_runtime_internal_evaluate(script->root_expression);
        result = value.boolean_value;
    }
    return result;
}


// @address 0xaa658 ?objective_task_test_activation@@YA_NFFPBUs_task_info@@_NPA_N@Z
bool objective_task_test_activation(short objective_index, short task_index, struct s_task_info const * task_info, bool update, bool * exhausted)
{
    s_task *task = objective_get_task(objective_index, task_index);
    s_task_record *task_record = objective_get_task_record(objective_index, task_index);
    bool task_exhausted = false;
    bool active;

    if (exhausted)
        *exhausted = false;

    if (task_record->flags.test(_task_record_exhausted))
    {
        task_exhausted = true;
        active = false;
    }
    else if (!TEST_BIT(task->inhibit_difficulty, objectives_game_difficulty_level_get()) && !objectives_task_flag_test(task,10))
    {
        if (task->maximum_squad_count > 0 && task_record->deaths >= task->maximum_squad_count)
        {
            task_exhausted = true;
            active = false;
        }
        else if (task->minimum_strength >= 0.0f && objectives_objective_task_get_strength(objective_index, task_index) < task->minimum_strength)
        {
            task_exhausted = true;
            active = false;
        }
        else if (task_info && task_info->squad_count == 0 && task_record->body_count > 0 &&
            (objectives_task_flag_test(task,3) || task->maximum_body_count > 0 && task_record->body_count >= task->maximum_body_count))
        {
            task_exhausted = true;
            active = false;
        }
        else if (task_record->flags.test(_task_record_active_latched))
        {
            active = true;
        }
        else
        {
            active = objectives_objective_task_test_script_activation(objective_index, task_index);
            if (update)
            {
                if (objectives_task_flag_test(task,0) && active)
                    task_record->flags.set(_task_record_active_latched, true);
                else if (objectives_task_flag_test(task,1) && !active)
                    task_exhausted = true;
            }
        }

        if (update)
        {
            if (active)
            {
                task_record->flags.set(_task_record_activated, true);
            }
            else if (task_exhausted && !task_record->flags.test(_task_record_exhausted))
            {
                task_exhaust(objective_index, task_index);
                s_task_iterator iterator;
                task_iterator_new(&iterator, objective_index, task_index);
                while (task_iterator_next(&iterator))
                {
                    if (!objective_get_task_record(iterator.objective_index, iterator.task_index)->flags.test(_task_record_exhausted))
                        task_exhaust(iterator.objective_index, iterator.task_index);
                }
            }
        }
    }
    else
    {
        task_exhausted = false;
        active = false;
    }

    if (exhausted)
        *exhausted = task_exhausted;
    return active;
}

// @address 0x511af8 ai_scripting_task_status
extern "C" short ai_scripting_task_status(long ai_index) {
 short status=-1;
 if(((unsigned long)ai_index>>29)==5) {
  short objective=(short)ai_index;
  if(objective>=0 && objective<global_scenario->objectives.count) {
   s_objective *definition=((s_objective *)global_scenario->objectives.address)+objective;
   short task=(ai_index>>16)&0x1fff;
   if(task>=0 && task<definition->tasks.count) {
    s_task_record *record=objective_get_task_record(objective,task);
    if(!record->flags.test(_task_record_activated)) status=0;
    else {
     ai_index_actor_iterator iterator;
     ai_index_actor_iterator_new(ai_index,&iterator);
     if(ai_index_actor_iterator_next(&iterator)) status=1;
     else {
      bool exhausted;
      if(::objective_task_test_activation(objective, task, 0, false, (bool *)(&exhausted))) status=2;
      else if(exhausted) status=4;
      else status=3;
     }
    }
   }
  }
 }
 return status;
}

static __forceinline short objectives_objective_get_by_name(long name) {
 short result=-1;
 scenario *s=global_scenario;
 for(short index=0;index<s->objectives.count;++index) {
  s_objective *o=((s_objective *)global_scenario->objectives.address)+index;
  if(o->name==name) { result=index; break; }
 }
 return result;
}
static __forceinline short objectives_objective_task_get_by_name(long objective_index,long name) {
 short result=-1;
 s_objective *o=((s_objective *)global_scenario->objectives.address)+(short)objective_index;
 for(short index=0;index<o->tasks.count;++index) {
  s_task *t=objective_get_task(o,index);
  if(t->name==name) { result=index; break; }
 }
 return result;
}
// @address 0xaa500 ai_scripting_set_task
extern "C" bool ai_scripting_set_task(long ai_index,long objective_name,long task_name) {
 short objective_index=-1;
 short task_index=-1;
 objective_index=objectives_objective_get_by_name(objective_name);
 if(objective_index!=-1 && task_name!=0) task_index=objectives_objective_task_get_by_name(objective_index,task_name);
 if(task_index==-1 && task_name!=0) objective_index=-1;
 ai_index_squad_iterator squads;
 ::ai_index_squad_iterator_new(ai_index, &squads);
 while(ai_index_squad_iterator_next(&squads)) ::squad_set_objective(squads.squad_index, objective_index, task_index, false);
 return true;
}

// @address 0x50d028 ?task_record_new@@YAXFF@Z
void task_record_new(short objective_index, short task_index)
{
 s_task_record *tasks_data=g_tasks_records;
 s_task_record *record = &tasks_data[(short)(((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index].first_task + task_index)];
 s_task_group *rfS0 = &record->group;
 s_task *task=objective_get_task(objective_index,task_index);
 record->leader=NONE;
 record->lowest_rank=NONE;
 record->break_time=NONE;
 record->highest_rank=NONE;
 record->last_search_time=NONE;
 record->engage_time=NONE;
 record->grenade_time=NONE;
 record->position_time=NONE;
 record->position=k_real_math_origin3d;
 record->deaths=0;
 record->body_count=0;
 record->count=0;
 rfS0->state=0;
 rfS0->flags=0;
 rfS0->time=objective_game_time_get();
 record->flags.clear();
 if ((_rotl(task->flags, 28) & 1) == 0)
  record->flags.set(_task_record_engage_allowed,true);
 else
  record->flags.set(_task_record_engage_allowed,false);
 record->flags.set(_task_record_search_allowed,true);
}

// @address 0x8c4954 g_objectives_next_task_index
short g_objectives_next_task_index;

// @address 0xa9c00 ?objectives_initialize_for_new_map@@YAXXZ
void objectives_initialize_for_new_map()
{
    s_data_array *data = (s_data_array *)g_objectives_data;
    data->valid = true;
    scenario *scenario_data = global_scenario;
    data_delete_all(data);
    g_objectives_next_task_index = 0;
    short objective_index = 0;
    if (scenario_data->objectives.count > 0)
    {
    do
    {
        s_objective *objective = ((s_objective *)global_scenario->objectives.address) + objective_index;
        long index = datum_new((s_data_array *)g_objectives_data);
        s_objective_record *record = objective_record_get_absolute(g_objectives_data, (unsigned short)index);
        record->first_squad_index = NONE;
        record->first_unassigned_squad_index = NONE;
        record->first_task = g_objectives_next_task_index;
        record->changed = true;
        for (short task_index = 0; task_index < objective->tasks.count; ++task_index)
            task_record_new(objective_index, task_index);
        g_objectives_next_task_index += objective->tasks.count;
        ++objective_index;
    } while (objective_index < scenario_data->objectives.count);
    }
}

// @rdata 0x82022850
static const float k_objectives_break_timeout_seconds = 10.0f;

// @address 0x50d248 ?objective_update@@YAXJ@Z
void objective_update(long objective_index)
{
    if (!((s_objective_record *)g_objectives_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(objective_index)].changed)
        return;
    short active = 0;
    scenario *scenario_data = global_scenario;
    s_objective *objective = &((s_objective *)scenario_data->objectives.address)[objective_index];
    for (short i = 0; i < objective->tasks.count; ++i)
        objective_get_task_record((unsigned short)objective_index, i)->count = 0;
    s_objective_record *record = &((s_objective_record *)g_objectives_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(objective_index)];
    long squad_index = record->first_squad_index;
    while (squad_index != NONE)
    {
        squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)squad_index];
        bool valid = false;
        short squad_objective_index = squad->objective_index;
        if (squad_objective_index >= 0 && squad_objective_index < scenario_data->objectives.count)
        {
            long squad_task_index = squad->task_index;
            if (squad_task_index != NONE)
                valid = squad_task_index >= 0 && squad_task_index < ((s_objective *)scenario_data->objectives.address)[squad_objective_index].tasks.count;
        }
        if (valid)
        {
            s_task_record *task = objective_get_task_record(squad->objective_index, squad->task_index);
            task->count += squad->living_count;
        }
        if (TEST_BIT(squad->flags, 7))
            active++;
        squad_index = squad->next_index;
    }
    squad_index = record->first_unassigned_squad_index;
    while (squad_index != NONE)
    {
        squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)squad_index];
        if (TEST_BIT(squad->flags, 7))
            active++;
        squad_index = squad->next_index;
    }
    short task_index = 0;
    if (objective->tasks.count > 0)
    {
        for (; task_index < objective->tasks.count; ++task_index)
        {
            const float break_timeout_rounding = 0.5f;
            task_update((unsigned short)objective_index, task_index);
            s_task_record *task = objective_get_task_record((unsigned short)objective_index, task_index);
            if (task->flags.test(_task_record_broken) &&
                ((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->time - task->break_time >
                (long)((float)((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->ticks_per_second * k_objectives_break_timeout_seconds
                    + break_timeout_rounding))
                task->flags.set(_task_record_broken, false);
            task_update_behaviors(objective_index, task_index);
        }
    }
    if (active == 0)
        ((s_objective_record *)g_objectives_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(objective_index)].changed = false;
}

t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_data_allocator> g_ai_objectives_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_records_allocator> g_ai_tasks_layout_allocation;

