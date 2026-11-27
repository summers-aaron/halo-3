#ifndef NETWORKING_TRANSPORT_TRANSPORT_QOS_TARGET_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_QOS_TARGET_TYPES_H

#include "networking/transport/transport_security_types.h"

struct s_transport_qos_target
{
    s_transport_secure_identifier session_id;
    s_transport_secure_key key;
    s_transport_secure_address address;
};

typedef char s_transport_qos_target_size[(sizeof(s_transport_qos_target) == 0x3c) ? 1 : -1];

#endif // NETWORKING_TRANSPORT_TRANSPORT_QOS_TARGET_TYPES_H
