#ifndef MOTOR_BIPED_DEAD_MOTOR_PROGRAM_H
#define MOTOR_BIPED_DEAD_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/biped_dead_motor_program.cpp.

union real_vector3d;

bool biped_dead_in_airborne_state(long object_index);
void biped_update_constraint_limits(long index);
void biped_relaxation_finish(long object_index);
void biped_relaxation_calculate_object_relative_safe_position(long index);
void biped_update_out_of_map_status(long biped_index);
void biped_initialize_head_shot_death(long biped_index, long rigid_body_index, const real_vector3d *direction);
extern "C" void biped_dead_reset_airborne_state(long object_index);
void biped_exit_relaxation(long biped_index, bool reposition);
bool biped_start_relaxation(long object_index, bool lift);

enum e_biped_relaxation_pose;
void biped_relaxation_pose_set(long biped_index, e_biped_relaxation_pose pose);

extern "C" float g_biped_relaxation_angular_velocity_threshold;
extern "C" unsigned char g_biped_ragdoll_force_physics;
extern "C" unsigned char g_biped_ragdoll_skip_rebuild;
extern "C" const float biped_dead_update_pool[];

#endif // MOTOR_BIPED_DEAD_MOTOR_PROGRAM_H
