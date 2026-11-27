#ifndef GAME_GAME_ENGINE_ASSAULT_GLOBALS_H
#define GAME_GAME_ENGINE_ASSAULT_GLOBALS_H

#include "game/game_engine_scoring_globals.h"

__forceinline c_area_set<c_area, 3> *s_game_engine_globals::assault_team_areas()
{
    return assault_goal_areas.begin();
}

typedef char assault_goal_areas_offset[(offsetof(s_game_engine_globals, assault_goal_areas) == 0xe18c) ? 1 : -1];
typedef char assault_bomb_spawn_areas_offset[(offsetof(s_game_engine_globals, assault_bomb_spawn_areas) == 0xeca8) ? 1 : -1];
typedef char assault_assault_bomb_exploded_offset[(offsetof(s_game_engine_globals, assault_bomb_exploded) == 0xf7c4) ? 1 : -1];
typedef char assault_defensive_team_offset[(offsetof(s_game_engine_globals, defensive_team) == 0xf7c8) ? 1 : -1];
typedef char assault_assault_helper_flags_offset[(offsetof(s_game_engine_globals, assault_helper_flags) == 0xf7d4) ? 1 : -1];
typedef char assault_bomb_arming_timers_offset[(offsetof(s_game_engine_globals, bomb_arming_timers) == 0xf7d8) ? 1 : -1];
typedef char assault_bomb_reset_timers_offset[(offsetof(s_game_engine_globals, bomb_reset_timers) == 0xf7fc) ? 1 : -1];
typedef char assault_bomb_weapon_flags_offset[(offsetof(s_game_engine_globals, bomb_weapon_flags) == 0xf80e) ? 1 : -1];
typedef char assault_player_that_placed_bombs_offset[(offsetof(s_game_engine_globals, player_that_placed_bombs) == 0xf818) ? 1 : -1];
typedef char assault_timeout_return_second_counter_offset[(offsetof(s_game_engine_globals, timeout_return_second_counter) == 0xf83c) ? 1 : -1];

class c_game_engine_assault_variant;
extern "C" { extern c_game_engine_assault_variant *g_assault_variant; }

#endif
