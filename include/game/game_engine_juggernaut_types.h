#ifndef GAME_GAME_ENGINE_JUGGERNAUT_TYPES_H
#define GAME_GAME_ENGINE_JUGGERNAUT_TYPES_H

#include "game/game_engine_area_set_types.h"
#include "memory/static_arrays.h"
#include "networking/simulation/simulation_game_engine_globals.h"

struct s_juggernaut_spawn_state {
    int player_index;
    unsigned short state_word;
    unsigned char state_byte;
    char padding;
    int state_status;
    c_area_set<c_area, 12> areas;
    c_static_array<long, 16> kills;
};


typedef char juggernaut_spawn_areas_offset[(offsetof(s_juggernaut_spawn_state, areas) == 0xc) ? 1 : -1];
typedef char juggernaut_spawn_size[(sizeof(s_juggernaut_spawn_state) == 0x50c) ? 1 : -1];

#include "game/game_engine_scoring_globals.h"

__forceinline s_juggernaut_spawn_state &s_game_engine_globals::juggernaut()
{
    return *(s_juggernaut_spawn_state *)variant_globals;
}

struct s_juggernaut_simulation_state {
    s_game_engine_state_data base;
    short player_index;
    unsigned char state_word;
    unsigned char state_byte;
};

typedef char juggernaut_simulation_player_offset[(offsetof(s_juggernaut_simulation_state, player_index) == 0x2c) ? 1 : -1];

#endif // GAME_GAME_ENGINE_JUGGERNAUT_TYPES_H
