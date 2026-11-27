// objectslibxbox/biped_climb_motor_program.obj: from the records 0x38c100 0x38c148 0x38c1f8 0x38c5d0 0x38c6f8 0x38c998 0x38ca78
// @flags /TP /O2 /Oc
#include "physics/character_physics_input.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include <new>
#include <stdlib.h>
#include <stddef.h>
#include "game/game_time_types.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "motor/motor_system_biped.h"
#include "math/matrix_math.h"
#include "motor/motor_programs.h"
#include "units/biped_types.h"
#include "motor/motor_event_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/real_math_globals.h"
#include "objects/object_datum.h"
#include "objects/object_types.h"
#include "physics/character_physics_output.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "cseries/cseries_wrapped.h"
#include "animations/animation_control_data.h"
#include "animations/animation_manager.h"
#include "animations/animation_state_change_settings.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "physics/character_physics_move_output.h"
#include "motor/motor_tasks_common.h"
#include <math.h>
#include "physics/character_physics_component.h"
#include "units/motor_request.h"
#include "units/motor_task.h"
#include "math/matrix_math_types.h"
#include "units/unit_definitions.h"
#include "units/biped_definitions.h"
#include "tag_files/string_ids.h"
// @address 0x38c100 ?biped_climb_default_event@@YA_NJPAVc_motor_task@@W4e_motor_event@@@Z
// @address 0x38c5d0 ?biped_climb_get_state_animation_from_physics@@YAXJW4e_animation_state@s_character_physics_update_output_datum@@_NPAJPA_N@Z

// The climb program's state in the biped's motor program block (unit.motor_program). biped_climb_update hands
// facing_direction, movement_vector and snap_facing to biped_update_desired_facing.
struct biped_climb_motor_state : c_motor_program_state
{
    short climb_flags;
    short facing_direction;
    signed char animation_type;                 // 0 standing or turning, 1 moving, 2 anything else
    signed char ticks;
    real_vector3d movement_vector;
    bool snap_facing;
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char biped_climb_motor_state_vector_offset[((unsigned long)&((biped_climb_motor_state *)0)->movement_vector == 0x10) ? 1 : -1];
typedef char biped_climb_motor_state_snap_offset[((unsigned long)&((biped_climb_motor_state *)0)->snap_facing == 0x1c) ? 1 : -1];
#endif

static __forceinline biped_climb_motor_state *biped_climb_motor_state_get(biped_datum *biped)
{
    return (biped_climb_motor_state *)((char *)biped + biped->unit.motor_offset);
}

// @address 0x38c100 ?biped_climb_default_event@@YA_NJPAVc_motor_task@@W4e_motor_event@@@Z
bool biped_climb_default_event(long object_index, c_motor_task *motor_task, e_motor_event motor_event)
{
    bool result = false;
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, NONE);
    biped_climb_motor_state *motor_state = biped_climb_motor_state_get(biped);
    if (motor_event == 1 && motor_state->snap_facing != 0) result = true;
    return result;
}

// @address 0x38c5d0 ?biped_climb_get_state_animation_from_physics@@YAXJW4e_animation_state@s_character_physics_update_output_datum@@_NPAJPA_N@Z
void biped_climb_get_state_animation_from_physics(long object_index,
    s_character_physics_update_output_datum::e_animation_state animation_state, bool moving, long *definition_index, bool *moved)
{
    if (*definition_index == _string_id_idle)
    {
        switch (animation_state)
        {
        case 1: *definition_index = _string_id_idle; break;
        case 2: *definition_index = _string_id_move_right; break;
        case 3: *definition_index = _string_id_move_left; break;
        case 4: *definition_index = _string_id_move_front; break;
        case 5: *definition_index = _string_id_move_back; break;
        case 6: *definition_index = _string_id_stunned_right; break;
        case 7: *definition_index = _string_id_stunned_left; break;
        case 8: *definition_index = _string_id_stunned_front; break;
        case 9: *definition_index = _string_id_stunned_back; break;
        case s_character_physics_update_output_datum::_animation_state_climb_up: *definition_index = _string_id_climb_up; break;
        case s_character_physics_update_output_datum::_animation_state_climb_down: *definition_index = _string_id_climb_down; break;
        }
        if (animation_state != s_character_physics_update_output_datum::_animation_state_none) *moved = moving;
        if (*definition_index == _string_id_idle && !*moved)
        {
            c_animation_channel *channel = motor_animation_resource_get_channel(object_index, _motor_resource_0);
            if (!channel->reached_last_frame() && channel->m_definition_index >= _string_id_turn_left
                && channel->m_definition_index <= _string_id_turn_right) *definition_index = channel->m_definition_index;
        }
    }
}

// @address 0x38c148 ?biped_climb_activate@@YA_NJ@Z
bool biped_climb_activate(long unit_index)
{
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(unit_index, _object_mask_biped);
    biped_climb_motor_state *climb_state = (biped_climb_motor_state *)object_header_block_get(object_get(unit_index), &biped->unit.motor_program);

    climb_state->climb_flags = 0;
    climb_state->state = 0;
    climb_state->animation_type = 0;
    climb_state->ticks = 0;
    climb_state->facing_direction = 0;
    climb_state->movement_vector = *global_zero_vector3d;

    if (biped->biped.physics.get_mode() != c_character_physics_component::_mode_ground)
        ((biped_datum *)object_get_and_verify_type(unit_index, _object_mask_biped))
            ->biped.physics.set_mode(c_character_physics_component::_mode_ground);
    return true;
}

static __forceinline real_vector3d *biped_climb_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = a->j * b->k - a->k * b->j;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline float biped_climb_game_tick_length()
{
    return tls_game_time_globals()->tick_length;
}

static __forceinline float biped_climb_sine(float angle) { return (float)sin(angle); }
static __forceinline float biped_climb_cosine(float angle) { return (float)cos(angle); }

static __forceinline biped_datum *biped_climb_biped_get(long object_index)
{
    return (biped_datum *)object_get_and_verify_type(object_index, NONE);
}

static __forceinline float biped_climb_ground_plane_k(biped_datum *biped)
{
    return biped->biped.physics.get_mode_ground()->get_ground_plane()->k;
}

// @address 0x38c1f8 ?biped_climb_update_turning@@YA_NJPAUs_animation_control_data@@@Z
bool biped_climb_update_turning(long object_index, s_animation_control_data *animation_control_data)
{
    int _salloc_pad0 = 0; int _salloc_pad1 = 0; int _salloc_pad2 = 0;
    bool result = false;
    biped_datum *unit = biped_climb_biped_get(object_index);
    biped_climb_motor_state *motor_state = biped_climb_motor_state_get(unit);
    c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    biped_definition *definition = TAG_GET('bipd', biped_definition, unit->object.definition_index);
    unsigned char action = unit->biped.physics.m_mode;
    if (action != 3)
    {
        result = true;
        if (action != 6 && action != 5)
        {
            real_vector3d axis_tmp;
            biped_climb_cross_product3d(&unit->object.up, &unit->unit.facing_vector, &axis_tmp);
            real_vector3d axis;
            biped_climb_cross_product3d(&axis_tmp, &unit->object.up, &axis);
            if (normalize3d(&axis) == 0.0f)
                axis = unit->object.forward;
            float facing_dot = dot_product3d(&axis, &unit->object.forward);
            real_vector3d facing_cross;
            biped_climb_cross_product3d(&axis, &unit->object.forward, &facing_cross);
            bool clockwise = dot_product3d(&unit->object.up, &facing_cross) > 0.0f;
            if (facing_dot < -0.9f)
            {
                if (animation->get_state_name() == _string_id_turn_right)
                    clockwise = true;
                else if (animation->get_state_name() == _string_id_turn_left)
                    clockwise = false;
            }
            if ((motor_state->animation_type) == 1 || (definition->flags & 1) != 0)
            {
                real_vector3d rotated = unit->object.forward;
                float angle = biped_climb_game_tick_length() * definition->moving_turning_speed;
                float cosine = biped_climb_cosine(angle);
                float sine = biped_climb_sine(angle);
                if (clockwise)
                    sine = -sine;
                rotate_vector_about_axis(&rotated, &unit->object.up, sine, cosine);
                real_vector3d rotated_cross;
                biped_climb_cross_product3d(&axis, &rotated, &rotated_cross);
                float rotated_sign = dot_product3d(&unit->object.up, &rotated_cross);
                // the turn swung past the facing axis: put forward back on it
                if ((clockwise && rotated_sign < 0.0f) || !(clockwise || rotated_sign <= 0.0f))
                {
                    real_vector3d side;
                    biped_climb_cross_product3d(&unit->object.up, &axis, &side);
                    if (normalize3d(&side) > 0.0f)
                        biped_climb_cross_product3d(&side, &unit->object.up, &rotated);
                    normalize3d(&rotated);
                }
                unit->object.forward = rotated;
            }
            else if ((motor_state->animation_type) == 0 && ((unsigned char)((unit->unit.unit_flags >> 11) & 1)) == 0)
            {
                float limit = 0.99f;
                if ((unit->unit.control_flags & 8) == 0)
                    limit = definition->turning_velocity_maximum + 0.0001f;
                if (facing_dot <= limit && ((unsigned char)((((const unit_definition *)definition)->flags >> 20) & 1)) == 0)
                    animation_control_data->state = clockwise ? _string_id_turn_right : _string_id_turn_left;
            }
        }
    }
    return result;
}


static __forceinline long biped_climb_game_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}

// @address 0x38c998 ?biped_climb_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_climb_move(long object_index, c_motor_request *motor_request)
{
    biped_datum *unit = biped_climb_biped_get(object_index);
    e_motor_program result = _motor_program_none;
    s_character_physics_move_output_datum output;
    motor_system_biped_move(object_index, false, false, false, true, &output);
    if (unit->biped.physics.m_mode == 1)
    {
        if (biped_climb_ground_plane_k(unit) > 0.71f)
        {
            result = _motor_program_ground;
        }
        else
        {
            biped_climb_motor_state *motor_state = biped_climb_motor_state_get(unit);
            if (motor_state->ticks > biped_climb_game_seconds_to_ticks_round(0.25f))
                result = _motor_program_airborne;
        }
    }
    return result;
}

// @address 0x38c6f8 ?biped_climb_update@@YA_NJ@Z
bool biped_climb_update(long object_index)
{
    s_animation_control_data control;
    control.set_defaults();
    const unsigned long &absolute_index = object_index & 0xffff;
    biped_datum *unit = (biped_datum *)((s_object_header *)tls_object_header_data()->data)[absolute_index].datum;
    c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    biped_climb_motor_state *state = biped_climb_motor_state_get(unit);
    bool result = false;
    // match: 0x38c6f8: zero-bit rotate preserves 32-bit GPR value
    const long &no_task = _rotl(0, 0);
    if (unit->object.havok_component_index != -1)
    {
        long ancestor_index = unit->object.parent_object_index;
        while (ancestor_index != -1)
        {
            object_datum *ancestor = &biped_climb_biped_get(ancestor_index)->object;
            if ((_rotl(ancestor->flags, 6) & 1) == 0)
                break;
            ancestor_index = ancestor->parent_object_index;
        }
        if (ancestor_index == -1)
        {
            unsigned char animation_type = 2;
            switch (animation->get_state_name())
            {
            case _string_id_idle: case _string_id_turn_left: case _string_id_turn_right:
                animation_type = 0;
                break;
            case _string_id_move_front: case _string_id_move_back: case _string_id_move_left: case _string_id_move_right:
            case _string_id_airborne: case _string_id_airborne_dead: case _string_id_airborne_arc:
                animation_type = 1;
                break;
            }
            state->animation_type = animation_type;
            if (unit->biped.physics.m_flags & FLAG(1))
            {
                if (state->ticks < 127)
                    state->ticks++;
            }
            else
                state->ticks = 0;
            s_character_physics_update_input_datum input;
            s_character_physics_update_output_datum output;
            biped_update_desired_facing(object_index, true, &state->facing_direction, &state->movement_vector, &state->snap_facing);
            result = biped_climb_update_turning(object_index, &control);
            if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, false, false, 0.0f, false, true))
            {
                biped_climb_get_state_animation_from_physics(
                    object_index, output.animation_state, output.animation_state_valid, &control.state, &control.moved);
                unit = (biped_datum *)((s_object_header *)tls_object_header_data()->data)[absolute_index].datum;
                control.mode = (_rotl(unit->unit.unit_flags, 10) & 1) ? _string_id_crouch : _string_id_climb;
                motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouch);
                result = true;
            }
        }
    }
    {
    s_animation_state_change_settings settings;
    s_state_changes_requested requested;
    motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
    c_motor_task *task = motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved);
    const bool completed = ((long)(ptrdiff_t)task - no_task != 0) | result;
    motor_animation_update_analog_blending(object_index);
    return completed;
    }
}


static __forceinline real_vector3d *biped_climb_finish_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    result->i = i;
    result->j = j;
    return result;
}

// @address 0x38ca78 biped_climb_submit
extern "C" e_motor_result biped_climb_submit(long object_index, const c_motor_request *request)
{
    real_vector3d original_forward;
    real_point3d position;
    real_vector3d up;
    real_matrix4x3 original_matrix;
    bool saved_orientation = false;
    biped_datum *unit = biped_climb_biped_get(object_index);
    if (request->m_ping.type >= 3)
    {
        unit->biped.m_flags.set(_biped_dead_relax_without_contact, true);
        unit->biped.m_flags.set(_biped_dead_reset_airborne_state, true);
        object_datum *object = &biped_climb_biped_get(object_index)->object;
        const real_matrix4x3 *const root_matrix = (const real_matrix4x3 *)((char *)object + object->node_matrices.offset);
        original_matrix = *root_matrix;
        object_get_orientation(object_index, &original_forward, &up);
        saved_orientation = true;
    }
    e_motor_result result = g_ping_task.submit(object_index, request);
    if (result == _motor_result_success && request->m_ping.type >= 3 && saved_orientation)
    {
        object_datum *object = &biped_climb_biped_get(object_index)->object;
        const biped_definition *definition = TAG_GET('bipd', const biped_definition, object->definition_index);
        float height = definition->physics.radius;
        object_get_origin(object_index, &position);
        point_from_line3d(&position, &up, height, &position);
        point_from_line3d(&position, global_down3d, height, &position);
        up = *global_up3d;
        real_vector3d forward;
        biped_climb_finish_cross_product3d(&up, &original_forward, &forward);
        if (!(normalize3d(&forward) > 0.0f))
            forward = global_identity4x3->forward;
        biped_climb_finish_cross_product3d(&forward, &up, &forward);
        if (!(normalize3d(&forward) <= 0.0f))
        {
            object_set_position_direct(object_index, &position, &forward, &up, 0, false);
            real_matrix4x3 matrix;
            matrix4x3_rotation_from_vectors(&matrix, &forward, &up);
            matrix.position = position;
            real_orientation *orientation = (real_orientation *)((char *)object + object->original_node_orientations.offset);
            real_matrix4x3 inverse_matrix;
            matrix4x3_inverse(&matrix, &inverse_matrix);
            matrix4x3_compute_product(&inverse_matrix, &original_matrix, &matrix);
            real_quaternion rotation;
            matrix3x3_rotation_to_quaternion(&matrix.rotation, &rotation);
            orientation->translation = matrix.position;
            orientation->rotation = rotation;
            object_compute_node_matrices(object_index);
        }
    }
    return result;
}
