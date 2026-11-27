#ifndef SIMULATION_SIMULATION_UPDATE_TYPES_H
#define SIMULATION_SIMULATION_UPDATE_TYPES_H

#include <stddef.h>
#include "simulation/player_action.h"
#include "units/unit_control.h"
#include "networking/replication/replication_prediction.h"
#include "simulation/simulation_machine_update_types.h"
#include "simulation/simulation_machine_update_entry_types.h"
#include "simulation/simulation_queue.h"
#include "saved_games/saved_film_globals.h"

// Complete update shared by world construction and wire encoding.
struct simulation_update
{
    long update_number;
    short flags;
    unsigned long player_valid_mask;
    unsigned long action_test_flags;
    player_action actions[16];
    unsigned long actor_valid_mask;
    long actor_unit_indices[16];
    unit_control_data actor_controls[16];
    bool machine_update_exists;
    simulation_machine_update machine_update;
    unsigned long prediction_mask;
    s_player_prediction predictions[16];
    long verify_game_time;
    long verify_random;
    unsigned long camera_valid_mask;
    union { s_simulation_camera_update camera; s_film_replay_metadata film_replay_metadata; };
    c_simulation_queue bookkeeping;
    c_simulation_queue simulation;

    // March 824a8240: the c_flags member constructor clears the update flags.
    simulation_update() { flags = 0; }
};

typedef char simulation_update_actions_offset[(offsetof(simulation_update, actions) == 0x10) ? 1 : -1];
typedef char simulation_update_actor_controls_offset[(offsetof(simulation_update, actor_controls) == 0x794) ? 1 : -1];
typedef char simulation_update_machine_offset[(offsetof(simulation_update, machine_update) == 0xf58) ? 1 : -1];
typedef char simulation_update_time_offset[(offsetof(simulation_update, verify_game_time) == 0x1440) ? 1 : -1];
typedef char simulation_update_queues_offset[(offsetof(simulation_update, bookkeeping) == 0x1470) ? 1 : -1];
typedef char simulation_update_size[(sizeof(void *) != 4 || ((sizeof(simulation_update) == 0x14a8))) ? 1 : -1];

#endif
