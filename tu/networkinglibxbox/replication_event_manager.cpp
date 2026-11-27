// networkinglibxbox/replication_event_manager.obj: from the records 0x416990 0x416a20 0x416c00
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "networking/network_memory.h"
// @address 0x416990 ?delete_outgoing_event@c_replication_event_manager@@QAAXPAVc_replication_outgoing_event@@@Z
// @address 0x416a20 ?send_event@c_replication_event_manager@@QAAXKPAVc_replication_outgoing_event@@@Z
// @address 0x416c00 ?notify_outgoing_event_retired@c_replication_event_manager@@AAAXJPAVc_replication_outgoing_event@@_N@Z
// @address 0x80608 GetTickCount

#include "networking/replication/replication_event.h"
#include "networking/replication/replication_event_manager.h"
#include "networking/replication/replication_event_manager_view.h"
#include "simulation/simulation_event_handler_types.h"
void c_replication_event_manager::delete_outgoing_event(c_replication_outgoing_event *outgoing_event)
{
    c_replication_outgoing_event **event_ptr = &m_outgoing_events;
    while (*event_ptr != 0)
    {
        if (*event_ptr == outgoing_event)
        {
            *event_ptr = outgoing_event->m_next;
            break;
        }
        event_ptr = &(*event_ptr)->m_next;
    }
    m_outgoing_event_count--;
    delete outgoing_event;
}

void c_replication_event_manager::send_event(unsigned long view_mask, c_replication_outgoing_event *outgoing_event)
{
    bool sent = false;
    outgoing_event->make_ready();
    for (long view_index = 0; view_index < 15; view_index++)
    {
        if ((view_mask & (1 << view_index)) != 0 && m_views[view_index] != 0)
        {
            m_views[view_index]->send_event(outgoing_event);
            sent = true;
        }
    }
    if (!sent)
    {
        m_client->c_simulation_event_handler::notify_outgoing_event_retired(outgoing_event);
        delete_outgoing_event(outgoing_event);
    }
}

void c_replication_event_manager::notify_outgoing_event_retired(long view_index, c_replication_outgoing_event *outgoing_event, bool acknowledged)
{
    outgoing_event->clear_outgoing_bit(view_index);
    if (outgoing_event->get_outgoing_mask() == 0)
    {
        m_client->c_simulation_event_handler::notify_outgoing_event_retired(outgoing_event);
        delete_outgoing_event(outgoing_event);
    }
}
