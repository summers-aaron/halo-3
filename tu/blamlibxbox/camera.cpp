// blamlibxbox/camera.obj: from the records 0x12a658 0x498d20
// @flags /TP /O2 /J
#include "shell/shell.h"
#include "main/main.h"
#include "camera/camera.h"



#include "interface/user_interface_window_manager.h"
#include "interface/c_gui_screen_widget.h"


static __forceinline long camera_responding_gate_field_64(c_gui_screen_widget *screen)
{
    return *reinterpret_cast<long *>(reinterpret_cast<char *>(screen) + 64);
}


// @address 0x12a658 ?camera_responding_gate@@YAEW4e_controller_index@@@Z
unsigned char camera_responding_gate(e_controller_index controller_index)
{
    c_gui_screen_widget *responding_screen = g_window_manager.get_responding_screen(controller_index, 0);
    c_gui_screen_widget *named_screen = g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x10081);
    int named_screen_ok = (named_screen != 0 && named_screen->transitioning_out() == 0);
    unsigned char named_screen_done = (unsigned char)named_screen_ok;
    int responding_unlisted;
    if (responding_screen == 0) {
        responding_unlisted = 0;
    } else {
        long screen_type = camera_responding_gate_field_64(responding_screen);
        responding_unlisted = (screen_type != 0x100cb && screen_type != 0x100cc && screen_type != 0x100c2);
    }
    return (unsigned char)(((unsigned char)main_has_pending_state_changes()) | named_screen_done | responding_unlisted);
}

// @address 0x498d20 camera_input_inhibited
extern "C" void camera_input_inhibited(void)
{
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x10081);
    if (screen != 0) {
        if (screen->transitioning_out() == 0) {
            screen->transition_out_with_transition_type(1, _screen_transition_normal);
        }
    }
}
