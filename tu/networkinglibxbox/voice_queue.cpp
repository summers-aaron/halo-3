// networkinglibxbox/voice_queue.obj: from the records 0x2f7ba8 0x41e480 0x41e628 0x41e888 0x41eae0 0x41ebe8 0x41ee88 0x41efe8
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include <string.h>
#include "networking/online/voice_xenon.h"
#include "networking/online/voice_queue.h"
#include "networking/online/voice_data_types.h"
#include "networking/online/voice_queue_peer_tracking_types.h"

#include "main/main_halt.h"
// @address 0x8c43e8 g_voice_queue_get_effect_version
#include "networking/online/voice_filter.h"
#if !defined(_MSC_VER) || defined(__clang__)
#include "sound/sound_xaudio_xenon_imports.h"
// the Waves import view's slot 3 (bss 0x828c43e8 on the Xbox)
#define g_voice_queue_get_effect_version (reinterpret_cast<voice_queue_get_effect_version_proc &>(sound_xaudio_waves_procedure_slot(3)))
#endif

#include "networking/online/player_voice_stream_header_types.h"

#pragma pack(push, 1)
#include "networking/online/voice_queue_full_packet_header_types.h"
#pragma pack(pop)
#include "networking/online/voice_routing.h"
#include "networking/online/voice_packet_header_types.h"
#include "networking/online/voice_queue_types.h"
#include "networking/online/voice_interface_types.h"
#include "networking/online/voice_observer_types.h"

struct s_peer_voice_stream_header { unsigned char flags; unsigned char player_count; };

// @address 0x2f7ba8 voice_filter_validate_version
extern "C" void voice_filter_validate_version(void *effect, const char *name)
{
    if (effect == 0)
        main_halt_and_display_errors();
    unsigned long version = 0;
    unsigned long platform = 0;
    if (!g_voice_queue_get_effect_version(effect, &version, &platform))
        main_halt_and_display_errors();
    if (strcmp(name, "Q10,EQ1M") == 0 || strcmp(name, "Q10,EQ4M") == 0)
    {
        if (version != 0x301 || platform != 0x6e510004)
            main_halt_and_display_errors();
    }
    else if (strcmp(name, "L360,L3M6") == 0)
    {
        if (version != 0x10301 || platform != 0x534c0005)
            main_halt_and_display_errors();
    }
    else
        main_halt_and_display_errors();
}

c_voice_queue g_voice_queue;
static __forceinline long voice_get_repeater_peer_index()
{
    long result = 0;
    if (g_voice_interface_globals.initialized)
        result = g_voice_interface_globals.repeater_peer_index;
    return result;
}

static __forceinline long voice_queue_packet_size_by_data_type(e_voice_data_type type)
{
    switch (type) {
    case _voice_data_chat:
        return 10;
    }
    return 0;
}

// @address 0x41e480 ?add_packet@c_player_voice_queue@@QAAXJPBXJPIBUs_voice_route@@PBUs_voice_packet_header@@@Z
void c_player_voice_queue::add_packet(
    // match: 0x41e480: pointer __restrict qualifier prevents alias reload
    long player, const void *data, long size, const s_voice_route *__restrict voice_route, const s_voice_packet_header *voice_packet_header)
{
    if (enabled) {
        s_voice_queue_packet *next_queue_slot = 0;
        if (packet_count + 1 >= 16)
            clear_voice_packet_queue();
        next_queue_slot = &packets[packet_count];
        next_queue_slot->pending = voice_route->routed_peer_mask;
        for (long peer_index = 0; peer_index < 16; peer_index++) {
            if (voice_route->routed_peer_mask & (1 << peer_index)) {
                peers[peer_index].queued_packet_count++;
                peers[peer_index].queued_packet_size += size;
            }
        }
        next_queue_slot->route = *voice_route;
        next_queue_slot->sequence = voice_packet_header->sequence;
        memcpy(next_queue_slot->data, data, size);
        pending_peer_mask |= voice_route->routed_peer_mask;
        packet_count++;
    }
}

long c_player_voice_queue::encode_player_voice_stream(s_player_voice_stream_header *header, unsigned char *data, long available_space, long peer, bool full)
{
    long result = 0;
    if (enabled) {
        if (!has_packets_to_transmit(peer))
            result = -1;
        else if (peer < 0 || peer >= 16)
            result = -1;
        else {
            unsigned short keep = (1 << packet_count) - 1;
            long encoded_count = 0;
            long encoded_size = 0;
            available_space -= 2;
            for (long i = 0; i < packet_count; i++) {
                s_voice_queue_packet *packet = &packets[i];
                if (packet->pending & (1 << peer)) {
                    e_voice_data_type type = _voice_data_chat;
                    long packet_size = voice_queue_packet_size_by_data_type(type);
                    long required_size = 0;
                    if (full)
                        required_size = packet_size + 3;
                    else
                        required_size = packet_size + 1;
                    if (available_space >= required_size) {
                        if (full) {
                            s_voice_queue_full_packet_header *packet_header = (s_voice_queue_full_packet_header *)data;
                            packet_header->sequence = packet->sequence;
                            packet_header->recipients = packet->route.repeater_peer_mask;
                            if (packet_header->recipients == 0)
                                packet_header->recipients |= 1 << peer;
                            data += 3;
                        } else {
                            *data = packet->sequence;
                            data++;
                        }
                        memcpy(data, packet->data, packet_size);
                        data += packet_size;
                        packet->pending &= ~(1 << peer);
                        peers[peer].queued_packet_count--;
                        peers[peer].queued_packet_size -= packet_size;
                        if (packet->pending == 0)
                            keep &= ~(1 << i);
                        encoded_count++;
                        encoded_size += required_size;
                        available_space -= required_size;
                    } else {
                        break;
                    }
                }
            }
            result = encoded_size + 2;
            header->packet_count = (unsigned char)encoded_count;
            header->player = (unsigned char)player_index;
            if (keep != ((1 << packet_count) - 1))
                compact_voice_packet_queue(keep);
            rebuild_pending_peer_mask();
        }
    }
    return result;
}

bool c_voice_queue::has_packets_to_transmit(long recipient)
{
    bool result = false;
    if (g_voice_queue.enabled) {
        for (long player_index = 0; player_index < 16; player_index++) {
            result = g_voice_queue.players[player_index].has_packets_to_transmit(recipient);
            if (result)
                break;
        }
    }
    return result;
}


long c_voice_queue::get_peer_voice_stream_size(long destination_peer_index)
{
    long stream_size = 0;
    if (g_voice_queue.enabled && g_voice_queue.has_packets_to_transmit(destination_peer_index)) {
        bool full = voice_get_repeater_peer_index() == destination_peer_index;
        stream_size = 2;
        for (long player_index = 0; player_index < 16; player_index++) {
            if (g_voice_queue.players[player_index].has_packets_to_transmit(destination_peer_index))
                stream_size += g_voice_queue.players[player_index].get_player_voice_stream_size(destination_peer_index, full);
        }
    }
    return stream_size;
}

long c_voice_queue::encode_peer_voice_stream(long destination_peer_index, unsigned char *data, long available_space)
{
    long stream_size = 0;
    long original_available_space = available_space;
    if (g_voice_queue.enabled) {
        if (g_voice_queue.has_packets_to_transmit(destination_peer_index)) {
            long encoded_player_count = 0;
            bool full = voice_get_repeater_peer_index() == destination_peer_index;
            s_peer_voice_stream_header *header = (s_peer_voice_stream_header *)data;
            s_player_voice_stream_header *player_stream = 0;
            available_space -= 2;
            if (full) header->flags |= 1;
            else header->flags &= ~1;
            data += 2;
            player_stream = (s_player_voice_stream_header *)data;
            for (long player_index = 0; player_index < 16; player_index++) {
                if (g_voice_queue.players[player_index].has_packets_to_transmit(destination_peer_index) && (unsigned long)available_space >= 2) {
                    long player_stream_size = 0;
                    data += 2;
                    player_stream_size = g_voice_queue.players[player_index].encode_player_voice_stream(
                        player_stream, data, available_space, destination_peer_index, full);
                    if (player_stream_size > 0) {
                        encoded_player_count++;
                        available_space -= player_stream_size;
                        data += player_stream_size - 2;
                        player_stream = (s_player_voice_stream_header *)data;
                    } else {
                        break;
                    }
                }
            }
            if (encoded_player_count > 0) {
                header->player_count = (unsigned char)encoded_player_count;
                stream_size = original_available_space - available_space;
            } else {
                header->player_count = 0;
                stream_size = 0;
            }
        }
    }
    return stream_size;
}

static __forceinline bool voice_local_machine_is_voice_repeater()
{
    bool result = false;
    if (g_voice_interface_globals.initialized)
    {
        long local_peer_index = g_voice_interface_globals.local_peer_index;
        result = local_peer_index != -1 && local_peer_index == g_voice_interface_globals.repeater_peer_index;
    }
    return result;
}

// @address 0x41efe8 ?decode_peer_voice_stream@c_voice_queue@@QAAXPBEJ@Z
void c_voice_queue::decode_peer_voice_stream(const unsigned char *data, long data_size)
{
    if (g_voice_queue.enabled)
    {
        const s_peer_voice_stream_header *header = (const s_peer_voice_stream_header *)data;
        long available_space = data_size - 2;
        bool full = (header->flags & 1) != 0;
        if (full && !voice_local_machine_is_voice_repeater())
            return;
        if (header->player_count >= 1 && header->player_count <= 16)
        {
            const unsigned char *player_data = data + 2;
            for (long player_stream_index = 0; player_stream_index < header->player_count; player_stream_index++)
            {
                const s_player_voice_stream_header *player_stream = (const s_player_voice_stream_header *)player_data;
                if (player_stream->player >= 16)
                    return;
                if (player_stream->packet_count >= 1 && player_stream->packet_count <= 16)
                {
                    long decoded_size = g_voice_queue.players[player_stream->player].decode_player_voice_stream(player_stream, available_space, full);
                    if (decoded_size <= 0)
                        return;
                    available_space -= decoded_size;
                    player_data += decoded_size;
                }
            }
        }
    }
}



static __forceinline s_voice_player_state *voice_queue_player_get_state(long player_index)
{
    s_voice_player_state *player_state = 0;
    player_state = &g_voice_interface_globals.player_states[player_index];
    return player_state;
}

static __forceinline bool voice_queue_player_has_voice(long player_index)
{
    bool has_voice = false;
    if (g_voice_interface_globals.initialized)
        has_voice = voice_queue_player_get_state(player_index)->status_flags.test(_voice_player_has_voice);
    return has_voice;
}

static __forceinline bool voice_queue_player_is_local(long player_index)
{
    bool local = false;
    if (g_voice_interface_globals.initialized)
        local = voice_queue_player_get_state(player_index)->status_flags.test(_voice_player_local);
    return local;
}

// @address 0x41e888 ?decode_player_voice_stream@c_player_voice_queue@@QAAJPBUs_player_voice_stream_header@@J_N@Z
long c_player_voice_queue::decode_player_voice_stream(const s_player_voice_stream_header *player_voice_stream_header, long available_space, bool full)
{
    long total_packet_size = 0;
    if (player_voice_stream_header->packet_count < 1 || player_voice_stream_header->packet_count > 16)
        return -1;
    const unsigned char *packet_data = (const unsigned char *)player_voice_stream_header;
    packet_data += sizeof(*player_voice_stream_header);
    available_space -= sizeof(s_player_voice_stream_header);
    total_packet_size += sizeof(s_player_voice_stream_header);
    for (long packet_index = 0; packet_index < player_voice_stream_header->packet_count; packet_index++) {
        if (available_space <= 0)
            return -1;
        long voice_packet_size;
        if (full) {
            long local_peer_index = voice_get_local_peer_index();
            if (local_peer_index == -1)
                return -1;
            const s_voice_queue_full_packet_header *full_packet_header = (const s_voice_queue_full_packet_header *)packet_data;
            unsigned short destination_peer_mask = full_packet_header->recipients;
            if (destination_peer_mask == 0)
                return -1;
            const void *voice_data = packet_data + 3;
            e_voice_data_type data_type = _voice_data_chat;
            long voice_data_size = voice_queue_packet_size_by_data_type(data_type);
            voice_packet_size = voice_data_size + 3;
            if (destination_peer_mask & (1 << local_peer_index)) {
                if (voice_queue_player_has_voice(player_index)) {
                    if (!voice_queue_player_is_local(player_index)) {
                        g_voice_observer.set_local_listeners_to_incoming_remote_player_voice(player_index, full_packet_header->sequence);
                        voice_queue_received_packet(player_index, voice_data, voice_data_size, data_type);
                    }
                }
                destination_peer_mask &= ~(1 << local_peer_index);
            }
            if (destination_peer_mask != 0) {
                s_voice_route voice_route;
                voice_route.routed_peer_mask = 0;
                memset(&voice_route.repeater_peer_mask, 0, sizeof(voice_route.repeater_peer_mask));
                g_voice_routing.get_voice_route_by_peer_mask(destination_peer_mask, &voice_route);
                add_packet(player_voice_stream_header->player, voice_data, voice_data_size, &voice_route, (const s_voice_packet_header *)packet_data);
            }
        } else {
            const void *voice_data = packet_data + 1;
            e_voice_data_type data_type = _voice_data_chat;
            long voice_data_size = voice_queue_packet_size_by_data_type(data_type);
            voice_packet_size = voice_data_size + 1;
            if (voice_queue_player_has_voice(player_index)) {
                if (!voice_queue_player_is_local(player_index)) {
                    const s_voice_packet_header *voice_packet_header = (const s_voice_packet_header *)packet_data;
                    g_voice_observer.set_local_listeners_to_incoming_remote_player_voice(player_index, voice_packet_header->sequence);
                    voice_queue_received_packet(player_index, voice_data, voice_data_size, data_type);
                }
            }
        }
        available_space -= voice_packet_size;
        total_packet_size += voice_packet_size;
        packet_data += voice_packet_size;
    }
    return total_packet_size;
}
