#ifndef NETWORKING_DELIVERY_NETWORK_CONNECTION_H
#define NETWORKING_DELIVERY_NETWORK_CONNECTION_H

#include "memory/sliding_window.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/delivery/network_channel_client.h"

class c_bitstream;

class c_network_connection : public c_network_channel_client
{
    friend class c_network_channel;
public:
    c_network_connection();
    ~c_network_connection();
    virtual bool connection_lost(e_network_channel_closure_reason *reason) const;
    bool allocated() const { return m_allocated; }
    void deallocate() { m_allocated = false; m_name = NULL; }
private:
    __forceinline void update_packet_events()
    {
        long packet_event;
        long sequence_number;
        while (remove_retired_packet(false, &packet_event, &sequence_number))
            ;
    }
public:
    struct s_connection_incoming_packet { unsigned short data; };
    struct s_connection_outgoing_packet
    {
        unsigned long timestamp;
        long size_on_wire;
        long round_trip_time;
        unsigned short acknowledgement_range;
        unsigned short flags;
    };
    virtual const char *get_client_name() const;
    virtual bool has_data_to_transmit(bool *urgent) const;
    virtual long space_required_bits(long maximum_bits, long available_bits);
    virtual bool write_to_packet(long packet_sequence_number, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits);
    virtual e_network_read_result read_from_packet(long *packet_sequence_number, c_bitstream *packet);
    virtual void notify_packet_acknowledged(long sequence_number);
    virtual void notify_packet_retired(long sequence_number, bool delivered);
    void reset();
    long generate_sequence_number(unsigned long timestamp);
    e_network_packet_event get_packet_event(long *sequence_number, long *size_on_wire, long *round_trip_msec);
    void notify_packet_received(long incoming_packet_sequence_number);
private:
    s_connection_incoming_packet *get_incoming_packet(long sequence_number);
    s_connection_outgoing_packet *get_outgoing_packet(long sequence_number);
    e_network_read_result handle_sequence_number(bool new_packet, long sequence_bits, long acknowledgement_range, long *incoming_sequence_number);
    e_network_read_result handle_acknowledgements(
        long channel_identifier, long acknowledged_sequence_bits, long bitvector_size, unsigned long *bitvector, bool remote_window_closed);
    bool retire_packet(e_network_packet_event *event, long *sequence_number, long *size, long *round_trip_time);
    void retire_incoming_packets();
    bool remove_retired_packet(bool force, long *packet_event, long *sequence_number);
public:
    long get_safe_sequence_range_remaining() const
    {
        long range_used = m_outgoing_window.newest_sequence_number() - m_highest_acknowledged_sequence_number;
        return 128 - range_used;
    }
public:
    bool m_allocated;
    bool m_overflowed;
    const char *m_name;
    c_sliding_window m_incoming_window;
    c_sliding_window m_outgoing_window;
    long m_last_retired_outgoing_sequence_number;
    long m_outgoing_window_size_bytes;
    s_connection_incoming_packet m_incoming_packets[128];
    s_connection_outgoing_packet m_outgoing_packets[128];
    long m_highest_acknowledged_sequence_number;
    long m_last_transmitted_incoming_sequence_number;
    long m_last_retired_incoming_sequence_number;
    bool m_incoming_window_closed;
    bool m_remote_window_closed;
    long m_retire_time;
};

#endif // NETWORKING_DELIVERY_NETWORK_CONNECTION_H
