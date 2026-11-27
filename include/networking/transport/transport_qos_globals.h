#ifndef NETWORKING_TRANSPORT_TRANSPORT_QOS_GLOBALS_H
#define NETWORKING_TRANSPORT_TRANSPORT_QOS_GLOBALS_H

struct s_data_array;

// Startup clears the leading word; QoS tests its initialized byte and uses the
// attempt array at +4. The shared field name data is also required by the placement proxy.
struct s_transport_qos_globals
{
    union
    {
        unsigned char initialized;
        unsigned long initialized_storage;
    };
    union
    {
        s_data_array *data;
        // Preserve the tracker's accepted post-datum_delete reload of this slot.
        s_data_array *volatile v_data;
    };
};

typedef char transport_qos_globals_size[(sizeof(void *) != 4 || sizeof(s_transport_qos_globals) == 8) ? 1 : -1];

extern "C" s_transport_qos_globals transport_qos_globals;

#if defined(_XBOX) && !defined(__clang__)
#include "networking/logic/network_bandwidth_globals.h"

// The Xbox placement symbol spans two separately owned globals at 0x8280a2b8 and 0x8280a2c0.
struct s_transport_qos_bandwidth_storage
{
    s_transport_qos_globals qos;
    s_bandwidth_globals bandwidth;
};

typedef char transport_qos_bandwidth_offset[(offsetof(s_transport_qos_bandwidth_storage, bandwidth) == 8) ? 1 : -1];
extern "C" s_transport_qos_bandwidth_storage transport_qos_shutdown_globals;
#endif

inline s_transport_qos_globals &transport_qos_get_globals()
{
#if defined(_XBOX) && !defined(__clang__)
    return transport_qos_shutdown_globals.qos;
#else
    return transport_qos_globals;
#endif
}
#endif // NETWORKING_TRANSPORT_TRANSPORT_QOS_GLOBALS_H
