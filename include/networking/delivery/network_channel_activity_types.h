#ifndef NETWORKING_DELIVERY_NETWORK_CHANNEL_ACTIVITY_TYPES_H
#define NETWORKING_DELIVERY_NETWORK_CHANNEL_ACTIVITY_TYPES_H

enum e_network_channel_activity
{
    _network_channel_activity_send_packet = 0,
    _network_channel_activity_send_sequenced_packet = 1,
    _network_channel_activity_send_voice_packet = 2,
    _network_channel_activity_receive_packet = 3,
    _network_channel_activity_packet_delivery = 4,
    _network_channel_activity_receive_voice = 5
};

#endif // NETWORKING_DELIVERY_NETWORK_CHANNEL_ACTIVITY_TYPES_H
