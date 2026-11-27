// blamlibxbox/input_xenon.obj: from the records 0x80320 0x80800 0x1a1f08 0x1a1f78
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "memory/tls_globals.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "input/input_xenon.h"
#include "game/game_time.h"
#include "input/input.h"
#include "input/input_xinput.h"

#if defined(_XBOX) && !defined(__clang__)
// match: 0x808c8: external linkage changes 56 bytes of loads in the
// input_update inlined into input_abstraction_update.
static s_input_globals input_globals;
#else
// input_globals.h declares the one definition that shell_xenon initializes.
#include "input/input_globals.h"
s_input_globals input_globals;
#endif

static __forceinline void input_xenon_update_gamepads(long milliseconds)
{
    for (long controller_index = 0; controller_index < 4; controller_index++)
    {
        gamepad_state *gamepad = &input_globals.gamepads[controller_index];
        if (input_xinput_update_gamepad(controller_index, milliseconds, gamepad, 0))
            input_globals.gamepad_connected[controller_index] = true;
        else
            input_globals.gamepad_connected[controller_index] = false;
    }
}

// @address 0x80320 ?input_update@@YAXXZ
void input_update(void)
{
    if (input_globals.initialized)
    {
        long current_time = GetTickCount();
        long milliseconds = PIN(current_time - input_globals.last_update_time, 0, 100);
        input_globals.input_suppressed = false;
        input_globals.last_update_time = current_time;
        input_xenon_update_gamepads(milliseconds);
        input_update_gamepads_rumble();
    }
}

extern "C" {
// @address 0x80800 input_set_gamepad_vibration
// match: 0x80800: preserve the accepted size-optimized kernel-version compatibility wrapper.
#pragma optimize("s", on)
void input_set_gamepad_vibration(unsigned long user_index, const s_xinput_vibration *vibration)
{
    s_xinput_vibration state = *vibration;
    s_xinput_capabilities capabilities;
    if (XboxKrnlVersion.Major == 2 && !XboxKrnlVersion.Minor && XboxKrnlVersion.Build < 0x15eb
        && !XamInputGetCapabilities(user_index, 1, &capabilities)
        && capabilities.sub_type == 2 && (capabilities.flags & 1) && (capabilities.flags & 2)) {
        state.left_motor_speed = vibration->right_motor_speed;
        state.right_motor_speed = vibration->left_motor_speed;
    }
    XamInputSetState(user_index, 0, &state, 0, 0, 0);
}
}
// match: 0x80800: restore the unit's optimization flags after the compatibility wrapper.
#pragma optimize("", on)

// @address 0x1a1f08 ?input_clear_gamepads_rumble@@YAXXZ
void input_clear_gamepads_rumble(void)
{
    if (input_globals.gamepad_connected[0])
    {
        input_globals.rumble[0][0] = 0;
        input_globals.rumble[0][1] = 0;
    }
    if (input_globals.gamepad_connected[1])
    {
        input_globals.rumble[1][0] = 0;
        input_globals.rumble[1][1] = 0;
    }
    if (input_globals.gamepad_connected[2])
    {
        input_globals.rumble[2][0] = 0;
        input_globals.rumble[2][1] = 0;
    }
    if (input_globals.gamepad_connected[3])
    {
        input_globals.rumble[3][0] = 0;
        input_globals.rumble[3][1] = 0;
    }
    input_update_gamepads_rumble();
}

// @address 0x1a1f78 ?input_update_gamepads_rumble@@YAXXZ
void input_update_gamepads_rumble(void)
{
    bool suppress_rumble = input_globals.rumble_suppressed != 0 || input_globals.input_suppressed;
    bool game_running = false;
    game_globals_storage *game_globals = tls_game_globals();
    if (game_globals != 0
        && game_globals->game_in_progress
        && !game_globals->initializing
        && game_globals->map_active)
    {
        game_running = true;
    }
    if (!game_running || game_time_is_paused())
        suppress_rumble = true;

    for (short controller_index = 0; controller_index < 4; controller_index++)
    {
        const unsigned short *rumble = input_globals.rumble[controller_index];
        s_xinput_vibration vibration;
        vibration.left_motor_speed = suppress_rumble ? 0 : rumble[0];
        vibration.right_motor_speed = suppress_rumble ? 0 : rumble[1];
        input_set_gamepad_vibration(controller_index, &vibration);
    }
}

const gamepad_state *input_get_gamepad_state(short controller_index)
{
    const gamepad_state *result = 0;
    if (input_globals.gamepad_connected[controller_index])
    {
        if (input_globals.input_suppressed)
            result = &input_globals.suppressed_gamepad;
        else
            result = &input_globals.gamepads[controller_index];
    }
    return result;
}
