// port/sdk/input (both hosts): the keyboard and mouse bindings' table and the rules for changing them (kbm_bindings.h).
#include "kbm_bindings.h"
#include "port_input.h"

// ---- names ----

// Each code's halo3.ini name and Guide label. Letters and digits are built (k_letters); the rest are listed.
static const struct {
    unsigned char code;
    const char *name, *label;
} k_named[] = {
    { k_port_key_f1, "f1", "F1" }, { k_port_key_f2, "f2", "F2" }, { k_port_key_f3, "f3", "F3" },
    { k_port_key_f4, "f4", "F4" }, { k_port_key_f5, "f5", "F5" }, { k_port_key_f6, "f6", "F6" },
    { k_port_key_f7, "f7", "F7" }, { k_port_key_f8, "f8", "F8" }, { k_port_key_f9, "f9", "F9" },
    { k_port_key_f10, "f10", "F10" }, { k_port_key_f11, "f11", "F11" }, { k_port_key_f12, "f12", "F12" },
    { k_port_key_escape, "escape", "ESC" }, { k_port_key_tab, "tab", "TAB" }, { k_port_key_space, "space", "SPACE" },
    { k_port_key_return, "return", "ENTER" }, { k_port_key_backspace, "backspace", "BACKSPACE" },
    { k_port_key_up, "up", "UP" }, { k_port_key_down, "down", "DOWN" }, { k_port_key_left, "left", "LEFT" },
    { k_port_key_right, "right", "RIGHT" },
    { k_port_key_lshift, "lshift", "LEFT SHIFT" }, { k_port_key_rshift, "rshift", "RIGHT SHIFT" },
    { k_port_key_lctrl, "lctrl", "LEFT CTRL" }, { k_port_key_rctrl, "rctrl", "RIGHT CTRL" },
    { k_port_key_lalt, "lalt", "LEFT ALT" }, { k_port_key_ralt, "ralt", "RIGHT ALT" },
    { k_port_key_capslock, "capslock", "CAPS LOCK" }, { k_port_key_home, "home", "HOME" },
    { k_port_key_end, "end", "END" }, { k_port_key_insert, "insert", "INSERT" },
    { k_port_key_delete, "delete", "DELETE" },
    { k_port_key_pageup, "pageup", "PAGE UP" }, { k_port_key_pagedown, "pagedown", "PAGE DOWN" },
    { k_port_key_grave, "grave", "`" }, { k_port_key_minus, "minus", "-" }, { k_port_key_equals, "equals", "=" },
    { k_port_key_lbracket, "lbracket", "[" }, { k_port_key_rbracket, "rbracket", "]" },
    { k_port_key_backslash, "backslash", "\\" }, { k_port_key_semicolon, "semicolon", ";" },
    { k_port_key_apostrophe, "apostrophe", "'" }, { k_port_key_comma, "comma", "," },
    { k_port_key_period, "period", "." },
    { k_port_key_slash, "slash", "/" },
    { PORT_KBM_MOUSE(k_port_mouse_left), "mouse_left", "LMB" },
    { PORT_KBM_MOUSE(k_port_mouse_right), "mouse_right", "RMB" },
    { PORT_KBM_MOUSE(k_port_mouse_middle), "mouse_middle", "MMB" },
    { PORT_KBM_MOUSE(k_port_mouse_4), "mouse4", "MOUSE 4" },
    { PORT_KBM_MOUSE(k_port_mouse_5), "mouse5", "MOUSE 5" },
    { PORT_KBM_WHEEL_UP, "wheel_up", "WHEEL UP" }, { PORT_KBM_WHEEL_DOWN, "wheel_down", "WHEEL DOWN" },
    { PORT_KBM_WHEEL, "wheel", "WHEEL" },
};
#define KBM_NAMED (sizeof(k_named) / sizeof(k_named[0]))

// Other spellings halo3.ini and scripts take.
static const struct {
    const char *name;
    unsigned char code;
} k_aliases[] = {
    { "esc", k_port_key_escape }, { "enter", k_port_key_return }, { "ctrl", k_port_key_lctrl },
    { "control", k_port_key_lctrl }, { "shift", k_port_key_lshift }, { "alt", k_port_key_lalt },
    { "del", k_port_key_delete }, { "lmb", PORT_KBM_MOUSE(k_port_mouse_left) },
    { "rmb", PORT_KBM_MOUSE(k_port_mouse_right) }, { "mmb", PORT_KBM_MOUSE(k_port_mouse_middle) },
    { "mouse1", PORT_KBM_MOUSE(k_port_mouse_left) }, { "mouse2", PORT_KBM_MOUSE(k_port_mouse_right) },
    { "mouse3", PORT_KBM_MOUSE(k_port_mouse_middle) }, { "wheelup", PORT_KBM_WHEEL_UP },
    { "wheeldown", PORT_KBM_WHEEL_DOWN },
};

// "a".."z" and "0".."9": a letter's or digit's name and label, each a NUL-terminated pair of these.
static const char k_lower[] = "a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";
static const char k_upper[] = "A\0B\0C\0D\0E\0F\0G\0H\0I\0J\0K\0L\0M\0N\0O\0P\0Q\0R\0S\0T\0U\0V\0W\0X\0Y\0Z";
static const char k_digits[] = "0\0001\0002\0003\0004\0005\0006\0007\0008\0009";

static const char *letter(int code, int label)
{
    if (code >= k_port_key_a && code <= k_port_key_z)
        return (label ? k_upper : k_lower) + 2 * (code - k_port_key_a);
    if (code >= k_port_key_0 && code <= k_port_key_9)
        return k_digits + 2 * (code - k_port_key_0);
    return 0;
}

static char lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c; }

static int equals_name(const char *text, unsigned long length, const char *name)
{
    unsigned long i = 0;
    for (; i < length && name[i] != 0; i++)
        if (lower(text[i]) != name[i])
            return 0;
    return i == length && name[i] == 0;
}

int port_kbm_code_of(const char *name, unsigned long length)
{
    if (length == 1) {
        const char c = lower(name[0]);
        if (c >= 'a' && c <= 'z')
            return k_port_key_a + (c - 'a');
        if (c >= '0' && c <= '9')
            return k_port_key_0 + (c - '0');
    }
    for (unsigned i = 0; i < KBM_NAMED; i++)
        if (equals_name(name, length, k_named[i].name))
            return k_named[i].code;
    for (unsigned i = 0; i < sizeof(k_aliases) / sizeof(k_aliases[0]); i++)
        if (equals_name(name, length, k_aliases[i].name))
            return k_aliases[i].code;
    return -1;
}

static const char *code_text(int code, int label)
{
    const char *l = letter(code, label);
    if (l != 0)
        return l;
    for (unsigned i = 0; i < KBM_NAMED; i++)
        if (k_named[i].code == code)
            return label ? k_named[i].label : k_named[i].name;
    return "";
}

const char *port_kbm_code_name(int code) { return code_text(code, 0); }
const char *port_kbm_code_label(int code) { return code_text(code, 1); }

int port_kbm_codes_overlap(int a, int b)
{
    if (a == PORT_KBM_NONE || b == PORT_KBM_NONE)
        return 0;
    if (a == b)
        return 1;
    const int wheel_a = a == PORT_KBM_WHEEL_UP || a == PORT_KBM_WHEEL_DOWN;
    const int wheel_b = b == PORT_KBM_WHEEL_UP || b == PORT_KBM_WHEEL_DOWN;
    return (a == PORT_KBM_WHEEL && wheel_b) || (b == PORT_KBM_WHEEL && wheel_a);
}

// ---- the actions ----

static const struct {
    const char *key, *label, *preset[PORT_KBM_PRESET_COUNT];
} k_actions[PORT_KBM_ACTION_COUNT] = {
#define PORT_KBM_ROW(id, key, label, modern, classic) { key, label, { modern, classic } },
    PORT_KBM_ACTIONS(PORT_KBM_ROW)
#undef PORT_KBM_ROW
};

#define HALO(action) (1ul << (action))

// What each action does in the game. Jump, Melee, Switch Grenade and Action / Reload carry the actions Halo 3 puts on
// the same controller button in its standard layout (A: jump, accept, skip a cinematic; B: melee and its seat and
// cancel forms; LB: switch grenades, and the left hand's reload / pick up when dual wielding), so the keyboard does all
// a button does; Jump and Crouch rise and descend where the game flies (the Forge monitor), as on PC's Forge.
static const struct {
    unsigned long halo;
    unsigned char contexts, role;
    unsigned short controller_button;
} k_meaning[PORT_KBM_ACTION_COUNT] = {
    [PORT_KBM_MOVE_FORWARD] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_AXIS, 0 },
    [PORT_KBM_MOVE_BACK] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_AXIS, 0 },
    [PORT_KBM_MOVE_LEFT] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_AXIS, 0 },
    [PORT_KBM_MOVE_RIGHT] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_AXIS, 0 },
    [PORT_KBM_JUMP] = { HALO(PORT_HALO_JUMP) | HALO(PORT_HALO_ACCEPT) | HALO(PORT_HALO_SKIP_CINEMATIC) |
                            HALO(PORT_HALO_RISE),
                        PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_CROUCH] = { HALO(PORT_HALO_CROUCH) | HALO(PORT_HALO_DESCEND), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON,
                          0 },
    [PORT_KBM_FIRE] = { HALO(PORT_HALO_FIRE), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_ZOOM] = { HALO(PORT_HALO_ZOOM),
                        PORT_KBM_CONTEXT_FOOT | PORT_KBM_CONTEXT_SEAT | PORT_KBM_CONTEXT_SEAT_ARMED,
                        PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_FIRE_LEFT] = { HALO(PORT_HALO_FIRE_LEFT), PORT_KBM_CONTEXT_DUAL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_MELEE] = { HALO(PORT_HALO_MELEE) | HALO(PORT_HALO_MELEE_FIRE) | HALO(PORT_HALO_CANCEL),
                         PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_THROW_GRENADE] = { HALO(PORT_HALO_THROW_GRENADE),
                                 PORT_KBM_CONTEXT_FOOT | PORT_KBM_CONTEXT_SEAT | PORT_KBM_CONTEXT_SEAT_ARMED,
                                 PORT_KBM_ROLE_BUTTON, 0 },
    // a driver's tap of the action button leaves the seat: Reload applies where there is a weapon in hand to reload
    [PORT_KBM_RELOAD] = { HALO(PORT_HALO_ACTION_RELOAD),
                          PORT_KBM_CONTEXT_FOOT | PORT_KBM_CONTEXT_DUAL | PORT_KBM_CONTEXT_SEAT_ARMED,
                          PORT_KBM_ROLE_TAP, 0 },
    [PORT_KBM_ACTION] = { HALO(PORT_HALO_ACTION_RELOAD), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_USE_EQUIPMENT] = { HALO(PORT_HALO_USE_EQUIPMENT), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_SWITCH_WEAPON] = { HALO(PORT_HALO_SWITCH_WEAPON), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_SWITCH_GRENADE] = { HALO(PORT_HALO_SWITCH_GRENADE) | HALO(PORT_HALO_ACTION_RELOAD_LEFT),
                                  PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_FLASHLIGHT] = { HALO(PORT_HALO_FLASHLIGHT), PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_VEHICLE_BOOST] = { HALO(PORT_HALO_FIRE_LEFT), PORT_KBM_CONTEXT_SEAT | PORT_KBM_CONTEXT_SEAT_ARMED,
                                 PORT_KBM_ROLE_BUTTON, 0 },
    [PORT_KBM_DPAD_UP] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_DPAD_UP },
    [PORT_KBM_DPAD_DOWN] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_DPAD_DOWN },
    [PORT_KBM_DPAD_LEFT] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_DPAD_LEFT },
    [PORT_KBM_DPAD_RIGHT] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_DPAD_RIGHT },
    [PORT_KBM_SCOREBOARD] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_BACK },
    [PORT_KBM_PAUSE] = { 0, PORT_KBM_CONTEXT_ALL, PORT_KBM_ROLE_CONTROLLER, PORT_XINPUT_START },
};

static int valid_action(int action) { return action >= 0 && action < PORT_KBM_ACTION_COUNT; }

int port_kbm_preset_of(const char *name, unsigned long length)
{
    if (equals_name(name, length, "modern"))
        return PORT_KBM_PRESET_MODERN;
    if (equals_name(name, length, "classic"))
        return PORT_KBM_PRESET_CLASSIC;
    return -1;
}

const char *port_kbm_preset_name(int preset)
{
    return preset == PORT_KBM_PRESET_CLASSIC ? "classic" : "modern";
}

const char *port_kbm_preset_label(int preset)
{
    return preset == PORT_KBM_PRESET_CLASSIC ? "Classic" : "Modern";
}

const char *port_kbm_action_key(int action) { return valid_action(action) ? k_actions[action].key : ""; }
const char *port_kbm_action_label(int action) { return valid_action(action) ? k_actions[action].label : ""; }

const char *port_kbm_action_default(int preset, int action)
{
    if (!valid_action(action))
        return "";
    const int which = preset == PORT_KBM_PRESET_CLASSIC ? PORT_KBM_PRESET_CLASSIC : PORT_KBM_PRESET_MODERN;
    return k_actions[action].preset[which];
}

int port_kbm_action_fixed(int action) { return action == PORT_KBM_PAUSE; }

unsigned long port_kbm_action_halo(int action) { return valid_action(action) ? k_meaning[action].halo : 0; }
unsigned port_kbm_action_contexts(int action) { return valid_action(action) ? k_meaning[action].contexts : 0; }
int port_kbm_action_role(int action) { return valid_action(action) ? k_meaning[action].role : PORT_KBM_ROLE_BUTTON; }

unsigned short port_kbm_action_controller_button(int action)
{
    return valid_action(action) ? k_meaning[action].controller_button : 0;
}

int port_kbm_compatible(int a, int b)
{
    return valid_action(a) && valid_action(b) && a != b && (k_meaning[a].contexts & k_meaning[b].contexts) == 0;
}

// ---- a set of bindings ----

static int space(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

int port_kbm_parse(const char *text, unsigned long length, unsigned char codes[PORT_KBM_BINDS])
{
    unsigned char parsed[PORT_KBM_BINDS] = { 0 };
    int count = 0;
    unsigned long at = 0;
    while (length > 0 && space(text[length - 1]))
        length--;
    while (at < length && space(text[at]))
        at++;
    if (at == length) { // "" : no input
        for (int i = 0; i < PORT_KBM_BINDS; i++)
            codes[i] = PORT_KBM_NONE;
        return 1;
    }
    for (;;) {
        unsigned long end = at;
        while (end < length && text[end] != ',')
            end++;
        unsigned long first = at, last = end;
        while (first < last && space(text[first]))
            first++;
        while (last > first && space(text[last - 1]))
            last--;
        if (count == PORT_KBM_BINDS)
            return 0;
        if (last == first || equals_name(text + first, last - first, "none")) {
            parsed[count++] = PORT_KBM_NONE;
        } else {
            const int code = port_kbm_code_of(text + first, last - first);
            if (code < 0)
                return 0;
            parsed[count++] = (unsigned char)code;
        }
        if (end == length)
            break;
        at = end + 1;
    }
    // an input given twice is one
    for (int i = 1; i < count; i++)
        for (int j = 0; j < i; j++)
            if (parsed[i] != PORT_KBM_NONE && port_kbm_codes_overlap(parsed[i], parsed[j]))
                parsed[i] = PORT_KBM_NONE;
    for (int i = 0; i < PORT_KBM_BINDS; i++)
        codes[i] = parsed[i];
    return 1;
}

static unsigned long append(char *out, unsigned long capacity, unsigned long at, const char *text)
{
    while (*text != 0 && at + 1 < capacity)
        out[at++] = *text++;
    if (capacity > 0)
        out[at < capacity ? at : capacity - 1] = 0;
    return at;
}

static unsigned long format(const unsigned char codes[PORT_KBM_BINDS], char *out, unsigned long capacity, int label)
{
    if (capacity == 0)
        return 0;
    out[0] = 0;
    // the last input given: an empty primary before an alternate stays ("none, wheel"), empty alternates go
    int last = -1;
    for (int i = 0; i < PORT_KBM_BINDS; i++)
        if (codes[i] != PORT_KBM_NONE)
            last = i;
    if (last < 0)
        return append(out, capacity, 0, label ? "UNBOUND" : "none");
    unsigned long at = 0;
    for (int i = 0; i <= last; i++) {
        if (i > 0)
            at = append(out, capacity, at, label ? " / " : ", ");
        at = append(out, capacity, at, codes[i] == PORT_KBM_NONE ? (label ? "-" : "none")
                                                                  : label ? port_kbm_code_label(codes[i])
                                                                          : port_kbm_code_name(codes[i]));
    }
    return at;
}

unsigned long port_kbm_format(const unsigned char codes[PORT_KBM_BINDS], char *out, unsigned long capacity)
{
    return format(codes, out, capacity, 0);
}

unsigned long port_kbm_format_label(const unsigned char codes[PORT_KBM_BINDS], char *out, unsigned long capacity)
{
    return format(codes, out, capacity, 1);
}

void port_kbm_preset_bindings(int preset, port_kbm_bindings *out)
{
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
        const char *text = port_kbm_action_default(preset, a);
        unsigned long length = 0;
        while (text[length] != 0)
            length++;
        if (!port_kbm_parse(text, length, out->codes[a])) // (the table's own text: always parses)
            for (int i = 0; i < PORT_KBM_BINDS; i++)
                out->codes[a][i] = PORT_KBM_NONE;
    }
}

int port_kbm_assign(port_kbm_bindings *bindings, int action, int slot, int code, port_kbm_assign_result *result)
{
    result->changed = 0;
    result->other = -1;
    result->other_slot = 0;
    result->other_code = PORT_KBM_NONE;
    if (!valid_action(action) || port_kbm_action_fixed(action) || slot < 0 || slot >= PORT_KBM_BINDS || code < 0 ||
        code >= PORT_KBM_CODE_COUNT || code == k_port_key_escape)
        return 0;
    unsigned char *codes = bindings->codes[action];
    if (codes[slot] == code)
        return 1;
    const int old = codes[slot];
    // Escape is the pause menu's (never captured; a hand-edited file could give it, or another input, to Pause Menu)
    for (int i = 0; i < PORT_KBM_BINDS && code != PORT_KBM_NONE; i++)
        for (int other = 0; other < PORT_KBM_ACTION_COUNT; other++)
            if (port_kbm_action_fixed(other) && port_kbm_codes_overlap(bindings->codes[other][i], code))
                return 0;
    if (code != PORT_KBM_NONE) {
        // the action's other slot gives up the same input
        for (int i = 0; i < PORT_KBM_BINDS; i++)
            if (i != slot && port_kbm_codes_overlap(codes[i], code))
                codes[i] = PORT_KBM_NONE;
        // an action that cannot share it gives it up, taking the old input in exchange when that is free for it
        for (int other = 0; other < PORT_KBM_ACTION_COUNT; other++) {
            if (other == action || port_kbm_compatible(action, other))
                continue;
            for (int i = 0; i < PORT_KBM_BINDS; i++) {
                if (!port_kbm_codes_overlap(bindings->codes[other][i], code))
                    continue;
                int exchange = old;
                for (int j = 0; j < PORT_KBM_BINDS && exchange != PORT_KBM_NONE; j++)
                    if (j != i && port_kbm_codes_overlap(bindings->codes[other][j], exchange))
                        exchange = PORT_KBM_NONE; // it has that one already
                for (int third = 0; third < PORT_KBM_ACTION_COUNT && exchange != PORT_KBM_NONE; third++) {
                    if (third == action || third == other || port_kbm_compatible(other, third))
                        continue;
                    for (int j = 0; j < PORT_KBM_BINDS; j++)
                        if (port_kbm_codes_overlap(bindings->codes[third][j], exchange))
                            exchange = PORT_KBM_NONE; // another action keeps it
                }
                bindings->codes[other][i] = (unsigned char)exchange;
                if (result->other < 0) {
                    result->other = other;
                    result->other_slot = i;
                    result->other_code = exchange;
                }
            }
        }
    }
    codes[slot] = (unsigned char)code;
    result->changed = 1;
    return 1;
}
