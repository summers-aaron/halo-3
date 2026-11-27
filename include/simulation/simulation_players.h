#ifndef SIMULATION_SIMULATION_PLAYERS_H
#define SIMULATION_SIMULATION_PLAYERS_H

#include "game/player_types.h"
#include <stddef.h>

// Each player slot records the
// identifier, leave time, machine/user indices and the desired configuration.
struct s_player_entry
{
    s_player_identifier identifier;
    bool left_game;
    long left_game_time;
    s_machine_identifier machine_identifier;
    long machine_index;
    long machine_user_index;
    s_player_configuration configuration;
};

struct s_player_collection
{
    unsigned long valid_mask;
    long unused;
    s_player_entry players[16];
};

typedef char simulation_player_entry_size[(sizeof(s_player_entry) == 0x130) ? 1 : -1];
typedef char simulation_player_entry_configuration_offset[(offsetof(s_player_entry, configuration) == 0x20) ? 1 : -1];
typedef char simulation_player_collection_size[(sizeof(s_player_collection) == 0x1308) ? 1 : -1];
typedef char simulation_player_collection_players_offset[(offsetof(s_player_collection, players) == 8) ? 1 : -1];

// Player update exchanged by watcher generation, queueing and encoding.
struct simulation_player_update
{
    long player_index;
    s_player_identifier player_identifier;
    long update_type;
    s_machine_identifier machine_identifier;
    long machine_user_index;
    long machine_controller_index;
    bool player_left_game;
    bool player_joined_in_progress;
    s_player_configuration player_data;
    long swap_player_index;
    s_player_identifier swap_player_identifier;
};

typedef char simulation_player_update_size[(sizeof(simulation_player_update) == 0x148) ? 1 : -1];
typedef char simulation_player_update_configuration_offset[(offsetof(simulation_player_update, player_data) == 0x28) ? 1 : -1];
typedef char simulation_player_update_swap_offset[(offsetof(simulation_player_update, swap_player_index) == 0x138) ? 1 : -1];

// Functions defined in tu/networkinglibxbox/simulation_players.cpp that other units call.

void simulation_player_collection_clear(s_player_collection *collection);
unsigned long simulation_player_collection_get_in_game_mask(const s_player_collection *collection);
void simulation_player_collection_apply_update(s_player_collection *collection, const simulation_player_update *update);
bool simulation_player_update_apply_remove(const simulation_player_update *update);
bool simulation_player_update_apply_added(const simulation_player_update *update);
bool simulation_player_update_apply_configuration(const simulation_player_update *update);

extern "C" void simulation_player_collection_build(s_player_collection *collection);
extern "C" bool simulation_player_update_apply_left_game(const simulation_player_update *update);
extern "C" bool simulation_player_update_apply_swap(const simulation_player_update *update);

#endif // SIMULATION_SIMULATION_PLAYERS_H
