#ifndef SIMULATION_SIMULATION_GAME_POSITION_UPDATE_TYPES_H
#define SIMULATION_SIMULATION_GAME_POSITION_UPDATE_TYPES_H

enum e_simulation_game_position_update_context
{
    _simulation_game_position_update_context_authority = 0,
    _simulation_game_position_update_context_predicted,
    _simulation_game_position_update_context_client_prediction,

    k_simulation_game_position_update_context_count
};

enum e_simulation_game_position_update_result
{
    _simulation_game_position_update_result_none = 0,
    _simulation_game_position_update_result_warp,
    _simulation_game_position_update_result_warp_position,
    _simulation_game_position_update_result_adjust_velocity,

    k_simulation_game_position_update_result_count
};

#endif // SIMULATION_SIMULATION_GAME_POSITION_UPDATE_TYPES_H
