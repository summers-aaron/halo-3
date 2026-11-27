#ifndef MOTOR_MOTOR_SYSTEM_BIPED_H
#define MOTOR_MOTOR_SYSTEM_BIPED_H

// Functions defined in tu/objectslibxbox/motor_system_biped.cpp.

union real_vector3d;
struct s_character_physics_move_output_datum;
struct s_character_physics_update_input_datum;
struct s_character_physics_update_output_datum;

void biped_set_forward_vector(long biped_index, const real_vector3d *input_vector);
void motor_system_biped_physics_postupdate_moving(long biped_index, s_character_physics_update_input_datum *input,
    s_character_physics_update_output_datum *output, bool *crouch_flag);
bool motor_system_biped_move(long object_index, bool b1, bool b2, bool b3, bool b4, s_character_physics_move_output_datum *output);
void biped_falling_damage(long object_index);
bool motor_system_biped_physics_preupdate_moving(long object_index, s_character_physics_update_input_datum *input,
    s_character_physics_update_output_datum *output, bool b0, bool b1, float scale, bool b2, bool b3);
bool biped_can_melee_target_unit(long biped_index, long target_index);
void biped_update_desired_facing(long biped_index, bool use_up, short *facing_direction_ref, real_vector3d *movement_vector_ref, bool *snap_facing_ref);
float biped_calculate_crouch_delta(long biped_index);
void biped_physics_update_crouch(long biped_index, s_character_physics_update_output_datum *output, bool *crouching);
void biped_update_pathfinding_cache(long object_index, s_character_physics_move_output_datum *output);

extern "C" {
extern unsigned int g_biped_forward_threshold_initialization_flags;
extern float g_biped_forward_threshold_cosine;
extern float g_biped_forward_sticky_threshold_cosine;
}

#endif // MOTOR_MOTOR_SYSTEM_BIPED_H
