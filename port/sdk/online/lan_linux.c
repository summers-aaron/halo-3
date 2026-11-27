// port/sdk/online (Linux host): the host services of System Link (lan_host.h) over POSIX sockets and pthreads.
// The game's Winsock (winsock_offline.cpp) defines socket, bind, sendto, recvfrom, setsockopt, ... with the Xbox's
// meaning (port/sdk/runtime/host-deny-linux.txt), and the executable's definitions are the ones its own references
// bind to; glibc's are reached through dlsym(RTLD_NEXT), past the executable. The rest of the socket calls go the
// same way so that none can bind to a game definition.
#define _GNU_SOURCE
#include "lan_host.h"
#include "../runtime/port_runtime.h"

#include <dlfcn.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <time.h>

static struct {
    int resolved;
    int (*socket)(int, int, int);
    int (*bind)(int, const struct sockaddr *, socklen_t);
    ssize_t (*sendto)(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
    ssize_t (*recvfrom)(int, void *, size_t, int, struct sockaddr *, socklen_t *);
    int (*setsockopt)(int, int, int, const void *, socklen_t);
    int (*close)(int);
    int (*poll)(struct pollfd *, nfds_t, int);
    int (*getifaddrs)(struct ifaddrs **);
    void (*freeifaddrs)(struct ifaddrs *);
} g_host;

int lan_host_startup(void)
{
    if (g_host.resolved)
        return 0;
#define LAN_RESOLVE(name)                                                                                              \
    if ((*(void **)&g_host.name = dlsym(RTLD_NEXT, #name)) == 0)                                                     \
        return -1;
    LAN_RESOLVE(socket)
    LAN_RESOLVE(bind)
    LAN_RESOLVE(sendto)
    LAN_RESOLVE(recvfrom)
    LAN_RESOLVE(setsockopt)
    LAN_RESOLVE(close)
    LAN_RESOLVE(poll)
    LAN_RESOLVE(getifaddrs)
    LAN_RESOLVE(freeifaddrs)
#undef LAN_RESOLVE
    g_host.resolved = 1;
    return 0;
}

static void socket_address(struct sockaddr_in *to, unsigned int address, unsigned short port)
{
    __builtin_memset(to, 0, sizeof(*to));
    to->sin_family = AF_INET;
    to->sin_port = __builtin_bswap16(port);
    to->sin_addr.s_addr = __builtin_bswap32(address);
}

int lan_host_open(unsigned short port, lan_host_socket *socket_out)
{
    *socket_out = LAN_HOST_NO_SOCKET;
    int s = g_host.socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
    if (s < 0)
        return LAN_HOST_FAILED;
    int on = 1;
    // broadcasts to the LAN; consoles on this PC share no port (each takes its own), so no SO_REUSEADDR
    if (g_host.setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != 0) {
        g_host.close(s);
        return LAN_HOST_FAILED;
    }
    // Larger UDP buffers prevent burst drops in 16-player BTB; DSCP EF (0xb8) prioritizes game traffic on routers/Wi-Fi.
    int buf_size = 2 * 1024 * 1024;
    (void)g_host.setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));
    (void)g_host.setsockopt(s, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));
    int tos = 0xb8;
    (void)g_host.setsockopt(s, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    struct sockaddr_in local;
    socket_address(&local, INADDR_ANY, port);
    if (g_host.bind(s, (const struct sockaddr *)&local, sizeof(local)) != 0) {
        int error = errno;
        g_host.close(s);
        return error == EADDRINUSE || error == EACCES ? LAN_HOST_IN_USE : LAN_HOST_FAILED;
    }
    *socket_out = s;
    return LAN_HOST_OK;
}

void lan_host_close(lan_host_socket socket)
{
    if (socket != LAN_HOST_NO_SOCKET)
        g_host.close((int)socket);
}

int lan_host_send(lan_host_socket socket, const void *data, unsigned int size, unsigned int address,
    unsigned short port)
{
    struct sockaddr_in to;
    socket_address(&to, address, port);
    ssize_t sent;
    do
        sent = g_host.sendto((int)socket, data, size, MSG_NOSIGNAL, (const struct sockaddr *)&to, sizeof(to));
    while (sent < 0 && errno == EINTR);
    if (sent == (ssize_t)size)
        return LAN_HOST_OK;
    if (sent < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOBUFS))
        return LAN_HOST_WOULD_BLOCK;
    if (sent < 0 && (errno == EHOSTUNREACH || errno == ENETUNREACH || errno == EADDRNOTAVAIL))
        return LAN_HOST_UNREACHABLE;
    return LAN_HOST_FAILED;
}

int lan_host_wait_readable(lan_host_socket socket, unsigned int timeout_ms)
{
    struct pollfd p = { (int)socket, POLLIN, 0 };
    int ready = g_host.poll(&p, 1, timeout_ms > 0x7fffffffu ? -1 : (int)timeout_ms);
    if (ready < 0)
        return errno == EINTR ? 0 : LAN_HOST_FAILED;
    return ready > 0 ? 1 : 0;
}

int lan_host_receive(lan_host_socket socket, void *buffer, unsigned int size, unsigned int *address,
    unsigned short *port)
{
    for (;;) {
        struct sockaddr_in from;
        socklen_t from_size = sizeof(from);
        ssize_t received
            = g_host.recvfrom((int)socket, buffer, size, MSG_DONTWAIT, (struct sockaddr *)&from, &from_size);
        if (received >= 0) {
            if (from_size < sizeof(from) || from.sin_family != AF_INET)
                continue;
            *address = __builtin_bswap32(from.sin_addr.s_addr);
            *port = __builtin_bswap16(from.sin_port);
            return (int)received;
        }
        if (errno == EINTR)
            continue;
        // an ICMP error queued for the socket is not a datagram: read on
        if (errno == ECONNREFUSED || errno == EHOSTUNREACH || errno == ENETUNREACH)
            continue;
        return errno == EAGAIN || errno == EWOULDBLOCK ? LAN_HOST_WOULD_BLOCK : LAN_HOST_FAILED;
    }
}

int lan_host_interfaces(lan_host_interface *interfaces, int capacity)
{
    struct ifaddrs *list = 0;
    if (!g_host.resolved || g_host.getifaddrs(&list) != 0)
        return 0;
    int count = 0;
    for (const struct ifaddrs *i = list; i != 0 && count < capacity; i = i->ifa_next) {
        if (i->ifa_addr == 0 || i->ifa_addr->sa_family != AF_INET || (i->ifa_flags & IFF_UP) == 0
            || (i->ifa_flags & IFF_LOOPBACK) != 0 || (i->ifa_flags & IFF_BROADCAST) == 0)
            continue;
        lan_host_interface *out = &interfaces[count++];
        out->address = __builtin_bswap32(((const struct sockaddr_in *)i->ifa_addr)->sin_addr.s_addr);
        out->netmask = i->ifa_netmask != 0
            ? __builtin_bswap32(((const struct sockaddr_in *)i->ifa_netmask)->sin_addr.s_addr)
            : 0xffffff00u;
        out->broadcast = i->ifa_broadaddr != 0
            ? __builtin_bswap32(((const struct sockaddr_in *)i->ifa_broadaddr)->sin_addr.s_addr)
            : (out->address | ~out->netmask);
    }
    g_host.freeifaddrs(list);
    return count;
}

unsigned long long lan_host_microseconds(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (unsigned long long)t.tv_sec * 1000000ull + (unsigned long long)t.tv_nsec / 1000ull;
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
    void *pages = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return pages == MAP_FAILED ? 0 : pages;
}

void lan_host_pages_free(void *pages, unsigned int size) { munmap(pages, size); }

// ---- lock, condition, thread ----

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_condition;
static pthread_once_t g_condition_once = PTHREAD_ONCE_INIT;
static pthread_t g_thread;

static void condition_create(void)
{
    pthread_condattr_t attributes;
    pthread_condattr_init(&attributes);
    pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
    pthread_cond_init(&g_condition, &attributes);
    pthread_condattr_destroy(&attributes);
}

void lan_host_lock(void)
{
    pthread_once(&g_condition_once, condition_create);
    pthread_mutex_lock(&g_lock);
}

void lan_host_unlock(void) { pthread_mutex_unlock(&g_lock); }

void lan_host_wait(unsigned int timeout_ms)
{
    if (timeout_ms == LAN_HOST_FOREVER) {
        pthread_cond_wait(&g_condition, &g_lock);
        return;
    }
    struct timespec until;
    clock_gettime(CLOCK_MONOTONIC, &until);
    until.tv_sec += timeout_ms / 1000;
    until.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
    if (until.tv_nsec >= 1000000000L) {
        until.tv_sec++;
        until.tv_nsec -= 1000000000L;
    }
    pthread_cond_timedwait(&g_condition, &g_lock, &until);
}

void lan_host_wake_all(void) { pthread_cond_broadcast(&g_condition); }

typedef struct thread_start {
    void (*entry)(void *);
    void *argument;
} thread_start;
static thread_start g_thread_start;

static void *thread_main(void *start)
{
    const thread_start *s = (const thread_start *)start;
    s->entry(s->argument);
    return 0;
}

int lan_host_thread_start(void (*entry)(void *), void *argument)
{
    g_thread_start.entry = entry;
    g_thread_start.argument = argument;
    return pthread_create(&g_thread, 0, thread_main, &g_thread_start) == 0 ? 0 : -1;
}

void lan_host_thread_join(void) { pthread_join(g_thread, 0); }
