// networkinglibxbox/network_channel.obj: from the records 0x417e48 0x417fc0 0x418148 0x418210 0x418588 0x418658 0x4186d8 0x4189c8 0x418e00 0x418e60 0x419068 0x419160 0x4191c8 0x419330 0x419360
// @flags /TP /O2 /Oc
// @address 0x418148 ?establish@c_network_channel@@QAAXK@Z
// @address 0x417e48 ?allocate@c_network_channel@@QAAXKPBDPAVc_network_link@@PAVc_network_observer@@PAVc_network_message_gateway@@PAVc_network_message_handler@@PAVc_network_message_type_collection@@PBUs_channel_configuration@@@Z
// @address 0x418588 ?close@c_network_channel@@QAAXW4e_network_channel_closure_reason@@@Z
// @address 0x418658 ?deallocate@c_network_channel@@QAAXXZ
// @address 0x4191c8 ?iterator_next@c_network_channel@@ABA_NPAUs_client_iterator@1@@Z

#include <string.h>
#include "networking/delivery/network_channel_activity_types.h"
#include "networking/delivery/network_channel_state_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "simulation/simulation.h"
#include "memory/bitstream.h"
#include "networking/transport/transport_address.h"
#include "networking/messages/network_message_type_collection.h"
#include "memory/sliding_window.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/network_statistics.h"
#include "networking/network_channel_client_info_types.h"
#include "networking/messages/network_message_queue.h"

#include "networking/delivery/network_channel.h"
#include "networking/network_time_types.h"
#include "networking/messages/network_messages_connect.h"
#include "cseries/crt_xdk.h"
// @address 0x8c447c g_network_time_globals
#include "networking/delivery/network_link.h"
#include "networking/messages/network_message_handler.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/messages/network_message_gateway.h"

namespace {
static __forceinline unsigned long network_channel_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    return GetTickCount();
}
static __forceinline void network_channel_reset_activity_times(unsigned long *activity_times)
{
    for (long activity = 0; activity < 6; ++activity)
        activity_times[activity] = network_channel_time_get();
}
static __forceinline void network_channel_send_connection_established(
    c_network_message_queue *queue, unsigned long remote_identifier, unsigned long identifier, bool initial_establishment)
{
    s_network_message_connect_establish message;
    message.identifier = identifier;
    message.remote_identifier = remote_identifier;
    if (initial_establishment)
        network_message_queue_send_connect_establish(queue, _network_message_type_connect_establish, sizeof(message), &message);
}
}

__forceinline void c_network_channel::record_activity(e_network_channel_activity activity)
{
    m_activity_times[activity] = network_channel_time_get();
}

__forceinline void c_network_channel::establish_connection()
{
    m_state = _network_channel_state_connected;
    m_connected_time = network_channel_time_get();
}

void c_network_channel::allocate(unsigned long flags, const char *name, c_network_link *link, c_network_observer *observer,
    c_network_message_gateway *gateway, c_network_message_handler *handler, c_network_message_type_collection *types,
    const s_channel_configuration *configuration)
{
    m_state = _network_channel_state_empty;
    m_closure_reason = _network_channel_closure_reason_none;
    memset(&m_closed_remote_address, 0, sizeof(m_closed_remote_address));
    m_connect_flags = 8;
    m_identifier = 0xffffffff;
    m_remote_identifier = 0xffffffff;
    m_observer = observer;
    m_link = link;
    m_message_gateway = gateway;
    m_message_handler = handler;
    m_configuration = configuration;
    m_client_count = 0;
    memset(m_clients, 0, sizeof(m_clients));
    m_simulation_interface = 0;
    for (long activity = 0; activity < 6; ++activity)
        m_activity_times[activity] = network_channel_time_get();
    c_network_connection *connection = &m_connection;
    connection->m_allocated = true;
    connection->m_name = NULL;
    connection->reset();
    register_base_client(49, (c_network_channel_client *)connection);
    c_network_message_queue *queue = &m_message_queue;
    queue->reset();
    queue->m_initialized = 1;
    queue->m_name = NULL;
    queue->m_message_types = types;
    register_base_client(25, (c_network_channel_client *)queue);
    if (m_connect_flags & 8)
        register_base_client(1, (c_network_channel_client *)&m_gatekeeper);
}

// @address 0x417fc0 ?open@c_network_channel@@QAAXPBUtransport_address@@_NK@Z
void c_network_channel::open(const transport_address *remote_address, bool should_connect, unsigned long channel_identifier)
{
    m_remote_address = *remote_address;
    m_should_connect = should_connect;
    m_state = _network_channel_state_connecting;
    if (channel_identifier == (unsigned long)-1)
        channel_identifier = m_link->generate_channel_identifier();
    m_identifier = channel_identifier;
    m_remote_identifier = 0xffffffff;
    if (m_should_connect)
    {
        m_connect_start_time = network_channel_time_get();
        m_next_connect_packet_time = network_channel_time_get();
        m_connect_packet_count = 0;
        check_to_send_connect_packet();
    }
    network_channel_reset_activity_times(m_activity_times);
    m_connection.reset();
    c_network_message_queue *queue = &m_message_queue;
    queue->reset();
    c_network_message_queue::s_outgoing_fragment_record *fragment = network_message_queue_allocate_outgoing_fragment(queue, 32);
    if (fragment != 0)
    {
        queue->m_outgoing_window.open(queue->m_outgoing_window.newest_sequence_number() + 1);
        memset(fragment, 0, sizeof(*fragment));
        fragment->size = 32;
        queue->m_connect_establish_pending = true;
    }
}

void c_network_channel::establish(unsigned long remote_channel_identifier)
{
    bool changed = false;
    if (m_state == _network_channel_state_connecting)
    {
        m_state = _network_channel_state_established;
        m_established_time = network_channel_time_get();
        changed = true;
        m_remote_identifier = remote_channel_identifier;
        network_channel_reset_activity_times(m_activity_times);
    }
    network_channel_send_connection_established(&m_message_queue, m_remote_identifier, m_identifier, changed);
}

void c_network_channel::close(e_network_channel_closure_reason closure_reason)
{
    if (connected() && closure_reason != _network_channel_closure_reason_connect_reinitiate)
    {
        s_network_message_connect_closed message;
        message.identifier = get_identifier();
        message.remote_identifier = get_remote_identifier();
        message.closure_reason = closure_reason;
        m_message_gateway->send_message_directed(&m_remote_address, _network_message_type_connect_closed, sizeof(message), &message);
    }
    if (m_simulation_interface)
        m_simulation_interface->notify_closed();
    m_state = _network_channel_state_closed;
    m_closure_reason = closure_reason;
    m_closed_remote_address = m_remote_address;
    m_identifier = 0xffffffff;
    m_remote_identifier = 0xffffffff;
}

// @address 0x418658 ?deallocate@c_network_channel@@QAAXXZ
void c_network_channel::deallocate()
{
    if (!closed())
    {
        close(_network_channel_closure_reason_channel_deleted);
    }
    if (m_message_queue.allocated())
    {
        m_message_queue.deallocate();
    }
    if (m_connection.allocated())
    {
        m_connection.deallocate();
    }
    m_state = _network_channel_state_none;
}

void c_network_channel::update_activity(e_network_channel_activity activity)
{
    m_activity_times[activity] = network_channel_time_get();
}

// @address 0x418e60 ?update_packet_delivery@c_network_channel@@AAAXXZ
void c_network_channel::update_packet_delivery()
{
    m_connection.update_packet_events();
    long sequence_number;
    long round_trip_msec;
    long size_on_wire;
    e_network_packet_event packet_event;
    while ((packet_event = m_connection.get_packet_event(&sequence_number, &size_on_wire, &round_trip_msec)) != _network_packet_event_none)
    {
        if (packet_event == _network_packet_event_acknowledged || packet_event == _network_packet_event_delivered)
        {
            c_network_channel *channel = this;
            channel->record_activity(_network_channel_activity_packet_delivery);
            if (channel->m_state == _network_channel_state_established)
                channel->establish_connection();
        }
        m_observer->handle_packet_event(this, packet_event, sequence_number, size_on_wire, round_trip_msec);
        if (packet_event == _network_packet_event_acknowledged)
        {
            s_client_iterator iterator;
            iterator_begin(&iterator, 4);
            while (iterator_next(&iterator))
                iterator.client->notify_packet_acknowledged(sequence_number);
        }
        if (packet_event == _network_packet_event_delivered || packet_event == _network_packet_event_delivered_late ||
            packet_event == _network_packet_event_lost)
        {
            bool lost = packet_event == _network_packet_event_lost;
            bool delivered = !lost;
            s_client_iterator iterator;
            iterator_begin(&iterator, 8);
            while (iterator_next(&iterator))
                iterator.client->notify_packet_retired(sequence_number, delivered);
        }
    }
}

namespace {
static __forceinline long network_channel_time_since(unsigned long time)
{
    return network_channel_time_get() - time;
}
}
// @address 0x419068 ?check_to_send_connect_packet@c_network_channel@@AAAXXZ
void c_network_channel::check_to_send_connect_packet()
{
    if (network_channel_time_since(m_connect_start_time) >= m_configuration->connect_timeout)
        close(_network_channel_closure_reason_connect_timeout);
    else if (network_channel_time_since(m_next_connect_packet_time) >= 0 && m_connect_packet_count < m_configuration->connect_packet_count)
    {
        s_network_message_connect_request message;
        message.flags = m_connect_flags;
        message.identifier = m_identifier;
        m_message_gateway->send_message_directed(&m_remote_address, _network_message_type_connect_request, sizeof(message), &message);
        m_next_connect_packet_time = network_channel_time_get() + m_configuration->connect_packet_interval;
    }
}

// @address 0x419160 ?complete_establishment@c_network_channel@@AAAXXZ
void c_network_channel::complete_establishment()
{
    m_state = _network_channel_state_connected;
    m_connected_time = network_channel_time_get();
}

// @address 0x4191c8 ?iterator_next@c_network_channel@@ABA_NPAUs_client_iterator@1@@Z
// match: 0x4191c8: pointer __restrict qualifier prevents alias reload
bool c_network_channel::iterator_next(s_client_iterator *__restrict iterator) const
{
    const s_network_channel_client_info *current_client = 0;
    long client_designator = iterator->client_designator;
    long client_index = iterator->client_index;
    for (;;)
    {
        current_client = 0;
        bool advanced = false;
        if (client_designator == -1)
        {
            client_designator = 0;
            client_index = 0;
            advanced = true;
        }
        if (!current_client && !(client_designator & (long)0x80000000))
        {
            long index = client_designator & 0x7fffffff;
            if (!advanced)
            {
                ++index;
                advanced = true;
                client_designator = index & 0x7fffffff;
                client_index = index;
            }
            if (index >= 0 && index < m_client_count)
                current_client = &m_clients[index];
            else
            {
                client_designator = 0x80000000;
                client_index = m_client_count;
                advanced = true;
            }
        }
        if (!current_client && (client_designator & (long)0x80000000))
        {
            const s_network_channel_client_info *clients = 0;
            long client_count = 0;
            long index = client_designator & 0x7fffffff;
            if (!advanced)
            {
                ++index;
                advanced = true;
                client_designator = (index & 0x7fffffff) | 0x80000000;
                client_index = index + m_client_count;
            }
            if (m_simulation_interface)
                client_count = m_simulation_interface->get_clients(&clients);
            if (index >= 0 && index < client_count)
                current_client = &clients[index];
            else
            {
                client_designator = -1;
                client_index = -1;
                advanced = true;
            }
        }
        if (client_designator == -1)
            break;
        else if ((current_client->flags & iterator->required_client_flags) == iterator->required_client_flags)
            break;
    }
    iterator->client_designator = client_designator;
    iterator->client_index = client_index;
    if (iterator->client_designator == -1)
    {
        iterator->client = 0;
        iterator->client_flags = 0;
    }
    else
    {
        iterator->client = current_client->client;
        iterator->client_flags = current_client->flags;
    }
    return iterator->client_designator != -1;
}

bool c_network_channel_simulation_gatekeeper::write_to_packet(long, c_bitstream *bitstream, long, long)
{
    bitstream->write_bool("simulation-data", m_initialized);
    return false;
}

// @address 0x419360 ?read_from_packet@c_network_channel_simulation_gatekeeper@@UAA?AW4e_network_read_result@@PAJPAVc_bitstream@@@Z
e_network_read_result c_network_channel_simulation_gatekeeper::read_from_packet(long *sequence_number, c_bitstream *packet)
{
    e_network_read_result result = _network_read_result_success;
    bool simulation_data = packet->read_bool("simulation-data");
    if (simulation_data != m_active)
        result = _network_read_result_failure;
    return result;
}

__forceinline void c_network_observer::notify_channel_packet_received(c_network_channel *channel, long size_on_wire)
{
    find_observer_by_channel(channel)->packet_statistics.add_event(size_on_wire);
}

__forceinline void c_network_observer::notify_channel_packet_sent(
    c_network_channel *channel, long size_on_wire, long game_bits, long filler_bits, long voice_bytes)
{
    m_bandwidth_statistics.add_event(size_on_wire);
    s_channel_observer *channel_observer = find_observer_by_channel(channel);
    if (channel_observer->voice_active)
    {
        long index = find_observer_index_by_observer(channel_observer);
        s_channel_observer *observer = &m_channel_observers[index];
        observer->period_bytes_received += size_on_wire;
        observer->interval_counts[0] += game_bits;
        observer->interval_counts[1] += filler_bits;
        observer->interval_counts[2] += voice_bytes * 8;
    }
}

// @address 0x4189c8 ?send_packet@c_network_channel@@AAAXPAVc_bitstream@@_N1JPAXPAJ33@Z
void c_network_channel::send_packet(c_bitstream *packet, bool sequenced, bool force_fill, long voice_bytes, void *voice_data,
    long *out_size_on_wire, long *out_game_bytes, long *out_voice_bytes)
// match: 0x4189c8: value range assumption guides branch lowering
{ __assume(packet != 0);
    long size_on_wire = 0;
    long sequence_number = -1;
    bool success = true;
    long game_bytes = 0;
    long voice_bytes_sent = 0;
    long game_bits = 0;
    long filler_bits = 0;
    if (sequenced)
    {
        sequence_number = m_connection.generate_sequence_number(network_channel_time_get());
        success = sequence_number != -1;
    }
    if (success)
    {
        long space_required = 0;
        packet->begin_writing(8, 0, false);
        long space_available = packet->get_space_left_in_bits();
        unsigned long client_flags;
        if (m_simulation_interface && m_simulation_interface->m_send_data_expected)
        {
            m_gatekeeper.m_initialized = true;
            client_flags = 0;
        }
        else
        {
            m_gatekeeper.m_initialized = false;
            client_flags = 1;
        }
        s_client_iterator iterator;
        iterator_begin(&iterator, client_flags);
        long space_required_bits_per_client[7];
        memset(space_required_bits_per_client, 0, sizeof(space_required_bits_per_client));
        while (iterator_next(&iterator))
        {
            long minimum_bits = iterator.client->space_required_bits(space_available, space_available - space_required);
            space_required_bits_per_client[iterator.client_index] = minimum_bits;
            space_required += minimum_bits;
        }
        ++space_required;
        if (space_required > space_available)
            success = false;
        if (success)
        {
            iterator_begin(&iterator, client_flags);
            while (iterator_next(&iterator))
            {
                long minimum_bits = space_required_bits_per_client[iterator.client_index];
                space_required -= minimum_bits;
                iterator.client->write_to_packet(sequence_number, packet, minimum_bits, space_required);
                if (packet->get_space_left_in_bits() < space_required)
                {
                    success = false;
                    break;
                }
            }
        }
        if (success)
        {
            game_bits = packet->get_space_used_in_bits() + 1;
            if (force_fill && packet->get_space_left_in_bits() > 15)
            {
                long filler_size = packet->get_space_left_in_bits() - 15;
                filler_bits = packet->get_space_left_in_bits() - 1;
                packet->write_bool("filler-exists", true);
                packet->write_integer("filler-size-bits", filler_size, 14);
                unsigned char filler[1536];
                memset(filler, 0, (filler_size + 7) / 8);
                packet->write_raw_data("filler", filler, filler_size);
            }
            else
                packet->write_bool("filler-exists", false);
        }
    }
    if (success)
    {
                packet->finish_writing(0);
                m_link->send_channel(packet, voice_bytes, static_cast<const unsigned char *>(voice_data), &m_remote_address, &size_on_wire);
                if (size_on_wire > 0)
                {
                    game_bytes = packet->m_data_size_bytes;
                    voice_bytes_sent = voice_bytes;
                }
                if (sequence_number != -1)
                {
                    c_network_connection *connection = &m_connection;
                    c_network_connection::s_connection_outgoing_packet *outgoing = connection->get_outgoing_packet(sequence_number);
                    outgoing->size_on_wire = size_on_wire;
                    outgoing->timestamp = network_channel_time_get();
                    connection->m_outgoing_window_size_bytes += size_on_wire;
                }
                m_observer->notify_channel_packet_sent(this, size_on_wire, game_bits, filler_bits, voice_bytes);
    }
    if (out_size_on_wire) *out_size_on_wire = size_on_wire;
    if (out_game_bytes) *out_game_bytes = game_bytes;
    if (out_voice_bytes) *out_voice_bytes = voice_bytes_sent;
}

// @address 0x11ab038 g_network_message_decode_buffer
// Initial contents are all zero across sizeof(s_network_message_queue_buffer).
s_network_message_queue_buffer g_network_message_decode_buffer;

// @address 0x4186d8 ?receive_packet@c_network_channel@@QAA_NPAVc_bitstream@@J_N@Z
bool c_network_channel::receive_packet(c_bitstream *packet, long size_on_wire, bool consistency_check)
{
    long problem = _network_read_result_success;
    long sequence_number = -1;
    bool success = false;
    if (!consistency_check)
        packet->begin_reading();
    unsigned long client_flags;
    if (m_simulation_interface && m_simulation_interface->m_send_data_expected)
    {
        m_gatekeeper.m_active = true;
        client_flags = 0;
    }
    else
    {
        m_gatekeeper.m_active = false;
        client_flags = 1;
    }
    s_client_iterator iterator;
    iterator_begin(&iterator, client_flags);
    while (iterator_next(&iterator))
    {
        problem = iterator.client->read_from_packet(&sequence_number, packet);
        if (problem != _network_read_result_success)
            break;
    }
    if (problem == _network_read_result_success && packet->read_bool("filler-exists"))
    {
        long filler_size = packet->read_integer("filler-size-bits", 14);
        if (filler_size < 1 || filler_size > 0x3000)
        {
            problem = _network_read_result_error;
        }
        else
        {
            unsigned char filler[1536];
            packet->read_raw_data("filler", filler, filler_size);
            if (packet->error_occurred())
                problem = _network_read_result_error;
        }
    }
    if (packet->error_occurred())
        problem = _network_read_result_error;
    switch (problem)
    {
    case _network_read_result_failure:
        packet->discard_remaining_data();
    case _network_read_result_success:
        packet->finish_reading();
        success = true;
        if (!consistency_check)
        {
            update_activity(_network_channel_activity_receive_packet);
            update_packet_delivery();
            m_connection.notify_packet_received(sequence_number);
            e_network_message_type message_type;
            long message_size;
            while (m_message_queue.retrieve_message(&message_type, &message_size, 0x10000, &g_network_message_decode_buffer))
            {
                if (established() || message_type == _network_message_type_connect_establish)
                {
                    if (m_state == _network_channel_state_established && message_type != _network_message_type_connect_establish)
                        complete_establishment();
                    m_message_handler->handle_channel_message(this, message_type, message_size, &g_network_message_decode_buffer);
                }
            }
        }
        break;
    case _network_read_result_discard:
        break;
    case _network_read_result_error:
        break;
    default:
        // match: 0x4186d8: unreachable hint omits dead branch check
        __assume(0);
    }
    if (!closed() && !consistency_check)
        m_observer->notify_channel_packet_received(this, size_on_wire);
    return success;
}

// @address 0x418210 ?idle@c_network_channel@@QAA_NXZ
bool c_network_channel::idle()
{
    bool sent_packet = false;
    if (m_state == _network_channel_state_connecting && m_should_connect)
        check_to_send_connect_packet();
    if (m_state == _network_channel_state_established)
    {
        if (network_channel_time_since(m_established_time) > m_configuration->establishment_timeout)
            close(static_cast<e_network_channel_closure_reason>(7));
    }
    if (network_channel_established())
    {
        s_client_iterator iterator;
        iterator_begin(&iterator, 16);
        while (iterator_next(&iterator))
        {
            e_network_channel_closure_reason reason = _network_channel_closure_reason_none;
            if (iterator.client->connection_lost(&reason))
            {
                close(reason);
                break;
            }
        }
    }
    if (network_channel_established())
    {
        bool game_data_pending = false;
        bool force_heartbeat = false;
        bool send_sequenced = false;
        unsigned long flags;
        if (m_simulation_interface && m_simulation_interface->m_send_data_expected)
            flags = 0;
        else
            flags = 1;
        s_client_iterator iterator;
        iterator_begin(&iterator, flags);
        while (iterator_next(&iterator))
        {
            bool urgent = false;
            if (iterator.client->has_data_to_transmit(&urgent))
            {
                if (iterator.client_flags & 32)
                    force_heartbeat = true;
                else
                    game_data_pending = true;
                if (urgent)
                    send_sequenced = true;
            }
        }
        bool sequenced;
        bool force_fill;
        long game_data_size;
        long voice_data_size;
        unsigned char voice_data[512];
        if (m_observer->channel_should_send_packet(this, game_data_pending, force_heartbeat, send_sequenced,
            &sequenced, &force_fill, &game_data_size, &voice_data_size, sizeof(voice_data), voice_data))
        {
            unsigned char data[1536];
            c_bitstream packet;
            bitstream_initialize(&packet, data, sizeof(data));
            packet.m_data = data;
            packet.m_data_max = data + game_data_size;
            packet.m_data_size_bytes = game_data_size;
            sent_packet = true;
            packet.reset(0);
            long size_on_wire;
            long game_bytes;
            long voice_bytes;
            send_packet(&packet, sequenced, force_fill, voice_data_size, voice_data_size > 0 ? voice_data : 0,
                &size_on_wire, &game_bytes, &voice_bytes);
            (this)->update_activity(_network_channel_activity_send_packet);
            if (sequenced)
                (this)->update_activity((static_cast<e_network_channel_activity>(1)));
            if (voice_data_size > 0)
                (this)->update_activity((static_cast<e_network_channel_activity>(2)));
        }
    }
    if (network_channel_established())
        update_packet_delivery();
    if (m_destination_unreachable)
    {
        if (!closed())
            close(static_cast<e_network_channel_closure_reason>(9));
        m_destination_unreachable = false;
    }
    return sent_packet;
}
