#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_MODEL_START_MENU_SETTINGS_APPEARANCE_MODEL_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_MODEL_START_MENU_SETTINGS_APPEARANCE_MODEL_H

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/user_interface_data.h"
#include "tag_files/string_ids.h"

class c_start_menu_settings_appearance_model : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_appearance_model(long name) : c_start_menu_pane_screen_widget(name)
    {
        set_automatic_button_key(_string_id_a_confirm_b_cancel);
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }

    virtual void initialize();
    virtual void initialize_datasource();
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void submenu_invoked(c_gui_list_widget *list_widget);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }

private:
    void add_model_customization_selections_datasource(long name, e_controller_index controller);
    void update_model_widget(bool force);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char start_menu_settings_appearance_model_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_settings_appearance_model) == 0x1b08))) ? 1 : -1];
#endif

// Functions defined in tu/interfacelibxbox/start_menu_settings_appearance_model.cpp.

class c_gui_screen_widget;
class c_player_profile_interface;

class c_settings_appearance_model_sidebar_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_settings_appearance_model_sidebar_items_datasource(c_gui_data *base_datasource,
        c_player_profile_interface *profile_interface, c_gui_screen_widget *owner_screen)
    {
        m_base_datasource = base_datasource;
        m_profile = profile_interface;
        m_screen = owner_screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    c_player_profile_interface *m_profile;
    c_gui_screen_widget *m_screen;
};

long get_current_model_customization_value(c_player_profile_interface *profile, long name);
bool get_model_customization_selection_name(c_gui_screen_widget *screen, long datasource_name, long selection, long column_name, long *value);
bool get_current_model_selection_area_and_item_index(c_gui_screen_widget *screen, long *area, long *item);
bool parse_xml_part_description(void *widget, wchar_t *buffer, long buffer_length);
bool parse_xml_part_count_current(void *widget, wchar_t *buffer, long buffer_length);
bool parse_xml_part_count_total(void *widget, wchar_t *buffer, long buffer_length);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_MODEL_START_MENU_SETTINGS_APPEARANCE_MODEL_H
