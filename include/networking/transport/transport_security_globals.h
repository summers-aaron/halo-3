#ifndef NETWORKING_TRANSPORT_TRANSPORT_SECURITY_GLOBALS_H
#define NETWORKING_TRANSPORT_TRANSPORT_SECURITY_GLOBALS_H

#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_address.h"
#include <stddef.h>
#include "game/player_identifier_types.h"

struct s_transport_security_key
{
    bool active;
    bool generated_locally;
    long key_platform;
    s_transport_secure_identifier key_id;
    s_transport_secure_key key;
};

struct s_transport_security_globals
{
    bool initialized;
    bool address_resolved;
    s_transport_secure_address secure_address;
    transport_address address;
    bool local_machine_nonce_valid;
    unsigned __int64 local_machine_nonce;
    union {
        unsigned char local_identifier[6];
        s_machine_identifier local_machine_identifier;
    };
    s_transport_security_key keys[10];
};

typedef char transport_security_globals_size[(sizeof(s_transport_security_globals) == 0x190) ? 1 : -1];
typedef char transport_security_key_size[(sizeof(s_transport_security_key) == 0x20) ? 1 : -1];
typedef char transport_security_globals_keys_offset[(offsetof(s_transport_security_globals, keys) == 0x50) ? 1 : -1];
typedef char transport_security_globals_identifier_offset[(offsetof(s_transport_security_globals, local_identifier) == 0x48) ? 1 : -1];
typedef char transport_security_globals_machine_offset[(offsetof(s_transport_security_globals, local_machine_identifier) == 0x48) ? 1 : -1];

#endif // NETWORKING_TRANSPORT_TRANSPORT_SECURITY_GLOBALS_H
