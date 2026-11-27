#ifndef MOTOR_BIPED_FLYING_MOTOR_PROGRAM_H
#define MOTOR_BIPED_FLYING_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/biped_flying_motor_program.cpp.

struct s_animation_control_data;
struct s_character_physics_update_input_datum;
struct s_character_physics_update_output_datum;

bool biped_update_turning(long object_index, s_animation_control_data *animation_control);
void biped_flying_update_moving(long object_index, s_animation_control_data *animation_control,
    s_character_physics_update_input_datum *input, s_character_physics_update_output_datum *output);

#endif // MOTOR_BIPED_FLYING_MOTOR_PROGRAM_H
