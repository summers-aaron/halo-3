// networkinglibxbox/life_cycle_handler_end_match_write_stats.obj: from the records 0x3eaca0 0x3eb120 0x3eb1c8 0x3eb258 0x3eb2b8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_results_storage_types.h"
#include "game/game_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "networking/online/managed_session_status_flags.h"
#include "game/player_types.h"
#include <string.h>
#include <stdlib.h>
#include "networking/logic/network_session_interface.h"
#include "main/main_game.h"
#include "networking/logic/network_banhammer.h"
#include "networking/logic/network_join.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "game/game_results_types.h"
#include "game/game_results_team_data_types.h"
#include "game/game_results_statistic_types.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "game/game_results.h"
#include "game/game_results_team_statistics_types.h"
#include "networking/logic/write_stats_flags.h"
#include "game/game_results_statistics_types.h"
#include "game/game_lsp.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/life_cycle/life_cycle_stats.h"
#include "game/game_results_player_data.h"
#include "game/game_results_finalized.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/logic/life_cycle/life_cycle_handler_end_match_write_stats.h"
#include "networking/network_time_types.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/network_join_types.h"
#include "networking/session/network_managed_session_status.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
// @address 0x3eb120 ?update_for_state_transition@c_life_cycle_state_handler_end_match_write_stats@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3eb258 ?exit@c_life_cycle_state_handler_end_match_write_stats@@UAAXPAVc_life_cycle_state_handler@@@Z
// @address 0x3eb2b8 ?update_write_stats@c_life_cycle_state_handler_end_match_write_stats@@AAAXXZ
enum e_network_session_mode;
struct s_game_results;
enum e_end_match_write_stats_flags {
    _write_started_bit = 0, _write_completed_bit = 1, _write_blocked_bit = 2,
    _all_completed_bit = 10,
    _end_match_write_stats_write_requested_bit = 1, _end_match_write_stats_sessions_ended_bit = 3,
    _end_match_write_stats_sessions_end_complete_bit = 4, _end_match_write_stats_stats_written_bit = 5,
    _end_match_write_stats_write_complete_bit = 6, _end_match_write_stats_results_checked_bit = 7,
    _end_match_write_stats_initialized_bit = 8, _end_match_write_stats_skip_write_bit = 9,
    _end_match_write_stats_done_bit = 10
};
class c_life_cycle_state_handler;
namespace {

__forceinline void end_match_write_stats_observer_prioritize_upload_bandwidth(
    c_network_observer *observer, bool prioritize) {
    if (observer->m_stream_disable_expansion != prioritize) {
        observer->m_stream_disable_expansion = prioritize;
        observer->m_stream_rebalance = true;
        observer->m_stream_settle = true;
    }
}
__forceinline bool end_match_write_stats_main_game_change_in_progress() {
    return main_game_globals.game_change_in_progress;
}
}
e_life_cycle_state_transition_type c_life_cycle_state_handler_end_match_write_stats::update_for_state_transition() {
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    bool completed = m_flags.test(_all_completed_bit);
    if (!completed) {
        result = _life_cycle_state_transition_continue;
    } else {
        c_life_cycle_state_manager *manager = get_manager();
        c_network_session *session = manager->get_group_session();
        e_network_session_mode mode = session->get_session_parameters()->m_parameters.session_mode.get();
        switch (mode) {
        case _network_session_mode_end_game: result = _life_cycle_state_transition_continue; break;
        default:
            manager->request_state_change(_life_cycle_state_post_match, 0, 0);
            result = _life_cycle_state_transition_change;
            break;
        }
    }
    return result;
}
void c_life_cycle_state_handler_end_match_write_stats::exit(c_life_cycle_state_handler *handler) {
    c_life_cycle_state_manager *manager = get_manager();
    end_match_write_stats_observer_prioritize_upload_bandwidth(manager->get_observer(), false);
    if (!end_match_write_stats_main_game_change_in_progress())
        ::main_menu_launch();
}
void c_life_cycle_state_handler_end_match_write_stats::update_write_stats() {
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_group_session();
    if (!m_flags.test(_write_started_bit)) {
        int status = network_leaderboard_get_write_status();
        if (status == 1) {
            if (!m_flags.test(_write_blocked_bit)) m_flags.set(_write_blocked_bit, true);
        } else {
            const c_game_results *final_game_results = game_results_get_final_results();
            if (m_flags.test(_write_blocked_bit)) m_flags.set(_write_blocked_bit, false);
            network_leaderboard_write_stats(session->managed_session_index(), final_game_results);
            m_flags.set(_write_started_bit, true);
        }
    }
    if (m_flags.test(_write_started_bit) && !m_flags.test(_write_completed_bit)) {
        int status = network_leaderboard_get_write_status();
        switch (status) {
        case 1: break;
        case 2: m_flags.set(_write_completed_bit, true); break;
        default: m_flags.set(_write_completed_bit, true); break;
        }
    }
}
// @address 0x8c447c g_network_time_globals
static __forceinline unsigned long end_match_write_stats_network_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    return GetTickCount();
}
// @address 0x3eb1c8 ?enter@c_life_cycle_state_handler_end_match_write_stats@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_end_match_write_stats::enter(c_life_cycle_state_handler *previous_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous_handler, 0, 0);
    m_flags.clear();
    m_start_time = end_match_write_stats_network_time_get();
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
    ::network_banhammer_upload_network_statistics();
}
struct s_game_results;
struct s_game_results;
// @address 0xf283c8 g_network_session_interface_local_users
struct s_network_configuration;
static __forceinline bool end_match_write_stats_peer_stats_written(const s_network_session_peer &peer)
{
    return peer.properties.status_flags.test(_network_session_peer_properties_status_match_stats_written_bit);
}
static __forceinline long end_match_write_stats_network_time_since(unsigned long time)
{
    unsigned long now;
    if (g_network_time_globals.enabled)
        now = g_network_time_globals.time;
    else
        now = GetTickCount();
    return now - time;
}
static __forceinline void end_match_write_stats_flag_match_complete()
{
    g_network_session_interface_local_users[0].flags |= 1 << 10;
}
// @address 0x3eaca0 ?update@c_life_cycle_state_handler_end_match_write_stats@@UAAXXZ
void c_life_cycle_state_handler_end_match_write_stats::update()
{
    c_network_session *group_session = get_manager()->get_group_session();

    if (!m_flags.test(_end_match_write_stats_initialized_bit))
    {
        bool skip_write = false;
        bool stats_available = false;
        if (game_results_get_game_finalized())
        {
            const c_game_results *results = game_results_get_final_results();
            if (results && results->initialized)
                stats_available = true;
        }
        if (!stats_available || !::online_local_xuid_is_online_enabled())
            skip_write = true;
        m_flags.set(_end_match_write_stats_skip_write_bit, skip_write);
        m_flags.set(_end_match_write_stats_initialized_bit, true);
    }

    c_write_stats_flags flags = m_flags;
    if (!flags.test(_end_match_write_stats_done_bit))
    {
        if (!flags.test(_end_match_write_stats_skip_write_bit) && !flags.test(_end_match_write_stats_write_requested_bit))
        {
            update_write_stats();
        }
        else if (!flags.test(_end_match_write_stats_skip_write_bit) && !flags.test(_end_match_write_stats_sessions_end_complete_bit))
        {
            c_life_cycle_state_manager *manager = get_manager();
            long group_managed_session_index = manager->get_group_session()->managed_session_index();
            if (!flags.test(_end_match_write_stats_sessions_ended_bit))
            {
                managed_session_game_end(group_managed_session_index);
                managed_session_game_end(manager->get_active_squad_session()->managed_session_index());
                m_flags.set(_end_match_write_stats_sessions_ended_bit, true);
            }
            if (!m_flags.test(_end_match_write_stats_sessions_end_complete_bit))
            {
                s_managed_session_status status;
                managed_session_get_status(&status.m_storage, group_managed_session_index);
                if (!(status.test(_managed_session_operation_in_progress_bit)))
                {
                    if (status.test(_managed_session_end_succeeded_bit))
                        m_flags.set(_end_match_write_stats_sessions_end_complete_bit, true);
                    else if (status.test(_managed_session_end_failed_bit))
                        m_flags.set(_end_match_write_stats_sessions_end_complete_bit, true);
                }
            }
        }
        else if (!flags.test(_end_match_write_stats_results_checked_bit) && group_session->is_host())
        {
            const c_game_results *results = game_results_get_final_results();
            bool results_ok = true;
            if (results && results->finalized)
                results_ok = ::game_lsp_submit_multiplayer_result((s_game_results const *)results);
            m_flags.set(_end_match_write_stats_results_checked_bit, results_ok);
        }
        else if (!flags.test(_end_match_write_stats_write_complete_bit))
        {
            m_flags.set(_end_match_write_stats_stats_written_bit, true);
            m_flags.set(_end_match_write_stats_write_complete_bit, true);
        }
        else
        {
            end_match_write_stats_flag_match_complete();
            m_flags.set(_end_match_write_stats_done_bit, true);
        }
    }

    if (group_session->is_host() && m_flags.test(_end_match_write_stats_done_bit) && life_cycle_is_session_peers_ready(this, group_session) &&
        group_session->get_session_membership()->peer_property_flag_test(_peer_property_flag_test_all,
            _network_session_peer_properties_status_match_stats_written_bit))
    {
        group_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_post_game);
    }

    if (end_match_write_stats_network_time_since(m_start_time) > g_network_configuration.end_game_write_stats_timeout_seconds * 1000)
    {
        if (!m_flags.test(_end_match_write_stats_done_bit))
        {
            end_match_write_stats_flag_match_complete();
            m_flags.set(_end_match_write_stats_done_bit, true);
        }
        if (group_session->is_host() && life_cycle_is_session_peers_ready(this, group_session))
        {
            const c_network_session_membership *membership = group_session->get_session_membership();
            if (!end_match_write_stats_peer_stats_written(*membership->get_peer(membership->local_peer_index())))
            {
                group_session->host_boot_machine(membership->local_peer_index(), _network_session_boot_reason_stats_write_failed);
                disband_group_session();
                return;
            }
            unsigned long failed_peer_mask = 0;
            for (long peer_index = membership->get_first_peer(); peer_index != -1; peer_index = membership->get_next_peer(peer_index))
            {
                if (!end_match_write_stats_peer_stats_written(*membership->get_peer(peer_index)))
                    failed_peer_mask |= 1 << peer_index;
            }
            if (failed_peer_mask)
            {
                for (long peer_index = 0; peer_index < 16; peer_index++)
                {
                    if (failed_peer_mask & (1 << peer_index))
                        group_session->host_boot_machine(peer_index, _network_session_boot_reason_stats_write_failed);
                }
            }
        }
    }
}
