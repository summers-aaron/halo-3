#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_GAME_START_STATUS_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_GAME_START_STATUS_TYPES_H

#include <stddef.h>

// The player error mask and map load progress are separate halfwords.
struct s_network_session_parameter_game_start_status
{
    long status;
    long error;
    unsigned short player_error_mask;
    unsigned short map_load_progress;
};
typedef char network_session_parameter_game_start_status_size[(sizeof(s_network_session_parameter_game_start_status) == 12) ? 1 : -1];
typedef char network_session_parameter_game_start_status_mask_offset[(offsetof(s_network_session_parameter_game_start_status,
    player_error_mask) == 8) ? 1 : -1];
typedef char network_session_parameter_game_start_status_progress_offset[(offsetof(s_network_session_parameter_game_start_status,
    map_load_progress) == 10) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_GAME_START_STATUS_TYPES_H
