// port/sdk/online (Linux host): the virtual keyboard's dialog (keyboard_ui_host.h) through Xlib, opened at run time
// from libX11.so.6 (RTLD_LOCAL, as the game window: port/sdk/runtime/window_linux.c) and nothing else: no toolkit.
//
// The dialog is a small window of its own, transient for the game window and centred over it, on a display
// connection of its own that only the dialog's thread uses after it is set up, so it never shares Xlib state with the
// game window's connection and the game's frames go on while the player types. It shows the title, the description
// (wrapped), the text with its caret, and "Enter: OK  Esc: Cancel" with the count of characters left. Keys:
//   characters             typed at the caret (a selection is replaced); a key that would pass the limit beeps
//   Backspace, Delete      the character before / after the caret (or the selection)
//   Left, Right, Home, End the caret                              Ctrl+A select all, Ctrl+U clear
//   Enter (or keypad)      accept                                 Escape, the window's close button: cancel
// Typing goes through the X input method in its built-in form (XSetLocaleModifiers("@im=none"): no IM daemon; dead
// keys and Compose work), whose Xutf8LookupString gives every keysym's character whatever the process locale. Without
// an input method the keysym itself is used (Latin-1 and Unicode keysyms, the keypad). Text is drawn with a core
// font, an ISO 10646 one when the server has it ("fixed", Latin-1, otherwise; characters it lacks show as '?').
#include "keyboard_ui_host.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short utf16;

// ---- the text being edited (no Xlib: tests/test_virtual_kbd.c drives it directly) ----

typedef struct kbd_edit {
    utf16 text[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    unsigned long length, caret, max_length;
    int selected; // the whole text is selected (VKBD_HIGHLIGHT_TEXT, Ctrl+A)
} kbd_edit;

static int kbd_is_high(utf16 unit) { return unit >= 0xd800 && unit <= 0xdbff; }
static int kbd_is_low(utf16 unit) { return unit >= 0xdc00 && unit <= 0xdfff; }

// Whether a code point is a character the player can type (not a C0 or C1 control, DEL, or a surrogate).
static int kbd_typable(unsigned long code)
{
    return code >= 0x20 && !(code >= 0x7f && code < 0xa0) && !(code >= 0xd800 && code <= 0xdfff) && code <= 0x10ffff;
}

static unsigned long kbd_length(const utf16 *text)
{
    unsigned long length = 0;
    while (text[length] != 0)
        length++;
    return length;
}

static void kbd_edit_init(kbd_edit *edit, const utf16 *text, unsigned long max_length, int selected)
{
    memset(edit, 0, sizeof(*edit));
    edit->max_length = max_length < KEYBOARD_UI_HOST_TEXT_MAX ? max_length : KEYBOARD_UI_HOST_TEXT_MAX;
    while (edit->length < edit->max_length && text[edit->length] != 0)
        edit->length++;
    if (edit->length > 0 && text[edit->length] != 0 && kbd_is_high(text[edit->length - 1]))
        edit->length--; // never half a surrogate pair
    memcpy(edit->text, text, edit->length * sizeof(utf16));
    edit->text[edit->length] = 0;
    edit->caret = edit->length;
    edit->selected = selected && edit->length > 0;
}

static void kbd_edit_remove(kbd_edit *edit, unsigned long at, unsigned long count)
{
    memmove(edit->text + at, edit->text + at + count, (edit->length - at - count) * sizeof(utf16));
    edit->length -= count;
    edit->text[edit->length] = 0;
}

// The selection, if any, removed. Returns 1 when there was one.
static int kbd_edit_take_selection(kbd_edit *edit)
{
    if (!edit->selected)
        return 0;
    edit->selected = 0;
    edit->length = edit->caret = 0;
    edit->text[0] = 0;
    return 1;
}

// One character (a Unicode code point) typed at the caret. Returns 0, changing nothing, for one that is not
// kbd_typable, and when it would not fit (the text keeps its selection then).
static int kbd_edit_insert(kbd_edit *edit, unsigned long code)
{
    if (!kbd_typable(code))
        return 0;
    const unsigned long units = code >= 0x10000 ? 2 : 1;
    const unsigned long length = edit->selected ? 0 : edit->length;
    if (length + units > edit->max_length)
        return 0;
    kbd_edit_take_selection(edit);
    memmove(edit->text + edit->caret + units, edit->text + edit->caret, (edit->length - edit->caret) * sizeof(utf16));
    if (units == 2) {
        edit->text[edit->caret] = (utf16)(0xd800 + ((code - 0x10000) >> 10));
        edit->text[edit->caret + 1] = (utf16)(0xdc00 + ((code - 0x10000) & 0x3ff));
    } else {
        edit->text[edit->caret] = (utf16)code;
    }
    edit->length += units;
    edit->caret += units;
    edit->text[edit->length] = 0;
    return 1;
}

// The caret's neighbour to the left / right, a surrogate pair as one character.
static unsigned long kbd_edit_before(const kbd_edit *edit, unsigned long at)
{
    if (at == 0)
        return 0;
    return at >= 2 && kbd_is_low(edit->text[at - 1]) && kbd_is_high(edit->text[at - 2]) ? at - 2 : at - 1;
}
static unsigned long kbd_edit_after(const kbd_edit *edit, unsigned long at)
{
    if (at >= edit->length)
        return edit->length;
    return at + 1 < edit->length && kbd_is_high(edit->text[at]) && kbd_is_low(edit->text[at + 1]) ? at + 2 : at + 1;
}

enum kbd_key {
    k_kbd_backspace, k_kbd_delete, k_kbd_left, k_kbd_right, k_kbd_home, k_kbd_end, k_kbd_select_all, k_kbd_clear
};

static void kbd_edit_key(kbd_edit *edit, enum kbd_key key)
{
    switch (key) {
    case k_kbd_backspace:
        if (!kbd_edit_take_selection(edit) && edit->caret > 0) {
            const unsigned long from = kbd_edit_before(edit, edit->caret);
            kbd_edit_remove(edit, from, edit->caret - from);
            edit->caret = from;
        }
        break;
    case k_kbd_delete:
        if (!kbd_edit_take_selection(edit) && edit->caret < edit->length)
            kbd_edit_remove(edit, edit->caret, kbd_edit_after(edit, edit->caret) - edit->caret);
        break;
    case k_kbd_left:
        edit->caret = edit->selected ? 0 : kbd_edit_before(edit, edit->caret);
        edit->selected = 0;
        break;
    case k_kbd_right:
        edit->caret = edit->selected ? edit->length : kbd_edit_after(edit, edit->caret);
        edit->selected = 0;
        break;
    case k_kbd_home:
        edit->caret = 0;
        edit->selected = 0;
        break;
    case k_kbd_end:
        edit->caret = edit->length;
        edit->selected = 0;
        break;
    case k_kbd_select_all:
        edit->selected = edit->length > 0;
        edit->caret = edit->length;
        break;
    case k_kbd_clear:
        edit->selected = 0;
        edit->length = edit->caret = 0;
        edit->text[0] = 0;
        break;
    }
}

// The character a keysym types when no input method gives one (X11/keysymdef.h): Latin-1, the Unicode keysyms
// (0x01000000 + code point) and the keypad; 0 for any other.
static unsigned long kbd_keysym_character(unsigned long keysym)
{
    if ((keysym >= 0x20 && keysym <= 0x7e) || (keysym >= 0xa0 && keysym <= 0xff))
        return keysym;
    if (keysym >= 0x01000100 && keysym <= 0x0110ffff)
        return keysym - 0x01000000;
    if (keysym >= 0xffb0 && keysym <= 0xffb9) // KP_0 .. KP_9
        return '0' + (keysym - 0xffb0);
    switch (keysym) {
    case 0xff80: return ' ';  // KP_Space
    case 0xffaa: return '*';  // KP_Multiply
    case 0xffab: return '+';  // KP_Add
    case 0xffac: return ',';  // KP_Separator
    case 0xffad: return '-';  // KP_Subtract
    case 0xffae: return '.';  // KP_Decimal
    case 0xffaf: return '/';  // KP_Divide
    case 0xffbd: return '=';  // KP_Equal
    case 0x20ac: return 0x20ac; // EuroSign
    default: return 0;
    }
}

// What a key does besides typing. Returns 1 accept, -1 cancel, 0 handled or nothing (see kbd_dialog_key).
#define KBD_CONTROL_MASK (1u << 2)
enum { k_kbd_none = 0, k_kbd_accept = 1, k_kbd_cancel = -1, k_kbd_edited = 2, k_kbd_typing = 3 };
static int kbd_keysym_action(kbd_edit *edit, unsigned long keysym, unsigned int state)
{
    switch (keysym) {
    case 0xff0d: case 0xff8d: return k_kbd_accept;  // Return, KP_Enter
    case 0xff1b: return k_kbd_cancel;               // Escape
    case 0xff08: kbd_edit_key(edit, k_kbd_backspace); return k_kbd_edited;
    case 0xffff: case 0xff9f: kbd_edit_key(edit, k_kbd_delete); return k_kbd_edited;   // Delete, KP_Delete
    case 0xff51: case 0xff96: kbd_edit_key(edit, k_kbd_left); return k_kbd_edited;     // Left, KP_Left
    case 0xff53: case 0xff98: kbd_edit_key(edit, k_kbd_right); return k_kbd_edited;    // Right, KP_Right
    case 0xff50: case 0xff95: kbd_edit_key(edit, k_kbd_home); return k_kbd_edited;     // Home, KP_Home
    case 0xff57: case 0xff9c: kbd_edit_key(edit, k_kbd_end); return k_kbd_edited;      // End, KP_End
    default: break;
    }
    if (state & KBD_CONTROL_MASK) {
        if (keysym == 'a' || keysym == 'A') {
            kbd_edit_key(edit, k_kbd_select_all);
            return k_kbd_edited;
        }
        if (keysym == 'u' || keysym == 'U') {
            kbd_edit_key(edit, k_kbd_clear);
            return k_kbd_edited;
        }
        return k_kbd_edited; // no other Ctrl shortcut types anything
    }
    return k_kbd_typing;
}

// The next code point of UTF-8 `s` (advanced past it); U+FFFD for a malformed sequence.
static unsigned long kbd_utf8_next(const char **s)
{
    const unsigned char *p = (const unsigned char *)*s;
    unsigned long code = 0xfffd;
    const unsigned int extra =
        *p >= 0xf0 && *p <= 0xf4 ? 3 : *p >= 0xe0 && *p < 0xf0 ? 2 : *p >= 0xc2 && *p < 0xe0 ? 1 : 0;
    if (*p < 0x80) {
        code = *p++;
    } else if (extra == 0) {
        p++;
    } else {
        unsigned long value = *p++ & (0x3fu >> extra);
        unsigned int n = 0;
        while (n < extra && (*p & 0xc0) == 0x80) {
            value = value << 6 | (*p++ & 0x3fu);
            n++;
        }
        const unsigned long least = extra == 1 ? 0x80 : extra == 2 ? 0x800 : 0x10000;
        if (n == extra && value >= least && value <= 0x10ffff && (value < 0xd800 || value > 0xdfff))
            code = value;
    }
    *s = (const char *)p;
    return code;
}

// UTF-16 into UTF-8 (to has `capacity` bytes, NUL-terminated); a lone surrogate becomes U+FFFD.
static void kbd_utf16_to_utf8(const utf16 *from, char *to, unsigned long capacity)
{
    unsigned long length = 0;
    for (unsigned long i = 0; from[i] != 0; i++) {
        unsigned long code = from[i];
        if (kbd_is_high(from[i]) && kbd_is_low(from[i + 1])) {
            code = 0x10000 + ((code - 0xd800) << 10) + (from[i + 1] - 0xdc00u);
            i++;
        } else if (code >= 0xd800 && code <= 0xdfff) {
            code = 0xfffd;
        }
        const unsigned long n = code < 0x80 ? 1 : code < 0x800 ? 2 : code < 0x10000 ? 3 : 4;
        if (length + n + 1 > capacity)
            break;
        static const unsigned char lead[5] = { 0, 0x00, 0xc0, 0xe0, 0xf0 };
        to[length++] = (char)(lead[n] | code >> (6 * (n - 1)));
        for (unsigned long k = 1; k < n; k++)
            to[length++] = (char)(0x80 | (code >> (6 * (n - 1 - k)) & 0x3f));
    }
    to[length] = 0;
}

// ---- Xlib ----

typedef unsigned long x_window;
typedef unsigned long x_atom;
typedef unsigned long x_keysym;
typedef struct x_display x_display;
typedef struct x_gc_record *x_gc;
typedef struct x_im_record *x_im;
typedef struct x_ic_record *x_ic;

#define X_KEY_PRESS_MASK (1L << 0)
#define X_BUTTON_PRESS_MASK (1L << 2)
#define X_EXPOSURE_MASK (1L << 15)
#define X_STRUCTURE_NOTIFY_MASK (1L << 17)
#define X_SUBSTRUCTURE_NOTIFY_MASK (1L << 19)
#define X_SUBSTRUCTURE_REDIRECT_MASK (1L << 20)
#define X_FOCUS_CHANGE_MASK (1L << 21)
#define X_KEY_PRESS 2
#define X_FOCUS_IN 9
#define X_FOCUS_OUT 10
#define X_EXPOSE 12
#define X_DESTROY_NOTIFY 17
#define X_MAP_NOTIFY 19
#define X_CLIENT_MESSAGE 33
#define X_PROP_MODE_REPLACE 0
#define X_ATOM_ATOM 4
#define X_LOOKUP_CHARS 2
#define X_LOOKUP_KEYSYM 3
#define X_LOOKUP_BOTH 4
#define X_IM_PREEDIT_NOTHING 0x0008L
#define X_IM_STATUS_NOTHING 0x0400L

// XEvent (Xlib.h): a union of 24 longs. XKeyEvent; XExposeEvent (count: more Expose events follow);
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
        unsigned int keycode;
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

// XCharStruct, XFontStruct, XChar2b, XColor, XSizeHints, XWMHints, XClassHint (Xlib.h, Xutil.h).
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

static struct {
    void *library;
    x_display *(*XOpenDisplay)(const char *);
    int (*XCloseDisplay)(x_display *);
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
    int (*XNextEvent)(x_display *, x_event *);
    int (*XLookupString)(void *, char *, int, x_keysym *, void *);
    int (*XBell)(x_display *, int);
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
    // the input method, all optional
    char *(*XSetLocaleModifiers)(const char *);
    x_im (*XOpenIM)(x_display *, void *, char *, char *);
    int (*XCloseIM)(x_im);
    x_ic (*XCreateIC)(x_im, ...);
    void (*XDestroyIC)(x_ic);
    void (*XSetICFocus)(x_ic);
    void (*XUnsetICFocus)(x_ic);
    int (*Xutf8LookupString)(x_ic, void *, char *, int, x_keysym *, int *);
    int (*XFilterEvent)(x_event *, x_window);
} g_xk;

static int kbd_load(void)
{
    if (g_xk.library != 0)
        return 1;
    void *library = dlopen("libX11.so.6", RTLD_NOW | RTLD_LOCAL);
    if (library == 0) {
        port_write_error("keyboard ui: libX11.so.6 is not available\n");
        return 0;
    }
#define LOAD(name)                                                                                                  \
    do {                                                                                                            \
        *(void **)&g_xk.name = dlsym(library, #name);                                                               \
        if (g_xk.name == 0) {                                                                                       \
            port_write_error("keyboard ui: libX11.so.6 has no " #name "\n");                                        \
            dlclose(library);                                                                                       \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    LOAD(XOpenDisplay);
    LOAD(XCloseDisplay);
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
    LOAD(XNextEvent);
    LOAD(XLookupString);
    LOAD(XBell);
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
#define LOAD_OPTIONAL(name) (*(void **)&g_xk.name = dlsym(library, #name))
    LOAD_OPTIONAL(XSetLocaleModifiers);
    LOAD_OPTIONAL(XOpenIM);
    LOAD_OPTIONAL(XCloseIM);
    LOAD_OPTIONAL(XCreateIC);
    LOAD_OPTIONAL(XDestroyIC);
    LOAD_OPTIONAL(XSetICFocus);
    LOAD_OPTIONAL(XUnsetICFocus);
    LOAD_OPTIONAL(Xutf8LookupString);
    LOAD_OPTIONAL(XFilterEvent);
#undef LOAD_OPTIONAL
    g_xk.library = library;
    return 1;
}

// ---- the dialog ----

#define KBD_MARGIN 16
#define KBD_PAD 6
#define KBD_GAP 10
#define KBD_DESCRIPTION_LINES 8
#define KBD_WIDTH_MIN 440
#define KBD_WIDTH_MAX 860

typedef struct kbd_dialog {
    keyboard_ui_host_request *request;
    kbd_edit edit;
    x_display *display;
    x_window window;
    unsigned long buffer; // the back buffer pixmap, drawn then copied (no flicker)
    x_gc gc;
    x_font *font;
    int wide_font; // the font has glyphs past U+00FF
    x_im im;
    x_ic ic;
    x_atom delete_atom;
    unsigned long background, foreground, dim, field, selection;
    int width, height, line_height, field_y, field_height, hint_y;
    // the description, wrapped: start and length (UTF-16 units) of each line
    unsigned long line_start[KBD_DESCRIPTION_LINES], line_length[KBD_DESCRIPTION_LINES];
    int line_count;
    unsigned long first_visible; // the first unit of the text shown in the field (it scrolls to keep the caret)
} kbd_dialog;

// `count` units of `text` as glyphs (password: one '*' per character); returns the number of glyphs.
static int kbd_glyphs(const kbd_dialog *dialog, const utf16 *text, unsigned long count, int password, x_char2b *out)
{
    int n = 0;
    for (unsigned long i = 0; i < count; i++) {
        unsigned long code = text[i];
        if (kbd_is_high(text[i]) && i + 1 < count && kbd_is_low(text[i + 1])) {
            i++;
            code = '?'; // a core font has no glyphs past the Basic Multilingual Plane
        } else if ((code >= 0xd800 && code <= 0xdfff) || (!dialog->wide_font && code > 0xff)) {
            code = '?';
        }
        if (password)
            code = '*';
        out[n].byte1 = (unsigned char)(code >> 8);
        out[n].byte2 = (unsigned char)code;
        n++;
    }
    return n;
}

static int kbd_text_width(const kbd_dialog *dialog, const utf16 *text, unsigned long count, int password)
{
    static x_char2b glyphs[KEYBOARD_UI_HOST_TEXT_MAX + 1]; // the dialog's thread only
    const int n = kbd_glyphs(dialog, text, count, password, glyphs);
    return n > 0 ? g_xk.XTextWidth16(dialog->font, glyphs, n) : 0;
}

static void kbd_draw_text(kbd_dialog *dialog, int x, int y, const utf16 *text, unsigned long count, int password)
{
    static x_char2b glyphs[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    const int n = kbd_glyphs(dialog, text, count, password, glyphs);
    if (n > 0)
        g_xk.XDrawString16(dialog->display, dialog->buffer, dialog->gc, x, y, glyphs, n);
}

static void kbd_draw_ascii(kbd_dialog *dialog, int x, int y, const char *text)
{
    utf16 units[64];
    unsigned long n = 0;
    for (; text[n] != 0 && n < 63; n++)
        units[n] = (unsigned char)text[n];
    units[n] = 0;
    kbd_draw_text(dialog, x, y, units, n, 0);
}

static int kbd_ascii_width(const kbd_dialog *dialog, const char *text)
{
    utf16 units[64];
    unsigned long n = 0;
    for (; text[n] != 0 && n < 63; n++)
        units[n] = (unsigned char)text[n];
    units[n] = 0;
    return kbd_text_width(dialog, units, n, 0);
}

// Wraps the description into at most KBD_DESCRIPTION_LINES lines of `width` pixels, at spaces where it can and at
// line breaks ('\n'; '\r' is dropped as a space).
static void kbd_wrap_description(kbd_dialog *dialog, int width)
{
    const utf16 *text = dialog->request->description;
    unsigned long at = 0;
    dialog->line_count = 0;
    while (text[at] != 0 && dialog->line_count < KBD_DESCRIPTION_LINES) {
        while (text[at] == ' ' || text[at] == '\r')
            at++;
        if (text[at] == 0)
            break;
        unsigned long end = at, fit = at, last_space = 0;
        while (text[end] != 0 && text[end] != '\n') {
            const unsigned long next = end + 1;
            if (kbd_text_width(dialog, text + at, next - at, 0) > width)
                break;
            if (text[end] == ' ' || text[end] == '\r')
                last_space = end;
            end = next;
            fit = end;
        }
        unsigned long line_end = fit;
        if (text[fit] != 0 && text[fit] != '\n' && last_space > at)
            line_end = last_space; // break at the last space that fits
        if (line_end == at)
            line_end = at + 1; // one glyph wider than the line: shown anyway
        dialog->line_start[dialog->line_count] = at;
        dialog->line_length[dialog->line_count] = line_end - at;
        dialog->line_count++;
        at = line_end;
        if (text[at] == '\n')
            at++;
    }
}

static void kbd_layout(kbd_dialog *dialog)
{
    const keyboard_ui_host_request *request = dialog->request;
    dialog->line_height = dialog->font->ascent + dialog->font->descent;
    const int em = kbd_ascii_width(dialog, "M");
    int width = (int)request->max_length * em + 2 * (KBD_MARGIN + KBD_PAD) + em;
    const int title = kbd_text_width(dialog, request->title, kbd_length(request->title), 0) + 2 * KBD_MARGIN;
    width = width > title ? width : title;
    width = width < KBD_WIDTH_MIN ? KBD_WIDTH_MIN : width > KBD_WIDTH_MAX ? KBD_WIDTH_MAX : width;
    dialog->width = width;
    kbd_wrap_description(dialog, width - 2 * KBD_MARGIN);
    int y = KBD_MARGIN + dialog->line_height; // the title
    if (dialog->line_count > 0)
        y += KBD_GAP + dialog->line_count * dialog->line_height;
    dialog->field_y = y + KBD_GAP;
    dialog->field_height = dialog->line_height + 2 * KBD_PAD;
    dialog->hint_y = dialog->field_y + dialog->field_height + KBD_GAP + dialog->font->ascent;
    dialog->height = dialog->hint_y + dialog->font->descent + KBD_MARGIN;
}

static void kbd_draw(kbd_dialog *dialog)
{
    const keyboard_ui_host_request *request = dialog->request;
    kbd_edit *edit = &dialog->edit;
    x_display *display = dialog->display;
    const unsigned long target = dialog->buffer;
    const int ascent = dialog->font->ascent;
    g_xk.XSetForeground(display, dialog->gc, dialog->background);
    g_xk.XFillRectangle(display, target, dialog->gc, 0, 0, (unsigned)dialog->width, (unsigned)dialog->height);
    // the title, and the description under it
    g_xk.XSetForeground(display, dialog->gc, dialog->foreground);
    const unsigned long title_length = kbd_length(request->title);
    kbd_draw_text(dialog, KBD_MARGIN, KBD_MARGIN + ascent, request->title, title_length, 0);
    kbd_draw_text(dialog, KBD_MARGIN + 1, KBD_MARGIN + ascent, request->title, title_length, 0); // bold
    g_xk.XSetForeground(display, dialog->gc, dialog->dim);
    for (int line = 0; line < dialog->line_count; line++)
        kbd_draw_text(dialog, KBD_MARGIN,
                      KBD_MARGIN + dialog->line_height + KBD_GAP + line * dialog->line_height + ascent,
                      request->description + dialog->line_start[line], dialog->line_length[line], 0);
    // the field: scrolled so the caret shows
    const int field_x = KBD_MARGIN, field_width = dialog->width - 2 * KBD_MARGIN;
    const int inner = field_width - 2 * KBD_PAD - 2;
    g_xk.XSetForeground(display, dialog->gc, dialog->field);
    g_xk.XFillRectangle(display, target, dialog->gc, field_x, dialog->field_y, (unsigned)field_width,
                        (unsigned)dialog->field_height);
    g_xk.XSetForeground(display, dialog->gc, dialog->dim);
    g_xk.XDrawRectangle(display, target, dialog->gc, field_x, dialog->field_y, (unsigned)field_width - 1,
                        (unsigned)dialog->field_height - 1);
    if (dialog->first_visible > edit->caret)
        dialog->first_visible = edit->caret;
    while (dialog->first_visible < edit->caret &&
           kbd_text_width(dialog, edit->text + dialog->first_visible, edit->caret - dialog->first_visible,
                          request->password) > inner)
        dialog->first_visible = kbd_edit_after(edit, dialog->first_visible);
    unsigned long shown = edit->length - dialog->first_visible;
    while (shown > 0 && kbd_text_width(dialog, edit->text + dialog->first_visible, shown, request->password) > inner)
        shown = kbd_edit_before(edit, dialog->first_visible + shown) - dialog->first_visible;
    const int text_x = field_x + KBD_PAD + 1, text_y = dialog->field_y + KBD_PAD + ascent;
    if (edit->selected && shown > 0) {
        const int selected = kbd_text_width(dialog, edit->text + dialog->first_visible, shown, request->password);
        g_xk.XSetForeground(display, dialog->gc, dialog->selection);
        g_xk.XFillRectangle(display, target, dialog->gc, text_x, dialog->field_y + KBD_PAD, (unsigned)selected,
                            (unsigned)dialog->line_height);
    }
    g_xk.XSetForeground(display, dialog->gc, dialog->foreground);
    kbd_draw_text(dialog, text_x, text_y, edit->text + dialog->first_visible, shown, request->password);
    const int caret_x = text_x + kbd_text_width(dialog, edit->text + dialog->first_visible,
                                                edit->caret - dialog->first_visible, request->password);
    g_xk.XFillRectangle(display, target, dialog->gc, caret_x, dialog->field_y + KBD_PAD, 2,
                        (unsigned)dialog->line_height);
    // the keys, and how many characters are left
    g_xk.XSetForeground(display, dialog->gc, dialog->dim);
    kbd_draw_ascii(dialog, KBD_MARGIN, dialog->hint_y, "Enter: OK    Esc: Cancel");
    char count[32];
    unsigned long left = edit->max_length - edit->length, n = 0;
    char digits[12];
    do {
        digits[n++] = (char)('0' + left % 10);
        left /= 10;
    } while (left != 0 && n < sizeof(digits));
    unsigned long at = 0;
    while (n > 0)
        count[at++] = digits[--n];
    memcpy(count + at, " left", 6);
    kbd_draw_ascii(dialog, dialog->width - KBD_MARGIN - kbd_ascii_width(dialog, count), dialog->hint_y, count);
    g_xk.XCopyArea(display, target, dialog->window, dialog->gc, 0, 0, (unsigned)dialog->width, (unsigned)dialog->height,
                   0, 0);
    g_xk.XFlush(display);
}

// A pixel for 0xRRGGBB on the default colormap, `fallback` when the server will not give one.
static unsigned long kbd_color(kbd_dialog *dialog, unsigned long rgb, unsigned long fallback)
{
    x_color color;
    memset(&color, 0, sizeof(color));
    color.red = (unsigned short)((rgb >> 16 & 0xff) * 0x101);
    color.green = (unsigned short)((rgb >> 8 & 0xff) * 0x101);
    color.blue = (unsigned short)((rgb & 0xff) * 0x101);
    const int screen = g_xk.XDefaultScreen(dialog->display);
    return g_xk.XAllocColor(dialog->display, g_xk.XDefaultColormap(dialog->display, screen), &color) ? color.pixel
                                                                                                    : fallback;
}

static void kbd_dialog_close(kbd_dialog *dialog)
{
    if (dialog->ic != 0)
        g_xk.XDestroyIC(dialog->ic);
    if (dialog->im != 0)
        g_xk.XCloseIM(dialog->im);
    if (dialog->gc != 0)
        g_xk.XFreeGC(dialog->display, dialog->gc);
    if (dialog->buffer != 0)
        g_xk.XFreePixmap(dialog->display, dialog->buffer);
    if (dialog->font != 0)
        g_xk.XFreeFont(dialog->display, dialog->font);
    if (dialog->window != 0)
        g_xk.XDestroyWindow(dialog->display, dialog->window);
    g_xk.XCloseDisplay(dialog->display);
}

// Opens the display, the font and the window (mapped). Returns 0, with nothing left open, when it cannot.
static int kbd_dialog_open(kbd_dialog *dialog)
{
    dialog->display = g_xk.XOpenDisplay(0);
    if (dialog->display == 0) {
        port_write_error("keyboard ui: cannot open the X display\n");
        return 0;
    }
    x_display *display = dialog->display;
    static const char *const fonts[] = {
        "-misc-fixed-medium-r-normal--18-*-*-*-*-*-iso10646-1",
        "-misc-fixed-medium-r-normal--15-*-*-*-*-*-iso10646-1",
        "-misc-fixed-medium-r-normal--13-*-*-*-*-*-iso10646-1",
        "-*-*-medium-r-normal--*-140-*-*-*-*-iso10646-1",
        "9x15",
        "fixed",
    };
    for (unsigned i = 0; i < sizeof(fonts) / sizeof(fonts[0]) && dialog->font == 0; i++)
        dialog->font = g_xk.XLoadQueryFont(display, fonts[i]);
    if (dialog->font == 0) {
        port_write_error("keyboard ui: the X server has no core font\n");
        g_xk.XCloseDisplay(display);
        return 0;
    }
    dialog->wide_font = dialog->font->max_byte1 > 0;
    const int screen = g_xk.XDefaultScreen(display);
    const x_window root = g_xk.XRootWindow(display, screen);
    dialog->background = kbd_color(dialog, 0x1e2227, g_xk.XBlackPixel(display, screen));
    dialog->foreground = kbd_color(dialog, 0xf0f0f0, g_xk.XWhitePixel(display, screen));
    dialog->dim = kbd_color(dialog, 0xa0a8b0, dialog->foreground);
    dialog->field = kbd_color(dialog, 0x0e1014, dialog->background);
    dialog->selection = kbd_color(dialog, 0x2f5f8f, dialog->dim);
    kbd_layout(dialog);

    // centred over the game window
    const x_window game = (x_window)port_window_native_window();
    int x = 0, y = 0;
    x_window ignored_root = 0, ignored_child = 0;
    int game_x = 0, game_y = 0;
    unsigned int game_width = 0, game_height = 0, border = 0, depth = 0;
    if (game != 0 &&
        g_xk.XGetGeometry(display, game, &ignored_root, &game_x, &game_y, &game_width, &game_height, &border, &depth) &&
        g_xk.XTranslateCoordinates(display, game, root, 0, 0, &game_x, &game_y, &ignored_child)) {
        x = game_x + ((int)game_width - dialog->width) / 2;
        y = game_y + ((int)game_height - dialog->height) / 2;
    }
    dialog->window = g_xk.XCreateSimpleWindow(display, root, x, y, (unsigned)dialog->width, (unsigned)dialog->height, 0,
                                              dialog->background, dialog->background);
    if (dialog->window == 0) {
        port_write_error("keyboard ui: XCreateSimpleWindow failed\n");
        g_xk.XFreeFont(display, dialog->font);
        g_xk.XCloseDisplay(display);
        return 0;
    }
    const x_window window = dialog->window;

    // the window manager's view: a fixed-size modal dialog of the game window, titled in UTF-8 (and Latin-1)
    char title[3 * KEYBOARD_UI_HOST_TEXT_MAX + 1];
    kbd_utf16_to_utf8(dialog->request->title, title, sizeof(title));
    if (title[0] == 0)
        memcpy(title, "Keyboard", 9);
    const x_atom utf8_string = g_xk.XInternAtom(display, "UTF8_STRING", 0);
    g_xk.XChangeProperty(display, window, g_xk.XInternAtom(display, "_NET_WM_NAME", 0), utf8_string, 8,
                         X_PROP_MODE_REPLACE, (const unsigned char *)title, (int)strlen(title));
    char latin1[KEYBOARD_UI_HOST_TEXT_MAX + 1];
    unsigned long n = 0;
    for (const utf16 *c = dialog->request->title; *c != 0 && n < KEYBOARD_UI_HOST_TEXT_MAX; c++)
        latin1[n++] = *c < 0x100 ? (char)*c : '?';
    latin1[n] = 0;
    g_xk.XStoreName(display, window, n > 0 ? latin1 : "Keyboard");
    x_atom type = g_xk.XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", 0);
    g_xk.XChangeProperty(display, window, g_xk.XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0), X_ATOM_ATOM, 32,
                         X_PROP_MODE_REPLACE, (const unsigned char *)&type, 1);
    x_atom state = g_xk.XInternAtom(display, "_NET_WM_STATE_MODAL", 0);
    g_xk.XChangeProperty(display, window, g_xk.XInternAtom(display, "_NET_WM_STATE", 0), X_ATOM_ATOM, 32,
                         X_PROP_MODE_REPLACE, (const unsigned char *)&state, 1);
    if (game != 0)
        g_xk.XSetTransientForHint(display, window, game);
    x_size_hints size;
    memset(&size, 0, sizeof(size));
    size.flags = X_US_POSITION | X_P_POSITION | X_P_MIN_SIZE | X_P_MAX_SIZE;
    size.x = x;
    size.y = y;
    size.width = size.min_width = size.max_width = dialog->width;
    size.height = size.min_height = size.max_height = dialog->height;
    g_xk.XSetWMNormalHints(display, window, &size);
    x_wm_hints hints;
    memset(&hints, 0, sizeof(hints));
    hints.flags = X_INPUT_HINT;
    hints.input = 1;
    g_xk.XSetWMHints(display, window, &hints);
    char name[] = "halo3", class_name[] = "Halo3";
    x_class_hint class_hint = { name, class_name };
    g_xk.XSetClassHint(display, window, &class_hint);
    dialog->delete_atom = g_xk.XInternAtom(display, "WM_DELETE_WINDOW", 0);
    g_xk.XSetWMProtocols(display, window, &dialog->delete_atom, 1);
    g_xk.XSelectInput(display, window, X_KEY_PRESS_MASK | X_BUTTON_PRESS_MASK | X_EXPOSURE_MASK |
                      X_STRUCTURE_NOTIFY_MASK | X_FOCUS_CHANGE_MASK);

    // the built-in input method (dead keys, Compose, every keysym's character), when there is one
    if (g_xk.XSetLocaleModifiers != 0 && g_xk.XOpenIM != 0 && g_xk.XCloseIM != 0 && g_xk.XCreateIC != 0 &&
        g_xk.XDestroyIC != 0 && g_xk.Xutf8LookupString != 0 && g_xk.XFilterEvent != 0) {
        g_xk.XSetLocaleModifiers("@im=none");
        dialog->im = g_xk.XOpenIM(display, 0, 0, 0);
        if (dialog->im != 0) {
            dialog->ic = g_xk.XCreateIC(dialog->im, "inputStyle", X_IM_PREEDIT_NOTHING | X_IM_STATUS_NOTHING,
                                        "clientWindow", window, "focusWindow", window, (void *)0);
            if (dialog->ic == 0) {
                g_xk.XCloseIM(dialog->im);
                dialog->im = 0;
            }
        }
    }

    dialog->buffer = g_xk.XCreatePixmap(display, window, (unsigned)dialog->width, (unsigned)dialog->height,
                                        (unsigned)g_xk.XDefaultDepth(display, screen));
    dialog->gc = g_xk.XCreateGC(display, window, 0, 0);
    g_xk.XSetFont(display, dialog->gc, dialog->font->fid);
    g_xk.XMapRaised(display, window);
    g_xk.XFlush(display);
    return 1;
}

// Asks the window manager to give the dialog the focus (EWMH _NET_ACTIVE_WINDOW from an application).
static void kbd_dialog_activate(kbd_dialog *dialog)
{
    x_event event;
    memset(&event, 0, sizeof(event));
    event.client.type = X_CLIENT_MESSAGE;
    event.client.window = dialog->window;
    event.client.message_type = g_xk.XInternAtom(dialog->display, "_NET_ACTIVE_WINDOW", 0);
    event.client.format = 32;
    event.client.data[0] = 1; // source: an application
    const x_window root = g_xk.XRootWindow(dialog->display, g_xk.XDefaultScreen(dialog->display));
    g_xk.XSendEvent(dialog->display, root, 0, X_SUBSTRUCTURE_REDIRECT_MASK | X_SUBSTRUCTURE_NOTIFY_MASK, &event);
    g_xk.XFlush(dialog->display);
}

// A key press: returns k_kbd_accept, k_kbd_cancel, or another value to keep going (redrawn).
static int kbd_dialog_key(kbd_dialog *dialog, x_event *event)
{
    char typed[64];
    x_keysym keysym = 0;
    int count = 0;
    unsigned long code = 0; // without an input method: the keysym's character
    if (dialog->ic != 0) {
        int status = 0;
        count = g_xk.Xutf8LookupString(dialog->ic, event, typed, (int)sizeof(typed) - 1, &keysym, &status);
        if (status != X_LOOKUP_CHARS && status != X_LOOKUP_BOTH)
            count = 0;
        if (status != X_LOOKUP_KEYSYM && status != X_LOOKUP_BOTH)
            keysym = 0;
    } else {
        g_xk.XLookupString(event, typed, (int)sizeof(typed) - 1, &keysym, 0); // its bytes are Latin-1: unused
        code = kbd_keysym_character(keysym);
    }
    typed[count > 0 ? count : 0] = 0;
    const int action = keysym != 0 ? kbd_keysym_action(&dialog->edit, keysym, event->key.state) : k_kbd_typing;
    if (action != k_kbd_typing)
        return action;
    int full = 0;
    if (code != 0)
        full = kbd_typable(code) && !kbd_edit_insert(&dialog->edit, code);
    for (const char *c = typed; *c != 0 && !full;) {
        code = kbd_utf8_next(&c);
        full = kbd_typable(code) && !kbd_edit_insert(&dialog->edit, code);
    }
    if (full)
        g_xk.XBell(dialog->display, 0);
    return k_kbd_edited;
}

static void *kbd_dialog_main(void *argument)
{
    kbd_dialog *dialog = (kbd_dialog *)argument;
    int result = k_kbd_none;
    while (result != k_kbd_accept && result != k_kbd_cancel) {
        x_event event;
        memset(&event, 0, sizeof(event));
        g_xk.XNextEvent(dialog->display, &event);
        if (dialog->ic != 0 && g_xk.XFilterEvent(&event, 0))
            continue; // a dead key or a Compose sequence in progress
        switch (event.type) {
        case X_KEY_PRESS:
            result = kbd_dialog_key(dialog, &event);
            if (result != k_kbd_accept && result != k_kbd_cancel)
                kbd_draw(dialog);
            break;
        case X_EXPOSE:
            if (event.expose.count == 0)
                kbd_draw(dialog);
            break;
        case X_MAP_NOTIFY:
            kbd_dialog_activate(dialog);
            break;
        case X_FOCUS_IN:
            if (dialog->ic != 0 && g_xk.XSetICFocus != 0)
                g_xk.XSetICFocus(dialog->ic);
            break;
        case X_FOCUS_OUT:
            if (dialog->ic != 0 && g_xk.XUnsetICFocus != 0)
                g_xk.XUnsetICFocus(dialog->ic);
            break;
        case X_CLIENT_MESSAGE:
            if ((x_atom)event.client.data[0] == dialog->delete_atom)
                result = k_kbd_cancel;
            break;
        case X_DESTROY_NOTIFY:
            result = k_kbd_cancel;
            dialog->window = 0; // already gone
            break;
        default:
            break;
        }
    }
    keyboard_ui_host_request *request = dialog->request;
    const int accepted = result == k_kbd_accept;
    if (accepted) {
        memcpy(request->text, dialog->edit.text, (dialog->edit.length + 1) * sizeof(utf16));
    }
    kbd_dialog_close(dialog);
    free(dialog);
    request->done(request, accepted); // last: the request is the caller's again
    return 0;
}

PORT_MS_ABI int keyboard_ui_host_show(keyboard_ui_host_request *request)
{
    if (port_window_native_window() == 0)
        return 0; // no game window (HALO3_WINDOW=0): nobody would see the dialog
    if (!kbd_load())
        return 0;
    kbd_dialog *dialog = (kbd_dialog *)calloc(1, sizeof(kbd_dialog));
    if (dialog == 0)
        return 0;
    dialog->request = request;
    kbd_edit_init(&dialog->edit, request->default_text, request->max_length, request->highlight);
    if (!kbd_dialog_open(dialog)) {
        free(dialog);
        return 0;
    }
    pthread_attr_t attributes;
    pthread_t thread;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    const int started = pthread_create(&thread, &attributes, kbd_dialog_main, dialog) == 0;
    pthread_attr_destroy(&attributes);
    if (!started) {
        port_write_error("keyboard ui: cannot start the dialog's thread\n");
        kbd_dialog_close(dialog);
        free(dialog);
        return 0;
    }
    return 1;
}

PORT_MS_ABI int keyboard_ui_host_scripted_text(char *utf8, unsigned int capacity)
{
    const char *text = getenv("HALO3_KBD_TEXT");
    if (text == 0 || capacity == 0)
        return 0;
    unsigned long n = 0;
    for (; text[n] != 0 && n + 1 < capacity; n++)
        utf8[n] = text[n];
    utf8[n] = 0;
    return 1;
}
