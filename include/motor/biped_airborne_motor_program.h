#ifndef MOTOR_BIPED_AIRBORNE_MOTOR_PROGRAM_H
#define MOTOR_BIPED_AIRBORNE_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/biped_airborne_motor_program.cpp.

struct s_animation_control_data;
struct s_character_physics_update_input_datum;
struct s_character_physics_update_output_datum;

extern "C" void biped_airborne_reset_falling_cooldown(long object_index);
void biped_falling_danger(long object_index);
void biped_airborne_update_moving(long object_index, s_animation_control_data *animation_control_data,
    s_character_physics_update_input_datum *input, s_character_physics_update_output_datum *output);

#endif // MOTOR_BIPED_AIRBORNE_MOTOR_PROGRAM_H
