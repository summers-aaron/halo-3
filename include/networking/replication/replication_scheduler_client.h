#ifndef NETWORKING_REPLICATION_REPLICATION_SCHEDULER_CLIENT_H
#define NETWORKING_REPLICATION_REPLICATION_SCHEDULER_CLIENT_H

#include "networking/delivery/network_delivery_types.h"
#include "networking/replication/replication_types.h"

class c_bitstream;

// Interface order and the client index are shared by replication_scheduler.cpp.
class __declspec(novtable) c_replication_scheduler_client
{
public:
    virtual bool has_data_to_transmit() = 0;
    virtual long build_outgoing_requests(const void *, long, s_replication_outgoing_request *) = 0;
    virtual long terminator_required_bits() const = 0;
    virtual void write_to_packet(LONG_PTR, long, const void *, long, c_bitstream *, long) = 0;
    virtual void write_terminator_to_packet(long, c_bitstream *) = 0;
    virtual e_network_read_result read_from_packet(long, c_bitstream *, long, s_replication_incoming_request *, long *) = 0;
    virtual void process_incoming_request(s_replication_incoming_request *) = 0;
    virtual void notify_packet_acknowledged(long) = 0;
    virtual void notify_packet_retired(long, bool) = 0;

    long m_client_index;
};

#endif // NETWORKING_REPLICATION_REPLICATION_SCHEDULER_CLIENT_H
