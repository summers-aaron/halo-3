// port/sdk/online (host code): the few host services System Link needs (lan.c), one implementation per host:
// lan_linux.c (POSIX sockets, pthreads) and lan_windows.c (Winsock 2 loaded at run time, Win32 threads).
// Addresses and ports are numbers in host order (192.168.1.5 is 0xc0a80105), as the game sees them.
#ifndef PORT_ONLINE_LAN_HOST_H
#define PORT_ONLINE_LAN_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

// The host's UDP socket, or LAN_HOST_NO_SOCKET.
typedef __INTPTR_TYPE__ lan_host_socket;
#define LAN_HOST_NO_SOCKET ((lan_host_socket)-1)

// lan_host_open / lan_host_send / lan_host_receive results that are not a byte count.
enum {
    LAN_HOST_OK = 0,
    LAN_HOST_IN_USE = -1,      // lan_host_open: the port is taken
    LAN_HOST_FAILED = -2,      // anything else
    LAN_HOST_WOULD_BLOCK = -3, // lan_host_send: no buffer space; lan_host_receive: nothing queued
    LAN_HOST_UNREACHABLE = -4, // lan_host_send: no route to the destination
};

typedef struct lan_host_interface {
    unsigned int address;   // this host's IPv4 on the interface
    unsigned int netmask;
    unsigned int broadcast; // the interface's directed broadcast address
} lan_host_interface;

// Loads the host's socket library (Windows) or resolves the host's socket functions (Linux: the game's Winsock
// defines socket, bind, sendto, ... with the Xbox's meaning, so the host's are reached by dlsym). 0 on success.
int lan_host_startup(void);

// A non-blocking UDP socket with SO_BROADCAST, bound to INADDR_ANY:port.
int lan_host_open(unsigned short port, lan_host_socket *socket_out);
void lan_host_close(lan_host_socket socket);
// LAN_HOST_OK, LAN_HOST_WOULD_BLOCK, LAN_HOST_UNREACHABLE or LAN_HOST_FAILED.
int lan_host_send(lan_host_socket socket, const void *data, unsigned int size, unsigned int address,
    unsigned short port);
// Waits up to timeout_ms for a datagram: 1 when one is queued, 0 on timeout, LAN_HOST_FAILED.
int lan_host_wait_readable(lan_host_socket socket, unsigned int timeout_ms);
// The next datagram (truncated to size), or LAN_HOST_WOULD_BLOCK / LAN_HOST_FAILED.
int lan_host_receive(lan_host_socket socket, void *buffer, unsigned int size, unsigned int *address,
    unsigned short *port);

// The host's IPv4 interfaces that are up, can broadcast and are not loopback: how many were written.
int lan_host_interfaces(lan_host_interface *interfaces, int capacity);

// A monotonic clock.
unsigned long long lan_host_microseconds(void);
// The environment variable, or 0.
const char *lan_host_getenv(const char *name);
// One line of text to the port's log (stderr).
void lan_host_log(const char *text);

// Whole pages, zeroed; not the C heap (the game's CRT owns malloc).
void *lan_host_pages_alloc(unsigned int size);
void lan_host_pages_free(void *pages, unsigned int size);

// The System Link lock (one per process, not recursive) and the condition its waiters sleep on.
void lan_host_lock(void);
void lan_host_unlock(void);
// Under the lock: releases it, sleeps until lan_host_wake_all or timeout_ms (LAN_HOST_FOREVER: no timeout), retakes it.
#define LAN_HOST_FOREVER 0xffffffffu
void lan_host_wait(unsigned int timeout_ms);
void lan_host_wake_all(void);

// The System Link thread. 0 on success.
int lan_host_thread_start(void (*entry)(void *), void *argument);
void lan_host_thread_join(void);

#ifdef __cplusplus
}
#endif

#endif
