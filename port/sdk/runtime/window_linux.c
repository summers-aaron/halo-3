// port/sdk/runtime (Linux host): the game window through Xlib, opened at run time from libX11.so.6 (RTLD_LOCAL: the
// library's symbols stay out of the global namespace where the game's names live). Interface: port_window.h.
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "port_window.h"
#include "port_config.h"
#include "port_runtime.h"
#include "../input/port_input.h"
#include "../xam/guide_host.h"
#include "../xam/guide_ingame.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// The Xlib subset used, with the types it takes (Xlib.h, Xutil.h): XID-based handles are unsigned long.
typedef unsigned long x_window;
typedef unsigned long x_atom;
typedef struct x_display x_display;

#define X_KEY_PRESS_MASK (1L << 0)
#define X_KEY_RELEASE_MASK (1L << 1)
#define X_BUTTON_PRESS_MASK (1L << 2)
#define X_BUTTON_RELEASE_MASK (1L << 3)
#define X_POINTER_MOTION_MASK (1L << 6)
#define X_EXPOSURE_MASK (1L << 15)
#define X_STRUCTURE_NOTIFY_MASK (1L << 17)
#define X_FOCUS_CHANGE_MASK (1L << 21)
#define X_SUBSTRUCTURE_NOTIFY_MASK (1L << 19)
#define X_SUBSTRUCTURE_REDIRECT_MASK (1L << 20)
#define X_ATOM_ATOM 4 // XA_ATOM
#define X_PROP_MODE_REPLACE 0
#define X_KEY_PRESS 2
#define X_KEY_RELEASE 3
#define X_BUTTON_PRESS 4
#define X_BUTTON_RELEASE 5
#define X_MOTION_NOTIFY 6
#define X_FOCUS_IN 9
#define X_FOCUS_OUT 10
#define X_EXPOSE 12
#define X_DESTROY_NOTIFY 17
#define X_CONFIGURE_NOTIFY 22
#define X_CLIENT_MESSAGE 33
#define X_GENERIC_EVENT 35
#define X_GRAB_MODE_ASYNC 1
#define X_GRAB_SUCCESS 0
// XInput2 (XI2.h): raw motion, selected on the root window for every master device
#define XI_RAW_MOTION 17
#define XI_ALL_MASTER_DEVICES 1

// XEvent: a union of 24 longs; XClientMessageEvent starts type, serial, send_event, display, window, message_type,
// format, data. XKeyEvent, XButtonEvent and XMotionEvent share their first fields up to state, then keycode / button.
// XGenericEventCookie (an XInput2 event): type, serial, send_event, display, extension, evtype, cookie, data.
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
        unsigned int detail; // keycode (XKeyEvent), button (XButtonEvent); XMotionEvent: is_hint
        int same_screen;
    } input;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window event, window;
        int x, y, width, height, border_width;
        x_window above;
        int override_redirect;
    } configure;
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
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        int extension;
        int evtype;
        unsigned int cookie;
        void *data;
    } cookie;
    long pad[24];
} x_event;

// XInput2 (XInput2.h): XIEventMask, and XIRawEvent with its XIValuatorState (one double per set mask bit, in order).
typedef struct {
    int deviceid;
    int mask_len;
    unsigned char *mask;
} xi_event_mask;

typedef struct {
    int type;
    unsigned long serial;
    int send_event;
    x_display *display;
    int extension;
    int evtype;
    unsigned long time;
    int deviceid, sourceid, detail, flags;
    struct {
        int mask_len;
        unsigned char *mask;
        double *values;
    } valuators;
    double *raw_values;
} xi_raw_event;

// XColor (for the blank cursor).
typedef struct {
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags, pad;
} x_color;

static struct {
    void *library;
    x_display *(*XOpenDisplay)(const char *);
    int (*XCloseDisplay)(x_display *);
    int (*XDefaultScreen)(x_display *);
    x_window (*XRootWindow)(x_display *, int);
    unsigned long (*XBlackPixel)(x_display *, int);
    x_window (*XCreateSimpleWindow)(x_display *, x_window, int, int, unsigned int, unsigned int, unsigned int,
                                    unsigned long, unsigned long);
    int (*XDestroyWindow)(x_display *, x_window);
    int (*XStoreName)(x_display *, x_window, const char *);
    int (*XSelectInput)(x_display *, x_window, long);
    x_atom (*XInternAtom)(x_display *, const char *, int);
    int (*XSetWMProtocols)(x_display *, x_window, x_atom *, int);
    int (*XMapWindow)(x_display *, x_window);
    int (*XFlush)(x_display *);
    int (*XPending)(x_display *);
    int (*XNextEvent)(x_display *, x_event *);
    unsigned long (*XLookupKeysym)(x_event *, int);
    int (*XLookupString)(x_event *, char *, int, unsigned long *, void *);
    int (*XWarpPointer)(x_display *, x_window, x_window, int, int, unsigned int, unsigned int, int, int);
    int (*XkbSetDetectableAutoRepeat)(x_display *, int, int *); // optional: key repeat without release events
    // optional: the window modes (port_config.h; without them the window stays framed)
    int (*XChangeProperty)(x_display *, x_window, x_atom, x_atom, int, int, const unsigned char *, int);
    int (*XSendEvent)(x_display *, x_window, int, long, x_event *);
    // raw mouse look (HALO3_RAW_MOUSE=1), all optional: pointer grab and blank cursor (libX11), raw motion (libXi)
    int (*XQueryExtension)(x_display *, const char *, int *, int *, int *);
    int (*XGetEventData)(x_display *, void *);
    void (*XFreeEventData)(x_display *, void *);
    int (*XGrabPointer)(x_display *, x_window, int, unsigned int, int, int, x_window, unsigned long, unsigned long);
    int (*XUngrabPointer)(x_display *, unsigned long);
    unsigned long (*XCreateBitmapFromData)(x_display *, x_window, const char *, unsigned int, unsigned int);
    unsigned long (*XCreatePixmapCursor)(x_display *, unsigned long, unsigned long, x_color *, x_color *, unsigned int,
                                         unsigned int);
    int (*XFreePixmap)(x_display *, unsigned long);
    int (*XDefineCursor)(x_display *, x_window, unsigned long);
    int (*XUndefineCursor)(x_display *, x_window);
    int (*XResetScreenSaver)(x_display *); // optional: keeps X11 blanker/DPMS awake while playing with a controller
    void *xi_library;
    int (*XIQueryVersion)(x_display *, int *, int *);
    int (*XISelectEvents)(x_display *, x_window, xi_event_mask *, int);
    int xi_opcode;              // the XInputExtension major opcode; 0: no raw motion (the pointer motion stands in)
    unsigned long blank_cursor; // 0: none
    int grabbed;
    int capture_seen; // port_input_mouse_capture as the last pump saw it (-1: not yet)
    unsigned int width, height;
    int pointer_x, pointer_y, pointer_known, focused;
    int guide; // the in-game guide shows: the keys and mouse are its (guide_ingame.h)
    // A bit per keycode: the keys down (a press of one already down is the server's auto-repeat: the window asks for
    // detectable auto-repeat, which sends no release in between), and those down when the in-game guide closed (the
    // Escape that closed it, the Enter that chose Resume Game): their repeats are not the game's, until let go.
    unsigned char keys_down[32], keys_held_over[32];
    x_display *display;
    x_window window;
    x_atom delete_atom;
    int open;
    int close_requested;
    int mode;                   // the window mode applied (port_window_mode)
    unsigned long mode_revision; // port_config_revision when it was
    long long last_ss_reset_sec; // CLOCK_MONOTONIC seconds of last XResetScreenSaver pulse
} g_x;

// Serialize event dispatch and input sampling across the game and render threads. Xlib's own locking also covers
// access to the native display by the Vulkan driver.
static pthread_mutex_t g_window_lock = PTHREAD_MUTEX_INITIALIZER;

void port_window_input_lock(void) { pthread_mutex_lock(&g_window_lock); }
void port_window_input_unlock(void) { pthread_mutex_unlock(&g_window_lock); }

static int load(void)
{
    if (g_x.library != 0)
        return 1;
    g_x.library = dlopen("libX11.so.6", RTLD_NOW | RTLD_LOCAL);
    if (g_x.library == 0) {
        port_write_error("window: libX11.so.6 is not available\n");
        return 0;
    }
    int (*init_threads)(void) = (int (*)(void))dlsym(g_x.library, "XInitThreads");
    if (init_threads == 0 || !init_threads()) {
        port_write_error("window: XInitThreads failed\n");
        dlclose(g_x.library);
        g_x.library = 0;
        return 0;
    }
#define LOAD(name)                                                                                                  \
    do {                                                                                                            \
        *(void **)&g_x.name = dlsym(g_x.library, #name);                                                            \
        if (g_x.name == 0) {                                                                                        \
            port_write_error("window: libX11.so.6 has no " #name "\n");                                             \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    LOAD(XOpenDisplay);
    LOAD(XCloseDisplay);
    LOAD(XDefaultScreen);
    LOAD(XRootWindow);
    LOAD(XBlackPixel);
    LOAD(XCreateSimpleWindow);
    LOAD(XDestroyWindow);
    LOAD(XStoreName);
    LOAD(XSelectInput);
    LOAD(XInternAtom);
    LOAD(XSetWMProtocols);
    LOAD(XMapWindow);
    LOAD(XFlush);
    LOAD(XPending);
    LOAD(XNextEvent);
    LOAD(XLookupKeysym);
    LOAD(XLookupString);
    LOAD(XWarpPointer);
#undef LOAD
    *(void **)&g_x.XkbSetDetectableAutoRepeat = dlsym(g_x.library, "XkbSetDetectableAutoRepeat");
    *(void **)&g_x.XChangeProperty = dlsym(g_x.library, "XChangeProperty");
    *(void **)&g_x.XSendEvent = dlsym(g_x.library, "XSendEvent");
    { // raw mouse look, which the Guide can turn on at any time ([input] raw_mouse)
#define LOAD_OPTIONAL(library, name) (*(void **)&g_x.name = dlsym(library, #name))
        LOAD_OPTIONAL(g_x.library, XQueryExtension);
        LOAD_OPTIONAL(g_x.library, XGetEventData);
        LOAD_OPTIONAL(g_x.library, XFreeEventData);
        LOAD_OPTIONAL(g_x.library, XGrabPointer);
        LOAD_OPTIONAL(g_x.library, XUngrabPointer);
        LOAD_OPTIONAL(g_x.library, XCreateBitmapFromData);
        LOAD_OPTIONAL(g_x.library, XCreatePixmapCursor);
        LOAD_OPTIONAL(g_x.library, XFreePixmap);
        LOAD_OPTIONAL(g_x.library, XDefineCursor);
        LOAD_OPTIONAL(g_x.library, XUndefineCursor);
        LOAD_OPTIONAL(g_x.library, XResetScreenSaver);
        g_x.xi_library = dlopen("libXi.so.6", RTLD_NOW | RTLD_LOCAL);
        if (g_x.xi_library != 0) {
            LOAD_OPTIONAL(g_x.xi_library, XIQueryVersion);
            LOAD_OPTIONAL(g_x.xi_library, XISelectEvents);
        }
#undef LOAD_OPTIONAL
    }
    return 1;
}

// Raw mouse look (HALO3_RAW_MOUSE=1): XInput2 raw motion on the root window (relative device counts before pointer
// acceleration, never stopped by a window or screen edge; counted only while the window has focus), and a blank
// cursor. Without libXi or XInput 2.0 the pointer motion stands in (re-centred: port_input_mouse_capture).
static void raw_mouse_setup(void)
{
    g_x.xi_opcode = 0;
    int opcode = 0, event = 0, error = 0, major = 2, minor = 0;
    if (g_x.XQueryExtension != 0 && g_x.XGetEventData != 0 && g_x.XFreeEventData != 0 && g_x.XIQueryVersion != 0 &&
        g_x.XISelectEvents != 0 && g_x.XQueryExtension(g_x.display, "XInputExtension", &opcode, &event, &error) &&
        g_x.XIQueryVersion(g_x.display, &major, &minor) == 0) {
        unsigned char bits[(XI_RAW_MOTION >> 3) + 1] = { 0 };
        bits[XI_RAW_MOTION >> 3] |= (unsigned char)(1u << (XI_RAW_MOTION & 7));
        xi_event_mask mask;
        mask.deviceid = XI_ALL_MASTER_DEVICES;
        mask.mask_len = (int)sizeof(bits);
        mask.mask = bits;
        const x_window root = g_x.XRootWindow(g_x.display, g_x.XDefaultScreen(g_x.display));
        if (g_x.XISelectEvents(g_x.display, root, &mask, 1) == 0)
            g_x.xi_opcode = opcode;
    }
    if (g_x.xi_opcode == 0)
        port_write_error("window: no XInput2 raw motion; raw mouse look uses the pointer motion\n");
    g_x.blank_cursor = 0;
    if (g_x.XCreateBitmapFromData != 0 && g_x.XCreatePixmapCursor != 0 && g_x.XFreePixmap != 0) {
        static const char empty[8] = { 0 };
        unsigned long bitmap = g_x.XCreateBitmapFromData(g_x.display, g_x.window, empty, 8, 8);
        if (bitmap != 0) {
            x_color black;
            memset(&black, 0, sizeof(black));
            g_x.blank_cursor = g_x.XCreatePixmapCursor(g_x.display, bitmap, bitmap, &black, &black, 0, 0);
            g_x.XFreePixmap(g_x.display, bitmap);
        }
    }
}

// With focus and gameplay capture on: the pointer hidden and confined to the window (XWayland turns a confining grab of a
// hidden pointer into a pointer lock); without either: released.
static void mouse_grab(int grab)
{
    grab = grab && port_input_mouse_capture();
    if (grab == g_x.grabbed)
        return;
    if (grab) {
        if (g_x.XGrabPointer == 0 ||
            g_x.XGrabPointer(g_x.display, g_x.window, 1,
                             (unsigned int)(X_BUTTON_PRESS_MASK | X_BUTTON_RELEASE_MASK | X_POINTER_MOTION_MASK),
                             X_GRAB_MODE_ASYNC, X_GRAB_MODE_ASYNC, g_x.window, g_x.blank_cursor, 0) != X_GRAB_SUCCESS)
            return; // not viewable yet, or another client grabs: tried again at the next click while focused
        if (g_x.blank_cursor != 0 && g_x.XDefineCursor != 0)
            g_x.XDefineCursor(g_x.display, g_x.window, g_x.blank_cursor);
    } else {
        if (g_x.XUngrabPointer != 0)
            g_x.XUngrabPointer(g_x.display, 0);
        if (g_x.XUndefineCursor != 0)
            g_x.XUndefineCursor(g_x.display, g_x.window);
    }
    g_x.grabbed = grab;
}

// An XInput2 raw motion event: valuators 0 and 1 (x, y) of the device, before acceleration.
static void raw_mouse_event(x_event *event)
{
    if (!g_x.XGetEventData(g_x.display, &event->cookie))
        return;
    if (event->cookie.evtype == XI_RAW_MOTION && g_x.focused && !g_x.guide && port_input_mouse_raw()) {
        const xi_raw_event *raw = (const xi_raw_event *)event->cookie.data;
        double motion[2] = { 0.0, 0.0 };
        int value = 0;
        for (int axis = 0; axis < raw->valuators.mask_len * 8; axis++) {
            if ((raw->valuators.mask[axis >> 3] >> (axis & 7)) & 1) {
                if (axis < 2)
                    motion[axis] = raw->raw_values[value];
                value++;
            }
        }
        if (motion[0] != 0.0 || motion[1] != 0.0)
            port_input_mouse_raw_motion(motion[0], motion[1]);
    }
    g_x.XFreeEventData(g_x.display, &event->cookie);
}

// ---- the window modes (port_config.h's port_window_mode) ----
//
// Through the window manager, as EWMH and the Motif hints have it: fullscreen is _NET_WM_STATE_FULLSCREEN (the whole
// monitor the window is on, over panels; the display mode is kept); borderless is no frame (_MOTIF_WM_HINTS'
// decorations 0) and maximized (_NET_WM_STATE_MAXIMIZED_VERT / _HORZ: the monitor's work area); windowed is a framed
// window of its own size again. Before the window is mapped the states are its _NET_WM_STATE property; once it is, a
// client message to the root window asks the window manager (EWMH: "_NET_WM_STATE ... sent to the root window").

// Asks the window manager to add (`on`) or remove the states `first` and `second` (0: none).
static void window_state(int on, x_atom first, x_atom second, x_atom state)
{
    x_event event;
    memset(&event, 0, sizeof(event));
    event.client.type = X_CLIENT_MESSAGE;
    event.client.window = g_x.window;
    event.client.message_type = state;
    event.client.format = 32;
    event.client.data[0] = on ? 1 : 0; // _NET_WM_STATE_ADD, _NET_WM_STATE_REMOVE
    event.client.data[1] = (long)first;
    event.client.data[2] = (long)second;
    event.client.data[3] = 1;          // from a normal application
    const x_window root = g_x.XRootWindow(g_x.display, g_x.XDefaultScreen(g_x.display));
    g_x.XSendEvent(g_x.display, root, 0, X_SUBSTRUCTURE_REDIRECT_MASK | X_SUBSTRUCTURE_NOTIFY_MASK, &event);
}

static void window_mode_apply(int mode, int mapped)
{
    if (g_x.XChangeProperty == 0 || g_x.XSendEvent == 0)
        return;
    const x_atom state = g_x.XInternAtom(g_x.display, "_NET_WM_STATE", 0);
    const x_atom fullscreen = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_FULLSCREEN", 0);
    const x_atom vertical = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_MAXIMIZED_VERT", 0);
    const x_atom horizontal = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_MAXIMIZED_HORZ", 0);
    const x_atom motif = g_x.XInternAtom(g_x.display, "_MOTIF_WM_HINTS", 0);
    // MotifWmHints: flags (MWM_HINTS_DECORATIONS), functions, decorations (0 none, 1 all), input mode, status
    const long hints[5] = { 2, 0, mode == PORT_WINDOW_MODE_BORDERLESS ? 0 : 1, 0, 0 };
    g_x.XChangeProperty(g_x.display, g_x.window, motif, motif, 32, X_PROP_MODE_REPLACE, (const unsigned char *)hints, 5);
    if (!mapped) {
        x_atom states[2];
        int count = 0;
        if (mode == PORT_WINDOW_MODE_FULLSCREEN)
            states[count++] = fullscreen;
        if (mode == PORT_WINDOW_MODE_BORDERLESS) {
            states[count++] = vertical;
            states[count++] = horizontal;
        }
        g_x.XChangeProperty(g_x.display, g_x.window, state, X_ATOM_ATOM, 32, X_PROP_MODE_REPLACE,
                            (const unsigned char *)states, count);
    } else {
        // the state left first, so a window going from one to the other is never both
        if (mode != PORT_WINDOW_MODE_FULLSCREEN)
            window_state(0, fullscreen, 0, state);
        if (mode != PORT_WINDOW_MODE_BORDERLESS)
            window_state(0, vertical, horizontal, state);
        if (mode == PORT_WINDOW_MODE_FULLSCREEN)
            window_state(1, fullscreen, 0, state);
        if (mode == PORT_WINDOW_MODE_BORDERLESS)
            window_state(1, vertical, horizontal, state);
    }
    g_x.XFlush(g_x.display);
    g_x.mode = mode;
}

// The mode halo3.ini (or HALO3_WINDOW_MODE) asks for now, once the settings changed (Guide > Settings).
static void window_mode_follow(void)
{
    const unsigned long revision = port_config_revision();
    if (revision == g_x.mode_revision)
        return;
    g_x.mode_revision = revision;
    const int mode = port_config_window_mode();
    if (mode != g_x.mode)
        window_mode_apply(mode, 1);
}

// ---- display sleep / screensaver inhibition (D-Bus + X11 fallback) ----
//
// Controller input alone (evdev) does not reset Linux desktop idle timers. While the game window is open, hold a
// session D-Bus inhibit (org.freedesktop.ScreenSaver, org.gnome.SessionManager, or org.freedesktop.portal.Inhibit
// for Flatpak / Wayland portals) on a private D-Bus connection (loaded at run time from libdbus-1.so.3), and also
// pulse XResetScreenSaver every 30 s for minimal X11 window managers without a desktop power daemon.

enum { k_inhibit_none, k_inhibit_screensaver, k_inhibit_gnome, k_inhibit_portal };

typedef struct { void *p[16]; } dbus_error_buf;
typedef struct { void *p[24]; } dbus_iter_buf;

static struct {
    void *library;
    int tried;
    void (*error_init)(dbus_error_buf *);
    void (*error_free)(dbus_error_buf *);
    uint32_t (*error_is_set)(const dbus_error_buf *);
    void *(*bus_get_private)(int, dbus_error_buf *);
    void (*connection_set_exit_on_disconnect)(void *, uint32_t);
    void (*connection_close)(void *);
    void (*connection_unref)(void *);
    void *(*connection_send_with_reply_and_block)(void *, void *, int, dbus_error_buf *);
    void *(*message_new_method_call)(const char *, const char *, const char *, const char *);
    void (*message_unref)(void *);
    void (*iter_init_append)(void *, dbus_iter_buf *);
    uint32_t (*iter_append_basic)(dbus_iter_buf *, int, const void *);
    uint32_t (*iter_open_container)(dbus_iter_buf *, int, const char *, dbus_iter_buf *);
    uint32_t (*iter_close_container)(dbus_iter_buf *, dbus_iter_buf *);
    uint32_t (*iter_init)(void *, dbus_iter_buf *);
    int (*iter_get_arg_type)(dbus_iter_buf *);
    void (*iter_get_basic)(dbus_iter_buf *, void *);
    void *conn;
    int kind;
    uint32_t cookie;
    uint32_t pm_cookie; // optional org.freedesktop.PowerManagement.Inhibit cookie (KDE/XFCE)
    char portal_path[256];
} g_inhibit;

static void inhibit_copy_text(char *dst, unsigned cap, const char *src)
{
    if (cap == 0)
        return;
    unsigned i = 0;
    if (src != 0) {
        for (; src[i] != 0 && i + 1 < cap; i++)
            dst[i] = src[i];
    }
    dst[i] = 0;
}

static int load_dbus(void)
{
    if (!g_inhibit.tried) {
        g_inhibit.tried = 1;
        g_inhibit.library = dlopen("libdbus-1.so.3", RTLD_NOW | RTLD_LOCAL);
        if (g_inhibit.library != 0) {
            *(void **)&g_inhibit.error_init = dlsym(g_inhibit.library, "dbus_error_init");
            *(void **)&g_inhibit.error_free = dlsym(g_inhibit.library, "dbus_error_free");
            *(void **)&g_inhibit.error_is_set = dlsym(g_inhibit.library, "dbus_error_is_set");
            *(void **)&g_inhibit.bus_get_private = dlsym(g_inhibit.library, "dbus_bus_get_private");
            *(void **)&g_inhibit.connection_set_exit_on_disconnect =
                dlsym(g_inhibit.library, "dbus_connection_set_exit_on_disconnect");
            *(void **)&g_inhibit.connection_close = dlsym(g_inhibit.library, "dbus_connection_close");
            *(void **)&g_inhibit.connection_unref = dlsym(g_inhibit.library, "dbus_connection_unref");
            *(void **)&g_inhibit.connection_send_with_reply_and_block =
                dlsym(g_inhibit.library, "dbus_connection_send_with_reply_and_block");
            *(void **)&g_inhibit.message_new_method_call = dlsym(g_inhibit.library, "dbus_message_new_method_call");
            *(void **)&g_inhibit.message_unref = dlsym(g_inhibit.library, "dbus_message_unref");
            *(void **)&g_inhibit.iter_init_append = dlsym(g_inhibit.library, "dbus_message_iter_init_append");
            *(void **)&g_inhibit.iter_append_basic = dlsym(g_inhibit.library, "dbus_message_iter_append_basic");
            *(void **)&g_inhibit.iter_open_container = dlsym(g_inhibit.library, "dbus_message_iter_open_container");
            *(void **)&g_inhibit.iter_close_container = dlsym(g_inhibit.library, "dbus_message_iter_close_container");
            *(void **)&g_inhibit.iter_init = dlsym(g_inhibit.library, "dbus_message_iter_init");
            *(void **)&g_inhibit.iter_get_arg_type = dlsym(g_inhibit.library, "dbus_message_iter_get_arg_type");
            *(void **)&g_inhibit.iter_get_basic = dlsym(g_inhibit.library, "dbus_message_iter_get_basic");
        }
    }
    return g_inhibit.error_init && g_inhibit.error_free && g_inhibit.error_is_set && g_inhibit.bus_get_private &&
           g_inhibit.connection_set_exit_on_disconnect && g_inhibit.connection_close && g_inhibit.connection_unref &&
           g_inhibit.connection_send_with_reply_and_block && g_inhibit.message_new_method_call &&
           g_inhibit.message_unref && g_inhibit.iter_init_append && g_inhibit.iter_append_basic &&
           g_inhibit.iter_open_container && g_inhibit.iter_close_container && g_inhibit.iter_init &&
           g_inhibit.iter_get_arg_type && g_inhibit.iter_get_basic;
}

static int try_inhibit_screensaver(void)
{
    void *msg = g_inhibit.message_new_method_call("org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver",
                                                  "org.freedesktop.ScreenSaver", "Inhibit");
    if (msg == 0)
        return 0;
    dbus_iter_buf iter;
    const char *app = "Halo 3", *reason = "Playing Halo 3";
    g_inhibit.iter_init_append(msg, &iter);
    g_inhibit.iter_append_basic(&iter, 's', &app);
    g_inhibit.iter_append_basic(&iter, 's', &reason);
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    void *reply = g_inhibit.connection_send_with_reply_and_block(g_inhibit.conn, msg, 500, &err);
    g_inhibit.message_unref(msg);
    if (g_inhibit.error_is_set(&err) || reply == 0) {
        g_inhibit.error_free(&err);
        if (reply != 0)
            g_inhibit.message_unref(reply);
        return 0;
    }
    g_inhibit.error_free(&err);
    int ok = 0;
    if (g_inhibit.iter_init(reply, &iter) && g_inhibit.iter_get_arg_type(&iter) == 'u') {
        uint32_t cookie = 0;
        g_inhibit.iter_get_basic(&iter, &cookie);
        g_inhibit.cookie = cookie;
        g_inhibit.kind = k_inhibit_screensaver;
        ok = 1;
    }
    g_inhibit.message_unref(reply);
    return ok;
}

// KDE Plasma / XFCE separate display blanking (org.freedesktop.ScreenSaver) from system sleep
// (org.freedesktop.PowerManagement.Inhibit); hold both when available, matching kde-inhibit --power --screenSaver.
static int try_inhibit_power_management(void)
{
    void *msg = g_inhibit.message_new_method_call("org.freedesktop.PowerManagement.Inhibit",
                                                  "/org/freedesktop/PowerManagement/Inhibit",
                                                  "org.freedesktop.PowerManagement.Inhibit", "Inhibit");
    if (msg == 0)
        return 0;
    dbus_iter_buf iter;
    const char *app = "Halo 3", *reason = "Playing Halo 3";
    g_inhibit.iter_init_append(msg, &iter);
    g_inhibit.iter_append_basic(&iter, 's', &app);
    g_inhibit.iter_append_basic(&iter, 's', &reason);
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    void *reply = g_inhibit.connection_send_with_reply_and_block(g_inhibit.conn, msg, 500, &err);
    g_inhibit.message_unref(msg);
    if (g_inhibit.error_is_set(&err) || reply == 0) {
        g_inhibit.error_free(&err);
        if (reply != 0)
            g_inhibit.message_unref(reply);
        return 0;
    }
    g_inhibit.error_free(&err);
    int ok = 0;
    if (g_inhibit.iter_init(reply, &iter) && g_inhibit.iter_get_arg_type(&iter) == 'u') {
        uint32_t cookie = 0;
        g_inhibit.iter_get_basic(&iter, &cookie);
        g_inhibit.pm_cookie = cookie;
        ok = 1;
    }
    g_inhibit.message_unref(reply);
    return ok;
}

static int try_inhibit_gnome(void)
{
    void *msg = g_inhibit.message_new_method_call("org.gnome.SessionManager", "/org/gnome/SessionManager",
                                                  "org.gnome.SessionManager", "Inhibit");
    if (msg == 0)
        return 0;
    dbus_iter_buf iter;
    const char *app = "Halo 3", *reason = "Playing Halo 3";
    uint32_t xid = (uint32_t)g_x.window, flags = 12u; // 4 = suspend | 8 = idle
    g_inhibit.iter_init_append(msg, &iter);
    g_inhibit.iter_append_basic(&iter, 's', &app);
    g_inhibit.iter_append_basic(&iter, 'u', &xid);
    g_inhibit.iter_append_basic(&iter, 's', &reason);
    g_inhibit.iter_append_basic(&iter, 'u', &flags);
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    void *reply = g_inhibit.connection_send_with_reply_and_block(g_inhibit.conn, msg, 500, &err);
    g_inhibit.message_unref(msg);
    if (g_inhibit.error_is_set(&err) || reply == 0) {
        g_inhibit.error_free(&err);
        if (reply != 0)
            g_inhibit.message_unref(reply);
        return 0;
    }
    g_inhibit.error_free(&err);
    int ok = 0;
    if (g_inhibit.iter_init(reply, &iter) && g_inhibit.iter_get_arg_type(&iter) == 'u') {
        uint32_t cookie = 0;
        g_inhibit.iter_get_basic(&iter, &cookie);
        g_inhibit.cookie = cookie;
        g_inhibit.kind = k_inhibit_gnome;
        ok = 1;
    }
    g_inhibit.message_unref(reply);
    return ok;
}

static int try_inhibit_portal(void)
{
    void *msg = g_inhibit.message_new_method_call("org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
                                                  "org.freedesktop.portal.Inhibit", "Inhibit");
    if (msg == 0)
        return 0;
    dbus_iter_buf iter, arr, dict, var;
    const char *win = "", *key = "reason", *reason = "Playing Halo 3";
    uint32_t flags = 12u; // 4 = suspend | 8 = idle
    g_inhibit.iter_init_append(msg, &iter);
    g_inhibit.iter_append_basic(&iter, 's', &win);
    g_inhibit.iter_append_basic(&iter, 'u', &flags);
    if (g_inhibit.iter_open_container(&iter, 'a', "{sv}", &arr)) {
        if (g_inhibit.iter_open_container(&arr, 'e', 0, &dict)) {
            g_inhibit.iter_append_basic(&dict, 's', &key);
            if (g_inhibit.iter_open_container(&dict, 'v', "s", &var)) {
                g_inhibit.iter_append_basic(&var, 's', &reason);
                g_inhibit.iter_close_container(&dict, &var);
            }
            g_inhibit.iter_close_container(&arr, &dict);
        }
        g_inhibit.iter_close_container(&iter, &arr);
    }
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    void *reply = g_inhibit.connection_send_with_reply_and_block(g_inhibit.conn, msg, 500, &err);
    g_inhibit.message_unref(msg);
    if (g_inhibit.error_is_set(&err) || reply == 0) {
        g_inhibit.error_free(&err);
        if (reply != 0)
            g_inhibit.message_unref(reply);
        return 0;
    }
    g_inhibit.error_free(&err);
    int ok = 0;
    if (g_inhibit.iter_init(reply, &iter) && g_inhibit.iter_get_arg_type(&iter) == 'o') {
        const char *path = 0;
        g_inhibit.iter_get_basic(&iter, &path);
        if (path != 0 && path[0] != 0) {
            inhibit_copy_text(g_inhibit.portal_path, sizeof(g_inhibit.portal_path), path);
            g_inhibit.kind = k_inhibit_portal;
            ok = 1;
        }
    }
    g_inhibit.message_unref(reply);
    return ok;
}

static void sleep_inhibit_begin(void)
{
    g_inhibit.kind = k_inhibit_none;
    g_inhibit.cookie = 0;
    g_inhibit.pm_cookie = 0;
    g_inhibit.portal_path[0] = 0;
    g_x.last_ss_reset_sec = 0;
    if (g_x.XResetScreenSaver != 0 && g_x.display != 0)
        g_x.XResetScreenSaver(g_x.display);
    if (!load_dbus())
        return;
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    g_inhibit.conn = g_inhibit.bus_get_private(0, &err); // 0 = DBUS_BUS_SESSION
    if (g_inhibit.error_is_set(&err) || g_inhibit.conn == 0) {
        g_inhibit.error_free(&err);
        if (g_inhibit.conn != 0) {
            g_inhibit.connection_close(g_inhibit.conn);
            g_inhibit.connection_unref(g_inhibit.conn);
            g_inhibit.conn = 0;
        }
        return;
    }
    g_inhibit.error_free(&err);
    g_inhibit.connection_set_exit_on_disconnect(g_inhibit.conn, 0);

    const int flatpak = access("/.flatpak-info", F_OK) == 0;
    int ok = 0;
    if (flatpak)
        ok = try_inhibit_portal() || try_inhibit_screensaver() || try_inhibit_gnome();
    else
        ok = try_inhibit_screensaver() || try_inhibit_gnome() || try_inhibit_portal();
    if (g_inhibit.kind == k_inhibit_screensaver || !ok) {
        if (try_inhibit_power_management())
            ok = 1;
    }

    if (!ok) {
        g_inhibit.connection_close(g_inhibit.conn);
        g_inhibit.connection_unref(g_inhibit.conn);
        g_inhibit.conn = 0;
    }
}

static void sleep_inhibit_pump(void)
{
    if (g_x.XResetScreenSaver == 0 || g_x.display == 0)
        return;
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return;
    if (ts.tv_sec - g_x.last_ss_reset_sec >= 30) {
        g_x.last_ss_reset_sec = ts.tv_sec;
        g_x.XResetScreenSaver(g_x.display);
    }
}

static void sleep_inhibit_send_and_unref(void *msg)
{
    if (msg == 0)
        return;
    dbus_error_buf err;
    g_inhibit.error_init(&err);
    void *reply = g_inhibit.connection_send_with_reply_and_block(g_inhibit.conn, msg, 300, &err);
    g_inhibit.error_free(&err);
    if (reply != 0)
        g_inhibit.message_unref(reply);
    g_inhibit.message_unref(msg);
}

static void sleep_inhibit_end(void)
{
    if (g_inhibit.conn == 0)
        return;
    if (g_inhibit.pm_cookie != 0) {
        void *pm = g_inhibit.message_new_method_call("org.freedesktop.PowerManagement.Inhibit",
                                                     "/org/freedesktop/PowerManagement/Inhibit",
                                                     "org.freedesktop.PowerManagement.Inhibit", "UnInhibit");
        if (pm != 0) {
            dbus_iter_buf iter;
            g_inhibit.iter_init_append(pm, &iter);
            g_inhibit.iter_append_basic(&iter, 'u', &g_inhibit.pm_cookie);
            sleep_inhibit_send_and_unref(pm);
        }
    }
    void *msg = 0;
    if (g_inhibit.kind == k_inhibit_screensaver) {
        msg = g_inhibit.message_new_method_call("org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver",
                                                "org.freedesktop.ScreenSaver", "UnInhibit");
        if (msg != 0) {
            dbus_iter_buf iter;
            g_inhibit.iter_init_append(msg, &iter);
            g_inhibit.iter_append_basic(&iter, 'u', &g_inhibit.cookie);
        }
    } else if (g_inhibit.kind == k_inhibit_gnome) {
        msg = g_inhibit.message_new_method_call("org.gnome.SessionManager", "/org/gnome/SessionManager",
                                                "org.gnome.SessionManager", "Uninhibit");
        if (msg != 0) {
            dbus_iter_buf iter;
            g_inhibit.iter_init_append(msg, &iter);
            g_inhibit.iter_append_basic(&iter, 'u', &g_inhibit.cookie);
        }
    } else if (g_inhibit.kind == k_inhibit_portal && g_inhibit.portal_path[0] != 0) {
        msg = g_inhibit.message_new_method_call("org.freedesktop.portal.Desktop", g_inhibit.portal_path,
                                                "org.freedesktop.portal.Request", "Close");
    }
    sleep_inhibit_send_and_unref(msg);
    g_inhibit.connection_close(g_inhibit.conn);
    g_inhibit.connection_unref(g_inhibit.conn);
    g_inhibit.conn = 0;
    g_inhibit.kind = k_inhibit_none;
    g_inhibit.cookie = 0;
    g_inhibit.pm_cookie = 0;
    g_inhibit.portal_path[0] = 0;
}

int port_window_system(void)
{
    return PORT_WINDOW_SYSTEM_XLIB;
}

static int window_open(const char *title, unsigned int width, unsigned int height)
{
    if (g_x.open)
        return 1;
    if (!load())
        return 0;
    x_display *display = g_x.XOpenDisplay(0);
    if (display == 0) {
        port_write_error("window: cannot open the X display (DISPLAY unset?)\n");
        return 0;
    }
    int screen = g_x.XDefaultScreen(display);
    unsigned long black = g_x.XBlackPixel(display, screen);
    x_window window = g_x.XCreateSimpleWindow(display, g_x.XRootWindow(display, screen), 0, 0, width, height, 0,
                                               black, black);
    if (window == 0) {
        port_write_error("window: XCreateSimpleWindow failed\n");
        g_x.XCloseDisplay(display);
        return 0;
    }
    __atomic_store_n(&g_x.display, display, __ATOMIC_RELEASE);
    __atomic_store_n(&g_x.window, window, __ATOMIC_RELEASE);
    g_x.XStoreName(g_x.display, g_x.window, title != 0 ? title : "halo3");
    g_x.XSelectInput(g_x.display, g_x.window, X_STRUCTURE_NOTIFY_MASK | X_EXPOSURE_MASK | X_KEY_PRESS_MASK |
                     X_KEY_RELEASE_MASK | X_BUTTON_PRESS_MASK | X_BUTTON_RELEASE_MASK | X_POINTER_MOTION_MASK |
                     X_FOCUS_CHANGE_MASK);
    if (g_x.XkbSetDetectableAutoRepeat != 0)
        g_x.XkbSetDetectableAutoRepeat(g_x.display, 1, 0);
    g_x.width = width;
    g_x.height = height;
    g_x.pointer_known = 0;
    g_x.delete_atom = g_x.XInternAtom(g_x.display, "WM_DELETE_WINDOW", 0);
    g_x.XSetWMProtocols(g_x.display, g_x.window, &g_x.delete_atom, 1);
    // the window mode it opens in (halo3.ini's window_mode, HALO3_WINDOW_MODE), then whatever the Guide makes it
    g_x.mode = PORT_WINDOW_MODE_WINDOWED;
    g_x.mode_revision = port_config_revision();
    const int mode = port_config_window_mode();
    if (mode != PORT_WINDOW_MODE_WINDOWED)
        window_mode_apply(mode, 0);
    g_x.XMapWindow(g_x.display, g_x.window);
    g_x.XFlush(g_x.display);
    g_x.grabbed = 0;
    g_x.capture_seen = -1;
    g_x.close_requested = 0;
    raw_mouse_setup(); // (raw motion is read only while raw mouse look is on)
    sleep_inhibit_begin();
    port_input_keyboard_attach(1);
    __atomic_store_n(&g_x.open, 1, __ATOMIC_RELEASE);
    return 1;
}

int port_window_open(const char *title, unsigned int width, unsigned int height)
{
    port_window_input_lock();
    const int opened = window_open(title, width, height);
    port_window_input_unlock();
    return opened;
}

void *port_window_native_display(void)
{
    return __atomic_load_n(&g_x.display, __ATOMIC_ACQUIRE);
}

__UINTPTR_TYPE__ port_window_native_window(void)
{
    return (__UINTPTR_TYPE__)__atomic_load_n(&g_x.window, __ATOMIC_ACQUIRE);
}

// Keysyms (X11/keysymdef.h) of the keys port_input.h knows; XLookupKeysym(event, 0) gives letters in lower case (and
// the unshifted symbol of a key: Tab for Shift+Tab, 1 for !).
static enum e_port_key key_of(unsigned long keysym)
{
    if (keysym >= 0x61 && keysym <= 0x7a) // a .. z
        return (enum e_port_key)(k_port_key_a + (int)(keysym - 0x61));
    if (keysym >= 0x30 && keysym <= 0x39) // 0 .. 9
        return (enum e_port_key)(k_port_key_0 + (int)(keysym - 0x30));
    if (keysym >= 0xffbe && keysym <= 0xffc9) // F1 .. F12
        return (enum e_port_key)(k_port_key_f1 + (int)(keysym - 0xffbe));
    switch (keysym) {
    case 0xff1b: return k_port_key_escape;
    case 0xff09: case 0xfe20: return k_port_key_tab; // Tab, ISO_Left_Tab
    case 0x20: return k_port_key_space;
    case 0xff0d: case 0xff8d: return k_port_key_return; // Return, KP_Enter
    case 0xff08: return k_port_key_backspace;
    case 0xff52: return k_port_key_up;
    case 0xff54: return k_port_key_down;
    case 0xff51: return k_port_key_left;
    case 0xff53: return k_port_key_right;
    case 0xffe1: return k_port_key_lshift;
    case 0xffe2: return k_port_key_rshift;
    case 0xffe3: return k_port_key_lctrl;
    case 0xffe4: return k_port_key_rctrl;
    case 0xffe9: case 0xffe7: return k_port_key_lalt; // Alt_L, Meta_L
    case 0xffea: case 0xffe8: case 0xfe03: return k_port_key_ralt; // Alt_R, Meta_R, ISO_Level3_Shift (AltGr)
    case 0xffe5: return k_port_key_capslock;
    case 0xff50: return k_port_key_home; // Home: the Guide button
    case 0xff57: return k_port_key_end;
    case 0xff63: return k_port_key_insert;
    case 0xffff: return k_port_key_delete;
    case 0xff55: return k_port_key_pageup;   // Prior
    case 0xff56: return k_port_key_pagedown; // Next
    case 0x60: return k_port_key_grave;
    case 0x2d: return k_port_key_minus;
    case 0x3d: return k_port_key_equals;
    case 0x5b: return k_port_key_lbracket;
    case 0x5d: return k_port_key_rbracket;
    case 0x5c: return k_port_key_backslash;
    case 0x3b: return k_port_key_semicolon;
    case 0x27: return k_port_key_apostrophe;
    case 0x2c: return k_port_key_comma;
    case 0x2e: return k_port_key_period;
    case 0x2f: return k_port_key_slash;
    default: return k_port_key_none;
    }
}

int port_window_key_code(__UINTPTR_TYPE__ host_key, __INTPTR_TYPE__ host_flags)
{
    (void)host_flags;
    return key_of((unsigned long)host_key);
}

// X buttons as port_kbm_code: 1 left, 2 middle, 3 right, 4 / 5 the wheel up / down, 8 / 9 the side buttons (back,
// forward); 0 for the others (6 / 7: the wheel tilted).
static int button_code(unsigned int button)
{
    switch (button) {
    case 1: return PORT_KBM_MOUSE(k_port_mouse_left);
    case 2: return PORT_KBM_MOUSE(k_port_mouse_middle);
    case 3: return PORT_KBM_MOUSE(k_port_mouse_right);
    case 4: return PORT_KBM_WHEEL_UP;
    case 5: return PORT_KBM_WHEEL_DOWN;
    case 8: return PORT_KBM_MOUSE(k_port_mouse_4);
    case 9: return PORT_KBM_MOUSE(k_port_mouse_5);
    default: return PORT_KBM_NONE;
    }
}

// The in-game guide (guide_ingame.h) opened or closed: while it shows, the pointer is free and shown and the game's
// keys and buttons are all up (the guide has the keyboard and mouse); once it closes, raw mouse look grabs it again.
static void guide_follow(void)
{
    const int guide = guide_ingame_input();
    if (guide == g_x.guide)
        return;
    g_x.guide = guide;
    if (!guide)
        memcpy(g_x.keys_held_over, g_x.keys_down, sizeof(g_x.keys_held_over));
    port_input_release_all();
    g_x.pointer_known = 0; // no look motion across the change
    mouse_grab(!guide && g_x.focused);
}

// A key pressed, repeated or released while the in-game guide shows: the guide's key (its own window's keys:
// guide_host_x11_key) and what happened to it (guide_key_action).
static void guide_key_event(x_event *event, int action)
{
    char typed[8];
    unsigned long keysym = 0, character = 0;
    const int count = g_x.XLookupString(event, typed, (int)sizeof(typed) - 1, &keysym, 0);
    const int key = guide_host_x11_key(keysym, event->input.state, typed, count, &character);
    if (key >= 0)
        guide_ingame_key_action(key, character, action);
}

static int key_bit(const unsigned char *bits, unsigned int keycode) { return (bits[(keycode & 0xff) >> 3] >> (keycode & 7)) & 1; }
static void key_bit_set(unsigned char *bits, unsigned int keycode, int on)
{
    const unsigned char mask = (unsigned char)(1u << (keycode & 7));
    bits[(keycode & 0xff) >> 3] = (unsigned char)(on ? bits[(keycode & 0xff) >> 3] | mask : bits[(keycode & 0xff) >> 3] & ~mask);
}

// Keyboard, mouse buttons, pointer motion and focus into port_input.h, or into the in-game guide while it shows (its
// presses and the pointer; releases still reach port_input.h, so nothing held before it opened stays down).
static void handle_input(x_event *event)
{
    guide_follow(); // per event: a key that closed the guide hands the next ones back to the game
    switch (event->type) {
    case X_KEY_PRESS:
    case X_KEY_RELEASE: {
        const unsigned int keycode = event->input.detail;
        const int press = event->type == X_KEY_PRESS, repeat = press && key_bit(g_x.keys_down, keycode);
        key_bit_set(g_x.keys_down, keycode, press);
        if (!press)
            key_bit_set(g_x.keys_held_over, keycode, 0);
        // a press offered first to the guide's binding capture (guide_capture), which then has it all
        if (g_x.guide && press && !repeat && guide_capture(key_of(g_x.XLookupKeysym(event, 0))))
            break;
        if (g_x.guide) // presses and their repeats the guide's; a release both's (nothing held before it opened stays down)
            guide_key_event(event, !press ? GUIDE_KEY_RELEASED : repeat ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
        if (!g_x.guide || !press) {
            if (press && key_bit(g_x.keys_held_over, keycode))
                break; // held since the guide closed: not the game's until let go
            port_input_key(key_of(g_x.XLookupKeysym(event, 0)), press);
        }
        break;
    }
    case X_BUTTON_PRESS:
    case X_BUTTON_RELEASE: {
        // X buttons: 1 left, 2 middle, 3 right, 4..7 the wheel, 8 / 9 the side buttons (button_code)
        const unsigned int button = event->input.detail;
        port_input_mouse_position(event->input.x, event->input.y, (int)g_x.width, (int)g_x.height);
        const int code = button_code(button);
        if (event->type == X_BUTTON_PRESS && g_x.guide) {
            if (code != PORT_KBM_NONE && guide_capture(code)) // the guide's binding capture first
                break;
            if (button >= 1 && button <= 3)
                guide_ingame_pointer(GUIDE_POINTER_PRESS, (int)button, event->input.x, event->input.y, (int)g_x.width,
                                     (int)g_x.height);
            else if (button == 4 || button == 5)
                guide_ingame_pointer(GUIDE_POINTER_WHEEL, button == 4 ? -1 : 1, event->input.x, event->input.y,
                                     (int)g_x.width, (int)g_x.height);
            break;
        }
        if (event->type == X_BUTTON_PRESS && g_x.focused)
            mouse_grab(1); // a grab that failed at focus (window not viewable yet, another grab): a click retries
        if (code == PORT_KBM_WHEEL_UP || code == PORT_KBM_WHEEL_DOWN) {
            if (event->type == X_BUTTON_PRESS) // (a notch: a press and a release at once)
                port_input_mouse_wheel(code == PORT_KBM_WHEEL_UP ? 1 : -1);
        } else if (code != PORT_KBM_NONE) {
            port_input_mouse_button((enum e_port_mouse_button)(code - PORT_KBM_MOUSE_FIRST), event->type == X_BUTTON_PRESS);
        }
        break;
    }
    case X_MOTION_NOTIFY: {
        const int cx = (int)(g_x.width / 2), cy = (int)(g_x.height / 2);
        const int x = event->input.x, y = event->input.y;
        port_input_mouse_position(x, y, (int)g_x.width, (int)g_x.height);
        if (g_x.guide) { // the pointer is the guide's: no look, no re-centring
            guide_ingame_pointer(GUIDE_POINTER_MOVE, 0, x, y, (int)g_x.width, (int)g_x.height);
            break;
        }
        if (g_x.pointer_known && !(port_input_mouse_capture() && x == cx && y == cy)) // not our own re-centring
            port_input_mouse_motion(x - g_x.pointer_x, y - g_x.pointer_y);
        g_x.pointer_x = x;
        g_x.pointer_y = y;
        g_x.pointer_known = 1;
        if (port_input_mouse_capture() && g_x.focused && (x != cx || y != cy)) {
            g_x.XWarpPointer(g_x.display, 0, g_x.window, 0, 0, 0, 0, cx, cy);
            g_x.pointer_x = cx;
            g_x.pointer_y = cy;
        }
        break;
    }
    case X_FOCUS_IN:
        __atomic_store_n(&g_x.focused, 1, __ATOMIC_RELEASE);
        mouse_grab(!g_x.guide);
        break;
    case X_EXPOSE: // mapped (focus can come first, when the window is not viewable and the grab fails)
        if (g_x.focused && !g_x.guide)
            mouse_grab(1);
        break;
    case X_CONFIGURE_NOTIFY: // resized: the frame is stretched over the new size (the guide's pointer)
        if (event->configure.width > 0 && event->configure.height > 0) {
            g_x.width = (unsigned int)event->configure.width;
            g_x.height = (unsigned int)event->configure.height;
        }
        break;
    case X_FOCUS_OUT: // what is let go while another window has the keys never reaches this one: nothing stays down
        __atomic_store_n(&g_x.focused, 0, __ATOMIC_RELEASE);
        mouse_grab(0);
        g_x.pointer_known = 0;
        port_input_release_all();
        memset(g_x.keys_down, 0, sizeof(g_x.keys_down));
        memset(g_x.keys_held_over, 0, sizeof(g_x.keys_held_over));
        guide_keys_released();
        break;
    default:
        break;
    }
}

// Called with the input lock held, including when the cursor is stationary.
void port_window_cursor_mode_changed(void)
{
    if (!g_x.open)
        return;
    g_x.pointer_known = 0;
    g_x.capture_seen = port_input_mouse_capture();
    mouse_grab(g_x.focused && !g_x.guide);
    g_x.XFlush(g_x.display);
}

// The caller holds the window lock through dispatch and any resulting input sample.
void port_window_poll_input(void)
{
    if (!g_x.open)
        return;
    guide_follow();
    // raw mouse look turned on or off (the Guide): the pointer grabbed or let go at once (a grab that fails is tried
    // again at the next focus or click, not every frame: XGrabPointer is a round trip to the server)
    const int capture = port_input_mouse_capture();
    if (capture != g_x.capture_seen) {
        g_x.capture_seen = capture;
        mouse_grab(g_x.focused && !g_x.guide);
    }
    while (g_x.XPending(g_x.display) > 0) {
        x_event event;
        memset(&event, 0, sizeof(event));
        g_x.XNextEvent(g_x.display, &event);
        if (event.type == X_CLIENT_MESSAGE && (x_atom)event.client.data[0] == g_x.delete_atom)
            g_x.close_requested = 1;
        else if (event.type == X_DESTROY_NOTIFY)
            g_x.close_requested = 1;
        else if (event.type == X_GENERIC_EVENT && g_x.xi_opcode != 0 && event.cookie.extension == g_x.xi_opcode)
            raw_mouse_event(&event);
        else
            handle_input(&event);
    }
}

int port_window_pump(void)
{
    port_window_input_lock();
    if (g_x.open) {
        window_mode_follow();
        sleep_inhibit_pump();
        port_window_poll_input();
    }
    const int keep_open = g_x.open && !g_x.close_requested;
    port_window_input_unlock();
    return keep_open;
}

int port_window_is_focused(void)
{
    return !__atomic_load_n(&g_x.open, __ATOMIC_ACQUIRE) || __atomic_load_n(&g_x.focused, __ATOMIC_ACQUIRE);
}

void port_window_close(void)
{
    port_window_input_lock();
    if (g_x.open) {
        port_input_keyboard_attach(0);
        sleep_inhibit_end();
        mouse_grab(0);
        g_x.XDestroyWindow(g_x.display, g_x.window);
        g_x.XCloseDisplay(g_x.display);
        __atomic_store_n(&g_x.display, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.window, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.focused, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.open, 0, __ATOMIC_RELEASE);
    }
    port_window_input_unlock();
}
