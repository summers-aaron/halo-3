#ifndef INTERFACE_USER_INTERFACE_MESSAGE_INLINES_H
#define INTERFACE_USER_INTERFACE_MESSAGE_INLINES_H

#include "interface/user_interface_messages.h"
#include "game/game_time_inline.h"

__forceinline c_message::c_message(e_ui_message_type type, long screen_name, e_controller_index controller, e_window_index window)
{
    m_game_time_at_creation = game_time_initialized() ? game_time_get() : 0;
    m_message_type = type;
    m_screen_name = screen_name;
    m_controller = controller;
    m_window = window;
}

#endif
