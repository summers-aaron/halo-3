#ifndef NETWORKING_TRANSPORT_TRANSPORT_SECURITY_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_SECURITY_TYPES_H

#include "networking/transport/transport_security.h"
#include <stddef.h>

// XNet secure-session key material is byte-aligned (XNKID/XNKEY). The March debug build's transport_security.obj
// asserts k_transport_secure_identifier_length==sizeof(XNKID) and k_transport_secure_key_length==sizeof(XNKEY).
// The identifier is the XNet key identifier (XNKID); the key is the XNet key-exchange key (XNKEY). Both are opaque.
enum
{
    k_transport_secure_identifier_length = 8,
    k_transport_secure_key_length = 16
};

struct s_transport_secure_identifier
{
    unsigned char key_identifier[k_transport_secure_identifier_length];
};
struct s_transport_secure_key
{
    unsigned char key_exchange_key[k_transport_secure_key_length];
};
struct s_transport_session_description
{
    s_transport_secure_identifier secure_identifier;
    s_transport_secure_address secure_address;
    s_transport_secure_key secure_key;
};

typedef char transport_secure_identifier_size[(sizeof(s_transport_secure_identifier) == 8) ? 1 : -1];
typedef char transport_secure_identifier_alignment[(__alignof(s_transport_secure_identifier) == 1) ? 1 : -1];
typedef char transport_secure_key_size[(sizeof(s_transport_secure_key) == 16) ? 1 : -1];
typedef char transport_secure_key_alignment[(__alignof(s_transport_secure_key) == 1) ? 1 : -1];
typedef char transport_session_description_address[(offsetof(s_transport_session_description, secure_address) == 8) ? 1 : -1];
typedef char transport_session_description_key[(offsetof(s_transport_session_description, secure_key) == 44) ? 1 : -1];
typedef char transport_session_description_size[(sizeof(s_transport_session_description) == 60) ? 1 : -1];

#endif // NETWORKING_TRANSPORT_TRANSPORT_SECURITY_TYPES_H
