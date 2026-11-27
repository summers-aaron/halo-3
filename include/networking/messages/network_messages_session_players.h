#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PLAYERS_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PLAYERS_H

#include "game/player_configuration.h"
#include "game/player_identifier_types.h"
#include "networking/transport/transport_security_types.h"

// Player membership messages shared by the codecs and session senders.
struct s_network_message_boot_machine
{
    s_transport_secure_identifier session_id;
    long reason;
    s_transport_secure_address boot_peer_address;
};

struct s_network_message_player_refuse
{
    s_transport_secure_identifier session_id;
    s_player_identifier player_identifier;
    long user_index;
    long refuse_reason;
};

struct s_network_message_player_remove
{
    s_transport_secure_identifier session_id;
    long user_index;
};

struct s_network_message_player_add
{
    s_transport_secure_identifier session_id;
    long user_index;
    s_player_identifier player_identifier;
    unsigned long player_update_number;
    long controller_index;
    s_player_configuration_from_client configuration;
    unsigned long voice;
};

struct s_network_message_player_properties
{
    s_transport_secure_identifier session_id;
    long user_index;
    unsigned long update_number;
    long controller_index;
    s_player_configuration_from_client configuration;
    unsigned long voice;
};

#endif
