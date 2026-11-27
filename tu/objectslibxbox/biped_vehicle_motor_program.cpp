// objectslibxbox/biped_vehicle_motor_program.obj: from the records 0xee7e0 0x38a028 0x38a090 0x38a1c8 0x38a1f0 0x38a2c8 0x38a308 0x38a458 0x38a4d0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "objects/object_header.h"
#include "memory/datum_index.h"
#include "units/motor_program_state.h"
#include "units/unit_seat.h"
#include "units/unit_seat_flags.h"
#include "units/unit_weapon_config_types.h"
#include "game/game_simulation_predicates.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "motor/motor_system_biped.h"
#include "motor/biped_vehicle_motor_program.h"
#include "animations/animation_manager.h"
#include "units/biped_types.h"
#include "cache/cache_files.h"
#include "units/motor_request.h"
#include "units/unit_definitions.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_control_data.h"
#include "animations/animation_state_change_settings.h"
#include "physics/character_physics_move_output.h"
// @address 0x38a2c8 ?biped_vehicle_activate@@YA_NJ@Z
// @address 0x38a028 ?biped_vehicle_board_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38a1c8 ?biped_vehicle_board_interrupted@@YA_NJPAVc_motor_task@@@Z
// @address 0x38a1f0 ?biped_vehicle_ejection_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x357718 ?motor_task_set_mode_and_state@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJMFJJ_N@Z
// @address 0x3577d8 ?motor_task_start_animation@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJW4e_unit_weapon_config@@FM_N@Z

// The biped_vehicle program block (unit.motor_program): the common program state, then one word that
// biped_vehicle_activate clears (biped_ground_activate clears its own activation_flags the same way).
struct biped_vehicle_motor_state {
    c_motor_program_state program;
    unsigned short activation_flags;
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char biped_vehicle_motor_state_activation_flags_offset[(offsetof(biped_vehicle_motor_state, activation_flags) == 8) ? 1 : -1];
#endif


// @address 0xee7e0 biped_vehicle_ejection_end
extern "C" bool biped_vehicle_ejection_end(long object_index, c_motor_task *task)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    unsigned long *vehicle_flags = (unsigned long *)object_header_block_get(object, &object->unit.action);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->unit.motor_program);
    bool ejected = (*vehicle_flags >> 2) & 1;
    state->set_program(object_index, _motor_program_airborne);
    motor_animation_exit_seat_reset_velocity(object_index, 0, 1, true, 0, 0);
    if (ejected && object_index != -1) {
        OBJECT_GET(object_index, unit_datum, _object_mask_unit)->object.damage_flags |= 0x40;
        object_wake(object_index);
    }
    s_object_header *header = object_header_get(object_index);
    if ((1 << header->object_type) & 1)
        ((biped_datum *)header->datum)->biped.flags |= 2;
    return true;
}

// @address 0x38a4d0 ?biped_vehicle_ejection_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_vehicle_ejection_finished(long object_index, c_motor_task *task)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    if (object->object.parent_object_index != -1 && object->unit.parent_seat_index != -1 &&
        unit_seat_test_flag(object->object.parent_object_index, object->unit.parent_seat_index, _unit_seat_boarding_seat_bit)) {
        motor_animation_exit_seat_reset_velocity(object_index, 1, 0, false, 0, 0);
        c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->unit.motor_program);
        state->set_program(object_index, _motor_program_airborne);
    }
    return true;
}

bool biped_vehicle_activate(long object_index)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    biped_vehicle_motor_state *motor = (biped_vehicle_motor_state *)object_header_block_get(object, &object->unit.motor_program);
    motor->activation_flags = 0;
    motor->program.state = 0;
    return true;
}

e_motor_result biped_vehicle_board_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_none;
    if (motor_task_start_animation(object_index, _motor_task_board, (e_action)motor_request->action, 184, 226, _unit_weapon_config_none, 0,
            0.0f, motor_request->force()) != 0)
    {
        result = _motor_result_success;
    }
    return result;
}

bool biped_vehicle_board_interrupted(long object_index, c_motor_task *task)
{
    biped_vehicle_board_end(object_index);
    return true;
}

e_motor_result biped_vehicle_ejection_submit(long object_index, const c_motor_request *request)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    unsigned long *flags = (unsigned long *)object_header_block_get(object, &object->unit.action);
    e_motor_result result = _motor_result_none;
    if (request->m_ejection.finished) {
        biped_vehicle_ejection_end(object_index, 0);
        result = _motor_result_pending;
    } else if (motor_task_set_mode_and_state(object_index, _motor_task_ejection, (e_action)request->action, 226, 185, 0.0f, 15, 2, 29, request->force())) {
        if (request->m_ejection.eject) {
            *flags |= 4;
        } else {
            *flags &= ~4UL;
        }
        result = _motor_result_success;
    }
    return result;
}

// @address 0x38a090 ?biped_vehicle_board_end@@YAXJ@Z
void biped_vehicle_board_end(long object_index)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    long parent_object_index = object->object.parent_object_index;
    if (parent_object_index != -1 && object->unit.parent_seat_index != -1)
    {
        unit_datum *parent = OBJECT_GET(parent_object_index, unit_datum, _object_mask_unit);
        unit_definition *definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
        unit_seat *seat = &definition->seats[object->unit.parent_seat_index];
        if (seat->linked_seat != 0xffff)
        {
            bool exit_to_linked_seat = ((seat->flags >> 13) & 1) != 0;
            if (exit_to_linked_seat)
            {
                unit_evict_parent_seat_occupant(object_index, false, true);
                motor_animation_exit_seat_reset_velocity(object_index, 1, 0, false, 0, 0);
                if (motor_task_enter_seat_internal(object_index, parent_object_index, seat->linked_seat, true, 16, 0, false))
                {
                    motor_animation_unit_entry_finished(object_index, 16);
                    return;
                }
            }
            else
            {
                motor_animation_exit_seat_reset_velocity(object_index, 1, 0, false, 0, 0);
            }
            c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->unit.motor_program);
            state->set_program(object_index, _motor_program_airborne);
        }
    }
}

// @address 0x38a308 ?biped_vehicle_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_vehicle_update(long object_index, s_animation_control_data *control_data)
{
    s_state_changes_requested requested;
    s_animation_control_data control;
    control.set_defaults();
    s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
    unit_datum *object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    long parent_object_index = object->object.parent_object_index;
    if (object->object.havok_component_index != -1)
    {
        s_object_header *parent_header = &headers[datum_index_to_absolute_index(parent_object_index)];
        control.mode = 226;
        control.state = 12;
        if (((1 << parent_header->object_type) & 0x1003) != 0)
        {
            unit_datum *parent = (unit_datum *)headers[datum_index_to_absolute_index(parent_object_index)].datum;
            unit_definition *definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
            unit_seat *seat = &definition->seats[object->unit.parent_seat_index];
            bool linked = ((seat->flags >> 11) & 1) != 0;
            if (linked && seat->linked_seat != 0xffff)
                control.flags |= 4;
        }
    }
    s_animation_state_change_settings settings;
    settings.flags = 0;
    requested.goals = 0;
    requested.transition = 0;
    requested.primary = 0;
    motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
    return 0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved);
}

// @address 0x38a458 ?biped_vehicle_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
enum e_motor_program biped_vehicle_move(long object_index, class c_motor_request *request)
{
    e_motor_program result = _motor_program_none;
    s_character_physics_move_output_datum move_output;
    motor_system_biped_move(object_index, false, false, false, false, &move_output);
    if (!game_simulation::game_is_predicted())
        biped_falling_damage(object_index);
    return result;
}
