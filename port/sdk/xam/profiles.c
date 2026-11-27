// port/sdk/xam (both hosts): the console's local profiles and the users signed in to them (xam_profile.h).
//
// The catalog is the four built-in profiles (none with HALO3_NO_PROFILE) followed by the ones the guide created, which
// the data directory keeps as text (port/sdk/kernel/storage.h, rewritten atomically on every change):
//   profiles/catalog.txt   "H3PROFILES 1\n", then a line per created profile: its offline XUID (16 hex digits), a
//                          space, its gamertag.
// It holds at most PROFILE_CREATED_MAX profiles, so every run lists them all after the built-in ones. A line whose
// name or XUID a profile listed before it has (a built-in one under HALO3_PROFILE_NAME, say) is not listed that run but
// stays in the file. A catalog file that does not check out is set aside as catalog.txt.corrupt (the built-in profiles
// stay usable); one that cannot be read is left alone, and no profile can be created until it can be (creating
// rewrites it). The state is read on first use (the environment, the file) and then kept under a lock; the guide's
// thread and the game's read and change it.
#ifdef _WIN32
__declspec(dllimport) char *__cdecl getenv(const char *name);
#else
char *getenv(const char *name);
#endif

#include "xam_profile.h"
#include "../kernel/storage.h"
#include "../online/live.h"
#include "../runtime/port_config.h"
#include "../runtime/port_runtime.h"

#define PROFILE_ERROR_SUCCESS 0ul
#define PROFILE_ERROR_FILE_NOT_FOUND 2ul
#define PROFILE_ERROR_PATH_NOT_FOUND 3ul
#define PROFILE_ERROR_TOO_MANY_NAMES 68ul
#define PROFILE_ERROR_INSUFFICIENT_BUFFER 122ul
#define PROFILE_ERROR_INVALID_PARAMETER 87ul
#define PROFILE_ERROR_INVALID_NAME 123ul
#define PROFILE_ERROR_ALREADY_EXISTS 183ul
#define PROFILE_ERROR_ALREADY_ASSIGNED 85ul
#define PROFILE_ERROR_NOT_FOUND 1168ul
#define PROFILE_ERROR_NOT_LOGGED_ON 1245ul
#define PROFILE_ERROR_NO_SUCH_USER 1317ul

#define PROFILE_CATALOG_FILE "profiles/catalog.txt"
#define PROFILE_CATALOG_HEADER "H3PROFILES 1\n"
#define PROFILE_CATALOG_LINE (16 + 1 + PORT_PROFILE_GAMERTAG_MAX + 1)
#define PROFILE_CREATED_MAX (PORT_PROFILE_CATALOG_MAX - PORT_PROFILE_BUILTIN) // the file's lines
#define PROFILE_CATALOG_BYTES (sizeof(PROFILE_CATALOG_HEADER) + PROFILE_CREATED_MAX * PROFILE_CATALOG_LINE)

typedef unsigned long long profile_qword;

typedef struct {
    char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1];
    profile_qword xuid;
} profile_record;

typedef struct {
    int kind;
    unsigned long profile, sponsor, guest_number, serial;
} profile_user;

static struct {
    volatile long ready; // 0 not read, 1 being read, 2 ready
    volatile char busy;
    profile_record catalog[PORT_PROFILE_CATALOG_MAX];
    unsigned long count;   // profiles in the catalog
    unsigned long builtin; // of which built in: PORT_PROFILE_BUILTIN, or 0 with HALO3_NO_PROFILE
    profile_record file[PROFILE_CREATED_MAX]; // the catalog file's lines, listed or not (rewritten from these)
    unsigned long file_count;
    unsigned long file_error; // reading the file failed (not "there is none"): creating would overwrite it
    volatile char creating;   // port_profile_create runs: one at a time
    profile_user users[PORT_PROFILE_USERS];
    unsigned long last_serial;
} g_profiles;

static void profiles_lock(void)
{
    while (__atomic_test_and_set(&g_profiles.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void profiles_unlock(void) { __atomic_clear(&g_profiles.busy, __ATOMIC_RELEASE); }

static unsigned long text_length(const char *text)
{
    unsigned long n = 0;
    while (text[n] != 0)
        n++;
    return n;
}

static char lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c - 'A' + 'a') : c; }

static int same_gamertag(const char *a, const char *b)
{
    for (; *a != 0 && lower(*a) == lower(*b); a++, b++)
        ;
    return lower(*a) == lower(*b);
}

static profile_qword fnv1a(profile_qword hash, const char *text)
{
    for (; *text != 0; text++)
        hash = (hash ^ (unsigned char)*text) * 0x100000001b3ull;
    return hash;
}

// The offline XUID derived from a gamertag (the 0xE000... range, bit 1 set so never PORT_PROFILE_OFFLINE_XUID): a
// created profile's, and the built-in Player 1's under HALO3_PROFILE_NAME.
static profile_qword gamertag_xuid(const char *gamertag)
{
    return 0xE000000000000000ull | (fnv1a(0xcbf29ce484222325ull, gamertag) & 0x0000ffffffffffffull) | 2;
}

int port_profile_gamertag_valid(const char *gamertag)
{
    if (gamertag == 0 || !((gamertag[0] >= 'a' && gamertag[0] <= 'z') || (gamertag[0] >= 'A' && gamertag[0] <= 'Z')))
        return 0;
    unsigned long n = 0;
    for (; gamertag[n] != 0; n++) {
        const char c = gamertag[n];
        const int letter_or_digit = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        if (n >= PORT_PROFILE_GAMERTAG_MAX || !(letter_or_digit || (c == ' ' && gamertag[n - 1] != ' ')))
            return 0;
    }
    return gamertag[n - 1] != ' ';
}

// ---- reading the state (once) ----

// HALO3_PROFILE_NAME: letters, digits and spaces, at most 15; anything else is dropped. Empty: "Player 1".
static void builtin_player_one(char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1], int *named)
{
    const char *name = getenv("HALO3_PROFILE_NAME");
    unsigned long n = 0;
    for (; name != 0 && *name != 0 && n < PORT_PROFILE_GAMERTAG_MAX; name++) {
        const char c = *name;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == ' ' && n != 0))
            gamertag[n++] = c;
    }
    while (n != 0 && gamertag[n - 1] == ' ')
        n--;
    gamertag[n] = 0;
    *named = n != 0;
    if (!*named)
        __builtin_memcpy(gamertag, "Player 1", 9);
}

static int hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return c >= 'a' && c <= 'f' ? c - 'a' + 10 : -1;
}

static void catalog_warning(const char *what, unsigned long error)
{
    char line[160] = "port: profiles: ";
    unsigned long n = text_length(line);
    for (; *what != 0 && n + 24 < sizeof(line); what++)
        line[n++] = *what;
    if (error != 0) {
        const char *prefix = " (error ";
        for (; *prefix != 0; prefix++)
            line[n++] = *prefix;
        char digits[12];
        unsigned long d = 0;
        do {
            digits[d++] = (char)('0' + error % 10);
            error /= 10;
        } while (error != 0);
        while (d != 0)
            line[n++] = digits[--d];
        line[n++] = ')';
    }
    line[n++] = '\n';
    line[n] = 0;
    port_write_error(line);
}

// Parses the catalog file into file[]: returns its lines, or -1 when the file does not check out.
static long catalog_parse(const char *text, unsigned long size, profile_record file[PROFILE_CREATED_MAX])
{
    const unsigned long header = sizeof(PROFILE_CATALOG_HEADER) - 1;
    if (size < header || __builtin_memcmp(text, PROFILE_CATALOG_HEADER, header) != 0)
        return -1;
    unsigned long at = header, count = 0;
    while (at < size) {
        if (count == PROFILE_CREATED_MAX || size - at < 19)
            return -1;
        profile_qword xuid = 0;
        for (int i = 0; i < 16; i++) {
            const int digit = hex_digit(text[at + i]);
            if (digit < 0)
                return -1;
            xuid = xuid << 4 | (profile_qword)digit;
        }
        if (text[at + 16] != ' ' || (xuid >> 60) != 0xE)
            return -1;
        at += 17;
        char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1];
        unsigned long n = 0;
        while (at < size && text[at] != '\n' && n < PORT_PROFILE_GAMERTAG_MAX)
            gamertag[n++] = text[at++];
        gamertag[n] = 0;
        if (at == size || text[at] != '\n' || !port_profile_gamertag_valid(gamertag))
            return -1;
        at++;
        __builtin_memcpy(file[count].gamertag, gamertag, n + 1);
        file[count].xuid = xuid;
        count++;
    }
    return (long)count;
}

// Whether a profile in records[0..count) has `gamertag` (letter case ignored) or `xuid`.
static int name_taken(const profile_record *records, unsigned long count, const char *gamertag, profile_qword xuid)
{
    for (unsigned long i = 0; i < count; i++)
        if (same_gamertag(records[i].gamertag, gamertag) || records[i].xuid == xuid)
            return 1;
    return 0;
}

static void profiles_read(void)
{
    const char *no_profile = getenv("HALO3_NO_PROFILE");
    const int none = no_profile != 0 && no_profile[0] != '\0' && no_profile[0] != '0';
    unsigned long count = 0;
    if (!none) {
        // the built-in profiles: Player 1 (PORT_PROFILE_OFFLINE_XUID, or its name's XUID under HALO3_PROFILE_NAME), then
        // Players 2..4, whose XUIDs derive from Player 1's gamertag and their number (so the second players of two
        // machines with distinct gamertags, System Link, are distinct too)
        int named = 0;
        builtin_player_one(g_profiles.catalog[0].gamertag, &named);
        g_profiles.catalog[0].xuid = named ? gamertag_xuid(g_profiles.catalog[0].gamertag) : PORT_PROFILE_OFFLINE_XUID;
        const profile_qword base = fnv1a(0xcbf29ce484222325ull, g_profiles.catalog[0].gamertag);
        for (unsigned long n = 1; n < PORT_PROFILE_BUILTIN; n++) {
            __builtin_memcpy(g_profiles.catalog[n].gamertag, "Player 2", 9);
            g_profiles.catalog[n].gamertag[7] = (char)('1' + n);
            const char suffix[3] = { '/', (char)('0' + n), 0 };
            g_profiles.catalog[n].xuid =
                0xE000000000000000ull | (fnv1a(base, suffix) & 0x0000ffffffffffffull) | 2;
        }
        count = PORT_PROFILE_BUILTIN;
    }
    g_profiles.builtin = count;

    static char text[PROFILE_CATALOG_BYTES + 1];
    unsigned int size = 0;
    const unsigned int result = port_storage_data_read(PROFILE_CATALOG_FILE, text, sizeof(text) - 1, &size);
    // (larger than a catalog can be: it does not check out either)
    const long lines = result == PROFILE_ERROR_SUCCESS ? catalog_parse(text, size, g_profiles.file)
                     : result == PROFILE_ERROR_INSUFFICIENT_BUFFER ? -1 : 0;
    if (lines >= 0) {
        g_profiles.file_count = (unsigned long)lines;
    } else {
        const unsigned long aside = port_storage_data_set_aside(PROFILE_CATALOG_FILE);
        catalog_warning(aside == PROFILE_ERROR_SUCCESS
            ? "profiles/catalog.txt does not check out; set aside as catalog.txt.corrupt"
            : "profiles/catalog.txt does not check out and could not be set aside", aside);
        if (aside != PROFILE_ERROR_SUCCESS)
            g_profiles.file_error = aside; // still there: never overwritten
    }
    if (result != PROFILE_ERROR_SUCCESS && result != PROFILE_ERROR_INSUFFICIENT_BUFFER &&
        result != PROFILE_ERROR_FILE_NOT_FOUND && result != PROFILE_ERROR_PATH_NOT_FOUND) {
        g_profiles.file_error = result;
        catalog_warning("cannot read profiles/catalog.txt; only the built-in profiles are listed", result);
    }
    for (unsigned long i = 0; i < g_profiles.file_count; i++) {
        const profile_record *line = &g_profiles.file[i];
        if (name_taken(g_profiles.catalog, count, line->gamertag, line->xuid))
            catalog_warning("a created profile has the name or XUID of one listed before it; not listed (kept)", 0);
        else
            g_profiles.catalog[count++] = *line;
    }
    g_profiles.count = count;

    // users 0..HALO3_LOCAL_PLAYERS-1 signed in to the built-in profiles of the same number
    if (!none) {
        const char *players = getenv("HALO3_LOCAL_PLAYERS");
        unsigned long signed_in = 1;
        if (players != 0 && players[0] >= '1' && players[0] <= '0' + (char)PORT_PROFILE_USERS && players[1] == '\0')
            signed_in = (unsigned long)(players[0] - '0');
        for (unsigned long user = 0; user < signed_in; user++) {
            g_profiles.users[user].kind = PORT_SIGNIN_PROFILE;
            g_profiles.users[user].profile = user;
            g_profiles.users[user].serial = ++g_profiles.last_serial;
        }
        const char *guests = getenv("HALO3_LOCAL_GUESTS");
        if (guests != 0 && guests[0] >= '1' && guests[0] <= '3' && guests[1] == '\0') {
            const unsigned long guest_count = (unsigned long)(guests[0] - '0');
            for (unsigned long g = 1; g <= guest_count && g < PORT_PROFILE_USERS; g++) {
                g_profiles.users[g].kind = PORT_SIGNIN_GUEST;
                g_profiles.users[g].sponsor = 0;
                g_profiles.users[g].guest_number = g;
                g_profiles.users[g].serial = ++g_profiles.last_serial;
            }
        }
    }
}

// The state, read on first use; then locked. Every caller of profiles_enter calls profiles_leave.
static void profiles_enter(void)
{
    if (__atomic_load_n(&g_profiles.ready, __ATOMIC_ACQUIRE) != 2) {
        long expected = 0;
        if (__atomic_compare_exchange_n(&g_profiles.ready, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            profiles_read();
            __atomic_store_n(&g_profiles.ready, 2, __ATOMIC_RELEASE);
        }
        while (__atomic_load_n(&g_profiles.ready, __ATOMIC_ACQUIRE) != 2)
            __builtin_ia32_pause();
    }
    profiles_lock();
}
static void profiles_leave(void) { profiles_unlock(); }

// ---- users (under the lock) ----

static int user_signed_in(unsigned long user) { return g_profiles.users[user].kind != PORT_SIGNIN_NONE; }

static unsigned long signed_in_mask(void)
{
    unsigned long mask = 0;
    for (unsigned long user = 0; user < PORT_PROFILE_USERS; user++)
        if (user_signed_in(user))
            mask |= 1ul << user;
    return mask;
}

// Signs `user` and the guests it sponsors out; returns their bits.
static unsigned long sign_out_locked(unsigned long user)
{
    port_live_sign_out(user);
    unsigned long changed = 0;
    if (g_profiles.users[user].kind == PORT_SIGNIN_PROFILE)
        for (unsigned long guest = 0; guest < PORT_PROFILE_USERS; guest++)
            if (g_profiles.users[guest].kind == PORT_SIGNIN_GUEST && g_profiles.users[guest].sponsor == user) {
                __builtin_memset(&g_profiles.users[guest], 0, sizeof(g_profiles.users[guest]));
                changed |= 1ul << guest;
            }
    if (user_signed_in(user)) {
        __builtin_memset(&g_profiles.users[user], 0, sizeof(g_profiles.users[user]));
        changed |= 1ul << user;
    }
    return changed;
}

static long guest_sponsor_locked(unsigned long user)
{
    for (unsigned long sponsor = 0; sponsor < PORT_PROFILE_USERS; sponsor++)
        if (sponsor != user && g_profiles.users[sponsor].kind == PORT_SIGNIN_PROFILE)
            return (long)sponsor;
    return -1;
}

void port_profile_guest_name(const char *sponsor, unsigned long guest_number, char name[PORT_PROFILE_GAMERTAG_MAX + 1])
{
    const char suffix[5] = { ' ', '(', (char)('0' + guest_number % 10), ')', 0 };
    unsigned long n = text_length(sponsor);
    if (n > PORT_PROFILE_GAMERTAG_MAX - 4)
        n = PORT_PROFILE_GAMERTAG_MAX - 4;
    while (n != 0 && sponsor[n - 1] == ' ')
        n--; // a sponsor's name cut at a space would leave two spaces
    __builtin_memcpy(name, sponsor, n);
    __builtin_memcpy(name + n, suffix, sizeof(suffix));
}

static void user_info_locked(unsigned long user, port_profile_user_info *info)
{
    __builtin_memset(info, 0, sizeof(*info));
    const profile_user *record = &g_profiles.users[user];
    info->kind = record->kind;
    if (record->kind == PORT_SIGNIN_NONE)
        return;
    info->serial = record->serial;
    if (record->kind == PORT_SIGNIN_PROFILE) {
        info->profile = record->profile;
        info->xuid = g_profiles.catalog[record->profile].xuid;
        __builtin_memcpy(info->name, g_profiles.catalog[record->profile].gamertag,
            text_length(g_profiles.catalog[record->profile].gamertag) + 1);
        return;
    }
    const profile_user *sponsor = &g_profiles.users[record->sponsor];
    info->sponsor = record->sponsor;
    info->guest_number = record->guest_number;
    info->profile = sponsor->profile;
    info->xuid = g_profiles.catalog[sponsor->profile].xuid | ((profile_qword)record->guest_number << 54);
    port_profile_guest_name(g_profiles.catalog[sponsor->profile].gamertag, record->guest_number, info->name);
}

PORT_MS_ABI int port_profile_signed_in(unsigned long user_index)
{
    if (user_index >= PORT_PROFILE_USERS)
        return 0;
    profiles_enter();
    const int signed_in = user_signed_in(user_index);
    profiles_leave();
    return signed_in;
}

unsigned long port_profile_signed_in_mask(void)
{
    profiles_enter();
    const unsigned long mask = signed_in_mask();
    profiles_leave();
    return mask;
}

int port_profile_user(unsigned long user_index, port_profile_user_info *info)
{
    __builtin_memset(info, 0, sizeof(*info));
    if (user_index >= PORT_PROFILE_USERS)
        return 0;
    profiles_enter();
    user_info_locked(user_index, info);
    profiles_leave();
    return info->kind != PORT_SIGNIN_NONE;
}

PORT_MS_ABI int port_profile_user_is_guest(unsigned long user_index)
{
    port_profile_user_info info;
    port_profile_user(user_index, &info);
    return info.kind == PORT_SIGNIN_GUEST;
}

PORT_MS_ABI unsigned long long port_profile_user_xuid(unsigned long user_index)
{
    port_profile_user_info info;
    port_profile_user(user_index, &info);
    return info.xuid;
}

PORT_MS_ABI unsigned long port_profile_user_serial(unsigned long user_index)
{
    port_profile_user_info info;
    port_profile_user(user_index, &info);
    return info.serial;
}

unsigned long port_profile_sign_in(unsigned long user_index, unsigned long profile, unsigned long *changed)
{
    if (changed != 0)
        *changed = 0;
    if (user_index >= PORT_PROFILE_USERS)
        return PROFILE_ERROR_INVALID_PARAMETER;
    profiles_enter();
    unsigned long result = PROFILE_ERROR_SUCCESS, bits = 0;
    if (g_profiles.count == 0) {
        result = PROFILE_ERROR_NOT_FOUND;
    } else if (profile >= g_profiles.count) {
        result = PROFILE_ERROR_INVALID_PARAMETER;
    } else {
        profile_user *record = &g_profiles.users[user_index];
        if (record->kind == PORT_SIGNIN_PROFILE && record->profile == profile) {
            // already signed in to it: nothing changes
        } else {
            for (unsigned long other = 0; other < PORT_PROFILE_USERS && result == PROFILE_ERROR_SUCCESS; other++)
                if (other != user_index && g_profiles.users[other].kind == PORT_SIGNIN_PROFILE &&
                    g_profiles.users[other].profile == profile)
                    result = PROFILE_ERROR_ALREADY_ASSIGNED;
            if (result == PROFILE_ERROR_SUCCESS) {
                bits = sign_out_locked(user_index) | 1ul << user_index;
                record->kind = PORT_SIGNIN_PROFILE;
                record->profile = profile;
                record->serial = ++g_profiles.last_serial;
            }
        }
    }
    profiles_leave();
    if (changed != 0)
        *changed = bits;
    return result;
}

unsigned long port_profile_sign_in_guest(unsigned long user_index, unsigned long *changed)
{
    if (changed != 0)
        *changed = 0;
    if (user_index >= PORT_PROFILE_USERS)
        return PROFILE_ERROR_INVALID_PARAMETER;
    profiles_enter();
    unsigned long result = PROFILE_ERROR_SUCCESS, bits = 0;
    const long sponsor = guest_sponsor_locked(user_index);
    profile_user *record = &g_profiles.users[user_index];
    if (sponsor < 0) {
        result = PROFILE_ERROR_NO_SUCH_USER;
    } else if (!(record->kind == PORT_SIGNIN_GUEST && record->sponsor == (unsigned long)sponsor)) {
        unsigned long used = 0; // a bit per guest number the sponsor has given out
        for (unsigned long other = 0; other < PORT_PROFILE_USERS; other++)
            if (other != user_index && g_profiles.users[other].kind == PORT_SIGNIN_GUEST &&
                g_profiles.users[other].sponsor == (unsigned long)sponsor)
                used |= 1ul << g_profiles.users[other].guest_number;
        unsigned long number = 1;
        while (number <= PORT_PROFILE_GUESTS && (used & (1ul << number)) != 0)
            number++;
        if (number > PORT_PROFILE_GUESTS) {
            result = PROFILE_ERROR_TOO_MANY_NAMES;
        } else {
            bits = sign_out_locked(user_index) | 1ul << user_index;
            record->kind = PORT_SIGNIN_GUEST;
            record->sponsor = (unsigned long)sponsor;
            record->guest_number = number;
            record->serial = ++g_profiles.last_serial;
        }
    }
    profiles_leave();
    if (changed != 0)
        *changed = bits;
    return result;
}

unsigned long port_profile_sign_out(unsigned long user_index, unsigned long *changed)
{
    if (changed != 0)
        *changed = 0;
    if (user_index >= PORT_PROFILE_USERS)
        return PROFILE_ERROR_INVALID_PARAMETER;
    profiles_enter();
    const unsigned long bits = sign_out_locked(user_index);
    profiles_leave();
    if (changed != 0)
        *changed = bits;
    return bits != 0 ? PROFILE_ERROR_SUCCESS : PROFILE_ERROR_NOT_LOGGED_ON;
}

long port_profile_guest_sponsor(unsigned long user_index)
{
    profiles_enter();
    const long sponsor = guest_sponsor_locked(user_index);
    profiles_leave();
    return sponsor;
}

// ---- the catalog ----

unsigned long port_profile_count(void)
{
    profiles_enter();
    const unsigned long count = g_profiles.count;
    profiles_leave();
    return count;
}

int port_profile_get(unsigned long index, char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1], unsigned long long *xuid)
{
    profiles_enter();
    const int found = index < g_profiles.count;
    if (found) {
        if (gamertag != 0)
            __builtin_memcpy(gamertag, g_profiles.catalog[index].gamertag,
                text_length(g_profiles.catalog[index].gamertag) + 1);
        if (xuid != 0)
            *xuid = g_profiles.catalog[index].xuid;
    }
    profiles_leave();
    if (!found) {
        if (gamertag != 0)
            gamertag[0] = 0;
        if (xuid != 0)
            *xuid = 0;
    }
    return found;
}

long port_profile_signed_in_on(unsigned long index)
{
    profiles_enter();
    long user = -1;
    for (unsigned long n = 0; n < PORT_PROFILE_USERS && user < 0; n++)
        if (g_profiles.users[n].kind == PORT_SIGNIN_PROFILE && g_profiles.users[n].profile == index)
            user = (long)n;
    profiles_leave();
    return user;
}

static void put_hex(char *out, profile_qword value)
{
    for (int i = 0; i < 16; i++)
        out[i] = "0123456789ABCDEF"[(value >> (60 - 4 * i)) & 15];
}

static unsigned long put_line(char *out, const profile_record *record)
{
    put_hex(out, record->xuid);
    out[16] = ' ';
    const unsigned long n = text_length(record->gamertag);
    __builtin_memcpy(out + 17, record->gamertag, n);
    out[17 + n] = '\n';
    return 17 + n + 1;
}

static unsigned long create_error_locked(const char *gamertag, profile_qword xuid)
{
    if (g_profiles.file_error != PROFILE_ERROR_SUCCESS)
        return g_profiles.file_error;
    if (name_taken(g_profiles.catalog, g_profiles.count, gamertag, xuid) ||
        name_taken(g_profiles.file, g_profiles.file_count, gamertag, xuid))
        return PROFILE_ERROR_ALREADY_EXISTS;
    if (g_profiles.file_count == PROFILE_CREATED_MAX || g_profiles.count == PORT_PROFILE_CATALOG_MAX)
        return PROFILE_ERROR_TOO_MANY_NAMES;
    return PROFILE_ERROR_SUCCESS;
}

unsigned long port_profile_create_error(const char *gamertag)
{
    if (!port_profile_gamertag_valid(gamertag))
        return PROFILE_ERROR_INVALID_NAME;
    profiles_enter();
    const unsigned long result = create_error_locked(gamertag, gamertag_xuid(gamertag));
    profiles_leave();
    return result;
}

unsigned long port_profile_create(const char *gamertag, unsigned long *index)
{
    if (index != 0)
        *index = 0;
    if (!port_profile_gamertag_valid(gamertag))
        return PROFILE_ERROR_INVALID_NAME;
    profile_record record;
    __builtin_memcpy(record.gamertag, gamertag, text_length(gamertag) + 1);
    record.xuid = gamertag_xuid(gamertag);
    // one create at a time: the file is rewritten (outside the state's lock) from the lines it has and this one
    while (__atomic_test_and_set(&g_profiles.creating, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    static char text[PROFILE_CATALOG_BYTES];
    unsigned long size = sizeof(PROFILE_CATALOG_HEADER) - 1, result = PROFILE_ERROR_SUCCESS;
    __builtin_memcpy(text, PROFILE_CATALOG_HEADER, size);
    profiles_enter();
    result = create_error_locked(gamertag, record.xuid);
    for (unsigned long i = 0; i < g_profiles.file_count && result == PROFILE_ERROR_SUCCESS; i++)
        size += put_line(text + size, &g_profiles.file[i]);
    profiles_leave();
    if (result == PROFILE_ERROR_SUCCESS) {
        size += put_line(text + size, &record);
        result = port_storage_data_write(PROFILE_CATALOG_FILE, text, size);
        if (result != PROFILE_ERROR_SUCCESS)
            catalog_warning("cannot save profiles/catalog.txt; the profile was not created", result);
    }
    unsigned long created = 0;
    if (result == PROFILE_ERROR_SUCCESS) {
        profiles_enter();
        g_profiles.file[g_profiles.file_count++] = record;
        created = g_profiles.count++; // room: at most PORT_PROFILE_BUILTIN + PROFILE_CREATED_MAX are listed
        g_profiles.catalog[created] = record;
        profiles_leave();
    }
    __atomic_clear(&g_profiles.creating, __ATOMIC_RELEASE);
    if (result == PROFILE_ERROR_SUCCESS && index != 0)
        *index = created;
    return result;
}

void port_profile_suggest_gamertag(char gamertag[PORT_PROFILE_GAMERTAG_MAX + 1])
{
    profiles_enter();
    for (unsigned long number = 5; number < 100; number++) {
        __builtin_memcpy(gamertag, "Player ", 7);
        unsigned long n = 7;
        if (number >= 10)
            gamertag[n++] = (char)('0' + number / 10);
        gamertag[n++] = (char)('0' + number % 10);
        gamertag[n] = 0;
        int used = 0;
        for (unsigned long i = 0; i < g_profiles.count && !used; i++)
            used = same_gamertag(g_profiles.catalog[i].gamertag, gamertag);
        if (!used)
            break;
    }
    profiles_leave();
}

PORT_MS_ABI int port_coop_4p_enabled(void) { return port_config_coop_4p(); }
