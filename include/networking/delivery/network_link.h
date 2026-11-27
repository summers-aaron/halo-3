#ifndef NETWORKING_DELIVERY_NETWORK_LINK_H
#define NETWORKING_DELIVERY_NETWORK_LINK_H

#include <stddef.h>
#include "networking/network_statistics.h"
#include "networking/transport/transport_address.h"

class c_bitstream;
class c_network_channel;
struct transport_endpoint;
struct s_network_link_incoming_packet_helper;

class c_network_out_of_band_consumer
{
public:
    virtual bool receive_out_of_band_packet(const transport_address *address, c_bitstream *packet) = 0;
};

// The link owns two endpoints and four time-statistics objects.
class c_network_link
{
public:
    c_network_link();
    ~c_network_link();

    static c_network_channel *get_associated_channel(const transport_address *address);
    static bool adjust_packet_size(bool unlimited, long packet_bytes_used, long *packet_size);
    struct s_link_packet
    {
        long type;
        bool simulate;
        transport_address address;
        long game_size;
        unsigned char game[1536];
        long voice_size;
        unsigned char voice[512];
    };

    long generate_channel_identifier()
    {
        long identifier = m_channel_identifier;
        if (++m_channel_identifier == -1)
            ++m_channel_identifier;
        return identifier;
    }

    bool create_endpoints();
    void destroy_endpoints();
    void process_incoming_packets();
    void process_all_channels();
    void send_out_of_band(const c_bitstream *game_data, const transport_address *address, long *size_on_wire);
    void send_channel(const c_bitstream *game_data, long voice_data_length, const unsigned char *voice_data,
        const transport_address *address, long *size_on_wire);

private:
    friend struct s_network_link_incoming_packet_helper;

    void initialize_packet(s_link_packet *packet);
    bool decode_packet(long size, const unsigned char *buffer, s_link_packet *packet) const;
    long compute_size_on_wire(const s_link_packet *packet) const;
    void send_packet_internal(const s_link_packet *packet);
    void process_packet_internal(const s_link_packet *packet);
    void encode_packet(const s_link_packet *packet, long *packet_size, unsigned char *packet_data, long packet_data_size) const;
    void send_data_immediate(long transport_type, const transport_address *address, long data_size, const void *data);

public: // network_globals.cpp initializes and disposes the link storage.
    bool m_initialized;
    long m_channel_identifier;
    long m_first_channel_index;
    bool m_endpoints_created;
    transport_endpoint *m_endpoints[2];
    c_network_out_of_band_consumer *m_out_of_band_consumer;
    c_network_time_statistics m_game_sent_statistics;
    c_network_time_statistics m_game_received_statistics;
    c_network_time_statistics m_wire_sent_statistics;
    c_network_time_statistics m_wire_received_statistics;
};

typedef char network_link_initialized_offset[(offsetof(c_network_link, m_initialized) == 0) ? 1 : -1];
#if !defined(_WIN64)
typedef char network_link_identifier_offset[(offsetof(c_network_link, m_channel_identifier) == 4) ? 1 : -1];
typedef char network_link_consumer_offset[(offsetof(c_network_link, m_out_of_band_consumer) == 0x18) ? 1 : -1];
typedef char network_link_statistics_offset[(offsetof(c_network_link, m_game_sent_statistics) == 0x20) ? 1 : -1];
#endif

typedef char network_link_packet_size[(sizeof(c_network_link::s_link_packet) == 0x824) ? 1 : -1];

// C entry points for endpoint creation and incoming packet processing.
extern "C" bool network_link_create_endpoint(long type, long port, bool broadcast, transport_endpoint **out);
extern "C" void network_link_process_incoming_packets(void *unused);

#endif // NETWORKING_DELIVERY_NETWORK_LINK_H
