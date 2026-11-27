// port/sdk/input (both hosts): the keyboard and mouse bindings, one table for everything that uses them
// (kbm_bindings.c): the input poll (keyboard_mouse.c), halo3.ini's [bindings] section (port/sdk/runtime/port_config.c)
// and the Guide's Settings > Keyboard & Mouse page (port/sdk/xam/guide.c).
//
// A binding is a gameplay action of the keyboard and mouse (PORT_KBM_ACTIONS) and the inputs that press it: up to
// PORT_KBM_BINDS keys or mouse buttons (port_kbm_code), a primary and an alternate. The actions drive Halo 3's own game
// actions (the input abstraction's e_input_action: tu/blamlibxbox/input_abstraction.cpp) directly, as a second device
// beside the controller: a controller's button preset (Default, Bumper Jumper, ...) maps its buttons onto those actions
// and never sees the keyboard, and the keyboard's bindings never see the preset (port_input.h). Two presets are
// defined, after Halo 3's keyboard layouts in the Master Chief Collection on PC: MCC Modern (the default) and MCC
// Classic (grenade on the right mouse button, zoom on the middle one). A preset is where the bindings start: the Guide
// applies one, and a binding halo3.ini leaves out takes the preset's.
//
// Two bindings may share an input only when Halo 3 never wants both at once (port_kbm_compatible): the actions of the
// left hand (Fire Left Weapon) exist only while dual wielding, when nothing zooms and no grenade is thrown, and Vehicle
// Boost only in a vehicle's seat. Otherwise the Guide's capture moves the input: the action that had it takes the
// rebound action's old input in exchange, or none (port_kbm_assign).
//
// Everything here is plain data and pure functions: no state, no host calls (the unit tests link it alone).
#ifndef PORT_INPUT_KBM_BINDINGS_H
#define PORT_INPUT_KBM_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

// ---- inputs ----

// The keys of the game window (the windows translate host key codes into these; HALO3_INPUT_SCRIPT's kbm= entries name
// them as port_kbm_code_of does). Left and right modifiers are two keys each.
enum e_port_key {
    k_port_key_none = 0,
    k_port_key_a, k_port_key_b, k_port_key_c, k_port_key_d, k_port_key_e, k_port_key_f, k_port_key_g, k_port_key_h,
    k_port_key_i, k_port_key_j, k_port_key_k, k_port_key_l, k_port_key_m, k_port_key_n, k_port_key_o, k_port_key_p,
    k_port_key_q, k_port_key_r, k_port_key_s, k_port_key_t, k_port_key_u, k_port_key_v, k_port_key_w, k_port_key_x,
    k_port_key_y, k_port_key_z,
    k_port_key_0, k_port_key_1, k_port_key_2, k_port_key_3, k_port_key_4, k_port_key_5, k_port_key_6, k_port_key_7,
    k_port_key_8, k_port_key_9,
    k_port_key_f1, k_port_key_f2, k_port_key_f3, k_port_key_f4, k_port_key_f5, k_port_key_f6, k_port_key_f7,
    k_port_key_f8, k_port_key_f9, k_port_key_f10, k_port_key_f11, k_port_key_f12,
    k_port_key_escape, k_port_key_tab, k_port_key_space, k_port_key_return, k_port_key_backspace,
    k_port_key_up, k_port_key_down, k_port_key_left, k_port_key_right,
    k_port_key_lshift, k_port_key_rshift, k_port_key_lctrl, k_port_key_rctrl, k_port_key_lalt, k_port_key_ralt,
    k_port_key_capslock, k_port_key_home, k_port_key_end, k_port_key_insert, k_port_key_delete, k_port_key_pageup,
    k_port_key_pagedown,
    k_port_key_grave, k_port_key_minus, k_port_key_equals, k_port_key_lbracket, k_port_key_rbracket,
    k_port_key_backslash, k_port_key_semicolon, k_port_key_apostrophe, k_port_key_comma, k_port_key_period,
    k_port_key_slash,
    k_port_key_count
};

// The mouse's buttons (mouse4 / mouse5: the side buttons, back and forward).
enum e_port_mouse_button {
    k_port_mouse_left = 0,
    k_port_mouse_right,
    k_port_mouse_middle,
    k_port_mouse_4,
    k_port_mouse_5,
    k_port_mouse_button_count
};

// What a binding names: a key (1 .. k_port_key_count - 1), a mouse button, a wheel notch up or down, or a notch either
// way (PORT_KBM_WHEEL: what a binding takes, never what an input is).
enum port_kbm_code {
    PORT_KBM_NONE = 0,
    PORT_KBM_MOUSE_FIRST = k_port_key_count, // + e_port_mouse_button
    PORT_KBM_WHEEL_UP = PORT_KBM_MOUSE_FIRST + k_port_mouse_button_count,
    PORT_KBM_WHEEL_DOWN,
    PORT_KBM_WHEEL,
    PORT_KBM_CODE_COUNT
};
#define PORT_KBM_MOUSE(button) (PORT_KBM_MOUSE_FIRST + (button))

// The code halo3.ini and HALO3_INPUT_SCRIPT spell `name` (`length` bytes; any case: "space", "lctrl", "mouse_left",
// "mouse4", "wheel_up", "wheel"; a few other spellings too: "ctrl", "enter", "esc", "lmb"), or -1.
int port_kbm_code_of(const char *name, unsigned long length);
// The code's name as halo3.ini writes it ("" for none), and as the Guide shows it ("LEFT CTRL", "LMB", "WHEEL UP").
const char *port_kbm_code_name(int code);
const char *port_kbm_code_label(int code);
// Two codes name the same input (equal, or the wheel and one of its directions).
int port_kbm_codes_overlap(int a, int b);

// ---- the actions ----

// PORT_KBM_ACTIONS(X): X(ID, key, label, modern, classic), in the order the Guide lists them and halo3.ini writes them.
// ID names PORT_KBM_<ID>; key is halo3.ini's [bindings] key; label the Guide's row; modern / classic the presets'
// bindings as halo3.ini writes them ("1, wheel": a primary and an alternate).
#define PORT_KBM_ACTIONS(X)                                                                                         \
    X(MOVE_FORWARD, "move_forward", "Move Forward", "w", "w")                                                       \
    X(MOVE_BACK, "move_back", "Move Back", "s", "s")                                                                \
    X(MOVE_LEFT, "move_left", "Move Left", "a", "a")                                                                \
    X(MOVE_RIGHT, "move_right", "Move Right", "d", "d")                                                             \
    X(JUMP, "jump", "Jump", "space", "space")                                                                       \
    X(CROUCH, "crouch", "Crouch", "lctrl", "lctrl")                                                                 \
    X(FIRE, "fire", "Fire Weapon", "mouse_left", "mouse_left")                                                      \
    X(ZOOM, "zoom", "Zoom", "mouse_right", "mouse_middle, z")                                                       \
    X(FIRE_LEFT, "fire_left", "Fire Left Weapon", "mouse_right", "mouse_right")                                     \
    X(MELEE, "melee", "Melee", "f", "f")                                                                            \
    X(THROW_GRENADE, "throw_grenade", "Throw Grenade", "g", "mouse_right")                                          \
    X(RELOAD, "reload", "Reload", "r", "r")                                                                         \
    X(ACTION, "action", "Action / Enter Vehicle", "e", "e")                                                         \
    X(USE_EQUIPMENT, "use_equipment", "Use Equipment", "q", "q")                                                    \
    X(SWITCH_WEAPON, "switch_weapon", "Switch Weapon", "1, wheel", "1, wheel")                                      \
    X(SWITCH_GRENADE, "switch_grenade", "Switch Grenade", "2", "g")                                                 \
    X(FLASHLIGHT, "flashlight", "Flashlight", "v", "v")                                                             \
    X(VEHICLE_BOOST, "vehicle_boost", "Vehicle Boost", "lshift", "lshift")                                          \
    X(DPAD_UP, "dpad_up", "D-Pad Up", "up", "up")                                                                   \
    X(DPAD_DOWN, "dpad_down", "D-Pad Down", "down", "down")                                                         \
    X(DPAD_LEFT, "dpad_left", "D-Pad Left", "left", "left")                                                         \
    X(DPAD_RIGHT, "dpad_right", "D-Pad Right", "right", "right")                                                    \
    X(SCOREBOARD, "scoreboard", "Scoreboard", "tab", "tab")                                                         \
    X(PAUSE, "pause", "Pause Menu", "escape", "escape")

enum port_kbm_action {
#define PORT_KBM_ENUM(id, key, label, modern, classic) PORT_KBM_##id,
    PORT_KBM_ACTIONS(PORT_KBM_ENUM)
#undef PORT_KBM_ENUM
    PORT_KBM_ACTION_COUNT
};

#define PORT_KBM_BINDS 2 // inputs per action: the primary and the alternate

enum port_kbm_preset {
    PORT_KBM_PRESET_MODERN, // "modern": MCC Modern (the default)
    PORT_KBM_PRESET_CLASSIC, // "classic": MCC Classic
    PORT_KBM_PRESET_COUNT
};

// The preset halo3.ini's [input] kbm_preset names ("modern", "classic"; any case), or -1; and its name and the Guide's
// label ("MCC Modern").
int port_kbm_preset_of(const char *name, unsigned long length);
const char *port_kbm_preset_name(int preset);
const char *port_kbm_preset_label(int preset);

const char *port_kbm_action_key(int action);   // halo3.ini's [bindings] key
const char *port_kbm_action_label(int action); // the Guide's row
const char *port_kbm_action_default(int preset, int action); // the preset's binding, as halo3.ini writes it
// The action cannot be rebound (Pause Menu: Escape, which also cancels the Guide's capture).
int port_kbm_action_fixed(int action);

// Halo 3's game actions (the input abstraction's e_input_action, 28 of them: what a controller's button preset maps its
// buttons onto). Named by what the game does with each (player_control.cpp, players.cpp, and the HUD's button names in
// user_interface_text.cpp's parse_controller_button_string); include/input/input_abstraction_types.h names some by
// guess.
enum port_halo_action {
    PORT_HALO_JUMP = 0,
    PORT_HALO_SWITCH_GRENADE = 1,
    PORT_HALO_SWITCH_WEAPON = 2,
    PORT_HALO_ACTION_RELOAD = 3,      // tap: reload (flip a vehicle, leave a seat); hold: pick up, enter, board
    PORT_HALO_ACTION_RELOAD_LEFT = 4, // the left hand's: tap reloads it, hold picks a weapon up into it (dual wield)
    PORT_HALO_MELEE = 5,
    PORT_HALO_USE_EQUIPMENT = 6,
    PORT_HALO_THROW_GRENADE = 7,
    PORT_HALO_FIRE = 8,               // the right trigger
    PORT_HALO_FIRE_LEFT = 9,          // the left trigger: the left hand's weapon; in a vehicle's seat, its boost
    PORT_HALO_MELEE_FIRE = 10,        // beside melee in every preset; read in a vehicle's seat
    PORT_HALO_START = 11,
    PORT_HALO_BACK = 12,
    PORT_HALO_CROUCH = 13,
    PORT_HALO_ZOOM = 14,
    PORT_HALO_FLASHLIGHT = 17,
    PORT_HALO_ACCEPT = 18,
    PORT_HALO_CANCEL = 19,
    PORT_HALO_RISE = 24,              // vertical throttle up (flying: the Forge monitor)
    PORT_HALO_DESCEND = 25,           // ... down
    PORT_HALO_SKIP_CINEMATIC = 27,
    PORT_HALO_ACTION_COUNT = 28
};

// Where a binding applies (port_kbm_action_contexts), as the player is now (the game reports it each frame:
// port_input_keyboard_report_player). Exactly one holds at a time.
#define PORT_KBM_CONTEXT_FOOT 0x1u        // on foot, one weapon (or none)
#define PORT_KBM_CONTEXT_DUAL 0x2u        // dual wielding
#define PORT_KBM_CONTEXT_SEAT 0x4u        // in a vehicle's seat, no weapon in hand (driving, a turret)
#define PORT_KBM_CONTEXT_SEAT_ARMED 0x8u  // in a seat with a weapon in hand (a passenger's)
#define PORT_KBM_CONTEXT_ALL 0xfu

// How a binding presses Halo 3's actions (port_kbm_action_role).
enum port_kbm_role {
    PORT_KBM_ROLE_BUTTON,  // as the controller's button: down while held, its time held counting
    PORT_KBM_ROLE_TAP,     // down while held, but never held long: Reload is the action button's tap (reload), never
                           // its hold (pick up a weapon, enter a vehicle), which Action is
    PORT_KBM_ROLE_AXIS,    // a movement direction (the left stick's): no game action
    PORT_KBM_ROLE_CONTROLLER, // a controller button the game's menus read (Pause: Start, Scoreboard: Back), pressed on
                              // the keyboard's controller (port_kbm_action_controller_button)
};

// Halo 3's actions the binding presses (a bit per port_halo_action), the contexts it applies in, its role, and (for
// PORT_KBM_ROLE_CONTROLLER) the XINPUT_GAMEPAD button.
unsigned long port_kbm_action_halo(int action);
unsigned port_kbm_action_contexts(int action);
int port_kbm_action_role(int action);
unsigned short port_kbm_action_controller_button(int action);
// Two actions may share an input: they never apply in the same context.
int port_kbm_compatible(int a, int b);

// ---- a set of bindings ----

typedef struct port_kbm_bindings {
    unsigned char codes[PORT_KBM_ACTION_COUNT][PORT_KBM_BINDS]; // port_kbm_code; PORT_KBM_NONE for an empty slot
} port_kbm_bindings;

// One action's binding as halo3.ini writes it: "space", "1, wheel", "none" for no input. Parsing takes any case, spaces
// around the names, "," between them; "none" or "" (an empty slot, as the second of "space, none") for none.
// Returns 0 when `text` is not a binding (an unknown name, more than PORT_KBM_BINDS inputs).
int port_kbm_parse(const char *text, unsigned long length, unsigned char codes[PORT_KBM_BINDS]);
// Writes "space", "1, wheel" or "none" into `out` (at most `capacity` bytes, NUL included). Returns its length.
unsigned long port_kbm_format(const unsigned char codes[PORT_KBM_BINDS], char *out, unsigned long capacity);
// The Guide's view of a binding: "SPACE", "1 / WHEEL", "UNBOUND".
unsigned long port_kbm_format_label(const unsigned char codes[PORT_KBM_BINDS], char *out, unsigned long capacity);

// The preset's bindings.
void port_kbm_preset_bindings(int preset, port_kbm_bindings *out);

// What port_kbm_assign changed besides the action rebound.
typedef struct port_kbm_assign_result {
    int changed;        // the binding changed at all (0: it had that input already)
    int other;          // the action that had the input (-1 none): it now has `other_code` in that slot instead
    int other_slot;
    int other_code;     // PORT_KBM_NONE: it lost the input and has none in that slot
} port_kbm_assign_result;

// Binds input `code` (PORT_KBM_NONE: unbinds) to slot `slot` of `action`. An action that cannot share the input
// (port_kbm_compatible) gives it up: it takes the slot's old input in exchange when that one is free for it, else none.
// The same input in the action's other slot is dropped. Returns 0 for what cannot be bound (a fixed action, Escape, an
// out-of-range value), else 1 with what changed in *result.
int port_kbm_assign(port_kbm_bindings *bindings, int action, int slot, int code, port_kbm_assign_result *result);

#ifdef __cplusplus
}
#endif

#endif
