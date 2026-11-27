#ifndef NETWORKING_NETWORK_MEMORY_BASE_TYPES_H
#define NETWORKING_NETWORK_MEMORY_BASE_TYPES_H
#include "networking/delivery/network_link.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/messages/network_message_handler.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_session_parameter_type_collection.h"
#include "networking/session/network_session.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_watcher_types.h"
#include "networking/simulation/simulation_type_collection.h"

struct s_network_base_memory_globals
{
    s_network_base_memory_globals();
    ~s_network_base_memory_globals();

    c_network_link link;
    c_network_message_type_collection message_types;
    c_network_message_gateway gateway;
    c_network_message_handler handler;
    c_network_observer observer;
    c_network_session_manager session_manager;
    c_network_session sessions[3];
    c_network_session_parameter_type_collection session_parameter_types;
    c_simulation_world world;
    c_simulation_watcher watcher;
    c_simulation_type_collection types;
};

#endif
