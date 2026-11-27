// networkinglibxbox/network_session_interface.obj: from the records 0x8a348 0x8b790 0x8b7e8 0x8dc48 0x90828 0x90a28 0x1cee08 0x3a2f98 0x3b08f8 0x3b0980 0x3b0a50 0x3b0b28 0x3b0b80 0x3b0c68 0x3b0cf0 0x3b0e20 0x3b0ed8 0x3b0fa0 0x3b11c0 0x3b1298 0x3b1390 0x3b1480 0x3b15a0 0x3b1740 0x3b1850 0x3b1970 0x3b19e8 0x3b1b00 0x3b1bd8 0x3b1cb0 0x3b1d88 0x3b1e60 0x3b2060 0x3b21e0 0x3b22f0 0x3b23c8 0x3b24d8 0x3b2788 0x3b29e0 0x3b2ad0 0x3b2ba8 0x3b2c80 0x3b2d08 0x3b2db8 0x3b2e38 0x3b2f20 0x3b2fc8 0x3b3110 0x3b3190 0x3b3608 0x3b37e0 0x3b38d8 0x3b3d28 0x3b4100 0x3b4450 0x3b4610 0x3b4bd0 0x3b4e98 0x3b4f60 0x3b5630 0x3b5738 0x3b5898 0x3b5930 0x3b59c0 0x3b5a50 0x3b5b20 0x3b5bf0 0x3b5cc0 0x3b5e68 0x3c6338 0x3cd980 0x3d1de0 0x412d08 0x412dc0 0x46d7f8 0x46d8b0 0x46d968 0x46f8f8 0x495440
// @flags /TP /O2 /Oc

#include "networking/logic/network_search_types.h"
#include "networking/online/online_url_version.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_session_queries.h"
#include "game/game_engine_team.h"
#include "networking/session/session_network_message_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/logic/network_banhammer.h"
#include "networking/logic/network_session_interface_globals.h"
#include "cseries/language.h"
#include "main/main_flag_state.h"
#include "main/main_runtime_globals.h"
#include "memory/static_arrays.h"
#include "networking/logic/squad_join_permission_types.h"
#include "interface/gui_game_mode_types.h"
#include "networking/logic/storage/hopper_load_status_types.h"
#include "interface/session_game_mode_types.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
#include "networking/online/online_nat_types.h"
#include "networking/transport/transport_globals.h"
#include "networking/transport/transport_available.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_state_accessors.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/game_engine.h"
#include "interface/user_interface.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "game/game_engine_scoring.h"
#include "game/multiplayer_game_hopper.h"
#include "interface/chud/chud_messaging.h"
#include "main/main.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/transport/transport_security.h"
#include "cache/cache_files.h"
#include "game/game.h"
#include "main/levels.h"
#include "game/game_globals.h"
#include "main/loading.h"
#include "memory/crc.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game.h"
#include "simulation/simulation_world.h"
#include "networking/session/network_session_class.h"
#include "tag_files/tag_types.h"
#include "networking/transport/transport_platform_types.h"
#include "cseries/language_types.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "scenario/scenario_tag.h"
#include "networking/network_configuration_types.h"
#include "cache/cache_file_globals.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/session/network_session_parameter_game_types.h"
#include "simulation/simulation_globals.h"
#include "networking/transport/transport_security_types.h"
#include "networking/logic/network_game_privacy_types.h"
#include "networking/logic/life_cycle/peer_request_types.h"
#include "networking/network_utilities.h"
#include "scenario/scenario.h"
#include "networking/online/online_url_name_globals.h"
#include "objects/object_type_definition.h"
#include "networking/session/network_game_simulation_protocol_types.h"
#include "networking/session/network_session_parameter_specializations.h"
#include "networking/session/network_session_parameters.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include "networking/session/network_session_parameter_payload_types.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game_internals.h"
#include "networking/session/network_session_parameter_variant_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "game/map_variant.h"
#include "game/game_globals_storage.h"
#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_session_interface_services.h"
#include "memory/tls_globals.h"
#include "networking/logic/network_life_cycle_session_helper_templates.h"
#include "networking/transport/transport_qos_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/network_time_types.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "saved_games/saved_film_description.h"
#include "networking/logic/life_cycle/life_cycle_session.h"
#include "interface/controller_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "networking/network_configuration.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "cseries/read_write_barrier.h"
#include "game/hopper_configuration.h"
#include "game/game_engine_scoring_globals.h"


// @address 0x73a8fc g_online_url_version
long g_online_url_version = 1;
enum e_network_game_start_mode;
enum e_network_game_privacy;

class c_game_variant;
// @address 0x3b5e68 ?set@?$c_generic_network_session_parameter@Us_nsi_session_string@@@@QAA_NPBUs_nsi_session_string@@@Z
// @address 0x3b0fa0 ?network_squad_session_set_coop_game_options@@YA_NW4e_campaign_id@@W4e_map_id@@PBDFPBX@Z
// @address 0x8a348 ?network_session_interface_get_local_user_identifier@@YA_NJPAUs_player_identifier@@_N@Z
// @address 0x3b1740 ?network_squad_session_set_film@@YA_NPBUs_saved_film_description@@@Z
// @address 0x3b1298 ?network_squad_session_set_map@@YA_NW4e_campaign_id@@W4e_map_id@@PBD@Z
// @address 0x3b4e98 ?network_life_cycle_session_get_map@@YA_NPAW4e_campaign_id@@PAW4e_map_id@@@Z
// @address 0x3b2e38 ?network_squad_session_set_countdown_timer@@YA_N_NJW4e_network_game_countdown_delayed_reason@@J@Z
// @address 0x3b19e8 ?network_squad_session_set_campaign_difficulty@@YA_NW4e_campaign_difficulty_level@@@Z
// @address 0x3b0980 ?network_squad_session_is_joinable@@YA?AW4e_network_join_refuse_reason@@XZ
// @address 0x8b790 network_squad_session_get_peer_count
// @address 0x8dc48 ?network_life_cycle_squad_session_get_ui_game_mode@@YA?AW4e_gui_game_mode@@XZ
// @address 0x8b7e8 network_squad_session_get_membership
// @address 0x3b0c68 ?network_group_session_set_disable_host_election@@YAJJ@Z
// @address 0x3b0cf0 ?network_squad_session_get_matchmaking_tip@@YA_NJJ@Z
// @address 0x3b0e20 network_group_session_get_membership
// @address 0x3cd980 network_squad_session_set_privacy_mode
// @address 0x1cee08 ?map_variant_is_valid@@YA_NPBUs_map_variant_validation@@@Z
// @address 0x3b08f8 ?network_life_cycle_in_matchmaking@@YA_NXZ
// @address 0x3b0b28 ?network_squad_session_get_maximum_player_count@@YAJXZ
// @address 0x3b0ed8 ?network_squad_session_can_set_game_settings@@YA_NXZ
// @address 0x3b11c0 ?network_squad_session_get_map@@YAXPAW4e_campaign_id@@PAW4e_map_id@@PAD@Z
// @address 0x3b1e60 ?network_squad_session_get_start_mode@@YA?AW4e_network_game_start_mode@@XZ
// @address 0x3b1850 network_squad_session_set_game_options
// @address 0x3b2060 ?network_squad_session_get_privacy_mode@@YA?AW4e_network_game_privacy@@XZ
// @address 0x3b2f20 ?network_squad_session_get_countdown_timer@@YAJXZ
// @address 0x3b2fc8 network_squad_session_get_countdown_delayed_culprit
// @address 0x3b3110 ?network_squad_session_get_session_class@@YA?AW4e_network_session_class@@XZ
// @address 0x3b3608 network_session_get_membership
// @address 0x3b37e0 ?network_session_set_player_failure_reason@@YAXJW4e_network_join_refuse_reason@@@Z
// @address 0x495440 ?network_squad_session_get_hopper@@YAGXZ
// @address 0x3b59c0 ?request_change@?$c_generic_network_session_parameter@F@@QAA_NPBF@Z
// @address 0x3a2f98 ?set@?$c_generic_network_session_parameter@Us_network_session_parameter_synchronous_out_of_sync@@@@QAA_NPBUs_network_session_parameter_synchronous_out_of_sync@@@Z
// @address 0x3b5b20 ?set@?$c_generic_network_session_parameter@F@@QAA_NPBF@Z
// @address 0x3b5630 network_session_interface_update_frame_rate
// @address 0x3b1480 ?network_squad_session_set_map_variant@@YA_NPBVc_map_variant@@@Z
// @address 0x3b21e0 network_squad_session_delegate_leadership
// @address 0x3b23c8 network_squad_session_boot_player
// @address 0x3b24d8 ?network_session_calculate_peer_connectivity@@YAXPAVc_network_session@@PAUs_network_session_peer_connectivity@@@Z
// @address 0x3b2ba8 ?network_session_interface_get_user_team_index@@YAJJ@Z
// @address 0x3d1de0 ?set@?$c_generic_network_session_parameter@Us_network_session_privacy_mode@@@@QAA_NPBUs_network_session_privacy_mode@@@Z
// @address 0x3b4f60 network_life_cycle_session_get_map_load_status
// @address 0x412d08 ?set@c_network_session_parameter_ui_game_mode@@QAA_NW4e_gui_game_mode@@@Z
// @address 0x412dc0 ?set@c_network_session_parameter_ui_game_mode@@QAA_NPBW4e_gui_game_mode@@@Z
// @address 0x46f8f8 network_session_interface_set_controller_desired_team

// 0x44 bytes: a membership slot (1e60, 2060, 2f20), with the countdown timer's layout (2f20 reads it as a slot).

// the countdown timer (0x65784) is a membership slot with the delayed reason and culprit that 3b2fc8 reads

// the session block at 0x4f08: its settings (block at 0xcc) and its membership (slots at 0x80, 0x2bac, 0x6087c, and
// the local peer at 0x61c80)
// a long-sized session parameter (0x34 bytes): the two at session+0x798c and +0x79c0

typedef c_game_variant s_network_session_interface_game_variant;
struct s_owner;

struct s_game_context;
struct s_nsi_session_string { char text[64]; };
struct s_network_session_interface_map_id { long value; };

struct s_session_membership {
  c_network_session_parameter_session_mode session_mode;
  c_network_session_parameter_session_size session_size;
  c_generic_network_session_parameter<s_network_session_privacy_mode> privacy_mode;
  c_generic_network_session_parameter<long> ui_game_mode;
  
  c_generic_network_session_parameter<s_network_session_parameter_map> map;      // (the map parameter)
  c_network_session_parameter_initial_participants initial_participants;
  c_generic_network_session_parameter<unsigned __int64> game_instance;
  c_generic_network_session_parameter<unsigned long> random_seed;
  c_generic_network_session_parameter<long> language;
  c_generic_network_session_parameter<long> determinism_version;
  c_generic_network_session_parameter<long> simulation_protocol;
  c_generic_network_session_parameter<long> difficulty;
  c_generic_network_session_parameter<e_network_game_simulation_protocol> campaign_active_primary_skulls;
  c_generic_network_session_parameter<e_network_game_simulation_protocol> campaign_active_secondary_skulls;
  c_generic_network_session_parameter<short> campaign_metagame_scoring;
  c_generic_network_session_parameter<short> coop_insertion_point;
  c_generic_network_session_parameter<s_network_ui_state> ui_state;
#ifdef HALO3_64BIT
  c_generic_network_session_parameter<bool> end_game;
#else
  struct { char header[0x28]; unsigned long flags; bool data; bool requested_data; } end_game;
#endif
  c_generic_network_session_parameter<e_network_game_start_mode> start_mode;
  c_network_session_parameter_game_variant game_variant;
  c_network_session_parameter_map_variant map_variant;

  c_generic_network_session_parameter<s_nsi_film_description> film;
  c_network_session_parameter_saved_film_game_options game_options;
  c_network_session_parameter_game_start_status game_start_status;
  c_network_session_parameter_countdown_timer countdown_timer;
  c_generic_network_session_parameter<s_network_session_parameter_voice_repeater> voice_repeater;
  c_network_session_parameter_requested_remote_join_data requested_remote_join_data;
  c_network_session_parameter_remote_join_data remote_join_data;
  c_generic_network_session_parameter<unsigned __int64> matchmaking_arbitration_nonce;
  c_network_session_parameter_matchmaking_hopper_list matchmaking_hopper_list;
  c_generic_network_session_parameter<s_game_hopper_picked_game_collection> matchmaking_game_list;
  c_generic_network_session_parameter<short> matchmaking_requested_hopper;
  c_generic_network_session_parameter<s_network_session_matchmaking_hopper> local_peer;
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
  c_generic_network_session_parameter<long> campaign_difficulty;

  c_peer_request_parameter parameter_62938;
  c_generic_network_session_parameter<s_nsi_session_string> session_string;
  c_network_session *m_parameter_session;
  c_network_observer *m_parameter_observer;
  c_network_session_parameter_base *m_parameter_interfaces[k_network_session_parameter_type_count];
  unsigned long m_parameter_flags;
  unsigned long m_parameter_peers_reset_mask;
};

// match: retain the existing session accessor view; canonical adoption changes linked register allocation.
struct s_network_session {
  long current_local_state() const { return m_local_state; }
  bool disconnected() const { return current_local_state() == 0; }
  bool established() const
  {
    bool established = false;
    switch (current_local_state()) {
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
      // match: 0x3b0980: without it established() lowers to a bounds check plus jump table instead of one cmplwi 4 / blt state range test
      __assume(0);
    }
    return established;
  }
  e_network_session_class session_class() const { return m_session_class; }
  long get_session_id() const { return session_id; }
  __forceinline const c_network_session_membership *get_session_membership() const
  {
    return &m_session_membership;
  }

#ifdef HALO3_64BIT
  void *m_vtable;
  c_network_message_gateway *m_message_gateway;
  c_network_observer *m_observer;
  c_network_session_manager *m_session_manager;
  long session_id;
  long m_session_type;
  e_network_session_class m_session_class;
  c_network_session_membership m_session_membership;
  union {
    s_session_membership parameters;
    __declspec(align(8)) char parameters_storage[0x63800];
  };
  e_network_session_state m_local_state;
#else
  c_network_message_gateway *m_message_gateway;
  c_network_observer *m_observer;
  c_network_session_manager *m_session_manager;
  long m_session_index;
  long session_id;
  long m_session_type;
  e_network_session_class m_session_class;
  c_network_session_membership m_session_membership;
  s_session_membership parameters;
  long m_local_state;
#endif
};

__forceinline long c_network_session_parameter_session_size::get_max_peer_count() const
{
        long result = 16;
        if (m_flags & 1)
            result = m_data.maximum_peer_count;
        return result;
    }

__forceinline bool c_network_session_parameter_session_size::request_set_max_player_count(long maximum_player_count)
{
        bool result = false;
        if (this->set_allowed())
            result = set_max_player_count(maximum_player_count);
        else if (this->change_request_allowed()) {
            m_requested_data.maximum_peer_count = m_data.maximum_peer_count;
            m_requested_data.maximum_player_count = maximum_player_count;
            set_desires_change_request();
            result = true;
        }
        return result;
    }

__forceinline bool c_network_session_parameter_map_variant::request_change(const c_map_variant *variant)
{
        bool result = false;
        if (this->set_allowed())
            result = set(variant);
        else if (this->change_request_allowed()) {
            if (!variant) {
                m_requested_data.valid = false;
                memset(&m_requested_data.map_variant, 0, sizeof(m_requested_data.map_variant));
            } else {
                m_requested_data.valid = true;
                memcpy(&m_requested_data.map_variant, variant, sizeof(m_requested_data.map_variant));
            }
            set_desires_change_request();
            result = true;
        }
        return result;
    }

struct s_game_context;

static __forceinline s_session_membership *network_session_interface_parameters(c_network_session *session)
{
  return (s_session_membership *)session->get_session_parameters();
}
static __forceinline const s_session_membership *network_session_interface_parameters(const c_network_session *session)
{
  return (const s_session_membership *)session->get_session_parameters();
}
static __forceinline s_session_membership *network_session_interface_parameters(s_network_session *session)
{
  return &session->parameters;
}
static __forceinline const s_session_membership *network_session_interface_parameters(const s_network_session *session)
{
  return &session->parameters;
}

typedef char network_session_interface_peer_size[(sizeof(s_network_session_interface_peer_properties) == 0xb0) ? 1 : -1];
typedef char network_session_interface_layout_check_1[(sizeof(void *) != 4 || ((offsetof(s_session_membership, session_size) == 0x44))) ? 1 : -1];
typedef char network_session_interface_layout_check_2[(sizeof(void *) != 4 || ((offsetof(s_session_membership, map_variant) == 0x6148))) ? 1 : -1];
typedef char network_session_interface_layout_check_3[(sizeof(void *) != 4 || ((offsetof(s_session_membership, session_string) == 0x62974))) ? 1 : -1];
typedef char network_session_interface_layout_check_4[(sizeof(void *) != 4 || ((offsetof(c_network_session, m_session_parameters) == 0x4f08))) ? 1 : -1];
typedef char network_session_interface_layout_check_5[(sizeof(void *) != 4 || ((offsetof(c_network_session, m_local_state) == 0x679f8))) ? 1 : -1];
typedef char network_session_interface_view_membership_check[(sizeof(void *) != 4 || ((offsetof(s_network_session, m_session_membership) == 0x20))) ? 1 : -1];
typedef char network_session_interface_view_parameters_check[(sizeof(void *) != 4 || ((offsetof(s_network_session, parameters) == 0x4f08))) ? 1 : -1];
typedef char network_session_interface_view_state_check[(sizeof(void *) != 4 || ((offsetof(s_network_session, m_local_state) == 0x679f8))) ? 1 : -1];
typedef char network_session_interface_parameters_size_check[
    (sizeof(s_session_membership) == sizeof(c_network_session_parameters)) ? 1 : -1];
#ifdef HALO3_64BIT
typedef char network_session_interface_64_membership_check[(offsetof(s_network_session, m_session_membership) == offsetof(c_network_session, m_session_membership)) ? 1 : -1];
typedef char network_session_interface_64_parameters_check[(offsetof(s_network_session, parameters) == offsetof(c_network_session, m_session_parameters)) ? 1 : -1];
typedef char network_session_interface_64_state_check[(offsetof(s_network_session, m_local_state) == offsetof(c_network_session, m_local_state)) ? 1 : -1];
#endif
typedef char network_session_interface_layout_check_6[(offsetof(network_session_interface_globals, game_instance) == 0x6d0) ? 1 : -1];
typedef char network_session_interface_layout_check_7[(offsetof(network_session_interface_globals, previous_film) == 0x764) ? 1 : -1];
typedef char network_session_interface_layout_check_8[(offsetof(network_session_interface_globals, cache) == 0x980) ? 1 : -1];
typedef char network_session_interface_layout_check_9[(offsetof(network_session_interface_globals, sessions) == 0x9b8) ? 1 : -1];

typedef char network_session_interface_layout_check_11[
    (sizeof(void *) != 4 || ((offsetof(c_network_session_parameter_map_variant,
        m_requested_data) + offsetof(s_network_session_parameter_map_variant, map_variant) == 0xe0d0))) ? 1 : -1];

network_session_interface_globals g_network_session_interface_globals;

#if defined(__clang__) || !defined(_XBOX)
bool g_network_online_connected;
bool g_network_squad_map_loading;

// The slot view begins at the peer header and advances by one user record.
#if defined(_MSC_VER)
__attribute__((alias("?g_network_session_interface_globals@@3Unetwork_session_interface_globals@@A")))
#else
__attribute__((alias("g_network_session_interface_globals")))
#endif
extern "C" s_player_slot g_network_session_interface_local_users[NUMBEROF(g_network_session_interface_globals.users)];
#endif

struct s_memory_region;

static __forceinline bool session_state_is_established(const s_network_session *session)
{
  return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}
static __forceinline bool network_session_try_get(s_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    s_network_session *current = (s_network_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (current->m_local_state != 0) {
      *session = current;
      result = true;
    }
  }
  return result;
}
static __forceinline bool network_session_try_get(c_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    c_network_session *current = network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (current->m_local_state != 0) {
      *session = current;
      result = true;
    }
  }
  return result;
}
static __forceinline long current_mode(void)
{
  long mode = 0;
  if (network_life_cycle_globals_get()->initialized)
    mode = network_life_cycle_globals_get()->manager.get_current_state();
  return mode;
}
static __forceinline const long *block_get_limits(const c_generic_network_session_parameter<long> *block)
{
  const long *limits = 0;
  if (block->m_flags & 1)
    limits = &block->m_data;
  return limits;
}
static __forceinline const s_network_session_parameter_map *membership_slot_get_data(
        const c_generic_network_session_parameter<s_network_session_parameter_map> *slot)
{
  const s_network_session_parameter_map *result = 0;
  if (slot->m_flags & 1)
    result = &slot->m_data;
  return result;
}
static __forceinline long membership_slot_get_valid_value(const c_network_session_parameter_countdown_timer *slot)
{
  long result = -1;
  if (slot->get_allowed() && slot->m_data.running)
    result = slot->m_data.countdown;
  return result;
}
static __forceinline s_network_session_matchmaking_hopper *session_peer_value(
        c_generic_network_session_parameter<s_network_session_matchmaking_hopper> *peer)
{
  s_network_session_matchmaking_hopper *result = 0;
  if (peer->m_flags & 1)
    result = &peer->m_data;
  return result;
}
static __forceinline const s_player_identifier *countdown_timer_get_delayed_culprit(
        const c_generic_network_session_parameter<s_network_session_parameter_countdown_timer> *parameter)
{
  if ((parameter->m_flags & 1) && parameter->m_data.running && parameter->m_data.reason == 1)
    return &parameter->m_data.culprit;
  return 0;
}

// Copy the local user's identifier when the slot is active.

// The index of the countdown's delayed culprit in the membership. The peer iteration (0x3cac90, 0x3cace0 in
// network_session_membership.cpp) keeps the culprit and the membership across its calls.

struct s_game_context;
struct s_owner;


static __forceinline bool network_session_interface_channel_connected(long state)
{
  bool connected = false;
  if ((unsigned long)state - _observer_state_connected
      <= _observer_state_disconnected - _observer_state_connected)
    connected = true;
  return connected;
}
static __forceinline bool network_session_interface_channel_get_qos_result(
    const c_network_observer::s_channel_observer *channel, bool *complete, s_transport_qos_result *qos_result)
{
  bool success = false;
  bool qos_complete = false;
  if (channel->flags & 8) {
    success = true;
    if (channel->flags & 16) {
      *qos_result = channel->qos_result;
      qos_complete = true;
    }
  }
  *complete = qos_complete;
  return success;
}
class c_network_session_parameters;
struct s_network_session_peer_properties;
// @address 0x8c447c g_network_time_globals

static __forceinline bool network_session_interface_get_observer(c_network_observer **observer)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    *observer = (c_network_observer *)network_life_cycle_globals_get()->manager.m_observer;
    result = true;
  }
  return result;
}

struct s_network_http_buffer_url;

// 0x8a348
// match: 0x498c20: without __restrict the caller handle_global_start_button_press reloads controller->user_index (lwz 16(r31)) after this call
bool network_session_interface_get_local_user_identifier(long index, s_player_identifier *__restrict key, bool any_state)
{
  bool result = any_state ? g_network_session_interface_globals.users[index].state != 0 : g_network_session_interface_globals.users[index].state == 1;
  if (result)
    memcpy(key, g_network_session_interface_globals.users[index].key, sizeof(g_network_session_interface_globals.users[index].key));
  return result;
}

// 0x8b790
extern "C" long network_squad_session_get_peer_count(void)
{
  long peer_count = 0;
  network_squad_session_get_membership(0, 0, 0, 0, &peer_count, 0, 0, 0, 0, 0);
  return peer_count;
}

// 0x8b7e8
extern "C" bool network_squad_session_get_membership(long *a, long *local_peer_index, long *c, long *leader_peer_index, long *peer_count,
    unsigned long *peer_valid_mask, const s_network_session_peer **peers, long *player_count, unsigned long *player_valid_mask,
    const s_network_session_player **players)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_squad_session(&session))
    result = network_session_get_membership((const s_network_session *)session, 0, local_peer_index, 0, leader_peer_index, peer_count,
        peer_valid_mask, peers, player_count, player_valid_mask, players);
  return result;
}

// 0x8dc48
enum e_gui_game_mode network_life_cycle_squad_session_get_ui_game_mode(void)
{
  s_network_session *session = 0;
  long result = -1;
  if (network_life_cycle_in_squad_session(&session)) {
    if (session->established()) {
      const s_session_membership *settings = network_session_interface_parameters(session);
      if (settings->ui_game_mode.m_flags & 1)
        result = *block_get_limits(&settings->ui_game_mode);
    }
  }
  return (e_gui_game_mode)result;
}

// 0x1cee08
bool map_variant_is_valid(const s_map_variant_validation *variant_view)
{
  const c_map_variant *variant = (const c_map_variant *)variant_view;
  bool valid = true;
  if (variant->version != 12)
    return false;
  if (variant->map_id != -1) {
    if (!variant->built_in && game_is_map_active() && tls_game_globals()->options.game_mode != 3) {
      unsigned long checksum = crc_checksum_buffer(-1, g_cache_file_globals.header.signature, sizeof(g_cache_file_globals.header.signature));
      if ((long)checksum != (long)variant->m_original_map_signature_hash)
        valid = false;
    }
    scenario *current_scenario = global_scenario;
    if (current_scenario && current_scenario->map_id == variant->map_id) {
      short count = 0;
      for (long object_type = 0; object_type < k_object_type_count; object_type++) {
        object_type_definition *type = object_type_definitions[object_type];
        if (type->scenario_datums_offset != -1 && type->scenario_palette_offset != -1) {
          long first = variant->object_type_start[object_type];
          // match: 0x1cee08: without volatile the 0xffff palette checks (lhz 0xc) move after the first-index load
          long offset = ((const volatile object_type_definition *)type)->scenario_datums_offset;
          if (first != -1) {
            long object_count = *(long *)((char *)current_scenario + offset);
            if (object_count > 0) {
              if (first != count) {
                valid = false;
                break;
              }
              count += object_count;
            }
          }
        }
      }
      if (count != variant->scenario_object_count)
        return false;
    }
  }
  return valid;
}

// 0x3a2f98
template <>
bool c_generic_network_session_parameter<s_network_session_parameter_synchronous_out_of_sync>::set(
    // match: 0x3a2f98: without __restrict the parameter byte load (lbz 0(r30)) is hoisted above the m_flags load (lwz 0x28(r31))
    const s_network_session_parameter_synchronous_out_of_sync *__restrict parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}

// 0x3b08f8
bool network_life_cycle_in_matchmaking(void)
{
  long mode;
  long *mode_out = &mode;
  *mode_out = current_mode();
  return mode == 7 || mode == 8 || mode == 9 || mode == 10 || mode == 11 || mode == 12 || mode == 13 || mode == 14 || mode == 15 || mode == 16 || mode == 17;
}

// 0x3b0980
e_network_join_refuse_reason network_squad_session_is_joinable(void)
{
  e_network_join_refuse_reason reason = _network_join_refuse_reason_none;
  s_network_session *session = 0;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    if (network_life_cycle_in_matchmaking())
      reason = _network_join_refuse_reason_in_matchmaking;
    else if (session->m_session_membership.m_current_membership.player_count > network_session_get_maximum_player_count(session))
      reason = _network_join_refuse_reason_squad_full;
  }
  return reason;
}

// @address 0x3b0a50 network_squad_session_get_host_player_index
extern "C" long network_squad_session_get_host_player_index(void)
{
  s_network_session *session = 0;
  long player_index = -1;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const c_network_session_membership *membership = &session->m_session_membership;
    long host_peer_index = membership->m_current_membership.host_peer_index;
    for (long index = 0; index < 16; index++) {
      if (membership->is_player_valid(index)
          && membership->m_current_membership.players[index].peer_index == host_peer_index) {
        player_index = index;
        break;
      }
    }
  }
  return player_index;
}

// 0x3b0b28
long network_squad_session_get_maximum_player_count(void)
{
  long result = 16;
  s_network_session *session = 0;
  if (network_session_try_get(&session))
    result = network_session_get_maximum_player_count(session);
  return result;
}

// 0x3b0b80
bool network_squad_session_set_maximum_player_count(long maximum_player_count)
{
    bool result = false;
    s_network_session *session = 0;
    if (network_life_cycle_in_squad_session(&session)) {
        if (maximum_player_count >= session->m_session_membership.m_current_membership.player_count) {
            c_network_session_parameter_session_size *parameter = &network_session_interface_parameters(session)->session_size;
            result = parameter->request_set_max_player_count(maximum_player_count);
        }
    }
    return result;
}

// 0x3b0c68
long network_group_session_set_disable_host_election(long disable_host_election)
{
  long result = 0;
  c_network_session *session = 0;
  if (network_session_try_get(&session)) {
    network_squad_session_set_privacy_mode(session, -1, disable_host_election, -1, -1);
    result = 1;
  }
  return result;
}

// 0x3b0cf0
static __forceinline bool network_session_interface_session_is_established(const s_network_session *session)
{
  switch (session->m_local_state) {
  case 0: case 1: case 2: case 3:
    return false;
  default:
    return true;
  }
}

bool network_squad_session_get_matchmaking_tip(long a, long index)
{
  bool result = false;
  s_network_session *session = 0;
  if (network_life_cycle_in_squad_session(&session) && network_session_interface_session_is_established(session)) {
    const c_network_session_membership *membership = &session->m_session_membership;
    s_session_membership *parameters = network_session_interface_parameters(session);
    long peer = -1;
    if (index != -1)
      peer = membership->get_peer_from_observer_channel(index);
    s_peer_request local;
    s_peer_request &request = local;
    request.a = a;
    if (peer == -1)
      peer = membership->m_local_peer_index;
    request.peer = peer;
    result = parameters->parameter_62938.request_change(&request);
  }
  return result;
}

// 0x3b0e20
static __forceinline bool network_session_interface_in_group_session(s_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    s_network_session *current = (s_network_session *)network_life_cycle_globals_get()->manager.m_group_session;
    if (!current->disconnected()) {
      if (session)
        *session = current;
      result = true;
    }
  }
  return result;
}
extern "C" bool network_group_session_get_membership(long *a, long *local_peer_index, long *c, long *d, long *e, unsigned long *f,
    const s_network_session_peer **g, long *player_count, unsigned long *player_valid_mask, const s_network_session_player **players)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_session_interface_in_group_session(&session))
    result = network_session_get_membership((const s_network_session *)session, 0, local_peer_index, 0, 0, 0, 0, 0, player_count,
        player_valid_mask, players);
  return result;
}

// 0x3b0ed8
bool network_squad_session_can_set_game_settings(void)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_session_try_get(&session) && session->established()
      && session->get_session_membership()->is_leader() && network_session_interface_parameters(session)->session_mode.get_allowed()
        && network_session_interface_parameters(session)->session_mode.m_data.mode == 1)
    result = true;
  return result;
}


// 0x3b0fa0
bool network_squad_session_set_coop_game_options(
    e_campaign_id campaign_id, e_map_id map_id, const char *scenario_path, short insertion_point, const void *campaign_options)
{
  bool result = false;
  s_network_session *session = 0;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    if (session->get_session_membership()->is_leader()) {
      s_session_membership *parameters = network_session_interface_parameters(session);
      s_network_session_parameter_map map;
      memset(&map, 0, sizeof(map));
      map.campaign_id = campaign_id;
      map.map_id = map_id;
      memcpy(map.campaign_players, campaign_options, sizeof(map.campaign_players));
      strncpy(map.scenario_path, scenario_path, sizeof(map.scenario_path));
      map.scenario_path[sizeof(map.scenario_path) - 1] = 0;
      if (((c_generic_network_session_parameter<s_network_session_parameter_map> *)&parameters->map)->request_change(&map)) {
        ((c_generic_network_session_parameter<short> *)&parameters->coop_insertion_point)->request_change(&insertion_point);
        result = true;
      }
    } else {
      result = true;
    }
  }
  return result;
}

// 0x3b11c0
// match: 0x3b11c0: without __restrict the two -1 stores sink below the g_network_life_cycle_globals address load
void network_squad_session_get_map(e_campaign_id *__restrict campaign_id, e_map_id *__restrict map_id, char *name)
{
  name[0] = 0;
  *campaign_id = _campaign_id_none;
  *map_id = _map_id_none;
  s_network_session *session = 0;
  if (network_life_cycle_in_squad_session(&session)) {
    const s_network_session_parameter_map *data = membership_slot_get_data(&network_session_interface_parameters(session)->map);
    if (data) {
      *campaign_id = data->campaign_id;
      *map_id = data->map_id;
      strncpy(name, data->scenario_path, 128);
      name[127] = 0;
    }
  }
}

// 0x3b1298
bool network_squad_session_set_map(e_campaign_id campaign_id, e_map_id map_id, const char *scenario_path)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    s_session_membership *parameters = network_session_interface_parameters(session);
    s_network_session_parameter_map map;
    memset(&map, 0, sizeof(map));
    map.campaign_id = campaign_id;
    map.map_id = map_id;
    strncpy(map.scenario_path, scenario_path, sizeof(map.scenario_path));
    map.scenario_path[sizeof(map.scenario_path) - 1] = 0;
    bool success = ((c_generic_network_session_parameter<s_network_session_parameter_map> *)&parameters->map)->request_change(&map);
    result = success &&
        parameters->countdown_timer.request_change(false, 0,
            _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

__forceinline bool c_network_session_parameter_ui_game_mode::request_change(e_gui_game_mode ui_game_mode)
{
  bool result = false;
  if (this->set_allowed())
    result = set(ui_game_mode);
  else if (this->change_request_allowed()) {
    m_requested_data = ui_game_mode;
    m_flags |= 2;
    notify_set_change_request();
    result = true;
  }
  return result;
}

// @address 0x3b1390 network_squad_session_set_ui_game_mode
extern "C" bool network_squad_session_set_ui_game_mode(e_gui_game_mode ui_game_mode)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    if (network_life_cycle_in_squad_session(&session))
      result = ((c_network_session_parameter_ui_game_mode *)&network_session_interface_parameters(session)->ui_game_mode)->request_change(
        ui_game_mode);
  }
  return result;
}

// 0x3b1480
bool network_squad_session_set_map_variant(const c_map_variant *map_variant)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = network_session_interface_parameters(session)->map_variant.request_change(map_variant);
  }
  return result;
}

// 0x3b15a0

static __forceinline bool network_session_interface_game_variant_request_change(
    c_network_session_parameter_game_variant *parameter, const s_network_session_interface_game_variant *game_variant)
{
    bool result = false;
    if (parameter->set_allowed())
        result = parameter->set(game_variant);
    else if (parameter->change_request_allowed()) {
        if (game_variant == 0) {
            parameter->m_requested_data.valid = false;
            memset(&parameter->m_requested_data.game_variant, 0, sizeof(parameter->m_requested_data.game_variant));
        } else {
            parameter->m_requested_data.valid = true;
            parameter->m_requested_data.game_variant.copy_from_and_validate(game_variant);
        }
        parameter->set_desires_change_request();
        result = true;
    }
    return result;
}

// @address 0x3b15a0 network_squad_session_set_game_variant
extern "C" bool network_squad_session_set_game_variant(const s_network_session_interface_game_variant *game_variant)
{
    bool result = false;
    if (network_squad_session_can_set_game_settings()) {
        s_network_session *session = 0;
        network_life_cycle_in_squad_session(&session);
        s_session_membership *parameters = network_session_interface_parameters(session);
        long mode;
        if (session->session_class() == 0)
            mode = 0;
        else
            mode = game_variant ? 2 : 1;
        bool success = network_session_interface_game_variant_request_change(
            &parameters->game_variant, game_variant)
            &&((c_generic_network_session_parameter<e_network_game_simulation_protocol> *)&parameters->simulation_protocol)
                  ->request_change((const e_network_game_simulation_protocol *)&mode);
        result = success &&
            network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
                _network_game_countdown_delayed_reason_none,
                0);
    }
    return result;
}

// 0x3b1740
bool network_squad_session_set_film(const s_saved_film_description *film)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    s_session_membership *parameters = network_session_interface_parameters(session);
    bool success = ((c_generic_network_session_parameter<s_saved_film_description> *)&parameters->film)->request_change(film);
    result = success &
        parameters->countdown_timer.request_change(false, 0,
            _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// 0x3b1850

static __forceinline bool network_squad_session_request_game_options(
    c_network_session_parameter_saved_film_game_options *parameter, long a, long b, const void *game_options)
{
  bool result = false;
  if (parameter->set_allowed())
    result = network_session_game_options_set(parameter, a, b, game_options);
  else if (parameter->change_request_allowed()) {
    parameter->m_requested_data.length_in_ticks = a;
    parameter->m_requested_data.start_ticks = b;
    memcpy(&parameter->m_requested_data.options, game_options, sizeof(parameter->m_requested_data.options));
    parameter->set_desires_change_request();
    result = true;
  }
  return result;
}

extern "C" bool network_squad_session_set_game_options(long a, long b, const void *game_options)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = network_squad_session_request_game_options(&network_session_interface_parameters(session)->game_options, a, b, game_options);
  }
  return result;
}

// @address 0x3b1970 network_squad_session_get_campaign_difficulty
extern "C" long network_squad_session_get_campaign_difficulty(void)
{
  s_network_session *session = 0;
  long result = 1;
  if (network_life_cycle_in_squad_session(&session)) {
    const long *value = network_session_interface_parameters(session)->difficulty.get();
    if (value)
      result = *value;
  }
  return result;
}

// 0x3b19e8
bool network_squad_session_set_campaign_difficulty(e_campaign_difficulty_level campaign_difficulty)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    bool success = ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&network_session_interface_parameters(
        session)->difficulty)->request_change(&campaign_difficulty);
    result = success &&
        network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
            _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// @address 0x3b1b00 network_squad_session_set_primary_skulls
extern "C" bool network_squad_session_set_primary_skulls(long value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = network_session_interface_parameters(session)->campaign_active_primary_skulls
        .request_change((const e_network_game_simulation_protocol *)&value)
      &&network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
          _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// @address 0x3b1bd8 network_squad_session_set_secondary_skulls
extern "C" bool network_squad_session_set_secondary_skulls(long value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = network_session_interface_parameters(session)->campaign_active_secondary_skulls
        .request_change((const e_network_game_simulation_protocol *)&value)
      &&network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
          _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// @address 0x3b1cb0 network_squad_session_set_campaign_metagame_scoring
extern "C" bool network_squad_session_set_campaign_metagame_scoring(short value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = ((c_generic_network_session_parameter<short> *)&network_session_interface_parameters(
        session)->campaign_metagame_scoring)->request_change(&value)
      &&network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
          _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// @address 0x3b1d88 network_squad_session_set_campaign_insertion_point
extern "C" bool network_squad_session_set_campaign_insertion_point(short value)
{
  bool result = false;
  if (network_squad_session_can_set_game_settings()) {
    s_network_session *session = 0;
    network_life_cycle_in_squad_session(&session);
    result = ((c_generic_network_session_parameter<short> *)&network_session_interface_parameters(
        session)->coop_insertion_point)->request_change(&value)
      &&network_session_interface_parameters(session)->countdown_timer.request_change(false, 0,
          _network_game_countdown_delayed_reason_none, 0);
  }
  return result;
}

// 0x3b1e60
enum e_network_game_start_mode network_squad_session_get_start_mode(void)
{
  s_network_session *session = 0;
  long result = 0;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const s_session_membership *membership = network_session_interface_parameters(session);
    if (membership->start_mode.m_flags & 1)
      result = *membership->start_mode.get();
  }
  return (e_network_game_start_mode)result;
}

// 0x3b2060
enum e_network_game_privacy network_squad_session_get_privacy_mode(void)
{
  s_network_session *session = 0;
  long result = 0;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const s_session_membership *membership = network_session_interface_parameters(session);
    if (membership->privacy_mode.m_flags & 1)
      result = membership->privacy_mode.get()->privacy_mode;
  }
  return (e_network_game_privacy)result;
}

// 0x3b21e0
extern "C" bool network_squad_session_delegate_leadership(long peer_index)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_squad_session(&session)) {
    long local_peer_index;
    long leader_peer_index;
    long peer_count;
    unsigned long peer_valid_mask;
    const s_network_session_peer *peers;
    unsigned long peer_mask;
    const s_network_session_player *players;
    if (network_squad_session_get_membership(0, &local_peer_index, 0, &leader_peer_index, &peer_count, &peer_valid_mask, &peers, 0,
        &peer_mask, &players)
        && local_peer_index == leader_peer_index && (peer_mask & (1 << peer_index))) {
      long player_index = players[peer_index].peer_index;
      if (player_index != local_peer_index &&
          ((c_network_session *)session)->leader_request_delegate_leadership(( s_transport_secure_address const *)(&peers[player_index])))
        result = true;
    }
  }
  return result;
}

// 0x3b23c8
extern "C" bool network_squad_session_boot_player(long peer_index)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_squad_session(&session)) {
    long local_peer_index;
    long leader_peer_index;
    long peer_count;
    unsigned long peer_valid_mask;
    const s_network_session_peer *peers;
    unsigned long peer_mask;
    const s_network_session_player *players;
    if (network_squad_session_get_membership(0, &local_peer_index, 0, &leader_peer_index, &peer_count, &peer_valid_mask, &peers, 0,
        &peer_mask, &players)
        && local_peer_index == leader_peer_index && (peer_mask & (1 << peer_index))) {
      long player_index = players[peer_index].peer_index;
      if (player_index != local_peer_index &&
          ((c_network_session *)session)->leader_request_boot_machine(( s_transport_secure_address const *)(&peers[player_index]),
        ( e_network_session_boot_reason)(4)))
        result = true;
    }
  }
  return result;
}

// 0x3b24d8
void network_session_calculate_peer_connectivity(c_network_session *session, s_network_session_peer_connectivity *connectivity)
{
  long latencies[16];
  long latency_count = 0;
  long latency_maximum = (-2147483647 - 1), latency_minimum = 2147483647;
  c_network_observer *observer = 0;
  network_session_interface_get_observer(&observer);
  const c_network_session_membership *membership = &session->m_session_membership;
  unsigned long local_probe_mask = membership->m_current_membership.peers[membership->m_local_peer_index].properties.connectivity.peer_probe_mask;
  memset(connectivity, 0, sizeof(*connectivity));
  for (long peer_index = membership->get_first_peer(); peer_index != -1;
      peer_index = membership->get_next_peer(peer_index))
  {
    bool connectable = false, probed = false;
    if (peer_index == membership->m_local_peer_index) {
      connectable = true;
      probed = true;
    } else {
      long observer_channel_index = session->get_peer_observer_channel(peer_index);
      if (observer_channel_index != -1) {
        long state = observer->m_channel_observers[observer_channel_index].state;
        if (network_session_interface_channel_connected(state) || state == 1) {
          probed = true;
          if ((!((const c_network_session *)session)->is_host() && state == 7) ||
              (session->is_host() && state != 1))
            connectable = true;
        }
        bool complete;
        s_transport_qos_result qos_result;
        if (network_session_interface_channel_get_qos_result(&observer->m_channel_observers[observer_channel_index], &complete,
        &qos_result) && complete &&
            qos_result.ping_msec_median != 65535)
        {
          long latency = PIN(qos_result.ping_msec_median, 50, 2000);
          latencies[latency_count++] = latency;
          latency_maximum = MAX(latency_maximum, latency);
          if (latency_minimum > latency)
            latency_minimum = latency;
        }
      }
    }
    if (connectable)
      connectivity->peer_connectivity_mask |= 1 << peer_index;
    if (probed || (local_probe_mask & (1 << peer_index)))
      connectivity->peer_probe_mask |= 1 << peer_index;
  }
  if (latency_count > 0) {
    connectivity->latency_minimum_msec = latency_minimum;
    connectivity->latency_maximum_msec = latency_maximum;
    connectivity->latency_average_msec = network_bandwidth_compute_average(latency_count, latencies);
  }
}

// 0x3b29e0
static __forceinline void network_session_interface_copy_wide_string(wchar_t *destination, const wchar_t *source, long count)
{
  for (long i = 0; i <= count - 1; i++) {
    if ((destination[i] = *source) == 0)
      break;
    source++;
  }
  destination[count - 1] = 0;
}

// @address 0x3b29e0 network_session_interface_set_player_and_session_names
extern "C" void network_session_interface_set_player_and_session_names(const wchar_t *name, const wchar_t *description)
{
  network_session_interface_copy_wide_string(g_network_session_interface_globals.peer.name, name, 16);
  network_session_interface_copy_wide_string(g_network_session_interface_globals.peer.description, description, 32);
}

// 0x3b2ba8
long network_session_interface_get_user_team_index(long user_index)
{
  long team_index = -1;
  const s_network_session_interface_user_record *user = &g_network_session_interface_globals.users[user_index];
  if (user->desired_team_index != -1) {
    team_index = user->desired_team_index;
  } else {
    s_network_session *session;
    if (network_life_cycle_in_squad_session(&session) && session->established()) {
      const c_network_session_membership *membership = &session->m_session_membership;
      long player_index = membership->m_current_membership.peers[membership->m_local_peer_index].user_player_indices[user_index];
      if (player_index != -1)
        team_index = membership->m_current_membership.players[player_index].configuration.host.team_index;
    }
  }
  return team_index;
}

// 0x3b2c80
bool network_session_interface_get_local_user_properties(
    long user_index, e_controller_index *controller_index, s_player_configuration *configuration, unsigned long *voice_settings)
{
    bool result = false;
    if ((long)g_network_session_interface_globals.users[user_index].state == 1) {
        if (controller_index)
            *controller_index = (e_controller_index)g_network_session_interface_globals.users[user_index].controller_index;
        if (configuration)
            memcpy(configuration, &g_network_session_interface_globals.users[user_index].configuration, sizeof(s_player_configuration));
        if (voice_settings)
            *voice_settings = g_network_session_interface_globals.users[user_index].voice_settings;
        result = true;
    }
    return result;
}

// @address 0x3b2d08 ?network_session_interface_set_local_user_properties@@YAXJW4e_controller_index@@PBUs_player_configuration@@K@Z
void network_session_interface_set_local_user_properties(long index, e_controller_index b, const s_player_configuration *data, unsigned long c)
{
  if (memcmp(&g_network_session_interface_globals.users[index].configuration, data,
        sizeof(g_network_session_interface_globals.users[index].configuration)) != 0
      || g_network_session_interface_globals.users[index].controller_index != b || g_network_session_interface_globals.users[index].voice_settings != c) {
    g_network_session_interface_globals.users[index].controller_index = b;
    g_network_session_interface_globals.users[index].change_count++;
    memcpy(&g_network_session_interface_globals.users[index].configuration, data,
        sizeof(g_network_session_interface_globals.users[index].configuration));
    g_network_session_interface_globals.users[index].voice_settings = c;
  }
}

// @address 0x3b2db8 network_session_interface_get_network_quality
extern "C" bool network_session_interface_get_network_quality(
    // match: 0x3b2db8: pointer __restrict qualifier prevents alias reload
    s_network_quality_data *quality, long *__restrict nat_type, long *__restrict value_a, long *__restrict value_b)
{
  bool result = false;
  if (g_network_session_interface_globals.peer.quality_valid) {
    *quality = g_network_session_interface_globals.peer.quality;
    *nat_type = online_get_nat_type();
    *value_a = g_network_session_interface_globals.peer.nat_flags_a;
    *value_b = g_network_session_interface_globals.peer.nat_flags_b;
    result = true;
  }
  return result;
}

// 0x3b2e38
bool network_squad_session_set_countdown_timer(bool delayed, long countdown_timer, e_network_game_countdown_delayed_reason delayed_reason,
        long user_index)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    if ((long)g_network_session_interface_globals.users[user_index].state == 1) {
      if (network_session_interface_parameters(session)->countdown_timer.request_change(delayed, countdown_timer,
              delayed_reason,
              (const s_player_identifier *)g_network_session_interface_globals.users[user_index].key))
        result = true;
    }
  }
  return result;
}

// 0x3b2f20
long network_squad_session_get_countdown_timer(void)
{
  s_network_session *session = 0;
  long result = -1;
  if (network_life_cycle_in_squad_session(&session) && session->established())
    result = membership_slot_get_valid_value(&network_session_interface_parameters(session)->countdown_timer);
  return result;
}

// 0x3b2fc8
extern "C" long network_squad_session_get_countdown_delayed_culprit(void)
{
  s_network_session *session = 0;
  long culprit_index = -1;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const s_player_identifier *culprit = countdown_timer_get_delayed_culprit((
        const c_generic_network_session_parameter<s_network_session_parameter_countdown_timer> *)&network_session_interface_parameters(
        session)->countdown_timer);
    if (culprit) {
      const c_network_session_membership *membership = &session->m_session_membership;
      for (long peer_index = membership->get_first_player(); peer_index != -1;
          peer_index = membership->get_next_player(peer_index))
      {
        if (memcmp(&membership->m_current_membership.players[peer_index].identifier, culprit, sizeof(s_player_identifier)) == 0) {
          culprit_index = peer_index;
          break;
        }
      }
    }
  }
  return culprit_index;
}

// 0x3b3110
e_network_session_class network_squad_session_get_session_class(void)
{
  s_network_session *session = 0;
  e_network_session_class session_class = _network_session_class_none;
  if (network_life_cycle_in_squad_session(&session)) {
    if (session->established())
      session_class = session->session_class();
  }
  return session_class;
}

static __forceinline bool network_session_interface_session_established(const s_network_session *session)
{
  return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}

static __forceinline bool network_session_interface_peer_visible(const c_network_session_membership *membership, long peer_index)
{
  long state = membership->m_current_membership.peers[peer_index].connection_state;
  return state == 8 || state == 1;
}

// 0x3b3608
typedef const s_player_identifier *(*network_session_interface_identifier_function)(long);
network_session_interface_identifier_function network_session_interface_identifier_provider()
{
  return user_interface_session_is_local_player;
}

extern "C" bool network_session_get_membership(const s_network_session *session_view, long *update_number, long *local_peer_index,
    long *host_peer_index, long *leader_peer_index, long *peer_count, unsigned long *peer_valid_mask, const s_network_session_peer **peers,
    long *player_count, unsigned long *player_valid_mask, const s_network_session_player **players)
{
  const s_network_session *session = session_view;
  bool result = false;
  if (network_session_interface_session_established(session)) {
    const c_network_session_membership *membership = &session->m_session_membership;
    if (local_peer_index)
      *local_peer_index = membership->m_local_peer_index;
    if (leader_peer_index)
      *leader_peer_index = membership->m_current_membership.leader_peer_index;
    if (peers)
      *peers = membership->m_current_membership.peers;
    if (players)
      *players = membership->m_current_membership.players;
    if (peer_count || peer_valid_mask || player_count || player_valid_mask) {
      long current_peer_count = membership->m_current_membership.peer_count;
      unsigned long current_peer_valid_mask = membership->m_current_membership.valid_peer_mask.get_bits_direct()[0];
      long current_player_count = membership->m_current_membership.player_count;
      unsigned long current_player_valid_mask = membership->m_current_membership.valid_player_mask.get_bits_direct()[0];
      for (long peer_index = membership->get_first_peer(); peer_index != -1;
          peer_index = membership->get_next_peer(peer_index))
      {
        if (!network_session_interface_peer_visible(membership, peer_index) && peer_index != membership->m_local_peer_index) {
          const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
          current_peer_count--;
          current_peer_valid_mask &= ~(1 << peer_index);
          for (unsigned long player = 0; player < 4; player++) {
            long player_index = peer->user_player_indices[player];
            if (player_index != -1) {
              current_player_count--;
              current_player_valid_mask &= ~(1 << player_index);
            }
          }
        }
      }
      if (peer_count)
        *peer_count = current_peer_count;
      if (peer_valid_mask)
        *peer_valid_mask = current_peer_valid_mask;
      if (player_count)
        *player_count = current_player_count;
      if (player_valid_mask)
        *player_valid_mask = current_player_valid_mask;
    }
    result = true;
  }
  return result;
}

// 0x3b37e0
void network_session_set_player_failure_reason(long player_index, e_network_join_refuse_reason reason)
{
    long *failure_reason = &g_network_session_interface_globals.users[player_index].state;
    switch (reason) {
    case _network_join_refuse_reason_player_not_online_enabled: *failure_reason = 3; break;
    case _network_join_refuse_reason_alternate_host: *failure_reason = 4; break;
    case _network_join_refuse_reason_squad_full: *failure_reason = 9; break;
    case 8: *failure_reason = 10; break;
    case _network_join_refuse_reason_in_matchmaking: *failure_reason = 6; break;
    case _network_join_refuse_reason_local_host: *failure_reason = 8; break;
    case _network_join_refuse_reason_still_connected: *failure_reason = 7; break;
    default: *failure_reason = 5; break;
    }
}

// 0x3b4e98
bool network_life_cycle_session_get_map(e_campaign_id *campaign_id, e_map_id *map_id)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_session(&session) && session->established()) {
    const s_session_membership *parameters = network_session_interface_parameters(session);
    if (parameters->map.m_flags & 1) {
      const s_network_session_parameter_map *map = membership_slot_get_data(&parameters->map);
      *campaign_id = (e_campaign_id)map->campaign_id;
      *map_id = (e_map_id)map->map_id;
      result = true;
    }
  }
  return result;
}

static __forceinline unsigned long network_session_interface_peer_mask_to_player_mask(long peer_mask)
{
    s_network_session *session;
    unsigned long player_mask = 0;
    if (network_life_cycle_in_session(&session) && session->established()) {
        const c_network_session_membership *membership = session->get_session_membership();
        for (long peer_index = 0; peer_mask != 0; peer_index++, peer_mask >>= 1) {
            if (peer_mask & 1) {
                const s_network_session_peer *peer = membership->get_peer(peer_index);
                for (long player = 0; player < 4; player++) {
                    long player_index = peer->user_player_indices[player];
                    if (player_index != -1)
                        player_mask |= 1 << player_index;
                }
            }
        }
    }
    return player_mask;
}

// 0x3b4f60
extern "C" long network_life_cycle_session_get_map_load_status(long map_id, long *out_progress, unsigned long *out_player_mask, long unused)
{
    s_network_session *session = 0;
    long status = 0;
    long progress = 0;
    long peers_not_ready = 0;
    long peers_error = 0;
    if (network_life_cycle_in_session(&session) && session->established()) {
        const c_network_session_membership *membership = session->get_session_membership();
        status = 4;
        progress = 100;
        for (long peer_index = membership->get_first_peer(); peer_index != -1;
            peer_index = membership->get_next_peer(peer_index))
        {
            const s_network_session_peer *peer = membership->get_peer(peer_index);
            long peer_status = peer->properties.map_status;
            long peer_progress = peer->properties.map_progress;
            if (peer->properties.map_id != map_id) {
                peer_status = 0;
                peer_progress = 0;
            }
            if (peer_status == 0 || peer_status == 2) {
                if (progress > peer_progress)
                    progress = peer_progress;
                peers_not_ready |= 1 << peer_index;
                if (status != 1 && peer_status < status)
                    status = peer_status;
            } else if (peer_status == 1) {
                status = 1;
                progress = 0;
                peers_error |= 1 << peer_index;
            } else if (peer_status < status) {
                status = peer_status;
            }
        }
    }
    if (out_progress)
        *out_progress = progress;
    if (out_player_mask)
        *out_player_mask = network_session_interface_peer_mask_to_player_mask(peers_error ? peers_error : peers_not_ready);
    return status;
}

// 0x3b5898
template <>
bool c_generic_network_session_parameter<s_network_session_parameter_map>::request_change(const s_network_session_parameter_map *parameter)
{
    bool result = false;
    if (this->set_allowed())
        result = set(parameter);
    else if (this->change_request_allowed()) {
        memcpy(&m_requested_data, parameter, sizeof(m_requested_data));
        set_desires_change_request();
        result = true;
    }
    return result;
}

// 0x3b5930
template <>
bool c_generic_network_session_parameter<e_network_game_simulation_protocol>::request_change(const e_network_game_simulation_protocol *parameter)
{
    bool result = false;
    if (this->set_allowed())
        result = ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)this)->set((const e_campaign_difficulty_level *)parameter);
    else if (this->change_request_allowed()) {
        m_requested_data = *parameter;
        set_desires_change_request();
        result = true;
    }
    return result;
}

// 0x3b59c0
template <>
bool c_generic_network_session_parameter<short>::request_change(const short *parameter)
{
    bool result = false;
    if (this->set_allowed())
        result = set(parameter);
    else if (this->change_request_allowed()) {
        m_requested_data = *parameter;
        set_desires_change_request();
        result = true;
    }
    return result;
}

// @address 0x3b5a50 ?set@?$c_generic_network_session_parameter@Us_network_session_parameter_map@@@@QAA_NPBUs_network_session_parameter_map@@@Z
template <>
bool c_generic_network_session_parameter<s_network_session_parameter_map>::set(const s_network_session_parameter_map *parameter)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, parameter, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// 0x3b5b20
template <>
bool c_generic_network_session_parameter<short>::set(const short *parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}

// 0x3b5bf0
template <>
bool c_generic_network_session_parameter<e_campaign_difficulty_level>::set(const e_campaign_difficulty_level *parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}

// @address 0x3b5cc0 ?set@?$c_generic_network_session_parameter@Us_saved_film_description@@@@QAA_NPBUs_saved_film_description@@@Z
// 0x3b5cc0
template<>
bool c_generic_network_session_parameter<s_saved_film_description>::set(const s_saved_film_description *parameter)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, parameter, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// 0x3b5e68
template <>
bool c_generic_network_session_parameter<s_nsi_session_string>::set(const s_nsi_session_string *parameter)
{
    bool success = false;
    if (set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, parameter, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// 0x3cd980
static __forceinline const s_network_session_privacy_mode *network_session_interface_privacy_current(
    const c_generic_network_session_parameter<s_network_session_privacy_mode> *parameter)
{
  const s_network_session_privacy_mode *result = 0;
  if (parameter->m_flags & 1)
    result = &parameter->m_data;
  return result;
}
static __forceinline bool network_session_interface_privacy_has_requested(
        const c_generic_network_session_parameter<s_network_session_privacy_mode> *parameter)
{
  return (parameter->m_flags >> 1) & 1;
}
static __forceinline const s_network_session_privacy_mode *network_session_interface_privacy_requested(
    const c_generic_network_session_parameter<s_network_session_privacy_mode> *parameter)
{
  const s_network_session_privacy_mode *result = 0;
  if (network_session_interface_privacy_has_requested(parameter))
    result = &parameter->m_requested_data;
  return result;
}
extern "C" void network_squad_session_set_privacy_mode(
    c_network_session *session, long privacy_mode, long closed_mode, long is_closed_by_user, long maximum_player_count)
{
  if (session->m_session_membership.is_leader()) {
    s_session_membership *parameters = network_session_interface_parameters(session);
    s_network_session_privacy_mode value;
    memset(&value, 0, sizeof(value));
    const s_network_session_privacy_mode *current = 0;
    if (parameters->privacy_mode.m_flags & 1) {
      current = network_session_interface_privacy_requested(&parameters->privacy_mode);
      if (!current)
        current = network_session_interface_privacy_current(&parameters->privacy_mode);
    }
    if (is_closed_by_user != -1)
      value.is_closed_by_user = is_closed_by_user != 0;
    else if (current)
      value.is_closed_by_user = current->is_closed_by_user;
    else
      value.is_closed_by_user = false;
    if (privacy_mode != -1)
      value.privacy_mode = (e_network_game_privacy)privacy_mode;
    else
      value.privacy_mode = current ? current->privacy_mode : _network_game_privacy_invite_only;
    if (closed_mode != -1)
      value.closed_mode = closed_mode;
    else if (current)
      value.closed_mode = current->closed_mode;
    else
      value.closed_mode = 0;
    if (maximum_player_count != -1) {
      if (maximum_player_count >= 16)
        maximum_player_count = 0;
      value.maximum_player_count = maximum_player_count;
    } else if (current)
      value.maximum_player_count = current->maximum_player_count;
    else
      value.maximum_player_count = 0;
    c_generic_network_session_parameter<s_network_session_privacy_mode> *parameter = &parameters->privacy_mode;
    if (parameter->set_allowed()) {
      parameter->set(&value);
      return;
    }
    if (parameter->change_request_allowed()) {
      parameter->m_requested_data = value;
      parameter->set_desires_change_request();
    }
  }
}

// 0x3d1de0
template <>
// match: 0x3d1de0: without __restrict the parameter word loads (lwz 0/8/0xc(r31)) reorder around the m_flags load (lwz 0x28(r30))
bool c_generic_network_session_parameter<s_network_session_privacy_mode>::set(const s_network_session_privacy_mode *__restrict parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}

__forceinline void c_network_session_parameter_ui_game_mode::clear_session_parameters_for_ui_mode_transition()
{
    m_session->get_session_parameters()->clear_parameters_with_mask(0xBFFFC7FFF80ui64);
}

// 0x412d08
bool c_network_session_parameter_ui_game_mode::set(e_gui_game_mode ui_game_mode)
{
    bool result = false;
    if (this->set_allowed()) {
        if (m_data != ui_game_mode || !get_allowed()) {
            m_data = ui_game_mode;
            set_update_required();
            clear_session_parameters_for_ui_mode_transition();
        }
        result = true;
    }
    return result;
}

// 0x412dc0
bool c_network_session_parameter_ui_game_mode::set(const e_gui_game_mode *ui_game_mode)
{
    return set(*ui_game_mode);
}

// @address 0x46d7f8 network_life_cycle_session_get_primary_skulls
extern "C" long network_life_cycle_session_get_primary_skulls(void)
{
  s_network_session *session = 0;
  long result = 0;
  if (network_life_cycle_in_session(&session) && session->established()) {
    const s_session_membership *parameters = network_session_interface_parameters(session);
    if (parameters->campaign_active_primary_skulls.m_flags & 1)
      result = *parameters->campaign_active_primary_skulls.get();
  }
  return result;
}

// @address 0x46d8b0 network_life_cycle_session_get_global_map_precache_status
extern "C" long network_life_cycle_session_get_global_map_precache_status(void)
{
  s_network_session *session = 0;
  long result = 0;
  if (network_life_cycle_in_session(&session) && session->established()) {
    const s_session_membership *parameters = network_session_interface_parameters(session);
    if (parameters->campaign_active_secondary_skulls.m_flags & 1)
      result = *parameters->campaign_active_secondary_skulls.get();
  }
  return result;
}

// @address 0x46d968 network_life_cycle_session_get_game_variant
extern "C" long network_life_cycle_session_get_game_variant(void)
{
  s_network_session *session = 0;
  short result = 0;
  if (network_life_cycle_in_session(&session) && session->established()) {
    const s_session_membership *parameters = network_session_interface_parameters(session);
    if (parameters->campaign_metagame_scoring.m_flags & 1)
      result = *parameters->campaign_metagame_scoring.get();
  }
  return result;
}

// 0x495440
unsigned short network_squad_session_get_hopper(void)
{
  s_network_session *session = 0;
  unsigned short result = 0xffff;
  if (network_life_cycle_in_squad_session(&session)) {
    if (session->established()) {
      s_session_membership *membership = network_session_interface_parameters(session);
      if (membership->local_peer.m_flags & 1)
        result = session_peer_value(&membership->local_peer)->hopper_identifier;
    }
  }
  return result;
}

// @address 0x3b22f0 network_squad_session_end_game
extern "C" bool network_squad_session_end_game(void)
{
  bool result = false;
  s_network_session *session = 0;
  if (network_life_cycle_globals_get()->initialized && network_life_cycle_globals_get()->manager.get_current_state() == 3
      && network_life_cycle_in_squad_session(&session) && session->get_session_membership()->is_leader()) {
    bool value = true;
    c_generic_network_session_parameter<bool> *parameter =
        (c_generic_network_session_parameter<bool> *)&network_session_interface_parameters(session)->end_game;
    if (parameter->request_change(&value))
      result = true;
  }
  return result;
}

static __forceinline bool network_session_interface_in_squad_session(s_network_session **out_active_squad_session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    s_network_session *active_squad_session = (s_network_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    const long &state = active_squad_session->current_local_state();
    if (state != 0) {
      if (out_active_squad_session)
        *out_active_squad_session = active_squad_session;
      result = true;
    }
  }
  return result;
}

static __forceinline void network_session_interface_set_desired_team_index(long user_index, long desired_team_index)
{
  s_network_session_interface_user_record *user = &g_network_session_interface_globals.users[user_index];
  s_network_session *active_squad_session;
  if (network_session_interface_in_squad_session(&active_squad_session)
      && session_state_is_established(active_squad_session)) {
    user->desired_team_session_id = active_squad_session->get_session_id();
    user->desired_team_index = desired_team_index;
    user->change_count++;
  }
}

// 0x46f8f8
extern "C" void network_session_interface_set_controller_desired_team(long controller_index, long desired_team_index)
{
  c_controller_interface *controller = &controller_globals().controllers[controller_index];
  if (controller->user_index != -1)
    network_session_interface_set_desired_team_index(controller->user_index, desired_team_index);
}

static __forceinline bool network_session_interface_slot_is_guest_account(unsigned short account_flags)
{
  return (account_flags & 15) == 9 && (account_flags & 192) != 0;
}

typedef char nsi_silver_client_offset[(offsetof(s_player_configuration_from_client, silver) == 0x49) ? 1 : -1];

// @address 0x3b2ad0 network_session_interface_remove_local_user
extern "C" void network_session_interface_remove_local_user(long user_index)
{
  s_network_session_interface_user_record *slot = &g_network_session_interface_globals.users[user_index];
  if (get_alpha_is_internal_beta()
      && !get_alpha_is_external_beta()
      && slot->state == 1
      && g_network_session_interface_globals.users[user_index].configuration.client.silver
      && !network_session_interface_slot_is_guest_account((unsigned short)(slot->configuration.client.xuid >> 48))) {
    ((int)::_internal_halt_render_thread_and_lock_resources("..\\networking\\logic\\network_session_interface.cpp", 0x855));
    XLaunchNewImage(0, 0);
  }
  memset(slot, 0, sizeof(*slot));
  slot->desired_team_index = -1;
}

static __forceinline long network_session_interface_system_time_get(void)
{
  if (g_network_time_globals.enabled)
    return g_network_time_globals.time;
  return GetTickCount();
}

// @address 0x119a488 g_network_frame_rate_update_time
long g_network_frame_rate_update_time;

// 0x3b5630
extern "C" void network_session_interface_update_frame_rate(void)
{
  long last = g_network_frame_rate_update_time;
  if (network_session_interface_system_time_get() - last >= 1000) {
    s_network_session *session = 0;
    if (network_life_cycle_in_squad_session(&session) && session->established() &&
        ((const c_network_session *)session)->is_host())
    {
      long value = simulation_world_compute_frame_rate_tier();
      ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&network_session_interface_parameters(
        session)->campaign_difficulty)->set((const e_campaign_difficulty_level *)&value);
    }
    g_network_frame_rate_update_time = network_session_interface_system_time_get();
  }
}

// @address 0x8086b6 g_network_online_connected
static __forceinline bool network_session_interface_message_in_squad(c_network_session **out)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    c_network_session *session = (c_network_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (session->m_local_state != 0) { if (out) *out = session; result = true; }
  }
  return result;
}
static __forceinline bool network_session_interface_message_in_group(c_network_session **out)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    c_network_session *session = (c_network_session *)network_life_cycle_globals_get()->manager.m_group_session;
    if (session->m_local_state != 0) { if (out) *out = session; result = true; }
  }
  return result;
}
static __forceinline void network_session_interface_online_set_connected(bool connected)
{
  g_network_online_connected = connected;
}
// @address 0x3b2788 ?network_session_interface_handle_message@@YAXW4e_session_network_message@@@Z
void network_session_interface_handle_message(e_session_network_message message)
{
  c_network_session *squad = 0; c_network_session *group = 0;
  const c_network_session_membership *squad_membership = 0; const c_network_session_membership *group_membership = 0;
  long session_class = -1;
  if (network_session_interface_message_in_squad(&squad)) { session_class = squad->session_class(
        ); squad_membership = squad->get_session_membership_unsafe(); }
  if (network_session_interface_message_in_group(&group)) group_membership = group->get_session_membership_unsafe();
  switch (message) {
  case _session_network_message_guide_opened:
    g_network_session_interface_globals.peer.disconnected = false;
    if (squad_membership && squad_membership->get_peer_count() > 1) g_network_session_interface_globals.peer.disconnected = true;
    else if (group_membership && group_membership->get_peer_count() > 1) g_network_session_interface_globals.peer.disconnected = true;
    break;
  // match: 0x3b2788: without it the switch gains a cmplwi 6 / bgt bounds check ahead of its jump table
  default: __assume(0); break;
  case _session_network_message_guide_closed: break;
  case _session_network_message_transport_startup: break;
  case _session_network_message_transport_shutdown:
    network_session_interface_online_set_connected(false);
    if (session_class == 1 || session_class == 2)
    {
        if (group)
            ((c_network_session *)group)->disband_and_reestablish_as_host(true);
        else
            ((c_network_session *)squad)->disband_and_reestablish_as_host(false);
    }
    network_life_cycle_handle_lost_connection();
    break;
  case _session_network_message_online_connected: network_session_interface_online_set_connected(true); break;
  case _session_network_message_online_duplicate_logon:
    network_session_interface_online_set_connected(false);
    if (session_class == 2 && transport_available())
    {
        if (group)
            ((c_network_session *)group)->disband_and_reestablish_as_host(true);
        else
            ((c_network_session *)squad)->disband_and_reestablish_as_host(false);
    }
    network_life_cycle_handle_lost_connection();
    break;
  case _session_network_message_online_disconnected:
    network_session_interface_online_set_connected(false);
    if (session_class == 2)
    {
        if (group)
            ((c_network_session *)group)->disband_and_reestablish_as_host(true);
        else
            ((c_network_session *)squad)->disband_and_reestablish_as_host(false);
    }
    network_life_cycle_handle_lost_connection();
    break;
  }
}

// 0x3b3d28
static __forceinline s_network_session_interface_cache *network_session_interface_cache(void)
{
  return &g_network_session_interface_globals.cache.properties;
}
// @address 0x7fc368 g_transport_security_globals
extern "C" {

// @address 0x59f080 XTLGetLanguage
}

static __forceinline long network_session_interface_content_index(long unique_id)
{
  long result = -1;
  const s_network_configuration_map_entry *entry = g_network_configuration.map_table;
  for (long index = 0; index < 32; index++, entry++) {
    if (entry->map_id == unique_id) {
      result = index;
      break;
    }
  }
  return result;
}
static __forceinline unsigned long network_session_interface_content_mask(void)
{
  long unique_ids[32];
  long count = 32;
  unsigned long mask = 0;
  levels_get_multiplayer_map_ids(unique_ids, &count);
  for (long i = 0; i < count; i++) {
    long index = network_session_interface_content_index(unique_ids[i]);
    if (index != -1)
      mask |= 1 << index;
  }
  return mask;
}
static __forceinline void network_session_interface_secure_address_from_identifier(s_transport_secure_address *address)
{
  memset(address, 0, sizeof(*address));
  for (long i = 0; i < 6; i++)
    address->identifier[i] = g_transport_security_globals.local_identifier[i];
}

// @address 0x3b3d28 ?network_session_update_peer_properties@@YAXPAVc_network_session@@PBUs_network_session_peer@@@Z
void network_session_update_peer_properties(c_network_session *session, const s_network_session_peer *peer)
{
  long update_index = session->session_index();
  s_network_session_interface_cache *cache = network_session_interface_cache();
  long last_update_time = cache->update_time[update_index];
  if (network_session_interface_system_time_get() - last_update_time >=
      g_network_configuration.peer_properties_update_interval)
  {
    bool updated = false;
    s_network_session_peer_properties properties;
    properties.status_flags.set_unsafe(0);
    c_network_observer *observer = 0;
    if (network_life_cycle_globals_get()->initialized)
      observer = (c_network_observer *)network_life_cycle_globals_get()->manager.m_observer;
    memset(&properties, 0, sizeof(properties));
    network_session_interface_copy_wide_string(properties.name, g_network_session_interface_globals.peer.name, 16);
    network_session_interface_copy_wide_string(properties.session_name, g_network_session_interface_globals.peer.description, 32);
    properties.upstream_bandwidth = 0;
    properties.downstream_bandwidth = 0;
    properties.is_peer_flagged = 0;
    properties.nat_type = 0;
    properties.language = get_current_language();
    network_session_calculate_peer_connectivity(session, &properties.connectivity);
    observer->quality_statistics_get_ratings((long *)&properties.connection_quality, &properties.connectivity_badness, &properties.host_badness);
    properties.nat_type = online_get_nat_type();
    if (g_network_session_interface_globals.peer.quality_valid) {
      properties.upstream_bandwidth = g_network_session_interface_globals.peer.quality.downstream_bandwidth;
      if (observer->m_stream_bandwidth_cap_bps != -1) {
        properties.downstream_bandwidth = observer->m_stream_bandwidth_cap_bps;
        properties.is_peer_flagged = observer->m_stream_low_bandwidth;
      }
    }
    properties.has_gamestate_bonus = network_session_interface_has_gamestate_bonus();
    properties.map_content_mask = network_session_interface_content_mask();
    properties.determinism_version = 1;
    properties.determinism_compatible_version = 1;
    properties.hopper_identifier = g_network_session_interface_globals.peer.hopper_identifier;
    properties.hopper_error = g_network_session_interface_globals.peer.hopper_error;
    properties.map_id = g_network_session_interface_globals.peer.map_id;
    properties.map_status = g_network_session_interface_globals.peer.map_status;
    properties.map_progress = g_network_session_interface_globals.peer.map_progress;
    properties.id = g_network_session_interface_globals.game_instance;
    properties.status_flags.set_unsafe(g_network_session_interface_globals.peer.flags);

    const c_network_session_membership *membership = session->get_session_membership_unsafe();
    s_transport_secure_address address;
    if (!transport_secure_address_get(&address))
      network_session_interface_secure_address_from_identifier(&address);
    const s_transport_secure_address *local_address;
    if (membership && membership->has_membership())
      local_address = &membership->m_current_membership.peers[membership->m_local_peer_index].secure_address;
    else
      local_address = &address;
    if (peer->connection_state == 6
        || memcmp(&peer->properties, &properties, sizeof(properties)) != 0
        || memcmp(&address, local_address, sizeof(address)) != 0) {
      if (session->peer_request_properties_update(&address, &properties))
        updated = true;
    }
    if (updated)
      cache->update_time[update_index] = network_session_interface_system_time_get();
  }
}

// 0x3b4450
// @address 0x3b4450 ?network_session_update_user_properties@@YAXPAVc_network_session@@J@Z
void network_session_update_user_properties(c_network_session *session, long user_index)
{
  s_network_session_interface_user_record *user = &g_network_session_interface_globals.users[user_index];
  long update_index = session->session_index();
  if (user->desired_team_index != -1) {
    const s_session_membership *parameters = network_session_interface_parameters(session);
    const c_network_session_membership *membership = &session->m_session_membership;
    bool clear = true;
    if (parameters->game_variant.get_allowed() && (parameters->game_variant.get()->flags & 1))
      clear = false;
    if (!clear) {
      clear = true;
      long player_index = membership->m_current_membership.peers[membership->m_local_peer_index].user_player_indices[user_index];
      if (player_index != -1 && membership->m_current_membership.players[player_index].configuration.host.team_index != user->desired_team_index)
        clear = false;
    }
    if (clear) {
      user->desired_team_index = -1;
      user->change_count++;
    }
  }
  long last_update_time = user->update_time[update_index];
  if (network_session_interface_system_time_get() - last_update_time >=
      g_network_configuration.user_properties_update_interval)
  {
    s_player_configuration_from_client configuration;
    memcpy(&configuration, &user->configuration, sizeof(configuration));
    if (user->desired_team_index != -1 && update_index == user->desired_team_session_id)
      configuration.desired_team_index = (unsigned char)user->desired_team_index;
    if (session->peer_request_player_desired_properties_update(user_index,
            g_network_session_interface_globals.users[user_index].change_count, (e_controller_index)user->controller_index, &configuration,
        user->voice_settings))
      user->update_time[update_index] = network_session_interface_system_time_get();
  }
}

// @address 0x3b4bd0 network_session_update_team_assignments
extern "C" void network_session_update_team_assignments(
    c_network_session *session, bool teams_enabled, unsigned char current_teams_enabled, bool current_flag, long maximum_team_count)
{
  c_network_session_membership *membership = &session->m_session_membership;
  s_network_session_player *players = membership->m_current_membership.players;
  bool changed = false;
  if (!teams_enabled) {
    for (long player_index = membership->get_first_player(); player_index != -1;
        player_index = membership->get_next_player(player_index))
    {
      s_network_session_player *player = &players[player_index];
      if (player_index != player->configuration.host.team_index) {
        changed = true;
        player->configuration.host.team_index = player_index;
      }
    }
  } else {
    long team_player_counts[8] = { 0 };
    bool team_used[8] = { 0 };
    long used_team_count = 0;
    for (long player_index = membership->get_first_player(); player_index != -1;
        player_index = membership->get_next_player(player_index))
    {
      s_network_session_player *player = &players[player_index];
      long desired_team_index = player->configuration.client.desired_team_index;
      long requested_team_index = player->configuration.host.user_team_index;
      if (desired_team_index != -1 && desired_team_index < maximum_team_count) {
        if (requested_team_index != desired_team_index) {
          changed = true;
          player->configuration.host.user_team_index = desired_team_index;
        }
        if (team_player_counts[desired_team_index] == 0)
          used_team_count++;
        team_player_counts[desired_team_index]++;
      } else if (requested_team_index != -1 && requested_team_index < maximum_team_count) {
        if (team_player_counts[desired_team_index] == 0)
          used_team_count++;
        team_player_counts[requested_team_index]++;
      } else {
        changed = true;
        player->configuration.host.user_team_index = -1;
      }
    }
    for (long team_index = 0; used_team_count < 2; team_index++) {
      if (team_player_counts[team_index] == 0) {
        team_used[team_index] = true;
        used_team_count++;
      }
    }
    for (long player_index = membership->get_first_player(); player_index != -1;
        player_index = membership->get_next_player(player_index))
    {
      s_network_session_player *player = &players[player_index];
      if (player->configuration.host.user_team_index == -1) {
        long best_count = 16;
        long best_team_index = -1;
        for (long team_index = 0; team_index < maximum_team_count; team_index++) {
          long count = team_player_counts[team_index];
          if (count < best_count && (count > 0 || team_used[team_index])) {
            best_count = count;
            best_team_index = team_index;
          }
        }
        player->configuration.host.user_team_index = best_team_index;
        team_player_counts[best_team_index]++;
      }
    }
    for (long player_index = membership->get_first_player(); player_index != -1;
        player_index = membership->get_next_player(player_index))
    {
      s_network_session_player *player = &players[player_index];
      if (player->configuration.host.team_index != player->configuration.host.user_team_index) {
        changed = true;
        player->configuration.host.team_index = player->configuration.host.user_team_index;
      }
    }
  }
  if (changed) {
    membership->m_current_membership.update_number++;
    membership->m_local_membership_update_number++;
  }
}

// 0x3b4610
extern "C" {

// @address 0x58f528 wcscmp
}

static __forceinline void network_session_interface_player_name_copy(wchar_t *destination, const wchar_t *source)
{
  long i = 0;
  do {
    if ((destination[i] = *source) == 0)
      break;
    source++;
    i++;
  } while (i <= 16);
}

static __forceinline bool network_session_interface_xuid_is_valid(unsigned short xuid_high)
{
  return (xuid_high & 0xf) == 9 && (xuid_high & 0xc0) != 0;
}

static __forceinline void network_session_interface_clear_player_name(s_player_configuration_from_host *configuration)
{
  // match: clear the leading name through its word-aligned host record.
  memset(configuration, 0, sizeof(configuration->name));
}

// @address 0x3b4610 ?network_session_check_properties@@YAXPAVc_network_session@@@Z
void network_session_check_properties(c_network_session *session)
{
  bool unlocked = false;
  if (global_game_globals && global_game_globals->multiplayer_globals_index != -1) {
    const s_multiplayer_globals_definition *definition =
        TAG_GET('mulg', const s_multiplayer_globals_definition, global_game_globals->multiplayer_globals_index);
    if (definition && definition->universal.count > 0)
      unlocked = true;
  }
  long state = session->m_local_state;
  bool membership_locked = state == 8 || state == 7 || state == 9;
  if (membership_locked || (long)main_globals.recursion_count)
    unlocked = false;
  if (unlocked) {
    s_session_membership *parameters = network_session_interface_parameters(session);
    const s_session_membership *game_parameters = network_session_interface_parameters(session);
    long *membership_update_numbers = g_network_session_interface_globals.cache.properties.membership_update_number;
    unsigned char *teams_enabled_cache = (unsigned char *)g_network_session_interface_globals.cache.properties.teams_enabled;
    unsigned char *observer_allowed_cache = (unsigned char *)g_network_session_interface_globals.cache.properties.observer_allowed;
    long *maximum_team_counts = g_network_session_interface_globals.cache.properties.maximum_team_count;
    long session_index = session->session_index();
    const c_game_variant *variant = 0;
    bool teams_enabled = false;
    long maximum_team_count = 8;
    bool observer_allowed = true;
    bool membership_updated = false;
    if (game_parameters->game_variant.get_allowed())
      variant = game_parameters->game_variant.get();
    if (membership_update_numbers[session_index] != session->m_session_membership.m_current_membership.update_number) {
      c_network_session_membership *membership = &session->m_session_membership;
      const wchar_t *unique_names[16] = { 0 };
      s_network_session_player *players = membership->m_current_membership.players;
      bool changed = false;
      membership_updated = true;
      for (long player_index = membership->get_first_player(); player_index != -1;
          player_index = membership->get_next_player(player_index))
      {
        s_network_session_player *player = &players[player_index];
        if (player->configuration.client.name[0] == 0)
          network_session_interface_clear_player_name(&player->configuration.host);
        else if (player->configuration.host.name[0] != 0)
          unique_names[player_index] = player->configuration.host.name;
      }
      for (long player_index = membership->get_first_player(); player_index != -1;
          player_index = membership->get_next_player(player_index))
      {
        s_network_session_player *player = &players[player_index];
        if (player->configuration.host.name[0] != 0 && wcscmp(player->configuration.client.name, player->configuration.host.name) != 0) {
          bool unique = true;
          if (!network_session_interface_xuid_is_valid((unsigned short)(player->configuration.client.xuid >> 48))) {
            for (long other_index = membership->get_first_player(); other_index != -1;
                other_index = membership->get_next_player(other_index))
            {
              if (unique_names[other_index] != 0 && wcscmp(player->configuration.client.name, unique_names[other_index]) == 0) {
                unique = false;
                break;
              }
            }
          }
          if (unique) {
            network_session_interface_clear_player_name(&player->configuration.host);
            network_session_interface_player_name_copy(player->configuration.host.name, player->configuration.client.name);
            changed = true;
          } else if (!network_player_name_is_random(player->configuration.host.name)) {
            network_get_unique_player_name(player->configuration.host.name, 16, 16, unique_names);
            changed = true;
          }
          unique_names[player_index] = player->configuration.host.name;
        }
      }
      for (long player_index = membership->get_first_player(); player_index != -1;
          player_index = membership->get_next_player(player_index))
      {
        s_network_session_player *player = &players[player_index];
        if (player->configuration.client.name[0] != 0 && player->configuration.host.name[0] == 0) {
          bool unique = true;
          if (!network_session_interface_xuid_is_valid((unsigned short)(player->configuration.client.xuid >> 48))) {
            for (long other_index = membership->get_first_player(); other_index != -1;
                other_index = membership->get_next_player(other_index))
            {
              if (unique_names[other_index] != 0 && wcscmp(player->configuration.client.name, unique_names[other_index]) == 0) {
                unique = false;
                break;
              }
            }
          }
          if (unique) {
            network_session_interface_clear_player_name(&player->configuration.host);
            network_session_interface_player_name_copy(player->configuration.host.name, player->configuration.client.name);
          } else {
            network_get_unique_player_name(player->configuration.host.name, 16, 16, unique_names);
          }
          changed = true;
          unique_names[player_index] = player->configuration.host.name;
        }
      }
      if (changed) {
        membership->m_current_membership.update_number++;
        membership->m_local_membership_update_number++;
      }
      membership_update_numbers[session_index] = session->m_session_membership.m_current_membership.update_number;
    }
    if (session->m_session_type == 1) {
      if (variant) {
        teams_enabled = variant->flags & 1;
        observer_allowed = false;
        if (parameters->map.m_flags & 1) {
          const s_network_session_parameter_map *map = membership_slot_get_data(&parameters->map);
          maximum_team_count = game_engine_variant_get_maximum_team_count(variant, (e_map_id)map->map_id);
        }
        if (maximum_team_count == 0 && teams_enabled)
          return;
      }
      if (membership_updated
          || teams_enabled_cache[session_index] != teams_enabled
          || observer_allowed_cache[session_index] != observer_allowed
          || maximum_team_counts[session_index] != maximum_team_count) {
        network_session_update_team_assignments(session, teams_enabled, teams_enabled_cache[session_index], observer_allowed, maximum_team_count);
        teams_enabled_cache[session_index] = teams_enabled;
        observer_allowed_cache[session_index] = observer_allowed;
        maximum_team_counts[session_index] = maximum_team_count;
      }
    }
  }
}

// 0x3c6338
struct s_network_banhammer_globals;
struct s_network_leaderboard_globals;

static __forceinline unsigned __int64 nsi_player_xuid(const s_network_session_player *player)
{
  return player->configuration.client.xuid;
}

// @address 0x3c6338 network_session_update_team_indices
extern "C" void network_session_update_team_indices(void)
{
  for (long user_index = 0; user_index < 4; user_index++) {
    s_network_leaderboard_query_user_statistics *user = (s_network_leaderboard_query_user_statistics *)&network_leaderboard_globals.users[user_index];
    if (user->valid) {
      switch (user->highest_skill_state) {
      case 0:
        {
        e_hopper_load_status load_status = network_banhammer_loaded_for_controller((e_controller_index)user->controller_index, 0);
        _ReadWriteBarrier();
        if (load_status == _hopper_load_status_loaded) {
          const s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[user->controller_index];
          long highest_skill = 1;
          if (controller->flags & 1)
            highest_skill = controller->highest_skill;
          user->highest_skill = highest_skill;
          user->highest_skill_state = 1;
        }
        }
        break;
      case 1:
        if (user->connection_state == 3) {
          c_network_session *session = 0;
          if (network_life_cycle_try_and_get_squad_session(&session)) {
            bool in_game = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
            if (in_game && session->m_session_class == 2) {
              s_session_membership *parameters = network_session_interface_parameters(session);
              if (parameters->local_peer.m_flags & 1) {
                const s_network_session_matchmaking_hopper *hopper = session_peer_value(&parameters->local_peer);
                if (hopper->is_ranked) {
                  c_network_session_membership *membership = &session->m_session_membership;
                  for (long player_index = membership->get_first_player(); player_index != -1;
                      player_index = membership->get_next_player(player_index))
                  {
                    const s_network_session_player *player = &membership->m_current_membership.players[player_index];
                    if (player->peer_index == membership->m_local_peer_index
                        && nsi_player_xuid(player) == user->xuid
                        && player->configuration.host.hopper_stats_valid
                        && player->configuration.client.hopper_statistics.hopper_identifier == hopper->hopper_identifier
                        && player->configuration.host.hopper_skill > user->highest_skill) {
                      user->highest_skill = player->configuration.host.hopper_skill;
                      user->downloader.reset();
                      user->highest_skill_state = 2;
                    }
                  }
                }
              }
            }
          }
        }
        break;
      case 2:
        {
          s_network_http_buffer_url url;
          url.name[0] = 0;
          url.service_type = 0;
          url.cache_mode = 0;
          url.lifetime_seconds = 600;
          url.queue_type = 0;
          ((c_static_string<256> *)url.name)->print("/gameapi/UserUpdatePlayerStats.ashx?userId=%016I64X&highestSkill=%d", user->xuid, user->highest_skill);
          url.cache_mode = 0;
          url.service_type = 2;
          url.queue_type = 1;
          http_buffer_downloader_set_url(&user->downloader, &url);
          user->downloader.get_data();
          if (network_http_buffer_downloader_get_public_status(user->downloader.m_internal_status) != 1) {
            e_hopper_load_status load_status = network_banhammer_loaded_for_controller((e_controller_index)user->controller_index, 0);
            _ReadWriteBarrier();
            if (load_status != _hopper_load_status_loaded) {
              user->highest_skill_state = 0;
            } else {
              s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[user->controller_index];
              if (controller->flags & 1)
                controller->highest_skill = user->highest_skill;
              user->highest_skill_state = 1;
              if (user->connection_state == 3)
                network_leaderboard_notify_user_stats_updated(user_index);
            }
          }
        }
        break;
      }
    }
  }
}

static __forceinline bool network_session_interface_established_host(const c_network_session *session)
{
  bool host = false;
  long state = session->m_local_state;
  if (state == 6 || state == 7 || state == 8 || state == 9)
    host = true;
  else {
    volatile bool established;
    if ((unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10)
      established = true;
    else
      established = false;
  }
  return host;
}
template<typename T> static __forceinline const T *network_session_interface_parameter_data(const c_generic_network_session_parameter<T> *parameter)
{
  const T *data = 0;
  if (parameter->m_flags & 1)
    data = &parameter->m_data;
  return data;
}
static __forceinline bool network_session_interface_string_matches(const s_session_membership *parameters, const s_nsi_session_string *value)
{
  const s_nsi_session_string *current = network_session_interface_parameter_data(&parameters->session_string);
  return current && strcmp(value->text, current->text) == 0;
}
// @address 0x3b4100 network_session_update_film_and_session_string
extern "C" void network_session_update_film_and_session_string(c_network_session *session)
{
  s_session_membership *parameters = (s_session_membership *)session->get_session_parameters();
  network_session_interface_globals *globals = (network_session_interface_globals *)(&g_network_session_interface_globals);
  if (parameters->film.get_allowed()) {
    const s_nsi_film_description *film = network_session_interface_parameter_data(&parameters->film);
    bool keep_film = false;
    if (film->type == 0 || film->type == 6)
      keep_film = true;
    else if (globals->film_reset_time != -1) {
      long last_reset = globals->film_reset_time;
      long elapsed = network_session_interface_system_time_get() - last_reset;
      if (memcmp(film, &globals->previous_film, sizeof(*film)) == 0)
        keep_film = true;
      else if (elapsed < g_network_configuration.film_reset_interval)
        keep_film = true;
    }
    if (!keep_film) {
      s_nsi_film_description empty;
      memset(&empty, 0, sizeof(empty));
      empty.type = 0;
      empty.valid = 1;
      empty.film_index = -1;
      network_squad_session_set_film((const s_saved_film_description *)&empty);
      globals->film_reset_time = network_session_interface_system_time_get();
    }
  }
  const char *selected = 0;
  s_nsi_session_string value = {};
  for (long user_index = 0; user_index < (long)NUMBEROF(globals->users); user_index++) {
    const s_network_session_interface_user_record *user = &globals->users[user_index];
    if (user->state == 1 && user->text[0]) {
      if (!selected || strcmp(user->text, selected) < 0)
        selected = user->text;
    }
  }
  if (selected)
    strncpy(value.text, selected, sizeof(value.text));
  const s_nsi_session_string *current;
  if (!(parameters->session_string.m_flags & 1) ||
      !(current = network_session_interface_parameter_data(&parameters->session_string)) || strcmp(value.text, current->text) != 0)
  {
    c_generic_network_session_parameter<s_nsi_session_string> *parameter = &parameters->session_string;
    s_network_session *owner = (s_network_session *)parameter->m_session;
    bool set_allowed = owner->established() && network_session_interface_established_host(parameter->m_session);
    if (set_allowed)
      parameter->set(&value);
    else if (parameter->change_request_allowed()) {
      parameter->m_requested_data = value;
      parameter->m_flags |= 2;
      parameter->notify_set_change_request();
    }
  }
}

// @address 0x3b5738 network_session_interface_get_map_load_progress
extern "C" float network_session_interface_get_map_load_progress(long scenario_type, short map_index, const char *path)
{
  long load_status = loading_get_map_load_state(map_index, path);
  float progress = cache_file_get_load_progress(scenario_type, map_index, path);
  s_network_session_interface_map_load *map_load = &g_network_session_interface_globals.map_load;
  float result;
  if (map_load->scenario_type == scenario_type
      && strncmp(map_load->scenario_path, path, sizeof(map_load->scenario_path)) == 0
      && map_load->map_index == map_index
      && map_load->load_status <= load_status) {
    if (progress < 1.0f && map_load->initial_progress < 1.0f) {
      result = (progress - map_load->initial_progress) / (1.0f - map_load->initial_progress);
      result = PIN(result, 0.0f, 1.0f);
    } else {
      result = 1.0f;
    }
  } else {
    map_load->scenario_type = scenario_type;
    strncpy(map_load->scenario_path, path, sizeof(map_load->scenario_path));
    map_load->scenario_path[sizeof(map_load->scenario_path) - 1] = 0;
    if (progress > 0.0f && progress < 1.0f)
      map_load->initial_progress = progress;
    else if ((progress > 0.0f) == false)
      map_load->initial_progress = 0.0f;
    else
      map_load->initial_progress = 1.0f;
    map_load->map_index = map_index;
    result = progress < 1.0f ? 0.0f : 1.0f;
  }
  map_load->load_status = (short)load_status;
  return result;
}

// @address 0x90828 network_session_interface_update
// @address 0x3b3190 ?network_session_interface_update_local_state@@YAXXZ
// @address 0x80a238 g_online_url_name_globals

static __forceinline const char *interface_update_parameter_name(const c_generic_network_session_parameter<s_nsi_session_string> *parameter)
{
  const char *result = 0;
  if (parameter->m_flags & 1)
    result = parameter->m_data.text;
  return result;
}
extern "C" void network_session_interface_update()
{
  network_session_interface_update_local_state();
  for (long i = 0; i < 3; i++) {
    s_network_session *session = g_network_session_interface_globals.sessions[i];
    if (session->m_local_state != 0)
      network_session_update_properties((c_network_session *)session);
  }
  network_session_interface_update_frame_rate();
  s_network_session *session = 0;
  if (network_life_cycle_in_squad_session(&session)) {
    const s_session_membership *membership = network_session_interface_parameters(session);
    if (membership->session_string.m_flags & 1) {
      char name[64];
      strncpy(name, interface_update_parameter_name(&membership->session_string), 64);
      name[63] = 0;
      if (strncmp(name, g_online_url_name_globals.name, 64) != 0) {
        strncpy(g_online_url_name_globals.name, name, 64);
        g_online_url_name_globals.name[63] = 0;
        g_online_url_version++;
      }
    }
  }
}

struct utf8;

static __forceinline bool network_session_interface_parameter_available(unsigned long flags)
{
  unsigned long available = flags & 1;
  // match: 0x90a28: without it flags & 1 is returned directly (clrlwi) instead of the cntlzw/xori boolean form
  __assume(flags >= available);
  return available != 0;
}
static __forceinline bool nsi_game_is_multiplayer(void)
{
  return tls_game_globals()->options.game_mode == 2;
}
static __forceinline const c_game_variant *nsi_current_game_variant(void)
{
  game_options *options = &tls_game_globals()->options;
  return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline void nsi_ustrncpy(wchar_t *dest, const wchar_t *source, long count)
{
  struct network_session_interface_copy_cursor {
    const wchar_t *source;
    long index;
    wchar_t *destination;
  };
  network_session_interface_copy_cursor cursor = { source, 0, dest };
  for (; cursor.index <= count; ++cursor.index, ++cursor.source, ++cursor.destination) {
    if ((*cursor.destination = *cursor.source) == 0)
      break;
  }
}
static __forceinline wchar_t *nsi_ustrnzcpy(wchar_t *dest, const wchar_t *source, long size)
{
  nsi_ustrncpy(dest, source, size - 1);
  dest[size - 1] = 0;
  return dest;
}
static __forceinline char *nsi_csstrnzcpy(char *dest, const char *source, unsigned int size)
{
  strncpy(dest, source, size);
  dest[size - 1] = 0;
  return dest;
}
static __forceinline void nsi_wchar_to_ascii(char *dest, const wchar_t *source, long size)
{
  long remaining = size;
  wchar_t character;
  do {
    character = *source;
    if (remaining == 1)
      *dest = 0;
    else if (character <= 0x7f)
      *dest = (char)character;
    else
      *dest = '?';
    source++;
    dest++;
    remaining--;
  } while (character != 0 && remaining > 0);
}
static __forceinline bool nsi_game_variant_has_teams(const c_game_variant *game_variant)
{
  return (game_variant->flags & 1) != 0;
}
static __forceinline void nsi_get_build_identifiers(long *executable_type, long *executable_version, long *compatible_version)
{
  *executable_type = 5;
  *executable_version = 0x2e4f;
  *compatible_version = k_network_compatible_version;
}
static __forceinline void nsi_game_engine_get_timer(long *timer_type, long *timer_seconds)
{
  long round_limit = 0;
  if (nsi_current_game_variant())
    round_limit = nsi_current_game_variant()->round_limit;
  long rounds_remaining = round_limit - game_engine_get_round();
  if (nsi_current_game_variant()->round_time_limit_minutes * 60 != 0) {
    *timer_type = 2;
    *timer_seconds = MAX(tls_game_engine_globals()->timer_seconds, 0);
  } else {
    *timer_type = 1;
    *timer_seconds = MAX(tls_game_engine_globals()->timer_seconds, 0);
  }
  if (*timer_type == 2 && rounds_remaining > 1)
    *timer_seconds += nsi_current_game_variant()->round_time_limit_minutes * (rounds_remaining - 1) * 60;
}

// @address 0x90a28 ?network_squad_session_build_status@@YA_NPAUs_network_squad_status_data@@@Z
bool network_squad_session_build_status(s_network_squad_status_data *game_status)
{
  s_network_session *session = 0;
  bool result = false;
  if (network_life_cycle_in_squad_session(&session) && session->session_class() != 0 && session->established()) {
    const c_game_variant *game_variant = 0;
    const s_session_membership *parameters = network_session_interface_parameters(session);
    const s_network_session_parameter_map *map = 0;
    const utf8 *hopper_name = 0;
    const wchar_t *session_name = 0;
    const wchar_t *game_variant_name = 0;
    const wchar_t *map_variant_name = 0;
    const wchar_t *film_name = 0;
    bool team_game = false;
    unsigned long occupied_team_mask = 0;
    long player_count = 0;
    const c_network_session_membership *membership = session->get_session_membership();
    session_name = membership->get_peer(membership->m_current_membership.leader_peer_index)->properties.session_name;
    if (parameters->map.get_allowed())
      map = parameters->map.get();
    if (network_session_interface_parameter_available(parameters->film.m_flags)) {
      const s_saved_film_description *film = (const s_saved_film_description *)network_session_interface_parameter_data(&parameters->film);
      film_name = film ? film->name : 0;
    }
    if (parameters->map_variant.get_allowed()) {
      const c_map_variant *map_variant = parameters->map_variant.get();
      map_variant_name = map_variant ? map_variant->m_metadata.name : 0;
    }
    if (network_session_interface_parameter_available(parameters->game_variant.m_flags)) {
      game_variant = parameters->game_variant.get();
      game_variant_name = game_variant ? game_variant->get_name() : 0;
    }
    if (network_session_interface_parameter_available(parameters->local_peer.m_flags)) {
      const s_network_session_matchmaking_hopper *identifier = parameters->local_peer.get();
      const c_hopper_configuration *hopper = identifier ? multiplayer_game_hoppers_get_hopper_configuration(identifier->hopper_identifier) : 0;
      hopper_name = hopper ? hopper->get_hopper_name() : 0;
    }
    memset(game_status, 0, sizeof(*game_status));
    game_status->protocol = 7;
    game_status->flags = 0;
    nsi_get_build_identifiers(&game_status->type, &game_status->version, &game_status->build);
    game_status->state = 1;
    game_status->details.session_class = session->session_class();
    game_status->details.session_type = session->m_session_type;
    game_status->details.join_mode = user_interface_squad_get_join_permission();
    if (session_name[0] != 0)
      nsi_ustrnzcpy(game_status->details.session_name, session_name, 32);
    ((const c_network_session *)session)->get_secure_key(&game_status->details.session_id, &game_status->details.session_key, 0);
    memcpy(&game_status->details.secure_address, membership->get_peer(membership->m_current_membership.host_peer_index),
      sizeof(game_status->details.secure_address));
    game_status->details.sort_priority = (short)current_mode();
    if (membership->get_peer_count() >= parameters->session_size.get_max_peer_count()) {
      long players = membership->m_current_membership.player_count;
      switch (game_status->details.join_mode) {
      case 0:
        game_status->details.slot_count_c = (short)players;
        break;
      case 1:
      case 2:
      case 3:
        game_status->details.slot_count_d = (short)players;
        break;
      }
    } else {
      long players = membership->m_current_membership.player_count;
      long open_slots = network_session_get_maximum_player_count(session) - players;
      switch (game_status->details.join_mode) {
      case 0:
        game_status->details.slot_count_c = (short)players;
        game_status->details.slot_count_a = (short)open_slots;
        break;
      case 1:
        game_status->details.slot_count_b = (short)open_slots;
        game_status->details.slot_count_d = (short)players;
        break;
      case 2:
        game_status->details.slot_count_b = (short)open_slots;
        game_status->details.slot_count_d = (short)players;
        break;
      case 3:
        game_status->details.slot_count_d = (short)players;
        break;
      }
    }
    switch (network_squad_session_get_start_mode()) {
    case 1:
      game_status->details.start_mode = 1;
      break;
    case 2:
      game_status->details.start_mode = 2;
      break;
    case 3:
      game_status->details.start_mode = 3;
      break;
    default:
      game_status->details.start_mode = 0;
      break;
    }
    if (hopper_name) {
      wchar_t wide_hopper_name[1024];
      utf8_string_to_wchar_string(hopper_name, wide_hopper_name, 1024, 0);
      nsi_wchar_to_ascii(game_status->details.hopper_name, wide_hopper_name, 16);
    } else {
      nsi_csstrnzcpy(game_status->details.hopper_name, "", 16);
    }
    game_status->details.kind = (short)network_life_cycle_squad_session_get_ui_game_mode();
    game_status->details.variant = (short)user_interface_get_session_game_mode();
    game_status->details.start_mode = (short)((network_squad_session_get_start_mode() == 2) + 1);
    game_status->details.game_engine_type = game_variant ? game_variant->get_game_engine_index() : 0;
    nsi_ustrnzcpy(game_status->details.game_variant_name, game_variant_name ? game_variant_name : L"", 16);
    game_status->details.campaign_id = map ? map->campaign_id : -1;
    game_status->details.campaign_difficulty = 1;
    if ((parameters->difficulty.m_flags & 1))
      game_status->details.campaign_difficulty = *parameters->difficulty.get();
    game_status->details.map_id = map ? map->map_id : -1;
    nsi_ustrnzcpy(game_status->details.map_variant_name, map_variant_name ? map_variant_name : L"", 16);
    nsi_ustrnzcpy(game_status->details.film_name, film_name ? film_name : L"", 16);
    game_status->details.teams_enabled = game_variant ? nsi_game_variant_has_teams(game_variant) : false;
    game_status->details.maximum_team_count =
        (short)(game_status->details.teams_enabled && map ? game_engine_variant_get_maximum_team_count(game_variant, (e_map_id)map->map_id) : 0);
    if (game_is_map_active() && nsi_game_is_multiplayer() && simulation_starting_up() && game_engine_running()) {
      if (game_engine_get_score_to_win_round() == 1) {
        short round_limit = 0;
        if (nsi_current_game_variant())
          round_limit = nsi_current_game_variant()->round_limit;
        game_status->details.score_to_win_round = round_limit;
        game_status->details.best_player_score = (short)game_engine_get_best_player_score();
      } else {
        game_status->details.score_to_win_round = (short)game_engine_get_score_to_win_round();
        game_status->details.best_player_score = (short)game_engine_get_best_player_score();
      }
    } else {
      game_status->details.score_to_win_round = 0;
      game_status->details.best_player_score = 0;
    }
    if (simulation_starting_up() && nsi_game_is_multiplayer() && (game_status->details.sort_priority == 3 || game_status->details.sort_priority == 15))
      team_game = true;
    game_status->details.average_rating = 0;
    for (long player_index = membership->get_first_player(); player_index != -1; player_index = membership->get_next_player(player_index)) {
      const s_network_session_player *player = &membership->m_current_membership.players[player_index];
      long list_index = player ? user_interface_session_player_get_player_index(&player->identifier) : -1;
      long rating = list_index != -1 ? game_engine_get_player_rating(list_index) : -1;
      game_status->details.players[player_count].identifier = player->identifier;
      memcpy(&game_status->details.players[player_count].configuration, player->configuration.client.name,
        sizeof(game_status->details.players[player_count].configuration));
      if (rating != -1)
        game_status->details.average_rating += (short)rating;
      if (team_game && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16)
        occupied_team_mask |= 1 << player->configuration.host.team_index;
      player_count++;
    }
    if (player_count > 0)
      game_status->details.average_rating /= player_count;
    else
      game_status->details.average_rating = 0;
    game_status->details.player_count = (short)player_count;
    if (team_game)
      nsi_game_engine_get_timer(&game_status->details.timer_type, &game_status->details.timer_seconds);
    if (game_status->details.teams_enabled && team_game) {
      game_status->details.occupied_team_mask = 0;
      for (long team = 0; team < 8; team++) {
        game_status->details.team_scores[team] = game_engine_get_team_rounds_won((e_game_team)team);
        if (game_status->details.team_scores[team] != 0 || (occupied_team_mask & (1 << team)) != 0)
          game_status->details.occupied_team_mask |= 1 << team;
      }
    }
    result = true;
  }
  return result;
}

// @address 0x8086b4 g_network_squad_map_loading
// @address 0x8086b5 g_network_group_map_loading
static bool g_network_group_map_loading;
// @address 0x73a934 g_network_session_current_map_id
long g_network_session_current_map_id = -1;

static __forceinline bool network_session_interface_session_in_game(const c_network_session *session)
{
  return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}
static __forceinline bool network_session_interface_simulation_is_aborted()
{
  return simulation_globals_get()->initialized && simulation_globals_get()->aborted;
}
static __forceinline bool network_session_interface_mode_uses_main_menu_map(long mode)
{
  bool result = false;
  if (mode == 4 || mode == 16)
    result = true;
  return result;
}
static __forceinline game_options *network_session_interface_game_options()
{
  return &tls_game_globals()->options;
}
static __forceinline bool network_session_interface_map_matches(const game_options *options, const s_network_session_parameter_map *map)
{
  bool same_map = options->campaign_id == map->campaign_id && options->map_id == map->map_id;
  return same_map && strncmp(map->scenario_path, options->scenario_path, sizeof(options->scenario_path)) == 0;
}

static __forceinline s_network_session_interface_map_id network_session_interface_map_get_id(const s_network_session_parameter_map *map)
{
  return *(const s_network_session_interface_map_id *)&map->map_id;
}
void network_session_interface_update_local_state()
{
  unsigned __int64 game_instance = (unsigned __int64)-1;
  bool in_session = false;
  long map_id = -1;
  c_network_session *session = 0;
  long map_status = 0;
  float map_progress = 0.0f;
  unsigned short hopper_identifier = 0xFFFF;
  bool *map_loading = 0;
  long mode = current_mode();
  if (network_session_interface_simulation_is_aborted())
    g_network_session_interface_globals.peer.flags |= 0x10000;
  else
    g_network_session_interface_globals.peer.flags &= ~0x10000;
  switch (mode) {
  case _life_cycle_state_pre_game:
  case _life_cycle_state_start_game:
  case _life_cycle_state_in_game:
  case _life_cycle_state_end_game_write_stats:
    in_session = network_life_cycle_try_and_get_squad_session(&session);
    map_loading = &g_network_squad_map_loading;
    break;
  case _life_cycle_state_matchmaking_start:
  case _life_cycle_state_matchmaking_find_match_client:
  case _life_cycle_state_matchmaking_find_match:
  case _life_cycle_state_matchmaking_find_and_assemble_match:
  case _life_cycle_state_matchmaking_assemble_match:
  case _life_cycle_state_matchmaking_arbitration:
  case _life_cycle_state_matchmaking_select_host:
  case _life_cycle_state_matchmaking_prepare_map:
  case _life_cycle_state_in_match:
  case _life_cycle_state_end_match_write_stats:
  case _life_cycle_state_post_match:
    in_session = network_life_cycle_in_group_session(&session);
    map_loading = &g_network_group_map_loading;
    break;
  }
  if (in_session && session && network_session_interface_session_in_game(session)) {
    s_session_membership *parameters = (s_session_membership *)session->get_session_parameters();
    bool map_load_requested = false;
    if (parameters->map.m_flags & 1) {
      short map_index = -1;
      const s_network_session_parameter_map *map;
      s_network_session_parameter_map main_menu_map;
      if (network_session_interface_mode_uses_main_menu_map(mode)) {
        memset(&main_menu_map, 0, sizeof(main_menu_map));
        main_menu_map.campaign_id = _campaign_id_none;
        main_menu_map.map_id = _map_id_mainmenu;
        levels_get_map_scenario_path(-1, _map_id_mainmenu, main_menu_map.scenario_path, sizeof(main_menu_map.scenario_path));
        map = &main_menu_map;
      } else {
        map = ((const c_generic_network_session_parameter<s_network_session_parameter_map> *)&parameters->map)->get();
        if (parameters->coop_insertion_point.m_flags & 1)
          map_index = *parameters->coop_insertion_point.get();
      }
      if (map && map->map_id != -1 && map->scenario_path[0]) {
        bool map_loaded = false;
        s_network_session_interface_map_id current_map_id = network_session_interface_map_get_id(map);
        g_network_session_current_map_id = current_map_id.value;
        map_id = current_map_id.value;
        if (simulation_starting_up())
          map_loaded = network_session_interface_map_matches(network_session_interface_game_options(), map);
        if (map_loaded) {
          map_progress = 1.0f;
          map_status = 4;
          *map_loading = false;
        } else {
          long scenario_type = map->campaign_id == -1;
          long load_status = loading_get_map_load_state(map_index, map->scenario_path);
          map_progress = network_session_interface_get_map_load_progress(scenario_type, map_index, map->scenario_path);
          if (load_status == 4) {
            map_status = 1;
            *map_loading = true;
            loading_load_map_with_progress(map_index, map->scenario_path, 1);
            map_load_requested = true;
          } else {
            *map_loading = false;
            switch (load_status) {
            case 1:
              map_status = 2;
              break;
            case 3:
              map_status = 3;
              break;
            }
            if (load_status != 1 && load_status != 3)
              loading_load_map_with_progress(map_index, map->scenario_path, 1);
            map_load_requested = true;
          }
        }
      }
    }
    if (!map_load_requested) {
      s_network_session_interface_map_load *map_load = &g_network_session_interface_globals.map_load;
      map_load->scenario_type = -1;
      map_load->load_status = 0;
    }
    if (parameters->local_peer.m_flags & 1)
      hopper_identifier = session_peer_value(&parameters->local_peer)->hopper_identifier;
  }
  if (game_in_progress() && network_session_interface_game_options()->game_mode != 3)
    game_instance = network_session_interface_game_options()->game_instance;
  g_network_session_interface_globals.peer.map_id = map_id;
  g_network_session_interface_globals.peer.map_status = map_status;
  g_network_session_interface_globals.peer.map_progress = (long)(map_progress * 100.0f);
  g_network_session_interface_globals.peer.map_hopper_identifier = hopper_identifier;
  g_network_session_interface_globals.game_instance = game_instance;
}

static __forceinline void network_session_interface_reset(long session_index)
{
  network_session_interface_cache()->update_time[session_index] = 0;
  for (long user_index = 0; user_index < 4; user_index++) {
    g_network_session_interface_globals.users[user_index].update_time[session_index] = 0;
    g_network_session_interface_globals.users[user_index].remove_time[session_index] = 0;
  }
}

static __forceinline void remove_player_rate_limited(c_network_session *session, long user_index)
{
  network_session_interface_globals *globals = (network_session_interface_globals *)(&g_network_session_interface_globals);
  s_network_session_interface_user_record *user = &globals->users[user_index];
  long session_index = session->session_index();
  long last_remove_time = user->remove_time[session_index];
  if (network_session_interface_system_time_get() - last_remove_time >= g_network_configuration.player_remove_interval) {
    if (session->peer_request_player_remove(user_index))
      user->remove_time[session_index] = network_session_interface_system_time_get();
  }
}

static __forceinline bool network_session_interface_membership_is_leader(const c_network_session_membership *membership)
{
  bool result = false;
  if (membership->has_membership())
    result = membership->m_local_peer_index == membership->m_current_membership.leader_peer_index;
  return result;
}

static __forceinline bool network_session_interface_membership_has_user(const c_network_session_membership *membership, long user_index)
{
  bool result = false;
  long peer_index = membership->m_local_peer_index;
  if (membership->is_peer_valid(peer_index)
      && membership->m_current_membership.peers[peer_index].user_player_indices[user_index] != -1)
    result = true;
  return result;
}

static __forceinline const s_peer_request *network_session_interface_get_peer_request(const c_peer_request_parameter *parameter)
{
  const s_peer_request *result = 0;
  if (parameter->m_flags & 1)
    result = &parameter->m_current;
  return result;
}

// @address 0x3b38d8 ?network_session_update_properties@@YAXPAVc_network_session@@@Z
void network_session_update_properties(c_network_session *session)
{
  network_session_interface_globals *globals = (network_session_interface_globals *)(&g_network_session_interface_globals);
  long connection_identifier = session->get_connection_identifier();
  long session_index = session->session_index();
  network_session_interface_properties_cache *cache = &globals->cache;
  if (connection_identifier != cache->connection_identifiers[session_index]) {
    network_session_interface_reset(session_index);
    cache->connection_identifiers[session_index] = connection_identifier;
  }
  if (network_session_interface_session_in_game(session) && !session->leaving_session()) {
    const c_network_session_membership *membership = &session->m_session_membership;
    const s_session_membership *parameters = (const s_session_membership *)session->get_session_parameters();
    const s_peer_request *peer_request = 0;
    if (parameters->parameter_62938.m_flags & 1)
      peer_request = network_session_interface_get_peer_request(&parameters->parameter_62938);
    const s_network_session_peer *local_peer = membership->get_peer(membership->m_local_peer_index);
    network_session_update_peer_properties(session, local_peer);
    if (network_session_interface_membership_is_leader(membership))
      network_session_update_film_and_session_string(session);
    for (long user_index = 0; user_index < 4; user_index++) {
      bool remove_player = false;
      bool add_player = false;
      long user_state = g_network_session_interface_globals.users[user_index].state;
      s_network_session_interface_user_record *user = &globals->users[user_index];
      if (user_state == 1 && !membership->does_player_exist((const s_player_identifier *)user->key, membership->m_local_peer_index, user_index))
        add_player = true;
      if (network_session_interface_membership_has_user(membership, user_index)) {
        if (user->state == 0 || add_player)
          remove_player = true;
      }
      if (remove_player) {
        if (simulation_starting_up() && tls_game_globals()->options.game_mode == 1
            && (!peer_request || peer_request->a == 0))
          network_squad_session_get_matchmaking_tip(3, -1);
        remove_player_rate_limited(session, user_index);
      } else if (user_state == 1) {
        e_network_join_refuse_reason refuse_reason = _network_join_refuse_reason_player_add_pending;
        if (add_player) {
          if ((simulation_starting_up() && tls_game_globals()->options.game_mode != 1)
              || !peer_request || peer_request->a == 0) {
            refuse_reason = session->peer_request_player_add_status(user_index, (const s_player_identifier *)user->key);
            if (refuse_reason == 21)
              session->peer_request_player_add(user_index, (const s_player_identifier *)user->key,
                  globals->users[user_index].change_count, (e_controller_index)user->controller_index,
                  (const s_player_configuration_from_client *)&user->configuration, user->voice_settings);
            else {
              network_session_set_player_failure_reason(user_index, refuse_reason);
              session->peer_complete_player_add(user_index, (const s_player_identifier *)user->key);
            }
          }
        } else {
          refuse_reason = _network_join_refuse_reason_none;
          session->peer_complete_player_add(user_index, (const s_player_identifier *)user->key);
        }
        if (refuse_reason == 0) {
          long player_index = membership->get_player_from_identifier((const s_player_identifier *)user->key);
          const s_network_session_player *player = &membership->m_current_membership.players[player_index];
          if (player->controller_index == -1 || player->desired_configuration_version != globals->users[user_index].change_count)
            network_session_update_user_properties(session, user_index);
          else
            globals->users[user_index].update_time[session->session_index()] = 0;
        }
      } else
        user->remove_time[session->session_index()] = 0;
    }
    if (session->is_host())
      network_session_check_properties(session);
  }
}
