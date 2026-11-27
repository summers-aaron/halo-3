// port/sdk/xam (host code): the guide's own window (guide.h), HALO3_GUIDE_RENDER=window's way of showing it and the
// stand-in while the in-game guide cannot draw (guide_ingame.h); one implementation per host: guide_linux.c (Xlib,
// loaded at run time from libX11.so.6, on a display connection of its own) and guide_windows.c (user32.dll and
// gdi32.dll, loaded at run time). Each runs the window on a host thread of its own, so the game's frames go on while
// the guide shows and the window never waits for the game: it draws guide_view_get's view, again whenever
// guide_host_wake says it changed, passes keys and clicks to guide_key / guide_click, and goes once the view is closed
// or belongs to another opening.
#ifndef PORT_XAM_GUIDE_HOST_H
#define PORT_XAM_GUIDE_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

// Shows the window for the guide opening `session` (guide_view::session) over the game window (port_window.h). Returns
// 1 when the window's thread runs, 0 when this host cannot show one (no game window, no display, the window library or
// a thread unavailable; a failure other than no game window is logged): the guide is then operated by controllers.
int guide_host_show(unsigned long session);
// The view changed (guide_revision): the window redraws soon. Any thread; nothing when no window shows.
void guide_host_wake(void);
// Runs entry(argument) on a new detached thread (the guide's work that waits on the network or the clipboard, kept off
// the game's and the window's threads). Returns 1 when it started, 0 when it did not (the caller then runs the work
// itself).
int guide_host_thread_start(void (*entry)(void *argument), void *argument);

// The guide's keys on this host's keyboard, for the guide's window and for the game window while the in-game guide
// shows (guide_ingame.h): guide_key's key, or -1 for a key the guide does not use.
#ifdef _WIN32
// A WM_KEYDOWN's virtual key, Shift held or not (Shift+Insert: GUIDE_KEY_PASTE). Typed characters come as WM_CHAR:
// guide_host_typed gives GUIDE_KEY_CHARACTER for one the guide takes, GUIDE_KEY_PASTE for Ctrl+V's (0x16), -1 else.
int guide_host_virtual_key(unsigned int virtual_key, int shift);
int guide_host_typed(unsigned int character);
#else
// An X11 key press: its keysym and modifier state as XLookupString gave them, with the text it typed (`*character` is
// set for GUIDE_KEY_CHARACTER). Ctrl+V and Shift+Insert are GUIDE_KEY_PASTE.
int guide_host_x11_key(unsigned long keysym, unsigned int state, const char *typed, int typed_count,
                       unsigned long *character);
#endif

#ifdef __cplusplus
}
#endif

#endif
