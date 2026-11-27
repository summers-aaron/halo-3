// objectslibxbox/motor_system.obj: from the records 0xc1638 0xdd8d8 0xe61d0 0xe8f00 0xecf88 0x107f70 0x1085c8 0x122a88 0x33e548 0x33e620 0x33e6a0 0x33e7b8
// @flags /TP /O2
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/motor_task.h"
// @address 0xc1638 ?program_default@c_motor_task@@QAA_NXZ
// @address 0xdd8d8 ?motor_system_interrupted@@YAXJPAVc_motor_task@@_N@Z
// @address 0xe61d0 motor_system_is_relaxed
// @address 0xe8f00 ?motor_system_in_airborne_state@@YA_NJ@Z
// @address 0x107f70 motor_system_reset_program
// @address 0x1085c8 motor_system_initialize_program
// @address 0xecf88 ?motor_system_finished@@YA_NJPAVc_motor_task@@PAVc_animation_channel@@@Z
// @address 0x33e548 ?motor_system_update@@YA_NJ@Z
// @address 0x33e6a0 ?motor_system_interrupted@@YA_NJ@Z
// @address 0x33e7b8 ?motor_system_object_post_update@@YA_NJ@Z
#include "motor/action_system.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "motor/motor_system.h"
#include "motor/motor_animation.h"
#include "motor/motor_programs.h"
#include "objects/mover_datum.h"
#include "motor/motor_programs.h"

#include "motor/motor_program_definition.h"

#include "animations/animation_manager.h"
#include "units/motor_program_state.h"

template<class T> class c_motor_system_flags {
    T storage;
public:
    __forceinline void set(long bit, bool enable) { if (enable) storage |= 1 << bit; else storage &= ~(1 << bit); }
};

static __forceinline c_motor_program_definition *debug_motor_program_definition_get(e_motor_program program)
{
    return g_program_definition[program];
}

static __forceinline c_motor_program_definition *motor_system_program_definition_get(long object_index)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    long program = *(long *)((char *)object + object->motor_program.offset);
    return program != -1 ? g_program_definition[program] : 0;
}

bool c_motor_task::program_default()
{
    return valid() && task == _motor_task_default;
}

// 0xdd8d8
void motor_system_interrupted(long object_index, c_motor_task *task, bool reset_channel)
{
    if (!task->interrupted) {
        c_motor_program_definition *program_definition = debug_motor_program_definition_get(task->program);
        s_motor_task_definition *task_definition = program_definition->find_task_definition(task->task);
        task->interrupted = true;
        if (task_definition && task_definition->interrupted)
            task_definition->interrupted(object_index, task);
        if (task->action != _action_none) {
            const s_action_definition *action_definition = action_get_definition(task->action);
            if (action_definition->interrupted)
                action_definition->interrupted(object_index, task->action);
        }
        if (reset_channel) {
            c_animation_channel *channel = motor_animation_task_get_channel(object_index, task);
            channel->reset();
        }
        task->clear();
    } else if (reset_channel) {
        c_animation_channel *channel = motor_animation_task_get_channel(object_index, task);
        channel->reset();
    }
}

extern "C" short motor_system_is_relaxed(long object_index)
{
  c_motor_program_definition *definition = motor_system_program_definition_get(object_index);
  short result = 0;
  if (definition->relaxation_state)
    result = definition->relaxation_state(object_index);
  return result;
}

bool motor_system_in_airborne_state(long object_index)
{
  c_motor_program_definition *definition = motor_system_program_definition_get(object_index);
  bool result = false;
  if (definition->in_airborne_state)
    result = definition->in_airborne_state(object_index);
  return result;
}

static __forceinline bool motor_system_task_program_default(c_motor_task *task)
{
    return task->valid() && task->task == 0;
}

// 0xecf88
bool motor_system_finished(long object_index, c_motor_task *task, c_animation_channel *channel)
{
    c_motor_program_definition *program_definition = debug_motor_program_definition_get(task->program);
    s_motor_task_definition *task_definition = program_definition->find_task_definition(task->task);
    bool finished = !motor_system_task_program_default(task);
    if (task_definition && task_definition->finished)
        finished = task_definition->finished(object_index, task);
    if (finished) {
        if (channel && channel->valid())
            channel->set_playback_complete();
        e_action action = task->action;
        task->clear();
        if (action != _action_none) {
            const s_action_definition *action_definition = action_get_definition(action);
            if (action_definition->finished)
                action_definition->finished(object_index, action);
        }
    }
    return finished;
}

// 0x107f70
void motor_system_reset_program(long object_index)
{
    c_motor_program_definition *definition = motor_system_program_definition_get(object_index);
    if (definition && definition->reset_program)
        definition->reset_program(object_index);
}

// 0x1085c8
void motor_system_initialize_program(long object_index, const c_static_flags<255> *node_flags, long node_count, real_orientation *orientations)
{
    c_motor_program_definition *definition = motor_system_program_definition_get(object_index);
    if (definition && definition->preprocess_node_orientations)
        definition->preprocess_node_orientations(object_index, node_flags, node_count, orientations);
}

// 0x122a88
void motor_system_animation_event(long object_index, c_motor_task *task, long event)
{
    if (task && task->valid()) {
        c_motor_program_definition *program_definition = debug_motor_program_definition_get(task->program);
        s_motor_task_definition *task_definition = program_definition->find_task_definition(task->task);
        if (task_definition && task_definition->animation_event)
            task_definition->animation_event(object_index, task, 0);
        if (task->action != _action_none) {
            const s_action_definition *action_definition = action_get_definition(task->action);
            if (action_definition->animation_event)
                action_definition->animation_event(object_index, task->action, event);
        }
    }
}

// 0x33e6a0
bool motor_system_interrupted(long object_index)
{
    bool result = false;
    for (short index = 0; index < 4; index++) {
        mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
        c_motor_task *task = (c_motor_task *)((char *)object + object->motor_tasks.offset) + index;
        if (task && task->valid()) {
            c_motor_program_definition *program_definition = debug_motor_program_definition_get(task->program);
            s_motor_task_definition *task_definition = program_definition->find_task_definition(task->task);
            result |= task->task != 0;
            if (task_definition && task_definition->can_interrupt) {
                if (!task_definition->can_interrupt(object_index, task))
                    motor_system_finished(object_index, task, motor_animation_task_get_channel(object_index, task));
            }
        }
    }
    return result;
}

bool motor_system_object_post_update(long object_index)
{
    bool result = false;
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    if (object->object.havok_component_index != -1) {
        do {
            if ((bool)((object->object.physics_flags >> 9) & 1)) {
                c_animation_manager *manager = (c_animation_manager *)((char *)object + object->object.animation.offset);
// match: 0x33e7b8: inlining valid() into motor_system_object_post_update adds 88 bytes.
#pragma inline_depth(0)
                if (manager == 0 || !manager->valid())
// match: 0x33e620: restoring the default depth keeps motor_system_submit from growing by 32 bytes.
#pragma inline_depth()
                    break;
                if (manager->paused() || manager->state_channel_paused())
                    break;
            }
            result = true;
        } while (false);
        if (result) {
            object->object.last_motion_time = tls_game_time_globals()->time;
            object_set_requires_motion(object_index);
        }
    }
    return result;
}

// 0x33e548
bool motor_system_update(long object_index)
{
    bool result = false;
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    short program_offset = object->motor_program.offset;
    long program = *(long *)((char *)object + program_offset);
    if (program != -1 && g_program_definition[program]) {
        bool interrupted = motor_system_interrupted(object_index);
        bool updated = action_update(object_index);
        program = *(long *)((char *)object + program_offset);
        bool changed = updated | interrupted;
        c_motor_program_definition *definition = program != -1 ? g_program_definition[program] : 0;
        bool handled = definition->update(object_index, 0);
        bool combined = handled | changed;
        bool posted = motor_system_object_post_update(object_index);
        result = posted | combined;
    }
    return result;
}

e_motor_result motor_system_submit(long object_index, const c_motor_request *request)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, 12291);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object_get_and_verify_type(object_index, ~0UL), &object->motor_program);
    e_motor_result result = _motor_result_failed;
    if (state->program >= 0 && state->program < 16) {
        c_motor_program_definition *definition = state->definition_get();
        if (definition && definition->submit) {
            result = definition->submit(object_index, request);
        }
    }
    return result;
}
