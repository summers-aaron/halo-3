#ifndef INPUT_INPUT_ABSTRACTION_TYPES_H
#define INPUT_INPUT_ABSTRACTION_TYPES_H

#include "math/real_math_types.h"
#include "shell/shell_types.h"

// Types of input_abstraction_globals (tu/blamlibxbox/input_abstraction.cpp).

enum e_input_action
{
    _input_action_jump = 0,
    _input_action_switch_grenade,
    _input_action_switch_weapon,
    _input_action_action,
    _input_action_reload_right,
    _input_action_use,
    _input_action_reload_left,
    _input_action_pick_up_left,
    _input_action_right_trigger,
    _input_action_left_trigger,
    _input_action_melee,
    _input_action_throw_grenade,
    _input_action_unknown12,
    _input_action_crouch,
    _input_action_zoom,
    _input_action_unknown15,
    _input_action_unknown16,
    _input_action_unknown17,
    _input_action_unknown18,
    _input_action_unknown19,
    _input_action_unknown20,
    _input_action_unknown21,
    _input_action_unknown22,
    _input_action_unknown23,
    _input_action_unknown24,
    _input_action_unknown25,
    _input_action_unknown26,
    _input_action_skip_cinematic,
    k_input_action_count
};

// One abstract button of a controller's game input state.
class c_abstract_button
{
public:
    c_abstract_button();
    float down_amount();
    unsigned char down_frames() const;
    unsigned short down_msec();

    void unlock() { m_alternate_button_index = -1; }
    __forceinline void latch() { m_flags |= 1; }
    __forceinline bool latched() const { return (m_flags & 1) != 0; }
    __forceinline void set_latch_bit(bool latched)
    {
        if (latched)
            m_flags |= 1;
        else
            m_flags &= ~1;
    }
    __forceinline bool access_valid() const { return m_alternate_button_index == -1 || m_alternate_button_index == m_button_index; }
    signed char access_context() const { return m_alternate_button_index; }
    void set_access_context(signed char context) { m_alternate_button_index = context; }
    __forceinline void update(unsigned short milliseconds, unsigned short frames, unsigned char amount)
    {
        m_down_milliseconds = milliseconds;
        m_down_frames = (unsigned char)frames;
        m_down_amount = (float)amount / 255.0f;
        if (!m_down_frames && latched())
            set_latch_bit(false);
    }

    // inline readers: the controller input path inlines these at its hot sites
    unsigned char get_down_frames() const
    {
        unsigned char result = 0;
        if (!latched() && access_valid())
            result = m_down_frames;
        return result;
    }
    float get_down_amount()
    {
        float result = 0.0f;
        if (!latched() && access_valid())
            result = m_down_amount;
        return result;
    }
    unsigned short get_down_msec() const
    {
        unsigned short result = 0;
        if (!latched() && access_valid())
            result = m_down_milliseconds;
        return result;
    }
    void set_access(long action) { m_button_index = (signed char)action; }
    void lock()
    {
        if (access_valid() && m_alternate_button_index != -1)
        {
            m_alternate_button_index = -1;
            m_flags |= 1;
        }
    }
    void unlock_to_access()
    {
        if (access_valid())
            m_alternate_button_index = m_button_index;
    }
    void set_locked()
    {
        if (m_alternate_button_index != -1)
        {
            m_alternate_button_index = -1;
            m_flags |= 1;
        }
    }
    void set_unlocked() { m_alternate_button_index = m_button_index; }

    unsigned short m_down_milliseconds;
    unsigned char m_down_frames;
    unsigned char m_flags;
    float m_down_amount;
    signed char m_button_index;
    signed char m_alternate_button_index;
};

struct s_gamepad_input_preferences
{
    float horizontal_look_sensitivity;
    float vertical_look_sensitivity;
    unsigned char buttons[k_input_action_count];
    short joystick_preset;
    bool look_inverted;
    bool flight_stick;
};

// A controller's preferences and the theater camera settings stored beside them (flying_camera.cpp reads the
// camera fields: the control scheme of the free camera's toggle button, its speed and its boost scale).
struct s_controller_look_settings
{
    s_gamepad_input_preferences preferences;
    short camera_control_scheme;
    float camera_speed;
    float camera_boost_scale;
};

struct s_game_input_state
{
    c_abstract_button buttons[16];
    unsigned char abstract_buttons[k_input_action_count];
    point2d thumbsticks[2];
    real_vector2d throttle;
    real_vector2d look;
    float aircraft_pitch;
    float vehicle_pitch;
    bool raw_look;

    c_abstract_button &get_button(long action)
    {
        buttons[abstract_buttons[action]].set_access(action);
        return buttons[abstract_buttons[action]];
    }
};

struct s_input_abstraction_globals
{
    __forceinline s_input_abstraction_globals() {}
    s_controller_look_settings look_settings[k_number_of_controllers];
    s_game_input_state input_states[k_number_of_controllers];
    long last_update_milliseconds;
    bool controller_active[k_number_of_controllers];
    bool initialized;
};

typedef char input_abstraction_button_size[(sizeof(c_abstract_button) == 12) ? 1 : -1];
typedef char input_abstraction_preferences_size[(sizeof(s_gamepad_input_preferences) == 0x28) ? 1 : -1];
typedef char input_abstraction_look_settings_size[(sizeof(s_controller_look_settings) == 0x34) ? 1 : -1];
typedef char input_abstraction_input_state_size[(sizeof(s_game_input_state) == 0x100) ? 1 : -1];
typedef char input_abstraction_globals_size[(sizeof(long) != 4 || sizeof(s_input_abstraction_globals) == 0x4dc) ? 1 : -1];

#endif // INPUT_INPUT_ABSTRACTION_TYPES_H
