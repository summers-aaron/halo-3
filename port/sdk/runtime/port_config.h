// port/sdk/runtime (both hosts): the player's settings (halo3.ini) and where the game's files are, for a launch with no
// options at all (a double-click) as for a scripted one (port_config.c).
//
// Each setting has a key in halo3.ini and a HALO3_* environment variable, and its value comes from, in order:
//   1. the variable, when the launch set it (a launch option: it always wins, and the Guide cannot change it);
//   2. halo3.ini, when one is in use (below);
//   3. the compiled default (the same as before halo3.ini existed, but System Link, which a player's launch turns on).
// port_config_init runs first thing in the process (entry_linux.c, entry_windows.c), before any subsystem reads its
// variables: it sets each variable the launch left unset from halo3.ini, so every subsystem reads its settings as it
// always has (getenv); the settings that apply at once (port_config_live) are read through port_config_* below
// instead, and follow the Guide's Settings page (port/sdk/xam/guide.c) as it changes them. HD text, draw distance
// and the keyboard and mouse settings are only read that way: halo3.ini sets no variable for them (a launch's
// variable still wins and locks the row), and the [bindings] have no variable at all.
//
// Which halo3.ini (HALO3_CONFIG names one: "0" or "" for none):
//   - portable: when the executable's folder, or the current folder, has halo3.ini or maps/mainmenu.map, that folder
//     is the game folder: <folder>/halo3.ini, maps under it, user data in <folder>/userdata;
//   - otherwise the user's: $XDG_CONFIG_HOME/halo3/halo3.ini (~/.config/halo3) and data in $XDG_DATA_HOME/halo3
//     (~/.local/share/halo3) on Linux; %APPDATA%\Halo3\halo3.ini and %LOCALAPPDATA%\Halo3 on Windows.
// halo3.ini is used (and written with every setting and what it does when it does not exist) by a player's launch:
// one in a desktop session that leaves the window to the game (HALO3_WINDOW unset; HALO3_VULKAN unset or 1), as a
// double-click does. Such a launch also opens the window and the renderer (HALO3_WINDOW=1, HALO3_VULKAN=1; [display]
// renderer picks its graphics API), turns System Link on and finds the shader pack (next to the executable, in the
// game folder or the data folder) by itself. Any other launch (a script or harness that sets HALO3_WINDOW, 0 or 1; no
// display) reads no halo3.ini and sets nothing unless HALO3_CONFIG names one, so scripted and test runs never depend
// on the player's settings.
#ifndef PORT_RUNTIME_PORT_CONFIG_H
#define PORT_RUNTIME_PORT_CONFIG_H

#include "../input/kbm_bindings.h"
#include "port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

// The settings: [section] key, variable, values.
enum port_setting {
    PORT_SETTING_WINDOW_MODE,   // [display] window_mode    HALO3_WINDOW_MODE    windowed | borderless | fullscreen  (live)
    PORT_SETTING_RESOLUTION,    // [display] resolution     HALO3_RESOLUTION     native | WxH (1920x1080, 3440x1440, ...)
    PORT_SETTING_RES_SCALE,     // [display] res_scale      HALO3_RES_SCALE      1 | 2 | 3
    PORT_SETTING_FPS_CAP,       // [display] fps_cap        HALO3_FPS            0 (uncapped) | 30..1000          (live)
    PORT_SETTING_VSYNC,         // [display] vsync          HALO3_VSYNC          auto | 1 | 0                     (live)
    PORT_SETTING_RENDERER,      // [display] renderer       HALO3_RENDERER       auto | vulkan | d3d12
    PORT_SETTING_HD_TEXT,       // [display] hd_text        HALO3_HD_TEXT        0 | 1                            (live)
    PORT_SETTING_DRAW_DISTANCE, // [display] draw_distance  HALO3_DRAW_DISTANCE  1 | 2 | 4                        (live)
    PORT_SETTING_ANISOTROPY,    // [display] anisotropy     HALO3_ANISOTROPY     0 | 4 | 8 | 16                   (live)
    PORT_SETTING_SHADOW_RES,    // [display] shadow_resolution HALO3_SHADOW_RES  512 | 1024 | 2048
    PORT_SETTING_SPLITSCREEN_QUALITY, // [display] splitscreen_quality HALO3_SPLITSCREEN_QUALITY 0 | 1            (live)
    PORT_SETTING_SPLITSCREEN_SYMMETRIC_HUD, // [display] splitscreen_symmetric_hud HALO3_SPLITSCREEN_SYMMETRIC_HUD (live)
    PORT_SETTING_AUDIO_BACKEND, // [audio]   backend        HALO3_AUDIO_BACKEND  auto | null | pulse | alsa | pipe | winmm
    PORT_SETTING_MIC_DEVICE,    // [audio]   microphone     HALO3_MIC_DEVICE     off | default | <device id>      (live)
    PORT_SETTING_LAN,           // [network] lan            HALO3_LAN            1 | 0
    PORT_SETTING_LAN_PORT,      // [network] lan_port       HALO3_LAN_PORT       1..65535
    PORT_SETTING_LAN_PEER,      // [network] lan_peer       HALO3_LAN_PEER       ip[:port],...
    PORT_SETTING_LIVE_SERVER,   // [network] live_server    HALO3_LIVE_SERVER    http://host:port
    PORT_SETTING_COOP_4P,       // [network] coop_4p        HALO3_COOP_4P        1 | 0                            (live)
    PORT_SETTING_MAPS_DIR,      // [paths]   maps_dir       HALO3_DISC           the maps' folder, or the game folder
    PORT_SETTING_SHADERS_PACK,  // [paths]   shaders_pack   HALO3_SHADER_PACK    the shader pack file
    PORT_SETTING_SHADERS_PACK_DXIL, // [paths] shaders_pack_dxil HALO3_SHADER_PACK_DXIL the Direct3D 12 renderer's pack
    PORT_SETTING_USERDATA_DIR,  // [paths]   userdata_dir   HALO3_DATA           profiles, saves, caches
    PORT_SETTING_RAW_MOUSE,     // [input]   raw_mouse      HALO3_RAW_MOUSE      1 | 0                            (live)
    PORT_SETTING_MOUSE_SENSITIVITY, // [input] mouse_sensitivity HALO3_MOUSE_SENS 0.1 .. 10.0                     (live)
    PORT_SETTING_MOUSE_ACCELERATION, // [input] mouse_acceleration HALO3_MOUSE_ACCEL 0.0 .. 2.0                   (live)
    PORT_SETTING_MOUSE_ZOOM_SENSITIVITY, // [input] mouse_zoom_sensitivity HALO3_MOUSE_ZOOM_SENS 0.1 .. 3.0       (live)
    PORT_SETTING_MOUSE_VEHICLE_SENSITIVITY, // [input] mouse_vehicle_sensitivity HALO3_MOUSE_VEHICLE_SENS 0.1 .. 3.0 (live)
    PORT_SETTING_INVERT_MOUSE_Y, // [input]  invert_mouse_y HALO3_INVERT_MOUSE_Y 0 | 1                            (live)
    PORT_SETTING_CROUCH_HOLD,   // [input]   crouch_hold    HALO3_CROUCH_HOLD    1 (hold) | 0 (toggle)            (live)
    PORT_SETTING_KBM_PRESET,    // [input]   kbm_preset     HALO3_KBM_PRESET     modern | classic                 (live)
    // [bindings] <action> = <inputs>: one per keyboard and mouse action (kbm_bindings.h's PORT_KBM_ACTIONS, in its
    // order: PORT_SETTING_BINDING_FIRST + port_kbm_action), "space", "1, wheel", "none"; no variable (live)
    PORT_SETTING_BINDING_FIRST,
    PORT_SETTING_COUNT = PORT_SETTING_BINDING_FIRST + PORT_KBM_ACTION_COUNT
};

// PORT_SETTING_WINDOW_MODE's values, as port_config_window_mode gives them.
enum port_window_mode {
    PORT_WINDOW_MODE_WINDOWED,   // a framed window of the game's size
    PORT_WINDOW_MODE_BORDERLESS, // no frame, filling the screen's work area (the taskbar or panels stay)
    PORT_WINDOW_MODE_FULLSCREEN, // the whole screen (the window system's fullscreen state; the display mode is kept)
};

#define PORT_CONFIG_VALUE 512 // bytes of a setting's value, the terminator included

// Reads the launch's variables and halo3.ini and sets the variables the launch left unset (see the top). Once, first
// thing in the process, before any other thread runs.
void port_config_init(void);

// halo3.ini's path ("" when no halo3.ini is in use: then settings changed are kept for this run only), and whether the
// game folder is the portable one.
const char *port_config_path(void);
int port_config_portable(void);

// A setting's [section], key and variable.
const char *port_config_section(int setting);
const char *port_config_key(int setting);
const char *port_config_variable(int setting);
// The setting applies at once when changed (the window mode, the frame rate cap, vsync); the others at the next launch.
int port_config_live(int setting);
// The launch set the setting's variable: its value is the variable's for this run, and it cannot be changed.
int port_config_locked(int setting);
// The setting's value as the player chose it (the variable's when locked), normalized ("windowed", "3", "auto", "1"):
// what the Guide shows. Returns its length.
unsigned long port_config_value(int setting, char *out, unsigned long capacity);
// The setting's compiled default (a player's launch's, for System Link).
const char *port_config_default(int setting);
// A setting that applies at the next launch has been changed from the value this run started with.
int port_config_restart_pending(int setting);
// Changes the setting (not a locked one): `value` in any form halo3.ini takes ("on", "Fullscreen", " 60 "). Returns 1
// when it changed, 0 when it already had that value or is locked, -1 for a value it cannot take. A live setting applies
// at once (port_config_revision changes). halo3.ini is written by port_config_save.
int port_config_set(int setting, const char *value);
// Writes the settings to halo3.ini, keeping the file's comments, other keys and order, and adding what it lacks (a new
// file has every setting with what it does). Returns 0 (ERROR_SUCCESS), ERROR_NOT_SUPPORTED (50) when no halo3.ini is
// in use, or the host's Win32 error. Any thread; not under a lock of the caller's that the game's threads wait on (it
// writes and flushes a file).
unsigned long port_config_save(void);

// The resolution this run renders at ([display] resolution as the launch had it; a change applies at the next launch):
// 1 and its size for a custom WxH, in square pixels, the game's render targets and display mode that size and res_scale
// 1; 0 (and 0 x 0) for native, the game's own 1152x640 shown 16:9 (res_scale applies). Any thread, any time.
PORT_MS_ABI int port_config_resolution(unsigned int *width, unsigned int *height);

// Changes with every change of a live setting: their readers below compare it with the one they last saw.
unsigned long port_config_revision(void);
// The live settings as they are now.
int port_config_window_mode(void);  // port_window_mode
unsigned long port_config_fps_cap(void); // 0 uncapped, 30..1000
int port_config_vsync(void);        // -1 auto (on at 30 FPS, off above), 0 off, 1 on
PORT_MS_ABI int port_config_hd_text(void);     // 0 original fonts, 1 HD fonts
PORT_MS_ABI float port_config_draw_distance(void); // 1 original, 2 high, 4 ultra
PORT_MS_ABI unsigned int port_config_anisotropy(void); // 0 original, 4, 8, 16
PORT_MS_ABI unsigned int port_config_shadow_resolution(void); // 512 original, 1024 high, 2048 ultra (restart)
PORT_MS_ABI int port_config_splitscreen_full_quality(void); // 0 original console reductions, 1 full quality
PORT_MS_ABI int port_config_splitscreen_symmetric_hud(void); // 0 retail TV overscan, 1 symmetric per viewport
PORT_MS_ABI int port_config_coop_4p(void);     // 1 on, 0 off
// [audio] microphone: "off", "default", or a host capture device ID (live). Returns its length.
PORT_MS_ABI unsigned long port_config_mic_device(char *out, unsigned long capacity);
// [input]: the keyboard and mouse (port/sdk/input/port_input.h). Any thread.
int port_config_raw_mouse(void);                 // 1 raw mouse look, 0 the mouse as a right stick
float port_config_mouse_sensitivity(void);       // 0.1 .. 10.0
float port_config_mouse_acceleration(void);      // 0.0 (off) .. 2.0
float port_config_mouse_zoom_sensitivity(void);  // 0.1 .. 3.0
float port_config_mouse_vehicle_sensitivity(void); // 0.1 .. 3.0
int port_config_invert_mouse_y(void);            // 1: moving the mouse forward looks down
int port_config_crouch_hold(void);          // 1 hold to crouch, 0 a press toggles it
int port_config_kbm_preset(void);           // port_kbm_preset: what the bindings started from
// [bindings]: every action's inputs as they are now (a binding halo3.ini leaves out has kbm_preset's).
void port_config_kbm_bindings(port_kbm_bindings *out);
// Sets the bindings that differ from `bindings` (the Guide's capture: port_kbm_assign on port_config_kbm_bindings).
// Returns how many changed. halo3.ini is written by port_config_save.
int port_config_set_kbm_bindings(const port_kbm_bindings *bindings);
// Applies a preset: kbm_preset set to it and every binding to its (the Guide's Preset row, and Reset to MCC Defaults
// with the preset chosen). Returns 1 when anything changed. A locked kbm_preset (HALO3_KBM_PRESET) still has its
// bindings applied.
int port_config_apply_kbm_preset(int preset);

#ifdef __cplusplus
}
#endif

#endif
