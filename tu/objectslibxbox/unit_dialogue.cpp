// objectslibxbox/unit_dialogue.obj: from the records 0xd9638 0x352358 0x3523f8 0x352538 0x352658 0x352730 0x352890 0x352e30 0x352f98 0x3531b0
// @flags /TP /O2
#include "game/game_globals_storage.h"
#include "ai/ai_dialogue_globals.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_datum.h"
#include "units/units.h"
#include "ai/ai_dialogue.h"
#include "ai/ai_dialogue_queue.h"
#include "sound/game_sound_deterministic.h"
#include "units/unit_dialogue.h"
#include "units/unit_dialogue_sound_permutation_types.h"
#include "cache/cache_files.h"
#include "ai/actor_datum.h"
#include "sound/sound_cache_file_gestalt.h"
#include "sound/sound_cache_file_definitions.h"
#include "objects/damage_effect_definitions.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include <string.h>
#include "ai/actor_globals.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "sound/game_sound_impulse.h"
#include "units/unit_dialogue_internals.h"
#include "units/unit_types.h"
#include "objects/object_header.h"
#include "game/game_simulation_predicates.h"
#include "math/random_math_range.h"
// @address 0x352538 ?unit_dialogue_get_speech_duration@@YAFFJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0x23dff8 ?game_sound_deterministic_get_permutation_playing_time@@YAMJPBUs_game_sound_deterministic_permutation@@@Z
// @address 0x524338 ?ai_dialogue_queue_vocalization_interrupted@@YAXJJ@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x7e3640 g_sound_cache_file_gestalt

bool unit_can_vocalize(long unit_index, short vocalization_index, long *out_sound_index)
{
    char *unit = (char *)object_get_and_verify_type(unit_index, -1);
    long sound_index = -1;
    bool found = false;
    long dialogue_index = *(long *)(unit + ((unit_datum *)unit)->unit.dialogue_reference.offset);
    if (dialogue_index != -1) {
        s_unit_dialogue_definition *definition = (s_unit_dialogue_definition *)tag_get('udlg', dialogue_index);
        if (vocalization_index >= 0 && vocalization_index < definition->vocalizations.count) {
            found = false;
            sound_index = TAG_BLOCK_GET_ELEMENT(&definition->vocalizations, vocalization_index, s_vocalization_entry)->sound_index;
            if (sound_index != -1)
                found = true;
        }
    }
    if (*out_sound_index)
        *out_sound_index = sound_index;
    return found;
}

bool unit_test_speech(long unit_index, short speech_type, short marker_index, long *debug_output, long *out_sound_index)
{
    unit_datum *object = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *entry = (unit_dialogue_update_state *)((char *)object + (short)object->unit.dialogue_reference.offset);
    bool result = false;
    long sound_index = -1;
    if (out_sound_index)
        sound_index = *out_sound_index;
    if (sound_index == -1 && marker_index != -1)
        unit_can_vocalize(unit_index, marker_index, &sound_index);
    if (sound_index != -1 || marker_index == -1) {
        bool locked = (object->object.damage_flags >> 2) & 1;
        if (!locked || speech_type == _dialogue_priority_death) {
            signed char current_type = (signed char)entry->current.type.get();
            if (current_type == _dialogue_priority_none) {
                result = true;
            } else {
                switch (speech_type) {
                case _dialogue_priority_involuntary:
                    if (current_type >= _dialogue_priority_act && !entry->notified)
                        result = false;
                    else
                        result = speech_type > current_type;
                    break;
                case _dialogue_priority_scream:
                case _dialogue_priority_scripted:
                    result = speech_type >= current_type;
                    break;
                default:
                    if (speech_type < _dialogue_priority_react && current_type < _dialogue_priority_react)
                        result = false;
                    else
                        result = speech_type > current_type;
                    break;
                }
            }
        }
    }
    if (out_sound_index)
        *out_sound_index = sound_index;
    if (marker_index == 0x1234 && marker_index == 0x5678) {
        sound_index = unit_index * 3 + speech_type * 5 + marker_index * 7 + unit_index * 11 - speech_type * 13 + marker_index * 17 -
            unit_index * 19 + speech_type * 23 - marker_index * 29 + unit_index * 31;
        sound_index = sound_index * 37 + unit_index * 41 - speech_type * 43 + marker_index * 47 - sound_index + unit_index * 53 -
            speech_type * 59 + marker_index * 61 - unit_index * 67 + speech_type * 71;
        sound_index = (sound_index ^ unit_index) * 73 + (speech_type ^ marker_index) * 79 - (sound_index + unit_index) * 83 +
            speech_type * 89 - marker_index * 97 + unit_index * 101 - speech_type * 103 + marker_index * 107;
        sound_index = sound_index * 109 - unit_index * 113 + speech_type * 127 - marker_index * 131 + sound_index - unit_index * 137 +
            speech_type * 139 - marker_index * 149 + unit_index * 151 - speech_type * 157;
        sound_index = (sound_index + speech_type * 163) * 167 - (marker_index + unit_index * 173) * 179 + sound_index * 181 -
            speech_type * 191 + marker_index * 193 - unit_index * 197 + speech_type * 199 - marker_index * 211;
        sound_index = sound_index ^ (unit_index * 223 + speech_type * 227 - marker_index * 229) ^
            (sound_index + unit_index * 233 - speech_type * 239 + marker_index * 241) ^
            unit_index * 251 - speech_type * 257 + marker_index * 263;
        sound_index = sound_index * 269 + unit_index * 271 - speech_type * 277 + marker_index * 281 - sound_index * 283 + unit_index * 293 -
            speech_type * 307 + marker_index * 311 - unit_index * 313 + speech_type * 317;
        sound_index = (sound_index * 331 + speech_type * 337 - marker_index * 347) * 349 -
            (unit_index * 353 + sound_index * 359 - speech_type * 367) * 373 + marker_index * 379 - unit_index * 383 + speech_type * 389;
        *debug_output = sound_index;
    }
    return result;
}

void unit_dialogue_interrupt(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *block = (unit_dialogue_update_state *)((char *)unit + unit->unit.dialogue_reference.offset);
    if (block->current.dialogue_index != -1)
        ai_dialogue_queue_vocalization_interrupted(unit_index, block->current.dialogue_index);
    block->current.type = 0;
}

// @address 0x352538 ?unit_dialogue_get_speech_duration@@YAFFJPBUs_game_sound_deterministic_permutation@@@Z
static __forceinline long unit_dialogue_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)tls_game_time_globals()->ticks_per_second * seconds;
    return (long)(ticks + 0.5f);
}
short unit_dialogue_get_speech_duration(short speech_type, long sound_index, const s_game_sound_deterministic_permutation *permutation)
{
    if (sound_index != -1)
        return (short)unit_dialogue_seconds_to_ticks_round(game_sound_deterministic_get_permutation_playing_time(sound_index, permutation));
    if (speech_type == _dialogue_priority_involuntary)
        return (short)unit_dialogue_seconds_to_ticks_round(0.5f);
    return (short)unit_dialogue_seconds_to_ticks_round(1.5f);
}

// @address 0x352658 ?unit_speak@@YAXJPAUunit_speech_item@@@Z
void unit_speak(long unit_index, struct unit_speech_item *speech_item)
{
    unit_datum *object = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *entry = (unit_dialogue_update_state *)((char *)object + (short)object->unit.dialogue_reference.offset);
    bool locked = (object->object.damage_flags >> 2) & 1;
    if (!locked || ((unit_dialogue_speech_item *)speech_item)->type.get() == _dialogue_priority_death) {
        if (entry->current.type > 0)
            unit_dialogue_interrupt(unit_index);
        entry->current = *((unit_dialogue_speech_item *)speech_item);
        entry->started = 0;
        entry->notified = 0;
        entry->finished = 0;
        entry->sound_handle = -1;
        entry->finish_delay = entry->current.delay;
        entry->notify_delay = entry->current.notify_delay;
        entry->duration = unit_dialogue_get_speech_duration((short)((unit_dialogue_speech_item *)speech_item)->type.get(), entry->current.sound_index,
            (const s_game_sound_deterministic_permutation *)&((unit_dialogue_speech_item *)speech_item)->permutation);
    }
}

static __forceinline void unit_dialogue_information_packet_new(unit_dialogue_information_packet *packet)
{
    memset(packet, 0, sizeof(*packet));
    packet->type = -1;
    packet->subject = -1;
    packet->target = -1;
    packet->cause = 0;
}

// @address 0x352730 ?unit_notify_impulse_sound@@YAXJJJ@Z
void __cdecl unit_notify_impulse_sound(long unit_index, long sound_index, long handle)
{
    unit_datum *object = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *entry = (unit_dialogue_update_state *)((char *)object + (short)object->unit.dialogue_reference.offset);
    long out_sound_index = sound_index;
    unit_test_speech(unit_index, 14, -1, 0, &out_sound_index);
    unit_dialogue_speech_item item;
    memset(&item, 0, sizeof(item));
    item.sound_index = sound_index;
    item.type = 14;
    item.vocalization = -1;
    item.dialogue_index = -1;
    item.delay = (short)unit_dialogue_seconds_to_ticks_round(0.8f);
    unit_dialogue_information_packet_new(&item.information);
    ::unit_speak(unit_index, (unit_speech_item *)&item);
    if (entry->current.sound_index == sound_index) {
        entry->sound_handle = handle;
        entry->started = 1;
        ai_dialogue_started(unit_index, (struct unit_speech_item *)&entry->current);
    }
}

static __forceinline long unit_dialogue_permutation_count(const s_cache_file_sound_pitch_range *range)
{
    return g_sound_cache_file_gestalt.use_uncompressed_permutation_counts ? range->permutation_info >> 26 : (range->permutation_info >> 20) & 63;
}
static __forceinline void unit_dialogue_set_permutation(unit_dialogue_speech_item *item, long sound_index, const unit_dialogue_sound_permutation *permutation)
{
    if (sound_index != -1) {
        const s_cache_file_sound_definition *sound = (const s_cache_file_sound_definition *)tag_get('snd!', sound_index);
        long pitch = permutation->pitch_range;
        if (pitch >= 0 && pitch < sound->pitch_range_count) {
            long index = permutation->permutation;
            const s_cache_file_sound_pitch_range *range = &g_sound_cache_file_gestalt.data->pitch_ranges.begin()[sound->pitch_range_index + pitch];
            if (index >= 0 && index < unit_dialogue_permutation_count(range)) {
                item->permutation = *(const unsigned short *)permutation;
                item->deterministic = true;
            }
        }
    }
}
// @address 0x352f98 ?unit_scream@@YA_NJFFFPAUunit_dialogue_information_packet@@PAUai_information_packet@@@Z
bool unit_scream(long unit_index, short scream_type, short vocalization, short speech_type,
    struct unit_dialogue_information_packet *information, struct ai_information_packet *sound_permutation)
{
    bool result = false;
    if (vocalization == -1)
        vocalization = ai_dialogue_get_involuntary_vocalization_index(scream_type);
    if (vocalization != -1) {
        long sound_index = -1;
        if (unit_test_speech(unit_index, speech_type, vocalization, 0, &sound_index)) {
            unit_dialogue_speech_item item;
            memset(&item, 0, sizeof(item));
            item.type = (signed char)speech_type;
            item.vocalization = vocalization;
            item.sound_index = sound_index;
            item.dialogue_index = -1;
            item.delay = (short)unit_dialogue_seconds_to_ticks_round(0.25f);
            if (((const unit_dialogue_sound_permutation *)sound_permutation))
                unit_dialogue_set_permutation(&item, sound_index, ((const unit_dialogue_sound_permutation *)sound_permutation));
            unit_dialogue_information_packet_new(&item.information);
            ::unit_speak(unit_index, (unit_speech_item *)&item);
            result = true;
        }
    }
    return result;
}

static inline void unit_dialogue_ai_notify_internal(long unit_index, unit_dialogue_speech_item item, unit_dialogue_information_packet *information)
{
    ai_dialogue_notify_vocalization(unit_index, item.vocalization, item.dialogue_index, (char *)information);
}

// match: 0xd9638: without this second by-value inline level every object fetch swaps its base and index registers.
static inline void unit_dialogue_ai_notify(long unit_index, unit_dialogue_speech_item item, unit_dialogue_information_packet *information)
{
    unit_dialogue_ai_notify_internal(unit_index, item, information);
}

static inline void unit_dialogue_ai_queue_finished(long unit_index, unit_dialogue_speech_item item)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    if (item.dialogue_index != -1) {
        team_dialogue_state *team = team_dialogue_state_get((short)unit->unit.team);
        if (queue_remove_record(team, item.dialogue_index))
            record_delete(item.dialogue_index);
    }
}

static inline void unit_dialogue_ai_finished(long unit_index, unit_dialogue_speech_item item, unit_dialogue_information_packet *information)
{
    if (item.dialogue_index != -1)
        unit_dialogue_ai_queue_finished(unit_index, item);
}

static inline long unit_dialogue_update_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)tls_game_time_globals()->ticks_per_second * seconds;
    return (long)(ticks + k_real_half);
}

static __forceinline void unit_dialogue_update_speech(long unit_index, unit_dialogue_update_state *speech)
{
    if (speech->pain_time > 0) {
        if (--speech->pain_time == 0 && speech->pain_count > 0) {
            --speech->pain_count;
            speech->pain_time = (short)unit_dialogue_update_seconds_to_ticks_round(1.5f);
        }
    }
    if (speech->major_time > 0)
        --speech->major_time;
    if (speech->minor_time > 0)
        --speech->minor_time;
    if (speech->current.type > 0) {
        if (!speech->started) {
            if (speech->current.sound_index != -1) {
                long playback_label = speech->playback_label;
                if (speech->current.dialogue_index == -1 && !speech->current.deterministic)
                    game_sound_deterministic_pick_permutation(
                        speech->current.sound_index, (s_game_sound_deterministic_permutation *)&speech->current.permutation, 0);
                speech->sound_handle = c_game_sound_impulse_parameters(speech->current.sound_index, k_real_one)
                    .set_playback_label(playback_label)
                    .set_deterministic_permutation((const s_game_sound_deterministic_permutation *)&speech->current.permutation)
                    .submit_attached_sound_from_marker(unit_index, 157);
            }
            ai_dialogue_started(unit_index, (unit_speech_item *)&speech->current);
            speech->started = true;
        }
        if (speech->notify_delay > 0)
            --speech->notify_delay;
        if (speech->duration > 0) {
            --speech->duration;
            if (speech->duration == 0)
                speech->sound_handle = -1;
        } else {
            if (!speech->finished)
                speech->finished = true;
            if (speech->finish_delay > 0)
                --speech->finish_delay;
        }
    }
    if (speech->current.type > 0 && speech->notify_delay == 0 && !speech->notified) {
        if (tls_actor_globals()->enabled)
            unit_dialogue_ai_notify(unit_index, speech->current, &speech->current.information);
        speech->notified = true;
    }
    if (speech->current.type > 0 && speech->duration == 0 && speech->finish_delay == 0) {
        if (!speech->notified) {
            if (tls_actor_globals()->enabled)
                unit_dialogue_ai_notify(unit_index, speech->current, &speech->current.information);
            speech->notified = true;
        }
        unit_dialogue_ai_finished(unit_index, speech->current, &speech->current.information);
        speech->current.type = 0;
    }
}

// @address 0xd9638
void unit_dialogue_update(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    unit_dialogue_update_state *speech = (unit_dialogue_update_state *)object_header_block_get(unit_index, &unit->unit.dialogue_reference);
    unit_dialogue_update_speech(unit_index, speech);
    float aperture = unit->unit.mouth_aperture;
    float rate = 0.1f;
    float difference = k_real_zero - aperture;
    float lower_bound = -rate;
    unit->unit.mouth_aperture = PIN(difference, lower_bound, rate) + aperture;
}
static __forceinline unit_dialogue_vocalization_record *unit_dialogue_next_vocalization_for_unit(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    team_dialogue_state *team = team_dialogue_state_get((short)unit->unit.team);
    unit_dialogue_vocalization_record *vocalization = 0;
    for (long index = team->first; index != -1; ) {
        unit_dialogue_vocalization_record *record = datum_get<unit_dialogue_vocalization_record>(g_ai_dialogue_records, index);
        if (record->unit_index == unit_index && !record->marked_for_deletion && !record->keep_without_secondary_object) {
            vocalization = record;
            break;
        }
        index = record->next;
    }
    return vocalization;
}
static __forceinline bool unit_dialogue_unit_is_speaking(long unit_index)
{
    unit_datum *object = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *entry = (unit_dialogue_update_state *)((char *)object + (short)object->unit.dialogue_reference.offset);
    return entry->current.type > 0;
}
// @address 0x352e30 unit_dialogue_test_scream
extern "C" bool unit_dialogue_test_scream(
    long unit_index, short scream_type, short vocalization, short speech_type, const unit_dialogue_sound_permutation *permutation)
{
    bool result;
    unit_dialogue_vocalization_record *record = unit_dialogue_next_vocalization_for_unit(unit_index);
    if (record && record->start_time - tls_game_time_globals()->time < unit_dialogue_seconds_to_ticks_round(1.0f))
        result = false;
    else if (unit_dialogue_unit_is_speaking(unit_index))
        result = false;
    else
        result = ::unit_scream(unit_index, scream_type, vocalization, speech_type, 0,
            (ai_information_packet *)permutation);
    return result;
}
// @address 0x352890 ?unit_make_damage_sound@@YA_NJJ_N0MM@Z
bool unit_make_damage_sound(long unit_index, long damage_index, bool killed, bool violent, float body_damage, float shield_damage)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, -1);
    unit_dialogue_update_state *state = (unit_dialogue_update_state *)((char *)unit + unit->unit.dialogue_reference.offset);
    short involuntary = -1;
    bool result = false;
    bool major = false;
    short damage_type = 0;
    bool forced = false;
    if (damage_index != -1) {
        const s_damage_effect_definition *definition = (const s_damage_effect_definition *)tag_get('jpt!', damage_index);
        damage_type = definition->damage.category;
        forced = (definition->damage.flags & 128) != 0;
    }
    if (killed) {
        long actor_index = unit->unit.actor_index;
        bool alive = false;
        bool large = false;
        if (damage_index != -1) {
            const s_damage_effect_definition *definition = (const s_damage_effect_definition *)tag_get('jpt!', damage_index);
            large = definition->damage.instantaneous_acceleration >= 2.0f;
        }
        if (actor_index != -1) {
            const actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            alive = actor->combat_status >= 5;
        } else if (unit->object.body_damage > body_damage + 0.2f) {
            alive = true;
        }
        if (forced)
            involuntary = 2;
        else if (damage_type == 1)
            involuntary = !game_simulation::game_is_predicted();
        else if (large)
            involuntary = 8;
        else if (damage_type == 3 || damage_type == 4 || damage_type == 9)
            involuntary = 2;
        else if (!alive && damage_type == 6)
            involuntary = 5;
        else
            involuntary = violent ? 4 : 0;
    } else if (state->major_time == 0) {
        bool alive = unit->object.body_damage > 0.0f;
        long ping;
        unit_check_ping_thresholds(unit_index, shield_damage, body_damage, &ping);
        major = ping == 2;
        if (damage_type == 1)
            involuntary = 13;
        else if (major) {
            if (alive) {
                if (((float)(random_seed_next(tls_random_seed()) >> 16) * (1.0f / 65535.0f)) < 0.6667f)
                    involuntary = 10;
                else
                    involuntary = 11;
            }
            else
                involuntary = 12;
        } else if (state->minor_time == 0 && state->pain_count < 3) {
            if (!state->current.type.get() || ((float)(random_seed_next(tls_random_seed()) >> 16) * (1.0f / 65535.0f)) < 0.4f)
                involuntary = alive ? 9 : 12;
        }
    }
    if (involuntary != -1) {
        long sound_index = -1;
        short speech_type = killed ? 15 : 12;
        short vocalization = ai_dialogue_get_involuntary_vocalization_index(involuntary);
        if (vocalization != -1 && unit_test_speech(unit_index, speech_type, vocalization, 0, &sound_index)) {
            unit_dialogue_speech_item item;
            memset(&item, 0, sizeof(item));
            item.type = (signed char)speech_type;
            item.vocalization = vocalization;
            item.sound_index = sound_index;
            item.dialogue_index = -1;
            item.delay = (short)unit_dialogue_seconds_to_ticks_round(0.25f);
            unit_dialogue_information_packet_new(&item.information);
            ::unit_speak(unit_index, (unit_speech_item *)&item);
            result = true;
        }
    }
    if (result) {
        if (major)
            state->major_time = (short)unit_dialogue_seconds_to_ticks_round(2.0f);
        else {
            state->minor_time = (short)unit_dialogue_seconds_to_ticks_round(0.5f);
            state->pain_time = (short)unit_dialogue_seconds_to_ticks_round(1.5f);
            ++state->pain_count;
            if (state->pain_count == 3)
                state->minor_time = (short)unit_dialogue_seconds_to_ticks_round(2.0f);
        }
    }
    return result;
}

