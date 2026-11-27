#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_OUT_OF_BAND_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_OUT_OF_BAND_H

// Functions defined in tu/networkinglibxbox/network_messages_out_of_band.cpp that other units call.

#include <stddef.h>

class c_network_message_type_collection;
class c_bitstream;

struct s_network_message_broadcast_search
{
    unsigned short protocol;
    unsigned __int64 search_nonce;
};

class c_network_message_broadcast_search
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

typedef char network_message_broadcast_search_nonce_offset[(offsetof(s_network_message_broadcast_search, search_nonce) == 8) ? 1 : -1];
typedef char network_message_broadcast_search_size[(sizeof(s_network_message_broadcast_search) == 16) ? 1 : -1];

struct s_network_message_ping
{
    unsigned short id;
    unsigned long timestamp;
    bool request_qos;
};

struct s_network_message_pong
{
    unsigned short id;
    unsigned long timestamp;
    long qos_response;
};

class c_network_message_ping
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

class c_network_message_pong
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

typedef char network_message_ping_size_check[(sizeof(s_network_message_ping) == 12) ? 1 : -1];
typedef char network_message_pong_size_check[(sizeof(s_network_message_pong) == 12) ? 1 : -1];

void network_message_types_register_out_of_band(c_network_message_type_collection *message_collection);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_OUT_OF_BAND_H
