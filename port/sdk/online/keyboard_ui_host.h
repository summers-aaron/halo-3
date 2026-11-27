// port/sdk/online (host code): the dialog that stands in for the guide's virtual keyboard (XShowKeyboardUI,
// keyboard_ui.cpp), one implementation per host: keyboard_ui_linux.c (Xlib, loaded at run time from libX11.so.6, on a
// display connection of its own) and keyboard_ui_windows.c (user32.dll, loaded at run time). Each runs the dialog on a
// host thread of its own, so the game's frames go on while the player types and the dialog never waits for the game.
// Text is UTF-16 (the game's wchar_t), as unsigned short.
#ifndef PORT_ONLINE_KEYBOARD_UI_HOST_H
#define PORT_ONLINE_KEYBOARD_UI_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

// The longest text, in UTF-16 units, the dialog edits and shows (each string's buffer holds one more, for the NUL).
#define KEYBOARD_UI_HOST_TEXT_MAX 1024u

typedef struct keyboard_ui_host_request keyboard_ui_host_request;
struct keyboard_ui_host_request {
    // The dialog's caption, the line under it, and the text it opens with: NUL-terminated, empty when not given.
    unsigned short title[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    unsigned short description[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    unsigned short default_text[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    // The most UTF-16 units the player may enter (the result buffer less its NUL), at most KEYBOARD_UI_HOST_TEXT_MAX.
    unsigned int max_length;
    int password;  // VKBD_LATIN_PASSWORD: the text shows as '*'
    int highlight; // VKBD_HIGHLIGHT_TEXT: the default text opens selected, so typing replaces it
    // Written by the dialog when the player accepts: the text entered, NUL-terminated, at most max_length units.
    unsigned short text[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    // Called once, on the dialog's thread, when the dialog has closed: accepted 1 (Enter, OK; `text` holds the
    // result), 0 (Escape, Cancel, the window closed). The request stays the caller's; the host does not touch it after.
    void (PORT_MS_ABI *done)(keyboard_ui_host_request *request, int accepted);
};

// Shows the dialog for `request` over the game window (port_window.h). Returns 1 when the dialog's thread runs:
// request->done follows, once (a dialog whose windows cannot be made on that thread, which only Windows makes there,
// closes cancelled). Returns 0 when this host cannot show one (no game window, no display, the window library or a
// thread unavailable; a failure other than no game window is logged): done is not called.
PORT_MS_ABI int keyboard_ui_host_show(keyboard_ui_host_request *request);

// HALO3_KBD_TEXT, for scripted and headless runs: when set (even empty), copies it to `utf8` (NUL-terminated, cut to
// `capacity` - 1 bytes; a character cut short is the reader's to drop) and returns 1; returns 0 when unset.
PORT_MS_ABI int keyboard_ui_host_scripted_text(char *utf8, unsigned int capacity);

#ifdef __cplusplus
}
#endif

#endif
