#ifndef NETWORKING_DELIVERY_NETWORK_CHANNEL_STATE_TYPES_H
#define NETWORKING_DELIVERY_NETWORK_CHANNEL_STATE_TYPES_H

enum e_network_channel_state
{
    _network_channel_state_none = 0,
    _network_channel_state_empty = 1,
    _network_channel_state_closed = 2,
    _network_channel_state_connecting = 3,
    _network_channel_state_established = 4,
    _network_channel_state_connected = 5
};

#endif // NETWORKING_DELIVERY_NETWORK_CHANNEL_STATE_TYPES_H
