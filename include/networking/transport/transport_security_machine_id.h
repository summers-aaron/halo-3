#ifndef NETWORKING_TRANSPORT_TRANSPORT_SECURITY_MACHINE_ID_H
#define NETWORKING_TRANSPORT_TRANSPORT_SECURITY_MACHINE_ID_H

#include "networking/transport/transport_security.h"
#include "networking/online/online_xenon.h"
#include "xbox/xnet.h"

static __forceinline bool transport_secure_address_get_machine_id(
    const s_transport_secure_address *secure_address, unsigned __int64 *machine_id)
{
    bool success = false;
    if (::online_local_xuid_is_online_enabled() && XNetXnAddrToMachineId((const XNADDR *)secure_address, machine_id) == 0)
        success = true;
    return success;
}

#endif
