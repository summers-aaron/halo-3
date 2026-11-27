// ailibxbox/actor_type_flood_tank.obj: from the records 0x573a60 0x573b18 0x573ba8 0x573ca0 0x573e68
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "math/real_math_types.h"
#include "ai/character_morph_properties.h"
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "game/game_time_inline.h"
#include "game/game_time_types.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "cseries/cseries.h"
// @address 0x573a60 ?tank_morph_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x573b18 ?tank_morph_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x573ca0 ?pureform_tank_throttle_control_get_impulse@@YAFJPAVc_behavior_state@@@Z
#include <math.h>
#include "ai/actor_datum.h"

#include "motor/motor_animation.h"
#include "ai/behavior.h"
#include "ai/character_definitions.h"
#include "ai/actor_type_flood_tank.h"
#include "ai/actor_type_pureform.h"
#include "ai/ai_reference_frame.h"
#include "ai/pureform_types.h"
#include "ai/ai_point3d.h"


extern "C" {
}
struct s_behavior_input;
struct s_behavior_arguments;
#define actor_data tls_actor_data()
#define actor_get(actor_index) datum_get<actor_datum>(actor_data, (actor_index))
// match: 0x573b18 uses this existing inline form; 0x573e68 calls the out-of-line actor_get_current_form.
static __forceinline e_pureform actor_type_flood_tank_get_current_form(long actor_index)
{
    e_pureform form = (e_pureform)NONE;
    actor_datum *actor = actor_get(actor_index);
    if ((FLAG(actor->type) & (FLAG(20) | FLAG(21) | FLAG(22))) != 0)
        form = character_get_form(actor->character_index);
    return form;
}

short tank_morph_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments)
{
    actor_datum *actor = actor_get(actor_index);
    short result = 0;
    int form = actor->morph.pending_form;

    if (form != -1 && !motor_animation_busy(actor->unit_index) && actor_executing_layer(actor_index, 27) == -1)
    {
        if ((unsigned int)form >= 2)
        {
            if ((unsigned int)form == 2)
                actor_morph_impl(actor_index, _pureform_stalker, actor->morph.force_morph, true);
        }
        else if (actor->uncovering)
        {
            result = 3;
        }
    }
    return result;
}

short tank_morph_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    actor_datum *actor = actor_get(actor_index);
    short result = -2;

    if (actor_type_flood_tank_get_current_form(actor_index) != _pureform_tank)
    {
        result = -1;
    }
    else if (actor->morph.pending_form == NONE)
    {
        result = -1;
    }
    return result;
}

// @address 0x573ba8 ?tank_morph_perform@@YA_NJPAVc_behavior_state@@@Z
bool tank_morph_perform(long actor_index, c_behavior_state *state)
{
    bool reached = false;
    bool failed = false;
    actor_datum *actor = actor_get(actor_index);
    e_pureform target_form = (e_pureform)actor->morph.pending_form;
    float distance;

    if (target_form == _pureform_stalker)
    {
        reached = true;
    }
    else if (actor->uncovering)
    {
        if (pureform_pathfind_to_firing_position(actor_index, target_form, &reached, &failed, &distance, true, true)
            && (failed || distance > 5.0f))
        {
            actor_morph_impl(actor_index, _pureform_stalker, false, false);
            return true;
        }
    }

    if (reached)
        actor_morph_impl(actor_index, target_form, actor->morph.force_morph, true);
    return true;
}





// 0x573ca0
short pureform_tank_throttle_control_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = actor_get(actor_index);
    character_morph_properties *morph = character_definition_morph_properties_get(actor->character_index);

    if (actor_executing_layer(actor_index, 38) == NONE && morph != 0)
    {
        bool path_executing = false;
        if (actor->path.valid != 0 && actor->path_state == 1)
            path_executing = true;
        if (path_executing && actor->target_prop_index != NONE)
        {
            float far_distance = 5.0f;
            float near_distance = 3.0f;
            float distance = ai_distance3d(&actor->path.destination, &actor->position.body);

            if (morph->tank_far_distance > 0.0f)
                far_distance = morph->tank_far_distance;
            if (morph->tank_near_distance > 0.0f)
                near_distance = morph->tank_near_distance;

            bool close = (actor->flags & FLAG(31)) != 0;

            if (close && distance > far_distance)
                close = false;
            else if (!close && distance < near_distance)
                close = true;

            actor->throttle_close = close;
            if (close)
                actor->flags |= FLAG(31);
            else
                actor->flags &= ~FLAG(31);
        }
        else
        {
            actor->flags &= ~FLAG(31);
        }
    }
    else
    {
        actor->flags &= ~FLAG(31);
    }

    if (morph != 0)
    {
        float speed = morph->throttle_hold_speed;
        if (!(speed > 0.0f))
            speed = 0.2f;
        if (actor->body_damage > speed)
        {
            float seconds = morph->throttle_hold_seconds;
            if (!(seconds > 0.0f))
                seconds = 3.0f;
            actor->throttle_ticks = (short)game_seconds_to_ticks_round(seconds);
        }
    }
    return -1;
}

// 0x573e68 (pureform_tank_distance_damage_morph_get_impulse)
// @address 0x573e68 ?pureform_tank_distance_damage_morph_get_impulse@@YAFJPAVc_behavior_state@@@Z
short pureform_tank_distance_damage_morph_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = actor_get(actor_index);
    character_morph_properties *morph = character_definition_morph_properties_get(actor->character_index);

    if (morph == 0 || actor->target_prop_index == NONE)
        return -1;

    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
    prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
    if (prop->flag == 0)
        return -1;

    short damage_ticks = actor->morph.distance_damage_ticks;
    float threshold;
    if (damage_ticks > 0)
        threshold = morph->distance_damage_threshold_sustain;
    else
        threshold = morph->distance_damage_threshold;
    if (actor_executing_layer(actor_index, 27) != NONE)
        threshold *= 2.0f;

    if (!(prop_ref->distance >= threshold))
    {
        actor->morph.distance_damage_reset_ticks = 0;
        actor->morph.distance_damage_ticks = 0;
        return -1;
    }

    if (prop_ref->stimulus == 3)
    {
        actor->morph.distance_damage_ticks = damage_ticks + 1;
        actor->morph.distance_damage_reset_ticks = 0;
    }
    else if (damage_ticks > 0)
    {
        short reset_ticks = (short)(actor->morph.distance_damage_reset_ticks + 1);
        actor->morph.distance_damage_ticks = damage_ticks + 1;
        actor->morph.distance_damage_reset_ticks = reset_ticks;
        if (reset_ticks > game_seconds_to_ticks_round(morph->distance_damage_reset_seconds))
        {
            actor->morph.distance_damage_ticks = 0;
            actor->morph.distance_damage_reset_ticks = 0;
        }
    }

    if (actor->morph.distance_damage_ticks > game_seconds_to_ticks_round(morph->distance_damage_morph_seconds)
        && actor_get_current_form(actor_index) != 0
        && ::actor_allowed_to_morph(actor_index, false, _pureform_ranged)
        && !motor_animation_busy(actor->unit_index)
        && actor_morph_impl(actor_index, _pureform_ranged, true, false))
    {
        actor->morph.distance_damage_ticks = 0;
        actor->morph.distance_damage_reset_ticks = 0;
    }
    return -1;
}

// @address 0x42d380 ?tank_morph_activate@@YA_NJPAVc_behavior_state@@@Z
bool tank_morph_activate(long, c_behavior_state *)
{
    return true;
}

// @address 0x5bec98 ?tank_morph_control@@YAXJPAVc_behavior_state@@@Z
void tank_morph_control(long, c_behavior_state *)
{
}

// @address 0x7c5a88 ?pureform_tank_distance_damage_morph_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_tank_distance_damage_morph_impulse = Behavior_impulse(170, 0x3ff007c0,
    pureform_tank_distance_damage_morph_get_impulse);

// @address 0x7c5a74 ?pureform_tank_throttle_control_impulse@@3VBehavior_impulse@@A
Behavior_impulse pureform_tank_throttle_control_impulse = Behavior_impulse(171, -1,
    pureform_tank_throttle_control_get_impulse);

// @address 0x7c5a28 ?pureform_tank_morph_behavior@@3VPrimitive_behavior@@A
Primitive_behavior pureform_tank_morph_behavior = Primitive_behavior(173, -1, tank_morph_get_desire,
    tank_morph_next, tank_morph_activate, 0, -1, tank_morph_perform, tank_morph_control);
