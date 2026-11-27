// OBJ: from the records 0x803e0 0x1f5f50
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "input/input_xinput.h"
#include "input/input.h"
#include <string.h>
struct debug_gamepad_data;

// Masks are XINPUT_GAMEPAD_DPAD_UP/DOWN/LEFT/RIGHT, START, BACK, LEFT/RIGHT_THUMB,
// A, B, X, Y, LEFT/RIGHT_SHOULDER (xinputdefs.h needs xtl.h, which conflicts here).
// @address 0x3929c k_xinput_button_masks
const long k_xinput_button_masks[14] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x1000, 0x2000, 0x4000, 0x8000, 0x0100, 0x0200
};

static __forceinline short input_xinput_scale_axis(short value, double dead_zone)
{
    double result = 0.0;
    double v = value;
    if (v > dead_zone)
        result = (v - dead_zone) * 32767.0 / (32767.0 - dead_zone);
    if (v < -dead_zone)
        result = (v + dead_zone) * -32768.0 / (dead_zone - 32768.0);
    return (short)result;
}

static __forceinline void input_xinput_update_button(unsigned char *frames, unsigned short *time, bool down, long milliseconds)
{
    *frames = down ? (*frames + 1 < 255 ? *frames + 1 : 255) : 0;
    *time = down ? (*time + milliseconds < 65535 ? *time + milliseconds : 65535) : 0;
}
static __forceinline void input_xinput_update_trigger(unsigned char *threshold, bool down, unsigned char value)
{
    if (down) {
        unsigned char lower = value < 32 ? 0 : value - 32;
        if (lower > *threshold)
            *threshold = lower;
    } else {
        unsigned char upper = value > 191 ? 255 : value + 64;
        if (upper < *threshold)
            *threshold = upper;
    }
}
// @address 0x803e0 ?input_xinput_update_gamepad@@YA_NKKPAUgamepad_state@@PAUdebug_gamepad_data@@@Z
bool input_xinput_update_gamepad(unsigned long user_index, unsigned long milliseconds,
    gamepad_state *out_state, debug_gamepad_data *out_debug_gamepad_data)
{
    bool result = false;
    s_xinput_state input;
    unsigned long status = XInputGetState(user_index, &input);
    if (status == 0) {
        gamepad_state *state = out_state;
        for (short trigger = 0; trigger < 2; trigger++) {
            state->trigger[trigger] = trigger ? input.right_trigger : input.left_trigger;
            bool down = state->trigger[trigger] > state->trigger_threshold[trigger];
            input_xinput_update_button(&state->button_frames[trigger], &state->button_milliseconds[trigger], down, (long)milliseconds);
            input_xinput_update_trigger(&state->trigger_threshold[trigger], down, state->trigger[trigger]);
        }
        for (short button = 0; button < 14; button++) {
            input_xinput_update_button(&state->button_frames[button + 2], &state->button_milliseconds[button + 2],
                (input.buttons & k_xinput_button_masks[button]) > 0, (long)milliseconds);
        }
        udpate_game_stick(true, &state->sticks[0], input.left_x, input.left_y);
        udpate_game_stick(false, &state->sticks[1], input.right_x, input.right_y);
        result = true;
    } else {
        memset(out_state, 0, sizeof(*out_state));
    }
    return result;
}

// @address 0x1f5f50 ?udpate_game_stick@@YAX_NPATpoint2d@@FF@Z
void udpate_game_stick(bool left_stick, point2d *stick, short x, short y)
{
    short dead_zone = left_stick ? 7849 : 8689;
    stick->x = input_xinput_scale_axis(x, dead_zone);
    stick->y = input_xinput_scale_axis(y, dead_zone);
}
