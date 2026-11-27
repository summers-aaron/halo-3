#ifndef NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_TYPES_H

// UDP,
// voice datagrams, and TCP in this order.
enum e_transport_type
{
    _transport_type_udp = 0,
    _transport_type_vdp,
    _transport_type_tcp,
};

#endif // NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_TYPES_H
