#ifndef NETWORKING_ONLINE_ONLINE_SESSION_TYPES_H
#define NETWORKING_ONLINE_ONLINE_SESSION_TYPES_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"
#include "networking/transport/transport_security_types.h"

enum e_online_session_flags
{
    _online_session_flag_host = 0,
    _online_session_flag_presence,
    _online_session_flag_stats,
    _online_session_flag_matchmaking,
    _online_session_flag_arbitration,
    _online_session_flag_peer_network,
    _online_session_flag_join_via_presence_disabled,
    _online_session_flag_join_via_presence_friends_only,
    _online_session_flag_join_disabled_while_in_progress,
    _online_session_flag_join_via_presence_disabled_while_in_progress,
    k_online_session_flags_count
};

enum e_online_session_player_flags
{
    _online_session_player_valid = 0,
    _online_session_player_private_slot,
    _online_session_player_joined
};

// The online session is the 0x160-byte state copied between a managed
// session's desired, pending and actual states. The manager's 0x638-byte
// entries are s_online_managed_session (March 825c28e8), not s_online_session.
struct s_online_session_player
{
    c_flags_no_init<e_online_session_player_flags, unsigned short, 3> flags;
    unsigned __int64 identifier;
};

struct s_online_session
{
    long controller_index;
    // Capability and join-policy flags for this session.
    c_flags_no_init<e_online_session_flags, unsigned short, 9> flags;
    long public_slot_count;
    long private_slot_count;
    s_transport_session_description description;
    unsigned __int64 nonce;
    void *handle;
    s_online_session_player players[16];
};


#if defined(_M_PPCBE) || defined(_XBOX)
typedef char online_session_player_size[(sizeof(s_online_session_player) == 0x10) ? 1 : -1];
typedef char online_session_size[(sizeof(s_online_session) == 0x160) ? 1 : -1];
typedef char online_session_handle_offset[(offsetof(s_online_session, handle) == 0x58) ? 1 : -1];
#endif

#endif
