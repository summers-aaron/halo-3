#ifndef MOTOR_BIPED_GROUND_MOTOR_PROGRAM_H
#define MOTOR_BIPED_GROUND_MOTOR_PROGRAM_H

#include "physics/character_physics_output.h"

// Functions defined in tu/objectslibxbox/biped_ground_motor_program.cpp.

class c_animation_manager;
class c_motor_task;
struct s_animation_control_data;

bool biped_perch_takeoff_finished(long object_index, c_motor_task *task);
char get_biped_state_index_from_string(c_animation_manager *animation_manager, long state);
bool biped_update_jumping(long object_index);
void biped_landing_animation(long object_index, s_animation_control_data *animation_control_data);
bool biped_jump(long object_index);
void biped_start_landing(long object_index, float velocity);

// Other callers retain incompatible C bridge signatures.
extern "C" bool biped_update_melee_aiming(long biped_index, s_animation_control_data *animation_control_data);

extern bool g_biped_fake_hard_landing;

#endif // MOTOR_BIPED_GROUND_MOTOR_PROGRAM_H
