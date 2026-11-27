// port/probe/extra (port/GAME-CHANGES.md, "The Guide's menu sounds and hold-to-repeat"): the Guide's link to Halo 3's
// own menus, once per UI update (update_xenon_messages, on the game's thread, just after user_interface_controller_update
// has run the game's own menus' input). PC-only game code, compiled like a game unit; the console build has no such
// unit (on the console the system's Guide has sounds of its own).
//
//   - the menus' hold-to-repeat timing, the user interface shared globals' hold_delay, repeat_delay and
//     repeat_interval (event_manager_tab's), passed to the guide (guide_menu_timing) whenever they change: the guide's
//     lists repeat at the game's own pace (325, 500 and 100 ms in retail's tag);
//   - the sounds the guide queued since the last update (port/sdk/xam/guide.h: guide_sound_take), played as the
//     game's menus play theirs: user_interface_play_sound(effect, -1), the user interface sound effects tag's
//     references (no screen of the guide's overrides them), unspatialized at full volume.
#include <xtl.h>
#include "cseries/cseries.h"
#include "interface/user_interface.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_sound.h"
#include "interface/user_interface_sound_effect_types.h"
#include "xam/guide.h"

// The guide's sounds are the game's effects (guide.h's enum guide_sound, by value).
#define GUIDE_MENU_SAME(guide, game) ((int)(guide) == (int)(game))
typedef char guide_menu_sound_check[
    (GUIDE_MENU_SAME(GUIDE_SOUND_ERROR, _ui_sound_error) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_VERTICAL_NAVIGATION, _ui_sound_vertical_navigation) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_HORIZONTAL_NAVIGATION, _ui_sound_horizontal_navigation) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_A_BUTTON, _ui_sound_a_button) && GUIDE_MENU_SAME(GUIDE_SOUND_B_BUTTON, _ui_sound_b_button) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_START_BUTTON, _ui_sound_start_button) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_LEFT_BUMPER, _ui_sound_left_bumper) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_RIGHT_BUMPER, _ui_sound_right_bumper) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_LEFT_TRIGGER, _ui_sound_left_trigger) &&
     GUIDE_MENU_SAME(GUIDE_SOUND_RIGHT_TRIGGER, _ui_sound_right_trigger) &&
     // effect 18 (the header's _ui_sound_screen_in_progress) is what gui_screen_spartan_rank_dialog_initialize plays
     GUIDE_MENU_SAME(GUIDE_SOUND_RANK_UP, _ui_sound_screen_in_progress) && GUIDE_SOUND_PARTY_UP == 19 && GUIDE_SOUNDS > 0)
        ? 1
        : -1];
#undef GUIDE_MENU_SAME

extern "C" void guide_menu_update(void)
{
    const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
    if (globals == NULL)
        return; // no UI globals (between maps): nothing can play; a sound waiting goes stale (guide_sound_take)
    static long s_hold_delay = -1;
    static unsigned long s_repeat_delay, s_repeat_interval;
    if (globals->hold_delay != s_hold_delay || globals->repeat_delay != s_repeat_delay ||
        globals->repeat_interval != s_repeat_interval) {
        s_hold_delay = globals->hold_delay;
        s_repeat_delay = globals->repeat_delay;
        s_repeat_interval = globals->repeat_interval;
        guide_menu_timing(s_hold_delay > 0 ? (unsigned long)s_hold_delay : 0, s_repeat_delay, s_repeat_interval);
    }
    const unsigned long now = GetTickCount();
    for (int sound = guide_sound_take(now); sound >= 0; sound = guide_sound_take(now))
        ::user_interface_play_sound((e_user_interface_sound_effect)sound, -1);
}
