// port/sdk/input (both hosts): player input for the Xbox controller API (XamInputGetState / XInputGetState) and for the
// game's input abstraction (its game actions).
//
// Two kinds of device feed controller 0:
//   - a hardware gamepad (controllers 0..3): /dev/input/js<n> on Linux (port/sdk/runtime/input_linux.c), the host's
//     XInput on Windows (port/sdk/runtime/input_windows.c), merged by XamInputGetState (port/sdk/xam/xam.c). Halo 3
//     maps its buttons onto its game actions through the profile's button preset (Default, Bumper Jumper, ...) and
//     its sticks through the joystick preset, as on the console;
//   - the keyboard and mouse of the game window (HALO3_WINDOW=1): window_linux.c / window_windows.c report key, button
//     and wheel changes and pointer motion here (keyboard_mouse.c). In play they drive Halo 3's game actions directly,
//     through their own bindings (kbm_bindings.h: halo3.ini's [bindings], the Guide's Settings > Keyboard & Mouse),
//     never through a preset: the game's input abstraction (port/probe/tu/blamlibxbox/input_abstraction.cpp) reads them
//     as a second device beside the controller's buttons (port_input_keyboard_actions), and player_control.cpp merges
//     both per action, so a gamepad and the keyboard work at once and the controller's button layout has no effect on
//     the keyboard. Only two bindings reach the game as controller buttons, because its menus read them so: Pause Menu
//     (Escape: Start) and Scoreboard (Tab: Back). The Guide button is Home.
// The keyboard is "in play" (gameplay) while the game runs and no menu has controller 0's input (the game reports it
// every frame: port_input_keyboard_report_game); otherwise (the main menu, lobbies, the pause menu, loading) it is a
// controller for the menus, in a fixed layout:
//   arrow keys       D-pad                    W A S D          left stick
//   Enter, Space     A                        Escape, Backspace B
//   X, Y             X, Y                     Q, E             left / right shoulder
//   Tab              Back                     Page Up / Down   left / right trigger
// In title menus the free pointer focuses visible rows on movement, left click chooses, right click backs out,
// and the wheel navigates the list. The Guide handles its own pointer.
//   Home             the Guide button: the Guide (port/sdk/xam/guide.h); the game never sees it
// A key held as play and the menus trade places (Escape opening the pause menu, Space choosing Resume) does nothing
// in its new role until it is let go.
//
// Mouse look ([input] in halo3.ini, port/sdk/runtime/port_config.h, live from the Guide): raw_mouse (HALO3_RAW_MOUSE,
// default 1) makes the game read mouse motion as an angle through its own raw-look path (s_game_input_state::raw_look:
// no dead zone, no response curve, no turn acceleration, no rate cap), one turn per main-loop frame; mouse_sensitivity
// (HALO3_MOUSE_SENS, default 1.0) scales it: 0.022 degrees per count at 1.0, the Quake/Source m_yaw scale, so a
// sensitivity carries over from those games. The profile's look sensitivity does not change it; the game's zoom
// magnification, stun and the slower turn rates of some vehicle seats still apply, as to the sticks. invert_mouse_y
// turns the mouse's up and down over (the profile's inversion is the controller's). Without raw mouse look the mouse
// moves a right stick: HALO3_MOUSE_SENSITIVITY (stick units per pixel of motion per poll, default 900) times
// mouse_sensitivity; HALO3_MOUSE_CAPTURE=1 keeps the pointer in the window (re-centred after each motion).
// On X11 (and XWayland) the counts are XInput2 raw motion, on Windows Raw Input's (WM_INPUT) (unaccelerated, unclamped
// by the window or screen edge), with the pointer hidden and confined to the window while it has focus; without them
// the counts are the pointer's own motion with the pointer re-centred. Gamepad sticks are unchanged in every frame the
// mouse did not move (in a frame it moved, the mouse's turn replaces the stick's); while the mouse aims
// (port_input_mouse_aiming) the game leaves out its aim-assist magnetism. crouch_hold (default 1) makes Crouch a hold;
// 0 a toggle (a press crouches, the next stands; a jump or a new life stands too).
//
// HALO3_INPUT_SCRIPT="<ms>:<key>[@<controller>][:<hold ms>],..." (keyboard_mouse.c) presses buttons of a scripted
// gamepad at fixed times after the first poll, named as keys of the port's first keyboard layout (space A, escape B,
// rt / lt the triggers, w a s d the left stick, ...); with a script the keyboard source is present without a window
// (headless runs and tests): controller 0 has the script's gamepad merged with the keyboard, and each other controller
// (1..3) the script's "@" entries name has a gamepad of its own. Its window entries operate the game window itself:
// the in-game guide (key=, text=, click=, ...) and the keyboard and mouse (kbm=<input>, mouse=<dx>x<dy>).
#ifndef PORT_INPUT_PORT_INPUT_H
#define PORT_INPUT_PORT_INPUT_H

#include "../runtime/port_abi.h"
#include "kbm_bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

// XINPUT_GAMEPAD (12 bytes) and its button bits.
typedef struct port_input_gamepad {
    unsigned short buttons;
    unsigned char left_trigger;
    unsigned char right_trigger;
    short thumb_lx, thumb_ly, thumb_rx, thumb_ry;
} port_input_gamepad;

#define PORT_XINPUT_DPAD_UP        0x0001u
#define PORT_XINPUT_DPAD_DOWN      0x0002u
#define PORT_XINPUT_DPAD_LEFT      0x0004u
#define PORT_XINPUT_DPAD_RIGHT     0x0008u
#define PORT_XINPUT_START          0x0010u
#define PORT_XINPUT_BACK           0x0020u
#define PORT_XINPUT_LEFT_THUMB     0x0040u
#define PORT_XINPUT_RIGHT_THUMB    0x0080u
#define PORT_XINPUT_LEFT_SHOULDER  0x0100u
#define PORT_XINPUT_RIGHT_SHOULDER 0x0200u
#define PORT_XINPUT_GUIDE          0x0400u // the Guide button: the system's (port/sdk/xam/xam.c), never the title's
#define PORT_XINPUT_A              0x1000u
#define PORT_XINPUT_B              0x2000u
#define PORT_XINPUT_X              0x4000u
#define PORT_XINPUT_Y              0x8000u

#define PORT_INPUT_USER_COUNT 4u

// ---- keyboard and mouse (keyboard_mouse.c), fed by the window ----

// The window opened (1) or closed (0): the keyboard and mouse are present while it is open.
PORT_MS_ABI void port_input_keyboard_attach(int attached);
PORT_MS_ABI void port_input_key(enum e_port_key key, int down);
PORT_MS_ABI void port_input_mouse_button(enum e_port_mouse_button button, int down);
// Wheel notches: positive up (away from the user), negative down. Each notch is a press of PORT_KBM_WHEEL_UP / DOWN
// for one poll (a few are kept for the polls after when they come faster).
PORT_MS_ABI void port_input_mouse_wheel(int notches);
PORT_MS_ABI void port_input_mouse_position(int x, int y, int win_w, int win_h);
// The title's responding screen owns the pointer until play resumes or the Guide takes over.
PORT_MS_ABI void port_input_set_ui_cursor_mode(int active);
enum port_ui_mouse_action {
    port_ui_mouse_move, port_ui_mouse_choose, port_ui_mouse_back, port_ui_mouse_up, port_ui_mouse_down
};
typedef struct port_ui_mouse_event {
    enum port_ui_mouse_action action;
    int x, y, width, height;
} port_ui_mouse_event;
PORT_MS_ABI int port_input_next_ui_mouse_event(port_ui_mouse_event *event);
PORT_MS_ABI void port_input_mouse_motion(int dx, int dy);
// Focus lost: every key and button up.
PORT_MS_ABI void port_input_release_all(void);
// Whether gameplay wants pointer capture (HALO3_MOUSE_CAPTURE=1 or raw look). Title menus release it.
int port_input_mouse_capture(void);
// Whether mouse look is raw ([input] raw_mouse, HALO3_RAW_MOUSE). Follows the Guide.
PORT_MS_ABI int port_input_mouse_raw(void);
// Raw mouse motion in device counts (XInput2 raw valuators; may be fractional): with raw mouse look, the window reports
// it here and the pointer's own motion (port_input_mouse_motion) is no longer used for look.
PORT_MS_ABI void port_input_mouse_raw_motion(double dx, double dy);
// Raw mouse look for controller `user` (the keyboard and mouse are controller 0's): the angles the mouse turned since
// the last call (consumed), in radians, right and up positive ([input] invert_mouse_y applied). Returns 0, with both
// zero, when raw mouse look is off, there is no keyboard, or the mouse did not move.
PORT_MS_ABI int port_input_mouse_look(unsigned int user, float *right_radians, float *up_radians);
// Whether the mouse is controller `user`'s aiming device now: raw mouse look is on and the mouse turned in the last
// 500 ms. The game leaves out the aim-assist magnetism (the slow-down and pull over a target) then, so mouse aim stays
// linear; the sticks keep it otherwise.
PORT_MS_ABI int port_input_mouse_aiming(unsigned int user);
// Taps between two polls. The game polls a controller once a main-loop pass (60 times a second at HALO3_FPS=30);
// a key, button or scripted press that goes down and up again between two polls would never be seen. The keyboard,
// mouse buttons, input script and Linux gamepads report the events that change their state, so a "poll" (the title's
// own XamInputGetState) reports a press that went down since the previous poll as down even when it is up again by
// now: once, as a press of one poll (no added latency: a held press and a release are reported as they are). Hosts'
// XInput gamepads (Windows) report state only; a press shorter than the poll interval cannot be seen there.
// Only taps of a running game are kept: when the previous poll is more than PORT_INPUT_TAP_WINDOW_MS ago (a load or a
// stall, which input_update clamps to 100 ms too) the presses since are dropped, as the console drops them, instead of
// arriving late.
#define PORT_INPUT_TAP_WINDOW_MS 100u

// The game's poll of controller 0's keyboard (with the input script's gamepad for controller 0): the controller buttons
// it presses (in play only Pause and Scoreboard's and the Guide button; in the menus its menu layout, above) and,
// through port_input_keyboard_actions, the game actions its bindings hold, with the presses since the last poll (taps
// above) and the motion since the last call (consumed; with raw mouse look the mouse motion is left to
// port_input_mouse_look). Returns 0 when there is no keyboard (no window, no script).
int port_input_keyboard_state(port_input_gamepad *out);
// The keyboard's input is not the game's this poll (a guide dialog shows, the window lost focus: xam.c): no game
// actions, and what is held now does nothing until it is let go.
void port_input_keyboard_suppress(void);
// The virtual gamepad of controller `user` (1..3) that HALO3_INPUT_SCRIPT drives (its "<key>@<user>" entries). `poll`:
// the game's poll, which also reports the scripted presses that began and ended since the previous one (taps above);
// otherwise (0) the state now, consuming nothing. Returns 0 when the script names no entry for that controller (or for
// controller 0, whose script gamepad port_input_keyboard_state reports).
int port_input_script_state(unsigned long user, port_input_gamepad *out, int poll);

// ---- the keyboard's game actions (the game's side: port/probe/tu/blamlibxbox) ----

// What the keyboard's bindings hold in play, as of the last poll of controller 0.
typedef struct port_input_kbm_state {
    unsigned int down; // a bit per port_halo_action a binding holds down
    unsigned int hold; // of those, the ones a binding holds as a button (PORT_KBM_ROLE_BUTTON): their time held
                       // counts; the others are pressed as taps (Reload's), never held long
    float move_x, move_y; // the Move bindings: -1..1, right and forward positive (a left stick's, square)
    int look;             // the mouse moved this poll without raw mouse look: look_x / look_y are its right stick
    float look_x, look_y; // -1..1, right and up positive (invert_mouse_y applied)
} port_input_kbm_state;
// The keyboard's game actions for controller `user` (only 0 has a keyboard). Returns 0, with everything zero, when
// there is no keyboard, it is not in play (a menu has its input), or the guide or a dialog has it.
PORT_MS_ABI int port_input_keyboard_actions(unsigned int user, port_input_kbm_state *out);
// The game, once per main-loop pass while it runs (player_control_update): whether controller 0 is in play (no menu
// or screen has its input), which the next poll takes. Two polls in a row with no report between them (the game is
// not running its player control: menus, loading) are the menus; one alone is not (a pass can poll twice).
PORT_MS_ABI void port_input_keyboard_report_game(int playing);
// ... and for controller 0's player, when it has one: where it is (PORT_KBM_CONTEXT_*: which bindings apply) and the
// unit it controls (-1: none; a new one ends a crouch toggled on).
PORT_MS_ABI void port_input_keyboard_report_player(unsigned context, int unit);
// ... and whether controller 0's player is currently zoomed/scoped (state->action.zoom >= 0), for mouse_zoom_sensitivity.
PORT_MS_ABI void port_input_keyboard_report_zoom(int zoomed);
// HALO3_KBM_TRACE=1: the keyboard's play / menu changes and the game actions it presses, on stderr (diagnostics).
PORT_MS_ABI int port_input_keyboard_trace(void);
// ... and the game's side (input_abstraction.cpp): controller 0's game actions as each device presses them now (a bit
// per port_halo_action: the keyboard's own buttons, the controller's through its button preset) and the preset's
// button per action (`buttons`, `count` of them: s_gamepad_input_preferences::buttons), on stderr.
PORT_MS_ABI void port_input_keyboard_trace_actions(unsigned int keys, unsigned int controller, const unsigned char *buttons,
    int count);
// HALO3_DIAG_BUTTON_PRESET=<0..5> / HALO3_DIAG_JOYSTICK_PRESET=<0..3> (diagnostics): the button layout (`joystick` 0:
// standard, south_paw, boxer, green_thumb, professional = Bumper Jumper, walkie-talkie) or joystick layout (1) the game
// gives controller 0's profile (player_control.cpp), so a headless run shows the keyboard does not follow it; -1 unset.
PORT_MS_ABI int port_input_keyboard_diag_preset(int joystick);
// Whether Player 0's last active input source was keyboard or mouse (1) vs gamepad (0). Updated on key/mouse presses,
// mouse movement past a small threshold (|dx| + |dy| >= 8), or gamepad button/trigger/stick activity past deadzone.
PORT_MS_ABI int port_input_last_source_is_kbm(void);
// Internal (xam.c): notes active gamepad input on Player 0.
PORT_MS_ABI void port_input_note_gamepad_activity(void);
// Writes the short human-readable primary bound key name (e.g. "E", "G", "LMB", "Space", "Ctrl", "Shift", "Up",
// "Tab", "Esc") for `kbm_action` into `out` (`out_size` bytes, NUL-terminated) and returns 1 (0 if unbound or invalid).
PORT_MS_ABI int port_input_kbm_action_label(int kbm_action, char *out, unsigned int out_size);
// Current player context bitmask (PORT_KBM_CONTEXT_*) last reported by player_control_update.
PORT_MS_ABI unsigned port_input_keyboard_context(void);

// ---- hardware gamepads (port/sdk/runtime/input_<host>.c) ----

// The state of controller `user`; 0 when none is connected. `poll`: the game's poll, which also reports the buttons and
// trigger pulls that went down since the previous poll and are up again (taps above; Linux); otherwise (0) the state
// now, the taps kept for the next poll.
int port_input_hardware_state(unsigned long user, port_input_gamepad *out, int poll);
// Rumble of controller `user`: the speeds of its left (low-frequency, strong) and right (high-frequency, weak) motors,
// 0..65535 each, kept until changed (0, 0 stops it); evdev force feedback on Linux, the host's XInputSetState on
// Windows. Returns 0 when no controller is connected (a connected one without motors, or whose rumble device cannot be
// written, returns 1 and stays still).
int port_input_hardware_vibration(unsigned long user, unsigned short left, unsigned short right);
// Stops every motor (process exit, port_exit).
void port_input_hardware_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
