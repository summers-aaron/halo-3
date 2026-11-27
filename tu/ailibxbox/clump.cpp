// ailibxbox/clump.obj: from the records 0xa9460 0xac440 0xac670 0xac818 0xac8e0 0xfb958 0x554200 0x5543d8 0x5544a8 0x5545d8 0x554710 0x5547e0 0x554a20 0x554dc0 0x554fe0 0x555120 0x555490 0x555588 0x555960 0x5559a8 0x555aa0 0x556040 0x556108 0x556788 0x5568c0 0x556c08 0x556d18 0x556f48 0x557088 0x557150 0x557230 0x557338 0x557400 0x557508 0x5575d8 0x557670 0x557798 0x557a88 0x557b58 0x557ba8 0x557bd0 0x557bf8 0x557c38 0x557c60 0x557f90 0x559740
// @flags /TP /O2 /Oc
#include "math/random_math_range.h"
#include "ai/ai_tls_data.h"
#include "ai/ai_information_data.h"
#include "memory/tls_data_arrays.h"
#include "ai/clump_importance_types.h"
#include "ai/swarms.h"
#include "objects/object_reference_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "objects/object_datum.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "math/real_math_constants.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "objects/object_iterator.h"
#include "ai/actor_globals.h"
#include "ai/clump_datum.h"
#include "ai/actor_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/objective_runtime.h"
#include "structures/cluster_bit_vectors.h"
#include "units/unit_types.h"
#include "creatures/creature_datum.h"
#include "ai/character_properties_types.h"
#include "ai/objectives.h"
#include "game/game_globals.h"
#include "ai/ai_globals_definition.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "math/real_math.h"
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "objects/objects.h"
#include "ai/ai_vehicles.h"
#include "ai/clump.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "ai/character_definitions.h"
#include "ai/character_perception.h"
#include "game/game_allegiance.h"
#include "structures/structure_bsp_definitions.h"
#include "game/player_types.h"
#include "units/unit_passenger_iterator.h"
#include "ai/joint_behavior_types.h"
#include "objects/object_marker_globals_types.h"
#include "ai/prop_dialogue_event_types.h"
#include "ai/dialogue_state_types.h"
#include "ai/clump_actor_cursor_types.h"
#include "ai/swarm_creature_iterator_types.h"
#include "scenario/scenario.h"
#include "ai/ai_dialogue.h"
#include "ai/ai_dialogue_definitions.h"
#include "ai/prop_type_info.h"
#include "items/projectile_types.h"
#include "items/projectile_definitions.h"
#include "items/equipment_definitions.h"
#include "objects/object_marker_access.h"
#include "ai/actor_iterator.h"
#include "objects/object_globals.h"
#include "ai/clump_iterator.h"
#include "ai/dialogue_event.h"


// @address 0x11df6a0 clump_data
__declspec(thread) s_data_array *clump_data;
// @address 0x3aa90 k_real_math_origin3d
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0xaa420 task_group_compute_next_state
// @address 0xac8e0 ?clump_update_behavior_state@@YAXJ@Z
// @address 0x51e3b0 ??0c_dialogue_event@@QAA@FJJJFPAUai_information_data@@@Z
// @address 0x51e898 ?ai_dialogue_event@@YA_NPAVc_dialogue_event@@@Z
// @address 0x564510 ?clump_get_first_unit@@YAJJ@Z
// @address 0x564a18 ?clump_dialogue_event_allowed@@YA_NJJ_NW4e_prop_dialogue_event@@PAFPAW4e_dialogue_state@@@Z
// @address 0xb4560 ?actor_has_ranged_weapon@@YA_NJ@Z
// @address 0x5191c0 character_definition_charge_entry_get
// @address 0xbf508 prop_state_get_by_index
// @address 0x557c60 clump_can_berserk
// @address 0x556d18 ?clump_new_prop_internal@@YAJPAUclump_datum@@M@Z
// @address 0x5018a0 ?actor_flush_prop@@YAXJJ@Z
// @address 0xae098 ?ai_get_root_vehicle@@YAJJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x5018a0 ?actor_flush_prop@@YAXJJ@Z
// @address 0xae098 ?ai_get_root_vehicle@@YAJJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x120098 ?prop_state_new@@YAXPAUprop_state@@@Z
// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0xfb958 ?clump_get_clump_prop@@YAJJJ@Z
// @address 0x555960 ?clump_qsort_compare_temp_props@@YAHPBX0@Z
// @address 0x557b58 clump_timed_entry_is_recent
// @address 0x557ba8 clump_group_state_get_search_flag
// @address 0x557bf8 group_state_attacking_next_state
// @address 0x557bd0 ?group_state_searching_override@@YA_NPAM0@Z
// @address 0x557c38 ?group_state_attacking_override@@YA_NPAM0@Z

      /* 0x10 */














struct temp_prop { short type,pad2; long object_index; float weight; long next_index; };


















// @address 0xa9460 clump_actor_cursor_next
extern "C" void *clump_actor_cursor_next(clump_actor_cursor *cursor)
{
    void *result = 0;
    if (ai_globals->enabled)
    {
        void *datum = ::data_iterator_next(&cursor->iterator);
        cursor->datum = datum;
        if (datum)
            result = datum;
        cursor->index = cursor->iterator.index;
    }
    return result;
}

void clump_invitations_update(long clump_index)
{
    short invitation_index;
    clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
    for (invitation_index = 0; invitation_index < 10; invitation_index++) {
        struct invitation_data *invitation = &clump->invitations[invitation_index];
        unsigned char active = (unsigned short)invitation->type != 0xffff && invitation->handle != -1;
        if (active && invitation->time != -1) {
            if (invitation->time < TLS_GAME_TIME_GLOBALS->time) {
                datum_get<joint_state_datum>(joint_state_data(), invitation->handle)->clump_index = -1;
                invitation->type = -1;
                invitation->time = 0;
                invitation->handle = -1;
                invitation->slot = -1;
                invitation->mode = 0;
            }
        }
    }
}

namespace {



static __forceinline clump_datum *clump_lookup_get(long clump_index)
{
    return datum_get<clump_datum>(clump_data, clump_index);
}
static __forceinline prop_datum *clump_prop_get(long prop_index)
{
    return datum_get<prop_datum>(::prop_data, prop_index);
}
static __forceinline long ai_creature_get_actor(long object_index)
{
    creature_datum *creature = (creature_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    long actor_index = -1;
    if (creature->creature.link_type == 0)
    {
        swarm_creature_state *state = (swarm_creature_state *)object_header_block_get(creature, &creature->creature.creature_reference);
        actor_index = state->swarm_index;
    }
    return actor_index;
}
}
long clump_get_clump_prop(long clump_index, long object_index)
{
    clump_datum *clump = clump_lookup_get(clump_index);
    long actor_index = -1;
    bool creature = false;
    long result = -1;
    if ((1 << object_header_get(object_index)->object_type) & 0x800)
    {
        actor_index = ai_creature_get_actor(object_index);
        creature = true;
    }
    long prop_index = clump->first_prop_index;
    while (prop_index != -1)
    {
        prop_datum *prop = clump_prop_get(prop_index);
        if (prop->owned_by_actor && creature && prop->actor_index == actor_index)
        {
            result = prop_index;
            break;
        }
        else if (prop->object_index == object_index)
        {
            result = prop_index;
            break;
        }
        else
        {
            prop_index = prop->next_prop;
            continue;
        }
    }
    return result;
}

namespace {

static __forceinline actor_datum *actor_prop_ref_actor_get(long index)
{
    return datum_get<actor_datum>(actor_data, index);
}
static __forceinline prop_ref_datum *actor_prop_ref_get(long index)
{
    return datum_get<prop_ref_datum>(prop_ref_data, index);
}
static __forceinline float actor_prop_ref_distance(const real_point3d *a, const real_point3d *b)
{
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;
    float squared = x*x;
    squared += z*z;
    squared += y*y;
    return sqrtf(squared);
}
}
// @address 0x557338 ?actor_add_prop_ref@@YAXJJ@Z
void actor_add_prop_ref(long actor_index, long prop_ref_index)
{
    actor_datum *actor = actor_prop_ref_actor_get(actor_index);
    prop_ref_datum *prop_ref = actor_prop_ref_get(prop_ref_index);
    if (actor->first_prop_ref_index != -1)
    {
        prop_ref_datum *previous = actor_prop_ref_get(actor->first_prop_ref_index);
        previous->previous_prop_ref_index = prop_ref_index;
    }
    prop_ref->next_prop_ref_index = actor->first_prop_ref_index;
    actor->first_prop_ref_index = prop_ref_index;
    prop_ref->actor_index = actor_index;
    real_point3d origin;
    object_get_origin(prop_ref->object_index, &origin);
    prop_ref->distance = actor_prop_ref_distance(&origin, &actor->position.body);
}


namespace {


static __forceinline prop_datum *clump_delete_get_prop(long prop_index)
{
    long absolute_index = prop_index & 0xffff;
    return &((prop_datum *)::prop_data->data)[absolute_index];
}
static __forceinline prop_ref_datum *clump_delete_get_prop_ref(long prop_ref_index)
{
    long absolute_index = prop_ref_index & 0xffff;
    return &((prop_ref_datum *)prop_ref_data->data)[absolute_index];
}
}

// @address 0x557400 ?clump_delete_prop@@YAXPAUclump_datum@@J@Z
void clump_delete_prop(clump_datum *clump, long prop_index)
{
    prop_datum *prop = clump_delete_get_prop(prop_index);
    long prop_ref_index = prop->first_prop_ref_index;
    while (prop_ref_index != -1)
    {
        prop_ref_datum *prop_ref = clump_delete_get_prop_ref(prop_ref_index);
        long next_prop_ref_index = prop_ref->prop_next_ref_index;

        actor_remove_prop_ref((prop_ref_datum *)prop_ref);
        clump_remove_prop_ref((prop_ref_datum *)prop_ref);

        if (prop_ref->view_index != -1)
        {
            ::datum_delete(TLS_PROP_DATA, prop_ref->view_index);
        }

        ::datum_delete(prop_ref_data, prop_ref_index);
        prop_ref_index = next_prop_ref_index;
    }

    long *prop_link = &clump->first_prop_index;
    while (*prop_link != -1)
    {
        prop_datum *current_prop = clump_delete_get_prop(*prop_link);
        if ((*prop_link & 0xffff) == (prop_index & 0xffff))
        {
            *prop_link = current_prop->next_prop;
            break;
        }
        else
        {
            prop_link = &current_prop->next_prop;
        }
    }

    ::datum_delete(::prop_data, prop_index);
}

// @address 0x557508 ?actor_remove_prop_ref@@YAXPAUprop_ref_datum@@@Z
void actor_remove_prop_ref(prop_ref_datum *prop_ref)
{
    prop_ref_datum *entry = (prop_ref_datum *)prop_ref;
    actor_datum *actor = actor_prop_ref_actor_get(entry->actor_index);
    long pref_index;
    if (entry->previous_prop_ref_index == -1)
    {
        pref_index = actor->first_prop_ref_index;
        actor->first_prop_ref_index = entry->next_prop_ref_index;
    }
    else
    {
        prop_ref_datum *previous = actor_prop_ref_get(entry->previous_prop_ref_index);
        pref_index = previous->next_prop_ref_index;
        previous->next_prop_ref_index = entry->next_prop_ref_index;
    }
    if (entry->next_prop_ref_index != -1)
    {
        prop_ref_datum *next = actor_prop_ref_get(entry->next_prop_ref_index);
        next->previous_prop_ref_index = entry->previous_prop_ref_index;
    }
    actor_flush_prop(entry->actor_index, pref_index);
    entry->actor_index = -1;
    entry->previous_prop_ref_index = -1;
    entry->next_prop_ref_index = -1;
}

void clump_remove_prop_ref(prop_ref_datum *prop_ref)
{
    prop_ref_datum *entry = (prop_ref_datum *)prop_ref;
    if (entry->prop_previous_ref_index == -1) {
        datum_get<prop_datum>(::prop_data, entry->prop_index)->first_prop_ref_index = entry->prop_next_ref_index;
    } else {
        datum_get<prop_ref_datum>(prop_ref_data, entry->prop_previous_ref_index)->prop_next_ref_index = entry->prop_next_ref_index;
    }
    if (entry->prop_next_ref_index != -1)
        datum_get<prop_ref_datum>(prop_ref_data, entry->prop_next_ref_index)->prop_previous_ref_index =
            entry->prop_previous_ref_index;
    entry->prop_index = -1;
    entry->prop_previous_ref_index = -1;
    entry->prop_next_ref_index = -1;
}
static __forceinline clump_datum *clump_get(long index) {
    return (clump_datum *)(clump_data->data + (index & 0xffff)*sizeof(clump_datum));
}
static __forceinline actor_datum *actor_get(long index) {
    return (actor_datum *)(actor_data->data + (index & 0xffff)*sizeof(actor_datum));
}


static __forceinline bool clump_delete_invitation_valid(invitation_data *invitation) {
    return (unsigned short)invitation->type != 65535 && invitation->handle != -1;
}
static __forceinline void clump_delete_invitation_clear(invitation_data *invitation) {
    invitation->type = -1;
    invitation->time = 0;
    invitation->handle = -1;
    invitation->slot = -1;
    invitation->mode = 0;
}
// @address 0x554fe0 ?clump_delete@@YAXJ@Z
void clump_delete(long clump_index)
{
    clump_datum *clump = (clump_datum *)clump_get(clump_index);
    clump_prop_iterator prop_iterator;
    clump_prop_iterator_new(&prop_iterator, clump);
    while (clump_prop_iterator_next(&prop_iterator)) {
        clump_delete_prop((clump_datum *)clump, prop_iterator.index);
    }
    clump_actor_iterator actor_iterator;
    clump_actor_iterator_new(&actor_iterator, clump_index);
    while (clump_actor_iterator_next(&actor_iterator)) {
        clump_remove_actor(clump_index, actor_iterator.actor_index);
    }
    for (short i = 0; i < 10; ++i) {
        invitation_data *invitation = &clump->invitations[i];
        if (clump_delete_invitation_valid(invitation)) {
            joint_state_datum *joint = (joint_state_datum *)(joint_state_data()->data + (unsigned short)invitation->handle * 204);
            joint->clump_index = -1;
            clump_delete_invitation_clear(invitation);
        }
    }
    ::datum_delete(clump_data, clump_index);
}
// @address 0x555490 clump_remove_actor
extern "C" void clump_remove_actor(long clump_index, long actor_index) {
    clump_datum *clump = clump_get(clump_index);
    long *actor_link = &clump->first_actor_index;
    while (*actor_link != -1) {
        actor_datum *actor = actor_get(*actor_link);
        if (*actor_link == actor_index) {
            *actor_link = actor->next_clump_actor_index;
            actor->clump_index = -1;
            actor_prop_ref_iterator iterator;
            actor_prop_ref_iterator_new(&iterator, actor_index);
            prop_ref_datum *prop_ref;
            while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0) {
                clump_remove_prop_ref(prop_ref);
            }
            --clump->actor_count;
            break;
        } else {
            actor_link = &actor->next_clump_actor_index;
        }
    }
}

int clump_qsort_compare_temp_props(const void *left_ptr, const void *right_ptr)
{
    const temp_prop *left = (const temp_prop *)left_ptr;
    const temp_prop *right = (const temp_prop *)right_ptr;
    if (left->weight > right->weight)
        return -1;
    else if (left->weight < right->weight)
        return 1;
    else if (left->next_index != -1 && right->next_index == -1)
        return -1;
    else
        return 1;
    return 0;
}

// @address 0x556c08 ?clump_cull_props@@YAFJM@Z
short clump_cull_props(long clump_index, float delta)
{
    clump_datum *clump = clump_get(clump_index);
    float next_expiry = 7.0f;
    float expiry = clump->minimum_prop_importance + 0.2f;
    short culled = 0;
    clump_prop_iterator iterator;
    clump_prop_iterator_new(&iterator, clump);
    prop_datum *prop;
    while ((prop = clump_prop_iterator_next(&iterator)) != 0) {
        if (prop->importance <= expiry) {
            clump_delete_prop((clump_datum *)clump, iterator.index);
            culled++;
        } else if (prop->importance < next_expiry) {
            next_expiry = prop->importance;
        }
    }
    clump->minimum_prop_importance = next_expiry;
    clump->activation_value = expiry;
    clump->prop_acquisition_delay_ticks = (short)game_seconds_to_ticks_round(15.0f);
    return culled;
}
static __forceinline clump_datum *clump_new_prop_scan_next(clump_actor_cursor *scan)
{
    clump_datum *result = 0;
    if (ai_globals->enabled) {
        void *datum = ::data_iterator_next((::s_data_iterator *)&scan->iterator);
        scan->datum = datum;
        if (datum)
            result = (clump_datum *)datum;
        scan->index = scan->iterator.index;
    }
    return result;
}
static const float k_clump_prop_activation_delta = 0.2f;
// @address 0x556d18 ?clump_new_prop_internal@@YAJPAUclump_datum@@M@Z
long clump_new_prop_internal(clump_datum *clump, float weight)
{
    long result = -1;
    if (weight > clump->activation_value)
    {
        result = datum_new(::prop_data);
        if (result == -1)
        {
            long game_time = TLS_GAME_TIME_GLOBALS->time;
            short culled = 0;
            for (;;)
            {
                float best_score = 0.0f;
                long best_clump_index = -1;
                clump_actor_cursor iterator;
                if (ai_globals->enabled)
                {
                    iterator.iterator.data = clump_data;
                    iterator.iterator.absolute_index = -1;
                    iterator.iterator.index = -1;
                }
                clump_datum *other;
                while ((other = clump_new_prop_scan_next(&iterator)) != 0)
                {
                    if (other->activation_value + k_clump_prop_activation_delta < 7.0f && weight > other->minimum_prop_importance)
                    {
                        float score = (6.0f - other->minimum_prop_importance) * 10.0f;
                        if (!other->active)
                        {
                            score += 3.0f;
                            float age = (float)(game_time - other->last_active_time) * TLS_GAME_TIME_GLOBALS->tick_length / 300.0f;
                            score += 7.0f * PIN(age, 0.0f, 1.0f);
                        }
                        if (score > best_score)
                        {
                            best_score = score;
                            best_clump_index = iterator.index;
                        }
                    }
                }
                if (best_clump_index == -1)
                    break;
                culled = clump_cull_props(best_clump_index, k_clump_prop_activation_delta);
                if (culled != 0)
                    break;
            }
            if (culled > 0)
                result = datum_new(::prop_data);
            if (result == -1)
            {
                clump->activation_value = weight + k_clump_prop_activation_delta;
                clump->prop_acquisition_delay_ticks = (short)game_seconds_to_ticks_round(15.0f);
            }
        }
    }
    return result;
}
// @address 0x556f48 clump_new_prop_absorb
// match: 0x556f48: pointer __restrict qualifier prevents alias reload
extern "C" void clump_new_prop_absorb(prop_datum *__restrict dest, const prop_datum *__restrict source)
{
    dest->status = source->status;
    dest->state = source->state;
    dest->next_discarded_area = source->next_discarded_area;
    dest->discarded_area_count = source->discarded_area_count;
    dest->search_area = source->search_area;
    memcpy(&dest->discarded_areas, &source->discarded_areas, sizeof(dest->discarded_areas));
    dest->ineffability_ticks = dest->ineffability_ticks > source->ineffability_ticks ? dest->ineffability_ticks : source->ineffability_ticks;
    dest->comment_ticks = dest->comment_ticks > source->comment_ticks ? source->comment_ticks : dest->comment_ticks;
    dest->lost_ticks = dest->lost_ticks > source->lost_ticks ? source->lost_ticks : dest->lost_ticks;
    dest->flags |= source->flags;
    dest->dialogue_flags |= source->dialogue_flags;
    dest->previous_dialogue_flags |= source->previous_dialogue_flags;
}

namespace {

static __forceinline long clump_prop_creature_actor(long object_index)
{
    creature_datum *object = (creature_datum *)datum_get<s_object_header>(object_header_data, object_index)->datum;
    long actor_index = -1;
    if (object->object.type == 11 && object->creature.link_type == 0)
    {
        swarm_creature_state *state = (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset);
        if (state)
            actor_index = state->swarm_index;
    }
    return actor_index;
}

}
// @address 0x557a88 ?clump_get_prop_datum@@YAJJJ@Z
long clump_get_prop_datum(long clump_index, long object_index)
{
    long result = -1;
    long actor_index = clump_prop_creature_actor(object_index);
    clump_prop_iterator iterator;
    clump_prop_iterator_new(&iterator, clump_index);
    prop_datum *prop;
    while ((prop = clump_prop_iterator_next(&iterator)) != 0)
    {
        if (prop->object_index == object_index ||
            (prop->owned_by_actor && actor_index != -1 && actor_index == prop->actor_index))
        {
            result = iterator.index;
            break;
        }
    }
    return result;
}


extern "C" short clump_timed_entry_is_recent(s_task_group *group, float *state_fractions, float *cumulative_fractions)
{
    if ((float)(TLS_GAME_TIME_GLOBALS->time - group->time) * TLS_GAME_TIME_GLOBALS->tick_length > 30.0f)
        return 0;
    return -1;
}

// 0x557ba8
extern "C" short clump_group_state_get_search_flag(s_task_group *group, float *state_fractions, float *cumulative_fractions)
{
    short result = -1;
    if (cumulative_fractions[3] == 0.0f)
        result = group->flags & 1;
    return result;
}

bool group_state_searching_override(float *state_fractions, float *cumulative_fractions)
{
    return cumulative_fractions[4] > 0.0f;
}

// 0x557bf8
extern "C" short group_state_attacking_next_state(s_task_group *group, float *state_fractions, float *cumulative_fractions)
{
    short result = -1;
    if (cumulative_fractions[3] == 0.0f)
        result = group->flags & 1;
    else if (cumulative_fractions[6] == 0.0f)
        result = 2;
    return result;
}

bool group_state_attacking_override(float *state_fractions, float *cumulative_fractions)
{
    return cumulative_fractions[6] > 0.0f;
}


// @address 0x559740 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_clump_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_clump_data_allocator>::deallocate(void *memory)
{
    joint_state_data() = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x557f90 clump_calculate_postombat_result

short clump_calculate_postombat_result(int clump_index)
{
    float actor_count = 0.0f;
    float death_count = 0.0f;
    short team = clump_get(clump_index)->team;
    short player_side = (team == _game_team_player || game_team_is_ally((e_game_team)team, _game_team_player)) ? (short)0 : (short)1;
    clump_actor_cursor iterator;
    if (ai_globals->enabled) {
        iterator.iterator.data = clump_data;
        iterator.iterator.absolute_index = -1;
        iterator.iterator.index = -1;
    }
    for (;;) {
        clump_datum *clump = 0;
        if (ai_globals->enabled) {
            iterator.datum = (clump_datum *)::data_iterator_next(&iterator.iterator);
            if (iterator.datum != 0) {
                clump = (clump_datum *)iterator.datum;
            }
            iterator.index = iterator.iterator.index;
        }
        if (clump == 0)
            break;
        if (clump->active) {
            short clump_team = clump->team;
            short side = (clump_team == _game_team_player || game_team_is_ally((e_game_team)clump_team, _game_team_player)) ? (short)0 : (short)1;
            if (side == player_side) {
                actor_count += clump->actor_count;
                death_count += clump->death_count;
            }
        }
    }
    if (death_count == 0.0f)
        return 1;
    short result = 0;
    if (death_count >= actor_count * 0.8f)
        result = 2;
    return result;
}

// match: 0x5559a8: the image sums dx*dx, dz*dz, dy*dy; shared distance3d swaps the fmadds chain.
static __forceinline float clump_distance3d(const real_point3d *a,const real_point3d *b) {
 float dx=b->x-a->x; float dy=b->y-a->y; float dz=b->z-a->z;
 float s=dx*dx; s+=dz*dz; s+=dy*dy; return sqrtf(s);
}
// @address 0x5559a8 ?clump_update_player_distance@@YAXJ@Z
void clump_update_player_distance(long index) {
 clump_datum *c=datum_get<clump_datum>(clump_data, index);
 float nearest=FLT_MAX;
 bool found=false;
 c_data_iterator<player_datum> it;
 it.begin(player_data);
 while(it.next()) {
  player_datum *p=it.get_datum();
  if(p->unit_index!=-1) {
   real_point3d origin;
   object_get_origin(p->unit_index,&origin);
   float distance=clump_distance3d(&origin,&c->position);
   if(distance<nearest) {nearest=distance;found=true;}
  }
 }
 if(found) c->player_distance=nearest;
 else c->player_distance=0.0f;
}

extern "C" {
// @address 0x72d2b8 k_prop_type_info
}
static __forceinline prop_view *clump_prop_update_view_get(const prop_ref_datum *ref) {
    prop_view *result=0;
    if(ref->view_index!=-1) {
        tracking_datum *datum = datum_get<tracking_datum>(TLS_PROP_DATA, ref->view_index);
        if(datum) result=&datum->tracking_view;
    }
    return result;
}
static __forceinline void clump_prop_update_acknowledge(prop_datum *prop,const void *state) {
    prop->status=1;
    if(state && (const void *)&prop->state!=state) memcpy(&prop->state,state,sizeof(prop->state));
    else prop_state_refresh(&prop->state,prop->object_index,-1,0);
}
// @address 0x554200 ?clump_prop_update@@YAXJJ@Z
void clump_prop_update(long clump_index,long prop_index) {
    prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
    if(prop->ineffability_ticks>0) prop->ineffability_ticks--;
    float weight=0.0f;
    long best=-1;
    short maximum=0;
    clump_prop_ref_iterator it;
    clump_prop_ref_iterator_new(&it,prop_index);
    prop_ref_datum *ref;
    while((ref=clump_prop_ref_iterator_next(&it))!=0) {
        prop_view *view=clump_prop_update_view_get(ref);
        if(ref->track_weight>weight) { weight=ref->track_weight; best=it.index; }
        if(view && view->status>maximum) maximum=view->status;
    }
    prop->maximum_salience=weight;
    prop->count=maximum;
    if(maximum>11) prop->flags|=128;
    if(prop->status<1) {
        const void *state=0;
        bool acknowledge=false;
        if(k_prop_type_info[prop->type].kind==0) acknowledge=true;
        else if(k_prop_type_info[prop->type].kind==1 && best!=-1) { state=prop_state_get_by_index((int)best); acknowledge=true; }
        if(acknowledge) clump_prop_update_acknowledge(prop,state);
    }
}

 
// @address 0x5543d8 ?clump_props_update@@YAXJ@Z
void clump_props_update(long index) {
 clump_datum *c=datum_get<clump_datum>(clump_data, index);
 float weight=0.0f;
 bool found=false;
 clump_prop_iterator it;
 clump_prop_iterator_new(&it,index);
 prop_datum *p;
 while((p=clump_prop_iterator_next(&it))!=0) {
  clump_prop_update(index,it.index);
  if(p->maximum_salience>weight) { weight=p->maximum_salience; found=true; }
 }
 if(found) c->maximum_prop_salience=weight;
 else c->maximum_prop_salience=1.0f;
 if(c->prop_acquisition_delay_ticks>0) c->prop_acquisition_delay_ticks--;
}

static __forceinline s_object_header *clump_state_datum_try_and_get(s_data_array *data,long index) {
 s_object_header *result=0;
 if (index != -1)
 {
     long absolute_index = index & 0xffff;
     if (absolute_index < data->first_unallocated)
     {
         s_object_header *h = (s_object_header *)(data->data + data->size * absolute_index);
         long identifier = h->identifier;
         if (identifier != 0 && identifier == (index >> 16))
             result = h;
     }
 }
 return result;
}
static __forceinline unit_datum *clump_state_object_try_and_get_and_verify_type(long index,unsigned long mask) {
 unit_datum *result=0;
 s_object_header *h=clump_state_datum_try_and_get((s_data_array *)object_header_data,index);
 if(h && ((1<<h->object_type)&mask)) result=(unit_datum *)h->datum;
 return result;
}
// @address 0x556788 ?clump_refresh_props@@YAXJ@Z
void clump_refresh_props(long index)
{
 clump_prop_iterator it;
 clump_prop_iterator_new(&it,index);
 prop_datum *p;
 while((p=clump_prop_iterator_next(&it))!=0) {
  unit_datum *u=clump_state_object_try_and_get_and_verify_type(p->object_index,4099);
  if(u) {
   p->ignore=((u->unit.unit_flags>>7)&1)!=0;
   if(p->valid) p->state.prefer_target=((u->unit.unit_flags>>8)&1)!=0;
  }
  const prop_type *type=&k_prop_type_info[p->type];
  if(type->state_update_mode==0 && p->status==1) prop_state_refresh(&p->state,p->object_index,-1,0);
 }
}



// @address 0x554710 clump_compute_membership_score
extern "C" float clump_compute_membership_score(actor_datum *actor, real_point3d *position, clump_datum *clump)
{
 float minimum_distance;
 if (clump->giant != 0)
  minimum_distance = 15.0f;
 else
  minimum_distance = 5.0f;
 float result = 0.0f;
 float distance = distance3d(position, &clump->position);
 if (distance < minimum_distance)
  result = 1.0f;
 else if (distance < clump->radius) {
  short amount = (short)((double)clump->actor_count * 10.0 / distance);
  double value = amount;
  if (value > 1.0)
   value = 1.0;
  result = (float)value;
 }
 return result;
}
// match: 0x5545d8: the aliasing pointers keep clump->position reloads.
static __forceinline real_point3d *clump_position_add(const real_point3d *a,const real_point3d *b,real_point3d *out) {
 out->x=a->x+b->x; out->y=a->y+b->y; out->z=a->z+b->z; return out;
}
static __forceinline real_point3d *clump_position_scale(const real_point3d *a,float scale,real_point3d *out) {
 out->x=scale*a->x; out->y=scale*a->y; out->z=scale*a->z; return out;
}
static __forceinline void clump_position_get_actor_position(long index,real_point3d *out) {
 *out=actor_get(index)->position.body;
}
static __forceinline void clump_position_iterator_new(clump_actor_iterator *it,long index) {
 it->next_actor_index=((clump_datum *)clump_data->data)[datum_index_to_absolute_index(index)].first_actor;
}
static __forceinline actor_datum *clump_position_iterator_next(clump_actor_iterator *it) {
 actor_datum *result=0;
 if(it->next_actor_index!=-1) {
  result=actor_get(it->next_actor_index); it->actor_index=it->next_actor_index; it->next_actor_index=result->next_clump_actor_index;
 }
 return result;
}
// @address 0x5545d8 ?clump_update_position@@YAXJ@Z
void clump_update_position(long index) {
 clump_datum *clump=&((clump_datum *)clump_data->data)[datum_index_to_absolute_index(index)];
 clump->position=k_real_math_origin3d;
 clump->actor_count=0;
 clump_actor_iterator it;
 clump_position_iterator_new(&it,index);
 real_point3d position;
 actor_datum *actor;
 while((actor=clump_position_iterator_next(&it))!=0) {
  clump_position_get_actor_position(it.actor_index,&position);
  clump_position_add(&position,&clump->position,&clump->position);
  clump->actor_count++;
 }
 if(clump->actor_count>0) {
  clump_position_scale(&clump->position,(float)(1.0/(double)clump->actor_count),&clump->position);
 }
}
const float clump_ref_initial_distance=FLT_MAX;
// match: 0x557150: the signed stimulus alias shares the -1 sentinel.
static __forceinline void clump_new_ref_init(prop_ref_datum *ref,prop_datum *prop) {
 ref->accumulated_awareness=0.0f; ref->status_refresh_ticks=0; ref->state_refresh_ticks=0; ref->owner_index=-1; ref->stimulus_type=-1;
 ref->object_index=prop->object_index; ref->type=(signed char)prop->type;
 ref->knowledge_state=(signed char)prop->status;
 ref->prop_index=-1; ref->actor_index=-1; ref->distance=clump_ref_initial_distance; ref->track_weight=0.0f;
 ref->flags=0; ref->flags_1e=4; ref->maximum_awareness=0;
 ref->next_prop_ref_index=-1; ref->previous_prop_ref_index=-1; ref->prop_next_ref_index=-1; ref->prop_previous_ref_index=-1;
 ref->stimulus_decay_ticks=0; ref->dialogue_event=0; ref->dialogue_index=0; ref->dialogue_ticks=0;
}



static __forceinline ai_dialogue_definitions *clump_init_get_dialogue_globals() {
 ai_dialogue_definitions *result=0;
 if(global_game_globals && global_game_globals->ai_globals.count()>0) {
  long index=global_game_globals->ai_globals.begin()->dialogue_globals;
  if(index!=-1) result=TAG_GET('adlg',ai_dialogue_definitions,index);
 }
 return result;
}
static __forceinline float clump_init_random_range(unsigned long *seed,float low,float high) {
 random_seed_next(seed);
 return low+(high-low)*(1.5259022e-5f*(float)(*seed>>16));
}
static __forceinline long clump_init_timer(float seconds) {
 return seconds>0.0f ? (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second*seconds+0.5f) : -1;
}
static __forceinline void clump_init_set_ai_flag(unsigned char *flags,long bit,bool value) {
 if(value) *flags |= (1<<bit); else *flags &= ~(1<<bit);
}



static __forceinline creature_datum *clump_deleted_object_get(long index) {
    return (creature_datum *)datum_get<s_object_header>(object_header_data, index)->datum;
}
static __forceinline swarm_creature_state *clump_deleted_swarm_block_get(creature_datum *object) {
    return object->creature.link_type == 0 ? (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset) : 0;
}
static __forceinline void clump_deleted_swarm_iterator_new(swarm_creature_iterator *iterator, long swarm_index) {
    iterator->current_object_index = -1;
    iterator->next_object_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
}
static __forceinline void clump_deleted_clump_iterator_new(clump_actor_cursor *cursor) {
    if (ai_globals->enabled) {
        cursor->iterator.absolute_index = -1;
        cursor->iterator.index = -1;
        cursor->iterator.data = clump_data;
    }
}
static __forceinline clump_datum *clump_deleted_clump_iterator_next(clump_actor_cursor *cursor) {
    clump_datum *result = 0;
    if (ai_globals->enabled) {
        cursor->datum = ::data_iterator_next(&cursor->iterator);
        if (cursor->datum) result = (clump_datum *)cursor->datum;
        cursor->index = cursor->iterator.index;
    }
    return result;
}
// @address 0x557798 ?clump_handle_deleted_object@@YAXJ@Z
void clump_handle_deleted_object(long deleted_object_index)
{
    bool is_swarm = false;
    bool multiple_props = false;
    long replacement_index = -1;
    creature_datum *object = clump_deleted_object_get(deleted_object_index);
    short object_type = object->object.type;
    if (object_type == 11) {
        swarm_creature_state *swarm = clump_deleted_swarm_block_get(object);
        if (swarm) {
            long swarm_index = swarm->member_swarm_index;
            is_swarm = true;
            if (swarm_index != -1) {
                swarm_creature_iterator iterator;
                clump_deleted_swarm_iterator_new(&iterator, swarm_index);
                while (swarm_creature_iterator_next(&iterator)) {
                    if (iterator.current_object_index != deleted_object_index) {
                        replacement_index = iterator.current_object_index;
                        break;
                    }
                }
            }
        }
    } else if ((1 << object_type) & 4099) {
        multiple_props = true;
    }
    clump_actor_cursor cursor;
    {
        clump_deleted_clump_iterator_new(&cursor);
        clump_datum *clump;
        for (;;) {
            clump = clump_deleted_clump_iterator_next(&cursor);
            if (!clump) break;
            bool changed = false;
            clump_prop_iterator prop_iterator;
            clump_prop_iterator_new(&prop_iterator, cursor.index);
            prop_datum *prop;
            while ((prop = clump_prop_iterator_next(&prop_iterator)) != 0) {
                bool deleted = false;
                if (prop->object_index == deleted_object_index) {
                    changed = true;
                    if (replacement_index != -1) {
                        prop->object_index = replacement_index;
                        deleted = false;
                    } else {
                        clump_delete_prop(clump, prop_iterator.index);
                        deleted = true;
                    }
                } else if (prop->state.object_index == deleted_object_index) {
                    prop->state.object_index = -1;
                    prop->state.b6b = false;
                    prop->state.b6a = false;
                }
                if (!deleted && is_swarm && prop->owned_by_actor) {
                    clump_prop_ref_iterator ref_iterator;
                    clump_prop_ref_iterator_new(&ref_iterator, prop_iterator.index);
                    prop_ref_datum *ref;
                    while ((ref = clump_prop_ref_iterator_next(&ref_iterator)) != 0) {
                        if (ref->object_index == deleted_object_index) {
                            ref->object_index = replacement_index;
                            changed = true;
                        }
                    }
                }
                if (changed && !multiple_props) break;
            }
        }
    }
    if (object->object.type == 1) {

        cursor.iterator.absolute_index = -1;
        cursor.iterator.index = -1;
        cursor.iterator.data = TLS_PROP_DATA;
        for (;;) {
            void *datum = 0;
            cursor.datum = ::data_iterator_next(&cursor.iterator);
            if (cursor.datum) datum = cursor.datum;
            cursor.index = cursor.iterator.index;
            if (!datum) break;
            tracking_datum *tracking = (tracking_datum *)datum;
            if (tracking->state.object_index == deleted_object_index) tracking->state.object_index = -1;
        }
    }
}
static __forceinline bool clump_consider_object_mark(long index) {
 s_object_marker_globals *markers=&OBJECT_MARKER_GLOBALS[g_registered_thread_index-1];
 long previous=markers->marks[datum_index_to_absolute_index(index)];
 long serial=markers->mark;
 markers->marks[datum_index_to_absolute_index(index)]=serial;
 return previous!=serial;
}
// @rdata 0x82058828
static const float k_clump_minimum_consider_weight = 0.050999999f;
static __forceinline float clump_consider_distance_squared(const real_point3d *a,const real_point3d *b) {
 float x=b->x-a->x,y=b->y-a->y,z=b->z-a->z;
 return x*x+(z*z+y*y);
}
static __forceinline bool clump_consider_prop_is_acknowledged(long index) {
 bool result=false;
 prop_datum *prop=datum_get<prop_datum>(::prop_data, index);
 if(prop->status>=1) result=true;
 else {
  clump_prop_ref_iterator it;
  clump_prop_ref_iterator_new(&it,index);
  prop_ref_datum *ref;
  while((ref=clump_prop_ref_iterator_next(&it))!=0) {
   if(ref->knowledge_state>=1) {result=true;break;}
  }
 }
 return result;
}

namespace {
static __forceinline object_datum *clump_recursive_object_get(long object_index) {
 return datum_get<object_header_datum>(object_header_data, object_index)->datum;
}
}

// @address 0x556040 ?recursive_clump_consider_object@@YAXJJPAUtemp_prop@@PAF@Z
void recursive_clump_consider_object(long object_index,long clump_index,temp_prop *temp,short *count) {
 clump_datum *clump=datum_get<clump_datum>(clump_data, clump_index);
 while(object_index!=-1 && *count<100) {
  object_datum *object=clump_recursive_object_get(object_index);
  float weight=clump_consider_object(object_index,clump_index,-1,&temp[*count],_clump_importance_none);
  if(weight>clump->activation_value) ++*count;
  if(object->first_child_object_index!=-1) recursive_clump_consider_object(object->first_child_object_index,clump_index,temp,count);
  object_index=object->next_object_index;
 }
}

// @address 0x556108 ?clump_consider_object@@YAMJJJPAUtemp_prop@@W4e_clump_importance@@@Z
float clump_consider_object(long object_index,long clump_index,long prop_index,temp_prop *temp,e_clump_importance importance) {
 clump_datum *clump=datum_get<clump_datum>(clump_data, clump_index);
 float weight=0.0f;
 short type=0;
 bool ignore=false;
 bool fixed=false;
 object_datum *object=(object_datum *)object_try_and_get_and_verify_type(object_index,~0UL);
 if(object && (!temp || clump_consider_object_mark(object_index))) {
  if(prop_index!=-1) {
   prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
   fixed=prop->expiration_time>TLS_GAME_TIME_GLOBALS->time;
  }
  float distance_squared;
  {
   real_point3d origin;
   object_get_origin(object_index,&origin);
   distance_squared=clump_consider_distance_squared(&clump->position,&origin);
  }
  if(prop_index!=-1 || distance_squared<clump->maximum_perception_distance*clump->maximum_perception_distance) {
   switch(object->type) {
    case _object_type_vehicle:
     if(((unit_datum *)object)->unit.actor_index==-1) {
      if(ai_get_root_vehicle(object_index)!=object_index) weight=0.0f; else weight=1.0f;
      type=4;
      break;
     }
    case _object_type_biped:
    case _object_type_giant: {
     bool dead=((object->damage_flags>>2)&1)!=0;
     bool player=false;
     long team;
     if(((unit_datum *)object)->unit.actor_index!=-1) team=datum_get<actor_datum>(actor_data, ((unit_datum *)object)->unit.actor_index)->team;
     else if(((unit_datum *)object)->unit.player_index!=-1) {
      team=datum_get<player_datum>(player_data, ((unit_datum *)object)->unit.player_index)->configuration.host.team_index;
      player=true;
     } else team=((unit_datum *)object)->unit.team;
     bool enemy;
     if(team==clump->team) enemy=false; else enemy=::game_team_is_enemy((e_game_team)team, (e_game_team)clump->team);
     if(dead) {
      weight=1.0f;
      if(enemy) type=6;
      else {if(clump->combat_state<=1) weight=3.0f; type=7;}
     } else {
      if(enemy) {weight=3.0f;type=1;} else type=2;
      if(player) weight=7.0f;
     }
     break;
    }
    case _object_type_creature:
     if(((creature_datum *)object)->creature.link_type==0) {
      swarm_creature_state *state=(swarm_creature_state *)object_header_block_get(object, &((creature_datum
          *)object)->creature.creature_reference);
      bool enemy=::game_team_is_enemy((e_game_team)((creature_datum *)object)->creature.team, (e_game_team)clump->team);
      if(enemy) {type=1;weight=3.0f;} else {type=2;weight=0.0f;}
      long swarm_index=state->member_swarm_index;
      if(swarm_index!=-1) {
       swarm_creature_iterator iterator;
       iterator.current_object_index=-1;
       iterator.next_object_index=datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
       while(swarm_creature_iterator_next(&iterator)) {
        s_object_marker_globals *markers=&OBJECT_MARKER_GLOBALS[g_registered_thread_index-1];
        markers->marks[datum_index_to_absolute_index(iterator.current_object_index)]=markers->mark;
       }
      }
     }
     break;
    case _object_type_projectile: {
     projectile_definition *definition=(projectile_definition *)tag_get('proj', object->definition);
     if(definition->ai_radius>0.0f && (object->parent==-1 || ((projectile_datum
         *)object)->projectile.flags.test(_projectile_timer_started_bit))) {weight=3.0f;type=3;}
     break;
    }
    case _object_type_equipment: {
     s_equipment_definition *definition=(s_equipment_definition *)tag_get('eqip', object->definition);
     if(definition->ai_radius>0.0f) {weight=3.0f;type=3;}
     break;
    }
    default: ignore=true; break;
   }
   if(weight>=7.0f) fixed=true;
   else if(weight>0.0f && weight<(float)importance) {weight=(float)importance;fixed=true;}
   if(!fixed && weight>0.0f) {
    float distance=sqrtf(distance_squared);
    if(prop_index!=-1 && clump_consider_prop_is_acknowledged(prop_index)) weight-=3.0f*(distance/(2.0f*clump->maximum_perception_distance));
    else weight-=3.0f*(distance/clump->maximum_perception_distance);
    weight=weight>0.0f ? weight : 0.0f;
   }
  }
 }
 if(!fixed) {
  if(prop_index!=-1) {
   prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
   weight+=3.0f*(prop->maximum_salience/clump->maximum_prop_salience);
  }
  weight=(weight>0.0f ? weight : 0.0f)<6.0f ? (weight>0.0f ? weight : 0.0f) : 6.0f;
 }
 float adjusted=weight*(clump->player_distance<10.0f ? 1.0f : (float)pow(weight*(1.0f/7.0f),(clump->player_distance-10.0f)*0.1f+1.0f));
 if(weight>0.05f && adjusted<=0.05f) adjusted=k_clump_minimum_consider_weight;
 float result=adjusted;
 if(prop_index!=-1) {
  prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
  if(result<prop->importance) {float blend=0.9f; result=blend*prop->importance+(1.0f-blend)*result;}
 }
 if(temp) {
  if(!ignore) {temp->type=type;temp->next_index=prop_index;temp->object_index=object_index;temp->weight=result;}
  else {temp->type=0;temp->next_index=-1;temp->object_index=-1;temp->weight=0.0f;}
 }
 return result;
}

// @address 0x5568c0 ?clump_new_prop_init@@YAXJJFMJ@Z
void clump_new_prop_init(long clump_index,long prop_index,short type,float weight,long object_index) {
 clump_datum *clump=datum_get<clump_datum>(clump_data, clump_index);
 prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
 prop->type=type; prop->importance=weight; prop->maximum_salience=0.0f; prop->object_index=object_index; prop->next_prop=-1;
 const prop_type *type_definition=&k_prop_type_info[prop->type];
 if(type_definition->kind==0) prop->status=1; else prop->status=0;
 prop->expiration_time=-1; prop->first_prop_ref_index=-1;
 object_datum *object=(object_datum *)datum_get<s_object_header>(object_header_data, prop->object_index)->datum;
 clump_init_set_ai_flag(&object->ai_flags,1,true);
 if((1<<object->type)&4099) {
  prop->actor_index=((unit_datum *)object)->unit.actor_index;
  prop->team=(short)((unit_datum *)object)->unit.team;
  bool enemy=::game_team_is_enemy((e_game_team)clump->team, (e_game_team)prop->team); prop->valid=enemy;
  prop->allegiance_changed=!enemy;
  prop->player=((unit_datum *)object)->unit.player_index!=-1;
  prop->owned_by_actor=false;
 } else if((1<<object->type)&2048) {
  switch(((creature_datum *)object)->creature.link_type) {
   case 0: {
      swarm_creature_state *state=(swarm_creature_state *)object_header_block_get(object, &((creature_datum
        *)object)->creature.creature_reference);
    prop->actor_index=state->swarm_index; prop->team=((creature_datum *)object)->creature.team;
    bool enemy=::game_team_is_enemy((e_game_team)clump->team, (e_game_team)prop->team); prop->valid=enemy;
    prop->allegiance_changed=!enemy; prop->player=false;
    prop->owned_by_actor=state->member_swarm_index!=-1;
    break;
   }
   case 1:
    prop->actor_index=-1; prop->team=-1; prop->valid=false; prop->allegiance_changed=false; prop->player=false; prop->owned_by_actor=false;
    break;
  }
 } else {
  prop->actor_index=-1; prop->team=-1; prop->valid=false; prop->allegiance_changed=false; prop->player=false; prop->owned_by_actor=false;
 }
 prop_state_new(&prop->state);
 prop->search_area.zone_index=-1; prop->search_area.area_index=-1; prop->discarded_area_count=false;
     prop->next_discarded_area=false; prop->ineffability_ticks=0;
 ai_dialogue_definitions *dialogue=clump_init_get_dialogue_globals();
 if(dialogue) {
  prop->comment_ticks=clump_init_timer(clump_init_random_range(TLS_RANDOM_SEED,dialogue->strike_delay_minimum,dialogue->strike_delay_maximum));
  prop->lost_ticks=clump_init_timer(dialogue->remind_delay);
 } else {
  prop->comment_ticks=-1; prop->lost_ticks=-1;
 }
 prop->flags=0;
 clump_prop_update(clump_index,prop_index);
}


static __forceinline clump_datum *clump_perception_get(long index)
{
    return datum_get<clump_datum>(clump_data, index);
}
static __forceinline actor_datum *clump_perception_actor_get(long index)
{
    return datum_get<actor_datum>(actor_data, index);
}
static __forceinline prop_datum *clump_perception_prop_get(long index)
{
    return datum_get<prop_datum>(::prop_data, index);
}

static __forceinline void clump_perception_swarm_iterator_new(swarm_creature_iterator *iterator, long swarm_index)
{
    iterator->next_object_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
}
static __forceinline creature_datum *clump_perception_swarm_iterator_next(swarm_creature_iterator *iterator)
{
    creature_datum *creature = 0;
    if (iterator->next_object_index != -1)
    {
        creature = (creature_datum *)datum_get<s_object_header>(object_header_data, iterator->next_object_index)->object;
        iterator->current_object_index = iterator->next_object_index;
        swarm_creature_state *link;
        if (creature->creature.link_type == 0 && (link = (swarm_creature_state *)((char *)creature +
            creature->creature.creature_reference.offset)) != 0)
            iterator->next_object_index = link->next_swarm_creature_index;
        else
            iterator->next_object_index = -1;
    }
    return creature;
}
static __forceinline void clump_perception_object_marker_begin()
{
    s_object_marker_globals *markers = &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
    markers->active = true;
    markers->mark++;
}
static __forceinline void clump_perception_object_marker_end()
{
    (&OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1])->active = false;
}
static __forceinline s_object_marker_globals *clump_perception_object_markers()
{
    return OBJECT_MARKER_GLOBALS + g_registered_thread_index - 1;
}
static __forceinline void clump_perception_object_mark(s_object_marker_globals *markers, long object_index)
{
    markers->marks[datum_index_to_absolute_index(object_index)] = markers->mark;
}
static __forceinline bool clump_perception_clusters_test(const s_game_cluster_bit_vectors *pvs, s_cluster_reference cluster)
{
    long bsp_index = cluster.bsp_index;
    long cluster_index = cluster.cluster_index;
    return pvs->bit_vectors[bsp_index].test(cluster_index);
}

static __forceinline long clump_perception_new_prop(long clump_index, short type, float weight, long object_index)
{
    long prop_index = clump_new_prop_internal((clump_datum *)clump_perception_get(clump_index), weight);
    if (prop_index != -1)
        clump_new_prop_init(clump_index, prop_index, type, weight, object_index);
    return prop_index;
}
// @address 0x555aa0 ?clump_perception_update@@YAXJ@Z
void clump_perception_update(long index)
{
    c_object_iterator<object_datum> iterator;
    s_game_cluster_bit_vectors active_pvs;
    s_game_cluster_bit_vectors cluster_pvs;
    temp_prop temp[100];
    clump_datum *clump = clump_perception_get(index);
    short count = 0;
    float max_distance = 50.0f;
    clump_actor_iterator actor_iterator;
    actor_datum *actor;
    actor_iterator.next_actor_index = clump->first_actor_index;
    while ((actor = clump_actor_iterator_next(&actor_iterator)) != 0)
    {
        s_mode_entry *properties = actor_perception_properties_get((int)actor_iterator.actor_index);
        if (properties && properties->maximum_distance > max_distance)
            max_distance = properties->maximum_distance;
    }
    clump->maximum_perception_distance = max_distance;
    clump_update_player_distance(index);
    if (clump->prop_acquisition_delay_ticks == 0)
    {
        if (clump->activation_value > 0.05f)
        {
            clump->activation_value *= 0.95f;
            if (clump->activation_value < 0.05f)
                clump->activation_value = 0.05f;
        }
    }
    memset(&active_pvs, 0, sizeof(active_pvs));
    clump_actor_iterator_new(&actor_iterator, index);
    while ((actor = clump_actor_iterator_next(&actor_iterator)) != 0)
    {
        if ((unsigned char)actor->position.location.cluster_reference.bsp_index != 0xFF)
        {
            structure_bsp_compute_cluster_active_pvs(actor->position.location.cluster_reference, &cluster_pvs);
            game_clusters_or(&cluster_pvs, &active_pvs, &active_pvs);
        }
        else
            memset(&actor->position.location, 0xFF, sizeof(actor->position.location));
    }
    clump_perception_object_marker_begin();
    s_object_marker_globals *markers = clump_perception_object_markers();
    clump_actor_iterator_new(&actor_iterator, index);
    while ((actor = clump_actor_iterator_next(&actor_iterator)) != 0)
    {
        if (actor->swarm)
        {
            swarm_creature_iterator swarm_iterator;
            clump_perception_swarm_iterator_new(&swarm_iterator, actor->swarm_index);
            while (clump_perception_swarm_iterator_next(&swarm_iterator))
                clump_perception_object_mark(markers, swarm_iterator.current_object_index);
        }
        else
            clump_perception_object_mark(markers, actor->unit_index);
    }
    clump_prop_iterator prop_iterator;
    prop_datum *prop;
    clump_prop_iterator_new(&prop_iterator, index);
    while ((prop = clump_prop_iterator_next(&prop_iterator)) != 0)
    {
        long object_index = prop->object_index;
        if (clump_consider_object(object_index, index, prop_iterator.index, &temp[count], _clump_importance_none) <= clump->activation_value)
            clump_delete_prop((clump_datum *)clump, prop_iterator.index);
        else
            count++;
    }
    iterator.begin(0x182b, 0);
    while (iterator.next())
    {
        object_datum *object = iterator.get_datum();
        if ((unsigned char)object->location.cluster_reference.bsp_index != 0xFF && clump_perception_clusters_test(&active_pvs,
            object->location.cluster_reference))
            recursive_clump_consider_object(iterator.get_index(), index, temp, &count);
    }
    clump_perception_object_marker_end();
    qsort(temp, count, sizeof(temp_prop), clump_qsort_compare_temp_props);
    for (short i = 0; i < count; i++)
    {
        short selected = i;
        if (count >= 20)
            selected = (short)((i + 20) % count);
        temp_prop *slot = &temp[selected];
        if (selected < 20)
        {
            long existing_index = slot->next_index;
            if (existing_index == -1)
            {
                long new_prop_index = clump_perception_new_prop(index, slot->type, slot->weight, slot->object_index);
                if (new_prop_index != -1)
                    ::clump_add_prop(index, new_prop_index);
            }
            else
            {
                s_data_array *prop_data = (s_data_array *)::prop_data;
                long absolute_index = existing_index & 0xffff;
                if (absolute_index < prop_data->first_unallocated)
                {
                    prop_datum *entry = (prop_datum *)(prop_data->data + absolute_index * prop_data->size);
                    if (entry->salt != 0 && entry->salt == (existing_index >> 16))
                    {
                        entry->importance = slot->weight;
                        entry->type = slot->type;
                    }
                }
            }
        }
        else
        {
            long existing_index = slot->next_index;
            if (existing_index != -1)
            {
                s_data_array *prop_data = (s_data_array *)::prop_data;
                long absolute_index = existing_index & 0xffff;
                if (absolute_index < prop_data->first_unallocated)
                {
                    prop_datum *entry = (prop_datum *)(prop_data->data + absolute_index * prop_data->size);
                    if (entry->salt != 0 && entry->salt == (existing_index >> 16))
                        clump_delete_prop((clump_datum *)clump, existing_index);
                }
            }
        }
    }
    float best_weight = 7.0f;
    clump_prop_iterator_new(&prop_iterator, index);
    while ((prop = clump_prop_iterator_next(&prop_iterator)) != 0)
    {
        if (best_weight < prop->importance)
            best_weight = prop->importance;
    }
    clump->minimum_prop_importance = best_weight;
}

// @address 0x557150 ?clump_prop_ref_new@@YAJJ@Z
long clump_prop_ref_new(long prop_index) {
 prop_datum *prop=datum_get<prop_datum>(::prop_data, prop_index);
 long result=datum_new(prop_ref_data);
 if(result!=-1) {
  prop_ref_datum *ref=datum_get<prop_ref_datum>(prop_ref_data, result);
  clump_new_ref_init((prop_ref_datum *)ref,prop);
 }
 return result;
}

static __forceinline void clump_prop_ref_attach(long prop_index, long prop_ref_index)
{
    prop_datum *prop = datum_get<prop_datum>(::prop_data, prop_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    long first_prop_ref_index = prop->first_prop_ref_index;
    if (first_prop_ref_index != -1)
    {
        prop_ref_datum *first_prop_ref = datum_get<prop_ref_datum>(prop_ref_data, first_prop_ref_index);
        first_prop_ref->prop_previous_ref_index = prop_ref_index;
    }
    prop_ref->prop_next_ref_index = first_prop_ref_index;
    prop->first_prop_ref_index = prop_ref_index;
    prop_ref->prop_index = prop_index;
}
// @address 0x557230 ?clump_add_prop@@YAXJJ@Z
void clump_add_prop(long clump_index, long prop_index)
{
    clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
    prop_datum *prop = datum_get<prop_datum>(::prop_data, prop_index);
    prop->next_prop = clump->first_prop_index;
    clump->first_prop_index = prop_index;
    if (clump->minimum_prop_importance > prop->importance)
        clump->minimum_prop_importance = prop->importance;
    clump_actor_iterator iterator;
    clump_actor_iterator_new(&iterator, clump);
    while (clump_actor_iterator_next(&iterator) != 0)
    {
        long prop_ref_index = clump_prop_ref_new(prop_index);
        if (prop_ref_index != -1)
        {
            clump_prop_ref_attach(prop_index, prop_ref_index);
            actor_add_prop_ref(iterator.actor_index, prop_ref_index);
        }
    }
}


// @rdata 0x82022dbc 4
static const float k_clumps_perception_update_seconds = 0.1f;
static __forceinline void *clumps_update_iterator_next(clump_actor_cursor *cursor)
{
    void *result = 0;
    if (ai_globals->enabled)
    {
        void *datum = data_iterator_next(&cursor->iterator);
        cursor->datum = datum;
        if (datum)
            result = datum;
        cursor->index = cursor->iterator.index;
    }
    return result;
}

// @address 0xac440 ?clumps_update@@YAXXZ
void clumps_update()
{
    short update_interval = (short)game_seconds_to_ticks_round(1.0f);
    short perception_interval = (short)game_seconds_to_ticks_round(k_clumps_perception_update_seconds);
    clump_actor_cursor cursor;
    if (ai_globals->enabled) {
        cursor.iterator.absolute_index = -1;
        cursor.iterator.index = -1;
        cursor.iterator.data = clump_data;
    }
    clump_datum *clump;
    while ((clump = (clump_datum *)clumps_update_iterator_next(&cursor)) != 0) {
        if (clump->active) {
            clump_delete_stale_props(cursor.index);
            clump_invitations_update(cursor.index);
            clump_update_behavior_state(cursor.index);
        }
        clump_props_update(cursor.index);
        if (clump->active)
            clump_dialogue_update(cursor.index);
    }
    long time = TLS_GAME_TIME_GLOBALS->time;
    if (time % update_interval == 0) {
        clumps_update_state();
        return;
    }
    if (time % perception_interval != 0)
        return;
    short maximum_age = 0;
    bool updated = false;
    if (ai_globals->enabled) {
        cursor.iterator.absolute_index = -1;
        cursor.iterator.index = -1;
        cursor.iterator.data = clump_data;
    }
    clump_datum *found;
    while ((found = (clump_datum *)clump_actor_cursor_next(&cursor)) != 0) {
        if (found->active) {
            if (found->age == ai_globals->age && !updated) {
                clump_perception_update(cursor.index);
                clump_refresh_props(cursor.index);
                found->age = 0;
                updated = true;
            } else {
                found->age++;
                if (found->age > maximum_age)
                    maximum_age = found->age;
            }
        } else {
            found->age = 0;
        }
    }
    ai_globals->age = maximum_age;
}

// @address 0xac670 clump_delete_stale_props
namespace {

static __forceinline void *clump_stale_object_try_and_get(long object_index) {
    s_data_array *data=(s_data_array *)object_header_data;
    void *result=0;
    if (object_index!=-1) {
        long absolute_index=object_index & 0xffff;
        if (absolute_index<data->first_unallocated) {
            object_header_datum *header=(object_header_datum *)(data->data+data->size*absolute_index);
            long identifier=header->identifier;
            if (identifier!=0 && identifier==(object_index>>16) && (1<<header->object_type)!=0)
                result=header->datum;
        }
    }
    return result;
}
}
extern "C" void clump_delete_stale_props(long clump_index)
{
    clump_datum *clump=datum_get<clump_datum>(clump_data, clump_index);
    clump_prop_iterator iterator;
    clump_prop_iterator_new(&iterator,clump_index);
    prop_datum *prop;
    while ((prop=clump_prop_iterator_next(&iterator))!=0) {
        if (!clump_stale_object_try_and_get(prop->object_index))
            clump_delete_prop((clump_datum *)clump,iterator.index);
    }
}
namespace {

static __forceinline void clump_group_state_new(s_task_group *group) { group->state=0; group->flags=0; group->time=TLS_GAME_TIME_GLOBALS->time; }


static __forceinline real_point3d *clump_new_set_point(real_point3d *point, float x, float y, float z) { point->x=x; point->y=y;
    point->z=z; return point; }
static __forceinline long clump_new_time_get() { return TLS_GAME_TIME_GLOBALS->time; }
static __forceinline clump_datum *clump_new_get(long index) { return datum_get<clump_datum>(clump_data, index); }
}
namespace {
static __forceinline void clump_membership_actor_iterator_new(actor_iterator *iterator, bool active_only)
{
    if (ai_globals->enabled) {
        iterator->m_iterator.absolute_index = -1;
        iterator->m_iterator.index = -1;
        iterator->active_only = active_only;
        iterator->m_iterator.data = actor_data;
    }
}
static __forceinline void clump_membership_iterator_new(clump_actor_cursor *iterator)
{
    if (ai_globals->enabled) {
        iterator->iterator.absolute_index = -1;
        iterator->iterator.index = -1;
        iterator->iterator.data = clump_data;
    }
}
static __forceinline clump_datum *clump_membership_iterator_next(clump_actor_cursor *iterator)
{
    clump_datum *result = 0;
    if (ai_globals->enabled) {
        void *datum = ::data_iterator_next(&iterator->iterator);
        iterator->datum = datum;
        if (datum)
            result = (clump_datum *)datum;
        iterator->index = iterator->iterator.index;
    }
    return result;
}
static __forceinline clump_datum *clump_membership_get(long clump_index)
{
    return (clump_datum *)(clump_data->data + (clump_index & 0xffff) * sizeof(clump_datum));
}
static __forceinline void clump_membership_get_actor_position(long actor_index, real_point3d *position)
{
    *position = actor_get(actor_index)->position.body;
}
static __forceinline bool clump_membership_actor_can_join(clump_datum *clump, actor_datum *actor)
{
    return clump->team == actor->team && clump->giant == actor_is_giant(actor);
}
}

// @address 0x5547e0 ?clumps_update_actor_membership@@YAXXZ
void clumps_update_actor_membership(void)
{
    actor_iterator iterator;
    clump_membership_actor_iterator_new(&iterator, true);
    actor_datum *actor;
    while ((actor = actor_iterator_next(&iterator)) != 0) {
        bool giant = actor_is_giant(actor);
        real_point3d position;
        clump_membership_get_actor_position(iterator.index, &position);
        clump_actor_cursor clump_iterator;
        clump_membership_iterator_new(&clump_iterator);
        float best_score = 0.0f;
        long best_clump_index = -1;
        clump_datum *clump;
        while ((clump = clump_membership_iterator_next(&clump_iterator)) != 0) {
            if (clump_membership_actor_can_join(clump, actor)) {
                float score = clump_compute_membership_score(actor, &position, clump);
                if (score > best_score) {
                    best_score = score;
                    best_clump_index = clump_iterator.index;
                }
            }
        }
        if (best_clump_index != -1) {
            if (best_clump_index != actor->clump_index) {
                if (actor->clump_index != -1) {
                    clump_absorb_memory(best_clump_index, actor->clump_index);
                    clump_remove_actor(actor->clump_index, iterator.index);
                }
                clump_add_actor(best_clump_index, iterator.index);
            }
        } else {
            long clump_index = clump_new(&position, actor->team, giant);
            if (clump_index != -1) {
                if (actor->clump_index != -1) {
                    clump_membership_get(clump_index)->position = position;
                    clump_absorb_memory(clump_index, actor->clump_index);
                    clump_remove_actor(actor->clump_index, iterator.index);
                }
                clump_add_actor(clump_index, iterator.index);
            }
        }
    }
}

// @address 0x554dc0 ?clump_new@@YAJPATreal_point3d@@F_N@Z
// match: 0x554dc0: pointer __restrict qualifier prevents alias reload
long clump_new(real_point3d *__restrict point, short p1, bool p2)
{

    long index = datum_new(clump_data);
    if (index == -1) {
        ai_globals_type *globals = ai_globals;
        clump_actor_cursor cursor;
        if (globals->enabled) {
            cursor.iterator.data = clump_data;
            cursor.iterator.absolute_index = -1;
            cursor.iterator.index = -1;
        }
        long oldest = -1;
        long oldest_time = 0x7fffffff;
        for (;;) {
            void *datum = 0;
            if (globals->enabled) {
                void *next = ::data_iterator_next(&cursor.iterator);
                cursor.datum = next;
                if (next) datum = next;
                cursor.index = cursor.iterator.index;
            }
            if (!datum) break;
            long time = ((clump_datum *)datum)->last_active_time;
            if (time < oldest_time) {
                oldest_time = time;
                oldest = cursor.index;
            }
        }
        if (oldest != -1) {
            clump_delete(oldest);
            index = datum_new(clump_data);
        }
    }
    if (index != -1) {
        clump_datum *clump = clump_new_get(index);
        clump_new_set_point(&clump->position, point->x, point->y, point->z);
        clump->actor_count = 0;
        clump->team = p1;
        clump->giant = p2;
        clump->radius = p2 ? 20.0f : 8.0f;
        clump->first_actor_index = -1;
        clump->first_prop_index = -1;
        clump->active = 1;
        clump->age = 0;
        clump->last_active_time = clump_new_time_get();
        clump_group_state_new(&clump->group);
        clump->maximum_perception_distance = 50.0f;
        clump->activation_value = 0.05f;
        clump->dialogue_flags = 0;
        clump->minimum_prop_importance = 7.0f;
        clump->last_time = -1;
        clump->maximum_prop_salience = 1.0f;
        clump->last_visible_enemy_time = -1;
        clump->player_distance = 0.0f;
        clump->disposable_actors_pending = 0;
        clump->update_count = 0;
        clump->dialogue_state = 1;
        clump->s58 = 0;
        short i;
        for (i = 0; i < 1; i++) clump->refs[i] = -1;
        for (i = 0; i < 10; i++) {
            invitation_data *inv = &clump->invitations[i];
            inv->type = -1; inv->time = 0; inv->handle = -1; inv->slot = -1; inv->mode = 0;
        }
    }
    return index;
}


namespace {


static __forceinline actor_datum *clump_berserk_actor_get(long index) {
    return (actor_datum *)(actor_data->data + (unsigned short)index * 0xa8c);
}

static __forceinline float clump_berserk_distance(const actor_datum *actor, const real_point3d *point) {
    float dy = actor->position.body.y - point->y;
    float dz = actor->position.body.z - point->z;
    float dx = actor->position.body.x - point->x;
    return dx * dx + (dz * dz + dy * dy);
}
}



namespace {

static __forceinline clump_datum *clump_bs_get(long index) { return datum_get<clump_datum>(clump_data, index); }
static __forceinline void clump_bs_group_set_state(s_task_group *group, short state)
{
    group->state = state;
    group->time = TLS_GAME_TIME_GLOBALS->time;
    if (state == 0)
        group->flags &= -2;
}
static __forceinline void clump_bs_reset(long index) {
    clump_datum *clump=clump_bs_get(index);
    clump->dialogue_ready=0;
    clump->update_count=0;
    clump_group_state_new(&clump->group);
    clump->s58=0;
}

static __forceinline void clump_bs_iterator_new(clump_actor_iterator *iterator, long index) { iterator->next_actor_index =
    ((clump_datum *)clump_bs_get(index))->first_actor_index; }
static __forceinline actor_datum *clump_bs_iterator_next(clump_actor_iterator *iterator) {
    actor_datum *result=0;
    if (iterator->next_actor_index != -1) {
        result = datum_get<actor_datum>(actor_data, iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = result->next_clump_actor_index;
    }
    return result;
}
}
void clump_update_behavior_state(long index)
{
    float counts[14];
    float totals[14];
    bool all_flagged = true;
    clump_datum *clump = clump_bs_get(index);
    for (int i = 0; i < 14; i++) { counts[i] = 0.0f; totals[i] = 0.0f; }
    clump_actor_iterator iterator;
    clump_bs_iterator_new(&iterator, index);
    actor_datum *actor;
    while ((actor = clump_bs_iterator_next(&iterator)) != 0) {
        short group = actor->situation.best_prop_class;
        for (int i = 0; i <= group; i++) totals[i] = (float)(totals[i] + 1.0);
        counts[group] = (float)(counts[group] + 1.0);
        all_flagged = actor->suppress_combat & all_flagged;
    }
    if (clump->actor_count > 0) {
        for (int i = 0; i < 14; i++) {
            counts[i] = counts[i] / (float)clump->actor_count;
            totals[i] = totals[i] / (float)clump->actor_count;
        }
    }
    if (!(clump->group.flags & 1) && totals[7] > 0.0f && !all_flagged) clump->group.flags |= 1;
    s_task_group *group = &clump->group;
    short state = task_group_compute_next_state(group, counts, totals);
    if (state != -1 && state != group->state) {
        clump_bs_group_set_state(group,state);
        if (state == 0) {
            clump_bs_reset(index);
        }
    } else if (group->state == 1) {
        if (TLS_GAME_TIME_GLOBALS->time - clump->group.time == game_seconds_to_ticks_round(1.0f)) {
            long unit = clump_get_first_unit(index);
            if (unit != -1) {
                short dialogue_index = -1;
                e_dialogue_state dialogue_state;
                if (clump_dialogue_event_allowed(index, -1, true, prop_dialogue_event_1, &dialogue_index, &dialogue_state)) {
                    clump_datum *speaker = clump_bs_get(index);
                    c_dialogue_event event(54, unit, -1, -1, -1, 0);
                    ai_dialogue_event(&event);
                    if (dialogue_state != 0) speaker->dialogue_state = dialogue_state;
                }
            }
        }
    }
}
// @address 0x557088 ?clump_new_prop@@YAJJFMJPAUprop_datum@@@Z
long clump_new_prop(long clump_index,short type,float weight,long object_index,prop_datum *source)
{
    clump_datum *clump=(clump_datum *)clump_new_get(clump_index);
    prop_datum copy;
    if(source) memcpy(&copy,source,sizeof(copy));
    long result=clump_new_prop_internal(clump,weight);
    if(result!=-1) {
        prop_datum *prop=datum_get<prop_datum>(::prop_data, result);
        clump_new_prop_init(clump_index,result,type,weight,object_index);
        if(source) clump_new_prop_absorb(prop,&copy);
    }
    return result;
}

static const float k_clump_dead_cull_delay_seconds = 10.0f;
static __forceinline actor_datum *clump_dead_actor_try_get(long index)
{
    s_data_array *data = actor_data;
    actor_datum *actor = 0;
    if (index != -1 && (long)(index & 0xffff) < data->first_unallocated) {
        actor_datum *datum = (actor_datum *)(data->data + data->size * (index & 0xffff));
        if (datum->identifier && (long)datum->identifier == (index >> 16))
            actor = datum;
    }
    return actor;
}


// @address 0x554a20 ?cull_dead_clumps@@YAXXZ
void cull_dead_clumps()
{
    clump_actor_cursor iterator;
    clump_deleted_clump_iterator_new(&iterator);
    clump_datum *item;
    while ((item = clump_deleted_clump_iterator_next(&iterator)) != 0) {
        clump_datum *clump = item;
        if (clump->disposable_actors_pending &&
            (clump->last_visible_enemy_time == -1 ||
                clump->last_visible_enemy_time + game_seconds_to_ticks_round(k_clump_dead_cull_delay_seconds) < TLS_GAME_TIME_GLOBALS->time))
        {
            clump->disposable_actors_pending = false;
            long actor_indices[256];
            short count = 0;
            clump_actor_iterator actors;
            clump_actor_iterator_new(&actors, clump);
            while (clump_actor_iterator_next(&actors))
                { long value = actors.actor_index; actor_indices[count++] = value; }
            for (short i = 0; i < count; ++i) {
                long index = actor_indices[i];
                actor_datum *actor = clump_dead_actor_try_get(index);
                if (!actor || !actor->disposable) continue;
                bool erase = true;
                if (actor->active) {
                    bool nearby = false;
                    c_data_iterator<player_datum> players;
                    players.begin(player_data);
                    while (players.next()) {
                        if (players.get_datum()->unit_index == -1) continue;
                        real_point3d point;
                        object_get_origin(players.get_datum()->unit_index, &point);
                        float x = point.x-actor->position.body.x, y = point.y-actor->position.body.y, z = point.z-actor->position.body.z;
                        if ((x*x + z*z) + y*y < 225.0f) { nearby = true; break; }
                    }
                    if (!nearby && actor->unit_index != -1) nearby = object_visible_to_any_player(actor->unit_index);
                    erase = !nearby;
                }
                if (erase) {
                    long vehicle = -1;
                    if (actor->root_vehicle_index != -1 && actor->root_vehicle_index != actor->unit_index)
                        vehicle = actor->root_vehicle_index;
                    actor_erase(index, true);
                    if (vehicle != -1 && object_try_and_get_and_verify_type(vehicle, ~0UL)) {
                        bool remove = true;
                        c_multi_unit_passenger_iterator passengers;
                        passengers.begin(vehicle);
                        while (passengers.next()) {
                            object_datum *unit =
                                (object_datum *)datum_get<s_object_header>(object_header_data, passengers.m_passenger_index)
                                    ->datum;
                            if (unit->object_type == 0) { remove = false; break; }
                        }
                        if (remove) object_delete(vehicle);
                    }
                } else clump->disposable_actors_pending = true;
            }
        }
        if (clump->actor_count == 0) clump_delete(iterator.index);
    }
}

namespace {
static __forceinline bool clump_state_has_active_actor(const clump_datum *clump)
{
    bool result = false;
    clump_actor_iterator iterator;
    iterator.next_actor_index = clump->first_actor_index;
    actor_datum *actor;
    while ((actor = clump_actor_iterator_next(&iterator)) != 0)
    {
        if (actor->active)
        {
            result = true;
            break;
        }
    }
    return result;
}
}

// @address 0x5544a8 ?clumps_update_state@@YAXXZ
void clumps_update_state(void)
{
    clump_actor_cursor iterator;
    clump_membership_iterator_new(&iterator);
    clump_datum *datum;
    while ((datum = (clump_datum *)clump_membership_iterator_next(&iterator)) != 0) {
        if (datum->active) {
            long clump_index = iterator.index;
            clump_datum *clump = clump_get(clump_index);
            if (clump->active) {
                if (!clump_state_has_active_actor(clump)) {
                    clump->active = 0;
                    clump->last_active_time = TLS_GAME_TIME_GLOBALS->time - 1;
                } else {
                    clump->last_active_time = TLS_GAME_TIME_GLOBALS->time;
                }
            }
            clump_update_position(clump_index);
        }
    }
    clumps_update_actor_membership();
    cull_dead_clumps();
}


// @address 0x557670 ?clump_aquire_prop@@YAJJJW4e_clump_importance@@@Z
long clump_aquire_prop(long clump_index, long object_index, e_clump_importance importance)
{
    temp_prop prop;
    prop.weight = 0.0f;
    prop.type = 0;
    prop.next_index = -1;
    prop.object_index = -1;
    s_object_marker_globals *markers = &OBJECT_MARKER_GLOBALS[g_registered_thread_index-1];
    markers->active = true;
    markers->mark++;
    clump_consider_object(object_index, clump_index, -1, &prop, importance);
    float weight = prop.weight;
    (&OBJECT_MARKER_GLOBALS[g_registered_thread_index-1])->active = false;
    long prop_index = clump_new_prop_internal(clump_get(clump_index), weight);
    if (prop_index != -1) {
        clump_new_prop_init(clump_index, prop_index, prop.type, weight, object_index);
        datum_get<prop_datum>(::prop_data, prop_index)->importance = (float)importance;
        ::clump_add_prop(clump_index, prop_index);
    }
    return prop_index;
}


static __forceinline prop_datum *clump_add_actor_prop_get(long prop_index)
{
    return datum_get<prop_datum>(::prop_data, prop_index);
}
static __forceinline void clump_add_actor_activate(long clump_index)
{
    clump_datum *clump = clump_get(clump_index);
    clump->activation_value = 0.05f;
    clump->active = true;
}

void clump_add_actor(long clump_index, long actor_index)
{
    clump_datum *clump = clump_get(clump_index);
    actor_datum *actor = actor_get(actor_index);
    clump->actor_count++;
    {
        actor_prop_ref_iterator iterator;
        actor_prop_ref_iterator_new(&iterator, actor_index);
        prop_ref_datum *prop_ref;
        while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0) {
            if (prop_ref->knowledge_state <= 0) {
                actor_remove_prop_ref((prop_ref_datum *)prop_ref);
                ::datum_delete(prop_ref_data, iterator.index);
            }
        }
    }
    actor->clump_index = clump_index;
    actor->next_clump_actor_index = clump->first_actor_index;
    clump->first_actor_index = actor_index;
    if (actor->active && !clump->active)
        clump_add_actor_activate(clump_index);
    if (actor->disposable)
        clump->disposable_actors_pending = true;
    clump_perception_object_marker_begin();
    {
        actor_prop_ref_iterator iterator;
        actor_prop_ref_iterator_new(&iterator, actor_index);
        prop_ref_datum *prop_ref;
        while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0) {
            clump_perception_object_mark(clump_perception_object_markers(), prop_ref->object_index);
            long prop_index = clump_get_prop_datum(clump_index, prop_ref->object_index);
            if (prop_index != -1) {
                clump_prop_ref_attach(prop_index, iterator.index);
                prop_datum *prop = clump_add_actor_prop_get(prop_index);
                if (prop_ref->knowledge_state >= 1 && prop->status <= 0) {
                    prop->status = 1;
                    prop_state_refresh(&prop->state, prop->object_index, -1, 0);
                }
            } else {
                actor_remove_prop_ref((prop_ref_datum *)prop_ref);
                if (prop_ref->owner_index != -1)
                    ::datum_delete(TLS_PROP_DATA, prop_ref->owner_index);
                ::datum_delete(prop_ref_data, iterator.index);
            }
        }
    }
    {
        clump_prop_iterator iterator;
        clump_prop_iterator_new(&iterator, clump_index);
        prop_datum *prop;
        while ((prop = clump_prop_iterator_next(&iterator)) != 0) {
            if (clump_consider_object_mark(prop->object_index)) {
                long prop_ref_index = clump_prop_ref_new(iterator.index);
                if (prop_ref_index != -1) {
                    clump_prop_ref_attach(iterator.index, prop_ref_index);
                    actor_add_prop_ref(actor_index, prop_ref_index);
                }
            }
        }
    }
    clump_perception_object_marker_end();
}
static __forceinline void clump_charge_subtract(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = a->x - b->x;
    v->j = a->y - b->y;
    v->k = a->z - b->z;
}
static __forceinline float clump_charge_square(const real_vector3d *v) { return v->i*v->i+(v->k*v->k+v->j*v->j); }
static __forceinline float clump_charge_distance(const actor_datum *actor, const real_point3d *point)
{
    real_vector3d delta;
    clump_charge_subtract(&actor->position.body, point, &delta);
    return clump_charge_square(&delta);
}
static __forceinline bool clump_charge_zero_closer(const long &count)
{
    return count == 0;
}
// match: 0x557c60: the member walk indexes the actor array base loaded once at entry.
static __forceinline actor_datum *clump_berserk_iterator_next(clump_actor_iterator *iterator, actor_datum *actors) {
    actor_datum *result=0;
    if(iterator->next_actor_index != -1) {
        result=&actors[(unsigned short)iterator->next_actor_index];
        iterator->actor_index=iterator->next_actor_index;
        iterator->next_actor_index=result->next_clump_actor_index;
    }
    return result;
}
static __forceinline bool clump_berserk_roll() {
    unsigned long *seed = TLS_RANDOM_SEED;
    random_seed_next(seed);
    return 1.5259022e-5f * (float)(*seed >> 16) < 0.7f;
}
static __forceinline bool clump_charge_roll()
{
    unsigned long *seed = TLS_RANDOM_SEED;
    // match: 0x557c60: reference-temporary LCG step; the shared call reassigns 0x557c60 registers.
    const unsigned long &product = *seed * 1664525;
    *seed = product + 1013904223;
    return 1.5259022e-5f * (float)(*seed >> 16) < 0.7f;
}
static __forceinline void clump_berserk_iterator_new(clump_actor_iterator *iterator, clump_datum *clump) { iterator->next_actor_index
    = clump->first_actor_index; }
// @address 0x557c60 clump_can_berserk
extern "C" bool clump_can_berserk(long clump_index, long actor_index, long target_index)
{
    clump_datum *flags = datum_get<clump_datum>(clump_data, clump_index);
    bool result = false;
    if (!flags->active) result = true;
    else {
    clump_datum *clump = (clump_datum *)flags;
    short maximum;
    long minimum_value;
    long source_index = actor_index;
    const real_point3d *target = 0;
    float distance = FLT_MAX;
    actor_datum *actor = clump_berserk_actor_get(actor_index);
    actor_datum *actors = (actor_datum *)actor_data->data;
    s_character_charge_entry *entry = character_definition_charge_entry_get(actor_index);
    if (entry) { maximum = entry->maximum_flagged; minimum_value = entry->minimum_flagged; }
    else { maximum = 0x7fff; minimum_value = 0; }
    const long &signed_minimum = minimum_value & 0xffff;
    short minimum = (short)signed_minimum;
    long count = 0;
    short no_weapon = 0, closer = 0, flagged = 0;
    if (target_index != -1) {
        prop_state *state = (prop_state *)prop_state_get_by_index(actor->target_prop_index);
        target = &state->position;
        distance = clump_charge_distance(actor, target);
    }
    clump_actor_iterator iterator;
    clump_berserk_iterator_new(&iterator, clump);
    actor_datum *member;
    while ((member = clump_berserk_iterator_next(&iterator, actors)) != 0) {
        const long &next_count = count + 1;
        // match: 0x557c60: const reference binding preserves temporary lifetime
        const short &narrowed_count = (short)next_count;
        count = narrowed_count;
        if (member->berserking && ++flagged >= maximum) break;
        if (!actor_has_ranged_weapon(iterator.actor_index)) ++no_weapon;
        if (target && iterator.actor_index != source_index && distance > clump_charge_distance(member, target)) {
            const long &next_closer = closer + 1;
            closer = (short)next_closer;
        }
    }
    const long &minimum_required = minimum;
    if (flagged < minimum_required && clump_charge_zero_closer(closer)) result = true;
    else if (flagged < maximum) {
        if ((short)count - flagged == 1) result = true;
        else {
            float threshold = (float)(short)count * 0.3f;
            if ((float)closer < threshold) result = clump_berserk_roll();
            else if (!actor_has_ranged_weapon(source_index) && (float)no_weapon < threshold) result = clump_charge_roll();
        }
    }
    }
    return result;
}

// match: 0x555588: datum_get<clump_datum> flips an add operand in clump_absorb_memory.
static __forceinline clump_datum *clump_absorb_get(long index) { return &((clump_datum *)clump_data->data)[datum_index_to_absolute_index(index)]; }
static __forceinline swarm_creature_state *clump_absorb_block(creature_datum *object)
{
    return object->creature.link_type == 0 ? (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset) : 0;
}
static __forceinline prop_datum *clump_absorb_try_prop(s_data_array *data, long index)
{
    prop_datum *result = 0;
    if (index != -1 && (long)(index & 0xffff) < data->first_unallocated) {
        prop_datum *prop = (prop_datum *)(data->data + (index & 0xffff) * data->size);
        if (prop->salt != 0 && prop->salt == (index >> 16))
            result = prop;
    }
    return result;
}
static __forceinline void clump_absorb_iterator_new(clump_prop_iterator *iterator, long clump_index)
{
    clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
    iterator->next_index = clump->first_prop_index;
}
static __forceinline prop_datum *clump_absorb_iterator_next(clump_prop_iterator *iterator)
{
    prop_datum *result = 0;
    if (iterator->next_index != -1) {
        result = datum_get<prop_datum>(::prop_data, iterator->next_index);
        iterator->index = iterator->next_index;
        iterator->next_index = result->next_prop;
    }
    return result;
}
static __forceinline void clump_absorb_marker_begin()
{
    s_object_marker_globals *marker = &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
    marker->active = true;
    ++marker->mark;
}
static __forceinline bool clump_absorb_mark(long object_index)
{
    s_object_marker_globals *marker = &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
    long generation = marker->mark;
    long previous = marker->marks[datum_index_to_absolute_index(object_index)];
    marker->marks[datum_index_to_absolute_index(object_index)] = generation;
    return previous == generation;
}
static __forceinline creature_datum *clump_absorb_creature_next(swarm_creature_iterator *iterator)
{
    creature_datum *result = 0;
    if (iterator->next_object_index != -1) {
        s_data_array *const &data=object_header_data;
        result=(creature_datum *)datum_get<s_object_header>(data, iterator->next_object_index)->datum;
        iterator->current_object_index = iterator->next_object_index;
        swarm_creature_state *block = clump_absorb_block(result);
        iterator->next_object_index = block ? block->next_swarm_creature_index : -1;
    }
    return result;
}
static __forceinline void clump_absorb_mark_with(s_object_marker_globals *marker, long object_index)
{
    long *slot = &marker->marks[datum_index_to_absolute_index(object_index)];
    *slot = marker->mark;
}
static __forceinline void clump_absorb_mark_destination(long destination_index)
{
    s_object_marker_globals *marker = &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
    clump_prop_iterator iterator;
    clump_absorb_iterator_new(&iterator, destination_index);
    prop_datum *prop;
    while ((prop = clump_absorb_iterator_next(&iterator)) != 0) {
        long object_index = prop->object_index;
        creature_datum *object = (creature_datum *)clump_stale_object_try_and_get(object_index);
        if (!object) continue;
        bool single = true;
        if (object->object.type == 11) {
            swarm_creature_state *block = clump_absorb_block(object);
            if (block && block->member_swarm_index != -1) {
                swarm_datum *swarm = datum_get<swarm_datum>(g_swarm_data, block->member_swarm_index);
                swarm_creature_iterator creatures;
                creatures.next_object_index = swarm->first_object_index;
                while (clump_absorb_creature_next(&creatures))
                    clump_absorb_mark_with(marker, creatures.current_object_index);
                single = false;
            }
        }
        if (single) clump_absorb_mark_with(marker, object_index);
    }
}
// @address 0x555588 ?clump_absorb_memory@@YAXJJ@Z
void clump_absorb_memory(long destination_index, long source_index)
{
    clump_datum *destination = clump_absorb_get(destination_index);
    clump_datum *source = clump_absorb_get(source_index);
    clump_absorb_marker_begin();
    clump_absorb_mark_destination(destination_index);
    clump_prop_iterator iterator;
    long pending[40];
    short count = 0;
    clump_absorb_iterator_new(&iterator, source_index);
    while (clump_absorb_iterator_next(&iterator)) {
        if (count < 40) pending[count++] = iterator.index;
    }
    for (short i = 0; i < count; ++i) {
        prop_datum *prop = clump_absorb_try_prop(::prop_data, pending[i]);
        if (!prop) continue;
        long object_index = prop->object_index;
        if (!object_try_and_get_and_verify_type(object_index, ~0UL)) continue;
        if (!clump_absorb_mark(object_index)) {
            long created = clump_new_prop(destination_index, prop->type, prop->importance, prop->object_index, (prop_datum *)prop);
            if (created != -1) clump_add_prop(destination_index, created);
        }
    }
    OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1].active = false;
    if (source->maximum_perception_distance > destination->maximum_perception_distance) destination->maximum_perception_distance =
        source->maximum_perception_distance;
    destination->dialogue_flags |= source->dialogue_flags;
    if (source->last_time > destination->last_time) destination->last_time = source->last_time;
    if (source->combat_state > destination->combat_state)
    {
        destination->combat_state = source->combat_state;
        destination->state_time = source->state_time;
        destination->state_flags |= source->state_flags;
    }
    destination->dialogue_state = destination->dialogue_state > source->dialogue_state ? destination->dialogue_state : source->dialogue_state;
}
