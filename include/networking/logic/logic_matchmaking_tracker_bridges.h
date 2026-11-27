#ifndef NETWORKING_LOGIC_MATCHMAKING_TRACKER_BRIDGES_H
#define NETWORKING_LOGIC_MATCHMAKING_TRACKER_BRIDGES_H

#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"

class c_session_tracker;
struct s_network_session_tracker_session_data;
struct s_transport_session_description;

extern "C" {
// match: 0x43f4f8: retain opaque tracker bridges; canonical callee summaries hoist the latency load.
bool matchmaking_tracker_mark_session_unsuitable(const s_transport_session_description *description,
    e_network_session_tracker_session_unsuitable_reason reason);
bool matchmaking_tracker_mark_session_undesirable(const s_transport_session_description *description, long reason);
bool matchmaking_tracker_get_session_data(c_session_tracker *tracker, long session_index, s_network_session_tracker_session_data *data);
}
#endif
