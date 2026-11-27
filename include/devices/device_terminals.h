#ifndef DEVICES_DEVICE_TERMINALS_H
#define DEVICES_DEVICE_TERMINALS_H

#include "devices/device_datum.h"
#include "devices/device_definitions.h"
#include "interface/gui_screens/terminals/gui_screen_terminals.h"

enum e_terminal_state
{
    _terminal_state_inactive,
    _terminal_state_activating,
    _terminal_state_active,
    k_terminal_state_count
};

struct s_terminal_datum : device_datum
{
    union { short flags; c_flags_no_init<long, short, 4> screen_flags; };
    c_enum<e_terminal_state, short, 0, k_terminal_state_count> state;
    short state_ticks;
    short screen_state;
    short screen_state_ticks;
    long hidden_object_index;
    real_vector3d orientation;
    real_vector2d velocity;
    real_point2d position;
    float reading_transition;
    bool reading_mode;
    short reading_ticks;
    short reading_duration;
    float overlay_intensity;
    float hex_intensity_limit;
    bool seeking;
    bool seek_finished;
    short seek_ticks;
    short seek_duration;
    short seek_end_duration;
    real_vector2d seek_velocity;
    real_vector2d seek_start_velocity;
    bool seeking_zero;
    real_point2d seek_start_position;
    real_vector2d seek_acceleration;
    real_vector2d seek_jerk;
    bool final_screen_state;
};
struct s_terminal_definition : s_device_definition
{
    long pad190;
    string_id action_string;
    short terminal_number;
    short reserved19a;
    float exposure;
    s_tag_reference activation_sound;
    s_tag_reference scripted_sound;
    // Terminal UI reads the page and exit sound indices at +0x1cc/+0x1dc.
    s_tag_reference page_sound;
    s_tag_reference exit_sound;
    s_tag_reference reading_sound;
    s_terminal_difficulty_definition displays[4];
};
typedef char terminal_definition_number_offset[(offsetof(s_terminal_definition, terminal_number) == 0x198) ? 1 : -1];
typedef char terminal_definition_displays_offset[(offsetof(s_terminal_definition, displays) == 0x1f0) ? 1 : -1];
typedef char terminal_action_string_offset[(offsetof(s_terminal_definition, action_string) == 0x194) ? 1 : -1];
typedef char terminal_exposure_offset[(offsetof(s_terminal_definition, exposure) == 0x19c) ? 1 : -1];
typedef char terminal_activation_sound_offset[(offsetof(s_terminal_definition, activation_sound) + offsetof(s_tag_reference, index) == 0x1ac) ? 1 : -1];
typedef char terminal_screen_state_offset[(offsetof(s_terminal_datum, screen_state) == 0x22e) ? 1 : -1];
typedef char terminal_hidden_object_offset[(offsetof(s_terminal_datum, hidden_object_index) == 0x234) ? 1 : -1];
typedef char terminal_final_screen_offset[(offsetof(s_terminal_datum, final_screen_state) == 0x29c) ? 1 : -1];

// Functions defined in tu/objectslibxbox/device_terminals.cpp that other units call.

union real_point3d;

extern "C" bool terminal_can_activate(long terminal_index, const real_point3d *position);

extern "C" const float k_terminal_scroll_rates[];

// Values terminal_compute_function_value returns for the layer1_x and layer1_y functions.
extern "C" float g_terminal_layer1_x;
extern "C" float g_terminal_layer1_y;

bool terminal_update(long terminal_index);

#endif // DEVICES_DEVICE_TERMINALS_H
