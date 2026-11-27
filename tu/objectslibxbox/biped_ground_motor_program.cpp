// objectslibxbox/biped_ground_motor_program.obj: from the records 0xe63c0 0x382128 0x382198 0x382248 0x382348 0x382378 0x3823a0 0x382420 0x382530 0x382578 0x3825a0 0x3826f8 0x382850 0x382968 0x382a00 0x382aa0 0x382ae8 0x382c20 0x382d38 0x382e48 0x382f48 0x382fb0 0x383248 0x383b50 0x383eb8 0x384050 0x3842b8 0x384620 0x3846f8 0x384780 0x384920 0x384e48 0x385110 0x385230 0x385420 0x385508 0x385610
// @flags /TP /O2 /Oc
#include "game/game_time_types.h"
#include "physics/character_physics_component.h"
#include "physics/character_physics_globals.h"
#include "memory/tls_globals.h"
#include "units/unit_dialogue.h"
#include "units/motor_enums.h"
#include "animations/animation_state_change_settings.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "units/unit_weapon_config_types.h"
#include "units/motor_program_state.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "objects/object_types.h"
#include "game/game_globals.h"
#include "game/falling_damage_types.h"
#include "math/real_math_constants.h"
// @address 0x2251c k_real_zero_storage
#include "animations/animation_id.h"
#include "units/motor_task.h"
// @address 0xd76b8 ?motor_animation_resource_get_channel@@YAPAVc_animation_channel@@JW4e_motor_resource@@@Z
// @address 0x102240 ?motor_animation_task_get_channel@@YAPAVc_animation_channel@@JPAVc_motor_task@@@Z
// @address 0xe8bf0 ?motor_animation_test_mode@@YA_NJJ@Z
// @address 0xe8418 ?motor_animation_test_mode@@YA_NJJJJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z
// @address 0x356738 ?motor_task_set_posture@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJ_NPBTreal_point3d@@PBTreal_vector3d@@F2PA_N@Z
// @address 0x354b38 ?unit_align_facing2d@@YAXJPBTreal_vector2d@@@Z
// @address 0x355538 ?motor_animation_validate_animation@@YA_NJJJPA_N@Z
// @address 0x3577d8 ?motor_task_start_animation@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJW4e_unit_weapon_config@@FM_N@Z
// @address 0xe6660 ?biped_set_forward_vector@@YAXJPBTreal_vector3d@@@Z
// @address 0x9cb80 ?object_get_velocities@@YAXJPATreal_vector3d@@0@Z
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "motor/motor_system_biped.h"
#include "motor/weapon_actions.h"
#include "units/bipeds.h"
#include "ai/actor_moving_hints.h"
#include "motor/biped_ground_motor_program.h"
#include "effects/player_effects.h"
#include "objects/object_early_movers.h"
#include "physics/havok_component_class.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "motor/state_change_flag_bits_types.h"
#include "motor/contact_point_types.h"
#include "motor/motor_event_types.h"
#include "cache/cache_files.h"
union real_vector3d;
class c_motor_task;
class c_animation_channel;
class c_animation_manager;

#include "math/real_math.h"
#include "math/matrix_math.h"

#include "units/motor_request.h"


#include "objects/object_header.h"
#include "motor/biped_ground_program_state.h"
#include "units/biped_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "animations/animation_manager.h"

#include "animations/animation_control_data.h"

#include "ai/interaction_target.h"

#include "motor/mover_action_state.h"

struct biped_bunker_program_state { c_motor_program_state motor; unsigned char flags; bool closed; char phase; };

#include "units/biped_definitions.h"

#include "physics/character_physics_move_output.h"

#include "cutscene/cinematic_globals_types.h"


#include "effects/material_effect_types.h"
#include "effects/material_effects.h"
#include "effects/material_effect_enums.h"
#include "game/player_information_definition.h"




// @address 0x382378 ?biped_ground_land_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_ground_land_submit(long object_index, const c_motor_request *motor_request)
{
    biped_start_landing(object_index, motor_request->m_land.scale);
    return _motor_result_pending;
}
static __forceinline char *biped_ground_object_get(int index)
{
    return (char *)object_header_get(index)->datum;
}

static __forceinline biped_datum *biped_ground_unit_get(long object_index)
{
    return (biped_datum *)object_header_get(object_index)->datum;
}

static __forceinline biped_ground_program_state *biped_ground_state_get(biped_datum *unit)
{
    return (biped_ground_program_state *)((char *)unit + unit->unit.motor_offset);
}

// @address 0x382128 ?biped_ground_default_event@@YA_NJPAVc_motor_task@@W4e_motor_event@@@Z
bool biped_ground_default_event(long object_index, c_motor_task *motor_task, e_motor_event motor_event)
{
    bool result = false;
    biped_datum *unit = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
    if (motor_event == 1 && state->snap_facing)
    {
        biped_set_forward_vector(object_index, &unit->unit.facing_vector);
        result = true;
    }
    return result;
}

// @address 0x382198 ?biped_ground_default_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_ground_default_finished(long object_index, c_motor_task *motor_task)
{
    bool result = false;
    long definition_index = ((c_animation_channel *)motor_animation_task_get_channel(object_index, motor_task))->m_definition_index;
    if (definition_index >= 32 && (definition_index <= 33 || definition_index == 0x2c8))
    {
        biped_datum *unit = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
        state->program = -1;
        state->landing_ticks = 0;
        state->landing_duration_ticks = 0;
        unit->biped.landing_root_offset = k_real_zero_storage;
        result = true;
    }
    return result;
}

// @address 0x382248 ?biped_ground_jump_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_ground_jump_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    if ((unsigned short)state->program != _motor_program_airborne && biped->biped.physics.m_mode != c_character_physics_component::_mode_melee)
    {
        if (motor_task_set_mode_and_state(object_index, _motor_task_jump, (e_action)motor_request->action, 226,
            motor_request->m_jump.takeoff ? 477 : 76, 0.1f, 1, 0, 29, motor_request->force()))
        {
            if (motor_request->m_jump.align)
                unit_align_facing2d(object_index, &motor_request->m_jump.direction);
            if (motor_request->m_jump.takeoff)
                biped_ground_unit_get(object_index)->biped.gravity_scale = 0.5f;
            result = _motor_result_success;
        }
    }
    return result;
}

// @address 0x382348 biped_ground_reset_jump
extern "C" int biped_ground_reset_jump(int object_index)
{
    biped_ground_unit_get(object_index)->biped.gravity_scale = 1.0f;
    return 1;
}

static __forceinline void *biped_ground_block_get(long index, const short *offset)
{
    biped_datum *object = biped_ground_unit_get(index);
    return (char *)object + *offset;
}
static __forceinline long biped_ground_motor_program_get(long index)
{
    biped_datum *object = biped_ground_unit_get(index);
    biped_ground_program_state *state = (biped_ground_program_state *)biped_ground_block_get(index, &object->unit.motor_offset);
    return state->motor.program;
}

// @address 0x382420 ?biped_ground_takeoff_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_ground_takeoff_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    if (motor_request->m_takeoff.takeoff)
    {
        biped_datum *biped = biped_ground_unit_get(object_index);
        c_motor_program_state *state = (c_motor_program_state *)biped_ground_block_get(object_index, &biped->unit.motor_offset);
        if (state->set_program(object_index, _motor_program_flying))
            result = _motor_result_pending;
    }
    else
    {
        real_point3d position;
        real_vector3d forward;
        bool finished;
        object_get_origin(object_index, &position);
        ::object_get_orientation(object_index, &forward, 0);
        if (motor_task_set_posture(object_index, _motor_task_takeoff, (e_action)motor_request->action, 477, 476, true, &position, &forward, 1,
                motor_request->force(), &finished))
            result = _motor_result_success;
        else if (finished)
            biped_perch_takeoff_finished(object_index, 0);
    }
    return result;
}


// @address 0x382530 ?biped_ground_takeoff_update@@YA_NJ@Z
bool biped_ground_takeoff_update(long object_index)
{
    bool result = true;
    biped_ground_program_state *state = biped_ground_state_get((biped_datum *)biped_ground_object_get(object_index));
    if ((state->flags & 1) != 0) result = false;
    return result;
}

// @address 0x382578 ?biped_ground_takeoff_interrupted@@YA_NJPAVc_motor_task@@@Z
bool biped_ground_takeoff_interrupted(long object_index, c_motor_task *task)
{
    biped_perch_takeoff_finished(object_index, task);
    return true;
}

// @address 0x3825a0 ?biped_bunker_close_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
static e_motor_result biped_bunker_close_submit(long object_index, const c_motor_request *motor_request)
{
    real_point3d position = motor_request->m_bunker.position;
    real_vector3d forward = motor_request->m_bunker.forward;
    biped_datum *biped = biped_ground_unit_get(object_index);
    e_motor_result result = _motor_result_failure;
    long state = 0;
    switch (motor_request->m_bunker.phase)
    {
    case 1: state = 260; break;
    case 2: state = 261; break;
    case 3: state = 726; break;
    }
    if (state != 0)
    {
        bool finished = false;
        if (motor_task_set_posture(object_index, _motor_task_bunker_close, (e_action)motor_request->action, 84, state, true, &position,
                &forward, 15, motor_request->force(), &finished))
            result = _motor_result_success;
        else if (finished)
            result = _motor_result_pending;
        if (result != _motor_result_failure)
        {
            if (((c_motor_program_state *)biped_ground_state_get(biped))->set_program(object_index, _motor_program_bunker))
            {
                biped_bunker_program_state *bunker = (biped_bunker_program_state *)biped_ground_state_get(biped);
                bunker->closed = true;
                bunker->phase = (char)motor_request->m_bunker.phase;
            }
        }
    }
    return result;
}

// @address 0x3826f8 ?biped_bunker_open_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
static e_motor_result biped_bunker_open_submit(long object_index, const c_motor_request *motor_request)
{
    real_point3d position = motor_request->m_bunker.position;
    real_vector3d forward = motor_request->m_bunker.forward;
    biped_datum *biped = biped_ground_unit_get(object_index);
    e_motor_result result = _motor_result_failure;
    long state = 0;
    switch (motor_request->m_bunker.phase)
    {
    case 1: state = 272; break;
    case 2: state = 273; break;
    case 3: state = 727; break;
    }
    if (state != 0)
    {
        bool finished = false;
        if (motor_task_set_posture(object_index, _motor_task_bunker_open, (e_action)motor_request->action, 84, state, true, &position,
                &forward, 15, motor_request->force(), &finished))
            result = _motor_result_success;
        else if (finished)
            result = _motor_result_pending;
        if (result != _motor_result_failure)
        {
            if (((c_motor_program_state *)biped_ground_state_get(biped))->set_program(object_index, _motor_program_bunker))
            {
                biped_bunker_program_state *bunker = (biped_bunker_program_state *)biped_ground_state_get(biped);
                bunker->closed = false;
                bunker->phase = (char)motor_request->m_bunker.phase;
            }
        }
    }
    return result;
}

// @address 0x382850 ?animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
static e_motor_result animation_task_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    bool special = false;
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    if (state->program != _motor_program_airborne &&
        motor_animation_validate_animation(object_index, motor_request->m_animation.mode, motor_request->m_animation.animation, &special))
    {
        long state = -1;
        if (special)
        {
            if (!motor_request->m_animation.skip_default_mode)
                state = _string_id_combat;
        }
        else
        {
            state = motor_request->m_animation.mode;
        }
        if (state != -1)
        {
            long animation = motor_request->m_animation.animation;
            float scale = motor_request->m_animation.interpolation;
            if (motor_task_start_animation(object_index, _motor_task_animation, (e_action)motor_request->action, animation, state,
                    _unit_weapon_config_none, 0, scale, motor_request->force()))
            {
                if (motor_request->m_animation.has_facing && biped_ground_unit_get(object_index)->object.parent_object_index == -1)
                    unit_align_facing2d(object_index, &motor_request->m_animation.direction);
                result = _motor_result_success;
            }
        }
    }
    return result;
}

// @address 0x382968 ?transient_posture_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result transient_posture_task_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    bool finished = false;
    if (motor_task_set_posture(object_index, _motor_task_transient_posture, (e_action)motor_request->action, motor_request->m_transient_posture.mode,
            motor_request->m_transient_posture.animation, true, &motor_request->m_transient_posture.position,
            &motor_request->m_transient_posture.alignment, 1, motor_request->force(), &finished))
        result = _motor_result_success;
    else if (finished)
        result = _motor_result_pending;
    return result;
}

static __forceinline unsigned short biped_ground_get_program(int index)
{
    biped_datum *unit = biped_ground_unit_get(index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
    return (unsigned short)state->program;
}

static __forceinline void biped_ground_clear_program(int index)
{
    biped_datum *unit = biped_ground_unit_get(index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
    state->program = -1;
    state->landing_ticks = 0;
    state->landing_duration_ticks = 0;
    unit->biped.landing_root_offset = k_real_zero_storage;
}

// @address 0x382f48 biped_ground_reset_motor_program
extern "C" void biped_ground_reset_motor_program(int index)
{
    if (biped_ground_get_program(index) != 0xffff) biped_ground_clear_program(index);
}

// @address 0x382a00 ?transient_posture_task_update@@YA_NJPAVc_motor_task@@@Z
bool transient_posture_task_update(long index, c_motor_task *task)
{
    bool result = true;
    if (biped_ground_motor_program_get(index) == 0)
    {
        biped_datum *biped = biped_ground_unit_get(index);
        biped_ground_program_state *state = (biped_ground_program_state *)biped_ground_block_get(index, &biped->unit.motor_offset);
        if (state->flags & 1)
        {
            mover_state *manager = (mover_state *)biped_ground_block_get(index, &biped->unit.action_offset);
            if (manager->interaction.valid() && biped->unit.actor_index != -1)
                ai_handle_interaction_failure(biped->unit.actor_index, &manager->interaction);
            result = false;
        }
    }
    else result = false;
    return result;
}


// @address 0xe63c0 ?biped_ground_jump_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_ground_jump_finished(long object_index, c_motor_task *task)
{
    bool jumped = biped_jump(object_index);
    biped_datum *biped = biped_ground_unit_get(object_index);
    if (jumped)
        ((c_motor_program_state *)biped_ground_state_get(biped))->set_program(object_index, _motor_program_airborne);
    else
        biped->biped.gravity_scale = 1.0f;
    return true;
}

// @address 0x3823a0 ?biped_perch_takeoff_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_perch_takeoff_finished(long object_index, c_motor_task *task)
{
    c_motor_program_state *state = (c_motor_program_state *)biped_ground_state_get(biped_ground_unit_get(object_index));
    motor_animation_posture_exit_end(object_index, 476, 226, 226, 12);
    state->set_program(object_index, _motor_program_flying);
    return true;
}

// @address 0x382aa0 ?transient_posture_task_interrupted@@YA_NJPAVc_motor_task@@@Z
bool transient_posture_task_interrupted(long object_index, c_motor_task *motor_task)
{
    motor_animation_posture_exit_end(object_index, motor_animation_default_ground_mode(object_index), 226, 226, 12);
    return true;
}

// @address 0x382ae8 ?hoist_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result hoist_task_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    bool interrupted = false;
    long posture = -1;
    switch (motor_request->m_hoist.hoist)
    {
    case 0: posture = 460; break;
    case 1: posture = 461; break;
    case 2: posture = 462; break;
    }
    if (posture != -1)
    {
        if (motor_task_set_posture(object_index, _motor_task_hoist, (e_action)motor_request->action, 84, posture, true,
                &motor_request->m_hoist.position, &motor_request->m_hoist.alignment, 1, motor_request->force(), &interrupted))
            result = _motor_result_success;
        else if (interrupted)
            result = _motor_result_pending;
        else if (posture != 460)
        {
            posture = 460;
            if (motor_task_set_posture(object_index, _motor_task_hoist, (e_action)motor_request->action, 84, posture, true,
                    &motor_request->m_hoist.position, &motor_request->m_hoist.alignment, 1, motor_request->force(), &interrupted))
                result = _motor_result_success;
            else if (interrupted)
                result = _motor_result_pending;
        }
    }
    if (result == _motor_result_success)
        unit_dialogue_test_scream(object_index, 17, -1, 11, 0);
    return result;
}

static __forceinline float biped_ground_magnitude_squared3d(const real_vector3d *v)
{
    return v->i * v->i + v->j * v->j + v->k * v->k;
}

// @address 0x382c20 ?hoist_task_update@@YA_NJPAVc_motor_task@@@Z
bool hoist_task_update(long object_index, c_motor_task *task)
{
    bool result = true;
    if (biped_ground_motor_program_get(object_index) == 0)
    {
        biped_datum *biped = biped_ground_unit_get(object_index);
        biped_ground_program_state *state = (biped_ground_program_state *)biped_ground_block_get(object_index, &biped->unit.motor_offset);
        if (state->flags & 1)
        {
            real_vector3d velocity = state->delta;
            if (velocity.k < 0.0f)
                velocity.k = 0.0f;
            if (biped_ground_magnitude_squared3d(&velocity) > 0.1f * 0.1f)
            {
                mover_state *manager = (mover_state *)biped_ground_block_get(object_index, &biped->unit.action_offset);
                if (manager->interaction.valid() && biped->unit.actor_index != -1)
                {
                    ai_handle_interaction_failure(biped->unit.actor_index, &manager->interaction);
                    unit_dialogue_test_scream(object_index, 20, -1, 11, 0);
                }
                result = false;
            }
        }
    }
    else
        result = false;
    return result;
}

// @address 0x383b50 ?biped_ground_get_desired_mode@@YAJJ@Z
long biped_ground_get_desired_mode(long object_index)
{
    long mode = _string_id_current;
    bool crouch = false;
    bool valid = false;

    biped_datum *unit = biped_ground_unit_get(object_index);
    long desired_mode = unit->unit.animation_state;
    switch (desired_mode)
    {
    case _string_id_patrol:
    case _string_id_combat:
    case _string_id_armored:
    case _string_id_protected:
        if ((unsigned char)((unsigned int)unit->unit.unit_flags >> 22) & 1)
            crouch = true;
    case _string_id_berserk:
    case _string_id_flaming:
    case _string_id_asleep:
    case _string_id_flee:
    case _string_id_stunned:
        valid = true;
        break;

    default:
        mode = _string_id_combat;
        break;
    }

    if (crouch)
    {
        mode = _string_id_crouch;
    }
    else
    {
        if (valid)
            mode = unit->unit.animation_state;
    }

    if (mode != _string_id_combat)
    {
        bool done = false;
        c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        long weapon_class = animation->m_current_goal.weapon_class;
        long weapon_type = animation->m_current_goal.weapon_type;
    
        if (crouch)
        {
            if (motor_animation_test_mode(object_index, mode, weapon_class, weapon_type))
            {
                done = true;
            }
            else if (!valid || (mode = unit->unit.animation_state) == _string_id_combat)
            {
                mode = _string_id_combat;
                done = true;
            }
        }
    
        if (!done)
        {
            if (!motor_animation_test_mode(object_index, mode, weapon_class, weapon_type))
                return _string_id_combat;
        }
    }

    return mode;
}

// @address 0x3846f8 ?get_biped_state_index_from_string@@YADPAVc_animation_manager@@J@Z
char get_biped_state_index_from_string(c_animation_manager *animation_manager, long state)
{
    char result = 2;
    switch (state)
    {
    case _string_id_idle: case _string_id_turn_left: case _string_id_turn_right:
        result = 0;
        break;
    case _string_id_move_front: case _string_id_move_back: case _string_id_move_left: case _string_id_move_right:
    case _string_id_airborne: case _string_id_airborne_dead: case _string_id_airborne_arc:
        result = 1;
        break;
    }
    return result;
}


struct cache_file_tag_instance;


static const float biped_ground_minimum_jump_hold_seconds = 0.16f;

// @address 0x385110 ?biped_update_jumping@@YA_NJ@Z
bool biped_update_jumping(long object_index)
{
    bool result = false;
    biped_datum *biped = biped_ground_unit_get(object_index);
    bool requested = (bool)((biped->unit.control_flags >> 1) & 1);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    signed char count = state->ticks_since_jump;
    if (count < 127)
        state->ticks_since_jump = (signed char)(count + 1);
    if (requested)
    {
        long jump_count = (long)((float)game_time_globals->ticks_per_second * 0.3f + 0.5f);
        if (biped->unit.player_index == -1 || biped->biped.jump_control_ticks < jump_count)
        {
            if (game_time_globals->tick_length * (float)state->ticks_since_jump > biped_ground_minimum_jump_hold_seconds)
            {
                if (biped_jump(object_index))
                {
                    biped->biped.jump_control_ticks = (signed char)jump_count;
                    result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x383eb8 ?biped_update_choose_seat_and_state_from_physics_animation_state@@YAXJW4e_animation_state@s_character_physics_update_output_datum@@_NPAJ2PA_N@Z
static void biped_update_choose_seat_and_state_from_physics_animation_state(
    long object_index,
    s_character_physics_update_output_datum::e_animation_state animation_state,
    bool moving,
    long *mode,
    long *definition_index,
    bool *moved)
{
    if ((unsigned short)biped_ground_state_get(biped_ground_unit_get(object_index))->program == _motor_program_airborne)
        return;

    long desired_mode = biped_ground_get_desired_mode(object_index);
    *mode = desired_mode;
    if (!motor_animation_test_mode(object_index, desired_mode))
        *mode = _string_id_combat;

    if (*definition_index == _string_id_idle)
    {
        switch (animation_state)
        {
        case 1: *definition_index = 12; break;
        case 2: *definition_index = 24; break;
        case 3: *definition_index = 23; break;
        case 4: *definition_index = 21; break;
        case 5: *definition_index = 22; break;
        case 6: *definition_index = 66; break;
        case 7: *definition_index = 65; break;
        case 8: *definition_index = 63; break;
        case 9: *definition_index = 64; break;
        case s_character_physics_update_output_datum::_animation_state_climb_up:
            *definition_index = _string_id_climb_up;
            break;
        case s_character_physics_update_output_datum::_animation_state_climb_down:
            *definition_index = _string_id_climb_down;
            break;
        }

        if (animation_state != 0)
            *moved = moving;

        if (*definition_index == _string_id_idle && !*moved)
        {
            c_animation_channel *channel = (c_animation_channel *)motor_animation_resource_get_channel(object_index, _motor_resource_0);
            if ((channel->m_state_flags.get_unsafe() & 2) == 0 && channel->m_definition_index >= 15 && channel->m_definition_index <= 16)
                *definition_index = channel->m_definition_index;
        }
    }
}


static __forceinline bool biped_ground_is_running_invisible_crouched_uber_melee(long object_index)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    bool result = false;
    if (biped->biped.physics.m_mode == c_character_physics_component::_mode_melee &&
        biped->biped.physics.get_mode_melee()->is_running_invisible_crouched_uber_melee())
        result = true;
    return result;
}

// @address 0x384050 ?biped_ground_update_moving@@YA_NJPAUs_animation_control_data@@PAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@@Z
bool biped_ground_update_moving(long object_index, s_animation_control_data *animation_control_data,
    s_character_physics_update_input_datum *input, s_character_physics_update_output_datum *output)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    c_animation_manager *manager = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    biped_update_choose_seat_and_state_from_physics_animation_state(object_index, output->animation_state, output->moving,
        &animation_control_data->mode, &animation_control_data->state, &animation_control_data->moved);
    if (biped->unit.player_index != -1)
    {
        bool enabled = ((((biped_definition *)tag_get('bipd', biped->object.definition_index))->physics.flags >> 2) & 1) != 0;
        if (enabled)
        {
            long animation_state = animation_control_data->state;
            if (animation_control_data->state == _string_id_idle && state->state_index == 1)
            {
                if (state->control_state_index)
                    state->idle_ticks = 0;
                else
                    state->idle_ticks++;
                if (game_time_globals->tick_length * (float)state->idle_ticks < 0.1f)
                    animation_control_data->state = manager->m_current_goal.state;
            }
            state->control_state_index = get_biped_state_index_from_string((c_animation_manager *)manager, animation_state);
        }
    }
    if (biped->biped.physics.m_mode == c_character_physics_component::_mode_ground && (unsigned short)state->program != 0xffff)
        biped_landing_animation(object_index, animation_control_data);
    else
        biped->biped.landing_root_offset = 0.0f;
    state->flags &= ~1;
    if (output->flags & 4)
    {
        bool enter = true;
        switch (biped->biped.physics.m_mode)
        {
        case 4:
            state->flags |= 1;
            state->delta.i = input->position.x - input->previous_position.x;
            state->delta.j = input->position.y - input->previous_position.y;
            state->delta.k = input->position.z - input->previous_position.z;
            enter = false;
            break;
        case 6:
        {
            mover_state *animation = (mover_state *)((char *)biped + biped->unit.action_offset);
            if (animation->target != -1 && (signed char)animation->target_storage[2] <= (signed char)animation->target_storage[3])
                enter = false;
            else if (biped_ground_is_running_invisible_crouched_uber_melee(object_index))
                ::biped_exit_invisible_crouched_uber_melee(object_index);
            break;
        }
        }
        if (enter)
            ::biped_enter_default_physics(object_index);
    }
    return true;
}

enum e_effect_type;
enum e_contact_point;

// @address 0x384620 ?biped_slipping_sounds@@YAXJPAUs_animation_control_data@@@Z
void biped_slipping_sounds(long object_index, s_animation_control_data *animation_control_data)
{
    real_vector3d velocity;
    float seconds_per_tick = game_time_globals->tick_length;
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    if ((float)state->slipping_ticks * seconds_per_tick > 0.1f)
    {
        object_get_velocities(object_index, &velocity, 0);
        if (biped_ground_magnitude_squared3d(&velocity) > 1.0f)
            biped_make_footstep(object_index, _effect_type_biped_sliding, _contact_point_body, 1.0f);
    }
}

// @address 0x385420 biped_ground_can_fall
extern "C" bool biped_ground_can_fall(int object_index)
{
    float scale = 0.18f;
    biped_datum *unit = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
    biped_definition *definition = (biped_definition *)tag_get('bipd', unit->object.definition_index);
    float multiplier = definition->physics.ground.airborne_reaction_time_scale;
    if (multiplier > 0.0f) scale = multiplier * scale;
    return !(bool)(_rotl(unit->object.physics_flags, 23) & 1) && (unit->biped.physics.m_flags & 2) != 0
        && state->airborne_ticks >= (int)((float)game_time_globals->ticks_per_second * scale + 0.5f)
        && unit->biped.physics.m_mode == c_character_physics_component::_mode_ground;
}


static __forceinline real_vector2d *biped_ground_set_real_vector2d(real_vector2d *vector, float i, float j)
{
    vector->i = i;
    vector->j = j;
    return vector;
}

static __forceinline float biped_ground_dot_product2d(const real_vector2d *a, const real_vector2d *b)
{
    return a->i * b->i + a->j * b->j;
}

static __forceinline float biped_ground_game_tick_length()
{
    return game_time_globals->tick_length;
}

// match: 0x382fb0: precise float keeps fsubs+fsqrts unfused in the pivot discriminant.
#pragma float_control(precise, on, push)
// match: 0x382fb0: fenv access serializes the pivot discriminant FP evaluation.
#pragma fenv_access(on)
static __forceinline float biped_ground_pivot_discriminant(float determinant_term, float radius_term)
{
    return radius_term - determinant_term;
}
// match: 0x382fb0: restores default fenv access after the pivot discriminant.
#pragma fenv_access(off)
// match: 0x382fb0: restores default float control after the pivot discriminant.
#pragma float_control(pop)

static __forceinline bool biped_ground_test_pivot_flag(unsigned short flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}

static __forceinline bool biped_ground_test_physics_flag(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}

// @address 0x382fb0 ?limit_moving_turn_amount_using_pivot@@YAMJM@Z
float limit_moving_turn_amount_using_pivot(long object_index, float turn_amount)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    float result = turn_amount;
    if (biped_ground_test_pivot_flag(biped->biped.flags, 6) && !biped_ground_test_physics_flag(definition->physics.flags, 3))
    {
        real_matrix4x3 world_matrix;
        real_matrix4x3 inverse_matrix;
        real_point3d local_pivot;
        object_get_world_matrix(object_index, &world_matrix);
        memcpy(&inverse_matrix, &world_matrix, sizeof(real_matrix4x3));
        matrix4x3_transform_point(&world_matrix, &biped->biped.moving_turn_position, &inverse_matrix.position);
        matrix4x3_inverse_transform_point(&inverse_matrix, &world_matrix.position, &local_pivot);
        real_vector2d pivot_vector;
        biped_ground_set_real_vector2d(&pivot_vector, local_pivot.x, local_pivot.y);
        float pivot_distance_squared = biped_ground_dot_product2d(&pivot_vector, &pivot_vector);
        if (pivot_distance_squared > 0.0001f)
        {
            real_vector3d velocity;
            object_get_velocities(object_index, &velocity, 0);
            real_point2d future_position;
            float velocity_i = velocity.i;
            float velocity_j = velocity.j;
            float tick_length = biped_ground_game_tick_length();
            future_position.x = world_matrix.position.x - velocity_i * tick_length;
            future_position.y = world_matrix.position.y - velocity_j * tick_length;
            float speed_squared = velocity_i * velocity_i + velocity_j * velocity_j;
            if (speed_squared < 0.0001f)
            {
                result = 0.0f;
            }
            else
            {
                real_vector2d direction;
                direction.i = velocity.i;
                direction.j = velocity.j;
                normalize2d(&direction);
                float dx = direction.j;
                float dy = -direction.i;
                float x1 = future_position.x;
                float y1 = future_position.y;
                float x2 = future_position.x + dx;
                float determinant = x1 * (future_position.y + dy) - x2 * y1;
                float dr_squared = dx * dx + dy * dy;
                float radius_term = pivot_distance_squared * dr_squared;
                float determinant_term = determinant * determinant;
                float discriminant = biped_ground_pivot_discriminant(determinant_term, radius_term);
                if (discriminant > 0.0f)
                {
                    float sign_dy;
                    if (!(dy < 0.0f))
                        sign_dy = 1.0f;
                    else
                        sign_dy = -1.0f;
                    float abs_dy = (float)fabs((double)dy);
                    float root = __fsqrts(biped_ground_pivot_discriminant(determinant_term, radius_term));
                    float inverse_dr_squared = 1.0f / dr_squared;
                    float determinant_dy = determinant * dy;
                    float root_abs_dy = root * abs_dy;
                    float negative_determinant_dx = -(determinant * dx);
                    float root_sign_dx = (root * sign_dy) * dx;
                    real_vector2d intersection0;
                    intersection0.i = (root_sign_dx + determinant_dy) * inverse_dr_squared;
                    intersection0.j = (negative_determinant_dx + root_abs_dy) * inverse_dr_squared;
                    normalize2d(&intersection0);
                    real_vector2d intersection1;
                    intersection1.i = (determinant_dy - root_sign_dx) * inverse_dr_squared;
                    intersection1.j = (negative_determinant_dx - root_abs_dy) * inverse_dr_squared;
                    normalize2d(&intersection1);
                    float angle0 = signed_angle_between_vectors2d(&pivot_vector, &intersection0);
                    float angle1 = signed_angle_between_vectors2d(&pivot_vector, &intersection1);
                    float minimum_angle = angle0 > angle1 ? angle1 : angle0;
                    float maximum_angle = angle0 > angle1 ? angle0 : angle1;
                    result = PIN(turn_amount, minimum_angle, maximum_angle);
                }
            }
        }
    }
    return result;
}


static __forceinline float biped_ground_turning_cosine(float angle)
{
    return (float)cos((double)angle);
}

static __forceinline float biped_ground_turning_sine(float angle)
{
    return (float)sin((double)angle);
}

static __forceinline void biped_ground_facing_cross(const real_vector3d *forward, const real_vector3d *desired, float *result)
{
    *result = desired->i * forward->j - desired->j * forward->i;
}

static __forceinline void biped_ground_facing_dot(const real_vector3d *forward, const real_vector3d *desired, float *result)
{
    *result = forward->i * desired->i + forward->j * desired->j;
}

static __forceinline real_vector2d *biped_ground_rotate_vector2d(const real_vector2d *vector, float sine, float cosine, real_vector2d *result)
{
    float j = sine * vector->i + cosine * vector->j;
    result->i = cosine * vector->i - sine * vector->j;
    result->j = j;
    return result;
}

static __forceinline real_vector3d *biped_ground_set_real_vector3d(real_vector3d *vector, float i, float j, float k)
{
    vector->i = i;
    vector->j = j;
    vector->k = k;
    return vector;
}

// @rdata 0x820221fc
static const float k_biped_ground_maximum_throttle = 1.0f;

// @address 0x383248 ?biped_ground_update_turning@@YA_NJPAUs_animation_control_data@@@Z
bool biped_ground_update_turning(long object_index, s_animation_control_data *animation_control_data)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_definition *definition = (biped_definition *)tag_get('bipd', biped->object.definition_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    c_animation_manager *animation_manager = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    bool result = false;
    float remaining_turn = 0.0f;
    if (biped->biped.physics.m_mode != c_character_physics_component::_mode_dead)
    {
        result = true;
        if (biped->biped.physics.m_mode != c_character_physics_component::_mode_melee &&
            biped->biped.physics.m_mode != c_character_physics_component::_mode_sentinel_b)
        {
            bool climbing = false;
            bool melee = false;
            if (animation_manager->m_current_goal.mode == _string_id_asleep)
                climbing = true;
            else if (animation_manager->m_current_goal.mode == _string_id_flaming)
                melee = true;
            if (!climbing)
            {
                biped->object.up = *global_up3d;
                if (biped->object.forward.k != 0.0f)
                {
                    biped->object.forward.k = 0.0f;
                    if (normalize2d(&biped->object.forward.xy) == 0.0f)
                        biped->object.forward = *global_forward3d;
                }
                real_vector3d facing_vector = biped->unit.facing_vector;
                facing_vector.k = 0.0f;
                if (normalize2d(&facing_vector.xy) == 0.0f)
                    facing_vector = biped->object.forward;
                float facing_cross;
                biped_ground_facing_cross(&biped->object.forward, &facing_vector, &facing_cross);
                float facing_dot;
                biped_ground_facing_dot(&biped->object.forward, &facing_vector, &facing_dot);
                bool turning_left = facing_cross > 0.0f;
                if (facing_dot < -0.9f)
                {
                    if (animation_manager->m_current_goal.state == _string_id_turn_right)
                        turning_left = true;
                    else if (animation_manager->m_current_goal.state == _string_id_turn_left)
                        turning_left = false;
                }
                if ((signed char)state->state_index == 1 || biped_ground_test_physics_flag(definition->flags, 0))
                {
                    float maximum_turning_angle = definition->moving_turning_speed * biped_ground_game_tick_length();
                    real_vector3d new_facing_vector = biped->object.forward;
                    float accumulated_turning_angle = maximum_turning_angle;
                    float turn_amount = maximum_turning_angle;
                    if (biped->unit.player_index == -1)
                    {
                        if (biped_ground_test_pivot_flag(biped->biped.flags, 6))
                        {
                            accumulated_turning_angle += biped->biped.moving_turn_remaining_angle;
                            turn_amount = biped->biped.moving_turn_scale * accumulated_turning_angle;
                        }
                        float signed_turn_amount = turning_left ? -turn_amount : turn_amount;
                        float limited_turn_amount = limit_moving_turn_amount_using_pivot(object_index, signed_turn_amount);
                        turn_amount = PIN((float)fabs((double)limited_turn_amount), 0.0f, k_pi);
                        remaining_turn = MAX(0.0f, accumulated_turning_angle - turn_amount);
                    }
                    float current_facing_angle = (float)fabs((double)angle_between_vectors3d(&facing_vector, &biped->object.forward));
                    if (turn_amount > current_facing_angle * 0.33f)
                        turn_amount = current_facing_angle * 0.33f;
                    float cosine = biped_ground_turning_cosine(turn_amount);
                    float sine = biped_ground_turning_sine(turn_amount);
                    if (turning_left)
                        sine = -sine;
                    if (biped_ground_test_physics_flag(definition->physics.flags, 3))
                    {
                        rotate_vector_about_axis(&new_facing_vector, &biped->object.up, sine, cosine);
                        real_vector3d relative_vector;
                        cross_product3d(&facing_vector, &new_facing_vector, &relative_vector);
                        facing_cross = dot_product3d(&relative_vector, &biped->object.up);
                    }
                    else
                    {
                        biped_ground_rotate_vector2d(&new_facing_vector.xy, sine, cosine, &new_facing_vector.xy);
                        facing_cross = cross_product2d(&facing_vector.xy, &new_facing_vector.xy);
                    }
                    if ((turning_left && facing_cross < 0.0f) || (!turning_left && facing_cross > 0.0f))
                    {
                        if (biped_ground_test_physics_flag(definition->physics.flags, 3))
                        {
                            real_vector3d left;
                            cross_product3d(&biped->object.up, &facing_vector, &left);
                            if (normalize3d(&left) > 0.0f)
                                cross_product3d(&left, &biped->object.up, &new_facing_vector);
                        }
                        else
                        {
                            new_facing_vector = facing_vector;
                        }
                        normalize3d(&new_facing_vector);
                    }
                    if (biped->unit.player_index == -1)
                    {
                        float minimum_throttle = MAX(definition->moving_turn_unknown, 0.1f);
                        float throttle_alignment = dot_product2d(&biped->unit.facing_vector.xy, &new_facing_vector.xy);
                        float maximum_throttle = PIN(throttle_alignment + 0.1f, minimum_throttle, k_biped_ground_maximum_throttle);
                        float throttle_magnitude = square_root(magnitude_squared3d(&biped->unit.analog_blending_throttle));
                        if (maximum_throttle < throttle_magnitude)
                        {
                            float throttle_scale = maximum_throttle / throttle_magnitude;
                            scale_vector3d(&biped->unit.analog_blending_throttle, throttle_scale, &biped->unit.analog_blending_throttle);
                        }
                    }
                    real_vector3d previous_facing_vector = biped->object.forward;
                    biped_set_forward_vector(object_index, &new_facing_vector);
                    if (biped_ground_test_pivot_flag(biped->biped.flags, 6))
                    {
                        biped->biped.velocity = *global_zero_vector2d;
                        if (fabs((double)(previous_facing_vector.i - new_facing_vector.i)) >= 0.0001f ||
                            fabs((double)(previous_facing_vector.j - new_facing_vector.j)) >= 0.0001f ||
                            fabs((double)(previous_facing_vector.k - new_facing_vector.k)) >= 0.0001f)
                        {
                            real_matrix4x3 world;
                            real_matrix4x3 previous_world;
                            object_get_world_matrix(object_index, &world);
                            memcpy(&previous_world, &world, sizeof(real_matrix4x3));
                            previous_world.forward = previous_facing_vector;
                            cross_product3d(&previous_world.up, &previous_world.forward, &previous_world.left);
                            normalize3d(&previous_world.left);
                            cross_product3d(&previous_world.forward, &previous_world.left, &previous_world.up);
                            normalize3d(&previous_world.up);
                            real_point3d pivot;
                            memcpy(&pivot, &biped->biped.moving_turn_position, sizeof(pivot));
                            real_point3d previous_pivot;
                            real_point3d current_pivot;
                            matrix4x3_transform_point(&previous_world, &pivot, &previous_pivot);
                            matrix4x3_transform_point(&world, &pivot, &current_pivot);
                            real_vector3d delta;
                            vector_from_points3d(&current_pivot, &previous_pivot, &delta);
                            real_vector3d velocity;
                            object_get_velocities(object_index, &velocity, 0);
                            scale_vector3d(&delta, (float)game_time_globals->ticks_per_second, &delta);
                            real_vector3d local_velocity;
                            matrix4x3_inverse_transform_vector(&world, &delta, &local_velocity);
                            biped->biped.velocity.i = local_velocity.i;
                            biped->biped.velocity.j = local_velocity.j;
                        }
                    }
                }
                else if ((signed char)state->state_index == 0 && !melee && !biped_ground_test_physics_flag(biped->unit.unit_flags, 11))
                {
                    float turning_threshold = 0.99f;
                    if ((biped->unit.control_flags & 8) == 0)
                        turning_threshold = definition->turning_velocity_maximum + 0.0001f;
                    if (facing_dot <= turning_threshold && !biped_ground_test_physics_flag(definition->unit.flags, 20))
                    {
                        if (biped->unit.player_index == -1 && biped_ground_test_physics_flag(definition->flags, 12) &&
                            sqrtf(biped->unit.throttle.k * biped->unit.throttle.k +
                                (biped->unit.throttle.i * biped->unit.throttle.i +
                                biped->unit.throttle.j * biped->unit.throttle.j)) > 0.001f)
                        {
                            biped_set_forward_vector(object_index, &biped_ground_unit_get(object_index)->unit.facing_vector);
                        }
                        else
                        {
                            animation_control_data->state = (turning_left ? 1 : 0) + 15;
                        }
                    }
                    if (biped->unit.player_index != -1 && !biped_ground_test_physics_flag(definition->flags, 0) &&
                        !biped_ground_test_physics_flag(definition->physics.flags, 3))
                    {
                        real_vector2d desired_facing;
                        real_vector2d current_facing;
                        biped_ground_set_real_vector2d(&desired_facing, biped->unit.facing_vector.i, biped->unit.facing_vector.j);
                        biped_ground_set_real_vector2d(&current_facing, biped->object.forward.i, biped->object.forward.j);
                        normalize2d(&desired_facing);
                        normalize2d(&current_facing);
                        float stationary_facing_dot = dot_product2d(&desired_facing, &current_facing);
                        if (stationary_facing_dot < -0.6f)
                        {
                            float stationary_facing_cross = cross_product2d(&desired_facing, &current_facing);
                            if (stationary_facing_cross > 0.0f)
                                biped_ground_rotate_vector2d(&desired_facing,
                                    (float)sin(120.0 * k_degrees_to_radians),
                                    (float)cos(120.0 * k_degrees_to_radians), &current_facing);
                            else
                                biped_ground_rotate_vector2d(&desired_facing,
                                    (float)sin(-120.0 * k_degrees_to_radians),
                                    (float)cos(-120.0 * k_degrees_to_radians), &current_facing);
                            biped_ground_set_real_vector3d(&biped->object.forward, current_facing.i, current_facing.j, 0.0f);
                        }
                    }
                }
            }
        }
    }
    biped->biped.moving_turn_remaining_angle = remaining_turn;
    return result;
}


// @address 0x3aa90 k_real_math_zero_vector3d_storage

// @address 0x382e48 ?biped_ground_activate@@YA_NJ@Z
bool biped_ground_activate(long object_index)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = (biped_ground_program_state *)biped_ground_block_get(object_index, &biped->unit.motor_offset);
    state->flags = 0;
    state->motor.state = 0;
    state->slipping_ticks = 0;
    state->state_index = 0;
    state->facing_direction = 0;
    state->movement_vector = k_real_math_zero_vector3d_storage;
    state->turn_amount = 0.0f;
    state->turn_velocity = 0.0f;
    state->airborne_ticks = 0;
    state->ticks_since_jump = 127;
    state->landing_ticks = 0;
    state->landing_duration_ticks = 0;
    state->program = -1;
    state->stopped_moving_ticks = 0;
    state->delta = k_real_math_zero_vector3d_storage;
    if (biped->biped.physics.m_mode == c_character_physics_component::_mode_flying || biped->biped.physics.m_mode == c_character_physics_component::_mode_dead)
    {
        biped_datum *current = biped_ground_unit_get(object_index);
        c_character_physics_component *physics = (c_character_physics_component *)&current->biped.physics.m_flags;
        physics->set_mode(c_character_physics_component::_mode_ground);
    }
    return true;
}

// @address 0x385508 ?biped_ground_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_ground_move(long object_index, c_motor_request *request)
{
  e_motor_program result = _motor_program_none;
  biped_datum *unit = biped_ground_unit_get(object_index);
  biped_ground_program_state *state = biped_ground_state_get(unit);
  s_character_physics_move_output_datum output;
  if (motor_system_biped_move(object_index, false, (float)state->ticks_since_jump * game_time_globals->tick_length < 0.16f, false, false, &output) &&
      unit->biped.physics.m_mode == c_character_physics_component::_mode_ground && (output.flags & 2) != 0)
    biped_start_landing(object_index, unit->biped.physics.get_mode_ground()->get_landing_velocity());
  if (biped_update_jumping(object_index) || biped_ground_can_fall(object_index))
    result = _motor_program_airborne;
  return result;
}


e_motor_result biped_ground_dash_submit(long object_index, const c_motor_request *motor_request)
{
    biped_start_melee_dash(object_index, motor_request->m_dash.target_unit_index, motor_request->m_dash.ticks, motor_request->m_dash.flag);
    return _motor_result_pending;
}


static __forceinline real_vector3d *biped_ground_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}

// @address 0x382d38 ?get_infected_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result get_infected_task_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_failure;
    real_point3d position;
    real_vector3d forward;
    object_get_origin(object_index, &position);
    {
        real_point3d target_position;
        object_get_origin(motor_request->m_dash.target_unit_index, &target_position);
        biped_ground_vector_from_points(&position, &target_position, &forward);
    }
    forward.k = 0.0f;
    if (normalize3d(&forward) <= 0.0f)
    {
        real_vector3d up;
        object_get_orientation(object_index, &forward, &up);
    }
    if (motor_task_set_posture(object_index, (e_motor_task)motor_request->m_task, (e_action)motor_request->action, 597, 226, true,
            &position, &forward, 15, false, 0))
    {
        biped_datum *unit = biped_ground_unit_get(object_index);
        unit_action_weapon_stow_immediate(object_index, true);
        unit->object.body_vitality = 1.0f;
        result = _motor_result_success;
    }
    return result;
}
// @address 0x8c443f ?g_biped_fake_hard_landing@@3_NA
bool g_biped_fake_hard_landing;


// @rdata 0x82022dc0
static const float k_biped_ground_landing_absorbtion = 0.5f;

// @address 0x3842b8 ?biped_landing_animation@@YAXJPAUs_animation_control_data@@@Z
void biped_landing_animation(long object_index, s_animation_control_data *animation_control_data)
{
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *motor_state = (biped_ground_program_state *)biped_ground_block_get(object_index, &biped->unit.motor_offset);
    biped->biped.landing_root_offset = 0.0f;
    e_effect_type effect_type = (unsigned short)motor_state->program == _motor_program_airborne ? _effect_type_biped_bodyfall : _effect_type_biped_jump_land;

    bool first = ++motor_state->landing_ticks >= motor_state->landing_duration_ticks && motor_state->landing_ticks <= 1;

    if (motor_state->landing_ticks >= motor_state->landing_duration_ticks)
    {
        biped_datum *unit = biped_ground_unit_get(object_index);
        biped_ground_program_state *state = biped_ground_state_get(unit);
        state->program = -1;
        state->landing_ticks = 0;
        state->landing_duration_ticks = 0;
        unit->biped.landing_root_offset = 0.0f;
    }

    bool cinematic_in_progress = false;
    if (cinematic_globals)
        cinematic_in_progress = cinematic_globals->in_progress;

    if (!cinematic_in_progress && (motor_state->landing_ticks == 1 || first))
        biped_make_footstep(object_index, effect_type, _contact_point_body, k_biped_ground_maximum_throttle);

    if ((unsigned short)motor_state->program != 0xffff)
    {
        if (((c_animation_manager *)((char *)biped + biped->object.animation.offset))->m_current_goal.state == 77
            || ((c_animation_manager *)((char *)biped + biped->object.animation.offset))->m_current_goal.state == _string_id_leap_land)
        {
            biped_datum *unit = biped_ground_unit_get(object_index);
            if (((c_animation_manager *)((char *)unit + unit->object.animation.offset))->find_goal(226, 226, 226, 712, 0))
            {
                animation_control_data->state = _string_id_leap_land;
                return;
            }
        }

        long previous_state = animation_control_data->state;
        biped->biped.landing_root_offset = 0.0f;
        short landing = motor_state->program;
        if (landing == 1)
        {
            animation_control_data->state = 33;
            if (g_biped_fake_hard_landing)
            {
                animation_control_data->state = 12;
                if (motor_state->landing_duration_ticks > 0)
                {
                    float recovery = (float)motor_state->landing_ticks / (float)motor_state->landing_duration_ticks;
                    recovery = PIN(recovery * recovery, 0.0f, k_biped_ground_maximum_throttle);
                    motor_state->turn_velocity = motor_state->turn_amount + motor_state->turn_velocity;

                    motor_state->turn_amount *= k_biped_ground_landing_absorbtion;
                    biped->biped.landing_root_offset = -((k_biped_ground_maximum_throttle - recovery) * motor_state->turn_velocity);
                    return;
                }
            }
        }
        else if (landing == 0)
        {
            animation_control_data->state = 12;
            if (motor_state->landing_duration_ticks > 0)
            {
                float recovery = (float)motor_state->landing_ticks / (float)motor_state->landing_duration_ticks;
                recovery = PIN(recovery * recovery, 0.0f, k_biped_ground_maximum_throttle);
                motor_state->turn_velocity = motor_state->turn_amount + motor_state->turn_velocity;

                motor_state->turn_amount *= k_biped_ground_landing_absorbtion;
                biped->biped.landing_root_offset = (k_biped_ground_maximum_throttle - recovery) * motor_state->turn_velocity * -0.75f;
            }
            if (previous_state == _string_id_move_front || previous_state == _string_id_move_back
                || previous_state == _string_id_move_left || previous_state == _string_id_move_right)
                animation_control_data->state = previous_state;
        }
    }
}

// @address 0x384780 ?biped_try_to_make_footsteps@@YAXJ@Z
void biped_try_to_make_footsteps(long biped_index)
{
    bool loud = false;
    bool animate = false;
    biped_datum *unit = biped_ground_unit_get(biped_index);
    biped_ground_program_state *state = biped_ground_state_get(unit);
    unsigned int mode = (unsigned int)(signed char)state->state_index;
    if (mode >= 1)
    {
        if (mode != 1)
            state->stopped_moving_ticks = 0;
        else
            state->stopped_moving_ticks = 1;
    }
    else
    {
        signed char count = (signed char)state->stopped_moving_ticks;
        if (count > 0)
        {
            count++;
            state->stopped_moving_ticks = count;
            if (game_time_globals->tick_length * (float)count > 0.1f)
            {
                loud = true;
                state->stopped_moving_ticks = 0;
            }
        }
    }
    if (!loud)
    {
        c_animation_manager *manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        if (manager->m_current_goal.state == _string_id_turn_left || manager->m_current_goal.state == _string_id_turn_right)
        {
            if (((c_animation_manager *)manager)->get_authored_state_time() == 0.0f)
                animate = true;
        }
    }
    if (!loud && !animate)
        return;
    float volume = 1.0f;
    if (loud)
    {
        volume = (float)unit->biped.footstep_volume * 0.007874016f;
        volume = PIN(volume, 0.0f, 1.0f);
    }
    biped_make_footstep(biped_index, _effect_type_biped_shuffle, _contact_point_body, volume);
    unit->biped.footstep_volume = 0;
}


// @address 0x384e48 ?biped_jump@@YA_NJ@Z
bool biped_jump(long object_index)
{
    bool result = false;
    biped_datum *biped = biped_ground_unit_get(object_index);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    c_animation_manager *manager = (c_animation_manager *)
        biped_ground_block_get(object_index, &biped->object.animation.offset);
    if ((unsigned short)state->program != _motor_program_airborne && biped->biped.physics.m_mode != c_character_physics_component::_mode_melee)
    {
        float jump_velocity = ((biped_definition *)
            tag_get('bipd', biped->object.definition_index))->jump_velocity;
        real_point3d center_of_mass;
        real_vector3d offset;
        object_get_center_of_mass(object_index, &center_of_mass);
        bool early;
        {
            real_vector3d axis;
            early = object_get_early_mover_local_space_velocity(object_index, &center_of_mass, &offset, &axis, false, true);
        }
        long player_index = biped->unit.player_index;
        float offset_k = offset.k;
        float offset_j = offset.j;
        float offset_i = offset.i;
        result = true;
        float one = 1.0f;
        if (player_index != -1)
        {
            float scale = global_game_globals->player_information.begin()->stun_turning_penalty;
            jump_velocity = (one - scale * biped->unit.look_difficulty_scale) * jump_velocity;
        }
        real_vector3d point;
        object_get_velocities(object_index, &point, 0);
        real_vector3d new_point = point;
        real_vector3d velocity = point;
        if (early)
        {
            velocity.i -= offset_i;
            velocity.j -= offset_j;
            velocity.k -= offset_k;
        }
        float up_dot = biped->object.up.i * velocity.i + (biped->object.up.j * velocity.j + biped->object.up.k * velocity.k);
        if (!(up_dot >= jump_velocity))
            point_from_line3d(&new_point, &biped->object.up, jump_velocity - up_dot, &new_point);
        long actor_index = biped->unit.actor_index;
        if (actor_index != -1)
        {
            bool jump_animation = manager->get_state_name() == 76 || manager->get_state_name() == 77;
            result = actor_moving_compute_jump_velocity(actor_index, object_index, jump_animation,
                jump_velocity, &new_point);
            if (result && early)
            {
                new_point.i += offset_i;
                new_point.j += offset_j;
                new_point.k += offset_k;
            }
        }
        if (result)
        {
            const s_falling_damage_definition *effects =
                (const s_falling_damage_definition *)global_game_globals->falling_damage.address;
            biped_make_footstep(object_index, _effect_type_biped_jump, _contact_point_body, one);
            state->ticks_since_jump = 0;
            object_set_velocities(object_index, &new_point, 0);
            long effect = effects->jumping_damage.index;
            if (effect != -1)
            {
                real_vector3d neg_up;
                negate_vector3d(&biped->unit.aiming_vector, &neg_up);
                player_effect_start(object_index, -1, effect, damage_owner_unknown(), &neg_up, one, one, 0, false);
            }
        }
    }
    return result;
}

static __forceinline void biped_ground_clear_landing_flag(unsigned short *flags)
{
    *flags &= ~2;
}

static __forceinline long biped_ground_round_landing_ticks(float ticks)
{
    return (long)(ticks + 0.5f);
}

static __forceinline float biped_ground_landing_tick_rate_real(const short &ticks_per_second)
{
    return (float)ticks_per_second;
}

static __forceinline float biped_ground_landing_seconds_to_ticks_real(float seconds)
{
    return biped_ground_landing_tick_rate_real(game_time_globals->ticks_per_second) * seconds;
}


// @address 0x385230 ?biped_start_landing@@YAXJM@Z
void biped_start_landing(long object_index, float velocity)
{
    const s_falling_damage_definition *effects =
        (const s_falling_damage_definition *)global_game_globals->falling_damage.address;
    biped_datum *biped = biped_ground_unit_get(object_index);
    bool force_hard_landing = (bool)((biped->biped.flags >> 1) & 1);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    const biped_definition *definition = (const biped_definition *)
        tag_get('bipd', biped->object.definition_index);
    float soft_velocity = definition->minimum_soft_landing_velocity;
    float hard_velocity = definition->minimum_hard_landing_velocity;
    float maximum_velocity = definition->maximum_hard_landing_velocity;
    if (force_hard_landing || velocity >= soft_velocity)
    {
        long effect_index;
        float recovery_time;
        float velocity_delta;
        float velocity_range;
        long landing_type;
        if (force_hard_landing)
        {
            recovery_time = definition->hard_landing_recovery_time;
            effect_index = effects->hard_landing_damage.index;
            velocity_delta = maximum_velocity;
            biped_ground_clear_landing_flag(&biped->biped.flags);
            velocity_range = maximum_velocity - hard_velocity;
            landing_type = 1;
        }
        else
        {
            if (velocity <= 0.0f)
                return;
            if (velocity < hard_velocity)
            {
                recovery_time = definition->soft_landing_recovery_time;
                effect_index = effects->soft_landing_damage.index;
                velocity_delta = velocity - soft_velocity;
                landing_type = 0;
                velocity_range = hard_velocity - soft_velocity;
            }
            else
            {
                recovery_time = definition->hard_landing_recovery_time;
                effect_index = effects->hard_landing_damage.index;
                velocity_delta = velocity;
                velocity_range = maximum_velocity - hard_velocity;
                landing_type = 1;
            }
        }
        if (effect_index != -1)
        {
            real_vector3d direction;
            negate_vector3d(&biped->unit.aiming_vector, &direction);
            player_effect_start(object_index, -1, effect_index, damage_owner_unknown(),
                &direction, 1.0f, 1.0f, 0, false);
        }
        if (velocity_range > 0.0f)
        {
            float scale = velocity_delta / velocity_range;
            scale = PIN(scale, 0.0f, 1.0f);
            long ticks = biped_ground_round_landing_ticks(
                biped_ground_landing_seconds_to_ticks_real(recovery_time) * scale);
            state->program = landing_type;
            state->landing_ticks = 0;
            state->landing_duration_ticks = (signed char)MIN(127, ticks);
            state->turn_amount = game_time_globals->tick_length * velocity;
        }
    }
}


static __forceinline long biped_ground_object_get_root_object(long object_index)
{
    while (object_index != NONE)
    {
        biped_datum *object = biped_ground_unit_get(object_index);
        if (!(bool)((object->object.flags >> 26) & 1))
            break;
        object_index = object->object.parent_object_index;
    }
    return object_index;
}



// @address 0x384920 ?biped_ground_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_ground_update(long object_index, s_animation_control_data *animation_control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    biped_datum *biped = biped_ground_unit_get(object_index);
    c_animation_manager *animation_manager = (c_animation_manager *)((char *)biped + biped->object.animation.offset);
    biped_ground_program_state *state = biped_ground_state_get(biped);
    bool result = false;
    if (biped->object.havok_component_index != NONE && biped_ground_object_get_root_object(biped->object.parent_object_index) == NONE)
    {
        state->state_index = get_biped_state_index_from_string(animation_manager, animation_manager->get_state_name());
        if (biped->biped.physics.m_flags & 2)
        {
            if (state->airborne_ticks < 127)
                state->airborne_ticks++;
            result = true;
        }
        else
        {
            state->airborne_ticks = 0;
        }
        if (biped->biped.physics.m_flags & 1)
        {
            if (state->slipping_ticks < 127)
                state->slipping_ticks++;
            result = true;
        }
        else
        {
            state->slipping_ticks = 0;
        }
        s_character_physics_update_input_datum input;
        biped_update_desired_facing(object_index, false, &state->facing_direction, &state->movement_vector, &state->snap_facing);
        result |= biped_ground_update_turning(object_index, &control);
        result |= biped_update_melee_aiming(object_index, &control);
        s_character_physics_update_output_datum output;
        if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, state->program == _motor_program_airborne, false, 0.0f, false, false))
        {
            biped_ground_update_moving(object_index, &control, &input, &output);
            motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouching);
            result = true;
        }
        if (biped->biped.physics.m_mode == c_character_physics_component::_mode_ground &&
            (biped->biped.physics.m_flags & 1) && (unsigned short)state->program == 0xffff)
            biped_slipping_sounds(object_index, &control);
        biped_try_to_make_footsteps(object_index);
    }
    s_animation_state_change_settings settings;
    s_state_changes_requested state_changes;
    bool force = false;
    motor_animation_build_state_change_settings_from_control_data(object_index, &control, &state_changes, &settings);
    {
        s_animation_state_change_settings current_settings;
        animation_manager->get_current_settings(&current_settings);
        s_state_changes_requested changes;
        if (!current_settings.compare_settings(settings, &changes))
        {
            if ((changes.goals.test(_comparison_goal_field_0_bit) || changes.goals.test(_comparison_goal_field_3_bit))
                && current_settings.transition.m_animation_id.valid())
            {
                const s_animation_goal_settings *previous_goal = animation_manager->get_previous_goal();
                if (current_settings.transition.m_playback_ratio < 0.75f)
                {
                    if (previous_goal->mode != NONE
                        && previous_goal->weapon_class == settings.goals.weapon_class
                        && previous_goal->weapon_type == settings.goals.weapon_type
                        && previous_goal->state != NONE)
                    {
                        long frame_delay;
                        settings.transition.m_animation_id = animation_manager->locate_transition_animation(
                            previous_goal->mode, previous_goal->state, settings.goals.mode, settings.goals.state, &frame_delay);
                        if (settings.transition.m_animation_id.valid())
                        {
                            settings.transition.m_playback_ratio = current_settings.transition.m_playback_ratio;
                            settings.transition.m_playback_flags.set_unsafe(21);
                        }
                        else
                        {
                            settings.transition.m_playback_ratio = 0.0f;
                        }
                        for (long i = 0; i < NUMBEROF(settings.transition.m_blend_ids); i++)
                        {
                            settings.transition.m_blend_ids[i].clear();
                            settings.transition.m_blend_ratios[i] = 0.0f;
                        }
                    }
                }
                else if (previous_goal->mode == settings.goals.mode
                    && previous_goal->weapon_class == settings.goals.weapon_class
                    && previous_goal->weapon_type == settings.goals.weapon_type
                    && previous_goal->state == settings.goals.state
                    && settings.transition.m_animation_id.valid())
                {
                    settings.transition.m_playback_ratio = PIN(1.0f - current_settings.transition.m_playback_ratio, 0.0f, 1.0f);
                }
            }
            if (changes.goals.test(_comparison_goal_field_3_bit) && (unsigned short)biped_ground_state_get(biped)->program != 0xffff)
                settings.interpolation_time = 0.13333334f;
        }
        else
        {
            biped_datum *unit = biped_ground_unit_get(object_index);
            c_motor_task *task = (c_motor_task *)((char *)unit + unit->unit.motor_tasks.offset);
            if (!task || task->program_default())
            {
                c_animation_channel *channel = (c_animation_channel *)motor_animation_resource_get_channel(object_index, _motor_resource_base);
                if ((channel->m_state_flags.get_unsafe() & 2) && !current_settings.primary.m_playback_flags.test(_animation_playback_looping_bit))
                {
                    settings.primary.m_flags.set(_channel_settings_paused_bit, false);
                    settings.primary.m_playback_ratio = 0.0f;
                    force = true;
                    settings.interpolation_time = 0.26666668f;
                }
            }
        }
    }
    result = (NULL != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, force || control.moved)) | result;
    motor_animation_update_analog_blending(object_index);
    return result;
}
