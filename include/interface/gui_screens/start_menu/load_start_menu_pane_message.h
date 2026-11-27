#ifndef INTERFACE_LOAD_START_MENU_PANE_MESSAGE_H
#define INTERFACE_LOAD_START_MENU_PANE_MESSAGE_H

#include "interface/user_interface_messages.h"
#include "networking/online/online_service_record.h"

class c_load_start_menu_pane_message : public c_load_screen_message {
public:
    __forceinline c_load_start_menu_pane_message(long name, e_controller_index controller, e_window_index window,
        long owner, const s_player_identifier *player, const s_player_identifier *target,
        const unsigned __int64 *xuid, const s_service_record_identity *identity, float translation, float rotation)
        : c_load_screen_message(name, controller, window, owner)
    {
        m_player_id = *player;
        m_target_player_id = *target;
        m_target_player_xuid = *xuid;
        m_service_record_identity = *identity;
        m_translation = translation;
        m_rotation = rotation;
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
    s_player_identifier m_player_id;
    s_player_identifier m_target_player_id;
    unsigned __int64 m_target_player_xuid;
    s_service_record_identity m_service_record_identity;
    float m_translation;
    float m_rotation;
};

#endif
