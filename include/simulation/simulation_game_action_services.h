#ifndef SIMULATION_SIMULATION_GAME_ACTION_SERVICES_H
#define SIMULATION_SIMULATION_GAME_ACTION_SERVICES_H

extern "C++" {
#include "simulation/game_interface/simulation_game_action.h"

extern "C" unsigned char simulation_object_get_interpolation_data(long object_index, s_simulation_object_interpolation_data *out);
extern "C" bool player_is_on_local_machine(long player_index);
extern "C" void simulation_action_object_latch_update(long object_index);
extern "C" void simulation_object_set_interpolation_data(long object_index, const s_simulation_object_interpolation_data *interpolation_data);
}

#endif // SIMULATION_SIMULATION_GAME_ACTION_SERVICES_H
