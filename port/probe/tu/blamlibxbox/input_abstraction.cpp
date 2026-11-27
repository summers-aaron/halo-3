// blamlibxbox/input_abstraction.obj: from the records 0x808c8 0x20e2b8 0x20e2c8 0x20e378 0x20e390 0x20e3e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_types.h"
#include "math/real_math.h"
#include "shell/shell.h"
#include "cseries/cseries.h"
#include <math.h>
#include "interface/controller_inline.h"
#include "math/real_math_trigonometry.h"
// @address 0x20e2b8 ??0c_abstract_button@@QAA@XZ
// @address 0x20e2c8 ?input_abstraction_initialize@@YAXXZ
// @address 0x20e378 ?input_abstraction_dispose@@YAXXZ
// @address 0x20e390 ?input_abstraction_initialize_for_new_map@@YAXXZ
// @address 0x20e3e8 ?input_abstraction_get_default_preferences@@YAXPAUs_gamepad_input_preferences@@@Z
// @address 0x80608 GetTickCount
#include <string.h>
#include "input/input_abstraction.h"
#include "input/input_xenon.h"
#include "input/input.h"
#include "input/input_abstraction_types.h"
#include "input/input_globals.h"
#include "cseries/crt_xdk.h"

#if !defined(_XBOX) || defined(__clang__)
#include "input/input_abstraction_keyboard.h"
#include "rasterizer/rasterizer_globals.h"

static const long k_input_abstraction_frame_rate = 60;
static bool g_input_abstraction_frame_advanced = true;
static __int64 g_input_abstraction_last_frame = -1;
static unsigned char g_input_abstraction_button_frames[k_number_of_controllers][NUMBEROF(input_abstraction_globals.input_states[0].buttons)];

// GAME-CHANGES "Keyboard and mouse": the keyboard and mouse's own button per game action
// (input_abstraction_keyboard.h), controller 0's, updated each frame from what its bindings hold
// (port_input_keyboard_actions), with their input frames as g_input_abstraction_button_frames counts the controller's.
static c_abstract_button g_input_abstraction_keyboard_buttons[k_input_action_count];
static unsigned char g_input_abstraction_keyboard_frames[k_input_action_count];
static unsigned long g_input_abstraction_keyboard_time;
static unsigned long g_input_abstraction_trace_keys = ~0ul, g_input_abstraction_trace_pad = ~0ul;

static void input_abstraction_advance_frame_clock()
{
    const unsigned long rate = port_d3d_vertical_blank_rate();
    if (rate <= k_input_abstraction_frame_rate)
    {
        g_input_abstraction_frame_advanced = true;
        g_input_abstraction_last_frame = -1;
        return;
    }

    // Count at most one input frame per pass, even if several blanks elapsed.
    const __int64 frame = rasterizer_globals.vblank_index.peek() * k_input_abstraction_frame_rate / (__int64)rate;
    g_input_abstraction_frame_advanced = frame != g_input_abstraction_last_frame;
    g_input_abstraction_last_frame = frame;
}

static void input_abstraction_count_frames(unsigned char &count, unsigned char frames)
{
    if (frames <= 1)
        count = frames;
    else if (g_input_abstraction_frame_advanced && count < 255)
        ++count;
}

static void input_abstraction_count_button_frames(e_controller_index controller_index, long button_index, unsigned char frames)
{
    input_abstraction_count_frames(g_input_abstraction_button_frames[controller_index][button_index], frames);
}

bool input_abstraction_input_frame_advanced()
{
    return g_input_abstraction_frame_advanced;
}

unsigned char input_abstraction_button_down_input_frames(long controller_index, const c_abstract_button &button)
{
    const unsigned char frames = button.get_down_frames();
    if (!frames || port_d3d_vertical_blank_rate() <= k_input_abstraction_frame_rate)
        return frames;
    const c_abstract_button *buttons = input_abstraction_globals.input_states[controller_index].buttons;
    return g_input_abstraction_button_frames[controller_index][&button - buttons];
}

// The larger in magnitude of two axis values (the controller's and the keyboard's).
static float input_abstraction_stronger(float a, float b)
{
    return fabs(a) >= fabs(b) ? a : b;
}

static void input_abstraction_reset_keyboard()
{
    memset(g_input_abstraction_keyboard_buttons, 0, sizeof(g_input_abstraction_keyboard_buttons));
    memset(g_input_abstraction_keyboard_frames, 0, sizeof(g_input_abstraction_keyboard_frames));
    for (long action_index = 0; action_index < k_input_action_count; action_index++)
    {
        g_input_abstraction_keyboard_buttons[action_index].set_access(-1);
        g_input_abstraction_keyboard_buttons[action_index].unlock();
    }
    g_input_abstraction_keyboard_time = 0;
}

// The keyboard's buttons as its bindings hold them now (`keys`): held frames and, for a binding held as a button, its
// time held (a tap binding's stays 0: Reload never reaches the action button's hold), as input_xinput_update_gamepad
// counts a controller's.
static void input_abstraction_update_keyboard(const port_input_kbm_state *keys)
{
    const unsigned long now = GetTickCount();
    const long elapsed = g_input_abstraction_keyboard_time != 0 ? (long)(now - g_input_abstraction_keyboard_time) : 0;
    const unsigned long milliseconds = (unsigned long)PIN(elapsed, 0, 100);
    g_input_abstraction_keyboard_time = now != 0 ? now : 1;
    for (long action_index = 0; action_index < k_input_action_count; action_index++)
    {
        c_abstract_button &button = g_input_abstraction_keyboard_buttons[action_index];
        const bool down = ((keys->down >> action_index) & 1) != 0;
        const bool held = ((keys->hold >> action_index) & 1) != 0;
        const unsigned short frames =
            down ? (unsigned short)(button.m_down_frames < 255 ? button.m_down_frames + 1 : 255) : 0;
        const unsigned long time = down && held ? button.m_down_milliseconds + milliseconds : 0;
        button.update((unsigned short)(time < 65535 ? time : 65535), frames, down ? 255 : 0);
        input_abstraction_count_frames(g_input_abstraction_keyboard_frames[action_index], (unsigned char)frames);
    }
}

c_input_action c_input_actions::get_button(long action, long keyboard_action)
{
    c_abstract_button &pad = m_input_state->get_button(action);
    c_abstract_button *keys = 0;
    if (m_controller_index == 0)
    {
        keys = &g_input_abstraction_keyboard_buttons[keyboard_action];
        keys->set_access(keyboard_action);
    }
    return c_input_action(pad, keys);
}

unsigned char input_abstraction_action_down_input_frames(long controller_index, long action)
{
    s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
    const unsigned char pad =
        input_abstraction_button_down_input_frames(controller_index, input_state->get_button(action));
    if (controller_index != 0)
        return pad;
    c_abstract_button &button = g_input_abstraction_keyboard_buttons[action];
    button.set_access(action);
    unsigned char keys = button.get_down_frames();
    if (keys && port_d3d_vertical_blank_rate() > k_input_abstraction_frame_rate)
        keys = g_input_abstraction_keyboard_frames[action];
    return pad > keys ? pad : keys;
}

void input_abstraction_trace_actions(long controller_index)
{
    if (controller_index != 0 || !port_input_keyboard_trace())
        return;
    const s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
    unsigned long keys = 0, pad = 0;
    for (long action_index = 0; action_index < k_input_action_count; action_index++)
    {
        if (g_input_abstraction_keyboard_buttons[action_index].m_down_frames)
            keys |= 1ul << action_index;
        if (input_state->buttons[input_state->abstract_buttons[action_index]].m_down_frames)
            pad |= 1ul << action_index;
    }
    if (keys == g_input_abstraction_trace_keys && pad == g_input_abstraction_trace_pad)
        return;
    g_input_abstraction_trace_keys = keys;
    g_input_abstraction_trace_pad = pad;
    const s_gamepad_input_preferences *preferences =
        &input_abstraction_globals.look_settings[controller_index].preferences;
    port_input_keyboard_trace_actions(keys, pad, preferences->buttons, k_input_action_count);
}
#endif

void input_abstraction_initialize(void)
{
    memset(&input_abstraction_globals, 0, sizeof(input_abstraction_globals));
#if !defined(_XBOX) || defined(__clang__)
    g_input_abstraction_frame_advanced = true;
    g_input_abstraction_last_frame = -1;
    memset(g_input_abstraction_button_frames, 0, sizeof(g_input_abstraction_button_frames));
    input_abstraction_reset_keyboard();
#endif
    for (e_controller_index controller_index = first_controller();
        controller_index != _controller_index_none;
        controller_index = next_controller(controller_index))
    {
        input_abstraction_get_default_preferences(&input_abstraction_globals.look_settings[controller_index].preferences);
        input_abstraction_globals.controller_active[controller_index] = input_globals.gamepad_connected[(short)controller_index];
        for (long button_index = 0; button_index < 16; button_index++)
            input_abstraction_globals.input_states[controller_index].buttons[button_index].unlock();
    }
    input_abstraction_globals.last_update_milliseconds = GetTickCount();
    input_abstraction_globals.initialized = true;
}

void input_abstraction_dispose(void)
{
    memset(&input_abstraction_globals, 0, sizeof(input_abstraction_globals));
}

void input_abstraction_initialize_for_new_map(void)
{
#if !defined(_XBOX) || defined(__clang__)
    for (long action_index = 0; action_index < k_input_action_count; action_index++)
        g_input_abstraction_keyboard_buttons[action_index].unlock();
#endif
    for (e_controller_index controller_index = first_controller();
        controller_index != _controller_index_none;
        controller_index = next_controller(controller_index))
    {
        for (long button_index = 0; button_index < 16; button_index++)
            input_abstraction_globals.input_states[controller_index].buttons[button_index].unlock();
    }
}

// @rdata 0x8203e1d0..0x8203e1e0
static const float input_abstraction_quadrant_angles[4] = {
    0.7853982f, 2.3561945f, -0.7853982f, -2.3561945f
};
// @rdata-end


static __forceinline float input_abstraction_magnitude(const real_vector2d *v)
{
    return square_root(magnitude_squared2d(v));
}

static __forceinline point2d *input_abstraction_set_point(point2d *point, short x, short y)
{
    point->x = x;
    point->y = y;
    return point;
}

// @rdata 0x8203e1cc
static const float input_abstraction_thumbstick_scale = 1.0f / 32767.0f;

// @address 0x808c8
void input_abstraction_update(void)
{
#if !defined(_XBOX) || defined(__clang__)
    input_abstraction_advance_frame_clock();
#endif
    for (e_controller_index controller_index = first_controller();
        controller_index != _controller_index_none;
        controller_index = next_controller(controller_index))
    {
        const gamepad_state *gamepad = input_get_gamepad_state((short)controller_index);
        s_controller_look_settings *settings = &input_abstraction_globals.look_settings[controller_index];
        s_game_input_state *input = &input_abstraction_globals.input_states[controller_index];
        for (long action_index = 0; action_index < k_input_action_count; action_index++)
        {
            long button_index = settings->preferences.buttons[action_index];
            input->abstract_buttons[action_index] = (unsigned char)button_index;
        }
#if !defined(_XBOX) || defined(__clang__)
        // GAME-CHANGES "Keyboard and mouse": the keyboard's own buttons (controller 0's), whatever the controller does
        port_input_kbm_state keys;
        const bool keyboard = port_input_keyboard_actions(controller_index, &keys) != 0;
        if (controller_index == _controller0)
            input_abstraction_update_keyboard(&keys);
#endif
        if (gamepad)
        {
            input->thumbsticks[0] = gamepad->sticks[0];
            input->thumbsticks[1] = gamepad->sticks[1];
            input->raw_look = false;
            real_vector2d left_stick;
            real_vector2d right_stick;
            float right_angle;
            float left_angle;
            const point2d *left = &gamepad->sticks[0];
            left_angle = arctangent((float)left->y, (float)left->x);
            float left_scale = 1.0f / (fabs(sine(left_angle)) > fabs(cosine(left_angle))
                ? fabs(sine(left_angle)) : fabs(cosine(left_angle)));
            left_stick.i = PIN((float)left->x * input_abstraction_thumbstick_scale * left_scale, -1.0f, 1.0f);
            left_stick.j = PIN((float)left->y * input_abstraction_thumbstick_scale * left_scale, -1.0f, 1.0f);
            const point2d *right = &gamepad->sticks[1];
            right_angle = arctangent((float)right->y, (float)right->x);
            float right_scale = 1.0f / (fabs(sine(right_angle)) > fabs(cosine(right_angle))
                ? fabs(sine(right_angle)) : fabs(cosine(right_angle)));
            right_stick.i = PIN((float)right->x * input_abstraction_thumbstick_scale * right_scale, -1.0f, 1.0f);
            right_stick.j = PIN((float)right->y * input_abstraction_thumbstick_scale * right_scale, -1.0f, 1.0f);
            for (long button_index = 0; button_index < 16; button_index++)
            {
                unsigned char amount = gamepad->button_frames[button_index] ? 255 : 0;
                if (button_index >= 0 && button_index < 2)
                    amount = gamepad->trigger[button_index];
                input->buttons[button_index].update(gamepad->button_milliseconds[button_index], gamepad->button_frames[button_index], amount);
#if !defined(_XBOX) || defined(__clang__)
                input_abstraction_count_button_frames(controller_index, button_index, gamepad->button_frames[button_index]);
#endif
            }
            if (settings->preferences.joystick_preset == 2 || settings->preferences.joystick_preset == 3)
            {
                float look_dead_angle = 10.0f * k_degrees_to_radians;
                float move_dead_angle = k_pi * 0.25f - look_dead_angle;
                short left_quadrant = (left_stick.i < 0.0f ? 1 : 0) | (left_stick.j < 0.0f ? 2 : 0);
                short right_quadrant = (right_stick.i < 0.0f ? 1 : 0) | (right_stick.j < 0.0f ? 2 : 0);
                float left_delta = left_angle - input_abstraction_quadrant_angles[left_quadrant];
                float right_delta = right_angle - input_abstraction_quadrant_angles[right_quadrant];
                float left_magnitude = input_abstraction_magnitude(&left_stick);
                float right_magnitude = input_abstraction_magnitude(&right_stick);
                if (fabs(left_delta) < move_dead_angle)
                {
                    float angle = fabs(left_angle);
                    if (angle < k_pi * 0.25f || angle > k_pi * 0.75f)
                    {
                        left_stick.i = (left_stick.i < 0.0f ? -1 : 1) * left_magnitude;
                        left_stick.j = (left_stick.j < 0.0f ? -1 : 1) * left_magnitude * (1.0f - fabs(left_delta) / move_dead_angle);
                    }
                    else
                    {
                        left_stick.j = (left_stick.j < 0.0f ? -1 : 1) * left_magnitude;
                        left_stick.i = (left_stick.i < 0.0f ? -1 : 1) * left_magnitude * (1.0f - fabs(left_delta) / move_dead_angle);
                    }
                }
                else if (fabs(left_stick.i) > fabs(left_stick.j))
                {
                    left_stick.i = (left_stick.i < 0.0f ? -1 : 1) * left_magnitude;
                    left_stick.j = 0.0f;
                }
                else
                {
                    left_stick.j = (left_stick.j < 0.0f ? -1 : 1) * left_magnitude;
                    left_stick.i = 0.0f;
                }
                left_stick.i = PIN(left_stick.i, -1.0f, 1.0f);
                left_stick.j = PIN(left_stick.j, -1.0f, 1.0f);
                if (fabs(right_delta) < look_dead_angle)
                {
                    float angle = fabs(right_angle);
                    if (angle < k_pi * 0.25f || angle > k_pi * 0.75f)
                    {
                        right_stick.i = (right_stick.i < 0.0f ? -1 : 1) * right_magnitude;
                        right_stick.j = (right_stick.j < 0.0f ? -1 : 1) * right_magnitude * (1.0f - fabs(right_delta) / move_dead_angle);
                    }
                    else
                    {
                        right_stick.j = (right_stick.j < 0.0f ? -1 : 1) * right_magnitude;
                        right_stick.i = (right_stick.i < 0.0f ? -1 : 1) * right_magnitude * (1.0f - fabs(right_delta) / move_dead_angle);
                    }
                }
                else if (fabs(right_stick.i) > fabs(right_stick.j))
                {
                    right_stick.i = (right_stick.i < 0.0f ? -1 : 1) * right_magnitude;
                    right_stick.j = 0.0f;
                }
                else
                {
                    right_stick.j = (right_stick.j < 0.0f ? -1 : 1) * right_magnitude;
                    right_stick.i = 0.0f;
                }
                right_stick.i = PIN(right_stick.i, -1.0f, 1.0f);
                right_stick.j = PIN(right_stick.j, -1.0f, 1.0f);
            }
            switch (settings->preferences.joystick_preset)
            {
            case 0:
                input->throttle.j = -left_stick.i;
                input->throttle.i = left_stick.j;
                input->look.i = -right_stick.i;
                input->vehicle_pitch = right_stick.j;
                input->look.j = right_stick.j;
                break;
            case 1:
                input->look.i = -left_stick.i;
                input->vehicle_pitch = left_stick.j;
                input->look.j = left_stick.j;
                input->throttle.j = -right_stick.i;
                input->throttle.i = right_stick.j;
                break;
            case 2:
                input->look.i = -left_stick.i;
                input->throttle.i = left_stick.j;
                input->throttle.j = -right_stick.i;
                input->vehicle_pitch = right_stick.j;
                input->look.j = right_stick.j;
                break;
            case 3:
                input->throttle.j = -left_stick.i;
                input->vehicle_pitch = left_stick.j;
                input->look.j = left_stick.j;
                input->look.i = -right_stick.i;
                input->throttle.i = right_stick.j;
                break;
            default:
                input->throttle.j = -left_stick.i;
                input->throttle.i = left_stick.j;
                input->look.i = -right_stick.i;
                input->vehicle_pitch = right_stick.j;
                input->look.j = right_stick.j;
                break;
            }
            input->look.j *= settings->preferences.look_inverted ? -1.0f : 1.0f;
            if (!settings->preferences.look_inverted && settings->preferences.flight_stick)
                input->aircraft_pitch = input->look.j * -1.0f;
            else
                input->aircraft_pitch = input->look.j;
#if !defined(_XBOX) || defined(__clang__)
            // GAME-CHANGES "Keyboard and mouse": the keyboard's Move bindings, as a left stick in the standard joystick
            // layout (the joystick preset is the controller's), the stronger of it and the controller's on each axis
            if (keyboard && (keys.move_x != 0.0f || keys.move_y != 0.0f))
            {
                input->throttle.i = input_abstraction_stronger(input->throttle.i, keys.move_y);
                input->throttle.j = input_abstraction_stronger(input->throttle.j, -keys.move_x);
            }
            // ... and the mouse without raw look: a right stick in that layout, in a frame it moved (invert_mouse_y
            // is the mouse's own; the profile's inversion and flight stick are the controller's)
            if (keyboard && keys.look)
            {
                input->look.i = -keys.look_x;
                input->look.j = keys.look_y;
                input->vehicle_pitch = keys.look_y;
                input->aircraft_pitch = keys.look_y;
            }
            // tmp/probe (GAME-CHANGES M3): raw mouse look (invert_mouse_y applied by port_input_mouse_look).
            float mouse_right, mouse_up;
            if (port_input_mouse_look(controller_index, &mouse_right, &mouse_up)) {
                const float horizontal = settings->preferences.horizontal_look_sensitivity;
                const float vertical = settings->preferences.vertical_look_sensitivity;
                input->raw_look = true;
                input->look.i = -mouse_right * (horizontal > 0.0f ? 120.0f / horizontal : 1.0f);
                input->look.j = mouse_up * (vertical > 0.0f ? 60.0f / vertical : 1.0f);
                input->aircraft_pitch = input->look.j;
            }
#endif
            if (!input_abstraction_globals.controller_active[controller_index])
                input_abstraction_globals.controller_active[controller_index] = true;
        }
        else
        {
            if (input_abstraction_globals.controller_active[controller_index])
            {
#if !defined(_XBOX) || defined(__clang__)
                memset(g_input_abstraction_button_frames[controller_index], 0, sizeof(g_input_abstraction_button_frames[controller_index]));
#endif
                for (long button_index = 0; button_index < 16; button_index++)
                    input->buttons[button_index].update(0, 0, 0);
                for (long stick_index = 0; stick_index < 2; stick_index++)
                {
                    input_abstraction_set_point(&input->thumbsticks[stick_index], 0, 0);
                }
                input->throttle.i = 0.0f;
                input->throttle.j = 0.0f;
                input->look.i = 0.0f;
                input->look.j = 0.0f;
                input->aircraft_pitch = 0.0f;
                input->vehicle_pitch = 0.0f;
            }
            input_abstraction_globals.controller_active[controller_index] = false;
        }
    }
}
void input_abstraction_get_default_preferences(s_gamepad_input_preferences *preferences)
{
    preferences->horizontal_look_sensitivity = 120.0f;
    preferences->vertical_look_sensitivity = 60.0f;
    memset(preferences->buttons, -1, sizeof(preferences->buttons));
    preferences->buttons[1] = 14;
    preferences->buttons[3] = 15;
    preferences->buttons[4] = 14;
    preferences->buttons[5] = 11;
    preferences->buttons[6] = 12;
    preferences->buttons[2] = 13;
    preferences->buttons[0] = 10;
    preferences->buttons[7] = 0;
    preferences->buttons[8] = 1;
    preferences->buttons[9] = 0;
    preferences->buttons[10] = 11;
    preferences->buttons[11] = 6;
    preferences->buttons[12] = 7;
    preferences->buttons[13] = 8;
    preferences->buttons[14] = 9;
    preferences->buttons[15] = 4;
    preferences->buttons[16] = 5;
    preferences->buttons[17] = 2;
    preferences->buttons[18] = 10;
    preferences->buttons[19] = 11;
    preferences->buttons[20] = 3;
    preferences->buttons[21] = 2;
    preferences->buttons[22] = 4;
    preferences->buttons[23] = 5;
    preferences->buttons[24] = 15;
    preferences->buttons[25] = 14;
    preferences->buttons[26] = 2;
    preferences->buttons[27] = 10;
    for (long action_index = 0; action_index < k_input_action_count; action_index++)
    {
    }
    preferences->joystick_preset = 0;
    preferences->look_inverted = false;
    preferences->flight_stick = false;
}

c_abstract_button::c_abstract_button()
{
    m_alternate_button_index = -1;
    m_button_index = -1;
}

s_input_abstraction_globals input_abstraction_globals;
