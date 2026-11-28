// port/sdk/runtime (Linux host): hardware gamepads through the kernel joystick API (/dev/input/js0..3, linux/joystick.h),
// read without blocking on each poll. A device that is missing or unplugged is looked for again every two seconds.
// Interface: port/sdk/input/port_input.h.
//
// Axes and buttons are mapped by the input codes the driver reports for each joystick index (JSIOCGAXMAP,
// JSIOCGBTNMAP), not by index: xpad and xone number an Xbox pad A B X Y LB RB View Menu Guide LS RS with axes
// LX LY LT RX RY RT HatX HatY, but hid-generic / hid-microsoft (an Xbox Wireless Controller over Bluetooth) leaves gaps
// in the button numbers (A=0 B=1 X=3 Y=4 LB=6 RB=7 View=10 Menu=11) and reports the right stick on Z/RZ and the
// triggers on BRAKE/GAS, and an Elite pad's paddles follow the D-pad buttons. Without the maps the fixed xpad order is
// used.
//
// Rumble: the joystick API has no force feedback, so it goes through the event device of the same input device
// (evdev force feedback), found beside the joystick in sysfs (/sys/class/input/js<n>/device/event<m>) and opened for
// writing when the joystick opens. Each controller has one FF_RUMBLE effect: the left motor (the low-frequency, strong
// one) drives strong_magnitude and the right motor (high-frequency) weak_magnitude. A change of speed updates the
// effect in place (EVIOCSFF with its id) and starts it (an EV_FF write); both motors 0 stops it. The effect lasts its
// longest (65.535 s) and is started again every 30 s while the speeds hold. Without a writable event device (no
// uaccess ACL and not in the input group) or without FF_RUMBLE the controller plays without rumble. The kernel stops
// and erases a file's effects when it is closed, so the motors stop when the process ends however it ends;
// port_input_hardware_shutdown stops them first on an orderly exit.
#include "../input/port_input.h"
#include "port_runtime.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define PORT_JS_BUTTON_MAX 128
#define PORT_JS_SCAN_MAX 32

// What a joystick axis feeds.
enum {
    k_axis_none = -1,
    k_axis_lx, k_axis_ly, k_axis_lt, k_axis_rx, k_axis_ry, k_axis_rt, k_axis_hat_x, k_axis_hat_y,
    k_axis_role_count
};
// Digital triggers (BTN_TL2 / BTN_TR2) set the trigger axis instead of a button bit.
#define PORT_JS_DIGITAL_LT 0x10000u
#define PORT_JS_DIGITAL_RT 0x20000u

// xpad's Xbox 360 order, for a driver that does not answer the map queries.
static const unsigned short k_xpad_buttons[15] = {
    BTN_SOUTH, BTN_EAST, BTN_NORTH, BTN_WEST, BTN_TL, BTN_TR, BTN_SELECT, BTN_START, BTN_MODE, BTN_THUMBL, BTN_THUMBR,
    BTN_TRIGGER_HAPPY1, BTN_TRIGGER_HAPPY2, BTN_TRIGGER_HAPPY3, BTN_TRIGGER_HAPPY4,
};
static const unsigned char k_xpad_axes[8] = { ABS_X, ABS_Y, ABS_Z, ABS_RX, ABS_RY, ABS_RZ, ABS_HAT0X, ABS_HAT0Y };

static struct {
    int fd;               // -1: not open
    int js_index;         // /dev/input/js<js_index>; -1: not open
    int rank;             // 2: hardware pad, 1: virtual Steam Input pad
    long long next_probe; // monotonic ms
    unsigned short buttons;
    short axes[k_axis_role_count];
    // since the last poll (port_input.h: taps between two polls): the buttons that went down, and the strongest pull of
    // each trigger (k_axis_lt, k_axis_rt) that started from rest (-32767: none)
    unsigned short pressed;
    short pulled[2];
    long long last_poll; // monotonic ms of the last poll, 0 before the first
    unsigned int button_bits[PORT_JS_BUTTON_MAX]; // joystick button index -> XInput bit (or PORT_JS_DIGITAL_*)
    signed char axis_roles[ABS_CNT];             // joystick axis index -> k_axis_*
    int ff_fd;                                   // the event device for rumble; -1: none
    short ff_id;                                 // the uploaded FF_RUMBLE effect; -1: none yet
    unsigned short ff_left, ff_right;            // the speeds playing (0, 0: stopped)
    long long ff_started;                        // monotonic ms the effect was last started
} g_pads[PORT_INPUT_USER_COUNT];
static int g_pads_initialized;

#define PORT_FF_LENGTH_MS 0xffff
#define PORT_FF_RESTART_MS 30000

static long long now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

static void reset_state(unsigned long user)
{
    g_pads[user].buttons = 0;
    for (int i = 0; i < k_axis_role_count; i++)
        g_pads[user].axes[i] = (i == k_axis_lt || i == k_axis_rt) ? -32767 : 0;
    g_pads[user].pressed = 0;
    g_pads[user].pulled[0] = g_pads[user].pulled[1] = -32767;
}

// A trigger axis event: a pull that started from rest (the trigger's 0) since the last poll keeps its strongest value
// for the poll (port_input.h: taps between two polls).
static void set_trigger_axis(unsigned long user, int role, short value)
{
    short *pulled = &g_pads[user].pulled[role == k_axis_lt ? 0 : 1];
    if (value > *pulled && (g_pads[user].axes[role] < -32767 + 257 || *pulled > -32767))
        *pulled = value;
    g_pads[user].axes[role] = value;
}

static unsigned int button_bit(unsigned short code)
{
    switch (code) {
    case BTN_SOUTH: return PORT_XINPUT_A;
    case BTN_EAST: return PORT_XINPUT_B;
    case BTN_NORTH: return PORT_XINPUT_X; // BTN_NORTH is BTN_X: xpad, xone and hid-generic all report X there
    case BTN_WEST: return PORT_XINPUT_Y;  // BTN_WEST is BTN_Y
    case BTN_TL: return PORT_XINPUT_LEFT_SHOULDER;
    case BTN_TR: return PORT_XINPUT_RIGHT_SHOULDER;
    case BTN_TL2: return PORT_JS_DIGITAL_LT;
    case BTN_TR2: return PORT_JS_DIGITAL_RT;
    case BTN_SELECT: return PORT_XINPUT_BACK;
    case BTN_START: return PORT_XINPUT_START;
    case BTN_THUMBL: return PORT_XINPUT_LEFT_THUMB;
    case BTN_THUMBR: return PORT_XINPUT_RIGHT_THUMB;
    case BTN_MODE: return PORT_XINPUT_GUIDE; // the Xbox button: the guide's (port/sdk/xam/xam.c)
    case BTN_DPAD_UP: return PORT_XINPUT_DPAD_UP;
    case BTN_DPAD_DOWN: return PORT_XINPUT_DPAD_DOWN;
    case BTN_DPAD_LEFT: return PORT_XINPUT_DPAD_LEFT;
    case BTN_DPAD_RIGHT: return PORT_XINPUT_DPAD_RIGHT;
    case BTN_TRIGGER_HAPPY1: return PORT_XINPUT_DPAD_LEFT; // xpad with dpad_to_buttons
    case BTN_TRIGGER_HAPPY2: return PORT_XINPUT_DPAD_RIGHT;
    case BTN_TRIGGER_HAPPY3: return PORT_XINPUT_DPAD_UP;
    case BTN_TRIGGER_HAPPY4: return PORT_XINPUT_DPAD_DOWN;
    default: return 0; // Share, Elite paddles: no XInput bit
    }
}

static signed char axis_role(unsigned char code, int has_pedals)
{
    switch (code) {
    case ABS_X: return k_axis_lx;
    case ABS_Y: return k_axis_ly;
    case ABS_RX: return k_axis_rx;
    case ABS_RY: return k_axis_ry;
    case ABS_Z: return has_pedals ? k_axis_rx : k_axis_lt; // hid-generic: right stick on Z/RZ, triggers on BRAKE/GAS
    case ABS_RZ: return has_pedals ? k_axis_ry : k_axis_rt;
    case ABS_BRAKE: return k_axis_lt;
    case ABS_GAS: return k_axis_rt;
    case ABS_HAT0X: return k_axis_hat_x;
    case ABS_HAT0Y: return k_axis_hat_y;
    default: return k_axis_none;
    }
}

// Appends `text` to the message being built in `line` (NUL-terminated, `size` bytes).
static void append(char *line, unsigned size, const char *text)
{
    unsigned n = 0;
    while (line[n] != 0)
        n++;
    for (; *text != 0 && n + 1 < size; text++)
        line[n++] = *text;
    line[n] = 0;
}

static void append_uint(char *line, unsigned size, unsigned value)
{
    char digits[12];
    unsigned d = 0;
    do {
        digits[d++] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0 && d < sizeof(digits));
    char text[13];
    unsigned i = 0;
    while (d > 0)
        text[i++] = digits[--d];
    text[i] = 0;
    append(line, size, text);
}

static void format_js_path(char *out, unsigned size, const char *prefix, int js_index, const char *suffix)
{
    out[0] = 0;
    append(out, size, prefix);
    append_uint(out, size, (unsigned)js_index);
    append(out, size, suffix);
}

static int read_sysfs_id(int js_index, const char *leaf, char *out, unsigned cap)
{
    char path[96];
    format_js_path(path, sizeof(path), "/sys/class/input/js", js_index, "/device/id/");
    append(path, sizeof(path), leaf);
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return 0;
    ssize_t got = read(fd, out, cap - 1);
    close(fd);
    if (got <= 0)
        return 0;
    while (got > 0 && (out[got - 1] == '\n' || out[got - 1] == '\r' || out[got - 1] == ' '))
        got--;
    out[got] = 0;
    return 1;
}

static int text_starts_with(const char *s, const char *prefix)
{
    for (; *prefix != 0; s++, prefix++)
        if (*s != *prefix)
            return 0;
    return 1;
}

// Classifies an open /dev/input/js<js_index>:
//   0 = not a gamepad (virtual mouse/keyboard HID, motion/IMU sensor, rudder/pedal device)
//   1 = virtual Steam Input gamepad (28de:11ff / "Microsoft X-Box 360 pad N")
//   2 = physical hardware gamepad
static int classify_gamepad(int fd, int js_index)
{
    unsigned char axis_count = 0, button_count = 0;
    unsigned char axis_codes[ABS_CNT];
    unsigned short button_codes[KEY_MAX - BTN_MISC + 1];
    if (ioctl(fd, JSIOCGAXES, &axis_count) >= 0 && ioctl(fd, JSIOCGBUTTONS, &button_count) >= 0 &&
        ioctl(fd, JSIOCGAXMAP, axis_codes) >= 0 && ioctl(fd, JSIOCGBTNMAP, button_codes) >= 0) {
        int has_pedals = 0;
        for (unsigned i = 0; i < axis_count && i < ABS_CNT; i++)
            has_pedals |= axis_codes[i] == ABS_GAS || axis_codes[i] == ABS_BRAKE;
        unsigned mapped_buttons = 0, stick_axes = 0;
        for (unsigned i = 0; i < button_count && i < PORT_JS_BUTTON_MAX; i++) {
            const unsigned int bit = button_bit(button_codes[i]);
            if (bit != 0 && bit != PORT_JS_DIGITAL_LT && bit != PORT_JS_DIGITAL_RT)
                mapped_buttons++;
        }
        for (unsigned i = 0; i < axis_count && i < ABS_CNT; i++) {
            const signed char role = axis_role(axis_codes[i], has_pedals);
            if (role == k_axis_lx || role == k_axis_ly || role == k_axis_rx || role == k_axis_ry ||
                role == k_axis_hat_x || role == k_axis_hat_y)
                stick_axes++;
        }
        if (mapped_buttons < 2 || stick_axes < 2)
            return 0;
    }
    char vendor[16] = "", product[16] = "";
    if (read_sysfs_id(js_index, "vendor", vendor, sizeof(vendor)) &&
        read_sysfs_id(js_index, "product", product, sizeof(product)) &&
        text_starts_with(vendor, "28de") && text_starts_with(product, "11ff"))
        return 1;
    char name[128] = "";
    if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) >= 0) {
        name[sizeof(name) - 1] = 0;
        if (text_starts_with(name, "Microsoft X-Box 360 pad "))
            return 1;
    }
    return 2;
}

// Builds the index maps of a freshly opened device from the codes the driver reports.
static void build_maps(unsigned long user)
{
    const int fd = g_pads[user].fd;
    unsigned char axis_count = 0, button_count = 0;
    unsigned char axis_codes[ABS_CNT];
    unsigned short button_codes[KEY_MAX - BTN_MISC + 1];
    if (ioctl(fd, JSIOCGAXES, &axis_count) < 0 || ioctl(fd, JSIOCGBUTTONS, &button_count) < 0
        || ioctl(fd, JSIOCGAXMAP, axis_codes) < 0 || ioctl(fd, JSIOCGBTNMAP, button_codes) < 0) {
        axis_count = sizeof(k_xpad_axes);
        button_count = sizeof(k_xpad_buttons) / sizeof(k_xpad_buttons[0]);
        for (unsigned i = 0; i < axis_count; i++)
            axis_codes[i] = k_xpad_axes[i];
        for (unsigned i = 0; i < button_count; i++)
            button_codes[i] = k_xpad_buttons[i];
    }
    int has_pedals = 0;
    for (unsigned i = 0; i < axis_count && i < ABS_CNT; i++)
        has_pedals |= axis_codes[i] == ABS_GAS || axis_codes[i] == ABS_BRAKE;
    for (unsigned i = 0; i < ABS_CNT; i++)
        g_pads[user].axis_roles[i] = i < axis_count ? axis_role(axis_codes[i], has_pedals) : k_axis_none;
    for (unsigned i = 0; i < PORT_JS_BUTTON_MAX; i++)
        g_pads[user].button_bits[i] = i < button_count ? button_bit(button_codes[i]) : 0;
    char name[128] = "";
    ioctl(fd, JSIOCGNAME(sizeof(name)), name);
    name[sizeof(name) - 1] = 0;
    char line[224] = "port: input: controller ";
    append_uint(line, sizeof(line), (unsigned)user);
    append(line, sizeof(line), " (/dev/input/js");
    append_uint(line, sizeof(line), (unsigned)g_pads[user].js_index);
    append(line, sizeof(line), "): ");
    append(line, sizeof(line), name);
    append(line, sizeof(line), "\n");
    port_write_error(line);
}

static void report_rumble(unsigned long user, const char *what, const char *path, const char *tail)
{
    char line[160] = "port: input: controller ";
    char digit[2] = { (char)('0' + user), 0 };
    append(line, sizeof(line), digit);
    append(line, sizeof(line), what);
    append(line, sizeof(line), path);
    append(line, sizeof(line), tail);
    append(line, sizeof(line), "\n");
    port_write_error(line);
}

static int write_ff(int fd, unsigned short code, int value)
{
    struct input_event event;
    __builtin_memset(&event, 0, sizeof(event));
    event.type = EV_FF;
    event.code = code;
    event.value = value;
    return write(fd, &event, sizeof(event)) == (ssize_t)sizeof(event);
}

// Opens the event device of joystick `user` for rumble, if it is writable and has FF_RUMBLE.
static void open_rumble(unsigned long user)
{
    g_pads[user].ff_fd = -1;
    g_pads[user].ff_id = -1;
    g_pads[user].ff_left = g_pads[user].ff_right = 0;
    if (g_pads[user].js_index < 0)
        return;
    char device[64];
    format_js_path(device, sizeof(device), "/sys/class/input/js", g_pads[user].js_index, "/device");
    DIR *directory = opendir(device);
    if (directory == 0)
        return;
    char path[64] = "";
    for (struct dirent *entry; path[0] == 0 && (entry = readdir(directory)) != 0;) {
        const char *name = entry->d_name;
        if (name[0] == 'e' && name[1] == 'v' && name[2] == 'e' && name[3] == 'n' && name[4] == 't'
            && name[5] >= '0' && name[5] <= '9') {
            append(path, sizeof(path), "/dev/input/");
            append(path, sizeof(path), name);
        }
    }
    closedir(directory);
    if (path[0] == 0)
        return;
    const int fd = open(path, O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) {
        report_rumble(user, ": no rumble (cannot write ", path, ")");
        return;
    }
    unsigned long features[(FF_CNT + 8 * sizeof(unsigned long) - 1) / (8 * sizeof(unsigned long))];
    __builtin_memset(features, 0, sizeof(features));
    const unsigned bits = 8 * sizeof(unsigned long);
    if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)), features) < 0
        || ((features[FF_RUMBLE / bits] >> (FF_RUMBLE % bits)) & 1) == 0) {
        report_rumble(user, ": no rumble (no FF_RUMBLE on ", path, ")");
        close(fd);
        return;
    }
    g_pads[user].ff_fd = fd;
    report_rumble(user, ": rumble through ", path, "");
}

// Stops the effect and closes the event device (the kernel erases the file's effects).
static void close_rumble(unsigned long user)
{
    if (g_pads[user].ff_fd < 0)
        return;
    if (g_pads[user].ff_id >= 0) {
        write_ff(g_pads[user].ff_fd, (unsigned short)g_pads[user].ff_id, 0);
        ioctl(g_pads[user].ff_fd, EVIOCRMFF, (int)g_pads[user].ff_id);
    }
    close(g_pads[user].ff_fd);
    g_pads[user].ff_fd = -1;
    g_pads[user].ff_id = -1;
    g_pads[user].ff_left = g_pads[user].ff_right = 0;
}

static void close_pad(unsigned long user)
{
    close_rumble(user);
    close(g_pads[user].fd);
    g_pads[user].fd = -1;
    g_pads[user].js_index = -1;
    g_pads[user].rank = 0;
    reset_state(user);
    g_pads[user].next_probe = now_ms() + 2000;
}

static int js_already_open(int js_index)
{
    for (unsigned long i = 0; i < PORT_INPUT_USER_COUNT; i++)
        if (g_pads[i].fd >= 0 && g_pads[i].js_index == js_index)
            return 1;
    return 0;
}

static int any_hardware_pad_open(void)
{
    for (unsigned long i = 0; i < PORT_INPUT_USER_COUNT; i++)
        if (g_pads[i].fd >= 0 && g_pads[i].rank >= 2)
            return 1;
    return 0;
}

// Drains pending events. Returns 0 if the device is gone.
static int update_pad(unsigned long user)
{
    if (!g_pads_initialized) {
        for (unsigned long i = 0; i < PORT_INPUT_USER_COUNT; i++) {
            g_pads[i].fd = -1;
            g_pads[i].js_index = -1;
            g_pads[i].rank = 0;
            g_pads[i].ff_fd = -1;
            g_pads[i].ff_id = -1;
            reset_state(i);
        }
        g_pads_initialized = 1;
    }
    if (g_pads[user].fd < 0) {
        if (now_ms() < g_pads[user].next_probe)
            return 0;
        int chosen_fd = -1, chosen_js = -1, chosen_rank = 0;
        int fallback_fd = -1, fallback_js = -1;
        for (int js = 0; js < PORT_JS_SCAN_MAX; js++) {
            if (js_already_open(js))
                continue;
            char path[32];
            format_js_path(path, sizeof(path), "/dev/input/js", js, "");
            const int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
            if (fd < 0)
                continue;
            const int rank = classify_gamepad(fd, js);
            if (rank >= 2) {
                chosen_fd = fd;
                chosen_js = js;
                chosen_rank = rank;
                break;
            }
            if (rank == 1 && fallback_fd < 0) {
                fallback_fd = fd;
                fallback_js = js;
            } else {
                close(fd);
            }
        }
        if (chosen_fd < 0 && fallback_fd >= 0 && !any_hardware_pad_open()) {
            chosen_fd = fallback_fd;
            chosen_js = fallback_js;
            chosen_rank = 1;
            fallback_fd = -1;
        }
        if (fallback_fd >= 0)
            close(fallback_fd);
        if (chosen_fd < 0) {
            g_pads[user].next_probe = now_ms() + 2000;
            return 0;
        }
        g_pads[user].fd = chosen_fd;
        g_pads[user].js_index = chosen_js;
        g_pads[user].rank = chosen_rank;
        reset_state(user);
        build_maps(user);
        open_rumble(user);
    }
    struct js_event events[32];
    for (;;) {
        ssize_t got = read(g_pads[user].fd, events, sizeof(events));
        if (got < 0) {
            if (errno == EAGAIN || errno == EINTR)
                return 1;
            close_pad(user); // ENODEV: unplugged
            return 0;
        }
        for (ssize_t i = 0; i < got / (ssize_t)sizeof(struct js_event); i++) {
            const struct js_event *e = &events[i];
            const unsigned char type = e->type & (unsigned char)~JS_EVENT_INIT;
            if (type == JS_EVENT_BUTTON && e->number < PORT_JS_BUTTON_MAX) {
                const unsigned int bit = g_pads[user].button_bits[e->number];
                if (bit == PORT_JS_DIGITAL_LT || bit == PORT_JS_DIGITAL_RT)
                    set_trigger_axis(user, bit == PORT_JS_DIGITAL_LT ? k_axis_lt : k_axis_rt, e->value ? 32767 : -32767);
                else if (e->value) {
                    if ((g_pads[user].buttons & bit) == 0)
                        g_pads[user].pressed |= (unsigned short)bit;
                    g_pads[user].buttons |= (unsigned short)bit;
                }
                else
                    g_pads[user].buttons &= (unsigned short)~bit;
            } else if (type == JS_EVENT_AXIS && e->number < ABS_CNT) {
                const signed char role = g_pads[user].axis_roles[e->number];
                if (role == k_axis_lt || role == k_axis_rt)
                    set_trigger_axis(user, role, e->value);
                else if (role != k_axis_none)
                    g_pads[user].axes[role] = e->value;
            }
        }
        if (got < (ssize_t)sizeof(events))
            return 1;
    }
}

static short negate(short value)
{
    return value == -32768 ? 32767 : (short)-value;
}

static unsigned char trigger(short value)
{
    return (unsigned char)(((long)value + 32767) * 255 / 65534);
}

int port_input_hardware_state(unsigned long user, port_input_gamepad *out, int poll)
{
    __builtin_memset(out, 0, sizeof(*out));
    if (user >= PORT_INPUT_USER_COUNT || !update_pad(user))
        return 0;
    short a[k_axis_role_count];
    __builtin_memcpy(a, g_pads[user].axes, sizeof(a));
    out->buttons = g_pads[user].buttons;
    if (poll) { // the presses since the last poll, once, when it is recent (port_input.h: taps between two polls)
        const long long now = now_ms();
        const int taps = g_pads[user].last_poll != 0 && now - g_pads[user].last_poll <= PORT_INPUT_TAP_WINDOW_MS;
        g_pads[user].last_poll = now;
        if (taps) {
            out->buttons |= g_pads[user].pressed;
            if (g_pads[user].pulled[0] > a[k_axis_lt])
                a[k_axis_lt] = g_pads[user].pulled[0];
            if (g_pads[user].pulled[1] > a[k_axis_rt])
                a[k_axis_rt] = g_pads[user].pulled[1];
        }
        g_pads[user].pressed = 0;
        g_pads[user].pulled[0] = g_pads[user].pulled[1] = -32767;
    }
    if (a[k_axis_hat_x] < -16384) out->buttons |= PORT_XINPUT_DPAD_LEFT;
    if (a[k_axis_hat_x] > 16384) out->buttons |= PORT_XINPUT_DPAD_RIGHT;
    if (a[k_axis_hat_y] < -16384) out->buttons |= PORT_XINPUT_DPAD_UP;
    if (a[k_axis_hat_y] > 16384) out->buttons |= PORT_XINPUT_DPAD_DOWN;
    out->thumb_lx = a[k_axis_lx];
    out->thumb_ly = negate(a[k_axis_ly]); // joystick y grows downwards, XInput's upwards
    out->left_trigger = trigger(a[k_axis_lt]);
    out->thumb_rx = a[k_axis_rx];
    out->thumb_ry = negate(a[k_axis_ry]);
    out->right_trigger = trigger(a[k_axis_rt]);
    return 1;
}

int port_input_hardware_vibration(unsigned long user, unsigned short left, unsigned short right)
{
    if (user >= PORT_INPUT_USER_COUNT || !g_pads_initialized || g_pads[user].fd < 0)
        return 0;
    const int fd = g_pads[user].ff_fd;
    if (fd < 0)
        return 1; // connected, without rumble
    const long long now = now_ms();
    const int on = left != 0 || right != 0;
    if (left == g_pads[user].ff_left && right == g_pads[user].ff_right
        && (!on || now - g_pads[user].ff_started < PORT_FF_RESTART_MS))
        return 1;
    if (on) {
        struct ff_effect effect;
        __builtin_memset(&effect, 0, sizeof(effect));
        effect.type = FF_RUMBLE;
        effect.id = g_pads[user].ff_id;
        effect.u.rumble.strong_magnitude = left;
        effect.u.rumble.weak_magnitude = right;
        effect.replay.length = PORT_FF_LENGTH_MS;
        int uploaded = ioctl(fd, EVIOCSFF, &effect) >= 0;
        if (!uploaded && g_pads[user].ff_id >= 0) { // the effect is gone (the device was reset): a new one
            effect.id = -1;
            uploaded = ioctl(fd, EVIOCSFF, &effect) >= 0;
        }
        if (!uploaded || !write_ff(fd, (unsigned short)effect.id, 1)) {
            report_rumble(user, ": rumble stopped (the event device refused the effect)", "", "");
            close_rumble(user);
            return 1;
        }
        g_pads[user].ff_id = effect.id;
        g_pads[user].ff_started = now;
    } else if (g_pads[user].ff_id >= 0) {
        write_ff(fd, (unsigned short)g_pads[user].ff_id, 0);
    }
    g_pads[user].ff_left = left;
    g_pads[user].ff_right = right;
    return 1;
}

void port_input_hardware_shutdown(void)
{
    if (!g_pads_initialized)
        return;
    for (unsigned long user = 0; user < PORT_INPUT_USER_COUNT; user++)
        close_rumble(user);
}
