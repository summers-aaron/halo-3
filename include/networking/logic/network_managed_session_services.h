#ifndef NETWORKING_LOGIC_NETWORK_MANAGED_SESSION_SERVICES_H
#define NETWORKING_LOGIC_NETWORK_MANAGED_SESSION_SERVICES_H

#include "networking/transport/transport_qos_globals.h"

extern "C++" {
enum e_controller_index;
struct s_transport_session_description;
struct s_transport_secure_identifier;
struct s_transport_secure_key;

e_controller_index get_first_signed_in_controller_index();
extern "C" int XNetUnregisterKey(const s_transport_session_description *session_id);
extern "C" int XNetRegisterKey(const s_transport_secure_identifier *identifier, const s_transport_secure_key *key);
extern "C" unsigned long XUserSetContextEx(unsigned long user_index, unsigned long context_id, unsigned long context_value, void *overlapped);
}

#endif // NETWORKING_LOGIC_NETWORK_MANAGED_SESSION_SERVICES_H
