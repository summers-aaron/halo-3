#ifndef NETWORKING_NETWORK_TIME_ACCESS_H
#define NETWORKING_NETWORK_TIME_ACCESS_H

#include "networking/network_time_types.h"
#include "cseries/crt_xdk.h"

static __forceinline unsigned long network_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    else
        return GetTickCount();
}

// Evaluate the start-time argument before querying the possibly external clock.
static __forceinline long network_time_since_inline(unsigned long start_time)
{
    return network_time_get() - start_time;
}

#endif
