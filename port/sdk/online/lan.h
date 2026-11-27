// port/sdk/online: System Link over UDP, the interface between the game's Winsock and XNet (winsock_offline.cpp,
// built like the game against the XDK headers; xnet.c) and the System Link core (lan.c, host code). Plain C types
// only: both sides include it.
//
// Addresses and ports are numbers in host order, which is what the game holds (htonl and htons are the identity in
// winsockx.h): 255.255.255.255:1001 is { 0xffffffff, 1001 }. Results are 0 or a Winsock error code (the values of
// winsockx.h / winerror.h, listed here as PORT_LAN_WSAE*).
//
// What the game sees is an Xbox on a System Link LAN (port/sdk/online/REPORT_LAN_UDP.md):
//   - each console's XNet runs over one host UDP port (HALO3_LAN_PORT, 3074 by default: the Xbox's), and the title's
//     own UDP and VDP ports (1000, 1001) are carried inside it;
//   - a peer is reached by the secure address XNetXnAddrToInAddr gives for its XNADDR and the session's XNKID, an
//     opaque handle 0.h.l.0 (the Xbox's are in 0.0.0.0/8); this console's own XNADDR maps to 127.0.0.1;
//   - broadcasts to 255.255.255.255 reach every console on the LAN and arrive from 0.0.0.1.
#ifndef PORT_ONLINE_LAN_H
#define PORT_ONLINE_LAN_H

#include "../runtime/port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PORT_LAN_WSAEINTR = 10004,
    PORT_LAN_WSAEACCES = 10013,
    PORT_LAN_WSAEFAULT = 10014,
    PORT_LAN_WSAEINVAL = 10022,
    PORT_LAN_WSAEWOULDBLOCK = 10035,
    PORT_LAN_WSAEALREADY = 10037,
    PORT_LAN_WSAENOTSOCK = 10038,
    PORT_LAN_WSAEDESTADDRREQ = 10039,
    PORT_LAN_WSAEMSGSIZE = 10040,
    PORT_LAN_WSAENOPROTOOPT = 10042,
    PORT_LAN_WSAEPROTONOSUPPORT = 10043,
    PORT_LAN_WSAEOPNOTSUPP = 10045,
    PORT_LAN_WSAEAFNOSUPPORT = 10047,
    PORT_LAN_WSAEADDRINUSE = 10048,
    PORT_LAN_WSAEADDRNOTAVAIL = 10049,
    PORT_LAN_WSAENETDOWN = 10050,
    PORT_LAN_WSAENOBUFS = 10055,
    PORT_LAN_WSAEISCONN = 10056,
    PORT_LAN_WSAENOTCONN = 10057,
    PORT_LAN_WSAEHOSTUNREACH = 10065,
};

// The largest datagram a title may send: the Xbox's XNet payload limit (the game sizes its packets to it,
// c_network_link::adjust_packet_size).
#define PORT_LAN_MAX_DATAGRAM 1264

// The XNet layouts the host side mirrors (winsock_offline.cpp checks them against winsockx.h).
#define PORT_LAN_XNADDR_SIZE 36
#define PORT_LAN_XNKID_SIZE 8
#define PORT_LAN_XNKEY_SIZE 16
#define PORT_LAN_STARTUP_PARAMS_SIZE 13
#define PORT_LAN_XNQOSINFO_SIZE 24
#define PORT_LAN_LISTEN_STATS_SIZE 28

typedef struct port_lan_address {
    unsigned int address;
    unsigned short port;
} port_lan_address;

// ---- the title's sockets (winsock_offline.cpp) ----

// Whether System Link is running (HALO3_LAN=1 and XNetStartup opened the host port). Without it every socket call
// answers as an Xbox with no network link (WSAENETDOWN).
PORT_MS_ABI int port_lan_active(void);

PORT_MS_ABI int port_lan_socket(int vdp, unsigned int *handle);
// WSAENOTSOCK when handle is not an open socket.
PORT_MS_ABI int port_lan_socket_valid(unsigned int handle);
PORT_MS_ABI int port_lan_close(unsigned int handle);
// address: INADDR_ANY, 127.0.0.1 or this console's own address; port 0 picks a free one.
PORT_MS_ABI int port_lan_bind(unsigned int handle, const port_lan_address *local);
// The default destination of send and the only source recv takes; address 0 removes it.
PORT_MS_ABI int port_lan_connect(unsigned int handle, const port_lan_address *remote);
PORT_MS_ABI int port_lan_shutdown(unsigned int handle);

enum {
    PORT_LAN_OPTION_BROADCAST,      // SO_BROADCAST
    PORT_LAN_OPTION_RECEIVE_BUFFER, // SO_RCVBUF: bytes of datagrams queued before more are dropped
    PORT_LAN_OPTION_NONBLOCKING,    // FIONBIO
};
PORT_MS_ABI int port_lan_set_option(unsigned int handle, int option, unsigned int value);
// FIONREAD: the size of the next datagram, 0 when none is queued.
PORT_MS_ABI int port_lan_pending(unsigned int handle, unsigned int *bytes);
// remote 0: the connected destination. *sent: the bytes sent.
PORT_MS_ABI int port_lan_send(unsigned int handle, const void *data, int size, const port_lan_address *remote, int *sent);
// Blocks unless the socket is non-blocking (then WSAEWOULDBLOCK). A datagram larger than size fills the buffer, is
// dropped and fails with WSAEMSGSIZE (*received = size). peek leaves it queued.
PORT_MS_ABI int port_lan_receive(unsigned int handle, void *buffer, int size, int peek, port_lan_address *remote,
    int *received);
// select: whether a receive would not block (*readable) / a send (*writable).
PORT_MS_ABI int port_lan_poll(unsigned int handle, int *readable, int *writable);
// select's wait: sleeps until the socket state may have changed since port_lan_activity returned activity, or
// timeout_ms passed (0xffffffff: forever). port_lan_milliseconds is the clock select times out by.
PORT_MS_ABI unsigned int port_lan_activity(void);
PORT_MS_ABI void port_lan_wait(unsigned int activity, unsigned int timeout_ms);
PORT_MS_ABI unsigned int port_lan_milliseconds(void);

// ---- XNet (xnet.c) ----

// HALO3_LAN=1 in the environment.
int port_lan_enabled(void);
int port_lan_startup(const unsigned char *params); // XNetStartupParams (13 bytes) or 0
void port_lan_cleanup(void);
unsigned int port_lan_ethernet_link_status(void);
unsigned int port_lan_title_xnaddr(void *xnaddr);
// The Live relay uses the same host UDP socket as System Link. The token is copied.
void port_lan_set_relay(unsigned int ipv4_host_order, unsigned short port, unsigned long long xuid, const char *token);
void port_lan_clear_relay(void);
int port_lan_register_key(const void *xnkid, const void *xnkey);
int port_lan_unregister_key(const void *xnkid);
int port_lan_xnaddr_to_inaddr(const void *xnaddr, const void *xnkid, unsigned int *secure);
int port_lan_inaddr_to_xnaddr(unsigned int secure, void *xnaddr, void *xnkid);
int port_lan_unregister_inaddr(unsigned int secure);
int port_lan_connect_peer(unsigned int secure);
unsigned int port_lan_connect_status(unsigned int secure);
int port_lan_machine_id(const void *xnaddr, unsigned long long *machine_id);
int port_lan_qos_listen(const void *xnkid, const unsigned char *data, unsigned int size, unsigned int bits_per_second,
    unsigned int flags);
int port_lan_qos_lookup(unsigned int count, const void *const *xnaddrs, const void *const *xnkids,
    const void *const *xnkeys, unsigned int gateway_count, unsigned int probes, unsigned int bits_per_second,
    void *event, void **qos);
int port_lan_qos_release(void *qos);
int port_lan_qos_listen_stats(const void *xnkid, void *stats);

#ifdef __cplusplus
}
#endif

#endif
