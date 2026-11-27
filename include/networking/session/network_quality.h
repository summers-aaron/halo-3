#ifndef NETWORKING_SESSION_NETWORK_QUALITY_H
#define NETWORKING_SESSION_NETWORK_QUALITY_H

#include <stddef.h>

struct s_network_quality_data
{
    long latency_minimum;
    long latency_average;
    long latency_maximum;
    long packet_loss;
    long upstream_bandwidth;
    long downstream_bandwidth;
    long words18[2];
};

typedef char network_quality_downstream_offset[(offsetof(s_network_quality_data, downstream_bandwidth) == 0x14) ? 1 : -1];
typedef char network_quality_size[(sizeof(s_network_quality_data) == 0x20) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_QUALITY_H
