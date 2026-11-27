// ailibxbox/actor_type_flood_ranged.obj: from the records 0x573028 0x5730c8 0x573158 0x573278 0x5733e0 0x5734e0 0x573790 0x573890 0x5738b8 0x5739a0
// @flags /TP /O2
#include "ai/behavior_info.h"
#include "memory/data_types.h"
#include "ai/behavior_stimulus_types.h"
#include "memory/tls_globals.h"
#include "game/game_time_inline.h"
#include "ai/character_morph_properties.h"
#include "ai/actor_datum.h"
#include "ai/prop_ref_datum.h"
// @address 0x573028 ?ranged_morph_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x5733e0 ?ranged_turtle_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x573890 ?ranged_turtle_control@@YAXJPAVc_behavior_state@@@Z
// @address 0x5739a0 ?pureform_ranged_uncover_perform@@YA_NJPAVc_behavior_state@@@Z
// @address 0x5730c8 ?ranged_morph_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x5738b8 ?pureform_ranged_uncover_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x518ed8 ?character_definition_morph_properties_get@@YAPAUcharacter_morph_properties@@J@Z
// @address 0x50b5c8 ?character_get_form@@YA?AW4e_pureform@@J@Z
// @address 0x50b640 ?actor_morph_inhibited@@YA_NJW4e_pureform@@@Z
// @address 0x50b430 ?actor_set_pending_form_impl@@YA_NJW4e_pureform@@_NW4e_morph_priority@@1@Z
// @address 0x3559c8 ?motor_animation_busy@@YA_NJ@Z
// @address 0x573278 ?pureform_ranged_proximity_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
#include <math.h>
#include "math/real_math.h"
#include "ai/actors.h"
#include "motor/motor_animation.h"
#include "ai/character_definitions.h"
#include "ai/actor_type_flood_ranged.h"
#include "ai/actor_datum.h"
#include "ai/actor_type_pureform.h"

// Sets the ranged-turtle control flag in the 2700-byte actor datum.
#include "ai/actor_type_pureform.h"
#include "ai/behavior.h"
#include "ai/pureform_types.h"


#include "ai/prop_datum.h"
#include "ai/actor_nearby_iterator_types.h"
#include "ai/actor_stimulus.h"
#include "math/random_math_range.h"


static __forceinline void *flood_ranged_datum_get(s_data_array *array,long index,long size) { return array->data+(index&65535)*size; }
static __forceinline actor_datum *flood_ranged_actor_get(long index) { return (actor_datum*)flood_ranged_datum_get(tls_actor_data(),
    index,sizeof(actor_datum)); }
static __forceinline bool flood_ranged_stimulus_test(long index, long stimulus)
{
    actor_datum *actor = flood_ranged_actor_get(index);
    return (actor->stimulus_flags & (1 << stimulus)) != 0;
}
static __forceinline void flood_ranged_stimulus_remove(actor_datum *actor, s_stimulus *stimulus)
{
    if (stimulus->type != -1)
    {
        actor->stimulus_flags &= ~(1 << stimulus->type);
        stimulus->type = -1;
    }
}
static __forceinline bool flood_ranged_stimulus_clear(long index, long stimulus)
{
    actor_datum *actor = flood_ranged_actor_get(index);
    bool result = false;
    for (short i = 0; i < 5; i++)
    {
        s_stimulus *entry = &actor->stimuli[i];
        if (entry->type == stimulus)
        {
            flood_ranged_stimulus_remove(actor, entry);
            result = true;
            break;
        }
    }
    return result;
}

static __forceinline actor_datum *flood_ranged_pureform_actor_get(long index)
{
    return (actor_datum *)flood_ranged_datum_get(tls_actor_data(), index, sizeof(actor_datum));
}
static __forceinline e_pureform flood_ranged_current_form(long actor_index)
{
    e_pureform form = _pureform_none;
    actor_datum *actor = flood_ranged_pureform_actor_get(actor_index);
    if ((((1 << actor->type) & ((1 << 20) | (1 << 21) | (1 << 22))) != 0))
        form = character_get_form(actor->character_index);
    return form;
}

short ranged_morph_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    actor_datum *actor = flood_ranged_pureform_actor_get(actor_index);
    short result = -2;

    if (flood_ranged_current_form(actor_index) != _pureform_ranged)
    {
        result = -1;
    }
    else if (actor->morph.pending_form == _pureform_none)
    {
        result = -1;
    }
    return result;
}


bool ranged_morph_perform(long actor_index, c_behavior_state *state)
{
    bool reached = false;
    bool failed = false;
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    e_pureform target_form = (e_pureform)actor->morph.pending_form;
    e_pureform morph_form = _pureform_none;
    float distance;

    if (target_form == _pureform_stalker)
    {
        reached = true;
    }
    else if (actor->uncovering)
    {
        if (pureform_pathfind_to_firing_position(actor_index, target_form, &reached, &failed, &distance, true, true) && !reached)
        {
            if (target_form == _pureform_tank)
            {
                if (failed || distance > 7.0f)
                    morph_form = _pureform_stalker;
                else
                    reached = true;
            }
            else if (target_form == _pureform_ranged)
            {
                if (distance > 1.5f)
                    morph_form = _pureform_stalker;
                else
                    reached = true;
            }
        }
    }

    if (morph_form != -1)
        actor_morph_impl(actor_index, morph_form, false, false);
    else if (reached)
        actor_morph_impl(actor_index, target_form, actor->morph.force_morph, true);
    return true;
}

short pureform_ranged_proximity_morph_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    long character = actor->character_index;
    character_morph_properties *properties = character_definition_morph_properties_get(character);
    if (properties && !motor_animation_busy(actor->unit_index) && actor->target_prop_index != -1)
    {
        float threshold = properties->proximity_threshold;
        prop_ref_datum *prop = (prop_ref_datum *)flood_ranged_datum_get(tls_prop_ref_data(), actor->target_prop_index, sizeof(prop_ref_datum));
        if (!(threshold > 0.0f))
            threshold = 2.0f;
        if (prop->distance < threshold)
        {
            long form = 0;
            if (::actor_allowed_to_morph(actor_index, false, _pureform_tank))
                form = 1;
            else if (::actor_allowed_to_morph(actor_index, false, _pureform_stalker))
                form = 2;
            if (form != 0 && properties->forms[form].index != character
                && ::actor_morph_impl(actor_index, (e_pureform)form, form == 1, false)
                && form != 1 && !actor_morph_inhibited(actor_index, _pureform_tank))
                actor_set_pending_form_impl(actor_index, _pureform_tank, true, _morph_priority_none, false);
        }
    }
    return -1;
}

struct s_behavior_input;
struct s_behavior_arguments;

// 0x5733e0
short ranged_turtle_get_desire(long actor_index, s_behavior_input const *input, s_behavior_arguments const *arguments)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    float damage_threshold = 0.2f;
    float turtle_radius = 1.0f;
    short desire = 0;
    if (properties)
    {
        if (properties->damage_threshold > 0.0f)
            damage_threshold = properties->damage_threshold;
        if (properties->ranged_morph_radius > 0.0f)
            turtle_radius = properties->ranged_morph_radius;
    }
    if (actor->morph.turtle_timer <= 0)
    {
        if (flood_ranged_stimulus_test(actor_index, _behavior_stimulus_turtle))
            desire = 3;
        else if (actor->body_damage > damage_threshold)
            desire = 3;
    }
    if (desire > 0 && actor->target_prop_index != -1)
    {
        prop_ref_datum *prop = (prop_ref_datum *)flood_ranged_datum_get(tls_prop_ref_data(), actor->target_prop_index, sizeof(prop_ref_datum));
        if (prop->distance < turtle_radius + 0.5f)
            desire = 0;
    }
    return desire;
}

// @address 0x573790 ?ranged_turtle_next@@YAFJPAVc_behavior_state@@0@Z
short ranged_turtle_next(long actor_index, c_behavior_state *, c_behavior_state *)
{
    actor_datum *actor=flood_ranged_actor_get(actor_index);
    character_morph_properties *properties=character_definition_morph_properties_get(actor->character_index);
    short next=-2;
    if(!flood_ranged_stimulus_test(actor_index, _behavior_stimulus_turtle)) next=-1;
    else if(properties && actor->target_prop_index!=-1) {
        prop_ref_datum *prop=(prop_ref_datum*)flood_ranged_datum_get(tls_prop_ref_data(),actor->target_prop_index,sizeof(prop_ref_datum));
        if(prop->distance<properties->ranged_morph_radius) { flood_ranged_stimulus_clear(actor_index, _behavior_stimulus_turtle); next=-1; }
    }
    return next;
}

void ranged_turtle_control(long actor_index, c_behavior_state *behavior_state)
{
    flood_ranged_actor_get(actor_index)->crouch = true;
}

static __forceinline char *flood_ranged_prop_view_get(const prop_ref_datum *ref)
{
    char *result = 0;
    if (ref->view_index != -1)
    {
        tracking_datum *view = (tracking_datum *)(tls_prop_data()->data + (ref->view_index & 0xffff) * sizeof(tracking_datum));
        if (view)
            result = view->view;
    }
    return result;
}

short pureform_ranged_uncover_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    short desire = 0;
    if (actor->target_prop_index != -1 && ::actor_allowed_to_morph(actor_index, false, _pureform_stalker) &&
        ::actor_get_current_form(actor_index) != _pureform_stalker && !motor_animation_busy(actor->unit_index))
    {
        prop_ref_datum *ref = (prop_ref_datum *)flood_ranged_datum_get(tls_prop_ref_data(), actor->target_prop_index, sizeof(prop_ref_datum));
        if (flood_ranged_prop_view_get(ref) && ref->status >= 3)
            desire = 3;
    }
    return desire;
}

short ranged_morph_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    short result = 0;
    int form = actor->morph.pending_form;
    if (form != -1 && !motor_animation_busy(actor->unit_index))
    {
        if ((unsigned int)form >= 2)
        {
            if ((unsigned int)form == 2)
                ::actor_morph_impl(actor_index, _pureform_stalker, actor->morph.force_morph, true);
        }
        else if (actor->uncovering)
        {
            result = 3;
        }
    }
    return result;
}

bool pureform_ranged_uncover_perform(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    bool result = true;

    if (actor->uncovering)
    {
        if (pureform_pathfind_to_firing_position(actor_index, _pureform_ranged, 0, 0, 0, false, false))
        {
            ::actor_morph_impl(actor_index, _pureform_stalker, false, false);
            actor_set_pending_form_impl(actor_index, _pureform_ranged, false, _morph_priority_none, false);
        }
        else
        {
            result = false;
        }
    }
    return result;
}

// @address 0x5734e0 ?ranged_turtle_activate@@YA_NJPAVc_behavior_state@@@Z
bool ranged_turtle_activate(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = flood_ranged_actor_get(actor_index);
    character_morph_properties *properties = character_definition_morph_properties_get(actor->character_index);
    if (!flood_ranged_stimulus_test(actor_index, _behavior_stimulus_turtle)) {
        float radius = 5.0f;
        float minimum = 5.0f;
        float maximum = 9.0f;
        if (properties) {
            if (properties->turtle_propagation_radius > 0.0f) radius = properties->turtle_propagation_radius;
            if (properties->turtle_time_min > 0.0f) minimum = properties->turtle_time_min;
            if (properties->turtle_time_max > 0.0f) maximum = properties->turtle_time_max;
        }
        short ticks = (short)game_seconds_to_ticks_round(_real_random_range(tls_random_seed(), minimum, maximum));
        stimulus_register(actor_index, _behavior_stimulus_turtle, ticks, 3);
        actor_nearby_actors_iterator iterator;
        actor_nearby_actors_iterator_new(&iterator, &actor->position.body, radius, actor->team, 0);
        actor_datum *other;
        while ((other = actor_nearby_actors_iterator_next(&iterator)) != 0) {
            if (other->type == actor->type && actor != other) {
                if (!flood_ranged_stimulus_test(iterator.actor_index, _behavior_stimulus_turtle)) {
                    float distance = distance3d(&other->position.body, &actor->position.body);
                    float delay = distance / radius;
                    other->morph.turtle_timer = (short)game_seconds_to_ticks_round(delay);
                }
                short friend_ticks = (short)game_seconds_to_ticks_round(_real_random_range(tls_random_seed(), minimum, maximum));
                stimulus_register(iterator.actor_index, _behavior_stimulus_turtle, friend_ticks, 3);
            }
        }
    }
    return true;
}

// @address 0x42d380 ?ranged_morph_activate@@YA_NJPAVc_behavior_state@@@Z
bool ranged_morph_activate(long, c_behavior_state *)
{
    return true;
}

// @address 0x5bec98 ?ranged_morph_control@@YAXJPAVc_behavior_state@@@Z
void ranged_morph_control(long, c_behavior_state *)
{
}

// @address 0x5bec98 ?ranged_turtle_deactivate@@YAXJPAVc_behavior_state@@@Z
void ranged_turtle_deactivate(long, c_behavior_state *)
{
}

// @address 0x42d380 ?ranged_turtle_perform@@YA_NJPAVc_behavior_state@@@Z
bool ranged_turtle_perform(long, c_behavior_state *)
{
    return true;
}

// @address 0x42d380 ?pureform_ranged_uncover_activate@@YA_NJPAVc_behavior_state@@@Z
bool pureform_ranged_uncover_activate(long, c_behavior_state *)
{
    return true;
}

// @address 0x5bec98 ?pureform_ranged_uncover_deactivate@@YAXJPAVc_behavior_state@@@Z
void pureform_ranged_uncover_deactivate(long, c_behavior_state *)
{
}

// @address 0x5bec98 ?pureform_ranged_uncover_control@@YAXJPAVc_behavior_state@@@Z
void pureform_ranged_uncover_control(long, c_behavior_state *)
{
}

// @address 0x7c5974 ?pureform_ranged_proximity_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_ranged_proximity_morph_impulse = Behavior_impulse(169, 0x3ff007c0,
    pureform_ranged_proximity_morph_get_impulse);

// @address 0x7c5928 ?pureform_ranged_morph_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_ranged_morph_behavior = Primitive_behavior(174, -1, ranged_morph_get_desire,
    ranged_morph_next, ranged_morph_activate, 0, -1, ranged_morph_perform, ranged_morph_control);

// @address 0x7c5988 ?pureform_ranged_turtle_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_ranged_turtle_behavior = Primitive_behavior(175, -1, ranged_turtle_get_desire,
    ranged_turtle_next, ranged_turtle_activate, ranged_turtle_deactivate, -1, ranged_turtle_perform,
    ranged_turtle_control);

// @address 0x572e60 ?pureform_ranged_uncover_next@@YAFJPAVc_behavior_state@@0@Z
short pureform_ranged_uncover_next(long, c_behavior_state *, c_behavior_state *)
{
    return -2;
}

// @address 0x7c59d8 ?pureform_ranged_uncover_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_ranged_uncover_behavior = Primitive_behavior(176, 0x3ff007fc,
    pureform_ranged_uncover_get_desire, pureform_ranged_uncover_next, pureform_ranged_uncover_activate,
    pureform_ranged_uncover_deactivate, -1, pureform_ranged_uncover_perform, pureform_ranged_uncover_control);
