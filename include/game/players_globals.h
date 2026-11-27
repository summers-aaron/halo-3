#include "physics/havok_memory_allocator_lock.h"
#include "game/player_control_deterministic_globals.h"
#ifndef GAME_PLAYERS_GLOBALS_H
#define GAME_PLAYERS_GLOBALS_H
#include "game/player_control_globals.h"

#include "game/players_globals_types.h"

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;

void __tls_set_g_player_data_allocator(void *address);
void __tls_set_g_players_globals_allocator(void *address);
void __tls_set_g_player_mapping_globals_allocator(void *address);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_data_allocator> g_player_data_allocator;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_players_globals_allocator> g_players_globals_allocator;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_mapping_globals_allocator> g_player_mapping_globals_allocator;

#include "game/players_global_data.h"

extern "C" {
extern const real_vector3d k_player_fix_position_search_directions[9];
}

static __forceinline s_players_globals *tls_players_globals()
{
    return players_globals;
}

#endif // GAME_PLAYERS_GLOBALS_H
