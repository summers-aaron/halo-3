// objectslibxbox/biped_flying_motor_program.obj: from the records 0x38a580 0x38a6a0 0x38a708 0x38a788 0x38a860 0x38a968 0x38aa60 0x38ac08
// @flags /TP /O2 /Oc
#include "physics/character_physics_input.h"
#include "math/real_math.h"
#include "units/motor_enums.h"
#include "objects/object_header.h"
#include "units/motor_program_state.h"
#include "physics/character_physics_output.h"
#include "game/game_time_types.h"
#include "motor/motor_animation.h"
#include "motor/motor_system_biped.h"
#include "physics/character_physics_utilities.h"
#include "physics/character_physics_component.h"
#include "motor/biped_flying_motor_program.h"
#include "motor/biped_ground_motor_program.h"
#include "physics/character_physics_mode_flying.h"
#include "animations/animation_manager.h"
#include "units/biped_types.h"
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_control_data.h"
#include "tag_files/tag_types.h"
#include "units/biped_definitions.h"
// @address 0x38a580 ?biped_flying_land_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38a860 ?biped_update_choose_seat_and_state_from_physics_animation_state@@YAXJW4e_animation_state@s_character_physics_update_output_datum@@_NPAJ2PA_N@Z
// @address 0x38a968 ?biped_flying_update_moving@@YAXJPAUs_animation_control_data@@PAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@@Z
// @address 0x3846f8 ?get_biped_state_index_from_string@@YADPAVc_animation_manager@@J@Z
// @address 0xe8bf0 ?motor_animation_test_mode@@YA_NJJ@Z
// @address 0x355e10 ?motor_animation_set_mode_weapons_and_state@@YA_NJJJJJMJJ_N@Z
// @address 0x356738 ?motor_task_set_posture@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJ_NPBTreal_point3d@@PBTreal_vector3d@@F2PA_N@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z


#include "units/motor_request.h"
#include "physics/character_physics_definition_types.h"
#include "units/motor_task.h"
#include "physics/character_physics_move_output.h"
#include "animations/animation_state_change_settings.h"



// The flying program's state in the biped's motor program block (unit.motor_program). biped_flying_activate
// clears activation_flags the way biped_ground_activate and biped_vehicle_activate clear theirs;
// angular_velocity is the turning rate character_physics_flying_update_turning keeps between ticks.
struct biped_flying_motor_state : c_motor_program_state
{
    unsigned short activation_flags;
    unsigned char flying;
    unsigned char state_changed;
    unsigned char ticks;
    float angular_velocity;
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char biped_flying_motor_state_flying_offset[((unsigned long)&((biped_flying_motor_state *)0)->flying == 0xa) ? 1 : -1];
typedef char biped_flying_motor_state_angular_velocity_offset[((unsigned long)&((biped_flying_motor_state *)0)->angular_velocity == 0x10) ? 1 : -1];
#endif






static __forceinline biped_datum *biped_flying_unit_get(long object_index)
{
    return (biped_datum *)object_header_get(object_index)->datum;
}

e_motor_result biped_flying_land_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    if (motor_request->m_land.has_point) {
        long mode;
        long state;
        e_motor_result submit = _motor_result_failure;
        c_motor_task *task = motor_task_set_posture(object_index, _motor_task_land, (e_action)motor_request->action, 687, 476, true,
            &motor_request->m_land.point, &motor_request->m_land.vector, 1, motor_request->force(), 0);
        ::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state));
        if (task != 0 && state == 687) {
            submit = _motor_result_success;
        } else if (motor_animation_set_mode_weapons_and_state(object_index, 138, 226, 226, 12, 8.0f, 0, 31, submit != _motor_result_failure)) {
            submit = _motor_result_pending;
        }
        if (submit != _motor_result_failure) {
            biped_datum *unit = biped_flying_unit_get(object_index);
            c_motor_program_state *state_block = (c_motor_program_state *)((char *)unit + unit->unit.motor_offset);
            state_block->set_program(object_index, _motor_program_ground);
        }
        result = submit;
    }
    return result;
}

// @address 0x38a708 ?biped_flying_activate@@YA_NJ@Z
bool biped_flying_activate(long object_index)
{
    biped_datum *biped = biped_flying_unit_get(object_index);
    biped_flying_motor_state *motor = (biped_flying_motor_state *)((char *)biped + biped->unit.motor_offset);
    motor->angular_velocity = 0.0f;
    motor->activation_flags = 0;
    motor->state = 0;
    motor->flying = 0;
    biped_flying_unit_get(object_index)->biped.physics.set_mode(c_character_physics_component::_mode_flying);
    return true;
}


// @rdata 0x82028bec
static const float k_biped_flying_turning_velocity_override = 0.99f;


// @address 0x38a788 ?biped_update_turning@@YA_NJPAUs_animation_control_data@@@Z
bool biped_update_turning(long object_index, s_animation_control_data *animation_control)
{
    biped_datum *biped = biped_flying_unit_get(object_index);
    biped_flying_motor_state *motor = (biped_flying_motor_state *)((char *)biped + biped->unit.motor_offset);
    biped_definition *definition = TAG_GET('bipd', biped_definition, biped->object.definition_index);
    if (!biped->biped.physics.get_mode_flying()->is_turning_disabled()) {
        float turning_velocity;
        if (biped->unit.control_flags & 8)
            turning_velocity = k_biped_flying_turning_velocity_override;
        else
            turning_velocity = definition->turning_velocity_maximum;
        character_physics_flying_update_turning(object_index, &definition->physics.flying,
            &biped->unit.facing_vector, &biped->unit.throttle, turning_velocity, &motor->angular_velocity);
        return false;
    }
    motor->angular_velocity = 0.0f;
    return false;
}

static void biped_update_choose_seat_and_state_from_physics_animation_state(long object_index,
    s_character_physics_update_output_datum::e_animation_state animation_state, bool moving, long *mode, long *state, bool *moved)
{
    if (motor_animation_test_mode(object_index, 0x1dc))
        *mode = 0x1dc;
    if (*state == 12) {
        switch (animation_state) {
        case 1: *state = 12; break;
        case 2: *state = 24; break;
        case 3: *state = 23; break;
        case 4: *state = 21; break;
        case 5: *state = 22; break;
        case 6: *state = 66; break;
        case 7: *state = 65; break;
        case 8: *state = 63; break;
        case 9: *state = 64; break;
        case s_character_physics_update_output_datum::_animation_state_climb_up: *state = 29; break;
        case s_character_physics_update_output_datum::_animation_state_climb_down: *state = 30; break;
        }
        if (animation_state != 0)
            *moved = moving;
    }
}

static __forceinline void *biped_flying_block_get(long object_index, const short *offset)
{
    return (char *)biped_flying_unit_get(object_index) + *offset;
}

static __forceinline float biped_flying_ticks_to_seconds(long ticks)
{
    return tls_game_time_globals()->tick_length * ticks;
}

void biped_flying_update_moving(long object_index, s_animation_control_data *animation_control,
    s_character_physics_update_input_datum *input, s_character_physics_update_output_datum *output)
{
    biped_datum *biped = biped_flying_unit_get(object_index);
    biped_flying_motor_state *motor = (biped_flying_motor_state *)biped_flying_block_get(object_index, &biped->unit.motor_offset);
    c_animation_manager *animation = (c_animation_manager *)biped_flying_block_get(object_index, &biped->object.animation.offset);
    biped_update_choose_seat_and_state_from_physics_animation_state(object_index, output->animation_state, output->moving,
        &animation_control->mode, &animation_control->state, &animation_control->moved);
    if (biped->unit.player_index != -1) {
        long state = animation_control->state;
        if (state == 12 && motor->flying == 1) {
            if (motor->state_changed) motor->ticks = 0;
            else ++motor->ticks;
            if (biped_flying_ticks_to_seconds(motor->ticks) < 0.1f) animation_control->state = animation->m_current_goal.state;
        }
        motor->state_changed = get_biped_state_index_from_string(animation, state);
    }
}


// @address 0x38ac08 ?biped_flying_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_flying_move(long object_index, c_motor_request *motor_request)
{
    s_character_physics_move_output_datum output;
    motor_system_biped_move(object_index, true, false, false, false, &output);
    return _motor_program_none;
}

// @address 0x38a6a0 ?biped_flying_land_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_flying_land_finished(long object_index, c_motor_task *motor_task)
{
    ::motor_animation_posture_exit_end(object_index, 138, 226, 226, 12);
    biped_datum *unit = (biped_datum *)object_header_get(object_index)->datum;
    unit->biped.physics.set_mode(c_character_physics_component::_mode_ground);
    return true;
}

static __forceinline long biped_flying_object_get_root_object(long object_index)
{
    while (object_index != -1)
    {
        biped_datum *object = (biped_datum *)biped_flying_unit_get(object_index);
        if (!(bool)((object->object.flags >> 26) & 1))
            break;
        object_index = object->object.parent_object_index;
    }
    return object_index;
}
// @address 0x38aa60 ?biped_flying_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_flying_update(long object_index, s_animation_control_data *animation_control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    biped_datum *datum = (biped_datum *)biped_flying_unit_get(object_index);
    c_animation_manager *animation = (c_animation_manager *)((char *)datum + datum->object.animation.offset);
    biped_flying_motor_state *motor = (biped_flying_motor_state *)((char *)datum + datum->unit.motor_offset);
    bool result = false;
    if (datum->object.havok_component_index != -1 && biped_flying_object_get_root_object(datum->object.parent_object_index) == -1)
    {
        long state = animation->m_current_goal.state;
        s_character_physics_update_input_datum input;
        s_character_physics_update_output_datum output;
        motor->flying = get_biped_state_index_from_string(animation, state);
        result = biped_update_turning(object_index, &control) | true;
        if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, false, true, motor->angular_velocity, false, false))
        {
            biped_flying_update_moving(object_index, &control, &input, &output);
            motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouch);
        }
    }
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        settings.flags = 0;
        requested.goals = 0;
        requested.transition = 0;
        requested.primary = 0;
        motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
        result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    motor_animation_update_analog_blending(object_index);
    return result;
}
