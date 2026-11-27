#ifndef GAME_GAME_ENGINE_SIMULATION_H
#define GAME_GAME_ENGINE_SIMULATION_H

// Functions defined in tu/blamlibxbox/game_engine_simulation.cpp.

struct s_game_engine_event_data;

long game_engine_globals_get_gamestate_index(void);
long game_engine_globals_get_statborg_gamestate_index(void);
long game_engine_globals_get_player_gamestate_index(short player_index);
void game_engine_promote_to_simulation_authority(void);
void game_engine_events_update(void);
void notify_audience_member_of_event(struct s_game_engine_event_data const * event, long * list, long max_count,
	struct s_game_engine_event_data const * _sym_p3);
extern "C" void game_engine_handle_event(const s_game_engine_event_data *event, bool queue);
bool game_engine_globals_apply_simulation_update(unsigned long update_mask, long state_size, const void *state_data);

struct s_multiplayer_event_response_definition;

s_multiplayer_event_response_definition *audience_member_find_response(long player_index, const s_game_engine_event_data *event_data);
s_multiplayer_event_response_definition *find_event_response(long player_index, const s_game_engine_event_data *event_data, long audience);
extern "C" void player_submit_event_response(long player_index, const s_game_engine_event_data *event_data, s_multiplayer_event_response_definition *response);
extern "C" void game_engine_simulation_play_event_sound(long player, const s_game_engine_event_data *event, s_multiplayer_event_response_definition *response);

enum e_simulation_entity_type;
e_simulation_entity_type game_engine_globals_get_simulation_entity_type(void);

#endif // GAME_GAME_ENGINE_SIMULATION_H
