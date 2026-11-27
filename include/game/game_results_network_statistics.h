#ifndef GAME_GAME_RESULTS_NETWORK_STATISTICS_H
#define GAME_GAME_RESULTS_NETWORK_STATISTICS_H

#include "networking/transport/transport_security.h"

#pragma pack(push, 1)
struct s_game_results_network_peer_statistics
{
    unsigned char valid;
    unsigned char has_gamestate_bonus;
    unsigned __int64 party_nonce;
    s_transport_secure_address secure_address;     // (10)
    long peer_version;                             // (46)
    long upstream_bandwidth;                       // (50)
    long downstream_bandwidth;                     // (54)
    long nat_type;                                 // (58)
    unsigned short peer_connectivity_mask;         // (62)
    unsigned short peer_probe_mask;                // (64)
};

struct s_game_results_network_statistics
{
    unsigned char squad;
    unsigned char group;
    s_game_results_network_peer_statistics peers[16]; // (1056 bytes)
};
#pragma pack(pop)

extern s_game_results_network_statistics g_game_results_network_statistics;

#endif
