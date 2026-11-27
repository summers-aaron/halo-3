// networkinglibxbox/transport_security.obj: from the records 0x8f5e0 0x3a0c80 0x3a0d50 0x3a0de8 0x3a0e00 0x3a0e60 0x3a0f68 0x3a0fd8 0x3a10b0 0x3a1138 0x3a1190 0x3a1250 0x3a1298 0x3a1338 0x3a1398 0x3a1538 0x3a15a0 0x3a82e8
// @flags /TP /O2 /Oc
#include "memory/static_arrays.h"
#include "networking/transport/transport_globals.h"
// @address 0x3a0c80 ?transport_security_initialize@@YAXXZ
// @address 0x3a1138 ?transport_secure_connection_initiate@@YA_NPBUtransport_address@@@Z
// @address 0x3a10b0 ?transport_secure_address_get_local_machine_id@@YA_KXZ
// @address 0x3a1398 ?transport_secure_address_to_string@@YAPADPBUs_transport_secure_address@@PADJ_N2@Z
// @address 0x3a1298 ?transport_secure_random@@YAXJPAE@Z
// @address 0x3a1338 ?transport_local_machine_nonce_get@@YA_KXZ
// @address 0x3a82e8 transport_security_get_address_strings
#include <string.h>   /* csmemset/csmemcpy = the memset/memcpy intrinsics */

#include "networking/online/online_xenon.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_machine_id.h"
#include "networking/transport/transport_security_types.h"
#include "networking/in_addr_types.h"
#include "networking/transport/transport_platform_types.h"
#include "xbox/xnet.h"
#include "cseries/cseries.h"
#include "networking/network_globals.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/transport/transport_security_storage.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
// @address 0x7fc368 g_transport_security_globals
// @address 0x1f6c58 ?generate_random_seed@@YAKXZ

extern "C" {
// @address 0x119afb4 transport_secure_identifier_string
char transport_secure_identifier_string[32];
}

inline void transport_address_ipv4_build(transport_address *address, unsigned long ip, unsigned short port)
{
    address->address_length = 4;
    address->address.ipv4_address = ip;
    address->port = port;
}

bool transport_secure_address_resolve(void)
{
    XNADDR xnaddr;
    unsigned long address_status = XNetGetTitleXnAddr(&xnaddr);
    if (address_status != XNET_GET_XNADDR_PENDING && address_status != XNET_GET_XNADDR_NONE)
    {
        g_transport_security_globals.address_resolved = true;
        if (address_status & (XNET_GET_XNADDR_DHCP | XNET_GET_XNADDR_PPPOE | XNET_GET_XNADDR_STATIC))
            transport_address_ipv4_build(&g_transport_security_globals.address, xnaddr.ina, 0);
    }
    else
    {
        g_transport_security_globals.address_resolved = false;
        memset(&g_transport_security_globals.address, 0, sizeof(transport_address));
    }
    memcpy(&g_transport_security_globals.secure_address, &xnaddr, sizeof(XNADDR));
    return g_transport_security_globals.address_resolved;
}


static __forceinline void transport_security_register_transition_functions(transport_transition_function_t startup_function,
    transport_transition_function_t shutdown_function, transport_transition_function_t reset_function,
    void *callback_data)
{
    transport_startup_state.startup_functions[transport_startup_state.transition_function_count] = startup_function;
    transport_startup_state.shutdown_functions[transport_startup_state.transition_function_count] = shutdown_function;
    transport_startup_state.reset_functions[transport_startup_state.transition_function_count] = reset_function;
    transport_startup_state.callback_datas[transport_startup_state.transition_function_count] = callback_data;
    transport_startup_state.transition_function_count++;
}

// 0x3a0c80
void transport_security_initialize(void)
{
    memset(&g_transport_security_globals, 0, sizeof(g_transport_security_globals));
    transport_security_register_transition_functions(
        NULL, transport_secure_address_reset_for_new_networking_mode, transport_secure_address_reset_for_new_networking_mode, NULL);
    XNetGetTitleXnAddr((XNADDR *)&g_transport_security_globals.secure_address);
    memcpy(g_transport_security_globals.local_identifier, g_transport_security_globals.secure_address.identifier,
        sizeof(g_transport_security_globals.local_identifier));
    g_transport_security_globals.initialized = true;
}

// 0x3a0d50
bool transport_secure_key_create(s_transport_session_description *description, e_transport_platform platform)
{
    bool created = false;
    if (transport_secure_address_get(&description->secure_address))
    {
        if (platform == _transport_platform_xenon)
        {
            int error = XNetCreateKey(&description->secure_identifier, &description->secure_key);
            if (error == 0)
                created = true;
        }
        else
        {
            memset(&description->secure_identifier, 0, sizeof(description->secure_identifier));
            memset(&description->secure_key, 0, sizeof(description->secure_key));
            created = true;
        }
    }
    return created;
}

void transport_secure_address_reset_for_new_networking_mode(void *)
{
    g_transport_security_globals.address_resolved = false;
}

bool transport_secure_address_get(s_transport_secure_address *address)
{
    if (address != 0)
    {
        memcpy(address, &g_transport_security_globals.secure_address, sizeof(s_transport_secure_address));
    }
    return g_transport_security_globals.address_resolved;
}

bool transport_secure_address_decode(const s_transport_session_description *description, const s_transport_secure_address *secure_address,
    unsigned short port, transport_address *address)
{
    in_addr ina;
    bool decoded = false;
    if (XNetXnAddrToInAddr((const XNADDR *)secure_address, &description->secure_identifier, &ina) == 0)
    {
        decoded = true;
    }
    if (decoded)
    {
        transport_address_ipv4_build(address, ina.s_addr, port);
        return transport_address_valid(address);
    }
    return decoded;
}

// 0x3a0f68
bool transport_secure_identifier_retrieve(const transport_address *usable_address, e_transport_platform platform,
    s_transport_secure_identifier *secure_identifier, s_transport_secure_address *secure_address)
{
    bool retrieved = false;
    if ((unsigned short)usable_address->address_length == 4 && usable_address->address.ipv4_address != 0)
    {
        in_addr xbox_address;
        if (transport_address_get_insecure_xbox_address(usable_address, &xbox_address))
        {
            int error = XNetInAddrToXnAddr(xbox_address, (XNADDR *)secure_address, secure_identifier);
            if (error == 0)
                retrieved = true;
        }
    }
    return retrieved;
}


static __forceinline unsigned __int64 transport_secure_address_extract_identifier_into_qword(const s_transport_secure_address *address)
{
    const XNADDR *xnaddr = (const XNADDR *)address;
    unsigned __int64 identifier = *(const unsigned __int64 *)xnaddr->abEnet;
    identifier >>= 16;
    return identifier;
}

// 0x3a0fd8
bool transport_secure_address_compare(const s_transport_secure_address *address1, const s_transport_secure_address *address2)
{
    bool equal = false;
    const XNADDR *xnaddr1 = (const XNADDR *)address1;
    const XNADDR *xnaddr2 = (const XNADDR *)address2;
    unsigned __int64 machine_id1;
    unsigned __int64 machine_id2;
    if (xnaddr1->inaOnline != 0 && xnaddr2->inaOnline != 0 &&
        XNetXnAddrToMachineId(xnaddr1, &machine_id1) == 0 &&
        XNetXnAddrToMachineId(xnaddr2, &machine_id2) == 0)
    {
        equal = machine_id1 == machine_id2;
    }
    else
    {
        machine_id1 = transport_secure_address_extract_identifier_into_qword(address1);
        machine_id2 = transport_secure_address_extract_identifier_into_qword(address2);
        if (machine_id1 == machine_id2)
            equal = xnaddr1->ina == 0 || xnaddr2->ina == 0 || xnaddr1->ina == xnaddr2->ina;
    }
    return equal;
}

// 0x3a10b0
// dev build here: online_is_connected_to_live()) passes no argument; the accepted callers use this declaration

unsigned __int64 transport_secure_address_get_local_machine_id(void)
{
    unsigned __int64 machine_id = 0;
    s_transport_secure_address secure_address;
    if (transport_secure_address_get(&secure_address))
    {
        unsigned __int64 local_machine_id = 0;
        if (transport_secure_address_get_machine_id(&secure_address, &local_machine_id))
            machine_id = local_machine_id;
    }
    return machine_id;
}

// 0x3a1138
bool transport_secure_connection_initiate(const transport_address *address)
{
    bool success = false;
    in_addr xbox_address;
    if (transport_address_get_insecure_xbox_address(address, &xbox_address))
    {
        int error = XNetConnect(xbox_address);
        if (error == 0)
            success = true;
    }
    return success;
}

// 0x3a1190
e_transport_secure_connection transport_secure_connection_status(const transport_address *address)
{
    in_addr xbox_address;
    e_transport_secure_connection connection_status;
    if (transport_address_get_insecure_xbox_address(address, &xbox_address))
    {
        unsigned long status = XNetGetConnectStatus(xbox_address);
        switch (status)
        {
        case XNET_CONNECT_STATUS_IDLE: connection_status = _transport_secure_connection_idle; break;
        case XNET_CONNECT_STATUS_PENDING: connection_status = _transport_secure_connection_pending; break;
        case XNET_CONNECT_STATUS_CONNECTED: connection_status = _transport_secure_connection_connected; break;
        case XNET_CONNECT_STATUS_LOST: connection_status = _transport_secure_connection_lost; break;
        default: connection_status = _transport_secure_connection_unknown; break;
        }
    }
    else
        connection_status = _transport_secure_connection_unknown;
    return connection_status;
}

// 0x3a1298
void transport_secure_random(long random_length, unsigned char *random_data)
{
    bool generated = false;
    if (g_transport_security_globals.initialized)
    {
        XNetRandom(random_data, random_length);
        generated = true;
    }
    if (!generated)
    {
        unsigned long seed = generate_random_seed();
        for (long index = 0; index < random_length; index++)
            random_data[index] = (unsigned char)random_range(&seed, 0, 256);
    }
}

// @address 0x3a1250 transport_secure_nonce_generate
extern "C" unsigned __int64 transport_secure_nonce_generate(void)
{
    unsigned __int64 value;
    unsigned char nonzero;
    do {
        transport_secure_random(8, (unsigned char *)&value);
        nonzero = 1;
        if (value == 0)
            nonzero = 0;
    } while (nonzero == 0);
    return value;
}

// 0x3a1338: the machine's nonce, generated on first use and kept
unsigned __int64 transport_local_machine_nonce_get(void)
{
    if (!g_transport_security_globals.local_machine_nonce_valid) {
        g_transport_security_globals.local_machine_nonce = transport_secure_nonce_generate();
        g_transport_security_globals.local_machine_nonce_valid = true;
    }
    return g_transport_security_globals.local_machine_nonce;
}

// 0x3a1398
static __forceinline unsigned long transport_security_bytes_to_long(const unsigned char *bytes)
{
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}
char *transport_secure_address_to_string(
    const s_transport_secure_address *secure_address, char *string, long maximum_string_length, bool include_online, bool include_mac)
{
    XNADDR xnaddr;
    memcpy(&xnaddr, secure_address, sizeof(xnaddr));
    csnzprintf(string, maximum_string_length, "%hd.%hd.%hd.%hd",
        xnaddr.ina >> 24, (xnaddr.ina >> 16) & 0xff, (xnaddr.ina >> 8) & 0xff, xnaddr.ina & 0xff);
    if (include_mac)
    {
        csnzappendf(string, maximum_string_length, "!MAC=%02X:%02X:%02X:%02X:%02X:%02X",
            xnaddr.abEnet[0], xnaddr.abEnet[1], xnaddr.abEnet[2], xnaddr.abEnet[3], xnaddr.abEnet[4], xnaddr.abEnet[5]);
    }
    if (include_online)
    {
        csnzappendf(string, maximum_string_length, "!online=%hd.%hd.%hd.%hd:%hd",
            xnaddr.inaOnline >> 24, (xnaddr.inaOnline >> 16) & 0xff, (xnaddr.inaOnline >> 8) & 0xff, xnaddr.inaOnline & 0xff, xnaddr.wPortOnline);
        if (include_mac)
        {
            csnzappendf(string, maximum_string_length, "!online-ident=%08X:%08X:%08X:%08X:%08X",
                transport_security_bytes_to_long(&xnaddr.abOnline[0]), transport_security_bytes_to_long(&xnaddr.abOnline[4]),
                transport_security_bytes_to_long(&xnaddr.abOnline[8]), transport_security_bytes_to_long(&xnaddr.abOnline[12]),
                transport_security_bytes_to_long(&xnaddr.abOnline[16]));
        }
    }
    return string;
}


char *transport_secure_identifier_get_string(const s_transport_secure_identifier *identifier)
{
    csnzprintf(transport_secure_identifier_string, 32, "%02X%02X%02X%02X:%02X%02X%02X%02X",
        identifier->key_identifier[0], identifier->key_identifier[1], identifier->key_identifier[2], identifier->key_identifier[3],
        identifier->key_identifier[4], identifier->key_identifier[5], identifier->key_identifier[6], identifier->key_identifier[7]);
    return transport_secure_identifier_string;
}

// 0x3a15a0
bool transport_address_get_insecure_xbox_address(const transport_address *insecure_address, in_addr *xbox_address)
{
    bool valid = false;
    if (transport_address_valid(insecure_address) && (unsigned short)insecure_address->address_length == 4)
    {
        xbox_address->s_addr = insecure_address->address.ipv4_address;
        if (xbox_address->bytes[0] == 0)
            valid = true;
    }
    return valid;
}

// 0x3a82e8
extern "C" void transport_security_get_address_strings(c_static_string<16> *insecure_address, c_static_string<128> *secure_address)
{
    s_transport_secure_address local_secure_address;
    local_secure_address.ina[0] = 0;
    memset(local_secure_address.ina + 1, 0, sizeof(s_transport_secure_address) - 1);

    insecure_address->set("0.0.0.0");
    secure_address->set("0.0.0.0");

    if (transport_secure_address_get(&local_secure_address))
    {
        transport_address address = g_transport_security_globals.address;
        if (g_transport_security_globals.address_resolved)
        {
            char secure_address_string[128];
            secure_address_string[0] = 0;
            memset(secure_address_string + 1, 0, sizeof(secure_address_string) - 1);

            transport_secure_address_to_string(&local_secure_address, secure_address_string, sizeof(secure_address_string), true, true);
            secure_address->set(secure_address_string);
            insecure_address->print("%hd.%hd.%hd.%hd",
                address.address.ipv4_address >> 24,
                (address.address.ipv4_address >> 16) & 0xff,
                (address.address.ipv4_address >> 8) & 0xff,
                address.address.ipv4_address & 0xff);
        }
    }
}
