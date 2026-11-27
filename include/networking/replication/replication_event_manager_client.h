#ifndef NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_CLIENT_H
#define NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_CLIENT_H

#include "networking/delivery/network_delivery_types.h"
#include "networking/replication/replication_types.h"

class c_bitstream;
class c_replication_outgoing_event;

// March's handler vtable at 82038a1c records this six-method interface order.
class c_replication_event_manager_client
{
public:
    virtual e_network_read_result read_incoming_event(long, const long *, long, long *, s_replication_allocation_block *, c_bitstream *) = 0;
    virtual void process_incoming_event(long, const long *, long, s_replication_allocation_block *) = 0;
    virtual void write_outgoing_event(long, long, const void *, c_bitstream *) = 0;
    virtual void notify_outgoing_event_retired(const c_replication_outgoing_event *) = 0;
    virtual void calculate_requirements(c_replication_outgoing_event *, const void *, float *, long *) = 0;
    virtual void write_description_to_string(const c_replication_outgoing_event *, const void *, long, char *) = 0;
};

#endif // NETWORKING_REPLICATION_REPLICATION_EVENT_MANAGER_CLIENT_H
