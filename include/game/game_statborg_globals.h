#ifndef GAME_GAME_STATBORG_GLOBALS_H
#define GAME_GAME_STATBORG_GLOBALS_H

struct s_statborg_mapping;
struct s_game_engine_score_cache;

// Simulation update payload: per-player and per-team stat deltas plus lock flags.
struct s_statborg_update_stats { short values[11]; unsigned char flag; char pad23; };
struct s_statborg_update { s_statborg_update_stats players[16]; s_statborg_update_stats teams[8]; unsigned char flag; };

extern "C" {
extern const s_statborg_mapping k_statborg_stat_mappings[14];
extern s_game_engine_score_cache g_game_engine_score_cache[2];
}

#endif // GAME_GAME_STATBORG_GLOBALS_H
