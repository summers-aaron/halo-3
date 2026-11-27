// objectslibxbox/biped_airborne_motor_program.obj: from the records 0x385640 0x385780 0x385bc0 0x386188 0x3864b8 0x385fa0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "objects/object_header.h"
#include "units/motor_request.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include "ai/actors.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "items/projectiles.h"
#include "motor/motor_system_biped.h"
#include "units/bipeds.h"
#include "motor/biped_airborne_melee.h"
#include "units/unit_dialogue.h"
#include "motor/biped_leap_motor_program.h"
#include "physics/character_physics_component.h"
#include "motor/biped_airborne_motor_program.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "units/motor_task.h"
#include "units/motor_program_state.h"
#include "units/biped_types.h"
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_control_data.h"
#include "animations/animation_manager.h"
#include "animations/animation_state_change_settings.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "physics/character_physics_move_output.h"
#include <stddef.h>
#include <new>
#include "animations/animation_id.h"
#include "physics/physics_constants_globals.h"
#include "physics/collision_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_test_vector.h"
#include "units/biped_definitions.h"
// @address 0x385780 biped_airborne_reset_falling_cooldown

// The airborne program's state in the biped's motor program block (unit.motor_program).
struct biped_airborne_motor_state : c_motor_program_state
{
    short airborne_flags;
    unsigned char falling_danger_ticks;
    signed char airborne_ticks;
    float blend;                                // the falling animation's playback ratio
    float target_blend;
    float blend_time;
    long descent_test_count;
};
typedef char biped_airborne_motor_state_blend_offset[(sizeof(void *) != 4 || (offsetof(biped_airborne_motor_state, blend) == 0xc)) ? 1 : -1];
typedef char biped_airborne_motor_state_size[(sizeof(void *) != 4 || (sizeof(biped_airborne_motor_state) == 0x1c)) ? 1 : -1];

// Private: biped_falling_danger (0x386188) needs this 0x68 frame storage; collision_result is 0x60.
struct biped_airborne_collision_storage
{
    collision_result result;
    unsigned long query_scratch[2];
};
typedef char biped_airborne_collision_storage_size[(sizeof(void *) != 4 || (sizeof(biped_airborne_collision_storage) == 0x68)) ? 1 : -1];

// Unit action block: actions.cpp s_actions_melee_next_state supplies the
// melee animation fields at +0x3c..+0x43.
struct s_biped_airborne_vitality_data
{
    long current_action_index;
    long desired_action_index;
    long action_queue[13];
    long animation;
    signed char weapon_slot;
    unsigned char intensity;
    signed char ticks;
    signed char primary_key_ticks;
};
typedef char biped_airborne_vitality_animation_offset[(sizeof(void *) != 4 || (offsetof(s_biped_airborne_vitality_data, animation) == 0x3c)) ? 1 : -1];

static __forceinline biped_datum *biped_airborne_object_get(long object_index)
{
    return (biped_datum *)object_header_get(object_index)->datum;
}
static __forceinline void *biped_airborne_block_get(long object_index, const s_object_block_reference *reference)
{
    return (char *)biped_airborne_object_get(object_index) + reference->offset;
}
static __forceinline biped_airborne_motor_state *biped_airborne_motor_state_get(biped_datum *biped)
{
    return (biped_airborne_motor_state *)((char *)biped + biped->unit.motor_offset);
}
static __forceinline bool biped_airborne_task_valid(const c_motor_task *task)
{
    return task->task != -1 && task->program != -1 && task->enabled;
}

// @address 0x385640 ?biped_airborne_activate@@YA_NJ@Z
bool biped_airborne_activate(long object_index)
{
    biped_datum *biped = biped_airborne_object_get(object_index);
    biped_airborne_motor_state *state = (biped_airborne_motor_state *)biped_airborne_block_get(object_index, &biped->unit.motor_program);
    state->airborne_flags = 0;
    state->state = 0;
    if (biped->biped.physics.m_mode != c_character_physics_component::_mode_ground)
        biped_airborne_object_get(object_index)->biped.physics.set_mode(c_character_physics_component::_mode_ground);

    real_vector3d velocity;
    object_get_velocities(object_index, &velocity, 0);
    float blend;
    if (!(velocity.k <= 0.0001f))
    {
        blend = 0.0f;
        state->blend = blend;
    }
    else
    {
        state->blend = 0.25f;
        blend = 0.5f;
    }
    state->blend_time = blend;
    state->descent_test_count = 0;
    state->target_blend = blend;

    biped = biped_airborne_object_get(object_index);
    c_motor_task *task = (c_motor_task *)biped_airborne_block_get(object_index, &biped->unit.motor_tasks);
    if (biped_airborne_task_valid(task) && (task->action == 6 || task->action == 42))
        motor_task_interrupt(object_index, 1, 0);
    return true;
}

// @address 0x385780 biped_airborne_reset_falling_cooldown
void biped_airborne_reset_falling_cooldown(long object_index)
{
    biped_datum *unit = (biped_datum *)object_header_get(object_index)->datum;
    unit->biped.falling_danger_cooldown = 0;
}

static __forceinline bool biped_airborne_game_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}

static __forceinline bool biped_airborne_physics_move(long biped_index, unsigned long *flags)
{
    s_character_physics_move_output_datum move_output;
    bool moved = motor_system_biped_move(biped_index, true, false, false, false, &move_output);
    *flags = move_output.flags;
    return moved;
}

// @address 0x3864b8 ?biped_airborne_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_airborne_move(long biped_index, c_motor_request *motor_request)
{
    e_motor_program result = _motor_program_none;
    biped_datum *biped = biped_airborne_object_get(biped_index);
    unsigned long move_flags;
    if (biped_airborne_physics_move(biped_index, &move_flags) && biped->biped.physics.get_mode() == c_character_physics_component::_mode_ground &&
        (move_flags & (1 << 1)) != 0)
    {
        float landing_velocity = biped->biped.physics.get_mode_ground()->get_landing_velocity();
        real_vector3d velocity;
        real_vector3d camera_velocity;
        biped_get_camera_velocities(biped_index, &velocity, &camera_velocity);
        motor_request->setup_land(_action_none, 0, 0, landing_velocity);
        result = _motor_program_ground;
    }
    else if (!biped_airborne_game_is_predicted())
        biped_falling_damage(biped_index);
    return result;
}

static __forceinline long biped_airborne_game_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)tls_game_time_globals()->ticks_per_second * seconds;
    return (long)(ticks + 0.5f);
}
static __forceinline float biped_airborne_game_seconds_to_ticks_real(float seconds)
{
    return seconds * (float)tls_game_time_globals()->ticks_per_second;
}

static __forceinline bool biped_airborne_flag_test(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}
static __forceinline real_point3d *biped_airborne_point_subtract(const real_point3d *point, const real_vector3d *vector, real_point3d *result)
{
    result->x = point->x - vector->i;
    result->y = point->y - vector->j;
    result->z = point->z - vector->k;
    return result;
}

static __forceinline bool biped_airborne_trace_ground(
    const real_point3d *origin, const real_vector3d *vector, collision_result *collision, real_point3d *ground)
{
    bool hit = collision_test_vector(k_biped_ground_collision_flags, false, origin, vector, -1, -1, -1, collision);
    if (hit) *ground = collision->point;
    return hit;
}

// @address 0x385fa0 ?biped_airborne_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_airborne_update(long object_index, s_animation_control_data *control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    bool result = false;
    biped_datum *datum = (biped_datum *)object_header_get(object_index)->datum;
    biped_airborne_motor_state *motor = biped_airborne_motor_state_get(datum);
    if (datum->object.havok_component_index != -1)
    {
        long parent_index = datum->object.parent_object_index;
        while (parent_index != -1)
        {
            biped_datum *parent = (biped_datum *)object_header_get(parent_index)->datum;
            if (!(bool)((parent->object.flags >> 26) & 1))
                break;
            parent_index = parent->object.parent_object_index;
        }
        if (parent_index == -1)
        {
            s_character_physics_update_input_datum input;
            s_character_physics_update_output_datum output;
            biped_falling_danger(object_index);
            if (motor->airborne_ticks < 127)
                motor->airborne_ticks++;
            leap_orientation_update(object_index, &control);
            biped_update_melee_aiming(object_index, (long)(ptrdiff_t)&control);
            if (::motor_system_biped_physics_preupdate_moving(object_index, &input,
                    &output, false, true, 0.0f, false, false))
            {
                biped_airborne_update_moving(object_index, &control, &input, &output);
                motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouch);
            }
            result = true;
        }
    }
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        ::motor_animation_build_state_change_settings_from_control_data(object_index, &control,
            &requested, &settings);
        if (requested.goals.test(_comparison_goal_field_3_bit) && (control.state == 31 || control.state == 988))
            settings.interpolation_time = 0.13333334f;
        if (control.state == 988)
            settings.primary.m_playback_ratio = motor->blend;
        result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    return result;
}

// @address 0x386188 ?biped_falling_danger@@YAXJ@Z
void biped_falling_danger(long object_index)
{
    real_vector3d vector;
    bool falling = false;
    biped_datum *biped = (biped_datum *)object_header_get(object_index)->datum;
    biped_airborne_motor_state *mover = biped_airborne_motor_state_get(biped);
    if (biped->biped.falling_danger_cooldown != 0)
        biped->biped.falling_danger_cooldown--;
    if (!biped_airborne_flag_test(biped->biped.flags, 2) &&
        biped->biped.physics.m_mode == 1 &&
        biped->biped.falling_danger_cooldown == 0 &&
        !biped_airborne_flag_test(biped->unit.unit_flags, 9) &&
        !biped_airborne_flag_test(TAG_GET('bipd', const biped_definition, biped->object.definition_index)->flags, 2) &&
        biped->unit.actor_index != -1 &&
        ((c_animation_manager *)((char *)biped + biped->object.animation.offset))->get_state_name() != 183 &&
        mover->airborne_ticks > biped_airborne_game_seconds_to_ticks_round(0.5f))
    {
        falling = true;
        biped_airborne_collision_storage collision_storage;
        collision_result &collision = collision_storage.result;
        real_point3d origin;
        object_get_origin(object_index, &origin);
        real_vector3d offset;
        offset.i = 0.0f;
        offset.j = 0.0f;
        offset.k = -0.4f;
        biped_airborne_point_subtract(&origin, &offset, &origin);
        vector.i = 0.0f;
        vector.j = 0.0f;
        vector.k = -6.0f;
        real_point3d ground_point;
        if (biped_airborne_trace_ground(&origin, &vector, &collision, &ground_point))
        {
            real_point3d current_origin;
            object_get_origin(object_index, &current_origin);
            real_vector3d velocity;
            object_get_velocities(object_index, &velocity, 0);
            if (velocity.k > 0.0f)
            {
                falling = false;
            }
            else
            {
                float maximum_velocity = actor_get_maximum_damage_velocity(biped->unit.actor_index) * 1.5f;
                if (global_gravity_get() * (current_origin.z - ground_point.z) * 2.0f + velocity.k * velocity.k < maximum_velocity * maximum_velocity)
                    falling = false;
            }
        }
    }

    long ticks;
    if (falling)
    {
        ticks = mover->falling_danger_ticks + 1;
        if (ticks >= 254)
            ticks = 254;
    }
    else
    {
        ticks = 0;
    }
    mover->falling_danger_ticks = (unsigned char)ticks;
    if (mover->falling_danger_ticks >= biped_airborne_game_seconds_to_ticks_round(1.0f))
    {
        if (biped->biped.last_airborne_speech_time == -1 ||
            (float)biped->biped.last_airborne_speech_time + biped_airborne_game_seconds_to_ticks_real(5.0f) < (float)tls_game_time_globals()->time)
        {
            biped->biped.last_airborne_speech_time = tls_game_time_globals()->time;
            ::unit_scream(object_index, 16, -1, 13, 0, 0);
        }
    }
}

// March debug: g_airborne_arc_enabled (biped_airborne_motor_program.obj), passed on to the arc collision estimate.
bool g_airborne_arc_enabled;

static const float k_airborne_descent_test_duration = 1.0f;
static const long k_airborne_descent_test_count = 4;
// @address 0x385bc0 ?biped_airborne_update_moving@@YAXJPAUs_animation_control_data@@PAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@@Z
void biped_airborne_update_moving(long object_index, s_animation_control_data *animation_control_data,
    s_character_physics_update_input_datum *input, s_character_physics_update_output_datum *output)
{
    biped_datum *unit = (biped_datum *)object_header_get(object_index)->datum;
    bool airborne = biped_airborne_flag_test(unit->unit.unit_flags, 22);
    c_animation_manager *manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    if (airborne)
        animation_control_data->mode = 141;
    else
        animation_control_data->mode = unit->unit.animation_state;
    if (animation_control_data->mode != 138)
    {
        c_animation_manager *weapon_data = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        long weapon_class = weapon_data->get_weapon_class_name();
        long weapon_type = weapon_data->get_weapon_type_name();
        bool tested = false;
        if (airborne)
        {
            if (motor_animation_test_mode(object_index, animation_control_data->mode, weapon_class, weapon_type))
                tested = true;
            else
                animation_control_data->mode = unit->unit.animation_state;
        }
        if (!tested && animation_control_data->mode != 138 && !motor_animation_test_mode(object_index, animation_control_data->mode, weapon_class, weapon_type))
            animation_control_data->mode = 138;
    }
    if (manager->get_mode_name() == 188)
    {
        animation_control_data->mode = 138;
        animation_control_data->state = 31;
        animation_control_data->moved = true;
    }
    if (manager->get_state_name() == 76 || manager->get_state_name() == 77)
        animation_control_data->state = 77;
    else
        animation_control_data->state = 31;
    if ((output->flags & 4) != 0 && unit->biped.physics.m_mode == 6)
    {
        bool enter = true;
        s_biped_airborne_vitality_data *vitality = (s_biped_airborne_vitality_data *)((char *)unit + unit->unit.action_offset);
        if (vitality->animation != -1 && vitality->ticks <= vitality->primary_key_ticks)
            enter = false;
        else
        {
            bool running = false;
            biped_datum *current = (biped_datum *)object_header_get(object_index)->datum;
            if (current->biped.physics.m_mode == 6 &&
                current->biped.physics.get_mode_melee()->is_running_invisible_crouched_uber_melee())
                running = true;
            if (running)
                biped_exit_invisible_crouched_uber_melee(object_index);
        }
        if (enter)
            biped_enter_default_physics(object_index);
    }
    if (animation_control_data->state != 31)
        return;
    c_animation_id animation_id = manager->find_state_animation_id(226, 226, 988);
    if (!animation_id.valid())
        return;
    biped_airborne_motor_state *motor = biped_airborne_motor_state_get(unit);
    real_vector3d velocity;
    object_get_velocities(object_index, &velocity, 0);
    float gravity = global_gravity_get();
    if (fabsf(gravity) >= k_real_epsilon)
    {
        if (velocity.k > k_real_epsilon)
        {
            float blend_time = velocity.k / gravity;
            motor->target_blend = 0.5f;
            if (blend_time < 0.0f)
                blend_time = 0.0f;
            motor->blend_time = blend_time;
        }
        else if (velocity.k < k_real_epsilon)
        {
            if ((motor->descent_test_count & 3) == 0)
            {
                motor->target_blend = 1.0f;
                float distance = 0.0f;
                if (projectile_estimate_collision_along_arc(object_index, k_airborne_descent_test_duration, k_airborne_descent_test_count,
                        k_collision_test_camera, g_airborne_arc_enabled, 0, &distance))
                    motor->blend_time = distance;
                else
                    motor->blend_time = 100.0f;
            }
            motor->descent_test_count++;
        }
        else
        {
            motor->blend_time = 0.0f;
            motor->descent_test_count = 0;
            motor->target_blend = 0.5f;
        }
    }
    else
    {
        motor->blend_time = 1.0f;
        motor->descent_test_count = 0;
        motor->target_blend = 0.5f;
    }
    float duration = motor->blend_time;
    float current = motor->blend;
    float delta = motor->target_blend - current;
    float t = 1.0f;
    if (fabsf(duration) >= k_real_epsilon)
    {
        float tick = tls_game_time_globals()->tick_length;
        t = tick / duration;
        t = PIN(t, 0.0f, 1.0f);
        motor->blend_time = MAX(duration - tick, 0.0f);
    }
    motor->blend = t * delta + current;
    animation_control_data->blend_ratios[0] = 0.0f;
    animation_control_data->blend_ratios[1] = 0.0f;
    animation_control_data->secondary_states[0] = -1;
    animation_control_data->state = 988;
    animation_control_data->secondary_states[1] = -1;
    animation_control_data->playback_flags = 64;
}
