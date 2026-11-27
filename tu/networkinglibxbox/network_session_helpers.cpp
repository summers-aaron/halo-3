// networkinglibxbox/network_session_helpers.obj: from the records 0x3a0ed8
// @flags /TP /O2 /Oc
// @address 0x3a0ed8 ?transport_secure_address_retrieve@@YA_NPBUtransport_address@@W4e_transport_platform@@PAUs_transport_secure_address@@@Z
#include <xtl.h>
#include <ppcintrinsics.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_address.h"

struct in_addr;


extern "C" {
}
bool transport_secure_address_retrieve(const transport_address *usable_address, e_transport_platform platform, s_transport_secure_address *secure_address)
{
    unsigned int ipv4_address;
    bool result;
    in_addr insecure_address;
    result = 0;
    if (usable_address->address_length == 4) {
        ipv4_address = usable_address->address.ipv4_address;
        if (ipv4_address != 0) {
            if (platform != 0) {
                memset(secure_address, 0, sizeof(*secure_address));
                memcpy(secure_address, &ipv4_address, sizeof(ipv4_address));
                result = 1;
            } else if (transport_address_get_insecure_xbox_address(usable_address, &insecure_address) != 0) {
                if (XNetInAddrToXnAddr(insecure_address, (XNADDR *)secure_address, 0) == 0) result = 1;
            }
        }
    }
    return result;
}
