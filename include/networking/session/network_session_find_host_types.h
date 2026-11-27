#ifndef NETWORKING_SESSION_NETWORK_SESSION_FIND_HOST_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_FIND_HOST_TYPES_H

struct s_network_session_find_host
{
    long host_peer_index : 5;
    long second_host_peer_index : 5;
    long all_peers_connected : 1;
    long host_fully_connected : 1;
    long unused : 4;
    long connected_peer_mask : 16;
};

typedef char network_session_find_host_size[(sizeof(void *) != 4 || sizeof(s_network_session_find_host) == 4) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_FIND_HOST_TYPES_H
