#ifndef NETWORKING_ONLINE_VOICE_QUEUE_TYPES_H
#define NETWORKING_ONLINE_VOICE_QUEUE_TYPES_H

#include <string.h>
#include "networking/online/voice_routing.h"
#include "networking/online/voice_packet_header_types.h"
#include "networking/online/voice_queue_peer_tracking_types.h"
#include "networking/online/player_voice_stream_header_types.h"

struct s_voice_queue_packet {
    s_voice_route route;
    unsigned char sequence;
    unsigned short pending;
    unsigned char data[10];
};

class c_player_voice_queue {
public:
    __forceinline c_player_voice_queue()
        : enabled(false)
    {
    }

    bool enabled;
    long packet_count;
    unsigned short pending_peer_mask;
    long player_index;
    s_voice_queue_packet packets[16];
    s_voice_queue_peer_tracking peers[16];

    __forceinline void initialize(long index)
    {
        player_index = index;
        enabled = true;
        packet_count = 0;
        pending_peer_mask = 0;
        memset(packets, 0, sizeof(packets));
        memset(peers, 0, sizeof(peers));
    }

    __forceinline void clear_voice_packet_queue()
    {
        packet_count = 0;
        pending_peer_mask = 0;
        memset(packets, 0, sizeof(packets));
        memset(peers, 0, sizeof(peers));
    }
    // match: 0x41e480: __restrict keeps the I in the decorated name (JPIBUs_voice_route); without it the emitter drops the I and the 0x41e480 tag misses
    void add_packet(long player, const void *data, long size, const s_voice_route *__restrict voice_route, const s_voice_packet_header *voice_packet_header);

    __forceinline bool has_packets_to_transmit(long peer)
    {
        bool result = false;
        if (enabled)
            result = (pending_peer_mask & (1 << peer)) != 0;
        return result;
    }
    __forceinline void compact_voice_packet_queue(unsigned short keep)
    {
        long output = 0;
        for (long i = 0; i < packet_count; i++) {
            if (keep & (1 << i)) {
                if (i != output) {
                    s_voice_queue_packet *destination = &packets[output];
                    s_voice_queue_packet *source = &packets[i];
                    *destination = *source;
                }
                output++;
            }
        }
        packet_count = output;
    }
    __forceinline void rebuild_pending_peer_mask()
    {
        pending_peer_mask = 0;
        for (long i = 0; i < packet_count; i++)
            pending_peer_mask |= packets[i].pending;
    }
    __forceinline long get_peer_packet_count(long peer)
    {
        long result = 0;
        if (enabled) result = peers[peer].queued_packet_count;
        return result;
    }
    __forceinline long get_peer_packet_size(long peer)
    {
        long result = 0;
        if (enabled) result = peers[peer].queued_packet_size;
        return result;
    }
    __forceinline long get_player_voice_stream_size(long peer, bool full)
    {
        long result = 0;
        if (enabled) {
            long count = get_peer_packet_count(peer);
            long size = get_peer_packet_size(peer);
            long header_size = full ? 3 : 1;
            result = count * header_size + 2 + size;
        }
        return result;
    }
    long encode_player_voice_stream(s_player_voice_stream_header *header, unsigned char *data, long available_space, long peer, bool full);
    long decode_player_voice_stream(const s_player_voice_stream_header *header, long available_space, bool full);
};

class c_voice_queue {
public:
    __forceinline c_voice_queue()
    {
        enabled = false;
    }

    bool enabled;
    c_player_voice_queue players[16];
    __forceinline void initialize()
    {
        for (long player_index = 0; player_index < 16; ++player_index)
            players[player_index].initialize(player_index);
        enabled = true;
    }
    bool has_packets_to_transmit(long recipient);
    long get_peer_voice_stream_size(long destination_peer_index);
    long encode_peer_voice_stream(long destination_peer_index, unsigned char *data, long available_space);
    void decode_peer_voice_stream(const unsigned char *data, long data_size);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char player_voice_queue_size[(sizeof(c_player_voice_queue) == 432) ? 1 : -1];
#endif

#endif // NETWORKING_ONLINE_VOICE_QUEUE_TYPES_H
