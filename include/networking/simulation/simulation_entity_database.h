#ifndef NETWORKING_SIMULATION_SIMULATION_ENTITY_DATABASE_H
#define NETWORKING_SIMULATION_SIMULATION_ENTITY_DATABASE_H

// Bungie: simulation\simulation_entity_database.cpp. Field names from the March 2007 asserts there
// ("entity->entity_index==NONE", "entity->exists_in_gameworld", "entity->event_reference_count<CHAR_MAX",
// "entity->creation_data_size==...", "entity->state_data_size==...", "entity->gamestate_index != NONE").

struct s_simulation_view_telemetry_data;

// One slot of the entity database (c_simulation_entity_database::entity_get).
struct s_simulation_entity
{
    long entity_index;
    long entity_type;                       // e_simulation_entity_type
    bool exists_in_gameworld;
    char event_reference_count;
    long gamestate_index;
    unsigned long update_mask;
    unsigned long latched_update_mask;
    long creation_data_size;
    void *creation_data;
    long state_data_size;
    void *state_data;
};

// Built by c_simulation_entity_database::calculate_update_requirements for the entity definition.
struct s_entity_update_data
{
    unsigned long update_mask;
    unsigned long last_update_time;                         // network time
    const s_simulation_view_telemetry_data *telemetry;
};

typedef char simulation_entity_database_size_entity[(sizeof(void *) != 4 || ((sizeof(s_simulation_entity) == 0x28) )) ? 1 : -1];
typedef char simulation_entity_database_size_update[(sizeof(void *) != 4 || ((sizeof(s_entity_update_data) == 0xc) )) ? 1 : -1];

#endif
