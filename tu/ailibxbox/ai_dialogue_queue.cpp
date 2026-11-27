// ailibxbox/ai_dialogue_queue.obj: from the records 0xd2ff0 0x5233b0 0x523550 0x5238d8 0x523a68 0x523ce8 0x523d68 0x523dd8 0x524070 0x524288 0x524338 0x524418
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/ai_dialogue_queue_data.h"
#include "ai/unit_dialogue_types.h"
#include "ai/ai_dialogue_globals.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "units/unit_types.h"
#include "ai/actor_globals.h"
#include "ai/ai_information_data.h"
#include "ai/ai_globals_definition.h"
#include "game/game_globals.h"
#include <stdlib.h>
#include <string.h>
#include "objects/object_reference_types.h"
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "objects/objects.h"
#include "ai/ai_dialogue.h"
#include "ai/ai_dialogue_queue.h"
#include "sound/game_sound_deterministic.h"
#include "sound/game_sound_deterministic_permutation.h"
#include "units/unit_dialogue.h"
#include "memory/data.h"
#include "game/game_allegiance.h"
#include "scenario/scenario.h"
#include "ai/ai_information_packet.h"
#include "ai/team_dialogue_state.h"
#include "ai/actor_datum.h"
#include "ai/clump_datum.h"
#include "ai/ai_math_constants.h"
#include "ai/dialogue_event.h"
#include "ai/dialogue_event_types.h"
#include "units/dialogue_definitions.h"
#include "ai/ai_dialogue_definitions.h"
// @address 0x5233b0 ?ai_dialogue_queue_unit_vocalization_weight@@YAMJF@Z
// @address 0x524418 ?ai_dialogue_queue_handle_object_deleted@@YAXJ@Z
struct ai_dialogue_definitions;
struct unit_speech_item;
struct team_dialogue_state;
struct s_data_array;

struct s_ai_dialogue_queue_entry {
    short identifier;
    short vocalization;
    long unit_index;
    long sound_index;
    short team;
    signed char response;
    long queued_time;
    long start_time;
    short minimum_delay;
    short speech_delay;
    short duration;
    short window;
    s_game_sound_deterministic_permutation permutation;
    short speech_type;
    long clump_index;
    ai_information_packet information;
    bool spoken;
    bool interrupted;
    long response_index;
    bool deleted;
    long dialogue_variant;
    long next;
};
typedef char dialogue_queue_entry_size[(sizeof(s_ai_dialogue_queue_entry) == 92) ? 1 : -1];

struct s_priority_definition;
struct s_game_globals;
namespace {

static __forceinline s_ai_dialogue_queue_entry *vocalization_record_get(long index)
{
    s_data_array *data = (s_data_array *)g_ai_dialogue_records;
    return datum_get<s_ai_dialogue_queue_entry>(data, index);
}
static __forceinline unit_datum *dialogue_unit_get(long index)
{
    return (unit_datum *)object_get_and_verify_type(index, NONE);
}
static __forceinline s_unit_dialogue *unit_dialogue_state_get(unit_datum *unit)
{
    return (s_unit_dialogue *)object_header_block_get(unit, &unit->unit.dialogue_reference);
}
static __forceinline void vocalization_record_delete(long index)
{
    s_ai_dialogue_queue_entry *record = vocalization_record_get(index);
    if (record->unit_index != -1) {
        unit_datum *unit = dialogue_unit_get(record->unit_index);
        s_unit_dialogue *dialogue = unit_dialogue_state_get(unit);
        if (dialogue->current.dialogue_index == index)
            dialogue->current.dialogue_index = -1;
    }
    ::datum_delete((s_data_array *)g_ai_dialogue_records, index);
}
}
void multi_deletion_remove_marked(team_dialogue_state *team)
{
    long *link = &team->first;
    long next;
    while (*link != -1) {
        s_ai_dialogue_queue_entry *record = vocalization_record_get(*link);
        if (record->deleted) {
            next = record->next;
            vocalization_record_delete(*link);
            *link = next;
            team->count--;
        } else {
            link = &record->next;
        }
    }
}
static __forceinline ai_dialogue_definitions *dialogue_queue_globals_get()
{
    ai_dialogue_definitions *result = 0;
    s_game_globals *game_globals = ((s_game_globals *)global_game_globals);
    if (game_globals && game_globals->ai_globals.count() > 0) {
        s_ai_globals_definition *ai_globals = (s_ai_globals_definition *)tag_block_get_element_with_size(
            &game_globals->ai_globals, 0, sizeof(s_ai_globals_definition));
        if (ai_globals->dialogue_globals != -1)
            result = (ai_dialogue_definitions *)tag_get('adlg', ai_globals->dialogue_globals);
    }
    return result;
}
static __forceinline long dialogue_queue_time_get();
static __forceinline long dialogue_queue_seconds_to_ticks_round(float seconds);
// 0x5233b0
float ai_dialogue_queue_unit_vocalization_weight(long unit_index, short vocalization_index)
{
    float weight = 1.0f;
    ai_dialogue_definitions *definitions = dialogue_queue_globals_get();
    if (definitions) {
        unit_datum *unit = dialogue_unit_get(unit_index);
        s_unit_dialogue *dialogue = unit_dialogue_state_get(unit);
        team_dialogue_state *team = team_dialogue_state_get((short)unit->unit.team);
        long record_index = team->first;
        long now = dialogue_queue_time_get();
        long end_time = (long)((float)now + 0.5f);
        short extra_ticks = (short)(long)((float)tls_game_time_globals()->ticks_per_second + 0.5f);
        if (dialogue->current.type > 0 && now + dialogue->duration > end_time) {
            weight = 0.5f;
        } else {
            while (record_index != -1) {
                s_ai_dialogue_queue_entry *record = vocalization_record_get(record_index);
                if (record->start_time > end_time + extra_ticks) {
                    break;
                } else if (record->unit_index == unit_index) {
                    weight = 0.5f;
                    break;
                } else {
                    record_index = record->next;
                }
            }
        }
    }
    return weight;
}

// @address 0x5238d8 queued_vocalization_push_back
extern "C" void queued_vocalization_push_back(s_ai_dialogue_queue_entry *record, s_ai_dialogue_queue_entry *pushed_record)
{
    ai_dialogue_definitions *dialogue_definitions = dialogue_queue_globals_get();
    if (pushed_record->start_time >= record->start_time && pushed_record->start_time < record->start_time + record->duration)
    {
        bool push_back = false;
        bool close_start = abs(pushed_record->start_time - record->start_time) < 3;
        s_dialogue_vocalization *vocalization = (s_dialogue_vocalization *)tag_block_get_element_with_size(
            &dialogue_definitions->vocalizations, pushed_record->vocalization, sizeof(s_dialogue_vocalization));

        if (record->spoken && record->unit_index == pushed_record->unit_index && pushed_record->speech_type < 11)
        {
            push_back = true;
        }
        else if (global_dialogue_priorities[vocalization->priority].priority >= record->speech_type)
        {
            push_back = false;
        }
        else if (!record->spoken && pushed_record->speech_type > record->speech_type && close_start)
        {
            push_back = false;
        }
        else if (!record->spoken && pushed_record->speech_type == record->speech_type && close_start &&
            pushed_record->response_index != -1 && record->response_index == -1)
        {
            push_back = false;
        }
        else
        {
            push_back = true;
        }

        if (push_back)
            pushed_record->start_time = record->start_time + record->duration;
    }
}
// @address 0x523ce8 ?queue_remove_record@@YA_NPAUteam_dialogue_state@@J@Z
bool queue_remove_record(struct team_dialogue_state * list, long index)
{
    bool removed = false;
    long *link = &list->first;
    while (*link != -1) {
        s_ai_dialogue_queue_entry *node = vocalization_record_get(*link);
        if (*link == index) {
            *link = node->next;
            removed = true;
            break;
        }
        link = &node->next;
    }
    if (removed)
        list->count--;
    return removed;
}
// @address 0x523d68 ?record_delete@@YAXJ@Z
void record_delete(long index)
{
    vocalization_record_delete(index);
}
static __forceinline unit_datum *dialogue_queue_unit_get(long index)
{
    return (unit_datum *)object_get_and_verify_type(index, NONE);
}
static __forceinline actor_datum *dialogue_queue_actor_get(long index)
{
    return datum_get<actor_datum>(tls_actor_data(), index);
}
static __forceinline clump_datum *dialogue_queue_clump_get(long index)
{
    return datum_get<clump_datum>(tls_clump_data(), index);
}
static __forceinline bool dialogue_queue_enabled()
{
    return tls_actor_globals()->dialogue && tls_actor_globals()->dialogue_disabled <= 0;
}
static __forceinline long dialogue_queue_time_get()
{
    return tls_game_time_globals()->time;
}
static __forceinline long dialogue_queue_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + AI_REAL_CONSTANTS[0]);
}
static __forceinline bool dialogue_queue_retry(long index)
{
    bool result = false;
    if (vocalization_record_get(index)->information.payload.type != 0)
        result = true;
    return result;
}
// @address 0x523dd8 dialogue_still_desirable
// match: 0x523dd8: pointer __restrict qualifier prevents alias reload
extern "C" bool dialogue_still_desirable(ai_dialogue_definitions *defs, long index, bool *__restrict retry)
{
    bool result = false;
    long now = dialogue_queue_time_get();
    s_ai_dialogue_queue_entry *record = vocalization_record_get(index);
    s_dialogue_vocalization *def = &defs->vocalizations[record->vocalization];
    if (retry)
        *retry = false;
    if (!dialogue_queue_enabled())
        result = false;
    else if (now - record->queued_time <= dialogue_queue_seconds_to_ticks_round(def->allowable_queue_delay))
    {
        result = true;
        unit_datum *u = dialogue_queue_unit_get(record->unit_index);
        if ((unsigned char)((u->object.damage_flags >> 2) & 1))
            result = false;
        else if (u->unit.actor != -1)
        {
            actor_datum *a = dialogue_queue_actor_get(u->unit.actor);
            if (a->combat_status > def->maximum_combat_status)
                result = false;
            else if (record->clump_index &&
                (a->clump_index == -1 || dialogue_queue_clump_get(a->clump_index)->dialogue_state != record->clump_index))
                result = false;
            else if (record->information.type >= 0 && record->information.type < 112 && record->information.listener_object_index != -1)
            {
                switch (record->information.type)
                {
                case _dialogue_event_curse:
                case _dialogue_event_threaten:
                case _dialogue_event_strike:
                case _dialogue_event_gloat:
                case _dialogue_event_betray:
                case _dialogue_event_forgive:
                case _dialogue_event_reanimate:
                    unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(record->information.listener_object_index, -1);
                    if (!object || (unsigned char)((object->object.damage_flags >> 2) & 1))
                        result = false;
                    break;
                }
            }
        }
    }
    if (!result && retry)
        *retry = dialogue_queue_retry(index);
    return result;
}
static __forceinline s_ai_dialogue_queue_entry *ai_dialogue_queue_record_try_and_get(long index)
{
    s_data_array *data = (s_data_array *)g_ai_dialogue_records;
    s_ai_dialogue_queue_entry *record = 0;
    unsigned short absolute_index = (unsigned short)index;
    if (absolute_index < data->first_unallocated) {
        s_ai_dialogue_queue_entry *candidate = (s_ai_dialogue_queue_entry *)datum_get(data, absolute_index);
        if (candidate->identifier != 0 && candidate->identifier == (index >> 16))
            record = candidate;
    }
    return record;
}
// @address 0x524288 multi_deletion_mark
extern "C" void multi_deletion_mark(team_dialogue_state *, long index)
{
    s_ai_dialogue_queue_entry *head = vocalization_record_get(index);
    head->deleted = true;
    long next = head->next;
    while (next != -1) {
        s_ai_dialogue_queue_entry *record = vocalization_record_get(next);
        if (record->response_index != -1) {
            s_ai_dialogue_queue_entry *linked = ai_dialogue_queue_record_try_and_get(record->response_index);
            if (linked && linked->deleted)
                record->deleted = true;
        }
        next = record->next;
    }
}

// @address 0x524070 ?ai_dialogue_queue_update@@YAXXZ
void ai_dialogue_queue_update(void)
{
    ai_dialogue_definitions *defs = dialogue_queue_globals_get();
    long now = dialogue_queue_time_get();
    for (short team_index = 0; team_index < 2; team_index++) {
        team_dialogue_state *team = &((team_dialogue_state *)g_team_dialogue_state)[team_index];
        long index = team->first;
        while (index != -1) {
            s_ai_dialogue_queue_entry *record = (s_ai_dialogue_queue_entry *)vocalization_record_get(index);
            if (!record->deleted) {
                if (record->start_time >= now)
                    break;
                if (!record->spoken) {
                    s_dialogue_vocalization *definition =
                        (s_dialogue_vocalization *)defs->vocalizations.address + record->vocalization;
                    bool retry = false;
                    if (dialogue_still_desirable((ai_dialogue_definitions *)defs, index, &retry)) {
                        if (::unit_test_speech(record->unit_index, record->speech_type, -1, (long *)(0), (long *)(0))) {
                            unit_speech_item item;
                            memset(&item, 0, sizeof(item));
                            item.vocalization = record->vocalization;
                            item.type = (unsigned char)definition->priority;
                            item.sound_index = record->sound_index;
                            item.pad10 = record->minimum_delay;
                            item.delay = record->speech_delay;
                            item.dialogue_index = index;
                            memcpy(&item.information, &record->information, sizeof(item.information));
                            memcpy(&item.permutation, &record->permutation, sizeof(item.permutation));
                            ::unit_speak(record->unit_index, (unit_speech_item *)&item);
                            record->spoken = true;
                        }
                    } else {
                        if (retry && tls_actor_globals()->enabled)
                            ai_dialogue_notify_vocalization(record->unit_index, -1, -1, (char *)&record->information);
                        multi_deletion_mark(team, index);
                    }
                }
            }
            index = record->next;
        }
        multi_deletion_remove_marked(team);
    }
}

// 0x524338
// @address 0x524338 ?ai_dialogue_queue_vocalization_interrupted@@YAXJJ@Z
void ai_dialogue_queue_vocalization_interrupted(long unit_index, long vocalization_index)
{
    unit_datum *unit = dialogue_unit_get(unit_index);
    if (vocalization_index == -1)
        return;
    team_dialogue_state *team = team_dialogue_state_get((short)unit->unit.team);
    s_ai_dialogue_queue_entry *record = vocalization_record_get(vocalization_index);
    unsigned char retry = record->interrupted;
    if (retry == 0)
    {
        if (record->information.payload.type != 0)
            retry = 1;
        if (retry != 0 && tls_actor_globals()->enabled)
            ai_dialogue_notify_vocalization(record->unit_index, -1, -1, (char *)&record->information);
        multi_deletion_mark(team, vocalization_index);
        multi_deletion_remove_marked(team);
        return;
    }
    record->deleted = true;
    multi_deletion_remove_marked(team);
}

// 0x524418
void ai_dialogue_queue_handle_object_deleted(long object_index)
{
    if (((1 << object_header_get(object_index)->object_type) & 0x1823) != 0) {
        s_ai_dialogue_queue_entry *record;
        bool remove;
        long next;
        for (short team_index = 0; team_index < 2; team_index++) {
            team_dialogue_state *team = &((team_dialogue_state *)g_team_dialogue_state)[team_index];
            long record_index = team->first;
            while (record_index != -1) {
                record = vocalization_record_get(record_index);
                next = record->next;
                remove = false;
                if (record->unit_index == object_index) {
                    remove = true;
                } else if (record->information.listener_object_index == object_index) {
                    if (!record->spoken)
                        remove = true;
                    else
                        record->information.listener_object_index = -1;
                }
                if (remove) {
                    ::queue_remove_record(team, record_index);
                    vocalization_record_delete(record_index);
                } else if (record->information.payload.type == 3 && record->information.payload.object_index == object_index) {
                    record->information.payload.object_index = -1;
                }
                record_index = next;
            }
        }
    }
}

static __forceinline bool dq_add_record_fits(s_ai_dialogue_queue_entry *record, long previous)
{
    if (record->start_time - record->queued_time <= record->window) {
        if (record->response && record->response_index != -1 && record->response_index != previous)
            return false;
        return true;
    }
    return false;
}
// @address 0x523a68 ?queue_add_record@@YA_NPAUteam_dialogue_state@@J@Z
bool queue_add_record(team_dialogue_state *team, long index)
{
    bool result = false;
    s_ai_dialogue_queue_entry *record = vocalization_record_get(index);
    bool match = true;
    if (team->obsessed_object != -1)
    {
        match = false;
        if (record->unit_index == team->obsessed_object || record->information.listener_object_index == team->obsessed_object)
            match = true;
    }
    if (match) {
        long previous = -1;
        long following = -1;
        long current = team->first;
        while (current != -1) {
            s_ai_dialogue_queue_entry *other = vocalization_record_get(current);
            queued_vocalization_push_back((s_ai_dialogue_queue_entry *)other, (s_ai_dialogue_queue_entry *)record);
            if (record->start_time <= other->start_time) {
                following = current;
                break;
            }
            previous = current;
            current = other->next;
        }
        if (dq_add_record_fits(record, previous)) {
            s_ai_dialogue_queue_entry *before = 0;
            if (previous != -1)
                before = vocalization_record_get(previous);
            if (before)
                before->next = index;
            else
                team->first = index;
            record->next = following;
            team->count++;
            if (following != -1) {
                s_ai_dialogue_queue_entry *tail = vocalization_record_get(index);
                do {
                    s_ai_dialogue_queue_entry * other = vocalization_record_get(following);
                    long start = tail->start_time + (long)((float)tls_game_time_globals()->ticks_per_second * 0.2f + 0.5f);
                    long *older = &other->start_time;
                    long *chosen = (*older > start) ? older : &start;
                    *older = *chosen;
                    queued_vocalization_push_back((s_ai_dialogue_queue_entry *)tail, (s_ai_dialogue_queue_entry *)other);
                    if (!other->deleted) {
                        if (other->start_time - other->queued_time <= other->window &&
                            !(other->response && other->response_index != -1 && other->response_index != previous)) {
                            tail = other;
                            previous = following;
                        } else {
                            multi_deletion_mark(team, following);
                        }
                    }
                    following = other->next;
                } while (following != -1);
            }
            result = true;
        }
    }
    multi_deletion_remove_marked(team);
    return result;
}

static __forceinline long ai_dialogue_queue_ticks(float seconds)
{
    return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
}

static __forceinline bool ai_dialogue_queue_add_record(short team, long index)
{
    team_dialogue_state *state = team_dialogue_state_get(team);
    return queue_add_record(state, index);
}
static __forceinline short ai_dialogue_queue_team_get(short team)
{
    short result;
    if (team == 1 || game_team_is_ally((e_game_team)team, _ai_dialogue_team_player))
        result = 0;
    else
        result = 1;
    return result;
}
static __forceinline long ai_dialogue_queue_variant_get(long unit_index)
{
    unit_datum *unit = dialogue_unit_get(unit_index);
    long dialogue_index = unit_dialogue_state_get(unit)->dialogue_index;
    long result = 0;
    if (dialogue_index != -1)
        result = ((dialogue_definition *)tag_get('udlg', dialogue_index))->mission_dialogue_designator;
    return result;
}
static __forceinline short ai_dialogue_queue_duration(short speech, short delay)
{
    float speech_ticks = (float)speech;
    float delay_ticks = (float)delay;
    return (short)(speech_ticks * 0.75f + delay_ticks);
}
// @address 0x523550 ?ai_queue_dialogue@@YAJPAVc_dialogue_event@@JFJJJPAUai_information_packet@@@Z
long ai_queue_dialogue(c_dialogue_event *event, long unit_index, short vocalization, long sound_index, long delay, long response_index,
    ai_information_packet *information)
{
    s_data_array *queue = (s_data_array *)g_ai_dialogue_records;
    long index = datum_new(queue);
    if (index != -1) {
        s_ai_dialogue_queue_entry *record = vocalization_record_get(index);
        s_dialogue_vocalization *definition =
            (s_dialogue_vocalization *)(((ai_dialogue_definitions *)::ai_dialogue_globals_get()))->vocalizations.address +
            vocalization;
        bool exhausted = false;
        unit_datum *unit = dialogue_unit_get(unit_index);
        record->unit_index = unit_index;
        record->vocalization = vocalization;
        record->sound_index = sound_index;
        record->response = false;
        record->next = -1;
        short team = ai_dialogue_queue_team_get((short)unit->unit.team);
        record->team = team;
        record->clump_index = event->clump_index;
        if (response_index != -1)
            record->response = vocalization_record_get(response_index)->team == team;
        record->spoken = false;
        record->interrupted = false;
        record->response_index = response_index;
        record->deleted = false;
        record->speech_type = definition->priority;
        game_sound_deterministic_pick_permutation(sound_index, &record->permutation, &exhausted);
        record->queued_time = ai_dialogue_queue_ticks(definition->pre_vocalization_delay) + delay;
        record->start_time = record->queued_time;
        record->minimum_delay = (short)ai_dialogue_queue_ticks(definition->notification_delay);
        record->speech_delay = (short)ai_dialogue_queue_ticks(definition->post_vocalization_delay);
        record->window = (short)ai_dialogue_queue_ticks(definition->allowable_queue_delay);
        short duration = unit_dialogue_get_speech_duration(record->speech_type, record->sound_index, &record->permutation);
        record->duration = ai_dialogue_queue_duration(duration, record->speech_delay);
        record->information = *information;
        if (response_index != -1 && !record->response)
            record->window = (short)ai_dialogue_queue_ticks(0.25f);
        record->dialogue_variant = ai_dialogue_queue_variant_get(unit_index);
        if (!ai_dialogue_queue_add_record((short)unit->unit.team, index)) {
            ::datum_delete((s_data_array *)g_ai_dialogue_records, index);
            index = -1;
        }
    }
    return index;
}
