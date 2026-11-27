// port/sdk/online (Windows host): the host services of System Link (lan_host.h) over Winsock 2 and Win32. Winsock is
// loaded at run time (ws2_32.dll), as the port loads XInput and Vulkan: the link has no import library for it, and
// the game's own Winsock (winsock_offline.cpp) has the Xbox's names.
#include "lan_host.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_runtime.h"

__declspec(dllimport) char *__cdecl getenv(const char *name);

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24,
    (void *security, __SIZE_TYPE__ stack_size, unsigned long(__stdcall *start)(void *), void *parameter,
        unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(int, SleepConditionVariableSRW, 16, (void **condition, void **lock, unsigned long milliseconds,
    unsigned long flags));
PORT_HOST_IMPORT(void, WakeAllConditionVariable, 4, (void **condition));
PORT_HOST_IMPORT(int, QueryPerformanceCounter, 4, (long long *count));
PORT_HOST_IMPORT(int, QueryPerformanceFrequency, 4, (long long *frequency));
PORT_HOST_IMPORT(void *, VirtualAlloc, 16, (void *address, __SIZE_TYPE__ size, unsigned long type,
    unsigned long protect));
PORT_HOST_IMPORT(int, VirtualFree, 12, (void *address, __SIZE_TYPE__ size, unsigned long type));

// Winsock 2 (winsock2.h, ws2ipdef.h, mstcpip.h), 32-bit and 64-bit.
typedef __UINTPTR_TYPE__ w_socket;
#define W_INVALID_SOCKET (~(w_socket)0)
typedef struct {
    short sin_family;
    unsigned short sin_port;
    unsigned long sin_addr;
    char sin_zero[8];
} w_sockaddr_in;
typedef struct {
    unsigned int count;
    w_socket sockets[64];
} w_fd_set;
typedef struct {
    long seconds, microseconds;
} w_timeval;
// INTERFACE_INFO: iiFlags, then iiAddress, iiBroadcastAddress, iiNetmask as 24-byte sockaddr_gen
typedef struct {
    unsigned long flags;
    w_sockaddr_in address;
    char address_rest[8];
    w_sockaddr_in broadcast;
    char broadcast_rest[8];
    w_sockaddr_in netmask;
    char netmask_rest[8];
} w_interface_info;
_Static_assert(sizeof(w_interface_info) == 76, "INTERFACE_INFO");

enum {
    W_AF_INET = 2,
    W_SOCK_DGRAM = 2,
    W_IPPROTO_IP = 0,
    W_IPPROTO_UDP = 17,
    W_IP_TOS = 3,
    W_SOL_SOCKET = 0xffff,
    W_SO_BROADCAST = 0x0020,
    W_SO_SNDBUF = 0x1001,
    W_SO_RCVBUF = 0x1002,
    W_SO_EXCLUSIVEADDRUSE = ~0x0004,
    W_IFF_UP = 0x01,
    W_IFF_BROADCAST = 0x02,
    W_IFF_LOOPBACK = 0x04,
    W_WSAEINTR = 10004,
    W_WSAEACCES = 10013,
    W_WSAEWOULDBLOCK = 10035,
    W_WSAEMSGSIZE = 10040,
    W_WSAEADDRINUSE = 10048,
    W_WSAEADDRNOTAVAIL = 10049,
    W_WSAENETUNREACH = 10051,
    W_WSAECONNRESET = 10054,
    W_WSAENOBUFS = 10055,
    W_WSAEHOSTUNREACH = 10065,
};
#define W_FIONBIO 0x8004667eL
#define W_SIO_UDP_CONNRESET 0x9800000cul
#define W_SIO_GET_INTERFACE_LIST 0x4004747ful

static struct {
    int resolved;
    int(__stdcall *WSAStartup)(unsigned short version, void *data);
    w_socket(__stdcall *socket)(int af, int type, int protocol);
    int(__stdcall *bind)(w_socket s, const void *name, int size);
    int(__stdcall *sendto)(w_socket s, const char *data, int size, int flags, const void *to, int to_size);
    int(__stdcall *recvfrom)(w_socket s, char *buffer, int size, int flags, void *from, int *from_size);
    int(__stdcall *setsockopt)(w_socket s, int level, int name, const char *value, int size);
    int(__stdcall *ioctlsocket)(w_socket s, long command, unsigned long *argument);
    int(__stdcall *closesocket)(w_socket s);
    int(__stdcall *select)(int count, w_fd_set *read, w_fd_set *write, w_fd_set *except, const w_timeval *timeout);
    int(__stdcall *WSAGetLastError)(void);
    int(__stdcall *WSAIoctl)(w_socket s, unsigned long code, void *in, unsigned long in_size, void *out,
        unsigned long out_size, unsigned long *returned, void *overlapped, void *completion);
} g_host;

int lan_host_startup(void)
{
    if (g_host.resolved)
        return 0;
    void *ws2 = host_LoadLibraryA("ws2_32.dll");
    if (ws2 == 0)
        return -1;
#define LAN_RESOLVE(name)                                                                                              \
    if ((*(void **)&g_host.name = host_GetProcAddress(ws2, #name)) == 0)                                              \
        return -1;
    LAN_RESOLVE(WSAStartup)
    LAN_RESOLVE(socket)
    LAN_RESOLVE(bind)
    LAN_RESOLVE(sendto)
    LAN_RESOLVE(recvfrom)
    LAN_RESOLVE(setsockopt)
    LAN_RESOLVE(ioctlsocket)
    LAN_RESOLVE(closesocket)
    LAN_RESOLVE(select)
    LAN_RESOLVE(WSAGetLastError)
    LAN_RESOLVE(WSAIoctl)
#undef LAN_RESOLVE
    unsigned char data[512]; // WSADATA
    if (g_host.WSAStartup(0x0202, data) != 0)
        return -1;
    g_host.resolved = 1;
    return 0;
}

static void socket_address(w_sockaddr_in *to, unsigned int address, unsigned short port)
{
    __builtin_memset(to, 0, sizeof(*to));
    to->sin_family = W_AF_INET;
    to->sin_port = __builtin_bswap16(port);
    to->sin_addr = __builtin_bswap32(address);
}

int lan_host_open(unsigned short port, lan_host_socket *socket_out)
{
    *socket_out = LAN_HOST_NO_SOCKET;
    w_socket s = g_host.socket(W_AF_INET, W_SOCK_DGRAM, W_IPPROTO_UDP);
    if (s == W_INVALID_SOCKET)
        return LAN_HOST_FAILED;
    int on = 1;
    unsigned long nonblocking = 1, off = 0, returned = 0;
    // no other process may take this console's port; datagrams to a closed port must not fail the next receive
    // (Windows reports the ICMP error as WSAECONNRESET otherwise)
    if (g_host.setsockopt(s, W_SOL_SOCKET, W_SO_EXCLUSIVEADDRUSE, (const char *)&on, sizeof(on)) != 0
        || g_host.setsockopt(s, W_SOL_SOCKET, W_SO_BROADCAST, (const char *)&on, sizeof(on)) != 0
        || g_host.ioctlsocket(s, W_FIONBIO, &nonblocking) != 0) {
        g_host.closesocket(s);
        return LAN_HOST_FAILED;
    }
    g_host.WSAIoctl(s, W_SIO_UDP_CONNRESET, &off, sizeof(off), 0, 0, &returned, 0, 0);
    // Larger UDP buffers prevent burst drops in 16-player BTB; DSCP EF (0xb8) prioritizes game traffic on routers/Wi-Fi.
    int buf_size = 2 * 1024 * 1024;
    (void)g_host.setsockopt(s, W_SOL_SOCKET, W_SO_RCVBUF, (const char *)&buf_size, sizeof(buf_size));
    (void)g_host.setsockopt(s, W_SOL_SOCKET, W_SO_SNDBUF, (const char *)&buf_size, sizeof(buf_size));
    int tos = 0xb8;
    (void)g_host.setsockopt(s, W_IPPROTO_IP, W_IP_TOS, (const char *)&tos, sizeof(tos));
    w_sockaddr_in local;
    socket_address(&local, 0, port);
    if (g_host.bind(s, &local, sizeof(local)) != 0) {
        int error = g_host.WSAGetLastError();
        g_host.closesocket(s);
        return error == W_WSAEADDRINUSE || error == W_WSAEACCES ? LAN_HOST_IN_USE : LAN_HOST_FAILED;
    }
    *socket_out = (lan_host_socket)s;
    return LAN_HOST_OK;
}

void lan_host_close(lan_host_socket socket)
{
    if (socket != LAN_HOST_NO_SOCKET)
        g_host.closesocket((w_socket)socket);
}

int lan_host_send(lan_host_socket socket, const void *data, unsigned int size, unsigned int address,
    unsigned short port)
{
    w_sockaddr_in to;
    socket_address(&to, address, port);
    int sent = g_host.sendto((w_socket)socket, (const char *)data, (int)size, 0, &to, sizeof(to));
    if (sent == (int)size)
        return LAN_HOST_OK;
    int error = sent < 0 ? g_host.WSAGetLastError() : 0;
    if (error == W_WSAEWOULDBLOCK || error == W_WSAENOBUFS)
        return LAN_HOST_WOULD_BLOCK;
    if (error == W_WSAEHOSTUNREACH || error == W_WSAENETUNREACH || error == W_WSAEADDRNOTAVAIL)
        return LAN_HOST_UNREACHABLE;
    return LAN_HOST_FAILED;
}

int lan_host_wait_readable(lan_host_socket socket, unsigned int timeout_ms)
{
    w_fd_set read;
    read.count = 1;
    read.sockets[0] = (w_socket)socket;
    w_timeval timeout = { (long)(timeout_ms / 1000), (long)(timeout_ms % 1000) * 1000 };
    int ready = g_host.select(0, &read, 0, 0, timeout_ms == LAN_HOST_FOREVER ? 0 : &timeout);
    if (ready < 0)
        return g_host.WSAGetLastError() == W_WSAEINTR ? 0 : LAN_HOST_FAILED;
    return ready > 0 ? 1 : 0;
}

int lan_host_receive(lan_host_socket socket, void *buffer, unsigned int size, unsigned int *address,
    unsigned short *port)
{
    for (;;) {
        w_sockaddr_in from;
        int from_size = sizeof(from);
        int received = g_host.recvfrom((w_socket)socket, (char *)buffer, (int)size, 0, &from, &from_size);
        if (received >= 0) {
            if (from_size < (int)sizeof(from) || from.sin_family != W_AF_INET)
                continue;
            *address = __builtin_bswap32(from.sin_addr);
            *port = __builtin_bswap16(from.sin_port);
            return received;
        }
        int error = g_host.WSAGetLastError();
        // an ICMP error or a datagram larger than any frame is not one: read on
        if (error == W_WSAECONNRESET || error == W_WSAEMSGSIZE || error == W_WSAEINTR)
            continue;
        return error == W_WSAEWOULDBLOCK ? LAN_HOST_WOULD_BLOCK : LAN_HOST_FAILED;
    }
}

int lan_host_interfaces(lan_host_interface *interfaces, int capacity)
{
    if (!g_host.resolved)
        return 0;
    w_socket s = g_host.socket(W_AF_INET, W_SOCK_DGRAM, W_IPPROTO_UDP);
    if (s == W_INVALID_SOCKET)
        return 0;
    w_interface_info list[32];
    unsigned long returned = 0;
    int count = 0;
    if (g_host.WSAIoctl(s, W_SIO_GET_INTERFACE_LIST, 0, 0, list, sizeof(list), &returned, 0, 0) == 0) {
        unsigned long n = returned / sizeof(w_interface_info);
        for (unsigned long i = 0; i < n && count < capacity; i++) {
            const w_interface_info *info = &list[i];
            if ((info->flags & W_IFF_UP) == 0 || (info->flags & W_IFF_LOOPBACK) != 0
                || (info->flags & W_IFF_BROADCAST) == 0 || info->address.sin_family != W_AF_INET)
                continue;
            lan_host_interface *out = &interfaces[count++];
            out->address = __builtin_bswap32(info->address.sin_addr);
            out->netmask = __builtin_bswap32(info->netmask.sin_addr);
            // iiBroadcastAddress is always 255.255.255.255: the subnet's is the address with the host bits set
            out->broadcast = out->address | ~out->netmask;
        }
    }
    g_host.closesocket(s);
    return count;
}

unsigned long long lan_host_microseconds(void)
{
    static long long frequency;
    if (frequency == 0)
        host_QueryPerformanceFrequency(&frequency);
    long long count;
    host_QueryPerformanceCounter(&count);
    return (unsigned long long)(count / frequency) * 1000000ull
        + (unsigned long long)(count % frequency) * 1000000ull / (unsigned long long)frequency;
}

const char *lan_host_getenv(const char *name) { return getenv(name); }

// One write per line: the lines of the port's threads (System Link, Live, the game's) do not run into each other.
void lan_host_log(const char *text)
{
    static const char k_prefix[] = "port: ";
    char line[640];
    unsigned int n = sizeof(k_prefix) - 1, length = 0;
    while (text[length] != 0)
        length++;
    if (length > sizeof(line) - sizeof(k_prefix) - 1) {
        port_write_error(k_prefix);
        port_write_error(text);
        port_write_error("\n");
        return;
    }
    __builtin_memcpy(line, k_prefix, n);
    __builtin_memcpy(line + n, text, length);
    n += length;
    line[n++] = '\n';
    line[n] = 0;
    port_write_error(line);
}

void *lan_host_pages_alloc(unsigned int size)
{
    return host_VirtualAlloc(0, size, 0x3000 /* MEM_COMMIT | MEM_RESERVE */, 0x04 /* PAGE_READWRITE */);
}

void lan_host_pages_free(void *pages, unsigned int size)
{
    (void)size;
    host_VirtualFree(pages, 0, 0x8000 /* MEM_RELEASE */);
}

// ---- lock, condition, thread: an SRW lock and a condition variable, both statically initialized (zero) ----

static void *g_lock;
static void *g_condition;
static void *g_thread;

void lan_host_lock(void) { host_AcquireSRWLockExclusive(&g_lock); }
void lan_host_unlock(void) { host_ReleaseSRWLockExclusive(&g_lock); }

void lan_host_wait(unsigned int timeout_ms)
{
    host_SleepConditionVariableSRW(&g_condition, &g_lock, timeout_ms == LAN_HOST_FOREVER ? 0xffffffffu : timeout_ms,
        0);
}

void lan_host_wake_all(void) { host_WakeAllConditionVariable(&g_condition); }

typedef struct thread_start {
    void (*entry)(void *);
    void *argument;
} thread_start;
static thread_start g_thread_start;

static unsigned long __stdcall thread_main(void *start)
{
    const thread_start *s = (const thread_start *)start;
    s->entry(s->argument);
    return 0;
}

int lan_host_thread_start(void (*entry)(void *), void *argument)
{
    g_thread_start.entry = entry;
    g_thread_start.argument = argument;
    g_thread = host_CreateThread(0, 0, thread_main, &g_thread_start, 0, 0);
    return g_thread != 0 ? 0 : -1;
}

void lan_host_thread_join(void)
{
    if (g_thread != 0) {
        host_WaitForSingleObject(g_thread, 0xffffffffu);
        host_CloseHandle(g_thread);
        g_thread = 0;
    }
}
