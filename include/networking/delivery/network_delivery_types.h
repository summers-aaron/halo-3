#ifndef NETWORKING_DELIVERY_NETWORK_DELIVERY_TYPES_H
#define NETWORKING_DELIVERY_NETWORK_DELIVERY_TYPES_H

enum e_network_read_result
{
    _network_read_result_success,
    _network_read_result_failure,
    _network_read_result_discard,
    _network_read_result_error,
};

// Connection retirement reports delivery at 1/2/3; subsequent acknowledgement
// feedback is 4/5. The channel and observer consume those distinct events.
enum e_network_packet_event
{
    _network_packet_event_none,
    _network_packet_event_delivered,
    _network_packet_event_delivered_late,
    _network_packet_event_lost,
    _network_packet_event_acknowledged,
    _network_packet_event_acknowledged_late,
};

#endif // NETWORKING_DELIVERY_NETWORK_DELIVERY_TYPES_H
