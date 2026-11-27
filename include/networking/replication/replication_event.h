#ifndef NETWORKING_REPLICATION_REPLICATION_EVENT_H
#define NETWORKING_REPLICATION_REPLICATION_EVENT_H

#include "networking/network_memory.h"
#include "networking/network_time_types.h"
#include "cseries/crt_xdk.h"
#include <stddef.h>

static __forceinline unsigned long replication_event_network_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    return GetTickCount();
}

// Outgoing replication event carrying a type, a cancellation timer, two entity references and a payload.
class c_replication_outgoing_event
{
public:
    __forceinline c_replication_outgoing_event()
    {
        m_state = 0;
        m_type = -1;
        m_cancel_timer = -1;
        for (long reference_index = 0; reference_index < 2; reference_index++)
            m_entity_references[reference_index] = -1;
        m_payload_data = 0;
        m_payload_size = 0;
        m_outgoing_mask = 0;
        m_pending_acknowledgement_mask = 0;
    }
    static void *operator new(size_t size);
    __forceinline long get_type() const { return m_type; }
    const void *get_payload_data() const { return m_payload_data; }
    __forceinline long get_entity_reference(long reference_index) const { return m_entity_references[reference_index]; }
    __forceinline void set_type(long type) { m_type = type; }
    __forceinline void set_cancel_timer(long cancel_timer) { m_cancel_timer = cancel_timer; }
    __forceinline void set_entity_reference(long reference_index, long entity_index) { m_entity_references[reference_index] = entity_index; }
    bool set_payload(const void *payload, long payload_size);
    ~c_replication_outgoing_event()
    {
        if (m_payload_data)
            ::network_heap_free_block(m_payload_data);
    }
    static void operator delete(void *pointer)
    {
        ::network_heap_free_block(pointer);
    }

    void make_ready()
    {
        m_state = 1;
        m_creation_time = replication_event_network_time_get();
    }
    void set_outgoing_bit(long view_index) { m_outgoing_mask |= (1 << view_index); }
    void clear_outgoing_bit(long view_index) { m_outgoing_mask &= ~(1 << view_index); }
    unsigned long get_outgoing_mask() const { return m_outgoing_mask; }

    long m_state;
    long m_type;
    unsigned long m_creation_time;
    long m_cancel_timer;
    long m_entity_references[2];
    void *m_payload_data;
    long m_payload_size;
    unsigned long m_outgoing_mask;
    unsigned long m_pending_acknowledgement_mask;
    c_replication_outgoing_event *m_next;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char replication_outgoing_event_size[(sizeof(void *) != 4 || sizeof(c_replication_outgoing_event) == 0x2c) ? 1 : -1];
typedef char replication_outgoing_event_payload_offset[(sizeof(void *) != 4 || offsetof(c_replication_outgoing_event, m_payload_data) == 0x18) ? 1 : -1];
#endif

#endif // NETWORKING_REPLICATION_REPLICATION_EVENT_H
