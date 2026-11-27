#ifndef NETWORKING_DELIVERY_NETWORK_CHANNEL_CLIENT_H
#define NETWORKING_DELIVERY_NETWORK_CHANNEL_CLIENT_H

#include "networking/delivery/network_channel_closure_reason_types.h"
#include "networking/delivery/network_delivery_types.h"

class __declspec(novtable) c_network_channel_client
{
public:
    c_network_channel_client() {}
    virtual const char *get_client_name() const = 0;
    virtual bool connection_lost(e_network_channel_closure_reason *reason) const = 0;
    virtual bool has_data_to_transmit(bool *urgent) const = 0;
    virtual long space_required_bits(long maximum_bits, long available_bits) = 0;
    virtual bool write_to_packet(long sequence_number, class c_bitstream *packet, long minimum_bits, long reserved_bits) = 0;
    virtual e_network_read_result read_from_packet(long *sequence_number, class c_bitstream *packet) = 0;
    virtual void notify_packet_acknowledged(long sequence_number) = 0;
    virtual void notify_packet_retired(long sequence_number, bool delivered) = 0;
};

#endif
