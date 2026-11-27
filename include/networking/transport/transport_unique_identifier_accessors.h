#ifndef NETWORKING_TRANSPORT_TRANSPORT_UNIQUE_IDENTIFIER_ACCESSORS_H
#define NETWORKING_TRANSPORT_TRANSPORT_UNIQUE_IDENTIFIER_ACCESSORS_H

#include "networking/transport/transport_security.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include <string.h>

__forceinline void transport_secure_address_extract_identifier(const s_transport_secure_address *address, s_transport_unique_identifier *identifier)
{
    const s_transport_secure_address *xnaddr = address;
    memcpy(identifier, xnaddr->identifier, sizeof(xnaddr->identifier));
}

#endif
