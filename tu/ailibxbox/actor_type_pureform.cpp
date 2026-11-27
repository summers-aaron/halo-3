// ailibxbox/actor_type_pureform.obj: from the records 0xc8898 0xcb910 0xd0a78 0x50b430 0x50b490 0x50b5c8 0x50b640 0x50b6e8 0x50b800 0x50b920 0x50c148 0x50c2b8 0x50c3b0 0x50c430 0x50c7a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "memory/data.h"
#include "ai/objective_definitions.h"
#include "ai/squad_data.h"
#include "ai/character_morph_properties.h"
#include "scenario/scenario.h"
#include "math/real_math_types.h"
#include "math/point_math.h"
#include "ai/actor_firing_position.h"
#include "math/real_math.h"
#include "ai/sector_ref.h"
#include "units/action_request.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "objects/object_placement.h"
#include "ai/clump_datum.h"
#include "ai/actor_datum.h"
#include "ai/squad_types.h"
#include "ai/actors.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/actor_type_pureform.h"
#include "ai/actor_type_flood_types.h"
#include "ai/path_types.h"
#include "ai/ai.h"
#include "ai/ai_script.h"
#include "ai/behavior.h"
#include "ai/actor_behavior_state.h"
#include "ai/sector.h"
#include "ai/squads.h"
#include "objects/widgets/muffins.h"
#include "units/unit_dialogue.h"
#include "ai/ai_actions.h"
#include "ai/character_definitions.h"
#include "ai/prop_ref_datum.h"
#include "units/unit_types.h"
#include "units/biped_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "ai/firing_position_definition.h"
#include "objects/object_datum.h"
#include "ai/Zone.h"
#include "ai/zone_access.h"
#include "ai/firing_position_ref.h"
#include "ai/firing_position_ref_inline.h"
#include "memory/data_types.h"
#include "ai/pathfinding_sector_types.h"
#include "memory/tls_globals.h"
#include "ai/actor_globals.h"
#include "ai/squad_iterator_types.h"
#include "game/game_globals.h"
// match: 0x50b920: precise rounding keeps game_seconds_to_ticks_round as fmuls+fadds, not fmadds
#pragma float_control(precise, on, push)
#include "game/game_time_inline.h"
// match: 0x50b920: precise rounding keeps game_seconds_to_ticks_round as fmuls+fadds, not fmadds
#pragma float_control(pop)
#include "ai/ai_globals_definition.h"
#include "ai/pureform_types.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "ai/actor_type_pureform_globals.h"
#include "math/real_math_constants.h"
#include "ai/clump.h"
#include "ai/clump_iterator.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_tag.h"
#include "ai/pathfinding_data.h"
#include "ai/path_state.h"

struct s_pureform_distribution {
    short counts[3];
};
// @address 0x50b430 ?actor_set_pending_form_impl@@YA_NJW4e_pureform@@_NW4e_morph_priority@@1@Z
// @address 0x50b490 ?actor_morph_impl@@YA_NJW4e_pureform@@_N1@Z
// @address 0x50b640 ?actor_morph_inhibited@@YA_NJW4e_pureform@@@Z
// @address 0x50b5c8 ?character_get_form@@YA?AW4e_pureform@@J@Z
// @address 0x50c3b0 ?pureform_default_form_get_impulse@@YAFJPAVc_behavior_state@@@Z
// @address 0x50c2b8 ?pureform_arrival_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
// @address 0x50c7a8 ?ai_scripting_morph@@YA_NJF@Z
// @address 0x50b800 ?actor_is_morphing@@YA_NJPAW4e_pureform@@0PA_N@Z

// @address 0xedff8 ?action_submit@@YA_NJPBUaction_request@@@Z
// @address 0x518ed8 ?character_definition_morph_properties_get@@YAPAUcharacter_morph_properties@@J@Z
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z


/* Tests the squad task inhibition bit for an actor pureform. */


bool actor_set_pending_form_impl(long actor_index, e_pureform form, bool force, e_morph_priority priority, bool immediate)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->morph.pending_form_priority <= priority) {
        actor->morph.pending_form = (short)form;
        actor->morph.pending_form_priority = (short)priority;
        actor->morph.immediate_morph = immediate;
        actor->morph.force_morph = force;
        if (immediate) {
            actor->morph.current_form = (short)form;
        }
        result = true;
    }
    return result;
}

// 0x50b490

static __forceinline e_pureform actor_type_pureform_get_current_form(long actor_index)
{
    long form = -1;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (((1 << actor->type) & 0x700000) != 0)
        form = character_get_form(actor->character_index);
    return (e_pureform)form;
}

static __forceinline void actor_type_pureform_clear_pending_form(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->morph.pending_form = -1;
    actor->morph.pending_form_priority = -1;
    actor->morph.immediate_morph = false;
    actor->morph.force_morph = false;
}

static __forceinline void actor_type_pureform_action_new(action_request *request, long action)
{
    memset(request, 0, sizeof(*request));
    request->action = action;
}

bool actor_morph_impl(long actor_index, e_pureform pureform, bool force, bool immediate)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    e_pureform current_form = actor_type_pureform_get_current_form(actor_index);
    bool result = false;
    if (current_form != pureform) {
        if (properties && properties->forms[pureform].index != actor->character_index) {
            action_request request;
            actor_type_pureform_action_new(&request, 8);
            request.morph.character = properties->forms[pureform].index;
            request.morph.time = properties->morph_time;
            request.morph.form = (char)pureform;
            request.morph.force = force;
            request.morph.immediate = immediate;
            request.morph.unknown = false;
            result = action_submit(actor->unit_index, &request);
        }
    } else {
        if (immediate) {
            if (actor->morph.immediate_morph)
                actor->morph.morph_locked = true;
            actor_type_pureform_clear_pending_form(actor_index);
        }
        result = true;
    }
    return result;
}

// 0x50b5c8
e_pureform character_get_form(long tag_index)
{
    long result = -1;
    s_tag_reference *entries = (s_tag_reference *)character_definition_morph_properties_get(tag_index);
    if (entries) {
        for (short i = 0; i < 3; i++) {
            if (entries[i].index == tag_index) {
                result = i;
                break;
            }
        }
    }
    return (e_pureform)result;
}

bool actor_morph_inhibited(long actor_index, e_pureform pureform)
{
    bool result = 0;
    long squad_index = datum_get<actor_datum>(actor_data, actor_index)->squad_index;
    if (squad_index != -1) {
        s_task *task_flags = ::squad_get_task(
            datum_get<squad_datum>(squad_data, squad_index));
        if (task_flags)
            result = ((1 << ((short)pureform + 6)) & task_flags->vehicle_inhibit_flags) != 0;
    }
    return result;
}

// 0x50c3b0
short pureform_default_form_get_impulse(long actor_index, c_behavior_state *behavior_state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->situation.best_prop_class <= 3 && actor->morph.morph_locked == 0) {
        if (actor->morph.pending_form != actor->morph.current_form) {
            actor_set_pending_form_impl(actor_index, (e_pureform)actor->morph.current_form, false, _morph_priority_none, true);
        }
    }
    return -1;
}

// @address 0xc8898 ?actor_get_current_form@@YA?AW4e_pureform@@J@Z
enum e_pureform actor_get_current_form(long actor_index)
{
  actor_datum *actor;
  int result;
  result = -1;
  actor = datum_get<actor_datum>(actor_data, actor_index);
  if ((int)((1 << ((int)actor->type) & 0x700000U)) != 0) {
    result = character_get_form(actor->character_index);
  }
  return (e_pureform)result;
}

// @address 0xcb910 actor_pureform_get_firing_point_selection_form
extern "C" int actor_pureform_get_firing_point_selection_form(int actor_index)
{
    int result = -1;
    actor_datum *player = datum_get<actor_datum>(actor_data, actor_index);
    if (((1 << player->type) & 0x700000) != 0) {
        short index = player->morph.pending_form;
        if (index != -1) {
            if (player->morph.immediate_morph)
                result = index;
        } else if (player->morph.morph_locked) {
            result = ((int)::actor_get_current_form(actor_index));
        }
    }
    return result;
}

// @address 0x3aa90 k_real_math_origin3d
inline long pureform_motor_program(long unit_index)
{
    unit_datum *object = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)unit_index].object;
    return ((c_motor_program_state *)((char *)object + object->unit.motor_offset))->program;
}
// @address 0x50b6e8 ?actor_allowed_to_morph@@YA_NJ_NW4e_pureform@@@Z
bool actor_allowed_to_morph(long actor_index, bool force, enum e_pureform _sym_p2)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (g_morph_disallowed || actor->morph.morph_delay_ticks > 0)
        return false;
    long unit_index = actor->unit_index;
    if (unit_index == -1 || action_executing(unit_index, _action_morph) || (unsigned short)actor->morph.pending_form != 0xffff)
        return false;
    bool result = !actor_morph_inhibited(actor_index, (e_pureform)((long)_sym_p2));
    if (result) {
        long motor_program = pureform_motor_program(unit_index);
        if (motor_program == 7) {
            if (((int)::actor_get_current_form(actor_index)) == 2 && ((long)_sym_p2) == 0)
                result = true;
            else if (((int)::actor_get_current_form(actor_index)) == 0 && ((long)_sym_p2) == 2)
                result = true;
            else result = false;
        } else if (motor_program != 0)
            result = false;
    }
    return result;
}

bool actor_is_morphing(long actor_index, e_pureform *pureform, e_pureform *pureform2, bool *immediate)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (pureform) {
        long form = -1;
        if (((1 << actor->type) & 0x700000) != 0)
            form = character_get_form(actor->character_index);
        *pureform = (e_pureform)form;
    }
    if (pureform2)
        *pureform2 = _pureform_none;
    if (immediate)
        *immediate = false;
    long unit_index = actor->unit_index;
    if (unit_index != -1 && (action_executing(unit_index, _action_morph) || action_executing(unit_index, _action_hostile_morph))) {
        unit_datum *object = (unit_datum *)datum_get<s_object_header>(object_header_data, unit_index)->object;
        struct s_pureform_morph_action_state
        {
            unsigned char action_state[0x54];
            long character_index;
            unsigned char immediate;
        };
        s_pureform_morph_action_state *motor = (s_pureform_morph_action_state *)((char *)object + object->unit.action_offset);
        long tag = motor->character_index;
        if (tag != -1 && pureform2)
            *pureform2 = character_get_form(tag);
        if (immediate)
            *(unsigned char *)immediate = motor->immediate;
        result = true;
    }
    return result;
}

static __forceinline actor_datum *actor_type_pureform_clump_actor_iterator_next(clump_actor_iterator *iterator)
{
    actor_datum *result = 0;
    if (iterator->next_actor_index != -1) {
        result = datum_get<actor_datum>(actor_data, iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = result->next_clump_actor_index;
    }
    return result;
}

// @address 0x50c148 ?group_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
short group_morph_get_impulse(long actor_index, c_behavior_state *behavior_state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    if (properties && actor->clump_index != -1) {
        short current_form = (short)((int)::actor_get_current_form(actor_index));
        clump_actor_iterator iterator;
        clump_actor_iterator_new(&iterator, actor->clump_index);
        short result = -1;
        actor_datum *other;
        while ((other = actor_type_pureform_clump_actor_iterator_next(&iterator)) != 0) {
            if (distance3d(&actor->position.body, &other->position.body) >= properties->group_morph_radius)
                continue;
            e_pureform pureform = _pureform_none;
            e_pureform pureform2 = _pureform_none;
            bool immediate = false;
            if (actor_is_morphing(iterator.actor_index, &pureform, &pureform2, &immediate)
                && immediate
                && pureform2 != -1
                && pureform2 != (e_pureform)current_form
                && pureform2 != _pureform_stalker
                && ::actor_allowed_to_morph(actor_index, false, (e_pureform)(long)pureform2)) {
                result = (short)pureform2;
                break;
            }
        }
        if (result != -1)
            actor_morph_impl(actor_index, (e_pureform)result, true, false);
    }
    return -1;
}

// @address 0x3559c8 ?motor_animation_busy@@YA_NJ@Z
short pureform_arrival_morph_get_impulse(long actor_index, c_behavior_state *behavior_state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    unsigned short pending_form = (unsigned short)actor->morph.pending_form;
    if (pending_form != 0xffff && actor->morph.pending_form_priority == 1) {
        long unit_index = actor->unit_index;
        if (!motor_animation_busy(unit_index) && actor->search_active != 0) {
            if ((short)pending_form != ((int)::actor_get_current_form(actor_index))) {
                if (!(pureform_motor_program(unit_index) == 7 && (short)pending_form == 1)) {
                    long tag = actor->character_index;
                    character_morph_properties *properties = character_definition_morph_properties_get(tag);
                    if (properties && properties->forms[(short)pending_form].index != tag)
                        actor_morph_impl(actor_index, (e_pureform)(short)pending_form, false, true);
                }
            }
        }
    }
    return -1;
}

// @address 0xd4f60 ?ai_index_actor_iterator_new@@YAXJPAUai_index_actor_iterator@@@Z
// @address 0xd5040 ?ai_index_actor_iterator_next@@YAPAUactor_datum@@PAUai_index_actor_iterator@@@Z
bool ai_scripting_morph(long actor_index, short form)
{
    bool result = false;
    if (form >= 0 && form < 3) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(actor_index, &iterator);
        actor_datum *actor;
        while ((actor = (actor_datum *)ai_index_actor_iterator_next(&iterator)) != 0) {
            character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
            long iterated_actor_index = iterator.actor_index;
            e_pureform current_form = actor_type_pureform_get_current_form(iterated_actor_index);
            if (properties && current_form != -1)
                result |= actor_morph_impl(iterated_actor_index, (e_pureform)form, false, false);
        }
    }
    return result;
}













extern "C" {
// @address 0x7c5f54 k_pureform_distance_preferences
actor_type_pureform_preference k_pureform_distance_preferences[3] = {
    { 5.0f, { _pureform_tank, _pureform_stalker, _pureform_ranged } },
    { 10.0f, { _pureform_stalker, _pureform_tank, _pureform_ranged } },
    { FLT_MAX, { _pureform_ranged, _pureform_stalker, _pureform_tank } },
};
}

// @address 0x8c494b g_morph_disallowed
bool g_morph_disallowed;

static __forceinline bool actor_type_pureform_set_pending_form_impl(
    long actor_index, e_pureform form, bool force, e_morph_priority priority, bool immediate)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->morph.pending_form_priority <= priority) {
        actor->morph.pending_form = (short)form;
        actor->morph.pending_form_priority = (short)priority;
        actor->morph.immediate_morph = immediate;
        actor->morph.force_morph = force;
        if (immediate) {
            actor->morph.current_form = (short)form;
        }
        result = true;
    }
    return result;
}

static __forceinline void actor_type_pureform_squad_actor_iterator_new(squad_actor_iterator *iterator, long squad_index, bool active_only)
{
    if (ai_globals->enabled) {
        iterator->squad_index = squad_index;
        iterator->index = -1;
        iterator->active_only = active_only;
        if (squad_index == -1) {
            iterator->next_index = ai_globals->first_actor_index;
        } else {
            squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
            iterator->next_index = squad->first_actor_index;
        }
    }
}

static __forceinline float actor_type_pureform_redistribution_distance3d(const real_point3d *a, const real_point3d *b)
{
    real_vector3d v;
    vector_from_points3d(a, b, &v);
    return sqrtf(v.i * v.i + (v.j * v.j + v.k * v.k));
}

// @address 0x50b920 ?squad_handle_morph_redistribution@@YAXJPAUs_pureform_distribution@@PAUs_tag_block@@@Z
void squad_handle_morph_redistribution(long squad_index, s_pureform_distribution *distribution, s_tag_block *area_set)
{
    short pureform_count = 0;
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    s_task *task = ::squad_get_task(squad);

    if (task != 0) {
        if (TEST_BIT(task->vehicle_inhibit_flags, 8) && TEST_BIT(task->vehicle_inhibit_flags, 6) && TEST_BIT(task->vehicle_inhibit_flags, 7))
            return;

        squad_actor_iterator iterator;
        squad_actor_iterator_new(&iterator, squad_index, true);
        actor_datum *actor;
        while ((actor = (actor_datum *)::squad_actor_iterator_next(&iterator)) != 0) {
            if (((1 << actor->type) & 0x700000) == 0)
                continue;

            pureform_count++;
            short current_form = (short)actor_type_pureform_get_current_form(iterator.index);
            if (TEST_BIT(task->vehicle_inhibit_flags, current_form + 6)) {
                float distance = 0.0f;
                if (actor->target_prop_index != NONE)
                    distance = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index)->distance;

                actor_type_pureform_clear_pending_form(iterator.index);
                for (short range = 0; range < 3; range++) {
                    if (distance < k_pureform_distance_preferences[range].maximum_distance) {
                        for (short i = 0; i < 3; i++) {
                            if (!TEST_BIT(task->vehicle_inhibit_flags, (short)k_pureform_distance_preferences[range].forms[i] + 6)) {
                                actor_type_pureform_set_pending_form_impl(
                                    iterator.index, k_pureform_distance_preferences[range].forms[i], false, (e_morph_priority)1, true);
                                break;
                            }
                        }
                        break;
                    }
                }
            } else if (actor->morph.pending_form != NONE && TEST_BIT(task->vehicle_inhibit_flags, actor->morph.pending_form + 6)) {
                actor_type_pureform_clear_pending_form(iterator.index);
            }
        }
    }

    if (distribution != 0) {
        short counts[3];
        short desired_counts[3] = {
            distribution->counts[0],
            distribution->counts[1],
            distribution->counts[2],
        };
        real_point3d centers[3];

        for (short i = 0; i < 3; i++) {
            centers[i] = k_real_math_origin3d;
            counts[i] = 0;
        }

        for (short i = 0; i < area_set->count; i++) {
            squads_area_zone_set_entry *reference = TAG_BLOCK_GET_ELEMENT(area_set, i, squads_area_zone_set_entry);
            if (reference->zone_index != NONE && reference->area_index != NONE) {
                zone_definition *zone = zone_definition_get((unsigned short)reference->zone_index);
                area_definition *area = TAG_BLOCK_GET_ELEMENT(&zone->areas, reference->area_index, area_definition);
                if (area->firing_position_count > 0) {
                    for (short form = 0; form < 3; form++) {
                        if (reference->firing_positions == 0 || TEST_BIT(reference->firing_positions, form)) {
                            add_points3d(&centers[form], &area->point.point, &centers[form]);
                            counts[form]++;
                        }
                    }
                }
            }
        }

        for (short form = 0; form < 3; form++) {
            if (counts[form] > 0) {
                scale_point3d(&centers[form], 1.0f / (float)counts[form], &centers[form]);
            }
        }

        bool redistributed = true;
        while (redistributed) {
            redistributed = false;
            for (short form = 0; form < 3; form++) {
                if (desired_counts[form] > 0) {
                    long best_actor_index = NONE;
                    float best_score = 0.0f;
                    squad_actor_iterator iterator;
                    actor_type_pureform_squad_actor_iterator_new(&iterator, squad_index, true);
                    redistributed = true;
                    actor_datum *actor;
                    while ((actor = (actor_datum *)::squad_actor_iterator_next(&iterator)) != 0) {
                        if (((1 << actor->type) & 0x700000) != 0 && actor->morph.pending_form_priority < 1) {
                            float score = 0.0f;
                            if (counts[form] > 0) {
                                float distance = actor_type_pureform_redistribution_distance3d(&actor->position.body, &centers[form]);
                                score = 10.0f / (1.0f + 0.6f * distance);
                            }
                            if (actor_type_pureform_get_current_form(iterator.index) == (e_pureform)form)
                                score += 5.0f;
                            if (score > best_score) {
                                best_actor_index = iterator.index;
                                best_score = score;
                            }
                        }
                    }
                    if (best_actor_index != NONE) {
                        actor_datum *best_actor = datum_get<actor_datum>(actor_data, best_actor_index);
                        actor_type_pureform_set_pending_form_impl(best_actor_index, (e_pureform)form, false, (e_morph_priority)1, true);
                        if (form == 1) {
                            if (!fp_ref_equals(best_actor->firing_position_reference, FP_REF_NONE)) {
                                firing_position_definition *element = (firing_position_definition
                                    *)firing_position_ref_get_definition(best_actor->firing_position_reference);
                                if (TEST_BIT(element->flags, 5)) {
                                    actor_set_current_firing_position(best_actor_index, FP_REF_NONE.value);
                                    best_actor->search_active = 0;
                                }
                            }
                        }
                    }
                    desired_counts[form]--;
                }
            }
        }
    }

    if (pureform_count > 0) {
        squad_actor_iterator iterator;
        actor_type_pureform_squad_actor_iterator_new(&iterator, squad_index, true);
        actor_datum *actor;
        while ((actor = (actor_datum *)::squad_actor_iterator_next(&iterator)) != 0) {
            if (actor_type_pureform_get_current_form(iterator.index) == _pureform_ranged &&
                actor->morph.pending_form == NONE &&
                actor->activation_ticks != NONE &&
                TLS_GAME_TIME_GLOBALS->time - actor->activation_ticks > game_seconds_to_ticks_round(0.5f)) {
                actor_type_pureform_set_pending_form_impl(iterator.index, _pureform_ranged, false, (e_morph_priority)1, true);
            }
        }
    }
}

/* 0x50c430 pureform_pathfind_to_firing_position */

// @address 0x50c430 ?pureform_pathfind_to_firing_position@@YA_NJW4e_pureform@@PA_N1PAM_N3@Z
bool pureform_pathfind_to_firing_position(long actor_index, e_pureform pureform, bool *p2, bool *p3, float *p4, bool special, bool use_former_position)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->uncovering) {
        long character_tag = actor->character_index;
        character_morph_properties *properties = character_definition_morph_properties_get(character_tag);
        if (properties) {
            if (properties->forms[2].index != -1) {
                bool saved_394 = actor->movement_3d;
                actor->character_index = properties->forms[2].index;
                actor->movement_3d = false;
                firing_position_evaluation_context context;
                memset(&context, 0, sizeof(context));
                context.type = 0;
                context.special = special;
                context.ignore_cover = (pureform == 1);
                if (use_former_position) {
                    if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE)) {
                        context.former_enabled = true;
                        context.former_strongly = true;
                        context.former_position_index = actor->firing_position_reference;
                    }
                }
                {
                    c_path_state path;
                    firing_position position;
                    do {
                        {
                            long previous_owner;
                            bool found;
                            firing_position_ref selected = actor_active_select_firing_position(
                                actor_index, &context, &position, &previous_owner, (path_state *)path.m_data, &found);
                            if (fp_ref_equals(selected, FP_REF_NONE)) {
                                result = false;
                                break;
                            }
                            {
                                firing_position_ref updated = actor_change_firing_position(
                                    actor_index, selected, &position, previous_owner, found ? (path_state *)path.m_data : NULL, true);
                                if (fp_ref_equals(updated, FP_REF_NONE)) {
                                    result = false;
                                    break;
                                }
                            }
                        }
                        result = true;
                        if (p3) {
                            if (position.concealed)
                                *p3 = true;
                            else if (actor_path_executing(actor_index)) {
                                actor_path *corner_array = &actor->path;
                                for (short i = 0; i < corner_array->point_count; i++) {
                                    actor_path_point *corner = &corner_array->points[i];
                                    if (corner->frame_index == 6)
                                        *p3 = true;
                                    else if (corner->sector.valid()) {
                                        *p3 = (pathfinding_get_sector(corner->sector)->flags & 0x10) == 0;
                                    }
                                    if (*p3)
                                        break;
                                }
                            }
                        }
                        if (p4) {
                            if (actor_path_executing(actor_index))
                                *p4 = actor_path_distance_remaining(actor_index, -1);
                            else
                                *p4 = 0.0f;
                        }
                    } while (false);
                    if (!result)
                        datum_get<actor_datum>(actor_data, actor_index)->uncovering = false;
                }
                actor->character_index = character_tag;
                actor->movement_3d = saved_394;
            }
        }
    }
    if (p2) {
        bool path_finished = datum_get<actor_datum>(actor_data, actor_index)->path_state == 2;
        if (path_finished) {
            long unit_index = actor->unit_index;
            if (unit_index != NONE && pureform == 1 && pureform_motor_program(unit_index) == 7)
                path_finished = false;
        }
        *p2 = path_finished;
    }
    return result;
}

// @address 0xd0a78 ?actor_handle_morph@@YA_NJJJ_N@Z
bool actor_handle_morph(long actor_index, long object_index, long character_index, bool exact)
{
    character_definition *tag_definition = (character_definition *)tag_get('char', character_index);
    object_placement_data placement;
    long previous_form;
    if (actor_index != -1)
        previous_form = actor_type_pureform_get_current_form(actor_index);
    else
        previous_form = -1;
    bool result = false;
    long new_object = -1;
    {
        object_placement_data_new(&placement, tag_definition->unit.index, -1, 0);
        object_get_origin(object_index, &placement.position);
        object_get_orientation(object_index, &placement.forward, &placement.up);
        placement.model_variant_index = 0;
        object_get_action_points(object_index, &placement);
        new_object = object_new(&placement);
    }
    if (new_object != -1) {
        character_general_properties *general = character_definition_general_properties_get(character_index);
        character_morph_properties *morph = character_definition_morph_properties_get(character_index);
        short form = (short)character_get_form(character_index);
        long primary_weapon = -1;
        short grenade_type = -1;
        if (morph)
            primary_weapon = morph->targets[form].index;
        actor_select_character_voice(character_index, new_object);
        if (actor_index != -1) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            actor->character_index = character_index;
            if (general) {
                actor->type = general->type;
                if (general->grenade_type > 0)
                    grenade_type = general->grenade_type - 1;
            }
            actor_customize_unit(character_index, new_object, primary_weapon, -1, grenade_type, -1);
            actor_attach_unit(actor_index, new_object);
        } else {
            actor_customize_unit(character_index, new_object, -1, -1, -1, -1);
            actor_index = actor_create(new_object, character_index, -1, false);
        }
        long old_program = pureform_motor_program(object_index);
        s_object_header *new_header = &((s_object_header *)object_header_data->data)[(unsigned short)new_object];
        biped_datum *new_datum = (biped_datum *)new_header->object;
        c_motor_program_state *new_mover = (c_motor_program_state *)((char *)new_datum + new_datum->unit.motor_offset);
        if (new_mover->program != (e_motor_program)old_program)
            new_mover->set_program(new_object, (e_motor_program)old_program);
        if (((s_object_header *)object_header_data->data)[(unsigned short)object_index].object_type == 0) {
            biped_datum *old_verified = (biped_datum *)((s_object_header *)object_header_data->data)[(unsigned short)object_index].object;
            biped_datum *new_verified = (biped_datum *)((s_object_header *)object_header_data->data)[(unsigned short)new_object].object;
            if (old_verified->biped.physics.m_mode == 1 && new_verified->biped.physics.m_mode == 1)
                new_verified->biped.physics.get_mode_ground()->set_ground_plane(old_verified->biped.physics.get_mode_ground()->get_ground_plane());
        }
        real_point3d origin;
        real_vector3d forward;
        real_vector3d up;
        object_get_origin(object_index, &origin);
        object_get_orientation(object_index, &forward, &up);
        object_set_position_direct(new_object, &origin, &forward, &up, 0, false);
        action_request request;
        memset(&request, 0, sizeof(request));
        request.action = 9;
        request.data[0] = (char)previous_form;
        action_submit(new_object, &request);
        object_compute_node_matrices(new_object);
        if (object_has_muffins(object_index))
            muffins_transfer_to_new_object(object_index, new_object);
        object_delete(object_index);
        if (actor_index != -1) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            behavior_change(actor_index, 0, 1, 0);
            if (form >= 0 && form < 3) {
                s_game_globals *game_globals = global_game_globals;
                if (game_globals && game_globals->ai_globals.count() > 0)
                    actor->morph.morph_delay_ticks = (short)(long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second
                        * ((s_ai_globals_definition *)game_globals->ai_globals.address)->morph_delays[form] + 0.5f);
                else
                    actor->morph.morph_delay_ticks = (short)(long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 3.0f + 0.5f);
            } else {
                long unit_index = datum_get<actor_datum>(actor_data, actor_index)->unit_index;
                if (unit_index != -1)
                    unit_scream(unit_index, 32, -1, 13, 0, 0);
            }
            actor->morph.morph_locked = 0;
            actor->morph.last_morph_time = TLS_GAME_TIME_GLOBALS->time;
            if (exact) {
                actor->morph.morph_locked = actor->morph.immediate_morph;
                if (actor->morph.pending_form_priority < 1 || actor->search_active != 0)
                    actor_type_pureform_clear_pending_form(actor_index);
            }
        }
        result = true;
    } else {
        if (actor_index != -1) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            actor->morph.last_morph_time = TLS_GAME_TIME_GLOBALS->time;
            actor_type_pureform_clear_pending_form(actor_index);
        }
    }
    return result;
}

// @address 0x7c3714 ?pureform_group_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_group_morph_impulse = Behavior_impulse(159, 0x3ff007c0, group_morph_get_impulse);

// @address 0x7c3728 ?pureform_arrival_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_arrival_morph_impulse = Behavior_impulse(160, -1, pureform_arrival_morph_get_impulse);

// @address 0x7c373c ?pureform_default_form_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_default_form_impulse = Behavior_impulse(161, 0x3ff0040e,
    pureform_default_form_get_impulse);
