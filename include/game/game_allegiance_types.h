#ifndef GAME_GAME_ALLEGIANCE_TYPES_H
#define GAME_GAME_ALLEGIANCE_TYPES_H

// The allegiance state game_allegiance.cpp keeps in the "game allegiance globals" restricted-memory member
// held per thread. game_allegiance_create writes every
// field of an entry, game_allegiance_broken reads team/other_team and sets broken/changed, and game_save.cpp's
// game_safe_to_save_internal walks the entries with a 20-byte stride. March names the entry type
// (game_allegiance_broken takes a game_allegiance *).
struct game_allegiance
{
    short team;
    short other_team;
    short threshold;
    short delay;
    bool reverse;
    bool forward;
    bool broken;
    bool changed;
    short incidents;
    short timer;
    long time;
};

// relation_bits and enemy_bits hold one bit per (team, other team) pair of the 16 teams.
struct s_game_allegiance_globals
{
    short count;
    game_allegiance allegiances[16];
    unsigned long relation_bits[8];
    unsigned long enemy_bits[8];
};

typedef char game_allegiance_size[(sizeof(game_allegiance) == 20) ? 1 : -1];
typedef char game_allegiance_globals_size[(sizeof(s_game_allegiance_globals) == 0x184) ? 1 : -1];

extern "C" {
extern __declspec(thread) s_game_allegiance_globals *g_game_allegiance_globals;   // game allegiance globals
}

// By value, like the accessors in memory/tls_globals.h: a reference makes the compiler reload the slot after
// every store through the pointer (game_allegiance_remove).
static __forceinline s_game_allegiance_globals *tls_game_allegiance_globals() { return g_game_allegiance_globals; }

static __forceinline void tls_game_allegiance_globals_set(s_game_allegiance_globals *value) { g_game_allegiance_globals = value; }

#endif // GAME_GAME_ALLEGIANCE_TYPES_H
