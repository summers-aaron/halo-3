#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_EVENTS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_EVENTS_H

#include "simulation/simulation_queue_event_types.h"

// Functions defined in tu/networkinglibxbox/simulation_game_events.cpp.

void simulation_event_build_entity_reference_indices(long count, const long *references, long *indices, long index_count);
void simulation_event_generate(e_simulation_event_type event_type, long reference_count, const long *reference_indices, long player_index,
	long payload_size, const void *payload, long time);
void simulation_event_generate_for_simulation_queue(e_simulation_event_type type, long count, const long *references, long size, void *payload);
void simulation_event_generate_exclusive(e_simulation_event_type event_type, long reference_count, const long *reference_indices, long player_index,
	long payload_size, const void *payload, long time);
void simulation_event_generate_exclusive_list(e_simulation_event_type event_type, long reference_count, const long *reference_indices, long *player_list,
	long player_count, long payload_size, const void *payload, long time);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_EVENTS_H
