#ifndef INTERFACE_GUI_LOCATION_MANAGER_H
#define INTERFACE_GUI_LOCATION_MANAGER_H

#include "interface/gui_game_mode_types.h"
class c_gui_screen_widget;

enum e_gui_location
{
    _gui_location_none = -1,
    _gui_location_main_menu = 0,
    _gui_location_pregame_lobby = 1,
    _gui_location_matchmaking_searching = 2,
    _gui_location_matchmaking_match_found = 3,
    _gui_location_in_game = 4,
    _gui_location_postgame_lobby = 5
};

class c_gui_location_manager {
public:
    void update();
    __forceinline bool get_show_postgame_stats_upon_lobby_entrance() const { return m_show_postgame_stats_upon_lobby_entrance; }
    __forceinline void set_show_postgame_stats_upon_lobby_entrance(bool show) { m_show_postgame_stats_upon_lobby_entrance = show; }
    static c_gui_screen_widget *get_location_screen();

    // Public so the static initializer can be spelled (layout and mangling unchanged).
    bool m_initialized;
    long m_squad_type;
    union
    {
        bool m_show_postgame_stats_upon_lobby_entrance;
        bool m_main_menu_launch_pending; // Existing gui_location_manager.cpp spelling.
    };
    unsigned long m_last_update_time;
    long m_change_location_timer;
private:
    e_gui_location get_current_ui_location();
    static long get_pregame_lobby_name(e_gui_game_mode game_mode);
    void change_location(long screen_name);
};

extern "C" c_gui_location_manager g_lobby_location_manager;

#endif
