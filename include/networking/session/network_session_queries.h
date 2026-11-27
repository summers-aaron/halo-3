#ifndef INCLUDE_NETWORKING_SESSION_NETWORK_SESSION_QUERIES_H
#define INCLUDE_NETWORKING_SESSION_NETWORK_SESSION_QUERIES_H

class c_network_session;
struct s_network_session_peer_connectivity;

extern "C" void network_session_update_membership(c_network_session *session);
extern "C" void network_session_refuse_pending_player(c_network_session *session, long reason);
void network_session_calculate_peer_connectivity(c_network_session *session, s_network_session_peer_connectivity *connectivity);

#endif // INCLUDE_NETWORKING_SESSION_NETWORK_SESSION_QUERIES_H
