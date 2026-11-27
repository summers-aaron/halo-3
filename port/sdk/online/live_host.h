// port/sdk/online (host code): host TCP, TLS, DNS, thread, sleep and file primitives for the Live client (live.c).
// Implemented by live_linux.c (dlsym(RTLD_NEXT) POSIX sockets, OpenSSL, pthreads) and live_windows.c (Winsock, Schannel, Win32).
// Addresses and ports are numbers in host order (127.0.0.1 is 0x7f000001).
#ifndef PORT_ONLINE_LIVE_HOST_H
#define PORT_ONLINE_LIVE_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef __INTPTR_TYPE__ live_host_socket;
#define LIVE_HOST_NO_SOCKET ((live_host_socket)-1)

enum {
    LIVE_HOST_OK = 0,
    LIVE_HOST_FAILED = -1,
    LIVE_HOST_WOULD_BLOCK = -2,
    LIVE_HOST_IN_PROGRESS = -3,
    LIVE_HOST_CLOSED = -4,
};

int live_host_startup(void);
int live_host_resolve(const char *host, unsigned int *ipv4_out);

int live_host_tcp_open(int nonblocking, live_host_socket *sock_out);
int live_host_tcp_connect(live_host_socket sock, unsigned int ipv4, unsigned short port);
int live_host_tcp_set_nonblocking(live_host_socket sock, int nonblocking);
int live_host_tcp_send(live_host_socket sock, const void *data, int size);
int live_host_tcp_recv(live_host_socket sock, void *buffer, int size);
int live_host_tcp_poll(live_host_socket sock, int *readable, int *writable, int *error);
int live_host_tcp_wait_connect(live_host_socket sock, unsigned int timeout_ms);
int live_host_tcp_wait_readable(live_host_socket sock, unsigned int timeout_ms);
int live_host_tcp_pending(live_host_socket sock, unsigned int *bytes_out);
int live_host_tcp_shutdown(live_host_socket sock, int how);
void live_host_tcp_close(live_host_socket sock);

// TLS on an already connected, nonblocking socket. Connect returns NULL only after verifying the
// peer for host. The caller owns the socket and closes the TLS state before closing the socket.
// Read/write include their own bounded wait (including data already buffered by the TLS provider).
typedef struct live_host_tls live_host_tls;
const char *live_host_tls_connect(live_host_socket sock, const char *host, const char *ca_file,
    unsigned int timeout_ms, live_host_tls **out);
int live_host_tls_send(live_host_tls *tls, const void *data, int size);
int live_host_tls_recv(live_host_tls *tls, void *buffer, int size);
// True when the provider has input buffered beyond the response returned to the HTTP parser.
int live_host_tls_pending(live_host_tls *tls);
void live_host_tls_close(live_host_tls *tls);

void live_host_lock(void);
void live_host_unlock(void);
// Under the lock: releases it, sleeps until live_host_wake_all or timeout_ms (LIVE_HOST_FOREVER: no timeout), retakes it.
#define LIVE_HOST_FOREVER 0xffffffffu
void live_host_wait(unsigned int timeout_ms);
void live_host_wake_all(void);
int live_host_thread_start(void (*entry)(void *), void *arg);
// The calling thread's id (never 0).
unsigned long live_host_thread_id(void);
void live_host_sleep_ms(unsigned int ms);

// Simple whole-file read/write for persisting the Live account token (returns bytes read/written or -1).
int live_host_read_file(const char *path, char *buffer, unsigned int capacity);
int live_host_write_file(const char *path, const char *data, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
