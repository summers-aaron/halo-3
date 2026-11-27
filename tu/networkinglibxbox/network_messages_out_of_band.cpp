// networkinglibxbox/network_messages_out_of_band.obj: from the records 0x3eed60 0x3eeda8 0x3eee48 0x3eee98 0x3eef58 0x3eef98 0x3ef028 0x3ef070 0x3ef138
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "networking/messages/network_messages_out_of_band.h"
#include "networking/network_game_definitions.h"
#include "networking/messages/network_message_type_collection.h"
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z

struct s_network_squad_status_data;

#include "networking/messages/network_message_broadcast_reply.h"
#include "networking/network_globals.h"
typedef char network_messages_out_of_band_reply_status_offset[(offsetof(s_network_message_broadcast_reply, status) == 16) ? 1 : -1];

class c_network_message_broadcast_reply
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

void c_network_message_ping::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_ping *message = (const s_network_message_ping *)message_storage;
    packet->write_integer("id", message->id, 16);
    packet->write_integer("timestamp", message->timestamp, 32);
    packet->write_bool("request_qos", message->request_qos);
}

bool c_network_message_ping::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    s_network_message_ping *message = (s_network_message_ping *)message_storage;
    message->id = (unsigned short)packet->read_integer("id", 16);
    message->timestamp = packet->read_integer("timestamp", 32);
    message->request_qos = packet->read_bool("request_qos");
    bool success = !packet->error_occurred();
    return success;
}

void c_network_message_pong::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_pong *message = (const s_network_message_pong *)message_storage;
    packet->write_integer("id", message->id, 16);
    packet->write_integer("timestamp", message->timestamp, 32);
    packet->write_integer("qos_response", message->qos_response, 2);
}

bool c_network_message_pong::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    s_network_message_pong *message = (s_network_message_pong *)message_storage;
    message->id = (unsigned short)packet->read_integer("id", 16);
    message->timestamp = packet->read_integer("timestamp", 32);
    message->qos_response = packet->read_integer("qos_response", 2);
    bool success = !packet->error_occurred();
    success = success && message->qos_response >= 0 && message->qos_response < 3;
    return success;
}

void c_network_message_broadcast_search::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_broadcast_search *message = (const s_network_message_broadcast_search *)message_storage;
    packet->write_integer("protocol", message->protocol, 16);
    packet->write_raw_data("nonce", &message->search_nonce, 64);
}

bool c_network_message_broadcast_search::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    s_network_message_broadcast_search *message = (s_network_message_broadcast_search *)message_storage;
    message->protocol = (unsigned short)packet->read_integer("protocol", 16);
    packet->read_raw_data("nonce", &message->search_nonce, 64);
    bool success = !packet->error_occurred();
    return success;
}

// @address 0x3ef028 ?encode@c_network_message_broadcast_reply@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_broadcast_reply::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_broadcast_reply *message = (const s_network_message_broadcast_reply *)message_storage;
    packet->write_integer("protocol", message->protocol, 16);
    packet->write_raw_data("search-nonce", &message->search_nonce, 64);
    squad_status_encode(packet, (const s_network_squad_status_data *)&message->status);
}

bool c_network_message_broadcast_reply::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    s_network_message_broadcast_reply *message = (s_network_message_broadcast_reply *)message_storage;
    bool success = true;
    message->protocol = (unsigned short)packet->read_integer("protocol", 16);
    packet->read_raw_data("search-nonce", &message->search_nonce, 64);
    success = success && squad_status_decode(packet, (s_network_squad_status_data *)&message->status);
    success = success && !packet->error_occurred();
    return success;
}

inline void c_network_message_type_collection::register_message_type(
        e_network_message_type message_type,
        const char *name,
        long flags,
        long minimum_size,
        long maximum_size,
        void (*encode)(c_bitstream *, long, const void *),
        bool (*decode)(c_bitstream *, long, void *),
        bool (*compare)(long, void *, void *),
        void (*dispose)(long, void *))
    {
        s_network_message_type_definition *definition = &m_message_types[message_type];
        if (message_type == _network_message_type_ping)
            m_message_types[0].name = name;
        else
            definition->name = name;
        definition->flags = flags;
        definition->minimum_size = minimum_size;
        definition->maximum_size = maximum_size;
        definition->encode = encode;
        definition->decode = decode;
        definition->compare = compare;
        definition->dispose = dispose;
        definition->initialized = true;
    }

// @address 0x8c4450 g_network_message_type_collection

// @address 0x3ef138 ?network_message_types_register_out_of_band@@YAXPAVc_network_message_type_collection@@@Z
void network_message_types_register_out_of_band(c_network_message_type_collection *message_collection)
{
    message_collection = g_network_message_type_collection;
    g_network_message_type_collection->register_message_type(_network_message_type_ping, "ping", 0,
        sizeof(s_network_message_ping), sizeof(s_network_message_ping),
        c_network_message_ping::encode, c_network_message_ping::decode, 0, 0);
    message_collection->register_message_type(_network_message_type_pong, "pong", 0, sizeof(s_network_message_pong), sizeof(s_network_message_pong),
        c_network_message_pong::encode, c_network_message_pong::decode, 0, 0);
    message_collection->register_message_type(_network_message_type_broadcast_search, "broadcast-search", 0,
        sizeof(s_network_message_broadcast_search), sizeof(s_network_message_broadcast_search),
        c_network_message_broadcast_search::encode, c_network_message_broadcast_search::decode, 0, 0);
    message_collection->register_message_type(_network_message_type_broadcast_reply, "broadcast-reply", 0,
        sizeof(s_network_message_broadcast_reply), sizeof(s_network_message_broadcast_reply),
        c_network_message_broadcast_reply::encode, c_network_message_broadcast_reply::decode, 0, 0);
}
