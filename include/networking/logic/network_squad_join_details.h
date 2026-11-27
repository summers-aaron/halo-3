#ifndef NETWORKING_LOGIC_NETWORK_SQUAD_JOIN_DETAILS_H
#define NETWORKING_LOGIC_NETWORK_SQUAD_JOIN_DETAILS_H

#include "networking/transport/transport_security_types.h"
#include <stddef.h>

// Session-class anchored portion of s_network_squad_status_data used by UI joins.
struct s_network_squad_join_details
{
  long session_class;
  long session_type;
  long join_mode;
  wchar_t session_name[32];
  short sort_priority;
  s_transport_secure_identifier session_id;
  s_transport_secure_address secure_address;
  s_transport_secure_key session_key;
  unsigned short slot_count_a;
  short slot_count_b;
  short slot_count_c;
  short slot_count_d;
  short kind;
  unsigned short variant;
  short start_mode;
  short average_rating;
  char hopper_name[16];
  short hopper_alignment;
  long game_engine_type;
  wchar_t game_variant_name[16];
  long campaign_id;
  long campaign_difficulty;
  long map_id;
  wchar_t map_variant_name[16];
  wchar_t film_name[16];
  bool teams_enabled;
  char team_alignment;
  short maximum_team_count;
  short score_to_win_round;
  short best_player_score;
  short player_count;
};
typedef char network_squad_join_identifier_offset[(offsetof(s_network_squad_join_details, session_id) == 0x4e) ? 1 : -1];
typedef char network_squad_join_player_count_offset[(offsetof(s_network_squad_join_details, player_count) == 0x124) ? 1 : -1];

#endif
