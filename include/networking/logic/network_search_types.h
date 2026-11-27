#ifndef NETWORKING_LOGIC_NETWORK_SEARCH_TYPES_H
#define NETWORKING_LOGIC_NETWORK_SEARCH_TYPES_H

#include <stddef.h>
#include "game/player_configuration.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_types.h"
#include "game/player_identifier_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"

// Session status, shared by the search slots and UI readers.
// local/types/catalog/s_network_squad_status_data.h and the accepted search/browser accesses.
struct s_network_squad_status_player
{
    s_player_identifier identifier;
    s_player_configuration configuration;
};
struct s_network_squad_status_data_game_details {
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
  short player_alignment;
  s_network_squad_status_player players[16];
  long timer_type;
  long timer_seconds;
  unsigned long occupied_team_mask;
  long team_scores[8];
  long statistics_alignment;
  s_matchmaking_gather_party_properties gather_party_properties;
};

struct s_network_squad_status_data {
  short protocol;
  short flags;
  long type;
  long version;
  long build;
  long state;
  long header_extension;
  s_network_squad_status_data_game_details details;
};
struct s_available_session
{
    union { unsigned char active; bool valid; };
    unsigned long last_reply_time;
    union { bool joinable; bool replied; };
    char session_description[0x30 - 9];
    bool changed;
    // Alignment before the eight-aligned status payload.
    s_network_squad_status_data status;
};
struct s_available_squad_globals
{
    long active;
    long reference_count;
    long maximum;
    long flags;
    const void *allocator;
    long count;
    s_available_session *slots;
};
typedef char network_search_status_details_offset[(offsetof(s_network_squad_status_data, details) == 0x18) ? 1 : -1];
typedef char network_search_status_players_offset[(offsetof(s_network_squad_status_data_game_details, players) == 0x128) ? 1 : -1];
typedef char network_search_status_count_offset[(offsetof(s_network_squad_status_data, details.player_count) == 0x13c) ? 1 : -1];
typedef char network_search_slot_reply_time_offset[(offsetof(s_available_session, last_reply_time) == 4) ? 1 : -1];
typedef char network_search_slot_changed_offset[(offsetof(s_available_session, changed) == 0x30) ? 1 : -1];
typedef char network_search_slot_status_offset[(offsetof(s_available_session, status) == 0x38) ? 1 : -1];
typedef char network_search_slot_pending_offset[
    (offsetof(s_available_session, status) + offsetof(s_network_squad_status_data, flags) == 0x3a) ? 1 : -1];
typedef char network_search_slot_size[(sizeof(s_available_session) == 0x1400) ? 1 : -1];
typedef char network_search_globals_count_offset[(sizeof(void *) != 4 || offsetof(s_available_squad_globals, count) == 0x14) ? 1 : -1];

typedef char network_search_status_session_type_offset[(offsetof(s_network_squad_status_data, details.session_type) == 0x1c) ? 1 : -1];
typedef char network_search_status_join_mode_offset[(offsetof(s_network_squad_status_data, details.join_mode) == 0x20) ? 1 : -1];
typedef char network_search_player_user_id_offset[
    (offsetof(s_network_squad_status_player, configuration) + offsetof(s_player_configuration_from_client, xuid) == 0x48) ? 1 : -1];
typedef char network_search_status_size[(sizeof(s_network_squad_status_data) == 0x13c8) ? 1 : -1];
typedef char network_search_player_size[(sizeof(s_network_squad_status_player) == 0x118) ? 1 : -1];
typedef char network_search_gather_offset[(offsetof(s_network_squad_status_data, details.gather_party_properties) == 0x12f0) ? 1 : -1];

#endif
