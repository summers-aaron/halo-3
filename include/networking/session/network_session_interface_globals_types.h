#ifndef NETWORKING_SESSION_NETWORK_SESSION_INTERFACE_GLOBALS_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_INTERFACE_GLOBALS_TYPES_H

#include "networking/session/network_session_interface_types.h"
#include "saved_games/saved_film_description.h"

struct s_network_session;

struct s_nsi_film_description {
  long type;
  unsigned long : 32;
  unsigned long : 32;
  long valid;
  char name[0x200];
  long film_index;
  long trailer;
};

struct s_network_session_interface_cache {   // (&g_network_session_interface_globals) + 2444
  long update_time[3];
  long membership_update_number[3];
  bool teams_enabled[3];
  bool observer_allowed[3];
  long maximum_team_count[3];
};
struct s_network_session_interface_map_load {
  long scenario_type;
  char scenario_path[128];
  short map_index;
  short load_status;
  float initial_progress;
};
struct network_session_interface_properties_cache {
  long connection_identifiers[3];
  s_network_session_interface_cache properties;
};


struct network_session_interface_globals {
  s_network_session_interface_peer_properties peer;
  s_network_session_interface_user_record users[4];
  unsigned __int64 game_instance;
  s_network_session_interface_map_load map_load;
  union
  {
    s_nsi_film_description previous_film;
    s_saved_film_description saved_film;
  };
  unsigned long film_reset_time;
  network_session_interface_properties_cache cache;
  s_network_session **sessions;
  __forceinline network_session_interface_globals() {}
};
typedef char session_interface_saved_film_offset[(offsetof(network_session_interface_globals, saved_film) == 0x764) ? 1 : -1];
typedef char session_interface_film_time_offset[(offsetof(network_session_interface_globals, film_reset_time) == 0x97c) ? 1 : -1];
extern network_session_interface_globals g_network_session_interface_globals;

typedef char session_interface_globals_size[(sizeof(network_session_interface_globals) == 0x9c0) ? 1 : -1];
typedef char session_interface_hopper_offset[(offsetof(network_session_interface_globals, peer)
  + offsetof(s_network_session_interface_peer_properties, hopper_identifier) == 0x90) ? 1 : -1];
typedef char session_interface_hopper_error_offset[(offsetof(s_network_session_interface_peer_properties, hopper_error) == 0x94) ? 1 : -1];
typedef char session_interface_map_hopper_offset[(offsetof(s_network_session_interface_peer_properties, map_hopper_identifier) == 0xa8) ? 1 : -1];
typedef char session_interface_desired_team_offset[(offsetof(network_session_interface_globals, users)
  + offsetof(s_network_session_interface_user_record, desired_team_index) == 0x21c) ? 1 : -1];
typedef char session_interface_sessions_offset[(offsetof(network_session_interface_globals, sessions) == 0x9b8) ? 1 : -1];

#endif
