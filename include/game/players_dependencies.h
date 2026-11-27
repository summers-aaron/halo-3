#ifndef GAME_PLAYERS_DEPENDENCIES_H
#define GAME_PLAYERS_DEPENDENCIES_H
#include <new>
#include "cseries/data_mining_event_types.h"
#include "game/player_training_tls.h"
#include "memory/tls_globals.h"

static __forceinline void data_mining_event_initialize(s_event *event, int level, const char *format, int count, const char *value)
{
    new (event) c_datamine(level, format, count, value);
}
extern "C" unsigned long g_active_designer_zone_mask;
extern "C" unsigned int g_active_cinematic_zone_mask;


extern "C" bool unit_animation_is_running_damage_overlay(unsigned int unit_index);

struct ai_player_state;
struct s_player_training_entry_data;
#include "main/main_game.h"
#include "saved_games/saved_film_globals.h"
#include "camera/saved_film_director_storage.h"


typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_data_allocator> t_player_data_allocation;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_players_globals_allocator> t_players_globals_allocation;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_mapping_globals_allocator> t_player_mapping_globals_allocation;

#endif // GAME_PLAYERS_DEPENDENCIES_H
