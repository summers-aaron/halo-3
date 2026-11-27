// port/sdk/runtime (both hosts): halo3.ini and the launch's defaults (port_config.h).
//
// halo3.ini is a plain INI file: [section] lines, key = value lines, and comment lines that start with ';' or '#'. A
// value may be quoted ("C:\Games\Halo 3"); unquoted, a ';' or '#' after a space starts a comment. Keys and sections
// are matched ignoring case; a key given twice takes its last value. What halo3.ini cannot take (an unknown key, a
// value out of range) is reported on stderr with its line and left out: the setting keeps its default.
//
// Writing it (port_config_save) keeps the file as the player left it, comments and unknown keys included: each
// setting's line gets the new value where it is, a setting the file lacks is added at the end of its section (a
// section it lacks at the end of the file), and a new file has every setting under a comment saying what it does.
#include "port_config.h"
#include "config_host.h"
#include "port_runtime.h"
#include "../kernel/storage.h"
#include "../kernel/storage_host.h"

#define CONFIG_ERROR_SUCCESS 0ul
#define CONFIG_ERROR_FILE_NOT_FOUND 2ul
#define CONFIG_ERROR_PATH_NOT_FOUND 3ul
#define CONFIG_ERROR_NOT_SUPPORTED 50ul
#define CONFIG_ERROR_INSUFFICIENT_BUFFER 122ul

#define CONFIG_FILE_MAX 32768ul   // bytes of halo3.ini read at most (a larger file is not read, and not overwritten)
#define CONFIG_TEXT_MAX 49152ul   // bytes of halo3.ini written at most
#define CONFIG_FILE_NAME "halo3.ini"

typedef unsigned long config_size;

// ---- the settings ----

enum { k_kind_choice, k_kind_number, k_kind_rate, k_kind_flag, k_kind_text, k_kind_path, k_kind_resolution,
       k_kind_decimal, k_kind_binding };

// [input] decimal settings: hundredths (written with one decimal, or two when they have them).
#define CONFIG_SENSITIVITY_MIN 10ul   // 0.1 .. 10.0
#define CONFIG_SENSITIVITY_MAX 1000ul
#define CONFIG_ACCELERATION_MIN 0ul   // 0.0 (off) .. 2.0
#define CONFIG_ACCELERATION_MAX 200ul
#define CONFIG_MULTIPLIER_MIN 10ul    // 0.1 .. 3.0 (zoom, vehicle)
#define CONFIG_MULTIPLIER_MAX 300ul

// [display] resolution: "native" (the game's own 1152x640 frame, shown 16:9) or a size WxH the game renders itself at,
// in square pixels. Both sides even (the scene depth's 4x view and the split-screen views halve them), within these
// bounds; the pixel count at most that of 5120x2160 (the render targets of 4K and 5K ultrawide fit the 32-bit game's
// address space, measured in tmp/custom_resolutions).
#define CONFIG_RESOLUTION_MIN_WIDTH 640ul
#define CONFIG_RESOLUTION_MIN_HEIGHT 360ul
#define CONFIG_RESOLUTION_MAX_WIDTH 7680ul
#define CONFIG_RESOLUTION_MAX_HEIGHT 4320ul
#define CONFIG_RESOLUTION_MAX_PIXELS (5120ul * 2160ul)
enum { k_section_display, k_section_audio, k_section_network, k_section_paths, k_section_input, k_section_bindings,
       k_section_count };

static const char *const k_sections[k_section_count] = { "display", "audio", "network", "paths", "input", "bindings" };
static const char *const k_window_modes[] = { "windowed", "borderless", "fullscreen", 0 };
static const char *const k_vsync_modes[] = { "auto", "1", "0", 0 };
static const char *const k_renderers[] = { "auto", "vulkan", "d3d12", 0 };
static const char *const k_draw_distances[] = { "1", "2", "4", 0 };
static const char *const k_anisotropies[] = { "0", "4", "8", "16", 0 };
static const char *const k_shadow_resolutions[] = { "512", "1024", "2048", 0 };
static const char *const k_kbm_presets[] = { "modern", "classic", 0 }; // port_kbm_preset's order

typedef struct {
    int section;
    const char *key, *variable;
    int kind, live;
    unsigned long minimum, maximum; // k_kind_number
    const char *const *choices;     // k_kind_choice: its values, as written
    const char *fallback;           // the compiled default
    const char *comment;            // what it does, over its line in a new file ('\n' between lines)
} config_setting;

static const config_setting k_settings[PORT_SETTING_COUNT] = {
    { k_section_display, "window_mode", "HALO3_WINDOW_MODE", k_kind_choice, 1, 0, 0, k_window_modes, "fullscreen",
      "windowed, borderless (no frame, filling the screen's work area) or fullscreen" },
    { k_section_display, "resolution", "HALO3_RESOLUTION", k_kind_resolution, 1, 0, 0, 0, "native",
      "native (the game's own 1152x640, shown 16:9; res_scale applies) or the size the game renders at:\n"
      "1280x720, 1920x1080, 2560x1440, 3840x2160, ultrawide 2560x1080 or 3440x1440, ... (res_scale is then 1)" },
    { k_section_display, "res_scale", "HALO3_RES_SCALE", k_kind_number, 0, 1, 3, 0, "1",
      "with resolution = native: 1 (the game's own 1152x640), 2 (about 1440p) or 3 (about 4K)" },
    { k_section_display, "fps_cap", "HALO3_FPS", k_kind_rate, 1, 30, 1000, 0, "30",
      "frames a second: 30 (as on the console), 60, 120, 144, 240 (up to 1000), or 0 for no cap" },
    { k_section_display, "vsync", "HALO3_VSYNC", k_kind_choice, 1, 0, 0, k_vsync_modes, "auto",
      "vertical sync: auto (on at 30 frames a second, off above), 1 (on) or 0 (off)" },
    { k_section_display, "renderer", "HALO3_RENDERER", k_kind_choice, 0, 0, 0, k_renderers, "auto",
      "the graphics API: auto (Vulkan), vulkan, or d3d12 (Direct3D 12, Windows; Vulkan when it cannot start)" },
    { k_section_display, "hd_text", "HALO3_HD_TEXT", k_kind_flag, 1, 0, 0, 0, "1",
      "1: use high-resolution menu and Guide fonts where available (Linux); 0: use the original fonts" },
    { k_section_display, "draw_distance", "HALO3_DRAW_DISTANCE", k_kind_choice, 1, 0, 0, k_draw_distances, "1",
      "object and decorator draw distance: 1 (original), 2 (high), or 4 (ultra)" },
    { k_section_display, "anisotropy", "HALO3_ANISOTROPY", k_kind_choice, 1, 0, 0, k_anisotropies, "0",
      "anisotropic texture filtering: 0 (original), 4 (4x), 8 (8x), or 16 (16x)" },
    { k_section_display, "shadow_resolution", "HALO3_SHADOW_RES", k_kind_choice, 0, 0, 0, k_shadow_resolutions, "512",
      "shadow map resolution: 512 (original), 1024 (high), or 2048 (ultra)" },
    { k_section_display, "splitscreen_quality", "HALO3_SPLITSCREEN_QUALITY", k_kind_flag, 1, 0, 0, 0, "0",
      "split-screen graphics quality: 0 (original console reductions) or 1 (full single-player quality)" },
    { k_section_display, "splitscreen_symmetric_hud", "HALO3_SPLITSCREEN_SYMMETRIC_HUD", k_kind_flag, 1, 0, 0, 0, "1",
      "split-screen HUD margins: 0 (retail TV overscan) or 1 (symmetric margins in each player's view)" },
    { k_section_audio, "backend", "HALO3_AUDIO_BACKEND", k_kind_text, 0, 0, 0, 0, "auto",
      "auto (the system's sound), null (no sound), pulse, alsa or pipe (Linux), winmm (Windows)" },
    { k_section_audio, "microphone", "HALO3_MIC_DEVICE", k_kind_text, 1, 0, 0, 0, "off",
      "voice chat microphone: off (disabled), default (system default microphone), or a capture device name/ID" },
    { k_section_network, "lan", "HALO3_LAN", k_kind_flag, 0, 0, 0, 0, "1",
      "System Link (LAN) play: 1 on, 0 off" },
    { k_section_network, "lan_port", "HALO3_LAN_PORT", k_kind_number, 0, 1, 65535, 0, "3074",
      "the first UDP port System Link uses" },
    { k_section_network, "lan_peer", "HALO3_LAN_PEER", k_kind_text, 0, 0, 0, 0, "",
      "other PCs to reach directly when broadcasts do not (a VPN): ip[:port], comma-separated" },
    { k_section_network, "live_server", "HALO3_LIVE_SERVER", k_kind_text, 0, 0, 0, 0, "https://slopboxlive.com",
      "a Live server (https://host or http://host:port); empty: Live is off" },
    { k_section_network, "coop_4p", "HALO3_COOP_4P", k_kind_flag, 1, 0, 0, 0, "1",
      "4-player local and System Link campaign co-op: 1 on, 0 off" },
    { k_section_paths, "maps_dir", "HALO3_DISC", k_kind_path, 0, 0, 0, 0, "",
      "the game folder (with maps/ and bink/ in it), or its maps folder" },
    { k_section_paths, "shaders_pack", "HALO3_SHADER_PACK", k_kind_path, 0, 0, 0, 0, "",
      "the shader pack file (halo3_shaders_xenos.pack)" },
    { k_section_paths, "shaders_pack_dxil", "HALO3_SHADER_PACK_DXIL", k_kind_path, 0, 0, 0, 0, "",
      "the Direct3D 12 renderer's shader pack file (halo3_shaders_dxil.pack; Windows)" },
    { k_section_paths, "userdata_dir", "HALO3_DATA", k_kind_path, 0, 0, 0, 0, "",
      "the folder for profiles, saved games and caches" },
    { k_section_input, "raw_mouse", "HALO3_RAW_MOUSE", k_kind_flag, 1, 0, 0, 0, "1",
      "1: the mouse turns the view by its own motion (no acceleration, no dead zone); 0: it moves a right stick" },
    { k_section_input, "mouse_sensitivity", "HALO3_MOUSE_SENS", k_kind_decimal, 1,
      CONFIG_SENSITIVITY_MIN, CONFIG_SENSITIVITY_MAX, 0, "1.0",
      "0.1 to 10.0; with raw_mouse, 1.0 turns 0.022 degrees per mouse count (the Quake / Source scale)" },
    { k_section_input, "mouse_acceleration", "HALO3_MOUSE_ACCEL", k_kind_decimal, 1,
      CONFIG_ACCELERATION_MIN, CONFIG_ACCELERATION_MAX, 0, "0.0",
      "0.0 (off) to 2.0; scales mouse look speed up with faster movement (0.0 leaves input completely linear)" },
    { k_section_input, "mouse_zoom_sensitivity", "HALO3_MOUSE_ZOOM_SENS", k_kind_decimal, 1,
      CONFIG_MULTIPLIER_MIN, CONFIG_MULTIPLIER_MAX, 0, "1.0",
      "0.1 to 3.0; mouse look sensitivity multiplier while zoomed/scoped (1.0: unchanged)" },
    { k_section_input, "mouse_vehicle_sensitivity", "HALO3_MOUSE_VEHICLE_SENS", k_kind_decimal, 1,
      CONFIG_MULTIPLIER_MIN, CONFIG_MULTIPLIER_MAX, 0, "1.0",
      "0.1 to 3.0; mouse look sensitivity multiplier while in a vehicle seat (1.0: unchanged)" },
    { k_section_input, "invert_mouse_y", "HALO3_INVERT_MOUSE_Y", k_kind_flag, 1, 0, 0, 0, "0",
      "1: moving the mouse forward looks down (the controller's inversion is the profile's)" },
    { k_section_input, "crouch_hold", "HALO3_CROUCH_HOLD", k_kind_flag, 1, 0, 0, 0, "1",
      "1: hold the crouch key to crouch; 0: a press crouches and the next stands" },
    { k_section_input, "kbm_preset", "HALO3_KBM_PRESET", k_kind_choice, 1, 0, 0, k_kbm_presets, "modern",
      "the keyboard layout the bindings start from: modern or classic (grenade on the\n"
      "right mouse button, zoom on the middle one); a binding left out below takes this preset's" },
#define CONFIG_BINDING_ROW(id, key, label, modern, classic) \
    { k_section_bindings, key, "", k_kind_binding, 1, 0, 0, 0, modern, "" },
    PORT_KBM_ACTIONS(CONFIG_BINDING_ROW)
#undef CONFIG_BINDING_ROW
};
typedef char config_settings_complete[sizeof(k_settings) / sizeof(k_settings[0]) == PORT_SETTING_COUNT ? 1 : -1];

static int binding_setting(int setting)
{
    return setting >= PORT_SETTING_BINDING_FIRST && setting < PORT_SETTING_COUNT;
}
// These settings still have subsystem readers that take their start-up values from the environment.
static int exported(int setting)
{
    return k_settings[setting].section < k_section_input && setting != PORT_SETTING_HD_TEXT &&
           setting != PORT_SETTING_DRAW_DISTANCE && setting != PORT_SETTING_ANISOTROPY &&
           setting != PORT_SETTING_SHADOW_RES && setting != PORT_SETTING_SPLITSCREEN_QUALITY &&
           setting != PORT_SETTING_SPLITSCREEN_SYMMETRIC_HUD && setting != PORT_SETTING_MIC_DEVICE && setting != PORT_SETTING_COOP_4P;
}

static const char k_file_comment[] =
    "Halo 3 settings, read as the game starts. The Guide's Settings page (the Guide button, or Home on the\n"
    "keyboard) changes the display, System Link and keyboard and mouse settings and saves them here.\n"
    "A launch option wins over this file: a HALO3_* environment variable that is set (HALO3_FPS=60, ...)\n"
    "replaces the setting it names for that launch.";
static const char k_paths_comment[] = "empty: found by the game. A relative path starts at this file's folder.";
static const char k_input_comment[] =
    "the keyboard and mouse (the Guide: Settings > Keyboard & Mouse). A controller keeps its own button layout.";
static const char k_bindings_comment[] =
    "the keyboard and mouse bindings: a key (a..z, 0..9, f1..f12, space, return, escape, tab, backspace, lshift,\n"
    "rshift, lctrl, rctrl, lalt, ralt, up, down, left, right, ...) or mouse_left, mouse_right, mouse_middle, mouse4,\n"
    "mouse5, wheel_up, wheel_down, wheel (either way); two, comma-separated, for an alternate; none for no input.\n"
    "pause (the pause menu) is always escape, which also cancels a rebinding in the Guide.";

// ---- state ----

static struct {
    volatile char busy;     // the values below (config_lock)
    int initialized;
    int active;             // a halo3.ini is in use: read at start-up, written by port_config_save
    int interactive;        // a player's launch (port_config.h)
    int portable;
    int crlf;               // write line ends as CRLF (a new file on Windows, or a file that has them)
    unsigned long revision; // port_config_revision
    char path[PORT_STORAGE_PATH];      // halo3.ini, "" when none is in use
    char directory[PORT_STORAGE_PATH]; // its folder: where a relative path starts
    char root[PORT_STORAGE_PATH];      // the portable game folder, "" when not portable
    char value[PORT_SETTING_COUNT][PORT_CONFIG_VALUE];  // as the player chose it (halo3.ini, the Guide)
    char launch[PORT_SETTING_COUNT][PORT_CONFIG_VALUE]; // in effect since the launch
    unsigned char locked[PORT_SETTING_COUNT];           // the launch set its variable
    unsigned char given[PORT_SETTING_COUNT];            // halo3.ini has it (a binding it lacks takes the preset's)
} g_config;

static volatile char g_save_busy; // one port_config_save at a time (g_file, g_text)
static char g_file[CONFIG_FILE_MAX];
static char g_text[CONFIG_TEXT_MAX];

static void config_lock(void)
{
    while (__atomic_test_and_set(&g_config.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void config_unlock(void) { __atomic_clear(&g_config.busy, __ATOMIC_RELEASE); }

// ---- text ----

static config_size length_of(const char *text)
{
    config_size n = 0;
    while (text[n] != 0)
        n++;
    return n;
}

static void copy_text(char *out, config_size capacity, const char *text, config_size length)
{
    if (capacity == 0)
        return;
    if (length >= capacity)
        length = capacity - 1;
    __builtin_memcpy(out, text, length);
    out[length] = 0;
}

static void copy_string(char *out, config_size capacity, const char *text) { copy_text(out, capacity, text, length_of(text)); }

static char lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c; }
static int space(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f'; }

// `text` (`length` bytes) equals `word` ignoring case.
static int equals_word(const char *text, config_size length, const char *word)
{
    config_size i = 0;
    for (; i < length && word[i] != 0; i++)
        if (lower(text[i]) != lower(word[i]))
            return 0;
    return i == length && word[i] == 0;
}

static int same(const char *a, const char *b)
{
    while (*a != 0 && *a == *b)
        a++, b++;
    return *a == *b;
}

// The value `text` (`length` bytes) without the spaces around it and, quoted, without its quotes.
static void trimmed(const char **text, config_size *length)
{
    const char *t = *text;
    config_size n = *length;
    while (n > 0 && space(t[0]))
        t++, n--;
    while (n > 0 && space(t[n - 1]))
        n--;
    if (n >= 2 && t[0] == '"' && t[n - 1] == '"')
        t++, n -= 2;
    *text = t;
    *length = n;
}

// A decimal number, all of `text`. Returns 0 when it is not one (or is over 9 digits).
static int number_of(const char *text, config_size length, unsigned long *out)
{
    if (length == 0 || length > 9)
        return 0;
    unsigned long n = 0;
    for (config_size i = 0; i < length; i++) {
        if (text[i] < '0' || text[i] > '9')
            return 0;
        n = n * 10ul + (unsigned long)(text[i] - '0');
    }
    *out = n;
    return 1;
}

static void number_text(unsigned long n, char *out)
{
    char digits[12];
    int count = 0;
    do {
        digits[count++] = (char)('0' + n % 10ul);
        n /= 10ul;
    } while (n != 0);
    for (int i = 0; i < count; i++)
        out[i] = digits[count - 1 - i];
    out[count] = 0;
}

// A decimal ("1", "2.5", ".75", "1.25"), all of `text`, in hundredths (a third decimal rounds). Returns 0 when it is
// not one.
static int decimal_of(const char *text, config_size length, unsigned long *hundredths)
{
    unsigned long whole = 0, fraction = 0;
    config_size i = 0, digits = 0, decimals = 0;
    for (; i < length && text[i] >= '0' && text[i] <= '9'; i++, digits++) {
        if (digits >= 6)
            return 0;
        whole = whole * 10ul + (unsigned long)(text[i] - '0');
    }
    if (i < length && text[i] == '.') {
        for (i++; i < length && text[i] >= '0' && text[i] <= '9'; i++, decimals++) {
            if (decimals < 3)
                fraction = fraction * 10ul + (unsigned long)(text[i] - '0');
        }
    }
    if (i != length || digits + decimals == 0)
        return 0;
    for (config_size d = decimals; d < 3; d++)
        fraction *= 10ul; // thousandths
    *hundredths = whole * 100ul + (fraction + 5ul) / 10ul;
    return 1;
}

// Hundredths as halo3.ini writes them: "1.0", "2.5", "0.75".
static void decimal_text(unsigned long hundredths, char *out)
{
    number_text(hundredths / 100ul, out);
    config_size at = length_of(out);
    out[at++] = '.';
    out[at++] = (char)('0' + hundredths / 10ul % 10ul);
    if (hundredths % 10ul != 0)
        out[at++] = (char)('0' + hundredths % 10ul);
    out[at] = 0;
}

// on / off as a flag's words ("1", "true", "yes", "on"): 1, 0, or -1 for neither.
static int flag_of(const char *text, config_size length)
{
    if (equals_word(text, length, "1") || equals_word(text, length, "on") || equals_word(text, length, "true") ||
        equals_word(text, length, "yes"))
        return 1;
    if (equals_word(text, length, "0") || equals_word(text, length, "off") || equals_word(text, length, "false") ||
        equals_word(text, length, "no"))
        return 0;
    return -1;
}

// A resolution "WxH" ('x' or 'X', spaces around it allowed: "1920 x 1080") the game can render at (the bounds at the
// top). Returns 0 for anything else, and for "native" (the caller's case).
static int resolution_of(const char *text, config_size length, unsigned long *width, unsigned long *height)
{
    config_size x = 0;
    while (x < length && text[x] != 'x' && text[x] != 'X')
        x++;
    if (x == length)
        return 0;
    const char *w = text, *h = text + x + 1;
    config_size w_length = x, h_length = length - x - 1;
    trimmed(&w, &w_length);
    trimmed(&h, &h_length);
    unsigned long wide, high;
    if (!number_of(w, w_length, &wide) || !number_of(h, h_length, &high))
        return 0;
    if (wide < CONFIG_RESOLUTION_MIN_WIDTH || wide > CONFIG_RESOLUTION_MAX_WIDTH || high < CONFIG_RESOLUTION_MIN_HEIGHT ||
        high > CONFIG_RESOLUTION_MAX_HEIGHT || (wide & 1ul) != 0 || (high & 1ul) != 0 ||
        wide * high > CONFIG_RESOLUTION_MAX_PIXELS)
        return 0;
    *width = wide;
    *height = high;
    return 1;
}

// `text` (`length` bytes, as halo3.ini, a variable or the Guide gives it) as setting `setting` keeps it, in `out`.
// Returns 0 for a value the setting cannot take.
static int normalize(int setting, const char *text, config_size length, char out[PORT_CONFIG_VALUE])
{
    const config_setting *s = &k_settings[setting];
    trimmed(&text, &length);
    unsigned long n;
    switch (s->kind) {
    case k_kind_resolution: {
        unsigned long width, height;
        if (length == 0 || equals_word(text, length, "native")) {
            copy_string(out, PORT_CONFIG_VALUE, "native");
            return 1;
        }
        if (!resolution_of(text, length, &width, &height))
            return 0;
        number_text(width, out);
        config_size at = length_of(out);
        out[at++] = 'x';
        number_text(height, out + at);
        return 1;
    }
    case k_kind_choice:
        if (setting == PORT_SETTING_VSYNC && flag_of(text, length) >= 0) {
            copy_string(out, PORT_CONFIG_VALUE, flag_of(text, length) ? "1" : "0");
            return 1;
        }
        for (int i = 0; s->choices[i] != 0; i++) {
            if (equals_word(text, length, s->choices[i])) {
                copy_string(out, PORT_CONFIG_VALUE, s->choices[i]);
                return 1;
            }
        }
        return 0;
    case k_kind_number:
    case k_kind_rate:
        if (!number_of(text, length, &n))
            return 0;
        if (!(n >= s->minimum && n <= s->maximum) && !(s->kind == k_kind_rate && n == 0))
            return 0;
        number_text(n, out);
        return 1;
    case k_kind_flag: {
        const int on = flag_of(text, length);
        if (on < 0)
            return 0;
        copy_string(out, PORT_CONFIG_VALUE, on ? "1" : "0");
        return 1;
    }
    case k_kind_decimal:
        if (setting == PORT_SETTING_MOUSE_ACCELERATION && equals_word(text, length, "off"))
            n = 0;
        else if (!decimal_of(text, length, &n) || n < s->minimum || n > s->maximum)
            return 0;
        decimal_text(n, out);
        return 1;
    case k_kind_binding: {
        unsigned char codes[PORT_KBM_BINDS];
        if (!port_kbm_parse(text, length, codes))
            return 0;
        port_kbm_format(codes, out, PORT_CONFIG_VALUE);
        // a fixed action (the pause menu) keeps its input: escape is also the Guide's cancel
        return !port_kbm_action_fixed(setting - PORT_SETTING_BINDING_FIRST) || same(out, k_settings[setting].fallback);
    }
    case k_kind_text:
    case k_kind_path:
        if (length >= PORT_CONFIG_VALUE)
            return 0;
        if (setting == PORT_SETTING_AUDIO_BACKEND && (length == 0 || equals_word(text, length, "auto"))) {
            copy_string(out, PORT_CONFIG_VALUE, "auto");
            return 1;
        }
        if (setting == PORT_SETTING_MIC_DEVICE) {
            if (length == 0 || flag_of(text, length) == 0 || equals_word(text, length, "none")) {
                copy_string(out, PORT_CONFIG_VALUE, "off");
                return 1;
            }
            if (equals_word(text, length, "default") || equals_word(text, length, "auto") ||
                flag_of(text, length) == 1) {
                copy_string(out, PORT_CONFIG_VALUE, "default");
                return 1;
            }
        }
        copy_text(out, PORT_CONFIG_VALUE, text, length);
        if (setting == PORT_SETTING_AUDIO_BACKEND) // the known names as the audio layer spells them
            for (config_size i = 0; i < length && out[i] != ':'; i++)
                out[i] = lower(out[i]);
        return 1;
    default:
        return 0;
    }
}

static int normalize_string(int setting, const char *text, char out[PORT_CONFIG_VALUE])
{
    return normalize(setting, text, length_of(text), out);
}

// ---- paths ----

// `base` and `name` joined by the host's separator (either separator in them becomes the host's). Returns 0 when it does
// not fit.
static int join(char out[PORT_STORAGE_PATH], const char *base, const char *name)
{
    config_size n = 0;
    for (; base[n] != 0; n++) {
        if (n + 1 >= PORT_STORAGE_PATH)
            return 0;
        out[n] = base[n] == '/' || base[n] == '\\' ? PORT_HOST_SEPARATOR : base[n];
    }
    if (n > 0 && out[n - 1] != PORT_HOST_SEPARATOR) {
        if (n + 1 >= PORT_STORAGE_PATH)
            return 0;
        out[n++] = PORT_HOST_SEPARATOR;
    }
    for (config_size i = 0; name[i] != 0; i++) {
        if (n + 1 >= PORT_STORAGE_PATH)
            return 0;
        out[n++] = name[i] == '/' || name[i] == '\\' ? PORT_HOST_SEPARATOR : name[i];
    }
    out[n] = 0;
    return 1;
}

// `path` with its last component removed (and the separator before it), "" when it has none.
static void parent_of(const char *path, char out[PORT_STORAGE_PATH])
{
    copy_string(out, PORT_STORAGE_PATH, path);
    config_size n = length_of(out);
    while (n > 1 && (out[n - 1] == '/' || out[n - 1] == '\\'))
        out[--n] = 0;
    while (n > 0 && out[n - 1] != '/' && out[n - 1] != '\\')
        n--;
    while (n > 1 && (out[n - 1] == '/' || out[n - 1] == '\\') && !(n == 3 && out[1] == ':'))
        n--;
    out[n] = 0;
}

// A path from halo3.ini: an absolute one as it is, a relative one from halo3.ini's folder.
static int resolve(const char *path, char out[PORT_STORAGE_PATH])
{
    if (port_config_host_absolute(path) || g_config.directory[0] == 0) {
        if (length_of(path) >= PORT_STORAGE_PATH)
            return 0;
        copy_string(out, PORT_STORAGE_PATH, path);
        return 1;
    }
    return join(out, g_config.directory, path);
}

static int file_in(const char *folder, const char *name)
{
    char path[PORT_STORAGE_PATH];
    return folder[0] != 0 && join(path, folder, name) && port_config_host_file_exists(path);
}

// A game folder: halo3.ini or the maps beside the executable (or in the current folder).
static int portable_folder(const char *folder)
{
    return file_in(folder, CONFIG_FILE_NAME) || file_in(folder, "maps/mainmenu.map");
}

// ---- reading halo3.ini ----

static void warn(const char *path, unsigned long line, const char *what, const char *text, config_size length)
{
    char number[12];
    number_text(line, number);
    char shown[96];
    copy_text(shown, sizeof(shown), text, length);
    port_write_error("port: config: ");
    port_write_error(path);
    port_write_error(":");
    port_write_error(number);
    port_write_error(": ");
    port_write_error(what);
    port_write_error(" '");
    port_write_error(shown);
    port_write_error("'; ignored\n");
}

// The section a [section] line names: k_section_*, or -1 for one halo3.ini does not have.
static int section_of(const char *name, config_size length)
{
    for (int i = 0; i < k_section_count; i++)
        if (equals_word(name, length, k_sections[i]))
            return i;
    return -1;
}

// A line split: its kind, and the section's name or the key and value. Returns 0 for a blank or comment line, 1 for a
// section line, 2 for a key line, 3 for anything else.
static int split_line(const char *line, config_size length, const char **name, config_size *name_length,
    const char **value, config_size *value_length)
{
    config_size i = 0;
    while (i < length && space(line[i]))
        i++;
    if (i == length || line[i] == ';' || line[i] == '#')
        return 0;
    if (line[i] == '[') {
        config_size end = i + 1;
        while (end < length && line[end] != ']')
            end++;
        if (end == length)
            return 3;
        *name = line + i + 1;
        *name_length = end - i - 1;
        trimmed(name, name_length);
        return 1;
    }
    config_size equals = i;
    while (equals < length && line[equals] != '=')
        equals++;
    if (equals == length)
        return 3;
    *name = line + i;
    *name_length = equals - i;
    while (*name_length > 0 && space((*name)[*name_length - 1]))
        (*name_length)--;
    const char *v = line + equals + 1;
    config_size n = length - equals - 1;
    while (n > 0 && space(v[0]))
        v++, n--;
    if (n > 0 && v[0] == '"') { // quoted: up to the closing quote
        config_size close = 1;
        while (close < n && v[close] != '"')
            close++;
        n = close < n ? close + 1 : n;
    } else { // a ';' or '#' at the start or after a space begins a comment
        for (config_size k = 0; k < n; k++) {
            if ((v[k] == ';' || v[k] == '#') && (k == 0 || space(v[k - 1]))) {
                n = k;
                break;
            }
        }
    }
    *value = v;
    *value_length = n;
    return 2;
}

// The setting a key of section `section` names, -1 for none.
static int setting_of(int section, const char *key, config_size length)
{
    for (int i = 0; i < PORT_SETTING_COUNT; i++)
        if (k_settings[i].section == section && equals_word(key, length, k_settings[i].key))
            return i;
    return -1;
}

// The UTF-8 byte order mark an editor (Notepad) may put at the start of the file: 3, or 0 when `text` has none.
static config_size bom_size(const char *text, config_size size)
{
    return size >= 3 && (unsigned char)text[0] == 0xef && (unsigned char)text[1] == 0xbb &&
                   (unsigned char)text[2] == 0xbf
               ? 3
               : 0;
}

// halo3.ini's settings into g_config.value (start-up: no other thread runs yet).
static void parse(const char *text, config_size size)
{
    const config_size bom = bom_size(text, size);
    text += bom;
    size -= bom;
    int section = -2; // before any section line
    unsigned long line_number = 0;
    for (config_size at = 0; at < size;) {
        config_size end = at;
        while (end < size && text[end] != '\n')
            end++;
        line_number++;
        const char *line = text + at;
        const config_size length = end - at;
        at = end + 1;
        if (length > 0 && line[length - 1] == '\r')
            g_config.crlf = 1;
        const char *name, *value;
        config_size name_length, value_length;
        switch (split_line(line, length, &name, &name_length, &value, &value_length)) {
        case 1:
            section = section_of(name, name_length);
            if (section < 0)
                warn(g_config.path, line_number, "unknown section", name, name_length);
            break;
        case 2: {
            if (section < 0) {
                if (section == -2)
                    warn(g_config.path, line_number, "key outside a section", name, name_length);
                break; // (an unknown section's keys: the section was reported)
            }
            const int setting = setting_of(section, name, name_length);
            char normal[PORT_CONFIG_VALUE];
            if (setting < 0)
                warn(g_config.path, line_number, "unknown key", name, name_length);
            else if (!normalize(setting, value, value_length, normal))
                warn(g_config.path, line_number, "a value it cannot take", line, length);
            else {
                copy_string(g_config.value[setting], PORT_CONFIG_VALUE, normal);
                g_config.given[setting] = 1;
            }
            break;
        }
        case 3:
            warn(g_config.path, line_number, "not a setting", line, length);
            break;
        default:
            break;
        }
    }
}

// ---- writing halo3.ini ----

typedef struct {
    char *text;
    config_size size, capacity;
    int overflow;
    const char *eol;
} config_out;

static void put(config_out *out, const char *text, config_size length)
{
    if (out->size + length > out->capacity) {
        out->overflow = 1;
        return;
    }
    __builtin_memcpy(out->text + out->size, text, length);
    out->size += length;
}
static void put_string(config_out *out, const char *text) { put(out, text, length_of(text)); }
static void put_line(config_out *out, const char *text, config_size length)
{
    put(out, text, length);
    put_string(out, out->eol);
}

// Comment lines: "; " and each line of `comment`.
static void put_comment(config_out *out, const char *comment)
{
    while (*comment != 0) {
        config_size n = 0;
        while (comment[n] != 0 && comment[n] != '\n')
            n++;
        put_string(out, "; ");
        put_line(out, comment, n);
        comment += n + (comment[n] == '\n');
    }
}

// A value as halo3.ini reads it back: quoted when it would otherwise read otherwise (a comment mark in it, spaces at
// its ends, quotes around it).
static void put_value(config_out *out, const char *value)
{
    const config_size n = length_of(value);
    int quote = n > 0 && (space(value[0]) || space(value[n - 1]) || (value[0] == '"' && value[n - 1] == '"'));
    for (config_size i = 0; i < n && !quote; i++)
        quote = value[i] == ';' || value[i] == '#';
    put_string(out, quote ? "\"" : "");
    put(out, value, n);
    put_string(out, quote ? "\"" : "");
}

// A setting's new line: "key = value".
static void put_setting(config_out *out, int setting, const char *value)
{
    put_string(out, k_settings[setting].key);
    put_string(out, value[0] != 0 ? " = " : " =");
    put_value(out, value);
    put_string(out, out->eol);
}

// A setting's line as the file has it (`line`, `length` bytes; its value at `old_value`, `old_length` bytes, as
// split_line found it) with `value` in place of the old one: the key as spelled, the spacing and a comment after the
// value are kept.
static void put_setting_over(config_out *out, const char *line, config_size length, const char *old_value,
    config_size old_length, const char *value)
{
    config_size prefix = (config_size)(old_value - line), kept = old_length;
    while (kept > 0 && space(old_value[kept - 1])) // the spaces between the value and a comment stay
        kept--;
    put(out, line, prefix);
    if (value[0] != 0 && kept == 0 && prefix > 0 && line[prefix - 1] == '=') // "key =" had no value: "key = value"
        put_string(out, " ");
    put_value(out, value);
    const config_size rest = prefix + kept;
    if (value[0] != 0 && rest < length && !space(line[rest])) // a comment right after: a space, or it is the value's
        put_string(out, " ");
    put_line(out, line + rest, length - rest);
}

// The settings of `section` not written yet, each under its comment.
static void put_missing(config_out *out, int section, const char values[][PORT_CONFIG_VALUE], unsigned char *written)
{
    for (int i = 0; i < PORT_SETTING_COUNT; i++) {
        if (k_settings[i].section != section || written[i])
            continue;
        put_comment(out, k_settings[i].comment);
        put_setting(out, i, values[i]);
        written[i] = 1;
    }
}

// halo3.ini with `values`: `old` (`old_size` bytes, `have_old`) rewritten in place (see the top), or a new file.
static void render(config_out *out, const char *old, config_size old_size, int have_old,
    const char values[][PORT_CONFIG_VALUE])
{
    unsigned char written[PORT_SETTING_COUNT] = { 0 };
    unsigned char seen[k_section_count] = { 0 };
    if (!have_old) {
        put_comment(out, k_file_comment);
    } else {
        const config_size bom = bom_size(old, old_size); // kept as the file had it
        put(out, old, bom);
        old += bom;
        old_size -= bom;
        int section = -2;
        config_size blanks = 0; // blank lines held back: a setting added at a section's end goes before them
        for (config_size at = 0; at < old_size;) {
            config_size end = at;
            while (end < old_size && old[end] != '\n')
                end++;
            const char *line = old + at;
            config_size length = end - at;
            at = end + 1;
            if (length > 0 && line[length - 1] == '\r')
                length--;
            const char *name, *value;
            config_size name_length, value_length;
            const int kind = split_line(line, length, &name, &name_length, &value, &value_length);
            config_size i = 0;
            while (i < length && space(line[i]))
                i++;
            if (i == length) { // blank
                blanks++;
                continue;
            }
            if (kind == 1 && section >= 0)
                put_missing(out, section, values, written);
            for (; blanks > 0; blanks--)
                put_string(out, out->eol);
            if (kind == 1) {
                section = section_of(name, name_length);
                if (section >= 0)
                    seen[section] = 1;
                put_line(out, line, length);
            } else if (kind == 2 && section >= 0 && setting_of(section, name, name_length) >= 0) {
                const int setting = setting_of(section, name, name_length);
                put_setting_over(out, line, length, value, value_length, values[setting]);
                written[setting] = 1;
            } else {
                put_line(out, line, length);
            }
        }
        if (section >= 0)
            put_missing(out, section, values, written);
        for (; blanks > 0; blanks--)
            put_string(out, out->eol);
    }
    for (int section = 0; section < k_section_count; section++) {
        if (seen[section])
            continue;
        if (out->size > 0)
            put_string(out, out->eol);
        put_string(out, "[");
        put_string(out, k_sections[section]);
        put_string(out, "]");
        put_string(out, out->eol);
        if (section == k_section_paths)
            put_comment(out, k_paths_comment);
        else if (section == k_section_input)
            put_comment(out, k_input_comment);
        else if (section == k_section_bindings)
            put_comment(out, k_bindings_comment);
        put_missing(out, section, values, written);
    }
}

// Writes halo3.ini with `values` (under g_save_busy). Returns 0 or a Win32 error.
static unsigned long write_file(const char values[][PORT_CONFIG_VALUE])
{
    unsigned int size = 0;
    unsigned long error = port_host_read_file(g_config.path, g_file, sizeof(g_file), &size);
    const int have_old = error == CONFIG_ERROR_SUCCESS;
    if (!have_old && error != CONFIG_ERROR_FILE_NOT_FOUND && error != CONFIG_ERROR_PATH_NOT_FOUND)
        return error; // (a file too large to read is left as it is)
    config_out out = { g_text, 0, sizeof(g_text), 0, g_config.crlf ? "\r\n" : "\n" };
    render(&out, g_file, size, have_old, values);
    if (out.overflow)
        return CONFIG_ERROR_INSUFFICIENT_BUFFER;
    if (g_config.directory[0] != 0) {
        error = port_host_make_directories(g_config.directory);
        if (error != CONFIG_ERROR_SUCCESS)
            return error;
    }
    return port_host_write_file_atomic(g_config.path, out.text, out.size);
}

// ---- start-up ----

// The variable's value for `value` of setting `setting` in `out`; 0 when the variable stays unset (the setting's
// automatic value: the subsystem decides, or finds the file, itself).
static int variable_value(int setting, const char *value, char out[PORT_STORAGE_PATH])
{
    if ((setting == PORT_SETTING_VSYNC || setting == PORT_SETTING_RENDERER || setting == PORT_SETTING_AUDIO_BACKEND) &&
        same(value, "auto"))
        return 0;
    if (setting == PORT_SETTING_RESOLUTION && same(value, "native"))
        return 0;
    if (value[0] == 0)
        return 0;
    if (k_settings[setting].kind != k_kind_path) {
        copy_string(out, PORT_STORAGE_PATH, value);
        return 1;
    }
    char path[PORT_STORAGE_PATH];
    if (!resolve(value, path)) {
        port_write_error("port: config: ");
        port_write_error(k_settings[setting].key);
        port_write_error(": the path is too long; ignored\n");
        return 0;
    }
    if (setting != PORT_SETTING_MAPS_DIR) {
        copy_string(out, PORT_STORAGE_PATH, path);
        return 1;
    }
    // the disc's root (HALO3_DISC) holds maps/: the folder named, or the one over a maps folder named
    char maps[PORT_STORAGE_PATH];
    if (join(maps, path, "maps") && port_host_directory_exists(maps)) {
        copy_string(out, PORT_STORAGE_PATH, path);
        return 1;
    }
    if (file_in(path, "mainmenu.map")) {
        parent_of(path, out);
        return out[0] != 0;
    }
    port_write_error("port: config: maps_dir: ");
    port_write_error(path);
    port_write_error(" has neither maps/ nor mainmenu.map in it\n");
    copy_string(out, PORT_STORAGE_PATH, path);
    return 1;
}

// The launch's value of the setting's variable; 0 when it did not set it, or the setting has none (a binding).
static const char *variable_of(int setting)
{
    const char *name = k_settings[setting].variable;
    return name[0] != 0 ? port_config_host_getenv(name) : 0;
}

// The keyboard preset in effect (under the lock, or at start-up).
static int preset_now(void)
{
    const char *value = g_config.locked[PORT_SETTING_KBM_PRESET] ? g_config.launch[PORT_SETTING_KBM_PRESET]
                                                                 : g_config.value[PORT_SETTING_KBM_PRESET];
    const int preset = port_kbm_preset_of(value, length_of(value));
    return preset >= 0 ? preset : PORT_KBM_PRESET_MODERN;
}

// The bindings halo3.ini did not give: the preset's (start-up).
static void bindings_from_preset(void)
{
    const int preset = preset_now();
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++)
        if (!g_config.given[PORT_SETTING_BINDING_FIRST + a])
            normalize_string(PORT_SETTING_BINDING_FIRST + a, port_kbm_action_default(preset, a),
                             g_config.value[PORT_SETTING_BINDING_FIRST + a]);
}

// The launch set `name` to something other than "1".
static int set_other_than_1(const char *name)
{
    const char *v = port_config_host_getenv(name);
    return v != 0 && !(v[0] == '1' && v[1] == 0);
}

// Sets `name` to `value` when the launch did not set it. Returns 1 when it did.
static int default_variable(const char *name, const char *value)
{
    if (port_config_host_getenv(name) != 0)
        return 0;
    if (!port_config_host_setenv(name, value)) {
        port_write_error("port: config: cannot set ");
        port_write_error(name);
        port_write_error("\n");
        return 0;
    }
    return 1;
}

// Sets `variable` to the first of `names` (in order) that is beside the executable, in the portable game folder or in
// the data folder, when neither the launch nor halo3.ini set it. Returns 1 when the variable is set (before or now).
static int find_pack_file(const char *variable, const char *const *names, unsigned count, const char *exe_directory,
                          const char *data_directory, const char *what)
{
    if (port_config_host_getenv(variable) != 0)
        return 1;
    const char *folders[3] = { exe_directory, g_config.root, data_directory };
    char path[PORT_STORAGE_PATH];
    for (unsigned n = 0; n < count; n++) {
        for (unsigned f = 0; f < 3; f++) {
            if (folders[f][0] != 0 && join(path, folders[f], names[n]) && port_config_host_file_exists(path)) {
                if (default_variable(variable, path)) {
                    port_write_error("port: config: ");
                    port_write_error(what);
                    port_write_error(" ");
                    port_write_error(path);
                    port_write_error("\n");
                }
                return 1;
            }
        }
    }
    return 0;
}

// The shader packs, when neither the launch nor halo3.ini names them: the Xenos pack (then the others) for every
// renderer, and on Windows the Direct3D 12 renderer's own DXIL pack (gpu_backend_shader_pack), each beside the
// executable, in the portable game folder or in the data folder.
static void find_shader_pack(const char *exe_directory, const char *data_directory)
{
    static const char *const k_names[] = { "halo3_shaders_xenos.pack", "halo3_shaders_all.pack", "halo3_shaders.pack" };
    int dxil = 0;
#if defined(_WIN32)
    static const char *const k_dxil_names[] = { "halo3_shaders_dxil.pack" };
    dxil = find_pack_file("HALO3_SHADER_PACK_DXIL", k_dxil_names, 1, exe_directory, data_directory,
                          "Direct3D 12 shader pack");
#endif
    // The renderer handles shaders/ in the effective data directory. Keep the Vulkan path automatic here
    // so it can try halo3_shaders.abi6.pack before falling back to the managed ABI 5 pack.
    const char *data_override = port_config_host_getenv("HALO3_DATA");
    const char *managed_root = data_override && data_override[0] ? data_override : data_directory;
    char managed_pack[PORT_STORAGE_PATH];
    if (managed_root[0] && join(managed_pack, managed_root, "shaders/halo3_shaders.pack") &&
        port_config_host_file_exists(managed_pack))
        return;
    if (find_pack_file("HALO3_SHADER_PACK", k_names, sizeof(k_names) / sizeof(k_names[0]), exe_directory,
                       data_directory, "shader pack"))
        return;
    if (dxil) {
        port_write_error("port: config: no Vulkan shader pack found (halo3_shaders_xenos.pack): Direct3D 12 only\n");
        return;
    }
    port_write_error("port: config: no shader pack found (halo3_shaders_xenos.pack beside the executable");
    if (data_directory[0] != 0) {
        port_write_error(" or in ");
        port_write_error(data_directory);
    }
    port_write_error("); set [paths] shaders_pack in halo3.ini\n");
}

void port_config_init(void)
{
    if (g_config.initialized)
        return;
    // 1. what the launch set, and the defaults
    for (int i = 0; i < PORT_SETTING_COUNT; i++) {
        const char *v = variable_of(i);
        g_config.locked[i] = v != 0;
        copy_string(g_config.value[i], PORT_CONFIG_VALUE, k_settings[i].fallback);
        if (v != 0 && !normalize_string(i, v, g_config.launch[i]))
            copy_string(g_config.launch[i], PORT_CONFIG_VALUE, v); // as given: its subsystem says what it makes of it
    }
    // a player's launch: a desktop, and a launch that left the window to the game (scripts, test harnesses, gdb and
    // Xvfb runs set HALO3_WINDOW, to 0 or 1: they start as they always have, whatever the player's halo3.ini says)
    g_config.interactive = port_config_host_display() && port_config_host_getenv("HALO3_WINDOW") == 0 &&
                           !set_other_than_1("HALO3_VULKAN");
    // 2. where things are
    char exe_directory[PORT_STORAGE_PATH] = "", current[PORT_STORAGE_PATH] = "", data_directory[PORT_STORAGE_PATH] = "";
    if (!port_host_executable_directory(exe_directory, sizeof(exe_directory)))
        exe_directory[0] = 0;
    if (!port_config_host_current_directory(current, sizeof(current)))
        current[0] = 0;
    if (exe_directory[0] != 0 && portable_folder(exe_directory))
        copy_string(g_config.root, PORT_STORAGE_PATH, exe_directory);
    else if (current[0] != 0 && portable_folder(current))
        copy_string(g_config.root, PORT_STORAGE_PATH, current);
    g_config.portable = g_config.root[0] != 0;
    if (!g_config.portable && !port_host_user_data_directory(data_directory, sizeof(data_directory)))
        data_directory[0] = 0;
    const char *named = port_config_host_getenv("HALO3_CONFIG");
    if (named != 0) {
        if (named[0] != 0 && !(named[0] == '0' && named[1] == 0) && length_of(named) < PORT_STORAGE_PATH) {
            copy_string(g_config.path, PORT_STORAGE_PATH, named);
            g_config.active = 1;
        } else if (named[0] != 0 && !(named[0] == '0' && named[1] == 0)) {
            port_write_error("port: config: HALO3_CONFIG is too long a path; no halo3.ini\n");
        }
    } else if (g_config.interactive) {
        char directory[PORT_STORAGE_PATH];
        const int found = g_config.portable ? (copy_string(directory, sizeof(directory), g_config.root), 1)
                                            : port_config_host_config_directory(directory, sizeof(directory));
        g_config.active = found && join(g_config.path, directory, CONFIG_FILE_NAME);
    }
    if (!g_config.active)
        g_config.path[0] = 0;
    else
        parent_of(g_config.path, g_config.directory);
    if (!g_config.active) // no halo3.ini: a launch behaves as one did before it existed (System Link when asked for)
        copy_string(g_config.value[PORT_SETTING_LAN], PORT_CONFIG_VALUE, "0");
#ifdef _WIN32
    g_config.crlf = 1;
#endif
    // 3. halo3.ini: read, or written with the defaults when it does not exist
    const char *state = "none";
    if (!g_config.active)
        bindings_from_preset();
    if (g_config.active) {
        unsigned int size = 0;
        const unsigned long error = port_host_read_file(g_config.path, g_file, sizeof(g_file), &size);
        if (error == CONFIG_ERROR_SUCCESS) {
            g_config.crlf = 0;
            parse(g_file, size);
            bindings_from_preset();
            state = "read";
        } else if (error == CONFIG_ERROR_FILE_NOT_FOUND || error == CONFIG_ERROR_PATH_NOT_FOUND) {
            bindings_from_preset();
            const unsigned long written = write_file((const char (*)[PORT_CONFIG_VALUE])g_config.value);
            state = written == CONFIG_ERROR_SUCCESS ? "created" : "cannot be created";
        } else {
            state = error == CONFIG_ERROR_INSUFFICIENT_BUFFER ? "too large; not read" : "cannot be read";
            bindings_from_preset();
        }
    }
    // 4. the launch's values, and the variables it left unset from halo3.ini
    int applied = 0;
    for (int i = 0; i < PORT_SETTING_COUNT; i++) {
        if (g_config.locked[i])
            continue;
        copy_string(g_config.launch[i], PORT_CONFIG_VALUE, g_config.value[i]);
        char variable[PORT_STORAGE_PATH];
        if (g_config.active && exported(i) && variable_value(i, g_config.value[i], variable))
            applied += default_variable(k_settings[i].variable, variable);
    }
    // a custom resolution renders at 1x (res_scale multiplies the game's own 1152x640 only: 3840x2160 at 3x would ask
    // for an 11520x6480 frame), whichever of the two the launch or halo3.ini set
    unsigned int width, height;
    const char *scale = port_config_host_getenv("HALO3_RES_SCALE");
    if (port_config_resolution(&width, &height) && scale != 0 && !(scale[0] == '1' && scale[1] == 0)) {
        port_write_error("port: config: resolution ");
        port_write_error(g_config.launch[PORT_SETTING_RESOLUTION]);
        port_write_error(" renders at 1x: res_scale ");
        port_write_error(scale);
        port_write_error(" ignored\n");
        if (!port_config_host_setenv("HALO3_RES_SCALE", "1"))
            port_write_error("port: config: cannot set HALO3_RES_SCALE\n");
    }
    // 5. a player's launch: the window and the renderer, and the files found
    int window = 0;
    if (g_config.interactive) {
        window = default_variable("HALO3_WINDOW", "1");
        window |= default_variable("HALO3_VULKAN", "1");
    }
    if (g_config.active) {
        char userdata[PORT_STORAGE_PATH];
        if (g_config.portable && join(userdata, g_config.root, "userdata") && default_variable("HALO3_DATA", userdata)) {
            port_write_error("port: config: user data in ");
            port_write_error(userdata);
            port_write_error("\n");
        }
        find_shader_pack(exe_directory, g_config.portable ? "" : data_directory);
    }
    __atomic_store_n(&g_config.initialized, 1, __ATOMIC_RELEASE);
    // 6. the log
    char count[12];
    number_text((unsigned long)applied, count);
    port_write_error("port: config: ");
    if (g_config.active) {
        port_write_error(g_config.path);
        port_write_error(" (");
        port_write_error(state);
        port_write_error(g_config.portable ? ", portable" : "");
        port_write_error("): ");
        port_write_error(count);
        port_write_error(" settings applied");
    } else {
        port_write_error(named != 0 ? "HALO3_CONFIG names none: no halo3.ini"
                                    : "scripted or headless launch (HALO3_WINDOW set, or no display): no halo3.ini");
    }
    port_write_error(window ? "; window and renderer on\n" : "\n");
}

// ---- the settings, at run time ----

const char *port_config_path(void) { return g_config.path; }

int port_config_portable(void) { return g_config.portable; }

static int valid(int setting) { return setting >= 0 && setting < PORT_SETTING_COUNT; }

const char *port_config_section(int setting) { return valid(setting) ? k_sections[k_settings[setting].section] : ""; }
const char *port_config_key(int setting) { return valid(setting) ? k_settings[setting].key : ""; }
const char *port_config_variable(int setting) { return valid(setting) ? k_settings[setting].variable : ""; }
int port_config_live(int setting) { return valid(setting) && k_settings[setting].live; }
int port_config_locked(int setting) { return valid(setting) && g_config.locked[setting]; }

const char *port_config_default(int setting)
{
    if (!valid(setting))
        return "";
    if (binding_setting(setting)) { // the preset's
        config_lock();
        const int preset = preset_now();
        config_unlock();
        return port_kbm_action_default(preset, setting - PORT_SETTING_BINDING_FIRST);
    }
    return setting == PORT_SETTING_LAN && !g_config.active ? "0" : k_settings[setting].fallback;
}

// The setting's value now, in `out` (under the lock, or before port_config_init: from its variable alone).
static void current(int setting, char out[PORT_CONFIG_VALUE])
{
    if (!__atomic_load_n(&g_config.initialized, __ATOMIC_ACQUIRE)) {
        const char *v = variable_of(setting);
        if (v == 0 || !normalize_string(setting, v, out))
            copy_string(out, PORT_CONFIG_VALUE, k_settings[setting].fallback);
        return;
    }
    copy_string(out, PORT_CONFIG_VALUE, g_config.locked[setting] ? g_config.launch[setting] : g_config.value[setting]);
}

unsigned long port_config_value(int setting, char *out, unsigned long capacity)
{
    if (capacity == 0)
        return 0;
    out[0] = 0;
    if (!valid(setting))
        return 0;
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(setting, value);
    config_unlock();
    copy_string(out, capacity, value);
    return length_of(out);
}

int port_config_restart_pending(int setting)
{
    if (!valid(setting) || k_settings[setting].live)
        return 0;
    config_lock();
    const int pending = !g_config.locked[setting] && !same(g_config.value[setting], g_config.launch[setting]);
    config_unlock();
    return pending;
}

int port_config_set(int setting, const char *value)
{
    char normal[PORT_CONFIG_VALUE];
    if (!valid(setting) || value == 0 || !normalize_string(setting, value, normal))
        return -1;
    config_lock();
    int changed = 0;
    if (!g_config.locked[setting] && !same(g_config.value[setting], normal)) {
        copy_string(g_config.value[setting], PORT_CONFIG_VALUE, normal);
        changed = 1;
        if (k_settings[setting].live)
            __atomic_store_n(&g_config.revision, g_config.revision + 1, __ATOMIC_RELEASE);
    }
    config_unlock();
    return changed;
}

unsigned long port_config_save(void)
{
    if (!g_config.active)
        return CONFIG_ERROR_NOT_SUPPORTED;
    while (__atomic_test_and_set(&g_save_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    static char values[PORT_SETTING_COUNT][PORT_CONFIG_VALUE]; // (under g_save_busy)
    config_lock();
    __builtin_memcpy(values, g_config.value, sizeof(values));
    config_unlock();
    const unsigned long error = write_file((const char (*)[PORT_CONFIG_VALUE])values);
    __atomic_clear(&g_save_busy, __ATOMIC_RELEASE);
    if (error != CONFIG_ERROR_SUCCESS) {
        char number[12];
        number_text(error, number);
        port_write_error("port: config: cannot write ");
        port_write_error(g_config.path);
        port_write_error(" (error ");
        port_write_error(number);
        port_write_error(")\n");
    }
    return error;
}

unsigned long port_config_revision(void) { return __atomic_load_n(&g_config.revision, __ATOMIC_ACQUIRE); }

PORT_MS_ABI int port_config_resolution(unsigned int *width, unsigned int *height)
{
    // the live resolution (applies immediately when changed): before port_config_init finishes, read the env variable
    char value[PORT_CONFIG_VALUE];
    if (!__atomic_load_n(&g_config.initialized, __ATOMIC_ACQUIRE)) {
        const char *v = port_config_host_getenv(k_settings[PORT_SETTING_RESOLUTION].variable);
        copy_string(value, PORT_CONFIG_VALUE, v != 0 ? v : "native");
    } else {
        config_lock();
        current(PORT_SETTING_RESOLUTION, value);
        config_unlock();
    }
    unsigned long w = 0, h = 0;
    config_size length = length_of(value);
    const char *text = value;
    trimmed(&text, &length);
    const int custom = resolution_of(text, length, &w, &h);
    if (!custom && length != 0 && !equals_word(text, length, "native")) {
        static volatile char reported;
        if (!__atomic_test_and_set(&reported, __ATOMIC_RELAXED)) {
            port_write_error("port: config: HALO3_RESOLUTION ");
            port_write_error(value);
            port_write_error(" is not native or an even WxH from 640x360 to 7680x4320 (at most 5120x2160's pixels): native\n");
        }
    }
    if (width != 0)
        *width = custom ? (unsigned int)w : 0;
    if (height != 0)
        *height = custom ? (unsigned int)h : 0;
    return custom;
}

int port_config_window_mode(void)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_WINDOW_MODE, value);
    config_unlock();
    return same(value, "windowed") ? PORT_WINDOW_MODE_WINDOWED
           : same(value, "borderless") ? PORT_WINDOW_MODE_BORDERLESS
                                       : PORT_WINDOW_MODE_FULLSCREEN;
}

unsigned long port_config_fps_cap(void)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_FPS_CAP, value);
    config_unlock();
    unsigned long n;
    return number_of(value, length_of(value), &n) && (n == 0 || (n >= 30 && n <= 1000)) ? n : 30;
}

int port_config_vsync(void)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_VSYNC, value);
    config_unlock();
    return same(value, "1") ? 1 : same(value, "0") ? 0 : -1;
}

static int flag_now(int setting, int fallback)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(setting, value);
    config_unlock();
    const int on = flag_of(value, length_of(value));
    return on >= 0 ? on : fallback;
}

PORT_MS_ABI int port_config_hd_text(void) { return flag_now(PORT_SETTING_HD_TEXT, 1); }

PORT_MS_ABI float port_config_draw_distance(void)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_DRAW_DISTANCE, value);
    config_unlock();
    return same(value, "2") ? 2.0f : same(value, "4") ? 4.0f : 1.0f;
}

PORT_MS_ABI unsigned int port_config_anisotropy(void)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_ANISOTROPY, value);
    config_unlock();
    return same(value, "4") ? 4u : same(value, "8") ? 8u : same(value, "16") ? 16u : 0u;
}

PORT_MS_ABI unsigned int port_config_shadow_resolution(void)
{
    // applies at the next launch (live = 0): read the launch value once initialized
    char value[PORT_CONFIG_VALUE];
    if (!__atomic_load_n(&g_config.initialized, __ATOMIC_ACQUIRE)) {
        const char *v = variable_of(PORT_SETTING_SHADOW_RES);
        if (v == 0 || !normalize_string(PORT_SETTING_SHADOW_RES, v, value))
            copy_string(value, PORT_CONFIG_VALUE, k_settings[PORT_SETTING_SHADOW_RES].fallback);
    } else {
        config_lock();
        copy_string(value, PORT_CONFIG_VALUE, g_config.launch[PORT_SETTING_SHADOW_RES]);
        config_unlock();
    }
    return same(value, "1024") ? 1024u : same(value, "2048") ? 2048u : 512u;
}

PORT_MS_ABI int port_config_splitscreen_full_quality(void) { return flag_now(PORT_SETTING_SPLITSCREEN_QUALITY, 0); }
PORT_MS_ABI int port_config_splitscreen_symmetric_hud(void) { return flag_now(PORT_SETTING_SPLITSCREEN_SYMMETRIC_HUD, 1); }

PORT_MS_ABI int port_config_coop_4p(void) { return flag_now(PORT_SETTING_COOP_4P, 1); }

PORT_MS_ABI unsigned long port_config_mic_device(char *out, unsigned long capacity)
{
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(PORT_SETTING_MIC_DEVICE, value);
    config_unlock();
    if (out != 0 && capacity > 0)
        copy_string(out, capacity, value);
    return length_of(value);
}

// ---- [input] and [bindings] ----

int port_config_raw_mouse(void) { return flag_now(PORT_SETTING_RAW_MOUSE, 1); }
int port_config_invert_mouse_y(void) { return flag_now(PORT_SETTING_INVERT_MOUSE_Y, 0); }
int port_config_crouch_hold(void) { return flag_now(PORT_SETTING_CROUCH_HOLD, 1); }

static float decimal_now(int setting, unsigned long fallback)
{
    const config_setting *s = &k_settings[setting];
    char value[PORT_CONFIG_VALUE];
    config_lock();
    current(setting, value);
    config_unlock();
    unsigned long hundredths;
    if (setting == PORT_SETTING_MOUSE_ACCELERATION && equals_word(value, length_of(value), "off"))
        hundredths = 0ul;
    else if (!decimal_of(value, length_of(value), &hundredths)) // a launch's variable that is not a number
        hundredths = fallback;
    if (hundredths < s->minimum)
        hundredths = s->minimum;
    if (hundredths > s->maximum)
        hundredths = s->maximum;
    return (float)hundredths / 100.0f;
}

float port_config_mouse_sensitivity(void) { return decimal_now(PORT_SETTING_MOUSE_SENSITIVITY, 100ul); }
float port_config_mouse_acceleration(void) { return decimal_now(PORT_SETTING_MOUSE_ACCELERATION, 0ul); }
float port_config_mouse_zoom_sensitivity(void) { return decimal_now(PORT_SETTING_MOUSE_ZOOM_SENSITIVITY, 100ul); }
float port_config_mouse_vehicle_sensitivity(void) { return decimal_now(PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY, 100ul); }

int port_config_kbm_preset(void)
{
    config_lock();
    const int preset = __atomic_load_n(&g_config.initialized, __ATOMIC_ACQUIRE) ? preset_now() : PORT_KBM_PRESET_MODERN;
    config_unlock();
    return preset;
}

void port_config_kbm_bindings(port_kbm_bindings *out)
{
    if (!__atomic_load_n(&g_config.initialized, __ATOMIC_ACQUIRE)) {
        port_kbm_preset_bindings(PORT_KBM_PRESET_MODERN, out);
        return;
    }
    config_lock(); // (parsed in place: a copy of every value is too large for the game threads' stacks)
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++) {
        const char *value = g_config.value[PORT_SETTING_BINDING_FIRST + a];
        if (!port_kbm_parse(value, length_of(value), out->codes[a])) // (normalized: always parses)
            out->codes[a][0] = out->codes[a][1] = PORT_KBM_NONE;
    }
    config_unlock();
}

// Sets binding `action` to `codes` (under the lock). Returns whether it changed.
static int set_binding(int action, const unsigned char codes[PORT_KBM_BINDS])
{
    char text[PORT_CONFIG_VALUE];
    port_kbm_format(codes, text, sizeof(text));
    char *value = g_config.value[PORT_SETTING_BINDING_FIRST + action];
    if (same(value, text))
        return 0;
    copy_string(value, PORT_CONFIG_VALUE, text);
    return 1;
}

int port_config_set_kbm_bindings(const port_kbm_bindings *bindings)
{
    int changed = 0;
    config_lock();
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++)
        changed += set_binding(a, bindings->codes[a]);
    if (changed)
        __atomic_store_n(&g_config.revision, g_config.revision + 1, __ATOMIC_RELEASE);
    config_unlock();
    return changed;
}

int port_config_apply_kbm_preset(int preset)
{
    if (preset < 0 || preset >= PORT_KBM_PRESET_COUNT)
        return 0;
    port_kbm_bindings bindings;
    port_kbm_preset_bindings(preset, &bindings);
    int changed = 0;
    config_lock();
    char *value = g_config.value[PORT_SETTING_KBM_PRESET];
    if (!g_config.locked[PORT_SETTING_KBM_PRESET] && !same(value, port_kbm_preset_name(preset))) {
        copy_string(value, PORT_CONFIG_VALUE, port_kbm_preset_name(preset));
        changed = 1;
    }
    for (int a = 0; a < PORT_KBM_ACTION_COUNT; a++)
        changed |= set_binding(a, bindings.codes[a]);
    if (changed)
        __atomic_store_n(&g_config.revision, g_config.revision + 1, __ATOMIC_RELEASE);
    config_unlock();
    return changed;
}
