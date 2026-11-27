#ifndef NETWORKING_SESSION_NETWORK_OBSERVER_H
#define NETWORKING_SESSION_NETWORK_OBSERVER_H

// Functions defined in tu/networkinglibxbox/network_observer.cpp.

class c_network_observer;
class c_network_session_membership;

#include "networking/session/network_observer_join_queries.h"
extern "C" bool network_observer_is_peer_joining(void);
extern "C" bool network_owner_entry_is_pending(c_network_session_membership *owner, long index);
extern "C" long network_observer_initialize_stream_bandwidth(c_network_observer *observer, long observer_index, long client_count,
	long maximum_bandwidth_bps, long initial_bandwidth_bps, long minimum_bandwidth_bps, const char *unused);
extern "C" long network_observer_update_stream_bandwidth(c_network_observer *observer, long total_bandwidth_bps, long stable_bandwidth_bps, bool stable,
	bool voice, long stream_count, long host_count, long client_count, unsigned long growing_mask, long requiring_count);

#endif // NETWORKING_SESSION_NETWORK_OBSERVER_H
