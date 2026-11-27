// networkinglibxbox/network_connection.obj: from the records 0x416c68 0x416d38 0x416d80 0x416e20 0x417098 0x417318 0x417468 0x417568 0x4175e0 0x417658 0x4177b0 0x4178a8 0x417a88 0x417c10 0x417d08
// @flags /TP /O2 /Oc
// @address 0x80608 GetTickCount
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x416c68 ?reset@c_network_connection@@QAAXXZ
// @address 0x416d38 ?has_data_to_transmit@c_network_connection@@UBA_NPA_N@Z
// @address 0x417568 ?get_incoming_packet@c_network_connection@@AAAPAUs_connection_incoming_packet@1@J@Z
// @address 0x4175e0 ?get_outgoing_packet@c_network_connection@@AAAPAUs_connection_outgoing_packet@1@J@Z
// @address 0x417658 ?handle_sequence_number@c_network_connection@@AAA?AW4e_network_read_result@@_NJJPAJ@Z
// @address 0x4178a8 ?handle_acknowledgements@c_network_connection@@AAA?AW4e_network_read_result@@JJJPAK_N@Z
// @address 0x417a88 ?retire_packet@c_network_connection@@AAA_NPAW4e_network_packet_event@@PAJ11@Z
// @address 0x417c10 ?retire_incoming_packets@c_network_connection@@AAAXXZ
// @address 0x417d08 ?remove_retired_packet@c_network_connection@@AAA_N_NPAJ1@Z
// @address 0x416d80 ?space_required_bits@c_network_connection@@UAAJJJ@Z
// @address 0x416e20 ?write_to_packet@c_network_connection@@UAA_NJPAVc_bitstream@@JJ@Z
// @address 0x417098 ?read_from_packet@c_network_connection@@UAA?AW4e_network_read_result@@PAJPAVc_bitstream@@@Z
// @address 0x417318 ?generate_sequence_number@c_network_connection@@QAAJK@Z
// @address 0x417468 ?get_packet_event@c_network_connection@@QAA?AW4e_network_packet_event@@PAJ00@Z
// @address 0x4177b0 ?notify_packet_received@c_network_connection@@QAAXJ@Z
#include <string.h>
#include "networking/delivery/network_connection.h"
#include "memory/bitstream.h"
#include "memory/sliding_window.h"
#include "networking/network_time_types.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
// @address 0x8c447c g_network_time_globals
#include "cseries/crt_xdk.h"
namespace {
static __forceinline unsigned long connection_time_get()
{
    if (g_network_time_globals.enabled) return g_network_time_globals.time;
    return GetTickCount();
}
static __forceinline long connection_time_since(unsigned long time)
{
    return (long)(connection_time_get() - time);
}
}
namespace {
static __forceinline c_network_connection::s_connection_incoming_packet *connection_incoming_packet(c_network_connection *connection, long sequence_number)
{
    long storage_index = connection->m_incoming_window.get_storage_index(sequence_number);
    c_network_connection::s_connection_incoming_packet *result = 0;
    if (storage_index != -1) result = &connection->m_incoming_packets[storage_index];
    return result;
}
}
void c_network_connection::reset()
{
    m_overflowed = false;
    long initial_sequence_number = (unsigned char)(g_network_configuration.connection_sequence_rate * connection_time_get() / 1000);
    m_outgoing_window.reset(true, initial_sequence_number);
    m_incoming_window.reset(false, 0);
    m_last_retired_outgoing_sequence_number = initial_sequence_number;
    m_outgoing_window_size_bytes = 0;
    m_last_transmitted_incoming_sequence_number = 0;
    m_last_retired_incoming_sequence_number = 0;
    m_highest_acknowledged_sequence_number = initial_sequence_number - 1;
    m_incoming_window_closed = false;
    m_remote_window_closed = true;
}
bool c_network_connection::has_data_to_transmit(bool *urgent) const
{
    bool result = false;
    if (m_incoming_window.initialized())
    {
        if (m_last_transmitted_incoming_sequence_number < m_incoming_window.newest_sequence_number()) result = true;
        else if (m_last_retired_incoming_sequence_number < m_incoming_window.newest_sequence_number()) result = true;
    }
    if (!m_remote_window_closed) result = true;
    return result;
}
c_network_connection::s_connection_incoming_packet *c_network_connection::get_incoming_packet(long sequence_number)
{
    long storage_index = m_incoming_window.get_storage_index(sequence_number);
    s_connection_incoming_packet *result = 0;
    if (storage_index != -1) result = &m_incoming_packets[storage_index];
    return result;
}
c_network_connection::s_connection_outgoing_packet *c_network_connection::get_outgoing_packet(long sequence_number)
{
    long storage_index = m_outgoing_window.get_storage_index(sequence_number);
    s_connection_outgoing_packet *result = 0;
    if (storage_index != -1) result = &m_outgoing_packets[storage_index];
    return result;
}
e_network_read_result c_network_connection::handle_sequence_number(
    bool new_packet, long sequence_bits, long acknowledgement_range, long *incoming_sequence_number)
{
    e_network_read_result result = _network_read_result_success;
    if (!m_incoming_window.initialized()) m_incoming_window.reset(true, sequence_bits - 1);
    long sequence_range = 256;
    long newest_sequence_bits = (unsigned char)m_incoming_window.newest_sequence_number();
    long sequence_delta = sequence_bits - newest_sequence_bits;
    if (sequence_delta > sequence_range / 2) sequence_delta -= sequence_range;
    else if (sequence_delta <= -sequence_range / 2) sequence_delta += sequence_range;
    long sequence_number = m_incoming_window.newest_sequence_number() + sequence_delta;
    if (new_packet) *incoming_sequence_number = sequence_number;
    else *incoming_sequence_number = -1;
    m_incoming_window_closed = acknowledgement_range == 0;
    long retired_sequence_number = sequence_number - acknowledgement_range;
    m_last_retired_incoming_sequence_number = retired_sequence_number > m_last_retired_incoming_sequence_number
        ? retired_sequence_number
        : m_last_retired_incoming_sequence_number;
    if (m_incoming_window.newest_sequence_number() <= retired_sequence_number) m_incoming_window.reset(true, retired_sequence_number);
    else if (m_incoming_window.trailing_edge() < retired_sequence_number)
    {
        while (m_incoming_window.trailing_edge() < retired_sequence_number)
        {
            long oldest_sequence_number = m_incoming_window.oldest_sequence_number();
            m_incoming_window.close(oldest_sequence_number);
        }
    }
    if (sequence_number <= m_incoming_window.trailing_edge() && (sequence_number != m_incoming_window.trailing_edge() || new_packet))
        result = _network_read_result_discard;
    return result;
}
e_network_read_result c_network_connection::handle_acknowledgements(
    long channel_identifier, long acknowledged_sequence_bits, long bitvector_size, unsigned long *bitvector, bool remote_window_closed)
{
    e_network_read_result result = _network_read_result_success;
    long sequence_range = 256;
    long newest_sequence_bits = (unsigned char)m_outgoing_window.newest_sequence_number();
    long sequence_delta = newest_sequence_bits - acknowledged_sequence_bits;
    if (sequence_delta < 0) sequence_delta += sequence_range;
    long acknowledged_sequence_number = m_outgoing_window.newest_sequence_number() - sequence_delta;
    s_connection_outgoing_packet *acknowledged_packet = get_outgoing_packet(acknowledged_sequence_number);
    long oldest_acknowledged;
    if (acknowledged_packet) oldest_acknowledged = acknowledged_sequence_number - acknowledged_packet->acknowledgement_range;
    else oldest_acknowledged = m_last_retired_outgoing_sequence_number;
    m_remote_window_closed = false;
    if (acknowledged_sequence_number <= m_outgoing_window.trailing_edge())
    {
        if (acknowledged_sequence_number < m_outgoing_window.trailing_edge() - g_network_configuration.connection_ancient_packet_threshold)
            result = _network_read_result_discard;
    }
    else
    {
        long latest_sequence_number = acknowledged_sequence_number - 1;
        for (long sequence_number = acknowledged_sequence_number; sequence_number > m_outgoing_window.trailing_edge(); --sequence_number)
        {
            s_connection_outgoing_packet *packet = get_outgoing_packet(sequence_number);
            if (!(packet->flags & 1))
            {
                bool acknowledged_directly = false;
                bool acknowledged;
                if (sequence_number == acknowledged_sequence_number) { acknowledged = true; acknowledged_directly = true; }
                else
                {
                    long index = latest_sequence_number - sequence_number;
                    if (index < bitvector_size) acknowledged = !(bitvector[index >> 5] & (1 << (index & 31)));
                    else if (sequence_number > m_last_retired_outgoing_sequence_number) acknowledged = true;
                    else if (sequence_number > oldest_acknowledged) acknowledged = true;
                    else acknowledged = false;
                }
                if (acknowledged)
                {
                    packet->flags |= 1;
                    if (acknowledged_directly) packet->flags |= 32;
                    else packet->flags &= ~32;
                    packet->round_trip_time = connection_time_since(packet->timestamp);
                    m_highest_acknowledged_sequence_number =
                        sequence_number > m_highest_acknowledged_sequence_number ? sequence_number : m_highest_acknowledged_sequence_number;
                }
            }
        }
    }
    if (acknowledged_sequence_number == m_outgoing_window.newest_sequence_number() &&
        m_last_retired_outgoing_sequence_number == m_outgoing_window.newest_sequence_number() && remote_window_closed)
        m_remote_window_closed = true;
    return result;
}
bool c_network_connection::retire_packet(
    // match: 0x417a88: out-pointer stores reload packet->flags (lhz +0xE) without it
    e_network_packet_event *__restrict event, long *__restrict sequence_number, long *__restrict size, long *__restrict round_trip_time)
{
    long number = m_last_retired_outgoing_sequence_number + 1;
    s_connection_outgoing_packet *packet = get_outgoing_packet(number);
    bool result = false;
    if (packet)
    {
        if (packet->flags & 1)
        {
            if (packet->flags & 2) *event = _network_packet_event_delivered_late;
            else { *event = _network_packet_event_delivered; packet->flags |= 2; }
            *sequence_number = number;
            *size = packet->size_on_wire;
            if (packet->flags & 32) *round_trip_time = packet->round_trip_time;
            else *round_trip_time = -1;
            packet->flags |= 4;
        }
        else
        {
            long elapsed = connection_time_since(packet->timestamp);
            if (elapsed >= m_retire_time)
            {
                packet->round_trip_time = elapsed;
                *event = _network_packet_event_lost;
                *sequence_number = number;
                *size = packet->size_on_wire;
                *round_trip_time = packet->round_trip_time;
                packet->flags |= 8;
            }
            else if (m_highest_acknowledged_sequence_number - number >= g_network_configuration.connection_drop_threshold)
            {
                packet->round_trip_time = elapsed;
                *event = _network_packet_event_lost;
                *sequence_number = number;
                *size = packet->size_on_wire;
                *round_trip_time = packet->round_trip_time;
                packet->flags |= 8;
            }
        }
        if ((packet->flags & 4) || (packet->flags & 8))
        {
            result = true;
            packet->flags |= 16;
            m_last_retired_outgoing_sequence_number = number;
            m_outgoing_window_size_bytes -= packet->size_on_wire;
        }
    }
    return result;
}
void c_network_connection::retire_incoming_packets()
{
    if (m_incoming_window.initialized())
    {
        while (!m_incoming_window.empty())
        {
            long sequence_number = m_incoming_window.oldest_sequence_number();
            s_connection_incoming_packet *packet = connection_incoming_packet(this, sequence_number);
            if (packet->data & 1) m_incoming_window.close(sequence_number);
            else break;
        }
    }
}
// match: 0x417d08: initial *packet_event store sinks below the frame addi without it
bool c_network_connection::remove_retired_packet(bool force, long *__restrict packet_event, long *sequence_number)
{
    bool retired = false;
    *packet_event = 0;
    if (!m_outgoing_window.empty())
    {
        long oldest_sequence_number = m_outgoing_window.oldest_sequence_number();
        if (oldest_sequence_number <= m_last_retired_outgoing_sequence_number)
        {
            s_connection_outgoing_packet *packet = get_outgoing_packet(oldest_sequence_number);
            if (packet->flags & 4) retired = true;
            else if (packet->flags & 8)
            {
                if (force) retired = true;
                else if ((packet->flags & 1) && (packet->flags & 2)) retired = true;
                else if (connection_time_since(packet->timestamp) >= m_retire_time + g_network_configuration.connection_packet_retire_time) retired = true;
            }
            // match: 0x417d08: omits flags-bit-3 re-test (rlwinm+cmplwi+beq) on the fall-through edge
            else { __assume(0); }
            if (retired)
            {
                *sequence_number = oldest_sequence_number;
                *packet_event = 6;
                m_outgoing_window.close(oldest_sequence_number);
            }
        }
    }
    return retired;
}
long c_network_connection::space_required_bits(long maximum_bits, long available_bits)
{
    retire_incoming_packets();
    if (m_incoming_window.initialized())
    {
        if (m_incoming_window.empty()) return 18;
        if (m_incoming_window.current_size() <= 9) return 26;
        return m_incoming_window.current_size() + 25;
    }
    return 19;
}
bool c_network_connection::write_to_packet(long packet_sequence_number, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits)
{
    long current_sequence_number = m_outgoing_window.newest_sequence_number();
    long unretired_window_size = current_sequence_number - m_last_retired_outgoing_sequence_number;

    packet->write_bool("packet-sequenced", packet_sequence_number != -1);
    packet->write_integer("out-sequence", (unsigned char)current_sequence_number, 8);
    packet->write_integer("out-window-size", unretired_window_size, 7);
    if (unretired_window_size == 0)
        m_remote_window_closed = true;

    retire_incoming_packets();

    if (m_incoming_window.initialized())
    {
        long incoming_sequence_number = m_incoming_window.newest_sequence_number();
        long latest_sequence_number = m_incoming_window.newest_sequence_number() - 1;
        packet->write_integer("in-sequence", (unsigned char)incoming_sequence_number, 8);
        m_last_transmitted_incoming_sequence_number = m_incoming_window.newest_sequence_number();

        if (m_incoming_window.empty())
        {
            packet->write_bool("bit0", false);
            packet->write_bool("bit1", false);
            packet->write_bool("known-retired", m_incoming_window_closed);
        }
        else if (m_incoming_window.current_size() <= 9)
        {
            bool missing_multiple_packets = false;
            long missing_packet_offset = -1;
            unsigned long bitmask = 0;
            for (long sequence_number = latest_sequence_number; sequence_number > m_incoming_window.trailing_edge(); sequence_number--)
            {
                s_connection_incoming_packet *incoming_packet = get_incoming_packet(sequence_number);
                if (!(incoming_packet->data & 1))
                {
                    long packet_offset = latest_sequence_number - sequence_number;
                    bitmask |= 1 << packet_offset;
                    if (missing_packet_offset == -1)
                        missing_packet_offset = packet_offset;
                    else
                        missing_multiple_packets = true;
                }
            }

            if (missing_multiple_packets)
            {
                packet->write_bool("bit0", true);
                packet->write_bool("bit1", false);
                for (long bit = 0; bit < 8; bit++)
                    packet->write_bool("bitmask", (bitmask & (1 << bit)) != 0);
            }
            else
            {
                packet->write_bool("bit0", false);
                packet->write_bool("bit1", true);
                packet->write_integer("missing-packet", missing_packet_offset, 3);
            }
        }
        else
        {
            packet->write_bool("bit0", true);
            packet->write_bool("bit1", true);
            packet->write_bool("bit2", false);
            packet->write_integer("window-size", m_incoming_window.current_size() - 1, 7);
            for (long sequence_number = latest_sequence_number; sequence_number > m_incoming_window.trailing_edge(); sequence_number--)
                packet->write_bool("bitmask", !(get_incoming_packet(sequence_number)->data & 1));
        }
    }
    else
    {
        packet->write_integer("in-sequence", 0, 8);
        packet->write_bool("bit0", true);
        packet->write_bool("bit1", true);
        packet->write_bool("bit2", true);
    }

    return false;
}
long c_network_connection::generate_sequence_number(unsigned long timestamp)
{
    long sequence_number = -1;
    if (!connection_lost(0))
    {
        if (m_outgoing_window.full())
        {
            long packet_event;
            long retired_sequence_number;
            remove_retired_packet(true, &packet_event, &retired_sequence_number);
        }

        if (get_safe_sequence_range_remaining() > 0 && !m_outgoing_window.full() &&
            m_outgoing_window.newest_sequence_number() - m_last_retired_outgoing_sequence_number + 1 < 128)
        {
            sequence_number = m_outgoing_window.newest_sequence_number() + 1;
            m_outgoing_window.open(sequence_number);
            m_remote_window_closed = false;
            long unretired_window_size = sequence_number - m_last_retired_outgoing_sequence_number;
            s_connection_outgoing_packet *packet = get_outgoing_packet(sequence_number);
            memset(packet, 0, sizeof(*packet));
            packet->timestamp = timestamp;
            packet->round_trip_time = -1;
            packet->acknowledgement_range = (unsigned short)unretired_window_size;
        }
        else
        {
            m_overflowed = true;
        }
    }
    return sequence_number;
}
// match: 0x417468: out-pointer stores reload packet->flags (lhz +0xE) without it
e_network_packet_event c_network_connection::get_packet_event(long *__restrict sequence_number, long *__restrict size_on_wire, long *__restrict round_trip_msec)
{
    e_network_packet_event packet_event = _network_packet_event_none;
    if (!retire_packet(&packet_event, sequence_number, size_on_wire, round_trip_msec) && !m_outgoing_window.empty())
    {
        for (long packet_sequence_number = m_outgoing_window.oldest_sequence_number();
            packet_sequence_number < m_outgoing_window.newest_sequence_number(); packet_sequence_number++)
        {
            s_connection_outgoing_packet *packet = get_outgoing_packet(packet_sequence_number);
            if ((packet->flags & 1) && !(packet->flags & 2))
            {
                e_network_packet_event acknowledged_event;
                if (packet->flags & 8)
                    acknowledged_event = _network_packet_event_acknowledged_late;
                else
                    acknowledged_event = _network_packet_event_acknowledged;
                *sequence_number = packet_sequence_number;
                *size_on_wire = packet->size_on_wire;
                if (packet->flags & 32)
                    *round_trip_msec = packet->round_trip_time;
                else
                    *round_trip_msec = -1;
                packet->flags |= 2;
                return acknowledged_event;
            }
        }
    }
    return packet_event;
}
void c_network_connection::notify_packet_received(long incoming_packet_sequence_number)
{
    if (incoming_packet_sequence_number != -1)
    {
        while (incoming_packet_sequence_number > m_incoming_window.newest_sequence_number())
        {
            m_incoming_window.open(m_incoming_window.newest_sequence_number() + 1);
            s_connection_incoming_packet *packet = connection_incoming_packet(this, m_incoming_window.newest_sequence_number());
            memset(packet, 0, sizeof(*packet));
        }
        s_connection_incoming_packet *packet = get_incoming_packet(incoming_packet_sequence_number);
        if (!(packet->data & 1))
            packet->data |= 1;
    }
}
e_network_read_result c_network_connection::read_from_packet(long *packet_sequence_number, c_bitstream *packet)
{
    e_network_read_result result = _network_read_result_success;
    if (connection_lost(0))
    {
        result = _network_read_result_discard;
    }
    else
    {
        bool packet_sequenced = packet->read_bool("packet-sequenced");
        long out_sequence = packet->read_integer("out-sequence", 8);
        long out_window_size = packet->read_integer("out-window-size", 7);
        if (packet->read_only_for_consistency())
        {
            *packet_sequence_number = packet_sequenced ? m_outgoing_window.newest_sequence_number() : -1;
        }
        else
        {
            result = handle_sequence_number(packet_sequenced, out_sequence, out_window_size, packet_sequence_number);
        }
        if (result == _network_read_result_success)
        {
            long in_sequence = packet->read_integer("in-sequence", 8);
            bool valid = true;
            bool known_retired = false;
            bool bit0 = packet->read_bool("bit0");
            bool bit1 = packet->read_bool("bit1");
            unsigned long bitvector[4];
            memset(bitvector, 0, sizeof(bitvector));
            long window_size;
            if (!bit0 && !bit1)
            {
                window_size = 0;
                known_retired = packet->read_bool("known-retired");
            }
            else if (bit0 && !bit1)
            {
                window_size = 0;
                for (long index = 0; index < 8; index++)
                {
                    if (packet->read_bool("bitmask"))
                    {
                        window_size = index + 1;
                        bitvector[0] |= 1 << index;
                    }
                }
            }
            else if (!bit0 && bit1)
            {
                long missing_packet = packet->read_integer("missing-packet", 3);
                window_size = missing_packet + 1;
                bitvector[0] |= 1 << missing_packet;
            }
            else
            {
                bool bit2 = packet->read_bool("bit2");
                if (!bit2)
                {
                    window_size = packet->read_integer("window-size", 7);
                    for (long index = 0; index < window_size; index++)
                    {
                        if (packet->read_bool("bitmask"))
                            bitvector[index >> 5] |= 1 << (index & 31);
                        else
                            bitvector[index >> 5] &= ~(1 << (index & 31));
                    }
                }
                else
                {
                    valid = false;
                }
            }
            if (valid && !packet->read_only_for_consistency())
            {
                result = handle_acknowledgements(*packet_sequence_number, in_sequence, window_size, bitvector, known_retired);
            }
        }
    }
    return result;
}
