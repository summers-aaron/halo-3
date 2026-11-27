// blamlibxbox/input.obj: from the records 0x2a6080
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "shell/shell.h"
#include "input/input.h"
#include "input/input_xenon.h"
#include "interface/controller_inline.h"
// @address 0x2a6080 input_has_button_pressed

bool input_has_button_pressed(int button)
{
    bool result = false;
    for (e_controller_index controller_index = first_controller();
         controller_index != _controller_none;
         controller_index = next_controller(controller_index))
    {
        const gamepad_state *gamepad = input_get_gamepad_state((short)controller_index);
        if (gamepad != 0 && gamepad->button_frames[button] != 0)
            result = true;
    }
    return result;
}
