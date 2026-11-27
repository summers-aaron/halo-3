#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

// Functions defined in tu/blamlibxbox/input.cpp that other units call.

#include "math/real_math_types.h"
#include <stddef.h>

struct gamepad_state {
    unsigned char trigger[2];
    unsigned char trigger_threshold[2];
    unsigned char button_frames[16];
    unsigned short button_milliseconds[16];
    point2d sticks[2];
};

typedef char input_gamepad_state_size[(sizeof(gamepad_state) == 60) ? 1 : -1];

struct s_input_globals {
    bool initialized;
    bool input_suppressed;
    bool rumble_suppressed;
    long last_update_time;
    unsigned short shell_initialization_flags;
    char padA[8];
    bool gamepad_connected[4];
    gamepad_state gamepads[4];
    char pad106[18];
    gamepad_state suppressed_gamepad;
    unsigned short rumble[4][2];
    long memory_units_last_change_time;
};

typedef char input_globals_flags_offset[(offsetof(s_input_globals, shell_initialization_flags) == 8) ? 1 : -1];
typedef char input_globals_memory_unit_time_offset[(offsetof(s_input_globals, memory_units_last_change_time) == 0x164) ? 1 : -1];
typedef char input_globals_size[(sizeof(s_input_globals) == 0x168) ? 1 : -1];

extern "C" bool input_has_button_pressed(int button);

#endif // INPUT_INPUT_H
