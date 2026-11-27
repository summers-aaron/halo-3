#ifndef GAME_GAME_SIMULATION_TYPES_H
#define GAME_GAME_SIMULATION_TYPES_H

enum e_game_simulation_type
{
    _game_simulation_none = 0,
    _game_simulation_local = 1,
    _game_simulation_synchronous_client = 2,
    _game_simulation_synchronous_server = 3,
    _game_simulation_distributed_client = 4,
    _game_simulation_distributed_server = 5
};

#endif // GAME_GAME_SIMULATION_TYPES_H
