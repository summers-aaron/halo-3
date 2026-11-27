// port/sdk/online (Linux host): TCP, DNS, thread and file primitives for the Live client (live_host.h).
// Every socket / DNS call is resolved through dlsym(RTLD_NEXT) so it never binds to the game's Xbox Winsock
// definitions in winsock_offline.cpp.
#define _GNU_SOURCE
#include "live_host.h"
#include "lan_host.h"

#include <arpa/inet.h>

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static struct {
    int resolved;
    int (*socket)(int, int, int);
    int (*connect)(int, const struct sockaddr *, socklen_t);
    ssize_t (*send)(int, const void *, size_t, int);
    ssize_t (*recv)(int, void *, size_t, int);
    int (*setsockopt)(int, int, int, const void *, socklen_t);
    int (*getsockopt)(int, int, int, void *, socklen_t *);
    int (*shutdown)(int, int);
    int (*close)(int);
    int (*poll)(struct pollfd *, nfds_t, int);
    int (*getaddrinfo)(const char *, const char *, const struct addrinfo *, struct addrinfo **);
    void (*freeaddrinfo)(struct addrinfo *);
} g_lh;

static pthread_mutex_t g_live_mutex = PTHREAD_MUTEX_INITIALIZER;

int live_host_startup(void)
{
    if (g_lh.resolved)
        return 0;
#define LH_RESOLVE(name)                                                                                               \
    if ((*(void **)&g_lh.name = dlsym(RTLD_NEXT, #name)) == 0)                                                          \
        return -1;
    LH_RESOLVE(socket)
    LH_RESOLVE(connect)
    LH_RESOLVE(send)
    LH_RESOLVE(recv)
    LH_RESOLVE(setsockopt)
    LH_RESOLVE(getsockopt)
    LH_RESOLVE(shutdown)
    LH_RESOLVE(close)
    LH_RESOLVE(poll)
    LH_RESOLVE(getaddrinfo)
    LH_RESOLVE(freeaddrinfo)
#undef LH_RESOLVE
    g_lh.resolved = 1;
    return 0;
}

int live_host_resolve(const char *host, unsigned int *ipv4_out)
{
    if (!g_lh.resolved && live_host_startup() != 0)
        return -1;
    struct addrinfo hints;
    __builtin_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = 0;
    if (g_lh.getaddrinfo(host, 0, &hints, &res) != 0 || res == 0)
        return -1;
    const struct sockaddr_in *in = (const struct sockaddr_in *)res->ai_addr;
    *ipv4_out = __builtin_bswap32(in->sin_addr.s_addr);
    g_lh.freeaddrinfo(res);
    return 0;
}

int live_host_tcp_open(int nonblocking, live_host_socket *sock_out)
{
    *sock_out = LIVE_HOST_NO_SOCKET;
    if (!g_lh.resolved && live_host_startup() != 0)
        return LIVE_HOST_FAILED;
    int flags = SOCK_STREAM | SOCK_CLOEXEC | (nonblocking ? SOCK_NONBLOCK : 0);
    int s = g_lh.socket(AF_INET, flags, IPPROTO_TCP);
    if (s < 0)
        return LIVE_HOST_FAILED;
    int on = 1;
    g_lh.setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    *sock_out = s;
    return LIVE_HOST_OK;
}

int live_host_tcp_set_nonblocking(live_host_socket sock, int nonblocking)
{
    int flags = fcntl((int)sock, F_GETFL, 0);
    if (flags < 0)
        return LIVE_HOST_FAILED;
    if (nonblocking)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    return fcntl((int)sock, F_SETFL, flags) == 0 ? LIVE_HOST_OK : LIVE_HOST_FAILED;
}

int live_host_tcp_connect(live_host_socket sock, unsigned int ipv4, unsigned short port)
{
    struct sockaddr_in addr;
    __builtin_memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = __builtin_bswap16(port);
    addr.sin_addr.s_addr = __builtin_bswap32(ipv4);
    int r;
    do
        r = g_lh.connect((int)sock, (const struct sockaddr *)&addr, sizeof(addr));
    while (r < 0 && errno == EINTR);
    if (r == 0)
        return LIVE_HOST_OK;
    if (errno == EINPROGRESS || errno == EALREADY || errno == EWOULDBLOCK)
        return LIVE_HOST_IN_PROGRESS;
    return LIVE_HOST_FAILED;
}

int live_host_tcp_send(live_host_socket sock, const void *data, int size)
{
    ssize_t n;
    do
        n = g_lh.send((int)sock, data, (size_t)size, MSG_NOSIGNAL);
    while (n < 0 && errno == EINTR);
    if (n >= 0)
        return (int)n;
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return LIVE_HOST_WOULD_BLOCK;
    return LIVE_HOST_FAILED;
}

int live_host_tcp_recv(live_host_socket sock, void *buffer, int size)
{
    ssize_t n;
    do
        n = g_lh.recv((int)sock, buffer, (size_t)size, 0);
    while (n < 0 && errno == EINTR);
    if (n > 0)
        return (int)n;
    if (n == 0)
        return 0;
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return LIVE_HOST_WOULD_BLOCK;
    return LIVE_HOST_FAILED;
}

int live_host_tcp_poll(live_host_socket sock, int *readable, int *writable, int *error)
{
    struct pollfd p = { (int)sock, POLLIN | POLLOUT, 0 };
    int r;
    do
        r = g_lh.poll(&p, 1, 0);
    while (r < 0 && errno == EINTR);
    if (r < 0)
        return LIVE_HOST_FAILED;
    if (readable)
        *readable = (p.revents & (POLLIN | POLLHUP | POLLERR)) != 0;
    if (writable)
        *writable = (p.revents & POLLOUT) != 0;
    if (error) {
        int so_err = 0;
        socklen_t len = sizeof(so_err);
        if ((p.revents & POLLERR) != 0)
            *error = 1;
        else if (g_lh.getsockopt((int)sock, SOL_SOCKET, SO_ERROR, &so_err, &len) == 0 && so_err != 0)
            *error = 1;
        else
            *error = 0;
    }
    return LIVE_HOST_OK;
}

int live_host_tcp_wait_connect(live_host_socket sock, unsigned int timeout_ms)
{
    struct pollfd p = { (int)sock, POLLOUT, 0 };
    int r;
    do
        r = g_lh.poll(&p, 1, (int)timeout_ms);
    while (r < 0 && errno == EINTR);
    if (r <= 0)
        return LIVE_HOST_FAILED;
    int so_err = 0;
    socklen_t len = sizeof(so_err);
    if (g_lh.getsockopt((int)sock, SOL_SOCKET, SO_ERROR, &so_err, &len) != 0 || so_err != 0)
        return LIVE_HOST_FAILED;
    return LIVE_HOST_OK;
}

int live_host_tcp_wait_readable(live_host_socket sock, unsigned int timeout_ms)
{
    struct pollfd p = { (int)sock, POLLIN, 0 };
    int r;
    do
        r = g_lh.poll(&p, 1, (int)timeout_ms);
    while (r < 0 && errno == EINTR);
    if (r < 0)
        return LIVE_HOST_FAILED;
    return r > 0 ? 1 : 0;
}

int live_host_tcp_pending(live_host_socket sock, unsigned int *bytes_out)
{
    int avail = 0;
    if (ioctl((int)sock, FIONREAD, &avail) != 0)
        return LIVE_HOST_FAILED;
    *bytes_out = avail > 0 ? (unsigned int)avail : 0;
    return LIVE_HOST_OK;
}

int live_host_tcp_shutdown(live_host_socket sock, int how)
{
    return g_lh.shutdown((int)sock, how) == 0 ? LIVE_HOST_OK : LIVE_HOST_FAILED;
}

void live_host_tcp_close(live_host_socket sock)
{
    if (sock != LIVE_HOST_NO_SOCKET)
        g_lh.close((int)sock);
}

// Public OpenSSL 1.1/3 ABI. The release build needs neither OpenSSL headers nor link libraries.
typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_method_st SSL_METHOD;
typedef struct x509_store_ctx_st X509_STORE_CTX;
typedef struct X509_VERIFY_PARAM_st X509_VERIFY_PARAM;
typedef struct bio_st BIO;
typedef struct bio_method_st BIO_METHOD;

#define TLS_FUNCTIONS(F) \
    F(ssl, const SSL_METHOD *, TLS_client_method, (void)) \
    F(ssl, SSL_CTX *, SSL_CTX_new, (const SSL_METHOD *)) \
    F(ssl, void, SSL_CTX_free, (SSL_CTX *)) \
    F(ssl, void, SSL_CTX_set_verify, (SSL_CTX *, int, int (*)(int, X509_STORE_CTX *))) \
    F(ssl, int, SSL_CTX_set_default_verify_paths, (SSL_CTX *)) \
    F(ssl, int, SSL_CTX_load_verify_locations, (SSL_CTX *, const char *, const char *)) \
    F(ssl, SSL *, SSL_new, (SSL_CTX *)) \
    F(ssl, void, SSL_free, (SSL *)) \
    F(ssl, long, SSL_ctrl, (SSL *, int, long, void *)) \
    F(ssl, X509_VERIFY_PARAM *, SSL_get0_param, (SSL *)) \
    F(ssl, void, SSL_set_bio, (SSL *, BIO *, BIO *)) \
    F(ssl, int, SSL_connect, (SSL *)) \
    F(ssl, struct stack_st_X509 *, SSL_get0_verified_chain, (const SSL *)) \
    F(ssl, long, SSL_get_verify_result, (const SSL *)) \
    F(ssl, int, SSL_get_error, (const SSL *, int)) \
    F(ssl, int, SSL_write, (SSL *, const void *, int)) \
    F(ssl, int, SSL_has_pending, (const SSL *)) \
    F(ssl, int, SSL_read, (SSL *, void *, int)) \
    F(crypto, void, ERR_clear_error, (void)) \
    F(crypto, int, X509_VERIFY_PARAM_set1_host, (X509_VERIFY_PARAM *, const char *, size_t)) \
    F(crypto, int, X509_VERIFY_PARAM_set1_ip_asc, (X509_VERIFY_PARAM *, const char *)) \
    F(crypto, BIO *, BIO_new, (const BIO_METHOD *)) \
    F(crypto, int, BIO_get_new_index, (void)) \
    F(crypto, BIO_METHOD *, BIO_meth_new, (int, const char *)) \
    F(crypto, void, BIO_meth_free, (BIO_METHOD *)) \
    F(crypto, int, BIO_meth_set_read, (BIO_METHOD *, int (*)(BIO *, char *, int))) \
    F(crypto, int, BIO_meth_set_write, (BIO_METHOD *, int (*)(BIO *, const char *, int))) \
    F(crypto, int, BIO_meth_set_ctrl, (BIO_METHOD *, long (*)(BIO *, int, long, void *))) \
    F(crypto, void, BIO_set_data, (BIO *, void *)) \
    F(crypto, void *, BIO_get_data, (BIO *)) \
    F(crypto, void, BIO_set_init, (BIO *, int)) \
    F(crypto, void, BIO_set_flags, (BIO *, int)) \
    F(crypto, void, BIO_clear_flags, (BIO *, int))

static struct {
#define TLS_MEMBER(lib, type, name, params) type (*name) params;
    TLS_FUNCTIONS(TLS_MEMBER)
#undef TLS_MEMBER
    BIO_METHOD *socket_bio;
} g_ssl;
static pthread_once_t tls_once = PTHREAD_ONCE_INIT;

struct live_host_tls {
    SSL *ssl;
    live_host_socket sock;
    unsigned int timeout_ms;
};

// OpenSSL's socket BIO can raise SIGPIPE. Use the same MSG_NOSIGNAL writes as plaintext
// and leave the game's signal handlers and thread masks alone.
enum { BIO_READ = 1, BIO_WRITE = 2, BIO_RETRY = 8, BIO_RETRY_FLAGS = 15, BIO_FLUSH = 11 };

static int tls_bio_result(BIO *bio, int result, int direction)
{
    g_ssl.BIO_clear_flags(bio, BIO_RETRY_FLAGS);
    if (result == LIVE_HOST_WOULD_BLOCK)
        g_ssl.BIO_set_flags(bio, BIO_RETRY | direction);
    return result < 0 ? -1 : result;
}

static int tls_bio_read(BIO *bio, char *buffer, int size)
{
    live_host_tls *tls = g_ssl.BIO_get_data(bio);
    return tls_bio_result(bio, live_host_tcp_recv(tls->sock, buffer, size), BIO_READ);
}

static int tls_bio_write(BIO *bio, const char *buffer, int size)
{
    live_host_tls *tls = g_ssl.BIO_get_data(bio);
    return tls_bio_result(bio, live_host_tcp_send(tls->sock, buffer, size), BIO_WRITE);
}

static long tls_bio_ctrl(BIO *bio, int cmd, long arg, void *ptr)
{
    (void)bio;
    (void)arg;
    (void)ptr;
    return cmd == BIO_FLUSH;
}

static int load_tls_pair(const char *ssl_name, const char *crypto_name)
{
    void *ssl = dlopen(ssl_name, RTLD_NOW | RTLD_LOCAL);
    if (!ssl)
        return 0;
    void *crypto = dlopen(crypto_name, RTLD_NOW | RTLD_LOCAL);
    if (!crypto)
        goto failed;
#define TLS_LOAD(lib, type, name, params) if (!(g_ssl.name = dlsym(lib, #name))) goto failed;
    TLS_FUNCTIONS(TLS_LOAD)
#undef TLS_LOAD
    int index = g_ssl.BIO_get_new_index();
    BIO_METHOD *bio = index >= 0 ? g_ssl.BIO_meth_new(index | 0x0400 /* BIO_TYPE_SOURCE_SINK */, "Live TCP") : 0;
    if (bio && g_ssl.BIO_meth_set_read(bio, tls_bio_read) && g_ssl.BIO_meth_set_write(bio, tls_bio_write) &&
        g_ssl.BIO_meth_set_ctrl(bio, tls_bio_ctrl)) {
        g_ssl.socket_bio = bio;
        return 1;
    }
    g_ssl.BIO_meth_free(bio);
failed:
    if (crypto)
        dlclose(crypto);
    dlclose(ssl);
    return 0;
}

static void tls_load(void)
{
    if (!load_tls_pair("libssl.so.3", "libcrypto.so.3"))
        load_tls_pair("libssl.so.1.1", "libcrypto.so.1.1");
}

enum { TLS_HANDSHAKE, TLS_READ, TLS_WRITE };
enum { SSL_WANT_READ = 2, SSL_WANT_WRITE = 3, SSL_ZERO_RETURN = 6 };

static int tls_io(live_host_tls *tls, int operation, void *buffer, int size)
{
    unsigned long long deadline = lan_host_microseconds() + tls->timeout_ms * 1000ull;
    for (;;) {
        // SSL_get_error requires the calling thread's error queue to have been empty before the operation.
        g_ssl.ERR_clear_error();
        int n;
        if (operation == TLS_HANDSHAKE)
            n = g_ssl.SSL_connect(tls->ssl);
        else if (operation == TLS_READ)
            n = g_ssl.SSL_read(tls->ssl, buffer, size);
        else
            n = g_ssl.SSL_write(tls->ssl, buffer, size);
        if (n > 0)
            return n;
        int err = g_ssl.SSL_get_error(tls->ssl, n);
        if (err == SSL_ZERO_RETURN)
            return 0; // authenticated close_notify
        if (err != SSL_WANT_READ && err != SSL_WANT_WRITE)
            return LIVE_HOST_FAILED;
        struct pollfd p = { (int)tls->sock, err == SSL_WANT_READ ? POLLIN : POLLOUT, 0 };
        unsigned long long now = lan_host_microseconds();
        if (now >= deadline)
            return LIVE_HOST_WOULD_BLOCK;
        int ready = g_lh.poll(&p, 1, (int)((deadline - now + 999) / 1000));
        if (ready == 0)
            return LIVE_HOST_WOULD_BLOCK;
        if (ready < 0 && errno != EINTR)
            return LIVE_HOST_FAILED;
    }
}

const char *live_host_tls_connect(live_host_socket sock, const char *host, const char *ca_file,
    unsigned int timeout_ms, live_host_tls **out)
{
    *out = 0;
    pthread_once(&tls_once, tls_load);
    if (!g_ssl.socket_bio)
        return "live: HTTPS unavailable (could not load OpenSSL 3 or 1.1)";
    SSL_CTX *ctx = g_ssl.SSL_CTX_new(g_ssl.TLS_client_method());
    if (!ctx)
        return "live: HTTPS could not initialize OpenSSL";
    g_ssl.SSL_CTX_set_verify(ctx, 1 /* SSL_VERIFY_PEER */, 0);
    int trusted = ca_file ? g_ssl.SSL_CTX_load_verify_locations(ctx, ca_file, 0)
                          : g_ssl.SSL_CTX_set_default_verify_paths(ctx);
    SSL *ssl = trusted ? g_ssl.SSL_new(ctx) : 0;
    g_ssl.SSL_CTX_free(ctx); // SSL_new retains a reference
    if (!ssl)
        return "live: HTTPS could not initialize the trusted CA store";
    live_host_tls *tls = lan_host_pages_alloc(sizeof(*tls));
    if (!tls) {
        g_ssl.SSL_free(ssl);
        return "live: HTTPS out of memory";
    }
    tls->ssl = ssl;
    tls->sock = sock;
    tls->timeout_ms = timeout_ms;
    // SSL_set_tlsext_host_name is SSL_ctrl(..., SSL_CTRL_SET_TLSEXT_HOSTNAME, TLSEXT_NAMETYPE_host_name, ...).
    // IP literals use the certificate's iPAddress SAN and are never sent as an SNI hostname.
    struct in_addr ip;
    X509_VERIFY_PARAM *param = g_ssl.SSL_get0_param(ssl);
    int identity = inet_pton(AF_INET, host, &ip) == 1 ? g_ssl.X509_VERIFY_PARAM_set1_ip_asc(param, host) :
        (g_ssl.SSL_ctrl(ssl, 55, 0, (void *)host) > 0 && g_ssl.X509_VERIFY_PARAM_set1_host(param, host, 0));
    BIO *bio = identity ? g_ssl.BIO_new(g_ssl.socket_bio) : 0;
    if (bio) {
        g_ssl.BIO_set_data(bio, tls);
        g_ssl.BIO_set_init(bio, 1);
        g_ssl.SSL_set_bio(ssl, bio, bio);
        if (tls_io(tls, TLS_HANDSHAKE, 0, 0) == 1 && g_ssl.SSL_get_verify_result(ssl) == 0 &&
            g_ssl.SSL_get0_verified_chain(ssl)) {
            *out = tls;
            return 0;
        }
    }
    const char *error = g_ssl.SSL_get_verify_result(ssl) != 0 ?
        "live: HTTPS certificate or hostname verification failed" : "live: HTTPS TLS handshake failed";
    live_host_tls_close(tls);
    return error;
}

int live_host_tls_send(live_host_tls *tls, const void *data, int size)
{
    return tls_io(tls, TLS_WRITE, (void *)data, size);
}

int live_host_tls_recv(live_host_tls *tls, void *buffer, int size)
{
    return tls_io(tls, TLS_READ, buffer, size);
}

int live_host_tls_pending(live_host_tls *tls) { return g_ssl.SSL_has_pending(tls->ssl); }

void live_host_tls_close(live_host_tls *tls)
{
    g_ssl.SSL_free(tls->ssl);
    lan_host_pages_free(tls, sizeof(*tls));
}

static pthread_cond_t g_live_condition;
static pthread_once_t g_live_condition_once = PTHREAD_ONCE_INIT;

static void live_condition_create(void)
{
    pthread_condattr_t attributes;
    pthread_condattr_init(&attributes);
    pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
    pthread_cond_init(&g_live_condition, &attributes);
    pthread_condattr_destroy(&attributes);
}

void live_host_lock(void)
{
    pthread_once(&g_live_condition_once, live_condition_create);
    pthread_mutex_lock(&g_live_mutex);
}

void live_host_unlock(void) { pthread_mutex_unlock(&g_live_mutex); }

void live_host_wait(unsigned int timeout_ms)
{
    if (timeout_ms == LIVE_HOST_FOREVER) {
        pthread_cond_wait(&g_live_condition, &g_live_mutex);
        return;
    }
    struct timespec until;
    clock_gettime(CLOCK_MONOTONIC, &until);
    until.tv_sec += timeout_ms / 1000u;
    until.tv_nsec += (long)(timeout_ms % 1000u) * 1000000L;
    if (until.tv_nsec >= 1000000000L) {
        until.tv_sec++;
        until.tv_nsec -= 1000000000L;
    }
    pthread_cond_timedwait(&g_live_condition, &g_live_mutex, &until);
}

void live_host_wake_all(void) { pthread_cond_broadcast(&g_live_condition); }

typedef struct {
    void (*entry)(void *);
    void *arg;
} live_thread_start;

static void *live_thread_trampoline(void *p)
{
    live_thread_start *s = (live_thread_start *)p;
    void (*entry)(void *) = s->entry;
    void *arg = s->arg;
    // Static slot reuse is avoided by copying before clearing; caller passes static struct.
    entry(arg);
    return 0;
}

static live_thread_start g_thread_slots[8];
static unsigned int g_thread_slot_idx;

int live_host_thread_start(void (*entry)(void *), void *arg)
{
    unsigned int idx = __atomic_fetch_add(&g_thread_slot_idx, 1, __ATOMIC_RELAXED) & 7u;
    g_thread_slots[idx].entry = entry;
    g_thread_slots[idx].arg = arg;
    pthread_t th;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int r = pthread_create(&th, &attr, live_thread_trampoline, &g_thread_slots[idx]);
    pthread_attr_destroy(&attr);
    return r == 0 ? 0 : -1;
}

unsigned long live_host_thread_id(void) { return (unsigned long)pthread_self(); }

void live_host_sleep_ms(unsigned int ms)
{
    struct timespec ts;
    ts.tv_sec = (time_t)(ms / 1000u);
    ts.tv_nsec = (long)((ms % 1000u) * 1000000u);
    nanosleep(&ts, 0);
}

int live_host_read_file(const char *path, char *buffer, unsigned int capacity)
{
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -1;
    unsigned int total = 0;
    while (total + 1 < capacity) {
        ssize_t n = read(fd, buffer + total, capacity - 1 - total);
        if (n <= 0)
            break;
        total += (unsigned int)n;
    }
    buffer[total] = 0;
    g_lh.resolved ? g_lh.close(fd) : close(fd);
    return (int)total;
}

int live_host_write_file(const char *path, const char *data, unsigned int size)
{
    char temporary[1024];
    const size_t len = strlen(path);
    if (len == 0 || len + sizeof(".tmp") > sizeof(temporary))
        return -1;
    memcpy(temporary, path, len);
    memcpy(temporary + len, ".tmp", sizeof(".tmp"));
    int fd = open(temporary, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
    if (fd < 0)
        return -1;
    unsigned int total = 0;
    while (total < size) {
        ssize_t n = write(fd, data + total, size - total);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            break;
        total += (unsigned int)n;
    }
    const int flushed = total == size && fchmod(fd, 0600) == 0 && fsync(fd) == 0;
    const int closed = (g_lh.resolved ? g_lh.close(fd) : close(fd)) == 0;
    if (flushed && closed && rename(temporary, path) == 0)
        return (int)total;
    unlink(temporary);
    return -1;
}
