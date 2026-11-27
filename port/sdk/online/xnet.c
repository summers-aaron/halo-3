// port/sdk/online (both hosts): the Xbox network (XNet) and the Winsock error state.
// With System Link on (HALO3_LAN=1, port/sdk/online/lan.c; Xbox Live, HALO3_LIVE_SERVER, turns it on too) XNet runs
// over UDP: XNetStartup opens this console's port, the keys, secure addresses, connections and QoS are the LAN's; with
// Live a session's QoS listener being enabled or disabled is reported to the Live server, and XNetQosRelease also
// frees the results of XNetQosServiceLookup (xlive_offline.cpp). Otherwise, or while XNet is not started, it answers
// as an Xbox with no network link, as it always has. The Winsock calls themselves are winsock_offline.cpp (game ABI),
// the Live / XSession / XContent surface xlive_offline.cpp.
#include "lan.h"
#include "live.h"
#include "../runtime/port_runtime.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define PORT_XNET_GET_XNADDR_NONE 0x00000001ul
#define PORT_XNET_CONNECT_STATUS_IDLE 0ul
#define PORT_WSAENETDOWN 10050
#define PORT_XNET_XNKID_MASK 0xf0
#define PORT_XNET_XNKID_SYSTEM_LINK 0x00

PORT_MS_ABI int XNetStartup(const void *params)
{
    return port_lan_startup((const unsigned char *)params);
}

PORT_MS_ABI int XNetCleanup(void)
{
    port_lan_cleanup();
    return 0;
}

PORT_MS_ABI u32 XNetGetTitleXnAddr(void *xnaddr)
{
    // XNET_GET_XNADDR_NONE without a network link: no address, but the console's MAC address (abEnet) all the same
    return port_lan_title_xnaddr(xnaddr);
}

PORT_MS_ABI u32 XNetGetEthernetLinkStatus(void)
{
    return port_lan_ethernet_link_status(); // 0: no cable connected
}

// XNetRandom: secure random bytes (keys, nonces), from the host.
PORT_MS_ABI int XNetRandom(u8 *buffer, unsigned int count)
{
    port_host_random(buffer, count);
    return 0;
}

// A System Link key: the XNKID's type is its high nibble (XNET_XNKID_MASK), 0 for System Link, as the Xbox gives a
// console that is not on Xbox Live (XNetXnKidIsSystemLink holds); the rest is random.
PORT_MS_ABI int XNetCreateKey(void *xnkid, void *xnkey)
{
    if (xnkid != 0) {
        XNetRandom((u8 *)xnkid, 8);
        u8 *id = (u8 *)xnkid;
        id[0] = (u8)((id[0] & ~PORT_XNET_XNKID_MASK) | PORT_XNET_XNKID_SYSTEM_LINK);
    }
    if (xnkey != 0)
        XNetRandom((u8 *)xnkey, 16);
    return 0;
}

PORT_MS_ABI int XNetRegisterKey(const void *xnkid, const void *xnkey)
{
    if (port_lan_active())
        return port_lan_register_key(xnkid, xnkey);
    return 0;
}

PORT_MS_ABI int XNetUnregisterKey(const void *xnkid)
{
    if (port_lan_active())
        return port_lan_unregister_key(xnkid);
    return 0;
}

PORT_MS_ABI int XNetXnAddrToInAddr(const void *xnaddr, const void *xnkid, u32 *in_addr_out)
{
    if (port_lan_active()) {
        unsigned int secure = 0;
        int error = port_lan_xnaddr_to_inaddr(xnaddr, xnkid, &secure);
        if (error == 0 && in_addr_out != 0)
            *in_addr_out = secure;
        return error;
    }
    if (in_addr_out != 0)
        *in_addr_out = 0x0100007fu; // 127.0.0.1
    return 0;
}

PORT_MS_ABI int XNetInAddrToXnAddr(u32 in_addr_val, void *xnaddr, void *xnkid)
{
    if (port_lan_active())
        return port_lan_inaddr_to_xnaddr((unsigned int)in_addr_val, xnaddr, xnkid);
    if (xnaddr != 0)
        __builtin_memset(xnaddr, 0, 36);
    if (xnkid != 0)
        __builtin_memset(xnkid, 0, 8);
    return 0;
}

PORT_MS_ABI int XNetUnregisterInAddr(u32 in_addr_val)
{
    if (port_lan_active())
        return port_lan_unregister_inaddr((unsigned int)in_addr_val);
    return 0;
}

PORT_MS_ABI int XNetXnAddrToMachineId(const void *xnaddr, u64 *machine_id)
{
    if (port_lan_active())
        return port_lan_machine_id(xnaddr, machine_id);
    if (machine_id != 0)
        *machine_id = 0x00000000deadbeefull;
    return 0;
}

PORT_MS_ABI u32 XNetGetConnectStatus(u32 in_addr_val)
{
    if (port_lan_active())
        return port_lan_connect_status((unsigned int)in_addr_val);
    return PORT_XNET_CONNECT_STATUS_IDLE;
}

PORT_MS_ABI int XNetConnect(u32 in_addr_val)
{
    if (port_lan_active())
        return port_lan_connect_peer((unsigned int)in_addr_val);
    return 0;
}

PORT_MS_ABI int XNetQosListen(const void *xnkid, const u8 *pb, unsigned int cb, u32 bits_per_sec, u32 flags)
{
    if (port_lan_active()) {
        int r = port_lan_qos_listen(xnkid, pb, cb, (unsigned int)bits_per_sec, (unsigned int)flags);
        if (r == 0 && port_live_enabled() && xnkid != 0) {
            if ((flags & 0x01ul) != 0)
                port_live_qos_listen_changed((const unsigned char *)xnkid, 1);
            else if ((flags & 0x02ul) != 0)
                port_live_qos_listen_changed((const unsigned char *)xnkid, 0);
        }
        return r;
    }
    return 0;
}

PORT_MS_ABI int XNetQosLookup(unsigned int cxna, const void *const *axna, const void *const *axnkid, const void *const *axnkey,
    unsigned int cina, const void *aina, const u32 *aina_svc, unsigned int probes, u32 bits_per_sec, u32 flags,
    void *event, void **qos)
{
    (void)aina; (void)aina_svc; (void)flags;
    if (port_lan_active())
        return port_lan_qos_lookup(cxna, axna, axnkid, axnkey, cina, probes, (unsigned int)bits_per_sec, event, qos);
    if (qos != 0)
        *qos = 0;
    return PORT_WSAENETDOWN;
}

PORT_MS_ABI int port_live_qos_service_release(void *qos);

PORT_MS_ABI int XNetQosRelease(void *qos)
{
    if (port_live_qos_service_release(qos))
        return 0;
    // frees a lookup even after XNetCleanup; without System Link running the release succeeds, as it always has
    int error = port_lan_qos_release(qos);
    return port_lan_active() ? error : 0;
}

// The QoS listener's counters (XNQOSLISTENSTATS): what this console's listener for the key received and sent.
PORT_MS_ABI int XNetQosGetListenStats(const void *xnkid, void *stats)
{
    return port_lan_qos_listen_stats(xnkid, stats);
}

PORT_MS_ABI int WSAStartup(u16 version_requested, void *wsa_data)
{
    if (wsa_data != 0) {
        __builtin_memset(wsa_data, 0, 400);
        *(u16 *)wsa_data = version_requested;
    }
    return 0;
}

PORT_MS_ABI int WSACleanup(void)
{
    return 0;
}

// Winsock errors are the thread's last error (winsock_offline.cpp sets them), as on Windows.
PORT_MS_ABI u32 GetLastError(void);
PORT_MS_ABI void SetLastError(u32 error);
PORT_MS_ABI void WSASetLastError(int error)
{
    SetLastError((u32)error);
}
PORT_MS_ABI int WSAGetLastError(void)
{
    return (int)GetLastError();
}
