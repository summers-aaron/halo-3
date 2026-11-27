// objectslibxbox/unit_phase_motor_program.obj: from the records 0x391f80 0x391fd8 0x392010 0x3921c8
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "units/motor_enums.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "motor/motor_programs.h"
#include "animations/animation_manager.h"
#include "objects/mover_datum.h"
#include "motor/state_change_flag_bits_types.h"
#include "units/motor_program_state.h"
#include "animations/animation_control_data.h"
#include "animations/animation_state_change_settings.h"
#include "units/motor_task.h"



// the unit's phase block: object.animation (+0x14e, offset at +0x150) and unit motor (+0x164, offset at +0x166)
struct unit_phase_motor_state {
    c_motor_program_state program;
    bool initialized;
    short counter;
    real_point3d position;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char unit_phase_motor_initialized_offset[(offsetof(unit_phase_motor_state, initialized) == 8) ? 1 : -1];
typedef char unit_phase_motor_counter_offset[(offsetof(unit_phase_motor_state, counter) == 0xa) ? 1 : -1];
typedef char unit_phase_motor_position_offset[(offsetof(unit_phase_motor_state, position) == 0xc) ? 1 : -1];
#endif

static __forceinline mover_datum *unit_phase_object_get(long object_index)
{
    return OBJECT_GET(object_index, mover_datum, _object_mask_unit);
}

// @address 0x391f80 ?unit_phase_activate@@YA_NJ@Z
bool unit_phase_activate(long object_index)
{
    mover_datum *object = unit_phase_object_get(object_index);
    c_animation_manager *state = (c_animation_manager *)object_header_block_get(object, &object->object.animation);
    state->m_flags |= 1;
    object_disconnect_from_physics(object_index);
    return true;
}

// @address 0x391fd8 ?unit_phase_deactivate@@YAXJ@Z
void unit_phase_deactivate(long object_index)
{
    mover_datum *object = unit_phase_object_get(object_index);
    c_animation_manager *state = (c_animation_manager *)object_header_block_get(object, &object->object.animation);
    state->m_flags &= ~1;
    object_reconnect_to_physics(object_index);
}

// @address 0x3921c8 ?unit_phase_move@@YA?AW4e_motor_program@@J@Z
e_motor_program unit_phase_move(long object_index)
{
    mover_datum *object = unit_phase_object_get(object_index);
    unit_phase_motor_state *state = (unit_phase_motor_state *)object_header_block_get(object, &object->motor_program);
    e_motor_program program = _motor_program_none;
    state->counter = state->counter - 1;
    if (state->counter <= 0)
        program = mover_default_program(object_index);
    return program;
}


static __forceinline void unit_phase_update_position(long object_index, unit_phase_motor_state *motor)
{
    real_point3d origin;
    object_get_origin(object_index, &origin);
    if (!motor->initialized)
    {
        motor->counter = 0;
        motor->initialized = true;
        motor->position = origin;
    }
    else
    {
        real_vector3d delta;
        vector_from_points3d(&origin, &motor->position, &delta);
        float length = normalize3d(&delta);
        if (length > 0.0f && motor->counter > 0)
        {
            point_from_line3d(&origin, &delta, length / (float)motor->counter, &origin);
            object_set_position_direct(object_index, &origin, 0, 0, 0, false);
        }
    }
}

// @address 0x392010 ?unit_phase_update@@YA_NJPAUs_animation_control_data@@@Z
bool unit_phase_update(long object_index, s_animation_control_data *control_data)
{
    s_animation_control_data local_control;
    local_control.set_defaults();
    mover_datum *object = unit_phase_object_get(object_index);
    unit_phase_motor_state *motor = (unit_phase_motor_state *)object_header_block_get(object, &object->motor_program);
    {
        real_point3d origin;
        object_get_origin(object_index, &origin);
        if (!motor->initialized)
        {
            motor->counter = 0;
            motor->initialized = true;
            motor->position = origin;
        }
        else
        {
            real_vector3d delta;
            vector_from_points3d(&origin, &motor->position, &delta);
            float length = normalize3d(&delta);
            if (length > 0.0f && motor->counter > 0)
            {
                point_from_line3d(&origin, &delta, length / (float)motor->counter, &origin);
                object_set_position_direct(object_index, &origin, 0, 0, 0, false);
            }
        }
    }
    object_set_requires_motion(object_index);
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        requested.goals.clear();
        requested.transition.clear();
        requested.primary.clear();
        motor_animation_build_state_change_settings_from_control_data(object_index, &local_control, &requested, &settings);
        motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, local_control.moved);
    }
    return true;
}
