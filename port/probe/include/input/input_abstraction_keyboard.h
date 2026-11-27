#ifndef INPUT_INPUT_ABSTRACTION_KEYBOARD_H
#define INPUT_INPUT_ABSTRACTION_KEYBOARD_H

// PC port (port/probe copies of tu/blamlibxbox/input_abstraction.cpp and player_control.cpp; GAME-CHANGES "Keyboard
// and mouse"): the keyboard and mouse as a second device of the input abstraction.
//
// On the console each controller has 16 buttons (s_game_input_state::buttons) and the profile's button preset maps
// Halo 3's 28 game actions onto them (abstract_buttons: several actions share a button, which the access contexts
// arbitrate); player_control reads an action through get_button. The keyboard instead has a button of its own per
// action (input_abstraction.cpp, controller 0's: the keyboard is controller 0's), pressed by its bindings
// (port/sdk/input/kbm_bindings.h) and never by the preset, and player_control reads each action as both: the
// controller's button through its preset, merged with the keyboard's (c_input_action). A controller and the keyboard
// so work at once, and no button layout (Default, Bumper Jumper, ...) changes what a key does. The keyboard's
// movement and mouse look are merged into the throttle and look the same way, past the joystick preset
// (input_abstraction_update).

#include "input/input_abstraction_types.h"
#include "../../../sdk/input/port_input.h"

// One game action as player_control reads it: what get_button gives (the controller's button the preset maps the
// action onto, with its access context and latch) merged with the keyboard's own button for the action (none for
// controllers 1..3). Reads take the larger of the two; the latch and access calls act on both, so each keeps the
// game's rules (a locked left trigger stays locked until let go, on either device).
class c_input_action
{
public:
    c_input_action(c_abstract_button &pad, c_abstract_button *keys) : m_pad(pad), m_keys(keys) {}

    unsigned char get_down_frames() const
    {
        return larger(m_pad.get_down_frames(), m_keys ? m_keys->get_down_frames() : 0);
    }
    unsigned char down_frames() const { return larger(m_pad.down_frames(), m_keys ? m_keys->down_frames() : 0); }
    unsigned short get_down_msec() const
    {
        const unsigned short pad = m_pad.get_down_msec(), keys = m_keys ? m_keys->get_down_msec() : 0;
        return pad > keys ? pad : keys;
    }
    float get_down_amount()
    {
        const float pad = m_pad.get_down_amount(), keys = m_keys ? m_keys->get_down_amount() : 0.0f;
        return pad > keys ? pad : keys;
    }
    bool access_valid() const { return m_pad.access_valid() || (m_keys && m_keys->access_valid()); }

    void latch()
    {
        m_pad.latch();
        if (m_keys)
            m_keys->latch();
    }
    void lock()
    {
        m_pad.lock();
        if (m_keys)
            m_keys->lock();
    }
    void unlock_to_access()
    {
        m_pad.unlock_to_access();
        if (m_keys)
            m_keys->unlock_to_access();
    }
    void set_locked()
    {
        m_pad.set_locked();
        if (m_keys)
            m_keys->set_locked();
    }
    void set_unlocked()
    {
        m_pad.set_unlocked();
        if (m_keys)
            m_keys->set_unlocked();
    }

private:
    static unsigned char larger(unsigned char a, unsigned char b) { return a > b ? a : b; }

    c_abstract_button &m_pad;
    c_abstract_button *m_keys;
};

// The game actions of controller `controller_index`'s input state, each read as c_input_action: player_control's
// `input_state->get_button(action)` becomes `actions->get_button(action)` (the console build keeps the pointer).
class c_input_actions
{
public:
    c_input_actions(long controller_index, s_game_input_state *input_state)
        : m_controller_index(controller_index), m_input_state(input_state) {}
    c_input_actions *operator->() { return this; }
    c_input_action get_button(long action) { return get_button(action, action); }
    // The controller's button for `action` with the keyboard's for `keyboard_action`: a dual wielder's triggers, which
    // the Southpaw preset swaps on the controller only (player_control.cpp, player_control_trigger): the keyboard's Fire
    // is always the right hand's and Fire Left the left's.
    c_input_action get_button(long action, long keyboard_action);

private:
    long m_controller_index;
    s_game_input_state *m_input_state;
};

// The input frames (input_abstraction_button_down_input_frames) the action has been down: the larger of the
// controller's button's and the keyboard's.
unsigned char input_abstraction_action_down_input_frames(long controller_index, long action);
// HALO3_KBM_TRACE=1: controller `controller_index`'s game actions as each device presses them, when they change.
void input_abstraction_trace_actions(long controller_index);

#endif // INPUT_INPUT_ABSTRACTION_KEYBOARD_H
