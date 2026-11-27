// networkinglibxbox/life_cycle_handler_end_game_write_stats.obj: from the records 0x3e29f0 0x3e2e98 0x3e2f40 0x3e2ff8 0x3e3018 0x3e32b8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_results_storage_types.h"
#include "game/game_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "memory/tls_globals.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include <stdlib.h>
#include "game/game.h"
#include "networking/logic/network_session_interface.h"
#include "main/main_game.h"
#include "networking/logic/network_banhammer.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_membership.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_class.h"
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
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_stats.h"
#include "networking/session/network_managed_session_status.h"
#include "game/game_results_player_data.h"
#include "game/game_results_finalized.h"
#include "networking/session/network_session_interface_types.h"
#include "game/game_globals_inline.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/network_time_types.h"
#include "cseries/crt_xdk.h"
#include "networking/session/network_session.h"
#include "networking/logic/life_cycle/life_cycle_handler_end_game_write_stats.h"
#include "networking/logic/life_cycle/life_cycle_handler_leaving.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
// @address 0x3e2e98 ?update_for_state_transition@c_life_cycle_state_handler_end_game_write_stats@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e2f40 ?enter@c_life_cycle_state_handler_end_game_write_stats@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
// @address 0x3e2ff8 ?exit@c_life_cycle_state_handler_end_game_write_stats@@UAAXPAVc_life_cycle_state_handler@@@Z
// @address 0x3e3018 ?game_get_stats_write_desire@c_life_cycle_state_handler_end_game_write_stats@@AAA?AW4e_end_game_stats_write_desire@@XZ
// @address 0x3e32b8 ?update_write_stats@c_life_cycle_state_handler_end_game_write_stats@@AAAXXZ
struct s_game_results;

enum e_end_game_write_stats_flags {
    _write_started_bit = 2, _write_completed_bit = 3, _write_blocked_bit = 4, _all_completed_bit = 13
};
// @address 0xf283c8 g_network_session_interface_local_users
namespace {

}
// @address 0x8c447c g_network_time_globals
struct s_networking_join_data;
class c_life_cycle_state_manager;
e_life_cycle_state_transition_type c_life_cycle_state_handler_end_game_write_stats::update_for_state_transition() {
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    bool completed = m_flags.test(_all_completed_bit);
    if (!completed) {
        result = _life_cycle_state_transition_continue;
    } else {
        c_life_cycle_state_manager *manager = get_manager();
        c_network_session *session = manager->get_active_squad_session();
        e_network_session_mode mode = session->get_session_parameters()->m_parameters.session_mode.get();
        switch (mode) {
        case _network_session_mode_end_game: result = _life_cycle_state_transition_continue; break;
        default:
            manager->request_state_change(_life_cycle_state_pre_game, 0, 0);
            result = _life_cycle_state_transition_change;
            break;
        }
    }
    return result;
}
void c_life_cycle_state_handler_end_game_write_stats::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    m_flags.clear();
    if (from_handler->get_state() != 3)
        m_flags.set(12, true);
    m_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    network_group_session_set_disable_host_election(0);
    if (g_network_join_data.join_queue_mode != _network_join_open_to_join_squad)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_open_to_join_squad;
    ::network_banhammer_upload_network_statistics();
}
// 0x3e2ff8 (no dev name): the main menu launches only when no game change is in progress
void c_life_cycle_state_handler_end_game_write_stats::exit(c_life_cycle_state_handler *to_handler)
{
    if (main_game_globals.game_change_in_progress) return;
    main_menu_launch();
}
e_end_game_stats_write_desire c_life_cycle_state_handler_end_game_write_stats::game_get_stats_write_desire() {
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_active_squad_session();
    if (session->session_class() != _network_session_class_xbox_live) {
        return _stats_write_desire_not_xbox_live;
    } else if (!game_is_active()) {
        return _stats_write_desire_game_not_active;
    } else if (game_is_campaign()) {
        return _stats_write_desire_campaign;
    } else if (game_is_cooperative()) {
        return _stats_write_desire_cooperative;
    } else if (!game_results_get_game_finalized()) {
        return _stats_write_desire_not_finalized;
    } else {
        const c_game_results *final_game_results = game_results_get_final_results();
        if (final_game_results == 0) {
            return _stats_write_desire_no_results;
        } else if (!final_game_results->initialized) {
            return _stats_write_desire_results_invalid;
        } else if (final_game_results->aborted) {
            return _stats_write_desire_aborted;
        } else if (final_game_results->invalid) {
            return _stats_write_desire_corrupt;
        } else {
            long player_count = 0;
            for (unsigned long machine_index = 0; machine_index < 16; machine_index++) {
                if (final_game_results->entries[machine_index].valid && final_game_results->entries[machine_index].joined
                    && !final_game_results->entries[machine_index].excluded) {
                    player_count++;
                }
            }
            if (player_count <= 1) return _stats_write_desire_too_few_players;
            else return _stats_write_desire_write;
        }
    }
}
void c_life_cycle_state_handler_end_game_write_stats::update_write_stats() {
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_active_squad_session();
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
struct s_network_configuration;
typedef char write_stats_timeout_offset[(offsetof(s_network_configuration, end_game_write_stats_timeout_seconds) == 0x294) ? 1 : -1];
static __forceinline bool life_cycle_handler_end_game_write_stats_status_test(unsigned short status, long bit)
{
    return ((status >> bit) & 1) != 0;
}
static __forceinline bool life_cycle_handler_end_game_write_stats_peer_complete(const s_network_session_peer &peer)
{
    return peer.properties.status_flags.test(_network_session_peer_properties_status_match_stats_written_bit);
}
static __forceinline long life_cycle_handler_end_game_write_stats_time_since(unsigned long time)
{
    unsigned long now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    return now - time;
}
static __forceinline void life_cycle_handler_end_game_write_stats_mark_complete()
{
    g_network_session_interface_local_users[0].flags |= 1;
}
// @address 0x3e29f0 ?update@c_life_cycle_state_handler_end_game_write_stats@@UAAXXZ
void c_life_cycle_state_handler_end_game_write_stats::update()
{
    c_network_session *session = get_manager()->get_active_squad_session();
    if (!m_flags.test(10))
    {
        bool skip_write = false;
        if (m_flags.test(12) || game_get_stats_write_desire() != _stats_write_desire_write)
            skip_write = true;
        m_flags.set(11, skip_write);
        m_flags.set(10, true);
    }
    c_write_stats_flags flags = m_flags;
    if (!flags.test(13))
    {
        if (!flags.test(1))
        {
            long index = get_manager()->get_active_squad_session()->managed_session_index();
            if (!flags.test(0))
            {
                managed_session_game_start(index);
                m_flags.set(0, true);
            }
            if (!m_flags.test(1))
            {
                unsigned short status;
                managed_session_get_status(&status, index);
                if (!(life_cycle_handler_end_game_write_stats_status_test(status, 8)))
                {
                    if (life_cycle_handler_end_game_write_stats_status_test(status, 9)) m_flags.set(1, true);
                    else if (life_cycle_handler_end_game_write_stats_status_test(status, 10)) m_flags.set(1, true);
                }
            }
        }
        else if (!flags.test(11) && !flags.test(3))
            update_write_stats();
        else if (!flags.test(6))
        {
            long index = get_manager()->get_active_squad_session()->managed_session_index();
            if (!flags.test(5))
            {
                managed_session_game_end(index);
                m_flags.set(5, true);
            }
            if (!m_flags.test(6))
            {
                unsigned short status;
                managed_session_get_status(&status, index);
                if (!(life_cycle_handler_end_game_write_stats_status_test(status, 11)))
                {
                    if (life_cycle_handler_end_game_write_stats_status_test(status, 12)) m_flags.set(6, true);
                    else if (life_cycle_handler_end_game_write_stats_status_test(status, 13)) m_flags.set(6, true);
                }
            }
        }
        else if (!flags.test(9) && session->is_host())
        {
            const c_game_results *results = game_results_get_final_results();
            bool submitted = true;
            if (results && results->finalized)
                submitted = ::game_lsp_submit_multiplayer_result((s_game_results const *)results);
            m_flags.set(9, submitted);
        }
        else if (!flags.test(8))
        {
            m_flags.set(7, true);
            m_flags.set(8, true);
        }
        else
        {
            life_cycle_handler_end_game_write_stats_mark_complete();
            m_flags.set(13, true);
        }
    }
    if (session->is_host() && life_cycle_is_session_peers_ready(this, session) && m_flags.test(13) &&
        session->get_session_membership_for_update()->peer_property_flag_test(_peer_property_flag_test_all,
            _network_session_peer_properties_status_flags_none))
        session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);

    if (life_cycle_handler_end_game_write_stats_time_since(m_time) > g_network_configuration.end_game_write_stats_timeout_seconds * 1000)
    {
        if (!m_flags.test(13))
        {
            life_cycle_handler_end_game_write_stats_mark_complete();
            m_flags.set(13, true);
        }
        if (session->is_host() && life_cycle_is_session_peers_ready(this, session))
        {
            c_network_session_membership *membership = session->get_session_membership_for_update();
            if (!life_cycle_handler_end_game_write_stats_peer_complete(*membership->get_peer(membership->local_peer_index())))
            {
                session->host_boot_machine(membership->local_peer_index(), _network_session_boot_reason_stats_write_failed);
                session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
            }
            else
            {
                unsigned long failed_peers = 0;
                for (long peer = membership->get_first_peer(); peer != -1; peer = membership->get_next_peer(peer))
                {
                    if (!life_cycle_handler_end_game_write_stats_peer_complete(*membership->get_peer(peer)))
                        failed_peers |= 1 << peer;
                }
                if (failed_peers)
                {
                    for (long peer = 0; peer < 16; ++peer)
                    {
                        if (failed_peers & (1 << peer))
                            session->host_boot_machine(peer, _network_session_boot_reason_stats_write_failed);
                    }
                }
            }
        }
    }
}

// @address 0x3e2ff8 ?exit@c_life_cycle_state_handler_leaving@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_leaving::exit(c_life_cycle_state_handler *to_handler)
{
    if (main_game_globals.game_change_in_progress) return;
    main_menu_launch();
}
