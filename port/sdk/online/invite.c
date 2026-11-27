// port/sdk/online (both hosts): game invites (invite.h): the lobby, lobby codes, invites received, accepting one.
#ifdef _WIN32
__declspec(dllimport) char *__cdecl getenv(const char *name);
#else
char *getenv(const char *name);
#endif

#include "invite.h"
#include "lan.h"
#include "live.h"
#include "../runtime/port_runtime.h"
#include "../xam/guide_ingame.h"
#include "../xam/xam_profile.h"
#include "../xam/xam_ui.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

extern PORT_MS_ABI unsigned long GetTickCount(void);
// xam.c: the user as the title sees them (with Live, user 0's Live account)
PORT_MS_ABI unsigned int XUserGetSigninState(unsigned int user_index);
PORT_MS_ABI unsigned int XUserGetName(unsigned int user_index, char *name, unsigned int name_len);
PORT_MS_ABI unsigned int XUserGetXUID(unsigned int user_index, unsigned long long *xuid);

#define INVITE_XSESSION_CREATE_USES_PRESENCE 0x00000002ul
#define INVITE_XSESSION_CREATE_INVITES_DISABLED 0x00000100ul
#define INVITE_XNKID_TYPE_MASK 0xe0u // XNetXnKidIsSystemLink / XNetXnKidIsOnlinePeer
#define INVITE_XNKID_SYSTEM_LINK 0x00u
#define INVITE_XNKID_SYSTEM_LINK_XPLAT 0x40u
#define INVITE_XNKID_ONLINE_PEER 0x80u
#define INVITE_PRESENCE_SESSIONS 4

// XSESSION_INFO's parts
#define SESSION_XNKID 0
#define SESSION_XNADDR 8
#define SESSION_XNKEY 44
// XNADDR's
#define XNADDR_INA 0
#define XNADDR_INA_ONLINE 4
#define XNADDR_PORT_ONLINE 8
#define XNADDR_ENET 10
#define XNADDR_ONLINE 16
#define XNADDR_ONLINE_SIZE 20

#define CODE_VERSION 1u
#define CODE_PAYLOAD_MAX 96u
#define CODE_GROUP 5u
// version, class, XNKID, XNKEY, XNADDR flags, ina, wPortOnline, abEnet, name length, CRC: the smallest payload
#define CODE_PAYLOAD_MIN (1u + 1u + 8u + 16u + 1u + 4u + 2u + 6u + 1u + 4u)
// the largest: a Live lobby's, inaOnline apart, the whole abOnline, an XUID and a 15-byte name
#define CODE_PAYLOAD_LONGEST \
    (1u + 1u + 8u + 16u + 1u + 4u + 4u + 2u + 6u + XNADDR_ONLINE_SIZE + 8u + 1u + (PORT_INVITE_NAME - 1u) + 4u)
#define CODE_DIGITS(bytes) (((bytes) * 8u + 4u) / 5u)
typedef char invite_code_payload_fits[CODE_PAYLOAD_LONGEST <= CODE_PAYLOAD_MAX ? 1 : -1];
#define CODE_GROUP_SEPARATORS ((CODE_DIGITS(CODE_PAYLOAD_LONGEST) - 1u) / CODE_GROUP)
#define CODE_LONGEST (3u + CODE_DIGITS(CODE_PAYLOAD_LONGEST) + CODE_GROUP_SEPARATORS)
// A displayed code can include a line break after each dash, including the prefix.
#define CODE_TEXT_MAX (CODE_LONGEST + 2u * (1u + CODE_GROUP_SEPARATORS))
typedef char invite_code_fits[CODE_LONGEST < PORT_INVITE_CODE_MAX ? 1 : -1];

static const char k_alphabet[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ"; // Crockford's base 32

typedef struct {
    int used;
    unsigned int handle;
    u8 session[PORT_INVITE_SESSION_SIZE];
    unsigned int flags;
    int host;
    unsigned long order; // which was reported last
} presence_session;

#define INVITE_SEEN 32 // invite ids remembered (well over the inbox, and over what a server delivers in one poll)

typedef struct {
    char id[PORT_INVITE_ID]; // "" unused
    int answered;            // the server was told
} seen_invite;

static struct {
    volatile char busy;
    int trace; // -1 undecided
    int squad_known;
    port_invite_lobby squad;
    presence_session presence[INVITE_PRESENCE_SESSIONS];
    unsigned long presence_order;
    port_invite inbox[PORT_INVITE_INBOX]; // oldest first
    unsigned int inbox_count;
    seen_invite seen[INVITE_SEEN];        // the invite ids received lately (port_invite_received)
    unsigned seen_next;
    port_invite accepted[PORT_PROFILE_USERS];
    int has_accepted[PORT_PROFILE_USERS];
} g_invite = { .trace = -1 };

static void invite_lock(void)
{
    while (__atomic_test_and_set(&g_invite.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void invite_unlock(void) { __atomic_clear(&g_invite.busy, __ATOMIC_RELEASE); }

// ---- text ----

static unsigned long bounded_length(const char *text, unsigned long capacity)
{
    unsigned long n = 0;
    while (n < capacity && text[n] != 0)
        n++;
    return n;
}

static int printable_ascii(const char *text, unsigned long length)
{
    for (unsigned long i = 0; i < length; i++) {
        const u8 c = (u8)text[i];
        if (c < 0x20 || c > 0x7e)
            return 0;
    }
    return 1;
}

// `text` into `to` (capacity bytes, NUL included), never cutting a UTF-8 sequence.
static void text_copy(char *to, unsigned long capacity, const char *text)
{
    unsigned long n = 0;
    for (; *text != 0 && n + 1 < capacity; text++) {
        const unsigned char c = (unsigned char)*text;
        const unsigned long size = c < 0x80 ? 1 : c >= 0xf0 ? 4 : c >= 0xe0 ? 3 : c >= 0xc0 ? 2 : 1;
        if (n + size >= capacity)
            break;
        to[n++] = *text;
        for (unsigned long k = 1; k < size && text[1] != 0; k++)
            to[n++] = *++text;
    }
    to[n] = 0;
}

static int same_bytes(const u8 *a, const u8 *b, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}

static int all_zero(const u8 *bytes, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        if (bytes[i] != 0)
            return 0;
    return 1;
}

PORT_MS_ABI int port_invite_trace(void)
{
    int trace = __atomic_load_n(&g_invite.trace, __ATOMIC_ACQUIRE);
    if (trace < 0) {
        const char *setting = getenv("HALO3_INVITE_TRACE");
        trace = setting != 0 && setting[0] == '1';
        __atomic_store_n(&g_invite.trace, trace, __ATOMIC_RELEASE);
    }
    return trace;
}

// A trace line built without the CRT's formatting.
typedef struct {
    char text[256];
    unsigned long length;
} trace_line;

static void line_add(trace_line *line, const char *text)
{
    for (; text != 0 && *text != 0 && line->length + 2 < sizeof(line->text); text++)
        line->text[line->length++] = *text;
    line->text[line->length] = 0;
}

static void line_number(trace_line *line, unsigned long value)
{
    char digits[12];
    int n = 0;
    do {
        digits[n++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    char text[12];
    int at = 0;
    while (n > 0)
        text[at++] = digits[--n];
    text[at] = 0;
    line_add(line, text);
}

static void line_hex(trace_line *line, const u8 *bytes, unsigned long size)
{
    char text[3] = { 0, 0, 0 };
    for (unsigned long i = 0; i < size; i++) {
        text[0] = "0123456789abcdef"[bytes[i] >> 4];
        text[1] = "0123456789abcdef"[bytes[i] & 15];
        line_add(line, text);
    }
}

static const char *class_name(int session_class)
{
    return session_class == PORT_INVITE_CLASS_SYSTEM_LINK ? "System Link"
           : session_class == PORT_INVITE_CLASS_LIVE      ? "Live"
                                                          : "offline";
}

static void line_write(trace_line *line)
{
    line_add(line, "\n");
    port_write_error(line->text);
}

// ---- the lobby ----

// Which class an XNKID is of: the console's XNET_XNKID_* types (System Link for a console off Live, an online peer's
// for a Live session), NONE for another (a title server's).
static int xnkid_class(const u8 *xnkid)
{
    const unsigned type = xnkid[0] & INVITE_XNKID_TYPE_MASK;
    if (type == INVITE_XNKID_SYSTEM_LINK || type == INVITE_XNKID_SYSTEM_LINK_XPLAT)
        return PORT_INVITE_CLASS_SYSTEM_LINK;
    if (type == INVITE_XNKID_ONLINE_PEER)
        return PORT_INVITE_CLASS_LIVE;
    return PORT_INVITE_CLASS_NONE;
}

// The port stores XNADDR's IPv4 numbers in host order on both x86 hosts.
static int unicast_ipv4(const u8 *ina)
{
    return ina[3] != 0 && ina[3] < 224;
}

// Whether `session` names a host another console can reach.
static int session_valid(const u8 *session, int session_class)
{
    const u8 *xnaddr = session + SESSION_XNADDR;
    const u8 *mac = xnaddr + XNADDR_ENET;
    return (session_class == PORT_INVITE_CLASS_SYSTEM_LINK || session_class == PORT_INVITE_CLASS_LIVE) &&
           xnkid_class(session + SESSION_XNKID) == session_class &&
           !all_zero(session + SESSION_XNKID, PORT_LAN_XNKID_SIZE) &&
           !all_zero(session + SESSION_XNKEY, PORT_LAN_XNKEY_SIZE) &&
           unicast_ipv4(xnaddr + XNADDR_INA) &&
           (all_zero(xnaddr + XNADDR_INA_ONLINE, 4) || unicast_ipv4(xnaddr + XNADDR_INA_ONLINE)) &&
           !all_zero(xnaddr + XNADDR_PORT_ONLINE, 2) && (mac[0] & 1) == 0 && !all_zero(mac, 6);
}

static int invite_valid(const port_invite *invite)
{
    if (invite == 0 || !session_valid(invite->session, invite->session_class))
        return 0;
    const unsigned long length = bounded_length(invite->inviter, sizeof(invite->inviter));
    return length < sizeof(invite->inviter) && printable_ascii(invite->inviter, length) &&
           bounded_length(invite->id, sizeof(invite->id)) < sizeof(invite->id);
}

PORT_MS_ABI void port_invite_set_squad(const port_invite_lobby *lobby)
{
    port_invite_lobby now;
    __builtin_memset(&now, 0, sizeof(now));
    if (lobby != 0 && session_valid(lobby->session, lobby->session_class))
        now = *lobby;
    invite_lock();
    const int changed =
        !g_invite.squad_known || !same_bytes((const u8 *)&now, (const u8 *)&g_invite.squad, sizeof(now));
    g_invite.squad_known = 1;
    g_invite.squad = now;
    invite_unlock();
    if (changed && port_invite_trace()) {
        trace_line line = { { 0 }, 0 };
        line_add(&line, "invite: lobby: ");
        if (now.session_class == PORT_INVITE_CLASS_NONE) {
            line_add(&line, "none");
        } else {
            line_add(&line, class_name(now.session_class));
            line_add(&line, now.host ? " (host) " : " (member) ");
            line_hex(&line, now.session + SESSION_XNKID, 8);
        }
        line_write(&line);
    }
}

// Under the lock: the presence session (the newest reported), 0 for none.
static const presence_session *presence_locked(void)
{
    const presence_session *newest = 0;
    for (int i = 0; i < INVITE_PRESENCE_SESSIONS; i++) {
        const presence_session *p = &g_invite.presence[i];
        if (p->used && (p->flags & INVITE_XSESSION_CREATE_USES_PRESENCE) != 0 &&
            (newest == 0 || p->order > newest->order))
            newest = p;
    }
    return newest;
}

PORT_MS_ABI void port_invite_presence_session(unsigned int handle, const unsigned char session[PORT_INVITE_SESSION_SIZE],
    unsigned int flags, int host)
{
    if (handle == 0 || session == 0)
        return;
    invite_lock();
    presence_session *slot = 0;
    for (int i = 0; i < INVITE_PRESENCE_SESSIONS && slot == 0; i++)
        if (g_invite.presence[i].used && g_invite.presence[i].handle == handle)
            slot = &g_invite.presence[i];
    for (int i = 0; i < INVITE_PRESENCE_SESSIONS && slot == 0; i++)
        if (!g_invite.presence[i].used)
            slot = &g_invite.presence[i];
    if (slot == 0) { // full: the oldest goes (a title deletes its sessions; this only guards against one that did not)
        slot = &g_invite.presence[0];
        for (int i = 1; i < INVITE_PRESENCE_SESSIONS; i++)
            if (g_invite.presence[i].order < slot->order)
                slot = &g_invite.presence[i];
    }
    const int changed = !slot->used || slot->handle != handle || slot->flags != flags || slot->host != (host != 0) ||
                        !same_bytes(slot->session, session, PORT_INVITE_SESSION_SIZE);
    slot->used = 1;
    slot->handle = handle;
    __builtin_memcpy(slot->session, session, PORT_INVITE_SESSION_SIZE);
    slot->flags = flags;
    slot->host = host != 0;
    slot->order = ++g_invite.presence_order;
    invite_unlock();
    if (changed && port_invite_trace() && (flags & INVITE_XSESSION_CREATE_USES_PRESENCE) != 0) { // (not per modify)
        trace_line line = { { 0 }, 0 };
        line_add(&line, "invite: Live presence session ");
        line_hex(&line, session + SESSION_XNKID, 8);
        line_add(&line, host ? " (host)" : " (member)");
        line_add(&line, (flags & INVITE_XSESSION_CREATE_INVITES_DISABLED) != 0 ? ", invites disabled" : "");
        line_write(&line);
    }
}

PORT_MS_ABI void port_invite_presence_session_deleted(unsigned int handle)
{
    invite_lock();
    for (int i = 0; i < INVITE_PRESENCE_SESSIONS; i++)
        if (g_invite.presence[i].used && g_invite.presence[i].handle == handle)
            g_invite.presence[i].used = 0;
    invite_unlock();
}

static const char k_no_lobby[] = "Host or join a System Link or Live lobby first.";
static const char k_closed[] = "This lobby is closed to invites.";

// Under the lock.
static int current_lobby_locked(port_invite_lobby *lobby, const char **why)
{
    const presence_session *presence = presence_locked();
    const int presence_open = presence != 0 && (presence->flags & INVITE_XSESSION_CREATE_INVITES_DISABLED) == 0;
    *why = k_no_lobby;
    if (g_invite.squad_known && g_invite.squad.session_class != PORT_INVITE_CLASS_NONE) {
        if (g_invite.squad.session_class == PORT_INVITE_CLASS_LIVE && presence != 0 && !presence_open &&
            same_bytes(presence->session + SESSION_XNKID, g_invite.squad.session + SESSION_XNKID, 8)) {
            *why = k_closed;
            return 0;
        }
        *lobby = g_invite.squad;
        *why = "";
        return 1;
    }
    if (presence != 0 && !g_invite.squad_known) { // no game report (a title without one): the console's rule alone
        if (!presence_open) {
            *why = k_closed;
            return 0;
        }
        __builtin_memcpy(lobby->session, presence->session, PORT_INVITE_SESSION_SIZE);
        lobby->session_class = xnkid_class(presence->session + SESSION_XNKID);
        lobby->host = presence->host;
        if (!session_valid(lobby->session, lobby->session_class))
            return 0;
        *why = "";
        return 1;
    }
    return 0;
}

PORT_MS_ABI int port_invite_current_lobby(port_invite_lobby *lobby, const char **why)
{
    port_invite_lobby found;
    const char *reason = k_no_lobby;
    __builtin_memset(&found, 0, sizeof(found));
    invite_lock();
    const int there = current_lobby_locked(&found, &reason);
    invite_unlock();
    if (lobby != 0)
        *lobby = found;
    if (why != 0)
        *why = reason;
    return there;
}

PORT_MS_ABI int port_invite_presence(unsigned char session[PORT_INVITE_SESSION_SIZE])
{
    invite_lock();
    const presence_session *presence = presence_locked();
    const int there = presence != 0 && (presence->flags & INVITE_XSESSION_CREATE_INVITES_DISABLED) == 0;
    if (there && session != 0)
        __builtin_memcpy(session, presence->session, PORT_INVITE_SESSION_SIZE);
    invite_unlock();
    return there;
}

// ---- lobby codes ----

static u32 crc32(const u8 *bytes, unsigned long size)
{
    u32 crc = 0xfffffffful;
    for (unsigned long i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (int bit = 0; bit < 8; bit++)
            crc = (crc >> 1) ^ (0xedb88320ul & (0ul - (crc & 1ul)));
    }
    return crc ^ 0xfffffffful;
}

// The payload of `invite` (see invite.h) into `payload`. Returns its size, 0 for none.
static unsigned long code_payload(const port_invite *invite, u8 *payload)
{
    const unsigned long name_size = bounded_length(invite->inviter, sizeof(invite->inviter));
    if (name_size == sizeof(invite->inviter) || !printable_ascii(invite->inviter, name_size) ||
        !session_valid(invite->session, invite->session_class))
        return 0;
    const u8 *session = invite->session, *xnaddr = invite->session + SESSION_XNADDR;
    unsigned long n = 0;
    payload[n++] = CODE_VERSION;
    payload[n++] = (u8)(invite->session_class | (invite->inviter_xuid != 0 ? 4 : 0));
    __builtin_memcpy(payload + n, session + SESSION_XNKID, 8);
    n += 8;
    __builtin_memcpy(payload + n, session + SESSION_XNKEY, 16);
    n += 16;
    const int online_zero = all_zero(xnaddr + XNADDR_INA_ONLINE, 4);
    const int online_same = !online_zero && same_bytes(xnaddr + XNADDR_INA_ONLINE, xnaddr + XNADDR_INA, 4);
    const unsigned mode = online_zero ? 0u : online_same ? 1u : 2u;
    unsigned online_size = XNADDR_ONLINE_SIZE;
    while (online_size > 0 && xnaddr[XNADDR_ONLINE + online_size - 1] == 0)
        online_size--;
    payload[n++] = (u8)(mode | online_size << 3);
    __builtin_memcpy(payload + n, xnaddr + XNADDR_INA, 4);
    n += 4;
    if (mode == 2) {
        __builtin_memcpy(payload + n, xnaddr + XNADDR_INA_ONLINE, 4);
        n += 4;
    }
    __builtin_memcpy(payload + n, xnaddr + XNADDR_PORT_ONLINE, 2);
    n += 2;
    __builtin_memcpy(payload + n, xnaddr + XNADDR_ENET, 6);
    n += 6;
    __builtin_memcpy(payload + n, xnaddr + XNADDR_ONLINE, online_size);
    n += online_size;
    if (invite->inviter_xuid != 0) {
        for (int i = 0; i < 8; i++)
            payload[n++] = (u8)(invite->inviter_xuid >> (56 - 8 * i));
    }
    payload[n++] = (u8)name_size;
    __builtin_memcpy(payload + n, invite->inviter, name_size);
    n += name_size;
    const u32 crc = crc32(payload, n);
    for (int i = 0; i < 4; i++)
        payload[n++] = (u8)(crc >> (8 * i));
    return n;
}

PORT_MS_ABI unsigned int port_invite_code_encode(const port_invite *invite, char *code, unsigned int capacity)
{
    if (code == 0 || capacity == 0)
        return 0;
    code[0] = 0;
    u8 payload[CODE_PAYLOAD_MAX];
    const unsigned long size = invite != 0 ? code_payload(invite, payload) : 0;
    if (size == 0)
        return 0;
    const unsigned long digits = (size * 8 + 4) / 5;
    const unsigned long length = 3 + digits + (digits - 1) / CODE_GROUP; // "H3-", the digits, a dash between groups
    if (length + 1 > capacity)
        return 0;
    unsigned int at = 0;
    code[at++] = 'H';
    code[at++] = '3';
    code[at++] = '-';
    u32 bits = 0;
    int bit_count = 0;
    unsigned long written = 0, next = 0;
    while (written < digits) {
        if (bit_count < 5) {
            bits = (bits << 8) | (next < size ? payload[next] : 0);
            next++;
            bit_count += 8;
        }
        const unsigned value = (unsigned)(bits >> (bit_count - 5)) & 31u;
        bit_count -= 5;
        if (written != 0 && written % CODE_GROUP == 0)
            code[at++] = '-';
        code[at++] = k_alphabet[value];
        written++;
    }
    code[at] = 0;
    return at;
}

// A character of a code as Crockford's base 32 reads it: its value, -1 for a separator, -2 for none of its characters.
static int code_digit(char c)
{
    if (c == '-' || c == ' ' || c == '\t' || c == '\r' || c == '\n')
        return -1;
    if (c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 'A');
    if (c == 'O')
        c = '0';
    else if (c == 'I' || c == 'L')
        c = '1';
    for (int i = 0; i < 32; i++)
        if (k_alphabet[i] == c)
            return i;
    return -2;
}

PORT_MS_ABI int port_invite_code_decode(const char *text, port_invite *invite)
{
    if (invite != 0)
        __builtin_memset(invite, 0, sizeof(*invite));
    if (text == 0)
        return PORT_INVITE_CODE_EMPTY;
    if (bounded_length(text, CODE_TEXT_MAX + 1u) > CODE_TEXT_MAX)
        return PORT_INVITE_CODE_LONG;
    while (code_digit(*text) == -1)
        text++;
    if (*text == 0)
        return PORT_INVITE_CODE_EMPTY;
    // "H3": the prefix (its dash optional)
    if (!((text[0] == 'H' || text[0] == 'h') && text[1] == '3'))
        return PORT_INVITE_CODE_PREFIX;
    text += 2;
    u8 payload[CODE_PAYLOAD_LONGEST];
    unsigned long size = 0;
    u32 bits = 0;
    int bit_count = 0;
    for (; *text != 0; text++) {
        const int value = code_digit(*text);
        if (value == -1)
            continue;
        if (value < 0)
            return PORT_INVITE_CODE_CHARACTER;
        if (size == sizeof(payload))
            return PORT_INVITE_CODE_LONG;
        bits = (bits << 5) | (u32)value;
        bit_count += 5;
        if (bit_count >= 8) {
            payload[size++] = (u8)(bits >> (bit_count - 8));
            bit_count -= 8;
        }
    }
    if (size < CODE_PAYLOAD_MIN)
        return PORT_INVITE_CODE_SHORT;
    // only the code port_invite_code_encode writes: no digit past the last byte, the last digit's spare bits zero (else
    // a wrong last character could still read as the same lobby)
    if (bit_count >= 5 || (bits & ((1u << bit_count) - 1u)) != 0)
        return PORT_INVITE_CODE_CHECKSUM;
    u32 stored = 0;
    for (int i = 0; i < 4; i++)
        stored |= (u32)payload[size - 4 + i] << (8 * i);
    if (crc32(payload, size - 4) != stored)
        return PORT_INVITE_CODE_CHECKSUM;
    if (payload[0] != CODE_VERSION)
        return PORT_INVITE_CODE_VERSION;
    // the fields, each checked against what is left before the checksum
    const unsigned long end = size - 4;
    unsigned long n = 1;
    port_invite decoded;
    __builtin_memset(&decoded, 0, sizeof(decoded));
    const unsigned flags = payload[n++];
    decoded.session_class = (int)(flags & 3u);
    if ((flags & ~7u) != 0 || n + 8 + 16 + 1 > end)
        return PORT_INVITE_CODE_INVALID;
    u8 *session = decoded.session, *xnaddr = decoded.session + SESSION_XNADDR;
    __builtin_memcpy(session + SESSION_XNKID, payload + n, 8);
    n += 8;
    __builtin_memcpy(session + SESSION_XNKEY, payload + n, 16);
    n += 16;
    const unsigned address_flags = payload[n++];
    const unsigned mode = address_flags & 3u, online_size = address_flags >> 3;
    if (mode == 3 || (address_flags & 4u) != 0 || online_size > XNADDR_ONLINE_SIZE ||
        n + 4 + (mode == 2 ? 4 : 0) + 2 + 6 + online_size > end)
        return PORT_INVITE_CODE_INVALID;
    __builtin_memcpy(xnaddr + XNADDR_INA, payload + n, 4);
    n += 4;
    if (mode == 2) {
        __builtin_memcpy(xnaddr + XNADDR_INA_ONLINE, payload + n, 4);
        n += 4;
    } else if (mode == 1) {
        __builtin_memcpy(xnaddr + XNADDR_INA_ONLINE, xnaddr + XNADDR_INA, 4);
    }
    __builtin_memcpy(xnaddr + XNADDR_PORT_ONLINE, payload + n, 2);
    n += 2;
    __builtin_memcpy(xnaddr + XNADDR_ENET, payload + n, 6);
    n += 6;
    __builtin_memcpy(xnaddr + XNADDR_ONLINE, payload + n, online_size);
    n += online_size;
    if ((flags & 4u) != 0) {
        if (n + 8 > end)
            return PORT_INVITE_CODE_INVALID;
        for (int i = 0; i < 8; i++)
            decoded.inviter_xuid = decoded.inviter_xuid << 8 | payload[n++];
    }
    if (n + 1 > end)
        return PORT_INVITE_CODE_INVALID;
    const unsigned long name_size = payload[n++];
    if (name_size >= sizeof(decoded.inviter) || name_size != end - n ||
        !printable_ascii((const char *)payload + n, name_size))
        return PORT_INVITE_CODE_INVALID;
    __builtin_memcpy(decoded.inviter, payload + n, name_size);
    decoded.inviter[name_size] = 0;
    if (!session_valid(decoded.session, decoded.session_class))
        return PORT_INVITE_CODE_INVALID;
    decoded.title_id = PORT_INVITE_TITLE_ID;
    decoded.from_game_invite = 1;
    if (invite != 0)
        *invite = decoded;
    return PORT_INVITE_CODE_OK;
}

PORT_MS_ABI const char *port_invite_code_message(int result)
{
    switch (result) {
    case PORT_INVITE_CODE_OK: return "";
    case PORT_INVITE_CODE_EMPTY: return "Type or paste the lobby code a friend shared.";
    case PORT_INVITE_CODE_PREFIX: return "A lobby code starts with H3-.";
    case PORT_INVITE_CODE_CHARACTER: return "That is not a lobby code: it has a character no code has.";
    case PORT_INVITE_CODE_SHORT: return "That code is too short: part of it is missing.";
    case PORT_INVITE_CODE_LONG: return "That code is too long for a lobby code.";
    case PORT_INVITE_CODE_CHECKSUM: return "That code is damaged: check that it was copied whole.";
    case PORT_INVITE_CODE_VERSION: return "That code was made by a newer version of the game.";
    default: return "That code does not name a lobby.";
    }
}

PORT_MS_ABI void port_invite_code_lines(const char *code, unsigned int groups_per_line, char *out, unsigned int capacity)
{
    if (capacity == 0)
        return;
    // as few lines as `groups_per_line` allows, their groups as even as can be (the first lines one more)
    unsigned long groups = 1;
    for (const char *c = code; *c != 0; c++)
        groups += *c == '-' && c[1] != 0;
    unsigned long base = groups, extra = 0;
    if (groups_per_line != 0) {
        const unsigned long lines = (groups + groups_per_line - 1) / groups_per_line;
        base = groups / lines;
        extra = groups % lines;
    }
    unsigned int at = 0;
    unsigned long line = 0, in_line = 0;
    for (; *code != 0 && at + 2 < capacity; code++) {
        out[at++] = *code;
        if (*code == '-' && code[1] != 0 && ++in_line == base + (line < extra ? 1u : 0u)) {
            out[at++] = '\n';
            line++;
            in_line = 0;
        }
    }
    out[at] = 0;
}

PORT_MS_ABI unsigned int port_invite_lobby_code(unsigned int user_index, char *code, unsigned int capacity, const char **why)
{
    if (code != 0 && capacity != 0)
        code[0] = 0;
    port_invite_lobby lobby;
    const char *reason = "";
    if (!port_invite_current_lobby(&lobby, &reason)) {
        if (why != 0)
            *why = reason;
        return 0;
    }
    port_invite invite;
    __builtin_memset(&invite, 0, sizeof(invite));
    __builtin_memcpy(invite.session, lobby.session, PORT_INVITE_SESSION_SIZE);
    invite.session_class = lobby.session_class;
    if (XUserGetName(user_index, invite.inviter, sizeof(invite.inviter)) != 0)
        invite.inviter[0] = 0;
    // an XUID only for Live (an offline XUID means nothing on another PC)
    unsigned long long xuid = 0;
    if (lobby.session_class == PORT_INVITE_CLASS_LIVE && XUserGetXUID(user_index, &xuid) == 0)
        invite.inviter_xuid = xuid;
    const unsigned int length = port_invite_code_encode(&invite, code, capacity);
    if (why != 0)
        *why = length != 0 ? "" : "The lobby code could not be made.";
    if (length != 0 && port_invite_trace()) {
        trace_line line = { { 0 }, 0 };
        line_add(&line, "invite: lobby code (");
        line_add(&line, class_name(lobby.session_class));
        line_add(&line, " ");
        line_hex(&line, lobby.session + SESSION_XNKID, 8);
        line_add(&line, ", ");
        line_number(&line, length);
        line_add(&line, " characters): ");
        line_add(&line, code);
        line_write(&line);
    }
    return length;
}

// ---- invites received ----
//
// The server delivers an invite until it is answered (live.h), so the ids seen lately are remembered: one delivered
// again is not taken twice (no second toast), and each is answered to the server once. When the inbox is full the
// oldest invite goes and is answered as declined, so the server stops delivering it.

static int same_text(const char *a, const char *b)
{
    while (*a != 0 && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

// Under the lock: the inbox index of `id`, -1 for none.
static long inbox_find(const char *id)
{
    for (unsigned long i = 0; i < g_invite.inbox_count; i++)
        if (same_text(g_invite.inbox[i].id, id))
            return (long)i;
    return -1;
}

static void inbox_remove_at(unsigned long index)
{
    for (unsigned long i = index; i + 1 < g_invite.inbox_count; i++)
        g_invite.inbox[i] = g_invite.inbox[i + 1];
    g_invite.inbox_count--;
}

// Under the lock: the remembered id `id`, 0 for none.
static seen_invite *seen_find(const char *id)
{
    for (unsigned i = 0; i < INVITE_SEEN; i++)
        if (g_invite.seen[i].id[0] != 0 && same_text(g_invite.seen[i].id, id))
            return &g_invite.seen[i];
    return 0;
}

// Under the lock: `id` remembered, in place of the one remembered longest.
static void seen_add(const char *id)
{
    seen_invite *slot = &g_invite.seen[g_invite.seen_next];
    g_invite.seen_next = (g_invite.seen_next + 1u) % INVITE_SEEN;
    text_copy(slot->id, sizeof(slot->id), id);
    slot->answered = 0;
}

// Under the lock: whether `id` is still to be answered to the server (and from now on it is not).
static int seen_answer(const char *id)
{
    seen_invite *seen = seen_find(id);
    if (seen == 0)
        return 1; // remembered no longer: told again, which a server takes as a no-op
    const int first = !seen->answered;
    seen->answered = 1;
    return first;
}

PORT_MS_ABI int port_invite_received(const port_invite *invite)
{
    if (!invite_valid(invite) || invite->id[0] == 0)
        return 0;
    port_invite kept = *invite;
    kept.received = (unsigned int)GetTickCount();
    kept.from_game_invite = 1;
    if (kept.title_id == 0)
        kept.title_id = PORT_INVITE_TITLE_ID;
    char evicted[PORT_INVITE_ID];
    evicted[0] = 0;
    invite_lock();
    const int fresh = seen_find(kept.id) == 0 && inbox_find(kept.id) < 0;
    if (fresh) {
        seen_add(kept.id);
        if (g_invite.inbox_count == PORT_INVITE_INBOX) { // the oldest goes
            if (seen_answer(g_invite.inbox[0].id))
                text_copy(evicted, sizeof(evicted), g_invite.inbox[0].id);
            inbox_remove_at(0);
        }
        g_invite.inbox[g_invite.inbox_count++] = kept;
    }
    invite_unlock();
    if (!fresh)
        return 0;
    if (evicted[0] != 0 && port_live_signed_in())
        port_live_invite_ack(evicted, 0);
    char text[GUIDE_LABEL * 2];
    text_copy(text, sizeof(text), kept.inviter[0] != 0 ? kept.inviter : "A friend");
    unsigned long n = bounded_length(text, sizeof(text));
    text_copy(text + n, sizeof(text) - n, " invited you to play");
    guide_toast_post(GUIDE_TOAST_INVITE, "Game Invite", text, "", "Home / Guide to accept");
    if (port_invite_trace()) {
        trace_line line = { { 0 }, 0 };
        line_add(&line, "invite: received ");
        line_add(&line, kept.id);
        line_add(&line, " from ");
        line_add(&line, kept.inviter);
        line_add(&line, ": ");
        line_add(&line, class_name(kept.session_class));
        line_add(&line, " lobby ");
        line_hex(&line, kept.session + SESSION_XNKID, 8);
        if (evicted[0] != 0) {
            line_add(&line, " (inbox full: ");
            line_add(&line, evicted);
            line_add(&line, " declined)");
        }
        line_write(&line);
    }
    return 1;
}

PORT_MS_ABI unsigned int port_invite_inbox(port_invite *out, unsigned int capacity)
{
    invite_lock();
    unsigned int n = 0;
    for (; n < g_invite.inbox_count && n < capacity; n++)
        out[n] = g_invite.inbox[g_invite.inbox_count - 1 - n];
    invite_unlock();
    return n;
}

PORT_MS_ABI unsigned int port_invite_inbox_count(void)
{
    invite_lock();
    const unsigned int n = g_invite.inbox_count;
    invite_unlock();
    return n;
}

PORT_MS_ABI void port_invite_answered(const char *id, int accepted)
{
    if (id == 0 || id[0] == 0 || bounded_length(id, PORT_INVITE_ID) == PORT_INVITE_ID)
        return;
    invite_lock();
    const long index = inbox_find(id);
    if (index >= 0)
        inbox_remove_at((unsigned long)index);
    const int tell = seen_answer(id);
    invite_unlock();
    if (tell && port_live_signed_in())
        port_live_invite_ack(id, accepted);
}

// ---- accepting ----

PORT_MS_ABI int port_invite_check(unsigned int user_index, const port_invite *invite)
{
    if (!invite_valid(invite))
        return PORT_INVITE_E_INVALID;
    if (user_index >= PORT_PROFILE_USERS || !port_profile_signed_in(user_index))
        return PORT_INVITE_E_NOT_SIGNED_IN;
    port_invite_lobby own;
    if (port_invite_current_lobby(&own, 0) &&
        same_bytes(own.session + SESSION_XNKID, invite->session + SESSION_XNKID, 8))
        return PORT_INVITE_E_OWN;
    if (invite->session_class == PORT_INVITE_CLASS_SYSTEM_LINK && !port_lan_active())
        return PORT_INVITE_E_SYSTEM_LINK;
    if (invite->session_class == PORT_INVITE_CLASS_LIVE && XUserGetSigninState(user_index) != 2)
        return PORT_INVITE_E_LIVE; // eXUserSigninState_SignedInToLive: the game can create a Live session for them
    return PORT_INVITE_OK;
}

PORT_MS_ABI const char *port_invite_error_message(int error)
{
    switch (error) {
    case PORT_INVITE_OK: return "";
    case PORT_INVITE_E_NOT_SIGNED_IN: return "Sign in to a profile first.";
    case PORT_INVITE_E_SYSTEM_LINK: return "That is a System Link lobby, and System Link is off on this PC.";
    case PORT_INVITE_E_LIVE: return "That is a Live lobby: sign in to Live to join it.";
    case PORT_INVITE_E_OWN: return "That is your own lobby.";
    default: return "That is not a lobby this game can join.";
    }
}

PORT_MS_ABI int port_invite_accept(unsigned int user_index, const port_invite *invite)
{
    const int error = port_invite_check(user_index, invite);
    if (error != PORT_INVITE_OK) {
        if (port_invite_trace()) {
            trace_line line = { { 0 }, 0 };
            line_add(&line, "invite: user ");
            line_number(&line, user_index);
            line_add(&line, " cannot accept: ");
            line_add(&line, port_invite_error_message(error));
            line_write(&line);
        }
        return error;
    }
    port_invite accepted = *invite;
    accepted.from_game_invite = 1;
    accepted.title_id = PORT_INVITE_TITLE_ID;
    unsigned long long xuid = 0;
    accepted.invitee_xuid = XUserGetXUID(user_index, &xuid) == 0 ? xuid : 0;
    invite_lock();
    g_invite.accepted[user_index] = accepted;
    g_invite.has_accepted[user_index] = 1;
    invite_unlock();
    if (port_invite_trace()) {
        trace_line line = { { 0 }, 0 };
        line_add(&line, "invite: user ");
        line_number(&line, user_index);
        line_add(&line, " accepted ");
        line_add(&line, class_name(accepted.session_class));
        line_add(&line, " lobby ");
        line_hex(&line, accepted.session + SESSION_XNKID, 8);
        line_add(&line, " of ");
        line_add(&line, accepted.inviter[0] != 0 ? accepted.inviter : "(unknown)");
        line_add(&line, accepted.id[0] != 0 ? " (Live invite)" : " (lobby code)");
        line_add(&line, ": XN_LIVE_INVITE_ACCEPTED");
        line_write(&line);
    }
    port_xam_invite_accepted(user_index);
    port_invite_answered(accepted.id, 1);
    return PORT_INVITE_OK;
}

PORT_MS_ABI int port_invite_accepted(unsigned int user_index, port_invite *invite)
{
    if (user_index >= PORT_PROFILE_USERS)
        return 0;
    invite_lock();
    const int there = g_invite.has_accepted[user_index];
    if (there && invite != 0)
        *invite = g_invite.accepted[user_index];
    invite_unlock();
    return there;
}
