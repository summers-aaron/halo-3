#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_TYPES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_TYPES_H

// Lifecycle state management and the common state-handler interface.

#include <stddef.h>
#include <string.h>
#include "memory/static_arrays.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/transport/transport_security_types.h"

extern "C" void network_life_cycle_handle_lost_connection();
extern "C" const long life_cycle_manager_skill_bounds[2];

enum e_network_session_type;

class c_network_session;
class c_network_session_manager;
class c_network_observer;
class c_matchmaking_seeker;
class c_game_variant;
struct s_life_cycle_matchmaking_progress;
struct s_matchmaking_session_properties;
struct s_network_session_player;
struct s_machine_identifier;
struct game_player_options;

enum
{
    k_maximum_state_change_entry_data_size = 16
};

class c_life_cycle_state_manager
{
public:
    void update();
    __forceinline void terminate() { set_current_state(_life_cycle_state_none, 0, 0); }
    __forceinline void enter_joining_state() { set_current_state(_life_cycle_state_joining, 0, 0); }
    friend void network_life_cycle_handle_lost_connection();
    bool current_state_ready_for_state_transition_or_query() const;
    bool current_state_ready_for_update() const;

    __forceinline void request_state_change(e_life_cycle_state state, long entry_data_size, const void *entry_data)
    {
        m_pending_state_change = true;
        m_pending_state = state;
        m_entry_data_size = entry_data_size;
        memset(m_entry_data, 0, sizeof(m_entry_data));
        if (m_entry_data_size > 0)
            memcpy(m_entry_data, entry_data, m_entry_data_size);
    }

    __forceinline c_life_cycle_state_handler *get_current_state_handler() const { return m_handlers[get_current_state()]; }
    __forceinline e_life_cycle_state get_current_state() const { return m_current_state; }
    __forceinline c_network_session *get_active_squad_session() const { return m_active_squad_session; }
    __forceinline c_network_session *get_target_session() const { return m_target_squad_session; }
    __forceinline c_network_session *get_group_session() const { return m_group_session; }
    __forceinline c_network_observer *get_observer() const { return m_observer; }

private:
    void set_current_state(e_life_cycle_state state, long entry_data_size, void *entry_data);
    void update_handle_session_states();
    void update_handle_session_parameters();

public:
    e_life_cycle_state m_current_state;
    c_static_array<c_life_cycle_state_handler *, k_life_cycle_state_count> m_handlers;
    c_network_session_manager *m_session_manager;
    c_network_session *m_active_squad_session;
    c_network_session *m_target_squad_session;
    c_network_session *m_group_session;
    c_network_observer *m_observer;
    bool m_pending_state_change;
    e_life_cycle_state m_pending_state;
    long m_entry_data_size;
    char m_entry_data[k_maximum_state_change_entry_data_size];
    __forceinline c_matchmaking_quality *get_matchmaking_quality() { return &m_matchmaking_quality; }
    __declspec(align(8)) c_matchmaking_quality m_matchmaking_quality;
    bool m_game_exhausted;
    bool m_game_exhausted_handled;
    char m_game_exhausted_padding[2];
    bool m_cached_identifier_valid;
    s_transport_secure_identifier m_cached_identifier;
    long m_last_game_end_sequence;
};

#pragma pack(push, 4)
class c_life_cycle_state_handler
{
    friend struct s_life_cycle_globals;
    friend class c_life_cycle_state_handler_matchmaking_arbitration;
    friend class c_life_cycle_state_handler_matchmaking_assemble_match;
    friend class c_life_cycle_state_handler_matchmaking_find_match;
public:
    c_life_cycle_state_handler() : m_handler_flags(0) {}
    virtual void update() = 0;
    virtual e_life_cycle_state_transition_type update_for_state_transition() = 0;
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *to_handler);
    virtual const char *get_state_string() = 0;
    virtual void handle_missing_required_session_parameter(e_network_session_type session_type);

    __forceinline void dispose() { m_manager->m_handlers[m_state] = 0; m_manager = 0; }

    __forceinline const unsigned char &get_handler_flags() const { return m_handler_flags; }
    __forceinline e_life_cycle_state get_state() const { return m_state; }
    // March: c_flags<e_life_cycle_state_handler_flags, unsigned char, 6>::test (a member, not stored as a c_flags yet).
    __forceinline bool test_flag(long flag) const { return (m_handler_flags & (1 << flag)) != 0; }
    __forceinline unsigned __int64 get_required_squad_session_parameter_mask() const { return m_required_squad_session_parameter_mask; }
    __forceinline unsigned __int64 get_required_group_session_parameter_mask() const { return m_required_group_session_parameter_mask; }
    // Public: life_cycle_manager.cpp's free functions (life_cycle_update_session_qos_address) call it.
    __forceinline c_life_cycle_state_manager *get_manager() const { return m_manager; }

protected:
    static void build_player_options(const s_network_session_player *player, unsigned long machine_valid_mask,
        const s_machine_identifier *machines, const c_game_variant *variant, long unused, long player_index, bool adjust_skill,
        long quality, game_player_options *options);
    static void update_advertised_session(const s_matchmaking_session_properties *session_properties);
    static bool session_has_minimum_player_count_to_start_game_in_hopper(const c_network_session *session);
    bool setup_initial_participants(c_network_session *session);
    void update_matchmaking_search_criteria(bool use_group);
    void disband_group_session();
    bool all_squad_peers_established_in_group();
    void get_matchmaking_progress_for_squad_search(s_life_cycle_matchmaking_progress *progress_out);
    void get_matchmaking_progress_for_setting_up_match(s_life_cycle_matchmaking_progress *progress_out);
    void update_search_progress(const c_matchmaking_seeker *matchmaking_seeker, bool unused, unsigned long time);
    void squad_session_host_abort_matchmaking();
    void reset_session_state();
    void matchmaking_update_required_files_for_hopper();

private:
    bool pick_teams_for_group_internal(bool update, unsigned long excluded_parties);

protected:
    // Offset 4 is never written by March's constructor or initialize(); the name is a guess (life_cycle_manager.cpp).
    long m_required_session_flags;
    // March initialize() stores its state argument at 8 and the manager at 12; five units read the state at 8.
    e_life_cycle_state m_state;
    c_life_cycle_state_manager *m_manager;
    unsigned char m_handler_flags;
    char m_handler_flags_padding[0x18 - 0x11];
    unsigned __int64 m_required_squad_session_parameter_mask;
    unsigned __int64 m_required_group_session_parameter_mask;
};
#pragma pack(pop)

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char life_cycle_manager_target_session_offset[(sizeof(void *) != 4 || (offsetof(c_life_cycle_state_manager, m_target_squad_session) == 0x54)) ? 1 : -1];
typedef char life_cycle_manager_observer_offset[(sizeof(void *) != 4 || (offsetof(c_life_cycle_state_manager, m_observer) == 0x5c)) ? 1 : -1];
typedef char life_cycle_manager_entry_data_offset[(sizeof(void *) != 4 || (offsetof(c_life_cycle_state_manager, m_entry_data) == 0x6c)) ? 1 : -1];
typedef char life_cycle_manager_quality_offset[
    (sizeof(void *) != 4 || offsetof(c_life_cycle_state_manager, m_matchmaking_quality) == 0x80) ? 1 : -1];
typedef char life_cycle_manager_game_exhausted_offset[(sizeof(void *) != 4 || (offsetof(c_life_cycle_state_manager, m_game_exhausted) == 0x98f4)) ? 1 : -1];
typedef char life_cycle_manager_game_exhausted_handled_offset[
    (sizeof(void *) != 4 || offsetof(c_life_cycle_state_manager, m_game_exhausted_handled) == 0x98f5) ? 1 : -1];
typedef char life_cycle_manager_size[(sizeof(void *) != 4 || (sizeof(c_life_cycle_state_manager) == 0x9908)) ? 1 : -1];
typedef char life_cycle_handler_size[(sizeof(void *) != 4 || (sizeof(c_life_cycle_state_handler) == 0x28)) ? 1 : -1];
#endif

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_TYPES_H
