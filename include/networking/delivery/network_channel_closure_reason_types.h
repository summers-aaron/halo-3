#ifndef NETWORKING_DELIVERY_NETWORK_CHANNEL_CLOSURE_REASON_TYPES_H
#define NETWORKING_DELIVERY_NETWORK_CHANNEL_CLOSURE_REASON_TYPES_H

enum e_network_channel_closure_reason
{
    _network_channel_closure_reason_none = 0,
    _network_channel_closure_reason_link_destroyed = 1,
    _network_channel_closure_reason_link_refused_listen = 2,
    _network_channel_closure_reason_channel_deleted = 3,
    _network_channel_closure_reason_connect_timeout = 4,
    _network_channel_closure_reason_connect_refused = 5,
    _network_channel_closure_reason_connect_reinitiate = 6,
    _network_channel_closure_reason_establish_timeout = 7,
    _network_channel_closure_reason_address_change = 8,
    _network_channel_closure_reason_destination_unreachable = 9,
    _network_channel_closure_reason_remote_closure = 10,
    _network_channel_closure_reason_connection_lost = 11,
    _network_channel_closure_reason_message_queue_lost = 12,
    _network_channel_closure_reason_security_lost = 13,
    _network_channel_closure_reason_observer_released = 14,
    _network_channel_closure_reason_observer_refused = 15,
    _network_channel_closure_reason_timeout = 16,
    _network_channel_closure_reason_observer_reset = 17,
    _network_channel_closure_reason_observer_reset_security = 18,

    k_network_channel_closure_reason_count = 19
};

#endif // NETWORKING_DELIVERY_NETWORK_CHANNEL_CLOSURE_REASON_TYPES_H
