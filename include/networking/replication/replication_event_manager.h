#ifndef NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_H
#define NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_H

#include "networking/replication/replication_event.h"
#include "networking/replication/replication_event_manager_view.h"

class c_replication_entity_manager;
class c_replication_event_manager_view;
class c_replication_outgoing_event;
class c_simulation_event_handler;

// March initialize/register_view identify the entity-manager pointer and
// view mask before the fifteen view pointers.
class c_replication_event_manager
{
    friend class c_replication_event_manager_view;

public:
    __forceinline void reset()
    {
        for (long view_index = 0; view_index < 15; view_index++)
            if (m_views[view_index])
                m_views[view_index]->reset();
    }
    void delete_outgoing_event(c_replication_outgoing_event *outgoing_event);
    void send_event(unsigned long view_mask, c_replication_outgoing_event *outgoing_event);
    __forceinline c_replication_outgoing_event *new_outgoing_event()
    {
        c_replication_outgoing_event *event = new c_replication_outgoing_event;
        if (event) {
            c_replication_outgoing_event **link = &m_outgoing_events;
            while (*link)
                link = &(*link)->m_next;
            *link = event;
            event->m_next = 0;
            m_outgoing_event_count++;
        }
        return event;
    }

private:
    void process_incoming_event(long event_type, const long *entity_references,
        long block_count, s_replication_allocation_block *payload_blocks);
    void notify_outgoing_event_retired(long view_index, c_replication_outgoing_event *outgoing_event, bool acknowledged);

public:
    void retire_outgoing_event(long view_index, c_replication_outgoing_event *outgoing_event, bool acknowledged)
    {
        notify_outgoing_event_retired(view_index, outgoing_event, acknowledged);
    }

    c_replication_entity_manager *m_entity_manager;
    c_simulation_event_handler *m_client;
    unsigned long m_view_mask;
    c_replication_event_manager_view *m_views[15];
    long m_outgoing_event_count;
    c_replication_outgoing_event *m_outgoing_events;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char replication_event_manager_size[(sizeof(void *) != 4 || sizeof(c_replication_event_manager) == 0x50) ? 1 : -1];
#endif

#endif // NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_H
