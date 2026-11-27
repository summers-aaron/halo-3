#ifndef NETWORKING_TRANSPORT_TRANSPORT_GLOBALS_H
#define NETWORKING_TRANSPORT_TRANSPORT_GLOBALS_H

#include "networking/transport/transport_types.h"

// The placed transport.obj state, defined in tu/networkinglibxbox/transport.cpp
// (with internal linkage on the Xbox image; see the match note there).
extern "C" s_transport_globals transport_startup_state;

#endif // NETWORKING_TRANSPORT_TRANSPORT_GLOBALS_H
