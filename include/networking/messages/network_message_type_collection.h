#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_TYPE_COLLECTION_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_TYPE_COLLECTION_H

#include <stddef.h>

class c_bitstream;

// The network message types c_network_message_type_collection registers (March 2007:
// network_message_type_collection.obj, register_message_type(e_network_message_type, const char *name, ...)).
// Each enumerator is named after the name its registration passes; the values are the message type ids
// (network_messages_out_of_band/connect/session_protocol/session_membership units, network_globals.cpp for 34..37,
// 43 and 45, network_messages_simulation_synchronous.cpp for 38..42 and 44; k_network_message_type_count is the
// collection's 46 entries).
enum e_network_message_type
{
    _network_message_type_ping = 0,
    _network_message_type_pong,
    _network_message_type_broadcast_search,
    _network_message_type_broadcast_reply,
    _network_message_type_connect_request,
    _network_message_type_connect_refuse,
    _network_message_type_connect_establish,
    _network_message_type_connect_closed,
    _network_message_type_join_request,
    _network_message_type_peer_connect,
    _network_message_type_join_abort,                   // 10
    _network_message_type_join_refuse,
    _network_message_type_leave_session,
    _network_message_type_leave_acknowledge,
    _network_message_type_session_disband,
    _network_message_type_session_boot,
    _network_message_type_host_handoff,
    _network_message_type_peer_handoff,
    _network_message_type_host_transition,
    _network_message_type_host_reestablish,
    _network_message_type_host_decline,                 // 20
    _network_message_type_peer_reestablish,
    _network_message_type_peer_establish,
    _network_message_type_election,
    _network_message_type_election_refuse,
    _network_message_type_time_synchronize,
    _network_message_type_membership_update,
    _network_message_type_peer_properties,
    _network_message_type_delegate_leadership,
    _network_message_type_boot_machine,
    _network_message_type_player_add,                   // 30
    _network_message_type_player_refuse,
    _network_message_type_player_remove,
    _network_message_type_player_properties,
    _network_message_type_parameters_update,
    _network_message_type_parameters_request,
    _network_message_type_view_establishment,
    _network_message_type_player_acknowledge,
    _network_message_type_synchronous_update,
    _network_message_type_synchronous_playback_control,
    _network_message_type_synchronous_actions,          // 40
    _network_message_type_synchronous_acknowledge,
    _network_message_type_synchronous_gamestate,
    _network_message_type_distributed_game_results,     // "game-results" (March: network_messages_simulation_distributed.obj)
    _network_message_type_synchronous_client_ready,
    _network_message_type_test,

    k_network_message_type_count                        // 46
};

// One message type's definition: name, size limits and encode/decode/compare/dispose callbacks.
struct s_network_message_type_definition
{
    bool initialized;
    const char *name;
    long flags;
    long minimum_size;
    long maximum_size;
    void (*encode)(c_bitstream *, long, const void *);
    bool (*decode)(c_bitstream *, long, void *);
    bool (*compare)(long, void *, void *);
    void (*dispose)(long, void *);
};

class c_network_message_type_collection
{
public:
    void encode_message(c_bitstream *, e_network_message_type, long, const void *) const;
    bool decode_message(c_bitstream *, e_network_message_type *, long *, void *) const;
    bool decode_message_header(c_bitstream *, e_network_message_type *, long *) const;
    __forceinline void encode_message_header(c_bitstream *, e_network_message_type, long) const;
    inline void register_message_type(e_network_message_type, const char *, long, long, long,
        void (*)(c_bitstream *, long, const void *), bool (*)(c_bitstream *, long, void *),
        bool (*)(long, void *, void *), void (*)(long, void *));

    s_network_message_type_definition m_message_types[k_network_message_type_count];
};

typedef char network_message_type_definition_size_check[(sizeof(void *) != 4 || ((sizeof(s_network_message_type_definition) == 36) )) ? 1 : -1];
typedef char network_message_type_definition_name_offset[(sizeof(void *) != 4 || ((offsetof(s_network_message_type_definition, name) == 4) )) ? 1 : -1];
typedef char network_message_type_definition_encode_offset[(sizeof(void *) != 4 || ((offsetof(s_network_message_type_definition, encode) == 20) )) ? 1 : -1];
typedef char network_message_type_collection_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_message_type_collection) == 0x678) )) ? 1 : -1];

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_TYPE_COLLECTION_H
