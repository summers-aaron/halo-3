// port/sdk/runtime (Linux host): the game window through native Wayland (libwayland-client.so.0, preferred whenever
// WAYLAND_DISPLAY is active) with Xlib fallback (libX11.so.6), both opened at run time with RTLD_LOCAL so library
// symbols stay out of the global namespace where the game's names live and the binary keeps zero non-glibc DT_NEEDED
// entries. Interface: port_window.h.
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
#include <errno.h>
#include <linux/input-event-codes.h>
#include <poll.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Serialize event dispatch and input sampling across the game and render threads.
static pthread_mutex_t g_window_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_active_system; // PORT_WINDOW_SYSTEM_NONE, PORT_WINDOW_SYSTEM_WAYLAND, or PORT_WINDOW_SYSTEM_XLIB

void port_window_input_lock(void) { pthread_mutex_lock(&g_window_lock); }
void port_window_input_unlock(void) { pthread_mutex_unlock(&g_window_lock); }

static int text_eq_ci(const char *a, const char *b)
{
    if (a == 0 || b == 0)
        return 0;
    for (; *a != 0 && *b != 0; a++, b++) {
        char ca = *a >= 'A' && *a <= 'Z' ? (char)(*a + ('a' - 'A')) : *a;
        char cb = *b >= 'A' && *b <= 'Z' ? (char)(*b + ('a' - 'A')) : *b;
        if (ca != cb)
            return 0;
    }
    return *a == 0 && *b == 0;
}

static int key_bit(const unsigned char *bits, unsigned int keycode)
{
    return (bits[(keycode & 0xffu) >> 3] >> (keycode & 7u)) & 1;
}

static void key_bit_set(unsigned char *bits, unsigned int keycode, int on)
{
    const unsigned char mask = (unsigned char)(1u << (keycode & 7u));
    bits[(keycode & 0xffu) >> 3] =
        (unsigned char)(on ? (bits[(keycode & 0xffu) >> 3] | mask) : (bits[(keycode & 0xffu) >> 3] & ~mask));
}

// ---- display sleep / screensaver inhibition (D-Bus + Wayland idle-inhibit + X11 fallback) ----
//
// Controller input alone (evdev) does not reset Linux desktop idle timers. On Wayland, zwp_idle_inhibit_manager_v1
// attaches an idle inhibitor directly to the game wl_surface; in addition, hold a session D-Bus inhibit
// (org.freedesktop.ScreenSaver, org.gnome.SessionManager, or org.freedesktop.portal.Inhibit) on a private D-Bus
// connection (loaded at run time from libdbus-1.so.3), and pulse XResetScreenSaver every 30 s when using X11.

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

static int try_inhibit_gnome(uint32_t xid)
{
    void *msg = g_inhibit.message_new_method_call("org.gnome.SessionManager", "/org/gnome/SessionManager",
                                                  "org.gnome.SessionManager", "Inhibit");
    if (msg == 0)
        return 0;
    dbus_iter_buf iter;
    const char *app = "Halo 3", *reason = "Playing Halo 3";
    uint32_t flags = 12u; // 4 = suspend | 8 = idle
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

static void sleep_inhibit_begin(uint32_t xid)
{
    g_inhibit.kind = k_inhibit_none;
    g_inhibit.cookie = 0;
    g_inhibit.pm_cookie = 0;
    g_inhibit.portal_path[0] = 0;
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
        ok = try_inhibit_portal() || try_inhibit_screensaver() || try_inhibit_gnome(xid);
    else
        ok = try_inhibit_screensaver() || try_inhibit_gnome(xid) || try_inhibit_portal();
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

// =====================================================================================================================
// Native Wayland Backend (libwayland-client.so.0, runtime dlopen, zero link-time dependencies)
//
// Protocols bound when advertised by the compositor:
//   - wl_compositor, wl_seat (wl_pointer + wl_keyboard), wl_shm
//   - xdg_wm_base / xdg_surface / xdg_toplevel (windowed, borderless, fullscreen, focus, close)
//   - wp_viewporter / wp_viewport (maps exact render swapchain resolution onto logical scaled output without Xwayland)
//   - zwp_relative_pointer_manager_v1 + zwp_pointer_constraints_v1 (raw unaccelerated mouse look + pointer lock)
//   - zwp_idle_inhibit_manager_v1 (surface-bound display sleep inhibition)
//   - zxdg_decoration_manager_v1 (server-side decorations in windowed mode, client-side/none in borderless)
// =====================================================================================================================

struct wl_proxy;
struct wl_display;
struct wl_interface;

typedef int32_t wl_fixed_t;

struct wl_array {
    size_t size;
    size_t alloc;
    void *data;
};

struct wl_message {
    const char *name;
    const char *signature;
    const struct wl_interface **types;
};

struct wl_interface {
    const char *name;
    int version;
    int method_count;
    const struct wl_message *methods;
    int event_count;
    const struct wl_message *events;
};

struct wl_cursor_image {
    uint32_t width;
    uint32_t height;
    uint32_t hotspot_x;
    uint32_t hotspot_y;
    uint32_t delay;
};

struct wl_cursor {
    unsigned int image_count;
    struct wl_cursor_image **images;
    char *name;
};

struct wl_cursor_theme;

#define WL_MARSHAL_FLAG_DESTROY (1u << 0)
#define WL_SEAT_CAPABILITY_POINTER 1u
#define WL_SEAT_CAPABILITY_KEYBOARD 2u
#define XDG_TOPLEVEL_STATE_ACTIVATED 4u
#define ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT 2u
#define ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE 1u
#define ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE 2u

// Wayland protocol interface descriptors for extensions outside core libwayland-client.so.0.
// Core interfaces (wl_surface_interface, wl_seat_interface, wl_pointer_interface, wl_output_interface,
// wl_region_interface) are patched into the type arrays at load_wayland() time before any proxy is created.
static const struct wl_interface *g_wl_types_null[8] = { 0 };

static struct wl_interface g_xdg_positioner_iface;
static struct wl_interface g_xdg_surface_iface;
static struct wl_interface g_xdg_toplevel_iface;
static struct wl_interface g_xdg_popup_iface;
static struct wl_interface g_xdg_wm_base_iface;

static const struct wl_interface *g_xdg_create_pos_types[1] = { &g_xdg_positioner_iface };
static const struct wl_interface *g_xdg_get_surface_types[2] = { &g_xdg_surface_iface, 0 /* wl_surface */ };
static const struct wl_interface *g_xdg_get_toplevel_types[1] = { &g_xdg_toplevel_iface };
static const struct wl_interface *g_xdg_get_popup_types[3] = {
    &g_xdg_popup_iface, &g_xdg_surface_iface, &g_xdg_positioner_iface
};
static const struct wl_interface *g_xdg_set_parent_types[1] = { &g_xdg_toplevel_iface };
static const struct wl_interface *g_xdg_window_menu_types[4] = { 0 /* wl_seat */, 0, 0, 0 };
static const struct wl_interface *g_xdg_move_types[2] = { 0 /* wl_seat */, 0 };
static const struct wl_interface *g_xdg_resize_types[3] = { 0 /* wl_seat */, 0, 0 };
static const struct wl_interface *g_xdg_fullscreen_types[1] = { 0 /* wl_output */ };

static const struct wl_message g_xdg_wm_base_requests[4] = {
    { "destroy", "", g_wl_types_null },
    { "create_positioner", "n", g_xdg_create_pos_types },
    { "get_xdg_surface", "no", g_xdg_get_surface_types },
    { "pong", "u", g_wl_types_null },
};
static const struct wl_message g_xdg_wm_base_events[1] = {
    { "ping", "u", g_wl_types_null },
};
static struct wl_interface g_xdg_wm_base_iface = {
    "xdg_wm_base", 4, 4, g_xdg_wm_base_requests, 1, g_xdg_wm_base_events
};

static const struct wl_message g_xdg_positioner_requests[1] = {
    { "destroy", "", g_wl_types_null },
};
static struct wl_interface g_xdg_positioner_iface = {
    "xdg_positioner", 4, 1, g_xdg_positioner_requests, 0, 0
};

static const struct wl_message g_xdg_surface_requests[5] = {
    { "destroy", "", g_wl_types_null },
    { "get_toplevel", "n", g_xdg_get_toplevel_types },
    { "get_popup", "n?oo", g_xdg_get_popup_types },
    { "set_window_geometry", "iiii", g_wl_types_null },
    { "ack_configure", "u", g_wl_types_null },
};
static const struct wl_message g_xdg_surface_events[1] = {
    { "configure", "u", g_wl_types_null },
};
static struct wl_interface g_xdg_surface_iface = {
    "xdg_surface", 4, 5, g_xdg_surface_requests, 1, g_xdg_surface_events
};

static const struct wl_message g_xdg_toplevel_requests[14] = {
    { "destroy", "", g_wl_types_null },
    { "set_parent", "?o", g_xdg_set_parent_types },
    { "set_title", "s", g_wl_types_null },
    { "set_app_id", "s", g_wl_types_null },
    { "show_window_menu", "ouii", g_xdg_window_menu_types },
    { "move", "ou", g_xdg_move_types },
    { "resize", "ouu", g_xdg_resize_types },
    { "set_max_size", "ii", g_wl_types_null },
    { "set_min_size", "ii", g_wl_types_null },
    { "set_maximized", "", g_wl_types_null },
    { "unset_maximized", "", g_wl_types_null },
    { "set_fullscreen", "?o", g_xdg_fullscreen_types },
    { "unset_fullscreen", "", g_wl_types_null },
    { "set_minimized", "", g_wl_types_null },
};
static const struct wl_message g_xdg_toplevel_events[3] = {
    { "configure", "iia", g_wl_types_null },
    { "close", "", g_wl_types_null },
    { "configure_bounds", "4ii", g_wl_types_null },
};
static struct wl_interface g_xdg_toplevel_iface = {
    "xdg_toplevel", 4, 14, g_xdg_toplevel_requests, 3, g_xdg_toplevel_events
};

static const struct wl_message g_xdg_popup_requests[1] = {
    { "destroy", "", g_wl_types_null },
};
static struct wl_interface g_xdg_popup_iface = {
    "xdg_popup", 4, 1, g_xdg_popup_requests, 0, 0
};

// wp_viewporter / wp_viewport
static struct wl_interface g_wp_viewport_iface;
static const struct wl_interface *g_wp_get_viewport_types[2] = { &g_wp_viewport_iface, 0 /* wl_surface */ };
static const struct wl_message g_wp_viewporter_requests[2] = {
    { "destroy", "", g_wl_types_null },
    { "get_viewport", "no", g_wp_get_viewport_types },
};
static struct wl_interface g_wp_viewporter_iface = {
    "wp_viewporter", 1, 2, g_wp_viewporter_requests, 0, 0
};
static const struct wl_message g_wp_viewport_requests[3] = {
    { "destroy", "", g_wl_types_null },
    { "set_source", "ffff", g_wl_types_null },
    { "set_destination", "ii", g_wl_types_null },
};
static struct wl_interface g_wp_viewport_iface = {
    "wp_viewport", 1, 3, g_wp_viewport_requests, 0, 0
};

// zwp_relative_pointer_manager_v1 / zwp_relative_pointer_v1
static struct wl_interface g_zwp_rel_ptr_iface;
static const struct wl_interface *g_zwp_get_rel_ptr_types[2] = { &g_zwp_rel_ptr_iface, 0 /* wl_pointer */ };
static const struct wl_message g_zwp_rel_ptr_mgr_requests[2] = {
    { "destroy", "", g_wl_types_null },
    { "get_relative_pointer", "no", g_zwp_get_rel_ptr_types },
};
static struct wl_interface g_zwp_rel_ptr_mgr_iface = {
    "zwp_relative_pointer_manager_v1", 1, 2, g_zwp_rel_ptr_mgr_requests, 0, 0
};
static const struct wl_message g_zwp_rel_ptr_requests[1] = {
    { "destroy", "", g_wl_types_null },
};
static const struct wl_message g_zwp_rel_ptr_events[1] = {
    { "relative_motion", "uuffff", g_wl_types_null },
};
static struct wl_interface g_zwp_rel_ptr_iface = {
    "zwp_relative_pointer_v1", 1, 1, g_zwp_rel_ptr_requests, 1, g_zwp_rel_ptr_events
};

// zwp_pointer_constraints_v1 / zwp_locked_pointer_v1 / zwp_confined_pointer_v1
static struct wl_interface g_zwp_locked_ptr_iface;
static struct wl_interface g_zwp_confined_ptr_iface;
static const struct wl_interface *g_zwp_lock_ptr_types[5] = {
    &g_zwp_locked_ptr_iface, 0 /* wl_surface */, 0 /* wl_pointer */, 0 /* wl_region */, 0
};
static const struct wl_interface *g_zwp_confine_ptr_types[5] = {
    &g_zwp_confined_ptr_iface, 0 /* wl_surface */, 0 /* wl_pointer */, 0 /* wl_region */, 0
};
static const struct wl_interface *g_zwp_set_region_types[1] = { 0 /* wl_region */ };
static const struct wl_message g_zwp_ptr_constraints_requests[3] = {
    { "destroy", "", g_wl_types_null },
    { "lock_pointer", "noo?ou", g_zwp_lock_ptr_types },
    { "confine_pointer", "noo?ou", g_zwp_confine_ptr_types },
};
static struct wl_interface g_zwp_ptr_constraints_iface = {
    "zwp_pointer_constraints_v1", 1, 3, g_zwp_ptr_constraints_requests, 0, 0
};
static const struct wl_message g_zwp_locked_ptr_requests[3] = {
    { "destroy", "", g_wl_types_null },
    { "set_cursor_position_hint", "ff", g_wl_types_null },
    { "set_region", "?o", g_zwp_set_region_types },
};
static const struct wl_message g_zwp_locked_ptr_events[2] = {
    { "locked", "", g_wl_types_null },
    { "unlocked", "", g_wl_types_null },
};
static struct wl_interface g_zwp_locked_ptr_iface = {
    "zwp_locked_pointer_v1", 1, 3, g_zwp_locked_ptr_requests, 2, g_zwp_locked_ptr_events
};
static const struct wl_message g_zwp_confined_ptr_requests[2] = {
    { "destroy", "", g_wl_types_null },
    { "set_region", "?o", g_zwp_set_region_types },
};
static const struct wl_message g_zwp_confined_ptr_events[2] = {
    { "confined", "", g_wl_types_null },
    { "unconfined", "", g_wl_types_null },
};
static struct wl_interface g_zwp_confined_ptr_iface = {
    "zwp_confined_pointer_v1", 1, 2, g_zwp_confined_ptr_requests, 2, g_zwp_confined_ptr_events
};

// zwp_idle_inhibit_manager_v1 / zwp_idle_inhibitor_v1
static struct wl_interface g_zwp_idle_inhibitor_iface;
static const struct wl_interface *g_zwp_create_inhibitor_types[2] = {
    &g_zwp_idle_inhibitor_iface, 0 /* wl_surface */
};
static const struct wl_message g_zwp_idle_mgr_requests[2] = {
    { "destroy", "", g_wl_types_null },
    { "create_inhibitor", "no", g_zwp_create_inhibitor_types },
};
static struct wl_interface g_zwp_idle_mgr_iface = {
    "zwp_idle_inhibit_manager_v1", 1, 2, g_zwp_idle_mgr_requests, 0, 0
};
static const struct wl_message g_zwp_idle_inhibitor_requests[1] = {
    { "destroy", "", g_wl_types_null },
};
static struct wl_interface g_zwp_idle_inhibitor_iface = {
    "zwp_idle_inhibitor_v1", 1, 1, g_zwp_idle_inhibitor_requests, 0, 0
};

// zxdg_decoration_manager_v1 / zxdg_toplevel_decoration_v1
static struct wl_interface g_zxdg_toplevel_deco_iface;
static const struct wl_interface *g_zxdg_get_deco_types[2] = {
    &g_zxdg_toplevel_deco_iface, &g_xdg_toplevel_iface
};
static const struct wl_message g_zxdg_deco_mgr_requests[2] = {
    { "destroy", "", g_wl_types_null },
    { "get_toplevel_decoration", "no", g_zxdg_get_deco_types },
};
static struct wl_interface g_zxdg_deco_mgr_iface = {
    "zxdg_decoration_manager_v1", 1, 2, g_zxdg_deco_mgr_requests, 0, 0
};
static const struct wl_message g_zxdg_toplevel_deco_requests[3] = {
    { "destroy", "", g_wl_types_null },
    { "set_mode", "u", g_wl_types_null },
    { "unset_mode", "", g_wl_types_null },
};
static const struct wl_message g_zxdg_toplevel_deco_events[1] = {
    { "configure", "u", g_wl_types_null },
};
static struct wl_interface g_zxdg_toplevel_deco_iface = {
    "zxdg_toplevel_decoration_v1", 1, 3, g_zxdg_toplevel_deco_requests, 1, g_zxdg_toplevel_deco_events
};

static struct {
    void *library;
    int tried;
    struct wl_display *(*display_connect)(const char *);
    void (*display_disconnect)(struct wl_display *);
    int (*display_get_fd)(struct wl_display *);
    int (*display_dispatch_pending)(struct wl_display *);
    int (*display_flush)(struct wl_display *);
    int (*display_roundtrip)(struct wl_display *);
    int (*display_prepare_read)(struct wl_display *);
    void (*display_cancel_read)(struct wl_display *);
    int (*display_read_events)(struct wl_display *);
    int (*proxy_add_listener)(struct wl_proxy *, void (**)(void), void *);
    void (*proxy_destroy)(struct wl_proxy *);
    uint32_t (*proxy_get_version)(struct wl_proxy *);
    void (*proxy_marshal)(struct wl_proxy *, uint32_t, ...);
    struct wl_proxy *(*proxy_marshal_constructor)(struct wl_proxy *, uint32_t, const struct wl_interface *, ...);
    struct wl_proxy *(*proxy_marshal_constructor_versioned)(struct wl_proxy *, uint32_t, const struct wl_interface *,
                                                            uint32_t, ...);
    const struct wl_interface *registry_iface;
    const struct wl_interface *compositor_iface;
    const struct wl_interface *shm_iface;
    const struct wl_interface *surface_iface;
    const struct wl_interface *seat_iface;
    const struct wl_interface *pointer_iface;
    const struct wl_interface *keyboard_iface;
    const struct wl_interface *output_iface;
    const struct wl_interface *region_iface;

    // Optional libwayland-cursor.so.0 for standard cursor shape when uncaptured (menus / in-game Guide)
    void *cursor_library;
    struct wl_cursor_theme *(*cursor_theme_load)(const char *, int, struct wl_proxy *);
    void (*cursor_theme_destroy)(struct wl_cursor_theme *);
    struct wl_cursor *(*cursor_theme_get_cursor)(struct wl_cursor_theme *, const char *);
    struct wl_proxy *(*cursor_image_get_buffer)(struct wl_cursor_image *);

    // Bound globals and active objects
    struct wl_display *display;
    struct wl_proxy *registry;
    struct wl_proxy *compositor;
    struct wl_proxy *shm;
    struct wl_proxy *seat;
    struct wl_proxy *pointer;
    struct wl_proxy *keyboard;
    struct wl_proxy *xdg_wm_base;
    struct wl_proxy *viewporter;
    struct wl_proxy *viewport;
    struct wl_proxy *rel_ptr_mgr;
    struct wl_proxy *rel_ptr;
    struct wl_proxy *ptr_constraints;
    struct wl_proxy *locked_ptr;
    struct wl_proxy *idle_mgr;
    struct wl_proxy *idle_inhibitor;
    struct wl_proxy *deco_mgr;
    struct wl_proxy *toplevel_deco;
    struct wl_proxy *surface;
    struct wl_proxy *xdg_surface;
    struct wl_proxy *xdg_toplevel;
    struct wl_cursor_theme *cursor_theme;
    struct wl_proxy *cursor_surface;
    struct wl_cursor_image *cursor_image;

    unsigned int req_width, req_height;
    unsigned int width, height; // logical surface dimensions (used for viewport destination & UI pointer mapping)
    int configured;
    int pending_width, pending_height;
    int pending_activated, has_pending_state;

    uint32_t pointer_enter_serial;
    int pointer_inside;
    int pointer_x, pointer_y;
    double motion_frac_x, motion_frac_y;
    double wheel_accum;
    int grabbed;
    int capture_seen;
    int focused;
    int guide;
    int shift_down, ctrl_down, alt_down, caps_lock;
    int repeat_rate, repeat_delay;
    unsigned int repeat_evdev;
    long long repeat_next_ms;
    unsigned char keys_down[32], keys_held_over[32];
    int open;
    int close_requested;
    int mode;
    unsigned long mode_revision;
} g_wl;

static long long monotonic_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return 0;
    return (long long)ts.tv_sec * 1000ll + (long long)(ts.tv_nsec / 1000000l);
}

static int load_wayland(void)
{
    if (g_wl.library != 0)
        return 1;
    if (g_wl.tried)
        return 0;
    g_wl.tried = 1;
    g_wl.library = dlopen("libwayland-client.so.0", RTLD_NOW | RTLD_LOCAL);
    if (g_wl.library == 0)
        return 0;
#define WL_LOAD(field, sym)                                                                                         \
    do {                                                                                                            \
        *(void **)&g_wl.field = dlsym(g_wl.library, #sym);                                                          \
        if (g_wl.field == 0) {                                                                                      \
            dlclose(g_wl.library);                                                                                  \
            g_wl.library = 0;                                                                                       \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    WL_LOAD(display_connect, wl_display_connect);
    WL_LOAD(display_disconnect, wl_display_disconnect);
    WL_LOAD(display_get_fd, wl_display_get_fd);
    WL_LOAD(display_dispatch_pending, wl_display_dispatch_pending);
    WL_LOAD(display_flush, wl_display_flush);
    WL_LOAD(display_roundtrip, wl_display_roundtrip);
    WL_LOAD(display_prepare_read, wl_display_prepare_read);
    WL_LOAD(display_cancel_read, wl_display_cancel_read);
    WL_LOAD(display_read_events, wl_display_read_events);
    WL_LOAD(proxy_add_listener, wl_proxy_add_listener);
    WL_LOAD(proxy_destroy, wl_proxy_destroy);
    WL_LOAD(proxy_get_version, wl_proxy_get_version);
    WL_LOAD(proxy_marshal, wl_proxy_marshal);
    WL_LOAD(proxy_marshal_constructor, wl_proxy_marshal_constructor);
    WL_LOAD(proxy_marshal_constructor_versioned, wl_proxy_marshal_constructor_versioned);
    WL_LOAD(registry_iface, wl_registry_interface);
    WL_LOAD(compositor_iface, wl_compositor_interface);
    WL_LOAD(shm_iface, wl_shm_interface);
    WL_LOAD(surface_iface, wl_surface_interface);
    WL_LOAD(seat_iface, wl_seat_interface);
    WL_LOAD(pointer_iface, wl_pointer_interface);
    WL_LOAD(keyboard_iface, wl_keyboard_interface);
    WL_LOAD(output_iface, wl_output_interface);
    WL_LOAD(region_iface, wl_region_interface);
#undef WL_LOAD

    // Patch runtime core interface addresses into extension message type tables.
    g_xdg_get_surface_types[1] = g_wl.surface_iface;
    g_xdg_window_menu_types[0] = g_wl.seat_iface;
    g_xdg_move_types[0] = g_wl.seat_iface;
    g_xdg_resize_types[0] = g_wl.seat_iface;
    g_xdg_fullscreen_types[0] = g_wl.output_iface;
    g_wp_get_viewport_types[1] = g_wl.surface_iface;
    g_zwp_get_rel_ptr_types[1] = g_wl.pointer_iface;
    g_zwp_lock_ptr_types[1] = g_wl.surface_iface;
    g_zwp_lock_ptr_types[2] = g_wl.pointer_iface;
    g_zwp_lock_ptr_types[3] = g_wl.region_iface;
    g_zwp_confine_ptr_types[1] = g_wl.surface_iface;
    g_zwp_confine_ptr_types[2] = g_wl.pointer_iface;
    g_zwp_confine_ptr_types[3] = g_wl.region_iface;
    g_zwp_set_region_types[0] = g_wl.region_iface;
    g_zwp_create_inhibitor_types[1] = g_wl.surface_iface;

    g_wl.cursor_library = dlopen("libwayland-cursor.so.0", RTLD_NOW | RTLD_LOCAL);
    if (g_wl.cursor_library != 0) {
        *(void **)&g_wl.cursor_theme_load = dlsym(g_wl.cursor_library, "wl_cursor_theme_load");
        *(void **)&g_wl.cursor_theme_destroy = dlsym(g_wl.cursor_library, "wl_cursor_theme_destroy");
        *(void **)&g_wl.cursor_theme_get_cursor = dlsym(g_wl.cursor_library, "wl_cursor_theme_get_cursor");
        *(void **)&g_wl.cursor_image_get_buffer = dlsym(g_wl.cursor_library, "wl_cursor_image_get_buffer");
    }
    return 1;
}

static struct wl_proxy *wl_registry_bind_obj(struct wl_proxy *registry, uint32_t name,
                                             const struct wl_interface *iface, uint32_t version)
{
    return g_wl.proxy_marshal_constructor_versioned(registry, 0u, iface, version, name, iface->name, version, NULL);
}

static void wl_destroy_request(struct wl_proxy **proxy, uint32_t destroy_opcode)
{
    if (*proxy != 0) {
        g_wl.proxy_marshal(*proxy, destroy_opcode);
        g_wl.proxy_destroy(*proxy);
        *proxy = 0;
    }
}

// Maps Linux evdev keycodes (<linux/input-event-codes.h>) to e_port_key.
static enum e_port_key key_of_evdev(unsigned int evdev)
{
    switch (evdev) {
    case KEY_A: return k_port_key_a;
    case KEY_B: return k_port_key_b;
    case KEY_C: return k_port_key_c;
    case KEY_D: return k_port_key_d;
    case KEY_E: return k_port_key_e;
    case KEY_F: return k_port_key_f;
    case KEY_G: return k_port_key_g;
    case KEY_H: return k_port_key_h;
    case KEY_I: return k_port_key_i;
    case KEY_J: return k_port_key_j;
    case KEY_K: return k_port_key_k;
    case KEY_L: return k_port_key_l;
    case KEY_M: return k_port_key_m;
    case KEY_N: return k_port_key_n;
    case KEY_O: return k_port_key_o;
    case KEY_P: return k_port_key_p;
    case KEY_Q: return k_port_key_q;
    case KEY_R: return k_port_key_r;
    case KEY_S: return k_port_key_s;
    case KEY_T: return k_port_key_t;
    case KEY_U: return k_port_key_u;
    case KEY_V: return k_port_key_v;
    case KEY_W: return k_port_key_w;
    case KEY_X: return k_port_key_x;
    case KEY_Y: return k_port_key_y;
    case KEY_Z: return k_port_key_z;
    case KEY_0: return k_port_key_0;
    case KEY_1: return k_port_key_1;
    case KEY_2: return k_port_key_2;
    case KEY_3: return k_port_key_3;
    case KEY_4: return k_port_key_4;
    case KEY_5: return k_port_key_5;
    case KEY_6: return k_port_key_6;
    case KEY_7: return k_port_key_7;
    case KEY_8: return k_port_key_8;
    case KEY_9: return k_port_key_9;
    case KEY_F1: return k_port_key_f1;
    case KEY_F2: return k_port_key_f2;
    case KEY_F3: return k_port_key_f3;
    case KEY_F4: return k_port_key_f4;
    case KEY_F5: return k_port_key_f5;
    case KEY_F6: return k_port_key_f6;
    case KEY_F7: return k_port_key_f7;
    case KEY_F8: return k_port_key_f8;
    case KEY_F9: return k_port_key_f9;
    case KEY_F10: return k_port_key_f10;
    case KEY_F11: return k_port_key_f11;
    case KEY_F12: return k_port_key_f12;
    case KEY_ESC: return k_port_key_escape;
    case KEY_TAB: return k_port_key_tab;
    case KEY_SPACE: return k_port_key_space;
    case KEY_ENTER:
    case KEY_KPENTER: return k_port_key_return;
    case KEY_BACKSPACE: return k_port_key_backspace;
    case KEY_UP: return k_port_key_up;
    case KEY_DOWN: return k_port_key_down;
    case KEY_LEFT: return k_port_key_left;
    case KEY_RIGHT: return k_port_key_right;
    case KEY_LEFTSHIFT: return k_port_key_lshift;
    case KEY_RIGHTSHIFT: return k_port_key_rshift;
    case KEY_LEFTCTRL: return k_port_key_lctrl;
    case KEY_RIGHTCTRL: return k_port_key_rctrl;
    case KEY_LEFTALT:
    case KEY_LEFTMETA: return k_port_key_lalt;
    case KEY_RIGHTALT:
    case KEY_RIGHTMETA: return k_port_key_ralt;
    case KEY_CAPSLOCK: return k_port_key_capslock;
    case KEY_HOME: return k_port_key_home;
    case KEY_END: return k_port_key_end;
    case KEY_INSERT: return k_port_key_insert;
    case KEY_DELETE: return k_port_key_delete;
    case KEY_PAGEUP: return k_port_key_pageup;
    case KEY_PAGEDOWN: return k_port_key_pagedown;
    case KEY_GRAVE: return k_port_key_grave;
    case KEY_MINUS: return k_port_key_minus;
    case KEY_EQUAL: return k_port_key_equals;
    case KEY_LEFTBRACE: return k_port_key_lbracket;
    case KEY_RIGHTBRACE: return k_port_key_rbracket;
    case KEY_BACKSLASH: return k_port_key_backslash;
    case KEY_SEMICOLON: return k_port_key_semicolon;
    case KEY_APOSTROPHE: return k_port_key_apostrophe;
    case KEY_COMMA: return k_port_key_comma;
    case KEY_DOT: return k_port_key_period;
    case KEY_SLASH: return k_port_key_slash;
    default: return k_port_key_none;
    }
}

static unsigned long evdev_ascii(unsigned int evdev, int shift, int caps)
{
    const enum e_port_key pk = key_of_evdev(evdev);
    if (pk >= k_port_key_a && pk <= k_port_key_z) {
        const int upper = shift ^ caps;
        return (unsigned long)((upper ? 'A' : 'a') + (pk - k_port_key_a));
    }
    switch (evdev) {
    case KEY_1: return shift ? '!' : '1';
    case KEY_2: return shift ? '@' : '2';
    case KEY_3: return shift ? '#' : '3';
    case KEY_4: return shift ? '$' : '4';
    case KEY_5: return shift ? '%' : '5';
    case KEY_6: return shift ? '^' : '6';
    case KEY_7: return shift ? '&' : '7';
    case KEY_8: return shift ? '*' : '8';
    case KEY_9: return shift ? '(' : '9';
    case KEY_0: return shift ? ')' : '0';
    case KEY_SPACE: return ' ';
    case KEY_MINUS: return shift ? '_' : '-';
    case KEY_EQUAL: return shift ? '+' : '=';
    case KEY_LEFTBRACE: return shift ? '{' : '[';
    case KEY_RIGHTBRACE: return shift ? '}' : ']';
    case KEY_BACKSLASH: return shift ? '|' : '\\';
    case KEY_SEMICOLON: return shift ? ':' : ';';
    case KEY_APOSTROPHE: return shift ? '"' : '\'';
    case KEY_GRAVE: return shift ? '~' : '`';
    case KEY_COMMA: return shift ? '<' : ',';
    case KEY_DOT: return shift ? '>' : '.';
    case KEY_SLASH: return shift ? '?' : '/';
    case KEY_KP0: return '0';
    case KEY_KP1: return '1';
    case KEY_KP2: return '2';
    case KEY_KP3: return '3';
    case KEY_KP4: return '4';
    case KEY_KP5: return '5';
    case KEY_KP6: return '6';
    case KEY_KP7: return '7';
    case KEY_KP8: return '8';
    case KEY_KP9: return '9';
    case KEY_KPDOT: return '.';
    case KEY_KPSLASH: return '/';
    case KEY_KPASTERISK: return '*';
    case KEY_KPMINUS: return '-';
    case KEY_KPPLUS: return '+';
    default: return 0;
    }
}

static void wl_guide_key_event(unsigned int evdev, int action)
{
    unsigned long character = 0;
    int key = -1;
    switch (evdev) {
    case KEY_UP:
    case KEY_KP8: key = GUIDE_KEY_UP; break;
    case KEY_DOWN:
    case KEY_KP2: key = GUIDE_KEY_DOWN; break;
    case KEY_LEFT:
    case KEY_KP4: key = GUIDE_KEY_LEFT; break;
    case KEY_RIGHT:
    case KEY_KP6: key = GUIDE_KEY_RIGHT; break;
    case KEY_PAGEUP:
    case KEY_KP9: key = GUIDE_KEY_PAGE_UP; break;
    case KEY_PAGEDOWN:
    case KEY_KP3: key = GUIDE_KEY_PAGE_DOWN; break;
    case KEY_ENTER:
    case KEY_KPENTER: key = GUIDE_KEY_ACCEPT; break;
    case KEY_ESC: key = GUIDE_KEY_BACK; break;
    case KEY_BACKSPACE: key = GUIDE_KEY_ERASE; break;
    case KEY_TAB: key = g_wl.shift_down ? GUIDE_KEY_PREVIOUS_TAB : GUIDE_KEY_NEXT_TAB; break;
    case KEY_HOME:
    case KEY_KP7: key = GUIDE_KEY_CLOSE; break;
    case KEY_INSERT:
    case KEY_KP0:
        if (g_wl.shift_down)
            key = GUIDE_KEY_PASTE;
        break;
    case KEY_V:
        if (g_wl.ctrl_down) {
            key = GUIDE_KEY_PASTE;
            break;
        }
        /* fallthrough */
    default:
        if (!g_wl.ctrl_down && !g_wl.alt_down) {
            character = evdev_ascii(evdev, g_wl.shift_down, g_wl.caps_lock);
            if (character != 0)
                key = GUIDE_KEY_CHARACTER;
        }
        break;
    }
    if (key >= 0)
        guide_ingame_key_action(key, character, action);
}

static void wl_update_cursor_image(void)
{
    if (g_wl.pointer == 0 || !g_wl.pointer_inside)
        return;
    if (g_wl.grabbed) {
        // Hide cursor (wl_pointer::set_cursor opcode 0: serial, NULL surface, 0, 0)
        g_wl.proxy_marshal(g_wl.pointer, 0u, g_wl.pointer_enter_serial, NULL, 0, 0);
        return;
    }
    if (g_wl.cursor_surface != 0 && g_wl.cursor_image != 0 && g_wl.cursor_image_get_buffer != 0) {
        struct wl_proxy *buffer = g_wl.cursor_image_get_buffer(g_wl.cursor_image);
        if (buffer != 0) {
            g_wl.proxy_marshal(g_wl.pointer, 0u, g_wl.pointer_enter_serial, g_wl.cursor_surface,
                               (int32_t)g_wl.cursor_image->hotspot_x, (int32_t)g_wl.cursor_image->hotspot_y);
            // wl_surface: attach (1), damage (2), commit (6)
            g_wl.proxy_marshal(g_wl.cursor_surface, 1u, buffer, 0, 0);
            g_wl.proxy_marshal(g_wl.cursor_surface, 2u, 0, 0,
                               (int32_t)g_wl.cursor_image->width, (int32_t)g_wl.cursor_image->height);
            g_wl.proxy_marshal(g_wl.cursor_surface, 6u);
        }
    }
}

static void wl_locked_ptr_nop(void *data, struct wl_proxy *proxy)
{
    (void)data;
    (void)proxy;
}

static void (*g_wl_locked_ptr_listener[2])(void) = {
    (void (*)(void))wl_locked_ptr_nop,
    (void (*)(void))wl_locked_ptr_nop,
};

static void wl_mouse_grab(int grab)
{
    grab = grab && port_input_mouse_capture();
    if (grab == g_wl.grabbed)
        return;
    if (grab) {
        if (g_wl.pointer == 0 || g_wl.surface == 0)
            return;
        if (g_wl.ptr_constraints != 0 && g_wl.locked_ptr == 0) {
            // zwp_pointer_constraints_v1::lock_pointer (opcode 1)
            g_wl.locked_ptr = g_wl.proxy_marshal_constructor(
                g_wl.ptr_constraints, 1u, &g_zwp_locked_ptr_iface, NULL, g_wl.surface, g_wl.pointer, NULL,
                (uint32_t)ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);
            if (g_wl.locked_ptr != 0)
                g_wl.proxy_add_listener(g_wl.locked_ptr, g_wl_locked_ptr_listener, 0);
        }
    } else {
        wl_destroy_request(&g_wl.locked_ptr, 0u);
    }
    g_wl.grabbed = grab;
    wl_update_cursor_image();
}

static void wl_guide_follow(void)
{
    const int guide = guide_ingame_input();
    if (guide == g_wl.guide)
        return;
    g_wl.guide = guide;
    if (!guide)
        memcpy(g_wl.keys_held_over, g_wl.keys_down, sizeof(g_wl.keys_held_over));
    port_input_release_all();
    g_wl.motion_frac_x = g_wl.motion_frac_y = 0.0;
    wl_mouse_grab(!guide && g_wl.focused);
}

static void wl_update_viewport(void)
{
    if (g_wl.viewport != 0 && g_wl.width > 0 && g_wl.height > 0) {
        // wp_viewport::set_destination (opcode 2) maps the Vulkan swapchain buffer cleanly onto the compositor's
        // logical surface rectangle, avoiding Xwayland fractional-scaling oversampling while allowing direct KMS scanout.
        g_wl.proxy_marshal(g_wl.viewport, 2u, (int32_t)g_wl.width, (int32_t)g_wl.height);
    }
}

static void wl_window_mode_apply(int mode)
{
    if (g_wl.xdg_toplevel == 0)
        return;
    // xdg_toplevel opcodes: set_maximized=9, unset_maximized=10, set_fullscreen=11, unset_fullscreen=12
    if (mode != PORT_WINDOW_MODE_FULLSCREEN)
        g_wl.proxy_marshal(g_wl.xdg_toplevel, 12u);
    if (mode != PORT_WINDOW_MODE_BORDERLESS)
        g_wl.proxy_marshal(g_wl.xdg_toplevel, 10u);
    if (g_wl.toplevel_deco != 0) {
        const uint32_t deco_mode = mode == PORT_WINDOW_MODE_WINDOWED
                                       ? ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
                                       : ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;
        g_wl.proxy_marshal(g_wl.toplevel_deco, 1u, deco_mode);
    }
    if (mode == PORT_WINDOW_MODE_FULLSCREEN)
        g_wl.proxy_marshal(g_wl.xdg_toplevel, 11u, NULL);
    else if (mode == PORT_WINDOW_MODE_BORDERLESS)
        g_wl.proxy_marshal(g_wl.xdg_toplevel, 9u);
    g_wl.mode = mode;
}

static void wl_window_mode_follow(void)
{
    const unsigned long revision = port_config_revision();
    if (revision == g_wl.mode_revision)
        return;
    g_wl.mode_revision = revision;
    const int mode = port_config_window_mode();
    if (mode != g_wl.mode) {
        wl_window_mode_apply(mode);
        g_wl.display_flush(g_wl.display);
    }
}

// ---- Wayland event listeners ----

static void wl_wm_base_ping(void *data, struct wl_proxy *wm_base, uint32_t serial)
{
    (void)data;
    g_wl.proxy_marshal(wm_base, 3u, serial); // xdg_wm_base::pong
}

static void (*g_wl_wm_base_listener[1])(void) = {
    (void (*)(void))wl_wm_base_ping,
};

static void wl_release_all_keys(void)
{
    port_input_release_all();
    memset(g_wl.keys_down, 0, sizeof(g_wl.keys_down));
    memset(g_wl.keys_held_over, 0, sizeof(g_wl.keys_held_over));
    g_wl.repeat_evdev = 0;
    g_wl.shift_down = g_wl.ctrl_down = g_wl.alt_down = 0;
    guide_keys_released();
}

static void wl_set_focused(int focused)
{
    const int prev = __atomic_load_n(&g_wl.focused, __ATOMIC_ACQUIRE);
    if (focused == prev)
        return;
    __atomic_store_n(&g_wl.focused, focused, __ATOMIC_RELEASE);
    if (focused) {
        wl_mouse_grab(!g_wl.guide);
    } else {
        wl_mouse_grab(0);
        g_wl.motion_frac_x = g_wl.motion_frac_y = 0.0;
        wl_release_all_keys();
    }
}

static void wl_xdg_surface_configure(void *data, struct wl_proxy *xdg_surface, uint32_t serial)
{
    (void)data;
    if (g_wl.pending_width > 0 && g_wl.pending_height > 0) {
        g_wl.width = (unsigned int)g_wl.pending_width;
        g_wl.height = (unsigned int)g_wl.pending_height;
    } else if (!g_wl.configured) {
        g_wl.width = g_wl.req_width;
        g_wl.height = g_wl.req_height;
    }
    if (g_wl.has_pending_state) {
        wl_set_focused(g_wl.pending_activated);
        g_wl.has_pending_state = 0;
    }
    g_wl.pending_width = g_wl.pending_height = 0;
    g_wl.proxy_marshal(xdg_surface, 4u, serial); // xdg_surface::ack_configure
    wl_update_viewport();
    if (!g_wl.configured) {
        g_wl.configured = 1;
        g_wl.proxy_marshal(g_wl.surface, 6u); // initial wl_surface::commit after first ack_configure
    }
}

static void (*g_wl_xdg_surface_listener[1])(void) = {
    (void (*)(void))wl_xdg_surface_configure,
};

static void wl_xdg_toplevel_configure(void *data, struct wl_proxy *toplevel, int32_t width, int32_t height,
                                      struct wl_array *states)
{
    (void)data;
    (void)toplevel;
    g_wl.pending_width = width;
    g_wl.pending_height = height;
    int activated = 0;
    if (states != 0 && states->data != 0) {
        const uint32_t *items = (const uint32_t *)states->data;
        const size_t count = states->size / sizeof(uint32_t);
        for (size_t i = 0; i < count; i++) {
            if (items[i] == XDG_TOPLEVEL_STATE_ACTIVATED)
                activated = 1;
        }
    }
    g_wl.pending_activated = activated;
    g_wl.has_pending_state = 1;
}

static void wl_xdg_toplevel_close(void *data, struct wl_proxy *toplevel)
{
    (void)data;
    (void)toplevel;
    g_wl.close_requested = 1;
}

static void wl_xdg_toplevel_bounds(void *data, struct wl_proxy *toplevel, int32_t width, int32_t height)
{
    (void)data;
    (void)toplevel;
    (void)width;
    (void)height;
}

static void (*g_wl_xdg_toplevel_listener[3])(void) = {
    (void (*)(void))wl_xdg_toplevel_configure,
    (void (*)(void))wl_xdg_toplevel_close,
    (void (*)(void))wl_xdg_toplevel_bounds,
};

static void wl_deco_configure(void *data, struct wl_proxy *deco, uint32_t mode)
{
    (void)data;
    (void)deco;
    (void)mode;
}

static void (*g_wl_deco_listener[1])(void) = {
    (void (*)(void))wl_deco_configure,
};

// Relative pointer motion (unaccelerated hardware counts as 24.8 wl_fixed_t).
static void wl_rel_ptr_motion(void *data, struct wl_proxy *rel_ptr, uint32_t utime_hi, uint32_t utime_lo,
                              wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel)
{
    (void)data;
    (void)rel_ptr;
    (void)utime_hi;
    (void)utime_lo;
    wl_guide_follow();
    if (!g_wl.focused || g_wl.guide)
        return;
    const double ux = (double)dx_unaccel / 256.0;
    const double uy = (double)dy_unaccel / 256.0;
    if (port_input_mouse_raw()) {
        if (ux != 0.0 || uy != 0.0)
            port_input_mouse_raw_motion(ux, uy);
    } else if (g_wl.grabbed) {
        g_wl.motion_frac_x += (double)dx / 256.0;
        g_wl.motion_frac_y += (double)dy / 256.0;
        const int ix = (int)g_wl.motion_frac_x;
        const int iy = (int)g_wl.motion_frac_y;
        if (ix != 0 || iy != 0) {
            g_wl.motion_frac_x -= (double)ix;
            g_wl.motion_frac_y -= (double)iy;
            port_input_mouse_motion(ix, iy);
        }
    }
}

static void (*g_wl_rel_ptr_listener[1])(void) = {
    (void (*)(void))wl_rel_ptr_motion,
};

// wl_pointer events
static void wl_pointer_enter(void *data, struct wl_proxy *pointer, uint32_t serial, struct wl_proxy *surface,
                             wl_fixed_t sx, wl_fixed_t sy)
{
    (void)data;
    (void)pointer;
    if (surface != g_wl.surface)
        return;
    g_wl.pointer_enter_serial = serial;
    g_wl.pointer_inside = 1;
    g_wl.pointer_x = sx / 256;
    g_wl.pointer_y = sy / 256;
    port_input_mouse_position(g_wl.pointer_x, g_wl.pointer_y, (int)g_wl.width, (int)g_wl.height);
    wl_update_cursor_image();
}

static void wl_pointer_leave(void *data, struct wl_proxy *pointer, uint32_t serial, struct wl_proxy *surface)
{
    (void)data;
    (void)pointer;
    (void)serial;
    if (surface == g_wl.surface)
        g_wl.pointer_inside = 0;
}

static void wl_pointer_motion(void *data, struct wl_proxy *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    (void)data;
    (void)pointer;
    (void)time;
    wl_guide_follow();
    const int x = sx / 256, y = sy / 256;
    const int prev_x = g_wl.pointer_x, prev_y = g_wl.pointer_y;
    g_wl.pointer_x = x;
    g_wl.pointer_y = y;
    port_input_mouse_position(x, y, (int)g_wl.width, (int)g_wl.height);
    if (g_wl.guide) {
        guide_ingame_pointer(GUIDE_POINTER_MOVE, 0, x, y, (int)g_wl.width, (int)g_wl.height);
        return;
    }
    if (!g_wl.grabbed && g_wl.pointer_inside)
        port_input_mouse_motion(x - prev_x, y - prev_y);
}

static int wl_button_code(uint32_t button, int *out_guide_btn)
{
    *out_guide_btn = 0;
    switch (button) {
    case BTN_LEFT: *out_guide_btn = 1; return PORT_KBM_MOUSE(k_port_mouse_left);
    case BTN_MIDDLE: *out_guide_btn = 2; return PORT_KBM_MOUSE(k_port_mouse_middle);
    case BTN_RIGHT: *out_guide_btn = 3; return PORT_KBM_MOUSE(k_port_mouse_right);
    case BTN_SIDE: return PORT_KBM_MOUSE(k_port_mouse_4);
    case BTN_EXTRA: return PORT_KBM_MOUSE(k_port_mouse_5);
    default: return PORT_KBM_NONE;
    }
}

static void wl_pointer_button(void *data, struct wl_proxy *pointer, uint32_t serial, uint32_t time,
                              uint32_t button, uint32_t state)
{
    (void)data;
    (void)pointer;
    (void)serial;
    (void)time;
    wl_guide_follow();
    const int press = state != 0;
    int guide_btn = 0;
    const int code = wl_button_code(button, &guide_btn);
    port_input_mouse_position(g_wl.pointer_x, g_wl.pointer_y, (int)g_wl.width, (int)g_wl.height);
    if (press && g_wl.guide) {
        if (code != PORT_KBM_NONE && guide_capture(code))
            return;
        if (guide_btn != 0)
            guide_ingame_pointer(GUIDE_POINTER_PRESS, guide_btn, g_wl.pointer_x, g_wl.pointer_y,
                                 (int)g_wl.width, (int)g_wl.height);
        return;
    }
    if (press && g_wl.focused && !g_wl.guide)
        wl_mouse_grab(1);
    if (code != PORT_KBM_NONE)
        port_input_mouse_button((enum e_port_mouse_button)(code - PORT_KBM_MOUSE_FIRST), press);
}

static void wl_pointer_axis(void *data, struct wl_proxy *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    (void)data;
    (void)pointer;
    (void)time;
    if (axis != 0u) // 0 = vertical scroll
        return;
    wl_guide_follow();
    // Standard Wayland discrete scroll step is ~10..15 units in surface space; positive value scrolls down.
    g_wl.wheel_accum += (double)value / 256.0;
    while (g_wl.wheel_accum >= 10.0 || g_wl.wheel_accum <= -10.0) {
        const int down = g_wl.wheel_accum > 0.0;
        g_wl.wheel_accum += down ? -10.0 : 10.0;
        const int code = down ? PORT_KBM_WHEEL_DOWN : PORT_KBM_WHEEL_UP;
        if (g_wl.guide) {
            if (!guide_capture(code))
                guide_ingame_pointer(GUIDE_POINTER_WHEEL, down ? 1 : -1, g_wl.pointer_x, g_wl.pointer_y,
                                     (int)g_wl.width, (int)g_wl.height);
        } else {
            port_input_mouse_wheel(down ? -1 : 1);
        }
    }
}

static void wl_pointer_frame(void *data, struct wl_proxy *pointer) { (void)data; (void)pointer; }
static void wl_pointer_axis_source(void *data, struct wl_proxy *pointer, uint32_t src) { (void)data; (void)pointer; (void)src; }
static void wl_pointer_axis_stop(void *data, struct wl_proxy *pointer, uint32_t t, uint32_t a) { (void)data; (void)pointer; (void)t; (void)a; }
static void wl_pointer_axis_discrete(void *data, struct wl_proxy *pointer, uint32_t a, int32_t d) { (void)data; (void)pointer; (void)a; (void)d; }

static void (*g_wl_pointer_listener[9])(void) = {
    (void (*)(void))wl_pointer_enter,
    (void (*)(void))wl_pointer_leave,
    (void (*)(void))wl_pointer_motion,
    (void (*)(void))wl_pointer_button,
    (void (*)(void))wl_pointer_axis,
    (void (*)(void))wl_pointer_frame,
    (void (*)(void))wl_pointer_axis_source,
    (void (*)(void))wl_pointer_axis_stop,
    (void (*)(void))wl_pointer_axis_discrete,
};

// wl_keyboard events
static void wl_keyboard_keymap(void *data, struct wl_proxy *keyboard, uint32_t format, int32_t fd, uint32_t size)
{
    (void)data;
    (void)keyboard;
    (void)format;
    (void)size;
    if (fd >= 0)
        close(fd);
}

static void wl_keyboard_enter(void *data, struct wl_proxy *keyboard, uint32_t serial, struct wl_proxy *surface,
                              struct wl_array *keys)
{
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)keys;
    if (surface == g_wl.surface)
        wl_set_focused(1);
}

static void wl_keyboard_leave(void *data, struct wl_proxy *keyboard, uint32_t serial, struct wl_proxy *surface)
{
    (void)data;
    (void)keyboard;
    (void)serial;
    if (surface == g_wl.surface)
        wl_set_focused(0);
}

static void wl_dispatch_key(unsigned int evdev, int press, int repeat)
{
    wl_guide_follow();
    if (evdev < 256u) {
        key_bit_set(g_wl.keys_down, evdev, press);
        if (!press)
            key_bit_set(g_wl.keys_held_over, evdev, 0);
    }
    if (evdev == KEY_LEFTSHIFT || evdev == KEY_RIGHTSHIFT)
        g_wl.shift_down = key_bit(g_wl.keys_down, KEY_LEFTSHIFT) || key_bit(g_wl.keys_down, KEY_RIGHTSHIFT);
    else if (evdev == KEY_LEFTCTRL || evdev == KEY_RIGHTCTRL)
        g_wl.ctrl_down = key_bit(g_wl.keys_down, KEY_LEFTCTRL) || key_bit(g_wl.keys_down, KEY_RIGHTCTRL);
    else if (evdev == KEY_LEFTALT || evdev == KEY_RIGHTALT)
        g_wl.alt_down = key_bit(g_wl.keys_down, KEY_LEFTALT) || key_bit(g_wl.keys_down, KEY_RIGHTALT);
    else if (evdev == KEY_CAPSLOCK && press && !repeat)
        g_wl.caps_lock ^= 1;

    const enum e_port_key pk = key_of_evdev(evdev);
    if (g_wl.guide && press && !repeat && guide_capture(pk))
        return;
    if (g_wl.guide)
        wl_guide_key_event(evdev, !press ? GUIDE_KEY_RELEASED : repeat ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
    if (!g_wl.guide || !press) {
        if (press && evdev < 256u && key_bit(g_wl.keys_held_over, evdev))
            return;
        port_input_key(pk, press);
    }
}

static void wl_keyboard_key(void *data, struct wl_proxy *keyboard, uint32_t serial, uint32_t time,
                            uint32_t key, uint32_t state)
{
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)time;
    const int press = state != 0u;
    const int repeat = state == 2u || (press && key < 256u && key_bit(g_wl.keys_down, key));
    if (press && !repeat && g_wl.repeat_rate > 0) {
        g_wl.repeat_evdev = key;
        g_wl.repeat_next_ms = monotonic_ms() + (g_wl.repeat_delay > 0 ? g_wl.repeat_delay : 400);
    } else if (!press && g_wl.repeat_evdev == key) {
        g_wl.repeat_evdev = 0;
    }
    wl_dispatch_key(key, press, repeat);
}

static void wl_keyboard_modifiers(void *data, struct wl_proxy *keyboard, uint32_t serial,
                                  uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                  uint32_t group)
{
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)mods_latched;
    (void)group;
    // Standard XKB modifier bit positions: bit 0 = Shift, bit 1 = Lock, bit 2 = Control, bit 3 = Mod1 (Alt)
    g_wl.shift_down = (mods_depressed & 1u) != 0;
    g_wl.caps_lock = (mods_locked & 2u) != 0;
    g_wl.ctrl_down = (mods_depressed & 4u) != 0;
    g_wl.alt_down = (mods_depressed & 8u) != 0;
}

static void wl_keyboard_repeat_info(void *data, struct wl_proxy *keyboard, int32_t rate, int32_t delay)
{
    (void)data;
    (void)keyboard;
    g_wl.repeat_rate = rate;
    g_wl.repeat_delay = delay;
}

static void (*g_wl_keyboard_listener[6])(void) = {
    (void (*)(void))wl_keyboard_keymap,
    (void (*)(void))wl_keyboard_enter,
    (void (*)(void))wl_keyboard_leave,
    (void (*)(void))wl_keyboard_key,
    (void (*)(void))wl_keyboard_modifiers,
    (void (*)(void))wl_keyboard_repeat_info,
};

static void wl_bind_rel_pointer(void)
{
    wl_destroy_request(&g_wl.rel_ptr, 0u);
    if (g_wl.rel_ptr_mgr != 0 && g_wl.pointer != 0) {
        g_wl.rel_ptr = g_wl.proxy_marshal_constructor(g_wl.rel_ptr_mgr, 1u, &g_zwp_rel_ptr_iface, NULL, g_wl.pointer);
        if (g_wl.rel_ptr != 0)
            g_wl.proxy_add_listener(g_wl.rel_ptr, g_wl_rel_ptr_listener, 0);
    }
}

static void wl_seat_capabilities(void *data, struct wl_proxy *seat, uint32_t caps)
{
    (void)data;
    const uint32_t seat_ver = g_wl.proxy_get_version(seat);
    if ((caps & WL_SEAT_CAPABILITY_POINTER) != 0 && g_wl.pointer == 0) {
        g_wl.pointer = g_wl.proxy_marshal_constructor(seat, 0u, g_wl.pointer_iface, NULL);
        if (g_wl.pointer != 0) {
            g_wl.proxy_add_listener(g_wl.pointer, g_wl_pointer_listener, 0);
            wl_bind_rel_pointer();
        }
    } else if ((caps & WL_SEAT_CAPABILITY_POINTER) == 0 && g_wl.pointer != 0) {
        wl_destroy_request(&g_wl.locked_ptr, 0u);
        wl_destroy_request(&g_wl.rel_ptr, 0u);
        if (seat_ver >= 3u)
            wl_destroy_request(&g_wl.pointer, 1u);
        else {
            g_wl.proxy_destroy(g_wl.pointer);
            g_wl.pointer = 0;
        }
        g_wl.grabbed = 0;
    }

    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) != 0 && g_wl.keyboard == 0) {
        g_wl.keyboard = g_wl.proxy_marshal_constructor(seat, 1u, g_wl.keyboard_iface, NULL);
        if (g_wl.keyboard != 0)
            g_wl.proxy_add_listener(g_wl.keyboard, g_wl_keyboard_listener, 0);
    } else if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) == 0 && g_wl.keyboard != 0) {
        if (seat_ver >= 3u)
            wl_destroy_request(&g_wl.keyboard, 0u);
        else {
            g_wl.proxy_destroy(g_wl.keyboard);
            g_wl.keyboard = 0;
        }
    }
}

static void wl_seat_name(void *data, struct wl_proxy *seat, const char *name)
{
    (void)data;
    (void)seat;
    (void)name;
}

static void (*g_wl_seat_listener[2])(void) = {
    (void (*)(void))wl_seat_capabilities,
    (void (*)(void))wl_seat_name,
};

static uint32_t min_u32(uint32_t a, uint32_t b) { return a < b ? a : b; }

static void wl_registry_global(void *data, struct wl_proxy *registry, uint32_t name, const char *iface,
                               uint32_t version)
{
    (void)data;
    if (strcmp(iface, "wl_compositor") == 0 && g_wl.compositor == 0) {
        g_wl.compositor = wl_registry_bind_obj(registry, name, g_wl.compositor_iface, min_u32(version, 4u));
    } else if (strcmp(iface, "wl_shm") == 0 && g_wl.shm == 0) {
        g_wl.shm = wl_registry_bind_obj(registry, name, g_wl.shm_iface, 1u);
    } else if (strcmp(iface, "wl_seat") == 0 && g_wl.seat == 0) {
        g_wl.seat = wl_registry_bind_obj(registry, name, g_wl.seat_iface, min_u32(version, 5u));
        if (g_wl.seat != 0)
            g_wl.proxy_add_listener(g_wl.seat, g_wl_seat_listener, 0);
    } else if (strcmp(iface, "xdg_wm_base") == 0 && g_wl.xdg_wm_base == 0) {
        g_wl.xdg_wm_base = wl_registry_bind_obj(registry, name, &g_xdg_wm_base_iface, min_u32(version, 4u));
        if (g_wl.xdg_wm_base != 0)
            g_wl.proxy_add_listener(g_wl.xdg_wm_base, g_wl_wm_base_listener, 0);
    } else if (strcmp(iface, "wp_viewporter") == 0 && g_wl.viewporter == 0) {
        g_wl.viewporter = wl_registry_bind_obj(registry, name, &g_wp_viewporter_iface, 1u);
    } else if (strcmp(iface, "zwp_relative_pointer_manager_v1") == 0 && g_wl.rel_ptr_mgr == 0) {
        g_wl.rel_ptr_mgr = wl_registry_bind_obj(registry, name, &g_zwp_rel_ptr_mgr_iface, 1u);
        if (g_wl.pointer != 0 && g_wl.rel_ptr == 0)
            wl_bind_rel_pointer();
    } else if (strcmp(iface, "zwp_pointer_constraints_v1") == 0 && g_wl.ptr_constraints == 0) {
        g_wl.ptr_constraints = wl_registry_bind_obj(registry, name, &g_zwp_ptr_constraints_iface, 1u);
    } else if (strcmp(iface, "zwp_idle_inhibit_manager_v1") == 0 && g_wl.idle_mgr == 0) {
        g_wl.idle_mgr = wl_registry_bind_obj(registry, name, &g_zwp_idle_mgr_iface, 1u);
    } else if (strcmp(iface, "zxdg_decoration_manager_v1") == 0 && g_wl.deco_mgr == 0) {
        g_wl.deco_mgr = wl_registry_bind_obj(registry, name, &g_zxdg_deco_mgr_iface, 1u);
    }
}

static void wl_registry_global_remove(void *data, struct wl_proxy *registry, uint32_t name)
{
    (void)data;
    (void)registry;
    (void)name;
}

static void (*g_wl_registry_listener[2])(void) = {
    (void (*)(void))wl_registry_global,
    (void (*)(void))wl_registry_global_remove,
};

static void wayland_close_unlocked(void)
{
    wl_destroy_request(&g_wl.locked_ptr, 0u);
    wl_destroy_request(&g_wl.rel_ptr, 0u);
    wl_destroy_request(&g_wl.idle_inhibitor, 0u);
    wl_destroy_request(&g_wl.toplevel_deco, 0u);
    wl_destroy_request(&g_wl.viewport, 0u);
    wl_destroy_request(&g_wl.xdg_toplevel, 0u);
    wl_destroy_request(&g_wl.xdg_surface, 0u);
    wl_destroy_request(&g_wl.surface, 0u);
    wl_destroy_request(&g_wl.cursor_surface, 0u);
    if (g_wl.cursor_theme != 0 && g_wl.cursor_theme_destroy != 0) {
        g_wl.cursor_theme_destroy(g_wl.cursor_theme);
        g_wl.cursor_theme = 0;
    }
    g_wl.cursor_image = 0;
    if (g_wl.pointer != 0) {
        if (g_wl.seat != 0 && g_wl.proxy_get_version(g_wl.seat) >= 3u)
            wl_destroy_request(&g_wl.pointer, 1u);
        else {
            g_wl.proxy_destroy(g_wl.pointer);
            g_wl.pointer = 0;
        }
    }
    if (g_wl.keyboard != 0) {
        if (g_wl.seat != 0 && g_wl.proxy_get_version(g_wl.seat) >= 3u)
            wl_destroy_request(&g_wl.keyboard, 0u);
        else {
            g_wl.proxy_destroy(g_wl.keyboard);
            g_wl.keyboard = 0;
        }
    }
    if (g_wl.seat != 0) {
        if (g_wl.proxy_get_version(g_wl.seat) >= 5u)
            wl_destroy_request(&g_wl.seat, 3u);
        else {
            g_wl.proxy_destroy(g_wl.seat);
            g_wl.seat = 0;
        }
    }
    wl_destroy_request(&g_wl.deco_mgr, 0u);
    wl_destroy_request(&g_wl.idle_mgr, 0u);
    wl_destroy_request(&g_wl.ptr_constraints, 0u);
    wl_destroy_request(&g_wl.rel_ptr_mgr, 0u);
    wl_destroy_request(&g_wl.viewporter, 0u);
    wl_destroy_request(&g_wl.xdg_wm_base, 0u);
    if (g_wl.shm != 0) {
        g_wl.proxy_destroy(g_wl.shm);
        g_wl.shm = 0;
    }
    if (g_wl.compositor != 0) {
        g_wl.proxy_destroy(g_wl.compositor);
        g_wl.compositor = 0;
    }
    if (g_wl.registry != 0) {
        g_wl.proxy_destroy(g_wl.registry);
        g_wl.registry = 0;
    }
    if (g_wl.display != 0) {
        g_wl.display_flush(g_wl.display);
        g_wl.display_disconnect(g_wl.display);
        __atomic_store_n(&g_wl.display, 0, __ATOMIC_RELEASE);
    }
    __atomic_store_n(&g_wl.surface, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_wl.focused, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_wl.open, 0, __ATOMIC_RELEASE);
}

static int wayland_window_open(const char *title, unsigned int width, unsigned int height)
{
    if (g_wl.open)
        return 1;
    if (!load_wayland())
        return 0;
    struct wl_display *display = g_wl.display_connect(0);
    if (display == 0)
        return 0;
    g_wl.display = display;
    // wl_display::get_registry (opcode 1)
    g_wl.registry = g_wl.proxy_marshal_constructor((struct wl_proxy *)display, 1u, g_wl.registry_iface, NULL);
    if (g_wl.registry == 0) {
        wayland_close_unlocked();
        return 0;
    }
    g_wl.proxy_add_listener(g_wl.registry, g_wl_registry_listener, 0);
    g_wl.display_roundtrip(display);
    g_wl.display_roundtrip(display); // receive initial seat capabilities

    if (g_wl.compositor == 0 || g_wl.xdg_wm_base == 0) {
        port_write_error("window: Wayland compositor lacks wl_compositor or xdg_wm_base\n");
        wayland_close_unlocked();
        return 0;
    }

    // wl_compositor::create_surface (opcode 0)
    struct wl_proxy *surface = g_wl.proxy_marshal_constructor(g_wl.compositor, 0u, g_wl.surface_iface, NULL);
    if (surface == 0) {
        wayland_close_unlocked();
        return 0;
    }
    __atomic_store_n(&g_wl.surface, surface, __ATOMIC_RELEASE);

    if (g_wl.viewporter != 0)
        g_wl.viewport = g_wl.proxy_marshal_constructor(g_wl.viewporter, 1u, &g_wp_viewport_iface, NULL, surface);
    if (g_wl.idle_mgr != 0)
        g_wl.idle_inhibitor =
            g_wl.proxy_marshal_constructor(g_wl.idle_mgr, 1u, &g_zwp_idle_inhibitor_iface, NULL, surface);

    if (g_wl.shm != 0 && g_wl.cursor_theme_load != 0 && g_wl.cursor_theme_get_cursor != 0) {
        g_wl.cursor_theme = g_wl.cursor_theme_load(0, 24, g_wl.shm);
        if (g_wl.cursor_theme != 0) {
            struct wl_cursor *cur = g_wl.cursor_theme_get_cursor(g_wl.cursor_theme, "left_ptr");
            if (cur == 0)
                cur = g_wl.cursor_theme_get_cursor(g_wl.cursor_theme, "default");
            if (cur != 0 && cur->image_count > 0) {
                g_wl.cursor_image = cur->images[0];
                g_wl.cursor_surface = g_wl.proxy_marshal_constructor(g_wl.compositor, 0u, g_wl.surface_iface, NULL);
            }
        }
    }

    // xdg_wm_base::get_xdg_surface (opcode 2), xdg_surface::get_toplevel (opcode 1)
    g_wl.xdg_surface = g_wl.proxy_marshal_constructor(g_wl.xdg_wm_base, 2u, &g_xdg_surface_iface, NULL, surface);
    if (g_wl.xdg_surface == 0) {
        wayland_close_unlocked();
        return 0;
    }
    g_wl.proxy_add_listener(g_wl.xdg_surface, g_wl_xdg_surface_listener, 0);

    g_wl.xdg_toplevel = g_wl.proxy_marshal_constructor(g_wl.xdg_surface, 1u, &g_xdg_toplevel_iface, NULL);
    if (g_wl.xdg_toplevel == 0) {
        wayland_close_unlocked();
        return 0;
    }
    g_wl.proxy_add_listener(g_wl.xdg_toplevel, g_wl_xdg_toplevel_listener, 0);

    // xdg_toplevel::set_title (opcode 2), set_app_id (opcode 3)
    const char *app_id = access("/.flatpak-info", F_OK) == 0 ? "io.github.summers_aaron.Halo3" : "halo3";
    g_wl.proxy_marshal(g_wl.xdg_toplevel, 2u, title != 0 ? title : "Halo 3");
    g_wl.proxy_marshal(g_wl.xdg_toplevel, 3u, app_id);

    if (g_wl.deco_mgr != 0) {
        g_wl.toplevel_deco =
            g_wl.proxy_marshal_constructor(g_wl.deco_mgr, 1u, &g_zxdg_toplevel_deco_iface, NULL, g_wl.xdg_toplevel);
        if (g_wl.toplevel_deco != 0)
            g_wl.proxy_add_listener(g_wl.toplevel_deco, g_wl_deco_listener, 0);
    }

    g_wl.req_width = width;
    g_wl.req_height = height;
    g_wl.width = width;
    g_wl.height = height;
    g_wl.configured = 0;
    g_wl.pending_width = g_wl.pending_height = 0;
    g_wl.has_pending_state = 0;
    g_wl.pointer_inside = 0;
    g_wl.grabbed = 0;
    g_wl.capture_seen = -1;
    g_wl.close_requested = 0;
    g_wl.guide = 0;
    g_wl.repeat_evdev = 0;
    g_wl.wheel_accum = 0.0;
    g_wl.motion_frac_x = g_wl.motion_frac_y = 0.0;
    __atomic_store_n(&g_wl.focused, 1, __ATOMIC_RELEASE);

    g_wl.mode = PORT_WINDOW_MODE_WINDOWED;
    g_wl.mode_revision = port_config_revision();
    wl_window_mode_apply(port_config_window_mode());

    // Initial commit without a buffer triggers the compositor's initial xdg_surface::configure.
    g_wl.proxy_marshal(surface, 6u);
    g_wl.display_roundtrip(display);
    wl_update_viewport();
    g_wl.display_flush(display);

    sleep_inhibit_begin(0);
    port_input_keyboard_attach(1);
    __atomic_store_n(&g_wl.open, 1, __ATOMIC_RELEASE);
    port_write_error("window: native Wayland surface opened\n");
    return 1;
}

static void wayland_poll_input(void)
{
    if (!g_wl.open || g_wl.display == 0)
        return;
    wl_guide_follow();
    const int capture = port_input_mouse_capture();
    if (capture != g_wl.capture_seen) {
        g_wl.capture_seen = capture;
        wl_mouse_grab(g_wl.focused && !g_wl.guide);
    }

    // Non-blocking read of pending Wayland socket events into the default queue, safe alongside Mesa's WSI queue.
    while (g_wl.display_prepare_read(g_wl.display) != 0) {
        if (g_wl.display_dispatch_pending(g_wl.display) < 0) {
            g_wl.close_requested = 1;
            return;
        }
    }
    g_wl.display_flush(g_wl.display);
    struct pollfd pfd;
    pfd.fd = g_wl.display_get_fd(g_wl.display);
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, 0) > 0 && (pfd.revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
        if (g_wl.display_read_events(g_wl.display) < 0) {
            g_wl.close_requested = 1;
            return;
        }
    } else {
        g_wl.display_cancel_read(g_wl.display);
    }
    if (g_wl.display_dispatch_pending(g_wl.display) < 0) {
        g_wl.close_requested = 1;
        return;
    }

    // Key repeat synthesis for the in-game Guide (Wayland compositors send press/release + repeat_info).
    if (g_wl.guide && g_wl.focused && g_wl.repeat_evdev != 0 && g_wl.repeat_rate > 0) {
        const long long now = monotonic_ms();
        if (now >= g_wl.repeat_next_ms) {
            const long long step = 1000ll / (long long)g_wl.repeat_rate;
            g_wl.repeat_next_ms = now + (step > 0 ? step : 33ll);
            wl_guide_key_event(g_wl.repeat_evdev, GUIDE_KEY_REPEATED);
        }
    }
    g_wl.display_flush(g_wl.display);
}

// =====================================================================================================================
// X11 Fallback Backend (libX11.so.6, runtime dlopen)
// =====================================================================================================================

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
#define X_ATOM_ATOM 4
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
#define XI_RAW_MOTION 17
#define XI_ALL_MASTER_DEVICES 1

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
        unsigned int detail;
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
    int (*XkbSetDetectableAutoRepeat)(x_display *, int, int *);
    int (*XChangeProperty)(x_display *, x_window, x_atom, x_atom, int, int, const unsigned char *, int);
    int (*XSendEvent)(x_display *, x_window, int, long, x_event *);
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
    int (*XResetScreenSaver)(x_display *);
    void *xi_library;
    int (*XIQueryVersion)(x_display *, int *, int *);
    int (*XISelectEvents)(x_display *, x_window, xi_event_mask *, int);
    int xi_opcode;
    unsigned long blank_cursor;
    int grabbed;
    int capture_seen;
    unsigned int width, height;
    int pointer_x, pointer_y, pointer_known, focused;
    int guide;
    unsigned char keys_down[32], keys_held_over[32];
    x_display *display;
    x_window window;
    x_atom delete_atom;
    int open;
    int close_requested;
    int mode;
    unsigned long mode_revision;
    long long last_ss_reset_sec;
} g_x;

static int load_x11(void)
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
    {
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

static void x11_raw_mouse_setup(void)
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

static void x11_mouse_grab(int grab)
{
    grab = grab && port_input_mouse_capture();
    if (grab == g_x.grabbed)
        return;
    if (grab) {
        if (g_x.XGrabPointer == 0 ||
            g_x.XGrabPointer(g_x.display, g_x.window, 1,
                             (unsigned int)(X_BUTTON_PRESS_MASK | X_BUTTON_RELEASE_MASK | X_POINTER_MOTION_MASK),
                             X_GRAB_MODE_ASYNC, X_GRAB_MODE_ASYNC, g_x.window, g_x.blank_cursor, 0) != X_GRAB_SUCCESS)
            return;
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

static void x11_raw_mouse_event(x_event *event)
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

static void x11_window_state(int on, x_atom first, x_atom second, x_atom state)
{
    x_event event;
    memset(&event, 0, sizeof(event));
    event.client.type = X_CLIENT_MESSAGE;
    event.client.window = g_x.window;
    event.client.message_type = state;
    event.client.format = 32;
    event.client.data[0] = on ? 1 : 0;
    event.client.data[1] = (long)first;
    event.client.data[2] = (long)second;
    event.client.data[3] = 1;
    const x_window root = g_x.XRootWindow(g_x.display, g_x.XDefaultScreen(g_x.display));
    g_x.XSendEvent(g_x.display, root, 0, X_SUBSTRUCTURE_REDIRECT_MASK | X_SUBSTRUCTURE_NOTIFY_MASK, &event);
}

static void x11_window_mode_apply(int mode, int mapped)
{
    if (g_x.XChangeProperty == 0 || g_x.XSendEvent == 0)
        return;
    const x_atom state = g_x.XInternAtom(g_x.display, "_NET_WM_STATE", 0);
    const x_atom fullscreen = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_FULLSCREEN", 0);
    const x_atom vertical = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_MAXIMIZED_VERT", 0);
    const x_atom horizontal = g_x.XInternAtom(g_x.display, "_NET_WM_STATE_MAXIMIZED_HORZ", 0);
    const x_atom motif = g_x.XInternAtom(g_x.display, "_MOTIF_WM_HINTS", 0);
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
        if (mode != PORT_WINDOW_MODE_FULLSCREEN)
            x11_window_state(0, fullscreen, 0, state);
        if (mode != PORT_WINDOW_MODE_BORDERLESS)
            x11_window_state(0, vertical, horizontal, state);
        if (mode == PORT_WINDOW_MODE_FULLSCREEN)
            x11_window_state(1, fullscreen, 0, state);
        if (mode == PORT_WINDOW_MODE_BORDERLESS)
            x11_window_state(1, vertical, horizontal, state);
    }
    g_x.XFlush(g_x.display);
    g_x.mode = mode;
}

static void x11_window_mode_follow(void)
{
    const unsigned long revision = port_config_revision();
    if (revision == g_x.mode_revision)
        return;
    g_x.mode_revision = revision;
    const int mode = port_config_window_mode();
    if (mode != g_x.mode)
        x11_window_mode_apply(mode, 1);
}

static void x11_sleep_inhibit_pump(void)
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

static int x11_window_open(const char *title, unsigned int width, unsigned int height)
{
    if (g_x.open)
        return 1;
    if (!load_x11())
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
    g_x.mode = PORT_WINDOW_MODE_WINDOWED;
    g_x.mode_revision = port_config_revision();
    const int mode = port_config_window_mode();
    if (mode != PORT_WINDOW_MODE_WINDOWED)
        x11_window_mode_apply(mode, 0);
    g_x.XMapWindow(g_x.display, g_x.window);
    g_x.XFlush(g_x.display);
    g_x.grabbed = 0;
    g_x.capture_seen = -1;
    g_x.close_requested = 0;
    g_x.last_ss_reset_sec = 0;
    if (g_x.XResetScreenSaver != 0)
        g_x.XResetScreenSaver(g_x.display);
    x11_raw_mouse_setup();
    sleep_inhibit_begin((uint32_t)window);
    port_input_keyboard_attach(1);
    __atomic_store_n(&g_x.open, 1, __ATOMIC_RELEASE);
    return 1;
}

static enum e_port_key key_of_x11(unsigned long keysym)
{
    if (keysym >= 0x61 && keysym <= 0x7a)
        return (enum e_port_key)(k_port_key_a + (int)(keysym - 0x61));
    if (keysym >= 0x30 && keysym <= 0x39)
        return (enum e_port_key)(k_port_key_0 + (int)(keysym - 0x30));
    if (keysym >= 0xffbe && keysym <= 0xffc9)
        return (enum e_port_key)(k_port_key_f1 + (int)(keysym - 0xffbe));
    switch (keysym) {
    case 0xff1b: return k_port_key_escape;
    case 0xff09: case 0xfe20: return k_port_key_tab;
    case 0x20: return k_port_key_space;
    case 0xff0d: case 0xff8d: return k_port_key_return;
    case 0xff08: return k_port_key_backspace;
    case 0xff52: return k_port_key_up;
    case 0xff54: return k_port_key_down;
    case 0xff51: return k_port_key_left;
    case 0xff53: return k_port_key_right;
    case 0xffe1: return k_port_key_lshift;
    case 0xffe2: return k_port_key_rshift;
    case 0xffe3: return k_port_key_lctrl;
    case 0xffe4: return k_port_key_rctrl;
    case 0xffe9: case 0xffe7: return k_port_key_lalt;
    case 0xffea: case 0xffe8: case 0xfe03: return k_port_key_ralt;
    case 0xffe5: return k_port_key_capslock;
    case 0xff50: return k_port_key_home;
    case 0xff57: return k_port_key_end;
    case 0xff63: return k_port_key_insert;
    case 0xffff: return k_port_key_delete;
    case 0xff55: return k_port_key_pageup;
    case 0xff56: return k_port_key_pagedown;
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

static int x11_button_code(unsigned int button)
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

static void x11_guide_follow(void)
{
    const int guide = guide_ingame_input();
    if (guide == g_x.guide)
        return;
    g_x.guide = guide;
    if (!guide)
        memcpy(g_x.keys_held_over, g_x.keys_down, sizeof(g_x.keys_held_over));
    port_input_release_all();
    g_x.pointer_known = 0;
    x11_mouse_grab(!guide && g_x.focused);
}

static void x11_guide_key_event(x_event *event, int action)
{
    char typed[8];
    unsigned long keysym = 0, character = 0;
    const int count = g_x.XLookupString(event, typed, (int)sizeof(typed) - 1, &keysym, 0);
    const int key = guide_host_x11_key(keysym, event->input.state, typed, count, &character);
    if (key >= 0)
        guide_ingame_key_action(key, character, action);
}

static void x11_handle_input(x_event *event)
{
    x11_guide_follow();
    switch (event->type) {
    case X_KEY_PRESS:
    case X_KEY_RELEASE: {
        const unsigned int keycode = event->input.detail;
        const int press = event->type == X_KEY_PRESS, repeat = press && key_bit(g_x.keys_down, keycode);
        key_bit_set(g_x.keys_down, keycode, press);
        if (!press)
            key_bit_set(g_x.keys_held_over, keycode, 0);
        if (g_x.guide && press && !repeat && guide_capture(key_of_x11(g_x.XLookupKeysym(event, 0))))
            break;
        if (g_x.guide)
            x11_guide_key_event(event, !press ? GUIDE_KEY_RELEASED : repeat ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
        if (!g_x.guide || !press) {
            if (press && key_bit(g_x.keys_held_over, keycode))
                break;
            port_input_key(key_of_x11(g_x.XLookupKeysym(event, 0)), press);
        }
        break;
    }
    case X_BUTTON_PRESS:
    case X_BUTTON_RELEASE: {
        const unsigned int button = event->input.detail;
        port_input_mouse_position(event->input.x, event->input.y, (int)g_x.width, (int)g_x.height);
        const int code = x11_button_code(button);
        if (event->type == X_BUTTON_PRESS && g_x.guide) {
            if (code != PORT_KBM_NONE && guide_capture(code))
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
            x11_mouse_grab(1);
        if (code == PORT_KBM_WHEEL_UP || code == PORT_KBM_WHEEL_DOWN) {
            if (event->type == X_BUTTON_PRESS)
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
        if (g_x.guide) {
            guide_ingame_pointer(GUIDE_POINTER_MOVE, 0, x, y, (int)g_x.width, (int)g_x.height);
            break;
        }
        if (g_x.pointer_known && !(port_input_mouse_capture() && x == cx && y == cy))
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
        x11_mouse_grab(!g_x.guide);
        break;
    case X_EXPOSE:
        if (g_x.focused && !g_x.guide)
            x11_mouse_grab(1);
        break;
    case X_CONFIGURE_NOTIFY:
        if (event->configure.width > 0 && event->configure.height > 0) {
            g_x.width = (unsigned int)event->configure.width;
            g_x.height = (unsigned int)event->configure.height;
        }
        break;
    case X_FOCUS_OUT:
        __atomic_store_n(&g_x.focused, 0, __ATOMIC_RELEASE);
        x11_mouse_grab(0);
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

static void x11_poll_input(void)
{
    if (!g_x.open)
        return;
    x11_guide_follow();
    const int capture = port_input_mouse_capture();
    if (capture != g_x.capture_seen) {
        g_x.capture_seen = capture;
        x11_mouse_grab(g_x.focused && !g_x.guide);
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
            x11_raw_mouse_event(&event);
        else
            x11_handle_input(&event);
    }
}

// =====================================================================================================================
// Public port_window API (dispatches to Wayland or Xlib)
// =====================================================================================================================

int port_window_system(void)
{
    const int active = __atomic_load_n(&g_active_system, __ATOMIC_ACQUIRE);
    if (active != PORT_WINDOW_SYSTEM_NONE)
        return active;
    const char *pref = getenv("HALO3_WINDOW_BACKEND");
    if (pref != 0 && (text_eq_ci(pref, "x11") || text_eq_ci(pref, "xlib")))
        return PORT_WINDOW_SYSTEM_XLIB;
    if ((pref != 0 && text_eq_ci(pref, "wayland")) ||
        (getenv("WAYLAND_DISPLAY") != 0 && getenv("WAYLAND_DISPLAY")[0] != 0))
        return PORT_WINDOW_SYSTEM_WAYLAND;
    return PORT_WINDOW_SYSTEM_XLIB;
}

int port_window_open(const char *title, unsigned int width, unsigned int height)
{
    port_window_input_lock();
    if (g_wl.open || g_x.open) {
        port_window_input_unlock();
        return 1;
    }
    const char *pref = getenv("HALO3_WINDOW_BACKEND");
    const int force_wayland = pref != 0 && text_eq_ci(pref, "wayland");
    const int force_x11 = pref != 0 && (text_eq_ci(pref, "x11") || text_eq_ci(pref, "xlib"));
    const char *wl_env = getenv("WAYLAND_DISPLAY");
    const int want_wayland = !force_x11 && (force_wayland || (wl_env != 0 && wl_env[0] != 0));

    int opened = 0;
    if (want_wayland && wayland_window_open(title, width, height)) {
        __atomic_store_n(&g_active_system, PORT_WINDOW_SYSTEM_WAYLAND, __ATOMIC_RELEASE);
        opened = 1;
    } else if (!force_wayland && x11_window_open(title, width, height)) {
        __atomic_store_n(&g_active_system, PORT_WINDOW_SYSTEM_XLIB, __ATOMIC_RELEASE);
        opened = 1;
    } else if (force_wayland) {
        port_write_error("window: cannot open native Wayland window (HALO3_WINDOW_BACKEND=wayland)\n");
    }
    port_window_input_unlock();
    return opened;
}

void *port_window_native_display(void)
{
    if (__atomic_load_n(&g_active_system, __ATOMIC_ACQUIRE) == PORT_WINDOW_SYSTEM_WAYLAND)
        return __atomic_load_n(&g_wl.display, __ATOMIC_ACQUIRE);
    return __atomic_load_n(&g_x.display, __ATOMIC_ACQUIRE);
}

__UINTPTR_TYPE__ port_window_native_window(void)
{
    if (__atomic_load_n(&g_active_system, __ATOMIC_ACQUIRE) == PORT_WINDOW_SYSTEM_WAYLAND)
        return (__UINTPTR_TYPE__)__atomic_load_n(&g_wl.surface, __ATOMIC_ACQUIRE);
    return (__UINTPTR_TYPE__)__atomic_load_n(&g_x.window, __ATOMIC_ACQUIRE);
}

int port_window_key_code(__UINTPTR_TYPE__ host_key, __INTPTR_TYPE__ host_flags)
{
    if (host_flags == 1)
        return key_of_evdev((unsigned int)host_key);
    return key_of_x11((unsigned long)host_key);
}

void port_window_cursor_mode_changed(void)
{
    if (g_wl.open) {
        g_wl.motion_frac_x = g_wl.motion_frac_y = 0.0;
        g_wl.capture_seen = port_input_mouse_capture();
        wl_mouse_grab(g_wl.focused && !g_wl.guide);
        g_wl.display_flush(g_wl.display);
        return;
    }
    if (!g_x.open)
        return;
    g_x.pointer_known = 0;
    g_x.capture_seen = port_input_mouse_capture();
    x11_mouse_grab(g_x.focused && !g_x.guide);
    g_x.XFlush(g_x.display);
}

void port_window_poll_input(void)
{
    if (g_wl.open)
        wayland_poll_input();
    else if (g_x.open)
        x11_poll_input();
}

int port_window_pump(void)
{
    port_window_input_lock();
    int keep_open = 0;
    if (g_wl.open) {
        wl_window_mode_follow();
        wayland_poll_input();
        keep_open = g_wl.open && !g_wl.close_requested;
    } else if (g_x.open) {
        x11_window_mode_follow();
        x11_sleep_inhibit_pump();
        x11_poll_input();
        keep_open = g_x.open && !g_x.close_requested;
    }
    port_window_input_unlock();
    return keep_open;
}

int port_window_is_focused(void)
{
    if (__atomic_load_n(&g_wl.open, __ATOMIC_ACQUIRE))
        return __atomic_load_n(&g_wl.focused, __ATOMIC_ACQUIRE);
    return !__atomic_load_n(&g_x.open, __ATOMIC_ACQUIRE) || __atomic_load_n(&g_x.focused, __ATOMIC_ACQUIRE);
}

void port_window_close(void)
{
    port_window_input_lock();
    if (g_wl.open) {
        port_input_keyboard_attach(0);
        sleep_inhibit_end();
        wayland_close_unlocked();
        __atomic_store_n(&g_active_system, PORT_WINDOW_SYSTEM_NONE, __ATOMIC_RELEASE);
    } else if (g_x.open) {
        port_input_keyboard_attach(0);
        sleep_inhibit_end();
        x11_mouse_grab(0);
        g_x.XDestroyWindow(g_x.display, g_x.window);
        g_x.XCloseDisplay(g_x.display);
        __atomic_store_n(&g_x.display, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.window, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.focused, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_x.open, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_active_system, PORT_WINDOW_SYSTEM_NONE, __ATOMIC_RELEASE);
    }
    port_window_input_unlock();
}
