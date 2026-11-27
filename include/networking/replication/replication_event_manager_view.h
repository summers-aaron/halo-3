#ifndef NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_VIEW_H
#define NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_VIEW_H

#include "networking/delivery/network_delivery_types.h"
#include "networking/replication/replication_event.h"
#include "networking/replication/replication_types.h"
#include "networking/replication/replication_scheduler_client.h"

class c_bitstream;
class c_replication_event_manager;

// Virtual order and fields are shared with replication_event_manager_view.cpp;
// March send_event (826da858) reads m_view_index at +0x0c and increments +0x20.
class c_replication_event_manager_view : public c_replication_scheduler_client
{
private:
    virtual bool has_data_to_transmit();
    virtual long build_outgoing_requests(const void *, long, s_replication_outgoing_request *);
    virtual long terminator_required_bits() const;
    virtual void write_to_packet(LONG_PTR, long, const void *, long, c_bitstream *, long);
    virtual void write_terminator_to_packet(long, c_bitstream *);
    virtual e_network_read_result read_from_packet(long, c_bitstream *, long, s_replication_incoming_request *, long *);
    virtual void process_incoming_request(s_replication_incoming_request *);
    virtual void notify_packet_acknowledged(long);
    virtual void notify_packet_retired(long, bool);

    void mark_packet_delivered(long packet_index, bool delivered);

public:
    __forceinline c_replication_event_manager_view() : m_initialized(false) {}
    __forceinline ~c_replication_event_manager_view() {}

    class c_event_record
    {
    public:
        c_replication_outgoing_event *m_event;
        c_event_record *m_next;

        static void operator delete(void *pointer)
        {
            ::network_heap_free_block(pointer);
        }
    };

    class c_packet_record
    {
    public:
        c_packet_record() : m_sequence_number(-1), m_events(0), m_next(0) {}

        long m_sequence_number;
        c_event_record *m_events;
        c_packet_record *m_next;

        static void operator delete(void *pointer)
        {
            ::network_heap_free_block(pointer);
        }
    };

    void reset();
    void send_event(c_replication_outgoing_event *outgoing_event)
    {
        outgoing_event->set_outgoing_bit(m_view_index);
        m_events_pending++;
    }

public: // Storage shared with simulation view initialization and teardown.
    bool m_initialized;
    bool m_client_disabled;
    long m_view_index;
    c_packet_record *m_packet_records;
    long m_packet_count;
    c_replication_event_manager *m_manager;
    long m_events_delivered;
    long m_events_pending;
    long m_events_in_transit;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char replication_event_manager_view_size[(sizeof(void *) != 4 || sizeof(c_replication_event_manager_view) == 0x28) ? 1 : -1];
#endif

#endif // NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_VIEW_H
