// port/sdk/online (both hosts): the Xbox Live, guide, content and session calls the game makes, answered as an Xbox
// answers them with no Xbox Live connection, local (offline) profiles signed in on users 0..3 (port/sdk/xam/xam.c)
// and its hard drive attached; or, with Xbox Live (HALO3_LIVE_SERVER, port/sdk/online/live.h), as one signed in to
// Live through that server. Built like the game, against the XDK headers, so every signature is the XDK's.
//
// Asynchronous calls (an XOVERLAPPED given) complete before they return: the result goes into the XOVERLAPPED
// (InternalLow last, which is what XHasOverlappedIoCompleted watches; InternalHigh the count; dwExtendedError the
// HRESULT), its event is set, its completion routine is queued to run from the thread's next alertable wait
// (xapi_io.h), and the call returns ERROR_IO_PENDING, as the XDK's do once they have queued their work. Without an
// XOVERLAPPED the result is returned. What each call answers:
//   - Xbox Live services (stats, friends, invites, marketplace, string verification, session arbitration,
//     matchmaking search, title servers): ERROR_NOT_LOGGED_ON / WSAENETDOWN, as with no user signed in to Live. With
//     Live: the title servers (LSP), matchmaking search, arbitration, statistics reads and writes, game invites sent
//     (XInviteSend) and service QoS go to the Live server (live.c); the others still fail;
//   - game invites accepted in the guide, Live's or a lobby code's (port/sdk/online/invite.h): XInviteGetAcceptedInfo
//     reads the last one a user accepted; the sessions created, modified, migrated and deleted are reported there (the
//     presence session is what invites name); an achievement newly earned shows the Achievement Unlocked toast
//     (port/sdk/xam/guide_ingame.h);
//   - each local profile (as a console keeps it on its hard drive, here in the data directory, port/sdk/kernel/
//     storage.h, under profiles/<offline XUID>/, with or without Live): its profile settings (profile_settings.bin)
//     and the achievements it has earned (achievements.bin), each rewritten atomically whenever the title changes it;
//     the title's achievements themselves (ids, gamerscore, strings) are its SPA, the XDBF resource of default.xex;
//   - the marketplace: it needs an Xbox Live account with a marketplace, so its dialogs complete as cancelled by the
//     user and the others report that no user is signed in. The guide's other calls (sign-in, message boxes, the
//     device selector, achievements, friends, gamercards, ...) are guide_ui.cpp's (port/sdk/xam/guide.h);
//   - content: the hard drive (port/sdk/kernel/content.h): packages are host directories, opened as drives;
//   - local sessions (XSessionCreate and the calls on a session handle): kept as local state, which is all an
//     offline or System Link Xbox has (the host's address is this console's XNADDR once System Link is up,
//     port/sdk/online/lan.c); without Live the Live-only ones (arbitration, statistics) fail. With Live a session
//     id is an online one, its key is registered with XNet, and the Live server is told of the sessions this console
//     hosts (create, modify, start, end, migrate, delete), of remote members joining and leaving, and arbitrates.
// XNet itself, its QoS listener statistics included, is port/sdk/online/xnet.c; the guide's virtual keyboard
// (XShowKeyboardUI) is port/sdk/online/keyboard_ui.cpp.
#include <xtl.h>
#include <xonline.h>
#include <winsockx.h>
#include <stdlib.h>

#include "../kernel/content.h"
#include "../kernel/storage.h"
#include "invite.h"
#include "live.h"
#include "../kernel/xapi_io.h"
#include "../runtime/port_runtime.h"
#include "../xam/guide.h"
#include "../xam/guide_ingame.h"
#include "../xam/xam_profile.h"

namespace {

// The completion of an XOVERLAPPED: InternalLow the result, InternalHigh the count, dwExtendedError `extended`; then
// its event, and its completion routine queued to `thread` (0: the calling thread), which issued the call. Once
// InternalLow is stored the title may recycle the XOVERLAPPED, so what is still needed from it is read first.
DWORD complete_with(PXOVERLAPPED overlapped, DWORD result, DWORD count, DWORD extended, DWORD thread = 0)
{
    const HANDLE event = overlapped->hEvent;
    const PXOVERLAPPED_COMPLETION_ROUTINE routine = overlapped->pCompletionRoutine;
    overlapped->InternalHigh = count;
    overlapped->dwExtendedError = extended;
    __atomic_store_n(&overlapped->InternalLow, (ULONG_PTR)result, __ATOMIC_RELEASE);
    if (event != 0)
        SetEvent(event);
    const DWORD issuer = thread != 0 ? thread : GetCurrentThreadId();
    if (routine != 0 && !port_apc_queue_thread(issuer, (void *)routine, result, count, overlapped))
        port_write_error("port: an XOVERLAPPED completion routine queue is full; the routine is dropped\n");
    return ERROR_IO_PENDING;
}

DWORD complete(PXOVERLAPPED overlapped, DWORD result, DWORD count = 0)
{
    if (overlapped == 0)
        return result;
    return complete_with(overlapped, result, count, result == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(result));
}

// How XAM completes the content and device calls: a failure is ERROR_FUNCTION_FAILED, its cause the HRESULT of the
// extended error.
DWORD complete_xam(PXOVERLAPPED overlapped, DWORD result, DWORD count = 0, DWORD thread = 0)
{
    if (overlapped == 0)
        return result;
    if (result == ERROR_SUCCESS)
        return complete_with(overlapped, ERROR_SUCCESS, count, S_OK, thread);
    return complete_with(overlapped, ERROR_FUNCTION_FAILED, 0, HRESULT_FROM_WIN32(result), thread);
}

// HALO3_LIVE_LOG: how long a call that may reach the Live server held its caller (live.h).
struct s_live_trace {
    const char *what;
    unsigned long long start;
    explicit s_live_trace(const char *name) : what(name), start(port_live_trace_start()) {}
    ~s_live_trace() { port_live_trace_end(what, "returned", start); }
};

// A critical section made on first use (these calls come before any initialization the port could run).
struct s_lazy_lock {
    CRITICAL_SECTION section;
    volatile LONG ready;
};

void lock_enter(s_lazy_lock *lock)
{
    if (lock->ready != 2) {
        if (InterlockedCompareExchange(&lock->ready, 1, 0) == 0) {
            InitializeCriticalSection(&lock->section);
            InterlockedExchange(&lock->ready, 2);
        }
        while (lock->ready != 2)
            SwitchToThread();
    }
    EnterCriticalSection(&lock->section);
}

void lock_leave(s_lazy_lock *lock) { LeaveCriticalSection(&lock->section); }

// The profile, its achievements and the enumerations share one lock.
s_lazy_lock g_lock;

// HALO3_PROFILE_TRACE=1: a line on stderr for every load, save and read of the profile, achievement and content calls.
int g_trace = -1;

bool trace_on()
{
    if (g_trace < 0) {
        const char *setting = getenv("HALO3_PROFILE_TRACE");
        g_trace = setting != 0 && setting[0] == '1';
    }
    return g_trace != 0;
}

// A trace line built without the CRT's formatting.
struct s_trace_line {
    char text[400];
    DWORD length;
    s_trace_line() : length(0) { text[0] = 0; }
    s_trace_line &add(const char *s)
    {
        for (; s != 0 && *s != 0 && length + 2 < sizeof(text); s++)
            text[length++] = *s;
        text[length] = 0;
        return *this;
    }
    s_trace_line &add_name(const char *s, DWORD capacity) // a name that may fill its buffer unterminated
    {
        for (DWORD i = 0; i < capacity && s[i] != 0 && length + 2 < sizeof(text); i++)
            text[length++] = s[i] >= ' ' && s[i] < 0x7f ? s[i] : '?';
        text[length] = 0;
        return *this;
    }
    s_trace_line &hex(unsigned long long value, int digits = 0)
    {
        char digit[17];
        int n = 0;
        do {
            digit[n++] = "0123456789abcdef"[value & 15];
            value >>= 4;
        } while ((value != 0 || n < digits) && n < 16);
        add("0x");
        while (n > 0 && length + 2 < sizeof(text))
            text[length++] = digit[--n];
        text[length] = 0;
        return *this;
    }
    s_trace_line &number(DWORD value)
    {
        char digit[10];
        int n = 0;
        do {
            digit[n++] = (char)('0' + value % 10);
            value /= 10;
        } while (value != 0);
        while (n > 0 && length + 2 < sizeof(text))
            text[length++] = digit[--n];
        text[length] = 0;
        return *this;
    }
    void write()
    {
        text[length++] = '\n';
        text[length] = 0;
        port_write_error(text);
    }
};

// ---- enumerations ----
//
// An enumeration handle is a kernel event (so the title's CloseHandle closes it) with a record of what XEnumerate
// returns from it. Nothing tells this file when a handle is closed, so a record lives until a new enumeration reuses
// its slot: one made with the same handle value (the old one was closed), else the oldest.

enum e_enumeration_kind {
    _enumeration_empty,         // nothing (Live services; title servers without Xbox Live)
    _enumeration_achievements,  // the title's achievements (XUserCreateAchievementEnumerator)
    _enumeration_content,       // content packages (XContentCreateEnumerator)
    _enumeration_title_servers, // the title servers the Xbox Live server lists (XTitleServerCreateEnumerator)
};

struct s_enumeration {
    HANDLE handle;
    DWORD kind;
    DWORD items_per_call; // cItem
    DWORD item_size;      // bytes a returned item takes in the buffer
    DWORD position;       // items returned so far
    DWORD flags;          // achievements: dwDetailFlags; content: dwContentFlags
    DWORD content_type;
    unsigned long long owner; // content: the user's XUID (0: none signed in)
    DWORD user_index;         // achievements: the user whose earned ones are listed
    DWORD stamp;              // when it was made (reuse order)
};

const DWORD k_enumeration_capacity = 32;
s_enumeration g_enumerations[k_enumeration_capacity];
DWORD g_enumeration_stamp;

// A new enumeration. Returns 0 (*handle set, the record filled from `record`) or a Win32 error.
DWORD create_enumeration(const s_enumeration &record, PDWORD buffer_size, PHANDLE handle)
{
    if (handle == 0)
        return ERROR_INVALID_PARAMETER;
    *handle = CreateEventA(0, TRUE, FALSE, 0);
    if (*handle == 0)
        return ERROR_NOT_ENOUGH_MEMORY;
    if (buffer_size != 0)
        *buffer_size = record.items_per_call * record.item_size;
    lock_enter(&g_lock);
    s_enumeration *slot = &g_enumerations[0];
    for (DWORD i = 0; i < k_enumeration_capacity; i++) {
        s_enumeration *candidate = &g_enumerations[i];
        if (candidate->handle == *handle) {
            slot = candidate;
            break;
        }
        if (candidate->stamp < slot->stamp)
            slot = candidate;
    }
    *slot = record;
    slot->handle = *handle;
    slot->position = 0;
    slot->stamp = ++g_enumeration_stamp;
    lock_leave(&g_lock);
    return ERROR_SUCCESS;
}

DWORD create_empty_enumerator(DWORD item_count, DWORD item_size, PDWORD buffer_size, PHANDLE handle)
{
    s_enumeration record = {};
    record.kind = _enumeration_empty;
    record.items_per_call = item_count;
    record.item_size = item_size;
    return create_enumeration(record, buffer_size, handle);
}

// Under the lock.
s_enumeration *find_enumeration(HANDLE handle)
{
    if (handle == 0 || handle == INVALID_HANDLE_VALUE)
        return 0;
    for (DWORD i = 0; i < k_enumeration_capacity; i++)
        if (g_enumerations[i].handle == handle && g_enumerations[i].stamp != 0)
            return &g_enumerations[i];
    return 0;
}

DWORD enumerate_achievements(s_enumeration *enumeration, BYTE *buffer, DWORD buffer_size, DWORD *count);
DWORD enumerate_content(s_enumeration *enumeration, BYTE *buffer, DWORD buffer_size, DWORD *count);

// The title servers (Halo 3's LSPs) the Xbox Live server listed last (XTitleServerCreateEnumerator), under the lock.
// Title-server enumerations are records like the others (above), so a handle value reused by a content enumeration
// is that enumeration's, whatever a closed title-server handle with that value had left.
const DWORD k_title_server_limit = 16;
XTITLE_SERVER_INFO g_title_servers[k_title_server_limit];
DWORD g_title_server_count;

// Under the lock.
DWORD enumerate_title_servers(s_enumeration *enumeration, BYTE *buffer, DWORD buffer_size, DWORD *count)
{
    *count = 0;
    if (enumeration->position >= g_title_server_count)
        return ERROR_NO_MORE_FILES;
    DWORD n = enumeration->items_per_call;
    if (buffer_size / (DWORD)sizeof(XTITLE_SERVER_INFO) < n)
        n = buffer_size / (DWORD)sizeof(XTITLE_SERVER_INFO);
    if (n == 0)
        return ERROR_INSUFFICIENT_BUFFER;
    if (n > g_title_server_count - enumeration->position)
        n = g_title_server_count - enumeration->position;
    memcpy(buffer, &g_title_servers[enumeration->position], n * sizeof(XTITLE_SERVER_INFO));
    enumeration->position += n;
    *count = n;
    return ERROR_SUCCESS;
}

} // namespace

extern "C" {

// ---- overlapped results ----

DWORD XGetOverlappedResult(PXOVERLAPPED pOverlapped, LPDWORD pdwResult, BOOL bWait)
{
    if (pOverlapped->InternalLow == ERROR_IO_PENDING) {
        if (!bWait || pOverlapped->hEvent == 0)
            return ERROR_IO_INCOMPLETE;
        WaitForSingleObject(pOverlapped->hEvent, INFINITE);
    }
    if (pdwResult != 0)
        *pdwResult = (DWORD)pOverlapped->InternalHigh;
    return (DWORD)pOverlapped->InternalLow;
}

DWORD XGetOverlappedExtendedError(PXOVERLAPPED pOverlapped)
{
    if (pOverlapped == 0)
        return (DWORD)HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    if (pOverlapped->InternalLow == ERROR_IO_PENDING)
        return (DWORD)HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
    return pOverlapped->dwExtendedError;
}

// The next items of an enumeration: as many as the buffer holds, at most the cItem it was made with. ERROR_NO_MORE_FILES
// once every item has been returned; ERROR_INSUFFICIENT_BUFFER when not one fits; ERROR_INVALID_HANDLE for a handle
// no enumeration made. XEnumerate is XAM's (XamEnumerate), so an overlapped call that fails completes as XAM's do:
// ERROR_FUNCTION_FAILED, its cause the extended error. The content catalogue counts on it: its enumeration task
// (c_content_enumeration_overlapped_task::is_result_successful) takes (ERROR_FUNCTION_FAILED,
// HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES)) as "no packages" and any other failure as the end of the enumeration,
// its item array disposed. With a bare ERROR_NO_MORE_FILES an empty saved-game stage ended it before the marketplace
// stage (downloadable maps) ran, and an empty marketplace stage threw away the saved games found before it.
DWORD XEnumerate(HANDLE hEnum, PVOID pvBuffer, DWORD cbBuffer, PDWORD pcItemsReturned, PXOVERLAPPED pOverlapped)
{
    if (pcItemsReturned != 0)
        *pcItemsReturned = 0;
    DWORD result = ERROR_INVALID_HANDLE, count = 0;
    lock_enter(&g_lock);
    s_enumeration *enumeration = find_enumeration(hEnum);
    if (enumeration != 0) {
        if (pvBuffer == 0 && cbBuffer != 0)
            result = ERROR_INVALID_PARAMETER;
        else if (enumeration->kind == _enumeration_achievements)
            result = enumerate_achievements(enumeration, (BYTE *)pvBuffer, cbBuffer, &count);
        else if (enumeration->kind == _enumeration_content)
            result = enumerate_content(enumeration, (BYTE *)pvBuffer, cbBuffer, &count);
        else if (enumeration->kind == _enumeration_title_servers)
            result = enumerate_title_servers(enumeration, (BYTE *)pvBuffer, cbBuffer, &count);
        else
            result = ERROR_NO_MORE_FILES;
    }
    lock_leave(&g_lock);
    if (pcItemsReturned != 0)
        *pcItemsReturned = count;
    return complete_xam(pOverlapped, result, count);
}

// ---- Xbox Live startup ----

DWORD XOnlineStartup()
{
    s_live_trace trace("XOnlineStartup");
    if (port_live_enabled())
        port_live_startup();
    return ERROR_SUCCESS;
}

DWORD XOnlineCleanup()
{
    if (port_live_enabled())
        port_live_cleanup();
    return ERROR_SUCCESS;
}

XONLINE_NAT_TYPE XOnlineGetNatType() { return XONLINE_NAT_OPEN; } // no router between this console and the Internet

// ---- content ----
//
// One device, the hard drive (port/sdk/kernel/content.h), with the data directory's free space. A user's saved
// games are theirs (owned by the profile's offline XUID); marketplace content (downloadable maps) is every profile's.

} // extern "C"

namespace {

const DWORD k_title_id = 0x4D5307E6; // Halo 3
const DWORD k_xcontenttype_marketplace = 2;
const DWORD k_license_mask_all = 0xffffffff; // every license bit: a purchased package
const DWORD k_xcontentdevicetype_hdd = 1;
const DWORD k_xcontentflag_creation_mask = 0x0000000f; // XCONTENTFLAG_CREATENEW 1 .. XCONTENTFLAG_TRUNCATEEXISTING 5
const DWORD k_xcontentflag_enum_excludecommon = 0x00001000;
const DWORD k_xuser_index_any = 0xff;
const DWORD k_error_no_such_user = 1317;
const unsigned long long k_content_shared_owner = 0; // the owner of content every profile shares

// The owner of a user's packages of a type: 0 (shared) for marketplace content or no user, else the user's XUID.
// Returns 0 (ERROR_SUCCESS) or ERROR_NO_SUCH_USER for a user index that is not signed in.
DWORD content_owner(DWORD user_index, DWORD content_type, unsigned long long *owner)
{
    *owner = k_content_shared_owner;
    if (user_index >= XUSER_MAX_COUNT)
        return ERROR_SUCCESS;
    if (!port_profile_signed_in(user_index))
        return k_error_no_such_user;
    if (content_type != k_xcontenttype_marketplace)
        *owner = port_profile_user_xuid(user_index);
    return ERROR_SUCCESS;
}

void package_from_content_data(const XCONTENT_DATA *data, unsigned long long owner, port_content_package *package)
{
    memset(package, 0, sizeof(*package));
    package->owner = owner;
    package->title_id = k_title_id;
    package->content_type = data->dwContentType;
    memcpy(package->file_name, data->szFileName, sizeof(package->file_name));
    for (DWORD i = 0; i + 1 < XCONTENT_MAX_DISPLAYNAME_LENGTH && data->szDisplayName[i] != 0; i++)
        package->display_name[i] = data->szDisplayName[i];
}

void trace_content(const char *call, const char *root, const XCONTENT_DATA *data, DWORD result, DWORD disposition)
{
    if (!trace_on())
        return;
    s_trace_line line;
    line.add("content: ").add(call);
    if (root != 0)
        line.add(" root=").add_name(root, 64);
    if (data != 0)
        line.add(" type=").number(data->dwContentType).add(" file=").add_name(data->szFileName, XCONTENT_MAX_FILENAME_LENGTH);
    line.add(" -> ").number(result);
    if (disposition != 0)
        line.add(disposition == 1 ? " created" : " opened");
    line.write();
}

// ---- packages opened in the background ----
//
// Opening downloadable content may install it first (port/sdk/kernel/content.c: a map pack's files extracted and
// checked, seconds for a large one), and the title opens its packages from its main loop with an XOVERLAPPED that it
// polls. As on a console, such a call returns ERROR_IO_PENDING at once and a thread of its own completes it; without
// an XOVERLAPPED (the title's worker threads), or when no thread can be had, the call opens the package itself.

struct s_content_open_request {
    char root[64];
    XCONTENT_DATA data;
    port_content_package package;
    DWORD creation;
    PDWORD disposition;   // the caller's, written when the open is done
    PDWORD license_mask;
    PXOVERLAPPED overlapped;
    DWORD thread;         // the caller's: its completion routine runs there
};

struct s_content_open_slot {
    volatile LONG busy;
    s_content_open_request request;
};

const DWORD k_content_open_slots = 8;
s_content_open_slot g_content_open_slots[k_content_open_slots];

// The open of `request` done: the caller's outputs, the trace, the XOVERLAPPED (completed for the caller's thread).
DWORD content_open_finish(const s_content_open_request &request, DWORD result, DWORD disposition)
{
    if (request.disposition != 0)
        *request.disposition = disposition;
    const bool marketplace = request.data.dwContentType == k_xcontenttype_marketplace;
    if (request.license_mask != 0 && result == ERROR_SUCCESS && marketplace)
        *request.license_mask = k_license_mask_all;
    trace_content("XContentCreateEx", request.root, &request.data, result, disposition);
    return complete_xam(request.overlapped, result, disposition, request.thread);
}

DWORD WINAPI content_open_main(LPVOID parameter)
{
    s_content_open_slot *slot = (s_content_open_slot *)parameter;
    const s_content_open_request request = slot->request;
    __atomic_store_n(&slot->busy, 0, __ATOMIC_RELEASE);
    unsigned int disposition = 0;
    const DWORD result = port_content_open(request.root, &request.package, request.creation, &disposition);
    content_open_finish(request, result, disposition);
    return 0;
}

// Starts the open of `request` (which has an XOVERLAPPED) on a thread of its own. Returns false when it cannot.
bool content_open_in_background(const s_content_open_request &request)
{
    for (DWORD i = 0; i < k_content_open_slots; i++) {
        s_content_open_slot *slot = &g_content_open_slots[i];
        if (__atomic_exchange_n(&slot->busy, 1, __ATOMIC_ACQUIRE) != 0)
            continue;
        slot->request = request;
        request.overlapped->InternalHigh = 0;
        request.overlapped->dwExtendedError = 0;
        __atomic_store_n(&request.overlapped->InternalLow, (ULONG_PTR)ERROR_IO_PENDING, __ATOMIC_RELEASE);
        HANDLE thread = CreateThread(0, 0, content_open_main, slot, 0, 0);
        if (thread == 0) {
            __atomic_store_n(&slot->busy, 0, __ATOMIC_RELEASE);
            return false;
        }
        CloseHandle(thread);
        return true;
    }
    return false;
}

port_content_package g_content_scratch[16]; // enumerate_content's, under the lock

// Under the lock: the next content packages, the user's own first, then (unless XCONTENTFLAG_ENUM_EXCLUDECOMMON)
// those every profile shares.
DWORD enumerate_content(s_enumeration *enumeration, BYTE *buffer, DWORD buffer_size, DWORD *count)
{
    *count = 0;
    DWORD capacity = enumeration->items_per_call;
    if (buffer_size / sizeof(XCONTENT_DATA) < capacity)
        capacity = buffer_size / sizeof(XCONTENT_DATA);
    const bool include_shared = enumeration->owner != k_content_shared_owner &&
                                (enumeration->flags & k_xcontentflag_enum_excludecommon) == 0;
    XCONTENT_DATA *items = (XCONTENT_DATA *)buffer;
    DWORD result = ERROR_SUCCESS, own_total = 0;
    while (*count < capacity && result == ERROR_SUCCESS) {
        // the next items from the owner's list, then from the shared one
        DWORD position = enumeration->position;
        unsigned int got = 0, total = 0;
        unsigned long long owner = enumeration->owner;
        DWORD want = capacity - *count;
        if (want > sizeof(g_content_scratch) / sizeof(g_content_scratch[0]))
            want = sizeof(g_content_scratch) / sizeof(g_content_scratch[0]);
        result = port_content_list(owner, k_title_id, enumeration->content_type, position, g_content_scratch, want, &got,
            &total);
        if (result == ERROR_SUCCESS && got == 0 && include_shared) {
            own_total = total;
            owner = k_content_shared_owner;
            result = port_content_list(owner, k_title_id, enumeration->content_type, position - own_total,
                g_content_scratch, want, &got, &total);
        }
        if (result != ERROR_SUCCESS || got == 0)
            break;
        for (DWORD i = 0; i < got; i++) {
            XCONTENT_DATA *item = &items[(*count)++];
            memset(item, 0, sizeof(*item));
            item->DeviceID = PORT_CONTENT_DEVICE_HARD_DRIVE;
            item->dwContentType = enumeration->content_type;
            for (DWORD k = 0; k < XCONTENT_MAX_DISPLAYNAME_LENGTH; k++)
                item->szDisplayName[k] = g_content_scratch[i].display_name[k];
            memcpy(item->szFileName, g_content_scratch[i].file_name, sizeof(item->szFileName));
        }
        enumeration->position += got;
    }
    if (result == ERROR_SUCCESS && *count == 0)
        result = capacity == 0 && buffer_size < sizeof(XCONTENT_DATA) ? ERROR_INSUFFICIENT_BUFFER : ERROR_NO_MORE_FILES;
    if (trace_on()) {
        s_trace_line line;
        line.add("content: XEnumerate type=").number(enumeration->content_type).add(" owner=").hex(enumeration->owner)
            .add(" -> ").number(*count).add(" items (result ").number(result).add(")");
        line.write();
    }
    return result;
}

} // namespace

extern "C" {

DWORD XContentGetDeviceData(XCONTENTDEVICEID DeviceID, PXDEVICE_DATA pDeviceData)
{
    if (pDeviceData == 0)
        return ERROR_INVALID_PARAMETER;
    memset(pDeviceData, 0, sizeof(*pDeviceData));
    if (DeviceID != PORT_CONTENT_DEVICE_HARD_DRIVE)
        return ERROR_DEVICE_NOT_CONNECTED;
    unsigned long long total = 0, available = 0;
    DWORD result = port_content_device_space(&total, &available);
    if (result != ERROR_SUCCESS)
        return result;
    pDeviceData->DeviceID = DeviceID;
    pDeviceData->DeviceType = k_xcontentdevicetype_hdd;
    pDeviceData->ulDeviceBytes = total;
    pDeviceData->ulDeviceFreeBytes = available;
    static const char k_name[] = "Hard Drive";
    for (DWORD i = 0; i < sizeof(k_name); i++)
        pDeviceData->wszFriendlyName[i] = (WCHAR)k_name[i];
    return ERROR_SUCCESS;
}

DWORD XContentCreateEnumerator(DWORD dwUserIndex, XCONTENTDEVICEID DeviceID, DWORD dwContentType,
    DWORD dwContentFlags, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum)
{
    if (pcbBuffer != 0)
        *pcbBuffer = 0;
    if (phEnum == 0 || cItem == 0 || (dwUserIndex >= XUSER_MAX_COUNT && dwUserIndex != k_xuser_index_any))
        return ERROR_INVALID_PARAMETER;
    if (DeviceID != 0 && DeviceID != PORT_CONTENT_DEVICE_HARD_DRIVE)
        return ERROR_DEVICE_NOT_CONNECTED;
    s_enumeration record = {};
    DWORD result = content_owner(dwUserIndex, dwContentType, &record.owner);
    if (result != ERROR_SUCCESS)
        return result;
    record.kind = _enumeration_content;
    record.items_per_call = cItem;
    record.item_size = sizeof(XCONTENT_DATA);
    record.flags = dwContentFlags;
    record.content_type = dwContentType;
    return create_enumeration(record, pcbBuffer, phEnum);
}

// XContentCreateEx opens the package as the drive szRootName ("<root>:\file"); InternalHigh (and *pdwDisposition) is
// XCONTENT_CREATED (1) or XCONTENT_OPENED (2). The license mask of the title's own content is 0; marketplace content
// (downloadable maps) is licensed to every user and console: 0xffffffff. Marketplace content with an XOVERLAPPED opens
// in the background (it may be installed first).
DWORD XContentCreateEx(DWORD dwUserIndex, LPCSTR szRootName, const XCONTENT_DATA *pContentData, DWORD dwContentFlags,
    PDWORD pdwDisposition, PDWORD pdwLicenseMask, SIZE_T dwFileCacheSize, ULARGE_INTEGER uliContentSize,
    PXOVERLAPPED pOverlapped)
{
    (void)dwFileCacheSize; (void)uliContentSize; // a host directory has no fixed size or cache
    if (pdwDisposition != 0)
        *pdwDisposition = 0;
    if (pdwLicenseMask != 0)
        *pdwLicenseMask = 0;
    if (pContentData == 0 || szRootName == 0)
        return ERROR_INVALID_PARAMETER;
    s_content_open_request request;
    memset(&request, 0, sizeof(request));
    const size_t root_length = strlen(szRootName);
    memcpy(request.root, szRootName, root_length < sizeof(request.root) ? root_length : sizeof(request.root) - 1);
    request.data = *pContentData;
    request.creation = dwContentFlags & k_xcontentflag_creation_mask;
    request.disposition = pdwDisposition;
    request.license_mask = pdwLicenseMask;
    request.overlapped = pOverlapped;
    request.thread = GetCurrentThreadId();
    DWORD result = ERROR_SUCCESS;
    unsigned long long owner = 0;
    if (pContentData->DeviceID != PORT_CONTENT_DEVICE_HARD_DRIVE)
        result = ERROR_DEVICE_NOT_CONNECTED;
    else if (root_length >= sizeof(request.root))
        result = ERROR_INVALID_NAME;
    else
        result = content_owner(dwUserIndex, pContentData->dwContentType, &owner);
    if (result != ERROR_SUCCESS)
        return content_open_finish(request, result, 0);
    package_from_content_data(pContentData, owner, &request.package);
    const bool marketplace = pContentData->dwContentType == k_xcontenttype_marketplace;
    if (pOverlapped != 0 && marketplace && content_open_in_background(request))
        return ERROR_IO_PENDING;
    unsigned int disposition = 0;
    result = port_content_open(request.root, &request.package, request.creation, &disposition);
    return content_open_finish(request, result, disposition);
}

DWORD XContentClose(LPCSTR szRootName, PXOVERLAPPED pOverlapped)
{
    const DWORD result = port_content_close(szRootName);
    trace_content("XContentClose", szRootName, 0, result, 0);
    return complete_xam(pOverlapped, result);
}

DWORD XContentFlush(LPCSTR szRootName, PXOVERLAPPED pOverlapped)
{
    const DWORD result = port_content_flush(szRootName);
    trace_content("XContentFlush", szRootName, 0, result, 0);
    return complete_xam(pOverlapped, result);
}

DWORD XContentDelete(DWORD dwUserIndex, const XCONTENT_DATA *pContentData, PXOVERLAPPED pOverlapped)
{
    if (pContentData == 0)
        return ERROR_INVALID_PARAMETER;
    unsigned long long owner = 0;
    DWORD result = pContentData->DeviceID != PORT_CONTENT_DEVICE_HARD_DRIVE
                       ? (DWORD)ERROR_DEVICE_NOT_CONNECTED
                       : content_owner(dwUserIndex, pContentData->dwContentType, &owner);
    if (result == ERROR_SUCCESS) {
        port_content_package package;
        package_from_content_data(pContentData, owner, &package);
        result = port_content_delete(&package);
    }
    trace_content("XContentDelete", 0, pContentData, result, 0);
    return complete_xam(pOverlapped, result);
}

DWORD XMarketplaceCreateAssetEnumerator(DWORD dwUserIndex, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum)
{
    (void)dwUserIndex; (void)cItem;
    if (pcbBuffer != 0)
        *pcbBuffer = 0;
    if (phEnum != 0)
        *phEnum = INVALID_HANDLE_VALUE;
    return ERROR_NOT_LOGGED_ON;
}

DWORD XMarketplaceConsumeAssets(DWORD dwUserIndex, DWORD cAssets, const XMARKETPLACE_ASSET *pAssets,
    XOVERLAPPED *pXOverlapped)
{
    (void)dwUserIndex; (void)cAssets; (void)pAssets;
    return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
}

// The title servers (Halo 3's LSPs): with Xbox Live, the ones the Live server lists (live.c), signing in first if need
// be; XEnumerate returns at most cItem a call, as from the other enumerations. None without Live, or when the list
// cannot be had.
DWORD XTitleServerCreateEnumerator(LPCSTR pszServerInfo, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum)
{
    s_live_trace trace("XTitleServerCreateEnumerator");
    (void)pszServerInfo;
    if (phEnum == 0)
        return ERROR_INVALID_PARAMETER;
    if (!port_live_enabled())
        return create_empty_enumerator(cItem, sizeof(XTITLE_SERVER_INFO), pcbBuffer, phEnum);

    if (!port_live_signed_in())
        port_live_startup();

    port_live_title_server fetched[k_title_server_limit];
    int fetched_count = 0;
    if (port_live_fetch_title_servers(k_title_id, fetched, (int)k_title_server_limit, &fetched_count) != 0)
        return create_empty_enumerator(cItem, sizeof(XTITLE_SERVER_INFO), pcbBuffer, phEnum);

    static_assert(sizeof(fetched[0].description) == sizeof(g_title_servers[0].szServerInfo), "LSP description size");
    XTITLE_SERVER_INFO servers[k_title_server_limit];
    DWORD count = 0;
    for (; count < (DWORD)fetched_count && count < k_title_server_limit; count++) {
        XTITLE_SERVER_INFO &server = servers[count];
        memset(&server, 0, sizeof(server));
        server.inaServer.s_addr = htonl(fetched[count].address);
        server.dwFlags = fetched[count].flags;
        // Halo 3's c_online_lsp_manager::service_type_in_list treats an empty description as matching
        // every Bungie LSP service (title/user/machine/stats/debug); a non-empty string must be a
        // comma-separated list of 3-letter service codes.
        if (strcmp(fetched[count].description, "bungie") != 0) {
            memcpy(server.szServerInfo, fetched[count].description, sizeof(server.szServerInfo));
            server.szServerInfo[XTITLE_SERVER_MAX_SERVER_INFO_LEN - 1] = 0;
        }
    }
    lock_enter(&g_lock);
    memcpy(g_title_servers, servers, count * sizeof(servers[0]));
    g_title_server_count = count;
    lock_leave(&g_lock);

    s_enumeration record = {};
    record.kind = _enumeration_title_servers;
    record.items_per_call = cItem;
    record.item_size = sizeof(XTITLE_SERVER_INFO);
    return create_enumeration(record, pcbBuffer, phEnum);
}

// ---- users, profiles, friends, achievements, stats ----

DWORD XUserAreUsersFriends(DWORD dwUserIndex, PXUID pXuids, DWORD dwXuidCount, PBOOL pfResult,
    PXOVERLAPPED pOverlapped)
{
    (void)dwUserIndex; (void)pXuids;
    for (DWORD i = 0; pfResult != 0 && i < dwXuidCount; i++)
        pfResult[i] = FALSE;
    return complete(pOverlapped, ERROR_NOT_LOGGED_ON);
}

DWORD XUserMuteListQuery(DWORD dwUserIndex, XUID XuidRemoteTalker, BOOL *pfOnMuteList)
{
    (void)dwUserIndex; (void)XuidRemoteTalker;
    if (pfOnMuteList != 0)
        *pfOnMuteList = FALSE;
    return ERROR_NOT_LOGGED_ON;
}

// ---- the local profiles' settings and achievements ----
//
// A console keeps a profile on its hard drive: its settings, and the achievements it has earned. Here they are two files
// of the data directory, under profiles/<the profile's offline XUID, 16 hex digits>/ (so profiles with different
// HALO3_PROFILE_NAMEs keep apart, and each signed-in user, HALO3_LOCAL_PLAYERS, has their own), each loaded on first
// use and rewritten atomically (storage.h) whenever the title changes them:
//   profile_settings.bin: what the title wrote reads back as XSOURCE_TITLE; anything else reads as the console's
//     default (XSOURCE_DEFAULT: controller vibration on, every other number 0, binary and string settings empty);
//   achievements.bin: each achievement earned and when (FILETIME, UTC). A local profile earns achievements offline,
//     as a console's does: XACHIEVEMENT_DETAILS_ACHIEVED without ..._ACHIEVED_ONLINE.
// Both files: a 24-byte header ("H3PS" / "H3AC", version 1, the record count, the payload's size and its FNV-1a hash,
// 4 reserved bytes), then the records, little-endian. A file that does not check out is renamed to <file>.corrupt (the
// profile starts from its defaults) rather than overwritten; one that exists but cannot be read keeps this run's
// changes in memory only.
} // extern "C"

namespace {

const DWORD k_store_header_size = 24;
const DWORD k_store_version = 1;
const DWORD k_profile_settings_magic = 0x53503348; // "H3PS"
const DWORD k_achievements_magic = 0x43413348;     // "H3AC"

enum e_store_state {
    _store_unloaded,
    _store_ready,       // loaded (or nothing to load): changes are saved
    _store_unavailable, // the file could not be read: changes stay in memory, the file is left alone
    _store_guest,       // a guest's: it has no file; changes stay in memory until it signs out
};

// Under the lock: the file image being read or written (the larger of the two stores, profile settings at capacity).
BYTE g_store_buffer[0x10000];

DWORD get32(const BYTE *p) { return (DWORD)p[0] | (DWORD)p[1] << 8 | (DWORD)p[2] << 16 | (DWORD)p[3] << 24; }

void put32(BYTE *p, DWORD v)
{
    p[0] = (BYTE)v;
    p[1] = (BYTE)(v >> 8);
    p[2] = (BYTE)(v >> 16);
    p[3] = (BYTE)(v >> 24);
}

DWORD fnv1a(const BYTE *data, DWORD size)
{
    DWORD hash = 0x811c9dc5;
    for (DWORD i = 0; i < size; i++)
        hash = (hash ^ data[i]) * 0x01000193;
    return hash;
}

// "profiles/<XUID of user `user_index`>/<leaf>"
void store_file_name(DWORD user_index, const char *leaf, char name[64])
{
    const unsigned long long xuid = port_profile_user_xuid(user_index);
    DWORD n = 0;
    for (const char *s = "profiles/"; *s != 0; s++)
        name[n++] = *s;
    for (int shift = 60; shift >= 0; shift -= 4)
        name[n++] = "0123456789ABCDEF"[(xuid >> shift) & 15];
    name[n++] = '/';
    for (; *leaf != 0 && n + 1 < 64; leaf++)
        name[n++] = *leaf;
    name[n] = 0;
}

void store_warning(const char *what, const char *name, DWORD error)
{
    s_trace_line line;
    line.add("port: profile: ").add(what).add(" ").add(name);
    if (error != 0)
        line.add(" (error ").number(error).add(")");
    line.write();
}

enum e_store_load { _store_load_missing, _store_load_ok, _store_load_corrupt, _store_load_unreadable };

// Reads the store file into g_store_buffer: on _store_load_ok, *payload / *payload_size / *count describe its records.
e_store_load store_read(const char *name, DWORD magic, const BYTE **payload, DWORD *payload_size, DWORD *count)
{
    unsigned int size = 0;
    const DWORD result = port_storage_data_read(name, g_store_buffer, sizeof(g_store_buffer), &size);
    if (result == ERROR_FILE_NOT_FOUND || result == ERROR_PATH_NOT_FOUND)
        return _store_load_missing;
    if (result == ERROR_INSUFFICIENT_BUFFER)
        return _store_load_corrupt; // larger than any this file writes
    if (result != ERROR_SUCCESS) {
        store_warning("cannot read", name, result);
        return _store_load_unreadable;
    }
    if (size < k_store_header_size || get32(g_store_buffer) != magic || get32(g_store_buffer + 4) != k_store_version ||
        get32(g_store_buffer + 12) != size - k_store_header_size ||
        get32(g_store_buffer + 16) != fnv1a(g_store_buffer + k_store_header_size, size - k_store_header_size))
        return _store_load_corrupt;
    *payload = g_store_buffer + k_store_header_size;
    *payload_size = size - k_store_header_size;
    *count = get32(g_store_buffer + 8);
    return _store_load_ok;
}

// A file that does not check out: set aside, so the next save does not destroy what a user may still recover.
void store_set_aside(const char *name)
{
    const DWORD result = port_storage_data_set_aside(name);
    store_warning(result == ERROR_SUCCESS ? "corrupt, set aside as <file>.corrupt; defaults used:"
                                          : "corrupt and could not be set aside; defaults used:",
        name, result);
}

// Writes g_store_buffer's header for `count` records in the `payload_size` bytes after it, and the file.
DWORD store_write(const char *name, DWORD magic, DWORD count, DWORD payload_size)
{
    put32(g_store_buffer, magic);
    put32(g_store_buffer + 4, k_store_version);
    put32(g_store_buffer + 8, count);
    put32(g_store_buffer + 12, payload_size);
    put32(g_store_buffer + 16, fnv1a(g_store_buffer + k_store_header_size, payload_size));
    put32(g_store_buffer + 20, 0);
    const DWORD result = port_storage_data_write(name, g_store_buffer, k_store_header_size + payload_size);
    if (result != ERROR_SUCCESS)
        store_warning("cannot save", name, result);
    return result;
}

// ---- profile settings ----
//
// Records: the setting id, its XUSER_DATA_TYPE, 3 bytes of 0, the size of its value, the value (padded to 4 bytes):
// INT32 / FLOAT 4 bytes, INT64 / DOUBLE / DATETIME 8, UNICODE / BINARY their cbData (at most the size in the id).

const DWORD k_profile_setting_capacity = 64;
const BYTE k_xuser_data_type_datetime = 7; // XUSER_DATA_TYPE_DATETIME, a FILETIME

struct s_profile_setting {
    DWORD id;
    XUSER_DATA data; // pointers of binary / string data point at `bytes`
    BYTE bytes[XPROFILE_SETTING_MAX_SIZE];
};
typedef char profile_settings_fit[(k_store_header_size + k_profile_setting_capacity * (12 + XPROFILE_SETTING_MAX_SIZE) <=
                                      sizeof(g_store_buffer)) ? 1 : -1];

// One table per user: each signed-in local profile has its own settings, and its own file.
struct s_profile_settings {
    s_profile_setting settings[k_profile_setting_capacity];
    DWORD count;
    DWORD store; // e_store_state
    DWORD serial; // the sign-in the table is of (port_profile_user_serial): another sign-in reads its own
};
s_profile_settings g_profile_settings[XUSER_MAX_COUNT];

BYTE profile_setting_type(DWORD id) { return (BYTE)((id >> 28) & 0xf); }
DWORD profile_setting_size(DWORD id) { return (id >> 16) & 0xfff; } // the most a setting holds (XPROFILEID)
bool profile_setting_has_data(DWORD id)
{
    return profile_setting_type(id) == XUSER_DATA_TYPE_BINARY || profile_setting_type(id) == XUSER_DATA_TYPE_UNICODE;
}

// The bytes of a fixed-size value (0 for binary and string data, and for no type).
DWORD fixed_value_size(BYTE type)
{
    switch (type) {
    case XUSER_DATA_TYPE_INT32:
    case XUSER_DATA_TYPE_FLOAT:
        return 4;
    case XUSER_DATA_TYPE_INT64:
    case XUSER_DATA_TYPE_DOUBLE:
    case k_xuser_data_type_datetime:
        return 8;
    default:
        return 0;
    }
}

// Whether `id` names a setting this file can hold: a known type, and for binary and string data a size that fits.
bool profile_setting_id_valid(DWORD id)
{
    const BYTE type = profile_setting_type(id);
    if (profile_setting_has_data(id))
        return profile_setting_size(id) <= XPROFILE_SETTING_MAX_SIZE;
    return fixed_value_size(type) != 0;
}

// Under the lock.
s_profile_setting *profile_setting_find(s_profile_settings *table, DWORD id)
{
    for (DWORD i = 0; i < table->count; i++)
        if (table->settings[i].id == id)
            return &table->settings[i];
    return 0;
}

// Under the lock: the settings from a store payload. Returns false (the table left empty) when the records do not
// check out.
bool profile_settings_parse(s_profile_settings *table, const BYTE *payload, DWORD size, DWORD count)
{
    table->count = 0;
    if (count > k_profile_setting_capacity)
        return false;
    DWORD at = 0;
    for (DWORD i = 0; i < count; i++) {
        if (size - at < 12)
            break;
        const DWORD id = get32(payload + at), value_size = get32(payload + at + 8);
        const BYTE type = payload[at + 4];
        at += 12;
        const DWORD padded = (value_size + 3) & ~3u;
        if (!profile_setting_id_valid(id) || type != profile_setting_type(id) || payload[at - 7] != 0 ||
            payload[at - 6] != 0 || payload[at - 5] != 0 || value_size > XPROFILE_SETTING_MAX_SIZE ||
            padded > size - at || profile_setting_find(table, id) != 0)
            break;
        if (profile_setting_has_data(id) ? value_size > profile_setting_size(id) : value_size != fixed_value_size(type))
            break;
        if (type == XUSER_DATA_TYPE_UNICODE && (value_size & 1) != 0)
            break;
        s_profile_setting *setting = &table->settings[table->count++];
        memset(setting, 0, sizeof(*setting));
        setting->id = id;
        setting->data.type = type;
        if (profile_setting_has_data(id)) {
            memcpy(setting->bytes, payload + at, value_size);
            setting->data.binary.cbData = value_size;
            setting->data.binary.pbData = setting->bytes;
        } else {
            memcpy(&setting->data.i64Data, payload + at, value_size); // every member starts the union (little-endian)
        }
        at += padded;
    }
    if (table->count == count && at == size)
        return true;
    table->count = 0;
    return false;
}

void trace_profile_store(const char *what, const char *name, DWORD count)
{
    if (!trace_on())
        return;
    s_trace_line line;
    line.add("profile: ").add(what).add(" ").add(name).add(" (").number(count).add(" settings)");
    line.write();
}

// Under the lock: user `user_index`'s settings.
s_profile_settings *profile_settings_load(DWORD user_index)
{
    s_profile_settings *table = &g_profile_settings[user_index];
    const DWORD serial = port_profile_user_serial(user_index);
    if (table->store != _store_unloaded && table->serial == serial)
        return table;
    // a new sign-in on this user (another profile, a guest, the same profile again): its own settings
    table->serial = serial;
    table->count = 0;
    if (port_profile_user_is_guest(user_index)) {
        table->store = _store_guest; // a guest starts from the defaults, and keeps nothing
        return table;
    }
    char name[64];
    store_file_name(user_index, "profile_settings.bin", name);
    const BYTE *payload = 0;
    DWORD payload_size = 0, count = 0;
    table->store = _store_ready;
    table->count = 0;
    switch (store_read(name, k_profile_settings_magic, &payload, &payload_size, &count)) {
    case _store_load_missing:
        trace_profile_store("no saved settings:", name, 0);
        break;
    case _store_load_ok:
        if (profile_settings_parse(table, payload, payload_size, count)) {
            trace_profile_store("loaded", name, table->count);
            break;
        }
        store_set_aside(name);
        break;
    case _store_load_corrupt:
        store_set_aside(name);
        break;
    case _store_load_unreadable:
        table->store = _store_unavailable;
        break;
    }
    return table;
}

// Under the lock: user `user_index`'s settings.
DWORD profile_settings_save(DWORD user_index)
{
    const s_profile_settings *table = &g_profile_settings[user_index];
    if (table->store != _store_ready)
        return ERROR_SUCCESS; // kept in memory (the file could not be read)
    char name[64];
    store_file_name(user_index, "profile_settings.bin", name);
    DWORD at = k_store_header_size;
    for (DWORD i = 0; i < table->count; i++) {
        const s_profile_setting *setting = &table->settings[i];
        const DWORD size = profile_setting_has_data(setting->id) ? setting->data.binary.cbData
                                                                 : fixed_value_size(setting->data.type);
        memset(g_store_buffer + at, 0, 12 + ((size + 3) & ~3u));
        put32(g_store_buffer + at, setting->id);
        g_store_buffer[at + 4] = setting->data.type;
        put32(g_store_buffer + at + 8, size);
        memcpy(g_store_buffer + at + 12, profile_setting_has_data(setting->id) ? setting->bytes
                                                                               : (const BYTE *)&setting->data.i64Data,
            size);
        at += 12 + ((size + 3) & ~3u);
    }
    const DWORD result = store_write(name, k_profile_settings_magic, table->count, at - k_store_header_size);
    if (result == ERROR_SUCCESS)
        trace_profile_store("saved", name, table->count);
    return result;
}

// Whether a stored setting already holds `data`.
bool profile_setting_holds(const s_profile_setting *stored, const XUSER_DATA &data)
{
    if (stored->data.type != data.type)
        return false;
    if (profile_setting_has_data(stored->id))
        return stored->data.binary.cbData == data.binary.cbData &&
               (data.binary.cbData == 0 || memcmp(stored->bytes, data.binary.pbData, data.binary.cbData) == 0);
    return memcmp(&stored->data.i64Data, &data.i64Data, fixed_value_size(data.type)) == 0;
}

} // namespace

extern "C" {

DWORD XUserReadProfileSettings(DWORD dwTitleId, DWORD dwUserIndex, DWORD dwNumSettingIds, const DWORD *pdwSettingIds,
    DWORD *pcbResults, PXUSER_READ_PROFILE_SETTING_RESULT pResults, PXOVERLAPPED pXOverlapped)
{
    (void)dwTitleId; // 0 or this title: the settings are this title's
    if (dwUserIndex >= XUSER_MAX_COUNT || pcbResults == 0 || (dwNumSettingIds != 0 && pdwSettingIds == 0))
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    if (!port_profile_signed_in(dwUserIndex))
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    // the results, the settings, then each binary / string setting's data at its largest size
    DWORD required = sizeof(XUSER_READ_PROFILE_SETTING_RESULT) + dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING);
    for (DWORD i = 0; i < dwNumSettingIds; i++)
        if (profile_setting_has_data(pdwSettingIds[i]))
            required += (profile_setting_size(pdwSettingIds[i]) + 3u) & ~3u;
    if (pResults == 0 || *pcbResults < required) {
        *pcbResults = required;
        return complete(pXOverlapped, ERROR_INSUFFICIENT_BUFFER);
    }
    memset(pResults, 0, required);
    XUSER_PROFILE_SETTING *settings = (XUSER_PROFILE_SETTING *)(pResults + 1);
    BYTE *data = (BYTE *)(settings + dwNumSettingIds);
    pResults->dwSettingsLen = dwNumSettingIds;
    pResults->pSettings = settings;
    DWORD from_title = 0;
    lock_enter(&g_lock);
    s_profile_settings *table = profile_settings_load(dwUserIndex);
    for (DWORD i = 0; i < dwNumSettingIds; i++) {
        const DWORD id = pdwSettingIds[i];
        XUSER_PROFILE_SETTING *setting = &settings[i];
        setting->user.dwUserIndex = dwUserIndex;
        setting->dwSettingId = id;
        setting->data.type = profile_setting_type(id);
        const s_profile_setting *stored = profile_setting_find(table, id);
        setting->source = stored != 0 ? XSOURCE_TITLE : XSOURCE_DEFAULT;
        from_title += stored != 0;
        if (profile_setting_has_data(id)) {
            // binary.cbData / pbData and string.cbData / pwszData share their place
            DWORD bytes = 0;
            if (stored != 0) {
                bytes = stored->data.binary.cbData;
                memcpy(data, stored->bytes, bytes);
            } else if (setting->data.type == XUSER_DATA_TYPE_UNICODE) {
                bytes = sizeof(WCHAR); // the empty string: its terminator
            }
            setting->data.binary.cbData = bytes;
            setting->data.binary.pbData = setting->data.type == XUSER_DATA_TYPE_BINARY && bytes == 0 ? 0 : data;
            data += (profile_setting_size(id) + 3u) & ~3u;
        } else if (stored != 0) {
            setting->data = stored->data;
        } else if (id == XPROFILE_OPTION_CONTROLLER_VIBRATION) {
            setting->data.nData = XPROFILE_CONTROLLER_VIBRATION_ON;
        }
    }
    lock_leave(&g_lock);
    if (trace_on()) {
        s_trace_line line;
        line.add("profile: XUserReadProfileSettings ").number(dwNumSettingIds).add(" ids, ").number(from_title)
            .add(" from the title");
        for (DWORD i = 0; i < dwNumSettingIds && i < 4; i++)
            if (settings[i].source == XSOURCE_TITLE)
                line.add(" [").hex(settings[i].dwSettingId).add(": ").number(profile_setting_has_data(settings[i].dwSettingId)
                    ? settings[i].data.binary.cbData : fixed_value_size(settings[i].data.type)).add(" bytes]");
        line.write();
    }
    return complete(pXOverlapped, ERROR_SUCCESS);
}

// Every setting is checked before any is stored, so a call stores all of its settings or none.
DWORD XUserWriteProfileSettings(DWORD dwUserIndex, DWORD dwNumSettings, const PXUSER_PROFILE_SETTING pSettings,
    PXOVERLAPPED pXOverlapped)
{
    if (dwUserIndex >= XUSER_MAX_COUNT || (dwNumSettings != 0 && pSettings == 0))
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    if (!port_profile_signed_in(dwUserIndex))
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    DWORD result = ERROR_SUCCESS;
    lock_enter(&g_lock);
    s_profile_settings *table = profile_settings_load(dwUserIndex);
    DWORD new_ids = 0;
    for (DWORD i = 0; i < dwNumSettings && result == ERROR_SUCCESS; i++) {
        const XUSER_PROFILE_SETTING *setting = &pSettings[i];
        const DWORD id = setting->dwSettingId;
        bool repeated = false;
        for (DWORD k = 0; k < i; k++)
            repeated = repeated || pSettings[k].dwSettingId == id;
        if (!profile_setting_id_valid(id) || setting->data.type != profile_setting_type(id) ||
            (profile_setting_has_data(id) &&
                (setting->data.binary.cbData > profile_setting_size(id) ||
                    (setting->data.binary.cbData != 0 && setting->data.binary.pbData == 0))))
            result = ERROR_INVALID_PARAMETER;
        else if (profile_setting_find(table, id) == 0 && !repeated)
            new_ids++;
    }
    if (result == ERROR_SUCCESS && table->count + new_ids > k_profile_setting_capacity)
        result = ERROR_NOT_ENOUGH_MEMORY;
    bool changed = false;
    for (DWORD i = 0; i < dwNumSettings && result == ERROR_SUCCESS; i++) {
        const XUSER_PROFILE_SETTING *setting = &pSettings[i];
        const DWORD id = setting->dwSettingId;
        s_profile_setting *stored = profile_setting_find(table, id);
        if (stored != 0 && profile_setting_holds(stored, setting->data))
            continue;
        if (stored == 0) {
            stored = &table->settings[table->count++];
            memset(stored, 0, sizeof(*stored));
            stored->id = id;
        }
        stored->data = setting->data;
        if (profile_setting_has_data(id)) {
            memcpy(stored->bytes, setting->data.binary.pbData, setting->data.binary.cbData);
            stored->data.binary.pbData = stored->bytes;
        }
        changed = true;
    }
    if (result == ERROR_SUCCESS && changed)
        result = profile_settings_save(dwUserIndex);
    lock_leave(&g_lock);
    if (trace_on()) {
        s_trace_line line;
        line.add("profile: XUserWriteProfileSettings ").number(dwNumSettings).add(" settings");
        for (DWORD i = 0; i < dwNumSettings && i < 4; i++)
            line.add(" [").hex(pSettings[i].dwSettingId).add("]");
        line.add(changed ? " changed" : " unchanged").add(" -> ").number(result);
        line.write();
    }
    return complete(pXOverlapped, result);
}

// ---- achievements ----
//
// The title's achievements are in its SPA (the XDBF database in default.xex's resource named for the title id): for
// each, in the SPA's order, its id, string ids, image, gamerscore and flags (its type, XACHIEVEMENT_TYPE_*, and
// XACHIEVEMENT_DETAILS_SHOWUNACHIEVED). An enumeration returns every one of them, earned or not, as a console's does:
// the game reads each item's gamerscore (dwCred, its armor unlocks at 250 / 500 / 750 / 1000) and earned flag.
// Without the SPA (no default.xex) it returns the achievements earned, with no gamerscore.
// achievements.bin records: the achievement id, its XACHIEVEMENT_DETAILS flags (ACHIEVED), the FILETIME it was earned
// (the 64-bit count of 100 ns since 1601, UTC: what the port's FILETIME holds in memory, whatever its halves are named).
} // extern "C"

namespace {

const DWORD k_achievement_record_capacity = 128;
const DWORD k_xachievement_details_achieved = 0x00020000;
const DWORD k_xachievement_details_label = 0x1, k_xachievement_details_description = 0x2,
            k_xachievement_details_unachieved = 0x4;
// the strings' largest sizes, in WCHARs with their terminators (XACHIEVEMENT_MAX_LABEL_LENGTH, ..._DESC_..., ..._UNACH_...)
const DWORD k_achievement_label_chars = 33, k_achievement_description_chars = 100, k_achievement_unachieved_chars = 100;

// XACHIEVEMENT_DETAILS (36 bytes on the console and on i686)
struct s_achievement_details {
    DWORD dwId;
    LPWSTR pwszLabel;
    LPWSTR pwszDescription;
    LPWSTR pwszUnachieved;
    DWORD dwImageId;
    DWORD dwCred;
    FILETIME ftAchieved;
    DWORD dwFlags;
};
typedef char achievement_details_size[sizeof(s_achievement_details) == (sizeof(void *) == 8 ? 56 : 36) ? 1 : -1];

struct s_achievement_record {
    DWORD id;
    DWORD flags;
    FILETIME achieved;
};

// One table per user: each signed-in local profile earns its own, kept in its own file.
struct s_achievements {
    s_achievement_record records[k_achievement_record_capacity];
    DWORD count;
    DWORD store; // e_store_state
    DWORD serial; // the sign-in the table is of (port_profile_user_serial)
};
s_achievements g_achievements[XUSER_MAX_COUNT];

DWORD get16_be(const BYTE *p) { return (DWORD)p[0] << 8 | p[1]; }
DWORD get32_be(const BYTE *p) { return get16_be(p) << 16 | get16_be(p + 2); }

// The parts of the SPA this file reads, found once (under the lock).
struct s_spa {
    bool searched;
    const BYTE *spa_base;
    DWORD spa_size;
    const BYTE *achievements; // XACH entries, 0x24 bytes each (big-endian)
    DWORD achievement_count;
    const BYTE *strings; // the XSTR entries of the default language: id, length, UTF-8
    DWORD strings_size;
    DWORD string_count;
};
s_spa g_spa;

extern "C" long XexGetModuleHandle(char *module_name, void **module);
extern "C" long XexGetModuleSection(void *module, const char *section_name, void **section_data, DWORD *section_size);

// The XDBF entry (section, id) of the SPA at `spa` (`size` bytes, bounds checked), and its size; 0 if absent.
const BYTE *spa_entry(const BYTE *spa, DWORD size, DWORD section, DWORD id, DWORD *entry_size)
{
    if (spa == 0 || size < 24 || get32_be(spa) != 0x58444246) // "XDBF"
        return 0;
    const DWORD entry_count = get32_be(spa + 8), entry_used = get32_be(spa + 12), free_count = get32_be(spa + 16);
    if (entry_count > 0x10000 || free_count > 0x10000 || entry_used > entry_count)
        return 0;
    const DWORD base = 24 + 18 * entry_count + 8 * free_count;
    if (base > size)
        return 0;
    for (DWORD i = 0; i < entry_used; i++) {
        const BYTE *entry = spa + 24 + 18 * i;
        const DWORD offset = get32_be(entry + 10), length = get32_be(entry + 14);
        if (get16_be(entry) != section || get32_be(entry + 2) != 0 || get32_be(entry + 6) != id)
            continue;
        if (offset > size - base || length > size - base - offset)
            return 0;
        *entry_size = length;
        return spa + base + offset;
    }
    return 0;
}

// Parses an XDBF SPA blob into `out`. Returns true when valid XDBF with an XACH table.
bool spa_parse(const BYTE *spa, DWORD size, s_spa *out)
{
    memset(out, 0, sizeof(*out));
    if (spa == 0 || size < 24 || get32_be(spa) != 0x58444246) // "XDBF"
        return false;
    out->spa_base = spa;
    out->spa_size = size;
    DWORD length = 0;
    const BYTE *xach = spa_entry(spa, size, 1, 0x58414348, &length); // metadata "XACH"
    if (xach != 0 && length >= 14 && get32_be(xach) == 0x58414348) {
        const DWORD count = get16_be(xach + 12);
        if (14 + 0x24 * count <= length) {
            out->achievements = xach + 14;
            out->achievement_count = count;
        }
    }
    DWORD language = 1; // English, unless the SPA says otherwise
    const BYTE *xstc = spa_entry(spa, size, 1, 0x58535443, &length); // metadata "XSTC": the default language
    if (xstc != 0 && length >= 16 && get32_be(xstc) == 0x58535443)
        language = get32_be(xstc + 12);
    const BYTE *xstr = spa_entry(spa, size, 3, language, &length); // the string table of that language
    if (xstr != 0 && length >= 14 && get32_be(xstr) == 0x58535452) {
        out->strings = xstr + 14;
        out->strings_size = length - 14;
        out->string_count = get16_be(xstr + 12);
    }
    return out->achievement_count != 0;
}

// Under the lock.
const s_spa &spa_get()
{
    if (g_spa.searched)
        return g_spa;
    g_spa.searched = true;
    void *module = 0, *data = 0;
    DWORD size = 0;
    const char *k_section = "4D5307E6"; // k_title_id in hexadecimal
    s_spa base = {};
    const bool have_base = XexGetModuleHandle(0, &module) == 0
        && XexGetModuleSection(module, k_section, &data, &size) == 0 && data != 0;
    if (have_base)
        spa_parse((const BYTE *)data, size, &base);

    // Prefer the expanded ODST Disc 2 SPA (79 achievements / 1,750 G + all DLC icons) when installed in the DLC folder.
    const DWORD k_dlc_spa_capacity = 2u * 1024u * 1024u;
    BYTE *dlc_buf = (BYTE *)HeapAlloc(GetProcessHeap(), 0, k_dlc_spa_capacity);
    bool used_dlc = false;
    if (dlc_buf != 0) {
        unsigned int dlc_size = 0;
        s_spa dlc = {};
        if (port_content_dlc_read_file("odst_mythic", "4D5307E6.spa", dlc_buf, k_dlc_spa_capacity, &dlc_size) == 0
            && spa_parse(dlc_buf, dlc_size, &dlc) && dlc.achievement_count > base.achievement_count) {
            g_spa = dlc;
            g_spa.searched = true;
            used_dlc = true;
        } else {
            HeapFree(GetProcessHeap(), 0, dlc_buf);
        }
    }

    if (!used_dlc) {
        if (!have_base) {
            store_warning("no SPA in default.xex: achievements enumerate without gamerscore;", k_section, 0);
            return g_spa;
        }
        g_spa = base;
        g_spa.searched = true;
    }

    if (trace_on()) {
        s_trace_line line;
        line.add("achievements: SPA ").add(used_dlc ? "odst_mythic/4D5307E6.spa" : k_section).add(": ")
            .number(g_spa.achievement_count).add(" achievements, ")
            .number(g_spa.string_count).add(" strings");
        line.write();
    }
    return g_spa;
}

// Under the lock: string `id` of the SPA as UTF-16 into `out` (`chars` WCHARs, terminated, cut short if need be).
void spa_string(DWORD id, LPWSTR out, DWORD chars)
{
    out[0] = 0;
    const s_spa &spa = spa_get();
    DWORD at = 0;
    for (DWORD i = 0; i < spa.string_count && at + 4 <= spa.strings_size; i++) {
        const DWORD string_id = get16_be(spa.strings + at), length = get16_be(spa.strings + at + 2);
        at += 4;
        if (length > spa.strings_size - at)
            return;
        if (string_id == id) {
            const BYTE *s = spa.strings + at;
            DWORD n = 0;
            for (DWORD k = 0; k < length && n + 1 < chars;) {
                DWORD c = s[k], extra = c >= 0xf0 ? 3 : c >= 0xe0 ? 2 : c >= 0xc0 ? 1 : 0;
                c &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : extra == 1 ? 0x1f : 0x7f;
                if (extra != 0 && k + extra >= length)
                    break; // a sequence cut short
                for (DWORD e = 1; e <= extra; e++)
                    c = c << 6 | (s[k + e] & 0x3f);
                k += 1 + extra;
                if (c >= 0x10000) {
                    if (n + 2 >= chars)
                        break;
                    c -= 0x10000;
                    out[n++] = (WCHAR)(0xd800 | (c >> 10));
                    out[n++] = (WCHAR)(0xdc00 | (c & 0x3ff));
                } else {
                    out[n++] = (WCHAR)c;
                }
            }
            out[n] = 0;
            return;
        }
        at += length;
    }
}

// UTF-16 into UTF-8 (`capacity` bytes, NUL-terminated; a character that does not fit is left out with what follows).
void wide_to_utf8(const WCHAR *from, char *to, DWORD capacity)
{
    DWORD n = 0;
    for (DWORD i = 0; from[i] != 0; i++) {
        DWORD code = from[i];
        if (code >= 0xd800 && code <= 0xdbff && from[i + 1] >= 0xdc00 && from[i + 1] <= 0xdfff) {
            code = 0x10000 + ((code - 0xd800) << 10) + (from[i + 1] - 0xdc00);
            i++;
        } else if (code >= 0xd800 && code <= 0xdfff) {
            code = 0xfffd;
        }
        const DWORD size = code < 0x80 ? 1 : code < 0x800 ? 2 : code < 0x10000 ? 3 : 4;
        if (n + size + 1 > capacity)
            break;
        static const BYTE lead[5] = { 0, 0x00, 0xc0, 0xe0, 0xf0 };
        to[n++] = (char)(lead[size] | code >> (6 * (size - 1)));
        for (DWORD k = 1; k < size; k++)
            to[n++] = (char)(0x80 | ((code >> (6 * (size - 1 - k))) & 0x3f));
    }
    to[n] = 0;
}

bool spa_has_achievement(DWORD id)
{
    const s_spa &spa = spa_get();
    for (DWORD i = 0; i < spa.achievement_count; i++)
        if (get16_be(spa.achievements + 0x24 * i) == id)
            return true;
    return false;
}

// An achievement newly earned, with its label, gamerscore ("20 G"), and image ID from the SPA.
struct s_achievement_toast {
    char label[GUIDE_LABEL * 2];
    char gamerscore[16];
    DWORD image_id;
};

// Under the lock.
void achievement_toast_details(DWORD id, s_achievement_toast *toast)
{
    const s_spa &spa = spa_get();
    toast->label[0] = toast->gamerscore[0] = 0;
    toast->image_id = 0;
    for (DWORD i = 0; i < spa.achievement_count; i++) {
        const BYTE *entry = spa.achievements + 0x24 * i;
        if (get16_be(entry) != id)
            continue;
        toast->image_id = get32_be(entry + 8);
        WCHAR label[k_achievement_label_chars];
        spa_string(get16_be(entry + 2), label, k_achievement_label_chars);
        wide_to_utf8(label, toast->label, sizeof(toast->label));
        s_trace_line score; // its number formatting, no CRT
        score.number(get16_be(entry + 12)).add(" G");
        memcpy(toast->gamerscore, score.text, score.length + 1 < sizeof(toast->gamerscore) ? score.length + 1 : 1);
        return;
    }
    s_trace_line label;
    label.add("Achievement ").number(id);
    memcpy(toast->label, label.text, label.length + 1);
}

// Under the lock.
const s_achievement_record *achievement_record_find(const s_achievements *table, DWORD id)
{
    for (DWORD i = 0; i < table->count; i++)
        if (table->records[i].id == id)
            return &table->records[i];
    return 0;
}

// Under the lock: the records from a store payload. Returns false (none kept) when they do not check out.
bool achievements_parse(s_achievements *table, const BYTE *payload, DWORD size, DWORD count)
{
    table->count = 0;
    if (count > k_achievement_record_capacity || size != count * 16)
        return false;
    for (DWORD i = 0; i < count; i++) {
        const BYTE *p = payload + 16 * i;
        s_achievement_record record;
        record.id = get32(p);
        record.flags = get32(p + 4);
        const unsigned long long ticks = (unsigned long long)get32(p + 12) << 32 | get32(p + 8);
        memcpy(&record.achieved, &ticks, sizeof(ticks));
        if ((record.flags & k_xachievement_details_achieved) == 0 || achievement_record_find(table, record.id) != 0) {
            table->count = 0;
            return false;
        }
        table->records[table->count++] = record;
    }
    return true;
}

void trace_achievement_store(const char *what, const char *name, DWORD count)
{
    if (!trace_on())
        return;
    s_trace_line line;
    line.add("achievements: ").add(what).add(" ").add(name).add(" (").number(count).add(" earned)");
    line.write();
}

// Under the lock: user `user_index`'s achievements.
s_achievements *achievements_load(DWORD user_index)
{
    s_achievements *table = &g_achievements[user_index];
    const DWORD serial = port_profile_user_serial(user_index);
    if (table->store != _store_unloaded && table->serial == serial)
        return table;
    table->serial = serial;
    table->count = 0;
    if (port_profile_user_is_guest(user_index)) {
        table->store = _store_guest; // guests earn no achievements
        return table;
    }
    char name[64];
    store_file_name(user_index, "achievements.bin", name);
    const BYTE *payload = 0;
    DWORD payload_size = 0, count = 0;
    table->store = _store_ready;
    table->count = 0;
    switch (store_read(name, k_achievements_magic, &payload, &payload_size, &count)) {
    case _store_load_missing:
        trace_achievement_store("none earned yet:", name, 0);
        break;
    case _store_load_ok:
        if (achievements_parse(table, payload, payload_size, count)) {
            trace_achievement_store("loaded", name, table->count);
            break;
        }
        store_set_aside(name);
        break;
    case _store_load_corrupt:
        store_set_aside(name);
        break;
    case _store_load_unreadable:
        table->store = _store_unavailable;
        break;
    }
    return table;
}

// Under the lock: user `user_index`'s achievements.
DWORD achievements_save(DWORD user_index)
{
    const s_achievements *table = &g_achievements[user_index];
    if (table->store != _store_ready)
        return ERROR_SUCCESS;
    char name[64];
    store_file_name(user_index, "achievements.bin", name);
    BYTE *p = g_store_buffer + k_store_header_size;
    for (DWORD i = 0; i < table->count; i++, p += 16) {
        put32(p, table->records[i].id);
        put32(p + 4, table->records[i].flags);
        unsigned long long ticks;
        memcpy(&ticks, &table->records[i].achieved, sizeof(ticks));
        put32(p + 8, (DWORD)ticks);
        put32(p + 12, (DWORD)(ticks >> 32));
    }
    const DWORD result = store_write(name, k_achievements_magic, table->count, 16 * table->count);
    if (result == ERROR_SUCCESS)
        trace_achievement_store("saved", name, table->count);
    return result;
}

// The bytes an enumeration item takes: the details, then the strings asked for.
DWORD achievement_item_size(DWORD detail_flags)
{
    DWORD size = sizeof(s_achievement_details);
    if ((detail_flags & k_xachievement_details_label) != 0)
        size += k_achievement_label_chars * sizeof(WCHAR);
    if ((detail_flags & k_xachievement_details_description) != 0)
        size += k_achievement_description_chars * sizeof(WCHAR);
    if ((detail_flags & k_xachievement_details_unachieved) != 0)
        size += k_achievement_unachieved_chars * sizeof(WCHAR);
    return size;
}

// Under the lock: the next achievements. The details come first in the buffer, the strings after all of them.
DWORD enumerate_achievements(s_enumeration *enumeration, BYTE *buffer, DWORD buffer_size, DWORD *count)
{
    *count = 0;
    const s_achievements *table = achievements_load(enumeration->user_index);
    const s_spa &spa = spa_get();
    const DWORD total = spa.achievement_count != 0 ? spa.achievement_count : table->count;
    if (enumeration->position >= total)
        return ERROR_NO_MORE_FILES;
    DWORD n = enumeration->items_per_call;
    if (buffer_size / enumeration->item_size < n)
        n = buffer_size / enumeration->item_size;
    if (n == 0)
        return ERROR_INSUFFICIENT_BUFFER;
    if (n > total - enumeration->position)
        n = total - enumeration->position;
    s_achievement_details *details = (s_achievement_details *)buffer;
    WCHAR *strings = (WCHAR *)(buffer + n * sizeof(s_achievement_details));
    DWORD earned = 0;
    for (DWORD i = 0; i < n; i++) {
        const DWORD index = enumeration->position + i;
        s_achievement_details *item = &details[i];
        memset(item, 0, sizeof(*item));
        const s_achievement_record *record = 0;
        if (spa.achievement_count != 0) {
            const BYTE *entry = spa.achievements + 0x24 * index;
            item->dwId = get16_be(entry);
            item->dwImageId = get32_be(entry + 8);
            item->dwCred = get16_be(entry + 12);
            item->dwFlags = get32_be(entry + 16) & ~(k_xachievement_details_achieved | 0x00010000);
            record = achievement_record_find(table, item->dwId);
        } else {
            record = &table->records[index];
            item->dwId = record->id;
        }
        // the strings asked for (empty without the SPA)
        const BYTE *entry = spa.achievement_count != 0 ? spa.achievements + 0x24 * index : 0;
        const DWORD string_flags[3] = { k_xachievement_details_label, k_xachievement_details_description,
            k_xachievement_details_unachieved };
        const DWORD string_chars[3] = { k_achievement_label_chars, k_achievement_description_chars,
            k_achievement_unachieved_chars };
        LPWSTR *string_pointers[3] = { &item->pwszLabel, &item->pwszDescription, &item->pwszUnachieved };
        for (DWORD s = 0; s < 3; s++) {
            if ((enumeration->flags & string_flags[s]) == 0)
                continue;
            *string_pointers[s] = strings;
            strings[0] = 0;
            if (entry != 0)
                spa_string(get16_be(entry + 2 + 2 * s), strings, string_chars[s]); // label, description, unachieved ids
            strings += string_chars[s];
        }
        if (record != 0) {
            item->dwFlags |= record->flags;
            item->ftAchieved = record->achieved;
            earned++;
        }
    }
    enumeration->position += n;
    *count = n;
    if (trace_on()) {
        s_trace_line line;
        line.add("achievements: XEnumerate -> ").number(n).add(" achievements, ").number(earned).add(" earned");
        line.write();
    }
    return ERROR_SUCCESS;
}

} // namespace

extern "C" {

PORT_MS_ABI int port_spa_image_get(unsigned int image_id, const void **out_data, unsigned int *out_size)
{
    if (out_data == 0 || out_size == 0)
        return 0;
    *out_data = 0;
    *out_size = 0;
    if (image_id == 0)
        return 0;

    lock_enter(&g_lock);
    const s_spa &spa = spa_get();
    DWORD length = 0;
    const BYTE *image = spa_entry(spa.spa_base, spa.spa_size, 2, image_id, &length);
    lock_leave(&g_lock);

    static const BYTE png_signature[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
    if (image == 0 || length <= sizeof(png_signature) || memcmp(image, png_signature, sizeof(png_signature)) != 0)
        return 0;
    *out_data = image;
    *out_size = length;
    return 1;
}

// Each achievement newly earned is recorded with the time; those already earned stay as they were. Ids the SPA does
// not have are refused (ERROR_INVALID_PARAMETER), as XAM refuses them; nothing is recorded then. A guest earns none: it
// has no profile to keep them in.
DWORD XUserWriteAchievements(DWORD dwNumAchievements, const XUSER_ACHIEVEMENT *pAchievements, PXOVERLAPPED pOverlapped)
{
    if (dwNumAchievements == 0 || pAchievements == 0)
        return complete(pOverlapped, ERROR_INVALID_PARAMETER);
    DWORD result = ERROR_SUCCESS;
    lock_enter(&g_lock);
    const bool known_ids = spa_get().achievement_count != 0;
    for (DWORD i = 0; i < dwNumAchievements && result == ERROR_SUCCESS; i++) {
        if (pAchievements[i].dwUserIndex >= XUSER_MAX_COUNT || !port_profile_signed_in(pAchievements[i].dwUserIndex))
            result = ERROR_NOT_LOGGED_ON;
        else if (known_ids && !spa_has_achievement(pAchievements[i].dwAchievementId))
            result = ERROR_INVALID_PARAMETER;
    }
    DWORD earned_by = 0; // a bit per user who earned one
    // the achievements newly earned, in order: one Achievement Unlocked toast each (guide_ingame.h), as the console
    // shows every one of a write, posted once the lock is released (Halo 3 writes one user's at a time: all those it
    // granted since its last write). No more can be newly earned than the users' profiles can keep.
    DWORD toast_ids[XUSER_MAX_COUNT * k_achievement_record_capacity];
    DWORD toast_count = 0;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    for (DWORD i = 0; i < dwNumAchievements && result == ERROR_SUCCESS; i++) {
        const DWORD user_index = pAchievements[i].dwUserIndex, id = pAchievements[i].dwAchievementId;
        s_achievements *table = achievements_load(user_index);
        if (table->store == _store_guest || achievement_record_find(table, id) != 0)
            continue; // a guest earns none (it has no profile); one earned already stays as it was
        if (table->count == k_achievement_record_capacity) {
            result = ERROR_NOT_ENOUGH_MEMORY;
            break;
        }
        s_achievement_record *record = &table->records[table->count++];
        record->id = id;
        record->flags = k_xachievement_details_achieved;
        record->achieved = now;
        earned_by |= 1u << user_index;
        toast_ids[toast_count++] = id; // one record each: at most XUSER_MAX_COUNT * k_achievement_record_capacity
        if (trace_on()) {
            s_trace_line line;
            line.add("achievements: user ").number(user_index).add(" earned ").hex(id);
            line.write();
        }
    }
    for (DWORD user_index = 0; user_index < XUSER_MAX_COUNT; user_index++) {
        if ((earned_by & (1u << user_index)) == 0)
            continue;
        const DWORD saved = achievements_save(user_index);
        if (result == ERROR_SUCCESS)
            result = saved;
    }
    lock_leave(&g_lock);
    for (DWORD i = 0; i < toast_count; i++) {
        s_achievement_toast toast;
        lock_enter(&g_lock);
        achievement_toast_details(toast_ids[i], &toast);
        lock_leave(&g_lock);
        guide_toast_post_icon(GUIDE_TOAST_ACHIEVEMENT, toast.image_id, "Achievement Unlocked", toast.label,
                              toast.gamerscore, "");
    }
    return complete(pOverlapped, result);
}

// The title's achievements for a signed-in user (xuid 0 or their own, port_profile_user_owns_xuid: with Xbox Live user
// 0's is the account's; another gamer's are not kept).
DWORD XUserCreateAchievementEnumerator(DWORD dwTitleId, DWORD dwUserIndex, XUID xuid, DWORD dwDetailFlags,
    DWORD dwStartingIndex, DWORD cItem, PDWORD pcbBuffer, PHANDLE ph)
{
    if (pcbBuffer != 0)
        *pcbBuffer = 0;
    if (ph != 0)
        *ph = INVALID_HANDLE_VALUE;
    if (dwUserIndex >= XUSER_MAX_COUNT || cItem == 0 || pcbBuffer == 0 || ph == 0)
        return ERROR_INVALID_PARAMETER;
    if (!port_profile_signed_in(dwUserIndex) || (xuid != 0 && !port_profile_user_owns_xuid(dwUserIndex, xuid)))
        return ERROR_NOT_LOGGED_ON;
    s_enumeration record = {};
    record.kind = dwTitleId == 0 || dwTitleId == k_title_id ? _enumeration_achievements : _enumeration_empty;
    record.user_index = dwUserIndex;
    record.items_per_call = cItem;
    record.item_size = achievement_item_size(dwDetailFlags);
    record.flags = dwDetailFlags;
    const DWORD result = create_enumeration(record, pcbBuffer, ph);
    if (result == ERROR_SUCCESS) {
        lock_enter(&g_lock);
        s_enumeration *enumeration = find_enumeration(*ph);
        if (enumeration != 0)
            enumeration->position = dwStartingIndex;
        lock_leave(&g_lock);
    }
    return result;
}

// The guide's achievements page (port/sdk/xam/guide.h): every achievement of the title's SPA, earned or not by the
// user's profile (a guest earns none), with its gamerscore, label and description; a secret one not earned shows its
// "unachieved" text instead (the SPA's XACHIEVEMENT_DETAILS_SHOWUNACHIEVED, 0x8, clear), as the console's viewer does.
unsigned long port_guide_achievements(unsigned long user_index, guide_achievement *out, unsigned long capacity)
{
    if (user_index >= XUSER_MAX_COUNT || out == 0 || !port_profile_signed_in(user_index))
        return 0;
    lock_enter(&g_lock);
    const s_achievements *table = achievements_load(user_index);
    const s_spa &spa = spa_get();
    unsigned long n = 0;
    for (; n < spa.achievement_count && n < capacity; n++) {
        const BYTE *entry = spa.achievements + 0x24 * n;
        guide_achievement *achievement = &out[n];
        memset(achievement, 0, sizeof(*achievement));
        achievement->id = get16_be(entry);
        achievement->image_id = get32_be(entry + 8);
        achievement->gamerscore = get16_be(entry + 12);
        achievement->earned = achievement_record_find(table, achievement->id) != 0;
        const bool secret = !achievement->earned && (get32_be(entry + 16) & 0x8) == 0;
        WCHAR text[k_achievement_description_chars];
        spa_string(get16_be(entry + 2), text, k_achievement_label_chars);
        wide_to_utf8(text, achievement->label, sizeof(achievement->label));
        spa_string(get16_be(entry + (secret ? 6 : 4)), text, k_achievement_description_chars);
        wide_to_utf8(text, achievement->description, sizeof(achievement->description));
    }
    lock_leave(&g_lock);
    return n;
}

// Total gamerscore earned by user_index across every achievement in the active SPA (1000 G base, 1750 G with ODST DLC).
extern "C" long port_achievements_earned_gamerscore(long user_index)
{
    if (user_index < 0 || (unsigned long)user_index >= XUSER_MAX_COUNT || !port_profile_signed_in((unsigned long)user_index))
        return 0;
    lock_enter(&g_lock);
    const s_achievements *table = achievements_load((unsigned long)user_index);
    const s_spa &spa = spa_get();
    long total = 0;
    for (DWORD n = 0; n < spa.achievement_count; n++) {
        const BYTE *entry = spa.achievements + 0x24 * n;
        if (achievement_record_find(table, get16_be(entry)) != 0)
            total += (long)get16_be(entry + 12);
    }
    lock_leave(&g_lock);
    return total;
}

DWORD XFriendsCreateEnumerator(DWORD dwUserIndex, DWORD dwStartingIndex, DWORD dwFriendsToReturn, DWORD *pcbBuffer,
    HANDLE *ph)
{
    (void)dwUserIndex; (void)dwStartingIndex; (void)dwFriendsToReturn;
    if (pcbBuffer != 0)
        *pcbBuffer = 0;
    if (ph != 0)
        *ph = INVALID_HANDLE_VALUE;
    return ERROR_NOT_LOGGED_ON;
}

// The invite user dwUserIndex accepted last in the guide (port/sdk/online/invite.h: a Live invite or a lobby code),
// after XN_LIVE_INVITE_ACCEPTED told the title of it: the inviter, the host's session and fFromGameInvite TRUE. With
// none, ERROR_NOT_FOUND; for a user not signed in, ERROR_NOT_LOGGED_ON.
DWORD XInviteGetAcceptedInfo(DWORD dwUserIndex, XINVITE_INFO *pInfo)
{
    if (pInfo == 0 || dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_INVALID_PARAMETER;
    memset(pInfo, 0, sizeof(*pInfo));
    if (!port_profile_signed_in(dwUserIndex))
        return ERROR_NOT_LOGGED_ON;
    port_invite invite;
    const bool there = port_invite_accepted(dwUserIndex, &invite) != 0;
    if (there) {
        pInfo->xuidInvitee = invite.invitee_xuid;
        pInfo->xuidInviter = invite.inviter_xuid;
        pInfo->dwTitleID = invite.title_id;
        memcpy(&pInfo->hostInfo, invite.session, sizeof(pInfo->hostInfo));
        pInfo->fFromGameInvite = invite.from_game_invite ? TRUE : FALSE;
    }
    if (port_invite_trace()) {
        s_trace_line line;
        line.add("invite: XInviteGetAcceptedInfo(").number(dwUserIndex).add(") -> ");
        if (there) {
            unsigned long long id = 0;
            for (int k = 0; k < 8; k++)
                id = id << 8 | invite.session[k];
            line.add("ERROR_SUCCESS, lobby ").hex(id, 16);
            line.add(" of ").add(invite.inviter);
        } else {
            line.add("ERROR_NOT_FOUND");
        }
        line.write();
    }
    return there ? ERROR_SUCCESS : ERROR_NOT_FOUND;
}

// With Xbox Live, each invitee gets an invite to the session that has the user's presence (port_invite_presence: the
// XSESSION_CREATE_USES_PRESENCE session, as XAM sends), through the Live server (live.h); the message text is not
// sent (the server delivers none). Without Live, or with no presence session to invite to, ERROR_NOT_LOGGED_ON /
// ERROR_INVALID_PARAMETER as before; a server that keeps no invites, ERROR_NOT_SUPPORTED.
DWORD XInviteSend(DWORD dwUserIndex, DWORD cInvitees, const XUID *pXuidInvitees, const WCHAR *pszText,
    XOVERLAPPED *pXOverlapped)
{
    (void)pszText;
    if (dwUserIndex >= XUSER_MAX_COUNT || cInvitees == 0 || pXuidInvitees == 0)
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    if (XUserGetSigninState(dwUserIndex) != eXUserSigninState_SignedInToLive)
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    unsigned char session[PORT_INVITE_SESSION_SIZE];
    if (!port_invite_presence(session))
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    DWORD result = ERROR_SUCCESS;
    for (DWORD i = 0; i < cInvitees && result == ERROR_SUCCESS; i++) {
        const int sent = port_live_invite_send(pXuidInvitees[i], session, PORT_INVITE_CLASS_LIVE);
        result = sent == 0 ? ERROR_SUCCESS : sent == PORT_LIVE_UNSUPPORTED ? ERROR_NOT_SUPPORTED : ERROR_NOT_LOGGED_ON;
    }
    return complete(pXOverlapped, result);
}

DWORD XStringVerify(DWORD dwFlags, const CHAR *szLocale, DWORD dwNumStrings, const STRING_DATA *pStringData,
    DWORD cbResults, STRING_VERIFY_RESPONSE *pResults, XOVERLAPPED *pXOverlapped)
{
    (void)dwFlags; (void)szLocale; (void)dwNumStrings; (void)pStringData; (void)cbResults; (void)pResults;
    return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
}

// ---- custom gamercard actions (the guide's player menu) ----

BOOL XCustomGetCurrentGamercard(DWORD *pdwUserIndex, XUID *pXuid)
{
    (void)pdwUserIndex; (void)pXuid;
    return FALSE; // no guide: no gamercard is open
}

DWORD XCustomGetLastActionPressEx(DWORD *pdwUserIndex, DWORD *pdwActionId, XUID *pXuid, BYTE *pbPayload,
    WORD *pcbPayload)
{
    (void)pdwUserIndex; (void)pdwActionId; (void)pXuid; (void)pbPayload; (void)pcbPayload;
    return FALSE; // no action was pressed
}

void XCustomRegisterDynamicActions(void) {}
void XCustomUnregisterDynamicActions(void) {}

DWORD XCustomSetDynamicActions(DWORD dwUserIndex, XUID xuid, const XCUSTOMACTION *pCustomActions, WORD cCustomActions)
{
    (void)dwUserIndex; (void)xuid; (void)pCustomActions; (void)cCustomActions;
    return ERROR_SUCCESS;
}

// ---- guide UI: the marketplace and the dirty-disc screen (the guide's other pages: guide_ui.cpp) ----

// XShowMarketplaceUI (0x59ffa8 in uiapithunk2.obj, the next thunk; the game's name for it is its retail address,
// FUN_8259ffa8): the marketplace needs an Xbox Live account, and the signed-in profile is local, so the guide
// reports that nobody is signed in to Live.
DWORD XShowMarketplaceUI(DWORD dwUserIndex, DWORD dwEntryPoint, ULONGLONG qwOfferID, DWORD dwContentCategories)
{
    (void)dwUserIndex; (void)dwEntryPoint; (void)qwOfferID; (void)dwContentCategories;
    return ERROR_NOT_LOGGED_ON;
}

extern "C" DWORD FUN_8259ffa8(DWORD dwUserIndex, DWORD dwEntryPoint, ULONGLONG qwOfferID, DWORD dwContentCategories)
{
    return XShowMarketplaceUI(dwUserIndex, dwEntryPoint, qwOfferID, dwContentCategories);
}

// XShowMarketplaceDownloadItemsUI (0x59ff68 in uiapithunk2.obj; the game calls it main_game_submit_live_request)
DWORD main_game_submit_live_request(DWORD dwUserIndex, DWORD dwEntryPoint, const ULONGLONG *pOfferIDs,
    DWORD dwOfferIdCount, HRESULT *phrResult, PXOVERLAPPED pOverlapped)
{
    (void)dwUserIndex; (void)dwEntryPoint; (void)pOfferIDs; (void)dwOfferIdCount;
    if (phrResult != 0)
        *phrResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
    return complete(pOverlapped, ERROR_CANCELLED);
}

// The disc could not be read: the Xbox shows its dirty-disc screen and never returns to the title.
void XShowDirtyDiscErrorUI(DWORD dwUserIndex)
{
    (void)dwUserIndex;
    port_write_error("port: XShowDirtyDiscErrorUI: the game data could not be read; exiting\n");
    port_run_atexit();
    port_exit(1);
    __builtin_unreachable();
}

// ---- system applications ----

// XMsgInProcessCall to the music player (HXAMAPP_XMP). There is no system music player: the title always has
// playback control. XMPGetPlaybackController (0x7001B; param1 -> {client, DWORD *controller, DWORD *locked}) reports
// controller 0 (the title) and not locked; Halo 3 asks it before campaign starts and shows "MUSIC WARNING" when the
// outputs say the user's music has control. Other music player requests (override / restore background music,
// set controller) succeed with nothing to change, as with the player idle. Other system applications do not exist.
#define PORT_XMP_GET_PLAYBACK_CONTROLLER 0x0007001Bu
HRESULT XMsgInProcessCall(HXAMAPP hxamapp, DWORD dwMessage, UINT_PTR param1, UINT_PTR param2)
{
    (void)param2;
    if (hxamapp != HXAMAPP_XMP)
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    if (dwMessage == PORT_XMP_GET_PLAYBACK_CONTROLLER && param1 != 0) {
        struct s_xmp_get_playback_controller {
            DWORD client;
            DWORD *controller;
            DWORD *locked;
        } *request = (struct s_xmp_get_playback_controller *)param1;
        if (request->controller != 0)
            *request->controller = 0;
        if (request->locked != 0)
            *request->locked = 0;
    }
    return S_OK;
}

// ---- XNet services ----

} // extern "C"

namespace {

const DWORD k_service_qos_pool_size = 8;
struct s_service_qos_slot {
    volatile LONG in_use;
    XNQOS qos;
};
s_service_qos_slot g_service_qos_pool[k_service_qos_pool_size];

} // namespace

extern "C" {

int port_live_qos_service_release(void *qos)
{
    if (qos == 0)
        return 0;
    for (DWORD i = 0; i < k_service_qos_pool_size; i++) {
        if (qos == &g_service_qos_pool[i].qos) {
            InterlockedExchange(&g_service_qos_pool[i].in_use, 0);
            return 1;
        }
    }
    return 0;
}

INT XNetQosServiceLookup(DWORD dwFlags, HANDLE hEvent, XNQOS **ppxnqos)
{
    (void)dwFlags;
    if (ppxnqos == 0)
        return WSAEINVAL;
    *ppxnqos = 0;
    if (!port_live_enabled())
        return WSAENETDOWN;
    for (DWORD i = 0; i < k_service_qos_pool_size; i++) {
        if (InterlockedCompareExchange(&g_service_qos_pool[i].in_use, 1, 0) == 0) {
            XNQOS &q = g_service_qos_pool[i].qos;
            memset(&q, 0, sizeof(q));
            q.cxnqos = 1;
            q.cxnqosPending = 0;
            q.axnqosinfo[0].bFlags = XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_TARGET_CONTACTED;
            q.axnqosinfo[0].cProbesXmit = 4;
            q.axnqosinfo[0].cProbesRecv = 4;
            q.axnqosinfo[0].wRttMinInMsecs = 20;
            q.axnqosinfo[0].wRttMedInMsecs = 20;
            q.axnqosinfo[0].dwUpBitsPerSec = 10000000;
            q.axnqosinfo[0].dwDnBitsPerSec = 10000000;
            *ppxnqos = &q;
            if (hEvent != 0)
                SetEvent(hEvent);
            return 0;
        }
    }
    return WSAENOBUFS;
}

INT XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR *pina)
{
    if (pina == 0)
        return WSAEINVAL;
    pina->s_addr = 0;
    if (!port_live_enabled())
        return WSAENETDOWN;
    unsigned int secure = 0;
    int err = port_live_server_to_inaddr(ntohl(ina.s_addr), dwServiceId, &secure);
    if (err != 0)
        return err;
    pina->s_addr = htonl(secure);
    return 0;
}

// ---- sessions ----

} // extern "C"

namespace {

const DWORD k_session_capacity = 16;
const DWORD k_session_member_capacity = 32;

struct s_session {
    HANDLE handle;
    DWORD flags, user_index, max_public, max_private;
    XSESSION_STATE state;
    ULONGLONG nonce;
    XSESSION_INFO info;
    DWORD member_count;
    XSESSION_MEMBER members[k_session_member_capacity];
};

s_session g_sessions[k_session_capacity];
CRITICAL_SECTION g_session_lock;
volatile LONG g_session_lock_ready;

void session_lock()
{
    if (g_session_lock_ready != 2) {
        if (InterlockedCompareExchange(&g_session_lock_ready, 1, 0) == 0) {
            InitializeCriticalSection(&g_session_lock);
            InterlockedExchange(&g_session_lock_ready, 2);
        }
        while (g_session_lock_ready != 2)
            SwitchToThread();
    }
    EnterCriticalSection(&g_session_lock);
}
void session_unlock() { LeaveCriticalSection(&g_session_lock); }

// Under the lock.
s_session *session_of(HANDLE handle)
{
    for (DWORD i = 0; i < k_session_capacity; i++)
        if (g_sessions[i].handle != 0 && g_sessions[i].handle == handle)
            return &g_sessions[i];
    return 0;
}

void random_bytes(void *buffer, DWORD size) { XNetRandom((BYTE *)buffer, size); }

DWORD members_in(const s_session *session, BOOL private_slots)
{
    DWORD count = 0;
    for (DWORD i = 0; i < session->member_count; i++)
        if (((session->members[i].dwFlags & XSESSION_MEMBER_FLAGS_PRIVATE_SLOT) != 0) == (private_slots != 0))
            count++;
    return count;
}

bool convert_user_property(const XUSER_PROPERTY &src, port_live_property &dst)
{
    memset(&dst, 0, sizeof(dst));
    dst.id = src.dwPropertyId;
    switch (src.value.type) {
    case XUSER_DATA_TYPE_INT32:
        dst.type = PORT_LIVE_PROP_INT32;
        dst.int_val = src.value.nData;
        return true;
    case XUSER_DATA_TYPE_INT64:
        dst.type = PORT_LIVE_PROP_INT64;
        dst.int_val = src.value.i64Data;
        return true;
    case XUSER_DATA_TYPE_DOUBLE:
        dst.type = PORT_LIVE_PROP_DOUBLE;
        dst.double_val = src.value.dblData;
        return true;
    case XUSER_DATA_TYPE_FLOAT:
        dst.type = PORT_LIVE_PROP_FLOAT;
        dst.double_val = (double)src.value.fData;
        return true;
    default:
        return false;
    }
}

// Packs count results into pSearchResults, capacity bytes; *needed_size: the bytes they take.
DWORD pack_search_results(const port_live_search_result *items, DWORD count, DWORD capacity, DWORD *needed_size,
    PXSESSION_SEARCHRESULT_HEADER pSearchResults)
{
    DWORD needed = (DWORD)sizeof(XSESSION_SEARCHRESULT_HEADER) + count * (DWORD)sizeof(XSESSION_SEARCHRESULT);
    *needed_size = needed;
    if (pSearchResults == 0 || capacity < needed)
        return ERROR_INSUFFICIENT_BUFFER;
    pSearchResults->dwSearchResults = count;
    pSearchResults->pResults = count > 0 ? (XSESSION_SEARCHRESULT *)(pSearchResults + 1) : 0;
    for (DWORD i = 0; i < count; i++) {
        XSESSION_SEARCHRESULT &r = pSearchResults->pResults[i];
        memset(&r, 0, sizeof(r));
        memcpy(r.info.sessionID.ab, items[i].xnkid, sizeof(r.info.sessionID.ab));
        memcpy(&r.info.hostAddress, items[i].xnaddr, sizeof(r.info.hostAddress));
        memcpy(r.info.keyExchangeKey.ab, items[i].xnkey, sizeof(r.info.keyExchangeKey.ab));
        r.dwOpenPublicSlots = items[i].open_public;
        r.dwOpenPrivateSlots = items[i].open_private;
        r.dwFilledPublicSlots = items[i].filled_public;
        r.dwFilledPrivateSlots = items[i].filled_private;
    }
    return ERROR_SUCCESS;
}


// ---- the sessions' Live server calls, on the Live worker ----
//
// An Xbox keeps a session's state in XAM and talks to Xbox Live in the background; the title polls the XOVERLAPPED
// (async_xoverlapped.cpp starts its tasks from the main loop, holding its task lock). Here the session table changes
// at once, on the caller's thread, and what the Live server must be told or asked is a job on the Live worker
// (live.h port_live_post), in the order the calls were made, so the server sees them in that order as before:
//   - with an XOVERLAPPED the call returns ERROR_IO_PENDING and the job completes it once the server has answered,
//     from the worker (the completion routine is queued to the calling thread), with what the call completed with
//     before: the title still waits for the server where it waited, without holding its main loop for it;
//   - without one, a call whose result does not depend on the server (XSessionModify, XSessionJoinRemote and
//     XSessionLeaveRemote, which the title makes synchronously from its main loop) returns its result at once and the
//     server is told from the worker; one whose result is the server's answer (search, arbitration) waits for it.
// A call's inputs are copied into its job; its outputs are the caller's, which it keeps until the call completes.

// A job's XOVERLAPPED and how it completes.
struct s_live_call {
    PXOVERLAPPED overlapped; // 0: no XOVERLAPPED
    DWORD thread;            // the caller's: its completion routine runs there
    DWORD result;            // what the call completes with when the server's answer does not change it
    const char *name;        // HALO3_LIVE_LOG: the call, and when it was made
    unsigned long long traced;
};

s_live_call live_call(const char *name, PXOVERLAPPED overlapped, DWORD result)
{
    s_live_call call = { overlapped, GetCurrentThreadId(), result, name, port_live_trace_start() };
    return call;
}

// The job is done: the XOVERLAPPED, if any, completes with result.
void live_call_finish(const s_live_call &call, DWORD result, DWORD count = 0)
{
    port_live_trace_end(call.name, "completed", call.traced);
    if (call.overlapped != 0)
        complete_with(call.overlapped, result, count, result == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(result),
            call.thread);
}

// Queues job (a T whose first member is its s_live_call) on the Live worker and returns what the call returns:
// ERROR_IO_PENDING with an XOVERLAPPED (pending until the job completes it); without one, call.result at once, or,
// for a call that returns the server's answer (wait), that answer once the job has it.
template <typename T>
DWORD live_call_post(void (*run)(const void *), T &job, bool wait)
{
    static_assert(sizeof(T) <= PORT_LIVE_POST_MAX, "a Live job larger than port_live_post takes");
    XOVERLAPPED waited;
    if (job.call.overlapped == 0 && wait) {
        memset(&waited, 0, sizeof(waited));
        job.call.overlapped = &waited;
    }
    if (job.call.overlapped != 0) {
        job.call.overlapped->InternalHigh = 0;
        job.call.overlapped->dwExtendedError = 0;
        __atomic_store_n(&job.call.overlapped->InternalLow, (ULONG_PTR)ERROR_IO_PENDING, __ATOMIC_RELEASE);
    }
    const PXOVERLAPPED overlapped = job.call.overlapped;
    const DWORD result = job.call.result;
    if (port_live_on_worker())
        run(&job); // made from a Live job (none is today): at once, since the worker cannot wait for a later call
    else
        port_live_post(run, &job, sizeof(job));
    if (overlapped == &waited) {
        while (__atomic_load_n(&waited.InternalLow, __ATOMIC_ACQUIRE) == ERROR_IO_PENDING)
            Sleep(1);
        return (DWORD)waited.InternalLow;
    }
    return overlapped != 0 ? ERROR_IO_PENDING : result;
}

struct s_session_create_job {
    s_live_call call;
    unsigned char xnkid[8];
    DWORD flags, user_index, max_public, max_private;
    XNADDR host;
    XNKEY key;
};

void session_create_job(const void *data)
{
    const s_session_create_job &job = *(const s_session_create_job *)data;
    if (!port_live_signed_in())
        port_live_startup();
    port_live_session_create(job.xnkid, job.flags, job.user_index, port_live_xuid(), (const unsigned char *)&job.host,
        job.key.ab, job.max_public, job.max_private);
    live_call_finish(job.call, job.call.result);
}

// The calls on a session that tell the server one thing (delete, start, end, modify).
enum e_session_notify { _session_notify_delete, _session_notify_start, _session_notify_end, _session_notify_modify };

struct s_session_notify_job {
    s_live_call call;
    e_session_notify kind;
    unsigned char xnkid[8];
    DWORD flags, max_public, max_private;
};

void session_notify_job(const void *data)
{
    const s_session_notify_job &job = *(const s_session_notify_job *)data;
    switch (job.kind) {
    case _session_notify_delete:
        port_live_session_delete(job.xnkid);
        break;
    case _session_notify_start:
        port_live_session_start(job.xnkid);
        break;
    case _session_notify_end:
        port_live_session_end(job.xnkid);
        break;
    case _session_notify_modify:
        port_live_session_modify(job.xnkid, job.flags, job.max_public, job.max_private);
        break;
    }
    live_call_finish(job.call, job.call.result);
}

struct s_session_members_job {
    s_live_call call;
    bool join;
    unsigned char xnkid[8];
    DWORD count;
    unsigned long long xuids[k_session_member_capacity];
    int private_slots[k_session_member_capacity];
};

void session_members_job(const void *data)
{
    const s_session_members_job &job = *(const s_session_members_job *)data;
    if (job.join)
        port_live_session_join(job.xnkid, job.xuids, job.private_slots, job.count);
    else
        port_live_session_leave(job.xnkid, job.xuids, job.count);
    live_call_finish(job.call, job.call.result);
}

struct s_session_migrate_job {
    s_live_call call;
    unsigned char xnkid[8];
    DWORD user_index;
    XNADDR host;
    XNKEY key;
};

void session_migrate_job(const void *data)
{
    const s_session_migrate_job &job = *(const s_session_migrate_job *)data;
    port_live_session_migrate(job.xnkid, job.user_index, port_live_xuid(), (const unsigned char *)&job.host,
        job.key.ab);
    live_call_finish(job.call, job.call.result);
}

// The results of a search or arbitration: the caller's buffer and its size in bytes, as the call found it; the size
// the results take goes back to the caller's *pcbResultsBuffer only when the caller waits for them (no XOVERLAPPED):
// with one, that DWORD may be a local of a frame long gone when the job runs (online_arbitration_xenon.cpp's
// c_arbitration_overlapped_task::start passes its own), and nothing reads it once the XOVERLAPPED completes.
struct s_live_results {
    void *buffer;
    DWORD capacity;
    DWORD *size_out;
};

s_live_results live_results(void *buffer, DWORD *pcbResultsBuffer, PXOVERLAPPED overlapped)
{
    s_live_results results = { buffer, *pcbResultsBuffer, overlapped == 0 ? pcbResultsBuffer : 0 };
    return results;
}

void live_results_size(const s_live_results &results, DWORD needed)
{
    if (results.size_out != 0)
        *results.size_out = needed;
}

const DWORD k_arbitration_machine_capacity = 16;

struct s_arbitration_job {
    s_live_call call;
    unsigned char xnkid[8];
    ULONGLONG nonce;
    s_live_results results;
};

void arbitration_job(const void *data)
{
    const s_arbitration_job &job = *(const s_arbitration_job *)data;
    port_live_arb_machine machines[k_arbitration_machine_capacity];
    unsigned int mcount = 0, total_players = 0;
    if (port_live_session_arbitrate(job.xnkid, job.nonce, machines, k_arbitration_machine_capacity, &mcount,
            &total_players)
        != 0) {
        live_call_finish(job.call, ERROR_FUNCTION_FAILED);
        return;
    }
    DWORD needed = (DWORD)sizeof(XSESSION_REGISTRATION_RESULTS) + mcount * (DWORD)sizeof(XSESSION_REGISTRANT)
        + total_players * (DWORD)sizeof(XUID);
    live_results_size(job.results, needed);
    if (job.results.buffer == 0 || job.results.capacity < needed) {
        live_call_finish(job.call, ERROR_INSUFFICIENT_BUFFER);
        return;
    }
    PXSESSION_REGISTRATION_RESULTS results = (PXSESSION_REGISTRATION_RESULTS)job.results.buffer;
    results->wNumRegistrants = mcount;
    XSESSION_REGISTRANT *regs = (XSESSION_REGISTRANT *)(results + 1);
    XUID *xuids_dst = (XUID *)(regs + mcount);
    results->rgRegistrants = mcount > 0 ? regs : 0;
    for (unsigned int i = 0; i < mcount; i++) {
        regs[i].qwMachineID = machines[i].machine_id;
        regs[i].bTrustworthiness = 1;
        regs[i].bNumUsers = machines[i].player_count;
        regs[i].rgUsers = machines[i].player_count > 0 ? xuids_dst : 0;
        for (unsigned int j = 0; j < machines[i].player_count; j++)
            *xuids_dst++ = machines[i].xuids[j];
    }
    live_call_finish(job.call, ERROR_SUCCESS);
}

const unsigned int k_stats_view_capacity = 16;
const unsigned int k_stats_property_capacity = 128;

struct s_write_stats_job {
    s_live_call call;
    unsigned char xnkid[8];
    XUID xuid;
    unsigned int view_count;
    unsigned int view_ids[k_stats_view_capacity];
    unsigned int property_counts[k_stats_view_capacity];
    port_live_property properties[k_stats_property_capacity];
};

void write_stats_job(const void *data)
{
    const s_write_stats_job &job = *(const s_write_stats_job *)data;
    port_live_session_write_stats(job.xnkid, job.xuid, job.view_count, job.view_ids, job.property_counts,
        job.properties);
    live_call_finish(job.call, job.call.result);
}

struct s_read_stats_job {
    s_live_call call;
    DWORD xuid_count;
    XUID *xuids; // base of the owned allocation, followed by rows, columns and views
    port_live_stats_view *views;
    PXUSER_STATS_READ_RESULTS results;
};

void read_stats_job(const void *data)
{
    const s_read_stats_job &job = *(const s_read_stats_job *)data;
    const DWORD count = job.results->dwNumViews;
    XUSER_STATS_VIEW *views = job.results->pViews;
    BYTE *cursor = (BYTE *)(views + count);
    port_live_read_stats(job.xuids, job.xuid_count, job.views, count);
    for (DWORD v = 0; v < count; v++) {
        const port_live_stats_view &src = job.views[v];
        XUSER_STATS_VIEW &view = views[v];
        view.dwTotalViewRows = view.dwNumRows = src.row_count;
        view.pRows = src.row_count ? (XUSER_STATS_ROW *)cursor : 0;
        cursor += sizeof(XUSER_STATS_ROW) * src.row_count;
        for (DWORD r = 0; r < src.row_count; r++) {
            const port_live_stats_row &player = src.rows[r];
            XUSER_STATS_ROW &row = view.pRows[r];
            row.xuid = player.xuid;
            row.dwRank = player.rank;
            row.i64Rating = player.rating;
            static_assert(sizeof(row.szGamertag) == sizeof(player.gamertag), "Live gamertag size");
            memcpy(row.szGamertag, player.gamertag, sizeof(row.szGamertag));
            row.dwNumColumns = player.column_count;
            row.pColumns = player.column_count ? (XUSER_STATS_COLUMN *)cursor : 0;
            cursor += sizeof(XUSER_STATS_COLUMN) * player.column_count;
            for (DWORD c = 0; c < player.column_count; c++) {
                const port_live_property &stat = src.columns[(SIZE_T)r * src.column_count + c];
                XUSER_STATS_COLUMN &column = row.pColumns[c];
                column.wColumnId = (WORD)stat.id;
                column.Value.type = (BYTE)stat.type;
                switch (stat.type) {
                case XUSER_DATA_TYPE_INT32:  column.Value.nData = (LONG)stat.int_val; break;
                case XUSER_DATA_TYPE_INT64:  column.Value.i64Data = stat.int_val; break;
                case XUSER_DATA_TYPE_DOUBLE: column.Value.dblData = stat.double_val; break;
                case XUSER_DATA_TYPE_FLOAT:  column.Value.fData = (FLOAT)stat.double_val; break;
                }
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, job.xuids);
    live_call_finish(job.call, ERROR_SUCCESS);
}

const unsigned int k_search_result_capacity = 50;
const unsigned int k_search_term_capacity = 32;

struct s_search_job {
    s_live_call call;
    bool by_id;
    unsigned char xnkid[8]; // by id
    DWORD procedure, results_wanted, users;
    unsigned int property_count, context_count;
    port_live_property properties[k_search_term_capacity];
    port_live_context contexts[k_search_term_capacity];
    s_live_results results;
};

void search_job(const void *data)
{
    const s_search_job &job = *(const s_search_job *)data;
    port_live_search_result found[k_search_result_capacity];
    unsigned int count = 0;
    int ok;
    if (job.by_id) {
        int exists = 0;
        ok = port_live_session_search_by_id(job.xnkid, &found[0], &exists) == 0;
        count = exists ? 1u : 0u;
    } else {
        ok = port_live_session_search(job.procedure, job.results_wanted, job.users, port_live_xuid(), job.properties,
                 job.property_count, job.contexts, job.context_count, found, job.results_wanted, &count)
            == 0;
    }
    if (!ok) {
        live_call_finish(job.call, ERROR_FUNCTION_FAILED);
        return;
    }
    DWORD needed = 0;
    DWORD result = pack_search_results(found, count, job.results.capacity, &needed,
        (PXSESSION_SEARCHRESULT_HEADER)job.results.buffer);
    live_results_size(job.results, needed);
    live_call_finish(job.call, result);
}

} // namespace

extern "C" {

DWORD XSessionCreate(DWORD dwFlags, DWORD dwUserIndex, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots,
    ULONGLONG *pqwSessionNonce, PXSESSION_INFO pSessionInfo, PXOVERLAPPED pXOverlapped, HANDLE *ph)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionCreate (overlapped)" : "XSessionCreate");
    if (ph == 0 || pqwSessionNonce == 0 || pSessionInfo == 0)
        return ERROR_INVALID_PARAMETER;
    session_lock();
    s_session *session = 0;
    for (DWORD i = 0; i < k_session_capacity && session == 0; i++)
        if (g_sessions[i].handle == 0)
            session = &g_sessions[i];
    if (session == 0) {
        session_unlock();
        return complete(pXOverlapped, ERROR_NOT_ENOUGH_MEMORY);
    }
    memset(session, 0, sizeof(*session));
    session->handle = CreateEventA(0, TRUE, FALSE, 0);
    session->flags = dwFlags;
    session->user_index = dwUserIndex;
    session->max_public = dwMaxPublicSlots;
    session->max_private = dwMaxPrivateSlots;
    session->state = XSESSION_STATE_LOBBY;
    if (dwFlags & XSESSION_CREATE_HOST) {
        // a new session: its id (a system-link id, XNetCreateKey's; with Live an online peer one), key and nonce; the
        // host is this console (zeros with no network: XNetGetTitleXnAddr reports none)
        XNetCreateKey(&session->info.sessionID, &session->info.keyExchangeKey);
        if (port_live_enabled()) {
            session->info.sessionID.ab[0] =
                (BYTE)((session->info.sessionID.ab[0] & ~XNET_XNKID_MASK) | XNET_XNKID_ONLINE_PEER);
        }
        XNetGetTitleXnAddr(&session->info.hostAddress);
        random_bytes(&session->nonce, sizeof(session->nonce));
        *pqwSessionNonce = session->nonce;
        *pSessionInfo = session->info;
    } else {
        // joining a session someone else hosts: its description comes from the caller
        session->info = *pSessionInfo;
        session->nonce = *pqwSessionNonce;
    }
    XSESSION_INFO info_copy = session->info;
    *ph = session->handle;
    session_unlock();
    // the session that has the user's presence is what invites name (invite.h)
    port_invite_presence_session((unsigned long)*ph, (const unsigned char *)&info_copy, dwFlags,
        (dwFlags & XSESSION_CREATE_HOST) != 0);

    if (port_live_enabled())
        XNetRegisterKey(&info_copy.sessionID, &info_copy.keyExchangeKey);

    if (port_live_enabled() && (dwFlags & XSESSION_CREATE_HOST) != 0) {
        s_session_create_job job;
        memset(&job, 0, sizeof(job));
        job.call = live_call("XSessionCreate", pXOverlapped, ERROR_SUCCESS);
        memcpy(job.xnkid, info_copy.sessionID.ab, 8);
        job.flags = dwFlags;
        job.user_index = dwUserIndex;
        job.max_public = dwMaxPublicSlots;
        job.max_private = dwMaxPrivateSlots;
        job.host = info_copy.hostAddress;
        job.key = info_copy.keyExchangeKey;
        return live_call_post(session_create_job, job, false);
    }
    return complete(pXOverlapped, ERROR_SUCCESS);
}

DWORD XSessionDelete(HANDLE hSession, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionDelete (overlapped)" : "XSessionDelete");
    bool notify_live = false;
    unsigned char xnkid[8];
    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0) {
        if (port_live_enabled() && (session->flags & XSESSION_CREATE_HOST) != 0) {
            notify_live = true;
            memcpy(xnkid, session->info.sessionID.ab, 8);
        }
        session->state = XSESSION_STATE_DELETED;
        session->handle = 0; // the caller closes the handle
    }
    session_unlock();
    port_invite_presence_session_deleted((unsigned long)hSession);
    if (notify_live) {
        s_session_notify_job job;
        memset(&job, 0, sizeof(job));
        job.call = live_call("XSessionDelete", pXOverlapped, ERROR_SUCCESS);
        job.kind = _session_notify_delete;
        memcpy(job.xnkid, xnkid, 8);
        return live_call_post(session_notify_job, job, false);
    }
    return complete(pXOverlapped, session != 0 ? ERROR_SUCCESS : ERROR_INVALID_HANDLE);
}

static DWORD session_set_state(HANDLE hSession, XSESSION_STATE state, PXOVERLAPPED pXOverlapped)
{
    static const char *const k_trace[2][2] = { { "XSessionEnd", "XSessionEnd (overlapped)" },
        { "XSessionStart", "XSessionStart (overlapped)" } };
    const bool start = state == XSESSION_STATE_INGAME;
    s_live_trace trace(k_trace[start][pXOverlapped != 0]);
    bool notify_live = false;
    unsigned char xnkid[8];
    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0) {
        session->state = state;
        if (port_live_enabled() && (session->flags & XSESSION_CREATE_HOST) != 0) {
            notify_live = true;
            memcpy(xnkid, session->info.sessionID.ab, 8);
        }
    }
    session_unlock();
    if (notify_live) {
        s_session_notify_job job;
        memset(&job, 0, sizeof(job));
        job.call = live_call(k_trace[start][0], pXOverlapped, ERROR_SUCCESS);
        job.kind = start ? _session_notify_start : _session_notify_end;
        memcpy(job.xnkid, xnkid, 8);
        return live_call_post(session_notify_job, job, false);
    }
    return complete(pXOverlapped, session != 0 ? ERROR_SUCCESS : ERROR_INVALID_HANDLE);
}

DWORD XSessionStart(HANDLE hSession, DWORD dwFlags, PXOVERLAPPED pXOverlapped)
{
    (void)dwFlags;
    return session_set_state(hSession, XSESSION_STATE_INGAME, pXOverlapped);
}

DWORD XSessionEnd(HANDLE hSession, PXOVERLAPPED pXOverlapped)
{
    return session_set_state(hSession, XSESSION_STATE_LOBBY, pXOverlapped);
}

DWORD XSessionModify(HANDLE hSession, DWORD dwFlags, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots,
    PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionModify (overlapped)" : "XSessionModify");
    bool notify_live = false;
    unsigned char xnkid[8];
    DWORD final_flags = 0;
    XSESSION_INFO info_copy = {};
    DWORD presence_flags = 0;
    bool presence_host = false;
    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0) {
        // the modifiers (invites, join via presence, join in progress) may change; how the session is used may not
        session->flags = (session->flags & ~XSESSION_CREATE_MODIFIERS_MASK) | (dwFlags & XSESSION_CREATE_MODIFIERS_MASK);
        session->max_public = dwMaxPublicSlots;
        session->max_private = dwMaxPrivateSlots;
        info_copy = session->info;
        presence_flags = session->flags;
        presence_host = (session->flags & XSESSION_CREATE_HOST) != 0;
        if (port_live_enabled() && (session->flags & XSESSION_CREATE_HOST) != 0) {
            notify_live = true;
            memcpy(xnkid, session->info.sessionID.ab, 8);
            final_flags = session->flags;
        }
    }
    session_unlock();
    if (session != 0) // invites allowed or not (XSESSION_CREATE_INVITES_DISABLED)
        port_invite_presence_session((unsigned long)hSession, (const unsigned char *)&info_copy, presence_flags,
            presence_host);
    if (notify_live) {
        s_session_notify_job job;
        memset(&job, 0, sizeof(job));
        job.call = live_call("XSessionModify", pXOverlapped, ERROR_SUCCESS);
        job.kind = _session_notify_modify;
        memcpy(job.xnkid, xnkid, 8);
        job.flags = final_flags;
        job.max_public = dwMaxPublicSlots;
        job.max_private = dwMaxPrivateSlots;
        return live_call_post(session_notify_job, job, false);
    }
    return complete(pXOverlapped, session != 0 ? ERROR_SUCCESS : ERROR_INVALID_HANDLE);
}

DWORD XSessionJoinRemote(HANDLE hSession, DWORD dwXuidCount, const XUID *pXuids, const BOOL *pfPrivateSlots,
    PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionJoinRemote (overlapped)" : "XSessionJoinRemote");
    s_session_members_job job;
    memset(&job, 0, sizeof(job));
    job.join = true;

    session_lock();
    s_session *session = session_of(hSession);
    DWORD result = session != 0 ? ERROR_SUCCESS : ERROR_INVALID_HANDLE;
    for (DWORD i = 0; session != 0 && i < dwXuidCount && result == ERROR_SUCCESS; i++) {
        BOOL private_slot = pfPrivateSlots != 0 && pfPrivateSlots[i];
        bool already = false;
        for (DWORD m = 0; m < session->member_count; m++) {
            if (session->members[m].xuidOnline == pXuids[i]) {
                already = true;
                break;
            }
        }
        if (!already) {
            if (session->member_count == k_session_member_capacity
                || members_in(session, private_slot) >= (private_slot ? session->max_private : session->max_public)) {
                result = XONLINE_E_SESSION_FULL;
                break;
            }
            XSESSION_MEMBER &member = session->members[session->member_count++];
            member.xuidOnline = pXuids[i];
            member.dwUserIndex = XUSER_INDEX_NONE;
            member.dwFlags = private_slot ? XSESSION_MEMBER_FLAGS_PRIVATE_SLOT : 0;
        }
        if (job.count < k_session_member_capacity) {
            job.xuids[job.count] = pXuids[i];
            job.private_slots[job.count] = private_slot ? 1 : 0;
            job.count++;
        }
    }
    const bool notify_live = session != 0 && result == ERROR_SUCCESS && port_live_enabled() && job.count > 0;
    if (notify_live)
        memcpy(job.xnkid, session->info.sessionID.ab, 8);
    session_unlock();

    if (notify_live) {
        job.call = live_call("XSessionJoinRemote", pXOverlapped, result);
        return live_call_post(session_members_job, job, false);
    }
    return complete(pXOverlapped, result);
}

DWORD XSessionLeaveRemote(HANDLE hSession, DWORD dwXuidCount, const XUID *pXuids, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionLeaveRemote (overlapped)" : "XSessionLeaveRemote");
    s_session_members_job job;
    memset(&job, 0, sizeof(job));
    job.join = false;

    session_lock();
    s_session *session = session_of(hSession);
    for (DWORD i = 0; session != 0 && i < dwXuidCount; i++) {
        if (job.count < k_session_member_capacity)
            job.xuids[job.count++] = pXuids[i];
        for (DWORD m = 0; m < session->member_count; m++) {
            if (session->members[m].xuidOnline == pXuids[i]) {
                session->members[m] = session->members[--session->member_count];
                break;
            }
        }
    }
    const bool notify_live = session != 0 && port_live_enabled() && job.count > 0;
    if (notify_live)
        memcpy(job.xnkid, session->info.sessionID.ab, 8);
    session_unlock();

    if (notify_live) {
        job.call = live_call("XSessionLeaveRemote", pXOverlapped, ERROR_SUCCESS);
        return live_call_post(session_members_job, job, false);
    }
    return complete(pXOverlapped, session != 0 ? ERROR_SUCCESS : ERROR_INVALID_HANDLE);
}

DWORD XSessionMigrateHost(HANDLE hSession, DWORD dwUserIndex, XSESSION_INFO *pSessionInfo, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionMigrateHost (overlapped)" : "XSessionMigrateHost");
    bool notify_live = false;
    XSESSION_INFO info_copy;
    memset(&info_copy, 0, sizeof(info_copy));
    XSESSION_INFO presence = {};
    DWORD presence_flags = 0;

    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0 && pSessionInfo != 0) {
        if (dwUserIndex != XUSER_INDEX_NONE) {
            // this console becomes the host: a new session id and key
            session->user_index = dwUserIndex;
            session->flags |= XSESSION_CREATE_HOST;
            XNetCreateKey(&session->info.sessionID, &session->info.keyExchangeKey);
            if (port_live_enabled()) {
                session->info.sessionID.ab[0] =
                    (BYTE)((session->info.sessionID.ab[0] & ~XNET_XNKID_MASK) | XNET_XNKID_ONLINE_PEER);
            }
            XNetGetTitleXnAddr(&session->info.hostAddress);
            *pSessionInfo = session->info;
            if (port_live_enabled()) {
                notify_live = true;
                info_copy = session->info;
            }
        } else {
            session->flags &= ~XSESSION_CREATE_HOST;
            session->info = *pSessionInfo;
        }
        presence = session->info;
        presence_flags = session->flags;
    }
    session_unlock();
    if (session != 0 && pSessionInfo != 0) // the new host's session is what invites name now
        port_invite_presence_session((unsigned long)hSession, (const unsigned char *)&presence, presence_flags,
            (presence_flags & XSESSION_CREATE_HOST) != 0);

    if (notify_live) {
        s_session_migrate_job job;
        memset(&job, 0, sizeof(job));
        job.call = live_call("XSessionMigrateHost", pXOverlapped, ERROR_SUCCESS);
        memcpy(job.xnkid, info_copy.sessionID.ab, 8);
        job.user_index = dwUserIndex;
        job.host = info_copy.hostAddress;
        job.key = info_copy.keyExchangeKey;
        return live_call_post(session_migrate_job, job, false);
    }
    return complete(pXOverlapped, session != 0 && pSessionInfo != 0 ? ERROR_SUCCESS : ERROR_INVALID_PARAMETER);
}

DWORD XSessionGetDetails(HANDLE hSession, DWORD *pcbResultsBuffer, XSESSION_LOCAL_DETAILS *pSessionDetails,
    PXOVERLAPPED pXOverlapped)
{
    if (pcbResultsBuffer == 0)
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    session_lock();
    s_session *session = session_of(hSession);
    if (session == 0) {
        session_unlock();
        return complete(pXOverlapped, ERROR_INVALID_HANDLE);
    }
    DWORD needed = sizeof(XSESSION_LOCAL_DETAILS) + session->member_count * sizeof(XSESSION_MEMBER);
    if (pSessionDetails == 0 || *pcbResultsBuffer < needed) {
        *pcbResultsBuffer = needed;
        session_unlock();
        return complete(pXOverlapped, ERROR_INSUFFICIENT_BUFFER);
    }
    XSESSION_LOCAL_DETAILS &details = *pSessionDetails;
    memset(&details, 0, sizeof(details));
    details.dwUserIndexHost = (session->flags & XSESSION_CREATE_HOST) ? session->user_index : XUSER_INDEX_NONE;
    details.dwFlags = session->flags;
    details.dwMaxPublicSlots = session->max_public;
    details.dwMaxPrivateSlots = session->max_private;
    DWORD used_public = members_in(session, FALSE), used_private = members_in(session, TRUE);
    details.dwAvailablePublicSlots = session->max_public > used_public ? session->max_public - used_public : 0;
    details.dwAvailablePrivateSlots = session->max_private > used_private ? session->max_private - used_private : 0;
    details.dwActualMemberCount = session->member_count;
    details.dwReturnedMemberCount = session->member_count;
    details.eState = session->state;
    details.qwNonce = session->nonce;
    details.sessionInfo = session->info;
    details.pSessionMembers = (XSESSION_MEMBER *)(&details + 1);
    memcpy(details.pSessionMembers, session->members, session->member_count * sizeof(XSESSION_MEMBER));
    session_unlock();
    return complete(pXOverlapped, ERROR_SUCCESS);
}

DWORD XSessionArbitrationRegister(HANDLE hSession, DWORD dwFlags, ULONGLONG qwSessionNonce, DWORD *pcbResultsBuffer,
    PXSESSION_REGISTRATION_RESULTS pRegistrationResults, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionArbitrationRegister (overlapped)" : "XSessionArbitrationRegister");
    (void)dwFlags;
    if (!port_live_enabled())
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    if (pcbResultsBuffer == 0)
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);

    s_arbitration_job job;
    memset(&job, 0, sizeof(job));
    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0)
        memcpy(job.xnkid, session->info.sessionID.ab, 8);
    session_unlock();
    if (session == 0)
        return complete(pXOverlapped, ERROR_INVALID_HANDLE);

    job.call = live_call("XSessionArbitrationRegister", pXOverlapped, ERROR_SUCCESS);
    job.nonce = qwSessionNonce;
    job.results = live_results(pRegistrationResults, pcbResultsBuffer, pXOverlapped);
    return live_call_post(arbitration_job, job, true);
}

DWORD XUserReadStats(DWORD dwTitleId, DWORD dwNumXuids, const XUID *pXuids, DWORD dwNumStatsSpecs,
    const XUSER_STATS_SPEC *pSpecs, PDWORD pcbResults, PXUSER_STATS_READ_RESULTS pResults, PXOVERLAPPED pOverlapped)
{
    if (!pcbResults || (dwNumXuids && !pXuids) || (dwNumStatsSpecs && !pSpecs))
        return ERROR_INVALID_PARAMETER;

    // Align the first array even for a byte buffer embedded in a game object. All subsequent array
    // elements have an eight-byte stride on both game ABIs.
    static_assert((sizeof(XUSER_STATS_VIEW) | sizeof(XUSER_STATS_ROW) | sizeof(XUSER_STATS_COLUMN)) % 8 == 0,
        "stats array alignment");
    DWORD header = sizeof(XUSER_STATS_READ_RESULTS);
    if (dwNumStatsSpecs)
        header += pResults ? (DWORD)(-(ULONG_PTR)((BYTE *)pResults + header) & 7) : 7;
    const ULONGLONG rows = (ULONGLONG)dwNumStatsSpecs * dwNumXuids;
    if (rows > 0xffffffffu / sizeof(XUSER_STATS_ROW))
        return ERROR_INVALID_PARAMETER;
    ULONGLONG needed = header + (ULONGLONG)dwNumStatsSpecs * sizeof(XUSER_STATS_VIEW) + rows * sizeof(XUSER_STATS_ROW);
    ULONGLONG columns = 0;
    for (DWORD v = 0; v < dwNumStatsSpecs; v++) {
        if (needed > 0xffffffffu || pSpecs[v].dwNumColumnIds > XUSER_STATS_ATTRS_IN_SPEC)
            return ERROR_INVALID_PARAMETER;
        const ULONGLONG n = (ULONGLONG)pSpecs[v].dwNumColumnIds * dwNumXuids;
        columns += n;
        needed += n * sizeof(XUSER_STATS_COLUMN);
    }
    if (needed > 0xffffffffu)
        return ERROR_INVALID_PARAMETER;
    if (!pResults || *pcbResults < needed) {
        *pcbResults = (DWORD)needed;
        return ERROR_INSUFFICIENT_BUFFER;
    }
    if (!port_live_enabled())
        return complete(pOverlapped, ERROR_NOT_LOGGED_ON);

    *pcbResults = (DWORD)needed;
    memset(pResults, 0, (SIZE_T)needed);
    pResults->dwNumViews = dwNumStatsSpecs;
    pResults->pViews = dwNumStatsSpecs ? (XUSER_STATS_VIEW *)((BYTE *)pResults + header) : 0;
    for (DWORD v = 0; v < dwNumStatsSpecs; v++)
        pResults->pViews[v].dwViewId = pSpecs[v].dwViewId;
    if (!dwNumXuids || !dwNumStatsSpecs || (dwTitleId && dwTitleId != k_title_id))
        return complete(pOverlapped, ERROR_SUCCESS);

    // Snapshot all query inputs before posting. Only the XDK result buffer and overlapped belong
    // to the caller after return. The HTTP decoder uses its own bounded row and column arrays.
    const ULONGLONG bytes = (ULONGLONG)dwNumXuids * sizeof(XUID) + rows * sizeof(port_live_stats_row) +
        columns * sizeof(port_live_property) + (ULONGLONG)dwNumStatsSpecs * sizeof(port_live_stats_view);
    if (bytes > (SIZE_T)-1)
        return ERROR_OUTOFMEMORY;
    s_read_stats_job job;
    job.xuids = (XUID *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)bytes);
    if (!job.xuids)
        return ERROR_OUTOFMEMORY;
    memcpy(job.xuids, pXuids, dwNumXuids * sizeof(XUID));
    port_live_stats_row *row = (port_live_stats_row *)(job.xuids + dwNumXuids);
    port_live_property *column = (port_live_property *)(row + (SIZE_T)rows);
    job.views = (port_live_stats_view *)(column + (SIZE_T)columns);
    for (DWORD v = 0; v < dwNumStatsSpecs; v++) {
        port_live_stats_view &view = job.views[v];
        view.view_id = pSpecs[v].dwViewId;
        view.column_count = pSpecs[v].dwNumColumnIds;
        static_assert(sizeof(view.column_ids) == sizeof(pSpecs[v].rgwColumnIds), "Live stats spec size");
        memcpy(view.column_ids, pSpecs[v].rgwColumnIds, view.column_count * sizeof(WORD));
        view.rows = row;
        view.columns = column;
        row += dwNumXuids;
        column += (SIZE_T)dwNumXuids * view.column_count;
    }
    job.call = live_call("XUserReadStats", pOverlapped, ERROR_SUCCESS);
    job.xuid_count = dwNumXuids;
    job.results = pResults;
    return live_call_post(read_stats_job, job, true);
}

DWORD XSessionWriteStats(HANDLE hSession, XUID xuid, DWORD dwNumViews, const XSESSION_VIEW_PROPERTIES *pViews,
    XOVERLAPPED *pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionWriteStats (overlapped)" : "XSessionWriteStats");
    if (!port_live_enabled())
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);

    s_write_stats_job job;
    memset(&job, 0, sizeof(job));
    session_lock();
    s_session *session = session_of(hSession);
    if (session != 0)
        memcpy(job.xnkid, session->info.sessionID.ab, 8);
    session_unlock();
    if (session == 0)
        return complete(pXOverlapped, ERROR_INVALID_HANDLE);

    job.xuid = xuid;
    unsigned int ptotal = 0;
    for (DWORD v = 0; v < dwNumViews && job.view_count < k_stats_view_capacity && pViews != 0; v++) {
        job.view_ids[job.view_count] = pViews[v].dwViewId;
        unsigned int count = 0;
        for (DWORD p = 0; p < pViews[v].dwNumProperties && ptotal < k_stats_property_capacity && pViews[v].pProperties != 0;
             p++) {
            if (convert_user_property(pViews[v].pProperties[p], job.properties[ptotal])) {
                ptotal++;
                count++;
            }
        }
        job.property_counts[job.view_count++] = count;
    }
    job.call = live_call("XSessionWriteStats", pXOverlapped, ERROR_SUCCESS);
    return live_call_post(write_stats_job, job, false);
}

DWORD XSessionSearchByID(XNKID sessionID, DWORD dwUserIndex, DWORD *pcbResultsBuffer,
    PXSESSION_SEARCHRESULT_HEADER pSearchResults, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionSearchByID (overlapped)" : "XSessionSearchByID");
    (void)dwUserIndex;
    if (!port_live_enabled())
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    if (pcbResultsBuffer == 0)
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);
    s_search_job job;
    memset(&job, 0, sizeof(job));
    job.call = live_call("XSessionSearchByID", pXOverlapped, ERROR_SUCCESS);
    job.by_id = true;
    memcpy(job.xnkid, sessionID.ab, 8);
    job.results = live_results(pSearchResults, pcbResultsBuffer, pXOverlapped);
    return live_call_post(search_job, job, true);
}

DWORD XSessionSearchEx(DWORD dwProcedureIndex, DWORD dwUserIndex, DWORD dwNumResults, DWORD dwNumUsers,
    WORD wNumProperties, WORD wNumContexts, PXUSER_PROPERTY pSearchProperties, PXUSER_CONTEXT pSearchContexts,
    DWORD *pcbResultsBuffer, PXSESSION_SEARCHRESULT_HEADER pSearchResults, PXOVERLAPPED pXOverlapped)
{
    s_live_trace trace(pXOverlapped != 0 ? "XSessionSearchEx (overlapped)" : "XSessionSearchEx");
    (void)dwUserIndex;
    if (!port_live_enabled())
        return complete(pXOverlapped, ERROR_NOT_LOGGED_ON);
    if (pcbResultsBuffer == 0)
        return complete(pXOverlapped, ERROR_INVALID_PARAMETER);

    s_search_job job;
    memset(&job, 0, sizeof(job));
    for (WORD i = 0; i < wNumProperties && job.property_count < k_search_term_capacity && pSearchProperties != 0; i++) {
        if (convert_user_property(pSearchProperties[i], job.properties[job.property_count]))
            job.property_count++;
    }
    for (WORD i = 0; i < wNumContexts && job.context_count < k_search_term_capacity && pSearchContexts != 0; i++) {
        job.contexts[job.context_count].id = pSearchContexts[i].dwContextId;
        job.contexts[job.context_count].value = pSearchContexts[i].dwValue;
        job.context_count++;
    }
    job.call = live_call("XSessionSearchEx", pXOverlapped, ERROR_SUCCESS);
    job.procedure = dwProcedureIndex;
    job.results_wanted = dwNumResults > k_search_result_capacity || dwNumResults == 0 ? k_search_result_capacity
                                                                                    : (unsigned int)dwNumResults;
    job.users = dwNumUsers == 0 ? 1u : (unsigned int)dwNumUsers;
    job.results = live_results(pSearchResults, pcbResultsBuffer, pXOverlapped);
    return live_call_post(search_job, job, true);
}

} // extern "C"
