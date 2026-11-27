#ifndef INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_H
#define INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_H

// Functions defined in tu/interfacelibxbox/gui_screen_campaign_select_difficulty.cpp that other units call.

extern "C" void gui_screen_campaign_select_difficulty_check_blocked(bool *blocked);


#include "interface/c_gui_screen_widget.h"


class c_gui_screen_campaign_select_difficulty : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_campaign_select_difficulty(long name) :
        c_gui_screen_widget(name), m_mode(0), m_campaign(-1), m_map(-1) {}
    virtual void initialize();
    virtual void initialize_datasource();
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual void update(unsigned long current_milliseconds);
private:
    void difficulty_chosen_immediate(e_controller_index controller_index);
public:

    unsigned long m_mode;      // use: 0 new campaign, 1 chosen level, 2 pregame lobby (difficulty_chosen_immediate)
    long m_campaign;
    long m_map;
    long m_initial_difficulty;
};

typedef char campaign_select_difficulty_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_campaign_select_difficulty) == 0x1a98))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_H
