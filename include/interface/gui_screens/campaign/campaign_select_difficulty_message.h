#ifndef INTERFACE_CAMPAIGN_SELECT_DIFFICULTY_MESSAGE_H
#define INTERFACE_CAMPAIGN_SELECT_DIFFICULTY_MESSAGE_H

#include "interface/user_interface_messages.h"

class c_load_campaign_select_difficulty_screen_message : public c_load_screen_message
{
public:
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    __forceinline c_load_campaign_select_difficulty_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        long setup_mode, long campaign_id, long map_id, long difficulty)
        : c_load_screen_message(0x100a4, controller, window, layered_position)
    {
        m_campaign_setup_mode = setup_mode;
        m_campaign_id = campaign_id;
        m_map_id = map_id;
        m_difficulty = difficulty;
    }

    __forceinline c_load_campaign_select_difficulty_screen_message(long screen_name, e_controller_index controller,
        e_window_index window, long layered_position, long setup_mode, long campaign_id, long map_id, long difficulty)
        : c_load_screen_message(screen_name, controller, window, layered_position)
        , m_campaign_setup_mode(setup_mode)
        , m_campaign_id(campaign_id)
        , m_map_id(map_id)
        , m_difficulty(difficulty)
    {
    }

    long m_campaign_setup_mode;
    long m_campaign_id;
    long m_map_id;
    long m_difficulty;
};

#endif
