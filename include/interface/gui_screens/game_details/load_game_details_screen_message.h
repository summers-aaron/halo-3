#ifndef INTERFACE_LOAD_GAME_DETAILS_SCREEN_MESSAGE_H
#define INTERFACE_LOAD_GAME_DETAILS_SCREEN_MESSAGE_H
#include "interface/user_interface_messages.h"
#include <string.h>

struct s_network_squad_status_data;

class c_load_game_details_screen_message : public c_load_screen_message {
public:
    __forceinline c_load_game_details_screen_message(long screen_name, e_controller_index controller, e_window_index window,
        long layered_position, long initial_state, const s_network_squad_status_data *game_details)
        : c_load_screen_message(screen_name, controller, window, layered_position)
        , m_initial_state(initial_state)
    {
        memcpy(m_game_details, game_details, sizeof(m_game_details));
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    long m_initial_state;
    char m_game_details[0x13c8];
};
#endif
