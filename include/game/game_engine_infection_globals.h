#ifndef GAME_GAME_ENGINE_INFECTION_GLOBALS_H
#define GAME_GAME_ENGINE_INFECTION_GLOBALS_H

class c_game_engine_infection_variant;
extern "C" c_game_engine_infection_variant *g_infection_variant;

#include "game/game_engine_area_set_types.h"
#include <stddef.h>

struct s_infection_bitvector {
    unsigned short bits;
    __forceinline bool test(long bit) const { return (bits & (1 << bit))!=0; }
    __forceinline void set(long bit,bool value) { if(value) bits |= 1 << bit; else bits &= ~(1 << bit); }
};

struct game_engine_infection_state
{
    union { unsigned short zombies; s_infection_bitvector zombie_flags; };
    union { unsigned short starting_zombies; s_infection_bitvector original_zombie_flags; };
    long haven_timer;
    union { unsigned short zombie_teams; s_infection_bitvector zombie_team_flags; };
    union { unsigned short haven_players; s_infection_bitvector human_flags; };
    c_area_set<c_area, 12> safe_havens;
    long infection_kills[16];
    long zombie_kills[16];
    bool last_human_bonus_awarded;
};
struct game_engine_infection_engine_globals
{
    char engine_state[0xe18c];
    game_engine_infection_state infection;
};

typedef char infection_state_havens_offset[(offsetof(game_engine_infection_state, safe_havens) == 0xc) ? 1 : -1];
typedef char infection_state_kills_offset[(offsetof(game_engine_infection_state, infection_kills) == 0x4cc) ? 1 : -1];
typedef char infection_globals_state_offset[(offsetof(game_engine_infection_engine_globals, infection) == 0xe18c) ? 1 : -1];

#endif
