#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_UI_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_UI_H

// Functions defined in tu/networkinglibxbox/network_session_parameters_ui.cpp that other units call.

class c_bitstream;

bool ui_game_mode_decode_function(c_bitstream *bitstream, long update_message_size, void *update_message);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_UI_H
