// objectslibxbox/biped_perch_motor_program.obj: from the records 0x38ac40 0x38ae48 0x38afa8 0x38b118 0x38b1a0 0x38b248 0x38b398 0x38b498 0x38b4e8 0x38b5d8 0x38b800 0x38b928 0x38ba88 0x38bb18 0x38bc88 0x38bdf8 0x38be48 0x38c038
// @flags /TP /O2 /Oc
#include "physics/character_physics_input.h"
#include "objects/object_header.h"
#include "math/matrix_math_types.h"
#include "units/motor_enums.h"
#include "units/motor_task.h"
#include "physics/character_physics_output.h"
#include "interface/gui_widget_types.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "motor/motor_system_biped.h"
#include "units/bipeds.h"
#include "units/unit_animation.h"
#include "objects/object_datum.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "cache/cache_files.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include <math.h>
#include <new>
#include "physics/character_physics_move_output.h"
#include "units/motor_request.h"
#include "units/motor_program_state.h"
#include "math/matrix_math.h"
#include "math/real_math_globals.h"
#include "animations/animation_control_data.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/animation_manager.h"
#include "animations/animation_state_change_settings.h"
#include "units/biped_types.h"
#include "tag_files/tag_types.h"
// @address 0xdb610 ?matrix3x3_rotation_to_quaternion@@YAPAUreal_quaternion@@PBUreal_matrix3x3@@PAU1@@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z
// @address 0x356178 ?motor_animation_default_ground_mode@@YAJJ@Z
// @address 0x355a60 ?motor_task_interrupt@@YAXJFF@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z
// @address 0xc1638 ?program_default@c_motor_task@@QAA_NXZ
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0x38afa8 ?biped_perch_attach_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38c038 ?biped_perch_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
// @address 0x38b118 ?biped_perch_detach_finished@@YA_NJPAVc_motor_task@@@Z
// @address 0x38b1a0 ?biped_perch_detach_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38b248 ?biped_perch_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38b398 ?biped_perch_takeoff_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38b498 ?biped_perch_activate@@YA_NJ@Z
// @address 0x38b4e8 ?biped_update_perch_animation@@YAXJW4e_animation_state@@_NPAJ2PA_N@Z
// @address 0x38bb18 ?biped_bunker_close_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38bc88 ?biped_bunker_open_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x38bdf8 ?biped_bunker_activate@@YA_NJ@Z


struct s_biped_perch_motor_block {
    c_motor_program_state motor;
    union { bool moved; unsigned short movement_flags; };
    signed char perch_type;
    unsigned char exit_requested;
    unsigned char apply_translation;
    real_point3d origin;
};
struct s_biped_bunker_motor_block {
    c_motor_program_state motor;
    bool moved;
    signed char open;
    signed char variant;
    unsigned char exit_requested;
};
typedef char biped_perch_layout_70[(sizeof(s_biped_perch_motor_block) == 0x1c) ? 1 : -1];
typedef char biped_perch_layout_71[(offsetof(s_biped_perch_motor_block, perch_type) == 0x0a) ? 1 : -1];
typedef char biped_perch_layout_72[(offsetof(s_biped_perch_motor_block, origin) == 0x10) ? 1 : -1];
typedef char biped_perch_layout_73[(sizeof(s_biped_bunker_motor_block) == 0x0c) ? 1 : -1];
typedef char biped_perch_layout_74[(offsetof(s_biped_bunker_motor_block, variant) == 0x0a) ? 1 : -1];


struct s_biped_attach_request {
    char type;
    bool forced;
    e_motor_task task;
    long action;
    real_point3d position;
    float x, y;
    bool climb;
    __forceinline bool force() const { return forced; }
};


#include "animations/animation_data.h"

#include "units/biped_definitions.h"
#include "math/integer_math.h"
#include "animations/animation_definitions.h"
#include "tag_files/tag_groups.h"

static __forceinline long biped_perch_motor_program_get(long object_index)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    return state->motor.program;
}

// @address 0x38ae48 ?biped_perch_attach_submit@@YAJJPBUs_biped_attach_request@@@Z
long biped_perch_attach_submit(long object_index, const s_biped_attach_request *request)
{
    long result = 0;
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    if (biped->object.parent_object_index == -1) {
        if (request->climb) {
            real_vector3d forward;
            forward.i = request->x;
            forward.j = request->y;
            forward.k = 0.0f;
            if (normalize3d(&forward) > 0.0f) object_set_position_direct(object_index, 0, &forward, &global_identity4x3->up, 0, false);
            ::biped_enter_climbing_physics(object_index, &request->position);
            result = 1;
        } else {
            real_vector3d forward;
            forward.i = request->x;
            forward.j = request->y;
            forward.k = 0.0f;
            bool finished;
            if (motor_task_set_posture(object_index, _motor_task_attach, (e_action)request->action, 189, 188, true, &request->position,
                    &forward, 15, request->force(), &finished))
                result = 2;
            else if (finished) result = 1;
        }
    }
    if (result != 0) {
        biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
        s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
        state->motor.set_program(object_index, _motor_program_perch);
    }
    return result;
}

bool biped_perch_attach_finished(long object_index, c_motor_task *task)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    real_matrix4x3 original = *(real_matrix4x3 *)((char *)unit + unit->object.node_matrices.offset);
    real_matrix4x3 matrix;
    real_matrix4x3 inverse;
    real_matrix4x3 world_inverse;
    real_quaternion rotation;
    ::motor_animation_posture_exit_end(object_index, 188, 226, 226, 12);
    if (::unit_animation_build_root_matrix_by_seat_and_state(object_index, 188, 12, false, 3, &matrix)) {
        matrix4x3_inverse(&matrix, &inverse);
        matrix4x3_compute_product(&original, &inverse, &matrix);
        ::biped_enter_climbing_physics(object_index, &matrix.position);
        unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
        // match: 0x38afa8: __restrict keeps node stores on base r10 with grouped rotation loads; removal recolors base to r7, interleaves loads/stores.
        real_orientation *__restrict node = (real_orientation *)((char *)unit + unit->object.original_node_orientations.offset);
        ::object_get_world_matrix(object_index, &inverse);
        matrix4x3_inverse(&inverse, &world_inverse);
        matrix4x3_compute_product(&world_inverse, &original, &matrix);
        matrix3x3_rotation_to_quaternion((const real_matrix3x3 *)&matrix.forward, &rotation);
        node->translation = matrix.position;
        node->rotation = rotation;
    }
    unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *block = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    block->perch_type = 4;
    return true;
}

bool biped_perch_detach_finished(long object_index, c_motor_task *task)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    long ground_mode = motor_animation_default_ground_mode(object_index);
    ::motor_animation_posture_exit_end(object_index, ground_mode, 226, 226, 12);
    return state->motor.set_program(object_index, _motor_program_ground);
}

e_motor_result biped_perch_detach_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_none;
    bool finished;
    if (::motor_task_set_posture(object_index, _motor_task_detach, (e_action)request->action, 190, 188, true,
            &request->m_detach.position, &request->m_detach.alignment, 15, request->force(), &finished))
    {
        result = _motor_result_success;
    } else if (finished) {
        biped_perch_detach_finished(object_index, 0);
        result = _motor_result_pending;
    }
    return result;
}

e_motor_result biped_perch_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_none;
    long animation = -1;
    short perch_type = 0;
    switch (request->m_perch.perch_type) {
    case 0: animation = 479; perch_type = 1; break;
    case 1: animation = 480; perch_type = 2; break;
    case 2: animation = 493; perch_type = 0; break;
    case 3: animation = 481; perch_type = 3; break;
    }
    if (animation != -1) {
        long movement_type = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped))->biped.physics.m_mode;
        long mode = movement_type == 5 || movement_type == 4 ? 12 : 84;
        bool immediate;
        if (motor_task_set_posture(object_index, _motor_task_perch, (e_action)request->action, mode, animation, true,
                &request->m_perch.position, &request->m_perch.direction, 15, request->force(), &immediate))
        {
            biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
            s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)biped + biped->unit.motor_offset);
            if (state->motor.set_program(object_index, _motor_program_perch)) {
                s_biped_perch_motor_block *perch_state = (s_biped_perch_motor_block *)((char *)biped + biped->unit.motor_offset);
                perch_state->perch_type = (char)perch_type;
                if (immediate) result = _motor_result_pending;
                else result = _motor_result_success;
            }
        }
    }
    return result;
}

e_motor_result biped_perch_takeoff_submit(long object_index, const c_motor_request *request)
{
    e_motor_result result = _motor_result_none;
    if (request->m_takeoff.takeoff) {
        biped_datum *perch = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
        s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)perch + perch->unit.motor_offset);
        if (state->motor.set_program(object_index, _motor_program_flying)) {
            result = _motor_result_pending;
        }
    } else {
        real_point3d origin;
        real_vector3d forward;
        bool immediate;
        object_get_origin(object_index, &origin);
        ::object_get_orientation(object_index, &forward, 0);
        if (::motor_task_set_posture(object_index, _motor_task_takeoff, (e_action)request->action, 85, 226, true,
                &origin, &forward, 15, request->force(), &immediate))
        {
            result = immediate ? _motor_result_pending : _motor_result_success;
        }
    }
    return result;
}

bool biped_perch_activate(long object_index)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *block = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    block->movement_flags = 0;
    block->motor.state = 0;
    block->perch_type = 1;
    block->exit_requested = 0;
    block->apply_translation = 0;
    return true;
}

void biped_update_perch_animation(long object_index, e_animation_state animation_state, bool value, long *perch_value, long *perch_state, bool *perch_flag)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *block = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    switch (block->perch_type) {
    case 0: *perch_value = 0x1ed; break;
    case 1: *perch_value = 0x1df; break;
    case 2: *perch_value = 0x1e0; break;
    case 3: *perch_value = 0x1e1; break;
    case 4: *perch_value = 188; break;
    }
    if (*perch_state == 12) {
        switch (animation_state) {
        case s_character_physics_update_output_datum::_animation_state_climb_down: *perch_state = 30; break;
        case s_character_physics_update_output_datum::_animation_state_climb_up: *perch_state = 29; break;
        case 1: *perch_state = 12; break;
        }
        if (animation_state != 0) *perch_flag = value;
    }
}

static __forceinline bool biped_perch_transition_underway(const c_animation_channel *channel)
{
    return channel->m_enabled && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}

static e_motor_result biped_bunker_close_submit(long object_index, const c_motor_request *request)
{
    long result = 0;
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_bunker_motor_block *block = (s_biped_bunker_motor_block *)((char *)unit + unit->unit.motor_offset);
    if (block->open != 1) {
        int state;
        int mode;
        if (::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && state == 12) {
            c_animation_manager *manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
            if (!biped_perch_transition_underway(&manager->m_transition_channel)) {
                signed char variant = (signed char)block->variant;
                switch (variant) {
                case 3: mode = 726; break;
                case 2: mode = 261; break;
                case 1: mode = 260; break;
                }
                c_motor_task *task = motor_task_set_mode_and_state(
                    object_index, _motor_task_bunker_close, (e_action)request->action, mode, 12, 0.1f, 15, 0, 31, request->force());
                result = (task != 0) + 1;
                block->open = 1;
            }
        }
    } else {
        result = 1;
    }
    return (e_motor_result)result;
}

static e_motor_result biped_bunker_open_submit(long object_index, const c_motor_request *request)
{
    long result = 0;
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_bunker_motor_block *block = (s_biped_bunker_motor_block *)((char *)unit + unit->unit.motor_offset);
    if (block->open) {
        int state;
        int mode;
        if (::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && state == 12) {
            c_animation_manager *manager = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
            if (!biped_perch_transition_underway(&manager->m_transition_channel)) {
                signed char variant = (signed char)block->variant;
                switch (variant) {
                case 3: mode = 727; break;
                case 2: mode = 273; break;
                case 1: mode = 272; break;
                }
                c_motor_task *task = motor_task_set_mode_and_state(
                    object_index, _motor_task_bunker_open, (e_action)request->action, mode, 12, 0.1f, 15, 0, 31, request->force());
                result = (task != 0) + 1;
                block->open = 0;
            }
        }
    } else {
        result = 1;
    }
    return (e_motor_result)result;
}

bool biped_bunker_activate(long object_index)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_bunker_motor_block *block = (s_biped_bunker_motor_block *)((char *)unit + unit->unit.motor_offset);
    block->motor.state = 0;
    block->moved = 0;
    block->open = 1;
    block->variant = 3;
    return true;
}

e_motor_program biped_perch_move(long object_index, c_motor_request *request)
{
    e_motor_program result = _motor_program_none;
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *state = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    s_character_physics_move_output_datum output;
    motor_system_biped_move(object_index, false, false, false, false, &output);
    if (state->moved) {
        motor_task_interrupt(object_index, 15, 0);
        if (unit->biped.physics.m_mode == 4) ::motor_animation_posture_exit_end(object_index, 138, 226, 226, 12);
        if (biped_perch_motor_program_get(object_index) == _motor_program_bunker) result = _motor_program_ground;
    }
    return result;
}

// @address 0x38b5d8 ?biped_update_perch_state@@YAXJPAUs_character_physics_update_output_datum@@@Z
void biped_update_perch_state(long object_index, s_character_physics_update_output_datum *output)
{
    biped_datum *biped = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *perch = (s_biped_perch_motor_block *)((char *)biped + biped->unit.motor_offset);
    real_point3d origin;
    real_vector3d forward;
    real_vector3d up;
    object_get_origin(object_index, &origin);
    ::object_get_orientation(object_index, &forward, &up);
    if (fabsf(biped->unit.throttle.k) > 0.0f || perch->perch_type == 4)
    {
        perch->perch_type = 4;
        biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
        c_motor_task *task = (c_motor_task *)((char *)unit + unit->unit.motor_tasks.offset);
        if (task && (task->program_default() || !task->valid()) && biped->biped.physics.m_mode != 5)
            ::biped_enter_climbing_physics(object_index, &origin);
    }
    else
    {
        if (biped->biped.physics.m_mode != 4)
            biped_enter_posture_physics(object_index, &origin, 0);
        int mode;
        int state;
        if (::motor_animation_get_mode_and_state(object_index, (long *)(&mode), (long *)(&state)) && state == 12)
        {
            bool known = false;
            bool positive;
            switch (perch->perch_type)
            {
            case 1:
                positive = true;
                known = true;
                break;
            case 2:
                positive = false;
                known = true;
                break;
            }
            if (known)
            {
                real_vector2d heading = biped->unit.desired_aiming_vector.xy;
                real_vector2d facing = forward.xy;
                if (normalize2d(&heading) > 0.0f && normalize2d(&facing) > 0.0f)
                {
                    float angle = signed_angle_between_vectors2d(&facing, &heading);
                    bool angle_positive = angle > 0.0f;
                    if (angle_positive != positive && fabsf(angle) > 0.34906584f)
                        perch->perch_type = angle_positive ? 1 : 2;
                }
            }
        }
    }
}

// @address 0x38ba88 biped_perch_try_exit
extern "C" long biped_perch_try_exit(long object_index)
{
    long result = -1;
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_bunker_motor_block *block = (s_biped_bunker_motor_block *)((char *)unit + unit->unit.motor_offset);
    s_character_physics_move_output_datum output;
    motor_system_biped_move(object_index, false, false, false, false, &output);
    if (block->exit_requested) {
        ::motor_animation_posture_exit_end(object_index, 138, 226, 226, 31);
        result = 1;
    }
    return result;
}

static __forceinline bool biped_perch_moving_in_climbing_physics(long object_index)
{
    long movement_type = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped))->biped.physics.m_mode;
    return movement_type == 4 || movement_type == 5;
}


static __forceinline void biped_perch_moving_translate(const real_point3d &origin, real_point3d *point)
{
    point->x += origin.x;
    point->y += origin.y;
    point->z += origin.z;
}

// @address 0x38b800 ?biped_perch_update_moving@@YAXJPAUs_animation_control_data@@PAUs_character_physics_update_input_datum@@PAUs_character_physics_update_output_datum@@@Z
void biped_perch_update_moving(long object_index, struct s_animation_control_data *animation_control_data,
    struct s_character_physics_update_input_datum *character_physics_update_input_datum,
    struct s_character_physics_update_output_datum *character_physics_update_output_datum)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    s_biped_perch_motor_block *perch = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    biped_update_perch_state(object_index, character_physics_update_output_datum);
    biped_update_perch_animation(object_index, (e_animation_state)character_physics_update_output_datum->animation_state,
        character_physics_update_output_datum->value, &animation_control_data->mode, &animation_control_data->state,
        &animation_control_data->moved);
    if (character_physics_update_output_datum->flags & 4) {
        perch->exit_requested = 1;
        return;
    }
    if (perch->apply_translation) {
        if (biped_perch_moving_in_climbing_physics(object_index)) {
            unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
            real_point3d position = {{perch->origin.x, perch->origin.y, perch->origin.z}};
            biped_perch_moving_translate(unit->object.position, &position);
            object_set_position_direct(object_index, &position, 0, 0, 0, false);
        }
        perch->apply_translation = 0;
    }
}


static __forceinline long biped_bunker_object_get_root_object(long object_index)
{
    while (object_index != -1)
    {
        biped_datum *object = (biped_datum *)object_get_and_verify_type(object_index, NONE);
        if (!(bool)((object->object.flags >> 26) & 1))
            break;
        object_index = object->object.parent_object_index;
    }
    return object_index;
}

// @address 0x38b928 ?biped_perch_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_perch_update(long biped_index, s_animation_control_data *animation_control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, NONE);
    bool result = false;
    if (biped->object.havok_component_index != -1 && biped_bunker_object_get_root_object(biped->object.parent_object_index) == -1)
    {
        s_character_physics_update_input_datum input;
        s_character_physics_update_output_datum output;
        if (::motor_system_biped_physics_preupdate_moving(biped_index, &input,
                &output, false, false, 0.0f, false, false))
        {
            biped_perch_update_moving(biped_index, &control, &input, &output);
            motor_system_biped_physics_postupdate_moving(biped_index, &input, &output, &control.crouch);
            result = true;
        }
    }
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        ::motor_animation_build_state_change_settings_from_control_data(biped_index, &control,
            &requested, &settings);
        result = (0 != motor_task_execute_state_change_settings(biped_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    return result;
}

// @address 0x38be48 ?biped_bunker_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_bunker_update(long biped_index, struct s_animation_control_data *animation_control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    biped_datum *biped = (biped_datum *)object_get_and_verify_type(biped_index, NONE);
    bool result = false;
    if (biped->object.havok_component_index != -1 && biped_bunker_object_get_root_object(biped->object.parent_object_index) == -1)
    {
        s_character_physics_update_input_datum input;
        s_character_physics_update_output_datum output;
        if (::motor_system_biped_physics_preupdate_moving(biped_index, &input,
                &output, false, false, 0.0f, false, false))
        {
            s_biped_bunker_motor_block *bunker = (s_biped_bunker_motor_block *)((char *)biped + biped->unit.motor_offset);
            control.state = 12;
            control.moved = false;
            switch (bunker->variant)
            {
            case 1:
                control.mode = bunker->open ? 260 : 272;
                break;
            case 2:
                control.mode = bunker->open ? 261 : 273;
                break;
            case 3:
                control.mode = bunker->open ? 726 : 727;
                break;
            }
            if (output.flags & 4)
                bunker->moved = true;
            motor_system_biped_physics_postupdate_moving(biped_index, &input, &output, &control.crouch);
            result = true;
        }
    }
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        ::motor_animation_build_state_change_settings_from_control_data(biped_index, &control,
            &requested, &settings);
        result = (0 != motor_task_execute_state_change_settings(biped_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    return result;
}


// @address 0x2fbcb0 ?get_keyframe_translation@c_channel_base@@QBAXMGFPATreal_point3d@@@Z

static __forceinline long biped_perch_animation_node_flags_offset(const c_animation_data *data)
{
    return (((data->m_node_count + 31) >> 3) & ~3) + data->m_sizes->m_default_data_size;
}
static __forceinline const unsigned long *biped_perch_animation_node_bits(const c_animation_data *data, long flags_offset)
{
    return (const unsigned long *)(data->m_data + data->m_sizes->m_static_node_flags_data_size + data->m_sizes->m_compressed_data_size + flags_offset);
}
static __forceinline bool biped_perch_animation_node_is_animated(const c_animation_data *data, short node_index, long flags_offset)
{
    const unsigned long *bits = biped_perch_animation_node_bits(data, flags_offset);
    return (bits[node_index >> 5] & (1UL << (node_index & 31))) != 0;
}

// @address 0x38ac40 ?biped_perch_default_finished@@YA_NJPAVc_motor_task@@@Z
bool biped_perch_default_finished(long object_index, c_motor_task *)
{
    biped_datum *unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
    c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    if (animation->m_current_goal.state != 29 && animation->m_current_goal.state != 30)
        return true;
    long perch_index =
        TAG_GET('bipd', const biped_definition, unit->object.definition_index)->perch_node_index;
    if (perch_index == -1)
        return true;
    c_animation_data data;
    real_point3d point;
    data.m_data = 0;
    data.m_sizes = 0;
    data.m_node_count = 0;
    data.m_movement_data_type = 0;
    real_point3d translation;
    long flags_offset;
    if (animation_definitions_try_and_get_data((const c_channel_base *)animation, &data) &&
        biped_perch_animation_node_is_animated(&data, perch_index, flags_offset = biped_perch_animation_node_flags_offset(&data)))
    {
        long count = bit_vector_count_bits(biped_perch_animation_node_bits(&data, flags_offset), perch_index + 1);
        long keyframe = count - 1;
        real_matrix4x3 world;
        object_get_world_matrix(object_index, &world);
        ((const c_channel_base *)animation)->get_keyframe_translation(0.0f, keyframe, perch_index, &point);
        matrix4x3_transform_point(&world, &point, &point);
        biped_datum *node_unit = ((biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped));
        const real_matrix4x3 *node = (const real_matrix4x3 *)((const char *)node_unit +
            (node_unit->object.node_matrices.offset + (short)perch_index * sizeof(real_matrix4x3)));
        translation.x = node->position.x - point.x;
        translation.y = node->position.y - point.y;
        translation.z = node->position.z - point.z;
    }
    else
    {
        translation = *global_origin3d;
    }
    s_biped_perch_motor_block *motor = (s_biped_perch_motor_block *)((char *)unit + unit->unit.motor_offset);
    motor->apply_translation = 1;
    motor->origin = translation;
    return true;
}

// tmp/probe (GAME-CHANGES F52): external names for the retail motor tables (retail 0x38bb18, 0x38bc88).
e_motor_result probe_perch_bunker_close_submit(long object_index, const c_motor_request *request) { return biped_bunker_close_submit(object_index, request); }
e_motor_result probe_perch_bunker_open_submit(long object_index, const c_motor_request *request) { return biped_bunker_open_submit(object_index, request); }
