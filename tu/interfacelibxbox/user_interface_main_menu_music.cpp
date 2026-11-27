// interfacelibxbox/user_interface_main_menu_music.obj: from the records 0x4a0c28
// @flags /TP /O2 /Oc

#include "interface/user_interface.h"
#include "interface/user_interface_main_menu_music.h"
#include "render/render.h"
#include "interface/user_interface_shared_globals.h"
struct s_user_interface_main_menu_music_globals
{
    char header[64];
    int main_menu_music_delay;
};


// @address 0x4a0c28 user_interface_main_menu_music_get_delay
extern "C" int user_interface_main_menu_music_get_delay(void)
{
    int delay = 0;
    const s_user_interface_main_menu_music_globals *globals =
        ((const s_user_interface_main_menu_music_globals *)::user_interface_shared_tag_globals_try_and_get());
    if (globals)
        delay = globals->main_menu_music_delay;
    if (render_is_load_recently_complete())
        return 3000;
    return delay;
}
