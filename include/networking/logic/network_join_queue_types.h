#ifndef NETWORKING_LOGIC_NETWORK_JOIN_QUEUE_TYPES_H
#define NETWORKING_LOGIC_NETWORK_JOIN_QUEUE_TYPES_H

#include "networking/logic/network_join_queue_entry_types.h"
#include "networking/session/network_session_remote_join_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include <stddef.h>
#include <string.h>

// Join state is shared with the observer and session.
enum e_network_join_queue_mode
{
  _network_join_closed_to_all_joins = 0,
  _network_join_open_to_join_squad,
  _network_join_queue_joins_to_group,
  _network_join_process_queued_group_joins,
  _network_join_queue_closed_while_in_matchmaking
};

#pragma pack(push, 4)
struct s_networking_join_data {
  bool join_disabled;
  long join_status;
  long join_result;
  unsigned long join_start_time;
  bool peer_join_started;
  s_network_session_remote_session_join_data remote_join_data;
  bool request_join_squad_to_group;
  bool request_join_squad_to_target_group;
  bool request_join_group_to_group;
  s_network_session_remote_session_join_data queued_group_join;
  long group_join_result;
  union {
    unsigned char join_payload[0x1d0 - 0xf4];
    struct {
      long group_join_state;
      s_matchmaking_search_party_properties search_properties;
    };
  };
  bool request_group_disband;
  long group_disband_type;
  e_network_join_queue_mode join_queue_mode;
  long join_queue_entry_count;
  long membership_peer_count;
  s_networking_join_queue_entry join_queue[32];
};
#pragma pack(pop)

typedef char network_join_queue_peer_size[(sizeof(s_networking_join_queue_peer) == 0x38) ? 1 : -1];
typedef char network_join_queue_entry_size[(sizeof(s_networking_join_queue_entry) == 0x8a0) ? 1 : -1];
typedef char network_join_queue_nonce_offset[(offsetof(s_networking_join_queue_entry, join_nonce) == 0x18) ? 1 : -1];
typedef char network_join_queue_mode_offset[(offsetof(s_networking_join_data, join_queue_mode) == 0x1d8) ? 1 : -1];
typedef char network_join_queue_count_offset[(offsetof(s_networking_join_data, join_queue_entry_count) == 0x1dc) ? 1 : -1];
typedef char network_join_queue_entries_offset[(offsetof(s_networking_join_data, join_queue) == 0x1e8) ? 1 : -1];
typedef char network_join_search_properties_offset[(offsetof(s_networking_join_data, search_properties) == 0xf8) ? 1 : -1];
typedef char network_join_data_size[(sizeof(s_networking_join_data) == 0x115e8) ? 1 : -1];

extern s_networking_join_data g_network_join_data;

// Remove an entry by replacing it with the last queued entry.
static __forceinline void network_join_remove_join_from_queue_internal(s_networking_join_queue_entry *entry)
{
  s_networking_join_data *join_data = &g_network_join_data;
  memcpy(entry, &join_data->join_queue[join_data->join_queue_entry_count - 1], sizeof(s_networking_join_queue_entry));
  join_data->join_queue_entry_count--;
}

#endif // NETWORKING_LOGIC_NETWORK_JOIN_QUEUE_TYPES_H
