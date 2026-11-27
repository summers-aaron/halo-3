// ailibxbox/actor_type_flood_stalker.obj: from the records 0x571b68 0x571c08 0x571c98 0x571ef0 0x571fc0 0x572058 0x572190 0x5722d0 0x572350 0x572358 0x572418 0x572580 0x572978 0x572aa0 0x572b00 0x572b38 0x572c20 0x572dd0 0x572e00 0x572e60 0x572e68
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/character_morph_properties.h"
#include "ai/actor_type_flood_stalker_types.h"
#include "ai/prop_datum.h"
#include "ai/pathfinding_sector_types.h"
#include "ai/actor_datum.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include <string.h>
#include "math/real_math_types.h"
#include "ai/sector_ref.h"
#include "game/game_time_types.h"
#include "objects/object_header.h"
#include "ai/behavior.h"
#include "ai/prop_ref_datum.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/unit_types.h"
#include "ai/ai.h"
#include "ai/props.h"
#include "ai/sector.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/actor_type_flood_stalker.h"
#include "ai/actor_type_pureform.h"
#include "ai/actor_moving_functions.h"
#include "ai/actor_type_pureform.h"
struct s_behavior_input;
struct s_behavior_arguments;


#include "ai/firing_position_ref.h"
// @address 0x11eac8 ?clear@c_destination_orders@@QAAXXZ
#include "ai/actor_moving.h"
#include "ai/behavior_info.h"
#include "ai/pureform_types.h"
#include "ai/actor_prop_ref_iterator.h"

#include "ai/actor_firing_position.h"
#include "ai/actor_behavior_state.h"
#include "ai/prop_view_inline.h"
#include "ai/firing_position_ref_inline.h"
#include "ai/pathfinding_data.h"
#include "ai/path_state.h"
// The stalker behaviors' data after the layer header.

class c_stealth_stalk_group_data : public c_behavior_state {
public:
    short timer;
};
class c_charge_recover_data : public c_behavior_state {
public:
    union {
        real_point3d target_position;
        real_vector3d target_forward;
    };
};
struct s_behavior_input;
struct s_behavior_arguments;

namespace {



}
#include "ai/style_vector.h"
#include "ai/pureform_types.h"
// @address 0x518ed8 ?character_definition_morph_properties_get@@YAPAUcharacter_morph_properties@@J@Z
// @address 0x50b430 ?actor_set_pending_form_impl@@YA_NJW4e_pureform@@_NW4e_morph_priority@@1@Z
// @address 0xd2aa8 ?behavior_decide_sequential_looping@@YAFJFPAVc_behavior_state@@@Z
// @address 0x50b5c8 ?character_get_form@@YA?AW4e_pureform@@J@Z
namespace {
static __forceinline actor_datum *stalker_actor_get(long index)
{
    return datum_get<actor_datum>(tls_actor_data(), index);
}
static __forceinline void actor_set_active_camo(long actor_index)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    unit_datum *unit = OBJECT_GET(actor->unit_index, unit_datum, _object_mask_unit);
    if (*(long *)((char *)unit + unit->unit.motor_offset) == 0)
        global_behavior_info[70].header->kind = g_action_selection_nonce;
    actor = stalker_actor_get(actor_index);
    if (actor->isolation_level > 0 && actor->isolation_level < 10)
        actor->isolation_level += 10;
}
}
namespace {
static __forceinline void *stalker_pureform_datum_get(s_data_array *array, long index, long size)
{
    return array->data + (index & 0xffff) * size;
}
static __forceinline e_pureform stalker_pureform_current_form(long actor_index)
{
    e_pureform form = _pureform_none;
    actor_datum *actor = (actor_datum *)stalker_pureform_datum_get(tls_actor_data(), actor_index, sizeof(actor_datum));
    if ((0x700000 & (1 << actor->type)) != 0)
        form = character_get_form(actor->character_index);
    return form;
}
}
namespace {
static __forceinline void actor_type_flood_stalker_cancel_morph(long actor_index)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    if (actor->morph.immediate_morph)
        actor->morph.morph_locked = true;
    actor = stalker_actor_get(actor_index);
    actor->morph.pending_form = -1;
    actor->morph.pending_form_priority = -1;
    actor->morph.immediate_morph = false;
    actor->morph.force_morph = false;
}
static __forceinline bool actor_type_flood_stalker_path_executing(long actor_index)
{
    bool result = false;
    actor_datum *actor = stalker_actor_get(actor_index);
    if (actor->path.valid && actor->path_state == 1)
        result = true;
    return result;
}
static __forceinline bool actor_type_flood_stalker_path_finished(long actor_index)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    return actor->path_state == 2;
}
static __forceinline long actor_type_flood_stalker_unit_type(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    return *(long *)((char *)unit + unit->unit.motor_offset);
}
static __forceinline void actor_type_flood_stalker_path_result_reset(long actor_index, bool reset_time)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    actor->path.valid = false;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    if (reset_time)
    {
        actor->path_result_time = 0;
        actor->path_result_count = 0;
    }
}
static __forceinline void actor_type_flood_stalker_path_clear(long actor_index)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    actor_type_flood_stalker_path_result_reset(actor_index, true);
    actor->path_state = 0;
    actor->destination_orders.clear();
}
static __forceinline void actor_type_flood_stalker_actor_path_clear(actor_datum *actor)
{
    actor->path.valid = false;
    actor->path.obstacle_object_index = NONE;
    actor->path.result_value = 0;
    actor->path_result_time = 0;
    actor->path_result_count = 0;
    actor->path_state = 0;
    actor->destination_orders.clear();
}
}
// @address 0x571b68 ?stealth_morph_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short stealth_morph_get_desire(long actor_index, s_behavior_input const *input, s_behavior_arguments const *arguments)
{
    short desire = 0;
    actor_datum *actor = stalker_actor_get(actor_index);
    if (actor->morph.pending_form != -1 && actor->uncovering)
    {
        switch ((unsigned long)actor->morph.pending_form)
        {
        case _pureform_ranged:
        case _pureform_tank:
            desire = 3;
            break;
        case _pureform_stalker:
            if (actor->morph.pending_form_priority >= 1)
                desire = 3;
            else
                actor_type_flood_stalker_cancel_morph(actor_index);
            break;
        }
    }
    return desire;
}

// @address 0x571c08 ?stealth_morph_next@@YAFJPAVc_behavior_state@@0@Z
short stealth_morph_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    short next = -2;
    actor_datum *actor = (actor_datum *)stalker_pureform_datum_get(tls_actor_data(), actor_index, sizeof(actor_datum));
    if (stalker_pureform_current_form(actor_index) != _pureform_stalker)
        next = -1;
    else if (actor->morph.pending_form == -1)
        next = -1;
    return next;
}

namespace {




static __forceinline actor_datum *actor_type_flood_stalker_path_actor_get(long index)
{
    return &((actor_datum *)tls_actor_data()->data)[(unsigned short)index];
}
static __forceinline bool actor_type_flood_stalker_path_check_finished(long index)
{
    return actor_type_flood_stalker_path_actor_get(index)->path_state == 2;
}
static __forceinline bool actor_type_flood_stalker_path_check_executing(long index)
{
    actor_datum *a = actor_type_flood_stalker_path_actor_get(index);
    bool result = false;
    if (a->path.valid && a->path_state == 1)
        result = true;
    return result;
}
static __forceinline pathfinding_sector *pathfinding_sector_get(c_sector_ref ref)
{
    pathfinding_data *data = pathfinding_data_get(ref.structure_bsp_index);
    return &((pathfinding_sector *)data->sectors.address)[(short)ref.sector_index];
}
static __forceinline long actor_type_flood_stalker_path_object_type(long index)
{
    unit_datum *unit = OBJECT_GET(index, unit_datum, _object_mask_all);
    return *(long *)((char *)unit + unit->unit.motor_offset);
}
}
// @address 0x571c98 ?pathfind_to_tank@@YA_NJPA_N@Z
bool pathfind_to_tank(long index, bool * finished)
{
    actor_datum *a = actor_type_flood_stalker_path_actor_get(index);
    bool result = true;
    actor_find_pathfinding_location(index);
    if (a->uncovering)
    {
        result = false;
        long target = a->target_prop_index;
        if (target != -1)
        {
            c_destination_orders orders;
            memset(&orders.action.direction, 0, sizeof(orders.action.direction));
            memset(&orders.action, 0, sizeof(orders.action));
            memset(&orders, 0, sizeof(orders));
            orders.to_prop(target);
            result = ::actor_move(index, &orders, (path_state *)0);
        }
    }
    if (actor_type_flood_stalker_path_check_finished(index))
        *finished = true;
    else if (actor_type_flood_stalker_path_check_executing(index) && a->pathfinding_sector.valid())
    {
        pathfinding_sector *s = pathfinding_sector_get(a->pathfinding_sector);
        if (s && (s->flags & 16))
        {
            actor_path *p = &a->path;
            bool all = true;
            short count = p->point_count;
            for (short i = 0; i < count; i++)
            {
                if (p->points[i].sector.valid())
                {
                    pathfinding_sector *sector_data = pathfinding_sector_get(p->points[i].sector);
                    all &= sector_data && (sector_data->flags & 16);
                    if (!all)
                        break;
                }
            }
            *finished = all;
        }
    }
    if (a->unit_index != -1 && actor_type_flood_stalker_path_object_type(a->unit_index) == 7)
        *finished = false;
    return result;
}
// @address 0x571ef0 ?stealth_morph_perform@@YA_NJPAVc_behavior_state@@@Z
bool stealth_morph_perform(long actor_index, c_behavior_state *state)
{
    bool should_morph = false;
    actor_datum *actor = stalker_actor_get(actor_index);
    short pending_form = actor->morph.pending_form;
    if (pending_form != 0 && actor->morph.pending_form_priority < 1) {
        if (pending_form == 1)
            pathfind_to_tank(actor_index, &should_morph);
        else
            should_morph = true;
    } else {
        if (actor->uncovering != 0)
            pureform_pathfind_to_firing_position(actor_index, (e_pureform)pending_form, &should_morph, 0, 0, true, true);
    }
    if (should_morph)
        actor_morph_impl(actor_index, (e_pureform)pending_form, actor->morph.force_morph, true);
    return true;
}
// @address 0x571fc0 ?pureform_stealth_active_camo_control_get_impulse@@YAFJPAVc_behavior_state@@@Z
short pureform_stealth_active_camo_control_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_set_active_camo(actor_index);
    return -1;
}
// @address 0x572058 ?pureform_stealth_damage_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
short pureform_stealth_damage_morph_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    if (properties && actor->body_damage > properties->stalker_morph_damage)
    {
        bool player_present = false;
        prop_ref_datum *player_pref = 0;
        actor_prop_ref_iterator iterator;
        actor_datum *iter_actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
        iterator.next_index = iter_actor->first_prop_ref_index;
        prop_ref_datum *pref;
        while (true)
        {
            if (iterator.next_index != -1)
            {
                pref = &((prop_ref_datum *)tls_prop_ref_data()->data)[(unsigned short)iterator.next_index];
                iterator.next_index = pref->next_prop_ref_index;
            }
            else
                pref = 0;
            if (pref == 0)
                break;
            if (pref->stimulus == 3)
            {
                prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), pref->prop_index);
                if (prop->player)
                {
                    player_pref = pref;
                    player_present = true;
                    break;
                }
            }
        }
        if (player_present)
        {
            e_pureform form = player_pref->distance > properties->stalker_morph_distance ? _pureform_ranged : _pureform_tank;
            if (::actor_allowed_to_morph(actor_index, false, form))
                actor_set_pending_form_impl(actor_index, form, true, _morph_priority_none, false);
        }
    }
    return -1;
}


// @address 0x572190 ?pureform_search_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
short pureform_search_morph_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    if (behavior_search_get_desire(actor_index, 0, 0) > 0 && ::actor_allowed_to_morph(actor_index, false, _pureform_stalker) &&
        ((int)::actor_get_current_form(actor_index)) != _pureform_stalker && !motor_animation_busy(actor->unit_index))
    {
        prop_ref_datum *ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        prop_view *view = prop_view_get(ref);
        if (!actor->morph.morph_locked || view->tracking_index > actor->morph.last_morph_time)
        {
            actor->flying = true;
            if (behavior_allowed_to_investigate(actor_index, actor->target_prop_index))
            {
                actor_morph_impl(actor_index, _pureform_stalker, false, false);
                return -1;
            }
            actor_prop_abandon_search(actor_index, actor->target_prop_index, 3);
        }
    }
    return -1;
}

// @address 0x5722d0 ?stealth_stalk_relevant@@YAFJ@Z
short stealth_stalk_relevant(long actor_index)
{
    short result = 0;
    int entry_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->target_prop_index;
    if (entry_index != -1) {
        int datum_index = datum_get<prop_ref_datum>(tls_prop_ref_data(), entry_index)->view_index;
        if (datum_index != -1) {
            tracking_datum *datum = datum_get<tracking_datum>(tls_prop_data(), datum_index);
            prop_view *view = datum ? &datum->tracking_view : 0;
            if (view)
                result = 3;
        }
    }
    return result;
}

// @address 0x572350 ?behavior_stealth_stalk_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_stealth_stalk_get_desire(long actor_index, s_behavior_input const *input, s_behavior_arguments const *arguments)
{
    return stealth_stalk_relevant(actor_index);
}

// @address 0x572358 ?behavior_stealth_stalk_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_stealth_stalk_activate(long actor_index, c_behavior_state *behavior_state)
{
    c_stealth_stalk_data *stalk = (c_stealth_stalk_data *)behavior_state;
    bool result = stealth_stalk_relevant(actor_index) > 0;
    if (result)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        prop_state *state = (prop_state *)prop_state_get_by_index(actor->target_prop_index);
        actor_type_flood_stalker_path_clear(actor_index);
        stalk->stalk_close = 0;
        stalk->stalk_timer = 0;
        stalk->target_position = state->aiming_position;
    }
    return result;
}

namespace {


}
// @address 0x572418 ?behavior_stealth_stalk_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_stealth_stalk_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    short result = -1;
    if (stealth_stalk_relevant(actor_index))
    {
        actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
        character_morph_properties *properties =
            (character_morph_properties *)character_definition_morph_properties_get(actor->character_index);
        prop_ref_datum *ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        prop_view *view = 0;
        if (ref->view_index != -1)
        {
            long offset = (ref->view_index << 8) & 0xffff00;
            tracking_datum *datum = (tracking_datum *)((char *)tls_prop_data()->data + offset);
            if (datum)
                view = &datum->tracking_view;
        }
        bool charge = false;
        result = -2;
        if (ref->knowledge_state >= 1 && ref->knowledge_state <= 2 && actor->search_active && ref->distance < properties->stalk_max_distance &&
            view->status >= 6 && view->awareness >= 3 && view->vitality < 0.8f)
            charge = true;
        if (charge && ::behavior_melee_charge_get_desire(actor_index, 0, 0) > 0)
            result = -1;
    }
    return result;
}

// @address 0x572978 ?behavior_stealth_stalk_control@@YAXJPAVc_behavior_state@@@Z
void behavior_stealth_stalk_control(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    if (actor->target_prop_index != -1)
    {
        prop_ref_datum *prop = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        c_stealth_stalk_data *stalk_state = (c_stealth_stalk_data *)state;
        float distance = stalk_state->stalk_close ? 6.0f : 4.0f;
        if (prop->distance < distance)
            stalk_state->stalk_close = true;
        else
            stalk_state->stalk_close = false;
        if (actor_type_flood_stalker_unit_type(actor->unit_index) != 7)
        {
            actor->look_mode = 3;
            actor->look_direction.to_movement();
        }
        else if (stalk_state->stalk_close)
        {
            actor->look_mode = 4;
            actor->look_direction.to_target();
        }
        else if (actor_type_flood_stalker_path_executing(actor_index))
        {
            actor->look_mode = 3;
            actor->look_direction.to_movement();
        }
        else
        {
            actor->look_mode = 2;
            actor->look_direction.to_target();
        }
    }
}
// @address 0x572aa0 ?behavior_charge_recover_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_charge_recover_get_desire(long actor_index, s_behavior_input const *input, s_behavior_arguments const *arguments)
{
    short desire = 0;
    actor_datum *actor = stalker_actor_get(actor_index);
    if (actor->target_prop_index != -1)
    {
        prop_ref_datum *prop = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        if (prop->distance < 3.0f)
            desire = 3;
    }
    return desire;
}
// @address 0x572b00 ?behavior_charge_recover_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_charge_recover_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    short next = actor_type_flood_stalker_path_finished(actor_index) ? -1 : -2;
    return next;
}
// @address 0x572dd0 ?behavior_charge_recover_control@@YAXJPAVc_behavior_state@@@Z
void behavior_charge_recover_control(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    actor->look_mode = 2;
    actor->look_direction.to_movement();
}

// @address 0x572e00 ?behavior_stealth_stalk_group_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_stealth_stalk_group_get_desire(long actor_index, s_behavior_input const *input, s_behavior_arguments const *arguments)
{
    actor_datum *actor = stalker_actor_get(actor_index);
    unsigned short desire = 0;
    if (actor->target_prop_index != -1) {
        prop_ref_datum *prop = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        if (prop->awareness >= 1)
            desire = 3;
    }
    return desire;
}

// @address 0x572e60 ?behavior_stealth_stalk_group_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_stealth_stalk_group_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    return -2;
}

// @address 0x572e68 ?behavior_stealth_stalk_group_decide@@YAFJFPAVc_behavior_state@@@Z
short behavior_stealth_stalk_group_decide(long actor_index, short state_index, c_behavior_state *state)
{
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
    c_stealth_stalk_group_data *group_state = (c_stealth_stalk_group_data *)actor_behavior_state_get(actor, state_index);
    if (group_state->child_index != -1 &&
        actor_behavior_state_get(actor, (short)(state_index + 1))->behavior_index == 27 &&
        actor->target_prop_index != -1)
    {
        prop_ref_datum *ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        if (ref->distance > 3.0f)
        {
            prop_view *view = prop_view_get(ref);
            bool fail = false;
            if (ref->stimulus == 3 || (view && view->awareness <= 0 && view->status >= 11))
                fail = true;
            if (fail)
            {
                group_state->child_index = -1;
                group_state->execution_state = 3;
                actor_datum *path_actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
                path_actor->path.valid = 0;
                path_actor->path.obstacle_object_index = -1;
                path_actor->path.result_value = 0;
                path_actor->path_result_time = 0;
                path_actor->path_result_count = 0;
                path_actor->path_state = 0;
                path_actor->destination_orders.clear();
            }
        }
        else
        {
            group_state->timer = 0;
        }
    }
    else
    {
        group_state->timer = 0;
    }
    return behavior_decide_sequential_looping(actor_index, state_index, state);
}

// 0x572b38 stealth stalk group activate: reset path, seed target from prop object
namespace {

}
// @address 0x572b38 ?behavior_charge_recover_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_charge_recover_activate(long actor_index, c_behavior_state *state)
{
    c_charge_recover_data *recover = (c_charge_recover_data *)state;
    actor_datum *actor = stalker_actor_get(actor_index);
    actor->path.valid = false;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    actor->path_result_time = 0;
    actor->path_result_count = 0;
    actor->path_state = 0;
    actor->destination_orders.clear();
    prop_ref_datum *ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
    object_header_datum *header = object_header_get(ref->object_index);
    long object_index = ref->object_index;
    long object_type_bit = 1 << header->object_type;
    if ((object_type_bit & 0x1003) != 0)
    {
        unit_datum *unit = (unit_datum *)header->datum;
        recover->target_position = unit->unit.aiming_position;
    }
    else
    {
        real_vector3d up;
        object_get_orientation(object_index, &recover->target_forward, &up);
    }
    return true;
}

struct path_state;

// @address 0x572c20 ?behavior_charge_recover_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_charge_recover_perform(long actor_index, c_behavior_state *state)
{
    c_charge_recover_data *recover = (c_charge_recover_data *)state;
    actor_datum *actor = stalker_actor_get(actor_index);
    if ((!actor_type_flood_stalker_path_executing(actor_index) || fp_ref_equals(actor->firing_position_reference, FP_REF_NONE)) &&
        actor->unit_index != -1 && !actor_type_flood_stalker_path_executing(actor_index) && actor->uncovering) {
        c_path_state path;
        bool found;
        long index;
        firing_position position;
        firing_position_evaluation_context context;
        memset(&context, 0, sizeof(context));
        context.allow_either = true;
        context.has_firing_direction = true;
        context.hint = true;
        context.panic_distance = 5.0f;
        context.danger_minimum_distance = 8.0f;
        context.danger_maximum_distance = 15.0f;
        context.radius = 50.0f;
        context.target_hint_position = recover->target_position;
        context.type = 7;
        context.negative = false;
        firing_position_ref ref = actor_active_select_firing_position(actor_index, &context,
            &position, &index, (path_state *)path.m_data, &found);
        if (!fp_ref_equals(ref, FP_REF_NONE)) {
            path_state *path_arg = (path_state *)path.m_data;
            if (!found)
                path_arg = 0;
            actor_change_firing_position(actor_index, ref, &position, index, path_arg, true);
        }
    }
    return true;
}

static const float k_stalk_thwart_seconds = 3.0f;
// @address 0x572580 ?behavior_stealth_stalk_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_stealth_stalk_perform(long actor_index, c_behavior_state *behavior_state)
{
    c_stealth_stalk_data *stalk = (c_stealth_stalk_data *)behavior_state;
    actor_datum *actor = stalker_actor_get(actor_index);
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
    if (actor->unit_index != -1 && !actor_type_flood_stalker_path_executing(actor_index) &&
        !fp_ref_equals(actor->firing_position_reference, FP_REF_NONE) &&
        OBJECT_GET(actor->unit_index, unit_datum, _object_mask_unit)->object.current_body_damage > 0.0f) {
        actor_discard_firing_position(actor_index, actor->firing_position_reference, false);
        actor_change_firing_position(actor_index, FP_REF_NONE, 0, -1, 0, true);
    }
    if (actor->uncovering) {
        firing_position_evaluation_context context;
        memset(&context, 0, sizeof(context));
        context.type = 0;
        context.preferred_flags = 6;
        context.attack_range_minimum = properties ? properties->stalk_min_distance : 3.0f;
        context.attack_range_maximum = properties ? properties->stalk_max_distance : 10.0f;
        context.attack_range_distance = properties ? properties->stalk_far_distance : 12.0f;
        context.allow_either = true;
        context.radius = 50.0f;
        if (actor_type_flood_stalker_path_finished(actor_index)) {
            prop_state *prop = prop_state_get(prop_ref);
            stalk->target_position = prop->aiming_position;
        }
        context.has_firing_direction = true;
        context.hint = true;
        context.negative = true;
        context.special = true;
        context.target_hint_position = stalk->target_position;
        if (actor_type_flood_stalker_path_executing(actor_index) && actor->destination_orders.type == 3) {
            firing_position_ref previous = actor->destination_orders.firing_position;
            if (!fp_ref_equals(previous, FP_REF_NONE)) {
                context.former_enabled = true;
                context.former_position_index = previous;
            }
        }
        c_path_state path;
        bool found;
        long previous_owner;
        firing_position position;
        firing_position_ref selected = actor_active_select_firing_position(actor_index, &context,
            &position, &previous_owner, (path_state *)path.m_data, &found);
        if (!fp_ref_equals(selected, FP_REF_NONE)) {
            selected = actor_change_firing_position(actor_index, selected, &position, previous_owner,
                found ? (path_state *)path.m_data : 0, true);
        }
    }
    if (!style_vector_test(166))
        return true;
    if (!actor_type_flood_stalker_path_executing(actor_index) && actor->pathfinding_sector.valid())
        stalk->stalk_timer++;
    else {
        short timer = stalk->stalk_timer;
        if (timer > 0) {
            timer -= 2;
            stalk->stalk_timer = timer;
            if (timer < 0)
                stalk->stalk_timer = 0;
        }
    }
    if (stalk->stalk_timer > (long)((float)tls_game_time_globals()->ticks_per_second * k_stalk_thwart_seconds + 0.5f) &&
        properties && !motor_animation_busy(actor->unit_index)) {
        e_pureform form = _pureform_tank;
        if (prop_ref->distance > properties->stalker_morph_distance)
            form = _pureform_ranged;
        actor_set_pending_form_impl(actor_index, form, false, _morph_priority_none, false);
    }
    return true;
}

// @address 0x535068 ?stealth_morph_activate@@YA_NJPAVc_behavior_state@@@Z
bool stealth_morph_activate(long actor_index, c_behavior_state *state)
{
    actor_type_flood_stalker_actor_path_clear(stalker_actor_get(actor_index));
    return true;
}

// @address 0x52bbf8 ?behavior_stealth_stalk_group_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_stealth_stalk_group_activate(long actor_index, c_behavior_state *state)
{
    c_stealth_stalk_group_data *group = static_cast<c_stealth_stalk_group_data *>(state);
    group->timer = 0;
    return true;
}

// @address 0x5bec98 ?stealth_morph_control@@YAXJPAVc_behavior_state@@@Z
void stealth_morph_control(long, c_behavior_state *)
{
}

// @address 0x7c5824 ?pureform_search_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_search_morph_impulse = Behavior_impulse(162, -1, pureform_search_morph_get_impulse);

// @address 0x7c57fc ?pureform_stealth_active_camo_control_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_stealth_active_camo_control_impulse = Behavior_impulse(163, -1,
    pureform_stealth_active_camo_control_get_impulse);

// @address 0x7c5810 ?pureform_stealth_damage_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_stealth_damage_morph_impulse = Behavior_impulse(164, -1,
    pureform_stealth_damage_morph_get_impulse);

// @address 0x7c5838 ?pureform_stealth_stalk_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_stealth_stalk_behavior = Primitive_behavior(165, 0x3ff007f0,
    behavior_stealth_stalk_get_desire, behavior_stealth_stalk_next, behavior_stealth_stalk_activate, 0, -1,
    behavior_stealth_stalk_perform, behavior_stealth_stalk_control);

// @address 0x72cd50 ?behavior_stealth_stalk_group_children@@3PAUchild_entry@@A
child_entry behavior_stealth_stalk_group_children[3] = {
    { 165, 0, true, 0, -1, 0.0f },
    { 27, 0, true, 0, -1, 0.0f },
    { 168, 0, true, 0, -1, 0.0f },
};

// @address 0x7c58d8 ?pureform_stealth_stalk_group_behavior@@3VMutex_group@@A
Mutex_group pureform_stealth_stalk_group_behavior = Mutex_group(167, 0x3ff007f0,
    behavior_stealth_stalk_group_get_desire, behavior_stealth_stalk_group_next, behavior_stealth_stalk_group_activate,
    0, -1, behavior_stealth_stalk_group_decide, 3, behavior_stealth_stalk_group_children);

// @address 0x7c5888 ?pureform_stealth_charge_recover_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_stealth_charge_recover_behavior = Primitive_behavior(168, 0x3ff007f0,
    behavior_charge_recover_get_desire, behavior_charge_recover_next, behavior_charge_recover_activate, 0,
    -1, behavior_charge_recover_perform, behavior_charge_recover_control);

// @address 0x7c57b0 ?pureform_stealth_morph_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_stealth_morph_behavior = Primitive_behavior(172, -1, stealth_morph_get_desire,
    stealth_morph_next, stealth_morph_activate, 0, -1, stealth_morph_perform, stealth_morph_control);
