// port/sdk/xam (both hosts): the XAM services (xam.xex, the Xbox's system application) a title calls. A PC has no
// dashboard to launch the title with data and no system notification UI; local (offline) profiles are signed in on
// users 0..3 (profiles.c, xam_profile.h), with optional Xbox Live accounts bound to the active profiles. The guide
// (guide.c) is the console's system UI: the Guide button opens it, the sign-in UI
// and other XShow* calls show its pages. Controllers are the window's keyboard and mouse and the host's gamepads
// (port/sdk/input/port_input.h).
#include "../input/port_input.h"
#include "../online/live.h"
#include "../runtime/port_window.h"
#include "guide.h"
#include "xam_profile.h"
#include "xam_ui.h"

typedef unsigned int port_dword;
typedef unsigned long long port_qword;

#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_NOT_FOUND 1168ul
#define PORT_ERROR_NOT_LOGGED_ON 1245ul
#define PORT_ERROR_NO_SUCH_USER 1317ul
#define PORT_ERROR_BAD_ARGUMENTS 160ul
#define PORT_E_NOT_LOGGED_ON ((long)0x800704dd)
#define PORT_E_INVALIDARG ((long)0x80070057)

// Launch data: what the title (or the dashboard) that launched this one passed it. The game was started directly.
PORT_MS_ABI port_dword XGetLaunchDataSize(port_dword *size)
{
    if (size != 0)
        *size = 0;
    return PORT_ERROR_NOT_FOUND;
}

// System notification UI: none to hold back, reposition, or drain.
PORT_MS_ABI port_dword XNotifyDelayUI(port_dword milliseconds)
{
    (void)milliseconds;
    return PORT_ERROR_SUCCESS;
}

PORT_MS_ABI void XNotifyPositionUI(port_dword position)
{
    (void)position;
}

// ---- system notifications ----
//
// A listener receives the notifications of the areas it asked for (XNOTIFY_SYSTEM, bit 0, and so on: the area is
// bits 25.. of the id). As a console does when a title starts, the first listener for system notifications is told
// which users are signed in (XN_SYS_SIGNINCHANGED, a bit per user) and that the input devices changed. After that the
// changes are the guide's, one dialog at a time (xam_ui.h):
//   - the guide operated by a player (guide.c, interactive): XN_SYS_UI TRUE when it opens, XN_SYS_SIGNINCHANGED as
//     players sign in and out, XN_SYS_UI FALSE when it closes, from whichever thread acted (the game's for controller
//     presses, the guide window's for its keys);
//   - a guide request completed at once (nobody operates the guide): XN_SYS_UI TRUE, any sign-in change, and, once the
//     listener has drained its queue in a later XNotifyGetNext loop (the guide stays open for a frame, as the game's UI
//     expects to see it open), XN_SYS_UI FALSE;
//   - the virtual keyboard (XShowKeyboardUI, port/sdk/online/keyboard_ui.cpp): XN_SYS_UI TRUE when it opens and FALSE
//     when it closes, which its dialog does on a host thread of its own.
// And with any of them, a game invite accepted in the guide (a Live invite or a lobby code, port/sdk/online/invite.h):
// XN_LIVE_INVITE_ACCEPTED to the listeners for Live notifications, after the guide's XN_SYS_UI FALSE.
// XN_SYS_SIGNINCHANGED's parameter is dwValid, the users signed in after the change (the XDK's "Bit specifying which
// players are valid"), which a title re-reads with XUserGetSigninState. So the listeners are shared under a lock.

#define PORT_XN_SYS_UI 0x00000009ul
#define PORT_XN_SYS_SIGNINCHANGED 0x0000000aul
#define PORT_XN_SYS_INPUTDEVICESCHANGED 0x00000012ul
#define PORT_XN_LIVE_CONNECTIONCHANGED 0x02000001ul
#define PORT_XN_LIVE_INVITE_ACCEPTED 0x02000002ul // XNID(0, _XNAREA_LIVE, 0x0002) (the XDK's xam.h)
#define PORT_XONLINE_S_LOGON_CONNECTION_ESTABLISHED 0x001510f0ul
#define PORT_XONLINE_S_LOGON_DISCONNECTED 0x001510f1ul
#define PORT_XNOTIFY_SYSTEM 0x00000001ull
#define PORT_XNOTIFY_LIVE 0x00000002ull
#define PORT_LISTENER_CAPACITY 8
#define PORT_LISTENER_QUEUE 16

typedef struct {
    port_qword areas;
    unsigned head, count;
    port_dword ids[PORT_LISTENER_QUEUE];
    unsigned long params[PORT_LISTENER_QUEUE];
    int pulse; // a request completed at once: 0 closed, 1 opened (queued), 2 open (drained once; closes on the next)
} port_listener;

// The guide dialog that shows (besides pulses).
enum { k_ui_none, k_ui_keyboard, k_ui_guide };

static port_listener g_listeners[PORT_LISTENER_CAPACITY];
static unsigned g_listener_count;
static int g_startup_notified;
static int g_live_notified; // a listener was told of the Xbox Live connection (live.h)
static int g_ui;            // k_ui_*
static volatile char g_listener_busy;
// Each controller's controls held at its last poll (held_controls: its buttons, the Guide button too, its triggers and
// sticks), and those a dialog that closed left held: hidden from the title, each until it is let go (xam_ui.h).
static unsigned long g_held[PORT_INPUT_USER_COUNT];
static unsigned long g_suppressed[PORT_INPUT_USER_COUNT];
#define PORT_HELD_LEFT_TRIGGER (1ul << 16)
#define PORT_HELD_RIGHT_TRIGGER (1ul << 17)
#define PORT_HELD_LEFT_STICK (1ul << 18)
#define PORT_HELD_RIGHT_STICK (1ul << 19)

static void listener_lock(void)
{
    while (__atomic_test_and_set(&g_listener_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void listener_unlock(void) { __atomic_clear(&g_listener_busy, __ATOMIC_RELEASE); }

static void listener_post(port_listener *listener, port_dword id, unsigned long param)
{
    if ((listener->areas & (1ull << ((id >> 25) & 0x3f))) == 0 || listener->count == PORT_LISTENER_QUEUE)
        return;
    const unsigned slot = (listener->head + listener->count) % PORT_LISTENER_QUEUE;
    listener->ids[slot] = id;
    listener->params[slot] = param;
    listener->count++;
}

static void listeners_post(port_dword id, unsigned long param)
{
    for (unsigned i = 0; i < g_listener_count; i++)
        listener_post(&g_listeners[i], id, param);
}

// Whether a guide dialog shows (a pulse until every listener has seen it close, the guide, the virtual keyboard).
// Under the lock.
static int ui_showing(void)
{
    if (g_ui != k_ui_none)
        return 1;
    for (unsigned i = 0; i < g_listener_count; i++)
        if (g_listeners[i].pulse != 0)
            return 1;
    return 0;
}

// A listener handle is the address of its record (never 0 or INVALID_HANDLE_VALUE). With Xbox Live (live.h), the
// first listener for Live notifications is told the connection is up (XN_LIVE_CONNECTIONCHANGED), signing in first if
// need be: that is HTTP to the Live server, so it is done before the listener lock is taken (the virtual keyboard's
// and the guide's threads take the lock too).
PORT_MS_ABI void *XNotifyCreateListener(port_qword areas)
{
    int live_connected = 0;
    if ((areas & PORT_XNOTIFY_LIVE) != 0 && !__atomic_load_n(&g_live_notified, __ATOMIC_ACQUIRE)
        && port_live_enabled()) {
        if (!port_live_signed_in())
            port_live_startup();
        live_connected = port_live_signed_in();
    }
    const unsigned long signed_in = port_profile_signed_in_mask(); // before the lock (profiles.c reads files once)
    listener_lock();
    port_listener *listener = 0;
    if (g_listener_count < PORT_LISTENER_CAPACITY) {
        listener = &g_listeners[g_listener_count];
        __builtin_memset(listener, 0, sizeof(*listener));
        listener->areas = areas;
        if (!g_startup_notified && (areas & PORT_XNOTIFY_SYSTEM) != 0) {
            g_startup_notified = 1;
            listener_post(listener, PORT_XN_SYS_SIGNINCHANGED, signed_in);
            listener_post(listener, PORT_XN_SYS_INPUTDEVICESCHANGED, 0);
        }
        if (live_connected && !g_live_notified) {
            __atomic_store_n(&g_live_notified, 1, __ATOMIC_RELEASE);
            listener_post(listener, PORT_XN_LIVE_CONNECTIONCHANGED, PORT_XONLINE_S_LOGON_CONNECTION_ESTABLISHED);
        }
        g_listener_count++;
    }
    listener_unlock();
    return listener;
}

// BOOL XNotifyGetNext(HANDLE, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam): the oldest queued notification
// (with a filter, the oldest with that id; the others stay queued).
PORT_MS_ABI int XNotifyGetNext(void *handle, port_dword filter, port_dword *id, unsigned long *param)
{
    if (param != 0)
        *param = 0;
    port_listener *listener = (port_listener *)handle;
    if (id == 0)
        return 0;
    int found = 0;
    listener_lock();
    if (listener >= g_listeners && listener < g_listeners + g_listener_count) {
        int drained = 0;
        if (listener->count == 0 && listener->pulse != 0) {
            if (listener->pulse == 1) {
                listener->pulse = 2; // this loop saw the guide open; it closes in the next one
                drained = 1;
            } else {
                listener->pulse = 0;
                listener_post(listener, PORT_XN_SYS_UI, 0);
            }
        }
        for (unsigned i = 0; i < listener->count && !drained; i++) {
            const unsigned slot = (listener->head + i) % PORT_LISTENER_QUEUE;
            if (filter != 0 && listener->ids[slot] != filter)
                continue;
            *id = listener->ids[slot];
            if (param != 0)
                *param = listener->params[slot];
            // remove entry i: shift the later ones down
            for (unsigned k = i; k + 1 < listener->count; k++) {
                const unsigned to = (listener->head + k) % PORT_LISTENER_QUEUE, from = (to + 1) % PORT_LISTENER_QUEUE;
                listener->ids[to] = listener->ids[from];
                listener->params[to] = listener->params[from];
            }
            listener->count--;
            found = 1;
            break;
        }
    }
    listener_unlock();
    return found;
}

// Opens dialog `ui`: 0, posting nothing, when one shows already.
static int ui_open(int ui)
{
    listener_lock();
    const int opened = !ui_showing();
    if (opened) {
        __atomic_store_n(&g_ui, ui, __ATOMIC_RELEASE);
        listeners_post(PORT_XN_SYS_UI, 1);
    }
    listener_unlock();
    return opened;
}

static void ui_close(int ui)
{
    listener_lock();
    const int closed = g_ui == ui;
    if (closed) {
        __atomic_store_n(&g_ui, k_ui_none, __ATOMIC_RELEASE);
        listeners_post(PORT_XN_SYS_UI, 0);
    }
    listener_unlock();
    if (closed) // what is held now (the A that chose, the B, the Guide button, a stick that scrolled) is not the title's
        for (unsigned user = 0; user < PORT_INPUT_USER_COUNT; user++)
            __atomic_store_n(&g_suppressed[user], __atomic_load_n(&g_held[user], __ATOMIC_ACQUIRE), __ATOMIC_RELEASE);
}

PORT_MS_ABI int port_xam_ui_open(void) { return ui_open(k_ui_keyboard); }
PORT_MS_ABI void port_xam_ui_close(void) { ui_close(k_ui_keyboard); }
PORT_MS_ABI int port_xam_ui_showing(void) { return __atomic_load_n(&g_ui, __ATOMIC_ACQUIRE) == k_ui_keyboard; }

int port_xam_guide_begin(void) { return ui_open(k_ui_guide); }
void port_xam_guide_end(void) { ui_close(k_ui_guide); }

int port_xam_guide_pulse(void)
{
    listener_lock();
    const int opened = !ui_showing();
    for (unsigned i = 0; i < g_listener_count && opened; i++) {
        port_listener *listener = &g_listeners[i];
        if ((listener->areas & PORT_XNOTIFY_SYSTEM) == 0)
            continue;
        listener_post(listener, PORT_XN_SYS_UI, 1);
        listener->pulse = 1;
    }
    listener_unlock();
    return opened;
}

void port_xam_signin_changed(unsigned long changed)
{
    if (changed == 0)
        return;
    const unsigned long signed_in = port_profile_signed_in_mask();
    const int live_connected = port_live_signed_in();
    listener_lock();
    if (live_connected && !g_live_notified) {
        __atomic_store_n(&g_live_notified, 1, __ATOMIC_RELEASE);
        listeners_post(PORT_XN_LIVE_CONNECTIONCHANGED, PORT_XONLINE_S_LOGON_CONNECTION_ESTABLISHED);
    } else if (!live_connected && g_live_notified) {
        __atomic_store_n(&g_live_notified, 0, __ATOMIC_RELEASE);
        listeners_post(PORT_XN_LIVE_CONNECTIONCHANGED, PORT_XONLINE_S_LOGON_DISCONNECTED);
    }
    listeners_post(PORT_XN_SYS_SIGNINCHANGED, signed_in);
    listener_unlock();
}

// A game invite accepted (port/sdk/online/invite.c): every listener for Live notifications (XNOTIFY_LIVE) is told
// XN_LIVE_INVITE_ACCEPTED with the accepting user's index, the XDK's "index (0-3) of the player accepting the
// invitation".
void port_xam_invite_accepted(unsigned long user_index)
{
    listener_lock();
    listeners_post(PORT_XN_LIVE_INVITE_ACCEPTED, user_index);
    listener_unlock();
}

// ---- users ----
//
// Each user is what profiles.c has signed in there: a local profile, a guest of one, or nobody. With Xbox Live
// (port/sdk/online/live.h: HALO3_LIVE_SERVER) any profile with an active Live binding uses its server-issued XUID
// and gamertag, and its guests are on Live with it. Saves stay under the profile's offline XUID
// (port_profile_user_xuid, xlive_offline.cpp).
//
// A guest (the XDK's XUSER_SIGNIN_INFO, XUSER_INFO_FLAG_GUEST): its sponsor's sign-in state, guest number 1..3 and the
// sponsor's user index; no offline XUID (INVALID_XUID when asked for it); with a Live sponsor, an online XUID: the
// sponsor's with the guest number in bits 54..55, which is what makes an XUID a guest's (a title's IsGuestXUID: the
// high 16 bits are 0x?9 with bits 6..7 set). Its gamertag is "<sponsor> (<n>)".

// Read the Live identity for this local profile or its guest. Guide callers can inspect ownership without HTTP.
static int user_live_identity(port_dword user_index, const port_profile_user_info *info, int start,
    port_qword *xuid, char gamertag[16])
{
    port_profile_user_info sponsor;
    const int guest = info->kind == PORT_SIGNIN_GUEST;
    const port_profile_user_info *owner = info;
    if (guest) {
        user_index = (port_dword)info->sponsor;
        if (!port_profile_user(user_index, &sponsor))
            return 0;
        owner = &sponsor;
    }
    if (owner->kind != PORT_SIGNIN_PROFILE)
        return 0;
    if (start && !port_live_user_signed_in(user_index) && port_live_user_enabled(user_index))
        port_live_startup();

    char sponsor_name[16];
    char *name = guest && gamertag ? sponsor_name : gamertag;
    if (!port_live_profile_identity(user_index, owner->name, xuid, name, 16))
        return 0;
    if (guest) {
        if (xuid)
            *xuid |= (port_qword)info->guest_number << 54;
        if (gamertag)
            port_profile_guest_name(sponsor_name, info->guest_number, gamertag);
    }
    return 1;
}

// Never signs in to Live (the guide asks under its lock): an XUID of the account's exists only once it is signed in.
PORT_MS_ABI int port_profile_user_owns_xuid(port_dword user_index, port_qword xuid)
{
    port_profile_user_info info;
    if (!port_profile_user(user_index, &info) || xuid == 0)
        return 0;
    port_qword live_xuid;
    return xuid == info.xuid || (user_live_identity(user_index, &info, 0, &live_xuid, 0) && xuid == live_xuid);
}

// XUSER_SIGNIN_STATE: 0 not signed in, 1 signed in locally, 2 signed in to Xbox Live (a guest: its sponsor's).
PORT_MS_ABI port_dword XUserGetSigninState(port_dword user_index)
{
    port_profile_user_info info;
    if (!port_profile_user(user_index, &info))
        return 0;
    return user_live_identity(user_index, &info, 1, 0, 0) ? 2 : 1;
}

PORT_MS_ABI port_dword XUserGetName(port_dword user_index, char *name, port_dword name_len)
{
    if (name == 0 || name_len == 0)
        return PORT_ERROR_BAD_ARGUMENTS;
    name[0] = 0;
    if (user_index >= PORT_PROFILE_USERS)
        return PORT_ERROR_BAD_ARGUMENTS;
    port_profile_user_info info;
    if (!port_profile_user(user_index, &info))
        return PORT_ERROR_NOT_LOGGED_ON;
    char gamertag[16];
    const char *source = user_live_identity(user_index, &info, 1, 0, gamertag) ? gamertag : info.name;
    port_dword n = 0;
    for (; source[n] != 0 && n + 1 < name_len; n++)
        name[n] = source[n];
    name[n] = 0;
    return PORT_ERROR_SUCCESS;
}

PORT_MS_ABI port_dword XUserGetXUID(port_dword user_index, port_qword *xuid)
{
    if (xuid == 0 || user_index >= PORT_PROFILE_USERS)
        return PORT_ERROR_BAD_ARGUMENTS;
    *xuid = 0;
    port_profile_user_info info;
    if (!port_profile_user(user_index, &info))
        return PORT_ERROR_NOT_LOGGED_ON;
    if (!user_live_identity(user_index, &info, 1, xuid, 0))
        *xuid = info.kind == PORT_SIGNIN_PROFILE ? info.xuid : 0;
    return PORT_ERROR_SUCCESS;
}

// XUSER_SIGNIN_INFO (40 bytes on the console and on i686: the XUID is 8-aligned)
typedef struct {
    port_qword xuid;
    port_dword info_flags;   // XUSER_INFO_FLAG_LIVE_ENABLED 1, XUSER_INFO_FLAG_GUEST 2
    port_dword signin_state; // XUSER_SIGNIN_STATE
    port_dword guest_number;
    port_dword sponsor_user_index;
    char user_name[16];
} port_xuser_signin_info;
typedef char port_xuser_signin_info_size[sizeof(port_xuser_signin_info) == 40 ? 1 : -1];

#define PORT_XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY 0x00000001ul
#define PORT_XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY 0x00000002ul
#define PORT_XUSER_INFO_FLAG_LIVE_ENABLED 0x00000001ul
#define PORT_XUSER_INFO_FLAG_GUEST 0x00000002ul

// For a profile signed in to Live, its XUID is the server-issued one whichever is
// asked for (network_update_online_users asks for the offline one, then the online one: the title knows the account
// by one XUID; its saves stay under the offline XUID, port_profile_user_owns_xuid). A local profile has no Xbox Live
// account: asked for its online XUID only, the XUID is INVALID_XUID (0), as on a console; otherwise (0,
// XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY) it is the offline XUID. A guest has no offline XUID (INVALID_XUID for
// ..._OFFLINE_XUID_ONLY: the title then takes its sponsor's), and its online XUID only with a Live sponsor.
PORT_MS_ABI long XamUserGetSigninInfo(port_dword user_index, port_dword flags, void *info)
{
    if (info == 0 || user_index >= PORT_PROFILE_USERS)
        return PORT_E_INVALIDARG;
    port_xuser_signin_info *out = (port_xuser_signin_info *)info;
    __builtin_memset(out, 0, sizeof(*out));
    port_profile_user_info user;
    if (!port_profile_user(user_index, &user))
        return PORT_E_NOT_LOGGED_ON;
    const int live = user_live_identity(user_index, &user, 1, &out->xuid, out->user_name);
    out->info_flags = live ? PORT_XUSER_INFO_FLAG_LIVE_ENABLED : 0;
    out->signin_state = live ? 2 : 1; // eXUserSigninState_SignedInToLive / _SignedInLocally
    if (user.kind == PORT_SIGNIN_GUEST) {
        out->info_flags |= PORT_XUSER_INFO_FLAG_GUEST;
        out->guest_number = user.guest_number;
        out->sponsor_user_index = user.sponsor;
        if ((flags & PORT_XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY) != 0)
            out->xuid = 0;
    } else if (!live && (flags & PORT_XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY) == 0) {
        out->xuid = user.xuid;
    }
    if (!live)
        __builtin_memcpy(out->user_name, user.name, sizeof(out->user_name));
    return 0;
}

port_dword XUserGetSigninInfo(port_dword user_index, port_dword flags, void *info)
{
    const long result = XamUserGetSigninInfo(user_index, flags, info);
    if (result == 0)
        return PORT_ERROR_SUCCESS;
    return result == PORT_E_NOT_LOGGED_ON ? PORT_ERROR_NO_SUCH_USER : PORT_ERROR_BAD_ARGUMENTS;
}

// A local profile has the privileges a console grants one with no parental controls set, and a guest its sponsor's;
// nobody else has any.
PORT_MS_ABI port_dword XUserCheckPrivilege(port_dword user_index, port_dword privilege, int *result)
{
    (void)privilege;
    if (result == 0 || user_index >= PORT_PROFILE_USERS)
        return PORT_ERROR_BAD_ARGUMENTS;
    *result = 0;
    if (!port_profile_signed_in(user_index))
        return PORT_ERROR_NOT_LOGGED_ON;
    *result = 1;
    return PORT_ERROR_SUCCESS;
}

void XUserSetContext(port_dword user_index, port_dword context_id, port_dword context_value)
{
    if (port_live_enabled())
        port_live_set_context((unsigned int)user_index, (unsigned int)context_id, (unsigned int)context_value);
}

PORT_MS_ABI port_dword XUserSetContextEx(port_dword user_index, port_dword context_id, port_dword context_value, void *overlapped)
{
    (void)overlapped;
    if (port_live_enabled())
        port_live_set_context((unsigned int)user_index, (unsigned int)context_id, (unsigned int)context_value);
    return PORT_ERROR_SUCCESS;
}

PORT_MS_ABI port_dword XUserSetPropertyEx(port_dword user_index, port_dword property_id, port_dword bytes, const void *value,
    void *overlapped)
{
    (void)overlapped;
    if (port_live_enabled() && value != 0) {
        unsigned int type = (unsigned int)((property_id >> 28) & 0xfu);
        if (type == PORT_LIVE_PROP_INT32 && bytes >= 4) {
            int v = 0;
            __builtin_memcpy(&v, value, 4);
            port_live_set_property_int32((unsigned int)user_index, (unsigned int)property_id, v);
        } else if (type == PORT_LIVE_PROP_INT64 && bytes >= 8) {
            long long v = 0;
            __builtin_memcpy(&v, value, 8);
            port_live_set_property_int64((unsigned int)user_index, (unsigned int)property_id, v);
        } else if (type == PORT_LIVE_PROP_DOUBLE && bytes >= 8) {
            double v = 0.0;
            __builtin_memcpy(&v, value, 8);
            port_live_set_property_double((unsigned int)user_index, (unsigned int)property_id, v);
        } else if (type == PORT_LIVE_PROP_FLOAT && bytes >= 4) {
            float v = 0.0f;
            __builtin_memcpy(&v, value, 4);
            port_live_set_property_float((unsigned int)user_index, (unsigned int)property_id, v);
        }
    }
    return PORT_ERROR_SUCCESS;
}

// ---- controllers (port/sdk/input/port_input.h) ----
//
// Controller 0 is the game window's keyboard and mouse merged with hardware gamepad 0 (either can play); 1..3 are
// hardware gamepads. A controller is connected while one of its sources is. The packet number changes whenever the
// state does, as XInput's does. The Guide button (XINPUT_GAMEPAD_GUIDE, 0x0400: a gamepad's, Home on the keyboard) is
// the system's: no title sees it; a press opens the guide's menu (guide.h). While a guide dialog shows (the guide, the
// virtual keyboard) a connected controller reads as idle: the guide has the input, and each poll passes the
// controller's buttons, triggers and left stick to the guide. When one closes, what a controller holds then (as of its
// last poll: a button, a trigger past XInput's threshold, a stick out of its dead zone) stays hidden from the title,
// each until it is let go (the A that chose, the B that backed out, the stick or D-pad that scrolled the guide, held a
// moment too long, do not act on the title's menu or in its game); a press that starts later is the title's. Sources
// are still read meanwhile, so mouse motion made then is consumed, not replayed.

#define PORT_ERROR_DEVICE_NOT_CONNECTED 1167ul

// What of `gamepad` is held (g_held's bits): XInput's trigger threshold (XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 30) and
// stick dead zones (XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE 7849, ..._RIGHT_THUMB_DEADZONE 8689), as titles read them.
static unsigned long held_controls(const port_input_gamepad *gamepad)
{
    unsigned long held = gamepad->buttons;
    if (gamepad->left_trigger > 30)
        held |= PORT_HELD_LEFT_TRIGGER;
    if (gamepad->right_trigger > 30)
        held |= PORT_HELD_RIGHT_TRIGGER;
    if (gamepad->thumb_lx > 7849 || gamepad->thumb_lx < -7849 || gamepad->thumb_ly > 7849 || gamepad->thumb_ly < -7849)
        held |= PORT_HELD_LEFT_STICK;
    if (gamepad->thumb_rx > 8689 || gamepad->thumb_rx < -8689 || gamepad->thumb_ry > 8689 || gamepad->thumb_ry < -8689)
        held |= PORT_HELD_RIGHT_STICK;
    return held;
}

// XINPUT_STATE
typedef struct {
    port_dword packet_number;
    port_input_gamepad gamepad;
} port_xinput_state;

// XINPUT_CAPABILITIES: Type, SubType, Flags, Gamepad, Vibration
typedef struct {
    unsigned char type;
    unsigned char sub_type;
    unsigned short flags;
    port_input_gamepad gamepad;
    unsigned short left_motor, right_motor;
} port_xinput_capabilities;

static struct {
    port_input_gamepad last;
    port_dword packet_number;
    int guide_down; // the Guide button at the last poll
} g_controllers[PORT_INPUT_USER_COUNT];
static int g_keyboard_present;            // the keyboard was present at the last poll of controller 0
static port_dword g_last_used_controller; // the controller whose buttons changed last (the guide's)

// The larger deflection of two stick axes.
static short stronger(short a, short b)
{
    const int magnitude_a = a < 0 ? -(int)a : a, magnitude_b = b < 0 ? -(int)b : b;
    return magnitude_a >= magnitude_b ? a : b;
}

// The merged state of controller `user`: its gamepad and the keyboard layout's virtual gamepad (controller 0: the
// window's keyboard and mouse plus the input script's keys, port_input_keyboard_state; 1..3: the input script's keys for
// that controller, port_input_script_state). Returns 0 when nothing is connected to it. Consumes the mouse motion of
// controller 0 and the taps since the last poll (port_input.h: taps between two polls), so only XamInputGetState polls
// it.
static struct {
    int active;
    unsigned short buttons;
    unsigned char left_trigger, right_trigger;
    short thumb_lx, thumb_ly;
} g_sim_gamepad[PORT_INPUT_USER_COUNT];

PORT_MS_ABI void port_sim_gamepad_set(unsigned int user, int active, unsigned short buttons,
    unsigned char left_trigger, unsigned char right_trigger, short thumb_lx, short thumb_ly)
{
    if (user >= PORT_INPUT_USER_COUNT)
        return;
    g_sim_gamepad[user].buttons = buttons;
    g_sim_gamepad[user].left_trigger = left_trigger;
    g_sim_gamepad[user].right_trigger = right_trigger;
    g_sim_gamepad[user].thumb_lx = thumb_lx;
    g_sim_gamepad[user].thumb_ly = thumb_ly;
    __atomic_store_n(&g_sim_gamepad[user].active, active ? 1 : 0, __ATOMIC_RELEASE);
}

static int controller_state(port_dword user, port_input_gamepad *out)
{
    if (__atomic_load_n(&g_sim_gamepad[user].active, __ATOMIC_ACQUIRE)) {
        __builtin_memset(out, 0, sizeof(*out));
        out->buttons = g_sim_gamepad[user].buttons;
        out->left_trigger = g_sim_gamepad[user].left_trigger;
        out->right_trigger = g_sim_gamepad[user].right_trigger;
        out->thumb_lx = g_sim_gamepad[user].thumb_lx;
        out->thumb_ly = g_sim_gamepad[user].thumb_ly;
        if (user == 0 && held_controls(out) != 0)
            port_input_note_gamepad_activity();
        return 1;
    }
    port_input_gamepad keyboard, pad;
    const int has_pad = port_input_hardware_state(user, &pad, 1);
    const int has_keyboard = user == 0 ? port_input_keyboard_state(&keyboard) : port_input_script_state(user, &keyboard, 1);
    if (user == 0) {
        g_keyboard_present = has_keyboard;
        if (has_pad && held_controls(&pad) != 0)
            port_input_note_gamepad_activity();
    }
    if (has_pad && has_keyboard) {
        out->buttons = (unsigned short)(pad.buttons | keyboard.buttons);
        out->left_trigger = pad.left_trigger > keyboard.left_trigger ? pad.left_trigger : keyboard.left_trigger;
        out->right_trigger = pad.right_trigger > keyboard.right_trigger ? pad.right_trigger : keyboard.right_trigger;
        out->thumb_lx = stronger(pad.thumb_lx, keyboard.thumb_lx);
        out->thumb_ly = stronger(pad.thumb_ly, keyboard.thumb_ly);
        out->thumb_rx = stronger(pad.thumb_rx, keyboard.thumb_rx);
        out->thumb_ry = stronger(pad.thumb_ry, keyboard.thumb_ry);
    } else if (has_pad) {
        *out = pad;
    } else if (has_keyboard) {
        *out = keyboard;
    }
    return has_pad || has_keyboard;
}

// Whether controller `user` has the keyboard layout's virtual gamepad, without consuming input: (controller 0) the
// keyboard as of the last XamInputGetState, (1..3) the input script's gamepad for it.
static int keyboard_present(port_dword user)
{
    port_input_gamepad scripted;
    return user == 0 ? g_keyboard_present : port_input_script_state(user, &scripted, 0);
}

// Whether something is connected to controller `user`, without consuming input: its gamepad now, or the keyboard
// layout's (keyboard_present).
static int controller_connected(port_dword user)
{
    if (__atomic_load_n(&g_sim_gamepad[user].active, __ATOMIC_ACQUIRE))
        return 1;
    port_input_gamepad pad;
    return port_input_hardware_state(user, &pad, 0) || keyboard_present(user);
}

port_dword XamInputGetState(port_dword user_index, port_dword flags, void *state)
{
    (void)flags;
    if (user_index >= PORT_INPUT_USER_COUNT || state == 0)
        return PORT_ERROR_BAD_ARGUMENTS;
    port_input_gamepad gamepad;
    if (!controller_state(user_index, &gamepad)) {
        g_controllers[user_index].guide_down = 0;
        __atomic_store_n(&g_held[user_index], 0, __ATOMIC_RELEASE);
        guide_controller_lost(user_index);
        return PORT_ERROR_DEVICE_NOT_CONNECTED;
    }
    const int focused = port_window_is_focused();
    if (!focused)
        __builtin_memset(&gamepad, 0, sizeof(gamepad));
    // before the guide sees the poll: a dialog this poll closes hides what it holds (ui_close)
    const unsigned long held = held_controls(&gamepad);
    __atomic_store_n(&g_held[user_index], held, __ATOMIC_RELEASE);
    // the Guide button: the system's
    const int guide_down = (gamepad.buttons & PORT_XINPUT_GUIDE) != 0;
    const int guide_pressed = guide_down && !g_controllers[user_index].guide_down;
    g_controllers[user_index].guide_down = guide_down;
    if (guide_showing())
        guide_controller(user_index, gamepad.buttons, gamepad.left_trigger, gamepad.right_trigger, gamepad.thumb_lx,
                         gamepad.thumb_ly);
    else if (guide_pressed)
        guide_button(user_index);
    gamepad.buttons &= (unsigned short)~PORT_XINPUT_GUIDE;
    const int dialog = __atomic_load_n(&g_ui, __ATOMIC_ACQUIRE) != k_ui_none;
    if (user_index == 0 && (dialog || !focused))
        port_input_keyboard_suppress(); // the keyboard's game actions too (port_input.h)
    if (dialog) {
        __builtin_memset(&gamepad, 0, sizeof(gamepad)); // a guide dialog has the input
    } else {
        // held since a dialog closed: not the title's yet; what has been let go is its own again
        const unsigned long suppressed = __atomic_and_fetch(&g_suppressed[user_index], held, __ATOMIC_ACQ_REL);
        if (suppressed != 0) {
            gamepad.buttons &= (unsigned short)~suppressed;
            if ((suppressed & PORT_HELD_LEFT_TRIGGER) != 0)
                gamepad.left_trigger = 0;
            if ((suppressed & PORT_HELD_RIGHT_TRIGGER) != 0)
                gamepad.right_trigger = 0;
            if ((suppressed & PORT_HELD_LEFT_STICK) != 0)
                gamepad.thumb_lx = gamepad.thumb_ly = 0;
            if ((suppressed & PORT_HELD_RIGHT_STICK) != 0)
                gamepad.thumb_rx = gamepad.thumb_ry = 0;
        }
    }
    if (__builtin_memcmp(&gamepad, &g_controllers[user_index].last, sizeof(gamepad)) != 0) {
        if (gamepad.buttons != g_controllers[user_index].last.buttons && gamepad.buttons != 0)
            g_last_used_controller = user_index;
        g_controllers[user_index].last = gamepad;
        g_controllers[user_index].packet_number++;
    }
    port_xinput_state *out = (port_xinput_state *)state;
    out->packet_number = g_controllers[user_index].packet_number;
    out->gamepad = gamepad;
    return PORT_ERROR_SUCCESS;
}

PORT_MS_ABI port_dword XamInputGetCapabilities(port_dword user_index, port_dword flags, void *capabilities)
{
    (void)flags;
    if (user_index >= PORT_INPUT_USER_COUNT || capabilities == 0)
        return PORT_ERROR_BAD_ARGUMENTS;
    if (!controller_connected(user_index))
        return PORT_ERROR_DEVICE_NOT_CONNECTED;
    port_xinput_capabilities *out = (port_xinput_capabilities *)capabilities;
    __builtin_memset(out, 0, sizeof(*out));
    out->type = 1;     // XINPUT_DEVTYPE_GAMEPAD
    out->sub_type = 1; // XINPUT_DEVSUBTYPE_GAMEPAD
    // every control present at full resolution, both motors
    out->gamepad.buttons = 0xf3ff;
    out->gamepad.left_trigger = 0xff;
    out->gamepad.right_trigger = 0xff;
    out->gamepad.thumb_lx = out->gamepad.thumb_ly = out->gamepad.thumb_rx = out->gamepad.thumb_ry = (short)0xffc0;
    out->left_motor = out->right_motor = 0xffff;
    return PORT_ERROR_SUCCESS;
}

// XINPUT_VIBRATION (wLeftMotorSpeed, wRightMotorSpeed) to the gamepad's motors (port_input_hardware_vibration); the
// keyboard has none. The game sets every controller's each frame (input_update_gamepads_rumble); the host side
// passes on changes only.
PORT_MS_ABI port_dword XamInputSetState(port_dword user_index, port_dword flags, void *vibration, unsigned char amplitude,
    unsigned char frequency, unsigned char offset)
{
    (void)flags; (void)amplitude; (void)frequency; (void)offset;
    if (user_index >= PORT_INPUT_USER_COUNT || vibration == 0)
        return PORT_ERROR_BAD_ARGUMENTS;
    const unsigned short *motors = (const unsigned short *)vibration;
    const unsigned short left = port_window_is_focused() ? motors[0] : 0;
    const unsigned short right = port_window_is_focused() ? motors[1] : 0;
    if (!port_input_hardware_vibration(user_index, left, right) && !keyboard_present(user_index))
        return PORT_ERROR_DEVICE_NOT_CONNECTED;
    return PORT_ERROR_SUCCESS;
}

// ---- the sign-in UI (port/sdk/xam/xam_profile.h) ----
//
// XShowSigninUI(panes, flags) on a console opens the guide's sign-in panes, where each player picks a profile, signs in
// as a guest, or creates a profile. Here the guide (guide.h) shows them, a pane per controller; the request knows which
// controllers are connected (the panes that wait for a player) and which was used last (the player who asked: the one
// pane of a 1-pane request). When nobody operates the guide, the users of those controllers that are not signed in
// sign in to their own built-in profile ("Player <n+1>", or the first free one) instead; none can when no profile
// exists (HALO3_NO_PROFILE) or when only profiles with Xbox Live are shown (XSSUI_FLAGS_SHOWONLYONLINEENABLED). The
// listeners see the guide open and close around it, and the change of sign-in, as a console's do.

#define PORT_ERROR_INVALID_PARAMETER 87ul

PORT_MS_ABI port_dword port_profile_show_signin(port_dword panes, port_dword flags)
{
    if (panes != 1 && panes != 2 && panes != 4)
        return PORT_ERROR_INVALID_PARAMETER;
    guide_request request;
    __builtin_memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_SIGNIN;
    request.user = g_last_used_controller;
    request.panes = panes;
    request.flags = flags;
    for (port_dword user = 0; user < PORT_INPUT_USER_COUNT; user++)
        if (controller_connected(user))
            request.connected |= 1ul << user;
    return guide_open(&request);
}

PORT_MS_ABI port_dword port_xam_last_used_controller(void) { return g_last_used_controller; }

PORT_MS_ABI void port_xam_guide_poll(void)
{
    for (port_dword user = 0; user < PORT_INPUT_USER_COUNT; user++) {
        port_xinput_state state;
        XamInputGetState(user, 0, &state);
    }
}
