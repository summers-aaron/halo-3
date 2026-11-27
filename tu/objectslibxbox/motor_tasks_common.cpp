// objectslibxbox/motor_tasks_common.obj: from the records 0x395420 0x3954f8 0x395508 0x395740 0x3958b0 0x3959d8 0x395a00 0x395c60 0x395d50 0x396160 0x396188 0x396320 0x3963a8 0x3963d0 0x396430 0x3964b8 0x396718 0x396790 0x396930 0x396a78 0x396bb8 0x396c40 0x396f60 0x397118 0x397370 0x3973c8 0x397518 0x397598 0x3975a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/ai_information_data.h"
#include "units/unit_weapon_config_types.h"
#include "math/real_math_types.h"
#include <string.h>
#include "math/real_math.h"
#include <math.h>
#include "objects/object_reference_types.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/motor_task.h"
#include "units/motor_program_state.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/prop_dialogue.h"
#include "ai/swarms.h"
#include "motor/motor_system.h"
#include "motor/weapon_actions.h"
#include "objects/damage.h"
#include "physics/havok.h"
#include "units/unit_animation.h"
#include "units/unit_dialogue.h"
#include "motor/motor_tasks_common.h"
#include "game/player_types.h"
#include "animations/animation_id.h"
#include "animations/animation_manager.h"
#include "motor/motor_programs.h"
#include "motor/motor_event_types.h"
#include "animations/animation_definitions.h"
#include "cache/cache_files.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "units/biped_types.h"
#include "game/game_time_inline.h"
#include "ai/actor_datum.h"
#include "ai/actor_globals.h"
#include "ai/ai_dialogue.h"
#include "creatures/creature_datum.h"
#include "ai/behavior_posture_info_types.h"
#include "tag_files/tag_types.h"
#include "units/biped_definitions.h"
#include "items/weapon_definitions.h"
#include "ai/dialogue_event.h"
#include "ai/actor_posture.h"
#include "memory/data.h"
#include "tag_files/tag_groups.h"
#include "math/random_math_range.h"
#include "motor/mover_inline.h"
#include "units/unit_inventory_inline.h"
#include "tag_files/string_ids.h"

struct unit_dialogue_information_packet;
struct ai_information_packet;
// @address 0x3954f8 ?animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x3959d8 ?unit_enter_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x396320 ?posture_exit_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x3963a8 ?posture_exit_interrupted@@YA_NJPAVc_motor_task@@@Z
// @address 0x3963d0 ?weapon_animation_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x396430 ?weapon_stow_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x3964b8 ?weapon_draw_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x397118 ?posture_animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x397370 ?biped_force_airborne_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x397598 ?leap_anticipation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x3975a8 ?leap_recovery_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x28eff0 ?find_goal@c_animation_manager@@QBA_NJJJJJ@Z
// @address 0x356178 ?motor_animation_default_ground_mode@@YAJJ@Z
// @address 0x355e10 ?motor_animation_set_mode_weapons_and_state@@YA_NJJJJJMJJ_N@Z
// @address 0xe8418 ?motor_animation_test_mode@@YA_NJJJJ@Z
// @address 0x355460 ?motor_animation_test_mode_and_impulse@@YA_NJJJJJ@Z
// @address 0x356e80 ?motor_animation_unit_entry_finished@@YAXJJ@Z
// @address 0x355538 ?motor_animation_validate_animation@@YA_NJJJPA_N@Z
// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
// @address 0x3577d8 ?motor_task_start_animation@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJW4e_unit_weapon_config@@FM_N@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z
// @address 0x354a18 ?unit_align_facing@@YAXJPBTreal_vector3d@@@Z
// @address 0xd9470 ?unit_weapon_next_index@@YAFJJFF@Z

static __forceinline long motor_tasks_common_mover_get_physics_mode(long object_index)
{
    long result = -1;
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    if (unit->object.type == _object_type_biped)
        result = OBJECT_GET(object_index, biped_datum, _object_mask_biped)->biped.physics.get_mode();
    return result;
}
static __forceinline void motor_tasks_common_posture_exit_end(long object_index)
{
    ::motor_animation_posture_exit_end(object_index, motor_animation_default_ground_mode(object_index), 226, 226, 12);
}
static __forceinline long motor_tasks_common_unit_get_desired_ready_weapon_index(long object_index, long inventory)
{
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    long result = -1;
    if (inventory != -1)
    {
        for (long i = 0; i < 2; ++i)
        {
            if (unit->unit.current_weapon_indices[i] == inventory)
            {
                inventory = -1;
                break;
            }
        }
    }
    if (inventory != -1)
        result = unit->unit.weapon_object_indices[inventory];
    return result;
}
static __forceinline bool motor_tasks_common_motor_animation_validate_current(long object_index, long mode, long impulse)
{
    bool valid = false;
    if (motor_animation_test_mode_and_impulse(object_index, mode, 226, 226, impulse))
        valid = true;
    else
    {
        object_datum *object = OBJECT_GET(object_index, object_datum, _object_mask_unit | _object_mask_creature);
        if (((c_animation_manager *)object_header_block_get(object_get(object_index), &object->animation))->find_goal(mode, 226, 226, impulse, 0))
            valid = true;
    }
    return valid;
}
static __forceinline bool motor_tasks_common_ai_posture_allow_animation(long actor, const long &animation)
{
    long posture = actor_get_posture(actor);
    bool result = true;
    if (posture > 0)
    {
        const behavior_posture_info *info = &k_behavior_posture_info[posture];
        result = false;
        if (!info->disallow)
        {
            switch (animation)
            {
            case _string_id_dive_front:
            case _string_id_dive_back:
            case _string_id_dive_left:
            case _string_id_dive_right:
            case _string_id_surprise_front:
            case _string_id_surprise_back:
            case _string_id_brace:
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x395420 animated_task_submit
extern "C" int animated_task_submit(long object_index, long task, const c_motor_request *request)
{
    int result = _motor_result_failed;
    bool used_default_mode = false;
    if (motor_animation_validate_animation(object_index, request->m_animation.mode, request->m_animation.animation, &used_default_mode) && !used_default_mode)
    {
        long mode = request->m_animation.mode;
        long impulse = request->m_animation.animation;
        float speed = request->m_animation.interpolation;
        if (motor_task_start_animation(object_index, (e_motor_task)task, request->action, impulse, mode, _unit_weapon_config_none, 0, speed, request->force()))
        {
            if (request->m_animation.has_facing && object_get(object_index)->parent_object_index == -1)
                unit_align_facing(object_index, &request->m_animation.facing);
            result = _motor_result_success;
        }
    }
    return result;
}

static e_motor_result animation_task_submit(long object_index, const c_motor_request *request)
{
    return (e_motor_result)animated_task_submit(object_index, 35, request);
}


static __forceinline bool motor_tasks_common_biped_test_flag(const biped_definition *definition, long bit)
{
    return (definition->flags & (1 << bit)) != 0;
}

// @address 0x395740 ?biped_stun_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_stun_submit(long object_index, const c_motor_request *request)
{
    biped_datum *biped = OBJECT_GET(object_index, biped_datum, _object_mask_biped);
    const biped_definition *definition = ((biped_definition *)tag_get(0, biped->object.definition_index));
    biped->biped.stun_ticks = (unsigned char)(game_seconds_to_ticks_round(definition->stun_duration) < 255
            ? game_seconds_to_ticks_round(definition->stun_duration)
            : 255);
    ::unit_scream(object_index, 21, -1, 13, 0, 0);
    if (biped->biped.stun_ticks > 1 && motor_tasks_common_biped_test_flag(definition, 10) && biped->biped.physics.get_mode() != 3)
    {
        long half_ticks = biped->biped.stun_ticks >> 1;
        if (half_ticks > 0)
            biped->biped.stun_ticks += _random_range(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__, 0, (short)half_ticks);
        object_set_at_rest(object_index, false);
        c_motor_program_state *state = (c_motor_program_state *)((char *)biped + biped->unit.motor_program.offset);
        state->set_program(object_index, _motor_program_dead);
    }
    return _motor_result_pending;
}


// @address 0x3958b0 ?unit_enter_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result unit_enter_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    bool force = request->force();
    bool animate = request->m_unit_enter.animate;
    short seat_index = request->m_unit_enter.seat_index;
    long vehicle_index = request->m_unit_enter.vehicle_index;
    e_action action = request->action;
    bool immediate = false;
    c_motor_task *task = 0;
    if ((force || motor_task_allow(object_index, action, 15)) &&
        motor_task_enter_seat_internal(object_index, vehicle_index, seat_index, animate, action, &immediate, force))
    {
        if (immediate)
            motor_animation_unit_entry_finished(object_index, action);
        else
            task = motor_task_new(object_index, _motor_resource_base, 1, 15, _motor_task_enter, action);
    }
    if (task || immediate)
    {
        result = immediate ? _motor_result_pending : _motor_result_success;
        s_object_header *header = object_header_get(object_index);
        if (header->object_type == _object_type_biped)
        {
            biped_datum *biped = (biped_datum *)header->datum;
            c_motor_program_state *state = (c_motor_program_state *)((char *)biped + biped->unit.motor_program.offset);
            state->set_program(object_index, _motor_program_biped_vehicle);
        }
    }
    return result;
}

bool unit_enter_finished(long object_index, c_motor_task *motor_task)
{
    motor_animation_unit_entry_finished(object_index, 17);
    return true;
}


static __forceinline c_motor_task *motor_tasks_common_motor_task_exit_seat(long unit_index, e_motor_task type, e_action action, bool force)
{
    c_motor_task *task = 0;
    if (force || motor_task_allow(unit_index, action, 15))
    {
        bool exited = false;
        if (motor_animation_try_and_exit_seat(unit_index, action == 14 ? 1190 : 85, &exited) && !exited)
            task = motor_task_new(unit_index, _motor_resource_base, 1, 15, type, action);
    }
    return task;
}

static __forceinline void motor_tasks_common_actor_handle_seat_exit_start(long actor_index)
{
    if (tls_actor_globals()->enabled)
        actor_dialogue_event(actor_index, 61, datum_get<actor_datum>(tls_actor_data(), actor_index)->root_vehicle_index, 0);
}

static __forceinline void motor_tasks_common_ai_player_handle_seat_exit_start(long player_index)
{
    c_dialogue_event event(61, datum_get<player_datum>(tls_player_data(), player_index)->unit_index, -1, -1, -1, 0);
    ai_dialogue_event(&event);
}

// @address 0x395a00 ?unit_exit_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result unit_exit_submit(long object_index, const c_motor_request *request)
{
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    e_motor_result result = _motor_result_failed;
    if (request->m_exit.immediate)
    {
        e_motor_program program = _motor_program_none;
        if (request->force() || motor_task_allow(object_index, request->action, 1))
        {
            motor_task_interrupt(object_index, 1, 0);
            motor_animation_exit_seat_reset_velocity(object_index, 0, 0, false, 0, (long *)&program);
            if (object_header_get(object_index)->object_type == _object_type_biped)
            {
                c_motor_program_state *state = (c_motor_program_state *)((char *)unit + unit->unit.motor_program.offset);
                state->set_program(object_index, _motor_program_airborne);
            }
            result = _motor_result_pending;
        }
    }
    else
    {
        unit->unit.seat_exit_queued = false;
        if (motor_tasks_common_motor_task_exit_seat(object_index, _motor_task_exit, request->action, request->force()))
        {
            unsigned long *flags = (unsigned long *)((char *)unit + unit->unit.action.offset);
            if (request->m_exit.exit_to_default)
                *flags |= 1;
            else
                *flags &= ~1;
            if (request->m_exit.seat_exit_flag_1)
                *flags |= 2;
            else
                *flags &= ~2;
            if (unit->unit.actor_index != -1)
                motor_tasks_common_actor_handle_seat_exit_start(unit->unit.actor_index);
            else if (unit->unit.player_index != -1)
                motor_tasks_common_ai_player_handle_seat_exit_start(unit->unit.player_index);
            result = _motor_result_success;
        }
    }
    return result;
}


// @address 0x395d50 ?biped_accelerate@@YAXJPBTreal_vector3d@@@Z
void biped_accelerate(long biped_index, const real_vector3d *acceleration)
{
    biped_datum *biped = OBJECT_GET(biped_index, biped_datum, _object_mask_biped);
    const biped_definition *definition = ((biped_definition *)tag_get(0, biped->object.definition_index));
    if (!(bool)((definition->unit.flags >> 20) & 1))
    {
        bool relaxing = motor_system_is_relaxed(biped_index) != 0;
        bool airborne;
        if (relaxing || magnitude_squared3d(acceleration) > 1.4f * 1.4f)
            airborne = true;
        else
            airborne = false;
        bool apply_angular = false;
        real_vector3d linear_acceleration = *acceleration;
        real_vector3d angular_acceleration = *global_zero_vector3d;
        if (!(bool)((biped->object.damage_flags >> 2) & 1))
            scale_vector3d(&linear_acceleration, 0.5f, &linear_acceleration);
        if (!relaxing)
        {
            float magnitude = sqrtf(magnitude_squared3d(&linear_acceleration));
            normalize3d(&linear_acceleration);
            linear_acceleration.k += 0.45f;
            normalize3d(&linear_acceleration);
            linear_acceleration.i *= magnitude;
            linear_acceleration.j *= magnitude;
            linear_acceleration.k *= magnitude;
        }
        biped->biped.physics.m_flags |= 1;
        if ((bool)((biped->object.damage_flags >> 2) & 1) || (bool)((definition->physics.flags >> 3) & 1) || biped->biped.physics.get_mode() == 2)
        {
            unsigned long *seed = tls_random_seed();
            random_seed_next(seed);
            float random = (float)(*seed >> 16) * 1.5259022e-05f;
            float angular_speed = sqrtf(magnitude_squared3d(&linear_acceleration)) * ((random + 1.0f) * 0.5f) * 1.5707964f;
            if (angular_speed >= 0.3f)
                angular_speed = 0.3f;
            real_vector3d axis;
            cross_product3d(global_up3d, &linear_acceleration, &axis);
            normalize3d(&axis);
            scale_vector3d(&axis, angular_speed, &angular_acceleration);
            apply_angular = airborne || !(bool)((biped->object.damage_flags >> 2) & 1);
        }
        if (airborne)
        {
            c_motor_request request;
            request.setup_force_airborne(_action_none);
            motor_system_submit(biped_index, &request);
        }
        bool apply_linear = biped->biped.physics.get_mode() != 3 || airborne;
        if (apply_linear || apply_angular)
            object_apply_acceleration(biped_index, -1, NULL, apply_linear ? &linear_acceleration : NULL, apply_angular ? &angular_acceleration : NULL);
        if ((bool)((biped->object.damage_flags >> 2) & 1) && airborne && biped->object.parent_object_index == -1 && biped->biped.physics.get_mode() == 3 &&
            motor_system_is_relaxed(biped_index) == 0 && acceleration->i * acceleration->i + (acceleration->j * acceleration->j +
                acceleration->k * acceleration->k) > 1.4f * 1.4f)
        {
            real_vector3d forward = *acceleration;
            forward.k = 0.0f;
            if (normalize3d(&forward) > 0.0f)
                object_set_position_direct(biped_index, NULL, &forward, global_up3d, NULL, false);
        }
    }
}

// @address 0x396160 ?biped_accelerate_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_accelerate_submit(long object_index, const c_motor_request *request)
{
    biped_accelerate(object_index, &request->m_accelerate.acceleration);
    return _motor_result_pending;
}

// @rdata 0x82022dbc
static const float k_posture_exit_interpolation_time = 0.1f;

// @address 0x396188 ?posture_exit_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result posture_exit_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    if (motor_tasks_common_mover_get_physics_mode(object_index) == 4 || mover_get_motor_program(object_index) == _motor_program_posture)
    {
        if (request->m_posture_exit.immediate)
            result = _motor_result_pending;
        else
        {
            bool force = request->force();
            if (force || motor_task_allow(object_index, request->action, 1))
            {
                c_motor_task *task = 0;
                if (request->m_posture_exit.animate)
                {
                    force = request->force();
                    task = motor_task_set_mode_and_state(object_index, _motor_task_exit, request->action, 226, 86,
                        k_posture_exit_interpolation_time, 15, 0, 29, force);
                }
                if (!task)
                    task = motor_task_set_mode_and_state(object_index, _motor_task_exit, request->action, 226, 85,
                        k_posture_exit_interpolation_time, 15, 0, 29, request->force());
                if (task)
                {
                    task->posture_exit_to_ground = request->m_posture_exit.exit_to_default;
                    result = _motor_result_success;
                }
                else
                    result = _motor_result_pending;
            }
            else
                result = _motor_result_failed;
        }
        if (result == _motor_result_pending)
        {
            c_motor_program_state *state = (c_motor_program_state *)((char *)object + object->unit.motor_program.offset);
            motor_tasks_common_posture_exit_end(object_index);
            if (request->m_posture_exit.exit_to_default)
                state->set_program(object_index, _motor_program_ground);
            else
                state->set_program(object_index, _motor_program_airborne);
        }
    }
    return result;
}

bool posture_exit_finished(long object_index, c_motor_task *motor_task)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object_get(object_index), &object->unit.motor_program);
    motor_tasks_common_posture_exit_end(object_index);
    if (motor_task->posture_exit_to_ground)
        state->set_program(object_index, _motor_program_ground);
    else
        state->set_program(object_index, _motor_program_airborne);
    return true;
}

bool posture_exit_interrupted(long object_index, c_motor_task *motor_task)
{
    posture_exit_finished(object_index, motor_task);
    return true;
}

e_motor_result weapon_animation_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    bool force = request->force();
    float speed = request->m_weapon_animation.interpolation;
    long config = request->m_weapon_animation.config;
    long animation = request->m_weapon_animation.animation;
    e_action action = request->action;
    if (motor_task_start_animation(object_index, _motor_task_weapon_animation, action, animation, 226, (e_unit_weapon_config)config, 0, speed, force))
    {
        result = _motor_result_success;
    }
    return result;
}

e_motor_result weapon_stow_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    signed char ready_index = unit->unit.current_weapon_indices[0];
    if (ready_index != -1 && unit->unit.weapon_object_indices[ready_index] != -1 && unit_action_weapon_stow_immediate(object_index, true))
        result = _motor_result_pending;
    return result;
}

e_motor_result weapon_draw_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    if (unit_inventory_get_weapon_or_none(object_index, unit->unit.current_weapon_indices[0]) != -1)
    {
        result = _motor_result_pending;
    }
    else if (request->m_weapon_draw.instant || motor_task_allow(object_index, request->action, 15))
    {
        result = _motor_result_pending;
        short inventory = unit_weapon_next_index(object_index, 139, -1, 0);
        long mode;
        if (inventory != -1)
        {
            mode = 139;
        }
        else
        {
            inventory = unit_weapon_next_index(object_index, 138, -1, 0);
            if (inventory != -1)
                mode = 138;
        }
        if (inventory != -1)
        {
            unit->unit.desired_weapon_indices[0] = (unsigned char)inventory;
            long weapon_index = motor_tasks_common_unit_get_desired_ready_weapon_index(object_index, inventory);
            if (weapon_index != -1)
            {
                object_datum *weapon = OBJECT_GET(weapon_index, object_datum, _object_mask_weapon);
                weapon_definition *definition = ((weapon_definition *)tag_get(0, weapon->definition));
                long weapon_class = definition->animation_weapon_class;
                long weapon_type = definition->animation_weapon_type;
                if (motor_animation_test_mode(object_index, mode, weapon_class, weapon_type))
                {
                    c_motor_task *task = 0;
                    bool alternate = request->m_weapon_draw.alternate;
                    long animation = alternate ? 974 : 975;
                    ::unit_animation_set_weapon(object_index, weapon_class, weapon_type, true);
                    if (!request->m_weapon_draw.instant)
                        task = motor_task_start_animation(object_index, request->m_task, request->action, animation, mode,
                            _unit_weapon_config_secondary, 12, k_standard_interpolation_time, true);
                    if (task)
                    {
                        result = _motor_result_success;
                    }
                    else if (request->m_weapon_draw.instant ||
                        motor_animation_set_mode_weapons_and_state(
                            object_index, mode, weapon_class, weapon_type, 12, 0.0f, 2, 29, false))
                    {
                        ::weapon_draw_event(object_index, task, (e_motor_event)0);
                        result = _motor_result_pending;
                    }
                }
            }
        }
    }
    return result;
}

// Per unit ping type: the animation damage type, animation name and animation class the ping plays.
// @rdata 0x8202861c..0x8202867c
static const long k_unit_ping_damage_types[8] = { -1, 0, 1, 2, 2, -1, -1, 3 };
static const long k_unit_ping_animation_names[8] = {
    -1, -1, _string_id_h_ping, _string_id_landing_dead, _string_id_landing_dead, _string_id_airborne_dead, _string_id_landing_dead, _string_id_landing_dead
};
static const long k_unit_ping_animation_classes[8] = {
    -1, -1, -1, _string_id_combat, _string_id_combat, _string_id_combat, _string_id_combat, _string_id_combat
};
// @rdata-end

void motor_ping_get_animation_data(short ping_type, long *animation_name, long *animation_class, long *damage_name)
{
    if (animation_name)
        *animation_name = k_unit_ping_animation_names[ping_type];
    if (animation_class)
        *animation_class = k_unit_ping_animation_classes[ping_type];
    if (damage_name)
    {
        short damage_type = (short)k_unit_ping_damage_types[ping_type];
        if (damage_type != -1)
            *damage_name = damage_type_string_id_list[damage_type];
        else
            *damage_name = -1;
    }
}


// @address 0x396c40 ?ping_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result ping_task_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    bool align = request->m_ping.align;
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit | _object_mask_creature);
    unsigned short *ping_flags = (unsigned short *)object_header_block_get(object_get(object_index), &unit->unit.animation_storage);
    c_animation_manager *animation_manager =
        (c_animation_manager *)object_header_block_get(object_get(object_index), &unit->object.animation);
    short actual_direction = -1;
    long animation, animation_class, damage;
    motor_ping_get_animation_data(request->m_ping.type, &animation, &animation_class, &damage);

    long mode = -1;
    e_motor_resource resource;
    if (animation != -1)
    {
        mode = 226;
        if ((unit->object.parent_object_index == -1 || unit->unit.parent_seat_index == -1) && animation_class != -1)
            mode = animation_class;
        resource = _motor_resource_0;
    }
    else
    {
        resource = _motor_resource_1;
    }

    short actual_region;
    if (motor_task_set_ping(object_index, _motor_task_ping, request->action, resource, animation, mode, damage, request->m_ping.direction,
            request->m_ping.region, &actual_direction, &actual_region))
    {
        if (resource == 0)
        {
            object_start_interpolation(object_index, 0.13333334f);
            *ping_flags |= 1;
        }
        result = _motor_result_success;
        align = (actual_direction == request->m_ping.direction) & align;
    }

    if (request->m_ping.type >= 3)
    {
        bool enable_ping_direction = actual_direction == 3;
        if (enable_ping_direction)
            *ping_flags |= 8;
        else
            *ping_flags &= ~8;
        if (result == _motor_result_failed)
        {
            const biped_definition *definition = ((biped_definition *)tag_get(0, unit->object.definition_index));
            animation_manager->m_flags |= 1;
            *ping_flags |= 4;
            bool destroy_on_death = (definition->unit.flags >> 1) & 1;
            if (destroy_on_death && !simulation_query_object_is_predicted(object_index))
                ::object_destroy_internal(object_index, -1, -1, 0, 0);
        }
    }

    if (result != _motor_result_failed && align && actual_direction != -1)
    {
        real_vector3d facing;
        switch (actual_direction)
        {
        case 3:
            facing = request->m_ping.facing;
            break;
        case 0:
            negate_vector3d(&request->m_ping.facing, &facing);
            break;
        case 1:
            cross_product3d(&unit->object.up, &request->m_ping.facing, &facing);
            break;
        case 2:
            cross_product3d(&request->m_ping.facing, &unit->object.up, &facing);
            break;
        default:
            // match: 0x396c40: unreachable hint omits dead branch check
            __assume(0);
        }
        unit_align_facing(object_index, &facing);
    }
    return result;
}

// @address 0x73bc08 ?g_ping_task@@3Us_motor_task_definition@@A
s_motor_task_definition g_ping_task = { _motor_task_ping, "ping", ping_task_submit, 0, 0, 0, 0 };

e_motor_result posture_animation_task_submit(long object_index, const c_motor_request *request)
{
    long previous_program = mover_get_motor_program(object_index);
    long mode = 226;
    e_motor_result result = _motor_result_failed;
    bool valid = motor_tasks_common_motor_animation_validate_current(object_index, mode, request->m_animation.animation);
    if (!valid)
    {
        biped_datum *unit = OBJECT_GET(object_index, biped_datum, _object_mask_biped);
        bool allowed = true;
        if (unit->unit.actor_index != -1)
            allowed = motor_tasks_common_ai_posture_allow_animation(unit->unit.actor_index, request->m_animation.animation);
        if (allowed)
        {
            long ground_mode = motor_animation_default_ground_mode(object_index);
            bool used_default = false;
            if (motor_animation_validate_animation(object_index, ground_mode, request->m_animation.animation, &used_default))
            {
                if (used_default)
                {
                    if (request->m_animation.skip_default_mode)
                        mode = -1;
                    else
                        mode = 138;
                }
                else
                {
                    mode = ground_mode;
                }
                if (mode != -1)
                {
                    c_motor_program_state *state = (c_motor_program_state *)((char *)unit + unit->unit.motor_program.offset);
                    ::motor_animation_posture_exit_end(object_index, motor_animation_default_ground_mode(object_index), 226, 226, 12);
                    state->set_program(object_index, _motor_program_ground);
                    valid = true;
                }
            }
        }
    }
    if (valid)
    {
        bool force = request->force();
        float playback_speed = request->m_animation.interpolation;
        long animation = request->m_animation.animation;
        e_action action = request->action;
        c_motor_task *task = motor_task_start_animation(
            object_index, _motor_task_animation, action, animation, mode, _unit_weapon_config_none, 0, playback_speed, force);
        if (task)
        {
            task->program = (e_motor_program)previous_program;
            result = _motor_result_success;
        }
    }
    return result;
}

e_motor_result biped_force_airborne_submit(long object_index, const c_motor_request *request)
{
    biped_datum *object = OBJECT_GET(object_index, biped_datum, _object_mask_biped);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object_get(object_index), &object->unit.motor_program);
    state->set_program(object_index, _motor_program_airborne);
    return _motor_result_pending;
}

e_motor_result leap_anticipation_task_submit(long object_index, const c_motor_request *request)
{
    return (e_motor_result)animated_task_submit(object_index, 41, request);
}

e_motor_result leap_recovery_task_submit(long object_index, const c_motor_request *request)
{
    return (e_motor_result)animated_task_submit(object_index, 43, request);
}


static __forceinline c_motor_task *motor_tasks_common_throw_first(long object_index, e_motor_task type, e_action action, float speed, bool force)
{
    return motor_task_start_animation(object_index, type, action, 112, 226, _unit_weapon_config_none, 0, speed, force);
}
static __forceinline c_motor_task *motor_tasks_common_throw_second(long object_index, e_motor_task type, e_action action, float speed, bool force)
{
    return motor_task_start_animation(object_index, type, action, 34, 226, _unit_weapon_config_none, 0, speed, force);
}
static __forceinline c_motor_task *motor_tasks_common_throw_third(long object_index, e_motor_task type, e_action action, float speed, bool force)
{
    return motor_task_start_animation(object_index, type, action, 34, 138, _unit_weapon_config_none, 0, speed, force);
}

// @address 0x396790 ?throw_grenade_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result throw_grenade_submit(long object_index, const c_motor_request *request)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    bool overheated = unit_weapon_in_overheated_loop(object_index, 0);
    e_motor_result result = _motor_result_failed;
    const float speed = 0.13333334f;
    if (overheated != 0 && motor_tasks_common_throw_first(object_index, request->m_task, request->action, speed, request->force()) != 0)
        result = _motor_result_success;
    if ((overheated == 0 || result == _motor_result_failed) &&
        motor_tasks_common_throw_second(object_index, request->m_task, request->action, speed, request->force()) != 0)
        result = _motor_result_success;
    if (result == _motor_result_failed)
    {
        long motor_program = mover_get_motor_program(object_index);
        int mode;
        int state;
        if ((motor_program == _motor_program_ground || motor_program == _motor_program_climb)
            && ::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && mode != 138
            && motor_tasks_common_throw_third(object_index, request->m_task, request->action, speed, request->force()) != 0)
            result = _motor_result_success;
    }
    if (result != _motor_result_failed && object->object.parent_object_index == -1)
        unit_align_facing(object_index, &OBJECT_GET(object_index, unit_datum, _object_mask_all)->unit.aiming_vector);
    return result;
}


static __forceinline c_motor_task *motor_tasks_common_start_stand(long object_index, e_motor_task type, e_action action, bool force)
{
    return motor_task_start_animation(object_index, type, action, 35, 226, _unit_weapon_config_none, 0, 0.13333334f, force);
}

// @address 0x396930 stand_submit
extern "C" int stand_submit(long object_index, const c_motor_request *request)
{
    int result = _motor_result_failed;
    object_datum *object = object_get(object_index);
    const float speed = 0.13333334f;
    long motor_program;
    int mode;
    int state;
    if (motor_tasks_common_start_stand(object_index, request->m_task, request->action, request->force()) != 0
        || (((motor_program = mover_get_motor_program(object_index)) == _motor_program_ground || motor_program == _motor_program_climb) &&
            ::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && mode != 138
            && motor_task_start_animation(object_index, request->m_task, request->action, 35, 138, _unit_weapon_config_none, 0, speed, request->force()) != 0))
    {
        result = _motor_result_success;
        if (object->parent_object_index == -1)
            unit_align_facing(object_index, &OBJECT_GET(object_index, unit_datum, _object_mask_all)->unit.aiming_vector);
    }
    return result;
}

// @address 0x397518 ?biped_force_heavy_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_force_heavy_submit(long object_index, const c_motor_request *request)
{
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(unit, &unit->unit.motor_program);
    if (request->m_force_heavy.heavy != TEST_BIT(state->state, 2))
    {
        if (request->m_force_heavy.heavy)
            state->state |= FLAG(2);
        else
            state->state &= ~FLAG(2);
        ::havok_object_rebuild(object_index, 0);
    }
    return _motor_result_pending;
}

static __forceinline bool motor_tasks_common_task_conflicts(const c_motor_task *task, short resource_mask)
{
    return (resource_mask & task->resources) != 0;
}

static __forceinline void motor_tasks_common_reset_interpolation(long object_index)
{
    object_datum *object = object_get(object_index);
    c_animation_manager *animation_manager = (c_animation_manager *)object_header_block_get(object_get(object_index), &object->animation);
    animation_manager->animation_interpolator()->disable();
    object_wake(object_index);
}

// @address 0x395508 ?unit_die_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result unit_die_submit(long object_index, const c_motor_request *request)
{
    s_object_header *header = object_header_get(object_index);
    unit_datum *object = (unit_datum *)header->datum;
    long object_type = header->object_type;
    c_motor_program_state *state = (c_motor_program_state *)((char *)object + object->unit.motor_program.offset);
    bool was_seated = false;
    if (object->unit.parent_seat_index != -1)
    {
        motor_animation_exit_seat_reset_velocity(object_index, 0, 0, false, 0, 0);
        motor_task_interrupt(object_index, 15, 0);
        was_seated = true;
        if (object_type == 0)
        {
            motor_task_set_mode_and_state(object_index, _motor_task_default, _action_none, 226, 79, 0.0f, 1, 528, 31, true);
            motor_tasks_common_reset_interpolation(object_index);
        }
    }
    if ((1L << object_type) & 1)
    {
        biped_datum *biped = OBJECT_GET(object_index, biped_datum, _object_mask_biped);
        bool airborne = motor_system_in_airborne_state(object_index);
        bool dead_in_air = state->program == _motor_program_custom_animation && (bool)((biped->biped.flags >> 11) & 1)
            && (bool)(((biped->biped.flags >> 12) & 15) & 1);
        state->set_program(object_index, _motor_program_dead);
        if (request->m_die.relax)
        {
            c_motor_request death_request;
            death_request.setup_force_relaxation_state(_action_none, 0, false);
            motor_system_submit(object_index, &death_request);
        }
        if (was_seated || airborne || dead_in_air)
        {
            c_motor_request airborne_request;
            airborne_request.setup_force_airborne(_action_none);
            motor_system_submit(object_index, &airborne_request);
        }
        if (dead_in_air)
            motor_tasks_common_reset_interpolation(object_index);
    }
    return _motor_result_pending;
}

// @address 0x396a78 ?melee_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result melee_task_submit(long object_index, const c_motor_request *request)
{
    const long animation = request->m_melee.animation;
    const e_action action = request->action;
    e_motor_result result = _motor_result_failed;
    c_motor_task *task = motor_task_start_animation(object_index, _motor_task_melee, action, animation, 226, _unit_weapon_config_none, 0, 0.0f, false);
    if (task)
    {
        result = _motor_result_success;
        if (!request->m_melee.skip_reset)
            motor_tasks_common_reset_interpolation(object_index);
        if (motor_tasks_common_task_conflicts(task, 1))
        {
            s_object_header *header = object_header_get(object_index);
            unit_datum *object = (unit_datum *)header->datum;
            if (object->object.parent_object_index == -1 && ((1L << header->object_type) & 0x1003))
            {
                if (request->m_melee.use_direction)
                    unit_align_facing2d(object_index, &request->m_melee.direction);
                else
                    unit_align_facing(object_index, &object->unit.aiming_vector);
            }
        }
    }
    return result;
}


static __forceinline creature_datum *motor_tasks_common_creature_get(long creature_index)
{
    return OBJECT_GET(creature_index, creature_datum, 4096);
}

static __forceinline c_motor_task *motor_tasks_common_motor_task_get(long object_index, long resource)
{
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_motor_task *tasks = (c_motor_task *)object_header_block_get(object_get(object_index), &object->unit.motor_tasks);
    return &tasks[resource];
}

static __forceinline void motor_tasks_common_ai_creature_handle_action_finished(long creature_index, long mode_name, long state_name)
{
    creature_datum *creature = motor_tasks_common_creature_get(creature_index);
    switch (creature->creature.link_type)
    {
    case 0:
        swarm_creature_handle_action_finished(creature_index, mode_name, state_name);
        break;
    }
}

// @address 0x3973c8 ?creature_ground_default_finished@@YA_NJPAVc_motor_task@@@Z
bool creature_ground_default_finished(long creature_index, c_motor_task *motor_task)
{
    bool finished = false;
    if (motor_tasks_common_motor_task_get(creature_index, 0) == motor_task)
    {
        creature_datum *creature = motor_tasks_common_creature_get(creature_index);
        c_animation_manager *animation_manager =
            (c_animation_manager *)object_header_block_get(object_get(creature_index), &creature->object.animation);
        const c_channel_base *state_channel = animation_manager->get_state_channel();
        if (state_channel->valid())
        {
            switch (animation_manager->get_state_name())
            {
            case _string_id_landing_dead:
                creature->creature.m_flags.set(_creature_flag_3, true);
                animation_manager->pause_playback(true);
                break;
            }
            if (creature->creature.m_flags.test(_creature_flag_1))
            {
                finished = true;
                if (!creature->creature.m_flags.test(_creature_flag_2))
                {
                    creature->creature.m_flags.set(_creature_flag_1, false);
                    if (creature->creature.link_type != -1)
                    {
                        motor_tasks_common_ai_creature_handle_action_finished(
                            creature_index,
                            animation_manager->get_mode_name(),
                            animation_manager->get_state_name());
                    }
                }
                else
                {
                    finished = false;
                }
            }
        }
    }
    return finished;
}

// @address 0x396718 ?weapon_draw_event@@YA_NJPAVc_motor_task@@W4e_motor_event@@@Z
bool weapon_draw_event(long object_index, class c_motor_task *motor_task, enum e_motor_event motor_event)
{
    unit_datum *unit = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    signed char index = unit->unit.current_weapon_indices[0];
    if (index != -1 && unit->unit.weapon_object_indices[index] != -1)
    {
        return true;
    }
    unit_action_weapon_ready(object_index, 226, true, false, true);
    return true;
}


static __forceinline bool motor_tasks_common_ai_posture_allow_hard_ping(long actor_index)
{
    long posture = actor_get_posture(actor_index);
    bool result = true;
    if (posture > 0)
        result = k_behavior_posture_info[posture].disallow == 0;
    return result;
}

// @address 0x395c60 ?unit_exit_finished@@YA_NJPAVc_motor_task@@@Z
bool unit_exit_finished(long object_index, c_motor_task *motor_task)
{
    long result = -1;
    unit_datum *object = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object_get(object_index), &object->unit.motor_program);
    long parent_object = object->object.parent_object_index;
    unsigned long *seat_flags = (unsigned long *)object_header_block_get(object_get(object_index), &object->unit.action);
    unsigned long immediate = *seat_flags & 1;
    unsigned long detach = (*seat_flags >> 1) & 1;
    motor_animation_exit_seat_reset_velocity(object_index, immediate, 0, true, detach, &result);
    if (detach != 0)
        unit_perform_detach_and_damage(object_index, parent_object);
    if (result != -1)
        state->set_program(object_index, (e_motor_program)result);
    else if (object_header_get(object_index)->object_type == _object_type_biped)
    {
        if ((immediate & 0xff) != 0)
            state->set_program(object_index, _motor_program_ground);
        else
            state->set_program(object_index, _motor_program_airborne);
    }
    return true;
}

// @address 0x396f60 ?posture_ping_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result posture_ping_task_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_failed;
    if (motor_task_allow(object_index, request->action, 15))
    {
        biped_datum *object = OBJECT_GET(object_index, biped_datum, _object_mask_biped);
        long actor = object->unit.actor_index;
        bool hard_ping_allowed = actor != -1 && motor_tasks_common_ai_posture_allow_hard_ping(actor);
        short ping_type = request->m_ping.type;
        if (ping_type >= 2)
        {
            if (ping_type >= 3 || actor == -1 || hard_ping_allowed)
            {
                c_motor_program_state *state = (c_motor_program_state *)((char *)object + object->unit.motor_program.offset);
                motor_task_interrupt(object_index, 15, 0);
                motor_tasks_common_posture_exit_end(object_index);
                state->set_program(object_index, _motor_program_ground);
                result = motor_system_submit(object_index, request);
            }
            else
            {
                ping_type = 1;
            }
        }
        if (ping_type < 2)
        {
            c_motor_request ping_request;
            memcpy(&ping_request, request, sizeof(ping_request));
            ping_request.m_ping.type = (signed char)ping_type;
            result = g_ping_task.submit(object_index, &ping_request);
        }
        if (ping_type >= 3)
        {
            if (!hard_ping_allowed)
                object->biped.m_flags.set(_biped_dead_relax_without_contact, true);
        }
    }
    return result;
}
