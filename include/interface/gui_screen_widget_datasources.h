#ifndef INTERFACE_GUI_SCREEN_WIDGET_DATASOURCES_H
#define INTERFACE_GUI_SCREEN_WIDGET_DATASOURCES_H

#include "interface/c_gui_screen_widget_externs.h"
#include "interface/gui_data.h"
#include "interface/user_interface_data.h"

class c_player_profile_interface;

// The display settings pane installs this proxy over its start menu items list:
// a proxy of that list that answers the current-value string ids from the controller's profile and the global
// preferences. Same shape as March's c_settings_controls_sidebar_items_datasource.
class c_alternate_controller_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_alternate_controller_datasource(c_gui_data *base_datasource, c_player_profile_interface *profile,
        c_gui_screen_widget *screen)
    {
        m_base_datasource = base_datasource;
        m_profile = profile;
        m_screen = screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);

    c_player_profile_interface *m_profile;
    c_gui_screen_widget *m_screen;
};

typedef char alternate_controller_datasource_size[(sizeof(void *) != 4 || sizeof(c_alternate_controller_datasource) == 0x14) ? 1 : -1];

#endif
