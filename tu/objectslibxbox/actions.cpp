// objectslibxbox/actions.obj: from the records 0x102b18 0x378bf8 0x378c68 0x378d10 0x378f20 0x378fc8 0x379160 0x3794b0 0x3796a0 0x379780 0x379868 0x3798b0 0x379970 0x3799d0 0x379a60 0x379b10 0x379b90 0x379c18 0x379c80 0x379ce8 0x379d48 0x379dd8 0x379e68 0x37a030 0x37a048 0x37a0c0 0x37a1a0 0x37aa50 0x37acb0 0x37af10 0x37af48 0x37b670 0x37b868 0x37bcf0 0x37be20 0x37bf38 0x37bfd8 0x37bfe0 0x37c0e8 0x37c208 0x37c258 0x37c308 0x37c368 0x37c718 0x37c800 0x37c888 0x37c8c8 0x37c940 0x37c958 0x37ca98 0x37cb68 0x37cc20 0x37cc50 0x37cd60 0x37cfd0 0x37d030 0x37d078 0x37d080 0x37d240 0x37d298 0x37d370 0x37d488 0x37d510 0x37d7f0 0x37d8b0 0x37d968 0x37d9f0 0x37dc10 0x37dc68 0x37dc90 0x37dd50 0x37dfa0 0x37e160 0x37e260 0x37e2d0 0x37e488 0x37e4e8 0x37e550
#include "ai/ai_tls_data.h"
#include "math/real_math_constants.h"
#include "math/real_constant_pool.h"
// @flags /TP /O2 /Oc
#include "units/biped_types.h"
#include "physics/character_physics_mode_melee.h"
#include "ai/character_behavior_properties.h"
#include "game/material_constants.h"
#include "memory/tls_data_arrays.h"
#include "units/biped_seat_impulse_types.h"
#include "objects/object_header.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "game/game_globals.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/action_request.h"
#include "units/unit_seat.h"
#include "units/unit_seat_flags.h"
#include "ai/ai_point3d.h"
#include "units/motor_task.h"
#include "units/motor_program_state.h"
#include "units/vehicle_types.h"
#include "units/unit_types.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "ai/actors.h"
#include "game/players.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/actor_type_pureform.h"
#include "ai/ai_reference_frame.h"
#include "ai/ai_vehicles.h"
#include "ai/behavior.h"
#include "ai/squads.h"
#include "ai/swarms.h"
#include "game/game_engine_util.h"
#include "interface/first_person_weapons.h"
#include "items/weapons.h"
#include "ai/weapon_datum_types.h"
#include "motor/motor_system.h"
#include "motor/weapon_actions.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "items/weapon_definitions.h"
#include "objects/widgets/widgets.h"
#include "physics/physics_model_definitions.h"
#include "units/unit_animation.h"
#include "ai/character_definitions.h"
#include "motor/biped_leap_motor_program.h"
#include "objects/object_datum.h"
#include "items/projectile_types.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/animation_manager.h"
#include "effects/effects.h"
#include "motor/actions.h"
#include "sound/game_sound.h"
#include "game/game_allegiance.h"
#include "units/vehicles.h"
#include "objects/mover_datum.h"
#include "game/player_types.h"
#include "ai/character_morph_properties.h"
#include "units/unit_animation_key_frame_status_types.h"
#include "animations/animation_definitions.h"
#include "animations/animation_definition_types.h"
#include "scenario/scenario.h"
#include "ai/hint_ref.h"
#include "ai/interaction_target.h"
#include "physics/havok_component.h"

#include "ai/ai_hint_definitions.h"
#include "ai/actor_moving_hints.h"
#include "units/bipeds.h"
#include "ai/actor_datum.h"
#include "game/game_engine_event_data.h"
#include "physics/collision_test_vector.h"
#include "units/unit_definitions.h"
#include "models/model_types.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "physics/collision_test_flags.h"
#include "cseries/crt_xdk.h"
struct unit_passenger_iterator;
enum e_action_hoist;
// @address 0x37a048 ?melee_ai_finished@@YAXJW4e_action@@@Z
// @address 0x379160 ?unit_action_vehicle_board_submit@@YA_NJPBUaction_request@@@Z
// @address 0x3794b0 ?unit_action_vehicle_board_update@@YA_NJW4e_action@@@Z
// @address 0x37aa50 ?unit_action_melee_player_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37acb0 ?unit_action_melee_player_update@@YA_NJW4e_action@@@Z
// @address 0x37be20 ?action_vault_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37bfe0 ?action_hoist_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37d7f0 ?leap_anticipation_update@@YA_NJW4e_action@@@Z
// @address 0xedff8 ?action_submit@@YA_NJPBUaction_request@@@Z
// @address 0x500580 ?actor_set_team@@YAXJF@Z
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z
// @address 0x3aba70 ?simulation_action_unit_board_vehicle@@YAXJ@Z
// @address 0x3abc40 ?simulation_action_unit_melee_initiate@@YAXJJJJ@Z
// @address 0x328108 ?unit_add_equipment_to_inventory@@YAXJJ@Z
// @address 0x473780 ?weapon_get_deterministic_first_person_animation@@YAPBVc_model_animation@@JJPAJ@Z
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0x5286c8 ?actor_hint_reserve@@YA_NJVc_hint_ref@@_N@Z
// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0x3559c8 ?motor_animation_busy@@YA_NJ@Z
// @address 0x354ee0 ?motor_animation_running@@YA_NJJ@Z
// @address 0x102240 ?motor_animation_task_get_channel@@YAPAVc_animation_channel@@JPAVc_motor_task@@@Z
// @address 0xe8f00 ?motor_system_in_airborne_state@@YA_NJ@Z
// @address 0x33e620 ?motor_system_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
// @address 0x355ec8 ?motor_task_find_by_action@@YAPAVc_motor_task@@JW4e_action@@@Z
// @address 0xe3258 ?object_compute_node_matrices@@YAXJ@Z
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0x5281e0 ?swarm_spawner_end@@YAXF@Z
// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
// @address 0x335928 ?widget_type_new_on_object@@YAXJFJ@Z
// @address 0x2fb898 ?get_authored_primary_key_time@c_animation_channel@@QBAMXZ
// @address 0x2fb998 ?get_playback_duration@c_animation_channel@@QBAMXZ
// @address 0x1d9370 ?player_set_facing@@YAXJPBTreal_vector3d@@@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z
// @address 0x355a60 ?motor_task_interrupt@@YAXJFF@Z
// @address 0x361b20 ?unit_animation_get_key_frame_status@@YA?AW4e_unit_animation_key_frame_status@@J_N@Z
// @address 0x2fa840 ?enable_looping@c_animation_channel@@QAAX_N@Z
// @address 0x2fa908 ?jump_to_last_frame_and_stop@c_animation_channel@@QAAXXZ
// @address 0x2fa988 ?set_frame_position_from_playback_ratio@c_animation_channel@@QAAXM@Z
// @address 0xdd8d8 ?motor_system_interrupted@@YAXJPAVc_motor_task@@_N@Z
// @address 0x34ae28 ?weapon_uses_charging_melee@@YA_NJ@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0x32dc58 ?unit_evict_parent_seat_occupant@@YA_NJ_N0@Z
// @address 0x32dfe0 ?unit_seat_try_opening@@YAXJJ_N00@Z
// @address 0x327bb0 ?unit_update_driver_and_gunner@@YAXJ@Z
// @address 0x378fc8 ?unit_allowed_to_launch_board@@YA_NJJF@Z
// @address 0x37c8c8 ?unit_action_flurry_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37c718 ?unit_action_deploy_turret_submit@@YA_NJPBUaction_request@@@Z
// @address 0x379868 ?unit_action_vehicle_ejection_update@@YA_NJW4e_action@@@Z
// @address 0x37d078 ?unit_action_infection_spew_interrupt@@YAXJW4e_action@@@Z
// @address 0x37d488 ?unit_action_jump_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37dc90 ?action_surge_submit@@YA_NJPBUaction_request@@@Z
// @address 0x379a60 ?unit_action_land_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37af10 ?unit_action_melee_player_interrupt@@YAXJW4e_action@@@Z
// @address 0x37e488 ?action_stop_custom_animation_submit@@YA_NJPBUaction_request@@@Z
// @address 0x37cc20 ?unit_action_emit_ai_finished@@YAXJW4e_action@@@Z




#include "units/motor_request.h"
#include "motor/mover_action_state.h"







#include "units/unit_passenger_iterator_types.h"











#include "units/biped_definitions.h"

// 0x37d080




// 0x37d370






// a four-byte value passed by value (the caller's 0x37dc10 hands its field this way)






// 0x3794b0








// 0x37aa50

// 0x37acb0



// 0x37be20



// 0x37bfe0




struct s_unit_action_player_melee_strike { long animation_name, unit_message, animation; };

#include "units/unit_messages.h"




#include "game/player_information_definition.h"
#include "motor/player_melee_action_state.h"

#include "ai/swarm_spawner_types.h"
#include "ai/swarm_spawner_definition.h"




















#include "physics/havok_component_class.h"
#include "effects/material_effect_enums.h"
#include "objects/damage_data.h"
#include "physics/collision_types.h"

typedef char actions_layout_guard_7[(sizeof(void *) != 4 || ((sizeof(s_damage_data) == 0x94))) ? 1 : -1];
typedef char actions_layout_guard_8[(offsetof(s_damage_data, cone_direction) == 0x6c) ? 1 : -1];
typedef char actions_layout_guard_9[(sizeof(void *) != 4 || ((offsetof(s_damage_data, proximity) == 0x8c))) ? 1 : -1];
typedef char actions_layout_guard_10[(sizeof(void *) != 4 || ((sizeof(collision_result) == 0x60))) ? 1 : -1];
typedef char actions_layout_guard_11[(sizeof(void *) != 4 || ((offsetof(collision_result, global_material) == 0x20))) ? 1 : -1];
typedef char actions_layout_guard_12[(sizeof(void *) != 4 || ((offsetof(collision_result, material_index) == 0x5a))) ? 1 : -1];









// 0x37af48





#include "animations/animation_manager_storage.h"
#include "ai/actor_squad.h"
#include "units/unit_dialogue.h"
#include "units/unit_inventory_inline.h"
#include "animations/animation_search_path_iterator.h"
#include "effects/material_effects.h"
struct s_actions_single_value
{
    int v;
};
struct s_actions_equipment_pickup_request { long action; s_actions_single_value equipment; };

#include "memory/data_typed_access.h"
#include "game/game_simulation_inline.h"
#include "game/game_time_access.h"
#include "math/random_math_range.h"
#include "motor/motor_animation_state.h"

// @address 0x102b18 ?setup_leap_anticipation@c_motor_request@@QAAXW4e_action@@JJMPBTreal_vector3d@@@Z
void c_motor_request::setup_leap_anticipation(e_action action_index, long animation_name, long animation_index,
    float interpolation, const real_vector3d *direction)
{
    // match: 0x102b18: value range assumption guides branch lowering
    __assume(action_index == _action_leap_anticipation);
    // match: 0x102b18: value range assumption guides branch lowering
    __assume(animation_index == 871);
    // match: 0x102b18: value range assumption guides branch lowering
    __assume(interpolation == 0.26666668f);
    action = action_index;
    m_task = _motor_task_leap_anticipation;
    leap.second = animation_index;
    leap.first = animation_name;
    leap.value = interpolation;
    if (direction)
    {
        leap.has_vector = true;
        leap.vector = *direction;
    }
    else
        leap.has_vector = false;
    m_valid = true;
}



// @address 0x37dc68 ?unit_action_equipment_drop_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_equipment_drop_submit(long unit_index, const action_request *request)
{
    ::unit_drop_current_equipment(unit_index);
    return true;
}

bool unit_action_land_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    motor_request.setup_land(30,&request->position,&request->alignment,0.0f);
    return motor_system_submit(unit_index,&motor_request) != 0;
}

void unit_action_melee_player_interrupt(long unit_index, e_action action)
{
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    state->target = -1;
}

bool action_stop_custom_animation_submit(long unit_index, const action_request *request)
{
    bool result = false;
    c_motor_request motor_request;
    motor_request.m_force = false;
    motor_request.action = (e_action)(request->action);
    motor_request.m_task = _motor_task_stop_animation;
    motor_request.m_valid = true;
    if (motor_system_submit(unit_index,&motor_request) != 0)
        result = true;
    return result;
}






// @address 0x22dc0 k_real_half_pool

// @address 0x378bf8 ?unit_action_vehicle_entry_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_vehicle_entry_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.update.value = source->update.value;
    update.update.index = source->update.index;
    update.update.mode = source->update.mode;
    update.m_force = source->update.flag;
    update.action = _action_vehicle_enter;
    update.m_task = (e_motor_task)(0x1a);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

static __forceinline long actions_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds*(float)game_time_globals->ticks_per_second+0.5f);
}
static __forceinline void actions_player_ejection_finished(long index)
{
    if (!game_is_playback()) {
        player_datum *player=datum_get_typed<player_datum>(player_data,index);
        player->flags |= 0x400;
        player->vehicle_ejection_ticks=(short)actions_seconds_to_ticks_round(2.0f);
        ::simulation_action_game_engine_player_update((short)index, 0x1000);
    }
}
static __forceinline bool actions_team_is_friendly(long team, long other_team)
{
    return !(unsigned char)game_team_is_enemy((e_game_team)team, (e_game_team)other_team) || game_team_is_ally((e_game_team)team, (e_game_team)other_team);
}
static __forceinline long actions_unit_team_index(long object_index)
{
    unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
    long team = -1;
    if (unit) team = unit->unit.team;
    return team;
}
// 0x378fc8
bool unit_allowed_to_launch_board(long vehicle_index, long unit_index, short seat_index)
{
    bool result = true;
    unit_datum *vehicle = (unit_datum *)object_header_get(vehicle_index)->datum;
    if (vehicle->unit.actor_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        long vehicle_team = actions_unit_team_index(vehicle_index);
        unit_passenger_iterator iterator;
        iterator.unit_index = unit_index;
        iterator.passenger_index = -1;
        iterator.seat_index = -1;
        iterator.next_object_index = unit->object.first_child_object_index;
        for (unit_datum *passenger = ::unit_passenger_iterator_next(&iterator); passenger;
            passenger = ::unit_passenger_iterator_next(&iterator))
        {
            if (iterator.seat_index == seat_index)
            {
                if (actions_team_is_friendly(vehicle_team, actions_unit_team_index(iterator.passenger_index)))
                {
                    result = false;
                    break;
                }
            }
            else if (iterator.seat_index == -1)
                continue;
            else
            {
                unit_seat *seat = &((unit_seat *)definition->seats.address)[iterator.seat_index];
                if ((unsigned char)((seat->flags >> 11) & 1) == 0 || seat->index != seat_index)
                    continue;
                if (passenger->unit.player_index != -1)
                {
                    result = false;
                    break;
                }
                if (action_executing(iterator.passenger_index, _action_vehicle_board))
                {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}


// @address 0x379780 ?unit_action_vehicle_ejection_finished@@YAXJW4e_action@@@Z
void unit_action_vehicle_ejection_finished(long unit_index, e_action action)
{
    unit_datum *unit=((unit_datum *)object_get_and_verify_type(unit_index,8195));
    if (unit->unit.player_index != -1) actions_player_ejection_finished(unit->unit.player_index);
}

// 0x379868
bool unit_action_vehicle_ejection_update(long unit_index, e_action action)
{
    bool running = motor_animation_running(unit_index, 185);
    if (!running)
        unit_action_vehicle_ejection_finished(unit_index, action);
    return running;
}



// @address 0x3798b0 ?unit_action_vehicle_exit_and_detach_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_vehicle_exit_and_detach_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    if (request->bytes.b != 0)
    {
        if (datum->unit.actor_index != -1)
            actor_attach_to_vehicle(datum->unit.actor_index, -1, false);
    }
    motor_request.bytes.b = 0;
    motor_request.bytes.c = 0;
    motor_request.action = _action_vehicle_exit_and_detach;
    motor_request.m_task = _motor_task_exit;
    motor_request.bytes.a = 1;
    motor_request.bytes.d = 1;
    motor_request.m_valid = true;
    return motor_system_submit(unit_index, &motor_request) != 0;
}

// @address 0x379970 ?unit_action_takeoff_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_takeoff_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.bytes.a = source->bytes.a;
    update.m_force = false;
    update.action = _action_takeoff;
    update.m_task = (e_motor_task)(0xc);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x3799d0 ?unit_action_perch_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_perch_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.motion.value = source->motion.value;
    update.motion.position = source->motion.position;
    update.motion.velocity = source->motion.velocity;
    update.m_force = false;
    update.action = _action_perch;
    update.m_task = (e_motor_task)(13);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x379b10 ?unit_action_climb_attach_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_climb_attach_submit(long object_index, const action_request *source)
{
    c_motor_request motor_request;
    motor_request.action = _action_climb_attach;
    motor_request.m_task = _motor_task_attach;
    motor_request.attach.point = source->attach.point;
    motor_request.attach.vector = source->attach.vector;
    motor_request.attach.flag = source->attach.flag;
    motor_request.m_valid = true;
    return motor_system_submit(object_index, &motor_request) != _motor_result_failed;
}

// @address 0x379b90 ?unit_action_climb_detach_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_climb_detach_submit(long object_index, const action_request *source)
{
    c_motor_request r;
    r.m_force = false;
    r.aligned_vectors.b = source->motion.velocity;
    r.action = _action_climb_detach;
    r.m_task = (e_motor_task)(16);
    r.m_valid = true;
    r.aligned_vectors.a = source->motion.position;
    return motor_system_submit(object_index, &r) != 0;
}

// 0x379c18
// @address 0x379c18 ?unit_action_open_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_open_submit(long object_index, struct action_request const * source)
{
    c_motor_request update;
    update.bytes.a = source->bytes.a;
    update.bytes.b = source->bytes.b;
    update.m_force = false;
    update.action = _action_open;
    update.m_task = _motor_task_open;
    update.m_valid = true;
    return motor_system_submit(object_index, &update) == 2;
}

// @address 0x379c80 ?unit_action_close_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_close_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.bytes.a = source->bytes.a;
    update.bytes.b = source->bytes.b;
    update.m_force = false;
    update.action = _action_close;
    update.m_task = (e_motor_task)(24);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) == 2;
}

// @address 0x379ce8 ?unit_action_vehicle_flip_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_vehicle_flip_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.update.value = source->update.value;
    update.m_force = false;
    update.action = _action_vehicle_flip;
    update.m_task = (e_motor_task)(0x19);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x379d48 ?unit_action_wall_lean_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_wall_lean_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.vectors.value = source->vectors.value;
    update.vectors.position = source->vectors.position;
    update.vectors.velocity = source->vectors.velocity;
    update.m_force = false;
    update.action = _action_wall_lean;
    update.m_task = (e_motor_task)(18);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x379dd8 ?unit_action_wall_lean_open_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_wall_lean_open_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.vectors.value = source->vectors.value;
    update.vectors.position = source->vectors.position;
    update.vectors.velocity = source->vectors.velocity;
    update.m_force = false;
    update.action = _action_wall_lean_open;
    update.m_task = (e_motor_task)(17);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x37bcf0 ?unit_action_ai_impulse_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_ai_impulse_submit(long object_index, const action_request *source)
{
    bool result = false;
    float value = 0.0f;
    switch(source->animation.mode) { case 1: value = 0.1f; break; case 2: value = 0.2f; break; }
    c_motor_request motor;
    motor.setup_animation(source->action, 226, source->animation.animation, value,
        source->animation.has_vector ? &source->animation.vector : 0, source->animation.extra);
    result = motor_system_submit(object_index, &motor) != 0;
    if(result) {
        unit_datum *unit = ((unit_datum *)object_get_and_verify_type(object_index,8195));
        unsigned short *animation = (unsigned short *)object_header_block_get(
            object_get_and_verify_type(object_index, 8195), &unit->unit.animation_storage);
        *animation |= 1;
    }
    return result;
}

// @address 0x37bf38 unit_action_clear_target
extern "C" void unit_action_clear_target(long unit_index, long action)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    if ((1 << object_header_get(unit_index)->object_type) & 0x1003)
    {
        if (unit->unit.actor_index != -1 && !state->interaction.type)
            actor_hint_reserve(unit->unit.actor_index, state->interaction.hint, false);
    }
    state->interaction.clear();
}

// 0x37bfd8
// @address 0x37bfd8 unit_action_reset_target
extern "C" void unit_action_reset_target(long unit_index, long action)
{
    unit_action_clear_target(unit_index, action);
}

// @address 0x37cfd0 unit_action_infection_spew_finished
extern "C" void unit_action_infection_spew_finished(unsigned int object_index)
{
    unit_datum *datum = (unit_datum *)object_header_get(object_index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    if (state->spawner_index != -1) {
        swarm_spawner_end((short)state->spawner_index);
        state->spawner_index = -1;
    }
}

// @address 0x37d030 unit_action_finish_infection_spew
extern "C" void unit_action_finish_infection_spew(long unit_index, long action, long flags)
{
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    if (flags & 1) {
        mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
        swarm_spawner_spawn_creature((short)state->spawner_index);
    }
}

// 0x37d078
void unit_action_infection_spew_interrupt(long unit_index, e_action action)
{
    unit_action_infection_spew_finished(unit_index);
}

// @address 0x37d298 unit_action_reset_player_ticks
extern "C" void unit_action_reset_player_ticks(long handle)
{
    unit_datum *object = (unit_datum *)object_header_get(handle)->datum;
    long index = object->unit.actor_index;
    if (index == -1) return;
    actor_datum *player = datum_get_typed<actor_datum>(actor_data, index);
    if (((1 << player->type) & 0x700000) == 0) return;
    player->morph.morph_delay_ticks = (int)(((float)game_time_globals->ticks_per_second * 3.0f) + 0.5f);
    player = datum_get_typed<actor_datum>(actor_data, object->unit.actor_index);
    player->morph.pending_form = -1;
    player->morph.pending_form_priority = -1;
    player->morph.immediate_morph = 0;
    player->morph.force_morph = 0;
}

// 0x37d488
bool unit_action_jump_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    motor_request.setup_jump(31, request->jump_flag, request->jump.has_position ? &request->jump.position_yz : 0);
    return motor_system_submit(unit_index, &motor_request) != 0;
}

// @address 0x37d968 ?unit_action_leap_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_leap_submit(long object_index, const action_request *source)
{
    c_motor_request r;
    r.m_force = false;
    r.aligned_vectors.a = source->vectors.position;
    r.action = _action_leap;
    r.m_task = (e_motor_task)(42);
    r.m_valid = true;
    r.aligned_vectors.b = source->vectors.velocity;
    return motor_system_submit(object_index, &r) != 0;
}

// 0x37dc90
bool action_surge_submit(long unit_index, const action_request *request)
{
    bool result = false;
    c_motor_request motor_request;
    motor_request.setup_surge(48, request->surge.value);
    if (motor_system_submit(unit_index, &motor_request) != 0)
    {
        if (request->surge.has_vector)
        {
            unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
            mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
            state->surge.has_vector = true;
            state->surge.vector = request->surge.vector;
        }
        result = true;
    }
    return result;
}

static __forceinline bool actions_surge_actor_point_get(actor_datum *actor, real_point3d *point)
{
    bool result = false;
    if ((actor->flags & 0x2000000) != 0)
    {
        ::ai_point_get_position(&actor->surge_destination, point);
        result = true;
    }
    return result;
}

// @address 0x37dd50 ?action_surge_finished@@YAXJW4e_action@@@Z
void action_surge_finished(long unit_index, e_action action)
{
    bool has_point = false;
    float speed = 0.2f;
    real_point3d point;
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    datum->object.shield_vitality = 0.0f;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    if (((1 << object_header_get(unit_index)->object_type) & 0x1003) != 0)
    {
        if (datum->unit.actor_index != -1)
        {
            actor_datum *actor = datum_get_typed<actor_datum>(actor_data, datum->unit.actor_index);
            character_vitality_properties *vitality = character_definition_vitality_properties_get(actor->character_index);
            datum->object.shield_stun_ticks = (short)actions_seconds_to_ticks_round(vitality->surge_duration);
        }
    }
    if (state->surge.has_vector)
    {
        has_point = true;
        point = state->surge.vector;
    }
    else if (((1 << object_header_get(unit_index)->object_type) & 0x1003) != 0 && datum->unit.actor_index != -1)
    {
        actor_datum *actor = datum_get_typed<actor_datum>(actor_data, datum->unit.actor_index);
        character_guardian_properties *surge = 0;
        long tag_index = actor->character_index;
        while (tag_index != -1)
        {
            character_definition *definition = (character_definition *)tag_get(0, tag_index);
            if (definition->guardian_properties.count > 0)
            {
                surge = (character_guardian_properties *)definition->guardian_properties.address;
                break;
            }
            tag_index = definition->parent_character.index;
        }
        has_point = actions_surge_actor_point_get(actor, &point);
        if (has_point && surge != 0 && surge->phase_time > 0.0f)
            speed = surge->phase_time;
    }
    if (has_point)
    {
        action_request request;
        memset(&request, 0, sizeof(request));
        request.action = 49;
        request.surge_finished.point = point;
        request.surge_finished.value = speed;
        action_submit(unit_index, &request);
    }
}

// @address 0x37e260 ?action_posture_exit_submit@@YA_NJPBUaction_request@@@Z
bool action_posture_exit_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.bytes.b = source->bytes.b;
    update.bytes.c = source->bytes.a;
    update.action = _action_posture_exit;
    update.m_force = false;
    update.m_task = (e_motor_task)(0x20);
    update.bytes.a = 1;
    update.m_valid = true;
    update.bytes.d = 0;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x37e4e8 ?action_infect_submit@@YA_NJPBUaction_request@@@Z
bool action_infect_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.action = (e_action)(source->action);
    update.update.value = source->update.value;
    update.update.index = source->update.index;
    update.m_force = false;
    update.m_task = (e_motor_task)(0x16);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}

// @address 0x37e550 ?action_get_infected_submit@@YA_NJPBUaction_request@@@Z
bool action_get_infected_submit(long object_index, const action_request *source)
{
    c_motor_request update;
    update.action = (e_action)(source->action);
    update.update.value = source->update.value;
    update.m_force = false;
    update.m_task = (e_motor_task)(0x15);
    update.m_valid = true;
    return motor_system_submit(object_index, &update) != 0;
}


static __forceinline void action_interrupt(long index, e_action action)
{
    c_motor_task *task = motor_task_find_by_action(index, action);
    if(task) motor_system_interrupted(index, task, true);
}
// 0x37e2d0
// @address 0x37e2d0 ?action_custom_animation_submit@@YA_NJPBUaction_request@@@Z
bool action_custom_animation_submit(long unit_index, const action_request *request)
{
    bool result = false;
    if (request->play.graph != -1 && request->play.animation != -1 && request->play.animation != 0 &&
        motor_task_allow(unit_index, _action_custom_animation, 15))
    {
        mover_datum *object = ((mover_datum *)object_get_and_verify_type(unit_index, 8195));
        c_motor_program_state *state = (c_motor_program_state *)((char *)object + object->motor_program.offset);
        c_motor_request motor_request;
        motor_task_interrupt(unit_index, 15, 0);
        if (state->program != _motor_program_custom_animation) result = state->set_program(unit_index, _motor_program_custom_animation);
        else result = true;
        if (result)
        {
            motor_request.setup_play_animation(1, request->play.graph, request->play.animation,
                request->play.custom, request->play.loop, request->play.object_index,
                request->play.use_position ? &request->play.position : 0,
                request->play.use_position ? &request->play.forward : 0,
                request->play.interpolate, request->play.extra);
            result = motor_system_submit(unit_index, &motor_request) != 0;
        }
    }
    return result;
}

// 0x37c718
bool unit_action_deploy_turret_submit(long unit_index, const action_request *request)
{
    bool success = false;
    c_motor_request motor;
    motor.setup_animation(38, 226, 710, 0.2f, request->surge.has_vector ? &request->surge.vector : 0, false);
    if (motor_system_submit(unit_index, &motor) == _motor_result_success)
    {
        unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
        mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
        state->turret.deployed = 0;
        success = true;
    }
    return success;
}


// @address 0x37c800 ?unit_action_deploy_turret_update@@YA_NJW4e_action@@@Z
bool unit_action_deploy_turret_update(long unit_index, e_action action)
{
    bool finished = false;
    c_motor_task *task = motor_task_find_by_action(unit_index, _action_deploy_turret);
    if (task)
    {
        c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
        if (unit_animation_get_key_frame_status(unit_index, channel, true) == 1)
            finished = true;
    }
    else
        finished = true;
    if (finished)
        deploy_turret(unit_index);
    return !finished;
}

// @address 0x37c888 ?unit_action_deploy_turret_finished@@YAXJW4e_action@@@Z
void unit_action_deploy_turret_finished(long unit_index, e_action action)
{
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    if (!state->turret.deployed)
        deploy_turret(unit_index);
}

// 0x37c8c8
bool unit_action_flurry_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    motor_request.setup_animation(37, 138, 713, 0.2f, 0, false);
    return motor_system_submit(unit_index, &motor_request) != 0;
}

// 0x37c958
// @address 0x37c958 launch_emit_animation
extern "C" bool launch_emit_animation(long unit_index, mover_state *state)
{
    bool success = false;
    c_motor_request request;
    request.setup_animation(35, 226, 659, 0.0f, 0, false);
    if (motor_system_submit(unit_index, &request) != 0)
    {
        c_motor_task *task = motor_task_find_by_action(unit_index, _action_emit_ai);
        if (task && task->valid())
        {
            c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
            if (state->emit_ai.timer == -1)
            {
                float seconds = channel->get_authored_primary_key_time();
                if (seconds < 0.0f) state->emit_ai.timer = game_time_globals->ticks_per_second;
                else state->emit_ai.timer = (short)actions_seconds_to_ticks_round(seconds);
            }
            success = true;
        }
    }
    return success;
}


// @address 0x37ca98 unit_action_emit_ai_submit
extern "C" bool unit_action_emit_ai_submit(long index, const action_request *request)
{
    unit_datum *datum = (unit_datum *)object_header_get(index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    state->emit_ai.squad_index = request->emit_ai.squad_index;
    state->emit_ai.spawn_index = request->emit_ai.spawn_index;
    short location_index = request->emit_ai.location_index;
    state->emit_ai.timer = -1;
    state->emit_ai.location_index = location_index;
    c_motor_request motor;
    motor.m_force = false;
    motor.m_open.immediate = false;
    motor.m_open.pause = false;
    motor.action = _action_emit_ai;
    motor.m_task = _motor_task_open;
    motor.m_valid = true;
    bool result;
    unsigned long motor_result = motor_system_submit(index, &motor);
    if (motor_result >= _motor_result_pending)
    {
        if (motor_result != _motor_result_pending)
        {
            if (motor_result >= 3)
                result = false;
            else
                return true;
        }
        else
            result = launch_emit_animation(index, state);
    }
    else
        result = false;
    return result;
}


bool unit_action_emit_ai_update(long unit_index, e_action action)
{
    bool result = true;
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    if (state->emit_ai.timer > 0 && --state->emit_ai.timer <= 0)
    {
        bool suppress_squad = (datum->object.damage_flags >> 2) & 1;
        if (!suppress_squad)
            squad_create(state->emit_ai.squad_index, state->emit_ai.spawn_index, state->emit_ai.location_index, 1, true, false, false);
        result = false;
        state->emit_ai.squad_index = -1;
        state->emit_ai.location_index = -1;
    }
    return result;
}

// 0x37cc20
void unit_action_emit_ai_finished(long unit_index, e_action action)
{
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    launch_emit_animation(unit_index, state);
}

// @address 0x37cc50 ?unit_action_resurrect_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_resurrect_submit(long index, const action_request *request)
{
    c_motor_request motor_request;
    bool success=false;
    action_interrupt(index,_action_death_ping);
    motor_request.setup_resurrect(5,request->resurrect.second,request->resurrect.first);
    if(motor_system_submit(index,&motor_request)!=0) {
        if((1<<object_header_get(index)->object_type)&1) {
            if((unsigned long)(request->resurrect.second-3)<=1) {
                mover_datum *object=((mover_datum *)object_get_and_verify_type(index,1));
                biped_definition *definition=(biped_definition *)tag_get(0, object->object.definition_index);
                if(definition->reanimation_morph_muffins.index!=-1) widget_type_new_on_object(index,3,definition->reanimation_morph_muffins.index);
            }
        }
        success=true;
    }
    return success;
}


// @address 0x37d240 ?unit_action_morph_finished@@YAXJW4e_action@@@Z
void unit_action_morph_finished(long unit_index, e_action action)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    if (state->morph.definition_index != -1) {
        long actor_index = -1;
        if (!state->morph.change_team)
            actor_index = unit->unit.actor_index;
        actor_handle_morph(actor_index, unit_index, state->morph.definition_index, state->morph.b);
    }
}


// The unit's old private view of the manager had this as reset_interpolation(); c_interpolator_control::disable()
// stores m_duration before m_flags, so the private store order is kept here.
static __forceinline void actions_reset_interpolation(c_animation_manager *object)
{
    object->m_interpolator.m_time = 0.0f;
    object->m_interpolator.m_flags = 0;
    object->m_interpolator.m_duration = 0.0f;
}
static __forceinline void actions_object_reset_interpolation(long object_index)
{
    object_datum *unit = (object_datum *)object_header_get(object_index)->datum;
    c_animation_manager *animation_manager = (c_animation_manager *)((char *)unit + unit->animation.offset);
    actions_reset_interpolation(animation_manager);
    object_wake(object_index);
}

// @address 0x37d370 ?unit_action_morph_arrival_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_morph_arrival_submit(long unit_index, const action_request *request)
{
    long animation = -1;
    c_motor_request motor_request;
    bool success = false;
    switch (request->animation.mode)
    {
        case 0: animation = 850; break;
        case 1: animation = 852; break;
        case 2: animation = 848; break;
        default: animation = 855; break;
    }
    if (animation != -1)
    {
        motor_request.setup_animation(9, 138, animation, 0.2f, 0, false);
        if (motor_system_submit(unit_index, &motor_request) != 0)
        {
            actions_object_reset_interpolation(unit_index);
            object_compute_node_matrices(unit_index);
            success = true;
        }
    }
    return success;
}

// @address 0x37e160 ?action_posture_submit@@YA_NJPBUaction_request@@@Z
bool action_posture_submit(long index, const action_request *request) {
    long result=0;
    c_motor_request motor_request;
    real_point3d position;
    real_vector3d forward;
    if(request->posture.has_position) { position=request->posture.position; forward=request->posture.forward; }
    else { object_get_origin(index,&position); ::object_get_orientation(index, &forward, 0); }
    motor_request.setup_posture(request->action,request->posture.posture,&position,&forward,request->posture.a,request->posture.b);
    result=motor_system_submit(index,&motor_request);
    return result!=0;
}

// ---------------------------------------------------------------------------
// Shared declarations for the vehicle board, melee, transient posture, hoist,
// leap and phase actions.


class c_model_animation;

// 0x379160





static __forceinline void actions_board_passenger_iterator_new(long unit_index, unit_passenger_iterator *iterator)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    iterator->unit_index = unit_index;
    iterator->passenger_index = -1;
    iterator->seat_index = -1;
    iterator->next_object_index = unit->object.first_child_object_index;
}

static __forceinline void actions_board_setup_request(c_motor_request *request, long action)
{
    request->m_force = false;
    request->m_valid = true;
    request->action = (e_action)action;
    request->m_task = _motor_task_board;
}

static __forceinline bool actions_board_is_authoritative()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return !result;
}

// 0x379160




// @address 0x378c68 ?unit_action_vehicle_entry_update@@YA_NJW4e_action@@@Z
bool unit_action_vehicle_entry_update(long unit_index, e_action action)
{
    s_object_header *header = object_header_get(unit_index);
    unit_datum *unit = (unit_datum *)header->datum;
    if (((1 << header->object_type) & 1) != 0)
    {
        if (game_ticks_to_seconds((float)unit->unit.seat_impulse_ticks) < 0.2f)
        {
            biped_impulse_vehicle_seat(unit_index, _biped_seat_impulse_exit);
            unit->unit.seat_impulse_ticks++;
        }
    }
    return true;
}

// @address 0x378f20 ?unit_action_vehicle_exit_update@@YA_NJW4e_action@@@Z
bool unit_action_vehicle_exit_update(long unit_index, e_action action)
{
    s_object_header *header = object_header_get(unit_index);
    unit_datum *unit = (unit_datum *)header->datum;
    if (((1 << header->object_type) & 1) != 0)
    {
        if (game_ticks_to_seconds((float)unit->unit.seat_impulse_ticks) < 0.2f)
        {
            biped_impulse_vehicle_seat(unit_index, _biped_seat_impulse_enter);
            unit->unit.seat_impulse_ticks++;
        }
    }
    return true;
}


// 0x3794b0


static __forceinline bool actions_acceleration_prevents_action(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    return (unit->unit.unit_flags & 0x01000000) != 0;
}
// match: 0x379160, 0x3794b0: return a normalized bit, as the original seat flags test did.
static bool actions_seat_test_flag(const unit_seat *seat, e_unit_seat_flags bit)
{
    return ((seat->flags >> bit) & 1) != 0;
}
static __forceinline bool actions_weapon_prevents_melee(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)object_header_get(weapon_index)->datum;
    weapon_definition *definition = (weapon_definition *)tag_get(0, weapon->definition_index);
    return (bool)(_rotl(definition->flags, 23) & 1) || (bool)(_rotl(definition->flags, 19) & 1);
}
static __forceinline bool actions_weapon_overcharged(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)object_header_get(weapon_index)->datum;
    return weapon->triggers[0].state == 1 || weapon->triggers[0].state == 2;
}

// @address 0x379e68 ?unit_action_melee_ai_submit@@YA_NJPBUaction_request@@@Z



// 0x37aa50


static __forceinline long actions_melee_object_timeout(biped_datum *object)
{
    long result;
    bool enabled = object->biped.physics.get_mode_melee()->is_initialized();
    if (enabled)
    {
        result = object->biped.physics.get_mode_melee()->get_ticks_remaining();
    }
    else
    {
        result = 30;
    }
    return result;
}


// 0x37acb0



// 0x37be20



// 0x37bfe0


// 0x37d7f0


static __forceinline float actions_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i*b->i + (b->j*a->j + b->k*a->k);
}

// @address 0x37d9f0 ?unit_action_leap_recovery_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_leap_recovery_submit(long unit_index, const action_request *request)
{
    bool success = false;
    c_motor_request motor_request;
    real_vector3d velocity = request->leap_recovery.velocity;
    real_vector3d normal = request->leap_recovery.normal;

    motor_request.setup_leap_recovery(34, 226, 872, k_standard_interpolation_time, 0);
    if (motor_system_submit(unit_index, &motor_request) != 0)
        success = true;
    else
    {
        motor_request.setup_leap_recovery(34, 188, 872, k_standard_interpolation_time, 0);
        if (motor_system_submit(unit_index, &motor_request) != 0)
            success = true;
    }

    if (success && ((1 << object_header_get(unit_index)->object_type) & 1) != 0)
    {
        c_motor_task *task = motor_task_find_by_action(unit_index, _action_leap_recovery);
        if (task)
        {
            c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
            if (channel)
            {
                mover_datum *unit = ((mover_datum *)object_get_and_verify_type(unit_index, 1));
                biped_definition *definition = (biped_definition *)tag_get(0, unit->object.definition_index);
                if (definition->leap_recovery_upper > 0.0f)
                {
                    float speed = actions_dot_product3d(&normal, &velocity);
                    speed = (float)fabs(speed);
                    if (speed < definition->leap_recovery_upper)
                    {
                        float range = definition->leap_recovery_upper - definition->leap_recovery_lower;
                        float ratio = 0.0f;
                        if (speed < definition->leap_recovery_lower)
                            ratio = 1.0f;
                        else if (range > 0.0001f)
                        {
                            float t = (speed - definition->leap_recovery_lower) / range;
                            t = (t > 0.0f ? t : 0.0f) < 1.0f ? (t > 0.0f ? t : 0.0f) : 1.0f;
                            ratio = 1.0f - t;
                        }
                        if (ratio > 0.0001f)
                        {
                            channel->enable_looping(false);
                            channel->set_frame_position_from_playback_ratio(ratio);
                        }
                    }
                }
            }
        }
    }
    return success;
}

// @address 0x37dc10 unit_action_equipment_pickup_submit
extern "C" int unit_action_equipment_pickup_submit(long unit_index, const s_actions_equipment_pickup_request *request)
{
    ::unit_drop_current_equipment(unit_index);
    if (request->equipment.v != -1)
    {
        // match: 0x37dc10: retain the aggregate ABI view for the word rotate.
        typedef void (*equipment_inventory_call)(long, s_actions_single_value);
        reinterpret_cast<equipment_inventory_call>(&unit_add_equipment_to_inventory)(unit_index, request->equipment);
    }
    return 1;
}




bool unit_action_vehicle_board_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    bool result = false;

    if (unit->object.parent_object_index != -1)
    {
        unit_datum *parent_object = (unit_datum *)object_header_get(unit->object.parent_object_index)->datum;
        if (((1 << parent_object->object.object_identifier.get_type()) & 0x1003) != 0)
        {
            long parent_index = unit->object.parent_object_index;
            unit_definition *parent_definition =
                (unit_definition *)tag_get(0, parent_object->object.definition_index);
            unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
                &parent_definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));

            if (!action_executing(unit_index, _action_vehicle_board) &&
                actions_seat_test_flag(seat, _unit_seat_boarding_seat_bit) &&
                seat->index != -1 &&
                unit_allowed_to_launch_board(unit_index, parent_index, seat->index))
            {
                c_motor_request motor_request;
                actions_board_setup_request(&motor_request, 16);
                if (motor_system_submit(unit_index, &motor_request) == 2)
                {
                    unit->unit.seat_exit_queued = false;
                    simulation_action_unit_board_vehicle(unit_index);

                    long occupant_index = ::unit_seat_get_occupant(parent_index, seat->index);
                    if (occupant_index != -1 && unit->unit.player_index != -1)
                    {
                        long player_index = unit->unit.player_index;
                        long player_absolute_index = player_index & 0xffff;
                        e_vehicle_type vehicle_type = _vehicle_type_none;
                        long object_index = parent_index;
                        long event_index = -1;
                        while (object_index != -1)
                        {
                            unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
                            if (((1 << object->object.object_identifier.get_type()) & 0x1003) != 0)
                                vehicle_type = vehicle_get_type(object_index);
                            object_index = object->object.parent_object_index;
                        }

                        if (((1 << vehicle_type) & 0x10b) != 0)
                        {
                            long event = 0;
                            ::game_engine_handle_time_messages(35, player_absolute_index, player_index, -1, (int *)(&event));
                            event_index = 52;
                        }
                        if (((1 << vehicle_type) & 0x94) != 0)
                        {
                            long event = 0;
                            ::game_engine_handle_time_messages(36, player_absolute_index, player_index, -1, (int *)(&event));
                            event_index = 53;
                        }
                        if (event_index != -1 && actions_board_is_authoritative())
                        {
                            unit_datum *occupant = (unit_datum *)object_header_get(occupant_index)->datum;
                            if (((1 << occupant->object.object_identifier.get_type()) & 0x1003) != 0 && occupant->unit.player_index != -1)
                            {
                                s_game_engine_event_data event_data;
                                game_engine_initialize_event(0, event_index, &event_data);
                                long cause_player_index = unit->unit.player_index;
                                event_data.cause_player_index = cause_player_index;
                                event_data.cause_team_index = datum_get_typed<player_datum>(player_data, cause_player_index)->configuration.host.team_index;
                                long effect_player_index = occupant->unit.player_index;
                                event_data.effect_player_index = effect_player_index;
                                event_data.effect_team_index = datum_get_typed<player_datum>(player_data, effect_player_index)->configuration.host.team_index;
                                game_engine_send_event(&event_data);
                            }
                        }
                    }

                    if (actions_seat_test_flag(seat, _unit_seat_boarding_must_open_target_seat_bit))
                    {
                        unit_seat *boarding_seat = (unit_seat *)tag_block_get_element_with_size(
                            &parent_definition->seats, seat->index, sizeof(unit_seat));
                        if (actions_seat_test_flag(boarding_seat, _unit_seat_driver_bit))
                        {
                            unit_seat_try_opening(parent_index, seat->index, false, false, false);
                            if (unit->unit.player_index != -1)
                                player_set_facing(unit->unit.player_index, &parent_object->unit.facing_vector);
                        }
                    }
                    result = true;
                }
            }

            if (result)
                unit_update_driver_and_gunner(parent_index);
        }
    }

    return result;
}

bool unit_action_vehicle_board_update(long unit_index, e_action action)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    bool finished = false;

    if ((1 & (1 << object_header_get(unit_index)->object_type)) != 0)
    {
        if (game_ticks_to_seconds((float)unit->unit.seat_impulse_ticks) < 0.2f)
        {
            biped_impulse_vehicle_seat(unit_index, _biped_seat_impulse_exit);
            unit->unit.seat_impulse_ticks++;
        }
    }

    e_unit_animation_key_frame_status key_frame_status = unit_animation_get_key_frame_status(unit_index, true);
    if (key_frame_status != _unit_animation_key_frame_status_after)
    {
        unit_evict_parent_seat_occupant(unit_index, false, false);
        finished = true;
    }

    if (!finished)
    {
        bool board_allowed = false;
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_datum *parent = (unit_datum *)object_header_get(unit->object.parent_object_index)->datum;
            unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = (unit_seat *)tag_block_get_element_with_size(
                &parent_definition->seats, unit->unit.parent_seat_index, sizeof(unit_seat));
            if (actions_seat_test_flag(seat, _unit_seat_boarding_seat_bit))
            {
                if (seat->index != -1)
                {
                    unit_passenger_iterator iterator;
                    actions_board_passenger_iterator_new(unit->object.parent_object_index, &iterator);
                    unit_datum *passenger;
                    while ((passenger = ::unit_passenger_iterator_next(&iterator)) != 0)
                    {
                        if (iterator.seat_index == seat->index)
                        {
                            if (game_team_is_enemy(unit->unit.team, passenger->unit.team))
                                board_allowed = true;
                            break;
                        }
                    }
                }
                else
                {
                    board_allowed = true;
                }
            }
            if (!board_allowed)
                action_interrupt(unit_index, _action_vehicle_board);
        }
    }

    return true;
}

// @address 0x3796a0 ?unit_action_vehicle_ejection_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_vehicle_ejection_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    bool result = false;

    if (unit->object.parent_object_index != -1 && (unsigned short)unit->unit.parent_seat_index != 0xffff)
    {
        unit_datum *parent_unit = (unit_datum *)object_header_get(unit_index)->datum;
        long parent_index = (long)parent_unit->object.parent_object_index;
        if (request->bytes.b != 0)
            action_interrupt(unit_index, _action_vehicle_ejection);

        c_motor_request motor_request;
        motor_request.m_force = false;
        motor_request.bytes.a = request->bytes.a;
        motor_request.bytes.b = request->bytes.b;
        motor_request.action = _action_vehicle_ejection;
        motor_request.m_task = _motor_task_ejection;
        motor_request.m_valid = true;
        if (motor_system_submit(unit_index, &motor_request) != 0)
        {
            unit_update_driver_and_gunner(parent_index);
            result = true;
        }
    }
    return result;
}

bool unit_action_melee_ai_submit(long unit_index, const action_request *request)
{
    bool result = false;
    if (!motor_animation_busy(unit_index) && !actions_acceleration_prevents_action(unit_index))
    {
        unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        long state = motor_animation_get_state(unit_index);
        long melee_action = -1;
        bool airborne = motor_system_in_airborne_state(unit_index);
        if ((bool)(_rotl(definition->flags, 24) & 1)) melee_action = 80;
        else if (state == 77) melee_action = 78;
        else if (!airborne && state != 31)
        {
            switch (request->melee_ai.melee_type)
            {
            case 0: melee_action = 10; break;
            case 1: melee_action = 599; break;
            case 4: melee_action = 600; break;
            case 5: melee_action = 601; break;
            case 2: melee_action = 664; break;
            case 3: melee_action = 736; break;
            }
        }
        else melee_action = 70;
        c_motor_request motor_request;
        motor_request.setup_melee(request->action, melee_action, request->melee_ai.has_vector ? &request->melee_ai.vector : 0, true);
        if (motor_system_submit(unit_index, &motor_request) != 0) result = true;
    }
    return result;
}

// @address 0x29208 k_player_melee_strikes
// @rdata 0x82029208..0x82029238
const s_unit_action_player_melee_strike k_player_melee_strikes[4] = {
    { _string_id_melee_strike_1, 34, _string_id_melee_strike_1 },
    { _string_id_melee_strike_2, 35, _string_id_melee_strike_2 },
    { _string_id_melee_strike_3, 36, _string_id_melee_strike_3 },
    { _string_id_melee_strike_4, 37, _string_id_melee_strike_4 },
};
// @rdata-end

// @address 0x37a0c0 ?unit_action_melee_player_choose_strike@@YAPBUs_unit_action_player_melee_strike@@J@Z
const s_unit_action_player_melee_strike *unit_action_melee_player_choose_strike(long unit_index)
{
    const s_unit_action_player_melee_strike *result = 0;
    long strike_indices[4];
    long strike_count = 0;
    long strike_index = 0;
    long *strike_index_slot = strike_indices;
    const long *animation = &k_player_melee_strikes[0].animation;
    unsigned int remaining = 4;
    do
    {
        if (weapon_get_deterministic_first_person_animation(unit_index, *animation, 0) != 0)
        {
            *strike_index_slot = strike_index;
            strike_count++;
            strike_index_slot++;
        }
        strike_index++;
        animation += 3;
        remaining--;
    } while (remaining != 0);

    if (strike_count > 0)
    {
        short strike_num = random_range(tls_random_seed(), 0, (short)strike_count);
        result = &k_player_melee_strikes[strike_indices[strike_num]];
    }
    return result;
}


static __forceinline bool actions_weapon_disables_dash(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)object_header_get(weapon_index)->datum;
    weapon_definition *definition =
        (weapon_definition *)tag_get(0, weapon->definition_index);
    return (definition->secondary_flags >> 2) & 1;
}
static __forceinline const c_model_animation *actions_channel_get_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = 0;
    if (channel->m_animation_id.m_animation_index >= 0)
    {
        animation = ((const c_model_animation_graph *)tag_get(0, channel->m_graph_index))->get_animation(channel->m_animation_id);
    }
    return animation;
}
static __forceinline void actions_unit_interrupt_reload(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    long primary_weapon = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_indices[0]);
    long secondary_weapon = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_indices[1]);
    bool interrupted_primary = false;
    bool interrupted_secondary = false;
    if (primary_weapon != -1 && weapon_reloading(primary_weapon, -1, 0))
    {
        weapon_interrupt_reload(primary_weapon, false);
        interrupted_primary = true;
    }
    if (secondary_weapon != -1 && weapon_reloading(secondary_weapon, -1, 0))
    {
        weapon_interrupt_reload(secondary_weapon, false);
        interrupted_secondary = true;
    }
    if (interrupted_primary || interrupted_secondary)
    {
        simulation_action_unit_reload_interrupt(unit_index, interrupted_primary, interrupted_secondary);
    }
}

// @address 0x37a1a0 unit_action_start_melee_strike
extern "C" bool unit_action_start_melee_strike(long unit_index, long melee_type, long target_object_index, unsigned char target_is_forced, long weapon_slot)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    s_player_melee_action_state *state = (s_player_melee_action_state *)((char *)unit + unit->unit.action_offset);
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_indices[0]);
    bool weapon_disables_dash = false;
    if (weapon_index != -1)
    {
        weapon_disables_dash = actions_weapon_disables_dash(weapon_index);
    }
    bool is_biped = ((1 << unit->object.object_type) & 1) != 0;
    long melee_animation = -1;
    e_unit_messages unit_message = _unit_message_none;
    long first_person_animation = -1;
    bool success = false;

    long current_animation = action_executing(unit_index, _action_melee_player) ? state->animation : -1;
    switch (current_animation)
    {
    case -1:
    case _string_id_melee_strike_1:
    case _string_id_melee_strike_2:
    case _string_id_melee_strike_3:
    case _string_id_melee_strike_4:
        if (melee_type == 2)
        {
            if (is_biped && motor_system_in_airborne_state(unit_index))
            {
                melee_animation = _string_id_melee_dash_airborne;
                unit_message = _unit_message_melee_dash_airborne;
                first_person_animation = _string_id_melee_dash_airborne;
            }
            else
            {
                melee_animation = _string_id_melee_dash;
                unit_message = _unit_message_melee_dash;
                first_person_animation = _string_id_melee_dash;
            }
        }
        if (melee_type == 3)
        {
            if (is_biped && motor_system_in_airborne_state(unit_index))
            {
                melee_animation = _string_id_melee_lunge_unaimed_airborne;
                unit_message = _unit_message_melee_lunge_unaimed_airborne;
                first_person_animation = _string_id_melee_lunge_unaimed_airborne;
            }
            else
            {
                melee_animation = _string_id_melee_lunge_unaimed;
                unit_message = _unit_message_melee_lunge_unaimed;
                first_person_animation = _string_id_melee_lunge_unaimed;
            }
        }
        else if (melee_type == 1)
        {
            if (weapon_index != -1)
            {
                const s_unit_action_player_melee_strike *strike = unit_action_melee_player_choose_strike(weapon_index);
                if (strike != 0)
                {
                    melee_animation = strike->animation_name;
                    unit_message = (e_unit_messages)strike->unit_message;
                    first_person_animation = strike->animation;
                }
            }
            if (melee_animation == -1 && weapon_index != -1 &&
                weapon_get_deterministic_first_person_animation(weapon_index, _string_id_melee_1sthit, 0) != 0)
            {
                melee_animation = _string_id_melee_1sthit;
                unit_message = _unit_message_melee_1st_hit;
                first_person_animation = _string_id_melee_1sthit;
            }
            if (melee_animation == -1)
            {
                melee_animation = _string_id_melee;
                unit_message = _unit_message_melee;
                first_person_animation = _string_id_melee;
            }
        }
        break;
    case _string_id_melee_1sthit:
        if (melee_type == 1)
        {
            melee_animation = _string_id_melee_2ndhit;
            unit_message = _unit_message_melee_2nd_hit;
            first_person_animation = _string_id_melee_2ndhit;
        }
        else
        {
            melee_animation = _string_id_melee_1sthit2idle;
            unit_message = _unit_message_melee_1st_hit_to_idle;
        }
        break;
    case _string_id_melee_2ndhit:
        if (melee_type == 1)
        {
            unit_message = _unit_message_melee_3rd_hit;
            melee_animation = _string_id_melee_3rdhit;
            first_person_animation = _string_id_melee_3rdhit;
        }
        else
        {
            melee_animation = _string_id_melee_2ndhit2idle;
            unit_message = _unit_message_melee_2nd_hit_to_idle;
        }
        break;
    case _string_id_melee_dash:
        melee_animation = _string_id_melee_lunge;
        unit_message = _unit_message_melee_lunge;
        first_person_animation = _string_id_melee_lunge;
        break;
    case _string_id_melee_dash_airborne:
        melee_animation = _string_id_melee_lunge_airborne;
        unit_message = _unit_message_melee_lunge_airborne;
        first_person_animation = _string_id_melee_lunge_airborne;
        break;
    }

    if (melee_animation != -1)
    {
        c_motor_request motor_request;
        bool submitted = false;
        motor_request.setup_melee(_action_melee_player, melee_animation, 0, false);
        if (motor_system_submit(unit_index, &motor_request) != _motor_result_failed)
        {
            submitted = true;
        }
        else
        {
            switch (melee_animation)
            {
            case _string_id_melee_strike_1:
                motor_request.setup_melee(_action_melee_player, _string_id_melee, 0, false);
                submitted = motor_system_submit(unit_index, &motor_request) != _motor_result_failed;
                break;
            case _string_id_melee_1sthit:
            case _string_id_melee_2ndhit:
            case _string_id_melee_3rdhit:
            case _string_id_melee_strike_2:
            case _string_id_melee_strike_3:
            case _string_id_melee_strike_4:
                motor_request.setup_melee(_action_melee_player, _string_id_melee_strike_1, 0, false);
                if (motor_system_submit(unit_index, &motor_request) == _motor_result_failed)
                {
                    motor_request.setup_melee(_action_melee_player, _string_id_melee, 0, false);
                    submitted = motor_system_submit(unit_index, &motor_request) != _motor_result_failed;
                }
                else
                {
                    submitted = true;
                }
                break;
            }
        }

        if (submitted)
        {
            if (melee_animation == _string_id_melee_dash || melee_animation == _string_id_melee_dash_airborne)
            {
                if (is_biped)
                {
                    if (!target_is_forced)
                    {
                        long auto_target_index = unit_get_melee_target(unit_index);
                        if (auto_target_index != -1)
                            target_object_index = auto_target_index;
                    }
                    if (target_object_index != -1 && target_object_index != unit_index)
                    {
                        c_motor_request dash_request;
                        dash_request.setup_dash(-1, target_object_index, -1, true);
                        motor_system_submit(unit_index, &dash_request);
                    }
                }
                state->primary_key_ticks = -1;
                state->secondary_key_ticks = -1;
                state->total_ticks = (signed char)actions_seconds_to_ticks_round(2.0f);
                success = true;
            }
            else
            {
                const c_model_animation *animation = 0;
                if (weapon_index != -1 && first_person_animation != -1)
                {
                    animation = weapon_get_deterministic_first_person_animation(weapon_index, first_person_animation, 0);
                }
                if (animation == 0)
                {
                    c_motor_task *task = motor_task_find_by_action(unit_index, _action_melee_player);
                    if (task != 0)
                    {
                        c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
                        if (channel != 0)
                        {
                            animation = actions_channel_get_animation(channel);
                        }
                    }
                }
                if (animation != 0)
                {
                    long primary_key_frame = animation->get_primary_key_frame_index();
                    long secondary_key_frame = animation->get_secondary_key_frame_index();
                    long frame_count = animation->get_frame_count();
                    if (primary_key_frame != -1)
                    {
                        state->primary_key_ticks = (signed char)actions_seconds_to_ticks_round((float)primary_key_frame / 30.0f);
                    }
                    else
                    {
                        state->primary_key_ticks = -1;
                    }
                    if (secondary_key_frame != -1)
                    {
                        state->secondary_key_ticks = (signed char)actions_seconds_to_ticks_round((float)secondary_key_frame / 30.0f);
                    }
                    else
                    {
                        state->secondary_key_ticks = -1;
                    }
                    state->total_ticks = (signed char)actions_seconds_to_ticks_round((float)frame_count / 30.0f);
                    if (is_biped && target_object_index != -1 && target_object_index != unit_index && !weapon_disables_dash)
                    {
                        c_motor_request dash_request;
                        dash_request.setup_dash(-1, target_object_index, state->primary_key_ticks, false);
                        motor_system_submit(unit_index, &dash_request);
                    }
                    success = true;
                }
            }
        }

        if (success)
        {
            state->animation = melee_animation;
            state->weapon_slot = (signed char)weapon_slot;
            state->intensity = 0;
            state->ticks = 0;
            state->started = false;
            state->silent = false;
            s_game_globals *game_globals = (s_game_globals *)global_game_globals;
            long player_information_index = 0;
            const s_player_information_definition *player_information =
            &((const s_player_information_definition *)game_globals->player_information.address)[player_information_index];
            if (player_information->run_forward > 0.0f)
            {
                float forward_speed = dot_product3d(&unit->object.forward, &unit->object.linear_velocity);
                float speed_fraction = forward_speed / player_information->run_forward;
                if (speed_fraction > 0.9f)
                {
                    state->intensity = 127;
                }
            }
            if (unit->object.object_type == 0 && motor_system_in_airborne_state(unit_index))
            {
                state->intensity = 255;
            }
            if (((1 << unit->object.object_type) & 0x1003) != 0)
            {
                actions_unit_interrupt_reload(unit_index);
            }
            unit_action_send_message_to_first_person(unit_index, unit_message);
        }
    }

    return success;
}

bool unit_action_melee_player_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    s_player_melee_action_state *state = (s_player_melee_action_state *)((char *)unit + unit->unit.action_offset);
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index);
    bool can_start = true;
    bool result = false;
    if (weapon_index != -1 && (actions_weapon_prevents_melee(weapon_index) ||
        (!weapon_uses_charging_melee(weapon_index) && actions_weapon_overcharged(weapon_index))))
        can_start = false;
    if (actions_acceleration_prevents_action(unit_index) || !motor_task_allow(unit_index, _action_melee_player, 4))
        can_start = false;
    if (action_executing(unit_index, _action_melee_player) &&
        (state->animation == -1 || state->secondary_key_ticks == -1 || state->ticks < state->secondary_key_ticks))
        can_start = false;
    if (unit->unit.player_index != -1)
    {
        player_datum *player = datum_get_typed<player_datum>(player_data, unit->unit.player_index);
        if (player->melee_remaining_ticks > 0 && request->melee_player.flag == 0) can_start = false;
    }
    if (can_start)
    {
        action_submit_event(unit_index, 47);
        ::unit_unzoom(unit_index, 0);
        if (unit_action_start_melee_strike(unit_index, request->melee_player.type, request->melee_player.target,
            request->melee_player.mode, request->melee_player.weapon_slot))
        {
            state->last_melee_time = -1;
            simulation_action_unit_melee_initiate(unit_index, request->melee_player.target, request->melee_player.type, request->melee_player.weapon_slot);
            ::unit_active_camouflage_ding(unit_index, 0.6f, 0.0f);
            result = true;
        }
    }
    return result;
}

bool unit_action_melee_player_update(long object_index, e_action action)
{
    bool result = false;
    biped_datum *object = (biped_datum *)object_header_get(object_index)->datum;
    s_player_melee_action_state *state = (s_player_melee_action_state *)((char *)object + object->unit.action_offset);
    long animation = state->animation;
    if (animation != -1)
    {
        int start = state->primary_key_ticks;
        unsigned char done;
        if (start >= 5)
        {
            start = 5;
        }
        done = 0;
        if (state->ticks == start && !state->started)
        {
            int remaining = 0;
            if (state->total_ticks - start >= 0)
            {
                remaining = state->total_ticks - start;
            }
            if (!state->silent)
            {
                unit_cause_player_melee_attack(object_index, animation, state->weapon_slot, (float)state->intensity * 0.003921569f);
            }
            unit_set_player_melee_remaining(object_index, remaining);
            state->started = 1;
        }
        if (state->animation == 604 || state->animation == 605)
        {
            biped_datum *self = (biped_datum *)object_header_get(object_index)->datum;
            int limit = (int)((float)game_time_globals->ticks_per_second * 0.067f + 0.5f);
            if (self->biped.physics.m_mode == 6)
            {
                long target_index = self->biped.physics.get_mode_melee()->get_target_unit_index();
                long timeout = actions_melee_object_timeout(self);
                if (target_index != -1)
                {
                    s_object_header *header = object_header_get(target_index);
                    if (!header->object_type)
                    {
                        biped_datum *target = (biped_datum *)header->datum;
                        if (target->biped.physics.m_mode == 6 && target->biped.physics.get_mode_melee()->get_target_unit_index() == object_index)
                        {
                            long target_timeout = actions_melee_object_timeout(target);
                            if (timeout > target_timeout)
                            {
                                timeout = target_timeout;
                            }
                        }
                    }
                }
                if (timeout < 0 || timeout <= limit)
                {
                    done = 1;
                }
            }
            else
            {
                done = 1;
            }
        }
        state->ticks++;
        if (state->ticks >= state->total_ticks || done)
        {
            result = unit_action_start_melee_strike(object_index, 0, -1, 0, state->weapon_slot);
        }
        else
        {
            result = 1;
        }
    }
    if (!result)
    {
        state->animation = -1;
    }
    return result;
}

bool action_vault_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    bool success = false;
    motor_request.setup_transient_posture(40, 279, 84, &request->position, &request->alignment);
    if (motor_system_submit(unit_index, &motor_request) != 0)
    {
        unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
        mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
        state->hoist_target = request->hoist.reference;
        if (((1 << unit->object.object_type) & 0x1003) != 0)
            unit_dialogue_test_scream(unit_index, 17, -1, 11, false);
        success = true;
    }
    return success;
}

bool action_hoist_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    bool success = false;
    real_point3d origin;
    object_get_origin(unit_index, &origin);
    long height = ((long)::ai_hint_get_best_hoist_height(unit_index, request->position.z - origin.z));
    motor_request.setup_hoist(request->action, height, &request->position, &request->alignment);
    if (motor_system_submit(unit_index, &motor_request) != 0)
    {
        unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
        mover_state *state = (mover_state *)((char *)unit + unit->unit.action.offset);
        state->hoist_target = request->hoist.reference;
        success = true;
    }
    return success;
}




// @address 0x37d510 ?leap_anticipation_submit@@YA_NJPBUaction_request@@@Z
bool leap_anticipation_submit(long unit_index, const action_request *request)
{
    bool success = false;
    if (((1 << object_header_get(unit_index)->object_type) & 1) != 0)
    {
        biped_datum *biped = (biped_datum *)((mover_datum *)object_get_and_verify_type(unit_index, 1));
        biped->biped.leap_destination = request->leap_anticipation.destination;
        biped->biped.leap_velocity = request->leap_anticipation.velocity;

        real_point3d origin;
        object_get_origin(unit_index, &origin);

        real_vector3d direction;
        vector_from_points3d(&origin, &biped->biped.leap_destination, &direction);
        if (normalize3d(&direction) > 0.0f)
        {
            c_motor_request motor_request;
            motor_request.setup_leap_anticipation(_action_leap_anticipation, 226, 871, k_standard_interpolation_time, &direction);
            if (motor_system_submit(unit_index, &motor_request) != 0)
            {
                success = true;
            }
            else
            {
                motor_request.setup_leap_anticipation(_action_leap_anticipation, 188, 871, k_standard_interpolation_time, &direction);
                if (motor_system_submit(unit_index, &motor_request) != 0)
                {
                    success = true;
                }
            }
        }

        if (!success)
        {
            action_request leap_request;
            memcpy(&leap_request, request, sizeof(leap_request));
            leap_request.action = 33;
            action_submit(unit_index, &leap_request);
        }
        else
        {
            c_motor_task *task = motor_task_find_by_action(unit_index, _action_leap_anticipation);
            if (task)
            {
                c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
                if (channel)
                {
                    biped_definition *definition = (biped_definition *)tag_get(0, biped->object.definition_index);
                    if (definition->anticipation_upper > 0.0f)
                    {
                        real_vector3d linear_velocity;
                        object_get_velocities(unit_index, &linear_velocity, 0);
                        float speed = 0.0f;
                        real_vector3d leap_velocity;
                        float required_speed;
                        if (leap_build_trajectory(unit_index, &request->leap_anticipation.destination, &request->leap_anticipation.velocity,
                                0, &leap_velocity, &required_speed, 0, 0) &&
                            required_speed > 0.0001f)
                        {
                            speed = dot_product3d(&leap_velocity, &linear_velocity) / required_speed;
                        }
                        if (speed > definition->anticipation_lower)
                        {
                            speed -= definition->anticipation_lower;
                            float range = definition->anticipation_upper - definition->anticipation_lower;
                            float ratio = 1.0f;
                            if (range > 0.0001f)
                            {
                                ratio = speed / range;
                            }
                            ratio = (ratio > 0.0f ? ratio : 0.0f) < 1.0f ? (ratio > 0.0f ? ratio : 0.0f) : 1.0f;
                            if (ratio > 0.0001f)
                            {
                                channel->enable_looping(false);
                                channel->set_frame_position_from_playback_ratio(ratio);
                            }
                        }
                    }
                }
            }
        }
    }
    return success;
}

bool leap_anticipation_update(long unit_index, e_action action)
{
    c_motor_task *task = motor_task_find_by_action(unit_index, action);
    bool result = true;
    if (task)
    {
        c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
        float playback_ratio = 0.0f;
        if (channel->valid())
            playback_ratio = channel->m_playback_ratio;
        if (playback_ratio * channel->get_playback_duration() > 0.4f)
        {
            channel->jump_to_last_frame_and_stop();
            leap_anticipation_finished(unit_index, action);
            result = false;
        }
    }
    return result;
}



// @address 0x37d8b0 ?leap_anticipation_finished@@YAXJW4e_action@@@Z
void leap_anticipation_finished(long unit_index, e_action action)
{
    s_object_header *header = object_header_get(unit_index);
    if (((1 << header->object_type) & 1) == 0)
        return;
    biped_datum *datum = (biped_datum *)header->datum;
    action_request request;
    memset(&request, 0, sizeof(request));
    request.action = 33;
    request.leap_recovery.velocity = datum->biped.leap_recovery_velocity;
    request.leap_recovery.normal = datum->biped.leap_recovery_normal;
    action_submit(unit_index, &request);
}

// @address 0x37dfa0 ?action_phase_submit@@YA_NJPBUaction_request@@@Z
bool action_phase_submit(long unit_index, const action_request *request)
{
    c_motor_request motor_request;
    motor_request.setup_phase(49, &request->position, request->alignment.i);
    long submit_result = motor_system_submit(unit_index, &motor_request);
    if (submit_result != 0)
    {
        object_datum *object = (object_datum *)((mover_datum *)object_get_and_verify_type(unit_index, ~0UL));
        if (((1 << object_header_get(unit_index)->object_type) & 0x1003) != 0)
        {
            unit_definition *definition = (unit_definition *)tag_get(0, object->definition_index);
            unit_passenger_iterator iterator;
            object_datum *unit = (object_datum *)((mover_datum *)object_get_and_verify_type(unit_index, 0x1003));
            iterator.unit_index = unit_index;
            iterator.passenger_index = -1;
            iterator.seat_index = -1;
            iterator.next_object_index = unit->first_child_object_index;
            while (::unit_passenger_iterator_next(&iterator) != 0)
            {
                if (iterator.seat_index != -1 && (bool)((((unit_seat *)definition->seats.address)[iterator.seat_index].flags >> 11) & 1))
                    action_submit_event(iterator.passenger_index, 20);
            }
        }
        long child_index = object->first_child_object_index;
        while (child_index != -1)
        {
            object_datum *child = (object_datum *)((mover_datum *)object_get_and_verify_type(child_index, ~0UL));
            long next_index = child->next_object_index;
            if (((1 << object_header_get(child_index)->object_type) & 0x20) != 0)
            {
                projectile_datum *projectile = (projectile_datum *)((mover_datum *)object_get_and_verify_type(child_index, 0x20));
                bool saved = (projectile->object.flags & 0x20000) != 0;
                unsigned long *flags = &projectile->object.flags;
                unsigned long *physics_flags = &projectile->object.physics_flags;
                *flags &= ~0x20000;
                *physics_flags &= ~2;
                projectile->projectile.flags.set(_projectile_perception_attached_bit, false);
                object_detach(child_index);
                if (saved)
                    projectile->object.flags |= 0x20000;
                else
                    projectile->object.flags &= ~0x20000UL;
            }
            child_index = next_index;
        }
    }
    return submit_result != 0;
}

// @address 0x37d080 ?unit_action_morph_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_morph_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    bool success = false;

    state->morph.definition_index = request->morph.definition_index;
    state->morph.a = request->morph.a;
    state->morph.b = request->morph.b;
    state->morph.change_team = request->morph.change_team;

    c_motor_request motor_request;
    long animation = -1;
    switch (request->morph.mode)
    {
    case 0: animation = 851; break;
    case 1: animation = 853; break;
    case 2: animation = 849; break;
    default: animation = 854; break;
    }
    if (animation != -1)
    {
        action_interrupt(unit_index, _action_get_infected);
        motor_request.setup_animation(request->action, 138, animation, 0.2f, 0, false);
        success = motor_system_submit(unit_index, &motor_request) != 0;
    }
    if (success)
    {
        state->morph.definition_index = request->morph.definition_index;
        if (request->morph.widget_index != -1)
            widget_type_new_on_object(unit_index, 3, request->morph.widget_index);
        long actor_index = unit->unit.actor_index;
        if (actor_index != -1)
        {
            bool change_team = request->morph.change_team;
            long definition_index = request->morph.definition_index;
            if (change_team)
            {
                actor_datum *actor = datum_get_typed<actor_datum>(actor_data, actor_index);
                character_definition *definition = (character_definition *)tag_get(0, definition_index);
                unit_definition *character = (unit_definition *)tag_get(0, definition->unit.index);
                actor_change_squad(actor_index, -1);
                actor_set_team(actor_index, character->default_team);
                actor->character_index = definition_index;
            }
        }
    }
    return success;
}



// @address 0x37a030 ?melee_ai_event@@YAXJW4e_action@@J@Z
void melee_ai_event(long unit_index, e_action action, long event_flags)
{
    if (event_flags & 1)
        unit_cause_ai_melee_attack(unit_index, 0);
}

// @address 0x37c940 ?action_flurry_event@@YAXJW4e_action@@J@Z
void action_flurry_event(long unit_index, e_action action, long event_flags)
{
    if (event_flags & 1)
        unit_cause_ai_melee_attack(unit_index, 2);
}

// 0x37a048
void melee_ai_finished(long unit_index, e_action action)
{
    s_object_header *header = object_header_get(unit_index);
    if (((1 << header->object_type) & 0x1003) == 0)
        return;
    unit_datum *unit = (unit_datum *)header->datum;
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    if (!(bool)(_rotl(definition->flags, 24) & 1))
        return;
    ::object_destroy_internal(unit_index, -1, -1, 0, 0);
}

// @address 0x37c308 ?unit_action_berserk_finished@@YAXJW4e_action@@@Z
void unit_action_berserk_finished(long unit_index, e_action action)
{
    bool has_stowed_weapon = true;
    unit_datum *datum = (unit_datum *)object_header_get(unit_index)->datum;
    if ((unsigned char)datum->unit.current_weapon_indices[0] != 255)
        has_stowed_weapon = false;
    if ((unsigned char)datum->unit.current_weapon_indices[1] != 255)
        has_stowed_weapon = false;
    if (has_stowed_weapon == 0)
        return;
    unit_animation_set_weapon(unit_index, 135, 0, true);
}

// @rdata 0x8202251c
static const float actions_infection_zero=0.0f;
// @rdata 0x82022850
static const float actions_infection_tick_scale=10.0f;
// @rdata 0x82022bbc
static const float actions_infection_lifetime_max=15.0f;
// @rdata 0x82022dbc
static const float actions_infection_minimum_delay=0.1f;
// @rdata 0x82022dc0
static const float actions_infection_maximum_delay=0.5f;
// @rdata 0x82022c0c
static const float actions_infection_velocity_max=2.5f;
// @rdata 0x82026484
static const float actions_infection_velocity_min=1.5f;
// @rdata 0x82029240
static const float actions_infection_minimum_angle=-0.7853982f;
// @rdata 0x82029244
static const float actions_infection_maximum_angle=0.7853982f;
// @rdata 0x8203385c
static const float actions_infection_blend=0.2f;
// @rdata 0x820397e4
static const float actions_infection_lifetime_min=6.0f;

static __forceinline short actions_swarm_num_spawners_of_type(long definition_index)
{
 short count=0;
 for(short index=0;index<5;++index) {
  const s_swarm_spawner *spawner=&g_swarm_spawners[index];
  if(spawner->creature_count>0 && spawner->definition_index==definition_index) ++count;
 }
 return count;
}
static __forceinline void actions_swarm_spawner_definition_new(swarms_spawner_definition *definition)
{
 definition->matrix=0;
 definition->node_index=-1;
 definition->minimum_delay=actions_infection_minimum_delay;
 definition->maximum_delay=actions_infection_maximum_delay;
}
static __forceinline long actions_infection_spawn_ticks() { return actions_seconds_to_ticks_round(actions_infection_tick_scale); }
// @address 0x37cd60 ?unit_action_infection_spew_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_infection_spew_submit(long unit_index,const action_request *request)
{
 bool result=false;
 unit_datum *unit=((unit_datum *)object_get_and_verify_type(unit_index,0x2003));
 if(unit->unit.actor_index!=-1) {
  actor_datum *actor=datum_get_typed<actor_datum>(actor_data, unit->unit.actor_index);
  character_morph_properties *properties=character_definition_morph_properties_get(actor->character_index);
  if(properties && properties->infection_definition_index!=-1) {
   long infection_definition=properties->infection_definition_index;
   if (actions_swarm_num_spawners_of_type(infection_definition) < 2 &&
       ((character_definition *)tag_get(0, infection_definition))->creature.index != -1)
   {
    swarms_spawner_definition spawner;
    actions_swarm_spawner_definition_new(&spawner);
    spawner.reference.definition_index=infection_definition;
    spawner.reference.starting_location_index=unit_index;
    spawner.reference.marker_name=properties->infection_marker;
    spawner.count=(short)actions_infection_spawn_ticks();
    object_marker marker;
    if(spawner.reference.marker_name==0 || internal_object_get_markers_by_string_id(unit_index,spawner.reference.marker_name,&marker,1,false)==0)
     spawner.reference.marker_name=157;
    spawner.values[4]=actions_infection_velocity_min;
    spawner.values[5]=actions_infection_velocity_max;
    spawner.values[0]=actions_infection_minimum_angle;
    spawner.values[1]=actions_infection_maximum_angle;
    spawner.values[2]=actions_infection_minimum_angle;
    spawner.values[3]=actions_infection_zero;
    spawner.values[6]=actions_infection_lifetime_min;
    spawner.values[7]=actions_infection_lifetime_max;
    short spawner_index=swarm_spawner_new(&spawner);
    if(spawner_index!=-1) {
     mover_state *state=(mover_state *)((char *)unit+unit->unit.action_offset);
     state->spawner_index=spawner_index;
     c_motor_request motor_request;
     motor_request.setup_animation(52,602,602,actions_infection_blend,0,false);
     result=motor_system_submit(unit_index,&motor_request)==_motor_result_success;
     if(!result) swarm_spawner_end(spawner_index);
    }
   }
  }
 }
 return result;
}



static __forceinline mover_datum *actions_smash_object_get(long object_index)
{
    return (mover_datum *)object_header_get(object_index)->datum;
}
static __forceinline unit_definition *actions_smash_tag_get(long definition_index)
{
    return (unit_definition *)tag_get(0, definition_index);
}
static __forceinline long actions_smash_object_get_ultimate_parent(long object_index)
{
    long ultimate_parent;
    do
    {
        ultimate_parent = object_index;
        object_index = actions_smash_object_get(object_index)->object.parent_object_index;
    } while (object_index != -1);
    return ultimate_parent;
}
static __forceinline c_havok_component *actions_smash_havok_component_get(long havok_component_index)
{
    return datum_get_typed<c_havok_component>(g_havok_component_data, havok_component_index);
}
static __forceinline unsigned char actions_smash_damage_reporting_info_build(unsigned char type, unsigned char modifier)
{
    return (unsigned char)((type << 2) | modifier);
}
static __forceinline void actions_smash_object_cause_damage_simple(s_damage_data *damage_data, long object_index)
{
    object_cause_damage(damage_data, object_index, -1, -1, -1, 0);
}
// match: 0x37b868: a scalar material argument preserves the effect-buffer register and schedule.
static __forceinline void actions_smash_material_effect_new(long material_effects_index, long object_index,
    e_effect_type effect_type, e_sweetener_size sweetener_size, short material_index,
    bool underwater, const real_point3d *position, const real_vector3d *normal, const real_vector3d *direction,
    const s_location *location, float volume)
{
    c_global_material_type material_type;
    material_type.m_index = material_index;
    long effects[7];
    material_effect_find_effects(false, position, material_effects_index, k_global_material_type_invalid,
        material_type, underwater, effect_type, sweetener_size, effects);
    effect_spawn_impact_effects(effects, position, normal, direction, location, 0);
    game_sound_new_material_effect_sounds(effects, position, normal, direction, (const long *)location, object_index, volume);
}

static const float k_smash_distance = 3.0f;
static const float k_smash_forward_scale = 0.1f;
static const float k_smash_height = 1.5f;

// @address 0x37b868 ?unit_action_smash_update@@YA_NJW4e_action@@@Z
bool unit_action_smash_update(long unit_index, e_action action)
{
    mover_datum *unit = actions_smash_object_get(unit_index);
    struct unit_definition *unit_definition = actions_smash_tag_get(unit->object.definition_index);
    mover_state *state = (mover_state *)((char *)unit + unit->action.offset);
    state->smash_ticks--;
    if (state->smash_ticks != 0 || state->smash_target_index == -1)
        return true;

    mover_datum *target = (mover_datum *)object_try_and_get_and_verify_type(state->smash_target_index, ~0UL);
    if (!target)
        return true;
    struct unit_definition *target_definition = actions_smash_tag_get(target->object.definition_index);
    mover_datum *ultimate_parent = actions_smash_object_get(actions_smash_object_get_ultimate_parent(state->smash_target_index));

    real_vector3d forward;
    forward.j = 0.0f;
    forward = unit->object.forward;
    forward.k = 0.0f;
    if (normalize3d(&forward) == 0.0f)
        return true;

    real_vector3d left;
    left.i = -forward.j;
    left.j = forward.i;
    left.k = 0.0f;
    real_point3d target_origin;
    real_point3d origin;
    object_get_origin(state->smash_target_index, &target_origin);
    object_get_origin(unit_index, &origin);
    real_vector3d to_target;
    vector_from_points3d(&origin, &target_origin, &to_target);
    if (dot_product3d(&to_target, &forward) <= 0.0f)
        return true;

    real_vector3d smash_vector;
    switch (state->smash.melee_type)
    {
    case 0:
        scale_vector3d(&forward, k_smash_distance, &smash_vector);
        break;
    case 1:
        scale_vector3d(&left, k_smash_distance, &smash_vector);
        point_from_line3d(&smash_vector, &forward, k_smash_forward_scale * k_smash_distance, &smash_vector);
        break;
    case 2:
        scale_vector3d(&left, -k_smash_distance, &smash_vector);
        point_from_line3d(&smash_vector, &forward, k_smash_forward_scale * k_smash_distance, &smash_vector);
        break;
    }
    smash_vector.k = k_smash_height;
    real_vector3d direction = smash_vector;
    if (normalize3d(&direction) == 0.0f)
        return true;

    if (unit_definition->obstacle_smash_damage.index != -1)
    {
        s_damage_data damage_data;
        damage_data_new(&damage_data, unit_index);
        damage_data.reporting_info = actions_smash_damage_reporting_info_build(3, 0);
        damage_data.scale[0] = 1.0f;
        damage_compute_damage_owner_from_object_index(unit_index, &damage_data.owner);
        damage_data.damage_owner_object_index = unit_index;
        damage_data.effect = unit_definition->obstacle_smash_damage.index;
        damage_data.origin = origin;
        damage_data.position = damage_data.origin;
        damage_data.direction = smash_vector;
        normalize3d(&damage_data.direction);
        damage_data.initial_direction = damage_data.direction;
        actions_smash_object_cause_damage_simple(&damage_data, state->smash_target_index);
    }

    if (TEST_BIT(_object_mask_biped, unit_definition->object_type) && ultimate_parent->object.havok_component_index != -1)
    {
        struct unit_definition *biped_definition = actions_smash_tag_get(unit->object.definition_index);
        if (target_definition->model.index != -1)
        {
            c_havok_component *havok_component = actions_smash_havok_component_get(ultimate_parent->object.havok_component_index);
            s_model_definition *model = (s_model_definition *)actions_smash_tag_get(target_definition->model.index);
            if (model->physics_model_index != -1)
            {
                c_global_material_type material_type = k_global_material_type_default;
                long rigid_body_index = havok_component->get_object_postion_controlling_rigid_body_index();
                if (rigid_body_index != -1 && havok_component->get_physics_model_rigid_body_index_count(rigid_body_index) > 0)
                {
                    long physics_model_rigid_body_index = havok_component->get_physics_model_rigid_body_index(rigid_body_index, 0);
                    if (physics_model_rigid_body_index != -1)
                    {
                        real_vector3d effect_direction;
                        scale_vector3d(&direction, -1.0f, &effect_direction);
                        physics_model_compute_best_material_rigid_body(model->physics_model_index, physics_model_rigid_body_index, &material_type);
                        e_sweetener_size sweetener_size =
                            (e_sweetener_size)((unsigned char)target_definition->sweetener_size <= (unsigned char)biped_definition->sweetener_size
                                    ? (unsigned char)target_definition->sweetener_size
                                    : (unsigned char)biped_definition->sweetener_size);
                        actions_smash_material_effect_new(biped_definition->material_effects.index, unit_index, _effect_type_obstacle_smash,
                            sweetener_size, material_type.m_index, true, &origin, global_down3d, &effect_direction, &unit->object.location, 1.0f);
                    }
                }
            }
        }
    }
    return true;
}

// @address 0x37b670 ?unit_action_smash_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_smash_submit(long unit_index, const action_request *request)
{
    long goal = -1;
    bool valid = true;
    short melee_type = request->melee_ai.melee_type;
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    switch (melee_type)
    {
    case 0:
        if (((c_animation_manager *)((char *)unit + unit->object.animation.offset))->find_goal(226, 226, 226, 0x4a8, 0))
        {
            goal = 0x4a8;
            break;
        }
    case 1:
        goal = 0x113;
        break;
    case 2:
        goal = 0x114;
        break;
    default:
        valid = false;
        break;
    }
    if (valid)
    {
        c_motor_request motor_request;
        motor_request.m_force = false;
        motor_request.flurry.effect = goal;
        motor_request.flurry.b = 0;
        motor_request.flurry.a = 0;
        motor_request.flurry.interpolation = 0.26666668f;
        motor_request.action = _action_smash;
        motor_request.m_task = _motor_task_animation;
        motor_request.flurry.animation = 226;
        motor_request.m_valid = true;
        state->smash.melee_type = (unsigned char)request->melee_ai.melee_type;
        state->smash_target_index = request->play.animation;
        state->smash_ticks = 0;
        state->smash.weapon_slot = (unsigned char)request->melee_player.weapon_slot;
        if (motor_system_submit(unit_index, &motor_request) != _motor_result_failed)
        {
            c_motor_task *task = motor_task_find_by_action(unit_index, _action_smash);
            state->smash_ticks = (short)(long)((float)game_time_globals->ticks_per_second * 0.8f + 0.5f);
            if (task)
            {
                c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
                if (channel)
                {
                    float key_time = channel->get_authored_primary_key_time();
                    if (key_time >= 0.0f)
                    {
                        state->smash_ticks = (short)(long)((float)game_time_globals->ticks_per_second * key_time + 0.5f);
                    }
                }
            }
        }
        else
        {
            state->smash_ticks = 1;
            unit_action_smash_update(unit_index, _action_smash);
        }
    }
    return valid;
}

// @address 0x37c0e8 ?unit_action_berserk_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_berserk_submit(long unit_index, const action_request *request)
{
    bool stow_weapon = false;
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    state->berserk.stow_weapon = request->bytes.a;
    action_interrupt(unit_index, _action_hard_ping);

    c_motor_request motor_request;
    motor_request.setup_animation(36, 226, 52, 0.2f, 0, false);
    if (motor_system_submit(unit_index, &motor_request) == _motor_result_success)
    {
        c_motor_task *task = motor_task_find_by_action(unit_index, _action_berserk);
        if (task && !(motor_animation_task_get_channel(unit_index, task)->m_future_event_flags.get_unsafe() & 1))
            stow_weapon = true;
    }
    else
    {
        stow_weapon = true;
    }
    if (stow_weapon && state->berserk.stow_weapon)
        unit_action_weapon_stow_immediate(unit_index, true);
    return true;
}

// @address 0x37c208 ?unit_action_berserk_event@@YAXJW4e_action@@J@Z
void unit_action_berserk_event(long unit_index, e_action action, long event_flags)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
    if (state->berserk.stow_weapon && (event_flags & 1))
        unit_action_weapon_stow_immediate(unit_index, false);
}

// @address 0x37c258 ?unit_action_berserk_interrupt@@YAXJW4e_action@@@Z
void unit_action_berserk_interrupt(long unit_index, e_action action)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    c_motor_task *task = motor_task_find_by_action(unit_index, _action_berserk);
    if (task)
    {
        mover_state *state = (mover_state *)((char *)unit + unit->unit.action_offset);
        if (state->berserk.stow_weapon)
        {
            c_animation_channel *channel = motor_animation_task_get_channel(unit_index, task);
            if (channel->m_future_event_flags.get_unsafe() & 1)
                unit_action_weapon_stow_immediate(unit_index, false);
        }
    }
    unit_action_berserk_finished(unit_index, action);
}

static __forceinline biped_datum *actions_ping_unit_get(long index)
{
    return (biped_datum *)object_header_get(index)->datum;
}
static __forceinline c_motor_task *actions_ping_motor_task_get(long index, e_motor_resource resource)
{
    biped_datum *unit = actions_ping_unit_get(index);
    return (c_motor_task *)((char *)unit + unit->unit.motor_tasks.offset) + resource;
}
static __forceinline bool actions_ping_setup_damage_channel(long unit_index, const biped_datum *unit,
    const c_animation_manager *animation_manager, long damage, short direction, short region)
{
    c_animation_channel channel;
    long mode = 226;
    biped_datum *program_unit = actions_ping_unit_get(unit_index);
    long program = *(long *)((char *)program_unit + program_unit->unit.motor_program.offset);
    if (program == 9 || program == 6)
    {
        if (motor_animation_test_mode(unit_index, 139) && unit->object.shield_vitality > 0.0f)
            mode = 139;
        else
            mode = 138;
    }
    bool result = animation_manager->setup_animation_channel_by_death_or_damage(&channel, mode, damage, direction, region, NULL, NULL);
    animation_channel_initialize_and_disable(&channel);
    return result;
}
// @address 0x37af48 ?unit_action_ping_submit@@YA_NJPBUaction_request@@@Z
bool unit_action_ping_submit(long unit_index, const action_request *request)
{
    signed char type = request->ping.type;
    signed char region = request->ping.region;
    bool has_direction = false;
    float angle = 0.0f;
    bool result = true;
    biped_datum *unit = actions_ping_unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    real_vector3d direction;
    real_vector3d ping_vector;
    if (request->ping.has_direction)
    {
        direction = request->ping.direction;
        if (normalize3d(&direction) > 0.0f)
        {
            angle = signed_angle_between_normals3d(&direction, &unit->object.forward, &unit->object.up);
            bool no_direction = (definition->flags >> 9) & 1;
            if (!no_direction && unit->object.object_type == 0 && unit->object.parent_object_index == -1 &&
                type >= 2 && unit->biped.physics.m_mode != 4)
            {
                has_direction = true;
                ping_vector = direction;
            }
        }
    }
    short ping_direction;
    if (fabsf(angle) < 0.7853982f)
    {
        ping_direction = 3;
    }
    else if (fabsf(angle) > 2.159845f)
    {
        ping_direction = 0;
    }
    else
    {
        if (angle > 0.0f)
            ping_direction = 1;
        else
            ping_direction = 2;
    }
    if (tls_game_globals()->options.game_mode == 2 && region == 2 && type == 7)
        ping_direction = 1;
    if (region == -1)
        region = 0;
    if (type == 2)
    {
        c_animation_manager *animation_manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        c_motor_task *task = actions_ping_motor_task_get(unit_index, _motor_resource_base);
        if (task && task->valid() && task->task == 33)
        {
            float hard_ping_time = definition->hard_ping_time;
            if (unit->unit.actor_index != -1)
            {
                actor_datum *actor = datum_get_typed<actor_datum>(actor_data, unit->unit.actor_index);
                character_vitality_properties *vitality = character_definition_vitality_properties_get(actor->character_index);
                if (vitality->ping_time > 0.0f)
                    hard_ping_time = vitality->ping_time;
            }
            result = animation_manager->get_authored_state_time() >= hard_ping_time;
        }
        if (result)
        {
            if (!motor_task_allow(unit_index, _action_hard_ping, 15) || unit_get_current_seat(unit_index) || motor_system_in_airborne_state(unit_index))
            {
                result = false;
            }
            else
            {
                long damage = damage_type_string_id_list[1];
                if (damage != -1)
                {
                    result = actions_ping_setup_damage_channel(unit_index, unit, animation_manager, damage, ping_direction, region);
                }
            }
            if (!result)
            {
                type = 1;
                result = true;
            }
        }
    }
    if (result && type == 1)
    {
        result = motor_task_allow(unit_index, _action_soft_ping, 2);
        if (result)
        {
            c_motor_task *task = actions_ping_motor_task_get(unit_index, _motor_resource_overlay);
            if (task && task->valid() && task->task == 33)
            {
            c_animation_channel *channel = motor_animation_resource_get_channel(unit_index, _motor_resource_overlay);
                result = channel->valid() && channel->m_frame_position * 0.033333335f >= definition->soft_ping_time;
            }
        }
    }
    if (result && type > 0)
    {
        long action;
        switch (type)
        {
        case 1: action = 65; break;
        case 2: action = 6; break;
        case 3: case 4: case 5: case 6: case 7: action = 0; break;
        default: action = -1; break;
        }
        if (action != -1)
        {
            c_motor_request motor_request;
            motor_request.setup_ping(action, type, region, ping_direction, has_direction ? &ping_vector : NULL);
            result = motor_system_submit(unit_index, &motor_request) != _motor_result_failed;
        }
    }
    if (result && type >= 3)
    {
        biped_datum *weapon_unit = actions_ping_unit_get(unit_index);
        long current_weapon = -1;
        if (weapon_unit->unit.current_weapon_index != -1)
            current_weapon = weapon_unit->unit.weapon_object_indices[weapon_unit->unit.current_weapon_index];
        long desired_weapon = -1;
        if (weapon_unit->unit.desired_weapon_index != -1)
            desired_weapon = weapon_unit->unit.weapon_object_indices[weapon_unit->unit.desired_weapon_index];
        if (type != 6 &&
            (current_weapon == -1 || !weapon_is_support_weapon_or_must_be_readied(current_weapon)) &&
            (desired_weapon == -1 || !weapon_is_support_weapon_or_must_be_readied(desired_weapon)))
        {
            c_animation_manager *animation_manager = (c_animation_manager *)((char *)weapon_unit + weapon_unit->object.animation.offset);
            float duration;
            float authored_time = animation_manager->m_state_channel.get_authored_primary_key_time();
            if (authored_time >= 0.0f)
                duration = authored_time;
            else
            {
                float authored_duration;
                motor_animation_base_channel_time(unit_index, &authored_duration);
                duration = _real_random_range(tls_random_seed(), authored_duration * 0.25f, authored_duration * 0.75f);
            }
            unit->unit.weapon_change_ticks = (signed char)actions_seconds_to_ticks_round(duration);
            unit->unit.weapon_change_ticks = unit->unit.weapon_change_ticks > 1 ? unit->unit.weapon_change_ticks : 1;
        }
        else
        {
            unit->unit.weapon_change_ticks = 0;
        }
    }
    return result;
}

static __forceinline void *actions_exit_unit_get(long unit_index)
{
    return object_header_get(unit_index)->datum;
}

// match: 0x378d10: pointer __restrict qualifier prevents alias reload
static __forceinline void actions_exit_passenger_iterator_new(long unit_index, unit_passenger_iterator *__restrict iterator)
{
    unit_datum *unit = (unit_datum *)actions_exit_unit_get(unit_index);
    iterator->unit_index = unit_index;
    iterator->passenger_index = -1;
    iterator->seat_index = -1;
    iterator->next_object_index = unit->object.first_child_object_index;
}

// @address 0x378d10
bool unit_action_vehicle_exit_submit(long unit_index, const action_request *request)
{
    unit_datum *unit = (unit_datum *)actions_exit_unit_get(unit_index);
    bool immediate = request->action == 20;
    bool exit_allowed = true;
    bool result = false;

    if (request->bytes.b != 0)
    {
        if (unit->unit.actor_index != -1)
            actor_attach_to_vehicle(unit->unit.actor_index, -1, false);
    }

    if (!immediate && unit->object.parent_object_index != -1)
    {
        const long *parent_index = &unit->object.parent_object_index;
        long seat_index = unit->unit.parent_seat_index;
        if (seat_index != -1)
        {
            unit_datum *parent = (unit_datum *)actions_exit_unit_get(*parent_index);
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_passenger_iterator iterator;
            actions_exit_passenger_iterator_new(*parent_index, &iterator);
            unit_datum *passenger;
            while ((passenger = unit_passenger_iterator_next(&iterator)) != 0)
            {
                if (iterator.seat_index != -1)
                {
                    unit_seat *seat = &((unit_seat *)definition->seats.address)[iterator.seat_index];
                    if ((bool)((seat->flags >> 11) & 1) && seat->index == seat_index)
                        exit_allowed = false;
                }
            }
        }
    }

    if (exit_allowed)
    {
        bool seat_exit = false;
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_datum *parent = (unit_datum *)actions_exit_unit_get(unit->object.parent_object_index);
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            seat_exit = ((((unit_seat *)definition->seats.address)[unit->unit.parent_seat_index].flags >> 24) & 1) != 0;
        }
        c_motor_request motor_request;
        motor_request.m_force = false;
        motor_request.action = _action_vehicle_exit;
        motor_request.m_task = _motor_task_exit;
        motor_request.bytes.a = request->bytes.a | seat_exit;
        motor_request.bytes.b = immediate;
        motor_request.bytes.c = false;
        motor_request.bytes.d = false;
        motor_request.m_valid = true;
        result = motor_system_submit(unit_index, &motor_request) != 0;
    }
    return result;
}

extern "C" {
}

// @address 0x37c368
void deploy_turret(long unit_index)
{
    biped_datum *unit = (biped_datum *)object_header_get(unit_index)->datum;
    unit_datum *datum = (unit_datum *)unit;
    mover_state *state = (mover_state *)((char *)datum + datum->unit.action_offset);
    long weapon_index = NONE;
    if (unit->unit.current_weapon_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
    if (weapon_index != NONE)
    {
        mover_datum *weapon = ((mover_datum *)object_get_and_verify_type(weapon_index, _object_mask_weapon));
        weapon_definition *definition = (weapon_definition *)tag_get(0, weapon->object.definition_index);
        if (definition->deployed_vehicle != NONE)
        {
            object_marker marker;
            real_point3d position;
            real_vector3d up;
            real_vector3d forward;
            action_request request;
            memset(&request, 0, sizeof(request));
            bool has_position = false;
            if (internal_object_get_markers_by_string_id(weapon_index, 711, &marker, 1, false) > 0)
            {
                collision_result collision;
                position = marker.matrix.position;
                forward = marker.matrix.forward;
                up = marker.matrix.up;
                real_point3d test_point;
                real_vector3d test_vector;
                point_from_line3d(&position, global_up3d, 0.2f, &test_point);
                scale_vector3d(global_up3d, -1.0f, &test_vector);
                if (collision_test_vector(k_biped_ground_collision_flags, false, &test_point, &test_vector, NONE, NONE, NONE, &collision))
                {
                    position = collision.point;
                    up = *global_up3d;
                    forward.k = 0.0f;
                    if (normalize3d(&forward) == 0.0f)
                        forward = *global_forward3d;
                    has_position = true;
                }
                else
                {
                    real_point3d origin;
                    object_get_origin(unit_index, &origin);
                    object_get_orientation(unit_index, &forward, &up);
                    position = marker.matrix.position;
                    position.z = origin.z;
                    has_position = true;
                }
            }
            if (!has_position)
            {
                object_get_origin(weapon_index, &position);
                real_point3d origin;
                object_get_origin(unit_index, &origin);
                object_get_orientation(unit_index, &forward, &up);
                position.z = origin.z;
            }
            request.action = 46;
            request.bytes.a = true;
            if (action_submit(unit_index, &request))
            {
                object_placement_data placement;
                object_placement_data_new(&placement, definition->deployed_vehicle, NONE, NULL);
                placement.position = position;
                placement.forward = forward;
                placement.up = up;
                long turret_index = object_new(&placement);
                if (turret_index != NONE && unit->unit.actor_index != NONE)
                {
                    unit_datum *turret = (unit_datum *)object_header_get(turret_index)->datum;
                    if (TEST_BIT(1 << turret->object.object_type, _object_type_vehicle))
                    {
                        action_request open_request;
                        memset(&open_request, 0, sizeof(open_request));
                        open_request.action = _action_open;
                        open_request.bytes.a = false;
                        unit_action_open_submit(turret_index, &open_request);
                    }
                    ai_handle_deployed_turret(unit->unit.actor_index, turret_index);
                    object_delete(weapon_index);
                }
            }
        }
    }
    state->turret.deployed = true;
}
// @address 0x22db8 k_real_two_fifths_storage
const float k_real_two_fifths_storage[] = { 0.4f };
