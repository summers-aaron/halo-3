#ifndef INTERFACE_START_MENU_GAME_CAMPAIGN_H
#define INTERFACE_START_MENU_GAME_CAMPAIGN_H

#include "cseries/cseries.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_start_menu_game_campaign : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_game_campaign(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = NONE;
    }
    virtual ~c_start_menu_game_campaign() {} // slot 1
    virtual void initialize(); // slot 2
    virtual void post_initialize(); // slot 3
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition); // slot 17
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual void initialize_datasource(); // slot 35
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data); // slot 42
    virtual bool handle_dialog_result(const c_dialog_result_message *message); // slot 44

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return true; } // slot 50
};

#endif
