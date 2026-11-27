// networkinglibxbox/network_link.obj: from the records 0x8f070 0x126978 0x3edd28 0x3edf20 0x3edfc8 0x3ee030 0x3ee0b8 0x3ee168 0x3ee228 0x3ee338 0x3ee498 0x3ee5d8 0x3ee6a8
// @flags /TP /O2
#include "simulation/simulation.h"
#include "cseries/crt_xdk.h"
#include "networking/delivery/network_channel_activity_types.h"
#include "networking/transport/transport_globals.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include "networking/network_memory.h"
#include "networking/transport/transport_endpoint_winsock.h"
#include "networking/transport/transport_address.h"
#include "memory/bitstream.h"
#include "networking/online/voice_xenon.h"
#include "networking/network_statistics.h"
#include "networking/online/voice_interface_types.h"
#include "networking/delivery/network_channel.h"
#include "networking/delivery/network_link.h"
#include "xbox/winsock_services.h"
#include "networking/transport/transport_available.h"
#include "networking/network_globals.h"
#include "networking/online/voice_lifecycle.h"

// @address 0x8f070 network_link_process_incoming_packets
// @address 0x3edfc8 ?destroy_endpoints@c_network_link@@QAAXXZ
// @address 0x3ee030 ?get_associated_channel@c_network_link@@SAPAVc_network_channel@@PBUtransport_address@@@Z
// @address 0x3ee0b8 ?adjust_packet_size@c_network_link@@SA_N_NJPAJ@Z
// @address 0x3ee168 ?send_out_of_band@c_network_link@@QAAXPBVc_bitstream@@PBUtransport_address@@PAJ@Z
// @address 0x3ee228 ?send_channel@c_network_link@@QAAXPBVc_bitstream@@JPBEPBUtransport_address@@PAJ@Z
// @address 0x3ee338 ?send_packet_internal@c_network_link@@AAAXPBUs_link_packet@1@@Z
// @address 0x3ee498 ?process_packet_internal@c_network_link@@AAAXPBUs_link_packet@1@@Z
// @address 0x3ee5d8 ?decode_packet@c_network_link@@ABA_NJPBEPAUs_link_packet@1@@Z
// @address 0x3ee6a8 ?compute_size_on_wire@c_network_link@@ABAJPBUs_link_packet@1@@Z

#include "networking/online/voice_queue_types.h"

struct s_network_link_incoming_packet_helper
{
    // match: 0x8f070: friend access shim under #pragma optimize("c", off) without exposing FUN_82* in network_link.h.
    static __forceinline transport_endpoint **endpoints(c_network_link *link) { return link->m_endpoints; }
    // match: 0x8f070: friend access shim under #pragma optimize("c", off) without exposing FUN_82* in network_link.h.
    static __forceinline bool decode(c_network_link *link, long size, const unsigned char *buffer, c_network_link::s_link_packet *packet)
    {
        return link->decode_packet(size, buffer, packet);
    }
    // match: 0x8f070: friend access shim under #pragma optimize("c", off) without exposing FUN_82* in network_link.h.
    static __forceinline void process(c_network_link *link, const c_network_link::s_link_packet *packet)
    {
        link->process_packet_internal(packet);
    }
};

static __forceinline void network_link_endpoint_delete(transport_endpoint *endpoint)
{
    transport_endpoint_disconnect(endpoint);
    if (endpoint != 0)
        RtlFreeHeap(GetProcessHeap(), 0, endpoint);
}

static __forceinline bool network_link_packet_has_voice(long type)
{
    return type != 1;
}

static __forceinline long network_link_transport_overhead(long transport_type)
{
    long overhead = 0;

    switch (transport_type)
    {
    case _transport_type_udp:
        overhead = 44;
        break;
    case _transport_type_vdp:
        overhead = 45;
        break;
    case _transport_type_tcp:
        overhead = 56;
        break;
    }

    return overhead;
}

// @address 0x3edf20 ?create_endpoints@c_network_link@@QAA_NXZ
bool c_network_link::create_endpoints()
{
    bool ok = true;
    ok = ok && network_link_create_endpoint(1, 0x3e8, 0, &m_endpoints[0]);
    ok = ok && network_link_create_endpoint(0, 0x3e9, 1, &m_endpoints[1]);
    if (ok) {
        m_endpoints_created = true;
    } else {
        destroy_endpoints();
    }
    return m_endpoints_created;
}

void c_network_link::destroy_endpoints(void)
{
    for (long i = 0; i < 2; i++) {
        if (m_endpoints[i] != 0) {
            network_link_endpoint_delete(m_endpoints[i]);
            m_endpoints[i] = 0;
        }
    }
    m_endpoints_created = false;
}

static __forceinline void size_round_down(long *packet_size)
{
  if (*packet_size > 8)
    *packet_size -= *packet_size % 8;
}

c_network_channel *c_network_link::get_associated_channel(const transport_address *address)
{
  c_network_channel *channel = network_channel_iterate(0);
  while (true) {
    if (channel == 0)
      return channel;
    bool matched = false;
    if (channel->m_state != 0 && channel->m_state != 1)
      matched = transport_address_equivalent(&channel->m_remote_address, address, false);
    if (matched != 0)
      break;
    channel = network_channel_iterate(channel);
  }
  return channel;
}

bool c_network_link::adjust_packet_size(bool unlimited, long packet_bytes_used, long *packet_size)
{
  bool result = true;
  if (unlimited) {
    size_round_down(packet_size);
    if (*packet_size > 1264)
      *packet_size = 1264;
  }
  else {
    long available = 1262 - packet_bytes_used;
    available -= available % 8;
    if (available >= 0) {
      size_round_down(packet_size);
      if (*packet_size > available)
        *packet_size = available;
    }
    else
      result = false;
  }
  return result;
}

inline void c_network_link::initialize_packet(s_link_packet *packet)
{
    packet->type = -1;
    packet->simulate = false;
    memset(&packet->address, 0, sizeof(packet->address));
    packet->game_size = 0;
    packet->voice_size = 0;
}

void c_network_link::send_out_of_band(const c_bitstream *game_data, const transport_address *address, long *size_on_wire)
{
    bool success = true;
    s_link_packet packet;

    initialize_packet(&packet);
    packet.type = 1;
    packet.address = *address;

    const unsigned char *data = game_data->get_data(&packet.game_size);
    if (packet.game_size <= sizeof(packet.game))
        memcpy(packet.game, data, packet.game_size);
    else
        success = false;

    if (success)
        send_packet_internal(&packet);
}

void c_network_link::send_channel(const c_bitstream *game_data, long voice_data_length, const unsigned char *voice_data,
    const transport_address *address, long *size_on_wire)
{
    long size = 0;
    bool success = true;
    s_link_packet packet;

    initialize_packet(&packet);
    packet.address = *address;
    packet.type = 0;
    packet.simulate = false;

    if (success && game_data) {
        const unsigned char *data = game_data->get_data(&packet.game_size);
        if ((unsigned long)packet.game_size <= sizeof(packet.game))
            memcpy(packet.game, data, packet.game_size);
        else
            success = false;
    }

    if (success && voice_data_length > 0) {
        packet.voice_size = voice_data_length;
        if ((unsigned long)voice_data_length <= sizeof(packet.voice))
            memcpy(packet.voice, voice_data, voice_data_length);
        else
            success = false;
    }

    if (success) {
        send_packet_internal(&packet);
        size = compute_size_on_wire(&packet);
    }

    if (size_on_wire)
        *size_on_wire = size;
}

// match: 0x3ee338: helper must inline into send_packet_internal.
__forceinline void c_network_link::encode_packet(const s_link_packet *packet, long *packet_size, unsigned char *packet_data, long packet_data_size) const
{
    if (network_link_packet_has_voice(packet->type)) {
        *(unsigned short *)packet_data = (unsigned short)packet->game_size;
        memcpy(packet_data + 2, packet->game, packet->game_size);
        memcpy(packet_data + 2 + packet->game_size, packet->voice, packet->voice_size);
        *packet_size = 2 + packet->game_size + packet->voice_size;
    }
    else {
        memcpy(packet_data, packet->game, packet->game_size);
        *packet_size = packet->game_size;
    }
}

// match: 0x3ee338: helper must inline into send_packet_internal.
__forceinline void c_network_link::send_data_immediate(long transport_type, const transport_address *address, long data_size, const void *data)
{
    transport_address destination = *address;

    switch (transport_type)
    {
    case _transport_type_udp:
        destination.port = 1000;
        break;
    case _transport_type_vdp:
        destination.port = 1001;
        break;
    default:
        // match: 0x3ee338: retained accepted transport-type assumption in send_data_immediate.
        __assume(0);
    }

    if (m_endpoints[transport_type] != 0) {
        long bytes_written = ::transport_endpoint_write_to(m_endpoints[transport_type], (void const *)(data),
            (short)data_size, &destination);
        if (bytes_written != data_size && bytes_written == -1) {
            if (transport_type == _transport_type_udp) {
                c_network_channel *channel = get_associated_channel(address);
                if (channel != 0)
                    channel->notify_destination_unreachable();
            }
        }
    }
}

void c_network_link::send_packet_internal(const s_link_packet *packet)
{
    long packet_size;
    unsigned char packet_data[4096];

    encode_packet(packet, &packet_size, packet_data, sizeof(packet_data));
    m_game_sent_statistics.add_event(packet->game_size);
    m_wire_sent_statistics.add_event(compute_size_on_wire(packet));

    if (packet_size <= 1264 && packet_size > 0) {
        send_data_immediate(packet->type, &packet->address, packet_size, packet_data);
    }
}

static __forceinline void network_link_process_voice_data(const unsigned char *data, long data_size)
{
    if (g_voice_interface_globals.initialized && voice_lifecycle_valid())
        g_voice_queue.decode_peer_voice_stream(data, data_size);
}

void c_network_link::process_packet_internal(const s_link_packet *packet)
{
    long size_on_wire = compute_size_on_wire(packet);
    m_game_received_statistics.add_event(packet->game_size);
    m_wire_received_statistics.add_event(size_on_wire);

    switch (packet->type)
    {
    case 0:
    {
        c_network_channel *channel = get_associated_channel(&packet->address);
        if (channel != 0 && !channel->closed()) {
            if (packet->game_size > 0) {
                c_bitstream packet_data;
                bitstream_initialize(&packet_data, packet->game, packet->game_size);
                channel->receive_packet(&packet_data, size_on_wire, false);
            }
            if (packet->voice_size > 0) {
                network_link_process_voice_data(packet->voice, packet->voice_size);
                channel->update_activity(_network_channel_activity_receive_voice);
            }
        }
        break;
    }
    case 1:
    {
        c_bitstream packet_data;
        bitstream_initialize(&packet_data, packet->game, packet->game_size);
        if (m_out_of_band_consumer)
            m_out_of_band_consumer->receive_out_of_band_packet(&packet->address, &packet_data);
        break;
    }
    default:
        // match: 0x3ee498: retained accepted packet-type assumption.
        __assume(0);
    }
}

bool c_network_link::decode_packet(long size, const unsigned char *buffer, s_link_packet *packet) const
{
    bool success = true;
    if (network_link_packet_has_voice(packet->type)) {
        if (size >= 2) {
            packet->game_size = *(const unsigned short *)buffer;
            packet->voice_size = size - (packet->game_size + 2);
            if (packet->game_size >= 0 && (unsigned long)packet->game_size <= 1536 && packet->voice_size >= 0 && (unsigned long)packet->voice_size <= 512) {
                XMemCpy(packet->game, buffer + 2, packet->game_size);
                XMemCpy(packet->voice, buffer + packet->game_size + 2, packet->voice_size);
            }
            else
                success = false;
        }
        else
            success = false;
    }
    else if ((unsigned long)size <= 1536) {
        packet->game_size = size;
        XMemCpy(packet->game, buffer, size);
    }
    else
        success = false;
    return success;
}

long c_network_link::compute_size_on_wire(const s_link_packet *packet) const
{
    long size = packet->game_size;
    long transport_type = network_link_packet_has_voice(packet->type) ? _transport_type_vdp : _transport_type_udp;

    if (size % 8 > 0)
        size += 8 - size % 8;

    return network_link_transport_overhead(transport_type) + size + packet->voice_size;
}

// @address 0x609c90 bind
// @address 0x609c70 setsockopt
#define INL_BIND(result, endpoint, address) do { \
    bool binding_ok = false; \
    long socket_address_size; \
    unsigned char socket_address[32]; \
    if (transport_available() && \
        transport_endpoint_get_socket_address(address, &socket_address_size, socket_address) && \
        transport_endpoint_create_socket(endpoint, address)) { \
        if (bind(endpoint->socket, socket_address, socket_address_size) == 0) binding_ok = true; \
        else WSAGetLastError(); \
    } \
    result = binding_ok; \
} while (0)
#define INL_BROADCAST(result, endpoint) do { \
    long value = 1; \
    bool option_ok = false; \
    if (transport_available() && endpoint->socket != -1) { \
        if (setsockopt(endpoint->socket, 0xffff, 32, &value, sizeof(value)) != 0) WSAGetLastError(); \
        else option_ok = true; \
    } \
    result = option_ok; \
} while (0)
// @address 0x3edd28 network_link_create_endpoint
extern "C" bool network_link_create_endpoint(long type, long port, bool broadcast, transport_endpoint **out)
{
    transport_endpoint *endpoint = transport_endpoint_create((e_transport_type)type);
    bool success = true;
    if (endpoint) {
        transport_address address;
        address.port = (unsigned short)port;
        address.address_length = 4;
        address.address.ipv4_address = 0;
        bool bound;
        INL_BIND(bound, endpoint, &address);
        success = success && bound;
        success = success && transport_endpoint_set_blocking(endpoint, false);
        if (broadcast)
        {
            long value;
            bool option_ok;
            success = success &&
                (value = 1, option_ok = false,
                    ((transport_available() && endpoint->socket != -1)
                            ? ((setsockopt(endpoint->socket, 0xffff, 32, &value, sizeof(value)) != 0) ? (WSAGetLastError(), 0)
                                                                                                        : (option_ok = true, 0))
                            : 0),
                    option_ok);
        }
        if (success) *out = endpoint;
        else {
            transport_endpoint_disconnect(endpoint);
            RtlFreeHeap(GetProcessHeap(), 0, endpoint);
        }
    } else success = false;
    return success;
}


// match: 0x126978: retained accepted optimization state before process_all_channels.
#pragma optimize("c", on)
// @address 0x126978 ?process_all_channels@c_network_link@@QAAXXZ
void c_network_link::process_all_channels(void)
{
    c_network_channel *channels[32] = { 0 };
    long channel_count = 0;
    for (c_network_channel *channel = network_channel_iterate(0);
         channel != 0; channel = network_channel_iterate(channel))
    {
        channels[channel_count++] = channel;
    }

    if (channel_count != 0)
    {
        c_network_link *link = g_network_link;
        long first_channel_index = link->m_first_channel_index % channel_count;
        long channel_index = first_channel_index;
        bool advance = false;
        do
        {
            if (channels[channel_index]->idle() && channel_index == first_channel_index)
                advance = true;
            channel_index = (channel_index + 1) % channel_count;
        } while (channel_index != first_channel_index);

        if (advance == true)
            ++link->m_first_channel_index;
    }
}

// match: 0x8f070: retained BASE optimization state for the held C entry.
#pragma optimize("c", off)



extern "C" void network_link_process_incoming_packets(void *unused)
{
    // match: 0x8f070: retained BASE parameter-home workaround; natural member probes are -4B/-8B.
    void **keep = &unused;
    c_network_link *link = g_network_link;
    unsigned char buffer[4096];
    transport_address address;
    c_network_link::s_link_packet packet;
    long size;
    long channel_index;
    bool received;
    do {
        bool processed = false;
        received = false;
        for (long i = 0; !received && i < 2; i++) {
            bool have = false;
            if (s_network_link_incoming_packet_helper::endpoints(link)[i] != 0) {
                short n = transport_endpoint_read_from(s_network_link_incoming_packet_helper::endpoints(link)[i], buffer, sizeof(buffer), &address);
                if (n > 0 && transport_address_valid(&address)) {
                    size = n;
                    have = true;
                }
            }
            if (have) {
                channel_index = i;
                received = true;
            }
        }
        if (received) {
            packet.simulate = false;
            memset(&packet.address, 0, sizeof(packet.address));
            packet.game_size = 0;
            packet.voice_size = 0;
            packet.type = channel_index;
            packet.address = address;
            processed = s_network_link_incoming_packet_helper::decode(
                link, size, (unsigned char const *)(buffer), &packet);
        }
        if (processed)
            s_network_link_incoming_packet_helper::process(link, &packet);
    } while (received);
}
