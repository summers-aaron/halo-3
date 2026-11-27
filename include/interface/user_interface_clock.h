#ifndef INTERFACE_USER_INTERFACE_CLOCK_H
#define INTERFACE_USER_INTERFACE_CLOCK_H

#include "multithreading/synchronized_value.h"
#include "interface/user_interface_data.h"
#include "interface/user_interface_main_menu_music.h"
#include <stddef.h>

// Global user-interface state, including the datasource registry.
struct s_user_interface_globals {
    c_synchronized_long flags;
    c_synchronized_long active_controller;
    c_synchronized_long milliseconds;
    c_synchronized_long last_input;
    c_synchronized_long new_map_milliseconds; // UI clock when the current map was initialized
    long scoreboard_blocked_until_tick; // game time tick until which the scoreboard stays blocked; INT_MAX when inactive
    c_synchronized_long campaign_id; // campaign of the map being loaded; -1 when none
    c_synchronized_long map_id; // map being loaded; -1 when none
    c_synchronized_long campaign_difficulty; // difficulty of the selected campaign; -1 when none
    c_user_interface_main_menu_music main_menu_music;
    c_synchronized_long shell_last_input;
    c_gui_tag_datasource entries[32];
    int count;
    long exit_start_time;
    long player_count;
    bool error_posted;
    __forceinline s_user_interface_globals()
    {
    }
    __forceinline ~s_user_interface_globals() {}
};

typedef char ui_globals_music_offset[(offsetof(s_user_interface_globals, main_menu_music) == 0x24) ? 1 : -1];
typedef char ui_globals_entries_offset[(offsetof(s_user_interface_globals, entries) == 0x40) ? 1 : -1];
typedef char ui_globals_count_offset[(sizeof(void *) != 4 || offsetof(s_user_interface_globals, count) == 0x2240) ? 1 : -1];
typedef char ui_globals_size[(sizeof(void *) != 4 || sizeof(s_user_interface_globals) == 0x2250) ? 1 : -1];

extern s_user_interface_globals g_user_interface_globals;

#endif // INTERFACE_USER_INTERFACE_CLOCK_H
