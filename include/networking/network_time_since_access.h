#ifndef INCLUDE_NETWORKING_NETWORK_TIME_SINCE_ACCESS_H
#define INCLUDE_NETWORKING_NETWORK_TIME_SINCE_ACCESS_H

#include "networking/network_time_access.h"
static __forceinline long network_time_since(unsigned long time)
{
    return (long)(network_time_get() - time);
}
#endif
