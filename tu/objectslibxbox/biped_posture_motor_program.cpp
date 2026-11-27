// objectslibxbox/biped_posture_motor_program.obj: from the records 0xe0e80 0x38e8e0 0x38ea98 0x38eb08 0x38eca8
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"
#include <new>
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "motor/motor_system_biped.h"
#include "objects/mover_datum.h"
#include "physics/havok.h"
#include "units/biped_types.h"
#include "animations/animation_control_data.h"
#include "animations/animation_state_change_settings.h"
#include "physics/character_physics_move_output.h"
#include "ai/ai_postures.h"
#include "units/motor_request.h"
// @address 0xe0e80 ?biped_posture_deactivate@@YAXJ@Z



struct posture_motor_state : c_motor_program_state { bool active; };
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char posture_motor_state_active_offset[((unsigned long)&((posture_motor_state *)0)->active == 8) ? 1 : -1];
#endif

#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/animation_manager.h"
#include "units/motor_task.h"

void biped_posture_deactivate(long object_index)
{
    biped_datum *datum = (biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped);
    posture_motor_state *block = (posture_motor_state *)((char *)datum + datum->unit.motor_offset);
    short flags = block->state;
    if ((flags & 4) == 0)
        return;
    block->state = flags & -5;
    ::havok_object_rebuild(object_index, 0);
}

// @address 0x38ea98 ?biped_posture_activate@@YA_NJ@Z
bool biped_posture_activate(long object_index)
{
    biped_datum *datum = (biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped);
    posture_motor_state *block = (posture_motor_state *)((char *)datum + datum->unit.motor_offset);
    block->active = false;
    if (datum->biped.physics.m_mode != 4) {
        block->state |= 4;
        ::havok_object_rebuild(object_index, 0);
    }
    return true;
}

// @address 0xdd5b0 ?motor_task_allow@@YA_NJW4e_action@@F@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0x356738 ?motor_task_set_posture@@YAPAVc_motor_task@@JW4e_motor_task@@W4e_action@@JJ_NPBTreal_point3d@@PBTreal_vector3d@@F2PA_N@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z






// @rdata 0x8202251c
static const float biped_posture_disabled_interpolation_time = 0.0f;

static __forceinline mover_datum *posture_object_datum_get(long index) {
    return (mover_datum *)object_get_and_verify_type(index, _object_mask_biped);
}

// @address 0x38e8e0 external_biped_posture_submit
extern "C" long external_biped_posture_submit(long object_index,const c_motor_request *request)
{
    real_point3d position=request->m_posture.position;
    real_vector3d forward=request->m_posture.forward;
    long posture=request->m_posture.posture;
    bool instant=request->m_posture.immediate;
    bool exit=request->m_posture.exit_to_default;
    mover_datum *biped=posture_object_datum_get(object_index);
    long mode=instant?12:84;
    long result=0;
    if(posture!=-1) {
        if(instant) {
            if(request->force()||motor_task_allow(object_index,(e_action)request->action,15)) {
                if(motor_animation_set_posture(object_index,mode,posture,exit,&position,&forward,0)) {
                    result=1;
                    mover_datum *object=posture_object_datum_get(object_index);
                    c_animation_manager *animation=(c_animation_manager*)((char*)object+object->object.animation.offset);
                    animation->m_interpolator.m_time=biped_posture_disabled_interpolation_time;
                    animation->m_interpolator.m_flags=0;
                    animation->m_interpolator.m_duration=biped_posture_disabled_interpolation_time;
                    object_wake(object_index);
                }
            }
        } else {
            bool finished;
            if (motor_task_set_posture(object_index, _motor_task_posture, (e_action)request->action, mode, posture, exit, &position, &forward,
                    15, request->force(), &finished))
                result = 2;
            else if(finished) result=1;
        }
        if(result!=0) {
            void *state=(char*)biped+biped->motor_program.offset;
            if(!((c_motor_program_state*)state)->set_program(object_index,_motor_program_posture)) result=0;
        }
    }
    return result;
}



// @address 0x38eca8 ?biped_bunker_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_bunker_move(long object_index, c_motor_request *request)
{
    e_motor_program result = _motor_program_none;
    mover_datum *biped = posture_object_datum_get(object_index);
    posture_motor_state *state = (posture_motor_state *)((char *)biped + biped->motor_program.offset);
    s_character_physics_move_output_datum output;
    motor_system_biped_move(object_index, false, false, false, false, &output);
    if (state->active) {
        motor_animation_posture_exit_end(object_index, 138, 226, 226, 31);
        result = _motor_program_airborne;
    }
    return result;
}


static __forceinline bool biped_posture_test_bit(unsigned long flags, long bit) { return (flags & (1UL << bit)) != 0; }
// @address 0x38eb08 ?biped_posture_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_posture_update(long object_index, struct s_animation_control_data *control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    bool updated = false;
    biped_datum *datum = (biped_datum *)object_get_and_verify_type(object_index, _object_mask_biped);
    if (datum->object.havok_component_index != -1)
    {
        long next = datum->object.parent_object_index;
        while (next != -1)
        {
            if (!biped_posture_test_bit(((object_datum *)object_get_and_verify_type(next, NONE))->flags, 26))
                break;
            next = ((object_datum *)object_get_and_verify_type(next, NONE))->parent_object_index;
        }
        if (next == -1)
        {
            s_character_physics_update_input_datum input;
            s_character_physics_update_output_datum output;
            if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, false, false, 0.0f, false, false))
            {
                control.state = 12;
                control.mode = 226;
                short motor_offset = datum->unit.motor_offset;
                control.moved = false;
                posture_motor_state *state = (posture_motor_state *)((char *)datum + motor_offset);
                if ((output.flags & 4) != 0)
                {
                    if (datum->unit.actor_index == -1 || ai_posture_allow_automatic_exit(datum->unit.actor_index))
                    {
                        state->active = true;
                    }
                }
                motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouch);
                updated = true;
            }
        }
    }

    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        settings.flags.clear();
        requested.goals.clear();
        requested.transition.clear();
        requested.primary.clear();
        motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
        updated = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | updated;
    }
    return updated;
}
