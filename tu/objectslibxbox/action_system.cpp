// objectslibxbox/action_system.obj: from the records 0xa0538 0xdc088 0xedff8 0x3549a8 0x354a18 0x354b38 0x354be0 0x50c888
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "math/real_math_functions.h"
#include "interface/first_person_weapons.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "units/motor_enums.h"
#include "units/motor_task.h"
#include "units/motor_program_state.h"
#include "game/player_training_globals.h"
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0xdc088 ?action_update@@YA_NJ@Z
// @address 0xedff8 ?action_submit@@YA_NJPBUaction_request@@@Z
// @address 0x3549a8 action_submit_event
// @address 0x354a18 ?unit_align_facing@@YAXJPBTreal_vector3d@@@Z
// @address 0x354b38 ?unit_align_facing2d@@YAXJPBTreal_vector2d@@@Z
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0x354be0 unit_handle_message_from_weapon
// @address 0x50c888 action_submit_initial_animation
#include <string.h>
#include "motor/action_system.h"
#include "objects/objects.h"
#include "objects/object_datum.h"
#include "units/biped_definitions.h"
#include "units/unit_types.h"
#include "units/action_request.h"
#include "memory/data_types.h"
#include "cache/cache_files.h"
#include "game/player_mapping_types.h"
#include "objects/object_header_access.h"
#include "math/real_math.h"
#include "motor/mover_inline.h"
#include "game/game_time_inline.h"
#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"
#include "units/unit_messages.h"

// @address 0x73cd88 ?g_action_usage_definitions@@3PAUs_action_usage@@A
s_action_usage g_action_usage_definitions[k_action_count] =
{
    { "_action_death_ping", 25, 0x9 },
    { "_action_custom_animation", 20, 0x9 },
    { "_action_stop_custom_animation", 20, 0x0 },
    { "_action_get_infected", 19, 0x8 },
    { "_action_hostile_morph", 19, 0x8 },
    { "_action_resurrect", 19, 0x8 },
    { "_action_hard_ping", 18, 0x9 },
    { "_action_ping", 18, 0x1 },
    { "_action_morph", 17, 0x8 },
    { "_action_morph_arrival", 17, 0x8 },
    { "_action_infect", 15, 0x8 },
    { "_action_posture", 12, 0x8 },
    { "_action_posture_exit", 12, 0x8 },
    { "_action_vehicle_exit", 10, 0x8 },
    { "_action_vehicle_exit_and_detach", 10, 0x8 },
    { "_action_vehicle_ejection", 10, 0x8 },
    { "_action_vehicle_board", 10, 0x8 },
    { "_action_vehicle_enter", 8, 0x8 },
    { "_action_open", 6, 0x9 },
    { "_action_close", 6, 0x9 },
    { "_action_vehicle_exit_immediate", 6, 0x8 },
    { "_action_vehicle_flip", 6, 0x8 },
    { "_action_climb_attach", 6, 0x8 },
    { "_action_climb_detach", 6, 0x8 },
    { "_action_wall_lean", 6, 0x8 },
    { "_action_wall_lean_open", 6, 0x8 },
    { "action_token", 6, 0x8 },
    { "_action_takeoff", 6, 0x8 },
    { "_action_perch", 6, 0x8 },
    { "_action_perch_jump", 6, 0x8 },
    { "_action_land", 6, 0x8 },
    { "_action_jump", 6, 0x8 },
    { "_action_leap_anticipation", 6, 0x8 },
    { "_action_leap", 6, 0x8 },
    { "_action_leap_recovery", 6, 0x8 },
    { "_action_emit_ai", 6, 0x8 },
    { "_action_berserk", 6, 0x8 },
    { "_action_flurry", 6, 0x8 },
    { "_action_deploy_turret", 6, 0x8 },
    { "_action_smash", 6, 0x8 },
    { "_action_vault", 6, 0x8 },
    { "_action_hoist", 6, 0x8 },
    { "_action_ai_impulse", 6, 0x8 },
    { "_action_melee_ai", 6, 0x8 },
    { "_action_weapon_pickup", 6, 0x8 },
    { "_action_weapon_trade", 6, 0x8 },
    { "_action_primary_weapon_drop", 6, 0x0 },
    { "_action_secondary_weapon_drop", 6, 0x0 },
    { "_action_surge", 6, 0x8 },
    { "_action_phase", 6, 0x8 },
    { "_action_stow", 6, 0x8 },
    { "_action_draw", 6, 0x8 },
    { "_action_infection_spew", 6, 0x8 },
    { "_action_kamikaze", 5, 0xc },
    { "_action_primary_weapon_switch", 5, 0x8 },
    { "_action_secondary_weapon_switch", 5, 0x8 },
    { "_action_grenade", 4, 0xc },
    { "_action_equipment", 4, 0xc },
    { "_action_equipment_pickup", 4, 0x8 },
    { "_action_equipment_drop", 4, 0x8 },
    { "_action_melee_player", 3, 0x1f },
    { "_action_primary_weapon_primary_reload", 3, 0x1d },
    { "_action_primary_weapon_secondary_reload", 3, 0x1d },
    { "_action_secondary_weapon_primary_reload", 3, 0x1d },
    { "_action_secondary_weapon_secondary_reload", 3, 0x1d },
    { "_action_soft_ping", 2, 0x1 },
    { "_action_primary_weapon_primary_recoil", 2, 0x0 },
    { "_action_primary_weapon_secondary_recoil", 2, 0x0 },
    { "_action_primary_weapon_primary_chamber", 2, 0x0 },
    { "_action_primary_weapon_secondary_chamber", 2, 0x0 },
    { "_action_primary_weapon_primary_charged", 2, 0x0 },
    { "_action_primary_weapon_secondary_charged", 2, 0x0 },
    { "_action_secondary_weapon_primary_recoil", 2, 0x0 },
    { "_action_secondary_weapon_secondary_recoil", 2, 0x0 },
    { "_action_secondary_weapon_primary_chamber", 2, 0x0 },
    { "_action_secondary_weapon_secondary_chamber", 2, 0x0 },
    { "_action_secondary_weapon_primary_charged", 2, 0x0 },
    { "_action_secondary_weapon_secondary_charged", 2, 0x0 },
    { "_action_primary_weapon_overheat_enter", 2, 0x0 },
    { "_action_primary_weapon_overheat_loop", 2, 0x0 },
    { "_action_primary_weapon_overheat_exit", 2, 0x0 },
    { "_action_secondary_weapon_overheat_enter", 2, 0x0 },
    { "_action_secondary_weapon_overheat_loop", 2, 0x0 },
    { "_action_secondary_weapon_overheat_exit", 2, 0x0 },
};

static __forceinline c_motor_task *object_get_entry(unit_datum *object, short index)
{
    return (c_motor_task *)((char *)object + object->unit.motor_tasks.offset) + index;
}

bool action_executing(long object_index, e_action action)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    bool result = false;
    for (short action_index = 0; action_index < 4; action_index++)
    {
        c_motor_task *entry = object_get_entry(object, action_index);
        if (entry && entry->valid() && entry->action == action)
        {
            result = true;
            break;
        }
    }
    return result;
}

static __forceinline c_motor_task *unit_get_motor_task(long object_index, e_motor_resource resource)
{
    unit_datum *object = (unit_datum *)object_get(object_index);
    return (c_motor_task *)((char *)object + object->unit.motor_tasks.offset + resource * sizeof(c_motor_task));
}

bool action_update(long object_index)
{
    bool result = false;
    for (short resource = 0; resource < 4; resource++)
    {
        c_motor_task *task = unit_get_motor_task(object_index, (e_motor_resource)resource);
        if (task && task->valid() && task->action != _action_none)
        {
            const s_action_definition *definition = action_get_definition(task->action);
            if (definition->update)
            {
                if (definition->update(object_index, task->action))
                    result = true;
                else
                    task->action = _action_none;
            }
        }
    }
    return result;
}

bool action_submit(long index, const action_request *request)
{
    bool result = true;
    const s_action_definition *handler = action_get_definition((e_action)request->type);
    if (handler && handler->submit) {
        object_wake(index);
        result = handler->submit(index, request);
        s_object_header *entry = object_header_get(index);
        if (((1 << entry->object_type) & _object_mask_unit) != 0) {
            int owner = ((unit_datum *)entry->object)->unit.player_index;
            if (owner != -1) {
                int type = request->type;
                unsigned int slot_index =
                    tls_player_mapping_globals()->player_input_user_mapping[datum_index_to_absolute_index(owner)];
                if (slot_index <= 3) {
                    s_recent_action *slots = tls_player_training_entries()[slot_index].recent_actions.entries();
                    int best = 0;
                    int best_time = slots[0].time;
                    for (int i = 1; i < 4; i++) {
                        if (slots[i].time < best_time) {
                            best = i;
                            best_time = slots[i].time;
                        }
                    }
                    s_recent_action *slot = &slots[best];
                    slot->time = tls_game_time_globals()->time;
                    slot->type = (short)type;
                    slot->result = (unsigned char)result;
                }
            }
        }
    }
    return result;
}

static __forceinline void event_new(action_request *event, long type)
{
    memset(event, 0, sizeof(*event));
    event->type = type;
}

extern "C" bool action_submit_event(long index, long type)
{
    action_request event;
    memset(&event, 0, sizeof(event));
    event_new(&event, type);
    return action_submit(index, &event);
}

void unit_align_facing(long object_index, const real_vector3d *data)
{
    if (!mover_has_variable_camera_up(object_index)) {
        unit_align_facing2d(object_index, &data->xy);
    } else {
        unit_datum *unit = (unit_datum *)object_get(object_index);
        real_vector3d side;
        cross_product3d(&unit->object.up, data, &side);
        real_vector3d new_up;
        cross_product3d(&side, &unit->object.up, &new_up);
        if (normalize3d(&new_up) > 0.0f)
            unit->object.forward = new_up;
    }
}

void unit_align_facing2d(long unit_index, const real_vector2d *facing)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    if (unit->object.parent_object_index == -1)
    {
        float (*normalize)(real_vector2d *) = normalize2d;
        real_vector2d forward = *facing;
        if (normalize(&forward) > 0.0f)
        {
            set_real_vector3d(&unit->object.forward, forward.i, forward.j, 0.0f);
            unit->object.up = global_identity4x3->up;
        }
    }
}

extern "C" void unit_handle_message_from_weapon(long unit_index, long weapon_index, long message, long parameter)
{
    switch (message) {
    case 1: case 2: case 3: case 4: {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        if (parameter == 0)
            unit->unit.dual_error_ticks[0] = (unsigned char)game_seconds_to_ticks_round(0.3f);
        else
            unit->unit.dual_error_ticks[1] = (unsigned char)game_seconds_to_ticks_round(0.3f);
        break;
    }
    case _unit_message_overheat_enter:
        action_submit_event(unit_index,
            parameter ? _action_secondary_weapon_overheat_enter : _action_primary_weapon_overheat_enter);
        break;
    case _unit_message_overheat_exit:
        action_submit_event(unit_index,
            parameter ? _action_secondary_weapon_overheat_exit : _action_primary_weapon_overheat_exit);
        break;
    }
    first_person_weapon_handle_message(unit_index, weapon_index, (e_unit_messages)message, parameter);
}

extern "C" bool action_submit_initial_animation(long object_index)
{
    if (object_index != -1) {
        s_object_header *header = object_header_get(object_index);
        if (((1 << header->object_type) & _object_mask_biped) != 0) {
            biped_definition *definition = TAG_GET('bipd', biped_definition, header->object->definition);
            if (definition->reanimation_character.index != -1) {
                action_request request;
                event_new(&request, _action_hostile_morph);
                request.initial_animation.animation = definition->reanimation_character.index;
                request.initial_animation.animation_index = definition->reanimation_morph_muffins.index;
                request.initial_animation.flag_a = false;
                request.initial_animation.flag_b = false;
                request.initial_animation.animation_mode = -1;
                request.initial_animation.flag_c = true;
                action_submit(object_index, &request);
            }
        }
    }
    return false;
}
