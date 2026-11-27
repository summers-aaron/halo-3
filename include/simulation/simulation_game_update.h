#ifndef SIMULATION_SIMULATION_GAME_UPDATE_H
#define SIMULATION_SIMULATION_GAME_UPDATE_H

struct simulation_update;
struct s_simulation_update_metadata;

// match: game.cpp retains the long argument used by its accepted C-linkage call.
extern "C" void simulation_build_game_update(long, simulation_update *, s_simulation_update_metadata *);

#endif // SIMULATION_SIMULATION_GAME_UPDATE_H
