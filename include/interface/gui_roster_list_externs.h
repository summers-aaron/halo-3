#ifndef INTERFACE_GUI_ROSTER_LIST_EXTERNS_H
#define INTERFACE_GUI_ROSTER_LIST_EXTERNS_H

#include "interface/gui_widget_hierarchy.h"

#include "interface/c_gui_list_widget.h"

struct s_roster_team
{
    bool active;
    long color;
    long change_start_time;
    unsigned long display_until;
};

class c_gui_roster_list_widget : public c_gui_list_widget
{
public:
    c_gui_roster_list_widget(bool game_roster_is_local_party, bool is_team_game);
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    __forceinline bool get_is_team_game() const { return m_team_game; }
    __forceinline void update_team_mode();
    bool m_game_roster_is_local_party;
    bool m_team_game;
    bool m_team_switching_enabled;
    bool m_show_party_indicators;
    s_roster_team m_teams[16];
};

extern "C" {
c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
c_gui_widget *gui_widget_get_next_list_item(c_gui_widget *widget, bool require_valid);
bool user_interface_networking_is_team_switching_allowed(void);
}

#endif // INTERFACE_GUI_ROSTER_LIST_EXTERNS_H
