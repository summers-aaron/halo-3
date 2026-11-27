// port/sdk/xam (Linux host): the guide's window (guide_host.h) through Xlib, opened at run time from libX11.so.6
// (RTLD_LOCAL, as the game window: port/sdk/runtime/window_linux.c and the virtual keyboard: keyboard_ui_linux.c) and
// nothing else: no toolkit.
//
// The window is a dialog of its own, transient for the game window and centred over it, on a display connection of its
// own that only the window's thread uses after it is set up, so it never shares Xlib state with the game window's
// connection and the game's frames go on while the guide shows. It draws the guide's panes side by side (one column a
// controller), each with its controller and player, the page's title and text, a text field when the page has one, the
// list (the selected item highlighted, the focused pane's brightest; the list scrolls to keep it in view) and the hint;
// the keys along the bottom. The thread sleeps in select() on the X connection and a pipe that guide_host_wake writes,
// so a change the controllers make (on the game's thread) is drawn at once. Keys:
//   Up, Down, Page Up, Page Down   the selection       Left, Right, Tab, Shift+Tab   the pane the keys drive
//   Enter, Space                   choose              Escape                        back (closing from a first page)
//   Backspace                      erase / back        Home, the window's close button  close the guide
//   letters, digits, space         typed into a text field
//   Ctrl+V, Shift+Insert           the clipboard into a text field (clipboard.h)
// The mouse: a click chooses the item under it, the wheel moves the selection. Text is drawn with a core font, an ISO
// 10646 one when the server has it ("fixed", Latin-1, otherwise; characters it lacks show as '?').
#ifndef _WIN32
#include "guide.h"
#include "guide_host.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"
#include "../input/kbm_bindings.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

int guide_view_get_osk(guide_view *view, const char **grids, int *selected);
const char *guide_osk_label(char key, int compact, char literal[2]);

// ---- Xlib ----

typedef unsigned long x_window;
typedef unsigned long x_atom;
typedef unsigned long x_keysym;
typedef struct x_display x_display;
typedef struct x_gc_record *x_gc;

#define X_KEY_PRESS_MASK (1L << 0)
#define X_KEY_RELEASE_MASK (1L << 1)
#define X_BUTTON_PRESS_MASK (1L << 2)
#define X_EXPOSURE_MASK (1L << 15)
#define X_STRUCTURE_NOTIFY_MASK (1L << 17)
#define X_SUBSTRUCTURE_NOTIFY_MASK (1L << 19)
#define X_SUBSTRUCTURE_REDIRECT_MASK (1L << 20)
#define X_FOCUS_CHANGE_MASK (1L << 21)
#define X_KEY_PRESS 2
#define X_KEY_RELEASE 3
#define X_FOCUS_OUT 10
#define X_BUTTON_PRESS 4
#define X_EXPOSE 12
#define X_DESTROY_NOTIFY 17
#define X_MAP_NOTIFY 19
#define X_CLIENT_MESSAGE 33
#define X_PROP_MODE_REPLACE 0
#define X_ATOM_ATOM 4
#define X_SHIFT_MASK (1u << 0)
#define X_CONTROL_MASK (1u << 2)

// XEvent (Xlib.h): a union of 24 longs. XKeyEvent, XButtonEvent (the same layout up to `button`), XExposeEvent,
// XClientMessageEvent.
typedef union {
    int type;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window window, root, subwindow;
        unsigned long time;
        int x, y, x_root, y_root;
        unsigned int state;
        unsigned int keycode; // a button event's button
        int same_screen;
    } key;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window window;
        int x, y, width, height, count;
    } expose;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window window;
        x_atom message_type;
        int format;
        long data[5];
    } client;
    long pad[24];
} x_event;

typedef struct {
    short lbearing, rbearing, width, ascent, descent;
    unsigned short attributes;
} x_char_struct;
typedef struct {
    void *ext_data;
    unsigned long fid;
    unsigned direction, min_char_or_byte2, max_char_or_byte2, min_byte1, max_byte1;
    int all_chars_exist;
    unsigned default_char;
    int n_properties;
    void *properties;
    x_char_struct min_bounds, max_bounds;
    x_char_struct *per_char;
    int ascent, descent;
} x_font;
typedef struct {
    unsigned char byte1, byte2;
} x_char2b;
typedef struct {
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags, pad;
} x_color;
typedef struct {
    long flags;
    int x, y, width, height, min_width, min_height, max_width, max_height, width_inc, height_inc;
    struct {
        int x, y;
    } min_aspect, max_aspect;
    int base_width, base_height, win_gravity;
} x_size_hints;
typedef struct {
    long flags;
    int input, initial_state;
    unsigned long icon_pixmap;
    x_window icon_window;
    int icon_x, icon_y;
    unsigned long icon_mask, window_group;
} x_wm_hints;
typedef struct {
    char *res_name, *res_class;
} x_class_hint;
#define X_US_POSITION (1L << 0)
#define X_P_POSITION (1L << 2)
#define X_P_MIN_SIZE (1L << 4)
#define X_P_MAX_SIZE (1L << 5)
#define X_INPUT_HINT (1L << 0)
// XErrorEvent
typedef struct {
    int type;
    x_display *display;
    unsigned long resource_id, serial;
    unsigned char error_code, request_code, minor_code;
} x_error_event;
typedef int (*x_error_handler)(x_display *, x_error_event *);

static struct {
    void *library;
    x_display *(*XOpenDisplay)(const char *);
    int (*XCloseDisplay)(x_display *);
    x_error_handler (*XSetErrorHandler)(x_error_handler);
    int (*XConnectionNumber)(x_display *);
    int (*XDefaultScreen)(x_display *);
    x_window (*XRootWindow)(x_display *, int);
    int (*XDefaultDepth)(x_display *, int);
    unsigned long (*XDefaultColormap)(x_display *, int);
    unsigned long (*XBlackPixel)(x_display *, int);
    unsigned long (*XWhitePixel)(x_display *, int);
    int (*XAllocColor)(x_display *, unsigned long, x_color *);
    x_window (*XCreateSimpleWindow)(x_display *, x_window, int, int, unsigned int, unsigned int, unsigned int,
                                    unsigned long, unsigned long);
    int (*XDestroyWindow)(x_display *, x_window);
    int (*XStoreName)(x_display *, x_window, const char *);
    int (*XChangeProperty)(x_display *, x_window, x_atom, x_atom, int, int, const unsigned char *, int);
    int (*XSetTransientForHint)(x_display *, x_window, x_window);
    void (*XSetWMNormalHints)(x_display *, x_window, x_size_hints *);
    int (*XSetWMHints)(x_display *, x_window, x_wm_hints *);
    int (*XSetClassHint)(x_display *, x_window, x_class_hint *);
    int (*XSelectInput)(x_display *, x_window, long);
    x_atom (*XInternAtom)(x_display *, const char *, int);
    int (*XSetWMProtocols)(x_display *, x_window, x_atom *, int);
    int (*XMapRaised)(x_display *, x_window);
    int (*XSendEvent)(x_display *, x_window, int, long, x_event *);
    int (*XFlush)(x_display *);
    int (*XPending)(x_display *);
    int (*XNextEvent)(x_display *, x_event *);
    int (*XLookupString)(void *, char *, int, x_keysym *, void *);
    x_keysym (*XLookupKeysym)(void *, int);
    int (*XGetGeometry)(x_display *, unsigned long, x_window *, int *, int *, unsigned int *, unsigned int *,
                        unsigned int *, unsigned int *);
    int (*XTranslateCoordinates)(x_display *, x_window, x_window, int, int, int *, int *, x_window *);
    x_font *(*XLoadQueryFont)(x_display *, const char *);
    int (*XFreeFont)(x_display *, x_font *);
    x_gc (*XCreateGC)(x_display *, unsigned long, unsigned long, void *);
    int (*XFreeGC)(x_display *, x_gc);
    int (*XSetForeground)(x_display *, x_gc, unsigned long);
    int (*XSetFont)(x_display *, x_gc, unsigned long);
    unsigned long (*XCreatePixmap)(x_display *, unsigned long, unsigned int, unsigned int, unsigned int);
    int (*XFreePixmap)(x_display *, unsigned long);
    int (*XFillRectangle)(x_display *, unsigned long, x_gc, int, int, unsigned int, unsigned int);
    int (*XDrawRectangle)(x_display *, unsigned long, x_gc, int, int, unsigned int, unsigned int);
    int (*XDrawString16)(x_display *, unsigned long, x_gc, int, int, const x_char2b *, int);
    int (*XTextWidth16)(x_font *, const x_char2b *, int);
    int (*XCopyArea)(x_display *, unsigned long, unsigned long, x_gc, int, int, unsigned int, unsigned int, int, int);
    int (*XkbSetDetectableAutoRepeat)(x_display *, int, int *); // optional
} g_xg;

static pthread_mutex_t g_load_lock = PTHREAD_MUTEX_INITIALIZER;

// ---- X errors ----
// Xlib has one error handler for the process, and its default exits it. A protocol error on a guide window's own
// connection (its window destroyed under it, say) must not end the game: the guide's handler, installed once over
// whatever handler was there, logs those and passes every other connection's errors on unchanged.

#define GUIDE_DISPLAYS 4 // connections open at once: a window closing while the next opens
static x_display *g_displays[GUIDE_DISPLAYS];
static x_error_handler g_previous_error_handler;

static void guide_display_register(x_display *display, x_display *replacement)
{
    for (int i = 0; i < GUIDE_DISPLAYS; i++) {
        x_display *expected = display;
        if (__atomic_compare_exchange_n(&g_displays[i], &expected, replacement, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
            return;
    }
}

static void append_decimal(char *text, unsigned value)
{
    char digits[12];
    int n = 0;
    do
        digits[n++] = (char)('0' + value % 10);
    while ((value /= 10) != 0 && n < (int)sizeof(digits));
    text += strlen(text);
    while (n > 0)
        *text++ = digits[--n];
    *text = 0;
}

static int guide_x_error(x_display *display, x_error_event *error)
{
    for (int i = 0; i < GUIDE_DISPLAYS; i++)
        if (__atomic_load_n(&g_displays[i], __ATOMIC_ACQUIRE) == display) {
            char text[96] = "guide: X error ";
            append_decimal(text, error->error_code);
            strcat(text, " (request ");
            append_decimal(text, error->request_code);
            strcat(text, ") on the guide's window, ignored\n");
            port_write_error(text);
            return 0;
        }
    return g_previous_error_handler != 0 ? g_previous_error_handler(display, error) : 0;
}

// Closes a connection guide_window_open opened, its errors the guide's until it is gone.
static void guide_display_close(x_display *display)
{
    g_xg.XCloseDisplay(display);
    guide_display_register(display, 0);
}

static int guide_x_load(void)
{
    pthread_mutex_lock(&g_load_lock);
    if (g_xg.library != 0) {
        pthread_mutex_unlock(&g_load_lock);
        return 1;
    }
    void *library = dlopen("libX11.so.6", RTLD_NOW | RTLD_LOCAL);
    if (library == 0) {
        pthread_mutex_unlock(&g_load_lock);
        port_write_error("guide: libX11.so.6 is not available\n");
        return 0;
    }
#define LOAD(name)                                                                                                  \
    do {                                                                                                            \
        *(void **)&g_xg.name = dlsym(library, #name);                                                               \
        if (g_xg.name == 0) {                                                                                       \
            port_write_error("guide: libX11.so.6 has no " #name "\n");                                              \
            dlclose(library);                                                                                       \
            pthread_mutex_unlock(&g_load_lock);                                                                     \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    LOAD(XOpenDisplay);
    LOAD(XCloseDisplay);
    LOAD(XSetErrorHandler);
    LOAD(XConnectionNumber);
    LOAD(XDefaultScreen);
    LOAD(XRootWindow);
    LOAD(XDefaultDepth);
    LOAD(XDefaultColormap);
    LOAD(XBlackPixel);
    LOAD(XWhitePixel);
    LOAD(XAllocColor);
    LOAD(XCreateSimpleWindow);
    LOAD(XDestroyWindow);
    LOAD(XStoreName);
    LOAD(XChangeProperty);
    LOAD(XSetTransientForHint);
    LOAD(XSetWMNormalHints);
    LOAD(XSetWMHints);
    LOAD(XSetClassHint);
    LOAD(XSelectInput);
    LOAD(XInternAtom);
    LOAD(XSetWMProtocols);
    LOAD(XMapRaised);
    LOAD(XSendEvent);
    LOAD(XFlush);
    LOAD(XPending);
    LOAD(XNextEvent);
    LOAD(XLookupString);
    LOAD(XLookupKeysym);
    LOAD(XGetGeometry);
    LOAD(XTranslateCoordinates);
    LOAD(XLoadQueryFont);
    LOAD(XFreeFont);
    LOAD(XCreateGC);
    LOAD(XFreeGC);
    LOAD(XSetForeground);
    LOAD(XSetFont);
    LOAD(XCreatePixmap);
    LOAD(XFreePixmap);
    LOAD(XFillRectangle);
    LOAD(XDrawRectangle);
    LOAD(XDrawString16);
    LOAD(XTextWidth16);
    LOAD(XCopyArea);
#undef LOAD
    // optional: key repeat without release events between (a press of a key already down is then a repeat)
    *(void **)&g_xg.XkbSetDetectableAutoRepeat = dlsym(library, "XkbSetDetectableAutoRepeat");
    g_previous_error_handler = g_xg.XSetErrorHandler(guide_x_error);
    g_xg.library = library;
    pthread_mutex_unlock(&g_load_lock);
    return 1;
}

// ---- the wake pipe (guide_host_wake) ----

static int g_wake[2] = { -1, -1 };
static pthread_once_t g_wake_once = PTHREAD_ONCE_INIT;

static void wake_open(void)
{
    int fds[2];
    if (pipe(fds) != 0)
        return;
    for (int i = 0; i < 2; i++) {
        fcntl(fds[i], F_SETFD, FD_CLOEXEC);
        fcntl(fds[i], F_SETFL, fcntl(fds[i], F_GETFL) | O_NONBLOCK);
    }
    g_wake[0] = fds[0];
    g_wake[1] = fds[1];
}

static volatile int g_windows; // windows showing (wake writes only while one does)

void guide_host_wake(void)
{
    if (__atomic_load_n(&g_windows, __ATOMIC_ACQUIRE) == 0 || g_wake[1] < 0)
        return;
    const char byte = 1;
    ssize_t written = write(g_wake[1], &byte, 1); // a full pipe is a wake pending already
    (void)written;
}

// ---- the window ----

#define GUIDE_MARGIN 14
#define GUIDE_PAD 6
#define GUIDE_PANE_WIDTH 300
#define GUIDE_SINGLE_WIDTH 560
#define GUIDE_HEIGHT 540
#define GUIDE_BODY_LINES 6
#define GUIDE_HINT_LINES 4
#define GUIDE_ROWS_MAX GUIDE_ITEMS

typedef struct guide_window {
    unsigned long session;
    x_display *display;
    x_window window;
    unsigned long buffer; // the back buffer pixmap, drawn then copied (no flicker)
    x_gc gc;
    x_font *font;
    int wide_font;
    x_atom delete_atom;
    unsigned long background, panel, foreground, dim, disabled, accent, accent_dim, field;
    int width, height, line_height;
    guide_view view;
    const char *osk[GUIDE_PANES];
    int osk_selected[GUIDE_PANES], osk_columns;
    unsigned long drawn; // the revision drawn
    unsigned char keys_down[32]; // a bit per keycode down (a press of one down already is the server's repeat)
    // where the last draw put each pane's rows (for the mouse)
    struct {
        int x, width, rows_y, row_height, first, rows;
    } layout[GUIDE_PANES];
} guide_window;

// UTF-8 `text` (at most `bytes` bytes) as glyphs (BMP only: '?' past it, or past Latin-1 without a wide font).
static int guide_glyphs(const guide_window *w, const char *text, unsigned long bytes, x_char2b *out, int capacity)
{
    const unsigned char *s = (const unsigned char *)text;
    int n = 0;
    unsigned long at = 0;
    while (at < bytes && s[at] != 0 && n < capacity) {
        unsigned long code = s[at], extra = code >= 0xf0 ? 3 : code >= 0xe0 ? 2 : code >= 0xc0 ? 1 : 0;
        code &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : extra == 1 ? 0x1f : 0x7f;
        at++;
        for (unsigned long k = 0; k < extra && at < bytes && (s[at] & 0xc0) == 0x80; k++)
            code = code << 6 | (s[at++] & 0x3f);
        if (code > 0xffff || (code > 0xff && !w->wide_font) || code < 0x20)
            code = '?';
        out[n].byte1 = (unsigned char)(code >> 8);
        out[n].byte2 = (unsigned char)code;
        n++;
    }
    return n;
}

static int guide_text_width(const guide_window *w, const char *text, unsigned long bytes)
{
    x_char2b glyphs[GUIDE_TEXT];
    const int n = guide_glyphs(w, text, bytes, glyphs, GUIDE_TEXT);
    return n > 0 ? g_xg.XTextWidth16(w->font, glyphs, n) : 0;
}

static void guide_draw_text(guide_window *w, int x, int y, const char *text, unsigned long bytes)
{
    x_char2b glyphs[GUIDE_TEXT];
    const int n = guide_glyphs(w, text, bytes, glyphs, GUIDE_TEXT);
    if (n > 0)
        g_xg.XDrawString16(w->display, w->buffer, w->gc, x, y, glyphs, n);
}

// The longest prefix of `text` (bytes) that fits `width`, cut at a character boundary.
static unsigned long guide_fit(const guide_window *w, const char *text, unsigned long bytes, int width)
{
    unsigned long fit = 0;
    for (unsigned long at = 0; at <= bytes;) {
        if (guide_text_width(w, text, at) > width)
            break;
        fit = at;
        if (at == bytes)
            break;
        at++;
        while (at < bytes && ((unsigned char)text[at] & 0xc0) == 0x80)
            at++;
    }
    return fit;
}

// `text` cut to `width`: the bytes drawn (*bytes) and their width with the ellipsis a cut adds. 0 and *bytes 0 when
// not even the ellipsis fits.
static const char *guide_ellipsis(const guide_window *w) { return w->wide_font ? "\xe2\x80\xa6" : "..."; }
static int guide_fitted(const guide_window *w, const char *text, int width, unsigned long *bytes)
{
    const unsigned long length = strlen(text);
    const int whole = guide_text_width(w, text, length);
    *bytes = length;
    if (whole <= width)
        return whole;
    const char *ellipsis = guide_ellipsis(w);
    const int ellipsis_width = guide_text_width(w, ellipsis, strlen(ellipsis));
    *bytes = 0;
    if (ellipsis_width > width)
        return 0;
    unsigned long fit = guide_fit(w, text, length, width - ellipsis_width);
    while (fit > 0 && text[fit - 1] == ' ')
        fit--;
    *bytes = fit;
    return guide_text_width(w, text, fit) + ellipsis_width;
}

// Draws `text` at (x, y) within `width`: whole, or cut at a character with an ellipsis. Returns the width drawn.
static int guide_draw_fitted(guide_window *w, int x, int y, const char *text, int width)
{
    unsigned long bytes = 0;
    const int drawn = guide_fitted(w, text, width, &bytes);
    if (drawn == 0)
        return 0;
    guide_draw_text(w, x, y, text, bytes);
    if (bytes < strlen(text)) {
        const char *ellipsis = guide_ellipsis(w);
        guide_draw_text(w, x + guide_text_width(w, text, bytes), y, ellipsis, strlen(ellipsis));
    }
    return drawn;
}

// Draws `text` wrapped to `width` (at spaces where it can, at '\n'), at most `max_lines`; returns the lines drawn.
static int guide_draw_wrapped(guide_window *w, int x, int y, int width, const char *text, int max_lines)
{
    int lines = 0;
    unsigned long at = 0, length = strlen(text);
    while (at < length && lines < max_lines) {
        while (text[at] == ' ')
            at++;
        unsigned long end = at;
        while (end < length && text[end] != '\n')
            end++;
        unsigned long fit = guide_fit(w, text + at, end - at, width);
        if (fit < end - at) {
            unsigned long space = fit;
            while (space > 0 && text[at + space] != ' ')
                space--;
            if (space > 0)
                fit = space;
            if (fit == 0)
                fit = 1;
        }
        guide_draw_text(w, x, y + lines * w->line_height + w->font->ascent, text + at, fit);
        lines++;
        at += fit;
        if (text[at] == '\n')
            at++;
    }
    return lines;
}

static void guide_fill(guide_window *w, unsigned long color, int x, int y, int width, int height)
{
    g_xg.XSetForeground(w->display, w->gc, color);
    g_xg.XFillRectangle(w->display, w->buffer, w->gc, x, y, (unsigned)width, (unsigned)height);
}

static int guide_draw_osk(guide_window *w, int pane, int left, int y, int width)
{
    const char *grid = w->osk[pane];
    const int columns = w->osk_columns, height = w->line_height + 2 * GUIDE_PAD;
    for (int i = 0; grid[i]; i++) {
        const int col = i % columns;
        const int x = left + col * width / columns, right = left + (col + 1) * width / columns;
        const int selected = i == w->osk_selected[pane];
        char literal[2];
        const char *label = guide_osk_label(grid[i], w->view.pane_count > 1, literal);
        const int label_width = guide_text_width(w, label, strlen(label));
        guide_fill(w, selected ? w->accent : w->field, x, y, right - x - 2, height - 2);
        g_xg.XSetForeground(w->display, w->gc, selected ? w->background : w->foreground);
        guide_draw_text(w, x + (right - x - 2 - label_width) / 2, y + GUIDE_PAD + w->font->ascent,
                        label, strlen(label));
        if (col + 1 == columns)
            y += height;
    }
    return y + GUIDE_PAD;
}

static void guide_draw_pane(guide_window *w, int index, int x, int width)
{
    const guide_view_pane *pane = &w->view.panes[index];
    const int inner = width - 2 * GUIDE_PAD, left = x + GUIDE_PAD, ascent = w->font->ascent;
    const int bottom = w->height - GUIDE_MARGIN - w->line_height - GUIDE_PAD; // above the keys' line
    guide_fill(w, w->panel, x, GUIDE_MARGIN, width, bottom - GUIDE_MARGIN);
    if (pane->focused && w->view.pane_count > 1) {
        g_xg.XSetForeground(w->display, w->gc, w->accent);
        g_xg.XDrawRectangle(w->display, w->buffer, w->gc, x, GUIDE_MARGIN, (unsigned)width - 1,
                            (unsigned)(bottom - GUIDE_MARGIN) - 1);
    }
    int y = GUIDE_MARGIN + GUIDE_PAD;
    // the controller and its player
    g_xg.XSetForeground(w->display, w->gc, w->accent);
    guide_draw_text(w, left, y + ascent, pane->heading, strlen(pane->heading));
    g_xg.XSetForeground(w->display, w->gc, w->dim);
    unsigned long status = 0;
    const int status_width = guide_fitted(w, pane->status, inner / 2, &status);
    guide_draw_fitted(w, left + inner - status_width, y + ascent, pane->status, status_width);
    y += w->line_height + GUIDE_PAD;
    // the page: title (bold), text
    g_xg.XSetForeground(w->display, w->gc, w->foreground);
    guide_draw_text(w, left, y + ascent, pane->title, strlen(pane->title));
    guide_draw_text(w, left + 1, y + ascent, pane->title, strlen(pane->title));
    y += w->line_height + GUIDE_PAD;
    g_xg.XSetForeground(w->display, w->gc, w->dim);
    y += guide_draw_wrapped(w, left, y, inner, pane->body, GUIDE_BODY_LINES) * w->line_height;
    if (pane->body[0] != 0)
        y += GUIDE_PAD;
    if (pane->has_field) {
        const int height = w->line_height + 2 * GUIDE_PAD;
        guide_fill(w, w->field, left, y, inner, height);
        g_xg.XSetForeground(w->display, w->gc, w->foreground);
        const char *shown = pane->field;
        if (w->osk[index]) // keep the end of a long lobby code and its caret inside the field
            while (*shown && guide_text_width(w, shown, strlen(shown)) > inner - 2 * GUIDE_PAD - 2)
                shown++;
        guide_draw_text(w, left + GUIDE_PAD, y + GUIDE_PAD + ascent, shown, strlen(shown));
        const int caret = left + GUIDE_PAD + guide_text_width(w, shown, strlen(shown));
        guide_fill(w, w->accent, caret, y + GUIDE_PAD, 2, w->line_height);
        y += height + GUIDE_PAD;
    }
    if (w->osk[index])
        y = guide_draw_osk(w, index, left, y, inner);
    // the hint, from the bottom up
    int hint_lines = 0;
    if (pane->hint[0] != 0) {
        // count the lines it wraps to by drawing it off the bottom of the buffer first
        hint_lines = guide_draw_wrapped(w, left, w->height + 10, inner, pane->hint, GUIDE_HINT_LINES);
    }
    const int hint_y = bottom - GUIDE_PAD - hint_lines * w->line_height;
    // the list, scrolled so the selection shows
    const int row_height = w->line_height + 2 * GUIDE_PAD;
    int rows = (hint_y - GUIDE_PAD - y) / row_height;
    if (rows < 1)
        rows = 1;
    int first = 0;
    if (pane->selected >= rows)
        first = pane->selected - rows + 1;
    if (first + rows > pane->item_count)
        first = pane->item_count > rows ? pane->item_count - rows : 0;
    w->layout[index].x = x;
    w->layout[index].width = width;
    w->layout[index].rows_y = y;
    w->layout[index].row_height = row_height;
    w->layout[index].first = first;
    w->layout[index].rows = rows;
    for (int row = 0; row < rows && first + row < pane->item_count; row++) {
        const guide_view_item *item = &pane->items[first + row];
        const int top = y + row * row_height;
        if (first + row == pane->selected)
            guide_fill(w, pane->focused ? w->accent : w->accent_dim, left, top, inner, row_height - 2);
        g_xg.XSetForeground(w->display, w->gc,
                            !item->enabled ? w->disabled : first + row == pane->selected ? w->background
                                                                                         : w->foreground);
        const int text_x = left + GUIDE_PAD + guide_text_width(w, "* ", 2);
        if (item->checked)
            guide_draw_text(w, left + GUIDE_PAD, top + GUIDE_PAD + ascent, "*", 1);
        // the text first, the detail (right-aligned) in the room it leaves; either cut with an ellipsis
        const int room = inner - 3 * GUIDE_PAD - (text_x - left);
        const int text_width = guide_draw_fitted(w, text_x, top + GUIDE_PAD + ascent, item->text, room);
        unsigned long detail = 0;
        const int detail_width = item->detail[0] != 0 ? guide_fitted(w, item->detail, room - text_width, &detail) : 0;
        if (detail != 0)
            guide_draw_fitted(w, left + inner - GUIDE_PAD - detail_width, top + GUIDE_PAD + ascent, item->detail,
                              detail_width);
    }
    g_xg.XSetForeground(w->display, w->gc, w->dim);
    if (first > 0)
        guide_draw_text(w, left + inner - guide_text_width(w, "^", 1), y - GUIDE_PAD + ascent / 2, "^", 1);
    if (first + rows < pane->item_count)
        guide_draw_text(w, left + inner - guide_text_width(w, "v", 1), y + rows * row_height + ascent / 2, "v", 1);
    if (pane->hint[0] != 0)
        guide_draw_wrapped(w, left, hint_y, inner, pane->hint, GUIDE_HINT_LINES);
}

static void guide_draw(guide_window *w)
{
    guide_fill(w, w->background, 0, 0, w->width, w->height);
    const int count = w->view.pane_count > 0 ? w->view.pane_count : 1;
    const int width = (w->width - 2 * GUIDE_MARGIN - (count - 1) * GUIDE_PAD) / count;
    for (int i = 0; i < w->view.pane_count; i++)
        guide_draw_pane(w, i, GUIDE_MARGIN + i * (width + GUIDE_PAD), width);
    static const char k_keys[] = "Enter: Select   Esc: Back   Home: Close";
    static const char k_keys_panes[] = "Enter: Select   Esc: Back   Tab: Next controller   Home: Close";
    const char *keys = w->view.pane_count > 1 ? k_keys_panes : k_keys;
    g_xg.XSetForeground(w->display, w->gc, w->dim);
    guide_draw_text(w, GUIDE_MARGIN, w->height - GUIDE_MARGIN - w->font->descent, keys, strlen(keys));
    g_xg.XCopyArea(w->display, w->buffer, w->window, w->gc, 0, 0, (unsigned)w->width, (unsigned)w->height, 0, 0);
    g_xg.XFlush(w->display);
}

static unsigned long guide_color(guide_window *w, unsigned long rgb, unsigned long fallback)
{
    x_color color;
    memset(&color, 0, sizeof(color));
    color.red = (unsigned short)((rgb >> 16 & 0xff) * 0x101);
    color.green = (unsigned short)((rgb >> 8 & 0xff) * 0x101);
    color.blue = (unsigned short)((rgb & 0xff) * 0x101);
    const int screen = g_xg.XDefaultScreen(w->display);
    return g_xg.XAllocColor(w->display, g_xg.XDefaultColormap(w->display, screen), &color) ? color.pixel : fallback;
}

static void guide_window_close(guide_window *w)
{
    if (w->gc != 0)
        g_xg.XFreeGC(w->display, w->gc);
    if (w->buffer != 0)
        g_xg.XFreePixmap(w->display, w->buffer);
    if (w->font != 0)
        g_xg.XFreeFont(w->display, w->font);
    if (w->window != 0)
        g_xg.XDestroyWindow(w->display, w->window);
    guide_display_close(w->display);
}

static int guide_window_open(guide_window *w)
{
    w->display = g_xg.XOpenDisplay(0);
    if (w->display == 0) {
        port_write_error("guide: cannot open the X display\n");
        return 0;
    }
    guide_display_register(0, w->display); // its errors are the guide's (guide_x_error)
    x_display *display = w->display;
    static const char *const fonts[] = {
        "-misc-fixed-medium-r-normal--15-*-*-*-*-*-iso10646-1",
        "-misc-fixed-medium-r-normal--13-*-*-*-*-*-iso10646-1",
        "-*-*-medium-r-normal--*-120-*-*-*-*-iso10646-1",
        "9x15",
        "fixed",
    };
    for (unsigned i = 0; i < sizeof(fonts) / sizeof(fonts[0]) && w->font == 0; i++)
        w->font = g_xg.XLoadQueryFont(display, fonts[i]);
    if (w->font == 0) {
        port_write_error("guide: the X server has no core font\n");
        guide_display_close(display);
        return 0;
    }
    w->wide_font = w->font->max_byte1 > 0;
    w->line_height = w->font->ascent + w->font->descent;
    const int screen = g_xg.XDefaultScreen(display);
    const x_window root = g_xg.XRootWindow(display, screen);
    w->background = guide_color(w, 0x16191d, g_xg.XBlackPixel(display, screen));
    w->panel = guide_color(w, 0x22272d, w->background);
    w->foreground = guide_color(w, 0xf2f2f2, g_xg.XWhitePixel(display, screen));
    w->dim = guide_color(w, 0xa4acb4, w->foreground);
    w->disabled = guide_color(w, 0x646b72, w->dim);
    w->accent = guide_color(w, 0x6cc417, w->foreground);
    w->accent_dim = guide_color(w, 0x3c4650, w->dim);
    w->field = guide_color(w, 0x0d0f12, w->background);
    w->width = w->view.pane_count > 1 ? 2 * GUIDE_MARGIN + w->view.pane_count * (GUIDE_PANE_WIDTH + GUIDE_PAD)
                                      : GUIDE_SINGLE_WIDTH;
    w->height = GUIDE_HEIGHT;

    // centred over the game window
    const x_window game = (x_window)port_window_native_window();
    int x = 0, y = 0;
    x_window ignored_root = 0, ignored_child = 0;
    int game_x = 0, game_y = 0;
    unsigned int game_width = 0, game_height = 0, border = 0, depth = 0;
    if (game != 0 &&
        g_xg.XGetGeometry(display, game, &ignored_root, &game_x, &game_y, &game_width, &game_height, &border, &depth) &&
        g_xg.XTranslateCoordinates(display, game, root, 0, 0, &game_x, &game_y, &ignored_child)) {
        x = game_x + ((int)game_width - w->width) / 2;
        y = game_y + ((int)game_height - w->height) / 2;
    }
    w->window = g_xg.XCreateSimpleWindow(display, root, x, y, (unsigned)w->width, (unsigned)w->height, 0,
                                         w->background, w->background);
    if (w->window == 0) {
        port_write_error("guide: XCreateSimpleWindow failed\n");
        g_xg.XFreeFont(display, w->font);
        guide_display_close(display);
        return 0;
    }
    const x_window window = w->window;
    // the window manager's view: a fixed-size modal dialog of the game window
    const char *title = "System Guide";
    g_xg.XChangeProperty(display, window, g_xg.XInternAtom(display, "_NET_WM_NAME", 0),
                         g_xg.XInternAtom(display, "UTF8_STRING", 0), 8, X_PROP_MODE_REPLACE,
                         (const unsigned char *)title, (int)strlen(title));
    g_xg.XStoreName(display, window, title);
    x_atom type = g_xg.XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", 0);
    g_xg.XChangeProperty(display, window, g_xg.XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0), X_ATOM_ATOM, 32,
                         X_PROP_MODE_REPLACE, (const unsigned char *)&type, 1);
    x_atom state = g_xg.XInternAtom(display, "_NET_WM_STATE_MODAL", 0);
    g_xg.XChangeProperty(display, window, g_xg.XInternAtom(display, "_NET_WM_STATE", 0), X_ATOM_ATOM, 32,
                         X_PROP_MODE_REPLACE, (const unsigned char *)&state, 1);
    if (game != 0)
        g_xg.XSetTransientForHint(display, window, game);
    x_size_hints size;
    memset(&size, 0, sizeof(size));
    size.flags = X_US_POSITION | X_P_POSITION | X_P_MIN_SIZE | X_P_MAX_SIZE;
    size.x = x;
    size.y = y;
    size.width = size.min_width = size.max_width = w->width;
    size.height = size.min_height = size.max_height = w->height;
    g_xg.XSetWMNormalHints(display, window, &size);
    x_wm_hints hints;
    memset(&hints, 0, sizeof(hints));
    hints.flags = X_INPUT_HINT;
    hints.input = 1;
    g_xg.XSetWMHints(display, window, &hints);
    char name[] = "halo3", class_name[] = "Halo3";
    x_class_hint class_hint = { name, class_name };
    g_xg.XSetClassHint(display, window, &class_hint);
    w->delete_atom = g_xg.XInternAtom(display, "WM_DELETE_WINDOW", 0);
    g_xg.XSetWMProtocols(display, window, &w->delete_atom, 1);
    g_xg.XSelectInput(display, window, X_KEY_PRESS_MASK | X_KEY_RELEASE_MASK | X_BUTTON_PRESS_MASK | X_EXPOSURE_MASK |
                      X_STRUCTURE_NOTIFY_MASK | X_FOCUS_CHANGE_MASK);
    if (g_xg.XkbSetDetectableAutoRepeat != 0)
        g_xg.XkbSetDetectableAutoRepeat(display, 1, 0);
    w->buffer = g_xg.XCreatePixmap(display, window, (unsigned)w->width, (unsigned)w->height,
                                   (unsigned)g_xg.XDefaultDepth(display, screen));
    w->gc = g_xg.XCreateGC(display, window, 0, 0);
    g_xg.XSetFont(display, w->gc, w->font->fid);
    g_xg.XMapRaised(display, window);
    g_xg.XFlush(display);
    return 1;
}

// Asks the window manager to give the window the focus (EWMH _NET_ACTIVE_WINDOW from an application).
static void guide_window_activate(guide_window *w)
{
    x_event event;
    memset(&event, 0, sizeof(event));
    event.client.type = X_CLIENT_MESSAGE;
    event.client.window = w->window;
    event.client.message_type = g_xg.XInternAtom(w->display, "_NET_ACTIVE_WINDOW", 0);
    event.client.format = 32;
    event.client.data[0] = 1; // source: an application
    const x_window root = g_xg.XRootWindow(w->display, g_xg.XDefaultScreen(w->display));
    g_xg.XSendEvent(w->display, root, 0, X_SUBSTRUCTURE_REDIRECT_MASK | X_SUBSTRUCTURE_NOTIFY_MASK, &event);
    g_xg.XFlush(w->display);
}

int guide_host_x11_key(unsigned long keysym, unsigned int state, const char *typed, int typed_count,
                       unsigned long *character)
{
    *character = 0;
    switch (keysym) {
    case 0xff52: case 0xff97: return GUIDE_KEY_UP;                  // Up, KP_Up
    case 0xff54: case 0xff99: return GUIDE_KEY_DOWN;                // Down, KP_Down
    case 0xff51: case 0xff96: return GUIDE_KEY_LEFT;                // Left, KP_Left
    case 0xff53: case 0xff98: return GUIDE_KEY_RIGHT;               // Right, KP_Right
    case 0xff55: case 0xff9a: return GUIDE_KEY_PAGE_UP;             // Prior, KP_Prior
    case 0xff56: case 0xff9b: return GUIDE_KEY_PAGE_DOWN;           // Next, KP_Next
    case 0xff0d: case 0xff8d: return GUIDE_KEY_ACCEPT;              // Return, KP_Enter
    case 0xff1b: return GUIDE_KEY_BACK;                             // Escape
    case 0xff08: return GUIDE_KEY_ERASE;                            // BackSpace
    case 0xff09: return (state & X_SHIFT_MASK) != 0 ? GUIDE_KEY_PREVIOUS_TAB : GUIDE_KEY_NEXT_TAB; // Tab: RB, LB
    case 0xfe20: return GUIDE_KEY_PREVIOUS_TAB;                     // ISO_Left_Tab (Shift+Tab): LB
    case 0xff50: case 0xff95: return GUIDE_KEY_CLOSE;               // Home, KP_Home: the Guide button
    case 0x0060: *character = '`'; return GUIDE_KEY_CHARACTER;      // grave: closes Guide outside a text field
    case 0xff63: case 0xff9e:                                       // Insert, KP_Insert: Shift+Insert pastes
        return (state & X_SHIFT_MASK) != 0 ? GUIDE_KEY_PASTE : -1;
    case 0x0076: case 0x0056:                                       // v, V: Ctrl+V pastes
        if ((state & X_CONTROL_MASK) != 0)
            return GUIDE_KEY_PASTE;
        break;
    default: break;
    }
    if (typed_count == 1 && (unsigned char)typed[0] >= 0x20 && (unsigned char)typed[0] < 0x7f) {
        *character = (unsigned char)typed[0];
        return GUIDE_KEY_CHARACTER;
    }
    return -1;
}

// A key pressed, repeated (pressed while down already: the server's auto-repeat) or released.
static void guide_window_key(guide_window *w, x_event *event)
{
    const unsigned int keycode = event->key.keycode & 0xff;
    const unsigned char mask = (unsigned char)(1u << (keycode & 7));
    const int press = event->type == X_KEY_PRESS, repeat = press && (w->keys_down[keycode >> 3] & mask) != 0;
    w->keys_down[keycode >> 3] = (unsigned char)(press ? w->keys_down[keycode >> 3] | mask : w->keys_down[keycode >> 3] & ~mask);
    char typed[8];
    x_keysym keysym = 0;
    const int count = g_xg.XLookupString(event, typed, (int)sizeof(typed) - 1, &keysym, 0);
    unsigned long character;
    // offered first to the binding capture (guide_capture), as the game window does: the key itself, whatever the
    // modifiers held (XLookupKeysym's first column: Shift+1 is 1, letters in lower case)
    if (press && !repeat && guide_capture(port_window_key_code(g_xg.XLookupKeysym(event, 0), 0)))
        return;
    const int key = guide_host_x11_key(keysym, event->key.state, typed, count, &character);
    if (key >= 0)
        guide_key_action(key, character, !press ? GUIDE_KEY_RELEASED : repeat ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
}

static void guide_window_click(guide_window *w, x_event *event)
{
    const int x = event->key.x, y = event->key.y;
    const unsigned button = event->key.keycode;
    // offered first to the binding capture (guide_capture): X buttons 1 left, 2 middle, 3 right, 4 / 5 the wheel, 8 / 9
    // the side buttons
    const int code = button == 1 ? PORT_KBM_MOUSE(k_port_mouse_left) : button == 2 ? PORT_KBM_MOUSE(k_port_mouse_middle)
                     : button == 3 ? PORT_KBM_MOUSE(k_port_mouse_right) : button == 4 ? PORT_KBM_WHEEL_UP
                     : button == 5 ? PORT_KBM_WHEEL_DOWN : button == 8 ? PORT_KBM_MOUSE(k_port_mouse_4)
                     : button == 9 ? PORT_KBM_MOUSE(k_port_mouse_5) : PORT_KBM_NONE;
    if (code != PORT_KBM_NONE && guide_capture(code))
        return;
    for (int i = 0; i < w->view.pane_count; i++) {
        if (x < w->layout[i].x || x >= w->layout[i].x + w->layout[i].width)
            continue;
        if (button == 4 || button == 5) { // the wheel
            guide_click(i, -1, 0);
            guide_key(button == 4 ? GUIDE_KEY_SCROLL_UP : GUIDE_KEY_SCROLL_DOWN, 0);
            return;
        }
        const int row = y >= w->layout[i].rows_y ? (y - w->layout[i].rows_y) / w->layout[i].row_height : -1;
        const int item = row >= 0 && row < w->layout[i].rows ? w->layout[i].first + row : -1;
        guide_click(i, item < w->view.panes[i].item_count ? item : -1, button == 1);
        return;
    }
}

static void *guide_window_main(void *argument)
{
    guide_window *w = (guide_window *)argument;
    const int x_fd = g_xg.XConnectionNumber(w->display);
    for (;;) {
        w->osk_columns = guide_view_get_osk(&w->view, w->osk, w->osk_selected);
        if (!w->view.open || w->view.session != w->session)
            break;
        if (w->view.revision != w->drawn) {
            w->drawn = w->view.revision;
            guide_draw(w);
        }
        if (g_xg.XPending(w->display) > 0) {
            x_event event;
            memset(&event, 0, sizeof(event));
            g_xg.XNextEvent(w->display, &event);
            switch (event.type) {
            case X_KEY_PRESS:
            case X_KEY_RELEASE:
                guide_window_key(w, &event);
                break;
            case X_FOCUS_OUT: // what is let go meanwhile never reaches this window: nothing stays down
                memset(w->keys_down, 0, sizeof(w->keys_down));
                guide_keys_released();
                break;
            case X_BUTTON_PRESS:
                guide_window_click(w, &event);
                break;
            case X_EXPOSE:
                if (event.expose.count == 0)
                    guide_draw(w);
                break;
            case X_MAP_NOTIFY:
                guide_window_activate(w);
                break;
            case X_CLIENT_MESSAGE:
                if ((x_atom)event.client.data[0] == w->delete_atom)
                    guide_key(GUIDE_KEY_CLOSE, 0);
                break;
            case X_DESTROY_NOTIFY:
                w->window = 0; // already gone
                guide_key(GUIDE_KEY_CLOSE, 0);
                break;
            default:
                break;
            }
            continue;
        }
        struct pollfd fds[2];
        fds[0].fd = x_fd;
        fds[0].events = POLLIN;
        fds[0].revents = 0;
        nfds_t nfds = 1;
        if (g_wake[0] >= 0) {
            fds[1].fd = g_wake[0];
            fds[1].events = POLLIN;
            fds[1].revents = 0;
            nfds = 2;
        }
        if (poll(fds, nfds, 250) > 0 && nfds == 2 && (fds[1].revents & POLLIN) != 0) {
            char drain[64];
            while (read(g_wake[0], drain, sizeof(drain)) > 0)
                ;
        }
    }
    guide_window_close(w);
    free(w);
    __atomic_sub_fetch(&g_windows, 1, __ATOMIC_ACQ_REL);
    return 0;
}

int guide_host_show(unsigned long session)
{
    if (port_window_native_window() == 0)
        return 0; // no game window (HALO3_WINDOW=0): nobody would see the guide
    if (!guide_x_load())
        return 0;
    pthread_once(&g_wake_once, wake_open);
    guide_window *w = (guide_window *)calloc(1, sizeof(guide_window));
    if (w == 0)
        return 0;
    w->session = session;
    w->osk_columns = guide_view_get_osk(&w->view, w->osk, w->osk_selected); // the opening panes set its width
    if (!w->view.open || w->view.session != session || !guide_window_open(w)) {
        free(w);
        return 0;
    }
    __atomic_add_fetch(&g_windows, 1, __ATOMIC_ACQ_REL);
    pthread_attr_t attributes;
    pthread_t thread;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    const int started = pthread_create(&thread, &attributes, guide_window_main, w) == 0;
    pthread_attr_destroy(&attributes);
    if (!started) {
        port_write_error("guide: cannot start the window's thread\n");
        __atomic_sub_fetch(&g_windows, 1, __ATOMIC_ACQ_REL);
        guide_window_close(w);
        free(w);
        return 0;
    }
    return 1;
}

// ---- the guide's worker threads (guide_host.h) ----

typedef struct {
    void (*entry)(void *argument);
    void *argument;
} guide_thread_start;

static void *guide_thread_main(void *start_argument)
{
    guide_thread_start start = *(guide_thread_start *)start_argument;
    free(start_argument);
    start.entry(start.argument);
    return 0;
}

int guide_host_thread_start(void (*entry)(void *argument), void *argument)
{
    guide_thread_start *start = (guide_thread_start *)malloc(sizeof(*start));
    if (start == 0)
        return 0;
    start->entry = entry;
    start->argument = argument;
    pthread_attr_t attributes;
    pthread_t thread;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    const int started = pthread_create(&thread, &attributes, guide_thread_main, start) == 0;
    pthread_attr_destroy(&attributes);
    if (!started)
        free(start);
    return started;
}
#endif // !_WIN32
