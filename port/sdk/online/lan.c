// port/sdk/online (host code, both hosts): System Link over UDP. The XNet of an Xbox on a System Link LAN, between
// PCs running the port: the title's datagram sockets, the keys and secure addresses of its sessions, the connection
// status of its peers and the QoS probes that find and measure hosts. Interface: lan.h; host services: lan_host.h.
//
// Each console's XNet sends and receives on one host UDP port (HALO3_LAN_PORT, 3074 by default, as the Xbox's XNet
// does; HALO3_LAN_SLOTS consoles on one PC take the next ports). A game datagram is a frame: a 60-byte header
// (the sender's XNADDR, the XNKID of the security association, the title ports) and the title's data. The title's
// sockets are this file's: a UDP or VDP socket bound to a title port (1000, 1001) receives the frames sent to that
// port. Frames are not encrypted (the Xbox's XNET_STARTUP_BYPASS_SECURITY); a frame between peers is taken only for
// a key this console registered.
//
// The System Link thread receives every frame and answers what XNet answers without the title: QoS probes,
// connection requests. It also runs the timers for QoS, connections and the H3RL Live relay. Everything is under
// one lock.
//
// Wire format (big-endian):
//   0  u32 'H3SL'   4 u8 version (1)   5 u8 kind   6 u16 flags (0)   8 u32 sequence (per sender)
//   12 u16 source title port   14 u16 destination title port
//   16 sender XNADDR: u32 ina, u32 inaOnline, u16 wPortOnline, u8 abEnet[6], u8 abOnline[20]
//   52 u8 XNKID[8] (zero for broadcasts)   60 payload
#include "lan.h"
#include "lan_host.h"
#include "live.h"
#include "../kernel/storage.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// port/sdk/kernel: the game's events (XNetQosLookup's hEvent).
PORT_MS_ABI int SetEvent(void *event);
// port/sdk/runtime: secure random bytes from the host.
int port_host_random(void *buffer, unsigned long size);

#define LAN_LOOPBACK 0x7f000001u
#define LAN_BROADCAST 0xffffffffu
#define LAN_DISCOVERY_SOURCE 0x00000001u // 0.0.0.1: where broadcasts come from
#define LAN_DEFAULT_PORT 3074
#define LAN_DEFAULT_SLOTS 4
#define LAN_SLOT_LIMIT 16
#define LAN_FIRST_DYNAMIC_PORT 49152

#define LAN_MAGIC 0x4833534cu // "H3SL"
#define LAN_VERSION 1
#define LAN_HEADER_SIZE 60
#define LAN_FRAME_LIMIT 1400
#define LAN_QOS_DATA_LIMIT 1020 // 255 * 4: the most XNetStartupParams.cfgQosDataLimitDiv4 allows
#define LAN_QOS_PROBE_SIZE 23
#define LAN_QOS_REPLY_HEADER_SIZE 19

#define RELAY_MAGIC 0x4833524cu // "H3RL"
#define RELAY_VERSION 1
#define RELAY_PEER_HEADER_SIZE 12
#define RELAY_DATA_HEADER_SIZE 18
#define RELAY_FRAME_LIMIT (RELAY_DATA_HEADER_SIZE + 1500)
#define RELAY_TOKEN_LIMIT 128
#define RELAY_REGISTER_INTERVAL_US 15000000ull
#define RELAY_DIRECT_LIFETIME_US 15000000ull
#define RELAY_PUNCH_INTERVAL_US 2000000ull
#define RELAY_PING_INTERVAL_US 50000ull
#define RELAY_FALLBACK_US 350000ull

enum {
    RELAY_REGISTER = 1,
    RELAY_REGISTER_ACK,
    RELAY_PUNCH_REQUEST,
    RELAY_PUNCH_PEER,
    RELAY_PUNCH_PING,
    RELAY_PUNCH_PONG,
    RELAY_DATA,
};

enum {
    LAN_KIND_UDP = 1,       // a title datagram to a UDP socket
    LAN_KIND_VDP,           // a title datagram to a VDP socket
    LAN_KIND_BROADCAST,     // a title datagram sent to 255.255.255.255
    LAN_KIND_QOS_PROBE,     // XNetQosLookup's probe
    LAN_KIND_QOS_REPLY,     // the listener's answer
    LAN_KIND_CONNECT,       // XNetConnect: open a security association
    LAN_KIND_ACCEPT,        // its answer
    LAN_KIND_DISCONNECT,    // the association was closed (XNetUnregisterInAddr, XNetUnregisterKey, XNetCleanup)
};

// XNet's values (winsockx.h).
enum {
    LAN_CONNECT_STATUS_IDLE = 0,
    LAN_CONNECT_STATUS_PENDING = 1,
    LAN_CONNECT_STATUS_CONNECTED = 2,
    LAN_CONNECT_STATUS_LOST = 3,
    LAN_GET_XNADDR_NONE = 0x01,
    LAN_GET_XNADDR_ETHERNET = 0x02,
    LAN_GET_XNADDR_STATIC = 0x04,
    LAN_ETHERNET_LINK_ACTIVE = 0x01,
    LAN_ETHERNET_LINK_100MBPS = 0x02,
    LAN_ETHERNET_LINK_FULL_DUPLEX = 0x08,
    LAN_QOS_LISTEN_ENABLE = 0x01,
    LAN_QOS_LISTEN_DISABLE = 0x02,
    LAN_QOS_LISTEN_SET_DATA = 0x04,
    LAN_QOS_LISTEN_SET_BITSPERSEC = 0x08,
    LAN_QOS_LISTEN_RELEASE = 0x10,
    LAN_XNQOSINFO_COMPLETE = 0x01,
    LAN_XNQOSINFO_TARGET_CONTACTED = 0x02,
    LAN_XNQOSINFO_TARGET_DISABLED = 0x04,
    LAN_XNQOSINFO_DATA_RECEIVED = 0x08,
};

// XNADDR (winsockx.h, byte-packed): IN_ADDR ina, IN_ADDR inaOnline, WORD wPortOnline, BYTE abEnet[6], abOnline[20].
#define XNADDR_INA 0
#define XNADDR_INA_ONLINE 4
#define XNADDR_PORT_ONLINE 8
#define XNADDR_ENET 10
#define XNADDR_ONLINE 16

// XNQOS and XNQOSINFO (winsockx.h, byte-packed).
#pragma pack(push, 1)
typedef struct lan_xnqosinfo {
    u8 flags, reserved;
    u16 probes_sent, probes_received, data_size;
    u8 *data;
    u16 rtt_minimum_ms, rtt_median_ms;
    u32 up_bits_per_second, down_bits_per_second;
} lan_xnqosinfo;
typedef struct lan_xnqos {
    u32 count, pending;
    lan_xnqosinfo info[1];
} lan_xnqos;
#pragma pack(pop)
_Static_assert(sizeof(lan_xnqosinfo) == (sizeof(void *) == 8 ? 28 : PORT_LAN_XNQOSINFO_SIZE), "XNQOSINFO");

// Limits: the XNetStartupParams the title passes, capped at what these tables hold.
#define LAN_SOCKET_LIMIT 16
#define LAN_INBOX_SLOTS 64
#define LAN_KEY_LIMIT 16
#define LAN_PEER_LIMIT 64
#define LAN_ENDPOINT_LIMIT 64
#define LAN_SENDER_LIMIT 64
#define LAN_TARGET_LIMIT 64
#define LAN_INTERFACE_LIMIT 16
#define LAN_CONFIGURED_PEER_LIMIT 16
#define LAN_QOS_LOOKUP_LIMIT 16
#define LAN_QOS_PROBE_LIMIT 16
#define LAN_QOS_TARGET_LIMIT 0xffff

// Timing.
#define LAN_THREAD_TICK_MS 50
#define LAN_CONNECT_RETRY_US 200000ull
#define LAN_CONNECT_TRIES 25 // 5 s, then the peer reads as lost
#define LAN_QOS_PROBE_SPACING_US 2000ull
#define LAN_QOS_ATTEMPT_US 400000ull
#define LAN_TARGETS_REFRESH_US 5000000ull
#define LAN_LINK_BITS_PER_SECOND 100000000u // what QoS reports for a LAN host: the 100 Mbit/s link XNet reports

typedef struct lan_datagram {
    u32 source;
    u16 source_port, size;
    u8 data[PORT_LAN_MAX_DATAGRAM];
} lan_datagram;

typedef struct lan_socket {
    u32 handle; // 0: free
    u8 vdp, nonblocking, broadcast, connected;
    u16 port;   // 0: not bound
    port_lan_address remote;
    u32 head, count, bytes, budget, dropped;
    lan_datagram inbox[LAN_INBOX_SLOTS];
} lan_socket;

typedef struct lan_key {
    u8 used;
    u8 kid[PORT_LAN_XNKID_SIZE], key[PORT_LAN_XNKEY_SIZE];
} lan_key;

typedef struct lan_listener {
    u8 used, enabled;
    u8 kid[PORT_LAN_XNKID_SIZE];
    u16 size;
    u8 data[LAN_QOS_DATA_LIMIT];
    u32 bits_per_second;
    // XNQOSLISTENSTATS after dwSizeOfStruct: data requests received, probes received, slots-full discards, data
    // replies sent, data reply bytes sent, probe replies sent
    u32 stats[6];
} lan_listener;

typedef struct lan_peer {
    u32 secure; // 0: free; else the handle 0.h.l.0
    u8 xnaddr[PORT_LAN_XNADDR_SIZE], kid[PORT_LAN_XNKID_SIZE];
    u8 status, tries;
    u64 next_us;
    u64 connect_us; // when this console last asked it to connect (HALO3_LAN_LOG: the handshake time)
} lan_peer;

// Per-console routes, shared by all of its security associations and any outstanding QoS lookups.
typedef struct lan_endpoint {
    u8 used, mac[6], local, pings_left;
    u32 address, public_address;
    u16 port, public_port;
    u64 seen_us, direct_seen_us, fallback_us, next_punch_us, next_ping_us;
} lan_endpoint;

typedef struct lan_relay {
    u32 address;
    u16 port, token_size;
    u64 xuid, next_register_us;
    u8 token[RELAY_TOKEN_LIMIT], nonce[8];
    int ready, awaiting_ack;
} lan_relay;

// The broadcasts already taken from a console: a broadcast reaches a console once per path (255.255.255.255, each
// subnet's broadcast address, HALO3_LAN_PEER), and is delivered once.
typedef struct lan_sender {
    u8 used, mac[6];
    u32 newest;
    u64 window, seen_us;
} lan_sender;

typedef struct lan_qos_target {
    u8 xnaddr[PORT_LAN_XNADDR_SIZE], kid[PORT_LAN_XNKID_SIZE];
    u8 done, attempt, cursor, waiting, contacted, disabled, have_data;
    u16 probes, answered, data_size;
    u32 answered_mask;
    u32 rtt_us[LAN_QOS_PROBE_LIMIT];
    u64 next_us;
} lan_qos_target;

typedef struct lan_qos_lookup {
    u8 *memory; // 0: free; the XNQOS the title holds, then the data blocks, then the targets
    u32 memory_size, id, count, pending;
    u64 started_us; // HALO3_LAN_LOG: how long the lookup took
    lan_qos_target *targets;
    u8 *data;
    void *event;
} lan_qos_lookup;

static struct {
    // configuration (the environment, read once)
    int configured, enabled, live_enabled, log, trace;
    u16 base_port, slots;
    u32 forced_address;
    int configured_peer_count;
    port_lan_address configured_peers[LAN_CONFIGURED_PEER_LIMIT]; // port 0: every slot port

    // this console
    int identity_ready, duplicate_logged;
    u8 mac[6];
    u8 xnaddr[PORT_LAN_XNADDR_SIZE];
    lan_relay relay;

    // XNet
    int running, failed, stopping;
    lan_host_socket host;
    u16 host_port;
    u32 sequence, broadcast_sequence;
    u32 socket_limit, key_limit, peer_limit, qos_data_limit, receive_buffer, qos_retries;
    u32 socket_generation;
    u16 next_dynamic_port;
    u32 next_secure, next_lookup;
    u32 activity;
    lan_socket sockets[LAN_SOCKET_LIMIT];
    lan_key keys[LAN_KEY_LIMIT];
    lan_listener listeners[LAN_KEY_LIMIT];
    lan_peer peers[LAN_PEER_LIMIT];
    lan_endpoint endpoints[LAN_ENDPOINT_LIMIT];
    lan_sender senders[LAN_SENDER_LIMIT];
    lan_qos_lookup lookups[LAN_QOS_LOOKUP_LIMIT];
    int target_count;
    port_lan_address targets[LAN_TARGET_LIMIT];
    u64 targets_refreshed_us;
    u64 thread_deadline_us; // when the System Link thread next runs its timers
    int interface_count;
    lan_host_interface interfaces[LAN_INTERFACE_LIMIT];
} g_lan;

// ---- small helpers ----

static void copy(void *to, const void *from, u32 size)
{
    u8 *t = (u8 *)to;
    const u8 *f = (const u8 *)from;
    while (size-- != 0)
        *t++ = *f++;
}

static void zero(void *to, u32 size)
{
    u8 *t = (u8 *)to;
    while (size-- != 0)
        *t++ = 0;
}

static int same(const void *a, const void *b, u32 size)
{
    const u8 *x = (const u8 *)a, *y = (const u8 *)b;
    for (u32 i = 0; i < size; i++)
        if (x[i] != y[i])
            return 0;
    return 1;
}

static int all_zero(const void *a, u32 size)
{
    const u8 *x = (const u8 *)a;
    for (u32 i = 0; i < size; i++)
        if (x[i] != 0)
            return 0;
    return 1;
}

static u32 load32(const void *p) { u32 v; copy(&v, p, 4); return v; }
static u16 load16(const void *p) { u16 v; copy(&v, p, 2); return v; }
static void store32(void *p, u32 v) { copy(p, &v, 4); }
static void store16(void *p, u16 v) { copy(p, &v, 2); }

static void put16(u8 *p, u16 v) { p[0] = (u8)(v >> 8); p[1] = (u8)v; }
static void put32(u8 *p, u32 v) { put16(p, (u16)(v >> 16)); put16(p + 2, (u16)v); }
static void put64(u8 *p, u64 v) { put32(p, (u32)(v >> 32)); put32(p + 4, (u32)v); }
static u16 get16(const u8 *p) { return (u16)((p[0] << 8) | p[1]); }
static u32 get32(const u8 *p) { return ((u32)get16(p) << 16) | get16(p + 2); }
static u64 get64(const u8 *p) { return ((u64)get32(p) << 32) | get32(p + 4); }

static u64 now_us(void) { return lan_host_microseconds(); }

static void touch(void)
{
    __atomic_store_n(&g_lan.activity, g_lan.activity + 1, __ATOMIC_RELEASE);
    lan_host_wake_all();
}

// ---- log ----

typedef struct lan_text {
    char text[256];
    u32 size;
} lan_text;

static void text_add(lan_text *t, const char *s)
{
    while (*s != 0 && t->size < sizeof(t->text) - 1)
        t->text[t->size++] = *s++;
    t->text[t->size] = 0;
}

static void text_number(lan_text *t, u32 value)
{
    char digits[12];
    int n = 0;
    do {
        digits[n++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    char s[12];
    for (int i = 0; i < n; i++)
        s[i] = digits[n - 1 - i];
    s[n] = 0;
    text_add(t, s);
}

static void text_u64(lan_text *t, u64 value)
{
    char s[21];
    int n = 20;
    s[n] = 0;
    do {
        s[--n] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0);
    text_add(t, s + n);
}

static void text_hex8(lan_text *t, u8 value)
{
    static const char k_hex[] = "0123456789abcdef";
    char s[3] = { k_hex[value >> 4], k_hex[value & 15], 0 };
    text_add(t, s);
}

static void text_endpoint(lan_text *t, u32 address, u16 port)
{
    for (int i = 0; i < 4; i++) {
        text_number(t, (address >> (24 - 8 * i)) & 0xff);
        if (i < 3)
            text_add(t, ".");
    }
    if (port != 0) {
        text_add(t, ":");
        text_number(t, port);
    }
}

static void text_mac(lan_text *t, const u8 *mac)
{
    for (int i = 0; i < 6; i++) {
        text_hex8(t, mac[i]);
        if (i < 5)
            text_add(t, ":");
    }
}

static void text_ms(lan_text *t, u64 microseconds)
{
    text_number(t, (u32)(microseconds / 1000u));
    char tenth[3] = { '.', (char)('0' + (microseconds / 100u) % 10u), 0 };
    text_add(t, tenth);
}

static void lan_log(lan_text *t) { lan_host_log(t->text); }

// ---- configuration ----

static int parse_number(const char **s, u32 limit, u32 *value)
{
    const char *p = *s;
    u32 v = 0;
    if (*p < '0' || *p > '9')
        return 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (u32)(*p - '0');
        if (v > limit)
            return 0;
        p++;
    }
    *s = p;
    *value = v;
    return 1;
}

// "a.b.c.d" (decimal), the address in host order.
static int parse_ipv4(const char **s, u32 *address)
{
    u32 a = 0;
    for (int i = 0; i < 4; i++) {
        u32 part;
        if (!parse_number(s, 255, &part))
            return 0;
        a = (a << 8) | part;
        if (i < 3) {
            if (**s != '.')
                return 0;
            (*s)++;
        }
    }
    *address = a;
    return 1;
}

static int env_flag(const char *name)
{
    const char *v = lan_host_getenv(name);
    if (v == 0 || *v == 0)
        return 0;
    return !(v[0] == '0' && v[1] == 0) && !(v[0] == 'o' && v[1] == 'f') && !(v[0] == 'n' && v[1] == 'o')
        && !(v[0] == 'f' && v[1] == 'a');
}

static void configure(int live_enabled)
{
    if (g_lan.configured)
        return;
    g_lan.live_enabled = live_enabled;
    g_lan.enabled = env_flag("HALO3_LAN") || live_enabled;
    g_lan.log = env_flag("HALO3_LAN_LOG");
    g_lan.trace = env_flag("HALO3_LAN_TRACE");
    g_lan.base_port = LAN_DEFAULT_PORT;
    g_lan.slots = LAN_DEFAULT_SLOTS;
    const char *v = lan_host_getenv("HALO3_LAN_PORT");
    u32 number;
    if (v != 0 && parse_number(&v, 0xffff, &number) && *v == 0 && number != 0)
        g_lan.base_port = (u16)number;
    v = lan_host_getenv("HALO3_LAN_SLOTS");
    if (v != 0 && parse_number(&v, LAN_SLOT_LIMIT, &number) && *v == 0 && number != 0)
        g_lan.slots = (u16)number;
    if (g_lan.base_port + g_lan.slots - 1 > 0xffff)
        g_lan.slots = (u16)(0x10000 - g_lan.base_port);
    v = lan_host_getenv("HALO3_LAN_ADDRESS");
    if (v != 0 && !(parse_ipv4(&v, &g_lan.forced_address) && *v == 0))
        g_lan.forced_address = 0;
    // HALO3_LAN_PEER: "ip[:port],ip[:port],...": also send broadcasts there (no port: to every slot port)
    v = lan_host_getenv("HALO3_LAN_PEER");
    while (v != 0 && *v != 0 && g_lan.configured_peer_count < LAN_CONFIGURED_PEER_LIMIT) {
        port_lan_address peer = { 0, 0 };
        u32 port = 0;
        int ok = parse_ipv4(&v, &peer.address);
        if (ok && *v == ':') {
            v++;
            ok = parse_number(&v, 0xffff, &port) && port != 0;
        }
        if (ok && (*v == ',' || *v == 0)) {
            peer.port = (u16)port;
            g_lan.configured_peers[g_lan.configured_peer_count++] = peer;
        } else {
            lan_text t = { { 0 }, 0 };
            text_add(&t, "system link: HALO3_LAN_PEER: expected ip[:port][,ip[:port]...]");
            lan_log(&t);
            break;
        }
        if (*v == ',')
            v++;
    }
    __atomic_store_n(&g_lan.configured, 1, __ATOMIC_RELEASE);
}

// ---- identity ----
//
// This console's MAC address, the XNADDR's abEnet. An Xbox's is its network adapter's, the same for the console's
// whole life, and the title builds lasting identities on it: the machine identifier (transport_security_initialize
// takes abEnet as its local identifier) and each signed-in user's player identifier (network_update_online_users: a
// hash of the user's XUID and the machine identifier). A campaign game saved to the storage device keeps both, and
// resuming it compares them with the session's (c_simulation_watcher::generate_player_updates): a saved player whose
// identifiers are not the session's has left the game, and the campaign ends ("The game ended because a player left
// the game"). So the address is the console's, not the process's: kept in the data directory, the console's hard
// drive (port/sdk/kernel/storage.h, where its profiles and saved games are), as CONSOLE_FILE, made at its first use
// (random, locally administered, unicast) and read on every run after. Consoles on one PC are instances with a data
// directory each (HALO3_DATA, as play/boot_mm_4p.sh runs them). The address is the console's with System Link off as
// well (port_lan_title_xnaddr), so a game saved with System Link on resumes with it off and the other way round.
//
// A data directory is one running console's: the first instance on it holds CONSOLE_LOCK until it exits, and an
// instance started on it meanwhile (another HALO3_PROFILE_NAME, say) is another console, with an address for that run
// only, or the two could not reach each other over System Link (each takes the other's frames for its own). A copy of
// a data directory has its console file too: an instance that hears its own address from another console says so
// (identity_check_duplicate).
//
// CONSOLE_FILE is CONSOLE_HEADER, then "mac " and the address as six two-digit hexadecimal numbers joined by ':', and
// a newline. It may be written by hand, to give a data directory the address its saved games were made with (lines
// may end "\r\n", the last newline may be missing). One that does not check out is set aside as console.txt.corrupt
// and replaced by a new address. With no data directory, or a file that cannot be read or written, the address is new
// for this run only (logged): games saved in such a run do not resume in another.
#define CONSOLE_FILE "console.txt"
#define CONSOLE_LOCK "console.lock"
#define CONSOLE_HEADER "H3CONSOLE 1\n"
#define CONSOLE_MAC_PREFIX "mac "
#define CONSOLE_FILE_SIZE (sizeof(CONSOLE_HEADER) - 1 + sizeof(CONSOLE_MAC_PREFIX) - 1 + 6 * 3)
#define CONSOLE_FILE_CAPACITY 64 // bytes read: a hand-written file's "\r"s fit, a longer file does not check out
#define CONSOLE_ERROR_SUCCESS 0ul
#define CONSOLE_ERROR_FILE_NOT_FOUND 2ul
#define CONSOLE_ERROR_PATH_NOT_FOUND 3ul
#define CONSOLE_ERROR_SHARING_VIOLATION 32ul
#define CONSOLE_ERROR_INSUFFICIENT_BUFFER 122ul

// An address a console can have: unicast (the group bit clear) and not zero.
static int console_mac_valid(const u8 *mac) { return (mac[0] & 1) == 0 && !all_zero(mac, 6); }

static int hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
}

// Whether `text` (size bytes) is a CONSOLE_FILE holding a valid address; the address in mac.
static int console_parse(const char *text, u32 size, u8 *mac)
{
    char file[CONSOLE_FILE_SIZE]; // the text without its '\r's, then with its last newline
    u32 length = 0;
    for (u32 i = 0; i < size; i++) {
        if (text[i] == '\r')
            continue;
        if (length == sizeof(file))
            return 0;
        file[length++] = text[i];
    }
    if (length == sizeof(file) - 1 && file[length - 1] != '\n')
        file[length++] = '\n';
    const u32 header = sizeof(CONSOLE_HEADER) - 1, prefix = sizeof(CONSOLE_MAC_PREFIX) - 1;
    if (length != sizeof(file) || !same(file, CONSOLE_HEADER, header)
        || !same(file + header, CONSOLE_MAC_PREFIX, prefix))
        return 0;
    const char *p = file + header + prefix;
    for (int i = 0; i < 6; i++, p += 3) {
        const int high = hex_digit(p[0]), low = hex_digit(p[1]);
        if (high < 0 || low < 0 || p[2] != (i < 5 ? ':' : '\n'))
            return 0;
        mac[i] = (u8)(high << 4 | low);
    }
    return console_mac_valid(mac);
}

static void console_log(const char *what, const u8 *mac, unsigned long error)
{
    lan_text t = { { 0 }, 0 };
    text_add(&t, "console: ");
    text_add(&t, what);
    if (mac != 0) {
        text_add(&t, " ");
        text_mac(&t, mac);
    }
    if (error != CONSOLE_ERROR_SUCCESS) {
        text_add(&t, " (error ");
        text_number(&t, (u32)error);
        text_add(&t, ")");
    }
    lan_log(&t);
}

// A new address: locally administered, unicast, random.
static void identity_random(void)
{
    do
        port_host_random(g_lan.mac, sizeof(g_lan.mac));
    while (all_zero(g_lan.mac + 2, 4));
    g_lan.mac[0] = (u8)((g_lan.mac[0] & 0xfc) | 0x02);
}

// Under the lock.
static void identity_create(void)
{
    if (g_lan.identity_ready)
        return;
    g_lan.identity_ready = 1;
    const unsigned long locked = port_storage_data_lock(CONSOLE_LOCK);
    if (locked == CONSOLE_ERROR_SHARING_VIOLATION) {
        identity_random();
        console_log("another instance runs on this data directory (it holds " CONSOLE_LOCK "); for this run only, MAC "
            "address", g_lan.mac, CONSOLE_ERROR_SUCCESS);
        return;
    }
    if (locked != CONSOLE_ERROR_SUCCESS && locked != CONSOLE_ERROR_PATH_NOT_FOUND)
        console_log(CONSOLE_LOCK " cannot be held: an instance started on this data directory would share this "
            "console's MAC address", 0, locked);
    char text[CONSOLE_FILE_CAPACITY];
    unsigned int size = 0;
    const unsigned int read = port_storage_data_read(CONSOLE_FILE, text, sizeof(text), &size);
    if (read == CONSOLE_ERROR_SUCCESS && console_parse(text, (u32)size, g_lan.mac))
        return;
    identity_random();
    if (read == CONSOLE_ERROR_SUCCESS || read == CONSOLE_ERROR_INSUFFICIENT_BUFFER) {
        const unsigned long aside = port_storage_data_set_aside(CONSOLE_FILE);
        if (aside != CONSOLE_ERROR_SUCCESS) {
            // still there: never overwritten
            console_log(CONSOLE_FILE " does not check out and cannot be set aside; for this run only, MAC address",
                g_lan.mac, aside);
            return;
        }
        console_log(CONSOLE_FILE " does not check out; set aside as " CONSOLE_FILE ".corrupt", 0,
            CONSOLE_ERROR_SUCCESS);
    } else if (read != CONSOLE_ERROR_FILE_NOT_FOUND && read != CONSOLE_ERROR_PATH_NOT_FOUND) {
        // there may be one: never overwritten
        console_log(CONSOLE_FILE " cannot be read; for this run only, MAC address", g_lan.mac, read);
        return;
    }
    lan_text t = { { 0 }, 0 };
    text_add(&t, CONSOLE_HEADER CONSOLE_MAC_PREFIX);
    text_mac(&t, g_lan.mac);
    text_add(&t, "\n");
    const unsigned long written = port_storage_data_write(CONSOLE_FILE, t.text, t.size);
    if (written != CONSOLE_ERROR_SUCCESS)
        console_log(CONSOLE_FILE " cannot be written; for this run only, MAC address", g_lan.mac, written);
    else
        console_log("new MAC address, kept in " CONSOLE_FILE ":", g_lan.mac, CONSOLE_ERROR_SUCCESS);
}

static const u8 *xnaddr_mac(const u8 *xnaddr) { return xnaddr + XNADDR_ENET; }

static int own_mac(const u8 *mac) { return same(mac, g_lan.mac, 6); }

// A frame from UDP port `port` that carries this console's address: this console's own, or another console's that
// has the same address, which neither can reach (each drops the other's frames as its own): one's data directory is a
// copy of the other's (identity_create). Logged once.
static void identity_check_duplicate(const u8 *xnaddr, u16 port)
{
    if (g_lan.duplicate_logged
        || (port == g_lan.host_port && load32(xnaddr + XNADDR_INA) == load32(g_lan.xnaddr + XNADDR_INA)
            && load16(xnaddr + XNADDR_PORT_ONLINE) == load16(g_lan.xnaddr + XNADDR_PORT_ONLINE)))
        return;
    g_lan.duplicate_logged = 1;
    lan_text t = { { 0 }, 0 };
    text_add(&t, "system link: the console at ");
    text_endpoint(&t, load32(xnaddr + XNADDR_INA), load16(xnaddr + XNADDR_PORT_ONLINE));
    text_add(&t, " has this console's MAC address too, so neither reaches the other: its data directory or this one");
    text_add(&t, " is a copy; remove " CONSOLE_FILE " from the copy to give it its own");
    lan_log(&t);
}

// ---- frames ----

static void xnaddr_encode(u8 *wire, const u8 *xnaddr)
{
    put32(wire + 0, load32(xnaddr + XNADDR_INA));
    put32(wire + 4, load32(xnaddr + XNADDR_INA_ONLINE));
    put16(wire + 8, load16(xnaddr + XNADDR_PORT_ONLINE));
    copy(wire + 10, xnaddr + XNADDR_ENET, 6 + 20);
}

static void xnaddr_decode(u8 *xnaddr, const u8 *wire)
{
    store32(xnaddr + XNADDR_INA, get32(wire + 0));
    store32(xnaddr + XNADDR_INA_ONLINE, get32(wire + 4));
    store16(xnaddr + XNADDR_PORT_ONLINE, get16(wire + 8));
    copy(xnaddr + XNADDR_ENET, wire + 10, 6 + 20);
}

typedef struct lan_frame {
    u8 kind;
    u32 sequence;
    u16 source_port, destination_port;
    u8 xnaddr[PORT_LAN_XNADDR_SIZE], kid[PORT_LAN_XNKID_SIZE];
    const u8 *payload;
    u32 payload_size;
} lan_frame;

static u32 frame_encode(u8 *wire, u32 sequence, u8 kind, u16 source_port, u16 destination_port, const u8 *kid,
    const void *payload, u32 payload_size)
{
    if (payload_size > LAN_FRAME_LIMIT - LAN_HEADER_SIZE)
        return 0;
    put32(wire + 0, LAN_MAGIC);
    wire[4] = LAN_VERSION;
    wire[5] = kind;
    put16(wire + 6, 0);
    put32(wire + 8, sequence);
    put16(wire + 12, source_port);
    put16(wire + 14, destination_port);
    xnaddr_encode(wire + 16, g_lan.xnaddr);
    if (kid != 0)
        copy(wire + 52, kid, PORT_LAN_XNKID_SIZE);
    else
        zero(wire + 52, PORT_LAN_XNKID_SIZE);
    copy(wire + LAN_HEADER_SIZE, payload, payload_size);
    return LAN_HEADER_SIZE + payload_size;
}

// sequence: the copies of one broadcast share it (the receiver takes the first, sender_first_copy).
static int frame_send_numbered(u32 sequence, u8 kind, u16 source_port, u16 destination_port, const u8 *kid,
    const void *payload, u32 payload_size, u32 address, u16 port)
{
    u8 wire[LAN_FRAME_LIMIT];
    u32 size = frame_encode(wire, sequence, kind, source_port, destination_port, kid, payload, payload_size);
    return size != 0 ? lan_host_send(g_lan.host, wire, size, address, port) : LAN_HOST_FAILED;
}

static int frame_decode(const u8 *wire, u32 size, lan_frame *frame)
{
    if (size < LAN_HEADER_SIZE || size > LAN_FRAME_LIMIT || get32(wire) != LAN_MAGIC
        || wire[4] != LAN_VERSION || get16(wire + 6) != 0)
        return 0;
    frame->kind = wire[5];
    if (frame->kind < LAN_KIND_UDP || frame->kind > LAN_KIND_DISCONNECT
        || (frame->kind >= LAN_KIND_CONNECT && size != LAN_HEADER_SIZE))
        return 0;
    frame->sequence = get32(wire + 8);
    frame->source_port = get16(wire + 12);
    frame->destination_port = get16(wire + 14);
    if (frame->kind <= LAN_KIND_BROADCAST) {
        if (frame->source_port == 0 || frame->destination_port == 0)
            return 0;
    } else if (frame->source_port != 0 || frame->destination_port != 0)
        return 0;
    xnaddr_decode(frame->xnaddr, wire + 16);
    copy(frame->kid, wire + 52, PORT_LAN_XNKID_SIZE);
    frame->payload = wire + LAN_HEADER_SIZE;
    frame->payload_size = size - LAN_HEADER_SIZE;
    return 1;
}

// ---- broadcast destinations ----

static void target_add(u32 address, u16 port)
{
    for (int i = 0; i < g_lan.target_count; i++)
        if (g_lan.targets[i].address == address && g_lan.targets[i].port == port)
            return;
    if (g_lan.target_count < LAN_TARGET_LIMIT) {
        g_lan.targets[g_lan.target_count].address = address;
        g_lan.targets[g_lan.target_count].port = port;
        g_lan.target_count++;
    }
}

// Every console's port on the LAN: the slot ports at 255.255.255.255 and at each interface's broadcast address, and
// the HALO3_LAN_PEER addresses.
static void targets_refresh(u64 now)
{
    g_lan.interface_count = lan_host_interfaces(g_lan.interfaces, LAN_INTERFACE_LIMIT);
    g_lan.target_count = 0;
    for (u32 slot = 0; slot < g_lan.slots; slot++) {
        u16 port = (u16)(g_lan.base_port + slot);
        target_add(LAN_BROADCAST, port);
        for (int i = 0; i < g_lan.interface_count; i++)
            if (g_lan.interfaces[i].broadcast != 0 && g_lan.interfaces[i].broadcast != LAN_BROADCAST)
                target_add(g_lan.interfaces[i].broadcast, port);
    }
    for (int i = 0; i < g_lan.configured_peer_count; i++) {
        const port_lan_address *peer = &g_lan.configured_peers[i];
        if (peer->port != 0)
            target_add(peer->address, peer->port);
        else
            for (u32 slot = 0; slot < g_lan.slots; slot++)
                target_add(peer->address, (u16)(g_lan.base_port + slot));
    }
    g_lan.targets_refreshed_us = now;
}

// The first IPv4 this console is reached at (after targets_refresh).
static u32 local_address(void)
{
    if (g_lan.forced_address != 0)
        return g_lan.forced_address;
    return g_lan.interface_count > 0 ? g_lan.interfaces[0].address : LAN_LOOPBACK;
}

// Whether address is this PC or on one of its subnets: reached directly.
static int on_link(u32 address)
{
    if ((address >> 24) == 0x7f)
        return 1;
    for (int i = 0; i < g_lan.interface_count; i++) {
        u32 mask = g_lan.interfaces[i].netmask;
        if ((address & mask) == (g_lan.interfaces[i].address & mask))
            return 1;
    }
    return 0;
}

// ---- consoles seen on the LAN ----

static lan_endpoint *endpoint_find(const u8 *mac)
{
    for (int i = 0; i < LAN_ENDPOINT_LIMIT; i++) {
        lan_endpoint *e = &g_lan.endpoints[i];
        if (e->used && same(e->mac, mac, 6))
            return e;
    }
    return 0;
}

static lan_endpoint *endpoint_get(const u8 *mac, u64 now)
{
    lan_endpoint *e = endpoint_find(mac);
    if (e == 0) {
        e = &g_lan.endpoints[0];
        for (int i = 1; e->used && i < LAN_ENDPOINT_LIMIT; i++)
            if (!g_lan.endpoints[i].used || g_lan.endpoints[i].seen_us < e->seen_us)
                e = &g_lan.endpoints[i];
        zero(e, sizeof(*e));
        e->used = 1;
        copy(e->mac, mac, 6);
    }
    e->seen_us = now;
    return e;
}

// A broadcast can teach a route, but cannot confirm a remote unicast path.
static void endpoint_learn(const u8 *mac, u32 address, u16 port, u64 now, int unicast, int local)
{
    lan_endpoint *e = endpoint_get(mac, now);
    if (e->port != 0 && !unicast && !(local && !e->local))
        return;
    if (e->address != address || e->port != port) {
        if (e->local && !local)
            return;
        e->direct_seen_us = 0;
    }
    e->address = address;
    e->port = port;
    e->local |= (u8)local;
    if (unicast) {
        e->direct_seen_us = now;
        e->fallback_us = 0;
        e->pings_left = 0;
    }
}

static int direct_fresh(const lan_endpoint *e, u64 now)
{
    return e != 0 && e->direct_seen_us != 0 && now - e->direct_seen_us < RELAY_DIRECT_LIFETIME_US;
}

static int unicast_ipv4(u32 address)
{
    return address >= 0x01000000u && address < 0xe0000000u;
}

static port_lan_address endpoint_of(const u8 *xnaddr, const lan_endpoint *e, u64 now)
{
    if (e != 0) {
        if (e->port != 0 && (e->local || direct_fresh(e, now) || e->public_port == 0))
            return (port_lan_address){ e->address, e->port };
        if (e->public_port != 0)
            return (port_lan_address){ e->public_address, e->public_port };
    }
    u32 lan_ip = load32(xnaddr + XNADDR_INA);
    u32 online_ip = load32(xnaddr + XNADDR_INA_ONLINE);
    u16 port = load16(xnaddr + XNADDR_PORT_ONLINE);
    if (unicast_ipv4(online_ip) && online_ip != lan_ip && port != 0) {
        // A matching public IP makes the advertised LAN IP worth trying. LAN discovery supplies the actual port
        // when a peer uses a different slot or its router changed the advertised port.
        if (g_lan.relay.ready && online_ip == load32(g_lan.xnaddr + XNADDR_INA_ONLINE) && on_link(lan_ip))
            return (port_lan_address){ lan_ip, port };
        return (port_lan_address){ online_ip, port };
    }
    return (port_lan_address){ lan_ip, port != 0 ? port : g_lan.base_port };
}

// ---- Live relay ----

static void relay_header(u8 *wire, u8 type)
{
    put32(wire, RELAY_MAGIC);
    wire[4] = RELAY_VERSION;
    wire[5] = type;
    copy(wire + 6, g_lan.mac, 6);
}

static int relay_send(const u8 *wire, u32 size)
{
    return lan_host_send(g_lan.host, wire, size, g_lan.relay.address, g_lan.relay.port);
}

static void relay_register(u64 now)
{
    lan_relay *relay = &g_lan.relay;
    u8 wire[30 + RELAY_TOKEN_LIMIT];
    relay_header(wire, RELAY_REGISTER);
    put64(wire + 12, relay->xuid);
    port_host_random(relay->nonce, sizeof(relay->nonce));
    copy(wire + 20, relay->nonce, sizeof(relay->nonce));
    put16(wire + 28, relay->token_size);
    copy(wire + 30, relay->token, relay->token_size);
    relay->awaiting_ack = relay_send(wire, 30 + relay->token_size) == LAN_HOST_OK;
    relay->next_register_us = now + RELAY_REGISTER_INTERVAL_US;
}

static void relay_punch_request(lan_endpoint *e, u64 now)
{
    if (now < e->next_punch_us)
        return;
    u8 wire[RELAY_DATA_HEADER_SIZE];
    relay_header(wire, RELAY_PUNCH_REQUEST);
    copy(wire + RELAY_PEER_HEADER_SIZE, e->mac, 6);
    relay_send(wire, sizeof(wire));
    e->next_punch_us = now + RELAY_PUNCH_INTERVAL_US;
}

static void relay_punch_direct(u8 type, u32 address, u16 port)
{
    u8 wire[RELAY_PEER_HEADER_SIZE];
    relay_header(wire, type);
    lan_host_send(g_lan.host, wire, sizeof(wire), address, port);
}

static int relay_data_send(u8 *wire, u32 size, const u8 *mac)
{
    relay_header(wire, RELAY_DATA);
    copy(wire + RELAY_PEER_HEADER_SIZE, mac, 6);
    return relay_send(wire, RELAY_DATA_HEADER_SIZE + size);
}

static int frame_send_peer(u8 kind, u16 source_port, u16 destination_port, const u8 *kid, const void *payload,
    u32 payload_size, const u8 *xnaddr, u64 now)
{
    u8 wire[RELAY_DATA_HEADER_SIZE + LAN_FRAME_LIMIT];
    u8 *inner = wire + RELAY_DATA_HEADER_SIZE;
    u32 size = frame_encode(inner, ++g_lan.sequence, kind, source_port, destination_port, kid, payload, payload_size);
    if (size == 0)
        return LAN_HOST_FAILED;
    const u8 *mac = xnaddr_mac(xnaddr);
    lan_endpoint *e = endpoint_find(mac);
    port_lan_address to = endpoint_of(xnaddr, e, now);
    int local = e != 0 && e->local;
    int can_relay = g_lan.relay.ready && !local;
    if (g_lan.relay.port != 0 && !local && !direct_fresh(e, now)) {
        e = endpoint_get(mac, now);
        if (e->fallback_us == 0)
            e->fallback_us = now + RELAY_FALLBACK_US;
        if (can_relay) {
            relay_punch_request(e, now);
            if (now >= e->fallback_us)
                return relay_data_send(wire, size, mac);
        }
    }
    int result = unicast_ipv4(to.address) ? lan_host_send(g_lan.host, inner, size, to.address, to.port)
                                         : LAN_HOST_UNREACHABLE;
    if (result != LAN_HOST_OK && can_relay) {
        e = endpoint_get(mac, now);
        e->direct_seen_us = 0;
        e->fallback_us = now;
        relay_punch_request(e, now);
        return relay_data_send(wire, size, mac);
    }
    return result;
}

// A relayed request needs its reply on the same path, including the first QoS probe or connection request.
static void frame_reply(u8 kind, const u8 *kid, const void *payload, u32 payload_size, u32 address, u16 port,
    const u8 *relay_mac)
{
    if (relay_mac == 0) {
        frame_send_numbered(++g_lan.sequence, kind, 0, 0, kid, payload, payload_size, address, port);
        return;
    }
    u8 wire[RELAY_DATA_HEADER_SIZE + LAN_FRAME_LIMIT];
    u32 size = frame_encode(wire + RELAY_DATA_HEADER_SIZE, ++g_lan.sequence, kind, 0, 0, kid, payload, payload_size);
    if (size != 0)
        relay_data_send(wire, size, relay_mac);
}

// Whether this broadcast from mac is new (and remembers it).
static int sender_first_copy(const u8 *mac, u32 sequence, u64 now)
{
    lan_sender *slot = 0;
    for (int i = 0; i < LAN_SENDER_LIMIT; i++) {
        lan_sender *s = &g_lan.senders[i];
        if (s->used && same(s->mac, mac, 6)) {
            slot = s;
            break;
        }
        if (slot == 0 || (slot->used && (!s->used || s->seen_us < slot->seen_us)))
            slot = s;
    }
    if (!slot->used || !same(slot->mac, mac, 6)) {
        slot->used = 1;
        copy(slot->mac, mac, 6);
        slot->newest = sequence;
        slot->window = 1;
        slot->seen_us = now;
        return 1;
    }
    slot->seen_us = now;
    int ahead = (int)(sequence - slot->newest);
    if (ahead > 0 && ahead < 64) {
        slot->window = (slot->window << ahead) | 1;
        slot->newest = sequence;
        return 1;
    }
    if (ahead <= 0 && ahead > -64) {
        u64 bit = 1ull << (u32)-ahead;
        if ((slot->window & bit) != 0)
            return 0;
        slot->window |= bit;
        return 1;
    }
    // far outside the window (a sender's broadcast numbers advance by one, so this is a burst of lost broadcasts, or
    // a forged frame): delivered, and the window starts over here, so no frame can hold the sender's later ones back
    slot->newest = sequence;
    slot->window = 1;
    return 1;
}

// ---- keys and peers ----

static lan_key *key_find(const u8 *kid)
{
    for (int i = 0; i < LAN_KEY_LIMIT; i++)
        if (g_lan.keys[i].used && same(g_lan.keys[i].kid, kid, PORT_LAN_XNKID_SIZE))
            return &g_lan.keys[i];
    return 0;
}

static lan_listener *listener_find(const u8 *kid)
{
    for (int i = 0; i < LAN_KEY_LIMIT; i++)
        if (g_lan.listeners[i].used && same(g_lan.listeners[i].kid, kid, PORT_LAN_XNKID_SIZE))
            return &g_lan.listeners[i];
    return 0;
}

static lan_peer *peer_of_secure(u32 secure)
{
    if (secure == 0)
        return 0;
    for (int i = 0; i < LAN_PEER_LIMIT; i++)
        if (g_lan.peers[i].secure == secure)
            return &g_lan.peers[i];
    return 0;
}

// Peers are one per console (MAC) and key, as the Xbox's security associations.
static lan_peer *peer_find(const u8 *mac, const u8 *kid)
{
    for (int i = 0; i < LAN_PEER_LIMIT; i++) {
        lan_peer *p = &g_lan.peers[i];
        if (p->secure != 0 && same(xnaddr_mac(p->xnaddr), mac, 6) && same(p->kid, kid, PORT_LAN_XNKID_SIZE))
            return p;
    }
    return 0;
}

static u32 peer_count(void)
{
    u32 count = 0;
    for (int i = 0; i < LAN_PEER_LIMIT; i++)
        count += g_lan.peers[i].secure != 0;
    return count;
}

// HALO3_LAN_LOG: what happens to the peers.
static void log_peer(const lan_peer *peer, const char *what)
{
    if (!g_lan.log)
        return;
    lan_text t = { { 0 }, 0 };
    text_add(&t, "system link: peer ");
    text_endpoint(&t, peer->secure, 0);
    text_add(&t, " (mac ");
    text_mac(&t, xnaddr_mac(peer->xnaddr));
    text_add(&t, " at ");
    port_lan_address to = endpoint_of(peer->xnaddr, endpoint_find(xnaddr_mac(peer->xnaddr)), now_us());
    text_endpoint(&t, to.address, to.port);
    text_add(&t, ") ");
    text_add(&t, what);
    lan_log(&t);
}


// HALO3_LAN_TRACE: every title datagram exchanged with a peer (direction, UDP/VDP, title ports, the peer's secure
// address, size, when in microseconds of lan_host_microseconds, the first 40 bytes): sent, received by the System Link
// thread, taken by the title (recvfrom); and the frames dropped (no association, malformed): the game's own protocol
// (out-of-band messages, channel packets) laid bare when a session misbehaves, and the time a datagram takes from one
// console's sendto to another's recvfrom (the clock is the system's monotonic one, the same in every process on a PC).
// Broadcasts are not traced.
static void trace_datagram(const char *what, int vdp, u16 source_port, u16 destination_port, u32 peer, const u8 *data,
    u32 size, int error)
{
    if (!g_lan.trace)
        return;
    lan_text t = { { 0 }, 0 };
    text_add(&t, "system link: ");
    text_add(&t, what);
    text_add(&t, vdp ? " vdp " : " udp ");
    text_number(&t, source_port);
    text_add(&t, "->");
    text_number(&t, destination_port);
    text_add(&t, " peer ");
    text_endpoint(&t, peer, 0);
    text_add(&t, " size ");
    text_number(&t, size);
    if (error != 0) {
        text_add(&t, " error ");
        text_number(&t, (u32)error);
    }
    text_add(&t, " at ");
    text_u64(&t, now_us());
    text_add(&t, ": ");
    for (u32 i = 0; i < size && i < 40; i++)
        text_hex8(&t, data[i]);
    lan_log(&t);
}

static void peer_set_status(lan_peer *peer, u8 status)
{
    static const char *const k_status[] = { "idle", "connecting", "connected", "lost" };
    if (peer->status != status) {
        const int asked = peer->status == LAN_CONNECT_STATUS_PENDING; // by this console: peer_connect
        peer->status = status;
        if (status == LAN_CONNECT_STATUS_CONNECTED && asked && peer->connect_us != 0 && g_lan.log) {
            lan_text t = { { 0 }, 0 };
            text_add(&t, "connected (handshake ");
            text_ms(&t, now_us() - peer->connect_us);
            text_add(&t, " ms)");
            log_peer(peer, t.text);
        } else
            log_peer(peer, k_status[status]);
    }
}

// The secure address of the console xnaddr names, for the key kid: this console's own is 127.0.0.1. A new peer is
// mapped to the next free handle 0.h.l.0: the Xbox's secure addresses have a zero first octet (0.0.0.0/8), and the
// game tests that byte as in_addr.bytes[0], the low byte on the PC (transport_address_get_insecure_xbox_address).
static int peer_map(const u8 *xnaddr, const u8 *kid, u32 *secure, lan_peer **mapped)
{
    *secure = 0;
    if (mapped != 0)
        *mapped = 0;
    if (own_mac(xnaddr_mac(xnaddr))) {
        *secure = LAN_LOOPBACK;
        return 0;
    }
    u32 ina = load32(xnaddr + XNADDR_INA);
    if (key_find(kid) == 0 || all_zero(xnaddr_mac(xnaddr), 6) || ina == 0 || ina == LAN_BROADCAST)
        return PORT_LAN_WSAEINVAL;
    lan_peer *peer = peer_find(xnaddr_mac(xnaddr), kid);
    if (peer == 0) {
        if (peer_count() >= g_lan.peer_limit)
            return PORT_LAN_WSAENOBUFS;
        for (int i = 0; i < LAN_PEER_LIMIT && peer == 0; i++)
            if (g_lan.peers[i].secure == 0)
                peer = &g_lan.peers[i];
        u32 handle;
        do {
            g_lan.next_secure = g_lan.next_secure % 0xffff + 1;
            handle = g_lan.next_secure << 8;
        } while (peer_of_secure(handle) != 0);
        zero(peer, sizeof(*peer));
        peer->secure = handle;
        copy(peer->xnaddr, xnaddr, PORT_LAN_XNADDR_SIZE);
        copy(peer->kid, kid, PORT_LAN_XNKID_SIZE);
        peer->status = LAN_CONNECT_STATUS_IDLE;
        log_peer(peer, "mapped");
    }
    *secure = peer->secure;
    if (mapped != 0)
        *mapped = peer;
    return 0;
}

// A timer due at when was armed off the System Link thread: if the thread sleeps past it, a datagram to this
// console's own port (too short to be a frame) wakes it to run its timers.
static void timer_armed(u64 when)
{
    if (g_lan.running && when < g_lan.thread_deadline_us) {
        g_lan.thread_deadline_us = when;
        static const u8 k_wake = 0;
        lan_host_send(g_lan.host, &k_wake, 1, LAN_LOOPBACK, g_lan.host_port);
    }
}

static void inbox_retire(u32 source);

// Ends the association with the peer and forgets it.
static void peer_drop(lan_peer *peer)
{
    if (peer->status == LAN_CONNECT_STATUS_PENDING || peer->status == LAN_CONNECT_STATUS_CONNECTED)
        frame_send_peer(LAN_KIND_DISCONNECT, 0, 0, peer->kid, 0, 0, peer->xnaddr, now_us());
    inbox_retire(peer->secure);
    peer->secure = 0;
}

static void peer_connect(lan_peer *peer, u64 now)
{
    peer->connect_us = now;
    peer_set_status(peer, LAN_CONNECT_STATUS_PENDING);
    peer->tries = 0;
    peer->next_us = now + LAN_CONNECT_RETRY_US;
    frame_send_peer(LAN_KIND_CONNECT, 0, 0, peer->kid, 0, 0, peer->xnaddr, now);
    timer_armed(peer->next_us);
}

// ---- the title's sockets ----

static lan_socket *socket_of(u32 handle)
{
    u32 index = (handle & 0xff) - 1;
    if (index >= LAN_SOCKET_LIMIT || g_lan.sockets[index].handle != handle || handle == 0)
        return 0;
    return &g_lan.sockets[index];
}

static int port_taken(u16 port, int vdp)
{
    for (int i = 0; i < LAN_SOCKET_LIMIT; i++) {
        const lan_socket *s = &g_lan.sockets[i];
        if (s->handle != 0 && s->port == port && s->vdp == vdp)
            return 1;
    }
    return 0;
}

static int socket_bind_port(lan_socket *s, u16 port)
{
    if (port == 0) {
        for (u32 tries = 0; tries < 0x10000 - LAN_FIRST_DYNAMIC_PORT; tries++) {
            u16 candidate = g_lan.next_dynamic_port;
            g_lan.next_dynamic_port = candidate == 0xffff ? LAN_FIRST_DYNAMIC_PORT : (u16)(candidate + 1);
            if (!port_taken(candidate, s->vdp)) {
                s->port = candidate;
                return 0;
            }
        }
        return PORT_LAN_WSAENOBUFS;
    }
    if (port_taken(port, s->vdp))
        return PORT_LAN_WSAEADDRINUSE;
    s->port = port;
    return 0;
}

static void inbox_drop_head(lan_socket *s)
{
    s->bytes -= s->inbox[s->head].size;
    s->head = (s->head + 1) % LAN_INBOX_SLOTS;
    s->count--;
}

// A VDP datagram starts with the size of its game data (the title's WORD, in its own byte order), voice follows.
static int vdp_valid(const u8 *data, u32 size)
{
    return size >= 2 && load16(data) <= size - 2;
}

// Hands a datagram to the sockets bound to port: of its kind, or both kinds for a broadcast.
static void deliver(u16 port, int kind, u32 source, u16 source_port, const u8 *data, u32 size)
{
    if (size > PORT_LAN_MAX_DATAGRAM)
        return;
    int delivered = 0;
    for (int i = 0; i < LAN_SOCKET_LIMIT; i++) {
        lan_socket *s = &g_lan.sockets[i];
        if (s->handle == 0 || s->port != port)
            continue;
        if (kind == LAN_KIND_BROADCAST) {
            if (s->vdp && !vdp_valid(data, size))
                continue;
        } else if (s->vdp != (kind == LAN_KIND_VDP))
            continue;
        if (s->connected && (s->remote.address != source || s->remote.port != source_port))
            continue;
        // the receive buffer (SO_RCVBUF, XNetStartupParams' default) holds at least one datagram
        if (s->count >= LAN_INBOX_SLOTS || (s->count != 0 && s->bytes + size > s->budget)) {
            s->dropped++;
            if (g_lan.log && (s->dropped & (s->dropped - 1)) == 0) {
                lan_text t = { { 0 }, 0 };
                text_add(&t, "system link: title port ");
                text_number(&t, s->port);
                text_add(&t, s->vdp ? " (VDP)" : " (UDP)");
                text_add(&t, ": receive buffer full, ");
                text_number(&t, s->dropped);
                text_add(&t, " datagrams dropped");
                lan_log(&t);
            }
            continue;
        }
        lan_datagram *d = &s->inbox[(s->head + s->count) % LAN_INBOX_SLOTS];
        d->source = source;
        d->source_port = source_port;
        d->size = (u16)size;
        copy(d->data, data, size);
        s->count++;
        s->bytes += size;
        delivered = 1;
    }
    if (delivered)
        touch();
}

// Drops what the sockets hold from a peer that is gone.
static void inbox_retire(u32 source)
{
    for (int i = 0; i < LAN_SOCKET_LIMIT; i++) {
        lan_socket *s = &g_lan.sockets[i];
        if (s->handle == 0 || s->count == 0)
            continue;
        u32 kept = 0, bytes = 0;
        for (u32 n = 0; n < s->count; n++) {
            const lan_datagram *d = &s->inbox[(s->head + n) % LAN_INBOX_SLOTS];
            if (d->source == source)
                continue;
            lan_datagram *to = &s->inbox[(s->head + kept) % LAN_INBOX_SLOTS];
            if (to != d)
                copy(to, d, sizeof(*d));
            kept++;
            bytes += to->size;
        }
        s->count = kept;
        s->bytes = bytes;
    }
}

// ---- QoS ----

static u32 rtt_ms(u32 microseconds) { return microseconds == 0 ? 0 : (microseconds + 999) / 1000; }

// HALO3_LAN_LOG: a lookup all of whose targets are done.
static void qos_log_done(const lan_qos_lookup *lookup)
{
    const lan_xnqos *qos = (const lan_xnqos *)lookup->memory;
    u32 contacted = 0, best = 0;
    for (u32 i = 0; i < lookup->count; i++) {
        if ((qos->info[i].flags & LAN_XNQOSINFO_TARGET_CONTACTED) == 0)
            continue;
        if (contacted++ == 0 || qos->info[i].rtt_minimum_ms < best)
            best = qos->info[i].rtt_minimum_ms;
    }
    lan_text t = { { 0 }, 0 };
    text_add(&t, "system link: QoS lookup of ");
    text_number(&t, lookup->count);
    text_add(&t, lookup->count == 1 ? " console done in " : " consoles done in ");
    text_ms(&t, now_us() - lookup->started_us);
    text_add(&t, " ms (");
    text_number(&t, contacted);
    text_add(&t, " contacted, best round trip ");
    text_number(&t, best);
    text_add(&t, " ms)");
    lan_log(&t);
}

static void qos_finish(lan_qos_lookup *lookup, u32 index)
{
    lan_qos_target *target = &lookup->targets[index];
    if (target->done)
        return;
    target->done = 1;
    lan_xnqos *qos = (lan_xnqos *)lookup->memory;
    lan_xnqosinfo *info = &qos->info[index];
    u8 flags = LAN_XNQOSINFO_COMPLETE;
    info->probes_sent = target->probes;
    if (target->contacted) {
        flags |= LAN_XNQOSINFO_TARGET_CONTACTED;
        if (target->disabled)
            flags |= LAN_XNQOSINFO_TARGET_DISABLED;
        info->probes_received = target->answered;
        // the minimum and the median of the round trips measured
        u32 samples[LAN_QOS_PROBE_LIMIT], n = 0;
        for (u32 i = 0; i < target->probes; i++)
            if (target->answered_mask & (1u << i))
                samples[n++] = target->rtt_us[i];
        for (u32 i = 1; i < n; i++)
            for (u32 j = i; j > 0 && samples[j - 1] > samples[j]; j--) {
                u32 t = samples[j];
                samples[j] = samples[j - 1];
                samples[j - 1] = t;
            }
        if (n != 0) {
            info->rtt_minimum_ms = (u16)rtt_ms(samples[0]);
            info->rtt_median_ms = (u16)rtt_ms(samples[n / 2]);
        }
        info->up_bits_per_second = LAN_LINK_BITS_PER_SECOND;
        info->down_bits_per_second = LAN_LINK_BITS_PER_SECOND;
        if (target->have_data) {
            info->data = lookup->data + index * LAN_QOS_DATA_LIMIT;
            info->data_size = target->data_size;
            flags |= LAN_XNQOSINFO_DATA_RECEIVED;
        }
    }
    // the title polls cxnqosPending: it drops once the entry is written
    __atomic_store_n(&info->flags, flags, __ATOMIC_RELEASE);
    lookup->pending--;
    __atomic_store_n((u32 *)(lookup->memory + 4), lookup->pending, __ATOMIC_RELEASE); // XNQOS.cxnqosPending
    if (lookup->pending == 0 && g_lan.log)
        qos_log_done(lookup);
    if (lookup->event != 0)
        SetEvent(lookup->event);
}

static void qos_probe_send(lan_qos_lookup *lookup, u32 index, u32 probe, u64 now)
{
    lan_qos_target *target = &lookup->targets[index];
    u8 payload[LAN_QOS_PROBE_SIZE];
    put32(payload + 0, lookup->id);
    put16(payload + 4, (u16)index);
    put16(payload + 6, (u16)probe);
    put64(payload + 8, now);
    payload[16] = target->have_data ? 0 : 1; // the first probes ask for the data too
    copy(payload + 17, xnaddr_mac(target->xnaddr), 6);
    frame_send_peer(LAN_KIND_QOS_PROBE, 0, 0, target->kid, payload, sizeof(payload), target->xnaddr, now);
}

// Sends each probe not yet answered, a few milliseconds apart, then waits; an attempt that leaves probes unanswered
// is repeated (XNetStartupParams.cfgQosProbeRetries times), then the target is done.
static void qos_tick(lan_qos_lookup *lookup, u32 index, u64 now)
{
    lan_qos_target *target = &lookup->targets[index];
    if (target->done || now < target->next_us)
        return;
    if (!target->waiting) {
        while (target->cursor < target->probes && (target->answered_mask & (1u << target->cursor)) != 0)
            target->cursor++;
        if (target->cursor < target->probes) {
            qos_probe_send(lookup, index, target->cursor, now);
            target->cursor++;
            target->next_us = now + LAN_QOS_PROBE_SPACING_US;
            return;
        }
        target->waiting = 1;
        target->next_us = now + LAN_QOS_ATTEMPT_US;
        // Even with one retry configured, give the relay a chance after the direct attempt expires.
        const lan_endpoint *e = endpoint_find(xnaddr_mac(target->xnaddr));
        if (g_lan.relay.ready && target->attempt + 1u == g_lan.qos_retries && e != 0 && !e->local
            && target->next_us < e->fallback_us)
            target->next_us = e->fallback_us;
        return;
    }
    if (target->attempt < g_lan.qos_retries) {
        target->attempt++;
        target->waiting = 0;
        target->cursor = 0;
        target->next_us = now;
        qos_tick(lookup, index, now);
        return;
    }
    qos_finish(lookup, index);
}

static void frame_learn(const lan_frame *frame, u32 address, u16 port, u64 now, int unicast)
{
    if (port != 0) {
        int local = on_link(address) && (!unicast || address == load32(frame->xnaddr + XNADDR_INA));
        endpoint_learn(xnaddr_mac(frame->xnaddr), address, port, now, unicast, local);
    }
}

static void qos_probed(const lan_frame *frame, u32 address, u16 port, u64 now, const u8 *relay_mac)
{
    const u8 *p = frame->payload;
    if (frame->payload_size != LAN_QOS_PROBE_SIZE || get16(p + 4) >= LAN_QOS_TARGET_LIMIT
        || get16(p + 6) >= LAN_QOS_PROBE_LIMIT || (p[16] & ~1u) != 0 || !own_mac(p + 17))
        return;
    lan_listener *listener = listener_find(frame->kid);
    if (listener == 0)
        return; // not listening for this key: the probe goes unanswered
    frame_learn(frame, address, port, now, 1);
    int data_request = p[16] & 1;
    listener->stats[1]++;
    if (data_request)
        listener->stats[0]++;
    u8 reply[LAN_QOS_REPLY_HEADER_SIZE + LAN_QOS_DATA_LIMIT];
    copy(reply, p, 16); // lookup, target, probe, timestamp
    u8 flags = 0;
    u16 size = 0;
    if (!listener->enabled)
        flags |= 2;
    else if (data_request && listener->size != 0) {
        flags |= 1;
        size = listener->size;
        copy(reply + LAN_QOS_REPLY_HEADER_SIZE, listener->data, size);
        listener->stats[3]++;
        listener->stats[4] += size;
    }
    reply[16] = flags;
    put16(reply + 17, size);
    listener->stats[5]++;
    frame_reply(LAN_KIND_QOS_REPLY, frame->kid, reply, LAN_QOS_REPLY_HEADER_SIZE + size, address, port, relay_mac);
}

static void qos_replied(const lan_frame *frame, u32 address, u16 port, u64 now)
{
    if (frame->payload_size < LAN_QOS_REPLY_HEADER_SIZE)
        return;
    const u8 *p = frame->payload;
    u32 id = get32(p), index = get16(p + 4), probe = get16(p + 6);
    u64 sent = get64(p + 8);
    u8 flags = p[16];
    u32 size = get16(p + 17);
    if (size > LAN_QOS_DATA_LIMIT || frame->payload_size != LAN_QOS_REPLY_HEADER_SIZE + size
        || probe >= LAN_QOS_PROBE_LIMIT || (flags & ~3u) != 0 || flags == 3
        || ((flags & 1u) != 0) != (size != 0))
        return;
    for (int i = 0; i < LAN_QOS_LOOKUP_LIMIT; i++) {
        lan_qos_lookup *lookup = &g_lan.lookups[i];
        if (lookup->memory == 0 || lookup->id != id || index >= lookup->count)
            continue;
        lan_qos_target *target = &lookup->targets[index];
        if (target->done || probe >= target->probes || (target->answered_mask & (1u << probe)) != 0
            || !same(xnaddr_mac(target->xnaddr), xnaddr_mac(frame->xnaddr), 6)
            || !same(target->kid, frame->kid, PORT_LAN_XNKID_SIZE))
            return;
        frame_learn(frame, address, port, now, 1);
        target->answered_mask |= 1u << probe;
        target->answered++;
        target->rtt_us[probe] = now > sent ? (u32)(now - sent) : 1;
        target->contacted = 1;
        target->disabled = (flags & 2) != 0;
        if ((flags & 1) != 0 && !target->have_data) {
            copy(lookup->data + index * LAN_QOS_DATA_LIMIT, p + LAN_QOS_REPLY_HEADER_SIZE, size);
            target->data_size = (u16)size;
            target->have_data = 1;
        }
        if (target->answered == target->probes)
            qos_finish(lookup, index);
        return;
    }
}

// The answer this console's own listener gives (a target that is this console).
static void qos_local(lan_qos_lookup *lookup, u32 index)
{
    lan_qos_target *target = &lookup->targets[index];
    const lan_listener *listener = listener_find(target->kid);
    if (listener != 0) {
        target->contacted = 1;
        target->answered = target->probes;
        for (u32 i = 0; i < target->probes; i++)
            target->answered_mask |= 1u << i;
        target->disabled = !listener->enabled;
        if (listener->enabled && listener->size != 0) {
            copy(lookup->data + index * LAN_QOS_DATA_LIMIT, listener->data, listener->size);
            target->data_size = listener->size;
            target->have_data = 1;
        }
    }
    qos_finish(lookup, index);
}

// ---- the System Link thread ----

// A relayed frame has a MAC for routing the reply and no direct source endpoint to learn.
static void receive_frame(const u8 *wire, u32 size, u32 address, u16 port, u64 now, const u8 *relay_mac)
{
    lan_frame frame;
    if (!frame_decode(wire, size, &frame) || !console_mac_valid(xnaddr_mac(frame.xnaddr)))
        return;
    const u8 *mac = xnaddr_mac(frame.xnaddr);
    if (relay_mac != 0 && !same(mac, relay_mac, 6))
        return;
    if (own_mac(mac)) {
        identity_check_duplicate(frame.xnaddr, port);
        return;
    }
    lan_peer *peer = 0;
    u32 secure;
    switch (frame.kind) {
    case LAN_KIND_BROADCAST:
        if (frame.payload_size > PORT_LAN_MAX_DATAGRAM || !all_zero(frame.kid, PORT_LAN_XNKID_SIZE))
            break;
        frame_learn(&frame, address, port, now, 0);
        if (sender_first_copy(mac, frame.sequence, now))
            deliver(frame.destination_port, LAN_KIND_BROADCAST, LAN_DISCOVERY_SOURCE, frame.source_port,
                frame.payload, frame.payload_size);
        break;
    case LAN_KIND_UDP:
    case LAN_KIND_VDP:
        // a peer that reaches this console with a key it registered is mapped (the host learns its clients so)
        if (frame.payload_size > PORT_LAN_MAX_DATAGRAM
            || (frame.kind == LAN_KIND_VDP && !vdp_valid(frame.payload, frame.payload_size))
            || peer_map(frame.xnaddr, frame.kid, &secure, &peer) != 0 || peer == 0) {
            trace_datagram("dropped", frame.kind == LAN_KIND_VDP, frame.source_port, frame.destination_port, 0,
                frame.payload, frame.payload_size, 0);
            break;
        }
        frame_learn(&frame, address, port, now, 1);
        peer_set_status(peer, LAN_CONNECT_STATUS_CONNECTED);
        trace_datagram("received", frame.kind == LAN_KIND_VDP, frame.source_port, frame.destination_port, secure,
            frame.payload, frame.payload_size, 0);
        deliver(frame.destination_port, frame.kind, secure, frame.source_port, frame.payload, frame.payload_size);
        break;
    case LAN_KIND_CONNECT:
        if (peer_map(frame.xnaddr, frame.kid, &secure, &peer) != 0 || peer == 0)
            break; // no such key here: no association
        frame_learn(&frame, address, port, now, 1);
        peer_set_status(peer, LAN_CONNECT_STATUS_CONNECTED);
        frame_reply(LAN_KIND_ACCEPT, frame.kid, 0, 0, address, port, relay_mac);
        break;
    case LAN_KIND_ACCEPT:
    case LAN_KIND_DISCONNECT:
        peer = peer_find(mac, frame.kid);
        if (peer == 0)
            break;
        frame_learn(&frame, address, port, now, 1);
        peer_set_status(peer, frame.kind == LAN_KIND_ACCEPT ? LAN_CONNECT_STATUS_CONNECTED : LAN_CONNECT_STATUS_LOST);
        break;
    case LAN_KIND_QOS_PROBE:
        qos_probed(&frame, address, port, now, relay_mac);
        break;
    case LAN_KIND_QOS_REPLY:
        qos_replied(&frame, address, port, now);
        break;
    }
}

static void receive_datagram(const u8 *wire, u32 size, u32 address, u16 port, u64 now)
{
    if (size < 4 || port == 0 || !unicast_ipv4(address))
        return;
    lan_relay *relay = &g_lan.relay;
    int from_relay = relay->port != 0 && address == relay->address && port == relay->port;
    if (get32(wire) != RELAY_MAGIC) {
        if (!from_relay)
            receive_frame(wire, size, address, port, now, 0);
        return;
    }
    if (relay->port == 0 || size < 6 || size > RELAY_FRAME_LIMIT || wire[4] != RELAY_VERSION)
        return;
    const u8 *mac = wire + 6;
    if (wire[5] == RELAY_REGISTER_ACK) {
        if (!from_relay || size != 20 || !relay->awaiting_ack || !same(wire + 12, relay->nonce, 8))
            return;
        u32 observed_ip = get32(wire + 6);
        u16 observed_port = get16(wire + 10);
        if (!unicast_ipv4(observed_ip) || observed_port == 0)
            return;
        store32(g_lan.xnaddr + XNADDR_INA_ONLINE, observed_ip);
        store16(g_lan.xnaddr + XNADDR_PORT_ONLINE, observed_port);
        copy(g_lan.xnaddr + XNADDR_ONLINE, g_lan.mac, 6);
        relay->ready = 1;
        relay->awaiting_ack = 0;
        touch();
        return;
    }
    if (size < RELAY_PEER_HEADER_SIZE || !console_mac_valid(mac) || own_mac(mac))
        return;
    switch (wire[5]) {
    case RELAY_PUNCH_PEER: {
        if (!from_relay || size != 18)
            break;
        u32 peer_ip = get32(wire + 12);
        u16 peer_port = get16(wire + 16);
        if (!unicast_ipv4(peer_ip) || peer_port == 0)
            break;
        lan_endpoint *e = endpoint_get(mac, now);
        e->public_address = peer_ip;
        e->public_port = peer_port;
        relay_punch_direct(RELAY_PUNCH_PING, peer_ip, peer_port);
        e->pings_left = 3;
        e->next_ping_us = now + RELAY_PING_INTERVAL_US;
        break;
    }
    case RELAY_PUNCH_PING:
    case RELAY_PUNCH_PONG:
        if (!from_relay && size == RELAY_PEER_HEADER_SIZE) {
            endpoint_learn(mac, address, port, now, 1, 0);
            if (wire[5] == RELAY_PUNCH_PING)
                relay_punch_direct(RELAY_PUNCH_PONG, address, port);
        }
        break;
    case RELAY_DATA:
        if (from_relay && size > RELAY_DATA_HEADER_SIZE && own_mac(wire + 12))
            receive_frame(wire + RELAY_DATA_HEADER_SIZE, size - RELAY_DATA_HEADER_SIZE, 0, 0, now, mac);
        break;
    }
}

static void relay_tick(u64 now, u64 *next)
{
    if (g_lan.relay.port == 0)
        return;
    if (now >= g_lan.relay.next_register_us)
        relay_register(now);
    if (g_lan.relay.next_register_us < *next)
        *next = g_lan.relay.next_register_us;
    for (int i = 0; i < LAN_ENDPOINT_LIMIT; i++) {
        lan_endpoint *e = &g_lan.endpoints[i];
        if (e->pings_left != 0 && now >= e->next_ping_us) {
            relay_punch_direct(RELAY_PUNCH_PING, e->public_address, e->public_port);
            e->pings_left--;
            e->next_ping_us = now + RELAY_PING_INTERVAL_US;
        }
        if (e->pings_left != 0 && e->next_ping_us < *next)
            *next = e->next_ping_us;
    }
}

// Runs what is due; returns the milliseconds until the next timer.
static u32 run_timers(u64 now)
{
    u64 next = now + LAN_THREAD_TICK_MS * 1000ull;
    relay_tick(now, &next);
    for (int i = 0; i < LAN_PEER_LIMIT; i++) {
        lan_peer *p = &g_lan.peers[i];
        if (p->secure == 0 || p->status != LAN_CONNECT_STATUS_PENDING)
            continue;
        if (now >= p->next_us) {
            if (++p->tries >= LAN_CONNECT_TRIES) {
                peer_set_status(p, LAN_CONNECT_STATUS_LOST);
                continue;
            }
            frame_send_peer(LAN_KIND_CONNECT, 0, 0, p->kid, 0, 0, p->xnaddr, now);
            p->next_us = now + LAN_CONNECT_RETRY_US;
        }
        if (p->next_us < next)
            next = p->next_us;
    }
    for (int i = 0; i < LAN_QOS_LOOKUP_LIMIT; i++) {
        lan_qos_lookup *lookup = &g_lan.lookups[i];
        if (lookup->memory == 0 || lookup->pending == 0)
            continue;
        for (u32 t = 0; t < lookup->count; t++) {
            qos_tick(lookup, t, now);
            if (!lookup->targets[t].done && lookup->targets[t].next_us < next)
                next = lookup->targets[t].next_us;
        }
    }
    u64 wait = next > now ? next - now : 0;
    return (u32)((wait + 999) / 1000);
}

static void lan_thread(void *unused)
{
    (void)unused;
    u8 wire[RELAY_FRAME_LIMIT + 1]; // a truncated host read still exceeds the frame limit
    for (;;) {
        lan_host_lock();
        if (!g_lan.running) {
            lan_host_unlock();
            break;
        }
        u64 now = now_us();
        u32 timeout = run_timers(now);
        g_lan.thread_deadline_us = now + timeout * 1000ull;
        lan_host_socket host = g_lan.host;
        lan_host_unlock();
        int ready = lan_host_wait_readable(host, timeout);
        if (ready < 0) {
            lan_host_lock();
            lan_host_wait(LAN_THREAD_TICK_MS);
            lan_host_unlock();
            continue;
        }
        for (int n = 0; ready > 0 && n < 256; n++) {
            u32 address;
            u16 port;
            int size = lan_host_receive(host, wire, sizeof(wire), &address, &port);
            if (size < 0)
                break;
            if ((u32)size > RELAY_FRAME_LIMIT)
                continue;
            lan_host_lock();
            if (g_lan.running)
                receive_datagram(wire, (u32)size, address, port, now_us());
            lan_host_unlock();
        }
    }
}

// ---- XNet (lan.h) ----

int port_lan_enabled(void)
{
    if (!__atomic_load_n(&g_lan.configured, __ATOMIC_ACQUIRE)) {
        int live_enabled = port_live_enabled(); // Live sign-in takes its lock before the LAN lock
        lan_host_lock();
        configure(live_enabled);
        lan_host_unlock();
    }
    return g_lan.enabled;
}

PORT_MS_ABI int port_lan_active(void) { return __atomic_load_n(&g_lan.running, __ATOMIC_ACQUIRE); }

static u32 startup_value(const unsigned char *params, int index, u32 fallback)
{
    return params != 0 && params[0] >= PORT_LAN_STARTUP_PARAMS_SIZE && params[index] != 0 ? params[index] : fallback;
}

static u32 minimum(u32 a, u32 b) { return a < b ? a : b; }

int port_lan_startup(const unsigned char *params)
{
    if (!port_lan_enabled())
        return 0;
    lan_host_lock();
    while (g_lan.stopping)
        lan_host_wait(LAN_HOST_FOREVER); // an XNetCleanup on another thread is still joining its thread
    if (g_lan.running) {
        lan_host_unlock();
        return 0;
    }
    lan_text t = { { 0 }, 0 };
    // XNetStartupParams: zero fields take the Xbox's defaults
    g_lan.socket_limit = minimum(startup_value(params, 2, 8), LAN_SOCKET_LIMIT);
    g_lan.receive_buffer = startup_value(params, 4, 16) * 1024;
    g_lan.key_limit = minimum(startup_value(params, 6, 8), LAN_KEY_LIMIT);
    g_lan.peer_limit = minimum(startup_value(params, 7, 32), LAN_PEER_LIMIT);
    g_lan.qos_data_limit = minimum(startup_value(params, 8, 64) * 4, LAN_QOS_DATA_LIMIT);
    g_lan.qos_retries = minimum(startup_value(params, 10, 3), 8);
    if (lan_host_startup() != 0) {
        g_lan.failed = 1;
        text_add(&t, "system link: the host's sockets are not available; no network");
        lan_log(&t);
        lan_host_unlock();
        return 0;
    }
    identity_create();
    int result = LAN_HOST_FAILED;
    for (u32 slot = 0; slot < g_lan.slots && result != LAN_HOST_OK; slot++) {
        g_lan.host_port = (u16)(g_lan.base_port + slot);
        result = lan_host_open(g_lan.host_port, &g_lan.host);
        if (result == LAN_HOST_FAILED)
            break;
    }
    if (result != LAN_HOST_OK) {
        g_lan.failed = 1;
        text_add(&t, "system link: no free UDP port in ");
        text_number(&t, g_lan.base_port);
        text_add(&t, "-");
        text_number(&t, g_lan.base_port + g_lan.slots - 1u);
        text_add(&t, " (HALO3_LAN_PORT, HALO3_LAN_SLOTS); no network");
        lan_log(&t);
        lan_host_unlock();
        return 0;
    }
    targets_refresh(now_us()); // the interfaces, then this console's address on them
    zero(g_lan.xnaddr, sizeof(g_lan.xnaddr));
    u32 local_ip = local_address();
    store32(g_lan.xnaddr + XNADDR_INA, local_ip);
    store16(g_lan.xnaddr + XNADDR_PORT_ONLINE, g_lan.host_port);
    copy(g_lan.xnaddr + XNADDR_ENET, g_lan.mac, 6);
    if (g_lan.live_enabled) {
        store32(g_lan.xnaddr + XNADDR_INA_ONLINE, local_ip);
        copy(g_lan.xnaddr + XNADDR_ONLINE, g_lan.mac, 6);
    }
    g_lan.next_dynamic_port = LAN_FIRST_DYNAMIC_PORT;
    g_lan.failed = 0;
    __atomic_store_n(&g_lan.running, 1, __ATOMIC_RELEASE);
    if (lan_host_thread_start(lan_thread, 0) != 0) {
        __atomic_store_n(&g_lan.running, 0, __ATOMIC_RELEASE);
        lan_host_close(g_lan.host);
        g_lan.failed = 1;
        text_add(&t, "system link: cannot start its thread; no network");
        lan_log(&t);
        lan_host_unlock();
        return 0;
    }
    if (g_lan.relay.port != 0)
        relay_register(now_us());
    text_add(&t, "system link: ");
    text_endpoint(&t, load32(g_lan.xnaddr + XNADDR_INA), g_lan.host_port);
    text_add(&t, " mac ");
    text_mac(&t, g_lan.mac);
    text_add(&t, ", broadcasts to ");
    text_number(&t, (u32)g_lan.target_count);
    text_add(&t, " destinations");
    lan_log(&t);
    touch();
    lan_host_unlock();
    return 0;
}

void port_lan_cleanup(void)
{
    lan_host_lock();
    if (!g_lan.running) {
        lan_host_unlock();
        return;
    }
    for (int i = 0; i < LAN_PEER_LIMIT; i++)
        if (g_lan.peers[i].secure != 0)
            peer_drop(&g_lan.peers[i]);
    __atomic_store_n(&g_lan.running, 0, __ATOMIC_RELEASE);
    g_lan.stopping = 1; // until the thread is joined and the tables reset: XNetStartup waits
    lan_host_unlock();
    lan_host_thread_join();
    lan_host_lock();
    lan_host_close(g_lan.host);
    g_lan.host = LAN_HOST_NO_SOCKET;
    // XNetCleanup closes every socket and drops every key; lookups the title still holds end uncontacted
    for (int i = 0; i < LAN_QOS_LOOKUP_LIMIT; i++)
        if (g_lan.lookups[i].memory != 0)
            for (u32 t = 0; t < g_lan.lookups[i].count; t++)
                qos_finish(&g_lan.lookups[i], t);
    zero(g_lan.sockets, sizeof(g_lan.sockets));
    zero(g_lan.keys, sizeof(g_lan.keys));
    zero(g_lan.listeners, sizeof(g_lan.listeners));
    zero(g_lan.peers, sizeof(g_lan.peers));
    zero(g_lan.endpoints, sizeof(g_lan.endpoints));
    zero(g_lan.senders, sizeof(g_lan.senders));
    g_lan.relay.ready = 0;
    g_lan.relay.awaiting_ack = 0;
    g_lan.relay.next_register_us = 0;
    g_lan.stopping = 0;
    touch();
    lan_host_unlock();
}

unsigned int port_lan_ethernet_link_status(void)
{
    // a console with System Link on has its cable plugged in, until XNetStartup finds no port to use
    if (!port_lan_enabled() || __atomic_load_n(&g_lan.failed, __ATOMIC_ACQUIRE))
        return 0;
    return LAN_ETHERNET_LINK_ACTIVE | LAN_ETHERNET_LINK_100MBPS | LAN_ETHERNET_LINK_FULL_DUPLEX;
}

unsigned int port_lan_title_xnaddr(void *xnaddr)
{
    lan_host_lock();
    u32 status = LAN_GET_XNADDR_NONE;
    if (g_lan.running) {
        if (xnaddr != 0)
            copy(xnaddr, g_lan.xnaddr, PORT_LAN_XNADDR_SIZE);
        status = LAN_GET_XNADDR_ETHERNET | LAN_GET_XNADDR_STATIC;
        if (g_lan.live_enabled || g_lan.relay.ready)
            status |= 0x00000080u; // XNET_GET_XNADDR_ONLINE
    } else if (xnaddr != 0) {
        // no network, so no address: the console's MAC address all the same (identity_create)
        zero(xnaddr, PORT_LAN_XNADDR_SIZE);
        identity_create();
        copy((u8 *)xnaddr + XNADDR_ENET, g_lan.mac, 6);
    }
    lan_host_unlock();
    return status;
}

void port_lan_set_relay(unsigned int ipv4_host_order, unsigned short port, unsigned long long xuid, const char *token)
{
    u16 size = 0;
    while (token != 0 && size <= RELAY_TOKEN_LIMIT && token[size] != 0)
        size++;
    lan_host_lock();
    lan_relay *relay = &g_lan.relay;
    zero(relay, sizeof(*relay));
    if (unicast_ipv4(ipv4_host_order) && port != 0 && size <= RELAY_TOKEN_LIMIT) {
        relay->address = ipv4_host_order;
        relay->port = port;
        relay->xuid = xuid;
        relay->token_size = size;
        copy(relay->token, token, size);
    }
    for (int i = 0; i < LAN_ENDPOINT_LIMIT; i++) {
        lan_endpoint *e = &g_lan.endpoints[i];
        e->public_port = 0;
        e->pings_left = 0;
        e->fallback_us = 0;
        e->next_punch_us = 0;
    }
    if (g_lan.running) {
        u32 lan_ip = load32(g_lan.xnaddr + XNADDR_INA);
        store32(g_lan.xnaddr + XNADDR_INA_ONLINE, g_lan.live_enabled ? lan_ip : 0);
        store16(g_lan.xnaddr + XNADDR_PORT_ONLINE, g_lan.host_port);
        if (!g_lan.live_enabled)
            zero(g_lan.xnaddr + XNADDR_ONLINE, 6);
        if (relay->port != 0)
            relay_register(now_us());
        touch();
    }
    lan_host_unlock();
}

void port_lan_clear_relay(void)
{
    port_lan_set_relay(0, 0, 0, 0);
}

int port_lan_register_key(const void *xnkid, const void *xnkey)
{
    if (xnkid == 0 || xnkey == 0)
        return PORT_LAN_WSAEFAULT;
    lan_host_lock();
    int result = 0;
    u32 used = 0;
    for (int i = 0; i < LAN_KEY_LIMIT; i++)
        used += g_lan.keys[i].used;
    if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (key_find((const u8 *)xnkid) != 0)
        result = PORT_LAN_WSAEALREADY;
    else if (used >= g_lan.key_limit)
        result = PORT_LAN_WSAENOBUFS;
    else
        for (int i = 0; i < LAN_KEY_LIMIT; i++)
            if (!g_lan.keys[i].used) {
                g_lan.keys[i].used = 1;
                copy(g_lan.keys[i].kid, xnkid, PORT_LAN_XNKID_SIZE);
                copy(g_lan.keys[i].key, xnkey, PORT_LAN_XNKEY_SIZE);
                break;
            }
    lan_host_unlock();
    return result;
}

int port_lan_unregister_key(const void *xnkid)
{
    if (xnkid == 0)
        return PORT_LAN_WSAEFAULT;
    lan_host_lock();
    lan_key *key = key_find((const u8 *)xnkid);
    if (key == 0) {
        lan_host_unlock();
        return PORT_LAN_WSAEINVAL;
    }
    key->used = 0;
    lan_listener *listener = listener_find((const u8 *)xnkid);
    if (listener != 0)
        listener->used = 0;
    for (int i = 0; i < LAN_PEER_LIMIT; i++)
        if (g_lan.peers[i].secure != 0 && same(g_lan.peers[i].kid, xnkid, PORT_LAN_XNKID_SIZE))
            peer_drop(&g_lan.peers[i]);
    touch();
    lan_host_unlock();
    return 0;
}

int port_lan_xnaddr_to_inaddr(const void *xnaddr, const void *xnkid, unsigned int *secure)
{
    if (xnaddr == 0 || xnkid == 0 || secure == 0)
        return PORT_LAN_WSAEFAULT;
    lan_host_lock();
    u32 handle = 0;
    int result = g_lan.running ? peer_map((const u8 *)xnaddr, (const u8 *)xnkid, &handle, 0) : PORT_LAN_WSAENETDOWN;
    if (result == 0)
        *secure = handle;
    lan_host_unlock();
    return result;
}

int port_lan_inaddr_to_xnaddr(unsigned int secure, void *xnaddr, void *xnkid)
{
    lan_host_lock();
    int result = 0;
    const lan_peer *peer = peer_of_secure(secure);
    if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (secure == LAN_LOOPBACK) {
        if (xnaddr != 0)
            copy(xnaddr, g_lan.xnaddr, PORT_LAN_XNADDR_SIZE);
        if (xnkid != 0)
            zero(xnkid, PORT_LAN_XNKID_SIZE);
    } else if (peer == 0)
        result = PORT_LAN_WSAEINVAL;
    else {
        if (xnaddr != 0)
            copy(xnaddr, peer->xnaddr, PORT_LAN_XNADDR_SIZE);
        if (xnkid != 0)
            copy(xnkid, peer->kid, PORT_LAN_XNKID_SIZE);
    }
    lan_host_unlock();
    return result;
}

int port_lan_unregister_inaddr(unsigned int secure)
{
    lan_host_lock();
    lan_peer *peer = peer_of_secure(secure);
    int result = peer != 0 ? 0 : PORT_LAN_WSAEINVAL;
    if (peer != 0) {
        peer_drop(peer);
        touch();
    }
    lan_host_unlock();
    return result;
}

int port_lan_connect_peer(unsigned int secure)
{
    lan_host_lock();
    lan_peer *peer = peer_of_secure(secure);
    int result = 0;
    if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (peer == 0)
        result = PORT_LAN_WSAEINVAL;
    else if (peer->status != LAN_CONNECT_STATUS_CONNECTED && peer->status != LAN_CONNECT_STATUS_PENDING)
        peer_connect(peer, now_us());
    lan_host_unlock();
    return result;
}

unsigned int port_lan_connect_status(unsigned int secure)
{
    lan_host_lock();
    const lan_peer *peer = peer_of_secure(secure);
    // only a mapped peer on a live link has a status; any other address (loopback included) reads as lost
    u32 status = g_lan.running && peer != 0 ? peer->status : LAN_CONNECT_STATUS_LOST;
    lan_host_unlock();
    return status;
}

int port_lan_machine_id(const void *xnaddr, unsigned long long *machine_id)
{
    if (xnaddr == 0 || machine_id == 0)
        return PORT_LAN_WSAEFAULT;
    // a System Link console's machine id: 0xFA and its MAC address
    const u8 *mac = xnaddr_mac((const u8 *)xnaddr);
    u64 id = 0;
    for (int i = 0; i < 6; i++)
        id = (id << 8) | mac[i];
    *machine_id = 0xfa00000000000000ull | id;
    return 0;
}

int port_lan_qos_listen(const void *xnkid, const unsigned char *data, unsigned int size, unsigned int bits_per_second,
    unsigned int flags)
{
    if (xnkid == 0)
        return PORT_LAN_WSAEINVAL;
    lan_host_lock();
    int result = 0;
    lan_listener *listener = listener_find((const u8 *)xnkid);
    if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (key_find((const u8 *)xnkid) == 0)
        result = PORT_LAN_WSAEINVAL;
    else if (flags & LAN_QOS_LISTEN_RELEASE) {
        if (listener != 0)
            listener->used = 0;
    } else {
        for (int i = 0; i < LAN_KEY_LIMIT && listener == 0; i++)
            if (!g_lan.listeners[i].used) {
                listener = &g_lan.listeners[i];
                zero(listener, sizeof(*listener));
                listener->used = 1;
                copy(listener->kid, xnkid, PORT_LAN_XNKID_SIZE);
            }
        if (listener == 0)
            result = PORT_LAN_WSAENOBUFS;
        else {
            if (flags & LAN_QOS_LISTEN_SET_DATA) {
                u32 n = data != 0 ? minimum(size, g_lan.qos_data_limit) : 0;
                copy(listener->data, data, n);
                listener->size = (u16)n;
            }
            if (flags & LAN_QOS_LISTEN_SET_BITSPERSEC)
                listener->bits_per_second = bits_per_second;
            if (flags & (LAN_QOS_LISTEN_ENABLE | LAN_QOS_LISTEN_DISABLE))
                listener->enabled = (flags & LAN_QOS_LISTEN_ENABLE) != 0;
        }
    }
    lan_host_unlock();
    return result;
}

int port_lan_qos_lookup(unsigned int count, const void *const *xnaddrs, const void *const *xnkids,
    const void *const *xnkeys, unsigned int gateway_count, unsigned int probes, unsigned int bits_per_second,
    void *event, void **qos_out)
{
    (void)bits_per_second;
    if (qos_out == 0 || count > LAN_QOS_TARGET_LIMIT || gateway_count > LAN_QOS_TARGET_LIMIT
        || (count != 0 && (xnaddrs == 0 || xnkids == 0 || xnkeys == 0)))
        return PORT_LAN_WSAEINVAL;
    for (u32 i = 0; i < count; i++)
        if (xnaddrs[i] == 0 || xnkids[i] == 0 || xnkeys[i] == 0)
            return PORT_LAN_WSAEINVAL;
    u32 total = count + gateway_count;
    u32 info_size = 8 + (u32)sizeof(lan_xnqosinfo) * (total != 0 ? total : 1);
    u32 data_offset = (info_size + 7) & ~7u;
    u32 targets_offset = (data_offset + count * LAN_QOS_DATA_LIMIT + 7) & ~7u;
    u32 memory_size = targets_offset + count * (u32)sizeof(lan_qos_target);
    lan_host_lock();
    if (!g_lan.running) {
        lan_host_unlock();
        return PORT_LAN_WSAENETDOWN;
    }
    lan_qos_lookup *lookup = 0;
    for (int i = 0; i < LAN_QOS_LOOKUP_LIMIT && lookup == 0; i++)
        if (g_lan.lookups[i].memory == 0)
            lookup = &g_lan.lookups[i];
    u8 *memory = lookup != 0 ? (u8 *)lan_host_pages_alloc(memory_size) : 0;
    if (memory == 0) {
        lan_host_unlock();
        return PORT_LAN_WSAENOBUFS;
    }
    lookup->memory = memory;
    lookup->memory_size = memory_size;
    lookup->id = ++g_lan.next_lookup;
    lookup->count = count;
    lookup->pending = count;
    lookup->data = memory + data_offset;
    lookup->targets = (lan_qos_target *)(memory + targets_offset);
    lookup->event = event;
    lookup->started_us = now_us();
    lan_xnqos *qos = (lan_xnqos *)memory;
    qos->count = total;
    qos->pending = count;
    // a System Link LAN has no Xbox Live gateways to probe
    for (u32 i = count; i < total; i++)
        qos->info[i].flags = LAN_XNQOSINFO_COMPLETE;
    u32 probe_count = probes == 0 ? 1 : minimum(probes, LAN_QOS_PROBE_LIMIT);
    u64 now = now_us();
    *qos_out = memory;
    for (u32 i = 0; i < count; i++) {
        lan_qos_target *target = &lookup->targets[i];
        copy(target->xnaddr, xnaddrs[i], PORT_LAN_XNADDR_SIZE);
        copy(target->kid, xnkids[i], PORT_LAN_XNKID_SIZE);
        target->probes = (u16)probe_count;
        target->next_us = now;
        if (own_mac(xnaddr_mac(target->xnaddr)))
            qos_local(lookup, i);
        else if (all_zero(xnaddr_mac(target->xnaddr), 6))
            qos_finish(lookup, i);
        else
            qos_tick(lookup, i, now);
    }
    timer_armed(now + LAN_QOS_PROBE_SPACING_US); // the next probes go out from the System Link thread
    if (count == 0 && event != 0)
        SetEvent(event);
    lan_host_unlock();
    return 0;
}

int port_lan_qos_release(void *qos)
{
    if (qos == 0)
        return PORT_LAN_WSAEINVAL;
    lan_host_lock();
    int result = PORT_LAN_WSAEINVAL;
    for (int i = 0; i < LAN_QOS_LOOKUP_LIMIT; i++) {
        lan_qos_lookup *lookup = &g_lan.lookups[i];
        if (lookup->memory == (u8 *)qos) {
            lan_host_pages_free(lookup->memory, lookup->memory_size);
            zero(lookup, sizeof(*lookup));
            result = 0;
            break;
        }
    }
    lan_host_unlock();
    return result;
}

int port_lan_qos_listen_stats(const void *xnkid, void *stats)
{
    if (stats == 0 || load32(stats) < PORT_LAN_LISTEN_STATS_SIZE)
        return PORT_LAN_WSAEINVAL;
    u32 values[6] = { 0, 0, 0, 0, 0, 0 };
    lan_host_lock();
    const lan_listener *listener = xnkid != 0 ? listener_find((const u8 *)xnkid) : 0;
    if (listener != 0)
        copy(values, listener->stats, sizeof(values));
    lan_host_unlock();
    copy((u8 *)stats + 4, values, sizeof(values));
    return 0;
}

// ---- the title's sockets (lan.h) ----

PORT_MS_ABI int port_lan_socket(int vdp, unsigned int *handle)
{
    lan_host_lock();
    int result = 0;
    u32 open = 0;
    for (int i = 0; i < LAN_SOCKET_LIMIT; i++)
        open += g_lan.sockets[i].handle != 0;
    if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (open >= g_lan.socket_limit)
        result = PORT_LAN_WSAENOBUFS;
    else
        for (int i = 0; i < LAN_SOCKET_LIMIT; i++) {
            lan_socket *s = &g_lan.sockets[i];
            if (s->handle != 0)
                continue;
            zero(s, sizeof(*s) - sizeof(s->inbox));
            g_lan.socket_generation = g_lan.socket_generation % 0xffff + 1;
            s->handle = (g_lan.socket_generation << 8) | (u32)(i + 1);
            s->vdp = vdp != 0;
            s->budget = g_lan.receive_buffer;
            *handle = s->handle;
            break;
        }
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_socket_valid(unsigned int handle)
{
    lan_host_lock();
    int result = socket_of(handle) != 0 ? 0 : PORT_LAN_WSAENOTSOCK;
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_close(unsigned int handle)
{
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    if (s != 0) {
        s->handle = 0;
        touch(); // a blocked receive or select on it returns
    }
    lan_host_unlock();
    return s != 0 ? 0 : PORT_LAN_WSAENOTSOCK;
}

PORT_MS_ABI int port_lan_bind(unsigned int handle, const port_lan_address *local)
{
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    int result = 0;
    if (s == 0)
        result = PORT_LAN_WSAENOTSOCK;
    else if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (s->port != 0)
        result = PORT_LAN_WSAEINVAL;
    else if (local->address != 0 && local->address != LAN_LOOPBACK
        && local->address != load32(g_lan.xnaddr + XNADDR_INA))
        result = PORT_LAN_WSAEADDRNOTAVAIL;
    else
        result = socket_bind_port(s, local->port);
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_connect(unsigned int handle, const port_lan_address *remote)
{
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    int result = 0;
    if (s == 0)
        result = PORT_LAN_WSAENOTSOCK;
    else if (remote->address == 0)
        s->connected = 0;
    else if (remote->port == 0)
        result = PORT_LAN_WSAEADDRNOTAVAIL;
    else {
        if (s->port == 0)
            result = socket_bind_port(s, 0);
        if (result == 0) {
            s->connected = 1;
            s->remote = *remote;
        }
    }
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_shutdown(unsigned int handle)
{
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    int result = s == 0 ? PORT_LAN_WSAENOTSOCK : s->connected ? 0 : PORT_LAN_WSAENOTCONN;
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_set_option(unsigned int handle, int option, unsigned int value)
{
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    int result = 0;
    if (s == 0)
        result = PORT_LAN_WSAENOTSOCK;
    else if (option == PORT_LAN_OPTION_BROADCAST)
        s->broadcast = value != 0;
    else if (option == PORT_LAN_OPTION_NONBLOCKING)
        s->nonblocking = value != 0;
    else if (option == PORT_LAN_OPTION_RECEIVE_BUFFER)
        s->budget = value < PORT_LAN_MAX_DATAGRAM ? PORT_LAN_MAX_DATAGRAM : minimum(value, 1u << 20);
    else
        result = PORT_LAN_WSAENOPROTOOPT;
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_pending(unsigned int handle, unsigned int *bytes)
{
    lan_host_lock();
    const lan_socket *s = socket_of(handle);
    if (s != 0)
        *bytes = s->count != 0 ? s->inbox[s->head].size : 0;
    lan_host_unlock();
    return s != 0 ? 0 : PORT_LAN_WSAENOTSOCK;
}

// Why a datagram cannot go to destination (Xenia's DatagramDestinationError), 0 if it can.
static int destination_error(const port_lan_address *destination, int broadcast)
{
    u32 a = destination->address;
    if (a == 0 || destination->port == 0 || ((a >> 24) == 0x7f && a != LAN_LOOPBACK))
        return PORT_LAN_WSAEADDRNOTAVAIL;
    if (a == LAN_BROADCAST && !broadcast)
        return PORT_LAN_WSAEACCES;
    return 0;
}

PORT_MS_ABI int port_lan_send(unsigned int handle, const void *data, int size, const port_lan_address *remote, int *sent)
{
    *sent = 0;
    lan_host_lock();
    lan_socket *s = socket_of(handle);
    int result = 0;
    port_lan_address to = { 0, 0 };
    if (s == 0)
        result = PORT_LAN_WSAENOTSOCK;
    else if (!g_lan.running)
        result = PORT_LAN_WSAENETDOWN;
    else if (remote == 0 && !s->connected)
        result = PORT_LAN_WSAENOTCONN;
    else {
        to = remote != 0 ? *remote : s->remote;
        result = destination_error(&to, s->broadcast);
    }
    if (result == 0 && (size < 0 || (u32)size > PORT_LAN_MAX_DATAGRAM))
        result = PORT_LAN_WSAEMSGSIZE;
    if (result == 0 && s->vdp && !vdp_valid((const u8 *)data, (u32)size))
        result = PORT_LAN_WSAEINVAL;
    if (result == 0 && s->port == 0)
        result = socket_bind_port(s, 0); // an unbound socket is bound on its first send
    if (result == 0) {
        u8 kind = s->vdp ? LAN_KIND_VDP : LAN_KIND_UDP;
        if (to.address == LAN_BROADCAST) {
            u64 now = now_us();
            if (now - g_lan.targets_refreshed_us >= LAN_TARGETS_REFRESH_US)
                targets_refresh(now);
            u32 sequence = ++g_lan.broadcast_sequence; // its own count: consecutive broadcasts differ by one
            for (int i = 0; i < g_lan.target_count; i++)
                frame_send_numbered(sequence, LAN_KIND_BROADCAST, s->port, to.port, 0, data, (u32)size,
                    g_lan.targets[i].address, g_lan.targets[i].port);
        } else if (to.address == LAN_LOOPBACK)
            deliver(to.port, kind, LAN_LOOPBACK, s->port, (const u8 *)data, (u32)size);
        else {
            lan_peer *peer = peer_of_secure(to.address);
            if (peer == 0)
                result = PORT_LAN_WSAEHOSTUNREACH; // not a secure address this console mapped
            else {
                // XNet opens the association on the first datagram to a peer
                u64 now = now_us();
                if (peer->status == LAN_CONNECT_STATUS_IDLE)
                    peer_connect(peer, now);
                int sent_frame = frame_send_peer(kind, s->port, to.port, peer->kid, data, (u32)size, peer->xnaddr, now);
                result = sent_frame == LAN_HOST_OK ? 0
                    : sent_frame == LAN_HOST_WOULD_BLOCK ? PORT_LAN_WSAEWOULDBLOCK
                    : sent_frame == LAN_HOST_UNREACHABLE ? PORT_LAN_WSAEHOSTUNREACH
                    : PORT_LAN_WSAENETDOWN;
            }
        }
    }
    if (s != 0 && to.address != LAN_BROADCAST && to.address != LAN_LOOPBACK)
        trace_datagram("sent", s->vdp, s->port, to.port, to.address, (const u8 *)data, size < 0 ? 0 : (u32)size,
            result);
    if (result == 0)
        *sent = size;
    lan_host_unlock();
    return result;
}

PORT_MS_ABI int port_lan_receive(unsigned int handle, void *buffer, int size, int peek, port_lan_address *remote,
    int *received)
{
    *received = 0;
    lan_host_lock();
    for (;;) {
        lan_socket *s = socket_of(handle);
        if (s == 0) {
            lan_host_unlock();
            return PORT_LAN_WSAENOTSOCK;
        }
        if (!g_lan.running) {
            lan_host_unlock();
            return PORT_LAN_WSAENETDOWN;
        }
        if (s->count != 0) {
            const lan_datagram *d = &s->inbox[s->head];
            u32 n = minimum(d->size, size < 0 ? 0u : (u32)size);
            copy(buffer, d->data, n);
            if (remote != 0) {
                remote->address = d->source;
                remote->port = d->source_port;
            }
            int result = d->size > n ? PORT_LAN_WSAEMSGSIZE : 0;
            *received = (int)n;
            if (!peek) {
                if (d->source != LAN_DISCOVERY_SOURCE)
                    trace_datagram("taken", s->vdp, d->source_port, s->port, d->source, d->data, d->size, 0);
                inbox_drop_head(s);
            }
            lan_host_unlock();
            return result;
        }
        if (s->nonblocking) {
            lan_host_unlock();
            return PORT_LAN_WSAEWOULDBLOCK;
        }
        lan_host_wait(LAN_HOST_FOREVER);
    }
}

PORT_MS_ABI int port_lan_poll(unsigned int handle, int *readable, int *writable)
{
    lan_host_lock();
    const lan_socket *s = socket_of(handle);
    if (s != 0) {
        *readable = s->count != 0 || !g_lan.running;
        *writable = g_lan.running;
    }
    lan_host_unlock();
    return s != 0 ? 0 : PORT_LAN_WSAENOTSOCK;
}

PORT_MS_ABI unsigned int port_lan_activity(void) { return __atomic_load_n(&g_lan.activity, __ATOMIC_ACQUIRE); }

PORT_MS_ABI unsigned int port_lan_milliseconds(void) { return (unsigned int)(lan_host_microseconds() / 1000); }

PORT_MS_ABI void port_lan_wait(unsigned int activity, unsigned int timeout_ms)
{
    lan_host_lock();
    u64 start = now_us();
    while (g_lan.activity == activity) {
        u32 remaining = LAN_HOST_FOREVER;
        if (timeout_ms != LAN_HOST_FOREVER) {
            u64 elapsed_ms = (now_us() - start) / 1000;
            if (elapsed_ms >= timeout_ms)
                break;
            remaining = timeout_ms - (u32)elapsed_ms;
        }
        lan_host_wait(remaining);
    }
    lan_host_unlock();
}
