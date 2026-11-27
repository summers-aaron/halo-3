#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H

struct s_object_header;
struct s_data_array;

// Retain this shared legacy C++ TLS linkage until its other users migrate together.
#include "objects/object_header_data.h"
#include "game/player_data.h"
extern "C" {
void simulation_game_dispose_first_static_definition(void);
void simulation_game_dispose_second_static_definition(void);
void simulation_game_dispose_third_static_definition(void);
void simulation_game_dispose_fourth_static_definition(void);
void simulation_ctf_definition_vtable_entry(void);
void simulation_sandbox_engine_globals_instance_dispose(void);
extern long g_simulation_sandbox_engine_globals_next_available_update_bit;
}
#if defined(__clang__)
// PC: the object refresh event (type 32) is a real object of its class (abstract base on the PC), simulation_game_objects.cpp
#include "simulation/game_interface/simulation_game_objects.h"
extern "C" c_simulation_object_refresh_event_definition g_simulation_registered_event_definition_32;
#endif

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_INTERFACE_GLOBALS_H
