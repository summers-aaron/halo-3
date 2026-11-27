#ifndef INCLUDE_NETWORKING_MESSAGES_NETWORK_MESSAGE_HANDLER_TYPES_H
#define INCLUDE_NETWORKING_MESSAGES_NETWORK_MESSAGE_HANDLER_TYPES_H

#include "simulation/player_action.h"

struct s_network_message_synchronous_gamestate {
    unsigned char chunk_type;
    long chunk_offset;
    long chunk_size;
};
struct s_network_message_synchronous_actions {
    long action_number;
    long update_number;
    unsigned long action_mask;
    player_action actions[4];
    unsigned long checksum;
};
typedef char synchronous_actions_size[(sizeof(s_network_message_synchronous_actions) == 480) ? 1 : -1];
typedef char synchronous_actions_checksum_offset[(offsetof(s_network_message_synchronous_actions, checksum) == 476) ? 1 : -1];

struct s_network_message_distributed_game_results {
    long establishment_identifier;
    long update_number;
    char update_data[1];
};
struct s_network_message_synchronous_update_ack_message {
    unsigned long mode;
    long received_time;
    long update_number;
};
#endif
