#ifndef INTERFACE_PLAYER_SELECT_SCREEN_MESSAGE_H
#define INTERFACE_PLAYER_SELECT_SCREEN_MESSAGE_H
#include "interface/user_interface_messages.h"
#include "game/player_identifier_types.h"
#include "game/player_configuration.h"
class c_load_player_select_screen_message : public c_load_screen_message
{
public:
    c_load_player_select_screen_message(long screen_name, e_controller_index controller, e_window_index window, long layered_position,
        const s_player_identifier *target_player_identifier, const s_player_identifier *player_identifier, bool = false);
    // The caller already holds the player's configuration (c_gui_game_details builds it from the squad
    // details), so nothing is looked up and the service record stays hidden.
    __forceinline c_load_player_select_screen_message(long screen_name, e_controller_index controller, e_window_index window, long layered_position,
        const s_player_identifier *target_player_identifier, const s_player_identifier *player_identifier,
        const s_player_configuration *configuration)
        : c_load_screen_message(screen_name, controller, window, layered_position),
        m_target_player_identifier(*target_player_identifier),
        m_player_identifier(*player_identifier),
        m_configuration(*configuration),
        m_valid(true),
        m_show_service_record(false)
    {
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    s_player_identifier m_target_player_identifier;
    s_player_identifier m_player_identifier;
    s_player_configuration m_configuration;
    bool m_valid;
    bool m_show_service_record;
};
typedef char player_select_message_size[(sizeof(void *) != 4 || sizeof(c_load_player_select_screen_message) == 360) ? 1 : -1];
#endif
