#ifndef INTERFACE_PLAYER_PROFILE_GLOBAL_SETTINGS_H
#define INTERFACE_PLAYER_PROFILE_GLOBAL_SETTINGS_H
class c_player_profile_interface;
#include <xtl.h>
struct start_menu_settings_result
{
    unsigned long count;
    XUSER_PROFILE_SETTING *settings;
};
extern "C" void player_profile_interface_apply_global_settings(c_player_profile_interface *profile,
    const start_menu_settings_result *settings, const unsigned long *setting_ids, int count);
extern "C" const unsigned long g_xenon_profile_setting_ids[14];
#endif
