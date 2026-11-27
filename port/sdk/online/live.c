// port/sdk/online (both hosts): clean Xbox Live client for halo-3-server (slopboxlive.com / zboxlive.com).
// HTTP/1.1 JSON over host TCP or hostname-verified TLS (live_host.h).
#include "lan.h"
#include "live.h"
#include "invite.h"
#include "lan_host.h"
#include "live_host.h"
#include "../xam/xam_profile.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern double strtod(const char *, char **);

#ifndef PORT_LIVE_PROTOCOL_VERSION
#define PORT_LIVE_PROTOCOL_VERSION 1
#endif

#define LIVE_TITLE_ID "4D5307E6"
#define LIVE_MAX_TCP_SOCKETS 64
#define LIVE_MAX_LSP_SERVERS 16
#define LIVE_MAX_USER_PROPS 32
#define LIVE_MAX_USER_CTXS 32
#define LIVE_MAX_HOSTED_SESSIONS 8
#ifndef LIVE_HTTP_TIMEOUT_MS // port/tests/unit/test_live_http.c sets a shorter one
#define LIVE_HTTP_TIMEOUT_MS 5000u
#endif
#define LIVE_HEARTBEAT_MS 15000u
#define LIVE_INVITE_POLL_MS 5000u
#define LIVE_INVITE_POLL_MAX_MS 60000u
#define LIVE_RESP_BUF_SIZE (128 * 1024)
#define LIVE_HTTP_IDLE_CAPACITY 4
#define LIVE_HTTP_IDLE_REUSE_MS 10000u
#define LIVE_JOB_CAPACITY 64 // ring slots (PORT_LIVE_POST_MAX bytes each); more calls wait in a list
#define LIVE_TITLE_SERVERS_REFRESH_MS 300000u
#define LIVE_SIGNIN_RETRY_MS 2000u
#define LIVE_SIGNIN_RETRY_MAX_MS 60000u
#define LIVE_DRAIN_MS 3000u
#define LIVE_ACCOUNT_PATH_MAX 1020u
#define LIVE_ORIGIN_MAX 144u
#define LIVE_LEGACY_ORIGIN "http://slopboxlive.com"
#define LIVE_SECURE_ORIGIN "https://slopboxlive.com"
#define LIVE_DEFAULT_RELAY_PORT 3075
#define LIVE_RELAY_TICKET_CAPACITY 129
#define LIVE_RELAY_RETRY_SECONDS 60u
#define LIVE_TOKEN_CAPACITY sizeof(((port_live_account *)0)->token)

// ---- small string / formatting helpers (no libc malloc or printf) ----

static unsigned int str_len(const char *s)
{
    unsigned int n = 0;
    while (s != 0 && s[n] != 0)
        n++;
    return n;
}

static int str_eq_ci_n(const char *a, const char *b, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z')
            ca = (char)(ca + 32);
        if (cb >= 'A' && cb <= 'Z')
            cb = (char)(cb + 32);
        if (ca != cb)
            return 0;
    }
    return 1;
}

typedef struct {
    char *data;
    unsigned int len;
    unsigned int cap;
} sbuf;

static void sb_init(sbuf *b, char *buf, unsigned int cap)
{
    b->data = buf;
    b->len = 0;
    b->cap = cap;
    if (cap > 0)
        buf[0] = 0;
}

static void sb_char(sbuf *b, char c)
{
    if (b->len + 1 < b->cap) {
        b->data[b->len++] = c;
        b->data[b->len] = 0;
    }
}

static void sb_str(sbuf *b, const char *s)
{
    while (s != 0 && *s)
        sb_char(b, *s++);
}

static void sb_json_str(sbuf *b, const char *s)
{
    sb_char(b, '"');
    while (s != 0 && *s) {
        unsigned char c = (unsigned char)*s++;
        if (c == '"' || c == '\\') {
            sb_char(b, '\\');
            sb_char(b, (char)c);
        } else if (c >= 0x20 && c < 0x7f) {
            sb_char(b, (char)c);
        }
    }
    sb_char(b, '"');
}

static void sb_u64(sbuf *b, u64 v)
{
    char tmp[24];
    int n = 0;
    do {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    } while (v != 0);
    while (n-- > 0)
        sb_char(b, tmp[n]);
}

static void sb_i64(sbuf *b, long long v)
{
    if (v < 0) {
        sb_char(b, '-');
        sb_u64(b, (u64)(-(v + 1)) + 1ull);
    } else {
        sb_u64(b, (u64)v);
    }
}

static void sb_double(sbuf *b, double v)
{
    if (v < 0.0) {
        sb_char(b, '-');
        v = -v;
    }
    u64 whole = (u64)v;
    u64 frac = (u64)((v - (double)whole) * 1000000.0 + 0.5);
    if (frac >= 1000000ull) {
        whole++;
        frac -= 1000000ull;
    }
    sb_u64(b, whole);
    sb_char(b, '.');
    u64 div = 100000ull;
    for (int i = 0; i < 6; i++) {
        sb_char(b, (char)('0' + ((frac / div) % 10)));
        div /= 10;
    }
}

static void sb_hex_bytes(sbuf *b, const u8 *bytes, unsigned int count)
{
    static const char hex[] = "0123456789ABCDEF";
    for (unsigned int i = 0; i < count; i++) {
        sb_char(b, hex[(bytes[i] >> 4) & 0xf]);
        sb_char(b, hex[bytes[i] & 0xf]);
    }
}

static void sb_hex32(sbuf *b, u32 v)
{
    u8 bytes[4] = { (u8)(v >> 24), (u8)(v >> 16), (u8)(v >> 8), (u8)v };
    sb_hex_bytes(b, bytes, 4);
}

static void sb_hex64(sbuf *b, u64 v)
{
    u8 bytes[8] = {
        (u8)(v >> 56), (u8)(v >> 48), (u8)(v >> 40), (u8)(v >> 32),
        (u8)(v >> 24), (u8)(v >> 16), (u8)(v >> 8), (u8)v
    };
    sb_hex_bytes(b, bytes, 8);
}

static void sb_ipv4(sbuf *b, u32 ip)
{
    sb_u64(b, (ip >> 24) & 0xff);
    sb_char(b, '.');
    sb_u64(b, (ip >> 16) & 0xff);
    sb_char(b, '.');
    sb_u64(b, (ip >> 8) & 0xff);
    sb_char(b, '.');
    sb_u64(b, ip & 0xff);
}

static void sb_base64(sbuf *b, const u8 *in, unsigned int len)
{
    static const char tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (unsigned int i = 0; i < len; i += 3) {
        u32 v = ((u32)in[i]) << 16;
        if (i + 1 < len)
            v |= ((u32)in[i + 1]) << 8;
        if (i + 2 < len)
            v |= (u32)in[i + 2];
        sb_char(b, tbl[(v >> 18) & 63]);
        sb_char(b, tbl[(v >> 12) & 63]);
        sb_char(b, i + 1 < len ? tbl[(v >> 6) & 63] : '=');
        sb_char(b, i + 2 < len ? tbl[v & 63] : '=');
    }
}

static int b64_val(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    if (c >= '0' && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return -1;
}

static unsigned int base64_decode(const char *in, u8 *out, unsigned int out_cap)
{
    unsigned int out_len = 0;
    u32 acc = 0;
    int bits = 0;
    for (; *in && *in != '='; in++) {
        int v = b64_val(*in);
        if (v < 0)
            continue;
        acc = (acc << 6) | (u32)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            if (out_len < out_cap)
                out[out_len++] = (u8)((acc >> bits) & 0xff);
        }
    }
    return out_len;
}

static int hex_nibble(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static int parse_hex_bytes(const char *s, u8 *out, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++) {
        int hi = hex_nibble(s[i * 2]);
        int lo = hex_nibble(s[i * 2 + 1]);
        if (hi < 0 || lo < 0)
            return 0;
        out[i] = (u8)((hi << 4) | lo);
    }
    return 1;
}

static int parse_hex_u64(const char *s, u64 *out)
{
    u8 bytes[8];
    if (!parse_hex_bytes(s, bytes, 8))
        return 0;
    u64 v = 0;
    for (int i = 0; i < 8; i++)
        v = (v << 8) | bytes[i];
    *out = v;
    return 1;
}

static int parse_ipv4_str(const char *s, u32 *out)
{
    u32 ip = 0;
    for (int i = 0; i < 4; i++) {
        if (*s < '0' || *s > '9')
            return 0;
        u32 oct = 0;
        while (*s >= '0' && *s <= '9') {
            oct = oct * 10 + (u32)(*s++ - '0');
            if (oct > 255)
                return 0;
        }
        ip = (ip << 8) | oct;
        if (i < 3) {
            if (*s != '.')
                return 0;
            s++;
        }
    }
    if (*s != 0)
        return 0;
    *out = ip;
    return 1;
}

static u32 load_u32_le(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static u16 load_u16_le(const u8 *p) { return (u16)((u16)p[0] | ((u16)p[1] << 8)); }

static u32 live_xnaddr_ip(const u8 *xnaddr)
{
    u32 ip = load_u32_le(xnaddr + 4);
    if (ip == 0)
        ip = load_u32_le(xnaddr);
    return ip != 0 ? ip : 0x7f000001u;
}

// ---- minimal JSON value extractors ----

static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
        p++;
    return p;
}

// Skips one JSON value starting at p, returning pointer just after it.
static const char *json_skip_value(const char *p)
{
    p = skip_ws(p);
    if (*p == '"') {
        p++;
        while (*p && *p != '"') {
            if (*p == '\\' && p[1])
                p++;
            p++;
        }
        return *p == '"' ? p + 1 : p;
    }
    if (*p == '{' || *p == '[') {
        char open = *p, close = (open == '{') ? '}' : ']';
        int depth = 1;
        p++;
        while (*p && depth > 0) {
            if (*p == '"') {
                p = json_skip_value(p);
                continue;
            }
            if (*p == open)
                depth++;
            else if (*p == close)
                depth--;
            p++;
        }
        return p;
    }
    while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
        p++;
    return p;
}

// Finds key in a JSON object starting at obj ('{'). Returns pointer to value or 0.
static const char *json_find_key(const char *obj, const char *key)
{
    const char *p = skip_ws(obj);
    if (*p != '{')
        return 0;
    p = skip_ws(p + 1);
    unsigned int klen = str_len(key);
    while (*p && *p != '}') {
        if (*p != '"')
            return 0;
        const char *kstart = p + 1;
        p = json_skip_value(p);
        unsigned int len = (unsigned int)((p - 1) - kstart);
        p = skip_ws(p);
        if (*p != ':')
            return 0;
        p = skip_ws(p + 1);
        if (len == klen && __builtin_memcmp(kstart, key, klen) == 0)
            return p;
        p = json_skip_value(p);
        p = skip_ws(p);
        if (*p == ',')
            p = skip_ws(p + 1);
    }
    return 0;
}

static int json_read_string(const char *val, char *out, unsigned int cap)
{
    val = skip_ws(val);
    if (*val != '"')
        return 0;
    val++;
    unsigned int n = 0;
    while (*val && *val != '"') {
        char c = *val++;
        if (c == '\\' && *val)
            c = *val++;
        if (n + 1 < cap)
            out[n++] = c;
    }
    if (cap > 0)
        out[n] = 0;
    return *val == '"';
}

// Whether the JSON string at `val` fits `cap` bytes, NUL included (json_read_string cuts one that does not).
static int json_string_fits(const char *val, unsigned int cap)
{
    val = skip_ws(val);
    if (*val != '"')
        return 0;
    val++;
    unsigned int n = 0;
    while (*val && *val != '"') {
        if (*val++ == '\\' && *val)
            val++;
        n++;
    }
    return n < cap;
}

static int json_read_uint(const char *val, u32 *out)
{
    val = skip_ws(val);
    if (*val < '0' || *val > '9')
        return 0;
    u32 v = 0;
    while (*val >= '0' && *val <= '9')
        v = v * 10 + (u32)(*val++ - '0');
    *out = v;
    return 1;
}

// ---- global Live state ----

typedef struct {
    int used;
    live_host_socket fd;
    int nonblocking;
    int connecting; // a connect in progress (HALO3_LIVE_LOG reports how it ends)
    u64 connect_us;
    u32 connect_ip;
    u16 connect_port;
} live_tcp_slot;

typedef struct {
    live_host_socket fd;
    live_host_tls *tls;
} live_connection;

typedef struct {
    live_connection conn;
    u64 since_us; // idle since
} live_idle_connection;

// A call on the Live worker (port_live_post): its function and a copy of its data, in its ring slot's storage
// (PORT_LIVE_POST_MAX bytes a slot, allocated once with the worker: no allocation per call) or its list node's.
typedef struct {
    void (PORT_MS_ABI *run)(const void *data);
    unsigned int size;
    u8 *data;
} live_job;

// A call posted while the queue's ring is full (a server that does not answer: LIVE_HTTP_TIMEOUT_MS a call): it waits
// in a list, in order, for room in the ring. Its data follows it, 8-byte aligned (xlive_offline.cpp's jobs hold
// 8-byte members, laid out by the MSVC rules).
typedef struct live_job_node {
    struct live_job_node *next;
    unsigned int bytes; // the allocation's size
    live_job job;
    u64 data[];
} live_job_node;

typedef struct {
    u32 real_ip;
    u16 real_port;
    u32 secure_ip;
} live_lsp_entry;

typedef struct {
    int used;
    u8 xnkid[8];
} live_hosted_session;

enum { LIVE_AUTO_DISABLED, LIVE_AUTO_CONFIGURED, LIVE_AUTO_LEGACY };

typedef struct {
    int enabled;
    int signed_in;
    u64 xuid;
    char profile[PORT_PROFILE_GAMERTAG_MAX + 1];
    char gamertag[32];
    char token[LIVE_TOKEN_CAPACITY];
    char legacy_path[LIVE_ACCOUNT_PATH_MAX];
    char startup_name[32];
    int auto_signin;
    int connecting;
    u32 generation;
    port_live_auth_result auth;
} live_user;

static struct {
    int configured;
    int enabled;
    int log_enabled;
    int started;
    int stop_heartbeat;
    volatile int friends_unsupported; // the server answered 404 (port_live_friends)
    volatile int invites_unsupported; // ... (game invites)

    int use_tls;
    char server_host[128];
    char server_origin[LIVE_ORIGIN_MAX];
    u32 server_ip;
    u16 server_port, relay_port;
    char relay_ticket[LIVE_RELAY_TICKET_CAPACITY];
    u64 relay_refresh_us; // under the lock: 0 when due, ~0ull while fetching or after a 404

    char data_path[LIVE_ACCOUNT_PATH_MAX];
    int data_path_fits;
    live_user users[4];

    port_live_property user_props[4][LIVE_MAX_USER_PROPS];
    u32 user_prop_count[4];
    port_live_context user_ctxs[4][LIVE_MAX_USER_CTXS];
    u32 user_ctx_count[4];

    live_hosted_session hosted[LIVE_MAX_HOSTED_SESSIONS];

    live_lsp_entry lsp_servers[LIVE_MAX_LSP_SERVERS];
    int lsp_count;

    live_tcp_slot tcp[LIVE_MAX_TCP_SOCKETS];

    // HTTP connections kept alive, the newest last (under the lock)
    live_idle_connection idle[LIVE_HTTP_IDLE_CAPACITY];
    int idle_count;

    // the Live worker's queue (under the lock): a ring (the call at its head is the one running, if any), then, while
    // it is full, a list
    live_job jobs[LIVE_JOB_CAPACITY];
    u8 *job_storage; // LIVE_JOB_CAPACITY slots of PORT_LIVE_POST_MAX bytes
    u32 job_head, job_count;
    live_job_node *overflow_head, *overflow_tail;
    int worker_started, job_running;
    unsigned long worker_thread; // its live_host_thread_id, once it runs

    // users signed in (bits 0..3) and their guests (LIVE_REGISTER_GUESTS) whose registration as online players is not
    // made yet (register_pending); after one failed, when it is tried again
    u32 register_pending;
    u32 register_failures;
    u64 register_retry_us;

    // a push of a user's properties to the sessions hosted is queued and not started (port_live_set_*)
    volatile int props_push_queued[4];

    // the title servers last listed (under the lock): when, and whether a refresh is queued
    port_live_title_server title_servers[LIVE_MAX_LSP_SERVERS];
    int title_server_count;
    u64 title_servers_us;
    int title_servers_refreshing;

    // sign-in: failed attempts in a row, and when the next may be made (under the lock)
    u32 signin_failures;
    u64 signin_retry_us;
    int signin_running; // a sign-in is under way (port_live_startup)
    int signin_queued;  // a retry is queued on the worker (signin_job)
    int heartbeat_started;
} g_live;

static void live_log(const char *msg)
{
    if (g_live.log_enabled)
        lan_host_log(msg);
}

static void live_log2(const char *a, const char *b)
{
    if (!g_live.log_enabled)
        return;
    char line[512];
    sbuf s;
    sb_init(&s, line, sizeof(line));
    sb_str(&s, "live: ");
    sb_str(&s, a);
    sb_str(&s, b);
    lan_host_log(line);
}

// ---- HALO3_LIVE_LOG timing ----

// "12.3": microseconds as milliseconds, to a tenth.
static void sb_ms(sbuf *b, u64 microseconds)
{
    sb_u64(b, microseconds / 1000u);
    sb_char(b, '.');
    sb_char(b, (char)('0' + (microseconds / 100u) % 10u));
}

PORT_MS_ABI unsigned long long port_live_trace_start(void)
{
    return port_live_enabled() && g_live.log_enabled ? lan_host_microseconds() : 0;
}

PORT_MS_ABI void port_live_trace_end(const char *what, const char *how, unsigned long long start)
{
    if (start == 0)
        return;
    char line[160];
    sbuf s;
    sb_init(&s, line, sizeof(line));
    sb_str(&s, "live: ");
    sb_str(&s, what);
    sb_char(&s, ' ');
    sb_str(&s, how);
    sb_str(&s, " after ");
    sb_ms(&s, lan_host_microseconds() - start);
    sb_str(&s, " ms");
    lan_host_log(line);
}

static const char *account_json_string(const char *json, const char *key, char *out, unsigned int capacity)
{
    const char *value = json_find_key(json, key);
    return value && json_string_fits(value, capacity) && json_read_string(value, out, capacity) ? value : 0;
}

static int same_text_ci(const char *a, const char *b)
{
    const unsigned int n = str_len(a);
    return n == str_len(b) && str_eq_ci_n(a, b, n);
}

static char lower_ascii(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + 'a' - 'A') : c; }

static int parse_origin(const char *url, char host[128], u16 *port, int *use_tls, char origin[LIVE_ORIGIN_MAX])
{
    if (!url || !*url)
        return 0;
    const unsigned int length = str_len(url);
    const int https = length >= 8 && str_eq_ci_n(url, "https://", 8);
    const char *p = url + (https ? 8 : length >= 7 && str_eq_ci_n(url, "http://", 7) ? 7 : 0);
    unsigned int n = 0;
    while (p[n] && p[n] != ':' && p[n] != '/') {
        const char c = lower_ascii(p[n]);
        if (n == 127 || !((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-'))
            return 0;
        host[n++] = c;
    }
    if (n == 0)
        return 0;
    host[n] = 0;
    p += n;
    *use_tls = https;
    *port = https ? 443 : 80;
    if (*p == ':') {
        u32 parsed = 0;
        for (p++; *p >= '0' && *p <= '9'; p++) {
            parsed = 10 * parsed + (u32)(*p - '0');
            if (parsed > 65535)
                return 0;
        }
        if (parsed == 0)
            return 0;
        *port = (u16)parsed;
    }
    if (*p != 0 && *p != '/')
        return 0;
    sbuf b;
    sb_init(&b, origin, LIVE_ORIGIN_MAX);
    sb_str(&b, https ? "https://" : "http://");
    sb_str(&b, host);
    if (*port != (https ? 443 : 80)) {
        sb_char(&b, ':');
        sb_u64(&b, *port);
    }
    return 1;
}

static int legacy_service(void)
{
    return same_text_ci(g_live.server_origin, LIVE_LEGACY_ORIGIN) || same_text_ci(g_live.server_origin, LIVE_SECURE_ORIGIN);
}

static int account_server_matches(const char *json)
{
    char server[LIVE_ORIGIN_MAX] = { 0 };
    if (!json_find_key(json, "server")) {
        // Before origin metadata, the shipped service was SlopBoxLive.
        return legacy_service();
    }
    char host[128], origin[LIVE_ORIGIN_MAX];
    u16 port;
    int use_tls;
    return account_json_string(json, "server", server, sizeof(server)) &&
           parse_origin(server, host, &port, &use_tls, origin) &&
           (same_text_ci(origin, g_live.server_origin) ||
               (same_text_ci(origin, LIVE_LEGACY_ORIGIN) && same_text_ci(g_live.server_origin, LIVE_SECURE_ORIGIN)));
}

static void account_path_begin(sbuf *path, char *out)
{
    sb_init(path, out, LIVE_ACCOUNT_PATH_MAX);
    sb_str(path, g_live.data_path);
    if (path->len == 0)
        return;
    const char last = out[path->len - 1];
#ifdef _WIN32
    if (last == '\\')
        return;
#endif
    if (last != '/')
        sb_char(path, '/');
}

static void profile_account_path_for_origin(const char *profile, const char *origin, char *out)
{
    sbuf path;
    account_path_begin(&path, out);
    sb_str(&path, "live_account.");
    const unsigned int slug_start = path.len;
    u64 hash = 0xcbf29ce484222325ull;
    for (const char *p = profile; *p; p++) {
        const char c = lower_ascii(*p);
        hash = (hash ^ (u8)c) * 0x100000001b3ull;
        if (path.len - slug_start < PORT_PROFILE_GAMERTAG_MAX)
            sb_char(&path, (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ? c : '-');
    }
    hash = (hash ^ '/') * 0x100000001b3ull;
    for (const char *p = origin; *p; p++)
        hash = (hash ^ (u8)*p) * 0x100000001b3ull;
    sb_char(&path, '.');
    sb_hex64(&path, hash);
    sb_str(&path, ".json");
    if (!g_live.data_path_fits || path.len == path.cap - 1)
        out[0] = 0;
}

static void profile_account_path(const char *profile, char *out)
{
    profile_account_path_for_origin(profile, g_live.server_origin, out);
}

static u16 live_relay_port(void)
{
    const char *p = lan_host_getenv("HALO3_RELAY_PORT");
    if (p == 0 || *p == 0)
        return LIVE_DEFAULT_RELAY_PORT;
    u32 port = 0;
    for (; *p != 0; p++) {
        if (*p < '0' || *p > '9')
            return LIVE_DEFAULT_RELAY_PORT;
        port = port * 10 + (u32)(*p - '0');
        if (port > 65535)
            return LIVE_DEFAULT_RELAY_PORT;
    }
    return (u16)port;
}

static void live_configure(void)
{
    if (g_live.configured)
        return;
    const char *log = lan_host_getenv("HALO3_LIVE_LOG");
    g_live.log_enabled = (log != 0 && log[0] != 0 && log[0] != '0');
    g_live.enabled = parse_origin(lan_host_getenv("HALO3_LIVE_SERVER"), g_live.server_host, &g_live.server_port,
        &g_live.use_tls, g_live.server_origin);
    g_live.relay_port = live_relay_port();

    const char *data = lan_host_getenv("HALO3_DATA");
    const unsigned int data_len = str_len(data);
    g_live.data_path_fits = !data || (data_len > 0 && data_len < sizeof(g_live.data_path));
    if (data_len != 0 && g_live.data_path_fits)
        __builtin_memcpy(g_live.data_path, data, data_len + 1);

    unsigned int live_players = 1;
    const char *lp = lan_host_getenv("HALO3_LOCAL_LIVE_PLAYERS");
    if (lp != 0 && lp[0] >= '1' && lp[0] <= '4' && lp[1] == 0)
        live_players = (unsigned int)(lp[0] - '0');
    static const char *const k_gt_env[4] = {
        "HALO3_GAMERTAG", "HALO3_GAMERTAG_2", "HALO3_GAMERTAG_3", "HALO3_GAMERTAG_4",
    };
    static const char *const k_acct_env[4] = {
        "HALO3_LIVE_ACCOUNT", "HALO3_LIVE_ACCOUNT_2", "HALO3_LIVE_ACCOUNT_3", "HALO3_LIVE_ACCOUNT_4",
    };
    for (unsigned int u = 0; u < 4; u++) {
        live_user *user = &g_live.users[u];
        const char *gt = lan_host_getenv(k_gt_env[u]);
        const char *acct = lan_host_getenv(k_acct_env[u]);
        sbuf path;
        if (acct && *acct) {
            sb_init(&path, user->legacy_path, sizeof(user->legacy_path));
            sb_str(&path, acct);
        } else if (u == 0) {
            account_path_begin(&path, user->legacy_path);
            sb_str(&path, "live_account.json");
            if (!g_live.data_path_fits)
                path.data[0] = 0;
        } else {
            sb_init(&path, user->legacy_path, sizeof(user->legacy_path));
            if (g_live.users[0].legacy_path[0]) {
                sb_str(&path, g_live.users[0].legacy_path);
                sb_str(&path, ".p");
                sb_u64(&path, u + 1);
            }
        }
        if (path.len == path.cap - 1)
            path.data[0] = 0;

        sbuf name;
        sb_init(&name, user->startup_name, sizeof(user->startup_name));
        sb_str(&name, gt);
        if (u < live_players || (gt && *gt) || (acct && *acct))
            user->auto_signin = LIVE_AUTO_CONFIGURED;
        if (!user->auto_signin && g_live.enabled && user->legacy_path[0]) {
            char saved[1024], token[LIVE_TOKEN_CAPACITY];
            if (live_host_read_file(user->legacy_path, saved, sizeof(saved)) > 0 && account_server_matches(saved) &&
                account_json_string(saved, "token", token, sizeof(token)) && token[0])
                user->auto_signin = LIVE_AUTO_LEGACY;
        }
        user->enabled = user->auto_signin != LIVE_AUTO_DISABLED;
    }
    __atomic_store_n(&g_live.configured, 1, __ATOMIC_RELEASE);
}

PORT_MS_ABI int port_live_enabled(void)
{
    if (!__atomic_load_n(&g_live.configured, __ATOMIC_ACQUIRE)) {
        live_host_lock();
        live_configure();
        live_host_unlock();
    }
    return g_live.enabled;
}

PORT_MS_ABI int port_live_signed_in(void) { return port_live_user_signed_in(0); }
PORT_MS_ABI u64 port_live_xuid(void) { return port_live_user_xuid(0); }
const char *port_live_gamertag(void) { return port_live_user_gamertag(0); }

int port_live_user_enabled(unsigned int user_index)
{
    return user_index < 4 && port_live_enabled() &&
           __atomic_load_n(&g_live.users[user_index].enabled, __ATOMIC_ACQUIRE);
}

int port_live_user_signed_in(unsigned int user_index)
{
    return user_index < 4 && port_live_enabled() &&
           __atomic_load_n(&g_live.users[user_index].signed_in, __ATOMIC_ACQUIRE);
}

int port_live_user_connecting(unsigned int user_index)
{
    return user_index < 4 && __atomic_load_n(&g_live.users[user_index].connecting, __ATOMIC_ACQUIRE);
}

u64 port_live_user_xuid(unsigned int user_index)
{
    return user_index < 4 ? __atomic_load_n(&g_live.users[user_index].xuid, __ATOMIC_ACQUIRE) : 0;
}

const char *port_live_user_gamertag(unsigned int user_index)
{
    port_live_enabled();
    return user_index < 4 ? g_live.users[user_index].gamertag : "";
}

int port_live_profile_identity(unsigned int user_index, const char *profile, u64 *xuid, char *gamertag,
    unsigned int capacity)
{
    if (user_index >= 4)
        return 0;
    live_host_lock();
    const live_user *user = &g_live.users[user_index];
    const int bound = user->signed_in && same_text_ci(user->profile, profile);
    if (bound) {
        if (xuid)
            *xuid = user->xuid;
        if (gamertag && capacity != 0) {
            sbuf s;
            sb_init(&s, gamertag, capacity);
            sb_str(&s, user->gamertag);
        }
    }
    live_host_unlock();
    return bound;
}

u32 port_live_user_generation(unsigned int user_index)
{
    if (user_index >= 4)
        return 0;
    live_host_lock();
    const u32 generation = g_live.users[user_index].generation;
    live_host_unlock();
    return generation;
}

int port_live_user_auth_result(unsigned int user_index, u32 generation, port_live_auth_result *out)
{
    if (user_index >= 4 || !out)
        return 0;
    live_host_lock();
    const live_user *user = &g_live.users[user_index];
    const int current = user->generation == generation;
    if (current)
        *out = user->auth;
    live_host_unlock();
    return current;
}

// ---- HTTP/1.1 client over live_host TCP, with persistent connections ----
//
// A request goes on a kept-alive connection to the server when one is idle (HTTP/1.1's default, RFC 9112 9.3), else on
// a new one: over the Internet a new connection costs a round trip (the TCP handshake) before the request's own. A
// connection is kept when its response was read whole by its length (Content-Length or chunked), the response is
// HTTP/1.1 and neither side said "Connection: close"; it is reused while it is younger than LIVE_HTTP_IDLE_REUSE_MS (well
// inside the servers' keep-alive timeouts: nginx's on slopboxlive.com is 20 s) and the server has not closed it (its FIN
// reads as readable). The request is the same bytes either way but for the "Connection: close" header it no longer sends.
//
// A request that a reused connection carried but that got no response byte back is sent once more on a new connection
// when that cannot apply it twice: the request was not sent at all, or it is a GET or DELETE (RFC 9110 9.2.2).

static int resolve_server_ip(void)
{
    if (__atomic_load_n(&g_live.server_ip, __ATOMIC_ACQUIRE) != 0)
        return 0;
    u32 ip = 0;
    if (parse_ipv4_str(g_live.server_host, &ip) || live_host_resolve(g_live.server_host, &ip) == 0) {
        __atomic_store_n(&g_live.server_ip, ip, __ATOMIC_RELEASE);
        return 0;
    }
    return -1;
}

static void connection_close(live_connection conn)
{
    if (conn.tls)
        live_host_tls_close(conn.tls);
    live_host_tcp_close(conn.fd);
}

static live_connection idle_take(void)
{
    const u64 now = lan_host_microseconds();
    for (;;) {
        live_host_lock();
        if (g_live.idle_count == 0) {
            live_host_unlock();
            return (live_connection){ LIVE_HOST_NO_SOCKET, 0 };
        }
        const live_idle_connection c = g_live.idle[--g_live.idle_count];
        live_host_unlock();
        int readable = 0, error = 0;
        if (now - c.since_us < LIVE_HTTP_IDLE_REUSE_MS * 1000ull
            && (!c.conn.tls || !live_host_tls_pending(c.conn.tls))
            && live_host_tcp_poll(c.conn.fd, &readable, 0, &error) == LIVE_HOST_OK && !readable && !error)
            return c.conn;
        connection_close(c.conn);
    }
}

// A connection whose exchange left it ready for another: kept (the oldest kept one makes room).
static void idle_put(live_connection conn)
{
    live_connection evicted = { LIVE_HOST_NO_SOCKET, 0 };
    live_host_lock();
    if (g_live.idle_count == LIVE_HTTP_IDLE_CAPACITY) {
        evicted = g_live.idle[0].conn;
        for (int i = 1; i < LIVE_HTTP_IDLE_CAPACITY; i++)
            g_live.idle[i - 1] = g_live.idle[i];
        g_live.idle_count--;
    }
    g_live.idle[g_live.idle_count].conn = conn;
    g_live.idle[g_live.idle_count].since_us = lan_host_microseconds();
    g_live.idle_count++;
    live_host_unlock();
    connection_close(evicted);
}

static void idle_close_all(void)
{
    live_host_lock();
    while (g_live.idle_count > 0)
        connection_close(g_live.idle[--g_live.idle_count].conn);
    live_host_unlock();
}

// A chunked body at p (end: what has come so far): 1 when its last chunk and trailer are in (*size: the body's bytes on
// the wire), 0 when more must come, -1 when it is not chunked encoding.
static int chunked_complete(const char *p, const char *end, unsigned int *size)
{
    const char *start = p;
    for (;;) {
        u32 length = 0;
        int digits = 0;
        while (p < end && hex_nibble(*p) >= 0) {
            if (length > 0x0fffffffu)
                return -1;
            length = (length << 4) | (u32)hex_nibble(*p++);
            digits++;
        }
        while (p < end && *p != '\n') // chunk extensions, the CR
            p++;
        if (p == end)
            return 0;
        if (digits == 0)
            return -1;
        p++;
        if (length == 0) {
            // trailer fields, then the empty line
            for (;;) {
                const char *line = p;
                while (p < end && *p != '\n')
                    p++;
                if (p == end)
                    return 0;
                p++;
                if (p - line <= 2) {
                    *size = (unsigned int)(p - start);
                    return 1;
                }
            }
        }
        if ((unsigned int)(end - p) < length + 2u)
            return 0;
        p += length;
        if (*p == '\r')
            p++;
        if (*p != '\n')
            return -1;
        p++;
    }
}

typedef struct {
    int status;            // the status code, or -1 when no whole response came
    unsigned int received; // bytes that came back
    unsigned int header;   // the header's size (the body follows)
    int chunked, reusable;
    int to_close;          // the body runs to the server's close (no length given)
    int unanswered;        // the server closed or reset the connection before a byte of a response came
} live_http_response;

// What has come of the response so far (raw, total bytes): whether it is whole, and the connection's fate.
static int response_complete(const char *method, const char *raw, unsigned int total, live_http_response *r)
{
    unsigned int header = 0;
    for (unsigned int i = 0; i + 3 < total; i++)
        if (raw[i] == '\r' && raw[i + 1] == '\n' && raw[i + 2] == '\r' && raw[i + 3] == '\n') {
            header = i + 4;
            break;
        }
    if (header == 0)
        return 0;
    // status line: HTTP/1.x NNN
    if (total < 12 || !str_eq_ci_n(raw, "HTTP/1.", 7) || raw[8] != ' ')
        return -1;
    const int http11 = raw[7] == '1';
    const char *sp = raw + 9;
    if (!(sp[0] >= '0' && sp[0] <= '9' && sp[1] >= '0' && sp[1] <= '9' && sp[2] >= '0' && sp[2] <= '9'))
        return -1;
    r->to_close = 0;
    r->status = (sp[0] - '0') * 100 + (sp[1] - '0') * 10 + (sp[2] - '0');
    r->header = header;
    // header fields (case-insensitive names, RFC 9110 5.1)
    int have_length = 0, close = !http11;
    u32 length = 0;
    r->chunked = 0;
    for (unsigned int i = 0; i < header;) {
        unsigned int eol = i;
        while (eol < header && raw[eol] != '\n')
            eol++;
        const char *line = raw + i;
        const unsigned int n = eol - i;
        if (n > 15 && str_eq_ci_n(line, "content-length:", 15)) {
            const char *v = line + 15;
            while (*v == ' ' || *v == '\t')
                v++;
            if (json_read_uint(v, &length))
                have_length = 1;
        } else if (n > 18 && str_eq_ci_n(line, "transfer-encoding:", 18)) {
            for (unsigned int k = 18; k + 7 <= n; k++)
                if (str_eq_ci_n(line + k, "chunked", 7))
                    r->chunked = 1;
        } else if (n > 11 && str_eq_ci_n(line, "connection:", 11)) {
            for (unsigned int k = 11; k + 5 <= n; k++)
                if (str_eq_ci_n(line + k, "close", 5))
                    close = 1;
        }
        i = eol + 1;
    }
    // a body: none for HEAD, 1xx, 204 and 304 (RFC 9112 6.3); then chunked, then Content-Length, else to the close
    unsigned int body = 0;
    if (method[0] == 'H' || (r->status >= 100 && r->status < 200) || r->status == 204 || r->status == 304) {
        body = 0;
    } else if (r->chunked) {
        const int done = chunked_complete(raw + header, raw + total, &body);
        if (done <= 0)
            return done;
    } else if (have_length) {
        if (total - header < length)
            return 0;
        body = length;
    } else {
        r->reusable = 0;
        r->to_close = 1;
        return 0; // whole once the server has closed
    }
    r->reusable = !close && header + body == total; // nothing past the response (no pipelining here)
    return 1;
}

// One exchange on the connection: the request out, the response into raw (raw_cap bytes). *sent_any: whether any byte of the
// request went out.
static void http_exchange(live_connection conn, const char *method, const char *head, unsigned int head_len,
    const char *body, unsigned int body_len, char *raw, unsigned int raw_cap, live_http_response *r, int *sent_any)
{
    r->status = -1;
    r->received = 0;
    r->header = 0;
    r->reusable = 0;
    r->to_close = 0;
    r->unanswered = 0;
    *sent_any = 0;
    const char *parts[2] = { head, body };
    const unsigned int sizes[2] = { head_len, body_len };
    for (int part = 0; part < 2; part++)
        for (unsigned int sent = 0; sent < sizes[part];) {
            const int size = (int)(sizes[part] - sent);
            const int n = conn.tls ? live_host_tls_send(conn.tls, parts[part] + sent, size)
                                   : live_host_tcp_send(conn.fd, parts[part] + sent, size);
            if (n <= 0) {
                // A TLS write can emit ciphertext before reporting an error. Do not replay an ambiguous POST.
                if (conn.tls)
                    *sent_any = 1;
                r->unanswered = !conn.tls;
                return;
            }
            *sent_any = 1;
            sent += (unsigned int)n;
        }
    unsigned int total = 0;
    int whole = 0, malformed = 0;
    while (total + 1 < raw_cap) {
        if (!conn.tls && live_host_tcp_wait_readable(conn.fd, LIVE_HTTP_TIMEOUT_MS) <= 0)
            break;
        const int size = (int)(raw_cap - 1 - total);
        const int n = conn.tls ? live_host_tls_recv(conn.tls, raw + total, size)
                               : live_host_tcp_recv(conn.fd, raw + total, size);
        if (n <= 0) {
            // the server closed: a response read to the close is whole
            if (n == 0 && total > 0 && response_complete(method, raw, total, r) == 0 && r->to_close)
                whole = 1;
            r->unanswered = !conn.tls && total == 0;
            break;
        }
        total += (unsigned int)n;
        const int done = response_complete(method, raw, total, r);
        if (done != 0) {
            whole = done > 0;
            malformed = done < 0;
            break;
        }
    }
    raw[total] = 0;
    r->received = total;
    if (!whole && !malformed && total + 1 >= raw_cap && r->header != 0) {
        // larger than raw: the body cut at raw's end (the callers' buffers are no larger), as before keep-alive; the
        // rest is left unread, so the connection is not kept
        whole = 1;
        r->reusable = 0;
    }
    if (!whole) {
        r->status = -1;
        r->reusable = 0;
    }
}

// The body of a whole response in raw (r->header on), decoded, into resp_body (resp_cap bytes, NUL-terminated).
static void response_body(const char *raw, const live_http_response *r, char *resp_body, unsigned int resp_cap)
{
    if (resp_body == 0 || resp_cap == 0)
        return;
    const char *p = raw + r->header;
    const char *end = raw + r->received;
    unsigned int out = 0;
    if (!r->chunked) {
        const unsigned int avail = (unsigned int)(end - p);
        out = avail < resp_cap - 1 ? avail : resp_cap - 1;
        __builtin_memcpy(resp_body, p, out);
    } else {
        while (p < end) {
            u32 length = 0;
            int any = 0;
            while (p < end && hex_nibble(*p) >= 0) {
                length = (length << 4) | (u32)hex_nibble(*p++);
                any = 1;
            }
            while (p < end && *p != '\n')
                p++;
            if (p < end)
                p++;
            if (!any || length == 0)
                break;
            if (length > (u32)(end - p))
                length = (u32)(end - p);
            const unsigned int copy = out + length >= resp_cap ? resp_cap - 1 - out : length;
            __builtin_memcpy(resp_body + out, p, copy);
            out += copy;
            p += length;
            if (p < end && *p == '\r')
                p++;
            if (p < end && *p == '\n')
                p++;
        }
    }
    resp_body[out] = 0;
}

static int http_connect(live_connection *conn)
{
    int status = live_host_tcp_open(1, &conn->fd);
    if (status == LIVE_HOST_OK)
        status = live_host_tcp_connect(conn->fd, g_live.server_ip, g_live.server_port);
    if (status == LIVE_HOST_IN_PROGRESS)
        status = live_host_tcp_wait_connect(conn->fd, LIVE_HTTP_TIMEOUT_MS);
    if (status != LIVE_HOST_OK) {
        if (g_live.use_tls)
            lan_host_log("live: HTTPS could not connect to the server");
    } else if (g_live.use_tls) {
        const char *ca = lan_host_getenv("HALO3_TLS_CA_FILE");
        if (!ca || !*ca)
            ca = lan_host_getenv("SSL_CERT_FILE");
        const char *error = live_host_tls_connect(conn->fd, g_live.server_host, ca && *ca ? ca : 0,
            LIVE_HTTP_TIMEOUT_MS, &conn->tls);
        if (!error)
            return 1;
        lan_host_log(error);
    } else if (live_host_tcp_set_nonblocking(conn->fd, 0) == LIVE_HOST_OK) {
        return 1;
    }
    connection_close(*conn);
    return 0;
}

// Performs an HTTP/1.1 request against g_live.server_host:server_port, with the account's bearer token `token`.
// Returns the HTTP status code (e.g. 200, 201, 204, 404), or -1 when no whole response came (no connection, no answer
// in LIVE_HTTP_TIMEOUT_MS). Never under the lock.
static int live_http_request_auth(const char *method, const char *path, const char *body, const char *token,
    char *resp_body, unsigned int resp_cap)
{
    if (resp_body != 0 && resp_cap > 0)
        resp_body[0] = 0;
    const u64 started = g_live.log_enabled ? lan_host_microseconds() : 0;
    if (resolve_server_ip() != 0) {
        if (started != 0)
            live_log2("http: cannot resolve ", g_live.server_host);
        return -1;
    }

    const unsigned int body_len = body != 0 ? str_len(body) : 0;
    char head[1024];
    sbuf h;
    sb_init(&h, head, sizeof(head));
    sb_str(&h, method);
    sb_char(&h, ' ');
    sb_str(&h, path);
    sb_str(&h, " HTTP/1.1\r\nHost: ");
    sb_str(&h, g_live.server_host);
    if (g_live.server_port != (g_live.use_tls ? 443 : 80)) {
        sb_char(&h, ':');
        sb_u64(&h, g_live.server_port);
    }
    sb_str(&h, "\r\nX-Halo3-Protocol-Version: ");
    sb_u64(&h, PORT_LIVE_PROTOCOL_VERSION);
    sb_str(&h, "\r\nAccept: application/json\r\n");
    if (token != 0 && token[0] != 0) {
        sb_str(&h, "Authorization: Bearer ");
        sb_str(&h, token);
        sb_str(&h, "\r\n");
    }
    if (body != 0) {
        sb_str(&h, "Content-Type: application/json\r\nContent-Length: ");
        sb_u64(&h, body_len);
        sb_str(&h, "\r\n");
    }
    sb_str(&h, "\r\n");

    char *raw = (char *)lan_host_pages_alloc(LIVE_RESP_BUF_SIZE);
    if (raw == 0)
        return -1;
    const int replayable = method[0] == 'G' || method[0] == 'D'; // GET, DELETE
    live_http_response r = { -1, 0, 0, 0, 0, 0, 0 };
    u64 connect_us = 0;
    int reused = 0, connected = 0;
    for (int attempt = 0; attempt < 2; attempt++) {
        live_connection conn = attempt == 0 ? idle_take() : (live_connection){ LIVE_HOST_NO_SOCKET, 0 };
        reused = conn.fd != LIVE_HOST_NO_SOCKET;
        if (!reused) {
            const u64 connect_start = lan_host_microseconds();
            if (!http_connect(&conn))
                break;
            connect_us = lan_host_microseconds() - connect_start;
        }
        connected = 1;
        int sent_any = 0;
        http_exchange(conn, method, head, h.len, body, body_len, raw, LIVE_RESP_BUF_SIZE, &r, &sent_any);
        if (r.status > 0 && r.reusable)
            idle_put(conn);
        else
            connection_close(conn);
        // a kept-alive connection the server had dropped as idle (it closed or reset it with no response, or the request
        // never went out): once more on a new one. A server closes an idle connection only between requests, so it did
        // not act on this one (and a failing server answers with an error status); browsers resend in the same case.
        // No answer in time is not that: only a request that cannot be applied twice is sent again then.
        if (r.status > 0 || !reused || r.received != 0 || (sent_any && !replayable && !r.unanswered))
            break;
    }
    if (r.status > 0)
        response_body(raw, &r, resp_body, resp_cap);
    lan_host_pages_free(raw, LIVE_RESP_BUF_SIZE);

    if (started != 0) {
        char line[320];
        sbuf s;
        sb_init(&s, line, sizeof(line));
        sb_str(&s, "live: http ");
        sb_str(&s, method);
        sb_char(&s, ' ');
        sb_str(&s, path);
        if (!connected) {
            sb_str(&s, " -> no connection after ");
            sb_ms(&s, lan_host_microseconds() - started);
            sb_str(&s, " ms");
        } else {
            sb_str(&s, " -> ");
            if (r.status > 0)
                sb_i64(&s, r.status);
            else
                sb_str(&s, "no response");
            sb_str(&s, " in ");
            sb_ms(&s, lan_host_microseconds() - started);
            if (reused) {
                sb_str(&s, " ms (kept-alive connection)");
            } else {
                sb_str(&s, " ms (new connection, connect ");
                sb_ms(&s, connect_us);
                sb_str(&s, " ms)");
            }
        }
        lan_host_log(line);
    }
    return r.status;
}

static int live_http_request(const char *method, const char *path, const char *body, char *resp_body,
    unsigned int resp_cap)
{
    char token[sizeof(g_live.users[0].token)];
    live_host_lock();
    __builtin_memcpy(token, g_live.users[0].token, sizeof(token));
    live_host_unlock();
    return live_http_request_auth(method, path, body, token, resp_body, resp_cap);
}

// ---- the Live worker ----
//
// The calls to the Live server that the title makes from its main loop (the XSession calls, its users' contexts and
// properties, the QoS listener: xlive_offline.cpp, xnet.c) must not hold the loop for a round trip to the server: an
// Xbox's XAM talks to Xbox Live in the background, and the title polls the XOVERLAPPED. They are queued here in the
// order they are made, and one thread makes them one after another, so the server sees them in that order, as when
// each was made by its caller. port_live_post copies the call's data and never waits (a job posted from a job runs
// after the jobs queued before it, and must not be waited for there); with no worker thread the caller makes the call
// itself, as before.

static void register_pending(int before_hosting);

static int on_live_worker(void)
{
    return g_live.worker_thread != 0 && g_live.worker_thread == live_host_thread_id();
}

PORT_MS_ABI int port_live_on_worker(void) { return on_live_worker(); }

static u8 *job_slot_storage(u32 slot) { return g_live.job_storage + (unsigned long)slot * PORT_LIVE_POST_MAX; }

static void live_worker(void *unused)
{
    (void)unused;
    live_host_lock();
    g_live.worker_thread = live_host_thread_id();
    for (;;) {
        while (g_live.job_count == 0)
            live_host_wait(LIVE_HOST_FOREVER);
        // the call at the head keeps its slot (its data) until it is done
        const live_job job = g_live.jobs[g_live.job_head];
        g_live.job_running = 1;
        live_host_unlock();
        register_pending(0);
        job.run(job.data);
        live_host_lock();
        g_live.job_head = (g_live.job_head + 1) % LIVE_JOB_CAPACITY;
        g_live.job_count--;
        live_job_node *node = g_live.overflow_head;
        if (node != 0) {
            // room in the ring: the oldest call of the list takes the slot at its end
            g_live.overflow_head = node->next;
            if (g_live.overflow_head == 0)
                g_live.overflow_tail = 0;
            const u32 slot = (g_live.job_head + g_live.job_count) % LIVE_JOB_CAPACITY;
            live_job *moved = &g_live.jobs[slot];
            *moved = node->job;
            moved->data = job_slot_storage(slot);
            if (moved->size != 0)
                __builtin_memcpy(moved->data, node->data, moved->size);
            g_live.job_count++;
        }
        g_live.job_running = 0;
        live_host_wake_all(); // live_drain; a post waiting for room
        if (node != 0) {
            live_host_unlock();
            lan_host_pages_free(node, node->bytes);
            live_host_lock();
        }
    }
}

PORT_MS_ABI int port_live_post(void (PORT_MS_ABI *run)(const void *data), const void *data, unsigned int size)
{
    live_job_node *spare = 0;
    live_host_lock();
    if (!g_live.worker_started && size <= PORT_LIVE_POST_MAX) {
        if (g_live.job_storage == 0)
            g_live.job_storage = (u8 *)lan_host_pages_alloc(LIVE_JOB_CAPACITY * PORT_LIVE_POST_MAX);
        if (g_live.job_storage != 0 && live_host_thread_start(live_worker, 0) == 0)
            g_live.worker_started = 1;
    }
    live_job *job = 0;
    while (g_live.worker_started && size <= PORT_LIVE_POST_MAX) {
        if (g_live.overflow_head == 0 && g_live.job_count < LIVE_JOB_CAPACITY) {
            const u32 slot = (g_live.job_head + g_live.job_count) % LIVE_JOB_CAPACITY;
            job = &g_live.jobs[slot];
            job->data = job_slot_storage(slot);
            g_live.job_count++;
            break;
        }
        if (spare == 0) {
            // the ring is full: the call waits in the list (its node allocated outside the lock, then the queue
            // looked at again: the worker may have made room meanwhile)
            live_host_unlock();
            const unsigned int bytes = (unsigned int)sizeof(live_job_node) + size;
            spare = (live_job_node *)lan_host_pages_alloc(bytes);
            if (spare != 0)
                spare->bytes = bytes;
            live_host_lock();
            if (spare != 0)
                continue;
        } else {
            spare->next = 0;
            if (g_live.overflow_tail != 0)
                g_live.overflow_tail->next = spare;
            else
                g_live.overflow_head = spare;
            g_live.overflow_tail = spare;
            job = &spare->job;
            job->data = (u8 *)spare->data;
            spare = 0;
            break;
        }
        // no memory for a node: wait for the worker to free a slot (it cannot wait for itself)
        if (on_live_worker())
            break;
        live_host_wait(LIVE_HOST_FOREVER);
    }
    if (job == 0) {
        // no worker thread (or the worker out of memory, or a call too large: never posted by the port): the call is
        // made here and now
        live_host_unlock();
        if (spare != 0)
            lan_host_pages_free(spare, spare->bytes);
        if (g_live.worker_started)
            live_log("live: a call could not be queued; made on the calling thread");
        run(data);
        return 1;
    }
    job->run = run;
    job->size = size;
    if (size != 0)
        __builtin_memcpy(job->data, data, size);
    live_host_wake_all();
    live_host_unlock();
    if (spare != 0)
        lan_host_pages_free(spare, spare->bytes);
    return 0;
}

// Waits up to timeout_ms for the queue to be done (XOnlineCleanup: the last session calls reach the server).
static void live_drain(unsigned int timeout_ms)
{
    const u64 until = lan_host_microseconds() + (u64)timeout_ms * 1000u;
    live_host_lock();
    while (g_live.job_count != 0 || g_live.job_running) {
        const u64 now = lan_host_microseconds();
        if (now >= until)
            break;
        live_host_wait((unsigned int)((until - now + 999) / 1000));
    }
    live_host_unlock();
}

// ---- account persistence & player heartbeat ----

typedef port_live_account live_account;

// Called under the Live lock so LAN sees ticket changes in account-generation order.
static void clear_relay_locked(void)
{
    __builtin_memset(g_live.relay_ticket, 0, sizeof(g_live.relay_ticket));
    port_lan_clear_relay();
}

static int read_account(const char *path, live_account *account)
{
    char json[1024], xuid[32];
    if (!path[0] || live_host_read_file(path, json, sizeof(json)) <= 0 || !account_server_matches(json))
        return 0;
    live_account saved = { 0 };
    if (!account_json_string(json, "gamertag", saved.gamertag, sizeof(saved.gamertag)) || !saved.gamertag[0] ||
        !account_json_string(json, "token", saved.token, sizeof(saved.token)) || !saved.token[0])
        return 0;
    if (account_json_string(json, "xuid", xuid, sizeof(xuid)) && str_len(xuid) == 16)
        parse_hex_u64(xuid, &saved.xuid);
    *account = saved;
    return 1;
}

static int save_account_file(const char *path, const live_account *account)
{
    char buf[1024];
    sbuf s;
    sb_init(&s, buf, sizeof(buf));
    sb_str(&s, "{\"xuid\":\"");
    sb_hex64(&s, account->xuid);
    sb_str(&s, "\",\"gamertag\":");
    sb_json_str(&s, account->gamertag);
    sb_str(&s, ",\"token\":");
    sb_json_str(&s, account->token);
    sb_str(&s, ",\"server\":");
    sb_json_str(&s, g_live.server_origin);
    sb_str(&s, "}\n");
    return live_host_write_file(path, buf, s.len);
}

static void auth_detail(port_live_auth_result *result, port_live_auth_status status, const char *detail)
{
    sbuf b;
    result->status = status;
    sb_init(&b, result->detail, sizeof(result->detail));
    sb_str(&b, detail);
}

static int contains_ci(const char *text, const char *word)
{
    const unsigned int n = str_len(word);
    for (unsigned int left = str_len(text); left >= n; left--, text++)
        if (str_eq_ci_n(text, word, n))
            return 1;
    return 0;
}

static void account_error(int status, const char *response, port_live_auth_result *result)
{
    char detail[sizeof(result->detail)] = { 0 }, error[sizeof(result->detail)] = { 0 };
    account_json_string(response, "error", error, sizeof(error));
    if (!account_json_string(response, "detail", detail, sizeof(detail))) {
        const char *errors = json_find_key(response, "errors");
        if (errors)
            account_json_string(errors, "detail", detail, sizeof(detail));
    }
    port_live_auth_status code = PORT_LIVE_AUTH_SERVER_ERROR;
    const char *fallback = "The Live server could not sign you in.";
    if (status == 409 || (status == 400 && (contains_ci(detail, "taken") || contains_ci(error, "taken")))) {
        code = PORT_LIVE_AUTH_GAMERTAG_TAKEN;
        fallback = "That gamertag is already taken on Xbox Live. Choose another name.";
    } else if (status == 400 &&
               (contains_ci(detail, "1-15") || contains_ci(error, "1-15") || contains_ci(detail, "invalid") ||
                contains_ci(error, "invalid") || contains_ci(detail, "required") || contains_ci(error, "required"))) {
        code = PORT_LIVE_AUTH_INVALID_GAMERTAG;
        fallback = "Enter a valid Xbox Live gamertag.";
    } else if (status == 401) {
        code = PORT_LIVE_AUTH_TOKEN_REJECTED;
        fallback = "The Live server rejected the account credentials.";
    } else if (status == 426) {
        code = PORT_LIVE_AUTH_UPGRADE_REQUIRED;
        fallback = "An update is required to play online. Restart the launcher to update.";
    } else if (status == 429) {
        code = PORT_LIVE_AUTH_RATE_LIMITED;
        fallback = "Rate limited. Wait a moment and try again.";
    } else if (status < 0) {
        code = PORT_LIVE_AUTH_UNREACHABLE;
        fallback = "Could not reach the Xbox Live server.";
    }
    auth_detail(result, code, detail[0] ? detail : error[0] ? error : fallback);
}

static int account_auth_ok(port_live_auth_status status)
{
    return status == PORT_LIVE_AUTH_OK || status == PORT_LIVE_AUTH_RENAMED_SUFFIX;
}

static int valid_account_token(const char *token)
{
    if (!*token)
        return 0;
    for (; *token; token++)
        if ((u8)*token < 0x21 || (u8)*token > 0x7e)
            return 0;
    return 1;
}

static void sign_in_account(const char *path, const char *profile, const char *requested, int scoped,
    live_account *account, port_live_auth_result *result)
{
    live_host_lock();
    const int ready = live_host_startup() == 0;
    live_host_unlock();
    if (!ready) {
        auth_detail(result, PORT_LIVE_AUTH_UNREACHABLE, "Could not initialize the Live network connection.");
        return;
    }
    if (!path[0]) {
        auth_detail(result, PORT_LIVE_AUTH_STORAGE_ERROR, "The Live account file path is invalid.");
        return;
    }
    const char *name = requested;
    int resumed = read_account(path, account);
    if (!resumed && scoped && same_text_ci(g_live.server_origin, LIVE_SECURE_ORIGIN)) {
        // Per-profile filenames also included the old HTTP origin in their hash.
        char old_path[LIVE_ACCOUNT_PATH_MAX];
        profile_account_path_for_origin(profile, LIVE_LEGACY_ORIGIN, old_path);
        resumed = read_account(old_path, account);
    }
    if (resumed) {
        if (!same_text_ci(requested, account->gamertag) && (!scoped || !same_text_ci(requested, profile))) {
            auth_detail(result, PORT_LIVE_AUTH_PROFILE_HAS_ACCOUNT,
                "This profile already has a saved Live account. Use its gamertag or choose another profile.");
            return;
        }
        // Resuming with the saved name prevents the account endpoint from treating sign-in as a rename.
        name = account->gamertag;
    } else {
        for (unsigned int u = 0; u < 4; u++) {
            live_account saved;
            if (read_account(g_live.users[u].legacy_path, &saved) && same_text_ci(saved.gamertag, requested)) {
                *account = saved;
                name = account->gamertag;
                break;
            }
        }
        // Older direct launches could leave their default account in the working directory.
        if (!account->token[0] && g_live.data_path[0] && legacy_service()) {
            live_account saved;
            if (read_account("live_account.json", &saved) && same_text_ci(saved.gamertag, requested)) {
                *account = saved;
                name = account->gamertag;
            }
        }
    }

    if (account->token[0] && !valid_account_token(account->token)) {
        auth_detail(result, PORT_LIVE_AUTH_TOKEN_REJECTED, "The saved Live token is invalid.");
        return;
    }
    if (str_len(name) >= sizeof(account->gamertag)) {
        auth_detail(result, PORT_LIVE_AUTH_INVALID_GAMERTAG, "Gamertags use 1 to 15 letters, digits or spaces.");
        return;
    }
    char body[256];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"gamertag\":");
    sb_json_str(&b, name);
    sb_str(&b, "}");

    // Send a bearer only for a matching origin. A rejected saved token must leave both account files intact.
    char resp[1024];
    const int status = live_http_request_auth("POST", "/xlive/v1/accounts", body, account->token, resp, sizeof(resp));
    if (status != 200 && status != 201) {
        account_error(status, resp, result);
        return;
    }

    live_account reply = *account; // a resume may omit the existing token
    char xuid[32];
    const char *token = json_find_key(resp, "token");
    if (!account_json_string(resp, "xuid", xuid, sizeof(xuid)) || str_len(xuid) != 16 ||
        !parse_hex_u64(xuid, &reply.xuid) || reply.xuid == 0 || (account->xuid && reply.xuid != account->xuid) ||
        !account_json_string(resp, "gamertag", reply.gamertag, sizeof(reply.gamertag)) || !reply.gamertag[0] ||
        (token && !account_json_string(resp, "token", reply.token, sizeof(reply.token))) || !valid_account_token(reply.token)) {
        auth_detail(result, PORT_LIVE_AUTH_SERVER_ERROR, "The Live server returned an invalid account.");
        return;
    }
    const int renamed = !same_text_ci(name, reply.gamertag);
    result->status = renamed ? PORT_LIVE_AUTH_RENAMED_SUFFIX : PORT_LIVE_AUTH_OK;
    sbuf detail;
    sb_init(&detail, result->detail, sizeof(result->detail));
    sb_str(&detail, renamed ? "Connected as " : "Connected to Xbox Live as ");
    sb_str(&detail, reply.gamertag);
    if (renamed) {
        sb_str(&detail, " (");
        sb_str(&detail, name);
        sb_str(&detail, " was taken)");
    }
    sb_char(&detail, '.');
    *account = reply;
}

static int snapshot_account_locked(unsigned int user_index, live_account *account)
{
    const live_user *user = &g_live.users[user_index];
    const int signed_in = user->signed_in;
    if (signed_in) {
        account->xuid = user->xuid;
        __builtin_memcpy(account->gamertag, user->gamertag, sizeof(account->gamertag));
        __builtin_memcpy(account->token, user->token, sizeof(account->token));
    }
    return signed_in;
}

static PORT_MS_ABI void relay_ticket_job(const void *data)
{
    const u32 generation = *(const u32 *)data;
    live_account account;
    live_host_lock();
    const live_user *user = &g_live.users[0];
    const int current = user->generation == generation && !g_live.stop_heartbeat && snapshot_account_locked(0, &account);
    live_host_unlock();
    if (!current)
        return;

    char response[1024], ticket[LIVE_RELAY_TICKET_CAPACITY] = { 0 };
    const int status = live_http_request_auth("POST", "/xlive/v1/relay/ticket", "{}", account.token,
        response, sizeof(response));
    u32 ttl = 0;
    const char *seconds = json_find_key(response, "ttlSeconds");
    const int valid = status == 200 && account_json_string(response, "ticket", ticket, sizeof(ticket)) &&
        ticket[0] && __builtin_strcmp(ticket, account.token) != 0 && seconds && json_read_uint(seconds, &ttl) && ttl > 0;

    live_host_lock();
    if (user->generation != generation || !user->signed_in || g_live.stop_heartbeat) {
        live_host_unlock();
        return;
    }
    if (valid) {
        const u64 delay = (u64)ttl * 3 / 4;
        const u64 seconds_to_refresh = delay < 60 ? 60 : delay > 3600 ? 3600 : delay;
        g_live.relay_refresh_us = lan_host_microseconds() + seconds_to_refresh * 1000000;
        __builtin_memcpy(g_live.relay_ticket, ticket, sizeof(ticket));
        port_lan_set_relay(__atomic_load_n(&g_live.server_ip, __ATOMIC_ACQUIRE), g_live.relay_port,
            account.xuid, g_live.relay_ticket);
    } else if (status == 404) {
        clear_relay_locked();
        // Keep refresh suspended until clear_live_user starts a new account generation.
        lan_host_log("live: the Live server has no relay ticket endpoint (404); relay disabled for this session");
    } else {
        if (g_live.relay_ticket[0])
            clear_relay_locked();
        g_live.relay_refresh_us = lan_host_microseconds() + LIVE_RELAY_RETRY_SECONDS * 1000000ull;
    }
    live_host_unlock();
}

static void refresh_relay_if_due(void)
{
    live_host_lock();
    const live_user *user = &g_live.users[0];
    const u32 generation = user->generation;
    const int due = user->signed_in && !g_live.stop_heartbeat && g_live.relay_port && g_live.use_tls &&
        lan_host_microseconds() >= g_live.relay_refresh_us;
    if (due)
        g_live.relay_refresh_us = ~0ull;
    live_host_unlock();
    if (due)
        port_live_post(relay_ticket_job, &generation, sizeof(generation));
}

static int register_player_for(u64 xuid, const char *gamertag, const char *token, int new_instance)
{
    u8 xnaddr[PORT_LAN_XNADDR_SIZE];
    __builtin_memset(xnaddr, 0, sizeof(xnaddr));
    port_lan_title_xnaddr(xnaddr);

    u32 ip = live_xnaddr_ip(xnaddr);
    u16 port = load_u16_le(xnaddr + 8);
    if (port == 0)
        port = 3074;
    const u8 *mac = xnaddr + 10;
    u64 machine_id = 0xfa00000000000000ull;
    for (int i = 0; i < 6; i++)
        machine_id |= ((u64)mac[i]) << ((5 - i) * 8);

    char body[512];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"xuid\":\"");
    sb_hex64(&b, xuid);
    sb_str(&b, "\",\"gamertag\":");
    sb_json_str(&b, gamertag);
    sb_str(&b, ",\"machineId\":\"");
    sb_hex64(&b, machine_id);
    sb_str(&b, "\",\"macAddress\":\"");
    sb_hex_bytes(&b, mac, 6);
    sb_str(&b, "\",\"hostAddress\":\"");
    sb_ipv4(&b, ip);
    sb_str(&b, "\",\"port\":");
    sb_u64(&b, port);
    if (new_instance)
        sb_str(&b, ",\"newInstance\":true");
    sb_str(&b, "}");

    char resp[512];
    int status = live_http_request_auth("POST", "/xlive/v1/players", body, token, resp, sizeof(resp));
    return (status >= 200 && status < 300) ? 0 : -1;
}

static int register_player(unsigned int user_index, int new_instance)
{
    live_account account;
    live_host_lock();
    const int signed_in = snapshot_account_locked(user_index, &account);
    live_host_unlock();
    if (!signed_in)
        return 0;
    return register_player_for(account.xuid, account.gamertag, account.token, new_instance);
}

// Register/heartbeat any signed-in local Live guests (controllers 1..3) with POST /xlive/v1/players (no bearer token,
// since guest XUIDs are sponsor_xuid | (guest_number << 54) and not standalone accounts). Without this, the Live server
// groups guests under phantom machine_id 0000000000000000 during XSessionArbitrationRegister, kicking everyone to lobby.
static void register_local_guests(int new_instance)
{
    for (unsigned long u = 0; u < PORT_PROFILE_USERS; u++) {
        port_profile_user_info info;
        if (!port_profile_user(u, &info) || info.kind != PORT_SIGNIN_GUEST)
            continue;
        port_profile_user_info sponsor;
        u64 sponsor_xuid;
        char sponsor_name[PORT_PROFILE_GAMERTAG_MAX + 1];
        if (!port_profile_user(info.sponsor, &sponsor) || sponsor.kind != PORT_SIGNIN_PROFILE ||
            sponsor.profile != info.profile ||
            !port_live_profile_identity((unsigned int)info.sponsor, sponsor.name, &sponsor_xuid,
                sponsor_name, sizeof(sponsor_name)))
            continue;
        u64 guest_xuid = sponsor_xuid | ((u64)info.guest_number << 54);
        char guest_name[PORT_PROFILE_GAMERTAG_MAX + 1];
        port_profile_guest_name(sponsor_name, info.guest_number, guest_name);
        if (register_player_for(guest_xuid, guest_name, 0, new_instance) == 0 && new_instance)
            live_log2("registered Live guest ", guest_name);
    }
}

static int poll_invites(void);
static void qos_listen_retry(void);

// The heartbeat thread: the player's heartbeat (register_player) every LIVE_HEARTBEAT_MS, and the game invites poll
// every LIVE_INVITE_POLL_MS (twice as long after each failure to reach the server, up to LIVE_INVITE_POLL_MAX_MS; never
// again once the server has said it keeps no invites).
static void heartbeat_thread(void *arg)
{
    (void)arg;
    unsigned int since_heartbeat = 0, since_poll = 0, poll_every = LIVE_INVITE_POLL_MS;
    for (;;) {
        live_host_lock();
        int any_signed_in = 0;
        for (unsigned int u = 0; u < 4; u++)
            any_signed_in |= g_live.users[u].signed_in;
        if (g_live.stop_heartbeat || !any_signed_in) {
            g_live.heartbeat_started = 0;
            live_host_unlock();
            return;
        }
        live_host_unlock();
        live_host_sleep_ms(500u);
        live_host_lock();
        const int stopping = g_live.stop_heartbeat;
        live_host_unlock();
        if (stopping)
            continue;
        since_heartbeat += 500u;
        since_poll += 500u;
        const int u0 = port_live_signed_in();
        if (u0)
            refresh_relay_if_due();
        if (since_heartbeat >= LIVE_HEARTBEAT_MS) {
            since_heartbeat = 0;
            if (u0)
                qos_listen_retry();
            for (unsigned int u = 0; u < 4; u++)
                register_player(u, 0);
            register_local_guests(0);
        }
        if (u0 && since_poll >= poll_every && port_live_invites_supported()) {
            since_poll = 0;
            const int result = poll_invites();
            poll_every = result != -1 ? LIVE_INVITE_POLL_MS
                         : poll_every >= LIVE_INVITE_POLL_MAX_MS / 2 ? LIVE_INVITE_POLL_MAX_MS : poll_every * 2;
        }
    }
}

// How long to wait after `failures` failed attempts in a row: LIVE_SIGNIN_RETRY_MS, twice as long after each further
// failure, up to LIVE_SIGNIN_RETRY_MAX_MS.
static u32 retry_delay_ms(u32 failures)
{
    u32 delay = LIVE_SIGNIN_RETRY_MS;
    for (u32 i = 1; i < failures && delay < LIVE_SIGNIN_RETRY_MAX_MS; i++)
        delay *= 2;
    return delay > LIVE_SIGNIN_RETRY_MAX_MS ? LIVE_SIGNIN_RETRY_MAX_MS : delay;
}

// The registrations of the users signed in as online players, the heartbeat's first (newInstance: the server drops
// what an earlier run of this console left, the sessions it hosted among it), on the worker: signing in does not wait
// for them. They come before the calls to the server made after the sign-in, as when signing in made them: the worker
// makes them before its next job, and before a call that makes this console a session's host (a session job may have
// started before a sign-in on another thread ended). Only the worker makes them. Bits 0..3: users 0..3;
// LIVE_REGISTER_GUESTS: the local guests of the users signed in (register_local_guests). One that fails is tried again
// before a later job, after the sign-in's back-off; before_hosting (the console is about to host a session): at once,
// and if it fails again it is given up, since a newInstance made after this console hosts a session would drop that
// session (the server keeps what an earlier run left, as when the registration failed before).
#define LIVE_REGISTER_GUESTS (1u << 4)

static void register_pending(int before_hosting)
{
    for (;;) {
        live_host_lock();
        u32 bit = 0;
        if (before_hosting || g_live.register_failures == 0 || lan_host_microseconds() >= g_live.register_retry_us) {
            while (bit <= 4 && (g_live.register_pending & (1u << bit)) == 0)
                bit++;
        } else {
            bit = 5; // not yet
        }
        const u32 generation = bit < 4 ? g_live.users[bit].generation : 0;
        live_account account;
        const int signed_in = bit < 4 && snapshot_account_locked(bit, &account);
        if (bit <= 4)
            g_live.register_pending &= ~(1u << bit);
        live_host_unlock();
        if (bit > 4)
            return;
        int failed = 0;
        if (signed_in)
            failed = register_player_for(account.xuid, account.gamertag, account.token, 1) != 0;
        else if (bit == 4)
            register_local_guests(1);
        live_host_lock();
        if (!failed) {
            g_live.register_failures = 0;
        } else if (!before_hosting && bit < 4 && g_live.users[bit].generation == generation) {
            g_live.register_pending |= 1u << bit;
            g_live.register_failures++;
            g_live.register_retry_us = lan_host_microseconds() + (u64)retry_delay_ms(g_live.register_failures) * 1000u;
        }
        live_host_unlock();
        if (failed) {
            live_log(before_hosting ? "live: registration as a new instance failed before hosting a session; given up"
                                    : "live: registration as a new instance failed; tried again later");
            if (!before_hosting)
                return;
        }
    }
}

static PORT_MS_ABI void register_job(const void *unused)
{
    (void)unused;
    register_pending(0);
}

// The registrations pending are made: at once on the worker, else by it, before its next job.
static void register_soon(void)
{
    if (on_live_worker()) {
        register_pending(0);
    } else {
        const u32 unused = 0;
        port_live_post(register_job, &unused, sizeof(unused));
    }
}

static PORT_MS_ABI void title_servers_refresh_job(const void *data);

static void publish_account_locked(unsigned int user_index, const char *profile, const live_account *account)
{
    live_user *user = &g_live.users[user_index];
    __atomic_store_n(&user->xuid, account->xuid, __ATOMIC_RELEASE);
    __builtin_memcpy(user->profile, profile, str_len(profile) + 1);
    __builtin_memcpy(user->gamertag, account->gamertag, sizeof(user->gamertag));
    __builtin_memcpy(user->token, account->token, sizeof(user->token));
    __atomic_store_n(&user->enabled, 1, __ATOMIC_RELEASE);
    if (user_index == 0) {
        g_live.started = 1;
        g_live.signin_failures = 0;
    }
    __atomic_store_n(&user->signed_in, 1, __ATOMIC_RELEASE);
    g_live.register_pending |= (1u << user_index) | LIVE_REGISTER_GUESTS;
    g_live.register_failures = 0;
    g_live.stop_heartbeat = 0;
    if (!g_live.heartbeat_started)
        g_live.heartbeat_started = live_host_thread_start(heartbeat_thread, 0) == 0;
}

static void save_authenticated_account(const char *path, const live_account *account, port_live_auth_result *result)
{
    if (account_auth_ok(result->status) && save_account_file(path, account) < 0)
        auth_detail(result, PORT_LIVE_AUTH_STORAGE_ERROR, "Could not save the Live account in the data directory.");
}

int port_live_prepare_profile(const char *profile, port_live_account *account, port_live_auth_result *result)
{
    char path[LIVE_ACCOUNT_PATH_MAX];
    profile_account_path(profile, path);
    __builtin_memset(account, 0, sizeof(*account));
    sign_in_account(path, profile, profile, 1, account, result);
    save_authenticated_account(path, account, result);
    return account_auth_ok(result->status);
}

// Commits credentials and publishes the identity only while this is still the active attempt.
static int authenticate_user(unsigned int user_index, u32 generation, const char *profile, const char *requested,
    const char *account_path)
{
    live_user *user = &g_live.users[user_index];
    live_host_lock();
    if (user->generation != generation) {
        live_host_unlock();
        return -1;
    }
    live_host_unlock();

    char path[LIVE_ACCOUNT_PATH_MAX];
    if (account_path) {
        sbuf b;
        sb_init(&b, path, sizeof(path));
        sb_str(&b, account_path);
        if (b.len == b.cap - 1)
            path[0] = 0;
    } else {
        profile_account_path(profile, path);
    }
    live_account account = { 0 };
    port_live_auth_result result = { 0 };
    sign_in_account(path, profile, requested, !account_path, &account, &result);

    live_host_lock();
    if (user->generation != generation) {
        live_host_unlock();
        return -1;
    }
    save_authenticated_account(path, &account, &result);
    user->auth = result;
    __atomic_store_n(&user->connecting, 0, __ATOMIC_RELEASE);
    const int ok = account_auth_ok(result.status);
    if (ok)
        publish_account_locked(user_index, profile, &account);
    live_host_unlock();
    if (ok)
        live_log2("signed in as ", account.gamertag);
    return ok ? 0 : -1;
}

void port_live_finish_signin(unsigned int user_index)
{
    register_soon();
    if (user_index == 0) {
        refresh_relay_if_due();
        live_host_lock();
        const int prefetch = g_live.users[0].signed_in && !g_live.title_servers_refreshing && g_live.title_server_count == 0;
        if (prefetch)
            g_live.title_servers_refreshing = 1;
        live_host_unlock();
        if (prefetch) {
            const u32 title = 0x4D5307E6u;
            port_live_post(title_servers_refresh_job, &title, sizeof(title));
        }
    }
}

int port_live_activate_profile(unsigned int user_index, u32 generation, const char *profile,
    const port_live_account *account, const port_live_auth_result *result)
{
    live_host_lock();
    const int current = user_index < 4 && g_live.users[user_index].generation == generation;
    if (current) {
        g_live.users[user_index].auth = *result;
        publish_account_locked(user_index, profile, account);
    }
    live_host_unlock();
    return current;
}

static PORT_MS_ABI void signin_job(const void *unused)
{
    (void)unused;
    live_host_lock();
    g_live.signin_queued = 0;
    live_host_unlock();
    port_live_startup();
}

// XOnlineStartup and XAM on demand sign in the configured startup profiles. Only transient account failures retry
// automatically. A profile switch or explicit sign-out disables automatic sign-in for that controller.
PORT_MS_ABI int port_live_startup(void)
{
    if (!port_live_enabled())
        return 0;
    live_host_lock();
    while (g_live.signin_running)
        live_host_wait(LIVE_HOST_FOREVER);
    live_user *primary = &g_live.users[0];
    if (g_live.started) {
        live_host_unlock();
        return 0;
    }
    if (!primary->auto_signin || primary->connecting ||
        (g_live.signin_failures != 0 && lan_host_microseconds() < g_live.signin_retry_us)) {
        live_host_unlock();
        return -1;
    }
    if (g_live.signin_failures != 0 && !on_live_worker()) {
        const int due = !g_live.signin_queued;
        if (due)
            g_live.signin_queued = 1;
        live_host_unlock();
        if (due) {
            const u32 unused = 0;
            port_live_post(signin_job, &unused, sizeof(unused));
        }
        return -1;
    }
    g_live.signin_running = 1;
    u32 generation[4];
    for (unsigned int u = 0; u < 4; u++)
        generation[u] = g_live.users[u].generation;
    live_host_unlock();

    int primary_ok = 0, any_ok = 0;
    for (unsigned int u = 0; u < 4; u++) {
        port_profile_user_info profile;
        if (!port_profile_user(u, &profile) || profile.kind != PORT_SIGNIN_PROFILE)
            continue;
        live_host_lock();
        live_user *user = &g_live.users[u];
        const int start = user->generation == generation[u] && user->auto_signin && !user->connecting && !user->signed_in;
        if (start)
            __atomic_store_n(&user->connecting, 1, __ATOMIC_RELEASE);
        const int legacy_only = user->auto_signin == LIVE_AUTO_LEGACY;
        live_host_unlock();
        if (!start)
            continue;
        const char *name = user->startup_name[0] ? user->startup_name : profile.name;
        if (legacy_only) {
            live_account saved;
            if (!read_account(user->legacy_path, &saved) || !same_text_ci(saved.gamertag, name)) {
                live_host_lock();
                if (user->generation == generation[u])
                    __atomic_store_n(&user->connecting, 0, __ATOMIC_RELEASE);
                live_host_unlock();
                continue;
            }
        }
        const int ok = authenticate_user(u, generation[u], profile.name, name, 0) == 0;
        any_ok |= ok;
        if (u == 0) {
            primary_ok = ok;
            if (!ok)
                break;
        }
    }

    live_host_lock();
    if (!primary_ok && primary->generation == generation[0] && primary->auth.status != PORT_LIVE_AUTH_NONE) {
        g_live.signin_failures++;
        const port_live_auth_status status = primary->auth.status;
        const int retry = status == PORT_LIVE_AUTH_UNREACHABLE || status == PORT_LIVE_AUTH_RATE_LIMITED ||
                          status == PORT_LIVE_AUTH_SERVER_ERROR;
        g_live.signin_retry_us = retry
            ? lan_host_microseconds() + (u64)retry_delay_ms(g_live.signin_failures) * 1000u : ~0ull;
    }
    g_live.signin_running = 0;
    live_host_wake_all();
    live_host_unlock();
    if (any_ok)
        port_live_finish_signin(0);
    return primary_ok ? 0 : -1;
}

static void clear_live_user(live_user *user)
{
    user->generation++;
    if (user == &g_live.users[0]) {
        clear_relay_locked();
        g_live.relay_refresh_us = 0;
    }
    __atomic_store_n(&user->signed_in, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&user->enabled, user->auto_signin != LIVE_AUTO_DISABLED, __ATOMIC_RELEASE);
    __atomic_store_n(&user->connecting, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&user->xuid, 0, __ATOMIC_RELEASE);
    __builtin_memset(user->token, 0, sizeof(user->token));
    __builtin_memset(user->profile, 0, sizeof(user->profile));
    __builtin_memset(user->gamertag, 0, sizeof(user->gamertag));
    __builtin_memset(&user->auth, 0, sizeof(user->auth));
}

void port_live_sign_out(unsigned long user_index)
{
    if (user_index >= 4)
        return;
    port_live_enabled();
    live_host_lock();
    live_user *user = &g_live.users[user_index];
    user->auto_signin = LIVE_AUTO_DISABLED;
    clear_live_user(user);
    g_live.register_pending &= ~(1u << user_index);
    if (user_index == 0) {
        g_live.started = 0;
        g_live.signin_failures = 0;
        __builtin_memset(g_live.hosted, 0, sizeof(g_live.hosted));
    }
    live_host_wake_all();
    live_host_unlock();
}

static int begin_connect(unsigned int user_index, u32 *generation)
{
    if (user_index >= 4 || !port_live_enabled())
        return 0;
    live_host_lock();
    live_user *user = &g_live.users[user_index];
    const int start = *generation == user->generation && !user->connecting && !user->signed_in;
    if (start) {
        *generation = ++user->generation;
        user->auto_signin = LIVE_AUTO_DISABLED;
        __builtin_memset(&user->auth, 0, sizeof(user->auth));
        __atomic_store_n(&user->connecting, 1, __ATOMIC_RELEASE);
    }
    live_host_unlock();
    return start;
}

int port_live_connect_user(unsigned int user_index, const char *gamertag, const char *account_path)
{
    const u32 expected = port_live_user_generation(user_index);
    port_profile_user_info profile;
    if (user_index >= 4 || !port_profile_user(user_index, &profile) || profile.kind != PORT_SIGNIN_PROFILE)
        return -1;
    u32 generation = expected;
    if (!begin_connect(user_index, &generation))
        return -1;
    const char *name = gamertag && *gamertag ? gamertag : profile.name;
    const int result = authenticate_user(user_index, generation, profile.name, name,
        account_path && *account_path ? account_path : 0);
    if (result == 0)
        port_live_finish_signin(user_index);
    return result;
}

typedef struct {
    unsigned int user_index;
    u32 generation;
    char profile[PORT_PROFILE_GAMERTAG_MAX + 1];
    char gamertag[32];
    void (*done)(unsigned int user_index, u32 generation, void *context);
    void *context;
} live_connect_job;

static PORT_MS_ABI void live_connect_job_run(const void *data)
{
    const live_connect_job *job = (const live_connect_job *)data;
    const int ok = authenticate_user(job->user_index, job->generation, job->profile, job->gamertag, 0) == 0;
    if (job->done != 0 && job->generation == port_live_user_generation(job->user_index))
        job->done(job->user_index, job->generation, job->context);
    if (ok)
        port_live_finish_signin(job->user_index);
}

int port_live_connect_user_async(unsigned int user_index, const char *profile, const char *gamertag,
    u32 generation, void (*done)(unsigned int user_index, u32 generation, void *context), void *context)
{
    live_connect_job job = { 0 };
    if (!profile || !*profile || str_len(profile) >= sizeof(job.profile) ||
        str_len(gamertag) >= sizeof(job.gamertag) || !begin_connect(user_index, &generation))
        return -1;
    job.user_index = user_index;
    job.generation = generation;
    sbuf s;
    sb_init(&s, job.profile, sizeof(job.profile));
    sb_str(&s, profile);
    sb_init(&s, job.gamertag, sizeof(job.gamertag));
    sb_str(&s, gamertag && *gamertag ? gamertag : profile);
    job.done = done;
    job.context = context;
    port_live_post(live_connect_job_run, &job, (unsigned int)sizeof(job));
    return 0;
}

// XOnlineCleanup: the queued calls are given LIVE_DRAIN_MS to reach the server (the last session's end and delete), the
// kept-alive connections are closed.
PORT_MS_ABI void port_live_cleanup(void)
{
    live_host_lock();
    g_live.stop_heartbeat = 1;
    live_host_unlock();
    live_drain(LIVE_DRAIN_MS);
    idle_close_all();
    live_host_lock();
    for (int i = 0; i < LIVE_MAX_TCP_SOCKETS; i++) {
        if (g_live.tcp[i].used) {
            live_host_tcp_close(g_live.tcp[i].fd);
            g_live.tcp[i].used = 0;
        }
    }
    for (unsigned int u = 0; u < 4; u++)
        clear_live_user(&g_live.users[u]);
    g_live.stop_heartbeat = 1;
    g_live.register_pending = 0;
    g_live.started = 0;
    g_live.signin_failures = 0;
    live_host_unlock();
}

// ---- user contexts & properties ----

static void append_props_and_ctxs_json(sbuf *b, const port_live_property *props, u32 prop_count,
    const port_live_context *ctxs, u32 ctx_count)
{
    sb_str(b, "\"properties\":[");
    for (u32 i = 0; i < prop_count; i++) {
        if (i > 0)
            sb_char(b, ',');
        sb_str(b, "{\"id\":\"");
        sb_hex32(b, props[i].id);
        sb_str(b, "\",\"type\":");
        switch (props[i].type) {
            case PORT_LIVE_PROP_INT32:
                sb_str(b, "\"int32\",\"value\":");
                sb_i64(b, (int)props[i].int_val);
                break;
            case PORT_LIVE_PROP_INT64:
                sb_str(b, "\"int64\",\"value\":");
                sb_i64(b, props[i].int_val);
                break;
            case PORT_LIVE_PROP_DOUBLE:
                sb_str(b, "\"double\",\"value\":");
                sb_double(b, props[i].double_val);
                break;
            case PORT_LIVE_PROP_FLOAT:
                sb_str(b, "\"float\",\"value\":");
                sb_double(b, props[i].double_val);
                break;
            default:
                sb_str(b, "\"int32\",\"value\":");
                sb_i64(b, props[i].int_val);
                break;
        }
        sb_char(b, '}');
    }
    sb_str(b, "],\"contexts\":[");
    for (u32 i = 0; i < ctx_count; i++) {
        if (i > 0)
            sb_char(b, ',');
        sb_str(b, "{\"id\":\"");
        sb_hex32(b, ctxs[i].id);
        sb_str(b, "\",\"value\":");
        sb_u64(b, ctxs[i].value);
        sb_char(b, '}');
    }
    sb_char(b, ']');
}

static void push_user_props_to_hosted_sessions(u32 user_index)
{
    if (!port_live_signed_in() || user_index >= 4)
        return;
    u8 kids[LIVE_MAX_HOSTED_SESSIONS][8];
    int nkids = 0;
    port_live_property props[LIVE_MAX_USER_PROPS];
    port_live_context ctxs[LIVE_MAX_USER_CTXS];
    u32 pcount = 0, ccount = 0;

    live_host_lock();
    for (int i = 0; i < LIVE_MAX_HOSTED_SESSIONS; i++) {
        if (g_live.hosted[i].used)
            __builtin_memcpy(kids[nkids++], g_live.hosted[i].xnkid, 8);
    }
    pcount = g_live.user_prop_count[user_index];
    ccount = g_live.user_ctx_count[user_index];
    __builtin_memcpy(props, g_live.user_props[user_index], pcount * sizeof(props[0]));
    __builtin_memcpy(ctxs, g_live.user_ctxs[user_index], ccount * sizeof(ctxs[0]));
    live_host_unlock();

    if (nkids == 0)
        return;

    char body[4096];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_char(&b, '{');
    append_props_and_ctxs_json(&b, props, pcount, ctxs, ccount);
    sb_char(&b, '}');

    for (int i = 0; i < nkids; i++) {
        char path[128];
        sbuf p;
        sb_init(&p, path, sizeof(path));
        sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
        sb_hex_bytes(&p, kids[i], 8);
        sb_str(&p, "/properties");
        live_http_request("POST", path, body, 0, 0);
    }
}

static PORT_MS_ABI void props_push_job(const void *data)
{
    const u32 user_index = *(const u32 *)data;
    __atomic_store_n(&g_live.props_push_queued[user_index], 0, __ATOMIC_RELEASE); // a change from now on queues another
    push_user_props_to_hosted_sessions(user_index);
}

// A user's contexts or properties changed (a value, or a new id): the sessions hosted are sent the whole table, on the
// worker, after the session calls queued before (a session being created is hosted by then). A push queued and not
// started yet carries the change too: the server merges the table by id (later values win), so one push of the
// latest table leaves it as one push per change did. Setting a value a context or property already has sends nothing.
static void user_props_changed(u32 user_index)
{
    if (!port_live_signed_in())
        return;
    if (__atomic_exchange_n(&g_live.props_push_queued[user_index], 1, __ATOMIC_ACQ_REL))
        return;
    port_live_post(props_push_job, &user_index, sizeof(user_index));
}

void port_live_set_context(u32 user_index, u32 id, u32 value)
{
    if (user_index >= 4)
        return;
    live_host_lock();
    u32 n = g_live.user_ctx_count[user_index];
    int found = 0;
    int changed = 0;
    for (u32 i = 0; i < n; i++) {
        if (g_live.user_ctxs[user_index][i].id == id) {
            changed = g_live.user_ctxs[user_index][i].value != value;
            g_live.user_ctxs[user_index][i].value = value;
            found = 1;
            break;
        }
    }
    if (!found && n < LIVE_MAX_USER_CTXS) {
        g_live.user_ctxs[user_index][n].id = id;
        g_live.user_ctxs[user_index][n].value = value;
        g_live.user_ctx_count[user_index] = n + 1;
        changed = 1;
    }
    live_host_unlock();
    if (changed)
        user_props_changed(user_index);
}

static void set_user_prop(u32 user_index, port_live_property prop)
{
    if (user_index >= 4)
        return;
    live_host_lock();
    u32 n = g_live.user_prop_count[user_index];
    int found = 0;
    int changed = 0;
    for (u32 i = 0; i < n; i++) {
        if (g_live.user_props[user_index][i].id == prop.id) {
            const port_live_property *old = &g_live.user_props[user_index][i];
            changed = old->type != prop.type || old->int_val != prop.int_val
                || __builtin_memcmp(&old->double_val, &prop.double_val, sizeof(prop.double_val)) != 0;
            g_live.user_props[user_index][i] = prop;
            found = 1;
            break;
        }
    }
    if (!found && n < LIVE_MAX_USER_PROPS) {
        g_live.user_props[user_index][n] = prop;
        g_live.user_prop_count[user_index] = n + 1;
        changed = 1;
    }
    live_host_unlock();
    if (changed)
        user_props_changed(user_index);
}

void port_live_set_property_int32(u32 u, u32 id, int v)
{
    port_live_property p = { id, PORT_LIVE_PROP_INT32, v, 0.0 };
    set_user_prop(u, p);
}
void port_live_set_property_int64(u32 u, u32 id, long long v)
{
    port_live_property p = { id, PORT_LIVE_PROP_INT64, v, 0.0 };
    set_user_prop(u, p);
}
void port_live_set_property_double(u32 u, u32 id, double v)
{
    port_live_property p = { id, PORT_LIVE_PROP_DOUBLE, 0, v };
    set_user_prop(u, p);
}
void port_live_set_property_float(u32 u, u32 id, float v)
{
    port_live_property p = { id, PORT_LIVE_PROP_FLOAT, 0, (double)v };
    set_user_prop(u, p);
}

// ---- LSP title servers & TCP stream sockets ----

// The title servers the Live server lists (GET /xlive/v1/titles/<title>/servers), into out: 0, or -1 when the list cannot
// be had. Each is remembered for XNetServerToInAddr.
static int fetch_title_servers(u32 title_id, port_live_title_server *out, int capacity, int *count_out)
{
    *count_out = 0;
    char path[64];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/");
    sb_hex32(&p, title_id);
    sb_str(&p, "/servers");

    char resp[4096];
    int status = live_http_request("GET", path, 0, resp, sizeof(resp));
    if (status != 200)
        return -1;

    const char *arr = json_find_key(resp, "servers");
    if (!arr)
        return -1;
    arr = skip_ws(arr);
    if (*arr != '[')
        return -1;
    arr = skip_ws(arr + 1);

    int count = 0;
    while (*arr && *arr != ']' && count < capacity) {
        if (*arr != '{')
            break;
        port_live_title_server srv;
        __builtin_memset(&srv, 0, sizeof(srv));
        srv.port = g_live.server_port;

        char addr_str[64] = { 0 };
        const char *av = json_find_key(arr, "address");
        if (av && json_read_string(av, addr_str, sizeof(addr_str)))
            parse_ipv4_str(addr_str, &srv.address);
        if (srv.address == 0)
            srv.address = g_live.server_ip;

        u32 port_u32 = 0;
        const char *pv = json_find_key(arr, "port");
        if (pv && json_read_uint(pv, &port_u32) && port_u32 > 0 && port_u32 <= 65535)
            srv.port = (u16)port_u32;

        const char *fv = json_find_key(arr, "flags");
        if (fv)
            json_read_uint(fv, &srv.flags);

        const char *dv = json_find_key(arr, "description");
        if (dv)
            json_read_string(dv, srv.description, sizeof(srv.description));

        // Remember real port for XNetServerToInAddr
        live_host_lock();
        int exists = 0;
        for (int i = 0; i < g_live.lsp_count; i++) {
            if (g_live.lsp_servers[i].real_ip == srv.address) {
                g_live.lsp_servers[i].real_port = srv.port;
                exists = 1;
                break;
            }
        }
        if (!exists && g_live.lsp_count < LIVE_MAX_LSP_SERVERS) {
            int idx = g_live.lsp_count++;
            g_live.lsp_servers[idx].real_ip = srv.address;
            g_live.lsp_servers[idx].real_port = srv.port;
            g_live.lsp_servers[idx].secure_ip = 0x00fe0000u | (u32)(idx + 1);
        }
        live_host_unlock();

        out[count++] = srv;
        arr = skip_ws(json_skip_value(arr));
        if (*arr == ',')
            arr = skip_ws(arr + 1);
    }
    *count_out = count;
    return 0;
}

// The list fetched into the cache (the title's own: the one XTitleServerCreateEnumerator asks for).
static int title_servers_fetch_cached(u32 title_id)
{
    port_live_title_server list[LIVE_MAX_LSP_SERVERS];
    int count = 0;
    const int result = fetch_title_servers(title_id, list, LIVE_MAX_LSP_SERVERS, &count);
    live_host_lock();
    if (result == 0) {
        __builtin_memcpy(g_live.title_servers, list, (unsigned int)count * sizeof(list[0]));
        g_live.title_server_count = count;
        g_live.title_servers_us = lan_host_microseconds();
    }
    g_live.title_servers_refreshing = 0;
    live_host_unlock();
    return result;
}

static PORT_MS_ABI void title_servers_refresh_job(const void *data) { title_servers_fetch_cached(*(const u32 *)data); }

// The title's servers, as XAM keeps them once signed in to Live: the list last fetched (signing in fetches it ahead, on
// the worker), refreshed in the background once older than LIVE_TITLE_SERVERS_REFRESH_MS, so only a title that asks
// before any list came waits for the server. Another title's list is fetched each time.
PORT_MS_ABI int port_live_fetch_title_servers(u32 title_id, port_live_title_server *out, int capacity, int *count_out)
{
    *count_out = 0;
    if (!port_live_enabled())
        return -1;
    if (title_id == 0)
        title_id = 0x4D5307E6u;
    if (title_id != 0x4D5307E6u)
        return fetch_title_servers(title_id, out, capacity, count_out);
    for (int attempt = 0; attempt < 2; attempt++) {
        live_host_lock();
        if (g_live.title_servers_us != 0) {
            const int count = g_live.title_server_count < capacity ? g_live.title_server_count : capacity;
            __builtin_memcpy(out, g_live.title_servers, (unsigned int)count * sizeof(out[0]));
            *count_out = count;
            const int refresh = !g_live.title_servers_refreshing
                && lan_host_microseconds() - g_live.title_servers_us >= LIVE_TITLE_SERVERS_REFRESH_MS * 1000ull;
            if (refresh)
                g_live.title_servers_refreshing = 1;
            live_host_unlock();
            if (refresh)
                port_live_post(title_servers_refresh_job, &title_id, sizeof(title_id));
            return 0;
        }
        live_host_unlock();
        // none yet (or the prefetch failed): fetched now, as before
        if (attempt == 0 && title_servers_fetch_cached(title_id) != 0)
            return -1;
    }
    return -1;
}

PORT_MS_ABI int port_live_server_to_inaddr(u32 server_ip, u32 service_id, u32 *secure_out)
{
    (void)service_id;
    if (!port_live_enabled() || secure_out == 0)
        return PORT_LAN_WSAENETDOWN;
    live_host_lock();
    for (int i = 0; i < g_live.lsp_count; i++) {
        if (g_live.lsp_servers[i].real_ip == server_ip) {
            *secure_out = g_live.lsp_servers[i].secure_ip;
            live_host_unlock();
            live_log("live: XNetServerToInAddr -> existing LSP server");
            return 0;
        }
    }
    if (g_live.lsp_count < LIVE_MAX_LSP_SERVERS) {
        int idx = g_live.lsp_count++;
        g_live.lsp_servers[idx].real_ip = server_ip != 0 ? server_ip : g_live.server_ip;
        g_live.lsp_servers[idx].real_port = g_live.server_port;
        g_live.lsp_servers[idx].secure_ip = 0x00fe0000u | (u32)(idx + 1);
        *secure_out = g_live.lsp_servers[idx].secure_ip;
        live_host_unlock();
        live_log("live: XNetServerToInAddr -> new LSP server");
        return 0;
    }
    live_host_unlock();
    return PORT_LAN_WSAENOBUFS;
}

PORT_MS_ABI int port_live_is_tcp_handle(u32 handle)
{
    return handle >= PORT_LIVE_TCP_HANDLE_BASE && handle < PORT_LIVE_TCP_HANDLE_BASE + LIVE_MAX_TCP_SOCKETS;
}

static live_tcp_slot *tcp_slot(u32 handle)
{
    if (!port_live_is_tcp_handle(handle))
        return 0;
    live_tcp_slot *s = &g_live.tcp[handle - PORT_LIVE_TCP_HANDLE_BASE];
    return s->used ? s : 0;
}

PORT_MS_ABI int port_live_tcp_socket(u32 *handle_out)
{
    if (!port_live_enabled())
        return PORT_LAN_WSAENETDOWN;
    live_host_socket fd = LIVE_HOST_NO_SOCKET;
    if (live_host_tcp_open(0, &fd) != LIVE_HOST_OK)
        return PORT_LAN_WSAENOBUFS;
    live_host_lock();
    for (u32 i = 0; i < LIVE_MAX_TCP_SOCKETS; i++) {
        if (!g_live.tcp[i].used) {
            g_live.tcp[i].used = 1;
            g_live.tcp[i].fd = fd;
            g_live.tcp[i].nonblocking = 0;
            g_live.tcp[i].connecting = 0;
            *handle_out = PORT_LIVE_TCP_HANDLE_BASE + i;
            live_host_unlock();
            return 0;
        }
    }
    live_host_unlock();
    live_host_tcp_close(fd);
    return PORT_LAN_WSAENOBUFS;
}

PORT_MS_ABI int port_live_tcp_close(u32 handle)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    if (!s) {
        live_host_unlock();
        return PORT_LAN_WSAENOTSOCK;
    }
    live_host_socket fd = s->fd;
    s->used = 0;
    live_host_unlock();
    live_host_tcp_close(fd);
    return 0;
}

PORT_MS_ABI int port_live_tcp_shutdown(u32 handle, int how)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    live_host_socket fd = s ? s->fd : LIVE_HOST_NO_SOCKET;
    live_host_unlock();
    if (fd == LIVE_HOST_NO_SOCKET)
        return PORT_LAN_WSAENOTSOCK;
    return live_host_tcp_shutdown(fd, how) == LIVE_HOST_OK ? 0 : PORT_LAN_WSAENOTCONN;
}

// HALO3_LIVE_LOG: how a title server (LSP) connection ended up, and after how long.
static void lsp_connect_log(u32 ip, u16 port, int connected, u64 microseconds)
{
    if (!g_live.log_enabled)
        return;
    char msg[128];
    sbuf m;
    sb_init(&m, msg, sizeof(msg));
    sb_str(&m, "live: title server connection to ");
    sb_ipv4(&m, ip);
    sb_char(&m, ':');
    sb_u64(&m, port);
    sb_str(&m, connected ? " made in " : " failed after ");
    sb_ms(&m, microseconds);
    sb_str(&m, " ms");
    lan_host_log(msg);
}

PORT_MS_ABI int port_live_tcp_connect(u32 handle, u32 address, u16 port)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    if (!s) {
        live_host_unlock();
        return PORT_LAN_WSAENOTSOCK;
    }
    u32 target_ip = address;
    u16 target_port = port;
    // Translate LSP secure address (0.254.0.N) to the real Live server IP and port
    // (Halo 3 connects on port 80, 1001, 1002, etc. depending on the LSP service).
    for (int i = 0; i < g_live.lsp_count; i++) {
        if (g_live.lsp_servers[i].secure_ip == address) {
            target_ip = g_live.lsp_servers[i].real_ip;
            target_port = g_live.lsp_servers[i].real_port;
            break;
        }
    }
    live_host_socket fd = s->fd;
    live_host_unlock();

    const u64 started = lan_host_microseconds();
    int r = live_host_tcp_connect(fd, target_ip, target_port);
    if (r == LIVE_HOST_IN_PROGRESS) {
        // a non-blocking socket: WSAEWOULDBLOCK, and the title waits for it to be writable (select); how it ends is
        // logged then (lsp_connect_log)
        live_host_lock();
        if (s->used && s->fd == fd) {
            s->connecting = 1;
            s->connect_us = started;
            s->connect_ip = target_ip;
            s->connect_port = target_port;
        }
        live_host_unlock();
        return PORT_LAN_WSAEWOULDBLOCK;
    }
    lsp_connect_log(target_ip, target_port, r == LIVE_HOST_OK, lan_host_microseconds() - started);
    return r == LIVE_HOST_OK ? 0 : PORT_LAN_WSAEHOSTUNREACH;
}

PORT_MS_ABI int port_live_tcp_set_nonblocking(u32 handle, int nonblocking)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    if (!s) {
        live_host_unlock();
        return PORT_LAN_WSAENOTSOCK;
    }
    s->nonblocking = nonblocking ? 1 : 0;
    live_host_socket fd = s->fd;
    live_host_unlock();
    return live_host_tcp_set_nonblocking(fd, nonblocking) == LIVE_HOST_OK ? 0 : PORT_LAN_WSAEINVAL;
}

PORT_MS_ABI int port_live_tcp_pending(u32 handle, u32 *bytes_out)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    live_host_socket fd = s ? s->fd : LIVE_HOST_NO_SOCKET;
    live_host_unlock();
    if (fd == LIVE_HOST_NO_SOCKET)
        return PORT_LAN_WSAENOTSOCK;
    return live_host_tcp_pending(fd, bytes_out) == LIVE_HOST_OK ? 0 : PORT_LAN_WSAEINVAL;
}

PORT_MS_ABI int port_live_tcp_send(u32 handle, const void *data, int size, int *sent_out)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    live_host_socket fd = s ? s->fd : LIVE_HOST_NO_SOCKET;
    live_host_unlock();
    if (fd == LIVE_HOST_NO_SOCKET)
        return PORT_LAN_WSAENOTSOCK;
    int n = live_host_tcp_send(fd, data, size);
    if (n >= 0) {
        *sent_out = n;
        return 0;
    }
    return n == LIVE_HOST_WOULD_BLOCK ? PORT_LAN_WSAEWOULDBLOCK : PORT_LAN_WSAENOTCONN;
}

PORT_MS_ABI int port_live_tcp_recv(u32 handle, void *buffer, int size, int *received_out)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    live_host_socket fd = s ? s->fd : LIVE_HOST_NO_SOCKET;
    live_host_unlock();
    if (fd == LIVE_HOST_NO_SOCKET)
        return PORT_LAN_WSAENOTSOCK;
    int n = live_host_tcp_recv(fd, buffer, size);
    if (n >= 0) {
        *received_out = n;
        return 0;
    }
    return n == LIVE_HOST_WOULD_BLOCK ? PORT_LAN_WSAEWOULDBLOCK : PORT_LAN_WSAENOTCONN;
}

PORT_MS_ABI int port_live_tcp_poll(u32 handle, int *readable, int *writable, int *error)
{
    live_host_lock();
    live_tcp_slot *s = tcp_slot(handle);
    live_host_socket fd = s ? s->fd : LIVE_HOST_NO_SOCKET;
    live_host_unlock();
    if (fd == LIVE_HOST_NO_SOCKET)
        return PORT_LAN_WSAENOTSOCK;
    int is_readable = 0, is_writable = 0, is_error = 0;
    if (live_host_tcp_poll(fd, &is_readable, &is_writable, &is_error) != LIVE_HOST_OK)
        return PORT_LAN_WSAEINVAL;
    if (readable)
        *readable = is_readable;
    if (writable)
        *writable = is_writable;
    if (error)
        *error = is_error;
    // a connect in progress ended: writable once made, an error once failed
    if (is_writable || is_error) {
        u64 started = 0;
        u32 ip = 0;
        u16 port = 0;
        live_host_lock();
        if (s->used && s->fd == fd && s->connecting) {
            s->connecting = 0;
            started = s->connect_us;
            ip = s->connect_ip;
            port = s->connect_port;
        }
        live_host_unlock();
        if (started != 0)
            lsp_connect_log(ip, port, !is_error, lan_host_microseconds() - started);
    }
    return 0;
}

// ---- XGI Sessions ----

static void qos_listen_report(const u8 xnkid[8], int anew);

static void track_hosted_session(const u8 xnkid[8], int add)
{
    live_host_lock();
    if (add) {
        int free_slot = -1;
        for (int i = 0; i < LIVE_MAX_HOSTED_SESSIONS; i++) {
            if (g_live.hosted[i].used && __builtin_memcmp(g_live.hosted[i].xnkid, xnkid, 8) == 0) {
                live_host_unlock();
                return;
            }
            if (!g_live.hosted[i].used && free_slot < 0)
                free_slot = i;
        }
        if (free_slot >= 0) {
            g_live.hosted[free_slot].used = 1;
            __builtin_memcpy(g_live.hosted[free_slot].xnkid, xnkid, 8);
        }
    } else {
        for (int i = 0; i < LIVE_MAX_HOSTED_SESSIONS; i++) {
            if (g_live.hosted[i].used && __builtin_memcmp(g_live.hosted[i].xnkid, xnkid, 8) == 0)
                g_live.hosted[i].used = 0;
        }
    }
    live_host_unlock();
}

// Session jobs captured this console's address on the title thread, possibly before the relay ACK arrived.
static const u8 *session_xnaddr(const u8 *original, u8 local[PORT_LAN_XNADDR_SIZE])
{
    unsigned int status = port_lan_title_xnaddr(local);
    return (status & 0x02u) != 0 && __builtin_memcmp(original + 10, local + 10, 6) == 0 ? local : original;
}

PORT_MS_ABI int port_live_session_create(const u8 xnkid[8], u32 flags, u32 user_index, u64 host_xuid, const u8 xnaddr[36],
    const u8 xnkey[16], u32 max_public, u32 max_private)
{
    if (!port_live_signed_in())
        return -1;
    if (on_live_worker())
        register_pending(1); // its newInstance would drop the session

    if (user_index >= 4)
        user_index = 0;

    u8 local[PORT_LAN_XNADDR_SIZE];
    xnaddr = session_xnaddr(xnaddr, local);
    u32 ip = live_xnaddr_ip(xnaddr);
    u16 port = load_u16_le(xnaddr + 8);
    if (port == 0)
        port = 3074;

    port_live_property props[LIVE_MAX_USER_PROPS];
    port_live_context ctxs[LIVE_MAX_USER_CTXS];
    u32 pcount = 0, ccount = 0;
    live_host_lock();
    pcount = g_live.user_prop_count[user_index];
    ccount = g_live.user_ctx_count[user_index];
    __builtin_memcpy(props, g_live.user_props[user_index], pcount * sizeof(props[0]));
    __builtin_memcpy(ctxs, g_live.user_ctxs[user_index], ccount * sizeof(ctxs[0]));
    live_host_unlock();

    char body[4096];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"sessionId\":\"");
    sb_hex_bytes(&b, xnkid, 8);
    sb_str(&b, "\",\"flags\":");
    sb_u64(&b, flags);
    sb_str(&b, ",\"hostXuid\":\"");
    sb_hex64(&b, host_xuid != 0 ? host_xuid : port_live_xuid());
    sb_str(&b, "\",\"hostAddress\":\"");
    sb_ipv4(&b, ip);
    sb_str(&b, "\",\"macAddress\":\"");
    sb_hex_bytes(&b, xnaddr + 10, 6);
    sb_str(&b, "\",\"port\":");
    sb_u64(&b, port);
    sb_str(&b, ",\"xnaddr\":\"");
    sb_base64(&b, xnaddr, 36);
    sb_str(&b, "\",\"xnkey\":\"");
    sb_base64(&b, xnkey, 16);
    sb_str(&b, "\",\"publicSlots\":");
    sb_u64(&b, max_public);
    sb_str(&b, ",\"privateSlots\":");
    sb_u64(&b, max_private);
    sb_char(&b, ',');
    append_props_and_ctxs_json(&b, props, pcount, ctxs, ccount);
    sb_char(&b, '}');

    int status = live_http_request("POST", "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions", body, 0, 0);
    if (status >= 200 && status < 300) {
        if ((flags & 0x08u) != 0) {
            track_hosted_session(xnkid, 1);
            qos_listen_report(xnkid, 1);
        }
        return 0;
    }
    return -1;
}

PORT_MS_ABI int port_live_session_modify(const u8 xnkid[8], u32 flags, u32 max_public, u32 max_private)
{
    if (!port_live_signed_in())
        return -1;
    char path[128], body[256];
    sbuf p, b;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/modify");

    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"flags\":");
    sb_u64(&b, flags);
    sb_str(&b, ",\"publicSlots\":");
    sb_u64(&b, max_public);
    sb_str(&b, ",\"privateSlots\":");
    sb_u64(&b, max_private);
    sb_char(&b, '}');

    int status = live_http_request("POST", path, body, 0, 0);
    return (status >= 200 && status < 300) ? 0 : -1;
}
// The QoS listeners of sessions (XNetQosListen): a session this console hosts has the server told whether its host
// answers QoS probes (the server's search leaves out one that does not). The title calls XNetQosListen from its main
// loop, enabling a session's listener when its QoS reply changes and disabling it every qos_reply_update_interval
// (100 ms) while it has none, so per session: the state last asked for, the state the server has been told (or need
// not be: a session this console does not host), and whether a job to tell it is queued. The job runs on the worker
// after the session calls made before it (XSessionCreate, XSessionMigrateHost), which add the sessions this console
// hosts; one that adds a session tells the server its listener's state at once (qos_listen_report). A report the server
// did not take (no answer, a server error) is made again: at the title's next call for it, or with the heartbeat.
#define LIVE_QOS_LISTEN_CAPACITY 16
#define LIVE_QOS_LISTEN_UNTOLD (-1)

typedef struct {
    u8 xnkid[8];
    int used;
    int wanted; // the listener: 1 enabled, 0 disabled
    int told;   // what the server has: wanted when there is nothing to tell it; LIVE_QOS_LISTEN_UNTOLD
    int queued; // a qos_listen_job for it is queued
    u32 asked;  // when the title last asked (a count): the least recent idle entry makes room for a new session
} live_qos_listen;

static live_qos_listen g_qos_listen[LIVE_QOS_LISTEN_CAPACITY]; // under the lock
static u32 g_qos_listen_asked;

static live_qos_listen *qos_listen_find(const u8 xnkid[8])
{
    for (int i = 0; i < LIVE_QOS_LISTEN_CAPACITY; i++) {
        if (g_qos_listen[i].used && __builtin_memcmp(g_qos_listen[i].xnkid, xnkid, 8) == 0)
            return &g_qos_listen[i];
    }
    return 0;
}

// Room for a session's listener: a free entry, else the one the title asked about least recently with no job queued
// (else, with every entry's job queued, the least recent: its job then finds nothing to do).
static live_qos_listen *qos_listen_room(void)
{
    live_qos_listen *idle = 0, *any = 0;
    for (int i = 0; i < LIVE_QOS_LISTEN_CAPACITY; i++) {
        live_qos_listen *e = &g_qos_listen[i];
        if (!e->used)
            return e;
        if (!e->queued && (idle == 0 || (int)(e->asked - idle->asked) < 0))
            idle = e;
        if (any == 0 || (int)(e->asked - any->asked) < 0)
            any = e;
    }
    return idle != 0 ? idle : any;
}

// A session this console no longer has (XSessionDelete): its listener is forgotten.
static void qos_listen_forget(const u8 xnkid[8])
{
    live_host_lock();
    live_qos_listen *listen = qos_listen_find(xnkid);
    if (listen != 0)
        listen->used = 0;
    live_host_unlock();
}

static int hosted_session_find(const u8 xnkid[8])
{
    for (int i = 0; i < LIVE_MAX_HOSTED_SESSIONS; i++) {
        if (g_live.hosted[i].used && __builtin_memcmp(g_live.hosted[i].xnkid, xnkid, 8) == 0)
            return 1;
    }
    return 0;
}

// On the worker: tells the server the state of the listener of xnkid's session if it hosts that session and has not
// been told it (anew: it has just come to host it).
static void qos_listen_report(const u8 xnkid[8], int anew)
{
    live_host_lock();
    live_qos_listen *listen = qos_listen_find(xnkid);
    if (listen == 0) {
        live_host_unlock();
        return;
    }
    if (anew)
        listen->told = LIVE_QOS_LISTEN_UNTOLD;
    const int enabled = listen->wanted;
    if (listen->told == enabled || !hosted_session_find(xnkid) || !port_live_signed_in()) {
        listen->told = enabled;
        live_host_unlock();
        return;
    }
    listen->told = enabled;
    live_host_unlock();

    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/modify");
    const int status
        = live_http_request("POST", path, enabled ? "{\"qosEnabled\":true}" : "{\"qosEnabled\":false}", 0, 0);
    if (status < 0 || status >= 500) {
        // not taken: told again (a 4xx is the server's final word: the session is gone, say)
        live_host_lock();
        listen = qos_listen_find(xnkid);
        if (listen != 0 && listen->told == enabled)
            listen->told = LIVE_QOS_LISTEN_UNTOLD;
        live_host_unlock();
    }
}

static PORT_MS_ABI void qos_listen_job(const void *data)
{
    const u8 *xnkid = (const u8 *)data;
    live_host_lock();
    live_qos_listen *listen = qos_listen_find(xnkid);
    if (listen != 0)
        listen->queued = 0;
    live_host_unlock();
    qos_listen_report(xnkid, 0);
}

// The heartbeat: the reports the server did not take are queued again.
static void qos_listen_retry(void)
{
    u8 again[LIVE_QOS_LISTEN_CAPACITY][8];
    int count = 0;
    live_host_lock();
    for (int i = 0; i < LIVE_QOS_LISTEN_CAPACITY; i++) {
        live_qos_listen *e = &g_qos_listen[i];
        if (e->used && !e->queued && e->told == LIVE_QOS_LISTEN_UNTOLD) {
            e->queued = 1;
            __builtin_memcpy(again[count++], e->xnkid, 8);
        }
    }
    live_host_unlock();
    for (int i = 0; i < count; i++)
        port_live_post(qos_listen_job, again[i], 8);
}

void port_live_qos_listen_changed(const u8 xnkid[8], int enabled)
{
    live_host_lock();
    live_qos_listen *listen = qos_listen_find(xnkid);
    if (listen == 0) {
        listen = qos_listen_room();
        __builtin_memcpy(listen->xnkid, xnkid, 8);
        listen->used = 1;
        listen->told = LIVE_QOS_LISTEN_UNTOLD;
        listen->queued = 0;
    }
    listen->wanted = enabled;
    listen->asked = ++g_qos_listen_asked;
    const int post = !listen->queued && listen->told != enabled;
    if (post)
        listen->queued = 1;
    live_host_unlock();
    if (post)
        port_live_post(qos_listen_job, xnkid, 8);
}

PORT_MS_ABI int port_live_session_join(const u8 xnkid[8], const u64 *xuids, const int *private_slots, u32 count)
{
    if (!port_live_signed_in())
        return -1;
    int has_guest = 0;
    for (u32 i = 0; i < count; i++) {
        if (((xuids[i] >> 54) & 3ull) != 0) {
            has_guest = 1;
            break;
        }
    }
    if (has_guest)
        register_local_guests(0);
    char path[128], body[2048];
    sbuf p, b;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/join");

    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"players\":[");
    for (u32 i = 0; i < count; i++) {
        if (i > 0)
            sb_char(&b, ',');
        sb_str(&b, "{\"xuid\":\"");
        sb_hex64(&b, xuids[i]);
        sb_str(&b, "\",\"private\":");
        sb_str(&b, (private_slots && private_slots[i]) ? "true}" : "false}");
    }
    sb_str(&b, "]}");

    int status = live_http_request("POST", path, body, 0, 0);
    return (status >= 200 && status < 300) ? 0 : (status == 409 ? 1 : -1);
}

PORT_MS_ABI int port_live_session_leave(const u8 xnkid[8], const u64 *xuids, u32 count)
{
    if (!port_live_signed_in())
        return -1;
    char path[128], body[1024];
    sbuf p, b;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/leave");

    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"xuids\":[");
    for (u32 i = 0; i < count; i++) {
        if (i > 0)
            sb_char(&b, ',');
        sb_char(&b, '"');
        sb_hex64(&b, xuids[i]);
        sb_char(&b, '"');
    }
    sb_str(&b, "]}");

    int status = live_http_request("POST", path, body, 0, 0);
    return (status >= 200 && status < 300) ? 0 : -1;
}

PORT_MS_ABI int port_live_session_start(const u8 xnkid[8])
{
    if (!port_live_signed_in())
        return -1;
    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/start");
    int status = live_http_request("POST", path, "{}", 0, 0);
    return (status >= 200 && status < 300) ? 0 : -1;
}

PORT_MS_ABI int port_live_session_end(const u8 xnkid[8])
{
    if (!port_live_signed_in())
        return -1;
    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/end");
    int status = live_http_request("POST", path, "{}", 0, 0);
    return (status >= 200 && status < 300) ? 0 : -1;
}

PORT_MS_ABI int port_live_session_delete(const u8 xnkid[8])
{
    track_hosted_session(xnkid, 0);
    qos_listen_forget(xnkid);
    if (!port_live_signed_in())
        return -1;
    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    int status = live_http_request("DELETE", path, 0, 0, 0);
    return (status >= 200 && status < 300) || status == 404 ? 0 : -1;
}

PORT_MS_ABI int port_live_session_migrate(const u8 xnkid[8], u32 user_index, u64 host_xuid, const u8 xnaddr[36], const u8 xnkey[16])
{
    if (!port_live_signed_in())
        return -1;
    if (on_live_worker())
        register_pending(1); // its newInstance would drop the session

    u8 local[PORT_LAN_XNADDR_SIZE];
    xnaddr = session_xnaddr(xnaddr, local);
    u32 ip = live_xnaddr_ip(xnaddr);
    u16 port = load_u16_le(xnaddr + 8);
    if (port == 0)
        port = 3074;

    char path[128], body[512];
    sbuf p, b;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/migrate");

    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"hostXuid\":\"");
    sb_hex64(&b, host_xuid != 0 ? host_xuid : port_live_xuid());
    sb_str(&b, "\",\"hostAddress\":\"");
    sb_ipv4(&b, ip);
    sb_str(&b, "\",\"macAddress\":\"");
    sb_hex_bytes(&b, xnaddr + 10, 6);
    sb_str(&b, "\",\"port\":");
    sb_u64(&b, port);
    sb_str(&b, ",\"xnaddr\":\"");
    sb_base64(&b, xnaddr, 36);
    sb_str(&b, "\",\"xnkey\":\"");
    sb_base64(&b, xnkey, 16);
    sb_str(&b, "\"}");

    int status = live_http_request("POST", path, body, 0, 0);
    if (status >= 200 && status < 300) {
        track_hosted_session(xnkid, 1);
        qos_listen_report(xnkid, 1);
        // the new host's user properties: a session it creates carries them, a migrated one is sent them
        if (user_index < 4)
            push_user_props_to_hosted_sessions(user_index);
        return 0;
    }
    return -1;
}

PORT_MS_ABI int port_live_session_arbitrate(const u8 xnkid[8], u64 nonce, port_live_arb_machine *machines, u32 max_machines,
    u32 *machine_count_out, u32 *total_players_out)
{
    *machine_count_out = 0;
    *total_players_out = 0;
    if (!port_live_signed_in())
        return -1;

    char path[128], body[64];
    sbuf p, b;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/arbitration");

    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"nonce\":\"");
    sb_hex64(&b, nonce);
    sb_str(&b, "\"}");

    char resp[8192];
    int status = live_http_request("POST", path, body, resp, sizeof(resp));
    if (status != 200)
        return -1;

    const char *tp = json_find_key(resp, "totalPlayers");
    if (tp)
        json_read_uint(tp, total_players_out);

    const char *marr = json_find_key(resp, "machines");
    if (!marr)
        return 0;
    marr = skip_ws(marr);
    if (*marr != '[')
        return 0;
    marr = skip_ws(marr + 1);

    u32 mcount = 0;
    while (*marr && *marr != ']' && mcount < max_machines) {
        if (*marr != '{')
            break;
        port_live_arb_machine *m = &machines[mcount];
        __builtin_memset(m, 0, sizeof(*m));

        char mid_str[32] = { 0 };
        const char *mv = json_find_key(marr, "machineId");
        if (mv && json_read_string(mv, mid_str, sizeof(mid_str)))
            parse_hex_u64(mid_str, &m->machine_id);

        const char *parr = json_find_key(marr, "players");
        if (parr) {
            parr = skip_ws(parr);
            if (*parr == '[') {
                parr = skip_ws(parr + 1);
                while (*parr && *parr != ']' && m->player_count < 16) {
                    char xs[32] = { 0 };
                    if (json_read_string(parr, xs, sizeof(xs))) {
                        u64 xu = 0;
                        if (parse_hex_u64(xs, &xu))
                            m->xuids[m->player_count++] = xu;
                    }
                    parr = skip_ws(json_skip_value(parr));
                    if (*parr == ',')
                        parr = skip_ws(parr + 1);
                }
            }
        }
        mcount++;
        marr = skip_ws(json_skip_value(marr));
        if (*marr == ',')
            marr = skip_ws(marr + 1);
    }
    *machine_count_out = mcount;
    return 0;
}

// The response uses decimal JSON numbers, including signed 64-bit ratings and counters.
// Keep integers out of floating point so values above 2^53 survive the round trip.
static int stats_integer(const char *p, long long *out)
{
    if (!p)
        return 0;
    const int negative = *p == '-';
    if (negative)
        p++;
    if (*p < '0' || *p > '9')
        return 0;
    const u64 limit = 0x7fffffffffffffffull + (unsigned int)negative;
    u64 value = 0;
    do {
        const unsigned int digit = (unsigned int)(*p++ - '0');
        if (value > (limit - digit) / 10)
            return 0;
        value = value * 10 + digit;
    } while (*p >= '0' && *p <= '9');
    p = skip_ws(p);
    if (*p != ',' && *p != '}')
        return 0;
    *out = negative && value ? -(long long)(value - 1) - 1 : (long long)value;
    return 1;
}

static int stats_uint(const char *obj, const char *key, u32 *out)
{
    long long value;
    if (!stats_integer(json_find_key(obj, key), &value) || value < 0 || value > 0xffffffffll)
        return 0;
    *out = (u32)value;
    return 1;
}

static const char *stats_first(const char *obj, const char *key)
{
    const char *array = json_find_key(obj, key);
    return array && *array == '[' ? skip_ws(array + 1) : "";
}

static const char *stats_next(const char *obj)
{
    const char *p = skip_ws(json_skip_value(obj));
    return *p == ',' ? skip_ws(p + 1) : "";
}

static int stats_column(const char *obj, const port_live_stats_view *view, port_live_property *column)
{
    if (!stats_uint(obj, "id", &column->id) || !stats_uint(obj, "type", &column->type))
        return 0;
    u32 i = 0;
    while (i < view->column_count && view->column_ids[i] != column->id)
        i++;
    if (i == view->column_count)
        return 0;
    const char *value = json_find_key(obj, "value");
    switch (column->type) {
    case PORT_LIVE_PROP_INT32:
    case PORT_LIVE_PROP_INT64:
        return stats_integer(value, &column->int_val) && (column->type == PORT_LIVE_PROP_INT64 ||
            (column->int_val >= -2147483648ll && column->int_val <= 2147483647ll));
    case PORT_LIVE_PROP_DOUBLE:
    case PORT_LIVE_PROP_FLOAT: {
        if (!value || (*value != '-' && (*value < '0' || *value > '9')))
            return 0;
        char *end;
        column->double_val = strtod(value, &end);
        const char *delimiter = skip_ws(end);
        return (*delimiter == ',' || *delimiter == '}') && __builtin_isfinite(column->double_val) &&
            (column->type == PORT_LIVE_PROP_DOUBLE || __builtin_fabs(column->double_val) <= __FLT_MAX__);
    }
    default:
        return 0;
    }
}

static void stats_rows(const char *obj, const u64 *xuids, u32 xuid_count, port_live_stats_view *view)
{
    for (const char *r = stats_first(obj, "rows"); *r == '{' && view->row_count < xuid_count; r = stats_next(r)) {
        port_live_stats_row row = { 0 };
        char xuid[17];
        if (!account_json_string(r, "xuid", xuid, sizeof(xuid)) || str_len(xuid) != 16 ||
            !parse_hex_u64(xuid, &row.xuid) || !stats_uint(r, "rank", &row.rank) ||
            !stats_integer(json_find_key(r, "rating"), &row.rating) ||
            !account_json_string(r, "gamertag", row.gamertag, sizeof(row.gamertag)))
            continue;
        u32 i = 0;
        while (i < xuid_count && xuids[i] != row.xuid)
            i++;
        if (i == xuid_count)
            continue;
        i = 0;
        while (i < view->row_count && view->rows[i].xuid != row.xuid)
            i++;
        if (i < view->row_count)
            continue;
        const __SIZE_TYPE__ offset = (__SIZE_TYPE__)view->row_count * view->column_count;
        for (const char *c = stats_first(r, "columns"); *c == '{' && row.column_count < view->column_count;
             c = stats_next(c)) {
            port_live_property column = { 0 };
            if (stats_column(c, view, &column))
                view->columns[offset + row.column_count++] = column;
        }
        view->rows[view->row_count++] = row;
    }
}

static void stats_read_views(const char *response, const u64 *xuids, u32 xuid_count, port_live_stats_view *views,
    u32 view_count, u8 *matched)
{
    const char *array = json_find_key(response, "views");
    if (!array || *array != '[')
        return;
    // The HTTP client can return a capped body. Publish rows only from a complete views array.
    const char *after = skip_ws(json_skip_value(array));
    if (*after != '}' && *after != ',')
        return;
    for (const char *v = skip_ws(array + 1); *v == '{'; v = stats_next(v)) {
        u32 id;
        if (!stats_uint(v, "viewId", &id))
            continue;
        for (u32 i = 0; i < view_count; i++) {
            if (!matched[i] && views[i].view_id == id) {
                matched[i] = 1;
                stats_rows(v, xuids, xuid_count, &views[i]);
                break;
            }
        }
    }
}

PORT_MS_ABI void port_live_read_stats(const u64 *xuids, u32 xuid_count, port_live_stats_view *views, u32 view_count)
{
    if (!views)
        return;
    for (u32 i = 0; i < view_count; i++)
        views[i].row_count = 0;
    if (!xuids || !xuid_count || !view_count || !port_live_enabled())
        return;

    live_account account;
    live_host_lock();
    unsigned int user = 0;
    while (user < 4 && !snapshot_account_locked(user, &account))
        user++;
    live_host_unlock();
    if (user == 4 || !account.token[0])
        return;

    // Upper bounds for the punctuation and decimal IDs, and exactly 16 hex digits per XUID.
    u64 capacity = 24 + 19ull * xuid_count;
    for (u32 i = 0; i < view_count; i++) {
        if (!views[i].rows || (views[i].column_count && !views[i].columns) ||
            views[i].column_count > sizeof(views[i].column_ids) / sizeof(views[i].column_ids[0]))
            return;
        capacity += 40 + 6 * views[i].column_count;
    }
    if (capacity + view_count > 0xffffffffu - LIVE_RESP_BUF_SIZE)
        return;
    const u32 bytes = (u32)capacity + LIVE_RESP_BUF_SIZE + view_count;
    char *body = (char *)lan_host_pages_alloc(bytes);
    if (!body)
        return;
    char *response = body + (u32)capacity;
    sbuf b;
    sb_init(&b, body, (u32)capacity);
    sb_str(&b, "{\"xuids\":[");
    for (u32 i = 0; i < xuid_count; i++) {
        if (i)
            sb_char(&b, ',');
        sb_char(&b, '"');
        sb_hex64(&b, xuids[i]);
        sb_char(&b, '"');
    }
    sb_str(&b, "],\"specs\":[");
    for (u32 i = 0; i < view_count; i++) {
        if (i)
            sb_char(&b, ',');
        sb_str(&b, "{\"viewId\":");
        sb_u64(&b, views[i].view_id);
        sb_str(&b, ",\"columnIds\":[");
        for (u32 c = 0; c < views[i].column_count; c++) {
            if (c)
                sb_char(&b, ',');
            sb_u64(&b, views[i].column_ids[c]);
        }
        sb_str(&b, "]}");
    }
    sb_str(&b, "]}");
    if (live_http_request_auth("POST", "/xlive/v1/titles/" LIVE_TITLE_ID "/stats/read", body, account.token,
            response, LIVE_RESP_BUF_SIZE) == 200) {
        u8 *matched = (u8 *)(response + LIVE_RESP_BUF_SIZE); // zeroed by lan_host_pages_alloc
        stats_read_views(response, xuids, xuid_count, views, view_count, matched);
    }
    lan_host_pages_free(body, bytes);
}

PORT_MS_ABI int port_live_session_write_stats(const u8 xnkid[8], u64 xuid, u32 view_count, const u32 *view_ids,
    const u32 *prop_counts, const port_live_property *props)
{
    if (!port_live_signed_in())
        return -1;
    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);
    sb_str(&p, "/stats");

    char body[8192];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"xuid\":\"");
    sb_hex64(&b, xuid);
    sb_str(&b, "\",\"flush\":false,\"views\":[");
    u32 offset = 0;
    for (u32 v = 0; v < view_count; v++) {
        if (v > 0)
            sb_char(&b, ',');
        sb_str(&b, "{\"viewId\":");
        sb_u64(&b, view_ids[v]);
        sb_char(&b, ',');
        append_props_and_ctxs_json(&b, props + offset, prop_counts[v], 0, 0);
        // append_props_and_ctxs_json adds ,"contexts":[], which the server ignores inside a view.
        sb_char(&b, '}');
        offset += prop_counts[v];
    }
    sb_str(&b, "]}");

    int status = live_http_request("POST", path, body, 0, 0);
    return (status >= 200 && status < 300) ? 0 : -1;
}

static int parse_session_object(const char *obj, port_live_search_result *out)
{
    __builtin_memset(out, 0, sizeof(*out));
    char sid[32] = { 0 }, xnaddr_b64[128] = { 0 }, xnkey_b64[64] = { 0 };

    const char *v = json_find_key(obj, "sessionId");
    if (!v || !json_read_string(v, sid, sizeof(sid)) || !parse_hex_bytes(sid, out->xnkid, 8))
        return 0;

    v = json_find_key(obj, "xnaddr");
    if (v && json_read_string(v, xnaddr_b64, sizeof(xnaddr_b64)))
        base64_decode(xnaddr_b64, out->xnaddr, 36);

    v = json_find_key(obj, "xnkey");
    if (v && json_read_string(v, xnkey_b64, sizeof(xnkey_b64)))
        base64_decode(xnkey_b64, out->xnkey, 16);

    if ((v = json_find_key(obj, "openPublicSlots")) != 0)
        json_read_uint(v, &out->open_public);
    if ((v = json_find_key(obj, "openPrivateSlots")) != 0)
        json_read_uint(v, &out->open_private);
    if ((v = json_find_key(obj, "filledPublicSlots")) != 0)
        json_read_uint(v, &out->filled_public);
    if ((v = json_find_key(obj, "filledPrivateSlots")) != 0)
        json_read_uint(v, &out->filled_private);
    return 1;
}

PORT_MS_ABI int port_live_session_search(u32 proc_index, u32 max_results, u32 num_users, u64 exclude_xuid,
    const port_live_property *props, u32 prop_count, const port_live_context *ctxs, u32 ctx_count,
    port_live_search_result *results, u32 results_cap, u32 *count_out)
{
    *count_out = 0;
    if (!port_live_signed_in())
        return -1;

    char body[4096];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"procedureIndex\":");
    sb_u64(&b, proc_index);
    sb_str(&b, ",\"maxResults\":");
    sb_u64(&b, max_results);
    sb_str(&b, ",\"numUsers\":");
    sb_u64(&b, num_users);
    if (exclude_xuid != 0) {
        sb_str(&b, ",\"excludeXuid\":\"");
        sb_hex64(&b, exclude_xuid);
        sb_char(&b, '"');
    }
    sb_char(&b, ',');
    append_props_and_ctxs_json(&b, props, prop_count, ctxs, ctx_count);
    sb_char(&b, '}');

    char *resp = (char *)lan_host_pages_alloc(LIVE_RESP_BUF_SIZE);
    if (!resp)
        return -1;
    int status = live_http_request("POST", "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/search", body, resp,
        LIVE_RESP_BUF_SIZE);
    if (status != 200) {
        lan_host_pages_free(resp, LIVE_RESP_BUF_SIZE);
        return -1;
    }

    const char *arr = json_find_key(resp, "sessions");
    u32 count = 0;
    if (arr) {
        arr = skip_ws(arr);
        if (*arr == '[') {
            arr = skip_ws(arr + 1);
            while (*arr && *arr != ']' && count < results_cap) {
                if (*arr != '{')
                    break;
                if (parse_session_object(arr, &results[count]))
                    count++;
                arr = skip_ws(json_skip_value(arr));
                if (*arr == ',')
                    arr = skip_ws(arr + 1);
            }
        }
    }
    lan_host_pages_free(resp, LIVE_RESP_BUF_SIZE);
    *count_out = count;
    return 0;
}

PORT_MS_ABI int port_live_session_search_by_id(const u8 xnkid[8], port_live_search_result *result_out, int *found_out)
{
    *found_out = 0;
    if (!port_live_signed_in())
        return -1;

    char path[128];
    sbuf p;
    sb_init(&p, path, sizeof(path));
    sb_str(&p, "/xlive/v1/titles/" LIVE_TITLE_ID "/sessions/");
    sb_hex_bytes(&p, xnkid, 8);

    char resp[4096];
    int status = live_http_request("GET", path, 0, resp, sizeof(resp));
    if (status == 404)
        return 0;
    if (status != 200)
        return -1;
    if (parse_session_object(resp, result_out))
        *found_out = 1;
    return 0;
}

// ---- friends & game invites (invite.h's Live channel) ----
//
// Routes (halo-3-server's /xlive/v1, with the account's bearer token; the contract is in this lane's report):
//   GET  /xlive/v1/friends        -> {"friends":[{"xuid":"<16 hex>","gamertag":"...","online":true}, ...]}
//   POST /xlive/v1/invites/send   {"to":["<16 hex>"],"titleId":"4D5307E6","sessionClass":1|2,
//                                  "session":{"sessionId":"<16 hex>","xnaddr":"<base64>","xnkey":"<base64>"}}
//   GET  /xlive/v1/invites/poll   -> {"invites":[{"id":"...","from":"<16 hex>","fromGamertag":"...","sessionClass":2,
//                                    "session":{...as sent}}, ...]}  (until acknowledged; the client drops repeats)
//   POST /xlive/v1/invites/ack    {"id":"...","accepted":true|false}
// A server that does not have them answers 404: that is remembered (one log line), the Guide says the server keeps no
// friends, and the poll stops. A server that cannot be reached is polled again later, each time twice as long after
// (LIVE_INVITE_POLL_MS up to LIVE_INVITE_POLL_MAX_MS), so nothing spins and nothing is logged per attempt.

static void session_json(sbuf *b, const u8 session[PORT_INVITE_SESSION_SIZE])
{
    sb_str(b, "{\"sessionId\":\"");
    sb_hex_bytes(b, session, 8);
    sb_str(b, "\",\"xnaddr\":\"");
    sb_base64(b, session + 8, 36);
    sb_str(b, "\",\"xnkey\":\"");
    sb_base64(b, session + 44, 16);
    sb_str(b, "\"}");
}

// A 404: the server has no such route. Remembered in *flag and logged once.
static void live_unsupported(volatile int *flag, const char *what)
{
    if (!__atomic_exchange_n(flag, 1, __ATOMIC_ACQ_REL)) {
        char line[160];
        sbuf s;
        sb_init(&s, line, sizeof(line));
        sb_str(&s, "live: the Live server has no ");
        sb_str(&s, what);
        sb_str(&s, " (404): the Guide shows none");
        lan_host_log(line);
    }
}

int port_live_friends(port_live_friend *out, unsigned int capacity, unsigned int *count_out)
{
    *count_out = 0;
    if (!port_live_signed_in())
        return -1;
    if (__atomic_load_n(&g_live.friends_unsupported, __ATOMIC_ACQUIRE))
        return PORT_LIVE_UNSUPPORTED;
    char *resp = (char *)lan_host_pages_alloc(LIVE_RESP_BUF_SIZE);
    if (resp == 0)
        return -1;
    const int status = live_http_request("GET", "/xlive/v1/friends", 0, resp, LIVE_RESP_BUF_SIZE);
    int result = -1;
    if (status == 404) {
        live_unsupported(&g_live.friends_unsupported, "friends list");
        result = PORT_LIVE_UNSUPPORTED;
    } else if (status == 200) {
        result = 0;
        const char *list = json_find_key(resp, "friends");
        unsigned int count = 0;
        if (list != 0 && *(list = skip_ws(list)) == '[') {
            list = skip_ws(list + 1);
            while (*list == '{' && count < capacity) {
                port_live_friend *f = &out[count];
                __builtin_memset(f, 0, sizeof(*f));
                char xuid[24] = { 0 };
                const char *v = json_find_key(list, "xuid");
                if (v != 0 && json_read_string(v, xuid, sizeof(xuid)) && parse_hex_u64(xuid, &f->xuid)) {
                    if ((v = json_find_key(list, "gamertag")) != 0)
                        json_read_string(v, f->gamertag, sizeof(f->gamertag));
                    v = json_find_key(list, "online");
                    f->online = v != 0 && *skip_ws(v) == 't';
                    count++;
                }
                list = skip_ws(json_skip_value(list));
                if (*list == ',')
                    list = skip_ws(list + 1);
            }
        }
        *count_out = count;
    }
    lan_host_pages_free(resp, LIVE_RESP_BUF_SIZE);
    return result;
}

PORT_MS_ABI int port_live_invite_send(unsigned long long to_xuid, const unsigned char session[PORT_INVITE_SESSION_SIZE],
    int session_class)
{
    if (!port_live_signed_in())
        return -1;
    if (__atomic_load_n(&g_live.invites_unsupported, __ATOMIC_ACQUIRE))
        return PORT_LIVE_UNSUPPORTED;
    char body[512];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"to\":[\"");
    sb_hex64(&b, to_xuid);
    sb_str(&b, "\"],\"titleId\":\"" LIVE_TITLE_ID "\",\"sessionClass\":");
    sb_u64(&b, (u64)session_class);
    sb_str(&b, ",\"session\":");
    session_json(&b, session);
    sb_char(&b, '}');
    char resp[256];
    const int status = live_http_request("POST", "/xlive/v1/invites/send", body, resp, sizeof(resp));
    if (status == 404) {
        live_unsupported(&g_live.invites_unsupported, "game invites");
        return PORT_LIVE_UNSUPPORTED;
    }
    return status >= 200 && status < 300 ? 0 : -1;
}

int port_live_invite_ack(const char *id, int accepted)
{
    if (!port_live_signed_in() || __atomic_load_n(&g_live.invites_unsupported, __ATOMIC_ACQUIRE))
        return -1;
    char body[160];
    sbuf b;
    sb_init(&b, body, sizeof(body));
    sb_str(&b, "{\"id\":");
    sb_json_str(&b, id);
    sb_str(&b, accepted ? ",\"accepted\":true}" : ",\"accepted\":false}");
    char resp[256];
    const int status = live_http_request("POST", "/xlive/v1/invites/ack", body, resp, sizeof(resp));
    if (status == 404) {
        live_unsupported(&g_live.invites_unsupported, "game invites");
        return PORT_LIVE_UNSUPPORTED;
    }
    return status >= 200 && status < 300 ? 0 : -1;
}

int port_live_invites_supported(void) { return !__atomic_load_n(&g_live.invites_unsupported, __ATOMIC_ACQUIRE); }
int port_live_friends_supported(void) { return !__atomic_load_n(&g_live.friends_unsupported, __ATOMIC_ACQUIRE); }

// One poll (the heartbeat thread): each invite delivered goes to the inbox (invite.c drops one it has). Returns 0, -1
// when the server could not be asked, PORT_LIVE_UNSUPPORTED.
static int poll_invites(void)
{
    char *resp = (char *)lan_host_pages_alloc(LIVE_RESP_BUF_SIZE);
    if (resp == 0)
        return -1;
    const int status = live_http_request("GET", "/xlive/v1/invites/poll", 0, resp, LIVE_RESP_BUF_SIZE);
    int result = -1;
    if (status == 404) {
        live_unsupported(&g_live.invites_unsupported, "game invites");
        result = PORT_LIVE_UNSUPPORTED;
    } else if (status == 200) {
        result = 0;
        const char *list = json_find_key(resp, "invites");
        if (list != 0 && *(list = skip_ws(list)) == '[') {
            list = skip_ws(list + 1);
            while (*list == '{') {
                port_invite invite;
                __builtin_memset(&invite, 0, sizeof(invite));
                port_live_search_result session;
                char from[24] = { 0 };
                const char *v = json_find_key(list, "id");
                const char *s = json_find_key(list, "session");
                u32 session_class = 0;
                // an id that does not fit is skipped, not cut: a cut one would be answered under another id
                if (v != 0 && json_string_fits(v, sizeof(invite.id)) &&
                    json_read_string(v, invite.id, sizeof(invite.id)) && invite.id[0] != 0 && s != 0 &&
                    parse_session_object(s, &session)) {
                    __builtin_memcpy(invite.session, session.xnkid, 8);
                    __builtin_memcpy(invite.session + 8, session.xnaddr, 36);
                    __builtin_memcpy(invite.session + 44, session.xnkey, 16);
                    if ((v = json_find_key(list, "sessionClass")) != 0)
                        json_read_uint(v, &session_class);
                    invite.session_class = (int)session_class;
                    if ((v = json_find_key(list, "from")) != 0 && json_read_string(v, from, sizeof(from)))
                        parse_hex_u64(from, &invite.inviter_xuid);
                    if ((v = json_find_key(list, "fromGamertag")) != 0)
                        json_read_string(v, invite.inviter, sizeof(invite.inviter));
                    invite.title_id = PORT_INVITE_TITLE_ID;
                    port_invite_received(&invite);
                }
                list = skip_ws(json_skip_value(list));
                if (*list == ',')
                    list = skip_ws(list + 1);
            }
        }
    }
    lan_host_pages_free(resp, LIVE_RESP_BUF_SIZE);
    return result;
}
