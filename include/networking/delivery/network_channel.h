#ifndef NETWORKING_DELIVERY_NETWORK_CHANNEL_H
#define NETWORKING_DELIVERY_NETWORK_CHANNEL_H

#include <stddef.h>
#include <string.h>
#include "networking/delivery/network_channel_activity_types.h"
#include "networking/delivery/network_channel_state_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "networking/delivery/network_connection.h"
#include "networking/transport/transport_address.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/network_channel_client_info_types.h"
#include "networking/network_memory.h"

class c_network_link;
class c_network_observer;
class c_network_message_gateway;
class c_network_message_handler;

class c_network_channel_simulation_interface
{
public:
    __forceinline c_network_channel_simulation_interface()
    {
        m_initialized = false;
        m_closure_callback_data = 0;
        m_closure_callback = 0;
        m_client_count = 0;
        memset(m_clients, 0, sizeof(m_clients));
        m_send_data_expected = false;
    }
    __forceinline void initialize(bool authority, void *closure_data, void (*closure_callback)(void *),
        long client_count, const unsigned long *client_flags, c_network_channel_client *const *clients)
    {
        m_initialized = true;
        m_authority = authority;
        m_closure_callback_data = closure_data;
        m_closure_callback = closure_callback;
        m_client_count = client_count;
        for (long client_index = 0; client_index < client_count; ++client_index)
        {
            s_network_channel_client_info *client_info = &m_clients[client_index];
            client_info->flags = client_flags[client_index];
            client_info->client = clients[client_index];
        }
        m_send_data_expected = false;
    }
    void dispose()
    {
        m_initialized = false;
    }
    void set_established(bool established)
    {
        m_send_data_expected = established;
    }
    bool established() const
    {
        return m_send_data_expected;
    }
    long get_clients(const s_network_channel_client_info **clients)
    {
        *clients = m_clients;
        return m_client_count;
    }
    void notify_closed() const
    {
        m_closure_callback(m_closure_callback_data);
    }
private:
    bool m_initialized;
    void *m_closure_callback_data;
    void (*m_closure_callback)(void *);
    long m_client_count;
    s_network_channel_client_info m_clients[4];
public:
    bool m_authority;
    bool m_send_data_expected;
};

class c_network_channel_simulation_gatekeeper : public c_network_channel_client
{
public:
    c_network_channel_simulation_gatekeeper();
    ~c_network_channel_simulation_gatekeeper();
    virtual const char *get_client_name() const;
    virtual bool connection_lost(e_network_channel_closure_reason *reason) const;
    virtual bool has_data_to_transmit(bool *urgent) const;
    virtual long space_required_bits(long maximum_bits, long available_bits);
    virtual bool write_to_packet(long, c_bitstream *bitstream, long, long);
    virtual e_network_read_result read_from_packet(long *sequence_number, c_bitstream *packet);
    virtual void notify_packet_acknowledged(long sequence_number);
    virtual void notify_packet_retired(long sequence_number, bool delivered);
    bool m_initialized, m_active;
};

class c_network_message_queue : public c_network_channel_client
{
public:
    c_network_message_queue();
    ~c_network_message_queue();
    virtual const char *get_client_name() const;
    virtual long space_required_bits(long maximum_bits, long available_bits);
    virtual void notify_packet_acknowledged(long sequence_number);
    virtual bool connection_lost(e_network_channel_closure_reason *reason) const;
    virtual bool has_data_to_transmit(bool *urgent) const;
    virtual bool write_to_packet(long packet_sequence_number, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits);
    virtual e_network_read_result read_from_packet(long *packet_sequence_number, c_bitstream *packet);
    virtual void notify_packet_retired(long identifier, bool acknowledged);
    void send_message(e_network_message_type message_type, long message_size, const void *message);
    long get_space_available() const { return (512 - m_outgoing_window.current_size()) * 32; }
    struct s_outgoing_fragment_record
    {
        unsigned char flags;
        unsigned char size;
        unsigned short bits;
        long identifier;
        s_outgoing_fragment_record *next;
        unsigned char data[0];
    };
    struct s_incoming_fragment_record
    {
        unsigned char flags;
        unsigned char size;
        unsigned short bits;
        s_incoming_fragment_record *next;
        char data[32];
    };
    void reset();
    void reset_for_channel_connection();
    bool retrieve_message(e_network_message_type *message_type, long *raw_message_size, long raw_message_buffer_size, void *raw_message_buffer);
private:
    bool handle_fragment(long fragment_sequence_number, bool last_fragment, long fragment_size_bits, long fragment_size_bytes, const void *fragment_data);
public:
    __forceinline const s_outgoing_fragment_record *get_first_outgoing_fragment() const
    {
        return m_first_outgoing_fragment;
    }
    __forceinline void free_first_outgoing_fragment()
    {
        s_outgoing_fragment_record *fragment = m_first_outgoing_fragment;
        if (fragment == m_last_outgoing_fragment) {
            m_first_outgoing_fragment = 0;
            m_last_outgoing_fragment = 0;
        } else {
            m_first_outgoing_fragment = fragment->next;
        }
#ifdef HALO3_64BIT
        m_outgoing_fragment_bytes -= fragment->size + offsetof(s_outgoing_fragment_record, data);
#else
        m_outgoing_fragment_bytes -= fragment->size + 12;
#endif
        ::network_heap_free_block(fragment);
    }
    void free_first_incoming_fragment();
    __forceinline bool allocated() const { return m_initialized; }
    __forceinline void deallocate()
    {
        reset();
        m_initialized = false;
        m_name = NULL;
    }
    bool m_initialized;
    bool m_connection_lost, m_connect_establish_pending, m_message_sent;
    bool m_connected;
    const char *m_name;
    c_network_message_type_collection *m_message_types;
    c_sliding_window m_outgoing_window;
    s_outgoing_fragment_record *m_first_outgoing_fragment;
    s_outgoing_fragment_record *m_last_outgoing_fragment;
    c_sliding_window m_incoming_window;
    s_incoming_fragment_record *m_first_incoming_fragment;
    s_incoming_fragment_record *m_last_incoming_fragment;
    long m_outgoing_fragment_bytes;
    long m_incoming_fragment_bytes;
};

struct s_channel_configuration
{
    long connect_packet_interval;
    long connect_packet_count;
    long connect_timeout;
    long establishment_timeout;
};

// Method names and qualifiers: March networking/delivery/network_channel.h.
// Observer state begins at +0xa70.
class c_network_channel
{
public:
    c_network_channel();
    ~c_network_channel();
    void *operator new(size_t, void *pointer) { return pointer; }
    struct s_client_iterator
    {
        unsigned long required_client_flags;
        long client_designator;
        long client_index;
        unsigned long client_flags;
        c_network_channel_client *client;
    };
    e_network_channel_state get_state() const { return m_state; }
    __forceinline bool closed() const { return get_state() <= _network_channel_state_closed; }
    bool connected() const { return get_state() == _network_channel_state_connected; }
    __forceinline bool network_channel_established() const { return get_state() >= _network_channel_state_established; }
    bool allocated() const { return m_state != _network_channel_state_none; }
    bool simulation_attached() const { return m_simulation_interface != 0; }
    bool simulation_is_authority() const { return m_simulation_interface && m_simulation_interface->m_authority; }
    c_network_channel_simulation_interface *const *simulation_interface_storage() const { return &m_simulation_interface; }
    void notify_destination_unreachable()
    {
        if (!closed())
            m_destination_unreachable = true;
    }
    void set_connection_packet_timeout(long timeout) { m_connection.m_retire_time = timeout; }
    e_network_channel_closure_reason closure_reason() const
    {
        e_network_channel_closure_reason reason = _network_channel_closure_reason_none;
        if (closed())
            reason = m_closure_reason;
        return reason;
    }
    unsigned long last_activity_time(e_network_channel_activity activity) const
    {
        unsigned long time = 0;
        if (!closed())
            time = m_activity_times[activity];
        return time;
    }
    long get_connection_safe_sequence_range() const
    {
        long range = 0;
        if (connected())
            range = 128;
        return range;
    }
    long get_connection_safe_sequence_range_remaining() const
    {
        long remaining = 0;
        if (connected())
            remaining = m_connection.get_safe_sequence_range_remaining();
        return remaining;
    }
    long get_message_space_available() const
    {
        long available = 0;
        if (connected())
            available = m_message_queue.get_space_available();
        return available;
    }
    long get_message_space_maximum_size_bytes() const { return 0x4000; }
    bool idle();
    unsigned long get_identifier() const { return m_identifier; }
    unsigned long get_remote_identifier() const { return m_remote_identifier; }
    void allocate(unsigned long flags, const char *name, c_network_link *link, c_network_observer *observer,
        c_network_message_gateway *gateway, c_network_message_handler *handler, c_network_message_type_collection *types,
        const s_channel_configuration *configuration);
    void open(const transport_address *remote_address, bool should_connect, unsigned long channel_identifier);
    void establish(unsigned long remote_channel_identifier);
    void close(e_network_channel_closure_reason closure_reason);
    void deallocate();
    void update_activity(e_network_channel_activity activity);
    bool established() const;
    bool receive_packet(c_bitstream *packet, long size_on_wire, bool consistency_check);
private:
    void send_packet(c_bitstream *packet, bool sequenced, bool force_fill, long voice_bytes, void *voice_data, long *size_on_wire,
        long *game_bytes, long *voice_bytes_sent);
    void complete_establishment();
    void update_packet_delivery();
    void check_to_send_connect_packet();
    __forceinline void record_activity(e_network_channel_activity activity);
    __forceinline void establish_connection();
    __forceinline void register_base_client(unsigned long flags, c_network_channel_client *client)
    {
        s_network_channel_client_info *info = &m_clients[m_client_count];
        info->flags = flags;
        info->client = client;
        ++m_client_count;
    }
    __forceinline void iterator_begin(s_client_iterator *iterator, unsigned long flags) const
    {
        iterator->required_client_flags = flags;
        iterator->client_designator = -1;
        iterator->client_index = -1;
        iterator->client_flags = 0;
        iterator->client = 0;
    }
    bool iterator_next(s_client_iterator *iterator) const;
public:
    c_network_link *m_link;
    c_network_observer *m_observer;
    c_network_message_gateway *m_message_gateway;
    c_network_message_handler *m_message_handler;
    const s_channel_configuration *m_configuration;
    c_network_connection m_connection;
    c_network_message_queue m_message_queue;
    c_network_channel_simulation_gatekeeper m_gatekeeper;
    long m_client_count;
    s_network_channel_client_info m_clients[3];
    c_network_channel_simulation_interface *m_simulation_interface;
    unsigned long m_connect_flags;
    unsigned long m_identifier;
    unsigned long m_remote_identifier;
    e_network_channel_state m_state;
    e_network_channel_closure_reason m_closure_reason;
    transport_address m_closed_remote_address;
    transport_address m_remote_address;
    bool m_should_connect;
    unsigned long m_connect_start_time;
    unsigned long m_next_connect_packet_time;
    long m_connect_packet_count;
    unsigned long m_established_time;
    unsigned long m_connected_time;
    unsigned long m_activity_times[6];
    bool m_destination_unreachable;
    char m_alignment_a6d[3];
};

typedef char network_channel_size[(sizeof(void *) != 4 || sizeof(c_network_channel) == 0xa70) ? 1 : -1];
typedef char network_channel_queue_offset[(sizeof(void *) != 4 || offsetof(c_network_channel, m_message_queue) == 0x974) ? 1 : -1];
typedef char network_channel_state_offset[(sizeof(void *) != 4 || offsetof(c_network_channel, m_state) == 0xa0c) ? 1 : -1];
typedef char network_channel_activity_offset[(sizeof(void *) != 4 || offsetof(c_network_channel, m_activity_times) == 0xa54) ? 1 : -1];

#endif
