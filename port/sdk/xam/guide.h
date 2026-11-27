// port/sdk/xam: the Guide (guide.c), the console's system UI over the title (the Xbox Guide; the player sees "Guide"):
// the Guide button's menu (profiles and guests, achievements, friends, messages and game invites, recent players, lobby
// codes, sign out) and the pages titles open through XShow* calls (sign-in panes, message boxes, the device selector,
// achievements, friends, game invites, gamercards, player review, ...). Game invites and lobby codes are
// port/sdk/online/invite.h's; the host clipboard they use is clipboard.h.
//
// The menu's Settings page (no sign-in needed) changes the game's own settings, saved to halo3.ini
// (port/sdk/runtime/port_config.h): the window mode, the resolution scale, the frame rate cap, vsync and System Link. A
// setting's value cycles with D-pad / arrow left and right (A, Enter or a click: the next value); the window mode, the
// frame rate cap and vsync apply at once, the others show "restart required" until the game starts again; a setting
// the launch set (its HALO3_* variable) cannot be changed. Reset to Defaults puts the page's settings back. Its
// Keyboard & Mouse row opens a page of its own ([input] and [bindings]): Preset (MCC Modern / MCC Classic: choosing one
// applies its bindings), Mouse Sensitivity (0.1 steps), Raw Mouse, Invert Mouse Y, Crouch Mode (Hold / Toggle), all
// applied at once, then a row per binding (port/sdk/input/kbm_bindings.h): left / right pick its key or its alternate,
// A / Enter waits for the next key or mouse button pressed (guide_capture; Escape cancels, Delete unbinds), which an
// action that cannot share it gives up in exchange for the old one, and Reset to MCC Defaults puts the preset's back.
//
// One guide dialog shows at a time, the virtual keyboard included (xam_ui.h): opening one posts XN_SYS_UI (1) and
// closing it XN_SYS_UI (0), and while it shows the title reads every controller as idle (xam.c). It is operated two ways
// at once, as on a console:
//   - controllers: each poll of a controller (XamInputGetState) passes its buttons, triggers and left stick to the
//     guide, which acts as Halo 3's menus do (below): D-pad / left stick up and down move the selection, A chooses, B
//     goes back (closing the guide from its first page), LB / RB switch the page (a blade the menu leads to: the one
//     before / after it), LT / RT page up / down, the Guide button closes it. A sign-in page has a pane per controller
//     (one pane: the asking controller's), each driven by its own controller; other pages by the controller that opened
//     them;
//   - the game window's keyboard and mouse, when the game has a window: the guide is drawn into the game's frame by
//     Halo 3's own UI code (guide_ingame.h, HALO3_GUIDE_RENDER=ingame, the default) or, with HALO3_GUIDE_RENDER=window
//     and until the game has drawn a frame, in a window of its own over the game's (guide_host.h). The keys are a
//     controller's: the arrows the D-pad, Enter (and Space outside a text field) A, Escape (and Backspace outside a text
//     field) B, Tab / Shift+Tab (and E / Q outside a text field) RB / LB, Page Up / Page Down LT / RT, Home the Guide
//     button; with several panes, Left / Right and Tab / Shift+Tab move between them.
// Halo 3's menu feel (tu/interfacelibxbox: user_interface_controller.cpp, c_gui_list_widget.cpp, c_gui_screen_widget.cpp,
// user_interface_window_manager.cpp):
//   - one press, one action: a button acts on the poll it goes down in, the first in Halo 3's order (A, B, X, Y, LB,
//     RB, Start, Back, LT, RT) when several do, and the stick and D-pad wait for a poll without one;
//   - hold to repeat (event_manager_update / event_manager_tab): the left stick past 15000 acts at once, then every
//     hold_delay until repeat_delay has passed, then every repeat_interval; a D-pad direction (or an arrow key) acts at
//     once, again once held for hold_delay, then repeats the same way. Two directions at once, the stick back within
//     15000 or the D-pad released stop it. The delays are the user interface shared globals' (325, 500 and 100 ms in
//     retail's tag; guide_menu_timing) in real time, each repeat timed from when it was due, not from the poll that
//     noticed it, so the cadence is the same at any frame rate;
//   - lists wrap, as Halo 3's menus' (their list widgets' "wraps" flag, set on the main menu's and the start menu's);
//     LT / RT go to the top of the rows shown, then to the first item (the bottom, then the last: handle_tab_direction);
//   - sounds (guide_sound_take): each action plays the sound Halo 3's window manager plays for its input
//     (window_manager_play_input_sound), and only when it did something (c_gui_screen_widget::process_message): a move
//     vertical_navigation, a change of pane horizontal_navigation, A a_button, B b_button, LB / RB and LT / RT their
//     bumper and trigger sounds, the Guide button start_button (it opens and closes the guide as Start does the start
//     menu); A on a disabled item, a character a field refuses and an action that fails play error, as
//     handle_focused_widget_selected does. A toast that pops in plays the rank-up sound (an achievement) or the
//     party-up sound (a game invite).
// When nobody can operate it (HALO3_GUIDE, below) a request completes at once instead, as the user would most likely
// answer it: the sign-in UI signs in the users of connected controllers, a message box takes its focus button, the
// device selector the hard drive, and the other pages open and close. HALO3_GUIDE=interactive / auto chooses; unset,
// the guide is interactive when the game has a window (HALO3_WINDOW=1) and automatic without one. Headless runs can
// operate an interactive guide through HALO3_INPUT_SCRIPT ("home" is the Guide button). HALO3_GUIDE_TRACE=1 writes each
// page the guide shows to stderr as text.
#ifndef PORT_XAM_GUIDE_H
#define PORT_XAM_GUIDE_H

#ifdef __cplusplus
extern "C" {
#endif

#define GUIDE_PANES 4
#define GUIDE_ITEMS 96
#define GUIDE_BUTTONS 3     // XMB_MAXBUTTONS
#define GUIDE_DEVICES 4
#define GUIDE_TEXT 1024     // UTF-8 bytes of a message box's text, NUL included
#define GUIDE_LABEL 64      // UTF-8 bytes of a title, button, item or detail, NUL included
#define GUIDE_FIELD 192     // UTF-8 bytes of a text field (a gamertag, a lobby code), NUL included
#define GUIDE_TABS 5        // the blades LB / RB switch between at most

// What a request opens.
enum guide_kind {
    GUIDE_KIND_MENU,            // the Guide button, XShowGuideUI
    GUIDE_KIND_SIGNIN,          // XShowSigninUI
    GUIDE_KIND_MESSAGE_BOX,     // XShowMessageBoxUI
    GUIDE_KIND_DEVICE_SELECTOR, // XShowDeviceSelectorUI (when there is a choice to make, or the title forces it)
    GUIDE_KIND_BLADE,           // one of the guide's pages, directly
};

// The guide's pages a title can open (GUIDE_KIND_BLADE) and the menu leads to.
enum guide_blade {
    GUIDE_BLADE_ACHIEVEMENTS,   // XShowAchievementsUI
    GUIDE_BLADE_FRIENDS,        // XShowFriendsUI
    GUIDE_BLADE_PLAYERS,        // XShowPlayersUI: recent players
    GUIDE_BLADE_MESSAGES,       // XShowMessagesUI
    GUIDE_BLADE_COMPOSE,        // XShowMessageComposeUI, XShowCustomMessageComposeUI
    GUIDE_BLADE_INVITE,         // XShowGameInviteUI
    GUIDE_BLADE_FRIEND_REQUEST, // XShowFriendRequestUI
    GUIDE_BLADE_GAMERCARD,      // XShowGamerCardUI
    GUIDE_BLADE_REVIEW,         // XShowPlayerReviewUI
    GUIDE_BLADE_SETTINGS,       // the game's settings (halo3.ini: port/sdk/runtime/port_config.h); the menu's only
    GUIDE_BLADE_COUNT
};

// How a request ended (guide_request::done).
enum guide_result {
    GUIDE_RESULT_CHOSEN = 0,    // a button or device was chosen: `value` is its index / the device id
    GUIDE_RESULT_CANCELLED = 1, // B, Escape, the Guide button, the window closed
};

// The request crosses the two i686 ABIs the port is built with: guide_ui.cpp is built like the game (MSVC layout, a
// 64-bit member 8-aligned) and guide.c as host code (System V, 4-aligned). Every 64-bit member is placed at an offset
// that is a multiple of 8 and every size is one, so both lay the structures out alike; the checks below hold in both.
typedef struct guide_device {
    unsigned long long free_bytes;
    unsigned int id;                  // XCONTENTDEVICEID
    int selectable;                   // has the space the title asked for
    char name[GUIDE_LABEL];           // UTF-8
} guide_device;

// guide_request::flags of a GUIDE_KIND_MENU request: open on the newest game invite received (the Guide button while a
// Game Invite toast shows, as the console's Guide opens the notification it shows), over the menu and Messages.
#define GUIDE_MENU_INVITE 0x00000001ul

typedef struct guide_request guide_request;
struct guide_request {
    // GUIDE_KIND_BLADE: the player of a gamercard, review or friend request
    unsigned long long xuid;
    int kind;                         // guide_kind
    unsigned int user;                // the controller that opened it (0..3)
    // GUIDE_KIND_SIGNIN: XShowSigninUI's cPanes and dwFlags (GUIDE_KIND_MENU: GUIDE_MENU_*), and a bit per controller
    // connected now (the panes that
    // wait for a player: the sign-in UI closes once cPanes users signed in through it or every connected controller's
    // user is signed in)
    unsigned int panes, flags, connected;
    // GUIDE_KIND_MESSAGE_BOX: at most GUIDE_BUTTONS buttons ("OK" when none), the one focused, XMB_NOICON ..
    unsigned int button_count, focus, icon;
    // GUIDE_KIND_DEVICE_SELECTOR
    unsigned int device_count;
    // GUIDE_KIND_BLADE
    int blade;                        // guide_blade
    // Called once when the request ends (after XN_SYS_UI (0) is posted), on the thread that ended it, or on the
    // calling thread before guide_open returns when it completes at once: result guide_result, value its value.
    void (PORT_MS_ABI *done)(guide_request *request, int result, unsigned long value);
    void *context;                    // the caller's
    guide_device devices[GUIDE_DEVICES];
    // GUIDE_KIND_MESSAGE_BOX (UTF-8), and the title of any request
    char title[GUIDE_LABEL];
    char text[GUIDE_TEXT];
    char buttons[GUIDE_BUTTONS][GUIDE_LABEL];
};
typedef char guide_device_layout[sizeof(guide_device) == 80 ? 1 : -1];
typedef char guide_request_layout[
    __builtin_offsetof(guide_request, devices) == (sizeof(void *) == 8 ? 64 : 56)
    && sizeof(guide_request) == (sizeof(void *) == 8 ? 1664 : 1656) ? 1 : -1];

// The guide's dialogs need a person: whether they are operated (HALO3_GUIDE; see above). Read at the first call.
int guide_interactive(void);
// Opens the guide for `request` (copied; `done` is called with the copy). Returns 0 (ERROR_SUCCESS) once the request
// is open or has completed at once (non-interactive), ERROR_ACCESS_DENIED (5) when a guide dialog shows already, or
// ERROR_INVALID_PARAMETER (87) for a request that cannot be shown.
PORT_MS_ABI unsigned long guide_open(const guide_request *request);
// The Guide button pressed on controller `user` (xam.c): opens the menu when nothing shows and the guide is interactive,
// closes the guide when it shows and the press is one of its controllers'. Returns whether the press was used.
int guide_button(unsigned long user);
// Whether the guide shows (an interactive request is open).
PORT_MS_ABI int guide_showing(void);
// The opening of the guide that shows (guide_view::session); 0 when it is closed.
unsigned long guide_showing_session(void);
// A poll of controller `user` while the guide shows: its XINPUT_GAMEPAD buttons (the Guide button 0x0400 included),
// triggers and left stick. Presses (changes from the previous poll) act, and held directions repeat (see the top); each
// poll also repeats a held arrow key of the window's (guide_key_action).
void guide_controller(unsigned long user, unsigned short buttons, unsigned char left_trigger,
    unsigned char right_trigger, short thumb_lx, short thumb_ly);
// Controller `user` is not connected: a guide its player opened closes (cancelled), unless a window shows it.
void guide_controller_lost(unsigned long user);
// XEnableGuestSignin: whether the sign-in panes offer guests even without XSSUI's guest flag (0x00080000).
PORT_MS_ABI void guide_enable_guest_signin(int enable);

// ---- the window's side (guide_host.h): what to draw, and the keys and mouse ----

// The window's keys, as the controller's buttons they stand for (see the top).
enum guide_key {
    GUIDE_KEY_UP, GUIDE_KEY_DOWN, GUIDE_KEY_LEFT, GUIDE_KEY_RIGHT, // the D-pad: held, they repeat
    GUIDE_KEY_PAGE_UP,      // Page Up: LT
    GUIDE_KEY_PAGE_DOWN,    // Page Down: RT
    GUIDE_KEY_ACCEPT,       // Enter (Space outside a text field): A
    GUIDE_KEY_BACK,         // Escape (Backspace outside a text field): B
    GUIDE_KEY_ERASE,        // Backspace in a text field
    GUIDE_KEY_NEXT_TAB,     // Tab (E outside a text field): RB
    GUIDE_KEY_CLOSE,        // Home (the Guide button), the window's close button
    GUIDE_KEY_CHARACTER,    // a character typed (into a text field; outside one Space is A, Q LB and E RB)
    GUIDE_KEY_PASTE,        // Ctrl+V, Shift+Insert: the clipboard's text into a text field (clipboard.h)
    GUIDE_KEY_PREVIOUS_TAB, // Shift+Tab (Q outside a text field): LB
    GUIDE_KEY_SCROLL_UP,    // the mouse wheel: the selection moves without wrapping round the list's end
    GUIDE_KEY_SCROLL_DOWN,
    GUIDE_KEY_VALUE_PREVIOUS, // a click on a setting's left / right mark: its value steps once (left / right that
    GUIDE_KEY_VALUE_NEXT,     // never repeat: no release follows a click)
};

// What happened to a key (guide_key_action).
enum guide_key_action {
    GUIDE_KEY_PRESSED,  // went down
    GUIDE_KEY_REPEATED, // the host's own auto-repeat of a key held down: only typing takes it (a character, Backspace
                        // in a text field); the arrows repeat at Halo 3's pace instead, and nothing else repeats
    GUIDE_KEY_RELEASED, // went up (an arrow stops repeating)
};

// guide_view_item::setting: the item is one of the Settings page's settings (its detail is its value: left / right and A
// change it), and whether a change waits for the next launch.
enum guide_setting_state {
    GUIDE_SETTING_NONE,         // not a setting
    GUIDE_SETTING_VALUE,        // a setting: the in-game guide shows its value in capitals, as Halo 3's option lists
    GUIDE_SETTING_RESTART,      // ... changed since the game started, and applied only the next time it starts
};

typedef struct guide_view_item {
    unsigned int image_id;      // SPA image ID (achievement icon), 0 for none
    char text[GUIDE_LABEL];
    char detail[GUIDE_LABEL];   // a right-aligned note ("Signed in", "20 G"), a setting's value ("Fullscreen")
    unsigned char enabled, checked;
    unsigned char name;         // the text is a player's name (a gamertag), shown as it is; else a menu option, which
                                // the in-game guide shows in capitals, as Halo 3's menus show theirs
    unsigned char setting;      // guide_setting_state
} guide_view_item;
typedef char guide_view_item_layout[sizeof(guide_view_item) == 2 * GUIDE_LABEL + 8 ? 1 : -1]; // no 64-bit member: alike in both ABIs

typedef struct guide_view_pane {
    char heading[GUIDE_LABEL];  // "Controller 2"
    char status[GUIDE_LABEL];   // "Player 1 (1)", "Not signed in"
    char title[GUIDE_LABEL];    // the page
    char body[GUIDE_TEXT];      // text over the list ('\n' breaks lines; the host wraps)
    char field[GUIDE_FIELD];    // a text field's text (has_field)
    char hint[GUIDE_TEXT / 4];  // under the list: the selected item's description, a notice, the keys
    guide_view_item items[GUIDE_ITEMS];
    int item_count, selected;   // selected: -1 none
    int has_field, focused;
    int field_readonly;         // the field shows text to read (a lobby code, '\n' between its lines), not to type
    int depth;                  // pages over the request's first: 0 when going back closes the guide
    // LB / RB (Shift+Tab / Tab) switch the page between these (the blades the menu leads to that it can open now, in
    // its order), the one shown tab_current; tab_count 0 when they switch none
    int tab_count, tab_current;
    char tabs[GUIDE_TABS][GUIDE_LABEL];
} guide_view_pane;

// Shared by the host and the game renderer. Keep the counters 32-bit across MSVC and SysV.
typedef struct guide_view {
    int open;                   // 0: the guide closed; the window goes
    unsigned int session;       // one per opening of the guide (guide_host_show)
    unsigned int revision;      // changes with anything shown
    unsigned int page_serial;   // changes when a pane opens, closes or shows another page (guide_page_serial)
    char caption[GUIDE_LABEL];  // the window's title
    int pane_count;
    guide_view_pane panes[GUIDE_PANES];
} guide_view;
typedef char guide_view_layout[
    __builtin_offsetof(guide_view, pane_count) == 80 && __builtin_offsetof(guide_view, panes) == 84
    && sizeof(guide_view) == 60372 ? 1 : -1];

// The view counter and key code points cross between the game and host.
#if defined(HALO3_64BIT) || (defined(__x86_64__) && !defined(_WIN32))
typedef unsigned int guide_view_word;
#else
typedef unsigned long guide_view_word;
#endif

// The guide as it shows now. Returns view->revision.
PORT_MS_ABI guide_view_word guide_view_get(guide_view *view);
// What changes with anything shown: a host redraws when it differs from the view it drew.
unsigned long guide_revision(void);
// What changes when a pane opens, closes or goes to another page (not with a selection or a field's text): where a
// host drew a page's parts holds while it is the same.
unsigned long guide_page_serial(void);
// A key of the guide's window, for the pane it focuses (`character`: the code point of GUIDE_KEY_CHARACTER): `action`
// guide_key_action's. Any thread; the hosts pass presses, their own repeats (as such) and releases. A held arrow
// repeats on the controllers' polls (guide_controller): the game polls them every frame, controller 0 (the keyboard's)
// whenever there is a keyboard.
void guide_key_action(int key, unsigned long character, int action);
// A key pressed (guide_key_action's GUIDE_KEY_PRESSED): the mouse's wheel and buttons, the window's close button.
PORT_MS_ABI void guide_key(int key, guide_view_word character);
// A key or mouse button pressed on the game window or the guide's own (`code`: a port_kbm_code, a wheel notch
// PORT_KBM_WHEEL_UP / DOWN; port/sdk/input/kbm_bindings.h), offered first, before it is the guide's key: the
// Keyboard & Mouse page waiting for a binding's new input takes it (returns 1, and nothing else may act on that
// press: the hosts drop the character it types too). Escape cancels, Delete unbinds; Home (the Guide button) cancels
// and is not taken (it closes the guide). Returns 0 when nothing waits.
PORT_MS_ABI int guide_capture(int code);
// Every key of the window's is up (its focus lost, the guide shown or hidden there): no arrow repeats.
void guide_keys_released(void);
// A click on item `item` of pane `pane` (-1: the pane only, focused): selects it, and chooses it when `choose`.
void guide_click(int pane, int item, int choose);
// The pointer moved onto item `item` of pane `pane`: selects it, with the sound of a move when the selection changes.
void guide_hover(int pane, int item);

// ---- Halo 3's menu sounds and timing (the game's UI update: port/probe/extra/guide_menu.cpp) ----

// Halo 3's menu sounds (e_user_interface_sound_effect, include/interface/user_interface_sound_effect_types.h: the user
// interface sound effects tag's references, in order; guide_menu.cpp checks them against the game's).
enum guide_sound {
    GUIDE_SOUND_ERROR = 0,
    GUIDE_SOUND_VERTICAL_NAVIGATION = 1,
    GUIDE_SOUND_HORIZONTAL_NAVIGATION = 2,
    GUIDE_SOUND_A_BUTTON = 3,
    GUIDE_SOUND_B_BUTTON = 4,
    GUIDE_SOUND_START_BUTTON = 7,
    GUIDE_SOUND_LEFT_BUMPER = 9,
    GUIDE_SOUND_RIGHT_BUMPER = 10,
    GUIDE_SOUND_LEFT_TRIGGER = 11,
    GUIDE_SOUND_RIGHT_TRIGGER = 12,
    GUIDE_SOUND_RANK_UP = 18,  // the Spartan rank dialog's (gui_screen_spartan_rank_dialog_initialize): a promotion
    GUIDE_SOUND_PARTY_UP = 19, // the postgame lobby's, as a player joins the rematch party
};
#define GUIDE_SOUNDS 16                  // waiting to play at most; more are dropped
#define GUIDE_SOUND_STALE_MILLISECONDS 250ul // one that waited longer (no UI update ran: a map load) is dropped
// Queues `sound` (guide_sound) to play at the game's next UI update. Any thread.
void guide_sound_post(int sound);
// The game's UI update (its thread, where Halo 3's menus play theirs), at `now` (GetTickCount): the next sound to play,
// -1 when none waits (those older than GUIDE_SOUND_STALE_MILLISECONDS are dropped).
PORT_MS_ABI int guide_sound_take(unsigned long now);

// The menus' hold-to-repeat timing in milliseconds (the user interface shared globals' hold_delay, repeat_delay and
// repeat_interval), passed on by the game's UI update; until it does, retail's tag's.
#define GUIDE_HOLD_DELAY_DEFAULT 325ul
#define GUIDE_REPEAT_DELAY_DEFAULT 500ul
#define GUIDE_REPEAT_INTERVAL_DEFAULT 100ul
PORT_MS_ABI void guide_menu_timing(unsigned long hold_delay, unsigned long repeat_delay, unsigned long repeat_interval);

// ---- what the guide reads from the rest of the port ----

// The title's achievements as user `user_index` has them (port/sdk/online/xlive_offline.cpp, from the title's SPA and
// the profile's saves). Returns how many were written to `out` (at most `capacity`), all of them for a profile, 0 for
// a user not signed in.
typedef struct guide_achievement {
    unsigned int id, gamerscore;
    unsigned int image_id;          // SPA image ID, 0 for none
    int earned;
    char label[GUIDE_LABEL];        // UTF-8
    char description[GUIDE_LABEL * 3];
} guide_achievement;
typedef char guide_achievement_layout[sizeof(guide_achievement) == 272 ? 1 : -1]; // no 64-bit member: alike in both ABIs
PORT_MS_ABI unsigned long port_guide_achievements(unsigned long user_index, guide_achievement *out, unsigned long capacity);

#ifdef __cplusplus
}
#endif

#endif
