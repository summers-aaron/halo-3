// port/sdk/xam (both hosts): the host's clipboard, for the guide's lobby codes (guide.c: Copy Lobby Code, and Ctrl+V in
// a text field). clipboard.c keeps the port's own copy; the host's clipboard is clipboard_linux.c (the X11 CLIPBOARD
// selection, through Xlib loaded at run time, on a display connection and thread of its own) or clipboard_windows.c
// (the Win32 clipboard, CF_UNICODETEXT, through user32.dll loaded at run time). A host with no clipboard (no display:
// a headless run) still has the port's own: what was copied in this run can be pasted in it.
//
// Text is UTF-8. Both calls may wait for the host (X11: the selection's owner answering, at most
// PORT_CLIPBOARD_WAIT_MILLISECONDS), so neither is called under a lock.
#ifndef PORT_XAM_CLIPBOARD_H
#define PORT_XAM_CLIPBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_CLIPBOARD_MAX 1024u // bytes of text kept or read, NUL included
#define PORT_CLIPBOARD_WAIT_MILLISECONDS 400u

// Copies `text` to the clipboard. Returns 1 when the host's clipboard has it, 0 when only the port's own does (no host
// clipboard).
int port_clipboard_set(const char *text);
// The clipboard's text into `text` (capacity bytes, NUL included; cut at a whole character). Returns 1 when there was
// text: the host's clipboard's, else (no host clipboard, or it could not be read) the port's own copy.
int port_clipboard_get(char *text, unsigned long capacity);

// ---- one per host (clipboard_linux.c, clipboard_windows.c) ----

// 1 when the host's clipboard took the text, 0 when there is none.
int port_clipboard_host_set(const char *text);
// 1 with the text when the host's clipboard has text, 0 when it has none or there is no host clipboard.
int port_clipboard_host_get(char *text, unsigned long capacity);

#ifdef __cplusplus
}
#endif

#endif
