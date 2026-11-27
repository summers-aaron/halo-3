// port/sdk/online (both hosts): the Xbox's Winsock (xonline sockapi.obj). Built like the game, against the XDK's
// winsockx.h, so the structures (an fd_set is a count and an array of SOCKETs, not a bit mask) and signatures are the
// Xbox's; the errors are the thread's last error, which WSAGetLastError reads (port/sdk/online/xnet.c).
//
// With System Link running (HALO3_LAN=1, XNetStartup done: port/sdk/online/lan.c) the datagram sockets are real: a
// UDP or VDP socket bound to a title port sends to and receives from the consoles on the LAN, addressed by the secure
// addresses XNet gives (XNetXnAddrToInAddr), 127.0.0.1 (this console) and 255.255.255.255 (every console). Stream
// (TCP) sockets have no transport, except with Xbox Live (HALO3_LIVE_SERVER, port/sdk/online/live.h, which turns
// System Link on too): then a stream socket is a host TCP connection to a title server (LSP), at the address
// XNetServerToInAddr gave. Otherwise every call that needs the network fails with WSAENETDOWN, as an Xbox
// with no cable answers, and the game takes its no-network paths: the system-link transport
// (transport_endpoint_winsock.cpp) and Havok's remote debugger connection (hkBsdSocket.cpp) see their sockets fail to
// open.
//
// The pure functions mean what they always mean: inet_addr parses an address (in network byte order, which is the
// Xbox's host order: htonl is the identity in winsockx.h), __WSAFDIsSet looks a socket up in an fd_set.
#include <xtl.h>
#include <winsockx.h>

#include <stddef.h>
#include <string.h>

#include "lan.h"
#include "live.h"

// The layouts port/sdk/online/lan.c mirrors, and its error codes.
static_assert(sizeof(XNADDR) == PORT_LAN_XNADDR_SIZE && offsetof(XNADDR, wPortOnline) == 8
        && offsetof(XNADDR, abEnet) == 10 && offsetof(XNADDR, abOnline) == 16,
    "XNADDR");
static_assert(sizeof(XNKID) == PORT_LAN_XNKID_SIZE && sizeof(XNKEY) == PORT_LAN_XNKEY_SIZE, "XNKID, XNKEY");
static_assert(sizeof(XNetStartupParams) == PORT_LAN_STARTUP_PARAMS_SIZE, "XNetStartupParams");
static_assert(sizeof(XNQOSINFO) == (sizeof(void *) == 8 ? 28 : PORT_LAN_XNQOSINFO_SIZE)
        && offsetof(XNQOSINFO, pbData) == 8
        && offsetof(XNQOSINFO, wRttMinInMsecs) == (sizeof(void *) == 8 ? 16 : 12)
        && offsetof(XNQOSINFO, dwUpBitsPerSec) == (sizeof(void *) == 8 ? 20 : 16)
        && offsetof(XNQOS, cxnqosPending) == 4 && offsetof(XNQOS, axnqosinfo) == 8,
    "XNQOS");
static_assert(sizeof(XNQOSLISTENSTATS) == PORT_LAN_LISTEN_STATS_SIZE, "XNQOSLISTENSTATS");
static_assert(WSAEACCES == PORT_LAN_WSAEACCES && WSAEFAULT == PORT_LAN_WSAEFAULT && WSAEINVAL == PORT_LAN_WSAEINVAL
        && WSAEWOULDBLOCK == PORT_LAN_WSAEWOULDBLOCK && WSAEALREADY == PORT_LAN_WSAEALREADY
        && WSAENOTSOCK == PORT_LAN_WSAENOTSOCK && WSAEMSGSIZE == PORT_LAN_WSAEMSGSIZE
        && WSAENOPROTOOPT == PORT_LAN_WSAENOPROTOOPT && WSAEPROTONOSUPPORT == PORT_LAN_WSAEPROTONOSUPPORT
        && WSAEOPNOTSUPP == PORT_LAN_WSAEOPNOTSUPP && WSAEAFNOSUPPORT == PORT_LAN_WSAEAFNOSUPPORT
        && WSAEADDRINUSE == PORT_LAN_WSAEADDRINUSE && WSAEADDRNOTAVAIL == PORT_LAN_WSAEADDRNOTAVAIL
        && WSAENETDOWN == PORT_LAN_WSAENETDOWN && WSAENOBUFS == PORT_LAN_WSAENOBUFS
        && WSAENOTCONN == PORT_LAN_WSAENOTCONN && WSAEHOSTUNREACH == PORT_LAN_WSAEHOSTUNREACH,
    "Winsock errors");

namespace {

const int k_msg_peek = 2; // MSG_PEEK (winsock.h; winsockx.h has no message flags)

int fail(int error)
{
    WSASetLastError(error);
    return SOCKET_ERROR;
}

int result(int error) { return error == 0 ? 0 : fail(error); }

// A socket handle the System Link layer may know (SOCKET is pointer-sized; its handles are 32-bit).
bool lan_handle(SOCKET s, unsigned int *handle)
{
    *handle = (unsigned int)s;
    return s != INVALID_SOCKET && (SOCKET)*handle == s;
}

int read_address(const struct sockaddr *name, int namelen, port_lan_address *address)
{
    if (name == 0 || namelen < (int)sizeof(sockaddr_in))
        return WSAEFAULT;
    const sockaddr_in *in = (const sockaddr_in *)name;
    if (in->sin_family != AF_INET)
        return WSAEAFNOSUPPORT;
    address->address = ntohl(in->sin_addr.s_addr);
    address->port = ntohs(in->sin_port);
    return 0;
}

void write_address(const port_lan_address *address, struct sockaddr *name, int *namelen)
{
    if (name == 0 || namelen == 0)
        return;
    sockaddr_in *in = (sockaddr_in *)name;
    memset(in, 0, sizeof(*in));
    in->sin_family = AF_INET;
    in->sin_port = htons(address->port);
    in->sin_addr.s_addr = htonl(address->address);
    *namelen = sizeof(sockaddr_in);
}

int receive(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(port_lan_active() || port_live_enabled() ? WSAENOTSOCK : WSAENETDOWN);
    if (port_live_is_tcp_handle(handle)) {
        if ((buf == 0 && len > 0) || len < 0)
            return fail(WSAEFAULT);
        int received = 0;
        int error = port_live_tcp_recv(handle, buf, len, &received);
        return error != 0 ? fail(error) : received;
    }
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    if ((flags & ~k_msg_peek) != 0)
        return fail(WSAEOPNOTSUPP);
    if ((buf == 0 && len > 0) || len < 0 || (from != 0 && (fromlen == 0 || *fromlen < (int)sizeof(sockaddr_in))))
        return fail(WSAEFAULT);
    port_lan_address source;
    int received = 0;
    int error = port_lan_receive(handle, buf, len, (flags & k_msg_peek) != 0, &source, &received);
    if (error != 0 && error != WSAEMSGSIZE)
        return fail(error);
    write_address(&source, from, fromlen);
    return error != 0 ? fail(error) : received;
}

int transmit(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(port_lan_active() || port_live_enabled() ? WSAENOTSOCK : WSAENETDOWN);
    if (port_live_is_tcp_handle(handle)) {
        if ((buf == 0 && len > 0) || len < 0)
            return fail(WSAEFAULT);
        int sent = 0;
        int error = port_live_tcp_send(handle, buf, len, &sent);
        return error != 0 ? fail(error) : sent;
    }
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    if (flags != 0)
        return fail(WSAEOPNOTSUPP);
    if ((buf == 0 && len > 0) || len < 0)
        return fail(WSAEFAULT);
    port_lan_address destination;
    if (to != 0) {
        int error = read_address(to, tolen, &destination);
        if (error != 0)
            return fail(error);
    }
    int sent = 0;
    int error = port_lan_send(handle, buf, len, to != 0 ? &destination : 0, &sent);
    return error != 0 ? fail(error) : sent;
}

} // namespace

extern "C" {

SOCKET socket(int af, int type, int protocol)
{
    if (type == SOCK_STREAM && port_live_enabled()) {
        if (af != AF_INET) {
            WSASetLastError(WSAEAFNOSUPPORT);
            return INVALID_SOCKET;
        }
        unsigned int handle = 0;
        int error = port_live_tcp_socket(&handle);
        if (error != 0) {
            WSASetLastError(error);
            return INVALID_SOCKET;
        }
        return (SOCKET)handle;
    }
    // no network: as an Xbox with no cable; a stream socket has no transport without Live
    if (!port_lan_active() || type == SOCK_STREAM) {
        WSASetLastError(WSAENETDOWN);
        return INVALID_SOCKET;
    }
    int error = 0;
    if (af != AF_INET)
        error = WSAEAFNOSUPPORT;
    else if (type != SOCK_DGRAM || (protocol != 0 && protocol != IPPROTO_UDP && protocol != IPPROTO_VDP))
        error = WSAEPROTONOSUPPORT;
    unsigned int handle = 0;
    if (error == 0)
        error = port_lan_socket(protocol == IPPROTO_VDP, &handle);
    if (error != 0) {
        WSASetLastError(error);
        return INVALID_SOCKET;
    }
    return (SOCKET)handle;
}

int closesocket(SOCKET s)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    if (port_live_is_tcp_handle(handle))
        return result(port_live_tcp_close(handle));
    return result(port_lan_close(handle));
}

int shutdown(SOCKET s, int how)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    if (how < SD_RECEIVE || how > SD_BOTH)
        return fail(port_live_is_tcp_handle(handle) || port_lan_socket_valid(handle) == 0 ? WSAEINVAL : WSAENOTSOCK);
    if (port_live_is_tcp_handle(handle))
        return result(port_live_tcp_shutdown(handle, how));
    return result(port_lan_shutdown(handle));
}

// Datagram sockets neither listen nor accept.
int listen(SOCKET s, int backlog)
{
    (void)backlog;
    unsigned int handle;
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    return fail(lan_handle(s, &handle) && port_lan_socket_valid(handle) == 0 ? WSAEOPNOTSUPP : WSAENOTSOCK);
}

SOCKET accept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
    (void)addr; (void)addrlen;
    listen(s, 0);
    return INVALID_SOCKET;
}

int bind(SOCKET s, const struct sockaddr *name, int namelen)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    if (port_live_is_tcp_handle(handle))
        return 0; // outbound TCP client sockets use ephemeral local ports
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    port_lan_address local;
    int error = read_address(name, namelen, &local);
    return result(error != 0 ? error : port_lan_bind(handle, &local));
}

int connect(SOCKET s, const struct sockaddr *name, int namelen)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    port_lan_address remote;
    int error = read_address(name, namelen, &remote);
    if (error != 0)
        return fail(error);
    if (port_live_is_tcp_handle(handle))
        return result(port_live_tcp_connect(handle, remote.address, remote.port));
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    return result(port_lan_connect(handle, &remote));
}

int send(SOCKET s, const char *buf, int len, int flags) { return transmit(s, buf, len, flags, 0, 0); }

int sendto(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
    return transmit(s, buf, len, flags, to, tolen);
}

int recv(SOCKET s, char *buf, int len, int flags) { return receive(s, buf, len, flags, 0, 0); }

int recvfrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
    return receive(s, buf, len, flags, from, fromlen);
}

// A BOOL or DWORD option: its first byte when shorter than a DWORD.
int setsockopt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
    unsigned int handle;
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    if (port_live_is_tcp_handle(handle))
        return 0;
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    if (port_lan_socket_valid(handle) != 0)
        return fail(WSAENOTSOCK);
    if (optval == 0 || optlen < 1)
        return fail(WSAEFAULT);
    unsigned int value = 0;
    if (optlen >= (int)sizeof(DWORD))
        memcpy(&value, optval, sizeof(DWORD));
    else
        value = (unsigned char)optval[0];
    if (level != SOL_SOCKET)
        return fail(WSAENOPROTOOPT);
    switch (optname) {
    case SO_BROADCAST:
        return result(port_lan_set_option(handle, PORT_LAN_OPTION_BROADCAST, value));
    case SO_RCVBUF:
        return result(port_lan_set_option(handle, PORT_LAN_OPTION_RECEIVE_BUFFER, value));
    // accepted with nothing to change for a datagram socket of this console's own port
    case SO_REUSEADDR:
    case SO_EXCLUSIVEADDRUSE:
    case SO_SNDBUF:
    case SO_SNDTIMEO:
    case SO_RCVTIMEO:
    case SO_LINGER:
    case SO_DONTLINGER:
        return 0;
    default:
        return fail(WSAENOPROTOOPT);
    }
}

int ioctlsocket(SOCKET s, long cmd, u_long *argp)
{
    unsigned int handle;
    const u_long code = (u_long)cmd; // an _IOR/_IOW code (FIONBIO is 0x8004667e): an unsigned value in a long
    if (!lan_handle(s, &handle))
        return fail(WSAENOTSOCK);
    if (argp == 0)
        return fail(WSAEFAULT);
    if (port_live_is_tcp_handle(handle)) {
        switch (code) {
        case FIONBIO:
            return result(port_live_tcp_set_nonblocking(handle, *argp != 0));
        case FIONREAD: {
            unsigned int bytes = 0;
            int error = port_live_tcp_pending(handle, &bytes);
            if (error == 0)
                *argp = bytes;
            return result(error);
        }
        default:
            return fail(WSAEINVAL);
        }
    }
    if (!port_lan_active())
        return fail(WSAENETDOWN);
    if (port_lan_socket_valid(handle) != 0)
        return fail(WSAENOTSOCK);
    switch (code) {
    case FIONBIO:
        return result(port_lan_set_option(handle, PORT_LAN_OPTION_NONBLOCKING, *argp != 0));
    case FIONREAD: {
        unsigned int bytes = 0;
        int error = port_lan_pending(handle, &bytes);
        if (error == 0)
            *argp = bytes;
        return result(error);
    }
    default:
        return fail(WSAEINVAL);
    }
}

// select: an empty wait (all sets empty) is invalid, as in Winsock. A datagram socket is readable with a datagram
// queued and always writable; none has exceptions. A Live LSP stream socket (live.h) is readable with bytes queued or
// its peer gone, writable once connected, and has an exception on a socket error (a failed connect); while one is in
// the sets the wait is re-polled every 5 ms (its readiness is not System Link's activity). timeout 0 waits until one
// is ready.
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
    (void)nfds;
    int given = (readfds != 0 && readfds->fd_count != 0) || (writefds != 0 && writefds->fd_count != 0)
        || (exceptfds != 0 && exceptfds->fd_count != 0);
    if (!given || (!port_lan_active() && !port_live_enabled())) {
        if (readfds != 0)
            readfds->fd_count = 0;
        if (writefds != 0)
            writefds->fd_count = 0;
        if (exceptfds != 0)
            exceptfds->fd_count = 0;
        return fail(given ? WSAENOTSOCK : WSAEINVAL);
    }
    if (timeout != 0 && (timeout->tv_sec < 0 || timeout->tv_usec < 0))
        return fail(WSAEINVAL);
    unsigned int wait_ms = 0xffffffffu;
    if (timeout != 0) {
        unsigned long long ms = (unsigned long long)timeout->tv_sec * 1000 + ((unsigned long)timeout->tv_usec + 999) / 1000;
        wait_ms = ms >= 0xffffffffull ? 0xfffffffeu : (unsigned int)ms;
    }
    unsigned int start = port_lan_milliseconds();
    fd_set *sets[3] = { readfds, writefds, exceptfds };
    for (;;) {
        unsigned int activity = port_lan_activity();
        int ready = 0;
        bool has_tcp = false;
        bool want[3][FD_SETSIZE];
        for (int k = 0; k < 3; k++) {
            if (sets[k] == 0)
                continue;
            for (u_int i = 0; i < sets[k]->fd_count && i < FD_SETSIZE; i++) {
                unsigned int handle;
                int readable = 0, writable = 0, err = 0;
                if (!lan_handle(sets[k]->fd_array[i], &handle))
                    return fail(WSAENOTSOCK);
                if (port_live_is_tcp_handle(handle)) {
                    has_tcp = true;
                    if (port_live_tcp_poll(handle, &readable, &writable, &err) != 0)
                        return fail(WSAENOTSOCK);
                    want[k][i] = k == 0 ? readable != 0 : k == 1 ? writable != 0 : err != 0;
                } else {
                    if (port_lan_poll(handle, &readable, &writable) != 0)
                        return fail(WSAENOTSOCK);
                    want[k][i] = k == 0 ? readable != 0 : k == 1 ? writable != 0 : false;
                }
                ready += want[k][i];
            }
        }
        unsigned int elapsed = port_lan_milliseconds() - start;
        if (ready != 0 || (wait_ms != 0xffffffffu && elapsed >= wait_ms)) {
            for (int k = 0; k < 3; k++) {
                if (sets[k] == 0)
                    continue;
                u_int kept = 0;
                for (u_int i = 0; i < sets[k]->fd_count && i < FD_SETSIZE; i++)
                    if (want[k][i])
                        sets[k]->fd_array[kept++] = sets[k]->fd_array[i];
                sets[k]->fd_count = kept;
            }
            return ready;
        }
        unsigned int slice = wait_ms == 0xffffffffu ? (has_tcp ? 5u : wait_ms) : (wait_ms - elapsed);
        if (has_tcp && slice > 5u)
            slice = 5u;
        port_lan_wait(activity, slice);
    }
}

int __WSAFDIsSet(SOCKET s, fd_set *set)
{
    for (u_int i = 0; set != 0 && i < set->fd_count; i++)
        if (set->fd_array[i] == s)
            return 1;
    return 0;
}

// inet_addr: "a.b.c.d", each part decimal, octal (leading 0) or hexadecimal (0x), with the BSD short forms ("a.b.c"
// puts the last part in the low 16 bits, "a.b" in the low 24, "a" is the whole address); leading spaces and one
// trailing space are allowed. INADDR_NONE when it is not an address.
unsigned long inet_addr(const char *cp)
{
    if (cp == 0)
        return INADDR_NONE;
    while (*cp == ' ')
        cp++;
    unsigned long parts[4];
    int count = 0;
    for (;;) {
        if (*cp < '0' || *cp > '9')
            return INADDR_NONE;
        unsigned long base = 10, value = 0;
        if (*cp == '0') {
            base = 8;
            cp++;
            if (*cp == 'x' || *cp == 'X') {
                base = 16;
                cp++;
            }
        }
        int digits = 0;
        for (;; cp++) {
            unsigned long digit;
            if (*cp >= '0' && *cp <= '9')
                digit = (unsigned long)(*cp - '0');
            else if (base == 16 && *cp >= 'a' && *cp <= 'f')
                digit = (unsigned long)(*cp - 'a' + 10);
            else if (base == 16 && *cp >= 'A' && *cp <= 'F')
                digit = (unsigned long)(*cp - 'A' + 10);
            else
                break;
            if (digit >= base || value > (0xfffffffful - digit) / base)
                return INADDR_NONE;
            value = value * base + digit;
            digits++;
        }
        if (base == 16 && digits == 0)
            return INADDR_NONE;
        parts[count++] = value;
        if (*cp != '.')
            break;
        if (count == 4)
            return INADDR_NONE;
        cp++;
    }
    if (*cp == ' ')
        cp++;
    if (*cp != 0)
        return INADDR_NONE;
    static const unsigned long k_last_limit[4] = { 0xfffffffful, 0xfffffful, 0xfffful, 0xfful };
    for (int i = 0; i < count - 1; i++)
        if (parts[i] > 0xff)
            return INADDR_NONE;
    if (parts[count - 1] > k_last_limit[count - 1])
        return INADDR_NONE;
    unsigned long address = parts[count - 1];
    for (int i = 0; i < count - 1; i++)
        address |= parts[i] << (24 - 8 * i);
    return htonl(address);
}

} // extern "C"
