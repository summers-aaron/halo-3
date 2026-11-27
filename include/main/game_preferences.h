#ifndef MAIN_GAME_PREFERENCES_H
#define MAIN_GAME_PREFERENCES_H

#include "interface/gui_network_session_advertisement_types.h"

// Functions defined in tu/blamlibxbox/game_preferences.cpp.

long global_preferences_get_betrayal_count();
void global_preferences_flush(void);
bool global_preferences_get_last_game_setup(long mode);
void global_preferences_init(void);

class c_gui_pregame_setup_manager;
struct s_global_preferences;

extern "C" {
extern char g_global_preferences_storage[];
}

void global_preferences_update(void);
long global_preferences_get_ejection_count(void);
void global_preferences_set_betrayal_count(long value);
void global_preferences_set_ejection_count(long value);
extern "C" void global_preferences_set_mode_setup(int *p);
extern "C" void global_preferences_get_mode_setup(c_gui_pregame_setup_manager *manager, int type);
extern "C" int global_preferences_get_subtitle_setting(void);
extern "C" int global_preferences_get_brightness_setting(void);
extern "C" unsigned char shell_mount_utility_drive(unsigned char format);
extern "C" void shell_initialize_utility_drive(void);
extern "C" void global_preferences_set_font_language(long value);
extern "C" int global_preferences_get_font_language(void);
extern "C" void global_preferences_set_last_menu_selection(long value);
extern "C" void global_preferences_save(void);
void global_preferences_set_xbox_live_private_privacy_setting(e_gui_network_session_advertisement_mode privacy_setting);

#include "main/game_preferences_types.h"
#endif // MAIN_GAME_PREFERENCES_H
