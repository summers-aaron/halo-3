// port/sdk/xam (both hosts): the guide drawn inside the game's frame (guide_ingame.c), the default way the guide shows.
//
// On the console the Xbox Guide is the system's own UI, composited over the title's frame; the title keeps running
// underneath. The port draws it the same way: once per presented frame, over the finished frame, with the game's own UI
// renderer, fonts and button glyphs (port/probe/extra/guide_ingame_ui.cpp, called from c_rasterizer::end_frame). The
// renderer reads the guide's view (guide.h) here and gives back where it drew what (the hit regions), which the game
// window's mouse is tested against. HALO3_GUIDE_RENDER chooses how the guide shows:
//   ingame  (default)  in the game's frame, while the game's renderer can draw it (an opening before that, at
//                      start-up, or while it cannot, shows in the guide's own window instead: guide_host.h);
//   window             the guide's own OS window over the game window (guide_linux.c / guide_windows.c).
// While the in-game guide shows, the game window's keys and mouse go to it instead of the game (window_linux.c,
// window_windows.c), as the guide's own window would take them: guide_ingame_key / guide_ingame_pointer. Headless runs
// reach the same entries through HALO3_INPUT_SCRIPT's window entries (port/sdk/input/keyboard_mouse.c).
// HALO3_GUIDE_SHOW="<page>@<ms>,..." (diagnostics) opens sample requests for the pages Halo 3 never opens itself:
// messagebox, devices, achievements, friends, players, messages, settings; and makes two things happen that otherwise
// take a campaign mission or a second PC: "achievement" earns user 0 the first achievement its profile lacks, through
// XUserWriteAchievements (the Achievement Unlocked toast follows), "invite" delivers a sample game invite to the
// inbox, as Live's poll would (the Game Invite toast follows) (guide_ingame.c).
//
// The renderer uses MSVC layout and the Linux host uses System V layout. The shared view, hit regions and toast
// contain only 32-bit scalars and byte arrays so their layouts match on both targets.
#ifndef PORT_XAM_GUIDE_INGAME_H
#define PORT_XAM_GUIDE_INGAME_H

#include "guide.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---- the renderer's side (the thread that presents the game's frames) ----

// What a hit region does under the mouse.
enum guide_ingame_target {
    GUIDE_TARGET_PANE,        // a pane's area: a click focuses it, the wheel scrolls its list
    GUIDE_TARGET_ITEM,        // an item of a pane's list: hovering selects it, a click chooses it
    GUIDE_TARGET_ACCEPT,      // the "Select" hint: Enter
    GUIDE_TARGET_BACK,        // the "Back" hint: Escape
    GUIDE_TARGET_NEXT_PANE,   // the "Next controller" hint: Tab (RB)
    GUIDE_TARGET_CLOSE,       // the "Close" hint: the Guide button
    GUIDE_TARGET_SCROLL_UP,   // a list's "more above" mark
    GUIDE_TARGET_SCROLL_DOWN, // a list's "more below" mark
    GUIDE_TARGET_PREVIOUS_TAB, // the LB hint: the blade before (Shift+Tab)
    GUIDE_TARGET_NEXT_TAB,     // the RB hint: the blade after (Tab)
    GUIDE_TARGET_VALUE_PREVIOUS, // a selected setting's left mark: its value before (an item's: hovering selects it)
    GUIDE_TARGET_VALUE_NEXT,     // ... right mark: its value after
};

#define GUIDE_INGAME_HITS 160

typedef struct guide_ingame_hit {
    float x0, y0, x1, y1;     // in the frame's units (guide_ingame_layout::width x height)
    int target;               // guide_ingame_target
    int pane, item;           // the pane (as guide_click counts them: the panes shown) and item; -1 none
} guide_ingame_hit;

typedef struct guide_ingame_layout {
    unsigned int session;      // the opening of the guide drawn (guide_view::session)
    unsigned int page_serial;  // ... and its pages (guide_view::page_serial): once they change, nothing is hit
    float width, height;       // the frame the regions are in (the game's back buffer, in its pixels)
    int hit_count;             // later regions are on top of earlier ones
    guide_ingame_hit hits[GUIDE_INGAME_HITS];
} guide_ingame_layout;
typedef char guide_ingame_layout_check[
    sizeof(guide_ingame_hit) == 28 && sizeof(guide_ingame_layout) == 20 + 28 * GUIDE_INGAME_HITS ? 1 : -1];

// Once per frame the game's renderer presents, before drawing the guide. `drawable`: the renderer can draw the guide in
// this frame (the game's UI globals are loaded); while it can, a guide opening shows in-game (unless
// HALO3_GUIDE_RENDER=window), and while it cannot, a new opening shows in the guide's own window. Returns 1 with the
// guide's view in `view` when the in-game guide shows (a view of an opening the guide's own window shows is not), 0
// when there is nothing to draw. The view is the renderer's own, kept from frame to frame: it is rebuilt only when
// something shown changed.
PORT_MS_ABI int guide_ingame_frame(guide_view *view, int drawable);
// After drawing: where the view's parts are, for the mouse. The regions stay until the next call.
PORT_MS_ABI void guide_ingame_set_layout(const guide_ingame_layout *layout);

// ---- the guide's side (guide.c) ----

// HALO3_GUIDE_RENDER is not "window" (read once).
int guide_ingame_wanted(void);
// Whether the game's renderer can draw the in-game guide right now (drawable in the last presented frame).
int guide_ingame_live(void);
// Claims opening `session` for in-game rendering when in-game rendering is wanted. Returns 1 when it does (the guide's
// own window is then never opened; if the renderer is transiently not drawable, drawing waits until it is). Called
// under the guide's lock (it takes no lock itself).
int guide_ingame_show(unsigned long session);
// The items of the `pane`-th pane shown (as guide_click counts them) that opening `session`'s last frame drew as rows,
// for LT / RT: 1 with the first and last, 0 when that frame drew another opening or other pages (or none). Called
// under the guide's lock (the layout's is taken inside it, never the other way).
int guide_ingame_rows_shown(unsigned long session, int pane, int *first, int *last);

// ---- the game window's side (window_linux.c, window_windows.c; any thread) ----

// Whether the in-game guide shows: the window's keys and mouse go to it (guide_ingame_key, guide_ingame_pointer) and
// not to the game. The window shows its pointer and stops capturing it meanwhile.
int guide_ingame_input(void);
// A key of the game window, translated by the host (guide_key_action's keys and actions; `character` for
// GUIDE_KEY_CHARACTER). guide_ingame_key: a press.
void guide_ingame_key_action(int key, unsigned long character, int action);
void guide_ingame_key(int key, unsigned long character);

// What the pointer did (guide_ingame_pointer).
enum guide_ingame_pointer_event {
    GUIDE_POINTER_MOVE,
    GUIDE_POINTER_PRESS,      // `button`: 1 left (choose), 2 middle, 3 right (back)
    GUIDE_POINTER_WHEEL,      // `button`: -1 up, 1 down
};
// The pointer at (x, y) in a window whose client area is width x height pixels (the frame is stretched over it); 0 x 0:
// (x, y) are the frame's own pixels (HALO3_INPUT_SCRIPT).
void guide_ingame_pointer(int event, int button, int x, int y, int width, int height);

// ---- toasts: the console's notification pop-ups ----
//
// A toast is a banner at the bottom of the frame, over the game and over the guide, that takes no input: it fades in,
// holds and fades out, and the next one queued follows it. Toasts show whether the guide shows or not. Posted for an
// achievement newly earned (port/sdk/online/xlive_offline.cpp, XUserWriteAchievements) and a game invite received
// (port/sdk/online/invite.c). A toast's clock runs only in frames the renderer can draw it in, so none is lost
// to a map load. The renderer is port/probe/extra/guide_ingame_ui.cpp.

enum guide_toast_kind {
    GUIDE_TOAST_ACHIEVEMENT, // title "Achievement Unlocked", text its name, detail its gamerscore ("20 G")
    GUIDE_TOAST_INVITE,      // title "Game Invite", text who invited, hint how to accept
    GUIDE_TOAST_SIGNIN,      // online gamertag when the system sign-in dialog closes
};

// Room for the toasts of a whole achievement write (Halo 3 writes one user's at a time: up to 79 with ODST DLC) and
// some invites; a toast posted while the queue is full is dropped (and logged).
#define GUIDE_TOAST_QUEUE 96
#define GUIDE_TOAST_FADE_IN_MILLISECONDS 250ul
#define GUIDE_TOAST_HOLD_MILLISECONDS 4000ul
#define GUIDE_TOAST_FADE_OUT_MILLISECONDS 450ul
#define GUIDE_TOAST_GAP_MILLISECONDS 300ul // between one toast and the next
#define GUIDE_TOAST_MILLISECONDS \
    (GUIDE_TOAST_FADE_IN_MILLISECONDS + GUIDE_TOAST_HOLD_MILLISECONDS + GUIDE_TOAST_FADE_OUT_MILLISECONDS)

// Only 4-byte and char members: alike in both ABIs.
typedef struct guide_toast {
    int kind;                    // guide_toast_kind
    unsigned int serial;         // one per toast posted
    unsigned int elapsed;        // milliseconds it has shown, in drawable frames (< GUIDE_TOAST_MILLISECONDS)
    unsigned int image_id;       // SPA image ID, 0 for a toast without an icon
    char title[GUIDE_LABEL];     // UTF-8
    char text[GUIDE_LABEL * 2];
    char detail[GUIDE_LABEL];    // right-aligned beside the text ("20 G"), "" for none
    char hint[GUIDE_LABEL];      // under the text, "" for none
} guide_toast;
typedef char guide_toast_layout[sizeof(guide_toast) == 16 + 5 * GUIDE_LABEL ? 1 : -1];

// Queues a toast (any thread; strings copied, cut to fit). Returns 1 when queued, 0 when GUIDE_TOAST_QUEUE wait already
// (the new one is dropped, and logged).
PORT_MS_ABI int guide_toast_post(int kind, const char *title, const char *text, const char *detail, const char *hint);
PORT_MS_ABI int guide_toast_post_icon(int kind, unsigned int image_id, const char *title, const char *text,
    const char *detail, const char *hint);
// The title's SPA PNG bytes, owned by the XEX resource loader for the life of the process.
PORT_MS_ABI int port_spa_image_get(unsigned int image_id, const void **out_data, unsigned int *out_size);
// The renderer, once per frame at `now` (GetTickCount): the toast to draw, with how long it has shown. `drawable` as
// for guide_ingame_frame: a toast begins, and its clock runs, only in drawable frames. Returns 1 with it in *toast, 0
// when none shows (between two, none queued, or a frame that is not drawable).
PORT_MS_ABI int guide_ingame_toast_frame(unsigned long now, int drawable, guide_toast *toast);
// Whether a Game Invite toast shows now: the Guide button then opens the invite (guide.c).
int guide_toast_invite_showing(void);
// Total number of toasts posted this run; if last_out is non-null and count > 0, copies the most recently posted toast.
PORT_MS_ABI unsigned int guide_toast_posted_count(guide_toast *last_out);

#ifdef __cplusplus
}
#endif

#endif
