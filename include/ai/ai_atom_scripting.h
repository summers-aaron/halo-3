#ifndef AI_AI_ATOM_SCRIPTING_H
#define AI_AI_ATOM_SCRIPTING_H

// Functions defined in tu/ailibxbox/ai_atom_scripting.cpp.

class c_ai_direction;

void cs_approach(long actor_index, long script_index, bool active, long target_index, float distance, float stop_distance, float max_distance);
void cs_custom_animation(long actor_index, long command_index, bool active, const c_ai_direction *direction, short animation_index);
void cs_look_player(long actor_index, long script_index, bool looking);
void cs_aim(long actor_index, long script_index, bool aiming, long target_index);
void cs_aim_player(long actor_index, long script_index, bool aiming);
void cs_aim_object(long actor_index, long script_index, bool aiming, long target_index);
void cs_face_object(long actor_index, long script_index, bool facing, long target_index);
void cs_face_player(long actor_index, long script_index, bool facing);
void cs_shoot_at_target2(long actor_index, long script_index, bool shooting, long target_index);
void cs_shoot_at_point(long actor_index, long script_index, bool shooting, long target_index);
extern "C" void cs_custom_animation_point(long actor_index, long command_index, bool active, long point_ref, short animation_index);
void cs_vehicle_speed_instantaneous(long actor_index, long script_index, float speed);
void cs_posture_exit(long actor_index, long script_index);
void cs_teleport(long actor_index, long script_index, long from_point_ref, long facing_point_ref);
void cs_stop_custom_animation(long actor_index, long script_index);
void cs_melee_direction(long actor_index, long script_index, bool blocking, long direction);
void cs_player_melee(long actor_index, long script_index, long type);
void cs_smash_direction(long actor_index, long script_index, bool blocking, long direction);
void cs_move_in_direction(long script_index, bool blocking, float angle, float parameter, float direction);
void cs_start_custom_animation_internal(long actor_index, long script_index, bool blocking, bool no_command,
	long animation, long variant, bool interpolate, long point_ref, bool b30);
float ai_scripting_play_sound_internal(long actor_index, long script_index, bool blocking,
	long sound_index, long playback_label, float duration_scale, float sound_scale);
void cs_posture_set(long actor_index, long script_index, long posture, bool enter);
extern "C" void cs_pause(long actor_index, bool active, float seconds);
extern "C" void cs_face_point(long actor_index, long script_index, bool facing, long target_index);
extern "C" void cs_custom_animation_player(long actor_index, long command_index, bool active, short animation_index);
long cs_find_player_unit(long actor_index);
extern "C" bool cs_running_atom_internal(long ai_index, short atom_mask);
extern "C" void cs_go_to_nearest(long actor_index, long command_index, bool active, long point_set_id);

#endif // AI_AI_ATOM_SCRIPTING_H
