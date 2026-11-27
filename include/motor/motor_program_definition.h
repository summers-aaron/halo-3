#ifndef MOTOR_MOTOR_PROGRAM_DEFINITION_H
#define MOTOR_MOTOR_PROGRAM_DEFINITION_H

#include <stddef.h>

#include "units/motor_enums.h"
#include "motor/motor_programs.h"

class c_motor_request;
struct s_animation_control_data;
struct s_animation_state_change_settings;
struct real_matrix4x3;
struct real_orientation;
template<long k_maximum_count> class c_static_flags;

// One motor program (biped ground, biped dead, vehicle, ...): a table of callbacks plus the program's task
// definitions; g_program_definition (motor_programs.cpp) indexes them by e_motor_program.
//
// Layout: March's constructor (82838bb0) stores its 17 arguments at +0x00..+0x40, and its mangled name gives every
// callback's type; one slot is a per-object reset callback used by the motor system,
// and everything March kept at +0x34 and later moves up by four (motor_animation.cpp reads the state-change
// callback at +0x38; find_task_definition reads the tasks at +0x40 and their count at +0x44).
// Names: each slot is named after the callback that fills it (biped_ground_activate, biped_ground_update, biped_ground_move,
// generic_motor_program_submit, biped_dead_in_airborne_state, biped_dead_relaxation_state) and March's mover.obj
// forwarders (mover_handle_deleted_object, mover_fix_transform_to_physics, mover_fix_transform_from_physics,
// mover_preprocess_node_orientations, mover_preprocess_root_node_matrix), each of which calls its slot.
// The slot at +0x34 is named after its one caller, motor_system_reset_program. The last slot (+0x3c) is the
// per-program render_debug override: March mover_render_debug (mover.obj) calls its +0x38 slot when set and falls
// back to mover_render_debug_internal, and only ground, airborne and leap pass one (all three fold to the empty
// ship-build stub at 0x825bec98).
class c_motor_program_definition
{
public:
    s_motor_task_definition *find_task_definition(e_motor_task task);

    bool (*activate)(long object_index);
    void (*deactivate)(long object_index);
    void (*dispose)(long object_index);                                                     // mover_delete
    e_motor_result (*submit)(long object_index, const c_motor_request *request);
    bool (*update)(long object_index, s_animation_control_data *animation_control);
    e_motor_program (*move)(long object_index, c_motor_request *request);
    void (*handle_deleted_object)(long object_index, long deleted_object_index);
    bool (*in_airborne_state)(long object_index);
    short (*relaxation_state)(long object_index);
    void (*fix_transform_to_physics)(long object_index, real_matrix4x3 *matrix);
    void (*fix_transform_from_physics)(long object_index, real_matrix4x3 *matrix);
    void (*preprocess_node_orientations)(long object_index, const c_static_flags<255> *node_flags, long node_count,
        real_orientation *orientations);
    void (*preprocess_root_node_matrix)(long object_index, real_matrix4x3 *matrix);
    void (*reset_program)(long object_index);
    bool (*state_change)(long object_index, s_animation_state_change_settings *settings);
    void (*render_debug)(long object_index);
    s_motor_task_definition **tasks;
    short task_count;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char motor_program_definition_size[(sizeof(void *) != 4 || (sizeof(c_motor_program_definition) == 0x48)) ? 1 : -1];
typedef char motor_program_definition_move_offset[(sizeof(void *) != 4 || (offsetof(c_motor_program_definition, move) == 0x14)) ? 1 : -1];
typedef char motor_program_definition_from_physics_offset[(sizeof(void *) != 4
    || (offsetof(c_motor_program_definition, fix_transform_from_physics) == 0x28)) ? 1 : -1];
typedef char motor_program_definition_root_offset[(sizeof(void *) != 4
    || (offsetof(c_motor_program_definition, preprocess_root_node_matrix) == 0x30)) ? 1 : -1];
typedef char motor_program_definition_tasks_offset[(sizeof(void *) != 4 || (offsetof(c_motor_program_definition, tasks) == 0x40)) ? 1 : -1];
#endif

#endif // MOTOR_MOTOR_PROGRAM_DEFINITION_H
