#ifndef INPUT_INPUT_XENON_H
#define INPUT_INPUT_XENON_H

// Functions defined in tu/blamlibxbox/input_xenon.cpp.

struct gamepad_state;

void input_update(void);
const gamepad_state *input_get_gamepad_state(short controller_index);
void input_clear_gamepads_rumble(void);
void input_update_gamepads_rumble(void);

#endif // INPUT_INPUT_XENON_H
