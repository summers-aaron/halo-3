#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_START_MENU_SETTINGS_APPEARANCE_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_START_MENU_SETTINGS_APPEARANCE_H

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "shell/shell_types.h"

class c_virtual_keyboard_task;
class c_controller_input_message;
class c_dialog_result_message;
class c_gui_list_item_widget;
class c_gui_data;

// The service-tag keyboard is a member here; March had it in c_start_menu_settings.
class c_start_menu_settings_appearance : public c_start_menu_pane_screen_widget
{
public:
    c_start_menu_settings_appearance(long name);
    virtual ~c_start_menu_settings_appearance() {}
    virtual void initialize();
    virtual void post_initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }

private:
    void set_service_tag(e_controller_index controller_index, const wchar_t *text);
    void cancel_service_tag_request();

    c_virtual_keyboard_task *m_virtual_keyboard_task;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char start_menu_settings_appearance_size[(sizeof(void *) != 4 || (sizeof(c_start_menu_settings_appearance) == 0x1b10)) ? 1 : -1];
#endif

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_START_MENU_SETTINGS_APPEARANCE_H
