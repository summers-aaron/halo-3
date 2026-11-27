#ifndef MOTOR_GIANT_SCARAB_MOTOR_PROGRAM_H
#define MOTOR_GIANT_SCARAB_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/giant_scarab_motor_program.cpp.

struct s_animation_control_data;
struct s_giant_locomotion_cycle;

bool giant_update_movement_speed(long object_index);
bool giant_build_locomotion_cycle(long object_index, s_giant_locomotion_cycle *cycle);
bool giant_update_buckling_state(long object_index, s_animation_control_data *control_data);
bool giant_update_animation_desires(long object_index, s_animation_control_data *control_data);

#endif // MOTOR_GIANT_SCARAB_MOTOR_PROGRAM_H
