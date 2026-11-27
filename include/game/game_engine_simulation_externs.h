#ifndef GAME_GAME_ENGINE_SIMULATION_EXTERNS_H
#define GAME_GAME_ENGINE_SIMULATION_EXTERNS_H

class c_game_variant;
struct s_game_engine_simulation_wchar_string;

extern "C" void game_engine_player_set_fade_state(long player_index, signed char state);
extern "C" c_game_variant *game_engine_get_current_variant(void);
extern "C" bool string_list_get_normal_string_by_language(long tag_index, long string_id, long language, s_game_engine_simulation_wchar_string *string);

#endif // GAME_GAME_ENGINE_SIMULATION_EXTERNS_H
