#ifndef GAME_GAME_ENGINE_CTF_EXTERNS_H
#define GAME_GAME_ENGINE_CTF_EXTERNS_H

template<long k_maximum_count> class c_static_wchar_string;

extern "C" int game_engine_get_team_score(int team_index);
extern "C" bool string_list_get_normal_string(long string_list_index, long string_id, c_static_wchar_string<256> *text);

#endif // GAME_GAME_ENGINE_CTF_EXTERNS_H
