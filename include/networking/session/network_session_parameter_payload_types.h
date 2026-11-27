#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_PAYLOAD_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_PAYLOAD_TYPES_H

#include "game/player_identifier_types.h"
#include "networking/logic/network_game_privacy_types.h"
#include "saved_games/saved_film_description.h"
#include <stddef.h>

enum e_network_game_countdown_delayed_reason;

// Replicated payload layouts shared by the parameter owner and its consumers.
struct s_network_session_privacy_mode
{
    e_network_game_privacy privacy_mode;
    long closed_mode;
    long maximum_player_count;
    bool is_closed_by_user;
};
struct s_network_session_parameter_synchronous_out_of_sync
{
    bool report_out_of_sync;
    bool out_of_sync;
};
typedef char network_session_privacy_mode_size[(sizeof(s_network_session_privacy_mode) == 0x10) ? 1 : -1];
typedef char network_session_synchronous_out_of_sync_size[(sizeof(s_network_session_parameter_synchronous_out_of_sync) == 2) ? 1 : -1];
struct s_network_session_parameter_saved_film
{
    s_saved_film_description film;
};
typedef char network_session_parameter_saved_film_size[(sizeof(s_network_session_parameter_saved_film) == 536) ? 1 : -1];

struct s_network_ui_state { unsigned char value; };
struct s_network_session_parameter_countdown_timer
{
    long countdown;
    e_network_game_countdown_delayed_reason reason;
    long delayed_countdown;
    s_player_identifier culprit;
    bool running;
};
struct s_network_session_parameter_voice_repeater
{
    long peer_index;
};
struct s_network_session_parameter_matchmaking_messaging
{
    long message_type;
    long display_cookie;
    long message_data;
};
struct s_network_session_parameter_request_campaign_quit
{
    long quit_request_type;
    long quit_request_peer_index;
};
struct s_network_session_parameter_leader_properties
{
    char name[64];
};
typedef char network_session_parameter_request_campaign_quit_size[(sizeof(s_network_session_parameter_request_campaign_quit) == 8) ? 1 : -1];
typedef char network_session_parameter_voice_repeater_size[(sizeof(s_network_session_parameter_voice_repeater) == 4) ? 1 : -1];
typedef char network_session_parameter_matchmaking_messaging_size[(sizeof(s_network_session_parameter_matchmaking_messaging) == 12) ? 1 : -1];
typedef char network_session_parameter_matchmaking_message_data_offset[
    (offsetof(s_network_session_parameter_matchmaking_messaging, message_data) == 8) ? 1 : -1];
typedef char network_session_parameter_leader_properties_size[(sizeof(s_network_session_parameter_leader_properties) == 64) ? 1 : -1];
typedef char network_session_parameter_countdown_size[(sizeof(s_network_session_parameter_countdown_timer) == 24) ? 1 : -1];
typedef char network_session_parameter_countdown_culprit[(offsetof(s_network_session_parameter_countdown_timer, culprit) == 12) ? 1 : -1];
typedef char network_session_parameter_countdown_running[(offsetof(s_network_session_parameter_countdown_timer, running) == 20) ? 1 : -1];

#endif
