#ifndef GAME_GAME_ENGINE_DISPLAY_H
#define GAME_GAME_ENGINE_DISPLAY_H

// Functions defined in tu/blamlibxbox/game_engine_display.cpp that other units call.

void game_engine_render_ui(long user_index);
bool game_engine_hud_get_state_message(long user_index, wchar_t *buffer, long buffer_size, bool flag);
extern "C" void game_engine_get_time_string(wchar_t *buffer);

template<long k_maximum_count> class c_static_wchar_string;

void game_engine_get_place_string(long place_index, c_static_wchar_string<256> *string);
extern "C" void game_engine_get_score_string(long score, c_static_wchar_string<256> *string);

void game_engine_get_team_name(long team_index, c_static_wchar_string<256> *name);

#endif // GAME_GAME_ENGINE_DISPLAY_H
