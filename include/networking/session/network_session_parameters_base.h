#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_BASE_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_BASE_H

// c_network_session_parameter_base: one replicated session parameter (the base of every member of
// c_network_session_parameters). The header March 2007's asserts name for its methods
// (networking\session\network_session_parameters_base.cpp:37..474); the class itself is declared here.
//
// Shape: a polymorphic base class; each parameter's data follows the base, with an 8-aligned
// payload after it. Virtual slots in declaration order (the 16
// slots of every parameter vtable, 0x82046770 = session mode). Fields named from the tree's owner unit
// (network_session_parameters.cpp, whose register_parameters passes type, name, priority and access to
// initialize()).
//
// Methods: three methods are defined out of line in network_session_parameters_base.cpp;
// (set_allowed 0x3d8c30, change_request_allowed 0x3d8ca0, get_session_description 0x3d8d20, still FUN_ in
// network_session_parameters_base.cpp). Bodies here only for the accessors with no
// copy of; the rest are prototypes.

#include "memory/static_arrays.h"

class c_network_session;

enum e_network_session_parameter_type
{
    _network_session_parameter_type_session_size,
    _network_session_parameter_type_session_mode,
    _network_session_parameter_type_privacy_mode,
    _network_session_parameter_type_ui_game_mode,
    _network_session_parameter_type_start_mode,
    _network_session_parameter_type_ui_state,
    _network_session_parameter_type_end_game,
    _network_session_parameter_type_map,
    _network_session_parameter_type_initial_participants,
    _network_session_parameter_type_game_instance,
    _network_session_parameter_type_random_seed,
    _network_session_parameter_type_language,
    _network_session_parameter_type_determinism_version,
    _network_session_parameter_type_simulation_protocol,
    _network_session_parameter_type_campaign_difficulty,
    _network_session_parameter_type_campaign_active_primary_skulls,
    _network_session_parameter_type_campaign_active_secondary_skulls,
    _network_session_parameter_type_campaign_metagame_scoring,
    _network_session_parameter_type_campaign_insertion_point,
    _network_session_parameter_type_game_variant,
    _network_session_parameter_type_map_variant,
    _network_session_parameter_type_saved_film,
    _network_session_parameter_type_saved_film_game_options,
    _network_session_parameter_type_game_start_status,
    _network_session_parameter_type_countdown_timer,
    _network_session_parameter_type_voice_repeater,
    _network_session_parameter_type_requested_remote_join_data,
    _network_session_parameter_type_remote_join_data,
    _network_session_parameter_type_matchmaking_arbitration_nonce,
    _network_session_parameter_type_matchmaking_hopper_list,
    _network_session_parameter_type_matchmaking_game_list,
    _network_session_parameter_type_matchmaking_requested_hopper,
    _network_session_parameter_type_matchmaking_hopper,
    _network_session_parameter_type_matchmaking_abort_requested,
    _network_session_parameter_type_matchmaking_search_preference,
    _network_session_parameter_type_matchmaking_progress,
    _network_session_parameter_type_matchmaking_progress_search_criteria,
    _network_session_parameter_type_matchmaking_assembly_peer_evict_mask,
    _network_session_parameter_type_matchmaking_rematch_data,
    _network_session_parameter_type_matchmaking_hopper_statistics,
    _network_session_parameter_type_matchmaking_tip,
    _network_session_parameter_type_matchmaking_suppress_progress,
    _network_session_parameter_type_synchronous_out_of_sync,
    _network_session_parameter_type_matchmaking_messaging,
    _network_session_parameter_type_host_frame_quality,
    _network_session_parameter_type_request_campaign_quit,
    _network_session_parameter_type_leader_properties,

    k_network_session_parameter_type_count
};

enum e_parameter_update_priority
{
    _parameter_update_priority_default,
    _parameter_update_priority_high,
};

enum e_parameter_change_request_access
{
    _parameter_change_request_access_default,
    _parameter_change_request_access_peer,
    _parameter_change_request_access_leader,
};

struct s_network_session_parameter_update
{
    unsigned char *payload;
};

class c_network_session_parameter_saved_film_game_options;
// Existing C bridge used by the session interface.
extern "C" bool network_session_game_options_set(
    c_network_session_parameter_saved_film_game_options *parameter,
    long length_in_ticks, long start_ticks, const void *options);

template<typename t_type> class c_generic_network_session_parameter;
struct s_network_session_parameter_matchmaking_messaging;
// Sets the matchmaking message (network_session_parameters_session.cpp).
extern "C" bool network_session_request_parameter_set_value(
    c_generic_network_session_parameter<s_network_session_parameter_matchmaking_messaging> *parameter, long message_type);

class c_network_session_parameter_base
{
    friend bool network_session_game_options_set(
        c_network_session_parameter_saved_film_game_options *parameter,
        long length_in_ticks, long start_ticks, const void *options);
    friend bool network_session_request_parameter_set_value(
        c_generic_network_session_parameter<s_network_session_parameter_matchmaking_messaging> *parameter, long message_type);
    friend struct c_network_session_parameters;
public:
    void initialize(e_network_session_parameter_type type, const char *name, e_parameter_update_priority update_priority,
        e_parameter_change_request_access change_request_access, c_network_session *session);
    void clear();
    void reset_peer_state(long peer_index);
    __forceinline void transition_state_to_become_host()
    {
        m_peers_updated.set_all(false);
        m_flags &= 1;
        notify_transition_state_to_become_host();
    }

    bool get_allowed() const { return (m_flags & 1) != 0; }
    const char *get_set_denied_reason() const;
    bool set_allowed() const;
    bool change_request_allowed() const;
    const char *get_change_request_denied_reason() const;
    e_parameter_update_priority get_update_priority() const { return (e_parameter_update_priority)m_update_priority; }
    e_parameter_change_request_access get_change_request_access() const { return (e_parameter_change_request_access)m_change_request_access; }

    bool peer_requires_update(long peer_index) const;
    long get_update_size() const;
    void build_update(long peer_index, s_network_session_parameter_update *update, long update_size) const;
    bool handle_update(const s_network_session_parameter_update *update, long update_size);
    void notify_update_sent_to_peer(long peer_index);

    bool desires_change_request() const { return ((m_flags >> 1) & 1) != 0; }
    long get_change_request_size() const;
    void build_change_request(long peer_index, void *change_request, long change_request_size) const;
    bool handle_change_request(const void *change_request, long change_request_size);
    void notify_change_request_sent();

protected:
    c_network_session_parameter_base();

    const char *get_session_description() const;
    void set_update_required()
    {
        m_flags |= 1;
        m_peers_updated.set_all(false);
        notify_set_update_required();
    }
    void set_desires_change_request()
    {
        m_flags |= 2;
        notify_set_change_request();
    }
    void set_peer_no_longer_requires_update(long peer_index);
    void set_change_request_no_longer_desired();

    // Subclass hooks for notifications and update payloads
    virtual void notify_set_update_required();
    virtual void notify_set_change_request();
    virtual void notify_update_payload_sent_to_peer(long peer_index);
    virtual void notify_change_request_payload_sent();
    // Base hooks default to permissive; overrides specialize per parameter kind.
    virtual bool can_set_peer_no_longer_requires_update(long) { return true; }
    virtual bool can_clear_change_request_desired() { return true; }
    virtual bool parameter_data_ready_for_consumption() const { return true; }
    virtual void get_update_payload(long peer_index, void *payload) const = 0;
    virtual long get_update_payload_size() const = 0;
    virtual bool handle_update_payload(const void *payload) = 0;
    virtual void get_change_request_payload(long peer_index, void *payload) const = 0;
    virtual long get_change_request_payload_size() const = 0;
    virtual bool handle_change_request_payload(const void *payload) = 0;
    virtual void notify_reset_peer_state(long peer_index) = 0;
    virtual void notify_clear() = 0;
    virtual void notify_transition_state_to_become_host() = 0;

    long m_type;                                    // e_network_session_parameter_type
    const char *m_name;
    c_network_session *m_session;
    long m_update_priority;                         // e_parameter_update_priority
    long m_change_request_access;                   // e_parameter_change_request_access
    c_static_array<bool, 16> m_peers_updated;       // one per peer: this peer has the current value
    unsigned long m_flags;                          // bit 0 allowed (has a value), bit 1 desires a change request
};
typedef char network_session_parameter_base_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_session_parameter_base) == 0x2c) )) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_BASE_H
