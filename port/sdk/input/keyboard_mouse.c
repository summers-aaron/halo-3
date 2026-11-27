// port/sdk/input (both hosts): the game window's keyboard and mouse (port_input.h): in play, the game actions of their
// bindings (kbm_bindings.h, from halo3.ini through port_config.h); in the menus, a controller in a fixed layout. The
// game poll drains window input before reading it. The window lock keeps dispatch, focus resets and sampling ordered
// when the render thread also pumps events.
#include "port_input.h"
#include "../runtime/port_config.h"
#include "../xam/guide.h"
#include "../xam/guide_ingame.h"
#include "../xam/xam_ui.h"

extern char *getenv(const char *name);
extern PORT_MS_ABI unsigned long GetTickCount(void);
PORT_MS_ABI void port_write_error(const char *text);

// The host window implementations share this lock with the input sampler. Poll with it held.
void port_window_input_lock(void);
void port_window_input_unlock(void);
void port_window_poll_input(void);
void port_window_cursor_mode_changed(void);

// ---- what the window reports ----

static volatile unsigned char g_keys[k_port_key_count];
static volatile unsigned char g_buttons[k_port_mouse_button_count];
// Keys and mouse buttons that went down since the last poll (port_input_keyboard_state), so a tap released before it is
// still reported once (port_input.h: taps between two polls). Set during dispatch, taken by the poll.
static volatile unsigned char g_key_presses[k_port_key_count];
static volatile unsigned char g_button_presses[k_port_mouse_button_count];
// Wheel notches up / down not given to a poll yet: each is a press of one poll, and a few that come faster than the
// polls wait for the next ones (more are dropped: a flick of the wheel is a few presses, not a queue of them).
#define KBM_WHEEL_QUEUE 3
static volatile int g_wheel_notches[2];
static volatile int g_mouse_dx, g_mouse_dy; // accumulated since the last poll
// Raw mouse look: raw device counts in 1/256 count units since the last port_input_mouse_look, and whether a raw source
// (XInput2) reports at all (else the pointer motion above stands in).
#define PORT_RAW_MOUSE_FRACTION 256.0
static volatile int g_raw_dx, g_raw_dy;
static volatile int g_raw_source;
static double g_raw_residual_x, g_raw_residual_y; // part of a unit retained across window events (no drift)
static volatile unsigned long g_raw_last_motion; // GetTickCount of the last raw look that turned
static volatile unsigned long g_raw_last_poll;   // GetTickCount of the last port_input_mouse_look call (for accel dt)
static volatile int g_raw_moved;               // a raw look turned at least once
static volatile int g_attached;
static volatile unsigned g_report_context = PORT_KBM_CONTEXT_FOOT;
static volatile int g_report_zoomed;

// ---- settings (port_config.h, live from the Guide), read again whenever they change ----

static volatile unsigned long g_settings_revision = ~0ul;
static volatile int g_setting_raw, g_setting_invert, g_setting_crouch_hold;
static volatile int g_setting_sensitivity;         // hundredths
static volatile int g_setting_acceleration;        // hundredths (0 = off)
static volatile int g_setting_zoom_sensitivity;    // hundredths
static volatile int g_setting_vehicle_sensitivity; // hundredths
static int g_stick_units = -1;             // HALO3_MOUSE_SENSITIVITY: stick units per pixel at sensitivity 1.0
static int g_capture_env = -1;             // HALO3_MOUSE_CAPTURE=1

static int env_int(const char *name, int fallback)
{
    const char *text = getenv(name);
    if (text == 0 || *text == 0)
        return fallback;
    int value = 0;
    for (; *text >= '0' && *text <= '9'; text++)
        value = value * 10 + (*text - '0');
    return *text == 0 ? value : fallback;
}

// Any thread (the window's and the game's): a refresh racing another writes the same values.
static void settings_refresh(void)
{
    const unsigned long revision = port_config_revision();
    if (revision == __atomic_load_n(&g_settings_revision, __ATOMIC_ACQUIRE))
        return;
    __atomic_store_n(&g_setting_raw, port_config_raw_mouse(), __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_invert, port_config_invert_mouse_y(), __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_crouch_hold, port_config_crouch_hold(), __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_sensitivity, (int)(port_config_mouse_sensitivity() * 100.0f + 0.5f), __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_acceleration, (int)(port_config_mouse_acceleration() * 100.0f + 0.5f),
                     __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_zoom_sensitivity, (int)(port_config_mouse_zoom_sensitivity() * 100.0f + 0.5f),
                     __ATOMIC_RELAXED);
    __atomic_store_n(&g_setting_vehicle_sensitivity, (int)(port_config_mouse_vehicle_sensitivity() * 100.0f + 0.5f),
                     __ATOMIC_RELAXED);
    __atomic_store_n(&g_settings_revision, revision, __ATOMIC_RELEASE);
}

static float sensitivity(void)
{
    return (float)__atomic_load_n(&g_setting_sensitivity, __ATOMIC_RELAXED) / 100.0f;
}

static float mouse_acceleration(void)
{
    return (float)__atomic_load_n(&g_setting_acceleration, __ATOMIC_RELAXED) / 100.0f;
}

static float mouse_zoom_sensitivity(void)
{
    return (float)__atomic_load_n(&g_setting_zoom_sensitivity, __ATOMIC_RELAXED) / 100.0f;
}

static float mouse_vehicle_sensitivity(void)
{
    return (float)__atomic_load_n(&g_setting_vehicle_sensitivity, __ATOMIC_RELAXED) / 100.0f;
}

// Self-contained square root (no libm dependency).
static double sqrt_approx(double x)
{
    if (x <= 0.0)
        return 0.0;
    double r = x > 1.0 ? x * 0.5 : 1.0;
    for (int i = 0; i < 20; i++)
        r = 0.5 * (r + x / r);
    return r;
}

// Effective mouse sensitivity for gameplay look: base sensitivity, optional velocity-based acceleration (0.0 is an exact
// no-op), and zoom / vehicle seat multipliers.
static float mouse_look_scale(double dx, double dy, unsigned long dt_ms)
{
    float scale = sensitivity();
    const float accel = mouse_acceleration();
    if (accel > 0.0f) {
        const double dt = dt_ms < 1ul ? 1.0 : dt_ms > 100ul ? 16.0 : (double)dt_ms;
        const double speed = sqrt_approx(dx * dx + dy * dy) / dt;
        double gain = 1.0 + (double)accel * speed * 0.1;
        if (gain > 6.0)
            gain = 6.0;
        scale *= (float)gain;
    }
    if (__atomic_load_n(&g_report_zoomed, __ATOMIC_RELAXED))
        scale *= mouse_zoom_sensitivity();
    const unsigned context = __atomic_load_n(&g_report_context, __ATOMIC_RELAXED);
    if ((context & (PORT_KBM_CONTEXT_SEAT | PORT_KBM_CONTEXT_SEAT_ARMED)) != 0)
        scale *= mouse_vehicle_sensitivity();
    return scale;
}

// ---- the window's side ----

#define PORT_INPUT_MOUSE_SOURCE_THRESHOLD 8
static volatile int g_last_source_is_kbm;

// Window dispatch and the title's event drain share the window input lock. A button
// keeps its own position even if the mouse moves again between two UI updates.
static int g_ui_cursor;
static port_ui_mouse_event g_pointer;
static port_ui_mouse_event g_ui_events[32];
static unsigned g_ui_head, g_ui_tail;
static unsigned char g_mouse_held[k_port_mouse_button_count];

static void queue_ui_event(enum port_ui_mouse_action action)
{
    unsigned slot = g_ui_head;
    const unsigned capacity = sizeof(g_ui_events) / sizeof(g_ui_events[0]);
    if (action == port_ui_mouse_move && slot != g_ui_tail &&
        g_ui_events[(slot - 1) % capacity].action == port_ui_mouse_move)
        --slot;
    else if (slot - g_ui_tail == capacity)
        return;
    else
        ++g_ui_head;
    g_ui_events[slot % capacity] = g_pointer;
    g_ui_events[slot % capacity].action = action;
}

PORT_MS_ABI void port_input_mouse_position(int x, int y, int win_w, int win_h)
{
    const int moved = g_pointer.width == 0 || x != g_pointer.x || y != g_pointer.y;
    g_pointer.x = x;
    g_pointer.y = y;
    g_pointer.width = win_w;
    g_pointer.height = win_h;
    if (moved && g_ui_cursor)
        queue_ui_event(port_ui_mouse_move);
}

PORT_MS_ABI int port_input_next_ui_mouse_event(port_ui_mouse_event *event)
{
    port_window_input_lock();
    const int available = g_ui_head != g_ui_tail;
    if (available)
        *event = g_ui_events[g_ui_tail++ % (sizeof(g_ui_events) / sizeof(g_ui_events[0]))];
    port_window_input_unlock();
    return available;
}

static void clear_mouse_motion(void)
{
    __atomic_store_n(&g_mouse_dx, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_mouse_dy, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_raw_dx, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_raw_dy, 0, __ATOMIC_RELAXED);
    g_raw_residual_x = g_raw_residual_y = 0.0;
    __atomic_store_n(&g_raw_moved, 0, __ATOMIC_RELAXED);
}

PORT_MS_ABI void port_input_set_ui_cursor_mode(int active)
{
    if (__atomic_load_n(&g_ui_cursor, __ATOMIC_ACQUIRE) == !!active)
        return;
    port_window_input_lock();
    port_window_poll_input(); // drain events belonging to the previous role
    __atomic_store_n(&g_ui_cursor, !!active, __ATOMIC_RELEASE);
    g_ui_tail = g_ui_head;
    for (int i = 0; i < k_port_mouse_button_count; ++i)
        g_buttons[i] = g_button_presses[i] = 0;
    __atomic_store_n(&g_wheel_notches[0], 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_wheel_notches[1], 0, __ATOMIC_RELAXED);
    clear_mouse_motion();
    port_window_cursor_mode_changed();
    port_window_input_unlock();
}

PORT_MS_ABI void port_input_keyboard_attach(int attached)
{
    port_input_release_all();
    __atomic_store_n(&g_attached, attached != 0, __ATOMIC_RELEASE);
}

PORT_MS_ABI void port_input_key(enum e_port_key key, int down)
{
    if (key > k_port_key_none && key < k_port_key_count) {
        if (down)
            __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
        if (down && !g_keys[key]) // a press, not a held key's auto-repeat
            __atomic_store_n(&g_key_presses[key], 1, __ATOMIC_RELEASE);
        g_keys[key] = down != 0;
    }
}

PORT_MS_ABI void port_input_mouse_button(enum e_port_mouse_button button, int down)
{
    if ((unsigned)button < k_port_mouse_button_count) {
        if (g_mouse_held[button] == !!down)
            return;
        g_mouse_held[button] = down != 0;
        if (down)
            __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
        if (g_ui_cursor) {
            if (down && button == k_port_mouse_left)
                queue_ui_event(port_ui_mouse_choose);
            if (down && button == k_port_mouse_right)
                queue_ui_event(port_ui_mouse_back);
            return;
        }
        if (down)
            __atomic_store_n(&g_button_presses[button], 1, __ATOMIC_RELEASE);
        g_buttons[button] = down != 0;
    }
}

PORT_MS_ABI void port_input_mouse_wheel(int notches)
{
    if (notches == 0)
        return;
    __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
    if (g_ui_cursor) {
        // Bound a single wheel burst just as the gameplay poll does.
        for (int i = 0; i < KBM_WHEEL_QUEUE && notches != 0; ++i) {
            queue_ui_event(notches > 0 ? port_ui_mouse_up : port_ui_mouse_down);
            notches += notches > 0 ? -1 : 1;
        }
        return;
    }
    volatile int *queue = &g_wheel_notches[notches > 0 ? 0 : 1];
    int count = notches > 0 ? notches : -notches;
    int now = __atomic_load_n(queue, __ATOMIC_RELAXED);
    while (count > 0 && now < KBM_WHEEL_QUEUE) {
        if (__atomic_compare_exchange_n(queue, &now, now + 1, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
            count--;
    }
}

PORT_MS_ABI void port_input_mouse_motion(int dx, int dy)
{
    const int adx = dx < 0 ? -dx : dx, ady = dy < 0 ? -dy : dy;
    if (adx + ady >= PORT_INPUT_MOUSE_SOURCE_THRESHOLD)
        __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
    if (g_ui_cursor)
        return;
    __atomic_fetch_add(&g_mouse_dx, dx, __ATOMIC_RELAXED);
    __atomic_fetch_add(&g_mouse_dy, dy, __ATOMIC_RELAXED);
}

static unsigned char g_masked[PORT_KBM_CODE_COUNT]; // the poll's: held across a change of role, ignored until let go
static volatile int g_release_masks;                  // port_input_release_all asked the poll to drop them

PORT_MS_ABI void port_input_release_all(void)
{
    for (int i = 0; i < k_port_key_count; i++)
        g_keys[i] = g_key_presses[i] = 0;
    for (int i = 0; i < k_port_mouse_button_count; i++)
        g_buttons[i] = g_button_presses[i] = 0;
    __atomic_store_n(&g_wheel_notches[0], 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_wheel_notches[1], 0, __ATOMIC_RELAXED);
    clear_mouse_motion();
    __builtin_memset(g_mouse_held, 0, sizeof(g_mouse_held));
    g_pointer.width = 0;
    g_ui_tail = g_ui_head;
    __atomic_store_n(&g_release_masks, 1, __ATOMIC_RELEASE);
}

int port_input_mouse_capture(void)
{
    if (g_capture_env < 0)
        g_capture_env = env_int("HALO3_MOUSE_CAPTURE", 0) == 1;
    return !g_ui_cursor && !guide_showing() && !port_xam_ui_showing() && (g_capture_env || port_input_mouse_raw());
}

PORT_MS_ABI int port_input_mouse_raw(void)
{
    settings_refresh();
    return !__atomic_load_n(&g_ui_cursor, __ATOMIC_ACQUIRE) && __atomic_load_n(&g_setting_raw, __ATOMIC_RELAXED);
}

PORT_MS_ABI void port_input_mouse_raw_motion(double dx, double dy)
{
    if (g_ui_cursor)
        return;
    __atomic_store_n(&g_raw_source, 1, __ATOMIC_RELAXED);
    // a device reports a few counts an event; anything beyond 2^20 is a bad valuator, not motion (and would not fit)
    const double limit = 1048576.0;
    dx = dx > limit ? limit : dx < -limit ? -limit : dx;
    dy = dy > limit ? limit : dy < -limit ? -limit : dy;
    const double x = dx * PORT_RAW_MOUSE_FRACTION + g_raw_residual_x;
    const double y = dy * PORT_RAW_MOUSE_FRACTION + g_raw_residual_y;
    const int units_x = (int)x, units_y = (int)y;
    g_raw_residual_x = x - units_x;
    g_raw_residual_y = y - units_y;
    __atomic_fetch_add(&g_raw_dx, units_x, __ATOMIC_RELAXED);
    __atomic_fetch_add(&g_raw_dy, units_y, __ATOMIC_RELAXED);
}

static int script_active(void);

// A keyboard is there: the window's, or the input script's (headless runs).
static int keyboard_present(void) { return __atomic_load_n(&g_attached, __ATOMIC_ACQUIRE) || script_active(); }

PORT_MS_ABI int port_input_mouse_aiming(unsigned int user)
{
    return user == 0 && port_input_mouse_raw() && keyboard_present() &&
           __atomic_load_n(&g_raw_moved, __ATOMIC_RELAXED) &&
           GetTickCount() - __atomic_load_n(&g_raw_last_motion, __ATOMIC_RELAXED) < 500u;
}

PORT_MS_ABI int port_input_mouse_look(unsigned int user, float *right_radians, float *up_radians)
{
    *right_radians = 0.0f;
    *up_radians = 0.0f;
    if (user != 0 || !port_input_mouse_raw())
        return 0;
    port_window_input_lock();
    port_window_poll_input();
    if (!port_input_mouse_raw() || !keyboard_present()) {
        port_window_input_unlock();
        return 0;
    }
    double dx, dy;
    if (__atomic_load_n(&g_raw_source, __ATOMIC_RELAXED)) {
        dx = __atomic_exchange_n(&g_raw_dx, 0, __ATOMIC_RELAXED) / PORT_RAW_MOUSE_FRACTION;
        dy = __atomic_exchange_n(&g_raw_dy, 0, __ATOMIC_RELAXED) / PORT_RAW_MOUSE_FRACTION;
        // pointer motion stays unused while the raw source reports
        __atomic_store_n(&g_mouse_dx, 0, __ATOMIC_RELAXED);
        __atomic_store_n(&g_mouse_dy, 0, __ATOMIC_RELAXED);
    } else {
        dx = __atomic_exchange_n(&g_mouse_dx, 0, __ATOMIC_RELAXED);
        dy = __atomic_exchange_n(&g_mouse_dy, 0, __ATOMIC_RELAXED);
    }
    port_window_input_unlock();
    const unsigned long now = GetTickCount();
    const unsigned long prev_poll = __atomic_exchange_n(&g_raw_last_poll, now, __ATOMIC_RELAXED);
    if (dx == 0.0 && dy == 0.0)
        return 0;
    const double adx = dx < 0.0 ? -dx : dx, ady = dy < 0.0 ? -dy : dy;
    if (adx + ady >= (double)PORT_INPUT_MOUSE_SOURCE_THRESHOLD)
        __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
    const unsigned long dt_ms = prev_poll != 0 ? now - prev_poll : 16ul;
    __atomic_store_n(&g_raw_last_motion, now, __ATOMIC_RELAXED);
    __atomic_store_n(&g_raw_moved, 1, __ATOMIC_RELAXED);
    // 0.022 degrees a count at sensitivity 1.0 (Quake / Source m_yaw)
    const float radians_per_count = mouse_look_scale(dx, dy, dt_ms) * 0.022f * 0.017453292f;
    *right_radians = (float)(dx * radians_per_count);
    *up_radians = (float)(-dy * radians_per_count); // screen y grows downwards
    if (__atomic_load_n(&g_setting_invert, __ATOMIC_RELAXED))
        *up_radians = -*up_radians;
    return 1;
}


// ---- scripted input (HALO3_INPUT_SCRIPT) ----
//
// HALO3_INPUT_SCRIPT="<ms>:<key>[@<controller>][:<hold ms>],...": presses of a scripted gamepad at fixed times
// (milliseconds after the first poll of a controller), each held 150 ms unless the entry says otherwise (a press
// shorter than the time between two polls is still seen by the next one: port_input.h, taps between two polls). The
// buttons are named as the keys of the port's first keyboard layout, the script's own (k_script_layout below; the
// keyboard itself is bound with kbm_bindings.h now): up, down, left, right (D-pad), space (A), return (Start),
// backspace and escape (B), e (X), 1 (Y), q (left shoulder), f and v (right shoulder), tab and f1 (Back), control and c
// (left thumb), z (right thumb), w, a, s, d (left stick), i, j, k, l (right stick), g (left trigger), home (the Guide
// button), plus rt (right trigger) and lt (left trigger). An entry presses the button on controller 0 unless it names
// another, 0..3 ("space@1" is controller 1's A). A script makes the keyboard source present without a window, so a
// headless run has a connected controller 0 (an unplugged controller 0 pauses a campaign, as on the console) and can
// walk the menus; each other controller the script names is connected too, for split-screen runs (a signed-in
// player's unplugged controller pauses a campaign as well). The script's gamepad goes through the profile's button
// preset like any controller.
//
// Window entries "<ms>:<what>=<value>" are the game window's own events instead: delivered once each, in order, at the
// first poll at or after their time, through the same entries the game window's keys and mouse take (window_linux.c,
// window_windows.c), so headless runs operate the window as a keyboard and mouse would:
//   key=<name>[:<ms>]   (the in-game guide, guide_ingame.h) a key pressed, held <ms> (default 100: less than the menus'
//                       hold delay, so one step) and let go: up, down, left, right, pageup, pagedown, enter, space,
//                       escape, backspace, tab (RB), shifttab (LB), home (the Guide button), paste (Ctrl+V); an arrow
//                       held longer repeats as Halo 3's menus do (port/sdk/xam/guide.h)
//   text=<characters>   (the in-game guide) each character typed (up to the next ','; at most 191: a lobby code)
//   move=<x>x<y>        (the in-game guide) the pointer moved to (x, y): pixels of the game's frame (the window's
//                       client area when the window is the frame's size)
//   click=<x>x<y>, rclick=<x>x<y>, wheelup=<x>x<y>, wheeldown=<x>x<y>
//                       (the in-game guide) a left / right button press or a wheel notch there
//   kbm=<input>[:<ms>]  a key or mouse button of the keyboard and mouse, by its binding name (kbm_bindings.h: space,
//                       f, lctrl, mouse_left, mouse4, wheel_up, ...), pressed and held <ms> (default 150; a wheel
//                       notch is a press of one poll): the game's while the in-game guide does not show, the
//                       Keyboard & Mouse page's capture while it waits for one (guide_capture), else nothing
//   mouse=<dx>x<dy>     the mouse moved by (dx, dy) counts (either may be negative): the game's look
#define PORT_INPUT_SCRIPT_MAX 64
#define PORT_INPUT_SCRIPT_WINDOW_MAX 64
#define PORT_INPUT_SCRIPT_TEXT 192 // a lobby code typed in one entry

// Script inputs: the codes of kbm_bindings.h by name, then the triggers.
enum { k_script_right_trigger = PORT_KBM_CODE_COUNT, k_script_left_trigger, k_script_input_count };
typedef char script_inputs_fit[k_script_input_count <= 256 ? 1 : -1];

// Window entries' kinds.
enum { k_window_key, k_window_text, k_window_move, k_window_click, k_window_rclick, k_window_wheel_up,
       k_window_wheel_down, k_window_kbm, k_window_mouse };

static struct {
    int parsed;                 // 0 not yet, 1 parsed (count entries), -1 none
    int count;
    unsigned users;             // a bit per controller an entry names (controller 0's is set by any script)
    unsigned long start;        // GetTickCount of the first poll
    unsigned polled;            // a bit per controller polled at least once (port_input.h: taps between two polls)
    unsigned long last_poll[PORT_INPUT_USER_COUNT]; // when the controller was polled last (ms after start)
    struct { unsigned long at, hold; unsigned char key, user; } events[PORT_INPUT_SCRIPT_MAX];
    int window_count;
    struct {
        unsigned long at, hold;
        int kind, key, x, y, delivered, released;
        unsigned long character; // a key entry's GUIDE_KEY_CHARACTER (space)
        char text[PORT_INPUT_SCRIPT_TEXT];
    } window[PORT_INPUT_SCRIPT_WINDOW_MAX];
} g_script;

static int script_key(const char *name, int length)
{
    if (length == 2 && name[0] == 'r' && name[1] == 't')
        return k_script_right_trigger;
    if (length == 2 && name[0] == 'l' && name[1] == 't')
        return k_script_left_trigger;
    const int code = port_kbm_code_of(name, (unsigned long)length);
    return code > 0 ? code : PORT_KBM_NONE;
}

static unsigned long script_number(const char **text)
{
    unsigned long value = 0;
    for (; **text >= '0' && **text <= '9'; (*text)++)
        value = value * 10u + (unsigned long)(**text - '0');
    return value;
}

static long script_signed(const char **text)
{
    const int negative = **text == '-';
    if (negative)
        (*text)++;
    const long value = (long)script_number(text);
    return negative ? -value : value;
}

// The guide key a window entry's "key=" names (guide.h's GUIDE_KEY_*), -1 for none.
static int script_window_key(const char *name, int length)
{
    static const struct { const char *name; int key; } k_names[] = {
        { "up", GUIDE_KEY_UP }, { "down", GUIDE_KEY_DOWN }, { "left", GUIDE_KEY_LEFT }, { "right", GUIDE_KEY_RIGHT },
        { "pageup", GUIDE_KEY_PAGE_UP }, { "pagedown", GUIDE_KEY_PAGE_DOWN }, { "enter", GUIDE_KEY_ACCEPT },
        { "escape", GUIDE_KEY_BACK }, { "backspace", GUIDE_KEY_ERASE }, { "tab", GUIDE_KEY_NEXT_TAB },
        { "shifttab", GUIDE_KEY_PREVIOUS_TAB }, { "home", GUIDE_KEY_CLOSE }, { "grave", GUIDE_KEY_CLOSE },
        { "paste", GUIDE_KEY_PASTE }, { "space", GUIDE_KEY_CHARACTER },
    };
    for (unsigned i = 0; i < sizeof(k_names) / sizeof(k_names[0]); i++) {
        const char *n = k_names[i].name;
        int j = 0;
        while (j < length && n[j] != 0 && n[j] == name[j])
            j++;
        if (j == length && n[j] == 0)
            return k_names[i].key;
    }
    return -1;
}

// A window entry at `at`: "<what>=<value>" up to the next ',' (`text` points after the '='; `what` names it). Returns
// where the entry ends.
static const char *script_window_entry(unsigned long at, const char *what, int what_length, const char *text)
{
    static const char *const k_kinds[] = { "key", "text", "move", "click", "rclick", "wheelup", "wheeldown", "kbm",
                                           "mouse" };
    int kind = -1;
    for (int i = 0; i < (int)(sizeof(k_kinds) / sizeof(k_kinds[0])) && kind < 0; i++) {
        int j = 0;
        while (j < what_length && k_kinds[i][j] != 0 && k_kinds[i][j] == what[j])
            j++;
        if (j == what_length && k_kinds[i][j] == 0)
            kind = i;
    }
    const char *value = text;
    while (*text != 0 && *text != ',')
        text++;
    const int length = (int)(text - value);
    if (kind < 0 || g_script.window_count >= PORT_INPUT_SCRIPT_WINDOW_MAX)
        return text;
    __builtin_memset(&g_script.window[g_script.window_count], 0, sizeof(g_script.window[0]));
    g_script.window[g_script.window_count].at = at;
    g_script.window[g_script.window_count].kind = kind;
    if (kind == k_window_key || kind == k_window_kbm) {
        int name_length = 0;
        while (name_length < length && value[name_length] != ':')
            name_length++;
        int key;
        if (kind == k_window_key) {
            key = script_window_key(value, name_length);
            g_script.window[g_script.window_count].character = key == GUIDE_KEY_CHARACTER ? ' ' : 0;
            g_script.window[g_script.window_count].hold = 100;
        } else {
            key = port_kbm_code_of(value, (unsigned long)name_length);
            if (key == PORT_KBM_WHEEL)
                key = -1; // an input names a direction
            g_script.window[g_script.window_count].hold = 150;
        }
        if (key < 0)
            return text;
        g_script.window[g_script.window_count].key = key;
        if (name_length < length) {
            const char *hold = value + name_length + 1;
            g_script.window[g_script.window_count].hold = script_number(&hold);
        }
    } else if (kind == k_window_text) {
        for (int i = 0; i < length && i < PORT_INPUT_SCRIPT_TEXT - 1; i++)
            g_script.window[g_script.window_count].text[i] = value[i];
    } else {
        const char *c = value;
        g_script.window[g_script.window_count].x =
            (int)(kind == k_window_mouse ? script_signed(&c) : (long)script_number(&c));
        if (*c++ != 'x')
            return text;
        g_script.window[g_script.window_count].y =
            (int)(kind == k_window_mouse ? script_signed(&c) : (long)script_number(&c));
    }
    g_script.window_count++;
    return text;
}

static int script_active(void)
{
    if (g_script.parsed == 0) {
        g_script.parsed = -1;
        const char *text = getenv("HALO3_INPUT_SCRIPT");
        if (text != 0 && *text != 0) {
            g_script.parsed = 1;
            g_script.users = 1u;
            while (*text != 0 && g_script.count < PORT_INPUT_SCRIPT_MAX) {
                const unsigned long at = script_number(&text);
                if (*text++ != ':')
                    break;
                const char *what = text;
                while (*text != 0 && *text != '=' && *text != ',' && *text != ':' && *text != '@')
                    text++;
                if (*text == '=') { // a window entry
                    text = script_window_entry(at, what, (int)(text - what), text + 1);
                    if (*text == ',')
                        text++;
                    else
                        break;
                    continue;
                }
                text = what;
                const char *name = text;
                while (*text != 0 && *text != ':' && *text != ',' && *text != '@')
                    text++;
                const int key = script_key(name, (int)(text - name));
                unsigned long user = 0;
                if (*text == '@') {
                    text++;
                    user = script_number(&text);
                }
                unsigned long hold = 150;
                if (*text == ':') {
                    text++;
                    hold = script_number(&text);
                }
                if (key != PORT_KBM_NONE && user < PORT_INPUT_USER_COUNT) {
                    g_script.events[g_script.count].at = at;
                    g_script.events[g_script.count].hold = hold;
                    g_script.events[g_script.count].key = (unsigned char)key;
                    g_script.events[g_script.count].user = (unsigned char)user;
                    g_script.users |= 1u << user;
                    g_script.count++;
                }
                if (*text == ',')
                    text++;
                else
                    break;
            }
            g_script.start = GetTickCount();
        }
    }
    return g_script.parsed > 0;
}

// A kbm= entry's input going down or up, as the window passes it on: a press goes to the guide's binding capture while
// the in-game guide shows, else to the game; a release always to the game (a key held as the guide opened is let go).
static void script_kbm(int code, int down)
{
    if (down && guide_ingame_input()) {
        guide_capture(code);
        return;
    }
    if (code > k_port_key_none && code < k_port_key_count)
        port_input_key((enum e_port_key)code, down);
    else if (code >= PORT_KBM_MOUSE_FIRST && code < PORT_KBM_MOUSE_FIRST + k_port_mouse_button_count)
        port_input_mouse_button((enum e_port_mouse_button)(code - PORT_KBM_MOUSE_FIRST), down);
    else if (down && (code == PORT_KBM_WHEEL_UP || code == PORT_KBM_WHEEL_DOWN))
        port_input_mouse_wheel(code == PORT_KBM_WHEEL_UP ? 1 : -1);
}

// The window entries due now, each once, in the script's order (guide_ingame.h: nothing when the in-game guide does not
// show; the frame's own pixels, so the pointer's window size is the frame's: 0 x 0), and the keys whose hold is over
// let go.
static void script_window_deliver(void)
{
    const unsigned long now = GetTickCount() - g_script.start;
    for (int i = 0; i < g_script.window_count; i++) {
        const int kind = g_script.window[i].kind;
        if ((kind == k_window_key || kind == k_window_kbm) && g_script.window[i].delivered &&
            !g_script.window[i].released && now - g_script.window[i].at >= g_script.window[i].hold) {
            g_script.window[i].released = 1;
            if (kind == k_window_key)
                guide_ingame_key_action(g_script.window[i].key, g_script.window[i].character, GUIDE_KEY_RELEASED);
            else
                script_kbm(g_script.window[i].key, 0);
        }
        if (g_script.window[i].delivered || now < g_script.window[i].at)
            continue;
        g_script.window[i].delivered = 1;
        const int x = g_script.window[i].x, y = g_script.window[i].y;
        switch (kind) {
        case k_window_key: guide_ingame_key(g_script.window[i].key, g_script.window[i].character); break;
        case k_window_text:
            for (const char *c = g_script.window[i].text; *c != 0; c++)
                guide_ingame_key(GUIDE_KEY_CHARACTER, (unsigned char)*c);
            break;
        case k_window_move: guide_ingame_pointer(GUIDE_POINTER_MOVE, 0, x, y, 0, 0); break;
        case k_window_click: guide_ingame_pointer(GUIDE_POINTER_PRESS, 1, x, y, 0, 0); break;
        case k_window_rclick: guide_ingame_pointer(GUIDE_POINTER_PRESS, 3, x, y, 0, 0); break;
        case k_window_wheel_up: guide_ingame_pointer(GUIDE_POINTER_WHEEL, -1, x, y, 0, 0); break;
        case k_window_wheel_down: guide_ingame_pointer(GUIDE_POINTER_WHEEL, 1, x, y, 0, 0); break;
        case k_window_kbm: script_kbm(g_script.window[i].key, 1); break;
        case k_window_mouse:
            if (!guide_ingame_input())
                port_input_mouse_motion(x, y);
            break;
        default: break;
        }
    }
}

// The inputs the script holds down now on controller `user` (k_script_input_count entries). `poll`: the game's poll,
// which also reports the presses that began since the controller's previous poll and have ended by now, once
// (port_input.h: taps between two polls).
static void script_keys(unsigned long user, unsigned char *keys, int poll)
{
    const unsigned long now = GetTickCount() - g_script.start;
    const int taps = poll && ((g_script.polled >> user) & 1u) &&
                     now - g_script.last_poll[user] <= PORT_INPUT_TAP_WINDOW_MS;
    for (int i = 0; i < g_script.count; i++)
        if (g_script.events[i].user == user && now >= g_script.events[i].at &&
            (now - g_script.events[i].at < g_script.events[i].hold ||
             (taps && g_script.events[i].at > g_script.last_poll[user])))
            keys[g_script.events[i].key] = 1;
    if (poll) {
        g_script.last_poll[user] = now;
        g_script.polled |= 1u << user;
    }
}

static short axis(int negative, int positive)
{
    return (short)((short)(positive ? 32767 : 0) - (short)(negative ? 32767 : 0));
}

static short clamp_axis(long value)
{
    return (short)(value > 32767 ? 32767 : value < -32767 ? -32767 : value);
}

// The script's gamepad (k_script_input_count entries of `k`: the keys it names, then the triggers).
static void script_gamepad(const unsigned char *k, port_input_gamepad *out)
{
    static const struct { unsigned char key; unsigned short button; } k_script_layout[] = {
        { k_port_key_up, PORT_XINPUT_DPAD_UP },         { k_port_key_down, PORT_XINPUT_DPAD_DOWN },
        { k_port_key_left, PORT_XINPUT_DPAD_LEFT },     { k_port_key_right, PORT_XINPUT_DPAD_RIGHT },
        { k_port_key_space, PORT_XINPUT_A },            { k_port_key_backspace, PORT_XINPUT_B },
        { k_port_key_escape, PORT_XINPUT_B },           { k_port_key_e, PORT_XINPUT_X },
        { k_port_key_tab, PORT_XINPUT_BACK },           { k_port_key_1, PORT_XINPUT_Y },
        { k_port_key_return, PORT_XINPUT_START },       { k_port_key_f1, PORT_XINPUT_BACK },
        { k_port_key_q, PORT_XINPUT_LEFT_SHOULDER },    { k_port_key_f, PORT_XINPUT_RIGHT_SHOULDER },
        { k_port_key_v, PORT_XINPUT_RIGHT_SHOULDER },   { k_port_key_lctrl, PORT_XINPUT_LEFT_THUMB },
        { k_port_key_c, PORT_XINPUT_LEFT_THUMB },       { k_port_key_z, PORT_XINPUT_RIGHT_THUMB },
        { k_port_key_home, PORT_XINPUT_GUIDE },         { k_port_key_grave, PORT_XINPUT_GUIDE },
    };
    for (unsigned i = 0; i < sizeof(k_script_layout) / sizeof(k_script_layout[0]); i++)
        if (k[k_script_layout[i].key])
            out->buttons |= k_script_layout[i].button;
    out->right_trigger = k[k_script_right_trigger] ? 255 : 0;
    out->left_trigger = k[k_port_key_g] || k[k_script_left_trigger] ? 255 : 0;
    out->thumb_lx = axis(k[k_port_key_a], k[k_port_key_d]);
    out->thumb_ly = axis(k[k_port_key_s], k[k_port_key_w]); // XInput: up is positive
    out->thumb_rx = axis(k[k_port_key_j], k[k_port_key_l]);
    out->thumb_ry = axis(k[k_port_key_k], k[k_port_key_i]);
}

// ---- the keyboard's two roles: play (its bindings) and the menus (a controller) ----

// The menus' layout (port_input.h).
static const struct { unsigned char code; unsigned short button; } k_menu_layout[] = {
    { k_port_key_up, PORT_XINPUT_DPAD_UP },        { k_port_key_down, PORT_XINPUT_DPAD_DOWN },
    { k_port_key_left, PORT_XINPUT_DPAD_LEFT },    { k_port_key_right, PORT_XINPUT_DPAD_RIGHT },
    { PORT_KBM_WHEEL_UP, PORT_XINPUT_DPAD_UP },    { PORT_KBM_WHEEL_DOWN, PORT_XINPUT_DPAD_DOWN },
    { k_port_key_return, PORT_XINPUT_A },          { k_port_key_space, PORT_XINPUT_A },
    { k_port_key_escape, PORT_XINPUT_B },          { k_port_key_backspace, PORT_XINPUT_B },
    { k_port_key_x, PORT_XINPUT_X },               { k_port_key_y, PORT_XINPUT_Y },
    { k_port_key_q, PORT_XINPUT_LEFT_SHOULDER },   { k_port_key_e, PORT_XINPUT_RIGHT_SHOULDER },
    { k_port_key_tab, PORT_XINPUT_BACK },          { k_port_key_home, PORT_XINPUT_GUIDE },
    { k_port_key_grave, PORT_XINPUT_GUIDE },
};

// The poll's state (the game's thread).
static unsigned long g_keyboard_last_poll;        // GetTickCount of the last poll, 0 before the first
static unsigned char g_wheel_given[2];            // the last poll gave a notch up / down: this one lets it go
static port_kbm_bindings g_bindings;               // as of g_bindings_revision
static unsigned long g_bindings_revision = ~0ul;
static int g_playing;                             // the last poll was in play
static int g_polls_unreported; // polls since the game last reported (port_input_keyboard_report_game)
static unsigned char g_action_held[PORT_KBM_ACTION_COUNT]; // each binding held at the last poll (press edges)
static int g_crouch_toggled;                      // crouch_hold 0: crouched by a press
static long g_crouch_unit = -1;                   // ... the unit it crouched
static port_input_kbm_state g_actions;            // the last poll's game actions
static int g_actions_valid;
static int g_trace = -1;
static unsigned long g_trace_down = ~0ul;         // the actions last traced
static unsigned short g_trace_buttons = 0xffffu;  // ... and the controller buttons

// The game's reports (its thread too, but written between polls; atomics for clarity).
static volatile int g_report_pending, g_report_playing;
static volatile long g_report_unit = -1;

PORT_MS_ABI int port_input_keyboard_trace(void)
{
    if (g_trace < 0)
        g_trace = env_int("HALO3_KBM_TRACE", 0) == 1;
    return g_trace;
}

static void trace_number(unsigned long value, int base)
{
    char text[16];
    int n = 0;
    do {
        text[n++] = "0123456789abcdef"[value % (unsigned long)base];
        value /= (unsigned long)base;
    } while (value != 0 && n < 15);
    char out[17];
    for (int i = 0; i < n; i++)
        out[i] = text[n - 1 - i];
    out[n] = 0;
    port_write_error(out);
}

// Halo 3's action names for the trace (port_halo_action).
static const char *halo_action_name(int action)
{
    static const char *const k_names[PORT_HALO_ACTION_COUNT] = {
        "jump", "switch_grenade", "switch_weapon", "action_reload", "action_reload_left", "melee", "use_equipment",
        "throw_grenade", "fire", "fire_left", "melee_fire", "start", "back", "crouch", "zoom", "15", "16",
        "flashlight", "accept", "cancel", "20", "21", "22", "23", "rise", "descend", "voice", "skip_cinematic",
    };
    return action >= 0 && action < PORT_HALO_ACTION_COUNT ? k_names[action] : "?";
}

static void trace_state(const port_input_gamepad *out)
{
    const unsigned long down = g_actions_valid ? g_actions.down : 0;
    if (down == g_trace_down && out->buttons == g_trace_buttons)
        return;
    g_trace_down = down;
    g_trace_buttons = out->buttons;
    port_write_error("kbm: ");
    port_write_error(g_playing ? "play" : "menus");
    if (g_playing) {
        port_write_error(" context ");
        trace_number(__atomic_load_n(&g_report_context, __ATOMIC_RELAXED), 16);
    }
    port_write_error(" actions {");
    for (int a = 0, first = 1; a < PORT_HALO_ACTION_COUNT; a++) {
        if (!((down >> a) & 1ul))
            continue;
        port_write_error(first ? "" : " ");
        port_write_error(halo_action_name(a));
        first = 0;
    }
    port_write_error("} controller buttons 0x");
    trace_number(out->buttons, 16);
    port_write_error("\n");
}

static int code_down(const unsigned char *down, int code)
{
    if (code == PORT_KBM_WHEEL)
        return down[PORT_KBM_WHEEL_UP] || down[PORT_KBM_WHEEL_DOWN];
    return code > PORT_KBM_NONE && code < PORT_KBM_CODE_COUNT && down[code];
}

// What input `code` does in play: 1 when it presses a game action (a binding not a controller button), and the
// controller buttons its bindings press there; in the menus: the menu layout's.
static int play_meaning(int code, unsigned short *buttons)
{
    int actions = 0;
    *buttons = (code == k_port_key_home || code == k_port_key_grave) ? PORT_XINPUT_GUIDE : 0;
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
        for (int i = 0; i < PORT_KBM_BINDS; i++) {
            if (!port_kbm_codes_overlap(g_bindings.codes[a][i], code))
                continue;
            if (port_kbm_action_role(a) == PORT_KBM_ROLE_CONTROLLER)
                *buttons |= port_kbm_action_controller_button(a);
            else
                actions = 1;
        }
    }
    return actions;
}

static unsigned short menu_meaning(int code)
{
    unsigned short buttons = 0;
    for (unsigned i = 0; i < sizeof(k_menu_layout) / sizeof(k_menu_layout[0]); i++)
        if (k_menu_layout[i].code == code)
            buttons |= k_menu_layout[i].button;
    return buttons;
}

// The keyboard went from the menus to play or back: what is held now and means something else there waits until it is
// let go (Escape opened the pause menu, where it would be B; Space chose Resume Game, where it would jump).
static void role_changed(const unsigned char *down)
{
    for (int code = 1; code < PORT_KBM_CODE_COUNT; code++) {
        if (!down[code] || code == PORT_KBM_WHEEL)
            continue;
        unsigned short play_buttons;
        const int play_actions = play_meaning(code, &play_buttons);
        if (play_actions || play_buttons != menu_meaning(code))
            g_masked[code] = 1;
    }
}

// Whether `code` is held now (the window's state, not this poll's taps): a masked input waits for this to be 0.
static int held_now(int code)
{
    if (code > k_port_key_none && code < k_port_key_count)
        return g_keys[code];
    if (code >= PORT_KBM_MOUSE_FIRST && code < PORT_KBM_MOUSE_FIRST + k_port_mouse_button_count)
        return g_buttons[code - PORT_KBM_MOUSE_FIRST];
    return 0; // a wheel notch is one poll's
}

// In play: the game actions the bindings hold (g_actions) and the controller buttons they press.
static void play(const unsigned char *down, int dx, int dy, unsigned long dt_ms, port_input_gamepad *out)
{
    port_input_kbm_state *s = &g_actions;
    __builtin_memset(s, 0, sizeof(*s));
    const unsigned context = __atomic_load_n(&g_report_context, __ATOMIC_RELAXED);
    const long unit = __atomic_load_n(&g_report_unit, __ATOMIC_RELAXED);
    int held[PORT_KBM_ACTION_COUNT];
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
        held[a] = 0;
        for (int i = 0; i < PORT_KBM_BINDS; i++)
            held[a] |= code_down(down, g_bindings.codes[a][i]);
    }
    const int crouch_pressed = held[PORT_KBM_CROUCH] && !g_action_held[PORT_KBM_CROUCH];
    const int jump_pressed = held[PORT_KBM_JUMP] && !g_action_held[PORT_KBM_JUMP];
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++)
        g_action_held[a] = (unsigned char)held[a];
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
        if (!held[a])
            continue;
        switch (port_kbm_action_role(a)) {
        case PORT_KBM_ROLE_CONTROLLER:
            out->buttons |= port_kbm_action_controller_button(a);
            break;
        case PORT_KBM_ROLE_AXIS:
            break;
        default:
            if ((port_kbm_action_contexts(a) & context) == 0)
                break;
            s->down |= port_kbm_action_halo(a);
            if (port_kbm_action_role(a) == PORT_KBM_ROLE_BUTTON)
                s->hold |= port_kbm_action_halo(a);
            break;
        }
    }
    // crouch_hold 0: a press crouches, the next stands; a jump or a new life stands too
    if (!__atomic_load_n(&g_setting_crouch_hold, __ATOMIC_RELAXED)) {
        if (unit != g_crouch_unit || jump_pressed)
            g_crouch_toggled = 0;
        if (crouch_pressed)
            g_crouch_toggled = !g_crouch_toggled;
        const unsigned long crouch = 1ul << PORT_HALO_CROUCH;
        s->down = g_crouch_toggled ? s->down | crouch : s->down & ~crouch;
        s->hold = g_crouch_toggled ? s->hold | crouch : s->hold & ~crouch;
    } else {
        g_crouch_toggled = 0;
    }
    g_crouch_unit = unit;
    s->move_x = (float)(held[PORT_KBM_MOVE_RIGHT] - held[PORT_KBM_MOVE_LEFT]);
    s->move_y = (float)(held[PORT_KBM_MOVE_FORWARD] - held[PORT_KBM_MOVE_BACK]);
    if (dx != 0 || dy != 0) { // the mouse as a right stick (raw mouse look leaves the motion to port_input_mouse_look)
        if (g_stick_units < 0)
            g_stick_units = env_int("HALO3_MOUSE_SENSITIVITY", 900);
        const float units = (float)g_stick_units * mouse_look_scale((double)dx, (double)dy, dt_ms);
        const short x = clamp_axis((long)((float)dx * units)), y = clamp_axis((long)((float)-dy * units));
        s->look = 1;
        s->look_x = (float)x / 32767.0f;
        s->look_y = (float)y / 32767.0f * (__atomic_load_n(&g_setting_invert, __ATOMIC_RELAXED) ? -1.0f : 1.0f);
    }
    if (down[k_port_key_home] || down[k_port_key_grave])
        out->buttons |= PORT_XINPUT_GUIDE;
    g_actions_valid = 1;
}

// In the menus: the menu layout's controller.
static void menus(const unsigned char *down, int dx, int dy, port_input_gamepad *out)
{
    __builtin_memset(&g_actions, 0, sizeof(g_actions));
    g_actions_valid = 0;
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++)
        g_action_held[a] = 0;
    for (unsigned i = 0; i < sizeof(k_menu_layout) / sizeof(k_menu_layout[0]); i++)
        if (down[k_menu_layout[i].code])
            out->buttons |= k_menu_layout[i].button;
    out->thumb_lx = axis(down[k_port_key_a], down[k_port_key_d]);
    out->thumb_ly = axis(down[k_port_key_s], down[k_port_key_w]); // XInput: up is positive
    out->left_trigger = down[k_port_key_pageup] ? 255 : 0;
    out->right_trigger = down[k_port_key_pagedown] ? 255 : 0;
    if (dx != 0 || dy != 0) {
        if (g_stick_units < 0)
            g_stick_units = env_int("HALO3_MOUSE_SENSITIVITY", 900);
        const float units = (float)g_stick_units * sensitivity();
        out->thumb_rx = clamp_axis((long)((float)dx * units));
        out->thumb_ry = clamp_axis((long)((float)-dy * units));
    }
}

static short stronger(short a, short b) { return (a < 0 ? -a : a) >= (b < 0 ? -b : b) ? a : b; }

int port_input_keyboard_state(port_input_gamepad *out)
{
    __builtin_memset(out, 0, sizeof(*out));
    port_window_input_lock();
    port_window_poll_input();
    const int scripted = script_active();
    if (!scripted && !__atomic_load_n(&g_attached, __ATOMIC_ACQUIRE)) {
        port_window_input_unlock();
        __builtin_memset(&g_actions, 0, sizeof(g_actions));
        g_actions_valid = 0;
        return 0;
    }
    settings_refresh();
    const unsigned long revision = port_config_revision();
    if (revision != g_bindings_revision) {
        g_bindings_revision = revision;
        port_config_kbm_bindings(&g_bindings);
    }
    if (scripted)
        script_window_deliver(); // its kbm= presses before the state is read
    // the inputs down this poll: held, or pressed since the last poll when that one is recent (port_input.h: taps)
    unsigned char down[PORT_KBM_CODE_COUNT] = { 0 };
    const unsigned long poll_time = GetTickCount();
    const unsigned long poll_dt = g_keyboard_last_poll != 0 ? poll_time - g_keyboard_last_poll : 16ul;
    const int taps = g_keyboard_last_poll != 0 && poll_dt <= PORT_INPUT_TAP_WINDOW_MS;
    g_keyboard_last_poll = poll_time != 0 ? poll_time : 1;
    for (int i = 1; i < k_port_key_count; i++)
        down[i] = g_keys[i] | (__atomic_exchange_n(&g_key_presses[i], 0, __ATOMIC_ACQ_REL) & taps);
    for (int i = 0; i < k_port_mouse_button_count; i++)
        down[PORT_KBM_MOUSE(i)] =
            g_buttons[i] | (__atomic_exchange_n(&g_button_presses[i], 0, __ATOMIC_ACQ_REL) & taps);
    for (int i = 0; i < 2; i++) { // a notch: down for one poll, up for the next
        if (!taps) {
            __atomic_store_n(&g_wheel_notches[i], 0, __ATOMIC_RELAXED);
            g_wheel_given[i] = 0;
        } else if (g_wheel_given[i]) {
            g_wheel_given[i] = 0;
        } else if (__atomic_load_n(&g_wheel_notches[i], __ATOMIC_ACQUIRE) > 0) {
            __atomic_fetch_sub(&g_wheel_notches[i], 1, __ATOMIC_ACQ_REL);
            down[PORT_KBM_WHEEL_UP + i] = 1;
            g_wheel_given[i] = 1;
        }
    }
    // play or the menus: the game reports once a main-loop pass while it runs, and polls once a pass too; a second
    // poll without a report (a load, the main menu: no game running) is in the menus, a single one (a pass that polls
    // twice) keeps the role
    int playing = g_playing;
    if (__atomic_exchange_n(&g_report_pending, 0, __ATOMIC_ACQ_REL)) {
        g_polls_unreported = 0;
        playing = __atomic_load_n(&g_report_playing, __ATOMIC_ACQUIRE);
    } else if (++g_polls_unreported >= 2) {
        g_polls_unreported = 2;
        playing = 0;
    }
    if (__atomic_exchange_n(&g_release_masks, 0, __ATOMIC_ACQ_REL))
        __builtin_memset(g_masked, 0, sizeof(g_masked));
    if (playing != g_playing) {
        role_changed(down);
        g_playing = playing;
    }
    for (int code = 1; code < PORT_KBM_CODE_COUNT; code++) {
        if (!g_masked[code])
            continue;
        if (held_now(code))
            down[code] = 0;
        else
            g_masked[code] = 0;
    }
    // look: the pointer motion since the last poll; raw mouse look leaves it to port_input_mouse_look
    const int raw = port_input_mouse_raw();
    const int dx = raw ? 0 : __atomic_exchange_n(&g_mouse_dx, 0, __ATOMIC_RELAXED);
    const int dy = raw ? 0 : __atomic_exchange_n(&g_mouse_dy, 0, __ATOMIC_RELAXED);
    port_window_input_unlock();
    if (!raw && (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) >= PORT_INPUT_MOUSE_SOURCE_THRESHOLD)
        __atomic_store_n(&g_last_source_is_kbm, 1, __ATOMIC_RELAXED);
    if (playing)
        play(down, dx, dy, poll_dt, out);
    else
        menus(down, dx, dy, out);
    if (port_input_keyboard_trace())
        trace_state(out);
    if (scripted) { // the script's gamepad on controller 0
        unsigned char k[k_script_input_count] = { 0 };
        script_keys(0, k, 1);
        port_input_gamepad pad;
        __builtin_memset(&pad, 0, sizeof(pad));
        script_gamepad(k, &pad);
        out->buttons |= pad.buttons;
        out->left_trigger = out->left_trigger > pad.left_trigger ? out->left_trigger : pad.left_trigger;
        out->right_trigger = out->right_trigger > pad.right_trigger ? out->right_trigger : pad.right_trigger;
        out->thumb_lx = stronger(out->thumb_lx, pad.thumb_lx);
        out->thumb_ly = stronger(out->thumb_ly, pad.thumb_ly);
        out->thumb_rx = stronger(out->thumb_rx, pad.thumb_rx);
        out->thumb_ry = stronger(out->thumb_ry, pad.thumb_ry);
    }
    return 1;
}

void port_input_keyboard_suppress(void)
{
    __builtin_memset(&g_actions, 0, sizeof(g_actions));
    g_actions_valid = 0;
    port_window_input_lock();
    for (int code = 1; code < PORT_KBM_CODE_COUNT; code++)
        if (held_now(code))
            g_masked[code] = 1;
    port_window_input_unlock();
}

int port_input_script_state(unsigned long user, port_input_gamepad *out, int poll)
{
    __builtin_memset(out, 0, sizeof(*out));
    if (user == 0 || user >= PORT_INPUT_USER_COUNT || !script_active() || ((g_script.users >> user) & 1u) == 0)
        return 0;
    unsigned char k[k_script_input_count] = { 0 };
    script_keys(user, k, poll);
    script_gamepad(k, out);
    return 1;
}

// ---- the game's side ----

PORT_MS_ABI int port_input_keyboard_actions(unsigned int user, port_input_kbm_state *out)
{
    if (user != 0 || !g_actions_valid) {
        __builtin_memset(out, 0, sizeof(*out));
        return 0;
    }
    *out = g_actions;
    return 1;
}

PORT_MS_ABI void port_input_keyboard_report_game(int playing)
{
    __atomic_store_n(&g_report_playing, playing != 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_report_pending, 1, __ATOMIC_RELEASE);
}

PORT_MS_ABI void port_input_keyboard_report_player(unsigned context, int unit)
{
    __atomic_store_n(&g_report_context, context & PORT_KBM_CONTEXT_ALL ? context & PORT_KBM_CONTEXT_ALL
                                                                        : PORT_KBM_CONTEXT_FOOT,
                     __ATOMIC_RELAXED);
    __atomic_store_n(&g_report_unit, unit, __ATOMIC_RELAXED);
}

PORT_MS_ABI void port_input_keyboard_report_zoom(int zoomed)
{
    __atomic_store_n(&g_report_zoomed, zoomed != 0, __ATOMIC_RELAXED);
}

// The controller buttons by the index the input abstraction counts them in (e_gamepad_button), for the trace.
static const char *const k_gamepad_buttons[16] = {
    "LT", "RT", "DPAD-UP", "DPAD-DOWN", "DPAD-LEFT", "DPAD-RIGHT", "START", "BACK",
    "LS", "RS", "A", "B", "X", "Y", "LB", "RB",
};

static void trace_actions(const char *device, unsigned long bits)
{
    port_write_error(" ");
    port_write_error(device);
    port_write_error(" {");
    for (int a = 0, first = 1; a < PORT_HALO_ACTION_COUNT; a++) {
        if (!((bits >> a) & 1ul))
            continue;
        port_write_error(first ? "" : " ");
        port_write_error(halo_action_name(a));
        first = 0;
    }
    port_write_error("}");
}

PORT_MS_ABI void port_input_keyboard_trace_actions(unsigned int keys, unsigned int controller, const unsigned char *buttons,
    int count)
{
    port_write_error("input: controller 0 actions:");
    trace_actions("keyboard", keys);
    trace_actions("controller", controller);
    // where the button preset puts a few actions (Default: jump A, melee B, reload RB, grenade LT; Bumper Jumper:
    // jump LB, melee RB, reload B)
    static const struct { int action; const char *name; } k_shown[] = {
        { PORT_HALO_JUMP, "jump" }, { PORT_HALO_MELEE, "melee" }, { PORT_HALO_ACTION_RELOAD, "reload" },
        { PORT_HALO_THROW_GRENADE, "grenade" }, { PORT_HALO_SWITCH_GRENADE, "switch_grenade" },
    };
    port_write_error(" preset:");
    for (unsigned i = 0; i < sizeof(k_shown) / sizeof(k_shown[0]); i++) {
        if (k_shown[i].action >= count)
            continue;
        port_write_error(" ");
        port_write_error(k_shown[i].name);
        port_write_error("=");
        port_write_error(buttons[k_shown[i].action] < 16 ? k_gamepad_buttons[buttons[k_shown[i].action]] : "-");
    }
    port_write_error("\n");
}

PORT_MS_ABI int port_input_keyboard_diag_preset(int joystick)
{
    static int presets[2] = { -2, -2 };
    if (presets[joystick != 0] == -2) {
        const int value = env_int(joystick ? "HALO3_DIAG_JOYSTICK_PRESET" : "HALO3_DIAG_BUTTON_PRESET", -1);
        presets[joystick != 0] = value >= 0 && value <= (joystick ? 3 : 5) ? value : -1;
    }
    return presets[joystick != 0];
}

PORT_MS_ABI int port_input_last_source_is_kbm(void)
{
    return __atomic_load_n(&g_last_source_is_kbm, __ATOMIC_RELAXED) != 0 ? 1 : 0;
}

PORT_MS_ABI void port_input_note_gamepad_activity(void)
{
    __atomic_store_n(&g_last_source_is_kbm, 0, __ATOMIC_RELAXED);
}

PORT_MS_ABI unsigned port_input_keyboard_context(void)
{
    return __atomic_load_n(&g_report_context, __ATOMIC_RELAXED);
}

// Short human-readable key label for dynamic HUD prompts (falls back to port_kbm_code_label for letters/digits/F-keys/punctuation).
static const char *kbm_short_label(int code)
{
    switch (code) {
    case k_port_key_escape: return "Esc";
    case k_port_key_tab: return "Tab";
    case k_port_key_space: return "Space";
    case k_port_key_return: return "Enter";
    case k_port_key_backspace: return "Bksp";
    case k_port_key_up: return "Up";
    case k_port_key_down: return "Down";
    case k_port_key_left: return "Left";
    case k_port_key_right: return "Right";
    case k_port_key_lshift: return "Shift";
    case k_port_key_rshift: return "RShift";
    case k_port_key_lctrl: return "Ctrl";
    case k_port_key_rctrl: return "RCtrl";
    case k_port_key_lalt: return "Alt";
    case k_port_key_ralt: return "RAlt";
    case k_port_key_capslock: return "Caps";
    case k_port_key_home: return "Home";
    case k_port_key_end: return "End";
    case k_port_key_insert: return "Ins";
    case k_port_key_delete: return "Del";
    case k_port_key_pageup: return "PgUp";
    case k_port_key_pagedown: return "PgDn";
    case PORT_KBM_MOUSE(k_port_mouse_left): return "LMB";
    case PORT_KBM_MOUSE(k_port_mouse_right): return "RMB";
    case PORT_KBM_MOUSE(k_port_mouse_middle): return "MMB";
    case PORT_KBM_MOUSE(k_port_mouse_4): return "M4";
    case PORT_KBM_MOUSE(k_port_mouse_5): return "M5";
    case PORT_KBM_WHEEL_UP: return "WheelUp";
    case PORT_KBM_WHEEL_DOWN: return "WheelDn";
    case PORT_KBM_WHEEL: return "Wheel";
    default: return port_kbm_code_label(code);
    }
}

static int g_label_codes[PORT_KBM_ACTION_COUNT];
static unsigned long g_label_revision = ~0ul;

PORT_MS_ABI int port_input_kbm_action_label(int kbm_action, char *out, unsigned int out_size)
{
    if (out != 0 && out_size > 0)
        out[0] = 0;
    if (out == 0 || out_size == 0 || kbm_action < 0 || kbm_action >= PORT_KBM_ACTION_COUNT)
        return 0;
    const unsigned long revision = port_config_revision();
    if (__atomic_load_n(&g_label_revision, __ATOMIC_ACQUIRE) != revision) {
        port_kbm_bindings bindings;
        port_config_kbm_bindings(&bindings);
        for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
            int c = bindings.codes[a][0];
            if (c == PORT_KBM_NONE)
                c = bindings.codes[a][1];
            __atomic_store_n(&g_label_codes[a], c, __ATOMIC_RELAXED);
        }
        __atomic_store_n(&g_label_revision, revision, __ATOMIC_RELEASE);
    }
    int code = __atomic_load_n(&g_label_codes[kbm_action], __ATOMIC_RELAXED);
    if (code == PORT_KBM_NONE)
        return 0;
    const char *label = kbm_short_label(code);
    if (label == 0 || label[0] == 0)
        return 0;
    unsigned int i = 0;
    while (label[i] != 0 && i + 1 < out_size) {
        out[i] = label[i];
        i++;
    }
    out[i] = 0;
    return 1;
}

