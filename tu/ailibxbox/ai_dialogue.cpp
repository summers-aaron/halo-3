// ailibxbox/ai_dialogue.obj: from the records 0xad658 0x50a638 0x51e388 0x51e3b0 0x51e658 0x51e6d0 0x51e728 0x51e7d0 0x51e898 0x51e920 0x51ea28 0x51f3a0 0x51f560 0x51f900 0x520130 0x520248 0x5205d0 0x520748 0x520d98 0x521138 0x5211a0 0x521288 0x521488 0x522430 0x5227a0 0x522940 0x522b20 0x522dc8 0x522f08 0x523068 0x5230c8 0x523218 0x5232b8 0x5232d0 0x5232f0 0x523390
#include "sound/game_sound_deterministic_permutation.h"
#include "math/real_constant_pool.h"
// @address 0x22dc0 k_real_constant_pool_half
#if !defined(_XBOX) || defined(__clang__)
const float k_real_constant_pool_half[] = { 0.5f };
#endif
#include "math/random_math_range.h"
#include "ai/joint_behavior.h"
#include "ai/command_script_data.h"
#include "ai/objective_storage.h"
// @flags /TP /O2 /Oc
#include "ai/squad_data.h"
#include "units/unit_types.h"
#include "units/unit_speech_item.h"
#include "units/dialogue_definitions.h"
#include "objects/object_definitions.h"
#include "objects/object_ai_properties.h"
#include "items/projectile_definitions.h"
#include "multithreading/thread_tls.h"
#include "ai/ai_dialogue_globals.h"
// @address 0x11df6fc g_ai_dialogue_records
// @address 0x11df858 g_team_dialogue_state
#include "ai/character_weapon_properties.h"
#include "ai/actor_nearby_iterator_types.h"
#include "creatures/creature_datum.h"
#include "ai/actor_type_flood_types.h"
#include "ai/ai_globals_definition.h"
#include "ai/ai_information_data.h"
#include "tag_files/tag_groups.h"
#include "ai/clump_importance_types.h"
#include "memory/tls_globals.h"
#include "units/unit_seat.h"
#include "units/unit_seat_source.h"
#include "memory/data_types.h"
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "game/game_types.h"
#include "units/vehicle_types.h"
#include "ai/actor_globals.h"
#include "memory/data.h"
#include "memory/datum_storage_access.h"
#include "game/player_types.h"
#include "math/real_math.h"
#include "ai/prop_ref_datum.h"
#include "ai/ai_actions.h"
#include "ai/clump_datum.h"
#include "ai/prop_datum.h"
#include "cseries/cseries.h"
// @address 0x521288 ai_dialogue_actor_can_vocalize
// @address 0x51e388 ?__tls_set_g_team_dialogue_state_allocator@@YAXPAX@Z
// @address 0x5230c8 ?dialogue_choose_option@@YAFPAVc_dialogue_option@@F@Z
// @address 0xae098 ?ai_get_root_vehicle@@YAJJ@Z
// @address 0x11fc98 ?unit_set_emotional_state@@YAXJJMM@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x5205d0 ?dialogue_speak@@YA_NPAUai_dialogue_definitions@@JPAVc_dialogue_option@@PAJ@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x5052d0 ?actor_nearby_actors_iterator_next@@YAPAUactor_datum@@PAUactor_nearby_actors_iterator@@@Z
// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0x505260 ?actor_nearby_actors_iterator_new@@YAXPAUactor_nearby_actors_iterator@@PBTreal_point3d@@MFF@Z
// @address 0x354898 ?vehicle_get_ultimate_parent@@YAJJ@Z
// @address 0x505b80 ?actor_is_leader@@YA_NJJ@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
#include <stdlib.h>
#include <string.h>
#include "tag_files/tag_types.h"
#include "ai/squad_types.h"
#include "scenario/scenario_tag.h"
#include <math.h>
#include <float.h>
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/actor_moving.h"
#include "ai/actor_perception.h"
#include "ai/ai_dialogue_queue.h"
#include "ai/ai_dialogue_queue_data.h"
#include "ai/ai_vehicles.h"
#include "ai/behavior.h"
#include "ai/event_handling.h"
#include "ai/objectives.h"
#include "ai/props.h"
#include "ai/squads.h"
#include "ai/squad_leader.h"
#include "render/render_objects_static_lighting.h"
#include "units/unit_animation.h"
#include "units/unit_dialogue.h"
#include "units/vehicles.h"
#include "ai/ai_dialogue.h"
#include "ai/actor_types.h"
#include "ai/character_definitions.h"
#include "ai/styles.h"
#include "game/game_globals.h"
#include "ai/clump.h"
#include "game/game_allegiance.h"
#include "ai/actor_datum.h"
#include "ai/task_actor_iterator_types.h"
#include "cache/cache_files.h"
#include "game/game_results.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "ai/objective_definitions.h"
#include "ai/behavior_posture_table.h"
#include "ai/behavior_info.h"
#include "ai/actor_type_grunt_types.h"
#include "units/biped_types.h"
#include "units/unit_definitions.h"
#include "math/real_math_constants.h"
#include "ai/ai_math_constants.h"
// @address 0x2251c k_ai_dialogue_real_constants
#include "ai/ai_information_packet.h"
#include "ai/team_dialogue_state.h"
#include "units/unit_dialogue_internals.h"

#include "ai/dialogue_option.h"

#include "ai/ai_dialogue_definitions.h"


#include "ai/dialogue_event.h"
#include "ai/dialogue_event_types.h"
#include "cache/restricted_memory_region_members.h"
#include "ai/actor_posture.h"
#include "ai/event_handling_sound_types.h"
#include "units/unit_seat_helpers.h"
#include "ai/clump_iterator.h"
#include "ai/actor_behavior_state.h"
#include "ai/swarms.h"
#include "ai/dialogue_pattern_types.h"
#include "game/game_time_access.h"
#include "game/game_globals_inline.h"

__declspec(thread) s_data_array *g_ai_dialogue_records;
__declspec(thread) team_dialogue_state *g_team_dialogue_state;

extern "C" {
// @address 0x226cc k_real_seven_tenths_storage
const float k_real_seven_tenths_storage = 0.7f;
}

struct e_game_results_damage_type_description;
struct s_dialogue_character_voice { float fraction; };
struct s_reply_vocalization_pattern : vocalization_pattern {
    s_reply_vocalization_pattern() {}
};


static __forceinline s_game_globals *dialogue_scenario_get_game_globals()
{
    return (s_game_globals *)global_game_globals;
}
static __forceinline void *dialogue_tag_get(unsigned long group_tag, long index)
{
    const cache_file_tag_instance *instance = &(global_tag_instances)[(unsigned short)index];
    return instance->definition;
}

namespace {
static __forceinline ai_dialogue_definitions *dialogue_globals_get_inline()
{
    ai_dialogue_definitions *result = 0;
    s_game_globals *game_globals = dialogue_scenario_get_game_globals();
    if (game_globals && game_globals->ai_globals.count() > 0) {
        s_ai_globals_definition *ai_globals = (s_ai_globals_definition *)game_globals->ai_globals.address;
        if (ai_globals->dialogue_globals != NONE)
            result = (ai_dialogue_definitions *)dialogue_tag_get('adlg', ai_globals->dialogue_globals);
    }
    return result;
}
}
// @address 0x51e658 ?ai_dialogue_get_involuntary_vocalization_index@@YAFF@Z

namespace {
static __forceinline bool ai_dialogue_aggregate_enabled()
{
    return tls_actor_globals()->dialogue_enabled && tls_actor_globals()->dialogue_suspend_level <= 0;
}
}
// @address 0x51e728 ?ai_dialogue_event_aggregate_options@@YAXPAVc_dialogue_event@@PAVc_dialogue_option@@PAFF@Z


// @address 0x51e7d0 ?ai_dialogue_event@@YAFPAVc_dialogue_option@@F@Z

// @address 0x51e898 ?ai_dialogue_event@@YA_NPAVc_dialogue_event@@@Z

static __forceinline unit_datum *dialogue_object_get_and_verify_type(long index, unsigned long mask)
{
    return (unit_datum *)datum_get<s_object_header>(tls_object_header_data(), index)->datum;
}
static __forceinline long dialogue_object_get_type(long index) { return datum_get<s_object_header>(tls_object_header_data(), index)->object_type; }
static __forceinline long dialogue_unit_inventory_get_weapon(long index, short slot)
{
    long result = -1;
    unit_datum *unit = dialogue_object_get_and_verify_type(index, 0x1003);
    if (slot != -1)
        result = unit->unit.weapon_object_indices[slot];
    return result;
}

// @address 0x520130 ?ai_dialogue_event_internal@@YA_NPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAVc_dialogue_option@@FPAF@Z


// @address 0x50a638 dialogue_compare_player_weighting
extern "C" bool dialogue_compare_player_weighting(long first_actor_index, long second_actor_index, const void *context)
{
    actor_datum *first = datum_get<actor_datum>(tls_actor_data(), first_actor_index);
    actor_datum *second = datum_get<actor_datum>(tls_actor_data(), second_actor_index);
    bool result = false;
    if (first->unit_index != NONE)
    {
        float first_weight = dialogue_player_weighting(first->unit_index);
        float second_weight = dialogue_player_weighting(second->unit_index);
        if (second_weight > first_weight)
            result = true;
    }
    else
    {
        result = true;
    }
    return result;
}

static __forceinline long ai_dialogue_creature_get_actor(long index)
{
    unit_datum *creature = dialogue_object_get_and_verify_type(index, 0x800);
    long result = NONE;
    if (((creature_datum *)creature)->creature.link_type == 0) {
        swarm_creature_state *block = (swarm_creature_state *)((char *)creature + ((creature_datum *)creature)->creature.creature_reference.offset);
        result = block->swarm_index;
    }
    return result;
}
// @address 0x51e3b0 ??0c_dialogue_event@@QAA@FJJJFPAUai_information_data@@@Z

// @address 0x521138 ?dialogue_emotion@@YAXJFF@Z


// @address 0x5227a0 ?dialogue_find_speaker@@YAJPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@_N33PAJPAFPAM6@Z

// @address 0x522940 ?dialogue_find_speaker_vehicle@@YAJPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@_NPAJPAFPAM6@Z

// @address 0x522b20 ?dialogue_find_speaker_joint@@YAJPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@_NPAJPAFPAM6@Z




// @address 0x522dc8 ?dialogue_find_speaker_clump@@YAJPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@PAJPAFPAM5@Z
long dialogue_find_speaker_clump(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    long *sound_definition_index, short *chosen_vocalization_index, float *vocalization_factor, float *player_factor)
{
    const float zero = k_real_zero_storage;
    actor_datum *speaker = event->speaker;
    long best_unit = NONE;
    short best_index = NONE;
    long best_sound = NONE;
    float best_weight = zero;
    float best_vocalization = zero;
    float best_player = zero;
    if (speaker != 0)
    {
        long clump_index = speaker->clump_index;
        if (clump_index != NONE)
        {
            clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), clump_index);
            clump_actor_iterator iterator;
            clump_actor_iterator_new(&iterator, clump);
            actor_datum *actor;
            while ((actor = clump_actor_iterator_next(&iterator)) != 0)
            {
                if (actor->unit_index != NONE)
                {
                    float factor;
                    long sound;
                    short vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, actor->unit_index, &sound, &factor);
                    if (vocalization != NONE && factor > zero)
                    {
                        float current_factor = factor;
                        float current_player = dialogue_player_weighting(actor->unit_index);
                        float weight = current_player * current_factor;
                        if (weight > best_weight)
                        {
                            best_unit = actor->unit_index;
                            best_weight = weight;
                            best_index = vocalization;
                            best_sound = sound;
                            best_vocalization = current_factor;
                            best_player = current_player;
                        }
                    }
                }
            }
        }
    }
    *vocalization_factor = best_vocalization;
    *player_factor = best_player;
    *sound_definition_index = best_sound;
    *chosen_vocalization_index = best_index;
    return best_unit;
}

// @address 0x522f08 ?dialogue_task_find_speaker@@YAJPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@PAJPAFPAM5@Z
long dialogue_task_find_speaker(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    long *out_datum, short *out_index, float *out_score, float *out_weight)
{
    actor_datum *speaker = event->speaker;
    long best_unit = -1;
    short best_index = -1;
    long best_datum = -1;
    float best_total = 0.0f;
    float best_score = 0.0f;
    float best_weight = 0.0f;
    if (speaker != 0)
    {
        long squad_index = speaker->squad_index;
        if (squad_index != -1)
        {
            squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
            if (squad_valid_task(squad))
            {
                task_actor_iterator iterator;
                task_actor_iterator_new(squad->objective_index, squad->task_index, &iterator);
                actor_datum *actor = task_actor_iterator_next(&iterator);
                while (actor != 0)
                {
                    if (actor->unit_index != -1)
                    {
                        float score;
                        long datum;
                        short can_speak = dialogue_unit_can_vocalize(definitions, event, pattern, actor->unit_index, &datum, &score);
                        if (can_speak != -1 && score > 0.0f)
                        {
                            float speaker_score = score;
                            float weight = dialogue_player_weighting(actor->unit_index);
                            float total = weight * speaker_score;
                            if (total > best_total)
                            {
                                best_unit = actor->unit_index;
                                best_total = total;
                                best_index = can_speak;
                                best_datum = datum;
                                best_score = speaker_score;
                                best_weight = weight;
                            }
                        }
                    }
                    actor = task_actor_iterator_next(&iterator);
                }
            }
        }
    }
    *out_score = best_score;
    *out_weight = best_weight;
    *out_datum = best_datum;
    *out_index = best_index;
    return best_unit;
}

static __forceinline short dialogue_team_get(short team)
{
    if (team != _ai_dialogue_team_player && !game_team_is_ally((e_game_team)team, _ai_dialogue_team_player))
        return 1;
    else
        return 0;
}
// @address 0x523068 ?team_dialogue_state_get@@YAPAUteam_dialogue_state@@F@Z

static __forceinline void *dialogue_restricted_region_get_member_address(long region, long member)
{
    char *base = restricted_region_get_base_address(region);
    long offset = g_restricted_regions[region].get_member_offset(member);
    return base + offset;
}
static __forceinline long dialogue_restricted_region_add_member(long region, const char *name,
    const char *type, unsigned int size, long alignment_bits, field_proc_t set_address)
{
    long member = g_restricted_regions[region].add_member(name, type, size, alignment_bits, set_address, 0, 0);
    void *address = dialogue_restricted_region_get_member_address(region, member);
    set_address(address);
    return member;
}
// @address 0x523218 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_team_dialogue_state_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z

// @address 0x5232b8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_team_dialogue_state_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z

// 0xad658
ai_dialogue_definitions *ai_dialogue_globals_get()
{
    ai_dialogue_definitions *result = 0;
    s_game_globals *game_globals = dialogue_scenario_get_game_globals();
    if (game_globals && game_globals->ai_globals.count() > 0) {
        s_ai_globals_definition *ai_globals = (s_ai_globals_definition *)game_globals->ai_globals.address;
        if (ai_globals->dialogue_globals != -1)
            result = (ai_dialogue_definitions *)dialogue_tag_get('adlg', ai_globals->dialogue_globals);
    }
    return result;
}

// 0x51e388
void __tls_set_g_team_dialogue_state_allocator(void *allocator)
{
    g_team_dialogue_state = (team_dialogue_state *)allocator;
}

// 0x51e3b0
c_dialogue_event::c_dialogue_event(short event_type, long event_unit_index, long event_target_index, long event_subtype,
    short event_vocalization, ai_information_data *event_information)
{
    type = event_type;
    unit_index = event_unit_index;
    target_index = event_target_index;
    subtype = event_subtype;
    vocalization = event_vocalization;
    clump_index = 0;
    if (event_information)
        information = *event_information;
    else
        information.values[0] = 0;
    unit = 0;
    unit_team = NONE;
    speaker_team = NONE;
    speaker = 0;
    if (event_unit_index != NONE) {
        unit = dialogue_object_get_and_verify_type(event_unit_index, 0x1003);
        unit_team = (short)unit->unit.team;
        if (unit->unit.actor_index != NONE) {
            speaker = datum_get<actor_datum>(tls_actor_data(), unit->unit.actor_index);
            speaker_team = speaker->type;
        }
    }
    target_unit = 0;
    other_unit_index = NONE;
    target_team = NONE;
    target_speaker = 0;
    target_speaker_team = NONE;
    target_index = event_target_index;
    if (target_index != NONE) {
        unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(target_index, -1);
        if (object) {
            if ((1 << object->object.type) & 0x1003) {
                other_unit_index = target_index;
                target_unit = object;
                target_team = (short)target_unit->unit.team;
                if (target_unit->unit.actor_index != NONE) {
                    target_speaker = datum_get<actor_datum>(tls_actor_data(), target_unit->unit.actor_index);
                    target_speaker_team = target_speaker->type;
                }
            } else if ((1 << object->object.type) & 0x800) {
                long actor_index = ai_dialogue_creature_get_actor(target_index);
                if (actor_index != NONE) {
                    target_speaker = datum_get<actor_datum>(tls_actor_data(), actor_index);
                    target_speaker_team = target_speaker->type;
                }
            }
        } else {
            target_index = NONE;
        }
    }
    if (target_team == NONE && unit_team == NONE)
        relation = 0;
    else if (target_team == unit_team)
        relation = 3;
    else if (::game_team_is_traitor((e_game_team)unit_team, (e_game_team)target_team))
        relation = 5;
    else if (!game_team_is_enemy((e_game_team)unit_team, (e_game_team)target_team))
        relation = 3;
    else if (game_team_is_enemy((e_game_team)unit_team, (e_game_team)target_team))
        relation = 4;
}

// 0x51e658
short ai_dialogue_get_involuntary_vocalization_index(short involuntary_index)
{
    short result = NONE;
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    if (definitions && involuntary_index >= 0 &&
        involuntary_index < definitions->involuntary_vocalizations.s_tag_block::count) {
        s_involuntary_vocalization *vocalization =
            &((s_involuntary_vocalization *)definitions->involuntary_vocalizations.address)[involuntary_index];
        result = vocalization->vocalization_index;
    }
    return result;
}

// 0x51e6d0
// @address 0x51e6d0 ai_dialogue_trigger_actor_scream
extern "C" bool ai_dialogue_trigger_actor_scream(long actor_index, short type)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->unit_index != NONE)
        unit_dialogue_test_scream(actor->unit_index, type, NONE, 11, 0);
    return false;
}

// 0x51e728
void ai_dialogue_event_aggregate_options(c_dialogue_event *event, c_dialogue_option *options, short *option_out, short)
{
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    if (ai_dialogue_aggregate_enabled() && definitions) {
        ai_dialogue_event_internal(definitions, event, options, 100, option_out);
    }
}

// 0x51e7d0
short ai_dialogue_event(c_dialogue_option *options, short option_count)
{
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    short result = NONE;
    if (definitions) {
        if (option_count > 0) {
            short option = dialogue_choose_option(options, option_count);
            if (option != NONE) {
                if (dialogue_speak(definitions, game_time_get(), &options[option], 0))
                    result = option;
            }
        }
    }
    return result;
}

// 0x51e898
bool ai_dialogue_event(c_dialogue_event *event)
{
    c_dialogue_option options[100];
    short count = 0;
    bool result = false;
    ai_dialogue_event_aggregate_options(event, options, &count, 100);
    if (count > 0)
        result = ai_dialogue_event(options, count) != NONE;
    return result;
}

// @address 0x51e920 ai_dialogue_event_has_valid_pattern
extern "C" bool ai_dialogue_event_has_valid_pattern(c_dialogue_event *event)
{
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    bool result = false;
    if (definitions && event->type >= 0 && event->type < definitions->ranges.s_tag_block::count) {
        vocalization_pattern_range *range = &((vocalization_pattern_range *)definitions->ranges.address)[event->type];
        if (range->count > 0) {
            for (short index = range->start; index < range->start + range->count; index++) {
                result = ai_dialogue_event_evaluate_pattern(definitions, event, &((vocalization_pattern *)definitions->patterns.address)[index], NULL, 0, NULL);
                if (result)
                    break;
            }
        }
    }
    return result;
}

// 0x51f3a0
// @address 0x51f3a0 ?pattern_match_ai_type_name@@YA_NJJPAM@Z
bool pattern_match_ai_type_name(long object_index, long name, float * weight)
{
    bool matched = false;
    if (object_index != -1) {
        unit_datum *o = dialogue_object_get_and_verify_type(object_index, -1);
        s_object_definition *d = (s_object_definition *)dialogue_tag_get(0x6f626a65, o->object.definition);
        if (d->ai_properties.count > 0) {
            object_ai_properties *p = (object_ai_properties *)d->ai_properties.address;
            if (p->ai_type_name == name)
                matched = true;
        }
        if (!matched && ((1 << o->object.type) & 0x1003)) {
            unit_datum *unit = o;
            if (unit->unit.parent_seat_index != -1 && dialogue_object_get_type(unit->object.parent) == 1) {
                long root = ai_get_root_vehicle(unit->object.parent);
                if (root != -1) {
                    unit_datum *vehicle = dialogue_object_get_and_verify_type(root, 2);
                    s_object_definition *vd = (s_object_definition *)dialogue_tag_get(0x76656869, vehicle->object.definition);
                    if (vd->ai_properties.count > 0) {
                        object_ai_properties *p = (object_ai_properties *)vd->ai_properties.address;
                        if (p->ai_type_name == name)
                            matched = true;
                    }
                }
            }
            if (!matched) {
                long weapon = dialogue_unit_inventory_get_weapon(object_index, dialogue_object_get_and_verify_type(object_index,
                    0x1003)->unit.current_weapon_index);
                if (weapon != -1) {
                    unit_datum *w = dialogue_object_get_and_verify_type(weapon, 4);
                    s_object_definition *wd = (s_object_definition *)dialogue_tag_get(0x77656170, w->object.definition);
                    if (wd->ai_properties.count > 0) {
                        object_ai_properties *p = (object_ai_properties *)wd->ai_properties.address;
                        if (p->ai_type_name == name)
                            matched = true;
                    }
                }
            }
        }
        if (matched && weight)
            *weight *= 2.0f;
    }
    return matched;
}

// 0x520130
bool ai_dialogue_event_internal(
    ai_dialogue_definitions *definitions, c_dialogue_event *event, c_dialogue_option *options, short maximum_options, short *option_out)
{
    bool result = false;
    if (event->type < 0 || event->type >= definitions->ranges.s_tag_block::count) {
        if (option_out)
            *option_out = 0;
    } else {
        vocalization_pattern_range *range = &((vocalization_pattern_range *)definitions->ranges.address)[event->type];
        if (range->count > 0) {
            for (short index = range->start;
                 index < range->start + range->count && (!options || *option_out < maximum_options);
                 index++) {
                vocalization_pattern *pattern = &((vocalization_pattern *)definitions->patterns.address)[index];
                result |= ai_dialogue_event_evaluate_pattern(definitions, event, pattern, options, maximum_options, option_out);
                if (result && !options)
                    break;
            }
        }
    }
    return result;
}


// 0x5205d0
bool dialogue_speak(ai_dialogue_definitions *dialogue_globals, long parameter, c_dialogue_option *option, long *queue_index)
{
    bool result = false;
    ai_information_packet ai_packet = { 0 };
    long game_time = tls_game_time_globals()->time;
    long speaker_object_index = option->speaker_object_index;
    unit_datum *object = dialogue_object_get_and_verify_type(speaker_object_index, 0x2003);
    long *team_state = (long *)team_dialogue_state_get((short)object->unit.team);
    if (queue_index)
        *queue_index = -1;
    memset(&ai_packet, 0, sizeof(ai_packet));
    ai_packet.type = option->event.type;
    ai_packet.listener_object_index = option->listener_object_index;
    ai_packet.payload = option->event.information;
    long index = ai_queue_dialogue((c_dialogue_event *)&option->event, speaker_object_index, option->vocalization_index, option->parameter,
        parameter, option->event.subtype, &ai_packet);
    if (index != -1) {
        if (queue_index)
            *queue_index = index;
        result = true;
    }
    if (result) {
        team_state[option->vocalization_index] = game_time +
            (long)(((s_dialogue_vocalization *)dialogue_globals->vocalizations.address)[option->vocalization_index].cooldown
            * (float)tls_game_time_globals()->ticks_per_second +
                0.5f);
    }
    return result;
}

// 0x521138
void dialogue_emotion(long actor_index, short emotion, short emotion_type)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (emotion >= 0 && emotion < 9)
    {
        unit_set_emotional_state(actor->unit_index, emotion, 1.0f, 0.5f);
        actor->emotion_type = emotion_type;
    }
}

// @address 0x5211a0 dialogue_event_can_bypass_squad
extern "C" unsigned char dialogue_event_can_bypass_squad(short type)
{
    bool result = false;
    switch (type - 62) {
    case _dialogue_event_evict_driver - 62: case _dialogue_event_evict_gunner - 62: case _dialogue_event_evict_passenger - 62:
    case _dialogue_event_player_look - 62: case _dialogue_event_player_look_longtime - 62:
    case _dialogue_event_weapon_trade_better - 62: case _dialogue_event_weapon_trade_worse - 62: case _dialogue_event_weapon_trade_equal - 62:
    case _dialogue_event_thank - 62: case _dialogue_event_ok - 62:
        result = true;
        break;
    default:
        break;
    }
    return result;
}

// 0x521288
extern "C" {
// @address 0x239e0 k_behavior_posture_info
}
// match: 0x521288: actor_get_posture receives the full actor index without a 16-bit mask.
extern "C" bool ai_dialogue_actor_can_vocalize(long actor_index, c_dialogue_event *event, s_dialogue_vocalization *vocalization)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_general_properties *properties = character_definition_general_properties_get(actor->character_index);
    bool can_vocalize = true;
    if (!properties || (properties->flags & 16) == 0)
    {
        if (actor->behavior_mode == 1 && actor != event->speaker && actor != event->target_speaker)
        {
            can_vocalize = false;
        }
        else if ((actor->berserking || actor_executing_layer(actor_index, 82) != NONE) && vocalization->priority < 13)
        {
            can_vocalize = false;
        }
        else if (actor->squad_index != NONE)
        {
            squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
            if (squad_valid_task(squad))
            {
                unsigned short task_index = squad->task_index;
                unsigned short objective_index = squad->objective_index;
                s_objective *objective = &((s_objective *)global_scenario->objectives.address)[(short)objective_index];
                s_task *task = &((s_task *)objective->tasks.address)[(short)task_index];
                if ((_rotl(task->flags, 28) & 1) != 0)
                {
                    s_objective_record *record = &((s_objective_record *)g_objectives_data->data)[objective_index];
                    s_task_record *task_record = &g_tasks_records[(short)((unsigned short)record->first_task + task_index)];
                    if ((_rotl((unsigned short)task_record->flags.get_unsafe(), 21) & 1) != 0)
                        can_vocalize = true;
                    else
                        can_vocalize = dialogue_event_can_bypass_squad(event->type) != 0;
                }
            }
        }
    }
    if (can_vocalize)
    {
        long posture = actor_get_posture(actor_index);
        if (posture != 0)
        {
            behavior_posture_info *posture_info = &k_behavior_posture_info[posture];
            if (posture_info && posture_info->disallow != 0)
                return false;
        }
    }
    return can_vocalize;
}

// 0x5227a0
long dialogue_find_speaker(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern, bool non_enemies,
    bool leader, bool same_type, long *sound_definition_index, short *chosen_vocalization_index, float *vocalization_factor,
    float *player_factor)
{
#if defined(_XBOX) && !defined(__clang__)
    const float zero = k_ai_dialogue_real_constants[0];
#else
    const float zero = 0.0f;
#endif
    long speaker = NONE;
    s_dialogue_vocalization *vocalization = 0;
    float best_weight = zero;
    float best_vocalization = zero;
    float best_player = zero;
    short best_index = NONE;
    long best_sound = NONE;
    real_point3d origin;
    object_get_origin(event->unit_index, &origin);
    actor_nearby_actors_iterator nearby;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x5227a0: the float loads through the constant-pool base register the function already holds
    actor_nearby_actors_iterator_new(&nearby, &origin, k_ai_dialogue_real_constants[424], (short)event->unit->unit.team, (short)!non_enemies);
#else
    actor_nearby_actors_iterator_new(&nearby, &origin, 15.0f, (short)event->unit->unit.team, (short)!non_enemies);
#endif
    for (;;) {
        actor_datum *actor = actor_nearby_actors_iterator_next(&nearby);
        if (!actor)
            break;
        long actor_index = nearby.actor_index;
        if (actor->unit_index != event->unit_index && actor->unit_index != event->target_index && actor->unit_index != NONE) {
            bool valid = true;
            if (!event->speaker)
                valid = true;
            else if (same_type)
                valid = event->speaker->type == actor->type;
            if (valid) {
                long sound;
                float factor;
                short vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, actor->unit_index, &sound, &factor);
                if (vocalization != NONE && factor > zero) {
                    float player = dialogue_player_weighting(actor->unit_index);
                    float weight = player * factor;
                    if (weight > best_weight) {
                        best_weight = weight;
                        best_vocalization = factor;
                        best_player = player;
                        speaker = actor->unit_index;
                        best_index = vocalization;
                        best_sound = sound;
                    }
                }
            }
        }
    }
    *sound_definition_index = best_sound;
    *chosen_vocalization_index = best_index;
    *vocalization_factor = best_vocalization;
    *player_factor = best_player;
    return speaker;
}

// 0x522940
long dialogue_find_speaker_vehicle(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    bool leader, long *sound_definition_index, short *chosen_vocalization_index, float *vocalization_factor, float *player_factor)
{
    long speaker = NONE;
    s_dialogue_vocalization *vocalization = 0;
    float best_weight = 0.0f;
    float best_vocalization = 0.0f;
    float best_player = 0.0f;
    short best_index = NONE;
    long best_sound = NONE;
    if (event->unit && event->unit->object.parent != NONE && event->unit->unit.parent_seat_index != NONE
        && dialogue_object_get_type(event->unit->object.parent) == 1) {
        long vehicle_index = vehicle_get_ultimate_parent(event->unit->object.parent);
        unit_seat_source seats[64];
        short seat_count = unit_get_all_seats(vehicle_index, seats, 64);
        for (short i = 0; i < seat_count; i++) {
            long occupant = ::unit_seat_get_occupant(seats[i].unit_index, seats[i].seat_index);
            if (occupant != event->unit_index && occupant != event->other_unit_index && occupant != NONE) {
                unit_datum *unit = dialogue_object_get_and_verify_type(occupant, 0x1003);
                if (!game_team_is_enemy((e_game_team)unit->unit.team, (e_game_team)event->unit->unit.team) && unit->unit.actor_index != NONE) {
                    long sound;
                    float factor;
                    short vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, occupant, &sound, &factor);
                    if (vocalization != NONE && factor > 0.0f) {
                        float player = dialogue_player_weighting(occupant);
                        float weight = player * factor;
                        if (weight > best_weight) {
                            best_weight = weight;
                            best_vocalization = factor;
                            best_player = player;
                            best_index = vocalization;
                            best_sound = sound;
                            speaker = occupant;
                        }
                    }
                }
            }
        }
    }
    *vocalization_factor = best_vocalization;
    *player_factor = best_player;
    *sound_definition_index = best_sound;
    *chosen_vocalization_index = best_index;
    return speaker;
}

// 0x522b20
long dialogue_find_speaker_joint(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern, bool leader,
    long *sound_definition_index, short *chosen_vocalization_index, float *vocalization_factor, float *player_factor)
{
    long speaker = NONE;
    s_dialogue_vocalization *vocalization = 0;
    float best_weight = 0.0f;
    float best_vocalization = 0.0f;
    float best_player = 0.0f;
    short best_index = NONE;
    long best_sound = NONE;
    if (event->speaker) {
        actor_datum *actor = event->speaker;
        short joint_layer = NONE;
        for (short layer = actor->layer_count; layer >= 0; layer--) {
            c_joint_behavior_data *state = (c_joint_behavior_data *)actor_behavior_state_get(actor, layer);
            if (state && state->behavior_index >= 0 && state->behavior_index < 197) {
                Behavior_header *definition = ((behavior_info *)global_behavior_info)[state->behavior_index].header;
                if ((short)definition->type == 2 || (short)definition->type == 1) {
                    if (static_cast<Behavior *>(definition)->joint) {
                        joint_state_datum *joint = datum_get<joint_state_datum>(joint_state_data(), state->joint_index);
                        if (joint->joint_type == 1) {
                            joint_layer = layer;
                            break;
                        }
                    }
                }
            }
        }
        if (joint_layer != NONE) {
            c_joint_behavior_data *state = (c_joint_behavior_data *)actor_behavior_state_get(actor, joint_layer);
            if (state) {
                joint_state_datum *joint = datum_get<joint_state_datum>(joint_state_data(), state->joint_index);
                for (short i = 0; i < 10; i++) {
                    joint_member *participant = &joint->participants[i];
                    if (participant->actor_index != NONE && participant->status == 3 &&
                        participant->actor_index != event->unit->unit.actor_index &&
                        (!leader || actor_is_leader(event->unit->unit.actor_index, participant->actor_index)))
                    {
                        actor_datum *candidate = datum_get<actor_datum>(tls_actor_data(), participant->actor_index);
                        if (candidate->unit_index != NONE) {
                            long sound;
                            float factor;
                            short vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, candidate->unit_index, &sound, &factor);
                            if (vocalization != NONE && factor > 0.0f) {
                                float player = dialogue_player_weighting(candidate->unit_index);
                                float weight = player * factor;
                                if (weight > best_weight) {
                                    best_weight = weight;
                                    best_vocalization = factor;
                                    best_player = player;
                                    best_index = vocalization;
                                    best_sound = sound;
                                    speaker = candidate->unit_index;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    *vocalization_factor = best_vocalization;
    *player_factor = best_player;
    *sound_definition_index = best_sound;
    *chosen_vocalization_index = best_index;
    return speaker;
}

// 0x523068
team_dialogue_state *team_dialogue_state_get(short team)
{
    return &((team_dialogue_state *)g_team_dialogue_state)[dialogue_team_get(team)];
}

// 0x5230c8
short dialogue_choose_option(c_dialogue_option *options, short option_count)
{
    short result = NONE;
    if (option_count == 1)
    {
        result = 0;
    }
    else
    {
        float maximum_weight = 0.0f;
        float total_weight = 0.0f;
        for (short i = 0; i < option_count; i++)
        {
            if (options[i].weight > maximum_weight)
                maximum_weight = options[i].weight;
        }
        for (short j = 0; j < option_count; j++)
        {
            if (options[j].weight >= 0.6f * maximum_weight)
                total_weight += options[j].weight;
            else
                options[j].weight = 0.0f;
        }
        float random_weight = _real_random_range(tls_random_seed(), 0.0f, total_weight);
        total_weight = 0.0f;
        for (short k = 0; k < option_count; k++)
        {
            if (options[k].weight > 0.0f)
            {
                total_weight += options[k].weight;
                if (random_weight <= total_weight)
                {
                    result = k;
                    break;
                }
            }
        }
    }
    return result;
}

// 0x523218
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_team_dialogue_state_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x523218: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    valid();
    m_member_index = dialogue_restricted_region_add_member(1, name, type, size, alignment_bits,
                                                          __tls_set_g_team_dialogue_state_allocator);
    m_thread_id = get_current_thread_index();
    return dialogue_restricted_region_get_member_address(1, m_member_index);
}

// 0x5232b8
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_team_dialogue_state_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x5232d0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_team_dialogue_state_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_team_dialogue_state_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    g_team_dialogue_state = 0;
    restricted_region_free_member(1, value);
}

// @address 0x5232f0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_vocalization_record_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_vocalization_record_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[1].add_member(name, "", size, 0, __tls_set_g_cached_object_render_states_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&g_ai_dialogue_records, (s_data_array *)(restricted_region_get_base_address(1) + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_base_address(1) + g_restricted_regions[1].get_member_offset(member_index);
}

// @address 0x523390 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_vocalization_record_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_vocalization_record_allocator>::deallocate(void *memory)
{
    g_ai_dialogue_records = 0;
    restricted_region_free_member(1, m_member_index);
}

// ---------------------------------------------------------------------------
// 0x51ea28 pattern_match_object_type
// @address 0x51ea28 ?pattern_match_object_type@@YA_NFJPAM@Z
// ---------------------------------------------------------------------------
// @rdata 0x82022510
static const float ai_dialogue_vocalization_fallback_scale = 0.8f;

static __forceinline actor_datum *pattern_actor_get(long actor_index)
{
    return datum_get<actor_datum>(tls_actor_data(), actor_index);
}
static __forceinline actor_datum *pattern_actor_get_from_object(unit_datum *object)
{
    actor_datum *result = 0;
    if (object->unit.actor_index != NONE)
        result = pattern_actor_get(object->unit.actor_index);
    return result;
}
static __forceinline clump_datum *pattern_clump_get(long clump_index)
{
    return datum_get<clump_datum>(tls_clump_data(), clump_index);
}
static __forceinline unit_datum *pattern_vehicle_get(long object_index)
{
    return (unit_datum *)dialogue_object_get_and_verify_type(object_index, 0x1003);
}
static __forceinline long pattern_unit_get_dialogue(unit_datum *unit)
{
    return *(long *)((char *)unit + unit->unit.dialogue_offset);
}
bool pattern_match_object_type(short pattern, long object_index, float *weight)
{
    bool result = true;
    unit_seat_source seats[64];
    unit_seat_source scan_seats[64];
    if (object_index != NONE)
    {
        s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
        unit_datum *object = (unit_datum *)datum_get<s_object_header>(headers, object_index)->datum;
        unit_datum *unit = 0;
        actor_datum *actor = 0;
        if (((1 << object->object.type) & 0x1003) != 0)
        {
            unit = object;
            actor = pattern_actor_get_from_object(object);
        }
        else if (((1 << object->object.type) & 0x800) != 0 && ((creature_datum *)object)->creature.link_type == 0)
        {
            long creature_actor = ((swarm_creature_state *)((char *)object + ((creature_datum *)object)->creature.creature_reference.offset))->swarm_index;
            if (creature_actor != NONE)
                actor = pattern_actor_get(creature_actor);
        }

        result = false;
        switch (pattern)
        {
        case _dialogue_object_any:
            if (weight != 0)
                *weight = *weight * ai_dialogue_vocalization_fallback_scale;
            result = true;
            break;
        case _dialogue_object_player:
            if (unit != 0)
                result = unit->unit.player_index + 1 != 0;
            break;
        case _dialogue_object_actor:
            result = actor != 0;
            break;
        case _dialogue_object_living_biped:
            result = object->object.type == 0 && (_rotl(object->object.damage_flags, 30) & 1) == 0;
            break;
        case _dialogue_object_dead_biped:
            result = object->object.type == 0 && (_rotl(object->object.damage_flags, 30) & 1) != 0;
            break;
        case _dialogue_object_vehicle:
            result = object->object.type == 1 && vehicle_get_type(object_index) != 5;
            break;
        case _dialogue_object_projectile:
            result = object->object.type - 5 == 0;
            break;
        case _dialogue_object_actor_or_player:
            if (actor != 0 || (unit != 0 && unit->unit.player_index != NONE))
                result = true;
            break;
        case _dialogue_object_turret:
            if (unit != 0 && unit->object.type == 1)
            {
                long root = ai_get_root_vehicle(object_index);
                result = vehicle_get_type(root) - 5 == 0;
            }
            break;
        case _dialogue_object_vehicle_occupant:
            if (unit != 0)
                result = (short)unit->unit.parent_seat_index + 1 != 0;
            break;
        case _dialogue_object_turret_occupant:
            if (unit == 0 || unit->unit.parent_seat_index == (short)0xffff)
                break;
            if (datum_get<s_object_header>(headers, unit->object.parent)->object_type != 1)
                break;
            result = vehicle_get_type(ai_get_root_vehicle(unit->object.parent)) - 5 == 0;
            break;
        case _dialogue_object_deployed_turret:
            if (actor == 0)
                break;
            result = actor->deployed_turret;
            break;
        case _dialogue_object_driver:
            if (unit == 0 || unit->unit.parent_seat_index == (short)0xffff)
                break;
            if (datum_get<s_object_header>(headers, unit->object.parent)->object_type != 1)
                break;
            result = object_index - ((unit_datum *)datum_get<s_object_header>(headers,
                unit->object.parent)->datum)->unit.driver_object_index == 0;
            break;
        case _dialogue_object_gunner:
            if (unit == 0 || unit->unit.parent_seat_index == (short)0xffff)
                break;
            if (datum_get<s_object_header>(headers, unit->object.parent)->object_type != 1)
                break;
            result = object_index - ((unit_datum *)datum_get<s_object_header>(headers,
                unit->object.parent)->datum)->unit.gunner_object_index == 0;
            break;
        case _dialogue_object_passenger:
            if (unit == 0 || unit->unit.parent_seat_index == (short)0xffff)
                break;
            if (datum_get<s_object_header>(headers, unit->object.parent)->object_type != 1)
                break;
            {
                unit_datum *parent = (unit_datum *)datum_get<s_object_header>(headers, unit->object.parent)->datum;
                result = parent->unit.driver_object_index != object_index && parent->unit.gunner_object_index != object_index;
            }
            break;
        case _dialogue_object_postcombat:
            if (actor == 0)
                break;
            if (actor->clump_index == NONE)
                break;
            result = pattern_clump_get(actor->clump_index)->combat_state - 1 == 0;
            break;
        case _dialogue_object_postcombat_winner:
            if (actor == 0)
                break;
            {
                long clump_index = actor->clump_index;
                if (clump_index == NONE)
                    break;
                result = pattern_clump_get(clump_index)->combat_state == 1 && (short)clump_calculate_postombat_result(clump_index) == 1;
            }
            break;
        case _dialogue_object_postcombat_loser:
            if (actor == 0)
                break;
            {
                long clump_index = actor->clump_index;
                if (clump_index == NONE)
                    break;
                result = pattern_clump_get(clump_index)->combat_state == 1 && (short)clump_calculate_postombat_result(clump_index) == 2;
            }
            break;
        case _dialogue_object_player_spartan:
            if (unit == 0)
                break;
            {
                long player_index = unit->unit.player_index;
                if (player_index == NONE)
                    break;
                result = game_get_player_character_type(player_index) == 0;
            }
            break;
        case _dialogue_object_player_elite:
            if (unit == 0)
                break;
            {
                long player_index = unit->unit.player_index;
                if (player_index == NONE)
                    break;
                result = game_get_player_character_type(player_index) - 1 == 0;
            }
            break;
        case _dialogue_object_heretic:
            if (actor != 0)
                result = (short)actor->team - 6 == 0;
            break;
        case _dialogue_object_scary:
            if (actor != 0 && unit != 0)
                result = actor_is_majorly_scary(unit->unit.actor_index);
            break;
        case _dialogue_object_only_vehicle_operator:
            if (unit == 0 || unit->unit.parent_seat_index == (short)0xffff)
                break;
            if (datum_get<s_object_header>(headers, unit->object.parent)->object_type != 1)
                break;
            {
                unit_datum *parent = (unit_datum *)datum_get<s_object_header>(headers, unit->object.parent)->datum;
                if (parent->unit.gunner_object_index != object_index && parent->unit.driver_object_index != object_index)
                    break;
            }
            result = true;
            {
                short seat_count = 0;
                long root = vehicle_get_ultimate_parent(unit->object.parent);
                ::unit_get_all_seats_internal(root, (unit_seat_source *)seats, (short *)(&seat_count), 64, false);
                for (short i = 0; i < seat_count; i++)
                {
                    long occupant = ::unit_seat_get_occupant(seats[i].unit_index, seats[i].seat_index);
                    if (occupant != NONE && occupant != object_index)
                    {
                        unit_datum *occupied = pattern_vehicle_get(seats[i].unit_index);
                        if (occupied->unit.gunner_object_index == occupant || occupied->unit.driver_object_index == occupant)
                        {
                            result = 0;
                            break;
                        }
                    }
                }
            }
            break;
        case _dialogue_object_normal_dialogue:
            if (unit == 0)
                break;
            {
                long dialogue_index = pattern_unit_get_dialogue(object);
                if (dialogue_index == NONE)
                    break;
                result = (((dialogue_definition *)dialogue_tag_get('udlg', dialogue_index))->flags & 1) == 0;
            }
            break;
        case _dialogue_object_special_dialogue:
            if (unit == 0)
                break;
            {
                long dialogue_index = pattern_unit_get_dialogue(object);
                if (dialogue_index == NONE)
                    break;
                result = (((dialogue_definition *)dialogue_tag_get('udlg', dialogue_index))->flags & 1) != 0;
            }
            break;
        case _dialogue_object_grenade:
            if (object->object.type != 5)
                break;
            {
                unsigned char damage_type = ((projectile_definition *)dialogue_tag_get('proj', object->object.definition))->damage_reporting_flags;
                if (damage_type > 60 || g_game_results_damage_type_descriptions[damage_type].breakdown_type != 4)
                    break;
            }
            result = true;
            break;
        case _dialogue_object_speech_disabled:
            if (unit != 0)
                result = (unit->unit.unit_flags >> 3) & 1;
            break;
        case _dialogue_object_flood:
            if (unit != 0)
                result = unit->unit.team - 4 == 0;
            break;
        case _dialogue_object_pureform:
            if (actor != 0)
                result = ((1 << (short)actor->type) & 0x700000) != 0;
            break;
        case _dialogue_object_player_with_empty_seat:
            if (unit == 0 || unit->unit.player_index == NONE)
                break;
            {
                long parent = unit->object.parent;
                if (parent == NONE)
                    break;
                if (unit->unit.parent_seat_index == (short)0xffff)
                    break;
                do {
                    unit_datum *link = (unit_datum *)datum_get<s_object_header>(headers, parent)->datum;
                    if ((_rotl(link->object.flags, 6) & 1) == 0)
                        break;
                    parent = link->object.parent;
                } while (parent != NONE);

                short seat_count = 0;
                bool found = false;
                ::unit_get_all_seats_internal(parent, (unit_seat_source *)scan_seats, (short *)(&seat_count), 64, false);
                for (short i = 0; i < seat_count; i++)
                {
                    if ((_rotl(*(const unsigned int *)scan_seats[i].definition, 21) & 1) == 0
                        && ::unit_seat_get_occupant(scan_seats[i].unit_index, scan_seats[i].seat_index) == NONE)
                    {
                        found = true;
                        break;
                    }
                }
                result = found;
            }
            break;
        }
    }
    else if (pattern != 0)
    {
        result = false;
    }
    return result;
}

__forceinline void c_dialogue_option::set(c_dialogue_event *source, short vocalization, long speaker, long listener, long parameter_value, float option_weight)
{
    event = *source;
    vocalization_index = vocalization;
    speaker_object_index = speaker;
    listener_object_index = listener;
    parameter = parameter_value;
    weight = option_weight;
}

static __forceinline short vocalization_get_ultimate_parent(ai_dialogue_definitions *definitions, short vocalization_index)
{
    long ultimate_index = vocalization_index;
    while (vocalization_index != NONE)
    {
        if (vocalization_index >= 0 && vocalization_index < definitions->vocalizations.s_tag_block::count)
        {
            s_dialogue_vocalization *vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[vocalization_index];
            ultimate_index = vocalization_index;
            vocalization_index = vocalization->parent_index;
        }
    }
    return (short)ultimate_index;
}

// @address 0x51f900 ?ai_dialogue_event_evaluate_pattern@@YA_NPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@PAVc_dialogue_option@@FPAF@Z
bool ai_dialogue_event_evaluate_pattern(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    c_dialogue_option *options, short maximum_options, short *option_out)
{
    long speaker = NONE;
    s_dialogue_vocalization *vocalization = 0;
    short out_vocalization = NONE;
    long parameter = NONE;
    float total_weight = 0.0f;
    float weight = 1.0f;
    float factor = 0.0f;
    float player_weight = 0.0f;
    bool valid = true;

    if (pattern->vocalization_index < 0 || pattern->vocalization_index >= definitions->vocalizations.s_tag_block::count)
    {
        valid = false;
    }
    else
    {
        vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[pattern->vocalization_index];
        total_weight = vocalization->weight;

        if (pattern->relation == 0)
        {
            if (event->relation != 0 && event->relation != NONE)
                weight *= 0.8f;
        }
        else if (pattern->relation != event->relation)
        {
            valid = false;
        }

        if (valid)
        {
            if (pattern->vocalization_filter == 0)
            {
                if (event->vocalization != NONE && event->vocalization != 0)
                    weight *= 0.8f;
            }
            else if (event->vocalization != pattern->vocalization_filter)
            {
                valid = false;
            }

            if (valid)
            {
                short pattern_speaker_team = (short)(pattern->speaker_team - 1);
                short pattern_target_team = (short)(pattern->target_team - 1);
                if (event->speaker_team != NONE && pattern_speaker_team == NONE)
                {
                    weight *= 0.6f;
                }
                else if (event->speaker_team != pattern_speaker_team)
                {
                    valid = false;
                }

                if (valid)
                {
                    if (event->target_speaker_team != NONE && pattern_target_team == NONE)
                    {
                        weight *= 0.6f;
                    }
                    else if (event->target_speaker_team != pattern_target_team)
                    {
                        valid = false;
                    }
                }
            }
        }

        if (valid)
            valid = pattern_match_object_type(pattern->target_object_type, event->target_index, &weight);
        if (valid)
            valid = pattern_match_object_type(pattern->unit_object_type, event->unit_index, &weight);
        if (valid && pattern->target_ai_type_name != 0)
            valid = pattern_match_ai_type_name(event->target_index, pattern->target_ai_type_name, &weight);
        if (valid && pattern->unit_ai_type_name != 0)
            valid = pattern_match_ai_type_name(event->unit_index, pattern->unit_ai_type_name, &weight);

        if (valid && pattern->spatial_relation != _dialogue_spatial_none)
        {
            valid = false;
            if (event->unit_index != NONE && event->target_index != NONE)
                valid = pattern_match_spatial_relation(event->unit_index, event->target_index, pattern->spatial_relation);
            if (valid)
                weight *= 1.5f;
        }
    }

    total_weight *= weight;

    if (valid && options != 0 && vocalization != 0 && total_weight > 0.0f)
    {
        switch (pattern->speaker_type)
        {
        case _dialogue_speaker_unit:
            if (event->unit_index != NONE)
            {
                speaker = event->unit_index;
                out_vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, speaker, &parameter, &factor);
                if (factor > 0.0f)
                    player_weight = dialogue_player_weighting(speaker);
            }
            break;
        case _dialogue_speaker_other_unit:
            if (event->other_unit_index != NONE)
            {
                speaker = event->other_unit_index;
                out_vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, speaker, &parameter, &factor);
                if (factor > 0.0f)
                    player_weight = dialogue_player_weighting(speaker);
            }
            break;
        case _dialogue_speaker_nearby_non_enemy:
            speaker = dialogue_find_speaker(definitions, event, pattern, true, false, false, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_nearby_non_enemy_same_type:
            speaker = dialogue_find_speaker(definitions, event, pattern, true, false, true, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_nearby_enemy:
            speaker = dialogue_find_speaker(definitions, event, pattern, false, false, false, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_vehicle_occupant:
            if (event->unit_index != NONE)
                speaker = dialogue_find_speaker_vehicle(definitions, event, pattern, false, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_joint_member:
            if (event->unit_index != NONE)
                speaker = dialogue_find_speaker_joint(definitions, event, pattern, false, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_task_member:
            speaker = dialogue_task_find_speaker(definitions, event, pattern, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_squad_leader:
            if (event->speaker != 0 && event->speaker->squad_index != NONE)
            {
                long leader_index = squad_get_leader((short)event->speaker->squad_index);
                if (leader_index != NONE)
                {
                    actor_datum *leader = datum_get<actor_datum>(tls_actor_data(), leader_index);
                    if (leader->unit_index != NONE && leader != event->speaker)
                    {
                        speaker = leader->unit_index;
                        out_vocalization = dialogue_unit_can_vocalize(definitions, event, pattern, speaker, &parameter, &factor);
                        if (factor > 0.0f)
                            player_weight = dialogue_player_weighting(speaker);
                    }
                }
            }
            break;
        case _dialogue_speaker_joint_leader:
            speaker = dialogue_find_speaker_joint(definitions, event, pattern, true, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_clump_member:
            speaker = dialogue_find_speaker_clump(definitions, event, pattern, &parameter, &out_vocalization, &factor, &player_weight);
            break;
        case _dialogue_speaker_none:
            speaker = NONE;
            player_weight = 0.0f;
            break;
        }

        total_weight *= factor * player_weight;

        if (speaker != NONE && total_weight > 0.0f)
        {
            unit_datum *object = (unit_datum *)datum_get<s_object_header>(tls_object_header_data(), speaker)->datum;
            long listener = NONE;
            switch (pattern->listener_type)
            {
            case _dialogue_listener_unit:
                listener = event->unit_index;
                break;
            case _dialogue_listener_target:
                listener = event->target_index;
                break;
            case _dialogue_listener_target_prop:
                if (object->unit.actor_index != NONE)
                {
                    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), object->unit.actor_index);
                    if (actor->target_prop_index != NONE)
                    {
                        listener = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index)->object_index;
                        break;
                    }
                }
            case _dialogue_listener_none:
                listener = NONE;
                break;
            }

            options[*option_out].set(event, out_vocalization, speaker, listener, parameter, total_weight);
            *option_out = *option_out + 1;
        }

        long ultimate_index = vocalization_get_ultimate_parent(definitions, pattern->vocalization_index);
        if (ultimate_index != NONE)
        {
            s_dialogue_vocalization *ultimate = &((s_dialogue_vocalization *)definitions->vocalizations.address)[ultimate_index];
            if (ultimate->response_type >= 0 && ultimate->response_type < 112)
            {
                c_dialogue_event internal_event;
                memcpy(&internal_event, event, sizeof(internal_event));
                internal_event.type = ultimate->response_type;
                ai_dialogue_event_internal(definitions, &internal_event, options, 100, option_out);
            }
        }
    }
    return valid;
}

// Vocalization record layout expected by the dialogue queue.
typedef char dialogue_record_layout_0[(sizeof(unit_dialogue_vocalization_record) == 0x5c) ? 1 : -1];
typedef char dialogue_record_layout_1[(offsetof(unit_dialogue_vocalization_record, duration) == 0x1c) ? 1 : -1];
typedef char dialogue_record_layout_2[(offsetof(unit_dialogue_vocalization_record, information) + offsetof(unit_dialogue_information_packet,
    subject) == 0x2c) ? 1 : -1];
typedef char dialogue_record_layout_3[(offsetof(unit_dialogue_vocalization_record, interrupted) == 0x49) ? 1 : -1];
typedef char dialogue_record_layout_4[(offsetof(unit_dialogue_vocalization_record, dialogue_variant) == 0x54) ? 1 : -1];
// @rdata 0x82022f04..0x82022f34
static const long g_involuntary_vocalization_names[12] = { 0x0, 0x31c, 0x308, 0x2d, 0x2e, 0x309, 0x30a, 0x30b, 0x30c, 0x30d, 0x30e, 0x30f };
// @rdata-end
// @address 0x22f38 ?global_dialogue_priorities@@3QBUs_priority_definition@@B
#if !defined(_XBOX) || defined(__clang__)
const s_priority_definition global_dialogue_priorities[16] = {
    { 0, 0, { 0.0f, 0.0f, 0.0f, 0.0f }, 0 },
    { 1, 0, { 180.0f, 0.0f, 0.5f, 0.0f }, 2 },
    { 2, 0, { 3.0f, 0.0f, 0.5f, 0.0f }, 2 },
    { 3, 0, { 5.0f, 0.0f, 0.5f, 0.0f }, 2 },
    { 4, 0, { 1.0f, 0.0f, 0.5f, 0.0f }, 2 },
    { 5, 0, { 5.0f, 0.0f, 0.5f, 0.0f }, 2 },
    { 6, 3, { 2.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 7, 6, { 3.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 8, 6, { 1.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 9, 9, { 3.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 10, 9, { 0.2f, 0.0f, 0.5f, 0.0f }, 9 },
    { 11, 10, { 0.2f, 0.0f, 0.5f, 0.0f }, 9 },
    { 12, 11, { 0.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 13, 11, { 0.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 14, 12, { 0.0f, 0.0f, 0.5f, 0.0f }, 9 },
    { 15, 14, { 0.0f, 0.0f, 0.5f, 0.0f }, 9 },
};
#endif

static __forceinline float ai_dialogue_ticks_to_seconds(long ticks)
{
    return tls_game_time_globals()->tick_length * (float)ticks;
}
static __forceinline short ai_dialogue_seconds_to_ticks_round(float seconds)
{
    return (short)(int)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}
static __forceinline void ai_dialogue_actor_move_freeze(long actor_index, short ticks)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->move_freeze_ticks < ticks)
        actor->move_freeze_ticks = ticks;
}
// match: c_ai_direction::clear() with the forward vector by address: folds the const pointer
// global_forward3d (?global_forward3d@@3QBTreal_vector3d@@B) to &global_identity.forward here; the tree declares it non-const.
static __forceinline void ai_dialogue_direction_clear(c_ai_direction *direction)
{
    direction->type = NONE;
    direction->m_point.set(global_z_axis3d);
}
static __forceinline void ai_dialogue_action_setup_animation(c_ai_action *action, const c_ai_direction *direction, long animation)
{
    action->type = 5;
    action->point_valid = false;
    if (direction)
    {
        action->direction = *direction;
    }
    else
    {
        ai_dialogue_direction_clear(&action->direction);
    }
    action->impulse_index = animation;
}
static __forceinline void ai_dialogue_actor_look_at(actor_datum *actor, float seconds, const unit_speech_item::s_information *information)
{
    if (actor->attention_level <= 1)
    {
        actor->attention_ticks = ai_dialogue_seconds_to_ticks_round(seconds);
        actor->attention_direction.to_object(information->subject);
        actor->attention_level = 1;
    }
}

// @address 0x520748 ?ai_dialogue_started@@YAXJPAUunit_speech_item@@@Z
void ai_dialogue_started(long unit_index, unit_speech_item *item)
{
    unit_datum *unit = dialogue_object_get_and_verify_type(unit_index, 0x1003);
    unit_datum *target = 0;
    unit_dialogue_vocalization_record *record = 0;
    actor_datum *actor = 0;
    unit_speech_item::s_information *information = &item->information;
    if (information->subject != NONE)
        target = (unit_datum *)object_try_and_get_and_verify_type(information->subject, -1);
    if (item->dialogue_index != NONE)
        record = datum_get<unit_dialogue_vocalization_record>((s_data_array *)g_ai_dialogue_records, item->dialogue_index);
    if (unit->unit.actor_index != NONE)
        actor = datum_get<actor_datum>(tls_actor_data(), unit->unit.actor_index);

    if (target && actor && information->subject != unit_index)
    {
        float seconds = 1.5f;
        if (record)
            seconds = ai_dialogue_ticks_to_seconds(record->duration);
        ai_dialogue_actor_look_at(actor, seconds, information);
    }

    ai_dialogue_definitions *definitions;
    if (item->vocalization != NONE && (definitions = ai_dialogue_globals_get()) != 0)
    {
        s_dialogue_vocalization *vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[item->vocalization];
        if (actor && vocalization->self_interrupt_seconds > 0.0f)
            ai_dialogue_actor_move_freeze(unit->unit.actor_index, ai_dialogue_seconds_to_ticks_round(vocalization->self_interrupt_seconds));
        if (vocalization->target_interrupt_seconds > 0.0f && target && ((1 << target->object.type) & 0x1003) && target->unit.actor_index != NONE)
            ai_dialogue_actor_move_freeze(target->unit.actor_index, ai_dialogue_seconds_to_ticks_round(vocalization->target_interrupt_seconds));
        if (actor && !motor_animation_busy(unit_index))
        {
            if (vocalization->action >= 0 && vocalization->action < 12)
            {
                long actor_index = unit->unit.actor_index;
                bool can_animate = true;
                if (actor_executing_layer(actor_index, 102) != NONE
                    || actor_executing_layer(actor_index, 106) != NONE
                    || actor_executing_layer(actor_index, 27) != NONE
                    || actor_executing_layer(actor_index, 28) != NONE)
                {
                    can_animate = false;
                }
                if (can_animate)
                {
                    switch (vocalization->action)
                    {
                    case _dialogue_action_shakefist:
                    case _dialogue_action_surprise_front:
                    case _dialogue_action_surprise_back:
                    case _dialogue_action_taunt:
                    case _dialogue_action_brace:
                    case _dialogue_action_point:
                    case _dialogue_action_wave:
                        if (target)
                        {
                            ai_dialogue_actor_look_at(actor, 2.0f, information);
                            ai_dialogue_action_setup_animation(&actor->action, &actor->attention_direction,
                                g_involuntary_vocalization_names[vocalization->action]);
                            break;
                        }
                    case _dialogue_action_cheer:
                    case _dialogue_action_hold:
                    case _dialogue_action_advance:
                    case _dialogue_action_fallback:
                        actor_impulse_submit(actor_index, g_involuntary_vocalization_names[vocalization->action], 0, 0);
                        break;
                    }
                }
            }
        }
        if (record && actor && vocalization->emotion > 0)
            dialogue_emotion(unit->unit.actor_index, vocalization->emotion, (short)(tls_game_time_globals()->ticks_per_second + record->duration));
    }

    if (actor && item->vocalization != NONE && item->dialogue_index != NONE && actor->clump_index != NONE)
    {
        character_general_properties *properties = character_definition_general_properties_get(actor->character_index);
        if (properties && (properties->flags & 0x10))
        {
            clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), actor->clump_index);
            clump_actor_iterator iterator;
            clump_actor_iterator_new(&iterator, clump);
            actor_datum *other;
            while ((other = clump_actor_iterator_next(&iterator)) != 0)
            {
                if (other != actor)
                {
                    other->pending_vocalization = item->vocalization;
                    other->vocalization_pitch_range = item->pitch_range;
                    other->vocalization_permutation = item->permutation_index;
                    other->vocalization_delay_ticks = ai_dialogue_seconds_to_ticks_round(_real_random_range(tls_random_seed(), 0.1f, 0.5f));
                }
            }
        }
    }
}

// @rdata 0x82021694
static const float ai_dialogue_range_maximum = FLT_MAX;
// @rdata 0x820221fc
static const float ai_dialogue_range_one = 1.0f;
// @rdata 0x82024440
static const float ai_dialogue_range_nine = 9.0f;

static __forceinline real_vector3d *ai_dialogue_range_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *out)
{
    out->i = b->x - a->x;
    out->j = b->y - a->y;
    out->k = b->z - a->z;
    return out;
}
static __forceinline bool ai_dialogue_actor_get_range(long actor_index, float *range)
{
    bool result = false;
    *range = ai_dialogue_range_maximum;
    character_weapon_properties *properties = actor_weapon_properties_get(actor_index);
    if (properties) {
        *range = properties->maximum_range;
        result = true;
    }
    return result;
}
static __forceinline float ai_dialogue_range_back_dot(const real_vector3d *facing, const real_vector3d *delta)
{
    float x = delta->i;
    return (facing->j * delta->j + facing->k * delta->k) + facing->i * x;
}
// @address 0x51f560 pattern_match_spatial_relation
extern "C" bool pattern_match_spatial_relation(long object_index, long other_index, short range_type)
{
    bool result = false;
    unit_datum *object = (unit_datum *)dialogue_object_get_and_verify_type(object_index, 0xffffffff);
    real_point3d origin, target;
    object_get_origin(object_index, &origin);
    object_get_origin(other_index, &target);
    real_vector3d delta;
    ai_dialogue_range_vector_from_points(&origin, &target, &delta);
    float distance_squared = (delta.j * delta.j + delta.k * delta.k) + delta.i * delta.i;
    switch (range_type) {
    case _dialogue_spatial_within_1m:
        result = distance_squared <= ai_dialogue_range_one;
        break;
    case _dialogue_spatial_within_2_5m:
        result = distance_squared <= 6.25f;
        break;
    case _dialogue_spatial_2_5m_to_8m:
        result = distance_squared <= 64.0f && distance_squared > 6.25f;
        break;
    case _dialogue_spatial_beyond_8m:
        result = distance_squared > 64.0f;
        break;
    case _dialogue_spatial_beyond_15m:
        result = distance_squared > 225.0f;
        break;
    case _dialogue_spatial_in_front_within_3m:
        if (distance_squared < ai_dialogue_range_nine) {
            real_vector3d facing_delta;
            ai_dialogue_range_vector_from_points(&origin, &target, &facing_delta);
            const real_vector3d *facing = &object->object.forward;
            float x = facing_delta.i;
            float facing_i = facing->i;
            float facing_j = facing->j;
            float facing_k = facing->k;
            float delta_j = facing_delta.j;
            float delta_k = facing_delta.k;
            float partial_dot = facing_j * delta_j + facing_k * delta_k;
            result = partial_dot + facing_i * x > 0.0f;
        }
        break;
    case _dialogue_spatial_behind_within_3m:
        if (distance_squared < ai_dialogue_range_nine)
            result = ai_dialogue_range_back_dot(&object->object.forward, &delta) < 0.0f;
        break;
    case _dialogue_spatial_above:
        if (delta.k > ai_dialogue_range_one)
            result = (float)atan2(delta.k, sqrtf(distance_squared)) > 0.19198622f;
        break;
    case _dialogue_spatial_below:
        if (delta.k < -1.0f)
            result = (float)atan2(delta.k, sqrtf(distance_squared)) < -0.19198622f;
        break;
    case _dialogue_spatial_clump_3_or_fewer:
        if (((1 << object->object.type) & 0x1003) && object->unit.actor_index != -1) {
            long clump_index = pattern_actor_get(object->unit.actor_index)->clump_index;
            if (clump_index != -1 && datum_get<clump_datum>(tls_clump_data(), clump_index)->actor_count <= 3)
                result = true;
        }
        break;
    case _dialogue_spatial_within_weapon_range:
        if (((1 << object->object.type) & 0x1003) && object->unit.actor_index != -1) {
            float range;
            if (ai_dialogue_actor_get_range(object->unit.actor_index, &range))
                result = distance_squared <= range * range;
        }
        break;
    }
    return result;
}


static __forceinline bool dialogue_flags_test(unsigned long flags, long bit) { return (flags & (1UL << bit)) != 0; }
static __forceinline long ai_dialogue_unit_get_dialogue_variant(long unit_index)
{
    long variant = 0;
    unit_datum *unit = (unit_datum *)dialogue_object_get_and_verify_type(unit_index, 0x1003);
    long dialogue_index = pattern_unit_get_dialogue(unit);
    if (dialogue_index != NONE)
        variant = ((dialogue_definition *)dialogue_tag_get('udlg', dialogue_index))->mission_dialogue_designator;
    return variant;
}
static __forceinline bool ai_dialogue_biped_is_stunned(long biped_index)
{
    bool stunned = false;
    unit_datum *biped = (unit_datum *)dialogue_object_get_and_verify_type(biped_index, 1);
    if (biped->object.type == 0)
        stunned = ((biped_datum *)biped)->biped.stun_ticks > 0;
    return stunned;
}
static __forceinline void ai_dialogue_unreachable() {}
static __forceinline float ai_dialogue_danger_level(short level)
{
    float result = 0.0f;
    s_game_globals *game_globals = dialogue_scenario_get_game_globals();
    if (game_globals && game_globals->ai_globals.count() > 0)
    {
        s_ai_globals_definition *ai_globals = (s_ai_globals_definition *)game_globals->ai_globals.address;
        switch (level)
        {
        case 0:
            break;
        case 1:
            result = ai_globals->danger_broadly_facing;
            break;
        case 2:
            result = ai_globals->danger_shooting_near;
            break;
        case 3:
            result = ai_globals->danger_shooting_at;
            break;
        case 4:
            result = ai_globals->danger_extremely_close;
            break;
        case 5:
            result = ai_globals->danger_shield_damage;
            break;
        case 6:
            result = ai_globals->danger_extended_shield_damage;
            break;
        case 7:
            result = ai_globals->danger_body_damage;
            break;
        case 8:
            result = ai_globals->danger_extended_body_damage;
            break;
        default:
            ai_dialogue_unreachable();
        }
    }
    return result;
}
static __forceinline short ai_dialogue_actor_get_attitude(long actor_index)
{
    short attitude = 0;
    style_definition *style = actor_get_task_style(actor_index);
    if (style)
        attitude = style->attitude;
    return attitude;
}
static __forceinline s_dialogue_character_voice *ai_dialogue_character_voice_get(long character_index)
{
    s_dialogue_character_voice *result = 0;
    while (character_index != NONE)
    {
        character_definition *definition = (character_definition *)dialogue_tag_get('char', character_index);
        if (definition->vocalization_properties.count > 0)
        {
            result = (s_dialogue_character_voice *)definition->vocalization_properties.address;
            break;
        }
        character_index = definition->parent_character.index;
    }
    return result;
}
static __forceinline bool ai_dialogue_prop_is_acknowledged(long prop_index)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
    return prop_ref->awareness_class >= 1 && prop_ref->awareness_class <= 2;
}
static __forceinline long ai_dialogue_prop_ref_get_object(long prop_index)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
    return datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index)->object_index;
}
static __forceinline bool ai_dialogue_actor_type_is_ally(actor_datum *actor)
{
    return ((1 << actor->type) & 0x780e00) != 0;
}
// @address 0x521488 ?dialogue_unit_can_vocalize@@YAFPAUai_dialogue_definitions@@PAVc_dialogue_event@@PAUvocalization_pattern@@JPAJPAM@Z
short dialogue_unit_can_vocalize(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    long unit_index, long *sound_definition_index_reference, float *vocalization_factor_reference)
{
    bool valid = true;
    unit_datum *unit = (unit_datum *)dialogue_object_get_and_verify_type(unit_index, 0x2003);
    float factor = 1.0f;
    actor_datum *actor = 0;
    short vocalization_index = pattern->vocalization_index;
    long sound_index;
    if (vocalization_index < 0 || vocalization_index >= definitions->vocalizations.s_tag_block::count)
    {
        vocalization_index = NONE;
        valid = false;
    }
    else
    {
        s_dialogue_vocalization *vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[vocalization_index];
        long game_time = game_time_get();
        long *team_state = (long *)team_dialogue_state_get((short)unit->unit.team);
        if (unit->unit.actor_index != NONE)
            actor = datum_get<actor_datum>(tls_actor_data(), unit->unit.actor_index);
        if (valid && actor)
            valid = ai_dialogue_actor_can_vocalize(unit->unit.actor_index, event, vocalization);
        if (dialogue_flags_test(unit->unit.unit_flags, 3))
            valid = false;
        if (unit->object.type == 0 && ai_dialogue_biped_is_stunned(unit_index))
            valid = false;
        if (unit->object.parent != NONE)
        {
            short seat_index = unit->unit.parent_seat_index;
            if (seat_index != NONE)
            {
                unit_datum *vehicle = dialogue_object_get_and_verify_type(unit->object.parent, 0x1003);
                unit_definition *definition = (unit_definition *)dialogue_tag_get('unit', vehicle->object.definition);
                if (seat_index >= 0 && seat_index < definition->seats.count())
                {
                    unit_seat *seat = &((unit_seat *)definition->seats.address)[seat_index];
                    if (dialogue_flags_test(seat->flags, 0) || dialogue_flags_test(seat->flags, 19))
                        valid = false;
                }
            }
        }
        if (valid && event->subtype != NONE)
        {
            unit_dialogue_vocalization_record *record = datum_get<unit_dialogue_vocalization_record>((s_data_array *)g_ai_dialogue_records, event->subtype);
            if (ai_dialogue_unit_get_dialogue_variant(unit_index) == record->dialogue_variant)
                valid = false;
        }
        if (valid)
        {
            if (actor)
            {
                long conditions = behavior_build_condition_flags(unit->unit.actor_index);
                if (((pattern->condition_flags | 0x3ff00000) & conditions) != conditions)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
                else if (actor->command_index != NONE)
                {
                    valid = datum_get<command_script_datum>(tls_command_script_data(), actor->command_index)->enable_dialogue;
                }
            }
            if (valid && (pattern->flags & 0x200) != 0 && unit->object.parent != NONE)
                valid = false;
            if (valid && actor && (pattern->flags & 1) != 0 && unit_index != event->unit_index)
            {
                bool acknowledged = false;
                if (event->unit_index != NONE)
                {
                    long prop_index = prop_ref_get_acknowledged_by_object_index(unit->unit.actor_index, event->unit_index);
                    if (prop_index != NONE)
                        acknowledged = ai_dialogue_prop_is_acknowledged(prop_index);
                }
                if (!acknowledged)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
            }
            if (valid && actor && (pattern->flags & 2) != 0 && unit_index != event->other_unit_index)
            {
                bool acknowledged = false;
                if (event->target_index != NONE)
                {
                    long prop_index = prop_ref_get_acknowledged_by_object_index(unit->unit.actor_index, event->target_index);
                    if (prop_index != NONE)
                        acknowledged = ai_dialogue_prop_is_acknowledged(prop_index);
                }
                if (!acknowledged)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
            }
            if (valid)
            {
                bool grouped = false;
                bool group_required = (pattern->flags & 4) != 0;
                if (game_is_campaign() && !game_team_is_enemy((e_game_team)unit->unit.team, _game_team_player))
                {
                    grouped = true;
                }
                else if (actor && actor->clump_index != NONE)
                {
                    clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), actor->clump_index);
                    if (clump->actor_count > 1)
                        grouped = true;
                }
                if (group_required && !grouped)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
                else if (!group_required && grouped)
                {
                    factor *= 0.6f;
                }
            }
            if (valid && actor && (pattern->flags & 8) != 0)
            {
                bool targeted = false;
                if (event->unit_index != NONE && actor->target_prop_index != NONE)
                    targeted = ai_dialogue_prop_ref_get_object(actor->target_prop_index) == event->unit_index;
                if (!targeted)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
                else
                {
                    factor *= 1.1f;
                }
            }
            if (valid && actor && (pattern->flags & 16) != 0)
            {
                bool targeted = false;
                if (event->other_unit_index != NONE && actor->target_prop_index != NONE)
                    targeted = ai_dialogue_prop_ref_get_object(actor->target_prop_index) == event->other_unit_index;
                if (!targeted)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
                else
                {
                    factor *= 1.1f;
                }
            }
            if (valid && (pattern->flags & 32) != 0)
            {
                if (!(event->target_unit && event->target_unit->unit.player_index != NONE)
                    && !(game_is_campaign() && !game_team_is_enemy((e_game_team)unit->unit.team, _game_team_player)))
                    valid = false;
            }
            if (valid && (pattern->flags & 128) != 0 && event->target_unit)
                valid = event->target_unit->object.parent == NONE;
            if (valid && (pattern->flags & 256) != 0 && event->unit)
                valid = event->unit->object.parent == NONE;
            if (valid && actor && pattern->executing_state != _dialogue_speaker_behavior_any)
            {
                valid = false;
                switch (pattern->executing_state)
                {
                case _dialogue_speaker_behavior_combat:
                    valid = (actor_executing_layer(unit->unit.actor_index, 13) != NONE);
                    break;
                case _dialogue_speaker_behavior_engage:
                    valid = (actor_executing_layer(unit->unit.actor_index, 25) != NONE) && !actor->suppress_combat;
                    break;
                case _dialogue_speaker_behavior_search:
                    valid = (actor_executing_layer(unit->unit.actor_index, 56) != NONE) || (actor_executing_layer(unit->unit.actor_index, 46) != NONE);
                    break;
                case _dialogue_speaker_behavior_cover:
                    valid = (actor_executing_layer(unit->unit.actor_index, 66) != NONE);
                    break;
                case _dialogue_speaker_behavior_retreat:
                    valid = (actor_executing_layer(unit->unit.actor_index, 82) != NONE) || (actor_executing_layer(unit->unit.actor_index, 84) != NONE);
                    break;
                case _dialogue_speaker_behavior_follow:
                    if (actor->squad_index != NONE)
                    {
                        s_task *task = squad_get_task(datum_get<squad_datum>(squad_data, actor->squad_index));
                        valid = task && task->follow_mode == 1;
                    }
                    break;
                case _dialogue_speaker_behavior_shoot:
                    if ((actor_executing_layer(unit->unit.actor_index, 25) != NONE) && !actor->suppress_combat
                        && (unsigned int)(actor->target_state - 2) <= 1)
                        valid = true;
                    break;
                case _dialogue_speaker_behavior_clump_idle:
                    if (actor->clump_index != NONE)
                        valid = datum_get<clump_datum>(tls_clump_data(), actor->clump_index)->combat_state == 0;
                    break;
                case _dialogue_speaker_behavior_clump_combat:
                    if (actor->clump_index != NONE)
                        valid = datum_get<clump_datum>(tls_clump_data(), actor->clump_index)->combat_state >= 2;
                    break;
                case _dialogue_speaker_behavior_fought_brutes:
                    if (actor->clump_index != NONE)
                        valid = ((short)datum_get<clump_datum>(tls_clump_data(), actor->clump_index)->death_flags & 1) != 0;
                    break;
                case _dialogue_speaker_behavior_fought_flood:
                    if (actor->clump_index != NONE)
                        valid = ((short)datum_get<clump_datum>(tls_clump_data(), actor->clump_index)->death_flags & 2) != 0;
                    break;
                default:
                    valid = true;
                    break;
                }
            }
            if (valid && (pattern->flags & 64) != 0)
            {
                valid = false;
                if (game_is_campaign() && event->target_unit && event->target_unit->unit.player_index != NONE)
                {
                    if (unit->unit.team == 1)
                    {
                        valid = true;
                    }
                    else if (!game_team_is_enemy((e_game_team)unit->unit.team, _game_team_player))
                    {
                        long character = game_get_player_character_type(event->target_unit->unit.player_index);
                        switch (unit->unit.team)
                        {
                        case 2:
                            valid = character == 0;
                            break;
                        case 3:
                        case 7:
                            valid = character == 1 || character == 4;
                            break;
                        default:
                            valid = false;
                            break;
                        }
                    }
                }
            }
            if (valid && pattern->danger_level > 0)
            {
                if (actor->danger < ai_dialogue_danger_level(pattern->danger_level))
                {
                    vocalization_index = NONE;
                    valid = false;
                }
            }
            if (valid)
            {
                if (actor)
                {
                    short attitude = ai_dialogue_actor_get_attitude(unit->unit.actor_index);
                    if (pattern->attitude != attitude)
                    {
                        if (pattern->attitude == 0)
                        {
                            factor *= 0.6f;
                        }
                        else
                        {
                            vocalization_index = NONE;
                            valid = false;
                        }
                    }
                }
                else if (pattern->attitude != 0)
                {
                    vocalization_index = NONE;
                    valid = false;
                }
            }
            if (actor && actor->combat_status > vocalization->maximum_combat_status)
            {
                vocalization_index = NONE;
                valid = false;
            }
            if (valid && pattern->event_unit_relation != _dialogue_spatial_none)
            {
                if (event->unit_index != NONE)
                {
                    valid = pattern_match_spatial_relation(unit_index, event->unit_index, pattern->event_unit_relation);
                    if (valid)
                        factor *= 1.5f;
                }
                else
                {
                    valid = false;
                }
            }
            if (valid && pattern->event_target_relation != _dialogue_spatial_none)
            {
                if (event->target_index != NONE)
                {
                    valid = pattern_match_spatial_relation(unit_index, event->target_index, pattern->event_target_relation);
                    if (valid)
                        factor *= 1.5f;
                }
                else
                {
                    valid = false;
                }
            }
            if (valid)
                valid = pattern_match_object_type(pattern->speaker_object_type, unit_index, 0);
            if (valid)
                factor *= ai_dialogue_queue_unit_vocalization_weight(unit_index, vocalization_index);
        }
        if (valid)
        {
            if (unit->unit.player_index != NONE)
            {
                vocalization_index = NONE;
                valid = false;
            }
            else
            {
                bool player_involved = false;
                bool ally_involved = false;
                float danger = 0.0f;
                if ((event->unit && event->unit->unit.player_index != NONE)
                    || (event->target_unit && event->target_unit->unit.player_index != NONE))
                {
                    player_involved = true;
                }
                else if ((event->speaker && ai_dialogue_actor_type_is_ally(event->speaker))
                    || (event->target_speaker && ai_dialogue_actor_type_is_ally(event->target_speaker)))
                {
                    ally_involved = true;
                }
                if (actor)
                {
                    s_dialogue_character_voice *voice = ai_dialogue_character_voice_get(actor->character_index);
                    if (voice)
                        danger = voice->fraction;
                }
                float best_factor = 0.0f;
                long best_sound_index = NONE;
                long best_vocalization_index = NONE;
                valid = false;
                while (vocalization_index != NONE && vocalization)
                {
                    float skip_fraction;
                    if (player_involved)
                        skip_fraction = vocalization->player_skip_fraction;
                    else if (ally_involved)
                        skip_fraction = vocalization->ally_skip_fraction;
                    else
                        skip_fraction = vocalization->skip_fraction;
                    bool skip = false;
                    if (vocalization->priority < 11 && danger > 0.0f)
                        skip_fraction = 1.0f - (1.0f - skip_fraction) * (1.0f - danger);
                    if (_real_random_range(tls_random_seed(), 0.0f, 1.0f) < skip_fraction)
                        skip = true;
                    sound_index = NONE;
                    if (!skip && unit_can_vocalize(unit_index, vocalization_index, &sound_index)
                        && team_state[vocalization_index] <= game_time)
                    {
                        if (danger > 0.0f)
                            factor = (1.0f - danger) * factor;
                        best_factor = factor;
                        best_sound_index = sound_index;
                        best_vocalization_index = vocalization_index;
                        valid = true;
                        break;
                    }
                    factor *= ai_dialogue_vocalization_fallback_scale;
                    vocalization_index = vocalization->parent_index;
                    if (vocalization_index != NONE)
                        vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[vocalization_index];
                }
                if (valid)
                {
                    factor = best_factor;
                    sound_index = best_sound_index;
                    vocalization_index = (short)best_vocalization_index;
                }
            }
        }
    }
    if (!valid)
    {
        *vocalization_factor_reference = 0.0f;
        *sound_definition_index_reference = NONE;
    }
    else
    {
        *vocalization_factor_reference = factor;
        *sound_definition_index_reference = sound_index;
    }
    return vocalization_index;
}


// @address 0x520248 ?dialogue_reply_event@@YA_NJJJPAVc_dialogue_option@@PAJ@Z
bool dialogue_reply_event(long unit_index, long object_index, long reply_record_index, c_dialogue_option *, long *)
{
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    s_data_array *record_data = (s_data_array *)g_ai_dialogue_records;
    unit_dialogue_vocalization_record *record = datum_get<unit_dialogue_vocalization_record>(record_data, reply_record_index);
    void *object = object_try_and_get_and_verify_type(object_index, -1);
    short vocalization_index = record->vocalization;
    short option_count = 0;
    c_dialogue_option options[100];
    bool result = false;
    while (vocalization_index != NONE)
    {
        s_dialogue_vocalization *chain = &((s_dialogue_vocalization *)definitions->vocalizations.address)[vocalization_index];
        if (chain->reply_count > 0)
        {
            c_dialogue_event event(-1, unit_index, object_index, reply_record_index, -1, 0);
            for (short i = 0; i < chain->reply_count; i++)
            {
                if (option_count >= 100)
                    break;
                s_dialogue_reply_option *reply = &chain->replies[i];
                s_reply_vocalization_pattern pattern;
                unsigned short entry_vocalization = reply->vocalization_index;
                if (entry_vocalization != 0xFFFF)
                {
                    pattern.pad0 = -1;
                    pattern.vocalization_index = entry_vocalization;
                    pattern.speaker_type = _dialogue_speaker_unit;
                    pattern.relation = 0;
                    pattern.reply_data = reply->data;
                    pattern.flags = 0;
                    pattern.target_team = 0;
                    pattern.target_object_type = 0;
                    pattern.target_ai_type_name = 0;
                    pattern.executing_state = _dialogue_speaker_behavior_any;
                    pattern.danger_level = -1;
                    pattern.attitude = 0;
                    pattern.speaker_object_type = 0;
                    pattern.padding1a = 0;
                    pattern.event_unit_relation = _dialogue_spatial_none;
                    pattern.event_target_relation = _dialogue_spatial_none;
                    pattern.condition_flags = -1;
                    pattern.spatial_relation = _dialogue_spatial_none;
                    pattern.vocalization_filter = 0;
                    pattern.speaker_team = 0;
                    pattern.unit_object_type = 0;
                    pattern.unit_ai_type_name = 0;
                    switch (reply->speaker_type)
                    {
                    case 0:
                        pattern.speaker_type = _dialogue_speaker_nearby_non_enemy;
                        break;
                    case 1:
                        pattern.speaker_type = _dialogue_speaker_nearby_enemy;
                        pattern.event_unit_relation = _dialogue_spatial_within_2_5m;
                        break;
                    case 2:
                        pattern.speaker_type = _dialogue_speaker_other_unit;
                        break;
                    case 3:
                        pattern.speaker_type = _dialogue_speaker_joint_member;
                        break;
                    case 4:
                        pattern.speaker_type = _dialogue_speaker_nearby_non_enemy_same_type;
                        break;
                    case 5:
                        pattern.speaker_type = _dialogue_speaker_squad_leader;
                        break;
                    case 6:
                        pattern.flags = 512;
                        pattern.speaker_type = _dialogue_speaker_nearby_non_enemy;
                        break;
                    default:
                        break;
                    }
                    pattern.listener_type = _dialogue_listener_unit;
                    ai_dialogue_event_evaluate_pattern(definitions, &event, (vocalization_pattern *)&pattern, options, 100, &option_count);
                }
                else if ((reply->flags & 2) != 0)
                {
                    unsigned short vocalization = reply->reply_vocalization;
                    if (vocalization != 0xFFFF && object != 0
                        && (((1 << ((unit_datum *)object )->object.type) & 0x1003) != 0))
                    {
                        c_dialogue_event reply_event((short)vocalization, object_index, unit_index, reply_record_index, -1, 0);
                        ai_dialogue_event_internal(definitions, &reply_event, options, 100, &option_count);
                    }
                }
            }
        }
        if (option_count > 0)
            break;
        vocalization_index = chain->parent_index;
    }
    short chosen = dialogue_choose_option(options, option_count);
    if (chosen != -1)
    {
        __int64 time = tls_game_time_globals()->ticks_per_second;
        long param = (long)((float)(double)time * 0.2f + 0.5f) + record->duration + record->start_time;
        long queue_index;
        if (dialogue_speak(definitions, param, &options[chosen], &queue_index))
            result = true;
    }
    return result;
}

static __forceinline long ai_dialogue_player_get_vehicle(long unit_index)
{
    s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
    long result;
    const long &index = unit_index & 0xffff;
    long parent = ((unit_datum *)(*datum_get<s_object_header>(headers, index)).datum )->object.parent;
    if (parent == NONE)
    {
        result = NONE;
    }
    else
    {
        do
        {
            unit_datum *object = (unit_datum *)datum_get<s_object_header>(headers, parent)->datum;
            if ((_rotl(object->object.flags, 6) & 1) == 0)
                break;
            parent = object->object.parent;
        } while (parent != NONE);
        result = parent;
    }
    return result;
}


static __forceinline void ai_dialogue_player_get_aiming_vector(long unit_index, real_vector3d *vector)
{
    *vector = ((unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)unit_index].datum )->unit.aiming_vector;
}

static __forceinline long ai_dialogue_player_vehicle_index(const actor_datum *actor)
{
    const long &index = +actor->root_vehicle_index;
    return index;
}

// @address 0x522430 ?dialogue_player_weighting@@YAMJ@Z
float dialogue_player_weighting(long unit_index)
{
    float weight = 0.0f;
    unit_datum *unit = dialogue_object_get_and_verify_type(unit_index, 0x2003);
    actor_datum *actor = 0;
    bool cached = false;
    if (unit->unit.actor_index != NONE)
    {
        actor = datum_get<actor_datum>(tls_actor_data(), unit->unit.actor_index);
        if (actor->player_weight_valid)
        {
            weight = actor->player_weight;
            cached = true;
        }
    }
    if (!cached && actor)
    {
        float maximum_weight = 0.0f;
        bool found_player = false;
        c_data_iterator<player_datum> players;
        for (players.begin(player_data); players.next();)
        {
            player_datum *player = players.get_datum();
            if (player->unit_index != NONE)
            {
                long vehicle_index = ai_dialogue_player_get_vehicle(player->unit_index);
                weight = 0.0f;
                found_player = true;
                real_point3d player_position;
                object_get_origin(player->unit_index, &player_position);
                float distance_squared = distance_squared3d(&actor->position.body, &player_position);
                if (vehicle_index != NONE && vehicle_index == ai_dialogue_player_vehicle_index(actor))
                {
                    weight = _real_random_range(tls_random_seed(), 0.98f, 1.0f);
                }
                else if (distance_squared > 20.0f * 20.0f)
                {
                    weight = 0.0f;
                }
                else
                {
                    real_vector3d aiming_vector;
                    real_vector3d direction;
                    ai_dialogue_player_get_aiming_vector(player->unit_index, &aiming_vector);
                    direction.i = actor->position.body.x - player_position.x;
                    direction.j = actor->position.body.y - player_position.y;
                    direction.k = actor->position.body.z - player_position.z;
                    if (normalize3d(&direction) > 0.0f)
                    {
                        weight = 1.0f - distance_squared / (20.0f * 20.0f);
                        float dot = dot_product3d(&aiming_vector, &direction);
                        if (dot < 0.0f)
                            weight = (PIN(dot + 1.0f, 0.0f, 1.0f) + 1.0f) * weight * 0.5f;
                        long prop_ref_index = prop_ref_get_by_object_index(unit->unit.actor_index, player->unit_index, false, _clump_importance_none);
                        if (prop_ref_index != NONE)
                        {
                            prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
                            if (prop_ref->awareness < 1)
                                weight *= 0.5f;
                        }
                    }
                    weight = 0.7f + 0.3f * weight;
                }
            }
            if (weight > maximum_weight)
                maximum_weight = weight;
        }
        if (!found_player)
            maximum_weight = 1.0f;
        actor->player_weight = maximum_weight;
        actor->player_weight_valid = true;
        weight = maximum_weight;
    }
    return weight;
}


// @address 0x520d98 ai_dialogue_notify_vocalization
extern "C" void ai_dialogue_notify_vocalization(long unit_index, short vocalization_index, long record_index, char *information)
{
    ai_information_data data = {0};
    short clump_count = 1;
    unit_datum *unit = dialogue_object_get_and_verify_type(unit_index, 0x1003);
    ai_dialogue_definitions *definitions = dialogue_globals_get_inline();
    s_dialogue_vocalization *vocalization = 0;
    if (vocalization_index != NONE && definitions)
        vocalization = &((s_dialogue_vocalization *)definitions->vocalizations.address)[vocalization_index];
    ai_information_packet *packet = (ai_information_packet *)information;
    if (record_index != NONE)
    {
        unit_dialogue_vocalization_record *record = datum_get<unit_dialogue_vocalization_record>((s_data_array *)g_ai_dialogue_records, record_index);
        bool *notified = &record->interrupted;
        *notified = true;
        dialogue_reply_event(unit_index, record->information.subject, record_index, 0, 0);
    }
    data.values[0] = 0;
    if (packet->payload.values[0] == 0)
    {
        if (vocalization)
        {
            switch (vocalization->sound_source_type)
            {
            case 1:
                data.values[0] = 3;
                data.values[2] = unit_index;
                clump_count = 2;
                break;
            case 2:
                if (packet->listener_object_index != NONE)
                {
                    bool enemy = true;
                    unit_datum *target = (unit_datum *)object_try_and_get_and_verify_type(packet->listener_object_index, -1);
                    if (target)
                    {
                        if (((1 << target->object.type) & 0x1003) && target->unit.actor_index != NONE &&
                            !game_team_is_enemy((e_game_team)unit->unit.team, (e_game_team)target->unit.team))
                            enemy = false;
                        if (enemy)
                        {
                            data.values[0] = 3;
                            data.values[2] = packet->listener_object_index;
                        }
                    }
                }
                break;
            }
        }
    }
    else
    {
        data = packet->payload;
    }
    if (data.values[0] != 0)
    {
        ai_sound_data sound;
        sound.type = 0;
        sound.volume = 1;
        sound.information_data = data;
        handle_sound(unit->unit.actor_index, unit_index, clump_count, true, &sound);
    }
    if (packet->listener_object_index != NONE && packet->listener_object_index != unit_index)
    {
        unit_datum *target = (unit_datum *)object_try_and_get_and_verify_type(packet->listener_object_index, -1);
        if (target && ((1 << target->object.type) & 0x1003))
        {
            if (target->unit.actor_index != NONE)
            {
                actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), target->unit.actor_index);
                unit_dialogue_vocalization_record *record = 0;
                float seconds = 1.5f;
                if (record_index != NONE)
                    record = datum_get<unit_dialogue_vocalization_record>((s_data_array *)g_ai_dialogue_records, record_index);
                if (record)
                    seconds = ai_dialogue_ticks_to_seconds(record->duration) + 1.0f;
                if (actor->attention_level <= 1)
                {
                    actor->attention_ticks = ai_dialogue_seconds_to_ticks_round(seconds);
                    actor->attention_direction.to_object(unit_index);
                    actor->attention_level = 1;
                }
                if (vocalization && record && vocalization->listener_emotion > 0)
                    dialogue_emotion(target->unit.actor_index, vocalization->listener_emotion,
                        (short)(tls_game_time_globals()->ticks_per_second + record->duration));
            }
        }
    }
}

extern "C" {
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_team_dialogue_state_allocator> g_team_dialogue_state_layout;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_vocalization_record_allocator> g_ai_dialogue_records_allocator;
}
