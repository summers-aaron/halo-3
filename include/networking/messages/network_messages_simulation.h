#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_H

// The simulation messages' payloads, and their encoders and decoders defined in
// tu/networkinglibxbox/network_messages_simulation.cpp.

#include "game/player_types.h"

class c_bitstream;

struct s_network_message_view_establishment
{
    long establishment_mode;
    long establishment_identifier;
    bool has_data;
    long data_size;
    unsigned char data[1];
};

class c_network_message_view_establishment
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

struct s_network_message_player_acknowledge
{
    unsigned long valid_mask;
    unsigned long in_game_mask;
    s_player_identifier identifiers[16];
};
typedef char s_network_message_player_acknowledge_size_check[(sizeof(s_network_message_player_acknowledge) == 0x88) ? 1 : -1];

class c_network_message_player_acknowledge
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_SIMULATION_H
