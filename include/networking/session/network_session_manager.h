#ifndef NETWORKING_SESSION_NETWORK_SESSION_MANAGER_H
#define NETWORKING_SESSION_NETWORK_SESSION_MANAGER_H

class c_network_session;
struct s_transport_secure_identifier;

// Tracks three sessions; get_session looks one up by its secure identifier.
class c_network_session_manager
{
public:
    __forceinline void register_session(c_network_session *session);
    c_network_session *get_session(const s_transport_secure_identifier *session_id);

    c_network_session *m_sessions[3];
};

typedef char network_session_manager_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_session_manager) == 12) )) ? 1 : -1];

#endif
