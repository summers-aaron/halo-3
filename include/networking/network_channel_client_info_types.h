#ifndef NETWORKING_NETWORK_CHANNEL_CLIENT_INFO_TYPES_H
#define NETWORKING_NETWORK_CHANNEL_CLIENT_INFO_TYPES_H

#include "memory/data_types.h"

class c_network_channel_client;

struct s_network_channel_client_info
{
    unsigned long flags;
    c_network_channel_client *client;
};

#endif // NETWORKING_NETWORK_CHANNEL_CLIENT_INFO_TYPES_H
