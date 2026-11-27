#ifndef INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_H
#define INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_H

// Functions defined in tu/interfacelibxbox/gui_screen_campaign_select_level.cpp.

#include "interface/c_gui_screen_widget.h"

class c_gui_screen_campaign_select_level : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_campaign_select_level(long name) :
        c_gui_screen_widget(name), m_mode(0), m_campaign_id(-1)
    {
        m_map_id = -1;
    }
    virtual void post_initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item, c_gui_data *data);
    virtual void initialize_datasource();
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual void update(unsigned long);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    unsigned long m_mode;
    long m_campaign_id, m_map_id;
};

extern "C" void gui_screen_campaign_select_level_handle_selection(c_gui_screen_campaign_select_level *screen, long controller_index);

#endif // INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_LEVEL_H
