#ifndef NETWORKING_SESSION_CREATION_H
#define NETWORKING_SESSION_CREATION_H
#include "cseries/cseries_wrapped.h"
#include "networking/session/network_session_class.h"
#include "networking/online/online_managed_session_access.h"
#include "networking/transport/transport_security_types.h"
enum e_transport_platform;
struct s_online_context;

// Session-side ABI declarations; the host entry retains its C linkage.
extern "C" long managed_session_create_host(e_network_session_class session_class,
    c_flags<e_managed_session_creation_flags, unsigned char, 4> flags,
    e_transport_platform transport_platform, long, long, const s_online_context *online_context, long);
long managed_session_create_client(e_network_session_class session_class,
    c_flags<e_managed_session_creation_flags, unsigned char, 4> flags,
    e_transport_platform transport_platform, long, long, const s_transport_secure_identifier *session_id,
    const s_transport_secure_address *address, const s_transport_secure_key *key,
    const s_online_context *online_context, long managed_session_flags);


#endif
