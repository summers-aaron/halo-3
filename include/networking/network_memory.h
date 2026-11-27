#ifndef NETWORKING_NETWORK_MEMORY_H
#define NETWORKING_NETWORK_MEMORY_H

// Functions defined in tu/networkinglibxbox/network_memory.cpp.

enum e_map_memory_configuration;
class c_network_channel;
class c_network_link;
class c_network_message_type_collection;
class c_network_message_gateway;
class c_network_message_handler;
class c_network_observer;
class c_network_session;
class c_network_session_manager;
class c_network_session_parameter_type_collection;

extern "C" bool network_memory_initialize(c_network_link **, c_network_message_type_collection **, c_network_message_gateway **,
    c_network_message_handler **, c_network_observer **, c_network_session **, c_network_session_manager **,
    c_network_session_parameter_type_collection **);

c_network_channel *network_channel_iterate(c_network_channel *channel);
void network_memory_base_dispose();
void network_memory_shared_dispose();
extern "C" void network_memory_shared_initialize(e_map_memory_configuration configuration);
void network_heap_free_block(void *block);

struct s_network_base_memory_globals;
struct s_network_shared_memory_globals;

void network_memory_track_physical_allocation(long size, void *address);

// Data defined in tu/networkinglibxbox/network_memory.cpp that other units use.
extern s_network_base_memory_globals network_base_memory_globals;
extern s_network_shared_memory_globals network_shared_memory_globals;

#endif // NETWORKING_NETWORK_MEMORY_H
