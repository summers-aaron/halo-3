// port/sdk/xam (both hosts): the Guide (guide.h): its pages, who operates them, and how a request ends.
//
// The guide holds one request at a time. Its panes are the controllers': a sign-in request shows all four (each
// controller signs its own user in), any other request the pane of the controller that opened it. A pane is a stack of
// pages (the first is the request's; the menu leads to others), each a list of items with one selected, built from the
// state it shows (the profiles, the request) whenever it is drawn or acted on. Pages:
//   menu           Resume Game, Profiles, Achievements, Friends, Recent Players, Messages, Settings, Copy
//                  Lobby Code, Join by Lobby Code, Sign Out
//   sign-in        each profile (one signed in on another controller cannot be chosen), Sign in to SlopBoxLive (when a Live
//                  server is configured and this slot is offline), Create Profile, Sign in as Guest (when XShowSigninUI
//                  or XEnableGuestSignin requests guest sign-in and a profile is signed in to sponsor one), Sign Out
//   create         a gamertag, submitted to Live first when configured, or explicitly created offline
//   message box    the title's text and buttons
//   devices        the storage devices to choose from
//   blades         achievements (the title's, earned or not, with gamerscore), friends (Live's, each a page that
//                  invites them), recent players, messages (the game invites received, each a page to accept or
//                  decline), message compose, game invite (Copy Lobby Code and the friends), friend request, gamercard,
//                  player review, settings (the game's own: halo3.ini, port/sdk/runtime/port_config.h; each a value
//                  that left / right and A change, saved as it changes, and Reset to Defaults)
//   lobby code     the lobby's code (port/sdk/online/invite.h), copied to the clipboard as the page opens
//   join code      a text field for a lobby code (typed, or pasted: Ctrl+V), and Join Lobby: the invite accepted
//   invite         a game invite received: Accept (the invite accepted), Decline
//   friend         a Live friend: Invite to Game
// Everything is under one lock, taken briefly by the controllers' polls (the game's thread) and the window's keys (its
// own). A sign-in change is posted (XN_SYS_SIGNINCHANGED) as it is made, under the lock, so it always precedes the
// XN_SYS_UI (0) of the request it is part of. What must not run under it is collected while it is held and done once it
// is released, in this order: creating a profile (a file written), loading achievements (files read), making a lobby
// code (the gamertag may need Live), the clipboard (the host may wait for another program), Live's friends list and
// invites (HTTP), checking an invite before accepting it and writing halo3.ini (a file flushed to the device), each
// putting its result in place if the opening of the guide that asked is still the one open; posting XN_SYS_UI (0) and
// calling the request's completion when it ended; accepting an invite (XN_LIVE_INVITE_ACCEPTED, so after the guide's
// XN_SYS_UI (0)); the trace; waking the window. Those that wait on the network, the clipboard or the disk (halo3.ini)
// run on the guide's worker thread (see "the worker"), the rest on the thread that acted.
#ifdef _WIN32
__declspec(dllimport) char *__cdecl getenv(const char *name);
#else
char *getenv(const char *name);
#endif

#include "guide.h"
#include "clipboard.h"
#include "guide_host.h"
#include "guide_ingame.h"
#include "xam_profile.h"
#include "xam_ui.h"
#include "../online/invite.h"
#include "../online/live.h"
#include "../runtime/port_config.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"
#include "../xhv/xhv_voice.h"

#define GUIDE_ERROR_SUCCESS 0ul
#define GUIDE_ERROR_ACCESS_DENIED 5ul
#define GUIDE_ERROR_INVALID_PARAMETER 87ul

#define GUIDE_XSSUI_SHOWONLYONLINEENABLED 0x00000002ul
#define GUIDE_XSSUI_ENABLE_GUEST 0x00080000ul
#define GUIDE_XSSUI_CONVERTOFFLINETOGUEST 0x00400000ul

// XINPUT_GAMEPAD buttons the guide reads, and the triggers as two more (guide_controller)
#define GUIDE_PAD_UP 0x0001u
#define GUIDE_PAD_DOWN 0x0002u
#define GUIDE_PAD_LEFT 0x0004u
#define GUIDE_PAD_RIGHT 0x0008u
#define GUIDE_PAD_START 0x0010u
#define GUIDE_PAD_BACK 0x0020u
#define GUIDE_PAD_LEFT_THUMB 0x0040u
#define GUIDE_PAD_RIGHT_THUMB 0x0080u
#define GUIDE_PAD_LEFT_SHOULDER 0x0100u
#define GUIDE_PAD_RIGHT_SHOULDER 0x0200u
#define GUIDE_PAD_GUIDE 0x0400u
#define GUIDE_PAD_A 0x1000u
#define GUIDE_PAD_B 0x2000u
#define GUIDE_PAD_X 0x4000u
#define GUIDE_PAD_Y 0x8000u
#define GUIDE_PAD_LEFT_TRIGGER 0x10000u
#define GUIDE_PAD_RIGHT_TRIGGER 0x20000u
#define GUIDE_PAD_DPAD (GUIDE_PAD_UP | GUIDE_PAD_DOWN | GUIDE_PAD_LEFT | GUIDE_PAD_RIGHT)
#define GUIDE_BUTTONS_UNKNOWN 0xffffffffu // a pane's controller not polled since the guide opened
// The left stick as Halo 3's UI reads it: past XInput's dead zone, rescaled to the full range (udpate_game_stick,
// input_xinput.cpp), and a direction once an axis is 15000 or more (event_manager_update, event_manager_tab).
#define GUIDE_STICK_DEAD_ZONE 7849
#define GUIDE_STICK_THRESHOLD 15000

#define GUIDE_STACK 4
#define GUIDE_PAGE_STEP 8 // Page Up / Page Down when the rows shown are not known (no in-game layout yet)

enum guide_page {
    PAGE_MENU,
    PAGE_SIGNIN,
    PAGE_CREATE,
    PAGE_LIVE_CONNECT,
    PAGE_MESSAGE_BOX,
    PAGE_DEVICES,
    PAGE_BLADE,
    PAGE_LOBBY_CODE, // the lobby's code
    PAGE_JOIN_CODE,  // a lobby code to join
    PAGE_INVITE,     // a game invite received
    PAGE_FRIEND,     // a Live friend
    PAGE_KBM,        // Settings > Keyboard & Mouse
};

enum guide_item_id {
    ITEM_INFO,       // shown, nothing to choose (an achievement)
    ITEM_RESUME,
    ITEM_PROFILES,
    ITEM_BLADE,      // arg: guide_blade
    ITEM_SIGN_OUT,
    ITEM_CLOSE_GAME,
    ITEM_PROFILE,    // arg: catalog index
    ITEM_CONNECT_LIVE,
    ITEM_CONNECT_LIVE_CONFIRM,
    ITEM_GUEST,
    ITEM_CREATE,
    ITEM_CREATE_CONFIRM,
    ITEM_CREATE_OFFLINE,
    ITEM_BUTTON,     // arg: button index
    ITEM_DEVICE,     // arg: device index
    ITEM_REVIEW,     // arg: k_review_choices index
    ITEM_LOBBY_CODE, // the lobby code page
    ITEM_JOIN_CODE,  // the join code page
    ITEM_COPY,       // the lobby code to the clipboard again
    ITEM_PASTE,      // the clipboard into the field
    ITEM_JOIN,       // the field's code accepted
    ITEM_INVITE,     // arg: an inbox index (port_invite_inbox, newest first)
    ITEM_ACCEPT,     // the invite page's invite accepted
    ITEM_DECLINE,    // ... declined
    ITEM_FRIEND,     // arg: a friends list index
    ITEM_SEND_INVITE, // the friend page's friend invited
    ITEM_SETTING,    // arg: port_setting (the Settings page)
    ITEM_RESET,      // the Settings page's settings back to their defaults
    ITEM_KBM_PAGE,   // the Settings page's Keyboard & Mouse row: its page
    ITEM_BINDING,    // arg: port_kbm_action (the Keyboard & Mouse page): choosing it captures the next input
    ITEM_KBM_RESET,  // the Keyboard & Mouse page's bindings back to the preset's
};

typedef struct {
    int id;
    unsigned long arg;
    unsigned int image_id;
    char text[GUIDE_LABEL], detail[GUIDE_LABEL];
    int enabled, checked;
    const char *why; // a disabled item: why it cannot be chosen
    int setting;     // guide_setting_state (the Settings page's rows)
    char about[GUIDE_LABEL * 2]; // an enabled item: what choosing it does (the hint), "" for nothing to add
} guide_item;

typedef struct {
    int page, blade, selected;
} guide_level;

// Hold-to-repeat, as Halo 3's menus (event_manager_tab, user_interface_controller.cpp; see guide.h): one per controller
// and one for the window's arrow keys.
typedef struct {
    long previous_x, previous_y; // the direction last given (g_event_manager_previous_stick)
    int active;                  // a direction was given at the last step (g_event_manager_tab_active)
    int timed;                   // an event was given since the direction was last let go: `last` and `start` hold
    unsigned long last;          // when the last event was given (when it was due: see repeat_step)
    unsigned long start;         // when the first was (the repeat's start: g_event_manager_repeat_start_times)
    int stepped;                 // a step was made: `step` is when
    unsigned long step;
} guide_repeat;

// A D-pad (a controller's, or the window's arrow keys): what is down, since when (GetTickCount), and which went down
// at the step being made (a press acts at once, as on the poll its button goes down).
enum { k_dpad_up, k_dpad_down, k_dpad_left, k_dpad_right, k_dpad_count };
typedef struct {
    unsigned char down[k_dpad_count], fresh[k_dpad_count];
    unsigned long since[k_dpad_count];
} guide_dpad;

// A lobby code fits the field: typed (PORT_INVITE_CODE_MAX - 1 characters), or shown as lines of 6 groups (a break per
// 30 characters at most).
typedef char guide_code_fits_field[PORT_INVITE_CODE_MAX + PORT_INVITE_CODE_MAX / 30u <= GUIDE_FIELD ? 1 : -1];

typedef struct {
    int active;
    guide_level stack[GUIDE_STACK];
    int depth;
    int osk_key, osk_shift;                    // highlighted cell and letter case on the controller keyboard
    int osk_menu_focus;                        // the mouse or list navigation selected the action below the field
    int awaiting_live;                         // this page requested an existing profile connection
    char field[GUIDE_FIELD];                   // the profile name, Live gamertag or lobby code
    char notice[GUIDE_LABEL * 2];             // the last action's outcome
    char code[PORT_INVITE_CODE_MAX];           // the lobby code page's code ("" while it is made)
    int code_class;                            // ... its lobby's class (PORT_INVITE_CLASS_*)
    port_invite invite;                        // the invite page's invite
    port_live_friend friend_;                  // the friend page's friend
    unsigned long buttons;                     // the controller's buttons at its last poll (GUIDE_BUTTONS_UNKNOWN),
                                               // the triggers as GUIDE_PAD_LEFT_TRIGGER / GUIDE_PAD_RIGHT_TRIGGER
    unsigned char trigger_threshold[2];        // each trigger's (input_xinput_update_trigger)
    guide_dpad dpad;                           // the controller's D-pad
    unsigned long stale;                       // D-pad directions held since before the first poll (ignored)
    guide_repeat repeat;                       // ... and its left stick and D-pad's repeat
    int kbm_slot;                              // the Keyboard & Mouse page: the selected binding's input shown and
                                               // captured, 0 the primary, 1 the alternate (left / right)
} guide_pane;

static const char *const k_review_choices[] = {
    "Prefer This Player", "Avoid: Bad Attitude", "Avoid: Offensive Name", "Avoid: Cheating", "Avoid: Quitting Early",
};
#define GUIDE_REVIEW_CHOICES (sizeof(k_review_choices) / sizeof(k_review_choices[0]))

// Live's friends list (g_guide.friends_state).
enum { k_friends_none, k_friends_reading, k_friends_read, k_friends_unsupported, k_friends_failed };

static const char *const k_blade_titles[GUIDE_BLADE_COUNT] = {
    "Achievements", "Friends", "Recent Players", "Messages", "Compose Message", "Invite to Game", "Friend Request",
    "Gamer Card", "Player Review", "Settings",
};
// ... as LB / RB's tabs name them: five fit a one-pane card whole
static const char *const k_blade_tabs[GUIDE_BLADE_COUNT] = {
    "Achievements", "Friends", "Players", "Messages", "Compose Message", "Invite to Game", "Friend Request",
    "Gamer Card", "Player Review", "Settings",
};

static struct {
    volatile char busy;
    int interactive; // -1 undecided
    int trace;       // -1 undecided
    int open;        // read without the lock by guide_showing
    unsigned int session, revision;
    unsigned int page_serial; // changes when a pane's page does (guide_page_serial)
    struct { int active, depth, page, blade; } pages[GUIDE_PANES]; // the pages page_serial was counted for
    guide_request request;
    guide_pane panes[GUIDE_PANES];
    int focus;               // the pane the window's keys drive
    int windowed;            // a window (its own, or the game's: in-game) shows this session: keys can operate a pane
                             // whose controller is gone
    unsigned long signins;   // users signed in through this request (at most request.panes)
    int working;             // a job of this session runs outside the lock: input waits
    int guest_enabled;       // XEnableGuestSignin
    guide_achievement achievements[GUIDE_ITEMS];
    unsigned long achievement_count;
    int achievements_user;              // whose achievements those are; -1 none
    unsigned long achievements_serial;  // ... as signed in then (port_profile_user_serial)
    // Live's friends list, read once per opening of the guide (the friends page)
    port_live_friend friends[GUIDE_ITEMS];
    unsigned long friend_count;
    int friends_state;                  // k_friends_*
    // the window's arrow keys: the focused pane's D-pad (guide_key_action), and their repeat
    guide_dpad keys;
    guide_repeat key_repeat;
    // the Keyboard & Mouse page waiting for an input to bind (guide_capture): its pane, the action and its slot
    struct { int active, pane, action, slot; } capture;
    // host microphones enumerated on the worker when the Settings page opens
    xhv_voice_device mic_devices[XHV_VOICE_MAX_DEVICES];
    unsigned mic_device_count;
} g_guide = { .interactive = -1, .trace = -1, .achievements_user = -1 };

// The items of the page being drawn or acted on (build_items), under the lock: valid until the next build_items. A
// list is too large for the stacks of the game's threads, which poll the controllers.
static guide_item g_items[GUIDE_ITEMS];
// The inbox as the Messages page's items were last built from it (build_items), under the lock: what an item's
// index names when it is chosen (choose builds the items first), so a poll that adds an invite in between cannot
// make it open another.
static port_invite g_inbox[PORT_INVITE_INBOX];
static unsigned long g_inbox_count;

static void guide_lock(void)
{
    while (__atomic_test_and_set(&g_guide.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void guide_unlock(void) { __atomic_clear(&g_guide.busy, __ATOMIC_RELEASE); }

// What to do once the lock is released (see the top of the file).
typedef struct {
    int create_pane;        // >= 0: create the profile named create_name for that pane's user
    int create_online;
    char create_name[PORT_PROFILE_GAMERTAG_MAX + 1];
    int connect_live_pane;  // >= 0: start async Live connect for that pane's user
    char connect_live_name[PORT_PROFILE_GAMERTAG_MAX + 1];
    char connect_live_profile[PORT_PROFILE_GAMERTAG_MAX + 1];
    unsigned int connect_live_generation;
    int load_achievements;  // >= 0: load that user's achievements
    int code_pane;          // >= 0: make the lobby code for that pane (and copy it)
    int copy_pane;          // >= 0: copy that pane's lobby code to the clipboard
    int paste_pane;         // >= 0: the clipboard into that pane's field
    int load_friends;       // Live's friends list
    int load_mics;          // host microphones (Settings page opened)
    int apply_mic;          // re-apply the selected microphone live (xhv_voice_refresh)
    int invite_pane;        // >= 0: invite that pane's friend (Live)
    int accept_pane;        // >= 0: check `accept` for that pane's user, then accept it once the guide has closed
    port_invite accept;
    char decline_id[PORT_INVITE_ID]; // a Live invite declined ("" none)
    int save_settings;      // write halo3.ini (the Settings page changed a setting)
    unsigned long session;  // the session that asked for any of the above
    int ended, result;
    unsigned long value;
    guide_request finished; // the request that ended
    int accepted;           // the request ended to accept `accept` for `accept_user`: done after XN_SYS_UI (0)
    unsigned long accept_user;
    int changed;            // something shown changed: trace, wake the window
} guide_effects;

static void effects_init(guide_effects *fx)
{
    __builtin_memset(fx, 0, sizeof(*fx));
    fx->create_pane = -1;
    fx->connect_live_pane = -1;
    fx->load_achievements = -1;
    fx->code_pane = fx->copy_pane = fx->paste_pane = fx->invite_pane = fx->accept_pane = -1;
}

// A sign-in change made under the lock: XN_SYS_SIGNINCHANGED is posted at once, still under it (the listeners' lock
// is taken inside the guide's, never the other way), so it reaches the listeners before the XN_SYS_UI (0) of a request
// that the next holder of the lock ends.
static void signin_changed(unsigned long bits) { port_xam_signin_changed(bits); }

// Whether the achievements loaded are user `user`'s as signed in now.
static int achievements_current(long user)
{
    return user >= 0 && g_guide.achievements_user == user &&
           g_guide.achievements_serial == port_profile_user_serial((unsigned long)user);
}

// ---- text ----

static unsigned long text_length(const char *text)
{
    unsigned long n = 0;
    while (text[n] != 0)
        n++;
    return n;
}

// Appends `text` to `to` (`capacity` bytes, NUL-terminated), never cutting a UTF-8 sequence.
static int same_text(const char *a, const char *b)
{
    while (*a != 0 && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static void append(char *to, unsigned long capacity, const char *text)
{
    unsigned long n = text_length(to);
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

static void copy(char *to, unsigned long capacity, const char *text)
{
    to[0] = 0;
    append(to, capacity, text);
}

static void append_number(char *to, unsigned long capacity, unsigned long value)
{
    char digits[24];
    unsigned long n = 0;
    do {
        digits[n++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    char text[24];
    unsigned long at = 0;
    while (n != 0)
        text[at++] = digits[--n];
    text[at] = 0;
    append(to, capacity, text);
}

static void append_hex(char *to, unsigned long capacity, unsigned long long value)
{
    char text[19] = "0x";
    for (int i = 0; i < 16; i++)
        text[2 + i] = "0123456789ABCDEF"[(value >> (60 - 4 * i)) & 15];
    text[18] = 0;
    append(to, capacity, text);
}

// "12.3 GB" (no 64-bit division: the port's runtime has no helper for it)
static void append_size(char *to, unsigned long capacity, unsigned long long bytes)
{
    const unsigned long megabytes = (unsigned long)(bytes >> 20);
    const unsigned long tenths = megabytes / 1024 * 10 + (megabytes % 1024 * 10 + 512) / 1024; // to the nearest
    append_number(to, capacity, tenths / 10);
    append(to, capacity, ".");
    append_number(to, capacity, tenths % 10);
    append(to, capacity, " GB");
}

// ---- settings ----

int guide_interactive(void)
{
    int interactive = __atomic_load_n(&g_guide.interactive, __ATOMIC_ACQUIRE);
    if (interactive < 0) {
        const char *setting = getenv("HALO3_GUIDE");
        if (setting != 0 && (setting[0] == 'i' || setting[0] == '1'))
            interactive = 1;
        else if (setting != 0 && (setting[0] == 'a' || setting[0] == '0'))
            interactive = 0;
        else
            interactive = port_window_native_window() != 0;
        __atomic_store_n(&g_guide.interactive, interactive, __ATOMIC_RELEASE);
    }
    return interactive;
}

static int trace_on(void)
{
    int trace = __atomic_load_n(&g_guide.trace, __ATOMIC_ACQUIRE);
    if (trace < 0) {
        const char *setting = getenv("HALO3_GUIDE_TRACE");
        trace = setting != 0 && setting[0] == '1';
        __atomic_store_n(&g_guide.trace, trace, __ATOMIC_RELEASE);
    }
    return trace;
}

PORT_MS_ABI void guide_enable_guest_signin(int enable)
{
    guide_lock();
    g_guide.guest_enabled = enable != 0;
    guide_unlock();
}

// ---- Halo 3's menu sounds and timing (guide.h) ----
//
// The sounds wait in a queue under a lock of their own, the innermost (taken under the guide's, never the other way),
// until the game's UI update plays them on its thread (port/probe/extra/guide_menu.cpp: user_interface_play_sound).

extern PORT_MS_ABI unsigned long GetTickCount(void);

static struct {
    volatile char busy;
    struct { int sound; unsigned long at; } queue[GUIDE_SOUNDS];
    unsigned head, count;
} g_sounds;

static struct { unsigned long hold_delay, repeat_delay, repeat_interval; } g_timing = {
    GUIDE_HOLD_DELAY_DEFAULT, GUIDE_REPEAT_DELAY_DEFAULT, GUIDE_REPEAT_INTERVAL_DEFAULT,
};

static void sounds_lock(void)
{
    while (__atomic_test_and_set(&g_sounds.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void sounds_unlock(void) { __atomic_clear(&g_sounds.busy, __ATOMIC_RELEASE); }

static const char *sound_name(int sound)
{
    switch (sound) {
    case GUIDE_SOUND_ERROR: return "error";
    case GUIDE_SOUND_VERTICAL_NAVIGATION: return "vertical_navigation";
    case GUIDE_SOUND_HORIZONTAL_NAVIGATION: return "horizontal_navigation";
    case GUIDE_SOUND_A_BUTTON: return "a_button";
    case GUIDE_SOUND_B_BUTTON: return "b_button";
    case GUIDE_SOUND_START_BUTTON: return "start_button";
    case GUIDE_SOUND_LEFT_BUMPER: return "left_bumper";
    case GUIDE_SOUND_RIGHT_BUMPER: return "right_bumper";
    case GUIDE_SOUND_LEFT_TRIGGER: return "left_trigger";
    case GUIDE_SOUND_RIGHT_TRIGGER: return "right_trigger";
    case GUIDE_SOUND_RANK_UP: return "rank_up";
    case GUIDE_SOUND_PARTY_UP: return "party_up";
    default: return "?";
    }
}

void guide_sound_post(int sound)
{
    if (sound < 0)
        return;
    const unsigned long now = GetTickCount();
    sounds_lock();
    const int queued = g_sounds.count < GUIDE_SOUNDS;
    if (queued) {
        const unsigned slot = (g_sounds.head + g_sounds.count++) % GUIDE_SOUNDS;
        g_sounds.queue[slot].sound = sound;
        g_sounds.queue[slot].at = now;
    }
    sounds_unlock();
    if (trace_on()) { // with the tick it was made at, for the cadence
        char line[96];
        copy(line, sizeof(line), queued ? "guide: sound " : "guide: sound dropped (queue full): ");
        append(line, sizeof(line), sound_name(sound));
        append(line, sizeof(line), " at ");
        append_number(line, sizeof(line), now);
        append(line, sizeof(line), "\n");
        port_write_error(line);
    }
}

PORT_MS_ABI int guide_sound_take(unsigned long now)
{
    int sound = -1, stale = 0;
    sounds_lock();
    while (g_sounds.count > 0 && sound < 0) {
        const unsigned slot = g_sounds.head;
        g_sounds.head = (g_sounds.head + 1) % GUIDE_SOUNDS;
        g_sounds.count--;
        // (tick differences are unsigned: right across the counter's wrap)
        if (now - g_sounds.queue[slot].at <= GUIDE_SOUND_STALE_MILLISECONDS)
            sound = g_sounds.queue[slot].sound;
        else
            stale++;
    }
    sounds_unlock();
    if (trace_on() && (sound >= 0 || stale != 0)) {
        if (stale != 0) {
            char line[64];
            copy(line, sizeof(line), "guide: stale sounds dropped: ");
            append_number(line, sizeof(line), (unsigned long)stale);
            append(line, sizeof(line), "\n");
            port_write_error(line);
        }
        if (sound >= 0) {
            port_write_error("guide: sound played: ");
            port_write_error(sound_name(sound));
            port_write_error("\n");
        }
    }
    return sound;
}

PORT_MS_ABI void guide_menu_timing(unsigned long hold_delay, unsigned long repeat_delay, unsigned long repeat_interval)
{
    if (hold_delay == 0 || repeat_interval == 0)
        return; // not a menu's timing: kept as it was
    __atomic_store_n(&g_timing.hold_delay, hold_delay, __ATOMIC_RELAXED);
    __atomic_store_n(&g_timing.repeat_delay, repeat_delay, __ATOMIC_RELAXED);
    __atomic_store_n(&g_timing.repeat_interval, repeat_interval, __ATOMIC_RELAXED);
    if (trace_on()) {
        char line[128];
        copy(line, sizeof(line), "guide: menu timing: hold_delay ");
        append_number(line, sizeof(line), hold_delay);
        append(line, sizeof(line), " ms, repeat_delay ");
        append_number(line, sizeof(line), repeat_delay);
        append(line, sizeof(line), " ms, repeat_interval ");
        append_number(line, sizeof(line), repeat_interval);
        append(line, sizeof(line), " ms\n");
        port_write_error(line);
    }
}

// ---- hold-to-repeat (guide.h): Halo 3's event_manager_update and event_manager_tab (user_interface_controller.cpp) ----

static long magnitude(long value) { return value < 0 ? -value : value; }

// When the key after one given at `last` is due, in milliseconds after the repeat's start (`last` too): the first time
// event_manager_tab's test holds, `now - last >= (now - start >= repeat_delay ? repeat_interval : hold_delay)`. Before
// repeat_delay a key comes every hold_delay; from then (from repeat_delay itself at the earliest) every
// repeat_interval.
static unsigned long repeat_due(unsigned long last, unsigned long hold_delay, unsigned long repeat_delay,
    unsigned long repeat_interval)
{
    const unsigned long by_interval = last + repeat_interval > repeat_delay ? last + repeat_interval : repeat_delay;
    return last + hold_delay < repeat_delay && last + hold_delay < by_interval ? last + hold_delay : by_interval;
}

// One step of event_manager_tab for direction (x, y) (each axis 0 or at least GUIDE_STICK_THRESHOLD) at `now`: the key
// it gives (GUIDE_KEY_UP / DOWN / LEFT / RIGHT), -1 none. An axis gives a key when it is the only one moved and was not
// moved at the step before (a new direction: `fresh_at` is when it began), or once the next key is due (repeat_due).
// The game counts from the frame that gave the last key; here from when that key was due, so the keys come at the same
// times however often the steps come (only later, by less than a step; a step more than a period late counts from
// itself, so a stall gives one key, not a burst).
static int repeat_step(guide_repeat *repeat, long x, long y, unsigned long now, unsigned long fresh_at)
{
    const unsigned long hold_delay = __atomic_load_n(&g_timing.hold_delay, __ATOMIC_RELAXED);
    const unsigned long repeat_delay = __atomic_load_n(&g_timing.repeat_delay, __ATOMIC_RELAXED);
    const unsigned long repeat_interval = __atomic_load_n(&g_timing.repeat_interval, __ATOMIC_RELAXED);
    const int moved_x = magnitude(x) >= GUIDE_STICK_THRESHOLD, moved_y = magnitude(y) >= GUIDE_STICK_THRESHOLD;
    const int previous_x = magnitude(repeat->previous_x) > GUIDE_STICK_THRESHOLD;
    const int previous_y = magnitude(repeat->previous_y) > GUIDE_STICK_THRESHOLD;
    // (times as offsets from the repeat's start: unsigned differences, right across the tick counter's wrap)
    const unsigned long due = repeat->timed ? repeat_due(repeat->last - repeat->start, hold_delay, repeat_delay,
                                                         repeat_interval)
                                            : 0;
    const int held = repeat->timed && now - repeat->start >= due;
    const int tab_x = moved_x && (!previous_x || held) && !moved_y && !previous_y;
    const int tab_y = moved_y && (!previous_y || held) && !moved_x && !previous_x;
    int key = -1;
    if (x == 0 && y == 0) {
        repeat->timed = 0; // let go: the next direction starts afresh
    } else if (tab_x || tab_y) {
        key = tab_x ? (x > 0 ? GUIDE_KEY_RIGHT : GUIDE_KEY_LEFT) : (y > 0 ? GUIDE_KEY_UP : GUIDE_KEY_DOWN);
        unsigned long at = fresh_at;
        if (!(tab_x ? !previous_x : !previous_y)) { // a repeat: given at when it was due, unless a period late
            const unsigned long late = now - repeat->start - due;
            at = late < repeat_interval && late < hold_delay ? repeat->start + due : now;
        }
        repeat->last = at;
        if (!repeat->timed) {
            repeat->start = at;
            repeat->timed = 1;
        }
    }
    repeat->previous_x = x;
    repeat->previous_y = y;
    return key;
}

// One step of event_manager_update's directions at `now`: the left stick (`stick_x`, `stick_y` as the UI reads it:
// guide_stick_axis) when it is past GUIDE_STICK_THRESHOLD, else the D-pad: a direction that went down at this step,
// or has been down for hold_delay (user_interface_controller_test_button_hold), and only one. Returns the key it gives,
// -1 none.
static int direction_step(guide_repeat *repeat, guide_dpad *dpad, long stick_x, long stick_y, unsigned long now)
{
    static const long k_vectors[k_dpad_count][2] = { { 0, 32767 }, { 0, -32767 }, { -32767, 0 }, { 32767, 0 } };
    const unsigned long hold_delay = __atomic_load_n(&g_timing.hold_delay, __ATOMIC_RELAXED);
    long x = magnitude(stick_x) >= GUIDE_STICK_THRESHOLD ? stick_x : 0;
    long y = magnitude(stick_y) >= GUIDE_STICK_THRESHOLD ? stick_y : 0;
    unsigned long fresh_at = now;
    if (x == 0 && y == 0) {
        int count = 0;
        for (int d = 0; d < k_dpad_count; d++) {
            if (!dpad->down[d] || !(dpad->fresh[d] || now - dpad->since[d] >= hold_delay))
                continue;
            count++;
            x = k_vectors[d][0];
            y = k_vectors[d][1];
            // pressed now: then; held for hold_delay since the last step (the steps between gave nothing): then is
            // when it began again; held for longer (another direction counted too until now): now, as the game
            const unsigned long held_at = dpad->since[d] + hold_delay;
            fresh_at = dpad->fresh[d]                                           ? dpad->since[d]
                       : !repeat->stepped || now - held_at < now - repeat->step ? held_at
                                                                                : now;
        }
        if (count > 1)
            x = y = 0;
    }
    for (int d = 0; d < k_dpad_count; d++)
        dpad->fresh[d] = 0;
    repeat->stepped = 1;
    repeat->step = now;
    int key = -1;
    if (x != 0 || y != 0) {
        key = repeat_step(repeat, x, y, now, fresh_at);
        repeat->active = 1;
    } else if (repeat->active) {
        repeat_step(repeat, 0, 0, now, now);
        repeat->active = 0;
    }
    return key;
}

// A stick axis as Halo 3's UI reads it (input_xinput_scale_axis): 0 within the dead zone, the rest rescaled to the
// full range.
static long guide_stick_axis(short value)
{
    const double v = value, dead_zone = GUIDE_STICK_DEAD_ZONE;
    double result = 0.0;
    if (v > dead_zone)
        result = (v - dead_zone) * 32767.0 / (32767.0 - dead_zone);
    if (v < -dead_zone)
        result = (v + dead_zone) * -32768.0 / (dead_zone - 32768.0);
    return (long)(short)result;
}

// A trigger as a button (input_xinput_update_trigger): down past a threshold that follows it (32 under a pull, 64
// over a release), so a trigger that rests partly pulled still clicks.
static int trigger_down(unsigned char *threshold, unsigned char value)
{
    const int down = value > *threshold;
    if (down) {
        const unsigned char lower = value < 32 ? 0 : (unsigned char)(value - 32);
        if (lower > *threshold)
            *threshold = lower;
    } else {
        const unsigned char upper = value > 191 ? 255 : (unsigned char)(value + 64);
        if (upper < *threshold)
            *threshold = upper;
    }
    return down;
}

// The D-pad key of a direction, and the other way round.
static int dpad_of_key(int key)
{
    switch (key) {
    case GUIDE_KEY_UP: return k_dpad_up;
    case GUIDE_KEY_DOWN: return k_dpad_down;
    case GUIDE_KEY_LEFT: return k_dpad_left;
    case GUIDE_KEY_RIGHT: return k_dpad_right;
    default: return -1;
    }
}

// ---- what a user shows as ----

static int user_on_live(unsigned long user, const port_profile_user_info *info)
{
    return user < PORT_PROFILE_USERS && info->kind == PORT_SIGNIN_PROFILE &&
           port_live_profile_identity((unsigned int)user, info->name, 0, 0, 0);
}

static int signin_user_on_live(unsigned long user)
{
    port_profile_user_info info, sponsor;
    if (!port_profile_user(user, &info))
        return 0;
    return user_on_live(user, &info) ||
           (info.kind == PORT_SIGNIN_GUEST && port_profile_user(info.sponsor, &sponsor) &&
            user_on_live(info.sponsor, &sponsor));
}

static void user_name(unsigned long user, char name[GUIDE_LABEL])
{
    port_profile_user_info info;
    if (!port_profile_user(user, &info)) {
        copy(name, GUIDE_LABEL, "Not signed in");
        return;
    }
    if (info.kind == PORT_SIGNIN_PROFILE && port_live_profile_identity((unsigned int)user, info.name, 0, name, GUIDE_LABEL))
        return;
    if (info.kind == PORT_SIGNIN_GUEST) {
        port_profile_user_info sponsor;
        char sponsor_name[PORT_PROFILE_GAMERTAG_MAX + 1];
        if (port_profile_user(info.sponsor, &sponsor) && sponsor.kind == PORT_SIGNIN_PROFILE &&
            port_live_profile_identity((unsigned int)info.sponsor, sponsor.name, 0, sponsor_name, sizeof(sponsor_name))) {
            char guest[PORT_PROFILE_GAMERTAG_MAX + 1];
            port_profile_guest_name(sponsor_name, info.guest_number, guest);
            copy(name, GUIDE_LABEL, guest);
            return;
        }
    }
    copy(name, GUIDE_LABEL, info.name);
}

// ---- the pages' items (under the lock) ----

static guide_level *pane_level(guide_pane *pane) { return &pane->stack[pane->depth - 1]; }

// One character per key, in display order. The editing keys use the same characters as their keyboard equivalents.
enum { OSK_COLUMNS = 10, OSK_ROWS = 4 };
static const char *osk_grid(const guide_pane *pane)
{
    const int page = pane->stack[pane->depth - 1].page;
    if (page == PAGE_CREATE || page == PAGE_LIVE_CONNECT)
        return pane->osk_shift ? "1234567890" "QWERTYUIOP" "ASDFGHJKL^" "ZXCVBNM \b\r"
                               : "1234567890" "qwertyuiop" "asdfghjkl^" "zxcvbnm \b\r";
    if (page == PAGE_JOIN_CODE)
        return pane->osk_shift ? "1234567890" "QWERTYUIOP" "ASDFGHJKL-" "ZXCVBNM\t\b\r"
                               : "1234567890" "qwertyuiop" "asdfghjkl-" "zxcvbnm\t\b\r";
    return 0;
}

const char *guide_osk_label(char key, int compact, char literal[2])
{
    switch (key) {
    case '^': return compact ? "Aa" : "Shift";
    case ' ': return compact ? "Sp" : "Space";
    case '\t': return compact ? "^V" : "Paste";
    case '\b': return compact ? "<-" : "Del";
    case '\r': return compact ? "OK" : "Done";
    default:
        literal[0] = key;
        literal[1] = 0;
        return literal;
    }
}

static int guests_offered(void)
{
    const guide_request *request = &g_guide.request;
    if (request->kind != GUIDE_KIND_SIGNIN)
        return 0;
    if ((request->flags & GUIDE_XSSUI_ENABLE_GUEST) != 0)
        return 1;
    if ((request->flags & GUIDE_XSSUI_CONVERTOFFLINETOGUEST) != 0)
        return 1;
    return g_guide.guest_enabled;
}

static int signin_online_only(void)
{
    return g_guide.request.kind == GUIDE_KIND_SIGNIN && (g_guide.request.flags & GUIDE_XSSUI_SHOWONLYONLINEENABLED);
}

// Online-only requests count Live users once their connections are active.
static int signin_room(void)
{
    return g_guide.request.kind != GUIDE_KIND_SIGNIN || signin_online_only() || g_guide.signins < g_guide.request.panes;
}

static void add_live_signin(unsigned long user, const port_profile_user_info *info, guide_item *items, int *count)
{
    if (!port_live_enabled() || info->kind != PORT_SIGNIN_PROFILE || user_on_live(user, info))
        return;
    guide_item *item = &items[(*count)++];
    item->id = ITEM_CONNECT_LIVE;
    copy(item->text, GUIDE_LABEL, "Sign in to SlopBoxLive");
    item->enabled = !port_live_user_connecting((unsigned int)user);
    copy(item->detail, GUIDE_LABEL, item->enabled ? "Play online with this profile" : "Signing in...");
    item->why = "Signing in to SlopBoxLive...";
    copy(item->about, sizeof(item->about), "Link this profile to SlopBoxLive to play online with your gamertag.");
}

static void signin_items(unsigned long user, guide_item *items, int *count)
{
    const guide_request *request = &g_guide.request;
    const int online_only = signin_online_only();
    const int guest_only = request->kind == GUIDE_KIND_SIGNIN && (request->flags & GUIDE_XSSUI_CONVERTOFFLINETOGUEST);
    port_profile_user_info info;
    port_profile_user(user, &info);
    if (!guest_only)
        add_live_signin(user, &info, items, count);
    const unsigned long profiles = port_profile_count();
    for (unsigned long i = 0; i < profiles && *count < GUIDE_ITEMS - 4 && !guest_only; i++) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_PROFILE;
        item->arg = i;
        port_profile_get(i, item->text, 0);
        const long on = port_profile_signed_in_on(i);
        const int live = on >= 0 && port_live_profile_identity((unsigned int)on, item->text, 0, item->text, GUIDE_LABEL);
        item->checked = on == (long)user;
        item->enabled = 1;
        if (item->checked) {
            copy(item->detail, GUIDE_LABEL, live ? "Signed in (Live)" : "Signed in");
        } else if (on >= 0) {
            copy(item->detail, GUIDE_LABEL, "Controller ");
            append_number(item->detail, GUIDE_LABEL, (unsigned long)on + 1);
            if (live)
                append(item->detail, GUIDE_LABEL, " (Live)");
            item->enabled = 0;
            item->why = "That profile is signed in on another controller.";
        }
        if (item->enabled && online_only && !live && !port_live_enabled()) {
            item->enabled = 0;
            item->why = "A Live server is required to sign in here.";
        }
        if (item->enabled && !item->checked && !signin_room()) {
            item->enabled = 0;
            item->why = "No more players can sign in now.";
        }
    }
    if (!guest_only && (!online_only || port_live_enabled())) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_CREATE;
        copy(item->text, GUIDE_LABEL, "Create Profile");
        item->enabled = port_profile_count() < PORT_PROFILE_CATALOG_MAX;
        item->why = "There is no room for another profile.";
    }
    if (guests_offered()) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_GUEST;
        copy(item->text, GUIDE_LABEL, "Sign in as Guest");
        const long sponsor = port_profile_guest_sponsor(user);
        item->enabled = sponsor >= 0;
        if (sponsor >= 0) {
            char name[GUIDE_LABEL];
            user_name((unsigned long)sponsor, name);
            copy(item->detail, GUIDE_LABEL, "of ");
            append(item->detail, GUIDE_LABEL, name);
            copy(item->about, sizeof(item->about), "Plays as a guest of ");
            append(item->about, sizeof(item->about), name);
            append(item->about, sizeof(item->about), ". Nothing a guest does is saved.");
        } else {
            item->why = "A guest needs a profile signed in on another controller.";
        }
        if (item->enabled && online_only && !signin_user_on_live((unsigned long)sponsor)) {
            item->enabled = 0;
            item->why = "A Live guest needs a sponsor signed in to Xbox Live.";
        }
        if (item->enabled && info.kind == PORT_SIGNIN_GUEST && info.sponsor == (unsigned long)sponsor) {
            item->checked = 1;
            copy(item->detail, GUIDE_LABEL, "Signed in");
        } else if (item->enabled && !signin_room()) {
            item->enabled = 0;
            item->why = "No more players can sign in now.";
        }
    }
    if (info.kind != PORT_SIGNIN_NONE) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_SIGN_OUT;
        copy(item->text, GUIDE_LABEL, "Sign Out");
        item->enabled = 1;
    }
}

// The menu's lobby items: Copy Lobby Code (while the players are in a lobby another console can join), Join by Lobby
// Code (a profile signed in to join with).
static void lobby_items(const port_profile_user_info *info, guide_item *items, int *count)
{
    const char *why = "";
    guide_item *item = &items[(*count)++];
    item->id = ITEM_LOBBY_CODE;
    copy(item->text, GUIDE_LABEL, "Copy Lobby Code");
    port_invite_lobby lobby;
    item->enabled = port_invite_current_lobby(&lobby, &why);
    item->why = why;
    if (item->enabled) {
        copy(item->detail, GUIDE_LABEL, lobby.session_class == PORT_INVITE_CLASS_LIVE ? "Live" : "System Link");
        copy(item->about, sizeof(item->about), "Copies a code that lets a friend join this lobby from their Guide.");
    }
    item = &items[(*count)++];
    item->id = ITEM_JOIN_CODE;
    copy(item->text, GUIDE_LABEL, "Join by Lobby Code");
    item->enabled = info->kind != PORT_SIGNIN_NONE;
    item->why = "Sign in to a profile first.";
    copy(item->about, sizeof(item->about), "Joins a friend's lobby with the code they copied from their Guide.");
}

// The friends list (Live) as items: Copy Lobby Code first on the Invite to Game page.
static void friend_items(int blade, guide_item *items, int *count)
{
    if (blade == GUIDE_BLADE_INVITE) {
        const char *why = "";
        guide_item *item = &items[(*count)++];
        item->id = ITEM_LOBBY_CODE;
        copy(item->text, GUIDE_LABEL, "Copy Lobby Code");
        item->enabled = port_invite_current_lobby(0, &why);
        item->why = why;
    }
    if (g_guide.friends_state != k_friends_read)
        return;
    for (unsigned long i = 0; i < g_guide.friend_count && *count < GUIDE_ITEMS; i++) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_FRIEND;
        item->arg = i;
        copy(item->text, GUIDE_LABEL, g_guide.friends[i].gamertag);
        copy(item->detail, GUIDE_LABEL, g_guide.friends[i].online ? "Online" : "Offline");
        item->enabled = 1;
    }
}

// The items of level `level` of pane `pane_index` (its top: build_items).
// ---- the Settings page (under the lock; port/sdk/runtime/port_config.h keeps the values and writes halo3.ini) ----
//
// Each row a setting with the values left / right and A step through, in order (wrapping round, as the guide's lists
// do), and the names the page shows them by (the in-game guide shows both in capitals).

#define GUIDE_SETTING_VALUES 7

typedef struct {
    int setting;   // port_setting
    const char *label;
    const char *about;  // the hint: what it does
    const char *values[GUIDE_SETTING_VALUES + 1]; // as halo3.ini has them; 0 ends
    const char *names[GUIDE_SETTING_VALUES];
} guide_setting_row;

static const guide_setting_row k_setting_rows[] = {
    { PORT_SETTING_WINDOW_MODE, "Window Mode", "Windowed, borderless (no frame, over the work area) or fullscreen.",
      { "windowed", "borderless", "fullscreen" }, { "Windowed", "Borderless", "Fullscreen" } },
    { PORT_SETTING_RESOLUTION, "Resolution", "The size the game renders at. 21:9 widens the view; menus and HUD stay 16:9.",
      { "native", "1280x720", "1920x1080", "2560x1440", "3840x2160", "2560x1080", "3440x1440" },
      { "Native (1152x640)", "720p (1280x720)", "1080p (1920x1080)", "1440p (2560x1440)", "4K (3840x2160)",
        "21:9 (2560x1080)", "21:9 (3440x1440)" } },
#ifndef HALO3_RELEASE_BUILD
    { PORT_SETTING_RES_SCALE, "Resolution Scale", "Draws the game at 1, 2 or 3 times its own resolution.",
      { "1", "2", "3" }, { "1x (Native)", "2x (1440p)", "3x (4K)" } },
#endif
    { PORT_SETTING_FPS_CAP, "FPS Cap", "Frames a second. Above 30 the frames between the game's ticks are blended.",
      { "30", "60", "120", "144", "240", "0" }, { "30", "60", "120", "144", "240", "Uncapped" } },
    { PORT_SETTING_VSYNC, "VSync", "Waits for the display's refresh. Auto: on at 30 FPS, off above.",
      { "auto", "1", "0" }, { "Auto", "On", "Off" } },
    { PORT_SETTING_HD_TEXT, "HD Text", "Use high-resolution fonts in menus and the Guide when available.",
      { "0", "1" }, { "Off", "On" } },
    { PORT_SETTING_DRAW_DISTANCE, "Draw Distance", "Extends object and decorator visibility and keeps higher object detail farther away.",
      { "1", "2", "4" }, { "Original", "High (2x)", "Ultra (4x)" } },
    { PORT_SETTING_ANISOTROPY, "Anisotropic Filtering", "Sharpens textures viewed at oblique angles.",
      { "0", "4", "8", "16" }, { "Original", "4x", "8x", "16x" } },
    { PORT_SETTING_SHADOW_RES, "Shadow Resolution", "Increases dynamic shadow map resolution.",
      { "512", "1024", "2048" }, { "Original (512)", "High (1024)", "Ultra (2048)" } },
    { PORT_SETTING_SPLITSCREEN_QUALITY, "Split-Screen Quality", "Keeps full single-player graphics detail in split-screen instead of console reductions.",
      { "0", "1" }, { "Original", "Full" } },
    { PORT_SETTING_SPLITSCREEN_SYMMETRIC_HUD, "Split-Screen HUD Margins", "Symmetric gives each player equal margins. Retail TV uses the outer edges of the display.",
      { "0", "1" }, { "Retail TV", "Symmetric" } },
    { PORT_SETTING_MIC_DEVICE, "Microphone", "Voice chat input device for multiplayer and co-op.",
      { "off", "default" }, { "Off", "System Default" } },
#ifndef HALO3_RELEASE_BUILD
    { PORT_SETTING_LAN, "System Link (LAN)", "System Link games with other PCs on this network.",
      { "1", "0" }, { "On", "Off" } },
    { PORT_SETTING_COOP_4P, "4-Player Campaign Co-op", "Allows up to 4 players in local and System Link campaign co-op.",
      { "0", "1" }, { "Off", "On" } },
#endif
};
#define GUIDE_SETTING_ROWS (sizeof(k_setting_rows) / sizeof(k_setting_rows[0]))

// The Keyboard & Mouse page's settings ([input]): what the mouse does, the crouch key's mode and the preset the
// bindings start from. The mouse decimal rows have no list: they step by tenths (decimal_setting_step).
static const guide_setting_row k_kbm_rows[] = {
    { PORT_SETTING_KBM_PRESET, "Preset",
      "Modern, or Classic (grenade on the right mouse button, zoom on the middle one). Sets every binding.",
      { "modern", "classic" }, { "Modern", "Classic" } },
    { PORT_SETTING_MOUSE_SENSITIVITY, "Mouse Sensitivity",
      "How far the view turns for the mouse's motion: 0.1 to 10.0 (raw mouse: 1.0 is 0.022 degrees a count).",
      { 0 }, { 0 } },
    { PORT_SETTING_MOUSE_ACCELERATION, "Mouse Acceleration",
      "Scales mouse look speed up with faster movement: Off (0.0) to 2.0.",
      { 0 }, { 0 } },
    { PORT_SETTING_MOUSE_ZOOM_SENSITIVITY, "Zoom Sensitivity",
      "Mouse look sensitivity multiplier while zoomed or scoped: 0.1x to 3.0x (1.0x: unchanged).",
      { 0 }, { 0 } },
    { PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY, "Vehicle Sensitivity",
      "Mouse look sensitivity multiplier while in a vehicle seat: 0.1x to 3.0x (1.0x: unchanged).",
      { 0 }, { 0 } },
#ifndef HALO3_RELEASE_BUILD
    { PORT_SETTING_RAW_MOUSE, "Raw Mouse",
      "On: the mouse turns the view by its own motion, with no acceleration or dead zone. Off: it moves a right stick.",
      { "1", "0" }, { "On", "Off" } },
#endif
    { PORT_SETTING_INVERT_MOUSE_Y, "Invert Mouse Y",
      "On: moving the mouse forward looks down. A controller keeps its profile's own setting.", { "0", "1" },
      { "Off", "On" } },
    { PORT_SETTING_CROUCH_HOLD, "Crouch Mode",
      "Hold: crouch while the key is down. Toggle: a press crouches and the next stands (so does a jump).",
      { "1", "0" }, { "Hold", "Toggle" } },
};
#define GUIDE_KBM_ROWS (sizeof(k_kbm_rows) / sizeof(k_kbm_rows[0]))
#define GUIDE_SENSITIVITY_STEP 10 // hundredths: mouse decimal settings step by 0.1

static const guide_setting_row *setting_row(int setting)
{
    for (unsigned i = 0; i < GUIDE_SETTING_ROWS; i++)
        if (k_setting_rows[i].setting == setting)
            return &k_setting_rows[i];
    for (unsigned i = 0; i < GUIDE_KBM_ROWS; i++)
        if (k_kbm_rows[i].setting == setting)
            return &k_kbm_rows[i];
    return 0;
}

static int setting_value_count(const guide_setting_row *row)
{
    int count = 0;
    while (count < GUIDE_SETTING_VALUES && row->values[count] != 0)
        count++;
    return count;
}

// The index of `value` among the row's values, -1 when it is none of them (halo3.ini may hold another: fps_cap 75).
static int setting_index(const guide_setting_row *row, const char *value)
{
    for (int i = 0; i < setting_value_count(row); i++)
        if (same_text(row->values[i], value))
            return i;
    return -1;
}

static int is_decimal_setting(int setting)
{
    return setting == PORT_SETTING_MOUSE_SENSITIVITY || setting == PORT_SETTING_MOUSE_ACCELERATION ||
           setting == PORT_SETTING_MOUSE_ZOOM_SENSITIVITY || setting == PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY;
}

static int mic_voice_disabled(void)
{
    const char *v = getenv("HALO3_VOICE");
    return v != 0 && same_text(v, "0");
}

static int mic_test_tone_active(void)
{
    const char *v = getenv("HALO3_VOICE_TEST_TONE");
    return v != 0 && same_text(v, "1");
}

static void mic_setting_detail(const char *value, char *out, unsigned long capacity)
{
    if (value[0] == 0 || same_text(value, "off")) {
        copy(out, capacity, "Off");
        return;
    }
    if (same_text(value, "default")) {
        copy(out, capacity, "System Default");
        return;
    }
    for (unsigned i = 0; i < g_guide.mic_device_count; i++) {
        if (same_text(g_guide.mic_devices[i].id, value)) {
            copy(out, capacity, g_guide.mic_devices[i].name);
            return;
        }
    }
    copy(out, capacity, value);
}

static int mic_setting_step(int step)
{
    char value[PORT_CONFIG_VALUE];
    port_config_value(PORT_SETTING_MIC_DEVICE, value, sizeof(value));
    const int count = 2 + (int)g_guide.mic_device_count;
    int index = -1;
    if (value[0] == 0 || same_text(value, "off"))
        index = 0;
    else if (same_text(value, "default"))
        index = 1;
    else {
        for (unsigned i = 0; i < g_guide.mic_device_count; i++) {
            if (same_text(g_guide.mic_devices[i].id, value)) {
                index = 2 + (int)i;
                break;
            }
        }
    }
    const int next = index < 0 ? (step > 0 ? 0 : count - 1) : ((index + step) % count + count) % count;
    const char *target = next == 0 ? "off" : next == 1 ? "default" : g_guide.mic_devices[next - 2].id;
    return port_config_set(PORT_SETTING_MIC_DEVICE, target) > 0;
}

// A mouse decimal setting in hundredths, as the game uses it now.
static unsigned long decimal_setting_now(int setting)
{
    switch (setting) {
    case PORT_SETTING_MOUSE_ACCELERATION:
        return (unsigned long)(port_config_mouse_acceleration() * 100.0f + 0.5f);
    case PORT_SETTING_MOUSE_ZOOM_SENSITIVITY:
        return (unsigned long)(port_config_mouse_zoom_sensitivity() * 100.0f + 0.5f);
    case PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY:
        return (unsigned long)(port_config_mouse_vehicle_sensitivity() * 100.0f + 0.5f);
    default:
        return (unsigned long)(port_config_mouse_sensitivity() * 100.0f + 0.5f);
    }
}

// Hundredths as halo3.ini takes them: "1.0", "2.5", "0.75".
static void sensitivity_text(unsigned long hundredths, char *out, unsigned long capacity)
{
    out[0] = 0;
    append_number(out, capacity, hundredths / 100ul);
    append(out, capacity, ".");
    append_number(out, capacity, hundredths / 10ul % 10ul);
    if (hundredths % 10ul != 0)
        append_number(out, capacity, hundredths % 10ul);
}

// How a mouse decimal setting shows on the Keyboard & Mouse page.
static void decimal_setting_detail(int setting, char *out, unsigned long capacity)
{
    const unsigned long hundredths = decimal_setting_now(setting);
    if (setting == PORT_SETTING_MOUSE_ACCELERATION && hundredths == 0ul) {
        copy(out, capacity, "Off");
        return;
    }
    sensitivity_text(hundredths, out, capacity);
    if (setting == PORT_SETTING_MOUSE_ZOOM_SENSITIVITY || setting == PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY)
        append(out, capacity, "x");
}

// A mouse decimal setting a tenth up (`step` 1) or down (-1), within its bounds (no wrap: a scale, not a list; a value
// between tenths goes to the next tenth). Returns whether it changed.
static int decimal_setting_step(int setting, int step)
{
    unsigned long min_v = 10ul, max_v = 1000ul;
    if (setting == PORT_SETTING_MOUSE_ACCELERATION) {
        min_v = 0ul;
        max_v = 200ul;
    } else if (setting == PORT_SETTING_MOUSE_ZOOM_SENSITIVITY || setting == PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY) {
        min_v = 10ul;
        max_v = 300ul;
    }
    const unsigned long now = decimal_setting_now(setting);
    unsigned long next;
    if (step > 0) {
        next = now / GUIDE_SENSITIVITY_STEP * GUIDE_SENSITIVITY_STEP + GUIDE_SENSITIVITY_STEP;
        if (next > max_v)
            next = max_v;
    } else {
        const unsigned long snapped =
            (now + GUIDE_SENSITIVITY_STEP - 1) / GUIDE_SENSITIVITY_STEP * GUIDE_SENSITIVITY_STEP;
        next = snapped > GUIDE_SENSITIVITY_STEP ? snapped - GUIDE_SENSITIVITY_STEP : 0ul;
        if (next < min_v)
            next = min_v;
    }
    char text[16];
    sensitivity_text(next, text, sizeof(text));
    return port_config_set(setting, text) > 0;
}

// Steps setting `setting` to the next (`step` 1) or previous (-1) of its row's values (one it has not: to the first,
// or the last). The keyboard preset applies its bindings with it. Returns whether it changed.
static int setting_step(int setting, int step)
{
    const guide_setting_row *row = setting_row(setting);
    if (row == 0 || port_config_locked(setting))
        return 0;
    if (setting == PORT_SETTING_MIC_DEVICE) {
        if (mic_voice_disabled() || mic_test_tone_active())
            return 0;
        return mic_setting_step(step);
    }
    if (is_decimal_setting(setting))
        return decimal_setting_step(setting, step);
    char value[PORT_CONFIG_VALUE];
    port_config_value(setting, value, sizeof(value));
    const int count = setting_value_count(row), index = setting_index(row, value);
    const int next = index < 0 ? (step > 0 ? 0 : count - 1) : ((index + step) % count + count) % count;
    if (setting == PORT_SETTING_KBM_PRESET)
        return port_config_apply_kbm_preset(port_kbm_preset_of(row->values[next], text_length(row->values[next])));
    return port_config_set(setting, row->values[next]) > 0;
}

// A setting's row: its value, whether it can change and why not, what it does.
static void setting_item(const guide_setting_row *row, guide_item *item, char *locked_why, unsigned long why_capacity)
{
    item->id = ITEM_SETTING;
    item->arg = (unsigned long)row->setting;
    copy(item->text, GUIDE_LABEL, row->label);
    char value[PORT_CONFIG_VALUE];
    port_config_value(row->setting, value, sizeof(value));
    const int index = setting_index(row, value);
    if (is_decimal_setting(row->setting))
        decimal_setting_detail(row->setting, item->detail, GUIDE_LABEL);
    else if (row->setting == PORT_SETTING_MIC_DEVICE)
        mic_setting_detail(value, item->detail, GUIDE_LABEL);
    else
        copy(item->detail, GUIDE_LABEL, index >= 0 ? row->names[index] : value);
    const int restart = port_config_restart_pending(row->setting);
    item->setting = restart ? GUIDE_SETTING_RESTART : GUIDE_SETTING_VALUE;
    item->enabled = !port_config_locked(row->setting);
    copy(locked_why, why_capacity, "Set for this launch by ");
    append(locked_why, why_capacity, port_config_variable(row->setting));
    append(locked_why, why_capacity, ": a launch option wins over the settings.");
    item->why = locked_why;
    if (row->setting == PORT_SETTING_MIC_DEVICE) {
        if (mic_voice_disabled()) {
            copy(item->detail, GUIDE_LABEL, "Off");
            item->enabled = 0;
            copy(locked_why, why_capacity, "Disabled for this launch by HALO3_VOICE=0.");
        } else if (mic_test_tone_active()) {
            copy(item->detail, GUIDE_LABEL, "Test Voice");
            item->enabled = 0;
            copy(locked_why, why_capacity, "Set for this launch by HALO3_VOICE_TEST_TONE=1.");
        }
    }
    copy(item->about, sizeof(item->about), row->about);
    append(item->about, sizeof(item->about), port_config_live(row->setting) ? " Applies now."
                                             : restart ? " Restart Halo 3 to apply the change."
                                                       : " Applies when Halo 3 starts.");
}

static void settings_items(guide_item *items, int *count)
{
    // why a setting the launch set cannot change (kept here: an item's why points at it)
    static char locked_why[GUIDE_SETTING_ROWS][GUIDE_LABEL * 2];
    for (unsigned i = 0; i < GUIDE_SETTING_ROWS && *count < GUIDE_ITEMS; i++) {
        const guide_setting_row *row = &k_setting_rows[i];
        guide_item *item = &items[(*count)++];
        setting_item(row, item, locked_why[i], sizeof(locked_why[i]));
#ifndef HALO3_RELEASE_BUILD
        // a custom resolution renders at 1x (port_config.h): the scale multiplies the native 1152x640 only
        char resolution[PORT_CONFIG_VALUE];
        port_config_value(PORT_SETTING_RESOLUTION, resolution, sizeof(resolution));
        if (row->setting == PORT_SETTING_RES_SCALE && !same_text(resolution, "native")) {
            copy(item->detail, GUIDE_LABEL, "1x (Custom Resolution)");
            item->setting = GUIDE_SETTING_VALUE;
            item->enabled = 0;
            item->why = "A custom resolution renders at 1x: the scale applies to the Native resolution.";
        }
#endif
    }
    if (*count < GUIDE_ITEMS) { // one row for the keyboard and mouse: their page has the rest
        guide_item *item = &items[(*count)++];
        item->id = ITEM_KBM_PAGE;
        copy(item->text, GUIDE_LABEL, "Keyboard & Mouse");
        copy(item->detail, GUIDE_LABEL, port_kbm_preset_label(port_config_kbm_preset()));
        item->enabled = 1;
        copy(item->about, sizeof(item->about),
             "Mouse look, the crouch key and the key bindings. A controller keeps its own button layout.");
    }
    if (*count < GUIDE_ITEMS) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_RESET;
        copy(item->text, GUIDE_LABEL, "Reset to Defaults");
        item->enabled = 1;
        copy(item->about, sizeof(item->about), "Puts the settings on this page back to their defaults.");
    }
}

// The Keyboard & Mouse page: its settings, then a row per binding (`selected`: the row selected, whose input
// `slot` shows bracketed), then Reset to MCC Defaults.
static void kbm_items(int pane_index, int selected, guide_item *items, int *count)
{
    static char locked_why[GUIDE_KBM_ROWS][GUIDE_LABEL * 2];
    const guide_pane *pane = &g_guide.panes[pane_index];
    for (unsigned i = 0; i < GUIDE_KBM_ROWS && *count < GUIDE_ITEMS; i++)
        setting_item(&k_kbm_rows[i], &items[(*count)++], locked_why[i], sizeof(locked_why[i]));
    if (*count < GUIDE_ITEMS) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_INFO;
        copy(item->text, GUIDE_LABEL, "Bindings");
        copy(item->detail, GUIDE_LABEL, "Key / Alternate");
        item->enabled = 1;
        copy(item->about, sizeof(item->about),
             "In play these keys drive Halo 3's actions directly: no controller button layout (Default, Bumper "
             "Jumper) changes them.");
    }
    port_kbm_bindings bindings;
    port_config_kbm_bindings(&bindings);
    for (int a = 0; a < PORT_KBM_ACTION_COUNT && *count < GUIDE_ITEMS; a++) {
        const int index = *count;
        guide_item *item = &items[(*count)++];
        item->id = ITEM_BINDING;
        item->arg = (unsigned long)a;
        copy(item->text, GUIDE_LABEL, port_kbm_action_label(a));
        item->setting = GUIDE_SETTING_VALUE;
        item->enabled = !port_kbm_action_fixed(a);
        item->why = "The pause menu stays on Escape, which also cancels a rebinding.";
        const unsigned char *codes = bindings.codes[a];
        if (g_guide.capture.active && g_guide.capture.pane == pane_index && g_guide.capture.action == a) {
            copy(item->detail, GUIDE_LABEL, "Press a key...");
        } else if (index == selected && item->enabled) { // both inputs, the one left / right and Enter act on bracketed
            for (int slot = 0; slot < PORT_KBM_BINDS; slot++) {
                append(item->detail, GUIDE_LABEL, slot > 0 ? " / " : "");
                append(item->detail, GUIDE_LABEL, slot == pane->kbm_slot ? "[" : "");
                append(item->detail, GUIDE_LABEL, codes[slot] != PORT_KBM_NONE ? port_kbm_code_label(codes[slot]) : "-");
                append(item->detail, GUIDE_LABEL, slot == pane->kbm_slot ? "]" : "");
            }
        } else {
            port_kbm_format_label(codes, item->detail, GUIDE_LABEL);
        }
        copy(item->about, sizeof(item->about), "Enter: press the new key or mouse button for ");
        append(item->about, sizeof(item->about), pane->kbm_slot ? "the alternate" : "this action");
        append(item->about, sizeof(item->about), ". Left / Right: the key or its alternate.");
    }
    if (*count < GUIDE_ITEMS) {
        guide_item *item = &items[(*count)++];
        item->id = ITEM_KBM_RESET;
        copy(item->text, GUIDE_LABEL, "Reset to Defaults");
        item->enabled = 1;
        copy(item->about, sizeof(item->about), "Puts the mouse settings and every binding back to ");
        append(item->about, sizeof(item->about), port_kbm_preset_label(port_config_kbm_preset()));
        append(item->about, sizeof(item->about), "'s defaults.");
    }
}

static int build_level_items(int pane_index, const guide_level *level, guide_item *items)
{
    __builtin_memset(items, 0, sizeof(guide_item) * GUIDE_ITEMS);
    guide_pane *pane = &g_guide.panes[pane_index];
    const guide_request *request = &g_guide.request;
    const unsigned long user = (unsigned long)pane_index;
    int count = 0;
    port_profile_user_info info;
    port_profile_user(user, &info);
    switch (level->page) {
    case PAGE_MENU: {
        static const struct {
            int id, blade;
            const char *text;
        } k_menu[] = {
            { ITEM_RESUME, 0, "Resume Game" },
            { ITEM_CONNECT_LIVE, 0, 0 },
            { ITEM_PROFILES, 0, "Profiles" },
            { ITEM_BLADE, GUIDE_BLADE_ACHIEVEMENTS, "Achievements" },
            { ITEM_BLADE, GUIDE_BLADE_FRIENDS, "Friends" },
            { ITEM_BLADE, GUIDE_BLADE_PLAYERS, "Recent Players" },
            { ITEM_BLADE, GUIDE_BLADE_MESSAGES, "Messages" },
            { ITEM_BLADE, GUIDE_BLADE_SETTINGS, "Settings" },
        };
        for (unsigned i = 0; i < sizeof(k_menu) / sizeof(k_menu[0]); i++) {
            if (k_menu[i].id == ITEM_CONNECT_LIVE) {
                add_live_signin(user, &info, items, &count);
                continue;
            }
            guide_item *item = &items[count++];
            item->id = k_menu[i].id;
            item->arg = (unsigned long)k_menu[i].blade;
            copy(item->text, GUIDE_LABEL, k_menu[i].text);
            item->enabled = 1;
            if (item->id == ITEM_BLADE && info.kind == PORT_SIGNIN_NONE && k_menu[i].blade != GUIDE_BLADE_SETTINGS) {
                item->enabled = 0;
                item->why = "Sign in to a profile first.";
            } else if (item->id == ITEM_BLADE && k_menu[i].blade == GUIDE_BLADE_ACHIEVEMENTS &&
                       info.kind == PORT_SIGNIN_GUEST) {
                item->enabled = 0;
                item->why = "Guests do not earn achievements.";
            } else if (item->id == ITEM_BLADE && k_menu[i].blade == GUIDE_BLADE_MESSAGES) {
                const unsigned long invites = port_invite_inbox_count();
                if (invites != 0) {
                    append_number(item->detail, GUIDE_LABEL, invites);
                    append(item->detail, GUIDE_LABEL, invites == 1 ? " game invite" : " game invites");
                }
            }
        }
        lobby_items(&info, items, &count);
        if (info.kind != PORT_SIGNIN_NONE) {
            guide_item *item = &items[count++];
            item->id = ITEM_SIGN_OUT;
            copy(item->text, GUIDE_LABEL, "Sign Out");
            user_name(user, item->detail);
            item->enabled = 1;
        }
        {
            guide_item *item = &items[count++];
            item->id = ITEM_CLOSE_GAME;
            copy(item->text, GUIDE_LABEL, "Close Game");
            item->enabled = 1;
        }
        break;
    }
    case PAGE_SIGNIN:
        signin_items(user, items, &count);
        break;
    case PAGE_CREATE:
    case PAGE_LIVE_CONNECT: {
        const int create = level->page == PAGE_CREATE;
        const int online = port_live_enabled();
        guide_item *item = &items[count++];
        item->id = create ? ITEM_CREATE_CONFIRM : ITEM_CONNECT_LIVE_CONFIRM;
        copy(item->text, GUIDE_LABEL, create ? "Create and Sign In" : "Sign in to SlopBoxLive");
        if (create && online)
            copy(item->detail, GUIDE_LABEL, "SlopBoxLive");
        item->enabled = port_profile_gamertag_valid(pane->field) &&
                        (create || !port_live_user_connecting((unsigned int)user));
        item->why = "Use 1 to 15 letters, digits or spaces. Start with a letter.";
        if (create && online && !signin_online_only()) {
            guide_item *offline = &items[count++];
            offline->id = ITEM_CREATE_OFFLINE;
            copy(offline->text, GUIDE_LABEL, "Offline Only");
            copy(offline->detail, GUIDE_LABEL, "Create on this device");
            offline->enabled = item->enabled;
            offline->why = item->why;
        }
        break;
    }
    case PAGE_MESSAGE_BOX:
        for (unsigned long i = 0; i < request->button_count && i < GUIDE_BUTTONS; i++) {
            guide_item *item = &items[count++];
            item->id = ITEM_BUTTON;
            item->arg = i;
            copy(item->text, GUIDE_LABEL, request->buttons[i]);
            item->enabled = 1;
        }
        break;
    case PAGE_DEVICES:
        for (unsigned long i = 0; i < request->device_count && i < GUIDE_DEVICES; i++) {
            guide_item *item = &items[count++];
            item->id = ITEM_DEVICE;
            item->arg = i;
            copy(item->text, GUIDE_LABEL, request->devices[i].name);
            append_size(item->detail, GUIDE_LABEL, request->devices[i].free_bytes);
            append(item->detail, GUIDE_LABEL, " free");
            item->enabled = request->devices[i].selectable;
            item->why = "That device does not have enough free space.";
        }
        break;
    case PAGE_BLADE:
        if (level->blade == GUIDE_BLADE_ACHIEVEMENTS && achievements_current(pane_index)) {
            for (unsigned long i = 0; i < g_guide.achievement_count && count < GUIDE_ITEMS; i++) {
                guide_item *item = &items[count++];
                item->id = ITEM_INFO;
                item->image_id = g_guide.achievements[i].image_id;
                copy(item->text, GUIDE_LABEL, g_guide.achievements[i].label);
                append_number(item->detail, GUIDE_LABEL, g_guide.achievements[i].gamerscore);
                append(item->detail, GUIDE_LABEL, " G");
                item->checked = g_guide.achievements[i].earned;
                item->enabled = 1;
            }
        } else if (level->blade == GUIDE_BLADE_FRIENDS || level->blade == GUIDE_BLADE_INVITE) {
            friend_items(level->blade, items, &count);
        } else if (level->blade == GUIDE_BLADE_MESSAGES) {
            g_inbox_count = port_invite_inbox(g_inbox, PORT_INVITE_INBOX);
            const port_invite *inbox = g_inbox;
            for (unsigned long i = 0; i < g_inbox_count; i++) {
                guide_item *item = &items[count++];
                item->id = ITEM_INVITE;
                item->arg = i;
                copy(item->text, GUIDE_LABEL, inbox[i].inviter[0] != 0 ? inbox[i].inviter : "A friend");
                copy(item->detail, GUIDE_LABEL, "Game Invite");
                item->enabled = 1;
                copy(item->about, sizeof(item->about), "Opens the invite to accept or decline it.");
            }
        } else if (level->blade == GUIDE_BLADE_SETTINGS) {
            settings_items(items, &count);
        } else if (level->blade == GUIDE_BLADE_REVIEW) {
            for (unsigned long i = 0; i < GUIDE_REVIEW_CHOICES; i++) {
                guide_item *item = &items[count++];
                item->id = ITEM_REVIEW;
                item->arg = i;
                copy(item->text, GUIDE_LABEL, k_review_choices[i]);
                item->enabled = 1;
            }
        }
        break;
    case PAGE_LOBBY_CODE: {
        guide_item *item = &items[count++];
        item->id = ITEM_COPY;
        copy(item->text, GUIDE_LABEL, "Copy to Clipboard");
        item->enabled = pane->code[0] != 0;
        item->why = "The lobby code is being made.";
        break;
    }
    case PAGE_JOIN_CODE: {
        // Join only with a code that reads (whether this user can join it is checked on Join: that may need Live)
        port_invite invite;
        const int result = port_invite_code_decode(pane->field, &invite);
        guide_item *item = &items[count++];
        item->id = ITEM_JOIN;
        copy(item->text, GUIDE_LABEL, "Join Lobby");
        item->enabled = result == PORT_INVITE_CODE_OK;
        item->why = port_invite_code_message(result);
        if (item->enabled) {
            copy(item->about, sizeof(item->about), invite.inviter[0] != 0 ? invite.inviter : "A friend");
            append(item->about, sizeof(item->about), "'s ");
            append(item->about, sizeof(item->about),
                   invite.session_class == PORT_INVITE_CLASS_LIVE ? "Live lobby." : "System Link lobby.");
        }
        item = &items[count++];
        item->id = ITEM_PASTE;
        copy(item->text, GUIDE_LABEL, "Paste");
        copy(item->detail, GUIDE_LABEL, "Ctrl+V");
        item->enabled = 1;
        copy(item->about, sizeof(item->about), "Puts the clipboard's text in the field.");
        break;
    }
    case PAGE_INVITE: {
        guide_item *item = &items[count++];
        item->id = ITEM_ACCEPT;
        copy(item->text, GUIDE_LABEL, "Accept");
        item->enabled = 1;
        copy(item->about, sizeof(item->about), "Joins their lobby.");
        item = &items[count++];
        item->id = ITEM_DECLINE;
        copy(item->text, GUIDE_LABEL, "Decline");
        item->enabled = 1;
        break;
    }
    case PAGE_FRIEND: {
        const char *why = "";
        guide_item *item = &items[count++];
        item->id = ITEM_SEND_INVITE;
        copy(item->text, GUIDE_LABEL, "Invite to Game");
        item->enabled = port_invite_current_lobby(0, &why);
        item->why = why;
        break;
    }
    case PAGE_KBM:
        kbm_items(pane_index, level->selected, items, &count);
        break;
    }
    return count;
}

static int build_items(int pane_index, guide_item *items)
{
    return build_level_items(pane_index, pane_level(&g_guide.panes[pane_index]), items);
}

// ---- LB / RB: the blades the menu leads to (under the lock) ----
//
// On a blade the menu leads to (Achievements, Friends, Recent Players, Messages, Settings, opened from the menu), LB and
// RB go to the one before or after it in the menu's order, wrapping round as the menu's list does and passing over those
// the menu cannot open now (no profile signed in, a guest's achievements), as the console's Guide moved between its
// blades.
// The menu's selection follows, so B goes back to the menu on the blade last shown.

static guide_item g_menu_items[GUIDE_ITEMS]; // the menu's items for tab_target (g_items holds the page's meanwhile)

// The menu item index of the blade `step` (-1, 1) from pane `pane_index`'s page, -1 when LB / RB switch nothing there.
static int tab_target(int pane_index, int step)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    if (pane->depth < 2 || pane_level(pane)->page != PAGE_BLADE || pane->stack[pane->depth - 2].page != PAGE_MENU)
        return -1;
    const int blade = pane_level(pane)->blade;
    const int count = build_level_items(pane_index, &pane->stack[pane->depth - 2], g_menu_items);
    int current = -1;
    for (int i = 0; i < count && current < 0; i++)
        if (g_menu_items[i].id == ITEM_BLADE && (int)g_menu_items[i].arg == blade)
            current = i;
    if (current < 0)
        return -1;
    for (int k = 1; k < count; k++) {
        const int i = ((current + step * k) % count + count) % count;
        if (g_menu_items[i].id == ITEM_BLADE && g_menu_items[i].enabled)
            return i;
    }
    return -1;
}

// The blades LB / RB switch between from pane `pane_index`'s page (tab_target's), in the menu's order, for its view.
static void tab_list(int pane_index, guide_view_pane *view)
{
    view->tab_count = 0;
    view->tab_current = -1;
    if (tab_target(pane_index, 1) < 0)
        return; // (g_menu_items: the menu's items, built by tab_target)
    const int blade = pane_level(&g_guide.panes[pane_index])->blade;
    for (int i = 0; i < GUIDE_ITEMS && view->tab_count < GUIDE_TABS; i++) {
        const guide_item *item = &g_menu_items[i];
        if (item->id != ITEM_BLADE || !(item->enabled || (int)item->arg == blade))
            continue;
        if ((int)item->arg == blade)
            view->tab_current = view->tab_count;
        copy(view->tabs[view->tab_count++], GUIDE_LABEL, k_blade_tabs[item->arg]);
    }
}

// ---- the pages' text (under the lock) ----

static unsigned long achievements_earned(unsigned long *gamerscore, unsigned long *total)
{
    unsigned long earned = 0;
    *gamerscore = *total = 0;
    for (unsigned long i = 0; i < g_guide.achievement_count; i++) {
        *total += g_guide.achievements[i].gamerscore;
        if (g_guide.achievements[i].earned) {
            earned++;
            *gamerscore += g_guide.achievements[i].gamerscore;
        }
    }
    return earned;
}

// The local user whose data `xuid` names, -1 for none.
static long local_user_of(unsigned long long xuid)
{
    for (unsigned long user = 0; user < PORT_PROFILE_USERS; user++)
        if (xuid != 0 && port_profile_signed_in(user) && port_profile_user_owns_xuid(user, xuid))
            return (long)user;
    return -1;
}

static void blade_text(int pane_index, int blade, guide_view_pane *view)
{
    const guide_request *request = &g_guide.request;
    const int live = port_live_enabled() && port_live_signed_in();
    switch (blade) {
    case GUIDE_BLADE_ACHIEVEMENTS:
        if (!achievements_current(pane_index)) {
            copy(view->body, GUIDE_TEXT, "Reading achievements...");
        } else if (g_guide.achievement_count == 0) {
            copy(view->body, GUIDE_TEXT, "This game's achievements are not available.");
        } else {
            unsigned long gamerscore, total;
            const unsigned long earned = achievements_earned(&gamerscore, &total);
            append_number(view->body, GUIDE_TEXT, earned);
            append(view->body, GUIDE_TEXT, " of ");
            append_number(view->body, GUIDE_TEXT, g_guide.achievement_count);
            append(view->body, GUIDE_TEXT, " achievements unlocked, ");
            append_number(view->body, GUIDE_TEXT, gamerscore);
            append(view->body, GUIDE_TEXT, " of ");
            append_number(view->body, GUIDE_TEXT, total);
            append(view->body, GUIDE_TEXT, " G");
        }
        break;
    case GUIDE_BLADE_FRIENDS:
    case GUIDE_BLADE_INVITE: {
        static const char k_code[] = "Share your lobby with a lobby code instead: it needs no server.";
        if (!live) {
            copy(view->body, GUIDE_TEXT, "Friends need a Live server. ");
            append(view->body, GUIDE_TEXT, k_code);
        } else if (g_guide.friends_state == k_friends_unsupported) {
            copy(view->body, GUIDE_TEXT, "This Live server keeps no friends list. ");
            append(view->body, GUIDE_TEXT, k_code);
        } else if (g_guide.friends_state == k_friends_failed) {
            copy(view->body, GUIDE_TEXT, "The Live server could not be reached. ");
            append(view->body, GUIDE_TEXT, k_code);
        } else if (g_guide.friends_state != k_friends_read) {
            copy(view->body, GUIDE_TEXT, "Reading friends...");
        } else if (g_guide.friend_count == 0) {
            copy(view->body, GUIDE_TEXT, "No friends yet. ");
            append(view->body, GUIDE_TEXT, k_code);
        } else {
            copy(view->body, GUIDE_TEXT, "Choose a friend to invite to your lobby.");
        }
        break;
    }
    case GUIDE_BLADE_MESSAGES: {
        const unsigned long invites = port_invite_inbox_count();
        if (invites != 0) {
            append_number(view->body, GUIDE_TEXT, invites);
            append(view->body, GUIDE_TEXT, invites == 1 ? " game invite. " : " game invites. ");
            append(view->body, GUIDE_TEXT, "Choose one to accept or decline it.");
        } else if (live && port_live_invites_supported()) {
            copy(view->body, GUIDE_TEXT, "No messages. Game invites from your friends arrive here.");
        } else {
            copy(view->body, GUIDE_TEXT, live ? "No messages. This Live server delivers no game invites."
                                              : "No messages. Game invites arrive here when signed in to Live.");
            append(view->body, GUIDE_TEXT, "\nA lobby code needs no server: Guide > Join by Lobby Code.");
        }
        break;
    }
    case GUIDE_BLADE_PLAYERS:
        copy(view->body, GUIDE_TEXT, "No recent players.");
        break;
    case GUIDE_BLADE_SETTINGS:
        if (port_config_path()[0] != 0) {
            copy(view->body, GUIDE_TEXT, "Saved as you change them, to ");
            append(view->body, GUIDE_TEXT, port_config_path());
        } else {
            copy(view->body, GUIDE_TEXT, "This launch has no halo3.ini (a script set HALO3_WINDOW, or HALO3_CONFIG names "
                                         "none): changes last until the game closes.");
        }
        break;
    case GUIDE_BLADE_COMPOSE:
        copy(view->body, GUIDE_TEXT, "Sending messages is not available.");
        break;
    case GUIDE_BLADE_FRIEND_REQUEST:
        copy(view->body, GUIDE_TEXT, "Friend requests are not available.");
        break;
    case GUIDE_BLADE_GAMERCARD:
    case GUIDE_BLADE_REVIEW: {
        const long user = local_user_of(request->xuid);
        char name[GUIDE_LABEL] = "Unknown gamer";
        if (user >= 0)
            user_name((unsigned long)user, name);
        copy(view->body, GUIDE_TEXT, "Gamertag: ");
        append(view->body, GUIDE_TEXT, name);
        append(view->body, GUIDE_TEXT, "\nXUID: ");
        append_hex(view->body, GUIDE_TEXT, request->xuid);
        if (blade == GUIDE_BLADE_GAMERCARD && achievements_current(user)) {
            unsigned long gamerscore, total;
            achievements_earned(&gamerscore, &total);
            append(view->body, GUIDE_TEXT, "\nGamerscore: ");
            append_number(view->body, GUIDE_TEXT, gamerscore);
            append(view->body, GUIDE_TEXT, " G");
        }
        if (blade == GUIDE_BLADE_GAMERCARD && user < 0)
            append(view->body, GUIDE_TEXT, "\nThis gamer's card is kept online and is not available here.");
        if (blade == GUIDE_BLADE_REVIEW)
            append(view->body, GUIDE_TEXT, "\nHow was playing with this gamer?");
        break;
    }
    default:
        break;
    }
}

static void pane_view(int pane_index, guide_view_pane *view)
{
    __builtin_memset(view, 0, sizeof(*view));
    guide_pane *pane = &g_guide.panes[pane_index];
    const guide_level *level = pane_level(pane);
    const guide_request *request = &g_guide.request;
    copy(view->heading, GUIDE_LABEL, "Controller ");
    append_number(view->heading, GUIDE_LABEL, (unsigned long)pane_index + 1);
    user_name((unsigned long)pane_index, view->status);
    guide_item *items = g_items;
    view->item_count = build_items(pane_index, items);
    for (int i = 0; i < view->item_count; i++) {
        view->items[i].image_id = items[i].image_id;
        copy(view->items[i].text, GUIDE_LABEL, items[i].text);
        copy(view->items[i].detail, GUIDE_LABEL, items[i].detail);
        view->items[i].enabled = (unsigned char)items[i].enabled;
        view->items[i].checked = (unsigned char)items[i].checked;
        view->items[i].name =
            (unsigned char)(items[i].id == ITEM_PROFILE || items[i].id == ITEM_FRIEND || items[i].id == ITEM_INVITE);
        view->items[i].setting = (unsigned char)items[i].setting;
    }
    view->selected = view->item_count > 0 ? level->selected : -1;
    view->focused = pane_index == g_guide.focus;
    view->depth = pane->depth - 1;
    tab_list(pane_index, view);
    switch (level->page) {
    case PAGE_MENU:
        copy(view->title, GUIDE_LABEL, "Guide");
        break;
    case PAGE_SIGNIN:
        copy(view->title, GUIDE_LABEL, "Sign In");
        copy(view->body, GUIDE_TEXT, "Choose a profile for this controller's player.");
        break;
    case PAGE_CREATE:
        copy(view->title, GUIDE_LABEL, "Create Profile");
        copy(view->body, GUIDE_TEXT, "Type a gamertag: letters, digits and spaces, starting with a letter.");
        copy(view->field, GUIDE_FIELD, pane->field);
        view->has_field = 1;
        break;
    case PAGE_LIVE_CONNECT:
        copy(view->title, GUIDE_LABEL, "Choose an online gamertag");
        copy(view->body, GUIDE_TEXT, "This gamertag will be linked to your current profile.");
        copy(view->field, GUIDE_FIELD, pane->field);
        view->has_field = 1;
        break;
    case PAGE_LOBBY_CODE:
        copy(view->title, GUIDE_LABEL, "Lobby Code");
        if (pane->code[0] == 0) {
            copy(view->body, GUIDE_TEXT, "Making the lobby code...");
        } else {
            copy(view->body, GUIDE_TEXT, "A friend enters this code in their Guide (Join by Lobby Code) to join your ");
            append(view->body, GUIDE_TEXT, pane->code_class == PORT_INVITE_CLASS_LIVE ? "Live" : "System Link");
            append(view->body, GUIDE_TEXT, " lobby.");
            port_invite_code_lines(pane->code, 6, view->field, GUIDE_FIELD);
            view->has_field = view->field_readonly = 1;
        }
        break;
    case PAGE_JOIN_CODE:
        copy(view->title, GUIDE_LABEL, "Join by Lobby Code");
        copy(view->body, GUIDE_TEXT, "Paste (Ctrl+V) or type the lobby code a friend copied from their Guide.");
        copy(view->field, GUIDE_FIELD, pane->field);
        view->has_field = 1;
        break;
    case PAGE_INVITE:
        copy(view->title, GUIDE_LABEL, "Game Invite");
        copy(view->body, GUIDE_TEXT, pane->invite.inviter[0] != 0 ? pane->invite.inviter : "A friend");
        append(view->body, GUIDE_TEXT, " invited you to play Halo 3 in their ");
        append(view->body, GUIDE_TEXT, pane->invite.session_class == PORT_INVITE_CLASS_LIVE ? "Live" : "System Link");
        append(view->body, GUIDE_TEXT, " lobby.");
        break;
    case PAGE_FRIEND:
        copy(view->title, GUIDE_LABEL, pane->friend_.gamertag);
        copy(view->body, GUIDE_TEXT, pane->friend_.online ? "Online." : "Offline.");
        append(view->body, GUIDE_TEXT, " An invite takes them to the lobby you are in.");
        break;
    case PAGE_MESSAGE_BOX:
        copy(view->title, GUIDE_LABEL, request->title);
        copy(view->body, GUIDE_TEXT, request->text);
        break;
    case PAGE_DEVICES:
        copy(view->title, GUIDE_LABEL, request->title[0] != 0 ? request->title : "Select Storage Device");
        copy(view->body, GUIDE_TEXT, "Choose where to save this game's data.");
        break;
    case PAGE_BLADE:
        copy(view->title, GUIDE_LABEL, k_blade_titles[level->blade]);
        blade_text(pane_index, level->blade, view);
        break;
    }
    if (level->page == PAGE_KBM) {
        copy(view->title, GUIDE_LABEL, "Keyboard & Mouse");
        blade_text(pane_index, GUIDE_BLADE_SETTINGS, view); // where they are saved (the Bindings row tells the rest)
    }
    // the hint: the selected item's description or why it cannot be chosen, the last notice, the keys
    if (view->selected >= 0 && view->selected < view->item_count) {
        const guide_item *item = &items[view->selected];
        if (item->enabled && item->about[0] != 0)
            copy(view->hint, sizeof(view->hint), item->about);
        else if (!item->enabled && item->why != 0)
            copy(view->hint, sizeof(view->hint), item->why);
        else if (level->page == PAGE_BLADE && level->blade == GUIDE_BLADE_ACHIEVEMENTS &&
                 (unsigned long)view->selected < g_guide.achievement_count) {
            const guide_achievement *achievement = &g_guide.achievements[view->selected];
            copy(view->hint, sizeof(view->hint), achievement->earned ? "Unlocked. " : "Locked. ");
            append(view->hint, sizeof(view->hint), achievement->description);
        }
    }
    if (g_guide.capture.active && g_guide.capture.pane == pane_index) { // the Keyboard & Mouse page waits for an input
        copy(view->hint, sizeof(view->hint), "Press a key or mouse button for ");
        append(view->hint, sizeof(view->hint), port_kbm_action_label(g_guide.capture.action));
        append(view->hint, sizeof(view->hint), g_guide.capture.slot ? " (alternate)" : "");
        append(view->hint, sizeof(view->hint), " (Esc to cancel, Delete to unbind)...");
    }
    if (pane->notice[0] != 0 && !same_text(view->hint, pane->notice)) { // (A on a disabled item: its why, once)
        if (view->hint[0] != 0)
            append(view->hint, sizeof(view->hint), "\n");
        append(view->hint, sizeof(view->hint), pane->notice);
    }
    const char *grid = osk_grid(pane);
    if (grid) {
        char help[sizeof(view->hint)], literal[2];
        copy(help, sizeof(help), pane->notice[0] ? pane->notice : view->hint);
        if (help[0])
            append(help, sizeof(help), "\n");
        append(help, sizeof(help), "A: ");
        append(help, sizeof(help), guide_osk_label(grid[pane->osk_key], 0, literal));
        append(help, sizeof(help), "   X: Delete   Y: Shift   Enter/Start: Done");
        if (level->page == PAGE_CREATE && port_live_enabled() && !signin_online_only())
            append(help, sizeof(help), "   RB: Offline Only");
        copy(view->hint, sizeof(view->hint), help);
    }
}

static void view_build(guide_view *view, const char **grids, int *selected)
{
    view->open = g_guide.open;
    view->session = g_guide.session;
    view->revision = g_guide.revision;
    view->page_serial = g_guide.page_serial;
    view->pane_count = 0;
    view->caption[0] = 0;
    if (!g_guide.open)
        return;
    copy(view->caption, GUIDE_LABEL, "System Guide");
    for (int i = 0; i < GUIDE_PANES; i++) {
        if (!g_guide.panes[i].active)
            continue;
        const int index = view->pane_count++;
        pane_view(i, &view->panes[index]);
        if (grids) {
            grids[index] = osk_grid(&g_guide.panes[i]);
            selected[index] = g_guide.panes[i].osk_key;
            if (grids[index] && !g_guide.panes[i].osk_menu_focus)
                view->panes[index].selected = -1;
        }
    }
}

// Host renderers take the field, immutable grid and cursor in one snapshot. The game's guide_view ABI stays fixed.
int guide_view_get_osk(guide_view *view, const char **grids, int *selected)
{
    guide_lock();
    view_build(view, grids, selected);
    guide_unlock();
    return OSK_COLUMNS;
}

PORT_MS_ABI guide_view_word guide_view_get(guide_view *view)
{
    guide_view_get_osk(view, 0, 0);
    return view->revision;
}

unsigned long guide_revision(void) { return __atomic_load_n(&g_guide.revision, __ATOMIC_ACQUIRE); }

unsigned long guide_page_serial(void) { return __atomic_load_n(&g_guide.page_serial, __ATOMIC_ACQUIRE); }

PORT_MS_ABI int guide_showing(void) { return __atomic_load_n(&g_guide.open, __ATOMIC_ACQUIRE); }

unsigned long guide_showing_session(void)
{
    guide_lock();
    const unsigned long session = g_guide.open ? g_guide.session : 0;
    guide_unlock();
    return session;
}

// ---- acting (under the lock) ----

static void changed(guide_effects *fx)
{
    __atomic_store_n(&g_guide.revision, g_guide.revision + 1, __ATOMIC_RELEASE);
    // a pane opened, closed or on another page: what a host drew of the old pages is no longer there
    int pages_changed = 0;
    for (int i = 0; i < GUIDE_PANES; i++) {
        const guide_pane *pane = &g_guide.panes[i];
        const int active = g_guide.open && pane->active && pane->depth > 0;
        const guide_level *top = active ? &pane->stack[pane->depth - 1] : 0;
        const int depth = active ? pane->depth : 0, page = top ? top->page : -1, blade = top ? top->blade : -1;
        if (g_guide.pages[i].active != active || g_guide.pages[i].depth != depth || g_guide.pages[i].page != page ||
            g_guide.pages[i].blade != blade) {
            g_guide.pages[i].active = active;
            g_guide.pages[i].depth = depth;
            g_guide.pages[i].page = page;
            g_guide.pages[i].blade = blade;
            pages_changed = 1;
        }
    }
    if (pages_changed)
        __atomic_store_n(&g_guide.page_serial, g_guide.page_serial + 1, __ATOMIC_RELEASE);
    fx->changed = 1;
}

static void end_request(guide_effects *fx, int result, unsigned long value)
{
    if (!g_guide.open)
        return;
    g_guide.capture.active = 0;
    __atomic_store_n(&g_guide.open, 0, __ATOMIC_RELEASE);
    fx->ended = 1;
    fx->result = result;
    fx->value = value;
    fx->finished = g_guide.request;
    changed(fx);
}

static void push_page(int pane_index, int page, int blade, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    if (pane->depth == GUIDE_STACK)
        return;
    guide_level *level = &pane->stack[pane->depth++];
    level->page = page;
    level->blade = blade;
    level->selected = 0;
    pane->notice[0] = 0;
    pane->awaiting_live = 0;
    pane->kbm_slot = 0;
    pane->osk_key = OSK_COLUMNS; // Q, at the start of the first letter row
    pane->osk_shift = page == PAGE_JOIN_CODE;
    pane->osk_menu_focus = 0;
    if (page == PAGE_SIGNIN) {
        // start on the profile the user has, else on the first that can be chosen
        const int count = build_items(pane_index, g_items);
        int chosen = -1, first_enabled = -1;
        for (int i = 0; i < count; i++) {
            if (g_items[i].checked && chosen < 0)
                chosen = i;
            if (g_items[i].enabled && first_enabled < 0)
                first_enabled = i;
        }
        level->selected = chosen >= 0 ? chosen : first_enabled >= 0 ? first_enabled : 0;
        const unsigned int user = (unsigned int)pane_index;
        port_live_auth_result result;
        if (port_live_user_auth_result(user, port_live_user_generation(user), &result) &&
            result.status == PORT_LIVE_AUTH_RENAMED_SUFFIX)
            copy(pane->notice, sizeof(pane->notice), result.detail);
    }
    if (page == PAGE_CREATE)
        port_profile_suggest_gamertag(pane->field);
    if (page == PAGE_LIVE_CONNECT) {
        port_profile_user_info profile;
        port_profile_user((unsigned long)pane_index, &profile);
        copy(pane->field, sizeof(pane->field), profile.name);
    }
    if (page == PAGE_JOIN_CODE)
        pane->field[0] = 0;
    if (page == PAGE_LOBBY_CODE) { // made (and copied) once the lock is released
        pane->code[0] = 0;
        fx->code_pane = pane_index;
        fx->session = g_guide.session;
    }
    if (page == PAGE_BLADE && (blade == GUIDE_BLADE_FRIENDS || blade == GUIDE_BLADE_INVITE) && port_live_signed_in() &&
        (g_guide.friends_state == k_friends_none || g_guide.friends_state == k_friends_failed)) {
        g_guide.friends_state = k_friends_reading;
        fx->load_friends = 1;
        fx->session = g_guide.session;
    }
    if (page == PAGE_BLADE && blade == GUIDE_BLADE_SETTINGS) {
        fx->load_mics = 1;
        fx->session = g_guide.session;
    }
    if ((page == PAGE_BLADE && (blade == GUIDE_BLADE_ACHIEVEMENTS || blade == GUIDE_BLADE_GAMERCARD))) {
        const long user = blade == GUIDE_BLADE_ACHIEVEMENTS ? pane_index : local_user_of(g_guide.request.xuid);
        if (user >= 0 && !achievements_current(user)) {
            fx->load_achievements = (int)user;
            fx->session = g_guide.session;
        }
    }
    changed(fx);
}

static int online_signin_complete(const guide_request *request)
{
    unsigned long online = 0, count = 0;
    for (unsigned long user = 0; user < PORT_PROFILE_USERS; user++) {
        if ((request->panes != 1 || user == request->user) && signin_user_on_live(user)) {
            online |= 1ul << user;
            count++;
        }
    }
    return count >= request->panes || (request->connected != 0 && (online & request->connected) == request->connected);
}

// Whether a sign-in request is answered: as many users signed in through it as it has panes (XShowSigninUI's cPanes),
// or every controller connected has its user signed in.
static int signin_complete(void)
{
    if (signin_online_only())
        return online_signin_complete(&g_guide.request);
    if (g_guide.request.panes != 0 && g_guide.signins >= g_guide.request.panes)
        return 1;
    for (unsigned long user = 0; user < PORT_PROFILE_USERS; user++)
        if ((g_guide.request.connected & (1ul << user)) != 0 && !port_profile_signed_in(user))
            return 0;
    return 1;
}

// After `user` signed in through pane `pane_index`: the notice, and the page or request that this ends.
static void show_signed_in(int pane_index, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    pane->awaiting_live = 0;
    const int online = signin_user_on_live((unsigned long)pane_index);
    char name[GUIDE_LABEL];
    user_name((unsigned long)pane_index, name);
    copy(pane->notice, sizeof(pane->notice), online ? "Signed in to SlopBoxLive as " : "Signed in as ");
    append(pane->notice, sizeof(pane->notice), name);
    append(pane->notice, sizeof(pane->notice), ".");
    while (pane->depth > 1 && pane_level(pane)->page != PAGE_SIGNIN)
        pane->depth--; // back from the create page
    if (g_guide.request.kind == GUIDE_KIND_SIGNIN) {
        if (signin_complete()) {
            if (online)
                guide_toast_post(GUIDE_TOAST_SIGNIN, "Signed in to SlopBoxLive as", name, "", "");
            end_request(fx, GUIDE_RESULT_CHOSEN, 0);
        }
    } else if (pane->depth > 1 && (!port_live_enabled() || online)) {
        pane->depth--; // from Profiles back to the menu
    }
    changed(fx);
}

static void signed_in(int pane_index, unsigned long bits, guide_effects *fx)
{
    signin_changed(bits);
    if (bits != 0)
        g_guide.signins++;
    show_signed_in(pane_index, fx);
}

// An action that failed: its notice, and Halo 3's error sound.
static void error_notice(guide_pane *pane, const char *what, unsigned long error)
{
    guide_sound_post(GUIDE_SOUND_ERROR);
    copy(pane->notice, sizeof(pane->notice), what);
    switch (error) {
    case 85: append(pane->notice, sizeof(pane->notice), ": that profile is signed in on another controller."); break;
    case 68:
        append(pane->notice, sizeof(pane->notice),
               what[0] == 'C' && what[7] == 'c' ? ": maximum number of local profiles reached."
                                                : ": that profile has 3 guests already.");
        break;
    case 183: append(pane->notice, sizeof(pane->notice), ": a profile has that gamertag already."); break;
    case 123: append(pane->notice, sizeof(pane->notice), ": that gamertag cannot be used."); break;
    case 1317: append(pane->notice, sizeof(pane->notice), ": no profile is signed in to sponsor a guest."); break;
    default:
        append(pane->notice, sizeof(pane->notice), " (error ");
        append_number(pane->notice, sizeof(pane->notice), error);
        append(pane->notice, sizeof(pane->notice), ").");
        break;
    }
}

// The settings changed (under the lock): halo3.ini is written once the lock is released (port_config_save, on the
// worker: it flushes a file), and the page shows the new values.
static void settings_changed(int pane_index, guide_effects *fx)
{
    g_guide.panes[pane_index].notice[0] = 0;
    fx->save_settings = 1;
    fx->session = g_guide.session;
    changed(fx);
}

static void settings_reset(int pane_index, guide_effects *fx)
{
    int any = 0;
    for (unsigned i = 0; i < GUIDE_SETTING_ROWS; i++) {
        const int setting = k_setting_rows[i].setting;
        if (!port_config_locked(setting)) {
            const int did = port_config_set(setting, port_config_default(setting)) > 0;
            any |= did;
            if (did && setting == PORT_SETTING_MIC_DEVICE)
                fx->apply_mic = 1;
        }
    }
    if (any)
        settings_changed(pane_index, fx);
    copy(g_guide.panes[pane_index].notice, sizeof(g_guide.panes[pane_index].notice),
         any ? "The settings are back to their defaults." : "The settings are at their defaults already.");
    changed(fx);
}

static void back(int pane_index, guide_effects *fx);

static void choose(int pane_index, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    guide_level *level = pane_level(pane);
    const int count = build_items(pane_index, g_items);
    if (level->selected < 0 || level->selected >= count)
        return;
    const guide_item chosen = g_items[level->selected]; // copied: acting may build another page's items
    const guide_item *item = &chosen;
    if (!item->enabled) { // handle_focused_widget_selected: a disabled item plays the error sound
        guide_sound_post(GUIDE_SOUND_ERROR);
        copy(pane->notice, sizeof(pane->notice), item->why != 0 ? item->why : "That cannot be chosen now.");
        changed(fx);
        return;
    }
    guide_sound_post(GUIDE_SOUND_A_BUTTON);
    const unsigned long user = (unsigned long)pane_index;
    unsigned long bits = 0, error = 0;
    switch (item->id) {
    case ITEM_RESUME:
        end_request(fx, GUIDE_RESULT_CHOSEN, 0);
        break;
    case ITEM_PROFILES:
        push_page(pane_index, PAGE_SIGNIN, 0, fx);
        break;
    case ITEM_BLADE:
        push_page(pane_index, PAGE_BLADE, (int)item->arg, fx);
        break;
    case ITEM_SIGN_OUT: {
        char name[GUIDE_LABEL];
        user_name(user, name);
        error = port_profile_sign_out(user, &bits); // also clears Live and cancels this profile's connect
        signin_changed(bits);
        copy(pane->notice, sizeof(pane->notice), name);
        append(pane->notice, sizeof(pane->notice), " signed out.");
        if (g_guide.request.kind == GUIDE_KIND_MENU)
            end_request(fx, GUIDE_RESULT_CHOSEN, 0); // the console's guide closes once its player has signed out
        changed(fx);
        break;
    }
    case ITEM_CLOSE_GAME:
        port_exit_flush();
        port_exit(0);
        break;
    case ITEM_PROFILE:
        error = port_profile_sign_in(user, item->arg, &bits);
        if (error == 0) {
            port_profile_user_info cur;
            if (port_live_enabled() && port_profile_user(user, &cur) && cur.kind == PORT_SIGNIN_PROFILE &&
                !port_live_user_signed_in((unsigned int)user) && !port_live_user_connecting((unsigned int)user)) {
                signin_changed(bits);
                if (bits != 0)
                    g_guide.signins++;
                fx->connect_live_pane = pane_index;
                copy(fx->connect_live_profile, sizeof(fx->connect_live_profile), cur.name);
                copy(fx->connect_live_name, sizeof(fx->connect_live_name), cur.name);
                fx->connect_live_generation = port_live_user_generation((unsigned int)user);
                fx->session = g_guide.session;
                pane->awaiting_live = 1;
                copy(pane->notice, sizeof(pane->notice), "Signing in to SlopBoxLive...");
            } else {
                signed_in(pane_index, bits, fx);
            }
        } else {
            error_notice(pane, "Cannot sign in", error);
        }
        changed(fx);
        break;
    case ITEM_CONNECT_LIVE:
    case ITEM_CONNECT_LIVE_CONFIRM: {
        port_profile_user_info cur;
        if (port_live_enabled() && port_profile_user(user, &cur) && cur.kind == PORT_SIGNIN_PROFILE &&
            !port_live_user_signed_in((unsigned int)user) && !port_live_user_connecting((unsigned int)user)) {
            fx->connect_live_pane = pane_index;
            copy(fx->connect_live_profile, sizeof(fx->connect_live_profile), cur.name);
            copy(fx->connect_live_name, sizeof(fx->connect_live_name),
                 item->id == ITEM_CONNECT_LIVE ? cur.name : pane->field);
            fx->connect_live_generation = port_live_user_generation((unsigned int)user);
            fx->session = g_guide.session;
            pane->awaiting_live = 1;
            copy(pane->notice, sizeof(pane->notice), "Signing in to SlopBoxLive...");
            changed(fx);
        }
        break;
    }
    case ITEM_GUEST:
        error = port_profile_sign_in_guest(user, &bits);
        if (error == 0)
            signed_in(pane_index, bits, fx);
        else
            error_notice(pane, "Cannot sign in a guest", error);
        changed(fx);
        break;
    case ITEM_CREATE:
        push_page(pane_index, PAGE_CREATE, 0, fx);
        break;
    case ITEM_CREATE_CONFIRM:
    case ITEM_CREATE_OFFLINE:
        fx->create_online = item->id == ITEM_CREATE_CONFIRM && port_live_enabled();
        if (fx->create_online) {
            copy(pane->notice, sizeof(pane->notice), "Signing in to SlopBoxLive...");
            changed(fx);
        }
        fx->create_pane = pane_index;
        copy(fx->create_name, sizeof(fx->create_name), pane->field);
        fx->session = g_guide.session;
        g_guide.working = 1;
        break;
    case ITEM_BUTTON:
        end_request(fx, GUIDE_RESULT_CHOSEN, item->arg);
        break;
    case ITEM_DEVICE:
        end_request(fx, GUIDE_RESULT_CHOSEN, g_guide.request.devices[item->arg].id);
        break;
    case ITEM_REVIEW:
        // The review is Xbox Live's to keep; this console's Live service takes none, so it goes to the log only.
        port_write_error("port: guide: player review: ");
        port_write_error(k_review_choices[item->arg]);
        port_write_error(" (not sent: no review service)\n");
        end_request(fx, GUIDE_RESULT_CHOSEN, item->arg);
        break;
    case ITEM_LOBBY_CODE:
        push_page(pane_index, PAGE_LOBBY_CODE, 0, fx);
        break;
    case ITEM_JOIN_CODE:
        push_page(pane_index, PAGE_JOIN_CODE, 0, fx);
        break;
    case ITEM_COPY:
        fx->copy_pane = pane_index;
        fx->session = g_guide.session;
        break;
    case ITEM_PASTE:
        fx->paste_pane = pane_index;
        fx->session = g_guide.session;
        g_guide.working = 1;
        break;
    case ITEM_JOIN:
        if (port_invite_code_decode(pane->field, &fx->accept) == PORT_INVITE_CODE_OK) {
            fx->accept_pane = pane_index;
            fx->session = g_guide.session;
            g_guide.working = 1;
        }
        break;
    case ITEM_INVITE: {
        if (item->arg < g_inbox_count) { // the inbox build_items read just now
            pane->invite = g_inbox[item->arg];
            push_page(pane_index, PAGE_INVITE, 0, fx);
        }
        break;
    }
    case ITEM_ACCEPT:
        fx->accept = pane->invite;
        fx->accept_pane = pane_index;
        fx->session = g_guide.session;
        g_guide.working = 1;
        break;
    case ITEM_DECLINE:
        copy(fx->decline_id, sizeof(fx->decline_id), pane->invite.id);
        back(pane_index, fx);
        copy(pane->notice, sizeof(pane->notice), "Invite declined.");
        changed(fx);
        break;
    case ITEM_FRIEND:
        if (item->arg < g_guide.friend_count) {
            pane->friend_ = g_guide.friends[item->arg];
            push_page(pane_index, PAGE_FRIEND, 0, fx);
        }
        break;
    case ITEM_SEND_INVITE:
        fx->invite_pane = pane_index;
        fx->session = g_guide.session;
        g_guide.working = 1;
        break;
    case ITEM_SETTING: // A (Enter, a click): the next value
        if (setting_step((int)item->arg, 1)) {
            if ((int)item->arg == PORT_SETTING_MIC_DEVICE)
                fx->apply_mic = 1;
            settings_changed(pane_index, fx);
        }
        break;
    case ITEM_RESET:
        settings_reset(pane_index, fx);
        break;
    case ITEM_KBM_PAGE:
        push_page(pane_index, PAGE_KBM, 0, fx);
        break;
    case ITEM_BINDING: // the next key or mouse button pressed is the binding's (guide_capture)
        g_guide.capture.active = 1;
        g_guide.capture.pane = pane_index;
        g_guide.capture.action = (int)item->arg;
        g_guide.capture.slot = pane->kbm_slot;
        pane->notice[0] = 0;
        changed(fx);
        break;
    case ITEM_KBM_RESET: {
        const int preset = port_config_kbm_preset();
        int any = port_config_apply_kbm_preset(preset);
        for (unsigned i = 0; i < GUIDE_KBM_ROWS; i++) {
            const int setting = k_kbm_rows[i].setting;
            if (setting != PORT_SETTING_KBM_PRESET && !port_config_locked(setting))
                any |= port_config_set(setting, port_config_default(setting)) > 0;
        }
        if (any)
            settings_changed(pane_index, fx);
        copy(pane->notice, sizeof(pane->notice), any ? "Every binding is back to " : "Every binding is ");
        append(pane->notice, sizeof(pane->notice), port_kbm_preset_label(preset));
        append(pane->notice, sizeof(pane->notice), any ? "'s." : "'s already.");
        changed(fx);
        break;
    }
    default:
        break;
    }
}

// Left / right (`step` -1, 1) on the Settings or Keyboard & Mouse page with a setting selected: its value steps, with
// the sound of a horizontal move (the error sound and why, for a setting the launch set); with a binding selected, its
// key or its alternate is the one Enter rebinds. Returns the sound, or -2 when the selected item is neither (the keys
// then do what they do on any page).
static int setting_key(int pane_index, int step, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    const guide_level *level = pane_level(pane);
    if (!(level->page == PAGE_BLADE && level->blade == GUIDE_BLADE_SETTINGS) && level->page != PAGE_KBM)
        return -2;
    const int count = build_items(pane_index, g_items);
    if (level->selected < 0 || level->selected >= count ||
        (g_items[level->selected].id != ITEM_SETTING && g_items[level->selected].id != ITEM_BINDING))
        return -2;
    const guide_item *item = &g_items[level->selected];
    if (item->id == ITEM_BINDING && item->enabled) {
        pane->kbm_slot = (pane->kbm_slot + (step > 0 ? 1 : PORT_KBM_BINDS - 1)) % PORT_KBM_BINDS;
        pane->notice[0] = 0;
        changed(fx);
        return GUIDE_SOUND_HORIZONTAL_NAVIGATION;
    }
    if (!item->enabled) {
        copy(pane->notice, sizeof(pane->notice), item->why);
        changed(fx);
        return GUIDE_SOUND_ERROR;
    }
    if (!setting_step((int)item->arg, step))
        return -1;
    if ((int)item->arg == PORT_SETTING_MIC_DEVICE)
        fx->apply_mic = 1;
    settings_changed(pane_index, fx);
    return GUIDE_SOUND_HORIZONTAL_NAVIGATION;
}

// Selects item `selected` of pane `pane_index`'s page (`count` items). Returns whether the selection changed.
static int select_item(int pane_index, int selected, int count, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    guide_level *level = pane_level(pane);
    if (!pane->osk_menu_focus && osk_grid(pane)) {
        pane->osk_menu_focus = 1;
        changed(fx);
    }
    selected = selected < 0 ? 0 : selected >= count ? count - 1 : selected;
    if (count == 0 || selected == level->selected)
        return 0;
    level->selected = selected;
    pane->notice[0] = 0;
    pane->kbm_slot = 0;
    changed(fx);
    return 1;
}

// Moves pane `pane_index`'s selection by `delta` (-1, 1), wrapping round the list's ends as Halo 3's menus' lists do
// (`wrap`: the D-pad, the stick, the arrows) or stopping at them (the mouse's wheel). Returns whether it moved: a list
// of one item, or an end with `wrap` 0, does nothing and plays nothing (handle_tab_direction is not handled).
static int move_selection(int pane_index, int delta, int wrap, guide_effects *fx)
{
    const int count = build_items(pane_index, g_items);
    if (count == 0)
        return 0;
    int selected = pane_level(&g_guide.panes[pane_index])->selected + delta;
    if (wrap)
        selected = (selected % count + count) % count;
    return select_item(pane_index, selected, count, fx);
}

// The rows of pane `pane_index`'s list the in-game guide shows (guide_ingame_rows_shown): 1 with the first and last.
static int rows_shown(int pane_index, int *first, int *last)
{
    int shown = 0; // the pane as the renderer counts them: the panes shown
    for (int i = 0; i < pane_index; i++)
        shown += g_guide.panes[i].active != 0;
    return guide_ingame_rows_shown(g_guide.session, shown, first, last);
}

// LT / RT (Page Up / Page Down), as handle_tab_direction pages a list: to the top (bottom) row shown, or, from there,
// to the first (last) item; no wrap. Without the rows shown (the in-game guide has not drawn this page), by
// GUIDE_PAGE_STEP items. Returns whether the selection moved.
static int page_selection(int pane_index, int down, guide_effects *fx)
{
    const int count = build_items(pane_index, g_items);
    if (count == 0)
        return 0;
    const int selected = pane_level(&g_guide.panes[pane_index])->selected;
    int first, last, target;
    if (rows_shown(pane_index, &first, &last))
        target = down ? (selected < last ? last : count - 1) : (selected > first ? first : 0);
    else
        target = selected + (down ? GUIDE_PAGE_STEP : -GUIDE_PAGE_STEP);
    return select_item(pane_index, target, count, fx);
}

static void back(int pane_index, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    if (g_guide.capture.pane == pane_index)
        g_guide.capture.active = 0;
    if (pane->depth > 1) {
        pane->depth--;
        pane->awaiting_live = 0;
        pane->notice[0] = 0;
        changed(fx);
    } else {
        end_request(fx, GUIDE_RESULT_CANCELLED, 0);
    }
}

// The window's keys to the next pane shown `delta` (-1, 1) away. Returns whether the focus moved (one pane: never).
static int next_pane(int delta, guide_effects *fx)
{
    for (int step = 1; step <= GUIDE_PANES; step++) {
        const int pane = ((g_guide.focus + delta * step) % GUIDE_PANES + GUIDE_PANES) % GUIDE_PANES;
        if (g_guide.panes[pane].active) {
            if (pane == g_guide.focus)
                return 0;
            g_guide.focus = pane;
            changed(fx);
            return 1;
        }
    }
    return 0;
}

// LB / RB (`step` -1, 1): the blade before / after (tab_target); else, for the window's keys (Shift+Tab / Tab), the
// pane before / after. Returns whether either changed.
static int switch_tab(int pane_index, int step, int keyboard, guide_effects *fx)
{
    const int item = tab_target(pane_index, step);
    if (item >= 0) {
        guide_pane *pane = &g_guide.panes[pane_index];
        const int blade = (int)g_menu_items[item].arg;
        pane->depth--;                        // the blade shown goes
        pane_level(pane)->selected = item;   // the menu's selection follows
        push_page(pane_index, PAGE_BLADE, blade, fx);
        return 1;
    }
    return keyboard && next_pane(step, fx);
}

// Whether a page accepts text input.
static int text_page(int page)
{
    return page == PAGE_CREATE || page == PAGE_LIVE_CONNECT || page == PAGE_JOIN_CODE;
}

// A character typed (or pasted) into the pane's field: a gamertag's letters, digits and spaces (at most 15), a lobby
// code's letters, digits and dashes. Returns 1 when it was taken, -1 for a space or line break in a lobby code (which
// a code's reader skips, so it is not kept and not refused), 0 when it was refused (not allowed, or the field full).
static int type_character(int pane_index, unsigned long character, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    const int code = pane_level(pane)->page == PAGE_JOIN_CODE;
    const unsigned long n = text_length(pane->field);
    const int alphanumeric = (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
                             (character >= '0' && character <= '9');
    if (code && (character == ' ' || character == '\t' || character == '\r' || character == '\n'))
        return -1;
    const int allowed = alphanumeric || (code ? character == '-' : character == ' ');
    if (!allowed || n >= (code ? PORT_INVITE_CODE_MAX - 1 : PORT_PROFILE_GAMERTAG_MAX))
        return 0;
    pane->field[n] = (char)character;
    pane->field[n + 1] = 0;
    if (osk_grid(pane)) {
        pane->notice[0] = 0;
        if (pane_level(pane)->page == PAGE_CREATE) {
            pane_level(pane)->selected = 0;
            pane->osk_menu_focus = 0;
        }
    }
    changed(fx);
    return 1;
}

// Controller-only shortcuts. On other pages the guide ignores these buttons.
enum { OSK_ERASE = GUIDE_KEY_VALUE_NEXT + 1, OSK_SHIFT, OSK_SUBMIT };

static void osk_move(guide_pane *pane, int key, guide_effects *fx)
{
    int row = pane->osk_key / OSK_COLUMNS, col = pane->osk_key % OSK_COLUMNS;
    if (key == GUIDE_KEY_UP || key == GUIDE_KEY_DOWN) {
        row = (row + (key == GUIDE_KEY_UP ? OSK_ROWS - 1 : 1)) % OSK_ROWS;
        guide_sound_post(GUIDE_SOUND_VERTICAL_NAVIGATION);
    } else {
        col = (col + (key == GUIDE_KEY_LEFT ? OSK_COLUMNS - 1 : 1)) % OSK_COLUMNS;
        guide_sound_post(GUIDE_SOUND_HORIZONTAL_NAVIGATION);
    }
    pane->osk_key = row * OSK_COLUMNS + col;
    changed(fx);
}

// Translate a controller press into the ordinary field action. Navigation, Shift and Done act here and consume it.
static int osk_pad_key(int pane_index, int key, unsigned long *character, int *sound, guide_effects *fx)
{
    guide_pane *pane = &g_guide.panes[pane_index];
    const char *grid = osk_grid(pane);
    if (!grid)
        return key;
    const int direction = dpad_of_key(key);
    if (direction < 0) {
        switch (key) {
        case GUIDE_KEY_ACCEPT:
            *character = (unsigned char)grid[pane->osk_key];
            *sound = GUIDE_SOUND_A_BUTTON;
            break;
        case OSK_ERASE: *character = '\b'; break;
        case OSK_SHIFT: *character = '^'; break;
        case GUIDE_KEY_NEXT_TAB:
            if (pane_level(pane)->page != PAGE_CREATE || !port_live_enabled() || signin_online_only())
                return key;
            pane_level(pane)->selected = 1;
            choose(pane_index, fx);
            pane_level(pane)->selected = 0;
            return -1;
        case OSK_SUBMIT: *character = '\r'; break;
        default: return key;
        }
    }
    if (g_guide.focus != pane_index || pane->osk_menu_focus) {
        g_guide.focus = pane_index; // physical typing follows the controller editing this pane
        pane->osk_menu_focus = 0;
        changed(fx);
    }
    if (direction >= 0) {
        osk_move(pane, key, fx);
        return -1;
    }
    switch (*character) {
    case '^':
        pane->osk_shift = !pane->osk_shift;
        changed(fx);
        guide_sound_post(GUIDE_SOUND_A_BUTTON);
        return -1;
    case '\r':
        // The mouse or physical keyboard may have selected Paste. Done always chooses Create or Join.
        pane_level(pane)->selected = 0;
        choose(pane_index, fx);
        return -1;
    case '\b': return GUIDE_KEY_ERASE;
    case '\t': return GUIDE_KEY_PASTE;
    default: return GUIDE_KEY_CHARACTER;
    }
}

// A key (guide_key, or a controller's button as one: `keyboard` 0) for pane `pane_index`, with the sound Halo 3's
// window manager plays for its input when it did something (window_manager_play_input_sound; guide.h).
static void act(int pane_index, int key, unsigned long character, int keyboard, guide_effects *fx)
{
    if (!g_guide.open || g_guide.working || pane_index < 0 || pane_index >= GUIDE_PANES ||
        !g_guide.panes[pane_index].active)
        return;
    guide_pane *pane = &g_guide.panes[pane_index];
    const int field = text_page(pane_level(pane)->page);
    if (key == GUIDE_KEY_CHARACTER && !field) { // outside a text field: Space A, Q LB, E RB, ` Guide
        key = character == ' '                     ? GUIDE_KEY_ACCEPT
              : character == 'q' || character == 'Q' ? GUIDE_KEY_PREVIOUS_TAB
              : character == 'e' || character == 'E' ? GUIDE_KEY_NEXT_TAB
              : character == '`'                     ? GUIDE_KEY_CLOSE
                                                     : -1;
    }
    if (key == GUIDE_KEY_ERASE && !field) // Backspace outside a text field: B
        key = GUIDE_KEY_BACK;
    if (g_guide.capture.active && g_guide.capture.pane == pane_index) {
        // waiting for an input to bind (guide_capture: the window's keys and mouse go there first): B cancels, the
        // Guide button closes the guide, the rest waits
        if (key == GUIDE_KEY_BACK) {
            g_guide.capture.active = 0;
            copy(pane->notice, sizeof(pane->notice), "Not changed.");
            changed(fx);
            guide_sound_post(GUIDE_SOUND_B_BUTTON);
            return;
        }
        if (key != GUIDE_KEY_CLOSE)
            return;
    }
    int sound = -1;
    if (!keyboard) {
        key = osk_pad_key(pane_index, key, &character, &sound, fx);
        if (key < 0)
            return;
    }
    switch (key) {
    case GUIDE_KEY_UP:
    case GUIDE_KEY_DOWN:
        if (move_selection(pane_index, key == GUIDE_KEY_UP ? -1 : 1, 1, fx))
            sound = GUIDE_SOUND_VERTICAL_NAVIGATION;
        break;
    case GUIDE_KEY_SCROLL_UP:
    case GUIDE_KEY_SCROLL_DOWN:
        if (move_selection(pane_index, key == GUIDE_KEY_SCROLL_UP ? -1 : 1, 0, fx))
            sound = GUIDE_SOUND_VERTICAL_NAVIGATION;
        break;
    case GUIDE_KEY_PAGE_UP:
    case GUIDE_KEY_PAGE_DOWN:
        if (page_selection(pane_index, key == GUIDE_KEY_PAGE_DOWN, fx))
            sound = key == GUIDE_KEY_PAGE_UP ? GUIDE_SOUND_LEFT_TRIGGER : GUIDE_SOUND_RIGHT_TRIGGER;
        break;
    case GUIDE_KEY_LEFT:
    case GUIDE_KEY_RIGHT:
    {
        const int setting_sound = setting_key(pane_index, key == GUIDE_KEY_LEFT ? -1 : 1, fx);
        if (setting_sound != -2) { // the Settings page: the selected setting's value
            sound = setting_sound;
        } else if (keyboard) { // the panes side by side
            if (next_pane(key == GUIDE_KEY_LEFT ? -1 : 1, fx))
                sound = GUIDE_SOUND_HORIZONTAL_NAVIGATION;
        } else if (g_guide.focus != pane_index) { // a controller's own pane: the window's keys follow it, silently
            g_guide.focus = pane_index;
            changed(fx);
        }
        break;
    }
    case GUIDE_KEY_VALUE_PREVIOUS:
    case GUIDE_KEY_VALUE_NEXT: {
        const int setting_sound = setting_key(pane_index, key == GUIDE_KEY_VALUE_PREVIOUS ? -1 : 1, fx);
        sound = setting_sound != -2 ? setting_sound : -1;
        break;
    }
    case GUIDE_KEY_PREVIOUS_TAB:
    case GUIDE_KEY_NEXT_TAB:
        if (switch_tab(pane_index, key == GUIDE_KEY_PREVIOUS_TAB ? -1 : 1, keyboard, fx))
            sound = key == GUIDE_KEY_PREVIOUS_TAB ? GUIDE_SOUND_LEFT_BUMPER : GUIDE_SOUND_RIGHT_BUMPER;
        break;
    case GUIDE_KEY_ACCEPT:
        choose(pane_index, fx); // its own sound: a_button, or error for a disabled item
        break;
    case GUIDE_KEY_BACK:
        back(pane_index, fx);
        sound = GUIDE_SOUND_B_BUTTON;
        break;
    case GUIDE_KEY_CLOSE: // the Guide button: it closes the guide as Start closes the start menu
        end_request(fx, GUIDE_RESULT_CANCELLED, 0);
        sound = GUIDE_SOUND_START_BUTTON;
        break;
    case GUIDE_KEY_ERASE:
        if (pane->field[0] != 0) {
            pane->field[text_length(pane->field) - 1] = 0;
            if (osk_grid(pane))
                pane->notice[0] = 0;
            changed(fx);
        }
        break;
    case GUIDE_KEY_CHARACTER:
        if (type_character(pane_index, character, fx) == 0)
            sound = GUIDE_SOUND_ERROR;
        break;
    case GUIDE_KEY_PASTE: // the clipboard is read once the lock is released
        if (field) {
            fx->paste_pane = pane_index;
            fx->session = g_guide.session;
            g_guide.working = 1;
        }
        break;
    default:
        break;
    }
    guide_sound_post(sound);
}

// ---- the controllers and the window's keys (under the lock) ----

// The window's arrow keys, as a D-pad, at `now`: the key a held one gives (direction_step), acted on.
static void keys_step(unsigned long now, guide_effects *fx)
{
    const int key = direction_step(&g_guide.key_repeat, &g_guide.keys, 0, 0, now);
    if (key >= 0)
        act(g_guide.focus, key, 0, 1, fx);
}

// Every key and button is up (a new opening of the guide, the window's focus lost): nothing repeats.
static void keys_reset(void)
{
    __builtin_memset(&g_guide.keys, 0, sizeof(g_guide.keys));
    __builtin_memset(&g_guide.key_repeat, 0, sizeof(g_guide.key_repeat));
}

// ---- after the lock ----

static void trace_view(void)
{
    static guide_view view; // the trace's (under trace_busy)
    static volatile char trace_busy;
    while (__atomic_test_and_set(&trace_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    guide_view_get(&view);
    static char line[GUIDE_TEXT * 4];
    if (!view.open) {
        port_write_error("guide: closed\n");
    }
    for (int p = 0; p < view.pane_count; p++) {
        const guide_view_pane *pane = &view.panes[p];
        copy(line, sizeof(line), "guide: [");
        append(line, sizeof(line), pane->heading);
        append(line, sizeof(line), ": ");
        append(line, sizeof(line), pane->status);
        append(line, sizeof(line), pane->focused ? "]* " : "] ");
        append(line, sizeof(line), pane->title);
        if (pane->body[0] != 0) {
            append(line, sizeof(line), " -- ");
            append(line, sizeof(line), pane->body);
        }
        if (pane->has_field) {
            append(line, sizeof(line), " [");
            append(line, sizeof(line), pane->field);
            append(line, sizeof(line), pane->field_readonly ? "]" : "_]");
        }
        for (int i = 0; i < pane->item_count; i++) {
            append(line, sizeof(line), i == pane->selected ? " | > " : " | ");
            if (pane->items[i].checked)
                append(line, sizeof(line), "* ");
            append(line, sizeof(line), pane->items[i].text);
            if (pane->items[i].detail[0] != 0) {
                append(line, sizeof(line), " (");
                append(line, sizeof(line), pane->items[i].detail);
                append(line, sizeof(line), ")");
            }
            if (!pane->items[i].enabled)
                append(line, sizeof(line), " [x]");
        }
        if (pane->hint[0] != 0) {
            append(line, sizeof(line), " -- ");
            append(line, sizeof(line), pane->hint);
        }
        for (char *c = line; *c != 0; c++)
            if (*c == '\n')
                *c = ' ';
        append(line, sizeof(line), "\n");
        port_write_error(line);
    }
    __atomic_clear(&trace_busy, __ATOMIC_RELEASE);
}

static void run_effects_now(guide_effects *fx);

static const char *live_failure_notice(const port_live_auth_result *result)
{
    switch (result->status) {
    case PORT_LIVE_AUTH_GAMERTAG_TAKEN:
        return "That gamertag is already taken. Choose another name.";
    case PORT_LIVE_AUTH_INVALID_GAMERTAG:
        return "Enter an Xbox Live gamertag with 1 to 15 letters, digits or spaces.";
    case PORT_LIVE_AUTH_TOKEN_REJECTED:
        return "The saved Live token was rejected. Retry or choose another profile.";
    case PORT_LIVE_AUTH_PROFILE_HAS_ACCOUNT:
    case PORT_LIVE_AUTH_UPGRADE_REQUIRED:
        return result->detail;
    case PORT_LIVE_AUTH_RATE_LIMITED:
        return "Rate limited. Wait a moment and try again.";
    case PORT_LIVE_AUTH_UNREACHABLE:
        return "Could not reach SlopBoxLive. Check your connection and try again.";
    case PORT_LIVE_AUTH_STORAGE_ERROR:
        return result->detail;
    default:
        return "The Xbox Live server could not sign you in. Try again later.";
    }
}

static void live_connect_done(unsigned int user_index, unsigned int generation, void *context)
{
    const unsigned long session = (unsigned long)(__SIZE_TYPE__)context;
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    port_live_auth_result result;
    if (!port_live_user_auth_result(user_index, generation, &result)) {
        guide_unlock();
        return;
    }
    const int ok = result.status == PORT_LIVE_AUTH_OK || result.status == PORT_LIVE_AUTH_RENAMED_SUFFIX;
    if (ok)
        signin_changed(1ul << user_index);
    guide_pane *pane = &g_guide.panes[user_index];
    if (g_guide.open && g_guide.session == session && pane->awaiting_live) {
        pane->awaiting_live = 0;
        if (ok) {
            show_signed_in((int)user_index, &fx);
        } else {
            guide_sound_post(GUIDE_SOUND_ERROR);
            if (result.status == PORT_LIVE_AUTH_GAMERTAG_TAKEN && pane_level(pane)->page != PAGE_LIVE_CONNECT) {
                push_page((int)user_index, PAGE_LIVE_CONNECT, 0, &fx);
                copy(pane->notice, sizeof(pane->notice), pane->field);
                append(pane->notice, sizeof(pane->notice), " is taken on SlopBoxLive. Pick an online gamertag:");
            } else {
                copy(pane->notice, sizeof(pane->notice), live_failure_notice(&result));
            }
        }
        changed(&fx);
    } else if (ok && g_guide.open) {
        changed(&fx);
    }
    guide_unlock();
    run_effects_now(&fx);
}

// Does the effects in `fx` on this thread (each slow one, below, on the worker).
static void run_effects_now(guide_effects *fx)
{
    for (;;) {
        if (fx->connect_live_pane >= 0) {
            const int pane_index = fx->connect_live_pane;
            fx->connect_live_pane = -1;
            const int started = port_live_connect_user_async((unsigned int)pane_index, fx->connect_live_profile,
                fx->connect_live_name, fx->connect_live_generation, live_connect_done, (void *)(__SIZE_TYPE__)fx->session);
            if (started != 0) {
                guide_lock();
                if (g_guide.open && g_guide.session == fx->session && g_guide.panes[pane_index].awaiting_live &&
                    fx->connect_live_generation == port_live_user_generation((unsigned int)pane_index)) {
                    g_guide.panes[pane_index].awaiting_live = 0;
                    guide_sound_post(GUIDE_SOUND_ERROR);
                    copy(g_guide.panes[pane_index].notice, sizeof(g_guide.panes[pane_index].notice),
                         "Could not start SlopBoxLive sign-in.");
                    changed(fx);
                }
                guide_unlock();
            }
            continue;
        }
        if (fx->create_pane >= 0) {
            const int pane_index = fx->create_pane;
            char name[PORT_PROFILE_GAMERTAG_MAX + 1];
            copy(name, sizeof(name), fx->create_name);
            fx->create_pane = -1;
            const int online = fx->create_online;
            port_live_account account;
            port_live_auth_result auth = { 0 };
            unsigned long index = 0;
            unsigned long error = online ? port_profile_create_error(name) : 0;
            const int rejected = online && !error && !port_live_prepare_profile(name, &account, &auth);
            if (!error && !rejected)
                error = port_profile_create(name, &index);
            int activated = 0;
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session) {
                guide_pane *pane = &g_guide.panes[pane_index];
                g_guide.working = 0;
                if (rejected) {
                    guide_sound_post(GUIDE_SOUND_ERROR);
                    copy(pane->notice, sizeof(pane->notice),
                        auth.status == PORT_LIVE_AUTH_UNREACHABLE && !signin_online_only()
                        ? "Could not reach SlopBoxLive. Try again or choose Offline Only." : live_failure_notice(&auth));
                } else if (error) {
                    error_notice(pane, "Cannot create the profile", error);
                } else {
                    unsigned long bits = 0;
                    error = port_profile_sign_in((unsigned long)pane_index, index, &bits);
                    if (error) {
                        error_notice(pane, "Cannot sign in", error);
                    } else {
                        if (online)
                            activated = port_live_activate_profile((unsigned int)pane_index,
                                port_live_user_generation((unsigned int)pane_index), name, &account, &auth);
                        signed_in(pane_index, bits, fx);
                    }
                }
                changed(fx);
            }
            guide_unlock();
            if (activated)
                port_live_finish_signin((unsigned int)pane_index);
            continue;
        }
        if (fx->load_achievements >= 0) {
            // One load at a time (loaded[] is shared by every thread that runs effects). What it read is kept only for
            // the session that asked and for the sign-in it was read for; a sign-in that changed meanwhile reads again.
            static guide_achievement loaded[GUIDE_ITEMS];
            static volatile char loading;
            const int user = fx->load_achievements;
            fx->load_achievements = -1;
            while (__atomic_test_and_set(&loading, __ATOMIC_ACQUIRE))
                __builtin_ia32_pause();
            const unsigned long serial = port_profile_user_serial((unsigned long)user);
            const unsigned long count = port_guide_achievements((unsigned long)user, loaded, GUIDE_ITEMS);
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session) {
                if (serial == port_profile_user_serial((unsigned long)user)) {
                    __builtin_memcpy(g_guide.achievements, loaded, sizeof(loaded[0]) * count);
                    g_guide.achievement_count = count;
                    g_guide.achievements_user = user;
                    g_guide.achievements_serial = serial;
                    changed(fx);
                } else {
                    fx->load_achievements = user;
                }
            }
            guide_unlock();
            __atomic_clear(&loading, __ATOMIC_RELEASE);
            continue;
        }
        if (fx->code_pane >= 0 || fx->copy_pane >= 0) {
            // the lobby code made (the inviter's gamertag may need Live) or taken from the page, then copied
            const int pane_index = fx->code_pane >= 0 ? fx->code_pane : fx->copy_pane;
            const int make = fx->code_pane >= 0;
            fx->code_pane = fx->copy_pane = -1;
            char code[PORT_INVITE_CODE_MAX];
            const char *why = "";
            code[0] = 0;
            if (make) {
                port_invite_lobby_code((unsigned long)pane_index, code, sizeof(code), &why);
            } else {
                guide_lock();
                if (g_guide.open && g_guide.session == fx->session)
                    copy(code, sizeof(code), g_guide.panes[pane_index].code);
                guide_unlock();
            }
            const int copied = code[0] != 0 && port_clipboard_set(code);
            port_invite decoded;
            const int class_of = port_invite_code_decode(code, &decoded) == PORT_INVITE_CODE_OK
                                     ? decoded.session_class
                                     : PORT_INVITE_CLASS_NONE;
            guide_lock();
            guide_pane *pane = &g_guide.panes[pane_index];
            if (g_guide.open && g_guide.session == fx->session && pane->active &&
                pane_level(pane)->page == PAGE_LOBBY_CODE) {
                if (make) {
                    copy(pane->code, sizeof(pane->code), code);
                    pane->code_class = class_of;
                }
                copy(pane->notice, sizeof(pane->notice),
                     code[0] == 0 ? why
                     : copied     ? "Copied to the clipboard."
                                  : "This PC has no clipboard to copy to: share the code shown.");
                if (code[0] == 0 || (!make && !copied)) // no code; Copy to Clipboard chosen, and nothing to copy to
                    guide_sound_post(GUIDE_SOUND_ERROR);
                changed(fx);
            }
            guide_unlock();
            continue;
        }
        if (fx->paste_pane >= 0) {
            const int pane_index = fx->paste_pane;
            fx->paste_pane = -1;
            char text[PORT_CLIPBOARD_MAX];
            const int got = port_clipboard_get(text, sizeof(text));
            guide_lock();
            guide_pane *pane = &g_guide.panes[pane_index];
            if (g_guide.open && g_guide.session == fx->session) {
                g_guide.working = 0;
                const int page = pane->active ? pane_level(pane)->page : -1;
                if (text_page(page)) {
                    // a whole code replaces what was typed; anything else is typed at the end
                    if (page == PAGE_JOIN_CODE && got && port_invite_code_decode(text, 0) == PORT_INVITE_CODE_OK)
                        pane->field[0] = 0;
                    int taken = 0;
                    for (const char *c = text; got && *c != 0; c++)
                        taken += type_character(pane_index, (unsigned char)*c, fx) > 0;
                    // what was pasted into the join page must make a whole code: else the field shows why (the error
                    // sound, as for anything refused)
                    const int code_result =
                        page == PAGE_JOIN_CODE ? port_invite_code_decode(pane->field, 0) : PORT_INVITE_CODE_OK;
                    copy(pane->notice, sizeof(pane->notice),
                         taken == 0 ? (got ? (page == PAGE_JOIN_CODE ? "The clipboard holds no lobby code."
                                                                     : "The clipboard holds no gamertag.")
                                           : "The clipboard is empty.")
                         : code_result != PORT_INVITE_CODE_OK ? port_invite_code_message(code_result)
                                                              : "");
                    if (taken == 0 || code_result != PORT_INVITE_CODE_OK)
                        guide_sound_post(GUIDE_SOUND_ERROR);
                    changed(fx);
                }
            }
            guide_unlock();
            continue;
        }
        if (fx->load_friends) {
            fx->load_friends = 0;
            static port_live_friend loaded[GUIDE_ITEMS]; // one read at a time (shared by the threads running effects)
            static volatile char reading;
            while (__atomic_test_and_set(&reading, __ATOMIC_ACQUIRE))
                __builtin_ia32_pause();
            unsigned int count = 0;
            const int result = port_live_friends(loaded, GUIDE_ITEMS, &count);
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session) {
                g_guide.friends_state = result == 0                       ? k_friends_read
                                        : result == PORT_LIVE_UNSUPPORTED ? k_friends_unsupported
                                                                          : k_friends_failed;
                g_guide.friend_count = result == 0 ? count : 0;
                __builtin_memcpy(g_guide.friends, loaded, sizeof(loaded[0]) * g_guide.friend_count);
                changed(fx);
            }
            guide_unlock();
            __atomic_clear(&reading, __ATOMIC_RELEASE);
            continue;
        }
        if (fx->invite_pane >= 0) {
            const int pane_index = fx->invite_pane;
            fx->invite_pane = -1;
            port_live_friend to;
            __builtin_memset(&to, 0, sizeof(to));
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session)
                to = g_guide.panes[pane_index].friend_;
            guide_unlock();
            port_invite_lobby lobby;
            const char *why = "";
            const int result = !port_invite_current_lobby(&lobby, &why) ? 1
                               : to.xuid == 0                            ? -1
                                    : port_live_invite_send(to.xuid, lobby.session, lobby.session_class);
            guide_lock();
            guide_pane *pane = &g_guide.panes[pane_index];
            if (g_guide.open && g_guide.session == fx->session) {
                g_guide.working = 0;
                if (result == 0) {
                    copy(pane->notice, sizeof(pane->notice), "Invite sent to ");
                    append(pane->notice, sizeof(pane->notice), to.gamertag);
                    append(pane->notice, sizeof(pane->notice), ".");
                } else {
                    static const char k_unsupported[] =
                        "This Live server delivers no game invites: share a lobby code instead.";
                    static const char k_unreached[] =
                        "The invite could not be sent: the Live server could not be reached.";
                    copy(pane->notice, sizeof(pane->notice),
                         result == 1 ? why : result == PORT_LIVE_UNSUPPORTED ? k_unsupported : k_unreached);
                    guide_sound_post(GUIDE_SOUND_ERROR);
                }
                changed(fx);
            }
            guide_unlock();
            continue;
        }
        if (fx->decline_id[0] != 0) {
            char id[PORT_INVITE_ID];
            copy(id, sizeof(id), fx->decline_id);
            fx->decline_id[0] = 0;
            port_invite_answered(id, 0); // Live is told (HTTP)
            continue;
        }
        if (fx->accept_pane >= 0) {
            // checked now (whether the user can join it may need Live); accepted once the guide has closed
            const int pane_index = fx->accept_pane;
            fx->accept_pane = -1;
            const int error = port_invite_check((unsigned long)pane_index, &fx->accept);
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session) {
                g_guide.working = 0;
                if (error != PORT_INVITE_OK) {
                    copy(g_guide.panes[pane_index].notice, sizeof(g_guide.panes[pane_index].notice),
                         port_invite_error_message(error));
                    guide_sound_post(GUIDE_SOUND_ERROR);
                    changed(fx);
                } else {
                    fx->accepted = 1;
                    fx->accept_user = (unsigned long)pane_index;
                    end_request(fx, GUIDE_RESULT_CHOSEN, 0); // the console's guide closes on Accept
                }
            }
            guide_unlock();
            continue;
        }
        if (fx->load_mics) {
            fx->load_mics = 0;
            xhv_voice_device found[XHV_VOICE_MAX_DEVICES];
            const unsigned count = xhv_voice_devices(found, XHV_VOICE_MAX_DEVICES);
            guide_lock();
            if (g_guide.open && g_guide.session == fx->session) {
                int same = count == g_guide.mic_device_count;
                for (unsigned i = 0; same && i < count; i++) {
                    if (!same_text(found[i].id, g_guide.mic_devices[i].id) ||
                        !same_text(found[i].name, g_guide.mic_devices[i].name))
                        same = 0;
                }
                if (!same) {
                    g_guide.mic_device_count = count;
                    if (count != 0)
                        __builtin_memcpy(g_guide.mic_devices, found, sizeof(found[0]) * count);
                    changed(fx);
                }
            }
            guide_unlock();
            continue;
        }
        if (fx->apply_mic) {
            fx->apply_mic = 0;
            xhv_voice_refresh();
            continue;
        }
        if (fx->save_settings) {
            // written whether or not the guide still shows (the setting changed already); a failure is shown on the
            // Settings page that asked, if it still shows. No halo3.ini (ERROR_NOT_SUPPORTED): the page says so.
            fx->save_settings = 0;
            const unsigned long error = port_config_save();
            if (error != 0 && error != 50) {
                guide_lock();
                for (int i = 0; i < GUIDE_PANES && g_guide.open && g_guide.session == fx->session; i++) {
                    guide_pane *pane = &g_guide.panes[i];
                    if (pane->active && ((pane_level(pane)->page == PAGE_BLADE &&
                                          pane_level(pane)->blade == GUIDE_BLADE_SETTINGS) ||
                                         pane_level(pane)->page == PAGE_KBM)) {
                        error_notice(pane, "Cannot save halo3.ini", error);
                        changed(fx);
                    }
                }
                guide_unlock();
            }
            continue;
        }
        break;
    }
    if (fx->ended) {
        port_xam_guide_end();
        if (fx->finished.done != 0)
            fx->finished.done(&fx->finished, fx->result, fx->value);
    }
    if (fx->accepted) // XN_LIVE_INVITE_ACCEPTED after the guide's XN_SYS_UI (0)
        port_invite_accept(fx->accept_user, &fx->accept);
    if (fx->changed) {
        if (trace_on())
            trace_view();
        guide_host_wake();
    }
}

// ---- the worker ----
//
// The effects that wait on something outside the port run on a worker thread (guide_host_thread_start), never on the
// thread that acted: that is the game's (a controller's poll) or the window's (keys, clicks; for the in-game guide the
// presenting thread), and neither may stall for the network or another program. Slow are: the lobby code (made with
// the inviter's gamertag, which may sign in to Live) and copying it, pasting (the X11 selection's owner answers, or
// its deadline passes), Live's friends list and invites sent, a decline told to Live, an accept's check (whether
// the user is on Live), enumerating/reopening the host microphone, and writing halo3.ini (flushed to the device). Their
// jobs queue under the lock, in order (so halo3.ini's writes follow the changes in the order they were made); one worker
// drains the queue and ends when it is empty (none waits idle). Each job checks, as before, that the opening of the
// guide that asked is still the one showing before it shows anything. Without a thread (or with the queue full) the
// acting thread does them itself.
#define GUIDE_JOBS 8

static struct {
    guide_effects jobs[GUIDE_JOBS]; // the oldest first
    unsigned count;
    int running;                    // a worker drains the queue
} g_worker; // under the lock

static int effects_slow(const guide_effects *fx)
{
    return (fx->create_pane >= 0 && fx->create_online) || fx->code_pane >= 0 || fx->copy_pane >= 0 ||
           fx->paste_pane >= 0 || fx->load_friends || fx->load_mics || fx->apply_mic || fx->invite_pane >= 0 ||
           fx->decline_id[0] != 0 || fx->accept_pane >= 0 || fx->save_settings;
}

// Moves the slow effects of `fx` to `job` (otherwise empty, of the same session).
static void effects_take_slow(guide_effects *fx, guide_effects *job)
{
    effects_init(job);
    job->session = fx->session;
    job->create_pane = fx->create_pane;
    job->create_online = fx->create_online;
    copy(job->create_name, sizeof(job->create_name), fx->create_name);
    fx->create_pane = -1;
    job->code_pane = fx->code_pane;
    job->copy_pane = fx->copy_pane;
    job->paste_pane = fx->paste_pane;
    job->load_friends = fx->load_friends;
    job->load_mics = fx->load_mics;
    job->apply_mic = fx->apply_mic;
    job->invite_pane = fx->invite_pane;
    job->accept_pane = fx->accept_pane;
    job->accept = fx->accept;
    copy(job->decline_id, sizeof(job->decline_id), fx->decline_id);
    job->save_settings = fx->save_settings;
    fx->code_pane = fx->copy_pane = fx->paste_pane = fx->invite_pane = fx->accept_pane = -1;
    fx->load_friends = fx->load_mics = fx->apply_mic = fx->save_settings = 0;
    fx->decline_id[0] = 0;
}

static void worker_main(void *unused)
{
    (void)unused;
    static guide_effects job; // one worker at a time (g_worker.running)
    for (;;) {
        guide_lock();
        if (g_worker.count == 0) {
            g_worker.running = 0;
            guide_unlock();
            return;
        }
        job = g_worker.jobs[0];
        for (unsigned i = 1; i < g_worker.count; i++)
            g_worker.jobs[i - 1] = g_worker.jobs[i];
        g_worker.count--;
        guide_unlock();
        run_effects_now(&job);
    }
}

// What the lock's holder decided, once it is released: the slow effects to the worker, the rest at once.
static void run_effects(guide_effects *fx)
{
    if (effects_slow(fx)) {
        int start = 0;
        guide_lock();
        // a write of halo3.ini or a mic refresh still queued handles this change too (it reads the settings as they are
        // when it runs): holding a setting's arrow down queues one job, not one per step
        for (unsigned i = 0; i < g_worker.count; i++) {
            if (fx->save_settings && g_worker.jobs[i].save_settings)
                fx->save_settings = 0;
            if (fx->apply_mic && g_worker.jobs[i].apply_mic)
                fx->apply_mic = 0;
            if (fx->load_mics && g_worker.jobs[i].load_mics && g_worker.jobs[i].session == fx->session)
                fx->load_mics = 0;
        }
        if (effects_slow(fx) && g_worker.count < GUIDE_JOBS) {
            effects_take_slow(fx, &g_worker.jobs[g_worker.count++]);
            start = !g_worker.running;
            g_worker.running = 1;
        }
        guide_unlock();
        if (start && !guide_host_thread_start(worker_main, 0))
            worker_main(0); // no thread: this one drains the queue
    }
    run_effects_now(fx);
}

// ---- requests ----

// A request that completes at once (nobody operates the guide): what the user would most likely have answered.
static unsigned long complete_at_once(const guide_request *request)
{
    if (!port_xam_guide_pulse())
        return GUIDE_ERROR_ACCESS_DENIED;
    guide_request finished = *request;
    int result = GUIDE_RESULT_CHOSEN;
    unsigned long value = 0, changed_bits = 0;
    switch (request->kind) {
    case GUIDE_KIND_SIGNIN:
        // every connected controller whose user is not signed in signs in to its own built-in profile (or the first
        // free one), unless only Live profiles may sign in (one pane: only the controller that asked)
        if ((request->flags & GUIDE_XSSUI_SHOWONLYONLINEENABLED) == 0) {
            for (unsigned long user = 0; user < PORT_PROFILE_USERS; user++) {
                if ((request->connected & (1ul << user)) == 0 || port_profile_signed_in(user))
                    continue;
                if (request->panes == 1 && user != request->user)
                    continue;
                const unsigned long count = port_profile_count();
                unsigned long bits = 0;
                if (user < count && port_profile_signed_in_on(user) < 0)
                    port_profile_sign_in(user, user, &bits);
                for (unsigned long profile = 0; profile < count && bits == 0; profile++)
                    if (port_profile_signed_in_on(profile) < 0)
                        port_profile_sign_in(user, profile, &bits);
                changed_bits |= bits;
            }
        } else if (!online_signin_complete(request)) {
            result = GUIDE_RESULT_CANCELLED;
        }
        break;
    case GUIDE_KIND_MESSAGE_BOX:
        value = request->focus < request->button_count ? request->focus : 0;
        break;
    case GUIDE_KIND_DEVICE_SELECTOR:
        result = GUIDE_RESULT_CANCELLED;
        for (unsigned long i = 0; i < request->device_count && result != GUIDE_RESULT_CHOSEN; i++)
            if (request->devices[i].selectable) {
                result = GUIDE_RESULT_CHOSEN;
                value = request->devices[i].id;
            }
        break;
    default:
        break;
    }
    port_xam_signin_changed(changed_bits);
    if (trace_on()) {
        static const char *const k_kinds[] = { "menu", "sign-in", "message box", "device selector", "page" };
        port_write_error("guide: (automatic) ");
        port_write_error(k_kinds[request->kind]);
        port_write_error(result == GUIDE_RESULT_CHOSEN ? " answered\n" : " cancelled\n");
    }
    if (finished.done != 0)
        finished.done(&finished, result, value);
    return GUIDE_ERROR_SUCCESS;
}

PORT_MS_ABI unsigned long guide_open(const guide_request *request)
{
    if (request == 0 || request->user >= GUIDE_PANES || request->kind < GUIDE_KIND_MENU ||
        request->kind > GUIDE_KIND_BLADE ||
        (request->kind == GUIDE_KIND_SIGNIN && request->panes != 1 && request->panes != 2 && request->panes != 4) ||
        (request->kind == GUIDE_KIND_BLADE && (request->blade < 0 || request->blade >= GUIDE_BLADE_COUNT)))
        return GUIDE_ERROR_INVALID_PARAMETER;
    if (!guide_interactive())
        return complete_at_once(request);
    if (!port_xam_guide_begin())
        return GUIDE_ERROR_ACCESS_DENIED;
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    g_guide.request = *request;
    guide_request *copy_of = &g_guide.request;
    if (copy_of->kind == GUIDE_KIND_MESSAGE_BOX && copy_of->button_count == 0) {
        copy(copy_of->buttons[0], GUIDE_LABEL, "OK");
        copy_of->button_count = 1;
    }
    if (copy_of->button_count > GUIDE_BUTTONS)
        copy_of->button_count = GUIDE_BUTTONS;
    if (copy_of->device_count > GUIDE_DEVICES)
        copy_of->device_count = GUIDE_DEVICES;
    __builtin_memset(g_guide.panes, 0, sizeof(g_guide.panes));
    g_guide.signins = 0;
    g_guide.working = 0;
    g_guide.windowed = 0;
    g_guide.focus = (int)request->user;
    g_guide.achievements_user = -1;
    g_guide.friends_state = k_friends_none;
    g_guide.friend_count = 0;
    keys_reset(); // a key held as the guide opens acts only once let go and pressed again
    g_guide.session++;
    __atomic_store_n(&g_guide.open, 1, __ATOMIC_RELEASE);
    for (int i = 0; i < GUIDE_PANES; i++) {
        guide_pane *pane = &g_guide.panes[i];
        pane->buttons = GUIDE_BUTTONS_UNKNOWN;
        // the sign-in UI: a pane per controller, or (one pane) the asking controller's only; any other page: its own
        pane->active = request->kind == GUIDE_KIND_SIGNIN ? request->panes != 1 || i == (int)request->user
                                                          : i == (int)request->user;
        if (!pane->active)
            continue;
        int page = PAGE_MENU, blade = 0;
        switch (request->kind) {
        case GUIDE_KIND_SIGNIN: page = PAGE_SIGNIN; break;
        case GUIDE_KIND_MESSAGE_BOX: page = PAGE_MESSAGE_BOX; break;
        case GUIDE_KIND_DEVICE_SELECTOR: page = PAGE_DEVICES; break;
        case GUIDE_KIND_BLADE: page = PAGE_BLADE; blade = request->blade; break;
        default: break;
        }
        push_page(i, page, blade, &fx);
        if (page == PAGE_MESSAGE_BOX && request->focus < copy_of->button_count)
            pane->stack[0].selected = (int)request->focus;
        if (page == PAGE_MENU && (request->flags & GUIDE_MENU_INVITE) != 0) {
            // the newest invite, over Messages (B goes back to the list, then the menu)
            port_invite newest;
            push_page(i, PAGE_BLADE, GUIDE_BLADE_MESSAGES, &fx);
            if (port_invite_inbox(&newest, 1) == 1) {
                pane->invite = newest;
                push_page(i, PAGE_INVITE, 0, &fx);
            }
        }
    }
    const unsigned long session = g_guide.session;
    // In-game when the game's renderer can draw it (the game window's keys and mouse reach it while it shows; decided
    // under the lock, so no later opening's decision is overwritten by this one's), else the guide's own window;
    // without a window the controllers operate it.
    const int ingame = guide_ingame_show(session);
    if (ingame && port_window_native_window() != 0)
        g_guide.windowed = 1;
    changed(&fx);
    guide_unlock();
    if (!ingame && guide_host_show(session)) {
        guide_lock();
        if (g_guide.session == session)
            g_guide.windowed = 1;
        guide_unlock();
    }
    run_effects(&fx);
    return GUIDE_ERROR_SUCCESS;
}

int guide_button(unsigned long user)
{
    if (user >= GUIDE_PANES || !guide_interactive() || guide_showing())
        return 0;
    // Ignore Guide button presses during boot splash / loading screens before the in-game UI renderer is drawable
    // (never fall back to the separate host Guide window unless HALO3_GUIDE_RENDER=window).
    if (guide_ingame_wanted() && !guide_ingame_live())
        return 0;
    guide_request request;
    __builtin_memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_MENU;
    request.user = user;
    if (guide_toast_invite_showing()) // the console's Guide button opens the notification it shows
        request.flags |= GUIDE_MENU_INVITE;
    const int opened = guide_open(&request) == GUIDE_ERROR_SUCCESS;
    if (opened) // as Start opens the start menu
        guide_sound_post(GUIDE_SOUND_START_BUTTON);
    return opened;
}

// Halo 3's order for buttons that go down at one poll (event_manager_update): the first acts, the others are lost, and
// the stick and D-pad wait for a poll without one. -1: a button the guide has no use for (it still takes the poll).
static const struct {
    unsigned long button;
    int key;
} k_button_order[] = {
    { GUIDE_PAD_A, GUIDE_KEY_ACCEPT },
    { GUIDE_PAD_B, GUIDE_KEY_BACK },
    { GUIDE_PAD_X, OSK_ERASE },
    { GUIDE_PAD_Y, OSK_SHIFT },
    { GUIDE_PAD_LEFT_SHOULDER, GUIDE_KEY_PREVIOUS_TAB },
    { GUIDE_PAD_RIGHT_SHOULDER, GUIDE_KEY_NEXT_TAB },
    { GUIDE_PAD_START, OSK_SUBMIT },
    { GUIDE_PAD_BACK, -1 },
    { GUIDE_PAD_LEFT_TRIGGER, GUIDE_KEY_PAGE_UP },
    { GUIDE_PAD_RIGHT_TRIGGER, GUIDE_KEY_PAGE_DOWN },
    { GUIDE_PAD_LEFT_THUMB, -1 },
    { GUIDE_PAD_RIGHT_THUMB, -1 },
};

// The D-pad's directions down in `state` (a poll's buttons) at `now`: one that went down now is fresh.
static void dpad_poll(guide_dpad *dpad, unsigned long state, unsigned long now)
{
    static const unsigned long k_bits[k_dpad_count] = { GUIDE_PAD_UP, GUIDE_PAD_DOWN, GUIDE_PAD_LEFT, GUIDE_PAD_RIGHT };
    for (int d = 0; d < k_dpad_count; d++) {
        const int down = (state & k_bits[d]) != 0;
        if (down && !dpad->down[d]) {
            dpad->fresh[d] = 1;
            dpad->since[d] = now;
        }
        dpad->down[d] = (unsigned char)down;
    }
}

void guide_controller(unsigned long user, unsigned short buttons, unsigned char left_trigger,
    unsigned char right_trigger, short thumb_lx, short thumb_ly)
{
    if (user >= GUIDE_PANES || !guide_showing())
        return;
    const unsigned long now = GetTickCount();
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    keys_step(now, &fx); // a held arrow key repeats on the polls
    guide_pane *pane = &g_guide.panes[user];
    if (g_guide.open && pane->active) {
        unsigned long state = buttons;
        if (trigger_down(&pane->trigger_threshold[0], left_trigger))
            state |= GUIDE_PAD_LEFT_TRIGGER;
        if (trigger_down(&pane->trigger_threshold[1], right_trigger))
            state |= GUIDE_PAD_RIGHT_TRIGGER;
        const long stick_x = guide_stick_axis(thumb_lx), stick_y = guide_stick_axis(thumb_ly);
        const unsigned long previous = pane->buttons;
        pane->buttons = state;
        g_guide.request.connected |= 1ul << user;
        if (previous == GUIDE_BUTTONS_UNKNOWN) {
            // The first poll only learns what is held already: a button or D-pad direction acts once let go and
            // pressed again, and a stick held over must come back first (it is the step before, and nothing has
            // been given for it to repeat).
            __builtin_memset(&pane->dpad, 0, sizeof(pane->dpad));
            __builtin_memset(&pane->repeat, 0, sizeof(pane->repeat));
            pane->stale = state & GUIDE_PAD_DPAD;
            pane->repeat.previous_x = magnitude(stick_x) >= GUIDE_STICK_THRESHOLD ? stick_x : 0;
            pane->repeat.previous_y = magnitude(stick_y) >= GUIDE_STICK_THRESHOLD ? stick_y : 0;
            pane->repeat.active = pane->repeat.previous_x != 0 || pane->repeat.previous_y != 0;
        } else {
            pane->stale &= state; // a direction held over is fresh again once let go
            dpad_poll(&pane->dpad, state & ~pane->stale, now);
            const unsigned long pressed = state & ~previous;
            if ((pressed & GUIDE_PAD_GUIDE) != 0) {
                act((int)user, GUIDE_KEY_CLOSE, 0, 0, &fx);
            } else {
                int key = -2; // -2: no button went down
                for (unsigned i = 0; i < sizeof(k_button_order) / sizeof(k_button_order[0]) && key == -2; i++)
                    if ((pressed & k_button_order[i].button) != 0)
                        key = k_button_order[i].key;
                if (key == -2) {
                    key = direction_step(&pane->repeat, &pane->dpad, stick_x, stick_y, now);
                } else {
                    for (int d = 0; d < k_dpad_count; d++)
                        pane->dpad.fresh[d] = 0; // a D-pad press at the same poll is lost, as the game's
                }
                if (key >= 0)
                    act((int)user, key, 0, 0, &fx);
            }
        }
    }
    guide_unlock();
    run_effects(&fx);
}

void guide_controller_lost(unsigned long user)
{
    if (user >= GUIDE_PANES || !guide_showing())
        return;
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    if (g_guide.open) {
        g_guide.request.connected &= ~(1ul << user);
        g_guide.panes[user].buttons = GUIDE_BUTTONS_UNKNOWN;
        // its player is gone and nobody else can answer it (with a window, its keys still can)
        if (g_guide.request.kind != GUIDE_KIND_SIGNIN && user == g_guide.request.user && !g_guide.windowed)
            end_request(&fx, GUIDE_RESULT_CANCELLED, 0);
    }
    guide_unlock();
    run_effects(&fx);
}

// Whether the focused pane shows a text field (its typing takes the host's own key repeat).
static int focus_has_field(void)
{
    const guide_pane *pane = &g_guide.panes[g_guide.focus];
    return g_guide.open && pane->active && pane->depth > 0 && text_page(pane->stack[pane->depth - 1].page);
}

void guide_key_action(int key, unsigned long character, int action)
{
    const unsigned long now = GetTickCount();
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    const int d = dpad_of_key(key);
    if (d >= 0) { // an arrow: the window's D-pad (its repeat is the guide's, at Halo 3's pace)
        if (action == GUIDE_KEY_PRESSED && g_guide.keys.down[d]) {
            // pressed while down: the host saw it go up (its release lost), so it was let go first
            g_guide.keys.down[d] = 0;
            keys_step(now, &fx);
        }
        if (action == GUIDE_KEY_PRESSED) {
            g_guide.keys.down[d] = 1;
            g_guide.keys.fresh[d] = 1;
            g_guide.keys.since[d] = now;
        } else if (action == GUIDE_KEY_RELEASED) {
            g_guide.keys.down[d] = 0;
        }
        if (action != GUIDE_KEY_REPEATED)
            keys_step(now, &fx);
    } else if (action == GUIDE_KEY_PRESSED ||
               (action == GUIDE_KEY_REPEATED && (key == GUIDE_KEY_CHARACTER || key == GUIDE_KEY_ERASE) &&
                focus_has_field())) {
        act(g_guide.focus, key, character, 1, &fx);
    }
    guide_unlock();
    run_effects(&fx);
}

PORT_MS_ABI void guide_key(int key, guide_view_word character) { guide_key_action(key, character, GUIDE_KEY_PRESSED); }

PORT_MS_ABI int guide_capture(int code)
{
    if (!guide_showing())
        return 0;
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    int taken = 0;
    if (g_guide.open && !g_guide.working && g_guide.capture.active && code > PORT_KBM_NONE && code < PORT_KBM_CODE_COUNT) {
        const int pane_index = g_guide.capture.pane, action = g_guide.capture.action, slot = g_guide.capture.slot;
        guide_pane *pane = &g_guide.panes[pane_index];
        g_guide.capture.active = 0;
        taken = 1;
        int sound = GUIDE_SOUND_A_BUTTON;
        if (code == k_port_key_home || code == k_port_key_grave) { // the Guide button: it closes the guide (not taken)
            taken = 0;
        } else if (code == k_port_key_escape) {
            copy(pane->notice, sizeof(pane->notice), "Not changed.");
            sound = GUIDE_SOUND_B_BUTTON;
        } else {
            const int input = code == k_port_key_delete ? PORT_KBM_NONE : code;
            port_kbm_bindings bindings;
            port_config_kbm_bindings(&bindings);
            port_kbm_assign_result result;
            if (!port_kbm_assign(&bindings, action, slot, input, &result)) {
                copy(pane->notice, sizeof(pane->notice), port_kbm_code_label(input));
                append(pane->notice, sizeof(pane->notice), " is the pause menu's: not changed.");
                sound = GUIDE_SOUND_ERROR;
            } else {
                if (result.changed && port_config_set_kbm_bindings(&bindings) > 0)
                    settings_changed(pane_index, &fx); // saved to halo3.ini (clears the notice: set below)
                copy(pane->notice, sizeof(pane->notice), port_kbm_action_label(action));
                append(pane->notice, sizeof(pane->notice), slot ? " (alternate): " : ": ");
                append(pane->notice, sizeof(pane->notice), input != PORT_KBM_NONE ? port_kbm_code_label(input) : "none");
                append(pane->notice, sizeof(pane->notice), ".");
                if (result.other >= 0) {
                    append(pane->notice, sizeof(pane->notice), " ");
                    append(pane->notice, sizeof(pane->notice), port_kbm_action_label(result.other));
                    append(pane->notice, sizeof(pane->notice), " had it: now ");
                    append(pane->notice, sizeof(pane->notice), result.other_code != PORT_KBM_NONE
                                                                   ? port_kbm_code_label(result.other_code)
                                                                   : "none");
                    append(pane->notice, sizeof(pane->notice), ".");
                }
            }
        }
        if (trace_on()) {
            port_write_error("guide: capture ");
            port_write_error(port_kbm_code_name(code));
            port_write_error(" for ");
            port_write_error(port_kbm_action_key(action));
            port_write_error(taken ? "\n" : " (the Guide button: closes)\n");
        }
        changed(&fx);
        if (taken)
            guide_sound_post(sound);
    }
    guide_unlock();
    run_effects(&fx);
    return taken;
}

void guide_keys_released(void)
{
    guide_lock();
    keys_reset();
    guide_unlock();
}

// A click or the pointer on item `item` of the `pane`-th pane shown (-1: the pane only): the window's keys to that
// pane, the item selected (with the move's sound when `hover` changes it), and chosen when `choose_it`.
static void pointer_at(int pane, int item, int choose_it, int hover)
{
    guide_effects fx;
    effects_init(&fx);
    guide_lock();
    int index = -1;
    for (int i = 0, shown = 0; i < GUIDE_PANES && index < 0; i++)
        if (g_guide.panes[i].active && shown++ == pane)
            index = i;
    if (g_guide.open && !g_guide.working && index >= 0) {
        const int count = build_items(index, g_items);
        if (index != g_guide.focus) {
            g_guide.focus = index;
            changed(&fx);
        }
        if (item >= 0 && item < count) {
            if (select_item(index, item, count, &fx) && hover)
                guide_sound_post(GUIDE_SOUND_VERTICAL_NAVIGATION);
            if (choose_it)
                choose(index, &fx);
        }
    }
    guide_unlock();
    run_effects(&fx);
}

void guide_click(int pane, int item, int choose_it) { pointer_at(pane, item, choose_it, 0); }

void guide_hover(int pane, int item) { pointer_at(pane, item, 0, 1); }
