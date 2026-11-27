// objectslibxbox/equipment.obj: from the records 0xe5580 0xe55d8 0xeb108 0xeba28 0xec0f8 0x35a1f8 0x35a300 0x35a5e8 0x3648e0 0x471998 0x471a70
// @address 0x11df75c g_chud_scripting_globals
// @address 0x11df67c players_globals
// @flags /TP /O2 /Oc
#include "render/camera_fx_globals_types.h"
#include "game/player_control_globals.h"
#include "animations/animation_playback_flags.h"
#include "camera/camera_scripting.h"
#include "camera/terminal_camera_tuning.h"
#include "objects/object_damage_flags.h"
#include "math/real_math_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"
#include "memory/data_types.h"
#include "cseries/cseries.h"
#include <stdlib.h>
#include <string.h>
#include <new>
#include "math/matrix_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "units/unit_types.h"
#include "game/game.h"
#include "game/players.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "objects/object_animation_manager_inline.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "camera/director.h"
#include "effects/effects.h"
#include "effects/effect_creation.h"
#include "items/equipment.h"
#include "items/weapons.h"
#include "math/periodic_functions.h"
#include "objects/damage.h"
#include "ai/ai.h"
#include "render/camera_fx_settings.h"
#include "objects/object_datum.h"
#include "sound/game_sound_definitions.h"
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "physics/havok_component_access.h"
#include "items/item_types.h"
#include "camera/camera_script_globals_types.h"
#include "items/equipment_definitions.h"
#include "items/equipment_globals.h"
#include "game/players_globals_types.h"
#include "game/player_control_types.h"
#include "render/camera_fx_globals.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "scenario/scenario_weapon_types.h"
#include "animations/animation_manager.h"
#include "objects/damage_response_types.h"
#include "objects/damage_data.h"
#include "devices/device_terminals.h"
#include "game/game_time_access.h"
#include <math.h>
#include "tag_files/tag_index.h"
#include "units/action_request.h"
#include "physics/havok_component_class.h"
#include "math/real_math_constants.h"
#include "math/random_math_range.h"
#include "memory/data_typed_access.h"
#include "math/real_math.h"
#include "tag_files/string_ids.h"
// @address 0xe5580 ?equipment_remaining_charges@@YAJJ@Z
// @address 0xeba28 ?equipment_active_fraction@@YAMJ@Z
// @address 0x471998 equipment_find_slot


struct s_equipment_transition_data { float transition_scale; float transition_time; };

static __forceinline bool equipment_state_playback_complete(const c_animation_manager *manager)
{
    bool complete = manager->m_state_channel.playback_complete();
    return complete;
}

// match: 0xeba28, 0xeb108: named half preserves the constant base and register allocation.
static __forceinline long equipment_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + k_real_half);
}

// match: 0xeb108: retain the direct game-time TLS reads; tls_game_time_globals() changes the phantom-update schedule.

long equipment_remaining_charges(long equipment_index)
{
    equipment_datum *equipment = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
    if (definition->max_charges == -1)
        return -1;
    return definition->max_charges - equipment->charges_used;
}

float equipment_active_fraction(long equipment_index)
{
    equipment_datum *equipment = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
    float fraction = 0.0f;
    if (equipment->activation_time != -1 && definition->duration > 0.001f)
    {
        long duration_ticks = equipment_seconds_to_ticks_round(definition->duration);
        if (duration_ticks < 1)
            duration_ticks = 1;
        fraction = k_real_one - (float)((game_time_get() - equipment->activation_time) / duration_ticks);
    }
    fraction = MIN(MAX(fraction, 0.0f), k_real_one);
    return fraction;
}

// @address 0xec0f8 ?equipment_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool equipment_compute_function_value(long equipment_index, long function_type, long unused, float *value_out, bool *active_out, bool *)
{
    bool handled = false;
    float zero = 0.0f;
    float one = 1.0f;
    float value = zero;
    equipment_datum *equipment = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    if (function_type != _string_id_age)
    {
        if (function_type == _string_id_death)
        {
            s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
            if (definition->equipment_types[3].count != 0)
            {
                s_equipment_proximity_mine_definition *mine = (s_equipment_proximity_mine_definition *)definition->equipment_types[3].address;
                game_time_globals_definition *game_time = game_time_globals;
                float elapsed = (float)(double)(game_time->time - equipment->activation_time) * game_time->tick_length;
                float remaining = mine->self_destruct_time - elapsed > zero ? mine->self_destruct_time - elapsed : zero;
                value = one - remaining / 5.0f;
                handled = true;
            }
        }
    }
    else
    {
        game_time_globals_definition *game_time = game_time_globals;
        float elapsed = (float)(double)(game_time->time - equipment->creation_time) * game_time->tick_length;
        value = elapsed / 5.0f;
        handled = true;
    }
    if (handled)
    {
        value = MIN(MAX(value, zero), one);
        *value_out = value;
        *active_out = value > zero;
    }
    return handled;
}

int equipment_find_slot(s_equipment_slot *slots, long identifier)
{
    long free_index = -1;
    long index;
    for (index = 0; index < 8; index++)
    {
        s_equipment_slot *slot = &slots[index];
        if (!slot->in_use && free_index == -1)
            free_index = index;
        if (slot->identifier == identifier)
            break;
    }
    if (index == 8)
        return free_index;
    return index;
}


// @address 0x471a70 equipment_compute_orbit_point
extern "C" void equipment_compute_orbit_point(s_equipment_slot *slots, long index, long sub_index, real_point3d *point)
{
    s_equipment_slot *slot = &slots[index];
    unsigned long seed = ((sub_index << 12) ^ sub_index) << 4 ^ slot->identifier ^ slot->start_time;
    float time = (float)(game_time_get() - slot->start_time) * 0.05f / (float)game_time_globals->ticks_per_second;
    float angle_offset = (float)((random_seed_next(&seed) >> 16) % 1000) * 0.01f;
    float angle_phase = (float)((random_seed_next(&seed) >> 16) % 1000) * 0.01f;
    float radius_phase = (float)((random_seed_next(&seed) >> 16) % 1000) * 0.01f;
    float direction = (float)((random_seed_next(&seed) >> 16) % 1000) * 0.001f > 0.5f ? 1.0f : -1.0f;
    float angle = periodic_function_evaluate(10, angle_phase + time) * direction * 0.9424779f + angle_offset;
    float radius = periodic_function_evaluate(10, radius_phase + time) * slot->radius;
    point->x = (float)cos(angle) * radius + slot->center.x;
    float sine = (float)sin(angle);
    point->z = slot->center.z;
    point->y = sine * radius + slot->center.y;
}

struct s_animation_event_data;
// @address 0x35a1f8 ?equipment_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool equipment_new(long equipment_index, object_placement_data *, bool *)
{
    equipment_datum *datum = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, datum->item.object.definition_index);
    datum->creation_time = game_time_get();
    datum->activation_time = definition->max_charges != 0 ? -1 : datum->creation_time;
    s_equipment_datum *equipment = (s_equipment_datum *)datum;
    const s_damage_owner &owner = *damage_owner_unknown();
    equipment->creator_damage_owner = owner;
    equipment->looping_effect_index = -1;
    float fraction = equipment_active_fraction(equipment_index);
    long state;
    if (fraction == 0.0f)
        state = 12;
    else
        state = 0x293;
    datum = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    equipment = (s_equipment_datum *)datum;
    if ((unsigned short)equipment->item.object.animation.offset != 0xffff) {
        c_animation_manager *manager = (c_animation_manager *)((char *)datum + (short)equipment->item.object.animation.offset);
        manager->set_goal(226, 226, 226, state, 130, 31);
    }
    return true;
}

// @address 0x35a300 ?equipment_place@@YAXJPAUs_scenario_equipment@@@Z
void equipment_place(long equipment_index, s_scenario_equipment *scenario_equipment)
{
    equipment_datum *equipment = (equipment_datum *)object_get(equipment_index);
    object_set_invalid_for_recycling(equipment_index, true);
    bool flag_clear = (scenario_equipment->equipment_flags & 4) == 0;
    if (flag_clear)
        equipment->item.inventory_flags |= 4;
    else
        equipment->item.inventory_flags &= ~4;
}

struct s_animation_event_data;

union real_rgb_color;
struct effect_vector_field;

// @rdata 0x8202a1a4
static const float equipment_default_transition_time = 4.0f;

// @address 0x35a5e8 equipment_activate
extern "C" unsigned char equipment_activate(long equipment_index, long unit_index, long flags)
{
    bool result = false;
    bool use_effects = true;
    equipment_datum *equipment = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    long definition_index = equipment->item.object.definition_index;
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, definition_index);
    if (equipment_active_fraction(equipment_index) == 0.0f)
    {
        unsigned char has_charges = (equipment_remaining_charges(equipment_index) != 0);
        if (has_charges != 0)
        {
            long which = 0;
            e_equipment_type type = equipment_definition_get_type(definition_index, 0);
            if (type != _equipment_type_none)
            {
                float one = 1.0f;
                long action_kind = 57;
                const real_vector3d *zero_vector = global_zero_vector3d;
                do
                {
                    which = which + 1;
                    switch (type)
                    {
                    case 0:
                        if (unit_index == -1)
                            break;
                        {
                            s_equipment_definition *shield_definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
                            ((unit_datum *)object_get_and_verify_type(unit_index, NONE))->object.shield_vitality =
                                ((s_equipment_super_shield_definition *)shield_definition->equipment_types[0].address)->shield_ceiling_scale;
                        }
                        result = true;
                        break;
                    case 1:
                        break;
                    case 2:
                        if (unit_index == -1)
                            break;
                        {
                            s_equipment_definition *action_definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
                            if (((s_equipment_spawner_definition *)action_definition->equipment_types[2].address)->spawned_object.index == -1)
                                break;
                            action_request request;
                            csmemset(&request, 0, sizeof(request));
                            request.action = action_kind;
                            request.throw_item.definition_index = equipment->item.object.definition_index;
                            request.throw_item.initiate = 0;
                            request.throw_item.forced = 0;
                            request.throw_item.release = 0;
                            memcpy(&request.throw_item.position, zero_vector, sizeof(request.throw_item.position));
                            request.throw_item.direction = *zero_vector;
                            result = action_submit(unit_index, (const action_request *)&request);
                            use_effects = (result == 0);
                        }
                        break;
                    case 3:
                    case 4:
                        result = true;
                        break;
                    case 6:
                    {
                        s_equipment_definition *transition_definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
                        s_equipment_transition_data *transition = (s_equipment_transition_data *)transition_definition->equipment_types[6].address;
                        if (unit_index == -1)
                            break;
                        long player_index = ((unit_datum *)object_get_and_verify_type(unit_index, NONE))->unit.player_index;
                        if (player_index == -1)
                            break;
                        float transition_time = (transition->transition_time > 0.0f) ? transition->transition_time : equipment_default_transition_time;
                        float transition_scale = transition->transition_scale;
                        player_datum *player = datum_get_typed<player_datum>(tls_player_data(), player_index);
                        long target_unit_index = player->unit_index;
                        if (target_unit_index != -1 && !(transition_scale <= 0.0f))
                        {
                            player->equipment_touch_time = game_time_get();
                            player->equipment_touch_scale = transition_scale;
                            unit_active_camouflage_enable(target_unit_index, transition_time, false);
                            result = true;
                        }
                        else
                            result = true;
                    }
                        break;
                    case 7:
                    case 8:
                        result = true;
                        break;
                    default:
                        // match: 0x35a5e8: unreachable hint omits dead branch check
                        __assume(0);
                        break;
                    }
                    if (result != 0)
                    {
                        equipment->activation_time = game_time_get();
                        simulation_action_object_update(equipment_index, 16);
                        if (use_effects != 0)
                        {
                            long sound_index = definition->sound_index;
                            if (sound_index != -1)
                                unspatialized_impulse_sound_new(sound_index, one);
                            long effect_index = definition->effect_index;
                            if (effect_index != -1 && unit_index != -1)
                            {
                                long effect_storage;
                                c_tag_index *effect = new (&effect_storage) c_tag_index(effect_index);
                                effect_new_from_object(*effect, 0, 0, unit_index, one, one, 0, 0, 1);
                            }
                            if (definition->max_charges != 0)
                                equipment->charges_used = equipment->charges_used + 1;
                        }
                    }
                    type = equipment_definition_get_type(equipment->item.object.definition_index, which);
                } while (type != _equipment_type_none);
            }
        }
        object_activate(equipment_index);
    }
    return result;
}

// @address 0xe55d8 ?equipment_update@@YAXJJ@Z
void equipment_update(long equipment_index, long unit_index)
{
    equipment_datum *equipment = (equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
    if ((_rotl(equipment->item.object.damage_flags, 30) & 1) == 0)
    {
        long which = 0;
        long type = equipment_definition_get_type(equipment->item.object.definition_index, 0);
        if (type != -1)
        {
            float half = 0.5f;
            do
            {
                which++;
                switch (type)
                {
                case 0: break;
                case 1: break;
                case 2: break;
                case 3: break;
                case 4: break;
                case 5: break;
                case 6: break;
                case 7:
                {
                    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
                    s_equipment_type_invincibility *invincibility = (s_equipment_type_invincibility *)definition->equipment_types[7].address;
                    if (equipment->activation_time != -1)
                    {
                        long elapsed = game_time_get() - equipment->activation_time;
                        long duration_ticks = (long)(invincibility->duration * (float)game_time_globals->ticks_per_second + half) - 1;
                        long end_ticks = (long)(definition->duration * (float)game_time_globals->ticks_per_second + half);
                        if (elapsed == 0 && invincibility->start_effect.index != -1)
                        {
                            long tag_storage;
                            c_tag_index *tag = new (&tag_storage) c_tag_index(invincibility->start_effect.index);
                            effect_new_from_object(*tag, 0, 0, unit_index, 0.0f, 0.0f, 0, 0, 1);
                        }
                        if (elapsed + 1 == end_ticks && invincibility->end_effect.index != -1)
                        {
                            long tag_storage;
                            c_tag_index *tag = new (&tag_storage) c_tag_index(invincibility->end_effect.index);
                            effect_new_from_object(*tag, 0, 0, unit_index, 0.0f, 0.0f, 0, 0, 1);
                        }
                        if (invincibility->looping_effect.index != -1)
                        {
                            if (equipment->looping_effect_index == -1)
                            {
                                long tag_storage;
                                c_tag_index *tag = new (&tag_storage) c_tag_index(invincibility->looping_effect.index);
                                equipment->looping_effect_index = effect_new_from_object(*tag, 0, 0, unit_index, 0.0f, 0.0f, 0, 0, 0);
                            }
                            else
                                effect_ping(equipment->looping_effect_index, invincibility->looping_effect.index, 0.0f, 0.0f, false);
                        }
                        if (elapsed >= 0 && elapsed <= duration_ticks && unit_index != -1)
                        {
                            s_unit_datum *unit = (s_unit_datum *)object_get_and_verify_type(unit_index, NONE);
                            if (duration_ticks > 0)
                            {
                                const float &recharge = 1.0f / (float)duration_ticks;
                                float vitality = unit->object.shield_vitality + recharge;
                                if (vitality > 1.0f)
                                    vitality = 1.0f;
                                unit->object.shield_vitality = vitality;
                            }
                            else
                                unit->object.shield_vitality = 1.0f;
                        }
                    }
                    break;
                }
                case 8: break;
                // match: 0xe55d8: unreachable hint omits dead branch check
                default: __assume(0); break;
                }
                type = equipment_definition_get_type(equipment->item.object.definition_index, which);
            } while (type != -1);
        }
    }
}

static __forceinline void havok_component_wake_all_bodies_in_phantoms(c_havok_component *component)
{
    for (long body_index = 0; body_index < component->get_body_in_phantom_count(); body_index++)
    {
        long object_index = (*component->m_bodies_in_phantom)[body_index].object_index;
        if (object_index != -1)
            object_set_at_rest(object_index, false);
    }
}

// Note: accepted C bridge differs from other callers' response and argument types.
static __forceinline void object_deplete_body(long object_index, const s_damage_owner *owner, unsigned char reporting_info)
{
    s_destroy_routine_response response;
    memset(&response, 0, sizeof(response));
    object_kill(object_index, 0, owner, &response, true, reporting_info);
}

static __forceinline s_equipment_datum *equipment_get(long equipment_index)
{
    return (s_equipment_datum *)object_get_and_verify_type(equipment_index, NONE);
}

static __forceinline c_animation_manager *object_get_animation_manager(s_equipment_datum *object)
{
    return (c_animation_manager *)((char *)object + (short)object->item.object.animation.offset);
}

static __forceinline bool equipment_begin_animation_state(long equipment_index, long state, long flags, unsigned long playback_flags)
{
    s_equipment_datum *equipment = equipment_get(equipment_index);
    bool result = false;
    if ((unsigned short)equipment->item.object.animation.offset != 0xffff)
        result = ((c_animation_manager *)((char *)equipment + (short)equipment->item.object.animation.offset))
                     ->set_goal(226, 226, 226, state, flags, playback_flags);
    return result;
}

static __forceinline bool equipment_animation_state_is_idle(long state)
{
    bool result = false;
    if (state == 12)
        result = true;
    else if (state == 659)
        result = true;
    return result;
}

static __forceinline const s_equipment_proximity_mine_definition *equipment_get_proximity_mine_definition(long definition_index)
{
    return (const s_equipment_proximity_mine_definition *)((s_equipment_definition *)tag_get(0, definition_index))->equipment_types[3].address;
}

static __forceinline const s_equipment_orbit_definition *equipment_get_orbit_definition(long definition_index)
{
    return (const s_equipment_orbit_definition *)((s_equipment_definition *)tag_get(0, definition_index))->equipment_types[4].address;
}

// @address 0xeb108 ?equipment_update@@YA_NJ@Z
bool equipment_update(long equipment_index)
{
    s_equipment_datum *equipment = equipment_get(equipment_index);
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, equipment->item.object.definition_index);
    float active_fraction = 0.0f;
    bool animation_updated = false;
    bool proximity_armed = false;
    bool phantoms_updated = false;

    bool phantom_active = equipment->item.object.m_physics_flags.test(_object_physics_phantom_active_bit);
    if (phantom_active && equipment->item.object.havok_component_index != -1)
    {
        long duration_ticks = equipment_seconds_to_ticks_round(definition->phantom_duration);
        c_havok_component *component = havok_component_get(equipment->item.object.havok_component_index);
        long age = game_time_get() - equipment->activation_time;
        if (duration_ticks != 0 && equipment->activation_time != -1 && age == duration_ticks)
            object_set_at_rest(equipment_index, false);
        if (equipment->activation_time == -1 || age >= duration_ticks)
        {
            havok_component_update_phantom_state(component, true);
            phantoms_updated = true;
        }
    }

    if (!equipment->item.object.m_damage_flags.test(_object_damage_body_depleted_bit))
    {
        long type_index = 0;
        float fraction = equipment_active_fraction(equipment_index);
        active_fraction = fraction;
        if (object_has_animation_manager(equipment_index))
        {
            c_animation_manager *manager = object_get_animation_manager(equipment);
            long current_state = manager->get_state_name();
            long state = fraction == 0.0f ? 12 : 659;
            animation_updated = manager->update_state_animation(object_animation_callback, equipment_index, 0, NULL, NULL);
            if (!animation_updated || equipment_state_playback_complete(manager) ||
                current_state != state && equipment_animation_state_is_idle(current_state))
            {
                bool started = equipment_begin_animation_state(equipment_index, state, 130, 31);
                if (!started && manager->m_state_channel.valid())
                    manager->set_state_position_to_last_frame();
                animation_updated = started;
            }
            if (animation_updated)
                object_set_requires_motion(equipment_index);
        }

        e_equipment_type type;
        while ((type = equipment_definition_get_type(equipment->item.object.definition_index, type_index++)) != _equipment_type_none)
        {
            switch (type)
            {
            case 0:
            case 1:
            case 2:
            case 5:
            case 6:
            case 7:
                break;
            case 3:
            {
                if (equipment->activation_time == -1 || game_is_predicted())
                    break;
                const s_equipment_proximity_mine_definition *mine = equipment_get_proximity_mine_definition(equipment->item.object.definition_index);
                float age = game_ticks_to_seconds((float)(game_time_get() - equipment->activation_time));
                proximity_armed = mine->self_destruct_time != 0.0f;
                if (mine->self_destruct_time != 0.0f && age > mine->self_destruct_time)
                {
                    if (equipment->item.object.havok_component_index != -1)
                        havok_component_wake_all_bodies_in_phantoms(havok_component_get(equipment->item.object.havok_component_index));
                    if (definition->activation_sound != -1)
                        object_impulse_sound_new(equipment_index, definition->activation_sound, -1, global_origin3d, global_forward3d, 1.0f);
                    s_damage_owner damage_owner = equipment->creator_damage_owner;
                    object_deplete_body(equipment_index, &damage_owner, 16);
                }
                else if (age > mine->arm_time)
                {
                    bool triggered = false;
                    if (equipment->item.object.havok_component_index != -1)
                    {
                        c_havok_component *component = havok_component_get(equipment->item.object.havok_component_index);
                        for (long body_index = 0; body_index < component->get_body_in_phantom_count(); body_index++)
                        {
                            long object_index = (*component->m_bodies_in_phantom)[body_index].object_index;
                            if (object_index != -1 && object_index != equipment_index)
                            {
                                real_vector3d velocity;
                                object_get_localized_velocities(
                                    havok_component_get(equipment_get(object_index)->item.object.havok_component_index)->get_object_index(), &velocity,
                                    NULL, NULL, NULL);
                                float i_squared = velocity.i * velocity.i;
                                float speed_squared = (i_squared + velocity.j * velocity.j) + velocity.k * velocity.k;
                                if (speed_squared >= mine->trigger_velocity * mine->trigger_velocity)
                                {
                                    triggered = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (triggered)
                    {
                        equipment->proximity_ticks = (unsigned char)MIN(equipment->proximity_ticks + 1, 254);
                        if (game_ticks_to_seconds((float)equipment->proximity_ticks) > mine->trigger_time)
                        {
                            if (mine->detonation_effect.index != -1)
                            {
                                real_point3d center;
                                object_get_center_of_mass(equipment_index, &center);
                                long effect_storage;
                                c_tag_index *effect = new (&effect_storage) c_tag_index(mine->detonation_effect.index);
                                effect_new_unattached_from_point(*effect, &center, global_up3d, global_up3d, 1, 1, NULL, &equipment->item.object.location);
                            }
                            if (mine->detonation_damage_effect.index != -1)
                            {
                                s_damage_data damage;
                                damage_data_new(&damage, mine->detonation_damage_effect.index);
                                damage.location = equipment->item.object.location;
                                object_get_center_of_mass(equipment_index, &damage.origin);
                                damage.position = damage.origin;
                                damage.direction = equipment->item.object.forward;
                                damage.cone_direction = equipment->item.object.forward;
                                damage.reporting_info = 236;
                                damage.owner = equipment->creator_damage_owner;
                                area_of_effect_cause_damage(&damage, -1, -1);
                            }
                            object_deplete_body(equipment_index, damage_owner_unknown(), 236);
                        }
                    }
                    else
                    {
                        equipment->proximity_ticks = 0;
                    }
                }
                break;
            }
            case 4:
            {
                const s_equipment_orbit_definition *orbit = equipment_get_orbit_definition(equipment->item.object.definition_index);
                real_point3d center;
                object_get_center_of_mass(equipment_index, &center);
                long function = orbit->function;
                float speed = orbit->speed;
                float radius = orbit->radius;
                long time = game_time_get();
                BLAM_ASSUME(time >= 0);
                long slot_index = equipment_find_slot(g_chud_scripting_globals->equipment_slots, equipment_index);
                if (slot_index != -1)
                {
                    s_equipment_slot *slot = &g_chud_scripting_globals->equipment_slots[slot_index];
                    if (!slot->in_use)
                    {
                        slot->start_time = time;
                        slot->in_use = true;
                    }
                    slot->identifier = equipment_index;
                    slot->active = true;
                    slot->center = center;
                    slot->radius = radius;
                    slot->speed = speed;
                    slot->function = function;
                }
                ai_break_squad_tasks_around_object(equipment_index);
                break;
            }
            case 8:
            {
                if (game_is_predicted() || equipment->item.object.havok_component_index == -1)
                    break;
                c_havok_component *component = havok_component_get(equipment->item.object.havok_component_index);
                for (long body_index = 0; body_index < component->get_body_in_phantom_count(); body_index++)
                {
                    long object_index = (*component->m_bodies_in_phantom)[body_index].object_index;
                    if (object_index != -1 && object_index != equipment_index)
                    {
                        s_object_header *header = object_header_get(object_index);
                        if (header->object_type == 0)
                        {
                            object_datum *biped = (object_datum *)header->datum;
                            biped->shield_stun_ticks = 0;
                            biped->body_stun_ticks = 0;
                        }
                    }
                }
                break;
            }
            default:
                UNREACHABLE();
            }
        }
    }

    return phantoms_updated || proximity_armed || animation_updated || active_fraction != 0.0f;
}

// @rdata 0x82022514
static const float k_equipment_camera_up_offset = 0.6f;
// @rdata 0x82035f08..0x82035f0c
static const real_decibel k_equipment_zero_gain = { 0.0f };
// @rdata-end
// @address 0x3aa90 k_real_math_origin3d
// @address 0x7c6050 g_terminal_camera_forward_offset
extern "C" { float g_terminal_camera_forward_offset = 0.29f; }
static __forceinline s_terminal_datum *camera_owner_get(long object_index)
{
    return (s_terminal_datum *)object_get_and_verify_type(object_index, NONE);
}
static void camera_owner_clear_flag(short *flags)
{
    *flags &= ~1;
}
// @address 0x3648e0 terminal_activate_camera
extern "C" void terminal_activate_camera(long object_index)
{
    // match: 0x3648e0: dead locals pin the fmuls fr0/fr12 sequencing in the offset multiplies below.
    float pad_a = 0.0f, pad_b = 0.0f, pad_c = 0.0f, pad_d = 0.0f;
    float pad_e = 0.0f, pad_f = 0.0f, pad_g = 0.0f, pad_h = 0.0f;
    float pad_i = 0.0f, pad_j = 0.0f, pad_k = 0.0f, pad_l = 0.0f;
    float pad_m = 0.0f, pad_n = 0.0f, pad_o = 0.0f, pad_p = 0.0f;
    s_terminal_datum *object = camera_owner_get(object_index);
    s_terminal_definition *definition = (s_terminal_definition *)tag_get(0, object->definition_index);
    director_script_camera(true);
    object->final_screen_state = false;
    players_globals->suppressed = true;
    player_control_globals->input_flags &= ~0x4000000;
    s_camera_fx_globals *fx = camera_fx_globals;
    float exposure_scale = fx->current_b;
    float exposure = definition->exposure;
    camera_fx_animate_exposure(exposure_scale == 0.0f ? exposure : fx->current_a, exposure, exposure_scale, 1.0f, equipment_default_transition_time);
    if (definition->activation_sound.index != -1)
        scripted_sound_new_internal(definition->activation_sound.index, object_index, 1.0f, k_equipment_zero_gain, k_equipment_zero_gain, -1, NULL);

    if (tls_game_globals()->options.game_mode == 1 && players_get_active_and_in_game_count(true) > 1 && object->hidden_object_index != -1)
    {
        real_point3d position;
        unit_get_camera_position(object->hidden_object_index, &position);
        real_vector3d aiming_vector = ((unit_datum *)object_get_and_verify_type(object->hidden_object_index, NONE))->unit.aiming_vector;
        real_euler_angles3d angles;
        euler_angles2d_from_vector3d((real_euler_angles2d *)&angles, &aiming_vector);
        angles.roll = 0.0f;
        scripted_camera_set_internal(_scenario_camera_type_0, &position, &angles, 0.0f, -1, -1);
    }

    real_euler_angles3d facing;
    memcpy(&facing, &k_real_math_origin3d, sizeof(facing));
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    object_get_origin(object_index, &position);
    object_get_orientation(object_index, &forward, &up);
    real_vector3d offset;
    offset.i = g_terminal_camera_forward_offset * forward.i;
    offset.j = g_terminal_camera_forward_offset * forward.j;
    offset.k = g_terminal_camera_forward_offset * forward.k;
    offset.i += up.i * k_equipment_camera_up_offset;
    offset.j += up.j * k_equipment_camera_up_offset;
    offset.k += up.k * k_equipment_camera_up_offset;
    forward.i = forward.i * -1.0f;
    forward.j = forward.j * -1.0f;
    forward.k = forward.k * -1.0f;
    euler_angles2d_from_vector3d((real_euler_angles2d *)&facing, &forward);
    position.x += offset.i;
    position.y += offset.j;
    position.z += offset.k;
    scripted_camera_set_internal(_scenario_camera_type_0, &position, &facing, equipment_default_transition_time, -1, -1);

    camera_owner_clear_flag(&object->flags);
    s_terminal_datum *owner = camera_owner_get(object_index);
    owner->screen_state = 1;
    owner->screen_state_ticks = 0;
    g_chud_scripting_globals->scripting.enabled = false;
    short fade_bit = definition->terminal_number;
    if (fade_bit > 0)
        players_globals->terminals_accessed |= 1 << (fade_bit - 1);
    object_set_hidden(object->hidden_object_index, true);
}
