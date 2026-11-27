#ifndef SIMULATION_SIMULATION_DISTRIBUTED_WORLD_TYPES_H
#define SIMULATION_SIMULATION_DISTRIBUTED_WORLD_TYPES_H

#include <stddef.h>
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_event_manager.h"
#include "networking/simulation/simulation_entity_database_operations.h"
#include "simulation/simulation_event_handler_types.h"

class c_simulation_distributed_world
{
public:
    c_replication_entity_manager m_replication_entity_manager;
    c_replication_event_manager m_replication_event_manager;
    c_simulation_entity_database m_entity_database;
    c_simulation_event_handler event_handler;
};
typedef char distributed_world_event_handler_offset[(sizeof(void *) != 4 || offsetof(c_simulation_distributed_world, event_handler) == 0xb0a8) ? 1 : -1];

#endif
