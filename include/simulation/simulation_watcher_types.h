#ifndef SIMULATION_SIMULATION_WATCHER_TYPES_H
#define SIMULATION_SIMULATION_WATCHER_TYPES_H

#include "cseries/cseries.h"
#include <string.h>
#include "simulation/simulation_players.h"
#include "simulation/watcher_machine_collection_types.h"
#include "networking/session/network_session.h"

class c_simulation_world;
class c_network_observer;
class c_network_session;
struct s_network_session_peer;
struct s_network_session_player;
struct s_transport_secure_address;

struct simulation_machine_update;
static __forceinline void simulation_world_build_machine_update(bool *, simulation_machine_update *);

class c_simulation_watcher : public c_network_channel_owner {
  friend void simulation_world_build_machine_update(bool *, simulation_machine_update *);
public:
    c_simulation_watcher()
    {
        m_world = 0;
        m_session = 0;
        m_observer = 0;
    }
    ~c_simulation_watcher()
    {
    }

    // c_network_channel_owner
    virtual bool desire_channel_heartbeat(long observer_channel_index) const;
    virtual long channel_is_load_bearing(long observer_channel_index) const;
    virtual bool attempt_channel_reconnection(long observer_channel_index, bool far_side_requested) const;
    virtual void notify_channel_connection(long observer_channel_index, unsigned long connection_identifier, bool connected);
    virtual void notify_channel_died(long observer_channel_index);

    __forceinline void initialize_watcher(c_simulation_world *world)
    {
        m_world = world;
        m_session = 0;
        m_observer = 0;
        m_initial_player_update = false;
        m_local_session_membership_update_number = -1;
        m_session_membership_update_number = -1;
        m_last_player_update = -1;
        m_machine_valid_mask = 0;
        m_local_machine_index = -1;
        memset(m_machine_identifiers, 0, sizeof(m_machine_identifiers));
        m_machines_from_gamestate = false;
        simulation_player_collection_clear(&m_players);
        m_player_machines.valid_mask = 0;
        memset(m_player_machines.identifiers, 0, sizeof(m_player_machines.identifiers));
        m_connection_setup = true;
    }
  void setup_connection();
  void handle_new_gamestate(unsigned long flags);
  void handle_player_creation();
  void handle_player_leave_game(long player_index);
  c_network_session *get_session() const { return m_session; }
  c_simulation_world *m_world;
  c_network_observer *m_observer;
  c_network_session *m_session;
  long m_local_session_membership_update_number;
  long m_session_membership_update_number;
  long m_last_player_update;
  unsigned long m_machine_valid_mask;
  long m_local_machine_index;
  s_machine_identifier m_machine_identifiers[16];
  bool m_machines_from_gamestate;
  s_player_collection m_players;
  s_watcher_machine_collection m_player_machines;
  bool m_connection_setup;
  bool m_initial_player_update;
  void generate_player_updates(long *player_update_count, long maximum_player_update_count, simulation_player_update *player_updates);
  bool in_online_networked_session() const;
  long get_machine_index_by_identifier(const s_machine_identifier *identifier) const;
  bool get_player_is_in_game(long index, const s_player_identifier *identifier) const;
  bool get_pending_acknowledgement(
      bool flag, unsigned long *type, s_machine_identifier *machine, unsigned long *p4, unsigned long *p5, s_player_identifier *player);
  bool maintain_connection();
private:
  bool update_machines(const s_transport_secure_address *,long,long,long,c_static_flags<16>,const s_network_session_peer *);
  bool maintain_simulation_client(long machine_index);
  bool maintain_simulation_authority(c_network_session *session, long local_peer_index, c_static_flags<16> valid, const s_network_session_peer *peers);
  bool setup_view(long machine_index, const s_network_session_peer *session_peer, long observer_channel_index);
  void apply_machines_to_players();
  simulation_player_update *player_update_allocate(long *player_update_count, long maximum_player_update_count, simulation_player_update *player_updates) const;
  bool player_update_must_leave_game(long player_index, const s_player_entry *player, c_static_flags<16> peer_valid_flags,
      const s_machine_identifier *network_machine_identifiers, c_static_flags<16> network_player_valid_flags,
      const s_network_session_player *network_players) const;
public:
  bool need_to_generate_updates() const;
  bool get_machine_connectivity(const s_machine_identifier *machine_identifier) const;
private:
  static void player_update_find_swap_location(const s_player_collection *collection, long add_player_index,
      const s_player_identifier *add_player_identifier, long *swap_player_index, bool *remove_existing_player);
};

enum e_network_session_boot_reason;

extern "C" void simulation_watcher_handle_machine_boot(c_simulation_watcher *watcher, unsigned long mask, e_network_session_boot_reason reason);

#endif // SIMULATION_SIMULATION_WATCHER_TYPES_H
