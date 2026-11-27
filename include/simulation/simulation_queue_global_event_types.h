#ifndef SIMULATION_SIMULATION_QUEUE_GLOBAL_EVENT_TYPES_H
#define SIMULATION_SIMULATION_QUEUE_GLOBAL_EVENT_TYPES_H

enum e_simulation_queue_game_global_event_type
{
    _simulation_queue_game_global_event_none = -1,

    _simulation_queue_game_global_event_claim_authority = 0,
    _simulation_queue_game_global_event_set_simulation_to_distributed_server,
    _simulation_queue_game_global_event_main_game_won,
    _simulation_queue_game_global_event_main_revert_map,
    _simulation_queue_game_global_event_main_save_map,
    _simulation_queue_game_global_event_main_game_lost,
    _simulation_queue_game_global_event_main_reset_map_complete,
    _simulation_queue_game_global_event_main_reset_map_begin,
    _simulation_queue_game_global_event_players_update_for_checkpoint,

    k_simulation_queue_game_global_event_type_count
};

#endif // SIMULATION_SIMULATION_QUEUE_GLOBAL_EVENT_TYPES_H
