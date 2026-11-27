// objectslibxbox/mover.obj: from the records 0xdc030 0xed230 0x107fd0 0x3532c0 0x353488 0x3534f8 0x353558 0xd65f0
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "animations/animation_id.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "units/motor_task.h"
#include "objects/object_datum.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "objects/object_type_definition.h"
#include "motor/motor_system.h"
#include "motor/mover.h"
#include "objects/mover_datum.h"
#include "units/unit_animation.h"
#include "units/unit_types.h"
#include "units/units.h"
#include "motor/motor_programs.h"
#include "motor/motor_program_definition.h"
#include "motor/motor_animation.h"
#include "units/motor_request.h"
#include "animations/animation_manager.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "ai/interaction_target.h"
#include "motor/mover_action_state.h"
#include <stddef.h>
#include <string.h>

// @address 0x79dbcc ?biped_mover_definition_data@@3Vc_mover_definition_data@@A
c_mover_definition_data biped_mover_definition_data = { 4, 0xf0, 2 };
// @address 0x79dbd4 ?vehicle_mover_definition_data@@3Vc_mover_definition_data@@A
c_mover_definition_data vehicle_mover_definition_data = { 4, 0x18, 2 };
// @address 0x79dbdc ?creature_mover_definition_data@@3Vc_mover_definition_data@@A
c_mover_definition_data creature_mover_definition_data = { 4, 0x0c, 2 };
// @address 0x79dbe4 ?giant_mover_definition_data@@3Vc_mover_definition_data@@A
c_mover_definition_data giant_mover_definition_data = { 4, 0x18, 2 };

// @address 0xdc030 ?mover_post_update@@YA_NJ@Z
bool mover_post_update(long object_index)
{
    return motor_system_update(object_index);
}

bool mover_is_in_entering_state(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    mover_datum *object = (mover_datum *)header->datum;
    bool found;
    if ((1 << header->object_type) & 0x1003) {
        long state = ((c_animation_manager *)object_header_block_get(object, &object->object.animation))->get_state_name();
        long *entering_state = g_unit_entering_state_names;
        found = false;
        for (; !found && (ptrdiff_t)entering_state < (ptrdiff_t)(g_unit_entering_state_names + 5); entering_state++) {
            if (*entering_state == state)
                found = true;
        }
    } else {
        found = ((c_animation_manager *)object_header_block_get(object, &object->object.animation))->get_state_name() == 84;
    }
    return found;
}

// @address 0x107fd0 ?mover_move@@YAXJ@Z
void mover_move(long object_index)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
    if (state->program != _motor_program_none) {
#ifdef HALO3_64BIT
        if ((unsigned long)state->program >= 16)
            return;
#endif
        c_motor_program_definition *program = g_program_definition[state->program];
        if (program != 0) {
            c_motor_request request;
            e_motor_program next_program = program->move(object_index, &request);
            if (next_program != _motor_program_none) {
#ifdef HALO3_64BIT
                // program->move() can trigger object deletion or g_object_memory_pool compaction,
                // invalidating the pre-call object and state pointers.
                object = (mover_datum *)object_try_and_get_and_verify_type(object_index, NONE);
                if (object == 0 || object->motor_program.size <= 0)
                    return;
                state = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
#endif
                state->set_program(object_index, next_program);
                if (request.valid())
                    motor_system_submit(object_index, &request);
            }
        }
    }
}

// @address 0x353488 ?mover_delete@@YAXJ@Z
void mover_delete(long object_index)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    if (object->motor_program.size > 0) {
        c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
        if (state->program != _motor_program_none) {
            c_motor_program_definition *definition = g_program_definition[state->program];
            if (definition != 0) {
                void (*dispose)(long) = definition->dispose;
                if (dispose != 0)
                    dispose(object_index);
            }
        }
    }
}


// @address 0x3532c0 ?mover_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool mover_new(long mover_index, object_placement_data *placement, bool *out_of_memory)
{
    mover_datum *mover = (mover_datum *)object_get_and_verify_type(mover_index, NONE);
    object_type_definition *definition = object_type_definitions[mover->object.identifier_type];
    c_mover_definition_data *data = definition->mover_definition_data;
    bool success = true;
    success = success && ::object_header_block_allocate(mover_index, offsetof(mover_datum, motor_tasks), (short)(data->task_count * sizeof(c_motor_task)), 2);
    success = success && ::object_header_block_allocate(mover_index, offsetof(mover_datum, motor_program), data->program_size, data->program_alignment);
    success = success && ::object_header_block_allocate(mover_index, offsetof(mover_datum, action), sizeof(mover_state), 2);
    if (success) {
        c_motor_program_state *program = (c_motor_program_state *)object_header_block_get(mover, &mover->motor_program);
        c_motor_task *tasks = (c_motor_task *)object_header_block_get(mover, &mover->motor_tasks);
        program->initialize();
        for (short i = 0; i < definition->mover_definition_data->task_count; ++i)
            tasks[i].clear();
        mover_state *state = (mover_state *)object_header_block_get(mover, &mover->action);
        memset(state, 0, sizeof(*state));
        state->object = -1;
        state->action = -1;
        state->task = -1;
        state->target = -1;
        state->interaction.clear();
    } else {
        *out_of_memory = true;
    }
    return success;
}

namespace {
static __forceinline mover_datum *mover_get_object(long object_index)
{
    return (mover_datum *)object_get_and_verify_type(object_index, NONE);
}
static __forceinline c_motor_program_state *mover_get_program_state(mover_datum *object)
{
    return (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
}
}

// @address 0x3534f8 ?mover_fix_transform_from_physics@@YAXJPAUreal_matrix4x3@@@Z
void mover_fix_transform_from_physics(long object_index, real_matrix4x3 *matrix)
{
    mover_datum *object = mover_get_object(object_index);
    c_motor_program_state *state = mover_get_program_state(object);
    c_motor_program_definition *definition = state->definition_get();
    if (definition != 0 && definition->fix_transform_from_physics != 0)
        definition->fix_transform_from_physics(object_index, matrix);
}

// @address 0x353558 ?mover_preprocess_root_node_matrix@@YAXJPAUreal_matrix4x3@@@Z
void mover_preprocess_root_node_matrix(long object_index, real_matrix4x3 *matrix)
{
    mover_datum *object = mover_get_object(object_index);
    c_motor_program_state *state = mover_get_program_state(object);
    c_motor_program_definition *definition = state->definition_get();
    if (definition != 0 && definition->preprocess_root_node_matrix != 0)
        definition->preprocess_root_node_matrix(object_index, matrix);
}

// ---- 0xd65f0 mover_update ----
static __forceinline c_animation_manager *mover_update_manager_get(mover_datum *mover)
{
    return (c_animation_manager *)object_header_block_get(mover, &mover->object.animation);
}
static __forceinline c_motor_task *mover_update_task_get(long object_index, long resource)
{
    mover_datum *mover = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    return (c_motor_task *)object_header_block_get(mover, &mover->motor_tasks) + resource;
}
static __forceinline bool mover_channel_valid(const c_animation_channel *channel)
{
    return channel->m_enabled != 0 && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}
static __forceinline mover_datum *mover_update_object_get(long object_index)
{
    return (mover_datum *)object_get_and_verify_type(object_index, NONE);
}
static __forceinline unsigned char mover_update_object_type_get(long object_index)
{
    return object_header_get(object_index)->object_type;
}
static __forceinline void *mover_update_block_get(long object_index, const s_object_block_reference *reference)
{
    mover_datum *object = mover_update_object_get(object_index);
    return object_header_block_get(object, reference);
}
static __forceinline c_animation_channel *mover_update_channel3_get(long object_index)
{
    mover_datum *object = mover_update_object_get(object_index);
    object_type_definition *type_definition = object_type_definitions[mover_update_object_type_get(object_index)];
    c_animation_channel *channel = 0;
    short count = 0;
    if (type_definition->mixing_board_definition_index != -1)
        count = (short)g_mixing_board_definition_list[type_definition->mixing_board_definition_index].get_component_type_count(1);
    if (count > 1)
        channel = ((c_animation_manager *)mover_update_block_get(object_index, &object->object.animation))->get_mixing_board_animation_channel(1);
    return channel;
}

// @address 0xd65f0 ?mover_update@@YA_NJ@Z
bool mover_update(long object_index)
{
    bool result = false;
    mover_datum *mover = (mover_datum *)object_get_and_verify_type(object_index, NONE);
    c_animation_manager *manager = mover_update_manager_get(mover);
    long node_count = (long)((unsigned long)mover->object.node_orientations.size >> 5);
    real_orientation *source = (real_orientation *)object_header_block_get(mover, &mover->object.node_orientations);
    real_orientation *orientations = (real_orientation *)object_header_block_get(mover, &mover->object.original_node_orientations);
    if (manager->m_graph_tag_index != -1 && !manager->paused()) {
        bool state_changed = manager->update_state_animation(object_animation_callback, object_index, node_count, orientations, source);
        result = manager->update_mixing_board_components(object_animation_callback, object_index) | state_changed;
    }
    bool changed = result;
    {
        c_motor_task *task0 = mover_update_task_get(object_index, 0);
        c_animation_channel *channel0 = motor_animation_resource_get_channel(object_index, _motor_resource_0);
        if (mover_channel_valid(channel0)) {
            if (task0 != 0 && task0->valid()) {
                unsigned short event_flags = channel0->m_event_flags;
                if (event_flags != 0)
                    motor_system_animation_event(object_index, task0, event_flags);
            }
            if ((channel0->m_state_flags & 2) != 0) {
                if (task0 != 0 && task0->valid())
                    motor_system_finished(object_index, task0, channel0);
            }
            if (channel0->playing())
                changed = true;
        }
    }
    if (!manager->paused()) {
        c_motor_task *task1 = mover_update_task_get(object_index, 1);
        c_animation_channel *channel1 = motor_animation_resource_get_channel(object_index, _motor_resource_1);
        if (channel1 != 0) {
            if (mover_channel_valid(channel1)) {
                if ((channel1->m_state_flags & 2) != 0) {
                    if (task1 != 0 && task1->valid())
                        motor_system_finished(object_index, task1, channel1);
                    channel1->reset();
                    if (((1 << object_header_get(object_index)->object_type) & 0x1003) != 0) {
                        unit_datum *unit = (unit_datum *)mover;
                        unsigned short *animation_flags = (unsigned short *)object_header_block_get(unit, &unit->unit.animation_storage);
                        *animation_flags = (unsigned short)(*animation_flags & 0xfdff);
                        unit_animation_locate_constant_overlay(object_index);
                    }
                }
                if (mover_channel_valid(channel1) && channel1->playing())
                    changed = true;
            }
        }
    }
    if (!manager->paused()) {
        c_motor_task *task2 = mover_update_task_get(object_index, 2);
        c_animation_channel *channel2 = motor_animation_resource_get_channel(object_index, _motor_resource_2);
        if (channel2 != 0) {
            if (mover_channel_valid(channel2)) {
                long saved_definition = channel2->m_definition_index;
                if (task2 != 0 && task2->valid()) {
                    unsigned short event_flags = channel2->m_event_flags;
                    if (event_flags != 0)
                        motor_system_animation_event(object_index, task2, event_flags);
                }
                if ((channel2->m_state_flags & 2) != 0) {
                    if (task2 != 0 && task2->valid())
                        motor_system_finished(object_index, task2, channel2);
                    if (channel2->m_definition_index == saved_definition && !motor_animation_action_loops(object_index, _motor_resource_2)) {
                        channel2->reset();
                        if (mover->object.object_type != _object_type_vehicle)
                            object_start_interpolation(object_index, 0.26666668f);
                    }
                }
                if (mover_channel_valid(channel2) && channel2->playing())
                    changed = true;
            }
        }
    }
    if (!manager->paused()) {
        c_motor_task *task3 = mover_update_task_get(object_index, 3);
        c_animation_channel *channel3 = mover_update_channel3_get(object_index);
        if (channel3 != 0 && mover_channel_valid(channel3)) {
            long saved_definition = channel3->m_definition_index;
            if (task3 != 0 && task3->valid()) {
                unsigned short event_flags = channel3->m_event_flags;
                if (event_flags != 0)
                    motor_system_animation_event(object_index, task3, event_flags);
            }
            if ((channel3->m_state_flags & 2) != 0) {
                if (task3 != 0 && task3->valid())
                    motor_system_finished(object_index, task3, channel3);
                if (channel3->m_definition_index == saved_definition && !motor_animation_action_loops(object_index, _motor_resource_3)) {
                    channel3->reset();
                    if (mover->object.object_type != _object_type_vehicle)
                        object_start_interpolation(object_index, 0.13333334f);
                }
            }
            if (mover_channel_valid(channel3) && channel3->playing())
                return true;
        }
    }
    return changed;
}
