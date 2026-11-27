#ifndef MOTOR_MOTOR_SYSTEM_H
#define MOTOR_MOTOR_SYSTEM_H

#include "units/motor_enums.h"

// Functions defined in tu/objectslibxbox/motor_system.cpp.

class c_motor_task;

void motor_system_interrupted(long object_index, c_motor_task *task, bool reset_channel);
bool motor_system_in_airborne_state(long object_index);
extern "C" short motor_system_is_relaxed(long object_index);
void motor_system_animation_event(long object_index, c_motor_task *task, long event);
bool motor_system_update(long object_index);
extern "C" void motor_system_reset_program(long object_index);
template<long k_maximum_count> class c_static_flags;
struct real_orientation;

// This sits in the mover object type table between mover_fix_transform_from_physics and
// mover_preprocess_root_node_matrix (0x8273d2b0..), where March has mover_preprocess_node_orientations: it passes its
// arguments through to the program's preprocess_node_orientations callback.
extern "C" void motor_system_initialize_program(long object_index, const c_static_flags<255> *node_flags, long node_count,
    real_orientation *orientations);

class c_motor_request;

e_motor_result motor_system_submit(long object_index, const c_motor_request *request);

class c_animation_channel;

bool motor_system_finished(long object_index, c_motor_task *task, c_animation_channel *channel);

#endif // MOTOR_MOTOR_SYSTEM_H
