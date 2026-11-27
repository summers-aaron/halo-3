#ifndef NETWORKING_REPLICATION_REPLICATION_SCHEDULER_H
#define NETWORKING_REPLICATION_REPLICATION_SCHEDULER_H

// Functions defined in tu/networkinglibxbox/replication_scheduler.cpp.

#include "networking/delivery/network_channel_client.h"
#include "networking/replication/replication_scheduler_client.h"

class c_bitstream;
class c_simulation_view_telemetry_provider;
enum e_network_channel_closure_reason;

// March replication_scheduler.cpp identifies the client table and terminator
// accounting. The scheduler tracks three clients and their terminator bits.
class c_replication_scheduler : public c_network_channel_client
{
public:
    c_replication_scheduler() { m_initialized = false; }
    ~c_replication_scheduler() {}
    virtual const char *get_client_name() const;
    virtual bool connection_lost(e_network_channel_closure_reason *reason) const { return false; }
    virtual bool has_data_to_transmit(bool *has_urgent_data) const;
    void register_client(long client_index, c_replication_scheduler_client *client);
    virtual long space_required_bits(long packet_index, long packet_acknowledged_index);
    virtual bool write_to_packet(long packet_index, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits);
    virtual e_network_read_result read_from_packet(long *packet_index, c_bitstream *packet);
    virtual void notify_packet_acknowledged(long packet_index);
    virtual void notify_packet_retired(long packet_index, bool delivered);
    bool m_initialized;
    long m_channel_index;
    c_replication_scheduler_client *m_clients[3];
    long m_client_terminator_bits[3];
    long m_total_terminator_bits;
    c_simulation_view_telemetry_provider *m_telemetry_provider;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char replication_scheduler_size[(sizeof(void *) != 4 || sizeof(c_replication_scheduler) == 0x2c) ? 1 : -1];
#endif

struct s_replication_sort_element;

extern "C" void replication_scheduler_sort_requests(s_replication_sort_element *base, unsigned long count);

#endif // NETWORKING_REPLICATION_REPLICATION_SCHEDULER_H
