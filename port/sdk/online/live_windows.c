// port/sdk/online (Windows host): TCP, DNS, thread and file primitives for the Live client (live_host.h).
// Loads ws2_32.dll at runtime to avoid symbol collision with the game's Xbox Winsock (winsock_offline.cpp).
#include "live_host.h"
#include "lan_host.h"
#include "../kernel/win32_host.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24,
    (void *security, __SIZE_TYPE__ stack_size, unsigned long(__stdcall *start)(void *), void *parameter,
        unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, Sleep, 4, (unsigned long ms));
PORT_HOST_IMPORT(unsigned long, GetCurrentThreadId, 0, (void));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(int, SleepConditionVariableSRW, 16,
    (void **condition, void **lock, unsigned long ms, unsigned long flags));
PORT_HOST_IMPORT(void, WakeAllConditionVariable, 4, (void **condition));
PORT_HOST_IMPORT(void *, CreateFileA, 28,
    (const char *name, unsigned long access, unsigned long share, void *sec, unsigned long disp, unsigned long flags,
        void *tmpl));
PORT_HOST_IMPORT(int, ReadFile, 20,
    (void *h, void *buf, unsigned long len, unsigned long *read_out, void *ov));
PORT_HOST_IMPORT(int, WriteFile, 20,
    (void *h, const void *buf, unsigned long len, unsigned long *written_out, void *ov));
PORT_HOST_IMPORT(int, GetFileSizeEx, 8, (void *h, long long *size));
PORT_HOST_IMPORT(int, FlushFileBuffers, 4, (void *h));
PORT_HOST_IMPORT(int, MoveFileExA, 12, (const char *from, const char *to, unsigned long flags));
PORT_HOST_IMPORT(int, DeleteFileA, 4, (const char *path));

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

typedef struct w_addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    __SIZE_TYPE__ ai_addrlen;
    char *ai_canonname;
    void *ai_addr;
    struct w_addrinfo *ai_next;
} w_addrinfo;

enum {
    W_AF_INET = 2,
    W_SOCK_STREAM = 1,
    W_IPPROTO_TCP = 6,
    W_SOL_SOCKET = 0xffff,
    W_SO_ERROR = 0x1007,
    W_TCP_NODELAY = 1,
    W_WSAEWOULDBLOCK = 10035,
    W_WSAEINPROGRESS = 10036,
    W_WSAEALREADY = 10037,
};
#define W_FIONBIO 0x8004667eL
#define W_FIONREAD 0x4004667fL

static struct {
    int resolved;
    int(__stdcall *WSAStartup)(unsigned short version, void *data);
    w_socket(__stdcall *socket)(int af, int type, int protocol);
    int(__stdcall *connect)(w_socket s, const void *name, int size);
    int(__stdcall *send)(w_socket s, const char *data, int size, int flags);
    int(__stdcall *recv)(w_socket s, char *buffer, int size, int flags);
    int(__stdcall *setsockopt)(w_socket s, int level, int name, const char *value, int size);
    int(__stdcall *getsockopt)(w_socket s, int level, int name, char *value, int *size);
    int(__stdcall *ioctlsocket)(w_socket s, long command, unsigned long *argument);
    int(__stdcall *shutdown)(w_socket s, int how);
    int(__stdcall *closesocket)(w_socket s);
    int(__stdcall *select)(int count, w_fd_set *r, w_fd_set *w, w_fd_set *e, const w_timeval *timeout);
    int(__stdcall *WSAGetLastError)(void);
    int(__stdcall *getaddrinfo)(const char *node, const char *service, const w_addrinfo *hints, w_addrinfo **res);
    void(__stdcall *freeaddrinfo)(w_addrinfo *res);
} g_lh;

static void *g_live_lock;

int live_host_startup(void)
{
    if (g_lh.resolved)
        return 0;
    void *ws = host_LoadLibraryA("ws2_32.dll");
    if (ws == 0)
        return -1;
#define LH_RESOLVE(name)                                                                                               \
    if ((*(void **)&g_lh.name = host_GetProcAddress(ws, #name)) == 0)                                                  \
        return -1;
    LH_RESOLVE(WSAStartup)
    LH_RESOLVE(socket)
    LH_RESOLVE(connect)
    LH_RESOLVE(send)
    LH_RESOLVE(recv)
    LH_RESOLVE(setsockopt)
    LH_RESOLVE(getsockopt)
    LH_RESOLVE(ioctlsocket)
    LH_RESOLVE(shutdown)
    LH_RESOLVE(closesocket)
    LH_RESOLVE(select)
    LH_RESOLVE(WSAGetLastError)
    LH_RESOLVE(getaddrinfo)
    LH_RESOLVE(freeaddrinfo)
#undef LH_RESOLVE
    unsigned char wsa_data[512];
    if (g_lh.WSAStartup(0x0202, wsa_data) != 0)
        return -1;
    g_lh.resolved = 1;
    return 0;
}

int live_host_resolve(const char *host, unsigned int *ipv4_out)
{
    if (!g_lh.resolved && live_host_startup() != 0)
        return -1;
    w_addrinfo hints;
    __builtin_memset(&hints, 0, sizeof(hints));
    hints.ai_family = W_AF_INET;
    hints.ai_socktype = W_SOCK_STREAM;
    w_addrinfo *res = 0;
    if (g_lh.getaddrinfo(host, 0, &hints, &res) != 0 || res == 0)
        return -1;
    const w_sockaddr_in *in = (const w_sockaddr_in *)res->ai_addr;
    *ipv4_out = __builtin_bswap32(in->sin_addr);
    g_lh.freeaddrinfo(res);
    return 0;
}

int live_host_tcp_open(int nonblocking, live_host_socket *sock_out)
{
    *sock_out = LIVE_HOST_NO_SOCKET;
    if (!g_lh.resolved && live_host_startup() != 0)
        return LIVE_HOST_FAILED;
    w_socket s = g_lh.socket(W_AF_INET, W_SOCK_STREAM, W_IPPROTO_TCP);
    if (s == W_INVALID_SOCKET)
        return LIVE_HOST_FAILED;
    int on = 1;
    g_lh.setsockopt(s, W_IPPROTO_TCP, W_TCP_NODELAY, (const char *)&on, sizeof(on));
    if (nonblocking) {
        unsigned long nb = 1;
        if (g_lh.ioctlsocket(s, W_FIONBIO, &nb) != 0) {
            g_lh.closesocket(s);
            return LIVE_HOST_FAILED;
        }
    }
    *sock_out = (live_host_socket)s;
    return LIVE_HOST_OK;
}

int live_host_tcp_set_nonblocking(live_host_socket sock, int nonblocking)
{
    unsigned long nb = nonblocking ? 1ul : 0ul;
    return g_lh.ioctlsocket((w_socket)sock, W_FIONBIO, &nb) == 0 ? LIVE_HOST_OK : LIVE_HOST_FAILED;
}

int live_host_tcp_connect(live_host_socket sock, unsigned int ipv4, unsigned short port)
{
    w_sockaddr_in addr;
    __builtin_memset(&addr, 0, sizeof(addr));
    addr.sin_family = W_AF_INET;
    addr.sin_port = __builtin_bswap16(port);
    addr.sin_addr = __builtin_bswap32(ipv4);
    if (g_lh.connect((w_socket)sock, &addr, sizeof(addr)) == 0)
        return LIVE_HOST_OK;
    int err = g_lh.WSAGetLastError();
    if (err == W_WSAEWOULDBLOCK || err == W_WSAEINPROGRESS || err == W_WSAEALREADY)
        return LIVE_HOST_IN_PROGRESS;
    return LIVE_HOST_FAILED;
}

int live_host_tcp_send(live_host_socket sock, const void *data, int size)
{
    int n = g_lh.send((w_socket)sock, (const char *)data, size, 0);
    if (n >= 0)
        return n;
    return g_lh.WSAGetLastError() == W_WSAEWOULDBLOCK ? LIVE_HOST_WOULD_BLOCK : LIVE_HOST_FAILED;
}

int live_host_tcp_recv(live_host_socket sock, void *buffer, int size)
{
    int n = g_lh.recv((w_socket)sock, (char *)buffer, size, 0);
    if (n >= 0)
        return n;
    return g_lh.WSAGetLastError() == W_WSAEWOULDBLOCK ? LIVE_HOST_WOULD_BLOCK : LIVE_HOST_FAILED;
}

int live_host_tcp_poll(live_host_socket sock, int *readable, int *writable, int *error)
{
    w_fd_set r = { 1, { (w_socket)sock } };
    w_fd_set w = { 1, { (w_socket)sock } };
    w_fd_set e = { 1, { (w_socket)sock } };
    w_timeval tv = { 0, 0 };
    int rc = g_lh.select(0, &r, &w, &e, &tv);
    if (rc < 0)
        return LIVE_HOST_FAILED;
    if (readable)
        *readable = r.count > 0;
    if (writable)
        *writable = w.count > 0;
    if (error)
        *error = e.count > 0;
    return LIVE_HOST_OK;
}

int live_host_tcp_wait_connect(live_host_socket sock, unsigned int timeout_ms)
{
    w_fd_set w = { 1, { (w_socket)sock } };
    w_fd_set e = { 1, { (w_socket)sock } };
    w_timeval tv = { (long)(timeout_ms / 1000u), (long)((timeout_ms % 1000u) * 1000u) };
    int rc = g_lh.select(0, 0, &w, &e, &tv);
    if (rc <= 0 || e.count > 0 || w.count == 0)
        return LIVE_HOST_FAILED;
    int so_err = 0;
    int len = sizeof(so_err);
    if (g_lh.getsockopt((w_socket)sock, W_SOL_SOCKET, W_SO_ERROR, (char *)&so_err, &len) != 0 || so_err != 0)
        return LIVE_HOST_FAILED;
    return LIVE_HOST_OK;
}

int live_host_tcp_wait_readable(live_host_socket sock, unsigned int timeout_ms)
{
    w_fd_set r = { 1, { (w_socket)sock } };
    w_timeval tv = { (long)(timeout_ms / 1000u), (long)((timeout_ms % 1000u) * 1000u) };
    int rc = g_lh.select(0, &r, 0, 0, &tv);
    if (rc < 0)
        return LIVE_HOST_FAILED;
    return rc > 0 ? 1 : 0;
}

int live_host_tcp_pending(live_host_socket sock, unsigned int *bytes_out)
{
    unsigned long avail = 0;
    if (g_lh.ioctlsocket((w_socket)sock, W_FIONREAD, &avail) != 0)
        return LIVE_HOST_FAILED;
    *bytes_out = (unsigned int)avail;
    return LIVE_HOST_OK;
}

int live_host_tcp_shutdown(live_host_socket sock, int how)
{
    return g_lh.shutdown((w_socket)sock, how) == 0 ? LIVE_HOST_OK : LIVE_HOST_FAILED;
}

void live_host_tcp_close(live_host_socket sock)
{
    if (sock != LIVE_HOST_NO_SOCKET)
        g_lh.closesocket((w_socket)sock);
}

// SSPI/Schannel and CryptoAPI public ABI. The port's MSVC-target cross build has no Windows SDK headers.
typedef struct { __UINTPTR_TYPE__ lower, upper; } sec_handle;
typedef struct { unsigned long size, type; void *data; } sec_buffer;
typedef struct { unsigned long version, count; sec_buffer *buffers; } sec_buffers;
typedef struct { unsigned long header, trailer, maximum, buffers, block; } sec_sizes;
typedef struct {
    unsigned long version, format, cert_count;
    void *certs, *roots;
    unsigned long mapper_count;
    void *mappers;
    unsigned long lifetime, flags, param_count;
    void *params;
} sch_credentials; // SCH_CREDENTIALS (v5, OS protocol defaults)
typedef struct {
    unsigned long encoding;
    unsigned char *encoded;
    unsigned long size;
    void *info, *store;
} cert_context;
typedef struct {
    unsigned long size;
    void *restricted_root, *restricted_trust, *restricted_other;
    unsigned long store_count;
    void **stores;
    unsigned long flags, timeout, max_cached, cycle;
    void *exclusive_root, *exclusive_trusted_people;
    unsigned long exclusive_flags;
} cert_engine_config;
typedef struct {
    unsigned long size;
    struct { unsigned long type; struct { unsigned long count; char **oids; } usage; } requested;
} cert_chain_para;
// The public prefix of CERT_CHAIN_CONTEXT through its CERT_TRUST_STATUS.
typedef struct { unsigned long size, trust_errors, trust_info; } cert_chain;
typedef struct { unsigned long size, auth_type, checks; unsigned short *server; } ssl_policy;
typedef struct { unsigned long size, flags; void *extra; } cert_policy;
typedef struct { unsigned long size, error; long chain, element; void *extra; } cert_policy_status;

#define SCHANNEL_FUNCTIONS(F) \
    F(sspi, long, AcquireCredentialsHandleA, (char *, char *, unsigned long, void *, void *, void *, void *, sec_handle *, void *)) \
    F(sspi, long, InitializeSecurityContextA, (sec_handle *, sec_handle *, char *, unsigned long, unsigned long, unsigned long, sec_buffers *, unsigned long, sec_handle *, sec_buffers *, unsigned long *, void *)) \
    F(sspi, long, QueryContextAttributesA, (sec_handle *, unsigned long, void *)) \
    F(sspi, long, EncryptMessage, (sec_handle *, unsigned long, sec_buffers *, unsigned long)) \
    F(sspi, long, DecryptMessage, (sec_handle *, sec_buffers *, unsigned long, unsigned long *)) \
    F(sspi, long, FreeContextBuffer, (void *)) \
    F(sspi, long, DeleteSecurityContext, (sec_handle *)) \
    F(sspi, long, FreeCredentialsHandle, (sec_handle *)) \
    F(crypt, int, CryptStringToBinaryA, (const char *, unsigned long, unsigned long, unsigned char *, unsigned long *, unsigned long *, unsigned long *)) \
    F(crypt, void *, CertOpenStore, (const char *, unsigned long, __UINTPTR_TYPE__, unsigned long, const void *)) \
    F(crypt, int, CertCloseStore, (void *, unsigned long)) \
    F(crypt, int, CertAddEncodedCertificateToStore, (void *, unsigned long, const unsigned char *, unsigned long, unsigned long, void *)) \
    F(crypt, int, CertCreateCertificateChainEngine, (const cert_engine_config *, void **)) \
    F(crypt, void, CertFreeCertificateChainEngine, (void *)) \
    F(crypt, int, CertGetCertificateChain, (void *, const cert_context *, void *, void *, const cert_chain_para *, unsigned long, void *, const cert_chain **)) \
    F(crypt, int, CertVerifyCertificateChainPolicy, (const char *, const cert_chain *, const cert_policy *, cert_policy_status *)) \
    F(crypt, void, CertFreeCertificateChain, (const cert_chain *)) \
    F(crypt, int, CertFreeCertificateContext, (const cert_context *))

static struct {
#define TLS_MEMBER(lib, type, name, params) type (__stdcall *name) params;
    SCHANNEL_FUNCTIONS(TLS_MEMBER)
#undef TLS_MEMBER
} g_tls;

static int schannel_load(void)
{
    static void *lock;
    static int loaded;
    host_AcquireSRWLockExclusive(&lock);
    if (!loaded) {
        void *sspi = host_LoadLibraryA("secur32.dll");
        void *crypt = host_LoadLibraryA("crypt32.dll");
        loaded = -1;
        if (sspi && crypt) {
#define TLS_LOAD(lib, type, name, params) if (!(g_tls.name = host_GetProcAddress(lib, #name))) goto done;
            SCHANNEL_FUNCTIONS(TLS_LOAD)
#undef TLS_LOAD
            loaded = 1;
        }
    }
done:
    host_ReleaseSRWLockExclusive(&lock);
    return loaded > 0;
}

enum {
    SEC_TOKEN = 2, SEC_DATA = 1, SEC_EXTRA = 5, SEC_TRAILER = 6, SEC_HEADER = 7,
    SEC_CONTINUE = 0x90312, SEC_EXPIRED = 0x90317, SEC_RENEGOTIATE = 0x90321,
    SEC_INCOMPLETE = (long)0x80090318, SEC_STREAM_SIZES = 4, SEC_REMOTE_CERT = 0x53,
    ISC_CONFIDENTIALITY = 0x10, ISC_USE_SUPPLIED_CREDS = 0x80, ISC_ALLOCATE = 0x100, ISC_STREAM = 0x8000,
    ISC_MANUAL_VALIDATION = 0x80000, SCH_MANUAL_VALIDATION = 8, SCH_NO_DEFAULT_CREDS = 0x10,
    SCH_STRONG_CRYPTO = 0x400000,
};

struct live_host_tls {
    live_host_socket sock;
    unsigned int timeout_ms;
    sec_handle cred, ctx;
    int have_cred, have_ctx;
    sec_sizes sizes;
    char host[128];
    void *chain_engine;
    unsigned char incoming[256 * 1024]; // space for fragmented handshake tokens and coalesced records
    unsigned int used;
    unsigned char outgoing[18 * 1024], plain[16 * 1024];
    unsigned int plain_offset, plain_size;
};

static void *private_chain_engine(const char *path)
{
    void *file = host_CreateFileA(path, 0x80000000ul, 1ul, 0, 3ul, 0x80ul, 0);
    if (file == (void *)(int)-1)
        return 0;
    long long length = 0;
    char *pem = 0;
    if (host_GetFileSizeEx(file, &length) && length > 0 && length < 0x7fffffff)
        pem = lan_host_pages_alloc((unsigned int)length + 1);
    unsigned long bytes_read = 0;
    int read_ok = pem && host_ReadFile(file, pem, (unsigned long)length, &bytes_read, 0) && bytes_read == length;
    host_CloseHandle(file);
    if (!pem)
        return 0;
    void *engine = 0, *store = 0;
    if (!read_ok)
        goto done;
    pem[bytes_read] = 0;
    store = g_tls.CertOpenStore((const char *)2 /* CERT_STORE_PROV_MEMORY */, 0, 0, 0, 0);
    if (!store)
        goto done;
    const char end[] = "-----END CERTIFICATE-----";
    char *block = pem;
    unsigned int count = 0;
    while ((block = __builtin_strstr(block, "-----BEGIN CERTIFICATE-----")) != 0) {
        char *next = __builtin_strstr(block, end);
        if (!next)
            goto done;
        next += sizeof(end) - 1;
        unsigned long bytes = (unsigned long)(next - block);
        // Decode in a separate buffer. CryptStringToBinary does not promise overlapping input/output.
        unsigned char *der = lan_host_pages_alloc(bytes);
        if (!der)
            goto done;
        unsigned long der_size = bytes;
        int ok = g_tls.CryptStringToBinaryA(block, bytes, 0 /* CRYPT_STRING_BASE64HEADER */, der, &der_size, 0, 0)
            && g_tls.CertAddEncodedCertificateToStore(store, 1 /* X509_ASN_ENCODING */, der, der_size, 2, 0);
        lan_host_pages_free(der, bytes);
        if (!ok)
            goto done;
        count++;
        block = next;
    }
    if (count) {
        cert_engine_config config = { 0 };
        config.size = sizeof(config);
        config.exclusive_root = store;
        if (!g_tls.CertCreateCertificateChainEngine(&config, &engine)) {
            // Older CryptoAPI versions take the Windows 7 layout, also with hExclusiveRoot.
            config.size = __builtin_offsetof(cert_engine_config, exclusive_flags);
            if (!g_tls.CertCreateCertificateChainEngine(&config, &engine))
                engine = 0;
        }
    }
done:
    if (store)
        g_tls.CertCloseStore(store, 0);
    lan_host_pages_free(pem, (unsigned int)length + 1);
    return engine;
}

static int verify_certificate(live_host_tls *tls)
{
    // A null engine uses the system roots. Apply the SSL server policy to both trust sources
    // before accepting any application data, including after a renegotiation.
    const cert_context *cert = 0;
    if (g_tls.QueryContextAttributesA(&tls->ctx, SEC_REMOTE_CERT, &cert) != 0 || !cert)
        return 0;
    char *eku = "1.3.6.1.5.5.7.3.1"; // id-kp-serverAuth
    cert_chain_para para = { sizeof(para), { 0, { 1, &eku } } };
    const cert_chain *chain = 0;
    int ok = g_tls.CertGetCertificateChain(tls->chain_engine, cert, 0, cert->store, &para, 0, 0, &chain);
    if (ok) {
        unsigned short hostname[sizeof(tls->host)];
        for (unsigned int i = 0; i < sizeof(tls->host); i++)
            hostname[i] = (unsigned char)tls->host[i];
        ssl_policy ssl = { sizeof(ssl), 2 /* AUTHTYPE_SERVER */, 0, hostname };
        cert_policy policy = { sizeof(policy), 0, &ssl };
        cert_policy_status status = { 0 };
        status.size = sizeof(status);
        ok = chain->trust_errors == 0 &&
            g_tls.CertVerifyCertificateChainPolicy((const char *)4 /* CERT_CHAIN_POLICY_SSL */, chain, &policy, &status)
            && status.error == 0;
        g_tls.CertFreeCertificateChain(chain);
    }
    g_tls.CertFreeCertificateContext(cert);
    return ok;
}

static unsigned long long tls_deadline(live_host_tls *tls)
{
    return lan_host_microseconds() + tls->timeout_ms * 1000ull;
}

static int tls_wait(live_host_tls *tls, int reading, unsigned long long deadline)
{
    unsigned long long now = lan_host_microseconds();
    if (now >= deadline)
        return 0;
    unsigned int left = (unsigned int)((deadline - now + 999) / 1000);
    if (reading)
        return live_host_tcp_wait_readable(tls->sock, left) == 1;
    return live_host_tcp_wait_connect(tls->sock, left) == LIVE_HOST_OK;
}

static int tls_fill(live_host_tls *tls, unsigned long long deadline)
{
    if (tls->used == sizeof(tls->incoming) || !tls_wait(tls, 1, deadline))
        return 0;
    int n = live_host_tcp_recv(tls->sock, tls->incoming + tls->used, (int)(sizeof(tls->incoming) - tls->used));
    if (n <= 0)
        return 0; // TCP EOF without a TLS close_notify is a truncated stream
    tls->used += (unsigned int)n;
    return 1;
}

static int tls_flush(live_host_tls *tls, const char *p, unsigned long size, unsigned long long deadline)
{
    while (size) {
        if (!tls_wait(tls, 0, deadline))
            return 0;
        int n = live_host_tcp_send(tls->sock, p, (int)size);
        if (n <= 0)
            return 0;
        p += n;
        size -= (unsigned long)n;
    }
    return 1;
}

static void tls_keep_extra(live_host_tls *tls, unsigned long extra)
{
    __builtin_memmove(tls->incoming, tls->incoming + tls->used - extra, extra);
    tls->used = extra;
}

static int tls_handshake(live_host_tls *tls, unsigned long long deadline)
{
    unsigned long flags = ISC_STREAM | ISC_CONFIDENTIALITY | ISC_USE_SUPPLIED_CREDS | ISC_ALLOCATE | ISC_MANUAL_VALIDATION;
    for (;;) {
        sec_buffer input[2] = { { tls->used, SEC_TOKEN, tls->incoming }, { 0, 0, 0 } };
        sec_buffers in = { 0, 2, input };
        sec_buffer token = { 0, SEC_TOKEN, 0 };
        sec_buffers out = { 0, 1, &token };
        unsigned long attributes = 0;
        long status = g_tls.InitializeSecurityContextA(&tls->cred, tls->have_ctx ? &tls->ctx : 0, tls->host,
            flags, 0, 0x10 /* SECURITY_NATIVE_DREP */, tls->have_ctx ? &in : 0, 0, &tls->ctx, &out, &attributes, 0);
        if (status == 0 || status == SEC_CONTINUE)
            tls->have_ctx = 1;
        int sent = 1;
        if (token.data) {
            sent = tls_flush(tls, token.data, token.size, deadline);
            g_tls.FreeContextBuffer(token.data);
        }
        if (!sent)
            return 0;
        if (status != SEC_INCOMPLETE) {
            if (status != 0 && status != SEC_CONTINUE)
                return 0;
            tls_keep_extra(tls, input[1].type == SEC_EXTRA ? input[1].size : 0);
            if (status == 0)
                return (attributes & ISC_CONFIDENTIALITY) && verify_certificate(tls) &&
                    g_tls.QueryContextAttributesA(&tls->ctx, SEC_STREAM_SIZES, &tls->sizes) == 0 &&
                    tls->sizes.maximum > 0 && tls->sizes.maximum <= sizeof(tls->plain) &&
                    (unsigned long long)tls->sizes.header + tls->sizes.maximum + tls->sizes.trailer <= sizeof(tls->outgoing);
            if (tls->used)
                continue;
        }
        if (!tls_fill(tls, deadline))
            return 0;
    }
}

const char *live_host_tls_connect(live_host_socket sock, const char *host, const char *ca_file,
    unsigned int timeout_ms, live_host_tls **out)
{
    *out = 0;
    if (!schannel_load())
        return "live: HTTPS unavailable (could not load Schannel or CryptoAPI)";
    live_host_tls *tls = lan_host_pages_alloc(sizeof(*tls));
    if (!tls)
        return "live: HTTPS out of memory";
    tls->sock = sock;
    tls->timeout_ms = timeout_ms;
    unsigned int length = (unsigned int)__builtin_strlen(host);
    const char *error = "live: HTTPS invalid server name";
    if (!length || length >= sizeof(tls->host))
        goto failed;
    __builtin_memcpy(tls->host, host, length + 1);
    if (ca_file && !(tls->chain_engine = private_chain_engine(ca_file))) {
        error = "live: HTTPS could not load the trusted CA file";
        goto failed;
    }
    sch_credentials cred = { 0 };
    cred.version = 5;
    cred.flags = SCH_STRONG_CRYPTO | SCH_NO_DEFAULT_CREDS | SCH_MANUAL_VALIDATION;
    error = "live: HTTPS could not initialize Schannel credentials";
    if (g_tls.AcquireCredentialsHandleA(0, "Microsoft Unified Security Protocol Provider", 2 /* outbound */,
            0, &cred, 0, 0, &tls->cred, 0) != 0)
        goto failed;
    tls->have_cred = 1;
    error = "live: HTTPS TLS handshake or certificate/hostname verification failed";
    if (!tls_handshake(tls, tls_deadline(tls)))
        goto failed;
    *out = tls;
    return 0;
failed:
    live_host_tls_close(tls);
    return error;
}

int live_host_tls_send(live_host_tls *tls, const void *data, int size)
{
    unsigned long n = (unsigned long)size;
    if (n > tls->sizes.maximum)
        n = tls->sizes.maximum;
    unsigned char *payload = tls->outgoing + tls->sizes.header;
    __builtin_memcpy(payload, data, n);
    sec_buffer record[4] = { { tls->sizes.header, SEC_HEADER, tls->outgoing }, { n, SEC_DATA, payload },
        { tls->sizes.trailer, SEC_TRAILER, payload + n }, { 0, 0, 0 } };
    sec_buffers message = { 0, 4, record };
    if (g_tls.EncryptMessage(&tls->ctx, 0, &message, 0) != 0)
        return LIVE_HOST_FAILED;
    unsigned long bytes = record[0].size + record[1].size + record[2].size;
    return tls_flush(tls, (char *)tls->outgoing, bytes, tls_deadline(tls)) ? (int)n : LIVE_HOST_FAILED;
}

int live_host_tls_recv(live_host_tls *tls, void *buffer, int size)
{
    unsigned long long deadline = tls_deadline(tls);
    while (!tls->plain_size) {
        if (!tls->used && !tls_fill(tls, deadline))
            return LIVE_HOST_FAILED;
        sec_buffer parts[4] = { { tls->used, SEC_DATA, tls->incoming }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
        sec_buffers message = { 0, 4, parts };
        long status = g_tls.DecryptMessage(&tls->ctx, &message, 0, 0);
        if (status == SEC_EXPIRED)
            return 0;
        if (status == SEC_INCOMPLETE) {
            if (!tls_fill(tls, deadline))
                return LIVE_HOST_FAILED;
            continue;
        }
        if (status != 0 && status != SEC_RENEGOTIATE)
            return LIVE_HOST_FAILED;
        unsigned long extra = 0;
        for (int i = 0; i < 4; i++) {
            if (parts[i].type == SEC_EXTRA)
                extra = parts[i].size;
            if (parts[i].type == SEC_DATA && parts[i].size) {
                if (parts[i].size > sizeof(tls->plain))
                    return LIVE_HOST_FAILED;
                __builtin_memcpy(tls->plain, parts[i].data, parts[i].size);
                tls->plain_size = parts[i].size;
                tls->plain_offset = 0;
            }
        }
        tls_keep_extra(tls, extra);
        if (status == SEC_RENEGOTIATE && !tls_handshake(tls, deadline))
            return LIVE_HOST_FAILED;
    }
    unsigned int n = tls->plain_size < (unsigned int)size ? tls->plain_size : (unsigned int)size;
    __builtin_memcpy(buffer, tls->plain + tls->plain_offset, n);
    tls->plain_offset += n;
    tls->plain_size -= n;
    return (int)n;
}

int live_host_tls_pending(live_host_tls *tls) { return tls->plain_size || tls->used; }

void live_host_tls_close(live_host_tls *tls)
{
    if (tls->have_ctx)
        g_tls.DeleteSecurityContext(&tls->ctx);
    if (tls->have_cred)
        g_tls.FreeCredentialsHandle(&tls->cred);
    if (tls->chain_engine)
        g_tls.CertFreeCertificateChainEngine(tls->chain_engine);
    lan_host_pages_free(tls, sizeof(*tls));
}

static void *g_live_condition; // CONDITION_VARIABLE (one pointer, zero-initialized)

void live_host_lock(void) { host_AcquireSRWLockExclusive(&g_live_lock); }
void live_host_unlock(void) { host_ReleaseSRWLockExclusive(&g_live_lock); }

void live_host_wait(unsigned int timeout_ms)
{
    // INFINITE is 0xffffffff, as LIVE_HOST_FOREVER; 0 flags: the lock is held exclusively
    host_SleepConditionVariableSRW(&g_live_condition, &g_live_lock, timeout_ms, 0);
}

void live_host_wake_all(void) { host_WakeAllConditionVariable(&g_live_condition); }

typedef struct {
    void (*entry)(void *);
    void *arg;
} live_thread_start;

static live_thread_start g_thread_slots[8];
static unsigned int g_thread_slot_idx;

static unsigned long __stdcall live_thread_trampoline(void *p)
{
    live_thread_start *s = (live_thread_start *)p;
    s->entry(s->arg);
    return 0;
}

int live_host_thread_start(void (*entry)(void *), void *arg)
{
    unsigned int idx = __atomic_fetch_add(&g_thread_slot_idx, 1, __ATOMIC_RELAXED) & 7u;
    g_thread_slots[idx].entry = entry;
    g_thread_slots[idx].arg = arg;
    void *h = host_CreateThread(0, 0, live_thread_trampoline, &g_thread_slots[idx], 0, 0);
    if (h == 0)
        return -1;
    host_CloseHandle(h);
    return 0;
}

unsigned long live_host_thread_id(void) { return host_GetCurrentThreadId(); }

void live_host_sleep_ms(unsigned int ms) { host_Sleep(ms); }

int live_host_read_file(const char *path, char *buffer, unsigned int capacity)
{
    // Share reads and allow an atomic replacement while this handle still reads the previous record.
    void *h = host_CreateFileA(path, 0x80000000ul, 5ul, 0, 3ul, 0x80ul, 0);
    if (h == (void *)(int)-1)
        return -1;
    unsigned long n = 0;
    int ok = host_ReadFile(h, buffer, capacity - 1, &n, 0);
    host_CloseHandle(h);
    if (!ok)
        return -1;
    buffer[n] = 0;
    return (int)n;
}

int live_host_write_file(const char *path, const char *data, unsigned int size)
{
    char temporary[1024];
    unsigned int len = 0;
    while (path[len] != 0 && len + sizeof(".tmp") <= sizeof(temporary))
        len++;
    if (len == 0 || len + sizeof(".tmp") > sizeof(temporary))
        return -1;
    __builtin_memcpy(temporary, path, len);
    __builtin_memcpy(temporary + len, ".tmp", sizeof(".tmp"));
    // CREATE_NEW, with the data directory's inherited ACL and a non-inheritable handle.
    void *h = host_CreateFileA(temporary, 0x40000000ul, 0ul, 0, 1ul, 0x80ul, 0);
    if (h == (void *)(int)-1)
        return -1;
    unsigned int total = 0;
    while (total < size) {
        unsigned long n = 0;
        if (!host_WriteFile(h, data + total, size - total, &n, 0) || n == 0)
            break;
        total += n;
    }
    const int flushed = total == size && host_FlushFileBuffers(h);
    const int closed = host_CloseHandle(h);
    // MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH
    if (flushed && closed && host_MoveFileExA(temporary, path, 0x9ul))
        return (int)total;
    host_DeleteFileA(temporary);
    return -1;
}
