#ifndef NETWORKING_TRACKED_SESSION_TYPES_H
#define NETWORKING_TRACKED_SESSION_TYPES_H

#include "memory/data_types.h"
#include "networking/logic/logic_session_tracker_types.h"

// A tracked session is one session-tracker slot: flags and per-type QoS
// completion live in the canonical session-data record (0x1468 bytes).
typedef s_session_tracker_session_data s_tracked_session;

#endif // NETWORKING_TRACKED_SESSION_TYPES_H
