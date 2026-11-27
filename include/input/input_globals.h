#ifndef INPUT_INPUT_GLOBALS_H
#define INPUT_INPUT_GLOBALS_H

#include "input/input.h"

// input_xenon.cpp owns this state.
extern "C" s_input_globals input_globals;

// Whether a gamepad is connected at this index (March 2007 defines it in input_xenon.obj).
__forceinline bool input_has_gamepad(short gamepad_index)
{
    return input_globals.gamepad_connected[gamepad_index];
}

#endif // INPUT_INPUT_GLOBALS_H
