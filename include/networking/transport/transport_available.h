#ifndef NETWORKING_TRANSPORT_TRANSPORT_AVAILABLE_H
#define NETWORKING_TRANSPORT_TRANSPORT_AVAILABLE_H

#include "networking/transport/transport_globals.h"

// Both link and socket operations require initialized, started transport.
__forceinline bool transport_available(const s_transport_globals &state)
{
    return state.initialized && state.started;
}

__forceinline bool transport_available()
{
    return transport_available(transport_startup_state);
}

#endif // NETWORKING_TRANSPORT_TRANSPORT_AVAILABLE_H
