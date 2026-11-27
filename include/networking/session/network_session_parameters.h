#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_H

#include "networking/logic/network_game_privacy_types.h"
#include "networking/session/network_session_parameter_payload_types.h"

// c_network_session_parameters: the replicated parameters of one c_network_session (0x62af0 bytes at +0x4f08):
// 47 parameter objects, then the session, the observer, a table of the 47 as their base, and two words of state.
// The header March 2007's asserts name (networking\session\network_session_parameters.cpp:263..1140 for the
// methods; the class is declared here).
//
// Layout: the parameter block (s_network_session_parameters_internal, the rest grouped with
// c_network_session's constructor 0x3b61b8 in network_memory.cpp) and the owner unit network_session_parameters.cpp,
// which holds all 47 parameters, including language, determinism
// version, the campaign skulls, metagame scoring and insertion point, saved film game options, matchmaking suppress
// progress, synchronous out of sync, matchmaking messaging, host frame quality, request campaign quit and leader
// properties (names from register_parameters' strings). Each offset is checked below.
//
// Parameter types: a parameter class of its own where March has one (session mode, session size, ui game mode,
// initial participants, game and map variant, game start status, countdown timer, the two remote join data,
// matchmaking hopper list, hopper, rematch data, hopper statistics), else c_generic_network_session_parameter<T>.
// The payload structs have fixed sizes; their fields are named where a unit's use or an
// accessor attests them, else they are storage. Their true headers (network_session_parameters_session.h, _game.h,
// _ui.h, _matchmaking.h, network_session_parameters_game_variant.h, ...) are declaration-only today and their own
// units still hold private copies of these classes: the classes sit here until those units switch (location
// dispute, recorded).
//
// Methods: March emits all 23 of c_network_session_parameters out of line in network_session_parameters.cpp;
// declared here as prototypes. The parameter classes' set/request_change are defined
// in their owner units: prototypes here; get() bodies are given inline.

#include "networking/session/network_session_parameters_chunked.h"
#include "networking/session/network_session_types.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/session/network_session_remote_join_types.h"
#include "networking/session/network_session_parameter_game_types.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/session/network_session_parameter_initial_participants_types.h"
#include "game/game_types.h"
#include "game/game_options.h"
#include "networking/session/network_session_parameter_variant_types.h"
#include <stddef.h>

class c_bitstream;
class c_network_observer;
struct s_player_identifier;
struct game_machine_options;
struct game_player_options;
struct s_network_message_parameters_update;
struct s_network_message_parameters_request;

enum e_gui_game_mode;
enum e_network_game_simulation_protocol;
enum e_network_game_start_mode;
enum e_matchmaking_search_preference;
enum e_network_game_countdown_delayed_reason;

#include "networking/session/network_session_parameter_change_request_source.h"

// ---- payloads ----

// fields from network_session_interface.cpp (network_session.cpp agrees on +0, +4, +0xc); the 0x3d1de0 set carries this name









// ---- parameter classes ----

class c_network_session_parameter_session_mode : public c_generic_network_session_parameter_data<s_network_session_parameter_session_mode>
{
public:
    c_network_session_parameter_session_mode();

    e_network_session_mode get() const
    {
        e_network_session_mode session_mode = _network_session_mode_none;
        if (get_allowed())
            session_mode = m_data.mode;
        return session_mode;
    }
    unsigned long get_time() const;
    bool set(e_network_session_mode session_mode);
    bool request_change(e_network_session_mode session_mode);

private:
    virtual bool handle_update_payload(const void *payload);
    virtual bool handle_change_request_payload(const void *payload);
};

class c_network_session_parameter_session_size : public c_generic_network_session_parameter_data<s_network_session_parameter_session_size>
{
public:
    c_network_session_parameter_session_size();

    long get_max_peer_count() const;
    bool set_max_peer_count(long maximum_peer_count);
    long get_max_player_count() const;
    bool set_max_player_count(long maximum_player_count);
    bool request_set_max_player_count(long maximum_player_count);
};

class c_network_session_parameter_ui_game_mode : public c_generic_network_session_parameter_data<e_gui_game_mode>
{
public:
    c_network_session_parameter_ui_game_mode();

    const e_gui_game_mode *get() const
    {
        const e_gui_game_mode *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(e_gui_game_mode game_mode);
    bool set(const e_gui_game_mode *game_mode);
    bool request_change(e_gui_game_mode game_mode);
    void clear_session_parameters_for_ui_mode_transition();

private:
    virtual bool handle_change_request_payload(const void *payload);
};

class c_network_session_parameter_initial_participants : public c_generic_network_session_parameter_data<s_network_session_parameter_initial_participants>
{
public:
    c_network_session_parameter_initial_participants();

    __forceinline bool get(const game_machine_options **machine_options, const game_player_options **player_options) const
    {
        bool result = false;
        if (get_allowed() && m_data.valid)
        {
            *machine_options = &m_data.machine_options;
            *player_options = m_data.player_options;
            result = true;
        }
        return result;
    }
    bool set(const game_machine_options *machine_options, const game_player_options *player_options);
};

#include "networking/session/network_session_parameter_variants.h"

class c_network_session_parameter_game_start_status : public c_generic_network_session_parameter_data<s_network_session_parameter_game_start_status>
{
public:
    c_network_session_parameter_game_start_status();

    const s_network_session_parameter_game_start_status *get() const
    {
        const s_network_session_parameter_game_start_status *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_parameter_game_start_status *start_status);
};

class c_network_session_parameter_countdown_timer : public c_generic_network_session_parameter_data<s_network_session_parameter_countdown_timer>
{
public:
    c_network_session_parameter_countdown_timer();

    long get_countdown_timer() const;
    e_network_game_countdown_delayed_reason get_countdown_delayed_reason() const;
    const s_player_identifier *get_countdown_delayed_culprit() const;
    bool set(bool active, long countdown_seconds, e_network_game_countdown_delayed_reason delayed_reason, const s_player_identifier *delayed_culprit);
    bool request_change(bool active, long countdown_seconds, e_network_game_countdown_delayed_reason delayed_reason,
        const s_player_identifier *delayed_culprit);

private:
    virtual bool handle_change_request_payload(const void *payload);
    bool set_internal(bool active, long countdown_seconds, long delayed_countdown, e_network_game_countdown_delayed_reason delayed_reason,
        const s_player_identifier *delayed_culprit);
};

class c_network_session_parameter_requested_remote_join_data : public c_generic_network_session_parameter_data<s_network_session_remote_session_join_data>
{
public:
    c_network_session_parameter_requested_remote_join_data();

    const s_network_session_remote_session_join_data *get() const
    {
        const s_network_session_remote_session_join_data *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_remote_session_join_data *join_data);
    bool request_change(const s_network_session_remote_session_join_data *join_data);
};

class c_network_session_parameter_remote_join_data : public c_generic_network_session_parameter_data<s_network_session_remote_session_join_data>
{
public:
    c_network_session_parameter_remote_join_data();

    const s_network_session_remote_session_join_data *get() const
    {
        const s_network_session_remote_session_join_data *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_remote_session_join_data *join_data);
};

class c_network_session_parameter_matchmaking_hopper_list : public c_generic_network_session_parameter_data<s_network_session_matchmaking_hopper_list>
{
public:
    c_network_session_parameter_matchmaking_hopper_list();

    const s_network_session_matchmaking_hopper_list *get() const
    {
        const s_network_session_matchmaking_hopper_list *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_matchmaking_hopper_list *hopper_list);
};

class c_network_session_parameter_matchmaking_hopper : public c_generic_network_session_parameter_data<s_network_session_matchmaking_hopper>
{
public:
    c_network_session_parameter_matchmaking_hopper();

    const s_network_session_matchmaking_hopper *get() const
    {
        const s_network_session_matchmaking_hopper *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_matchmaking_hopper *hopper);
};

class c_network_session_parameter_matchmaking_rematch_data
    : public c_generic_network_session_parameter_data<s_network_session_parameter_matchmaking_rematch_data>
{
public:
    c_network_session_parameter_matchmaking_rematch_data();

    const s_network_session_parameter_matchmaking_rematch_data *get() const
    {
        const s_network_session_parameter_matchmaking_rematch_data *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_network_session_parameter_matchmaking_rematch_data *rematch_data);
    __forceinline bool request_change(const s_network_session_parameter_matchmaking_rematch_data *rematch_data)
    {
        bool result = false;
        if (set_allowed()) {
            result = handle_change_request_payload(rematch_data);
        } else if (change_request_allowed()) {
            m_requested_data = *rematch_data;
            m_flags |= 2;
            notify_set_change_request();
            result = true;
        }
        return result;
    }
    static bool change_request_data_valid(const s_network_session_parameter_matchmaking_rematch_data *rematch_data);

private:
    virtual bool handle_change_request_payload(const void *payload);
};

class c_network_session_parameter_matchmaking_hopper_statistics : public c_generic_network_session_parameter_data<s_matchmaking_single_hopper_statistics>
{
public:
    c_network_session_parameter_matchmaking_hopper_statistics();

    const s_matchmaking_single_hopper_statistics *get() const
    {
        const s_matchmaking_single_hopper_statistics *result = 0;
        if (get_allowed())
            result = &m_data;
        return result;
    }
    bool set(const s_matchmaking_single_hopper_statistics *statistics);
};

// Language and host frame quality are distinct parameter types for long payloads.
class c_network_session_parameter_language : public c_generic_network_session_parameter<long>
{
public:
    c_network_session_parameter_language();
};

class c_network_session_parameter_host_frame_quality : public c_generic_network_session_parameter<long>
{
public:
    c_network_session_parameter_host_frame_quality();
};

// ---- the class ----

class c_network_session_parameters
{
public:
    c_network_session_parameters();
    ~c_network_session_parameters();

    void initialize(c_network_session *session, c_network_observer *observer);
    void clear();
    void clear_parameters_with_mask(unsigned __int64 parameter_mask);
    void update();
    void reset_peer_state(long peer_index);
    void transition_state(bool is_host);
    const char *get_session_description() const;
    bool initial_update_received() const;
    void set_initial_update_received() { m_flags |= 1; }
    bool parameters_valid(unsigned __int64 parameter_mask) const;
    bool parameters_transmitted_to_peers(unsigned __int64 parameter_mask) const;
    bool handle_update(const s_network_message_parameters_update *update);
    bool handle_change_request(e_parameter_change_request_source change_request_source, const s_network_message_parameters_request *change_request);
    unsigned __int64 build_update_for_peer(long peer_index, bool high_priority_only, s_network_message_parameters_update *update);

private:

    void register_parameters();
    void check_parameter_registration();
    void check_to_send_updates();
    void check_to_send_change_requests();
    unsigned __int64 get_outstanding_parameter_update_mask_for_peer(long peer_index) const;
    void add_parameters_to_update(long peer_index, bool high_priority_only, unsigned char *buffer, long buffer_size,
        unsigned __int64 *update_mask, long *update_size);
    bool parameters_handle_update(unsigned __int64 update_mask, const unsigned char *buffer, long buffer_size,
        long *update_size, unsigned __int64 *handled_mask);

public:
    struct s_network_session_parameters_internal
    {
        s_network_session_parameters_internal();

        c_network_session_parameter_session_mode session_mode;
        c_network_session_parameter_session_size session_size;
        c_generic_network_session_parameter<s_network_session_privacy_mode> privacy_mode;
        c_network_session_parameter_ui_game_mode ui_game_mode;
        c_generic_network_session_parameter<s_network_session_parameter_map> map;
        c_network_session_parameter_initial_participants initial_participants;
        c_generic_network_session_parameter<unsigned __int64> game_instance;
        c_generic_network_session_parameter<unsigned long> random_seed;
        c_network_session_parameter_language language;
        c_generic_network_session_parameter<long> determinism_version;
        c_generic_network_session_parameter<e_network_game_simulation_protocol> simulation_protocol;
        c_generic_network_session_parameter<e_campaign_difficulty_level> campaign_difficulty;
        c_generic_network_session_parameter<long> campaign_active_primary_skulls;
        c_generic_network_session_parameter<long> campaign_active_secondary_skulls;
        c_generic_network_session_parameter<short> campaign_metagame_scoring;
        c_generic_network_session_parameter<short> campaign_insertion_point;
        c_generic_network_session_parameter<s_network_ui_state> ui_state;
        c_generic_network_session_parameter<bool> end_game;
        c_generic_network_session_parameter<e_network_game_start_mode> start_mode;
        c_network_session_parameter_game_variant game_variant;
        c_network_session_parameter_map_variant map_variant;
        c_generic_network_session_parameter<s_network_session_parameter_saved_film> saved_film;
        c_network_session_parameter_saved_film_game_options saved_film_game_options;
        c_network_session_parameter_game_start_status game_start_status;
        c_network_session_parameter_countdown_timer countdown_timer;
        c_generic_network_session_parameter<long> voice_repeater;
        c_network_session_parameter_requested_remote_join_data requested_remote_join_data;
        c_network_session_parameter_remote_join_data remote_join_data;
        c_generic_network_session_parameter<unsigned __int64> matchmaking_arbitration_nonce;
        c_network_session_parameter_matchmaking_hopper_list matchmaking_hopper_list;
        c_generic_network_session_parameter<s_game_hopper_picked_game_collection> matchmaking_game_list;
        c_generic_network_session_parameter<short> matchmaking_requested_hopper;
        c_network_session_parameter_matchmaking_hopper matchmaking_hopper;
        c_generic_network_session_parameter<bool> matchmaking_abort_requested;
        c_generic_network_session_parameter<e_matchmaking_search_preference> matchmaking_search_preference;
        c_generic_network_session_parameter<s_replicated_life_cycle_matchmaking_progress> matchmaking_progress;
        c_generic_network_session_parameter<s_replicated_life_cycle_matchmaking_progress_search_criteria> matchmaking_progress_search_criteria;
        c_generic_network_session_parameter<unsigned long> matchmaking_assembly_peer_evict_mask;
        c_network_session_parameter_matchmaking_rematch_data matchmaking_rematch_data;
        c_network_session_parameter_matchmaking_hopper_statistics matchmaking_hopper_statistics;
        c_generic_network_session_parameter<long> matchmaking_tip;
        c_generic_network_session_parameter<bool> matchmaking_suppress_progress;
        c_generic_network_session_parameter<s_network_session_parameter_synchronous_out_of_sync> synchronous_out_of_sync;
        c_generic_network_session_parameter<s_network_session_parameter_matchmaking_messaging> matchmaking_messaging;
        c_network_session_parameter_host_frame_quality host_frame_quality;
        c_generic_network_session_parameter<s_network_session_parameter_request_campaign_quit> request_campaign_quit;
        c_generic_network_session_parameter<s_network_session_parameter_leader_properties> leader_properties;
    };

    s_network_session_parameters_internal m_parameters;
    c_network_session *m_session;
    c_network_observer *m_observer;
    c_network_session_parameter_base *m_parameter_interfaces[k_network_session_parameter_type_count];
    unsigned long m_flags;
    unsigned long m_peers_reset_mask;                                                       // one bit per peer, cleared by reset_peer_state
};

#define NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(member, offset) \
    typedef char network_session_parameters_offset_check_##member[(sizeof(void *) != 4 || \
        offsetof(c_network_session_parameters::s_network_session_parameters_internal, member) == (offset)) ? 1 : -1]
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(session_size, 0x44);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(privacy_mode, 0x80);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(ui_game_mode, 0xcc);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(map, 0x100);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(initial_participants, 0x330);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(game_instance, 0x2940);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(random_seed, 0x2980);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(language, 0x29b4);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(simulation_protocol, 0x2a1c);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(campaign_difficulty, 0x2a50);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(campaign_metagame_scoring, 0x2aec);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(ui_state, 0x2b4c);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(end_game, 0x2b7c);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(start_mode, 0x2bac);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(game_variant, 0x2be0);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(map_variant, 0x6148);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(saved_film, 0x30310);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(saved_film_game_options, 0x30770);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(game_start_status, 0x60838);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(countdown_timer, 0x6087c);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(voice_repeater, 0x608d8);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(requested_remote_join_data, 0x60910);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(remote_join_data, 0x60a10);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_arbitration_nonce, 0x60b10);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_hopper_list, 0x60b50);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_game_list, 0x618c4);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_requested_hopper, 0x61c50);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_hopper, 0x61c80);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_abort_requested, 0x61d44);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_search_preference, 0x61d74);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_progress, 0x61da8);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_progress_search_criteria, 0x61e14);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_assembly_peer_evict_mask, 0x61e88);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_rematch_data, 0x61ebc);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_hopper_statistics, 0x627f0);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_tip, 0x6282c);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_suppress_progress, 0x62860);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(synchronous_out_of_sync, 0x62890);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(matchmaking_messaging, 0x628c0);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(host_frame_quality, 0x62904);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(request_campaign_quit, 0x62938);
NETWORK_SESSION_PARAMETERS_OFFSET_CHECK(leader_properties, 0x62974);
#undef NETWORK_SESSION_PARAMETERS_OFFSET_CHECK
typedef char network_session_parameters_internal_size_check[(sizeof(void *) != 4
    || ((sizeof(c_network_session_parameters::s_network_session_parameters_internal) == 0x62a20) )) ? 1 : -1];
typedef char network_session_parameters_size_check[(sizeof(void *) != 4 || ((sizeof(c_network_session_parameters) == 0x62af0) )) ? 1 : -1];

extern "C" bool network_session_parameters_decode_update_message(c_bitstream *bitstream, s_network_message_parameters_update *update_message);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_H
