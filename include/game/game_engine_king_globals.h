#ifndef GAME_GAME_ENGINE_KING_GLOBALS_H
#define GAME_GAME_ENGINE_KING_GLOBALS_H

#include "game/game_engine_area_set_types.h"
#include "game/game_engine_scoring_globals.h"

struct s_king_globals
{
    __forceinline s_king_globals()
    {
        areas.m_area_count = 0;
        areas.m_area_type_flags = 0;
        areas.m_area_flags = 0;
    }
    c_area_set<c_area, 10> areas;
    c_static_array<long, 8> team_inside_ticks;
    c_static_array<long, 8> team_outside_ticks;
    long hill_move_ticks;
    unsigned short players_in_hill;
    unsigned short alignment;
    c_static_array<long, 16> player_hill_ticks;
};

__forceinline s_king_globals &s_game_engine_globals::king()
{
    return *reinterpret_cast<s_king_globals *>(variant_globals);
}

typedef char king_globals_size[(sizeof(s_king_globals) == 0x480) ? 1 : -1];
typedef char king_globals_team_ticks_offset[(offsetof(s_king_globals, team_inside_ticks) == 0x3f8) ? 1 : -1];
typedef char king_globals_players_offset[(offsetof(s_king_globals, players_in_hill) == 0x43c) ? 1 : -1];
typedef char king_globals_player_ticks_offset[(offsetof(s_king_globals, player_hill_ticks) == 0x440) ? 1 : -1];
#endif
