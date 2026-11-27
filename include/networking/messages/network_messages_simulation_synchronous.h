#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_SYNCHRONOUS_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_SYNCHRONOUS_H

// Functions defined in tu/networkinglibxbox/network_messages_simulation_synchronous.cpp.

#include "simulation/simulation_update_types.h"
#include "simulation/simulation_update_metadata.h"

class c_network_message_type_collection;

void network_message_types_register_simulation_synchronous(class c_network_message_type_collection * message_collection);

class c_bitstream;
struct s_network_message_simulation_synchronous_entry;
struct s_network_message_simulation_synchronous_payload;
struct s_network_message_synchronous_update
{
    simulation_update update;
    s_simulation_update_metadata metadata;
};

extern "C" void network_message_synchronous_encode_actions(c_bitstream *stream, long message_storage_size, const void *message_data);
extern "C" bool network_message_synchronous_decode_actions(c_bitstream *stream, long message_storage_size, void *message_storage);
extern "C" void network_message_synchronous_encode_update_ack(c_bitstream *stream, long message_storage_size,
    const s_network_message_simulation_synchronous_entry *message);
extern "C" bool network_message_synchronous_decode_update_ack(c_bitstream *stream, long message_storage_size,
    s_network_message_simulation_synchronous_entry *message);
extern "C" void network_message_synchronous_encode_update_received(c_bitstream *stream, long message_storage_size, const long *value);
extern "C" int network_message_synchronous_decode_update_received(c_bitstream *stream, long message_storage_size, long *value);
extern "C" void network_message_synchronous_encode_gamestate(c_bitstream *stream, long message_storage_size,
    const s_network_message_simulation_synchronous_payload *message);
extern "C" void network_message_synchronous_encode_update(c_bitstream *stream, long message_storage_size,
    s_network_message_synchronous_update *message);
extern "C" bool network_message_synchronous_decode_update(c_bitstream *stream, long message_storage_size,
    s_network_message_synchronous_update *message);
extern "C" bool network_message_synchronous_decode_game_results(c_bitstream *stream, long message_storage_size, void *message_storage);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_SYNCHRONOUS_H
