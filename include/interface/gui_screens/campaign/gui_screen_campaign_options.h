#ifndef INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_OPTIONS_H
#define INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_OPTIONS_H

#include <stddef.h>
#include "interface/gui_screens/campaign/gui_screen_screenshot_viewer.h"

struct s_autosave_queue;

class c_gui_screen_campaign_options : public c_gui_screen_screenshot_viewer
{
public:
    __forceinline c_gui_screen_campaign_options(long name) :
        c_gui_screen_screenshot_viewer(name), m_selected_index(0), m_loaded_index(-1) {}

    virtual void initialize();
    virtual void dispose();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual wchar_t *get_selected_item_title(wchar_t *output, long capacity);
    virtual wchar_t *get_selected_item_description(wchar_t *output, long capacity);

    s_autosave_queue *m_queue;
    long m_selected_index;
    long m_loaded_index;
    long m_state;
};

typedef char campaign_options_queue_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_screen_campaign_options, m_queue) == 0x1a88) ? 1 : -1];
typedef char campaign_options_selected_index_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_screen_campaign_options, m_selected_index) == 0x1a8c) ? 1 : -1];
typedef char campaign_options_loaded_index_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_screen_campaign_options, m_loaded_index) == 0x1a90) ? 1 : -1];
typedef char campaign_options_state_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_screen_campaign_options, m_state) == 0x1a94) ? 1 : -1];
typedef char campaign_options_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_campaign_options) == 0x1a98) ? 1 : -1];

class c_gui_screen_campaign_settings : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_campaign_settings(long name) : c_gui_screen_widget(name) {}

    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
};

typedef char campaign_settings_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_campaign_settings) == 0x1a88) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_OPTIONS_H
