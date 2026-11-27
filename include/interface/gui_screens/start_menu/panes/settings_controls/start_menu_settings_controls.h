#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_START_MENU_SETTINGS_CONTROLS_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_START_MENU_SETTINGS_CONTROLS_H

#include "interface/user_interface_data.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "tag_files/string_ids.h"

// Functions defined in tu/interfacelibxbox/start_menu_settings_controls.cpp.

class c_player_profile_interface;

class c_gui_screen_widget;

// March start_menu_settings_controls.obj: a proxy of the screen's 0x1010f list that answers the current-value
// string ids from the controller's profile.
class c_settings_controls_sidebar_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_settings_controls_sidebar_items_datasource(c_gui_data *base_datasource,
        c_player_profile_interface *profile_interface, c_gui_screen_widget *owner_screen)
    {
        m_base_datasource = base_datasource;
        profile = profile_interface;
        screen = owner_screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element, long value_name, long *value);

    c_player_profile_interface *profile;
    c_gui_screen_widget *screen;
};

extern "C" bool parse_xml_profile_name(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_crouch_lock_description(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_appearance_description(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_flags_description(void *context, wchar_t *output, long capacity);

// The voice and film auto-save settings panes proxy the same 0x1010f list; each answers its own current values.
class c_settings_voice_sidebar_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_settings_voice_sidebar_items_datasource(c_gui_data *base_datasource,
        c_player_profile_interface *profile_interface, c_gui_screen_widget *owner_screen)
    {
        m_base_datasource = base_datasource;
        profile = profile_interface;
        screen = owner_screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element, long value_name, long *value);

    c_player_profile_interface *profile;
    c_gui_screen_widget *screen;
};

class c_settings_film_autosave_sidebar_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_settings_film_autosave_sidebar_items_datasource(c_gui_data *base_datasource,
        c_player_profile_interface *profile_interface, c_gui_screen_widget *owner_screen)
    {
        m_base_datasource = base_datasource;
        profile = profile_interface;
        screen = owner_screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element, long value_name, long *value);

    c_player_profile_interface *profile;
    c_gui_screen_widget *screen;
};

// The camera settings pane proxies the same 0x1010f list; it answers its own current values.
class c_settings_camera_sidebar_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_settings_camera_sidebar_items_datasource(c_gui_data *base_datasource,
        c_player_profile_interface *profile_interface, c_gui_screen_widget *owner_screen)
    {
        m_base_datasource = base_datasource;
        profile = profile_interface;
        screen = owner_screen;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long element, long value_name, long *value);

    c_player_profile_interface *profile;
    c_gui_screen_widget *screen;
};
#include "interface/player_profile_global_settings.h"

class c_controller_input_message;
class c_gui_list_widget;

// The controls settings pane of the start menu.
class c_start_menu_settings_controls : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_controls(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = _string_id_a_confirm_b_cancel;
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }
    virtual ~c_start_menu_settings_controls() {}
    virtual void initialize();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void initialize_datasource();
    virtual void submenu_invoked(c_gui_list_widget *list_widget);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return false; }
};

class c_start_menu_settings_voice : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_voice(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = _string_id_a_confirm_b_cancel;
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void submenu_invoked(c_gui_list_widget *list_widget);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }
};

class c_start_menu_settings_film_autosave : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_film_autosave(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = _string_id_a_confirm_b_cancel;
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }
    virtual void initialize();
    virtual void initialize_datasource();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void submenu_invoked(c_gui_list_widget *list_widget);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }
};

typedef char settings_voice_pane_size[(sizeof(c_start_menu_settings_voice) == sizeof(c_start_menu_pane_screen_widget)) ? 1 : -1];
typedef char settings_film_autosave_pane_size[
    (sizeof(c_start_menu_settings_film_autosave) == sizeof(c_start_menu_pane_screen_widget)) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_START_MENU_SETTINGS_CONTROLS_H
