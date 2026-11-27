#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITIES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITIES_H

// Functions defined in tu/networkinglibxbox/simulation_game_entities.cpp that other units call.

void simulation_entity_update(long entity_index, long unused, unsigned long update_mask);
void simulation_entity_latch_update(long entity_index, long unused, unsigned long update_mask);
long simulation_entity_get_gamestate_index(long entity_index);
extern "C" void simulation_entity_delete(long entity_index, long object_index, long gamestate_index);
extern "C" bool simulation_entity_is_object_entity(long entity_index);

enum e_simulation_entity_type;
e_simulation_entity_type simulation_entity_type_from_object_creation(long definition_index, long object_index, bool dynamic);
long simulation_entity_create(e_simulation_entity_type type, long object_index, long gamestate_index);

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITIES_H
