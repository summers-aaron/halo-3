#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_GATEWAY_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_GATEWAY_H

#include "memory/bitstream.h"
#include "networking/delivery/network_link.h"
#include "networking/messages/network_message_handler.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/transport/transport_address.h"

class c_network_message_gateway : public c_network_out_of_band_consumer
{
public:
    c_network_message_gateway();
    ~c_network_message_gateway();
    virtual bool receive_out_of_band_packet(const transport_address *address, c_bitstream *packet);
    void send_all_pending_messages();
    __forceinline void flush_outgoing_packet()
    {
        if (m_outgoing_packet_pending)
        {
            m_outgoing_packet.write_bool("has_message", false);
            m_outgoing_packet.finish_writing(0);
            m_link->send_out_of_band(&m_outgoing_packet, &m_outgoing_packet_address, 0);
            m_outgoing_packet_pending = false;
        }
    }
    bool send_message_directed(const transport_address *outgoing, e_network_message_type message_type, long message_size, const void *message);

    __forceinline bool send_message_broadcast(e_network_message_type message_type, long message_size, const void *message)
    {
        transport_address address;
        address.address_length = 4;
        address.address.ipv4_address = 0xffffffff;
        address.port = 1001;
        return send_message_directed(&address, message_type, message_size, message);
    }
    __forceinline void send_pending_messages_to_address(const transport_address *address)
    {
        if (m_outgoing_packet_pending && transport_address_equivalent(&m_outgoing_packet_address,
            address, false))
            send_all_pending_messages();
    }

public: // Initialization and disposal are shared with network_globals.cpp.
    bool m_initialized;
    c_network_link *m_link;
    s_network_message_type_definition *m_message_types;
    c_network_message_handler *m_message_handler;
    bool m_outgoing_packet_pending;
    unsigned char m_outgoing_packet_storage[0x600];
    transport_address m_outgoing_packet_address;
    c_bitstream m_outgoing_packet;
};

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_GATEWAY_H
