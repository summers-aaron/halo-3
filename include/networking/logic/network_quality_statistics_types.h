#ifndef NETWORKING_LOGIC_NETWORK_QUALITY_STATISTICS_TYPES_H
#define NETWORKING_LOGIC_NETWORK_QUALITY_STATISTICS_TYPES_H

#include "networking/logic/network_bandwidth_globals.h"

struct s_network_observer_quality_statistics
{
    unsigned __int64 client_badness[2];
    unsigned __int64 host_badness[2];
};

struct s_network_observer_quality_statistics_set
{
    s_network_observer_quality_statistics type[2];
    unsigned __int64 connectivity[2];
};

// The observer's connection quality history and bandwidth history, saved to and restored from the banhammer
// statistics file as one block.
struct s_network_quality_statistics
{
    s_network_observer_quality_statistics_set quality_statistics;
    s_bandwidth_history bandwidth_history;
};
typedef char network_quality_statistics_bandwidth_offset[(offsetof(s_network_quality_statistics, bandwidth_history) == 0x50) ? 1 : -1];
typedef char network_quality_statistics_size[(sizeof(s_network_quality_statistics) == 0xc0) ? 1 : -1];

#endif // NETWORKING_LOGIC_NETWORK_QUALITY_STATISTICS_TYPES_H
