#ifndef NETWORKING_SESSION_NETWORK_SESSION_H
#define NETWORKING_SESSION_NETWORK_SESSION_H

#include <stddef.h>

#include "networking/session/network_session_class.h"

#include "networking/session/network_session_types.h"

#include "shell/shell_types.h"

// A networking session with membership, parameters and local connection state.

#include "networking/session/network_session_membership.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_state_types.h"
#include "networking/transport/transport_address.h"

template<typename t_enum, typename t_storage, long k_count> class c_flags;

enum e_network_session_state;
enum e_network_session_mode;
enum e_network_observer_owner;
enum e_transport_platform;
enum e_network_session_boot_reason;
enum e_network_message_type;
enum e_network_election_refuse_reason;
enum e_peer_reestablish_state;
enum e_session_creation_flags;
enum e_bandwidth_configuration_host_preference;

class c_network_message_gateway;
class c_network_observer;
class c_network_session_manager;
class c_network_session_membership;
class c_network_session_parameters;
class c_network_channel;
struct s_online_context;
struct s_transport_secure_identifier;
struct s_transport_secure_key;
struct s_transport_secure_address;
struct s_machine_identifier;
struct s_player_identifier;
struct s_player_configuration;
struct s_player_configuration_from_client;
struct s_network_session_peer;
struct s_network_session_player;
struct s_network_session_peer_properties;
struct s_network_session_join_request;
struct s_network_session_join_request_payload;
struct s_network_session_find_host;
struct s_network_election_decision;
struct s_network_message_join_request;
struct s_network_message_peer_connect;
struct s_network_message_leave_request;
struct s_network_message_session_disband;
struct s_network_message_session_boot;
struct s_network_message_host_decline;
struct s_network_message_peer_establish;
struct s_network_message_time_synchronize;
struct s_network_message_membership_update;
struct s_network_message_peer_properties;
struct s_network_message_delegate_leadership;
struct s_network_message_boot_machine;
struct s_network_message_player_add;
struct s_network_message_player_refuse;
struct s_network_message_player_remove;
struct s_network_message_player_properties;
struct s_network_message_parameters_update;
struct s_network_message_parameters_request;
struct s_network_message_host_handoff;
struct s_network_message_peer_handoff;
struct s_network_message_host_transition;
struct s_network_message_host_reestablish;
struct s_network_message_peer_reestablish;
struct s_network_message_election;
struct s_network_message_election_refuse;

#include "networking/session/network_session_join_request_types.h"

#include "networking/session/network_session_local_state_types.h"

// Receives channel heartbeat, reconnection and connection-state notifications.
class c_network_channel_owner
{
public:
    virtual bool desire_channel_heartbeat(long observer_channel_index) const = 0;
    virtual long channel_is_load_bearing(long observer_channel_index) const = 0;
    virtual bool attempt_channel_reconnection(long observer_channel_index, bool far_side_requested) const = 0;
    virtual void notify_channel_connection(long observer_channel_index, unsigned long connection_identifier, bool connected) = 0;
    virtual void notify_channel_died(long observer_channel_index) = 0;
};

extern "C" void network_session_update_membership(c_network_session *session);
struct s_network_session_election_state;

// Pending and local player-add requests.
struct s_network_session_pending_player_request
{
    s_player_identifier player_identifier;
    long peer_index;
    long user_index;
    unsigned __int64 xuid;
    bool silver;
    bool guest;
    bool bot;
};
struct s_network_session_player_request
{
    s_player_identifier player_identifier;
    long refuse_reason;
    unsigned long time;
};

class c_network_session : public c_network_channel_owner
{
public:
    enum e_session_message_transmission_type
    {
        _session_message_transmission_type_direct = 0,
        _session_message_transmission_type_direct_only = 1,
        _session_message_transmission_type_all = 2,
    };

    c_network_session();
    ~c_network_session() {}

    // c_network_channel_owner
    virtual bool desire_channel_heartbeat(long observer_channel_index) const;
    virtual long channel_is_load_bearing(long observer_channel_index) const;
    virtual bool attempt_channel_reconnection(long observer_channel_index, bool far_side_requested) const;
    virtual void notify_channel_connection(long observer_channel_index, unsigned long connection_identifier, bool connected);
    virtual void notify_channel_died(long observer_channel_index);

    // network_session.cpp
    bool initialize_session(long session_index, e_network_session_type session_type, c_network_message_gateway *message_gateway,
        c_network_observer *observer, c_network_session_manager *session_manager);
    void destroy_session();
    bool create_host_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags, e_network_session_class session_class,
        e_transport_platform transport_platform,
        const c_network_session_membership *membership, const s_online_context *online_context, long managed_session_flags);
    bool join_remote_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags, e_network_session_class session_class,
        e_transport_platform transport_platform,
        const s_transport_secure_identifier *session_id, const s_transport_secure_key *key, const s_transport_secure_address *address,
        unsigned __int64 join_nonce,
        const c_network_session_membership *membership, const s_network_session_join_request_payload *payload,
        const s_online_context *online_context, long managed_session_flags, bool unknown);
    bool connect_peer_to_remote_session(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags,
        e_network_session_class session_class, e_transport_platform transport_platform,
        const s_transport_secure_identifier *session_id, const s_transport_secure_key *key, const s_transport_secure_address *address, unsigned __int64 nonce,
        const s_online_context *online_context, long unknown);
    bool ready_for_remote_peers_to_join() const;
    void set_default_session_parameters();
    void idle();
    void disconnect();
    const c_network_session_membership *get_session_membership() const { return &m_session_membership; }
    c_network_session_membership *get_session_membership_for_update() { return &m_session_membership; }
    __forceinline const c_network_session_membership *get_session_membership_unsafe() const
    {
        const c_network_session_membership *membership = 0;
        if (m_local_state != _network_session_state_none && m_session_membership.has_membership())
            membership = &m_session_membership;
        return membership;
    }
    long get_session_membership_update_number() const;
    long get_local_session_membership_update_number() const { return m_session_membership.local_membership_update_number(); }
    const c_network_session_parameters *get_session_parameters() const { return (const c_network_session_parameters *)m_session_parameters; }
    c_network_session_parameters *get_session_parameters() { return (c_network_session_parameters *)m_session_parameters; }
    long get_peer_observer_channel(long peer_index) const
    {
        long observer_channel_index = -1;
        if (peer_index != -1 && m_session_membership.is_peer_valid(peer_index))
            observer_channel_index = m_session_membership.get_observer_channel_index(peer_index);
        return observer_channel_index;
    }
    bool observing_channel(long observer_channel_index) const;
    bool compare_session_id(const s_transport_secure_identifier *session_id) const;
    bool get_session_id(s_transport_secure_identifier *session_id) const
    {
        return managed_session_get_id(m_managed_session_index, session_id);
    }
    bool get_secure_key(s_transport_secure_identifier *transport_secure_identifier, s_transport_secure_key *transport_secure_key,
        e_transport_platform *transport_platform) const;
    bool peer_request_properties_update(const s_transport_secure_address *transport_secure_address,
        const s_network_session_peer_properties *network_session_peer_properties);
    void peer_request_player_add(long user_index, const s_player_identifier *identifier, long player_sequence_number, e_controller_index controller_index,
        const s_player_configuration_from_client *configuration, unsigned long voice_settings);
    e_network_join_refuse_reason peer_request_player_add_status(long user_index, const s_player_identifier *identifier);
    void peer_complete_player_add(long user_index, const s_player_identifier *identifier);
    bool peer_request_player_desired_properties_update(long user_index, long peer_index, e_controller_index controller_index,
        const s_player_configuration_from_client *configuration, unsigned long voice_settings);
    bool peer_request_player_remove(long user_index);
    bool leader_request_delegate_leadership(const s_transport_secure_address *player);
    bool leader_request_boot_machine(const s_transport_secure_address *address, e_network_session_boot_reason boot_reason);
    bool host_boot_machine(long peer_index, e_network_session_boot_reason boot_reason);
    bool host_delegate_host(long peer_index, bool option1, bool option2);
    bool host_set_player_current_properties(long player_index, const s_player_configuration *configuration);
    bool host_assume_leadership();
    __forceinline bool membership_is_locked() const
    {
        return m_local_state == _network_session_state_host_handoff
            || m_local_state == _network_session_state_host_disband
            || m_local_state == _network_session_state_host_reestablish;
    }
    bool membership_is_stable() const;
    bool session_is_full(long peers, long players) const;
    bool join_allowed_by_privacy() const;
    void disband_and_reestablish_as_host(bool unknown);
    void join_accept(const s_network_session_join_request *join_request, const transport_address *address);
    bool join_abort(const transport_address *address, unsigned __int64 join_nonce);
    bool waiting_for_host_connection(const transport_address *address) const;
    void host_connection_refused(const transport_address *address, e_network_join_refuse_reason refuse_reason);
    bool join_abort_in_progress(const transport_address *address) const;
    void join_abort_successful(const transport_address *address);
    bool is_peer_joining_this_session() const;
    bool force_disconnect_peer(const s_transport_secure_address *address);
    bool handle_leave_internal(long peer_index);
    bool channel_is_authoritative(c_network_channel *channel);
    bool address_is_remote_peer(const transport_address *address) const;
    bool peer_is_member(const s_transport_secure_address *address) const;
    bool player_is_member(const s_player_identifier *identifier) const;
    bool all_players_are_present() const;
    long find_peer_by_machine_identifier(const s_machine_identifier *machine_identifier) const;
    bool has_managed_session_connection() const;
    bool waiting_for_initial_update() const;
    const char *get_state_string() const;
    const char *get_mode_string() const;
    const char *get_id_string() const;
    const char *get_peer_description(long peer_index) const;
    void describe_status(char *buffer, long buffer_size) const;
    static const char *get_type_string(e_network_session_type session_type);
    static const char *get_class_string(e_network_session_class session_class);
    e_network_session_mode session_mode() const;
    const s_network_session_peer *get_peer(long peer_index) const;
    s_network_session_peer *get_peer(long peer_index);
    const s_network_session_player *get_player(long player_index) const;
    s_network_session_player *get_player(long player_index);
    long get_player_space_available() const;
    unsigned long time_get() const;
    e_network_join_refuse_reason can_accept_player_join_request(const s_player_identifier *identifier, long peer_index, long user_index, unsigned __int64 xuid,
        bool silver, bool guest, bool bot, bool remote) const;
    e_network_join_refuse_reason can_accept_join_request(const s_network_session_join_request *join_request) const;
    void acknowledge_join_request(const transport_address *address, e_network_join_refuse_reason refuse_reason);
    void check_to_send_peer_join_abort();

    // network_session_election.cpp
    long election_generate_initial_decision(s_network_election_decision *decision) const;
    bool election_update_decision_connectivity(s_network_election_decision *decision, const s_transport_secure_address *voter,
        const s_transport_secure_address *candidate) const;
    bool election_update_decision_agreement(s_network_election_decision *decision, const s_transport_secure_address *voter, bool agree) const;

    // network_session_host.cpp
    s_network_session_find_host find_good_peer_to_be_host(e_bandwidth_configuration_host_preference host_preference,
        bool prefer_current_host, bool exclude_current_host,
        bool unused_flag, long maximum_peer_count, long excluded_peer_mask, long ignored_connectivity_mask, bool unused_flag2);
    bool has_multiple_potential_hosts();
    bool delay_host_migration();
    bool does_peer_have_gamestate(long peer_index);

    // network_session_message.cpp
    bool handle_join_request(const transport_address *address, const s_network_message_join_request *message);
    void handle_peer_connect(const transport_address *address, const s_network_message_peer_connect *message);
    bool handle_leave_request(const transport_address *address);
    void handle_leave_acknowledgement(const transport_address *address);
    bool handle_session_disband(const transport_address *address, const s_network_message_session_disband *message);
    bool handle_session_boot(const transport_address *address, const s_network_message_session_boot *message);
    bool handle_host_decline(c_network_channel *channel, const s_network_message_host_decline *message);
    bool handle_peer_establish(c_network_channel *channel, const s_network_message_peer_establish *message);
    bool handle_time_synchronize(const transport_address *address, const s_network_message_time_synchronize *message);
    bool handle_membership_update(const s_network_message_membership_update *message);
    bool handle_peer_properties(c_network_channel *channel, const s_network_message_peer_properties *message);
    bool handle_delegate_leadership(c_network_channel *channel, const s_network_message_delegate_leadership *message);
    bool handle_boot_machine(c_network_channel *channel, const s_network_message_boot_machine *message);
    bool handle_player_add(c_network_channel *channel, const s_network_message_player_add *message);
    bool handle_player_refuse(c_network_channel *channel, const s_network_message_player_refuse *message);
    bool handle_player_remove(c_network_channel *channel, const s_network_message_player_remove *message);
    bool handle_player_properties(c_network_channel *channel, const s_network_message_player_properties *message);
    bool handle_parameters_update(const s_network_message_parameters_update *message);
    bool handle_parameters_request(c_network_channel *channel, const s_network_message_parameters_request *message);
    bool handle_host_handoff(const s_network_message_host_handoff *message);
    bool handle_peer_handoff(c_network_channel *channel, const s_network_message_peer_handoff *message);
    bool handle_host_transition(const s_network_message_host_transition *message);
    bool handle_host_reestablish(const transport_address *address, const s_network_message_host_reestablish *message);
    bool handle_peer_reestablish(c_network_channel *channel, const s_network_message_peer_reestablish *message);
    bool handle_election(const transport_address *address, const s_network_message_election *message, e_network_election_refuse_reason *refuse_reason,
        bool *refuse, s_transport_secure_address *refuse_address);
    bool handle_election_refuse(const transport_address *address);

    // Session state accessors.
    long session_index() const { return m_session_index; }
    e_network_observer_owner observer_owner() const { return (e_network_observer_owner)m_session_index; }
    e_network_session_type session_type() const { return m_session_type; }
    void set_session_type(e_network_session_type session_type) { m_session_type = session_type; }
    e_network_session_class session_class() const { return m_session_class; }
    long managed_session_index() const { return m_managed_session_index; }
    long current_local_state() const { return m_local_state; }
    bool is_host() const;
    bool established() const
    {
        bool established = false;
        switch (current_local_state())
        {
        case _network_session_state_none:
        case _network_session_state_peer_creating:
        case _network_session_state_peer_joining:
        case _network_session_state_peer_join_abort:
        case _network_session_state_election:
            established = false;
            break;
        case _network_session_state_peer_established:
        case _network_session_state_peer_leaving:
        case _network_session_state_host_established:
        case _network_session_state_host_disband:
        case _network_session_state_host_handoff:
        case _network_session_state_host_reestablish:
            established = true;
            break;
        default:
            // match: 0x123978, 0x3dbd08: unreachable state; preserve declaration order without cseries.h.
            __assume(0);
        }
        return established;
    }
    bool leaving_session() const;
    bool is_leader() const { return m_session_membership.is_leader(); }
    bool delegating_host() const
    {
        return m_local_state == _network_session_state_host_handoff && !m_local_state_data.host_handoff.host_leaving;
    }
    bool disconnected() const { return current_local_state() == _network_session_state_none; }     
    bool host_established() const { return current_local_state() == _network_session_state_host_established; }
    bool peer_joining() const { return current_local_state() == _network_session_state_peer_joining; }     
    void leave_session() { m_disconnect_reestablish_state = 1; initiate_leave_protocol(false); }
    c_network_observer *get_observer() { return m_observer; }
    const c_network_observer *get_observer() const { return m_observer; }
    long get_connection_identifier() const { return m_connection_identifier; }
    bool time_exists() const { return m_time_exists; }
    void time_release() { m_time_exists = false; }
    e_network_join_refuse_reason join_refuse_reason() const { return m_join_refuse_reason; }
    void clear_join_refuse_reason() { m_join_refuse_reason = _network_join_refuse_reason_none; }
    unsigned __int64 host_join_nonce() const { return m_host_join_nonce; }
    bool get_close_session() { return m_close_session; }
    void set_close_session(bool close_session) { m_close_session = close_session; }
    void set_close_session_for_matchmaking(bool close_session) { m_close_session_for_matchmaking = close_session; }
    void set_disallow_local_joins(bool disallow_local_joins) { m_disallow_local_joins = disallow_local_joins; }
    void set_disable_host_election(bool disable_host_election) { m_disable_host_election = disable_host_election; }

private:
    friend class c_life_cycle_state_handler_pre_game;
    friend class c_life_cycle_state_handler_leaving;
    friend class c_network_message_handler;
    // network_session.cpp
    bool create_session_for_remote_join(c_flags<e_session_creation_flags, unsigned char, 4> creation_flags,
        e_network_session_class session_class, e_transport_platform transport_platform,
        const s_transport_secure_identifier *session_id, const s_transport_secure_key *key, const s_transport_secure_address *address,
        bool unknown, unsigned __int64 nonce,
        s_network_session_join_request *join_request, const s_online_context *online_context, long managed_session_flags);
    friend void network_session_refuse_pending_player(c_network_session *session, long reason);
    friend void network_session_update_membership(c_network_session *session);
    void update_membership();
    void process_pending_joins();
    void add_pending_join_to_session(unsigned __int64 join_nonce);
    void abort_pending_join(unsigned __int64 join_nonce);
    void abort_pending_joins_with_timeout(long timeout);
    void initiate_leave_protocol(bool force);
    void boot_peer(long peer_index, e_network_session_boot_reason boot_reason);
    void disband_peer(long peer_index);
    void reset_local_host_state(bool becoming_host);
    void handoff_remove_peer_from_consideration(long peer_index);
    void reestablish_remove_peer_from_reestablishment(long peer_index);
    e_peer_reestablish_state check_to_send_peer_reestablish(e_peer_reestablish_state peer_reestablish_state);
    void check_to_send_peer_join_request();
    void check_to_send_peer_leave_request();
    void check_to_send_host_reestablish();
    void check_to_send_membership_update();
    void check_to_send_time_synchronization();
    void handle_disconnection();
    __forceinline void reset_connection_state()
    {
        ++m_connection_identifier;
        m_session_membership.flush_all_local_state();
        m_time_synchronization_end_time = -1;
        m_time_synchronization_start_time = -1;
    }
    __forceinline void time_set(long time) { m_time_exists = true; m_time = time; }
    void send_message_to_peer(e_session_message_transmission_type transmission_type, long peer_index, e_network_message_type message_type,
        long message_size, const void *message) const;
    void send_message_to_all_peers(e_session_message_transmission_type transmission_type, e_network_message_type message_type,
        long message_size, const void *message) const;

    // network_session_state.cpp
    void change_local_state_internal(e_network_session_state new_state, long state_data_size, const void *state_data);
    void change_local_state_peer_creating(e_transport_platform transport_platform, bool unknown, unsigned __int64 join_nonce,
        const s_network_session_join_request *join_request);
    void change_local_state_peer_joining(long channel, const s_transport_secure_address *address, const transport_address *usable_address, bool use_join_nonce,
        unsigned __int64 join_nonce, const s_network_session_join_request *request);
    void change_local_state_peer_join_abort();
    void change_local_state_peer_established();
    void change_local_state_peer_leaving();
    void change_local_state_host_established();
    void change_local_state_host_disband();
    void change_local_state_host_handoff(long peer_index, bool leaving, bool option1, bool option2);
    void change_local_state_host_reestablish(bool use_election, long reestablishment_mode);
    __forceinline void network_session_state_generate_election_data(const c_network_session_membership *, s_network_session_election_state *) const;
    void change_local_state_election();
    void idle_peer_creating();
    void idle_peer_joining();
    void idle_peer_join_abort();
    void idle_peer_established();
    void idle_peer_leaving();
    void idle_host_disband();
    void idle_host_handoff();
    void idle_host_reestablish();
    void idle_election();
    void idle_observer_state();

public:
    c_network_message_gateway *m_message_gateway;
    c_network_observer *m_observer;
    c_network_session_manager *m_session_manager;
    long m_session_index;
    e_network_session_type m_session_type;
    e_network_session_class m_session_class;
    c_network_session_membership m_session_membership;
#ifdef HALO3_64BIT
    __declspec(align(8)) char m_session_parameters[0x63800]; // c_network_session_parameters grows on 64-bit (vtables + pointer table)
#else
    char m_session_parameters[0x62af0];                    // c_network_session_parameters (raw until typed)
#endif
    e_network_session_state m_local_state;
    s_network_session_local_state_data m_local_state_data;
    long m_connection_identifier;
    unsigned long m_time_synchronization_end_time;         // network time a time synchronization answer came in
    unsigned long m_time_synchronization_start_time;       // network time the last request went out
    bool m_time_exists;
    long m_time;
    unsigned long : 32;                           // never initialized, read, or written: no accessor or assert in the March 2007 build touches these bytes
    long m_managed_session_index;
    e_network_join_refuse_reason m_join_refuse_reason;
    unsigned __int64 m_host_join_nonce;
    bool m_close_session;
    bool m_close_session_for_matchmaking;
    bool m_disallow_local_joins;
    bool m_disable_host_election;
    unsigned long m_last_host_migration_time;              // network time this session last changed host
    unsigned long m_host_migration_deferred_time;          // network time host migration was last put off (joins, inactive views, no better host)
    long m_host_migration_attempted_peer_mask;            // peers already tried as host since the last host change
    long m_disconnect_reestablish_state;                                  // 0 none, 1 leave_session, 2 set once all peers are established
    s_network_session_pending_player_request m_player_we_are_adding;
    s_network_session_player_request m_local_user_player_add[4];
};

typedef char network_session_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_session) == 0x68378) )) ? 1 : -1];
typedef char network_session_membership_offset_check[(sizeof(void *) != 4 || offsetof(c_network_session, m_session_membership) == 0x20) ? 1 : -1];
typedef char network_session_parameters_offset_check[(sizeof(void *) != 4 || offsetof(c_network_session, m_session_parameters) == 0x4f08) ? 1 : -1];

// Membership queries its owning session for the session type.
inline e_network_session_type c_network_session_membership::session_type() const { return m_session->session_type(); }

typedef char session_state_data_layout[
    (sizeof(void *) != 4 || offsetof(c_network_session, m_local_state_data) == 0x67a00) ? 1 : -1];
typedef char session_connection_layout[
    (sizeof(void *) != 4 || offsetof(c_network_session, m_connection_identifier) == 0x682d8) ? 1 : -1];
typedef char session_pending_player_layout[
    (sizeof(void *) != 4 || offsetof(c_network_session, m_player_we_are_adding) == 0x68318) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_H
