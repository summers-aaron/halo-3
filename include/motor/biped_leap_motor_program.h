#ifndef MOTOR_BIPED_LEAP_MOTOR_PROGRAM_H
#define MOTOR_BIPED_LEAP_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/biped_leap_motor_program.cpp.

union real_point3d;
union real_vector3d;
struct s_animation_control_data;

bool leap_build_trajectory(long object_index, const real_point3d *point, const real_vector3d *vector,
    const float *unused_speed, real_vector3d *aim_output, float *speed_output, short *unused_ticks, float *unused_time);
bool leap_orientation_update(long object_index, s_animation_control_data *);

struct real_matrix4x3;

void project_world_matrix_to_up_vector(const real_vector3d *up_vector, const real_matrix4x3 *world_matrix, real_matrix4x3 *ground_matrix);

extern "C" float g_biped_leap_alignment_threshold;

#endif // MOTOR_BIPED_LEAP_MOTOR_PROGRAM_H
