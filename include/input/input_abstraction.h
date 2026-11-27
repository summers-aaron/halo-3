#ifndef INPUT_INPUT_ABSTRACTION_H
#define INPUT_INPUT_ABSTRACTION_H

// Functions defined in tu/blamlibxbox/input_abstraction.cpp that other units call.

struct s_gamepad_input_preferences;

void input_abstraction_update(void);
void input_abstraction_get_default_preferences(s_gamepad_input_preferences *preferences);

#if !defined(_XBOX) || defined(__clang__)
class c_abstract_button;

bool input_abstraction_input_frame_advanced();
unsigned char input_abstraction_button_down_input_frames(long controller_index, const c_abstract_button &button);
#endif

// Data defined in tu/blamlibxbox/input_abstraction.cpp that other units use.

struct s_input_abstraction_globals;

extern s_input_abstraction_globals input_abstraction_globals;

void input_abstraction_initialize(void);
void input_abstraction_dispose(void);

void input_abstraction_initialize_for_new_map(void);
#endif // INPUT_INPUT_ABSTRACTION_H
