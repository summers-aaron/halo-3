#ifndef GAME_PLAYER_CONTROL_H
#define GAME_PLAYER_CONTROL_H

// Functions defined in tu/blamlibxbox/player_control.cpp.

struct player_action;

float player_control_get_target_autoaim_level(long player_index);
short player_control_get_zoom_level(long player_index);
void __tls_set_g_player_control_globals_allocator(void *allocator);
void __tls_set_g_player_control_globals_deterministic_allocator(void *allocator);
void player_control_set_external_action(long player_index, const player_action *action);
void player_control_update(float world_seconds_elapsed, float game_seconds_elapsed);
void player_control_modify_desired_angles(long user_index, float yaw_delta, float pitch_delta, float frame_time, float unused);

struct control;

extern "C" bool player_control_can_use_debug_controls(void);
extern "C" bool player_control_is_old_controls_active(void);
extern "C" void player_control_set_user_unit(long input_user_index, long unit_index);
extern "C" float player_control_get_look_pitch(void);
extern "C" void player_control_set_input_scale(float value, float duration);
extern "C" void player_control_set_look_point(long player_index, long point_reference, float angle);
extern "C" void player_control_set_action_from_unit(long unit_index, control *action);
extern "C" void player_control_update_state_actions(long user_index);
float evaluate_piecewise_linear_function(short sample_count, float samples[], float input);
extern "C" void player_control_clear_action(control *action);
extern "C" void player_control_register_globals_tls_callback(void);
extern "C" void player_control_register_deterministic_tls_callback(void);
extern "C" void player_control_update_player(long player_index, long input_user_index, long controller_index,
	float world_seconds_elapsed, float game_seconds_elapsed);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
	&__tls_set_g_player_control_globals_allocator> t_player_control_globals_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
	&__tls_set_g_player_control_globals_deterministic_allocator> t_player_control_globals_deterministic_allocator;
extern t_player_control_globals_allocator g_player_control_globals_allocator;
extern t_player_control_globals_deterministic_allocator g_player_control_globals_deterministic_allocator;
extern "C" {
extern const char k_player_control_globals_allocation_name[];
extern const char k_player_control_deterministic_allocation_name[];
}

void player_control_initialize();
void player_control_initialize_for_new_map();

void player_control_dispose(void);
void player_control_dispose_from_old_map(void);
#endif // GAME_PLAYER_CONTROL_H
