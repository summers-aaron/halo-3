#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H

struct s_object_header;
struct s_data_array;

// Retain this shared legacy C++ TLS linkage until its other users migrate together.
#include "objects/object_header_data.h"
#include "game/player_data.h"
extern "C" {
extern long g_simulation_sandbox_engine_globals_next_available_update_bit;
}

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H
