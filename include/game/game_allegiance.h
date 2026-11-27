#ifndef GAME_GAME_ALLEGIANCE_H
#define GAME_GAME_ALLEGIANCE_H

#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/game_allegiance.cpp that other units call.

struct game_allegiance;

void game_allegiance_broken(struct game_allegiance * _sym_p0, bool broken, bool force, bool notify);
void __tls_set_g_game_allegiance_globals_allocator(void *value);
extern "C" long game_allegiance_get_player_team(long player_index);
extern "C" void game_allegiance_update_allied_teams(long team_a, long team_b, bool broken, bool permanently);
bool game_team_is_friendly(e_game_team team, e_game_team other_team);
bool game_team_is_enemy(e_game_team team_a, e_game_team team_b);
bool game_team_is_ally(e_game_team a, e_game_team b);
bool game_team_is_traitor(e_game_team team_a, e_game_team team_b);
bool game_allegiance_remove(e_game_team team, e_game_team other_team);
bool game_allegiance_incident(e_game_team team, e_game_team other_team, short incident);
void game_allegiance_provoke(e_game_team team, e_game_team other_team);
void game_allegiance_create(e_game_team team, bool reverse, e_game_team other_team, bool forward, short threshold, short delay);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3,
	static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_game_allegiance_globals_allocator> g_game_allegiance_globals_allocator;

typedef t_restricted_allocation_manager<3,
	static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_game_allegiance_globals_allocator> t_game_allegiance_allocation_manager;

void game_allegiance_initialize();
void game_allegiance_dispose();
void game_allegiance_initialize_for_new_map();
void game_allegiance_dispose_from_old_map();

#endif // GAME_GAME_ALLEGIANCE_H
