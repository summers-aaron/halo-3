#ifndef INPUT_INPUT_XINPUT_H
#define INPUT_INPUT_XINPUT_H

// Functions defined in tu/blamlibxbox/input_xinput.cpp.

union point2d;

struct gamepad_state;
struct debug_gamepad_data;
struct s_xinput_state {
    unsigned long packet_number;
    unsigned short buttons;
    unsigned char left_trigger;
    unsigned char right_trigger;
    short left_x, left_y, right_x, right_y;
};

struct s_xinput_gamepad {
    unsigned short buttons;
    unsigned char left_trigger;
    unsigned char right_trigger;
    short thumb_lx;
    short thumb_ly;
    short thumb_rx;
    short thumb_ry;
};
struct s_xinput_vibration {
    unsigned short left_motor_speed;
    unsigned short right_motor_speed;
};
struct s_xinput_capabilities {
    unsigned char type;
    unsigned char sub_type;
    unsigned short flags;
    s_xinput_gamepad gamepad;
    s_xinput_vibration vibration;
};

extern "C" unsigned long XInputGetState(unsigned long user_index, s_xinput_state *state);

// xam.xex input imports
extern "C" unsigned long XamInputGetCapabilities(unsigned long user_index, unsigned long flags, s_xinput_capabilities *capabilities);
extern "C" unsigned long XamInputSetState(unsigned long user_index, unsigned long flags, s_xinput_vibration *vibration,
    unsigned char amplitude, unsigned char frequency, unsigned char offset);
extern "C" const long k_xinput_button_masks[14];

bool input_xinput_update_gamepad(unsigned long user_index, unsigned long milliseconds,
    gamepad_state *out_state, debug_gamepad_data *out_debug_gamepad_data);

void udpate_game_stick(bool left_stick, point2d *stick, short x, short y);

#endif // INPUT_INPUT_XINPUT_H
