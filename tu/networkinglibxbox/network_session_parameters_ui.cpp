// networkinglibxbox/network_session_parameters_ui.obj: from the records 0x3effa0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/session/network_session_parameters_ui.h"
#include "networking/session/network_session.h"



// @address 0x3effa0 ?ui_game_mode_decode_function@@YA_NPAVc_bitstream@@JPAX@Z
bool ui_game_mode_decode_function(c_bitstream *bitstream, long update_message_size, void *update_message)
{
    bitstream->read_raw_data("data", update_message, 64);
    bool success = !bitstream->error_occurred();
    return success;
}


