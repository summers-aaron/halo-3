#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
// networkinglibxbox/life_cycle_handler_post_match.obj: from the records 0x3eb3d0 0x3eb490 0x3ebaf8 0x3ebc48 0x3ebce8 0x3ec0e8 0x3ec868 0x3ec908 0x3ec9b8 0x3ecb80 0x3eb910 0x3ebd68
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "cseries/cseries.h"
// @address 0x3a1600 ?is_host@c_network_session@@QBA_NXZ
// @address 0x3ed6c8 ?set@c_network_session_parameter_session_mode@@QAA_NW4e_network_session_mode@@@Z
// @address 0x400d30 ?network_banhammer_controller_idle_for_too_long@@YA_NW4e_controller_index@@@Z
// @address 0x3cc8b0 ?all_peers_established@c_network_session_membership@@QBA_NXZ
// @address 0x3cbc88 ?peer_property_flag_test@c_network_session_membership@@QBA_NW4e_peer_property_flag_test_type@@W4e_network_session_peer_properties_status_flags@@@Z
#include <stdlib.h>
#include <string.h>
#include "cseries/bit_operations.h"
#include "math/integer_math.h"
#include "networking/network_time_since_access.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "networking/logic/network_join.h"
#include "networking/transport/transport_security.h"
#include "networking/logic/network_broadcast_search.h"
#include "networking/session/network_session_class.h"
#include "networking/session/network_session_membership.h"
#include "networking/transport/transport_security_types.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_post_match.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/session/network_observer.h"
#include "networking/session/network_session_find_host_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/bandwidth_configuration_host_preference_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_parameters_session.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/network_configuration_types.h"
#include "cseries/cseries_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/session/network_session_parameters.h"
#include "networking/network_configuration.h"
#include "networking/network_time_types.h"
#include "main/main_game.h"
// @address 0x95e608 main_game_globals
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x11df680 game_globals
#include "game/game_time_runtime.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "memory/gameplay_tls_globals.h"
struct s_replicated_life_cycle_progress_post_match;
struct utf8;
struct s_game_state;
struct s_life_cycle_matchmaking_progress;


extern "C" {
    struct s_globals;
}
struct s_network_leaderboard_globals;


#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"







namespace {



static __forceinline void network_session_interface_set_peer_status_flag(long flag, bool enabled)
{
    if (enabled)
        g_network_session_interface_local_users[0].flags |= 1UL << flag;
    else
        g_network_session_interface_local_users[0].flags &= ~(1UL << flag);
}
static __forceinline bool network_session_interface_test_peer_status_flag(long flag)
{
    return (g_network_session_interface_local_users[0].flags & (1UL << flag)) != 0;
}

static __forceinline bool life_cycle_handler_post_match_is_guest_account(unsigned __int64 xuid) {
    unsigned long flags = (unsigned long)(xuid >> 48);
    return (flags & 15) == 9 && (flags & 192) != 0;
}
}

// @address 0x3eb3d0 life_cycle_handler_post_match_find_peer_first_player
long life_cycle_handler_post_match_find_peer_first_player(const c_network_session_membership *membership, long peer_index)
{
    long lowest_player_index = -1;
    long lowest_controller_index = -1;
    for (long player_index = (membership)->get_first_player(); player_index != -1;
        player_index = (membership)->get_next_player(player_index))
    {
        const s_network_session_player *player = &membership->m_current_membership.players[player_index];
        if (player->peer_index == peer_index) {
            if (lowest_controller_index == -1 || player->controller_index < lowest_controller_index) {
                if (!life_cycle_handler_post_match_is_guest_account(player->configuration.client.xuid)) {
                    lowest_controller_index = player->controller_index;
                    lowest_player_index = player_index;
                }
            }
        }
    }
    return lowest_player_index;
}

// @address 0x3eb490 ?get_progress@c_life_cycle_state_handler_post_match@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
void c_life_cycle_state_handler_post_match::get_progress(s_life_cycle_matchmaking_progress *progress)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    const c_network_session_parameters *parameters = group_session->get_session_parameters();
    if (!(parameters->m_parameters.matchmaking_hopper.get_allowed()) || !(parameters->m_parameters.matchmaking_hopper_list.get_allowed()) ||
        !(parameters->m_parameters.matchmaking_progress.get_allowed()))
        return;

    c_network_session *squad_session = manager->get_active_squad_session();
    const c_network_session_membership *membership = group_session->get_session_membership();
    const c_network_session_membership *progress_membership = group_session->get_session_membership();
    const s_replicated_life_cycle_matchmaking_progress *status = parameters->m_parameters.matchmaking_progress.get();
    if (status->type != 5)
        return;

    const s_network_session_matchmaking_hopper *hopper_map = parameters->m_parameters.matchmaking_hopper.get();
    long local_team_index = get_peer_rematch_team_index(progress_membership->local_peer_index());
    progress->type = 5;
    progress->post_match = status->post_match;
    progress->rematch_requested = (_rotl(m_flags, 26) & 1) != 0;
    progress->rematch_enabled = hopper_map->rematch_allowed;
    bool rematch_room = hopper_map->rematch_allowed &&
        squad_session->get_session_membership()->m_current_membership.player_count < hopper_map->maximum_party_size;

    long team_player_counts[16];
    for (long team_index = 0; team_index < 16; team_index++)
        team_player_counts[team_index] = 0;
    for (long player_index = (membership)->get_first_player(); player_index != -1;
        player_index = (membership)->get_next_player(player_index))
    {
        long team_index = get_player_rematch_team_index(player_index);
        if (team_index != -1)
            team_player_counts[team_index]++;
    }

    bool rematch_ready;
    if (local_team_index != -1)
        rematch_ready = rematch_room && team_player_counts[local_team_index] >
            squad_session->get_session_membership()->m_current_membership.player_count;
    else
        rematch_ready = false;
    progress->rematch_ready = rematch_ready;
    progress->rematch_team_player_mask = 0;
    progress->rematch_requested_player_mask = 0;
    progress->rematch_party_player_mask = 0;

    if (rematch_ready && local_team_index != -1) {
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            if ((_rotl(progress_membership->m_current_membership.peers[peer_index].properties.status_flags.get_unsafe(), 17) & 1) != 0) {
                long player_index = life_cycle_handler_post_match_find_peer_first_player(membership, peer_index);
                if (player_index != -1 && local_team_index == get_player_rematch_team_index(player_index)) {
                    SET_BIT(progress->rematch_team_player_mask, player_index, true);
                    bool rematch_requested_player = progress_membership->m_current_membership.players[player_index].configuration.client.desires_rematch;
                    unsigned long *requested_mask = &progress->rematch_requested_player_mask;
                    *requested_mask = rematch_requested_player ? (*requested_mask | FLAG(player_index))
                        : (*requested_mask & ~FLAG(player_index));
                }
            }
        }

        for (long player_index = (membership)->get_first_player(); player_index != -1;
            player_index = (membership)->get_next_player(player_index))
        {
            unsigned long player_bit = FLAG(player_index);
            if ((player_bit & progress->rematch_requested_player_mask) != 0) {
                unsigned __int64 party_nonce = progress_membership
                    ->m_current_membership.peers[progress_membership->m_current_membership.players[player_index].peer_index].party_nonce;
                SET_BIT(progress->rematch_party_player_mask, player_index, true);
                for (long other_player_index = (membership)->get_first_player(); other_player_index != -1;
                     other_player_index = (membership)->get_next_player(other_player_index)) {
                    if (other_player_index != player_index &&
                        party_nonce == progress_membership
                        ->m_current_membership.peers[progress_membership->m_current_membership.players[other_player_index].peer_index].party_nonce)
                        SET_BIT(progress->rematch_party_player_mask, other_player_index, true);
                }
            }
        }
    }

    const s_network_session_matchmaking_hopper_list *hopper = parameters->m_parameters.matchmaking_hopper_list.get();
    for (long map_index = 0; map_index < hopper->hopper_count; map_index++) {
        if (hopper->hoppers[map_index].identifier == hopper_map->hopper_identifier) {
            ::utf8_string_to_wchar_string(
                (utf8 const *)(const unsigned char *)hopper->hoppers[map_index].name, (wchar_t *)(progress->map_name), 32, (long *)(0));
            break;
        }
    }
    progress->teams_enabled = hopper_map->is_ranked;
    progress->map_options = hopper_map->team_game;
    progress->squad_player_count = squad_session->get_session_membership()->m_current_membership.player_count;
    progress->group_player_count = progress_membership->m_current_membership.player_count;
}

// @address 0x3ebaf8 ?update_for_state_transition@c_life_cycle_state_handler_post_match@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_post_match::update_for_state_transition()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    c_network_session *squad = manager->get_active_squad_session();
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    switch (group->get_session_parameters()->m_parameters.session_mode.get()) {
    case _network_session_mode_post_game:
        transition = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_disbanding:
        if (squad->is_host()) {
            c_network_session_parameter_session_mode *parameters = &squad->get_session_parameters()->m_parameters.session_mode;
            if (g_network_configuration.return_to_pre_game)
                parameters->set(_network_session_mode_idle);
            else
                parameters->set(_network_session_mode_matchmaking_start);
        }
        transition = _life_cycle_state_transition_continue;
        break;
    }
    if (transition == _life_cycle_state_transition_continue) {
        transition = _life_cycle_state_transition_none;
        switch (squad->get_session_parameters()->m_parameters.session_mode.get()) {
        case _network_session_mode_matchmaking_slave:
            transition = _life_cycle_state_transition_continue;
            break;
        case _network_session_mode_idle:
            manager->request_state_change(_life_cycle_state_pre_game, 0, 0);
            transition = _life_cycle_state_transition_change;
            break;
        case _network_session_mode_matchmaking_start:
            manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            transition = _life_cycle_state_transition_change;
            break;
        }
    }
    return transition;
}


// @address 0x3ebc48 ?enter@c_life_cycle_state_handler_post_match@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_post_match::enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous, 0, 0);
    g_network_session_interface_local_users[0].hopper_identifier = 0xffff;
    g_network_session_interface_local_users[0].hopper_error = 0;
    m_flags = 0;
    m_start_time = 0;
    m_settle_start_time = 0;
    m_2c = 0;
    m_30 = 0;
    m_3c = -1;
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
    if (previous->get_state() == _life_cycle_state_joining)
        m_flags = (unsigned short)(m_flags | 64);
}

// @address 0x3ebce8 ?exit@c_life_cycle_state_handler_post_match@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_post_match::exit(c_life_cycle_state_handler *previous_handler)
{
    if ((_rotl(m_flags, 28) & 1) == 0) {
        network_leaderboard_globals.refresh_pending = true;
        m_flags = m_flags | 16;
    }
}


namespace {



static __forceinline bool life_cycle_handler_post_match_flag_test(const unsigned short *flags, long bit)
{
    return (*(const short *)flags & (short)(1 << bit)) != 0;
}
static __forceinline void life_cycle_handler_post_match_flag_set(unsigned short *flags, long bit, bool enable)
{
    short mask = (short)(1 << bit);
    if (enable)
        *(short *)flags |= mask;
    else
        *(short *)flags &= ~mask;
}
static __forceinline void life_cycle_handler_post_match_set_join_queue_mode(e_network_join_queue_mode mode)
{
    s_networking_join_data *join_data = &g_network_join_data;
    if (join_data->join_queue_mode != mode)
        network_join_flush_join_queue();
    join_data->join_queue_mode = mode;
}
}

// @address 0x3ec0e8 ?update_squad_host@c_life_cycle_state_handler_post_match@@AAAXXZ
void c_life_cycle_state_handler_post_match::update_squad_host()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();

    if (!life_cycle_handler_post_match_flag_test(&m_flags, 5)) {
        const c_network_session_membership *squad_membership = squad_session->get_session_membership();
        long squad_leader_peer_index = squad_membership->leader_peer_index();
        c_network_session *group_session = manager->get_group_session();
        const c_network_session_membership *group_membership = group_session->get_session_membership();
        c_network_session_parameters::s_network_session_parameters_internal *group_parameters =
            &group_session->get_session_parameters()->m_parameters;
        const s_network_session_matchmaking_hopper *hopper = group_parameters->matchmaking_hopper.get();

        bool leader_wants_rematch = false;
        bool ready_for_matchmaking_start = false;
        s_replicated_life_cycle_progress_post_match state;
        get_state_from_progress(&state);

        if (hopper->rematch_allowed) {
            long leader_player_index = life_cycle_handler_post_match_find_peer_first_player(squad_membership, squad_leader_peer_index);
            if (leader_player_index != -1)
                leader_wants_rematch = squad_membership->m_current_membership.players[leader_player_index].configuration.client.desires_rematch;
        }

        if (network_session_interface_test_peer_status_flag(12) != leader_wants_rematch)
            network_session_interface_set_peer_status_flag(12, leader_wants_rematch);

        if (!network_observer_is_peer_joining()) {
            if (life_cycle_handler_post_match_flag_test(&m_flags, 3)) {
                long join_result = network_join_is_any_join_in_progress();
                if (join_result != 1) {
                    bool join_succeeded = (join_result == 0);
                    if (!join_succeeded) {
                        c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                            &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                        network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging, 3);
                        ready_for_matchmaking_start = true;
                    }
                    network_join_clear_squad_join_result();
                    life_cycle_handler_post_match_flag_set(&m_flags, 3, false);
                }
            } else {
                if (leader_wants_rematch && group_parameters->matchmaking_rematch_data.get_allowed()) {
                    const s_network_session_parameter_matchmaking_rematch_data *rematch_data
                        = group_parameters->matchmaking_rematch_data.get();
                    long team_index = get_peer_rematch_team_index(group_membership->local_peer_index());
                    if (team_index != -1 && TEST_BIT(rematch_data->team_rematch_mask, team_index)) {
                        const s_network_session_parameter_matchmaking_rematch_team_data *team_data = &rematch_data->teams[team_index];
                        if (team_data->host_peer_index != group_membership->local_peer_index()) {
                            if (life_cycle_handler_post_match_flag_test(&m_flags, 2)) {
                                network_group_session_set_disable_host_election(3);
                                life_cycle_handler_post_match_set_join_queue_mode(_network_join_queue_closed_while_in_matchmaking);
                                life_cycle_handler_post_match_flag_set(&m_flags, 2, false);
                            }
                        } else {
                            bool send_request = false;
                            bool first_request = false;
                            network_group_session_set_disable_host_election(0);
                            life_cycle_handler_post_match_set_join_queue_mode(_network_join_open_to_join_squad);

                            e_transport_platform platform;
                            s_transport_session_description host_description;
                            if (::managed_session_get_security_information(
                                    squad_session->managed_session_index(), false,
                                    &host_description, &platform))
                            {
                                if (!life_cycle_handler_post_match_flag_test(&m_flags, 2)) {
                                    m_30 = network_time_get();
                                    first_request = true;
                                    life_cycle_handler_post_match_flag_set(&m_flags, 2, true);
                                }
                                if (!team_data->join_data_valid ||
                                    memcmp(&team_data->host_description, &host_description, sizeof(host_description)) != 0) {
                                    send_request = true;
                                }
                            }
                            if (send_request && (first_request || network_time_since(m_30) >= 500)) {
                                s_network_session_parameter_matchmaking_rematch_data change_request;
                                memset(&change_request, 0, sizeof(change_request));
                                SET_BIT(change_request.team_rematch_mask, team_index, true);
                                memcpy(&change_request.teams[team_index], team_data, sizeof(*team_data));
                                change_request.teams[team_index].join_data_valid = true;
                                memcpy(&change_request.teams[team_index].host_description, &host_description, sizeof(host_description));
                                group_parameters->matchmaking_rematch_data.request_change(&change_request);
                                m_30 = network_time_get();
                            }
                        }
                    }
                }

                if (state.countdown_running && state.countdown == 0) {
                    bool can_rematch = true;
                    long team_index = get_peer_rematch_team_index(group_membership->local_peer_index());
                    if (!leader_wants_rematch) {
                        can_rematch = false;
                    } else if (team_index == -1) {
                        if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                            c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                                &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                            network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging, 3);
                        }
                        can_rematch = false;
                    } else if (!group_parameters->matchmaking_rematch_data.get_allowed()) {
                        if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                            c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                                &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                            network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging, 3);
                        }
                        can_rematch = false;
                    } else {
                        const s_network_session_parameter_matchmaking_rematch_data *rematch_data
                            = group_parameters->matchmaking_rematch_data.get();
                        if (!TEST_BIT(rematch_data->team_rematch_mask, team_index)) {
                            can_rematch = false;
                        } else if (!TEST_BIT(rematch_data->teams[team_index].rematch_peer_mask,
                                       group_membership->local_peer_index())) {
                            if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                                c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                                    &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                                network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging,
                                    3);
                            }
                            can_rematch = false;
                        } else if (rematch_data->teams[team_index].host_peer_index == -1) {
                            if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                                c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                                    &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                                network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging,
                                    3);
                            }
                            can_rematch = false;
                        } else if (!rematch_data->teams[team_index].join_data_valid) {
                            if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                                c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                                    &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                                network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging,
                                    3);
                            }
                            can_rematch = false;
                        }
                    }

                    if (!can_rematch) {
                        ready_for_matchmaking_start = true;
                    } else {
                        const s_network_session_parameter_matchmaking_rematch_data *rematch_data
                            = group_parameters->matchmaking_rematch_data.get();
                        const s_network_session_parameter_matchmaking_rematch_team_data *team_data = &rematch_data->teams[team_index];
                        if (!life_cycle_handler_post_match_flag_test(&m_flags, 7)) {
                            m_3c = squad_membership->m_current_membership.peer_count;
                            life_cycle_handler_post_match_flag_set(&m_flags, 7, true);
                        }
                        if (team_data->host_peer_index == group_membership->local_peer_index()) {
                            if (group_membership->all_peers_established() && squad_membership->all_peers_established()) {
                                unsigned long joined_peer_mask = 0;
                                for (long group_peer_index = (group_membership)->get_first_peer(); group_peer_index != -1;
                                     group_peer_index = (group_membership)->get_next_peer(group_peer_index)) {
                                    if (TEST_BIT(team_data->rematch_peer_mask, group_peer_index)) {
                                        const s_transport_secure_address *group_peer_address =
                                            &group_membership->m_current_membership.peers[group_peer_index]
                                                 .secure_address;
                                        for (long squad_peer_index = (squad_membership)->get_first_peer(); squad_peer_index != -1;
                                             squad_peer_index = (squad_membership)->get_next_peer(squad_peer_index)) {
                                            const s_transport_secure_address *squad_peer_address =
                                                &squad_membership->m_current_membership.peers[squad_peer_index]
                                                     .secure_address;
                                            if (transport_secure_address_compare(group_peer_address, squad_peer_address))
                                                SET_BIT(joined_peer_mask, group_peer_index, true);
                                        }
                                    }
                                }
                                if (joined_peer_mask == team_data->rematch_peer_mask)
                                    ready_for_matchmaking_start = true;
                            }
                        } else if (!life_cycle_handler_post_match_flag_test(&m_flags, 3)) {
                            if (!network_join_to_remote_squad(false, false, false, _network_session_class_xbox_live,
                                    &team_data->host_description.secure_identifier, &team_data->host_description.secure_key,
                                    &team_data->host_description.secure_address)) {
                                ready_for_matchmaking_start = true;
                            } else {
                                life_cycle_handler_post_match_flag_set(&m_flags, 3, true);
                            }
                        }
                    }
                }
            }
        }

        if (ready_for_matchmaking_start) {
            if (life_cycle_handler_post_match_flag_test(&m_flags, 7) &&
                squad_membership->m_current_membership.peer_count == m_3c) {
                c_network_session_parameters::s_network_session_parameters_internal *squad_parameters =
                    &get_manager()->get_active_squad_session()->get_session_parameters()->m_parameters;
                network_session_request_parameter_set_value(&squad_parameters->matchmaking_messaging, 3);
            }
            life_cycle_handler_post_match_flag_set(&m_flags, 5, true);
        }
    }

    if (life_cycle_handler_post_match_flag_test(&m_flags, 5) && life_cycle_handler_post_match_is_settled(this)) {
        c_network_session_parameters::s_network_session_parameters_internal *squad_parameters = &squad_session->get_session_parameters()->m_parameters;
        const c_network_session_membership *squad_membership = squad_session->get_session_membership();
        if (squad_membership->peer_property_flag_test_any_peer(_network_session_peer_properties_status_leader_idle_controller_bit)) {
            network_session_request_parameter_set_value(
                &squad_parameters->matchmaking_messaging, 0);
            squad_parameters->session_mode.set(_network_session_mode_idle);
        } else if (g_network_configuration.return_to_pre_game) {
            squad_parameters->session_mode.set(_network_session_mode_idle);
        } else {
            squad_parameters->session_mode.set(_network_session_mode_matchmaking_start);
        }
    }
}

// @address 0x3ec868 ?get_state_from_progress@c_life_cycle_state_handler_post_match@@AAAXPAUs_replicated_life_cycle_progress_post_match@@@Z
// match: 0x3ec868: pointer __restrict qualifier prevents alias reload
void c_life_cycle_state_handler_post_match::get_state_from_progress(struct s_replicated_life_cycle_progress_post_match * __restrict progress)
{
  const c_network_session_parameters *parameters = get_manager()->get_group_session()->get_session_parameters();
  memset(progress, 0, sizeof(*progress));
  progress->countdown = parameters->m_parameters.matchmaking_hopper.get()->rematch_timer;
  if (parameters->m_parameters.matchmaking_progress.get_allowed()) {
    const s_replicated_life_cycle_matchmaking_progress *value = parameters->m_parameters.matchmaking_progress.get();
    if (value->type == 5)
      *progress = value->post_match;
  }
}

__forceinline long c_life_cycle_state_handler_post_match::get_player_rematch_team_index(long player_index)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    const c_network_session_parameters *session = group->get_session_parameters();
    const s_network_session_matchmaking_hopper *hopper = session->m_parameters.matchmaking_hopper.get();
    signed char team_game = (signed char)hopper->is_ranked;
    long result;
    if (team_game) {
        const c_network_session_membership *membership = group->get_session_membership();
        const s_network_session_player *player = &membership->m_current_membership.players[player_index];
        result = player->configuration.host.team_index;
    } else {
        result = 0;
    }
    return result;
}

// @address 0x3ec908 ?get_peer_rematch_team_index@c_life_cycle_state_handler_post_match@@AAAJJ@Z
long c_life_cycle_state_handler_post_match::get_peer_rematch_team_index(long peer_index)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    const c_network_session_membership *membership = group->get_session_membership();
    long team_index = -1;
    for (long player_index = (membership)->get_first_player(); player_index != -1;
        player_index = (membership)->get_next_player(player_index))
    {
        const s_network_session_player *player = &membership->m_current_membership.players[player_index];
        if (player->peer_index == peer_index) {
            team_index = get_player_rematch_team_index(player_index);
            if (team_index != -1)
                break;
        }
    }
    return team_index;
}

// @address 0x3ec9b8 life_cycle_handler_post_match_is_settled
bool life_cycle_handler_post_match_is_settled(c_life_cycle_state_handler_post_match *handler)
{
    bool result;
    result = 0;
    c_life_cycle_state_manager *manager;
    manager = handler->get_manager();
    int start_time;
    start_time = handler->m_start_time;
    unsigned char time_locked;
    time_locked = g_network_time_globals.enabled;
    const c_network_session_membership *squad_membership;
    squad_membership = manager->get_active_squad_session()->get_session_membership();
    const c_network_session_membership *group_membership;
    group_membership = manager->get_group_session()->get_session_membership();
    int time_value;
    int now;
    if (time_locked != 0) {
        time_value = g_network_time_globals.time;
        now = time_value;
    } else {
        now = GetTickCount();
        time_value = g_network_time_globals.time;
        time_locked = g_network_time_globals.enabled;
    }
    if (now - start_time >= g_network_configuration.post_match_recover_time && squad_membership->all_peers_established() &&
        group_membership->peer_property_flag_test(_peer_property_flag_test_all, _network_session_peer_properties_status_match_ready_for_next_match_bit))
    {
        result = 1;
        if ((_rotl(handler->m_flags, 24) & 1) == 0) {
            if (time_locked != 0) {
                now = time_value;
            } else {
                now = GetTickCount();
                time_value = g_network_time_globals.time;
                time_locked = g_network_time_globals.enabled;
            }
            handler->m_settle_start_time = now;
            unsigned short *flags_ref;
            flags_ref = &handler->m_flags;
            *flags_ref = (unsigned short)(*flags_ref | 256);
        }
        bool all_ready;
        all_ready = true;
        now = (squad_membership)->get_first_player();
        while (now != -1) {
            const s_network_session_player *player = &squad_membership->m_current_membership.players[now];
            bool player_ready;
            player_ready = true;
            if (player->configuration.client.global_statistics.valid == 0 || player->configuration.client.hopper_statistics.valid == 0
                || player->configuration.host.global_stats_valid == 0 || player->configuration.host.hopper_stats_valid == 0) {
                player_ready = false;
            }
            if (!player_ready) {
                all_ready = false;
                break;
            }
            now = (squad_membership)->get_next_player(now);
        }
        if (!all_ready) {
            int settle_start;
            settle_start = handler->m_settle_start_time;
            if (time_locked != 0) {
                now = time_value;
            } else {
                now = GetTickCount();
            }
            if (now - settle_start > g_network_configuration.post_match_settle_time) {
                result = 1;
            } else {
                result = 0;
            }
        }
    }
    return result;
}

// @address 0x3ecb80 ?update_leader_idle_controllers@c_life_cycle_state_handler_post_match@@AAAXXZ
void c_life_cycle_state_handler_post_match::update_leader_idle_controllers()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad = manager->get_active_squad_session();
    const c_network_session_membership *membership = squad->get_session_membership();
    long local_peer_index = membership->local_peer_index();
    if (membership->leader_peer_index() == local_peer_index) {
        bool idle_controller = false;
        bool active_controller = false;
        for (long player_index = (membership)->get_first_player(); player_index != -1;
            player_index = (membership)->get_next_player(player_index))
        {
            const s_network_session_player *player = &membership->m_current_membership.players[player_index];
            if (player->peer_index == local_peer_index && player->controller_index != -1) {
                if (network_banhammer_controller_idle_for_too_long((e_controller_index)player->controller_index))
                    idle_controller = true;
                else
                    active_controller = true;
            }
        }
        if (idle_controller && !active_controller)
            network_session_interface_set_peer_status_flag(_network_session_peer_properties_status_leader_idle_controller_bit, true);
    }
}

// @address 0x3eb910 ?update@c_life_cycle_state_handler_post_match@@UAAXXZ
void c_life_cycle_state_handler_post_match::update()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    c_network_session *squad_session = manager->get_active_squad_session();
    matchmaking_update_required_files_for_hopper();
    update_leader_idle_controllers();
    if (!life_cycle_handler_post_match_flag_test(&m_flags, 0) && simulation_starting_up() && main_game_globals.game_change_in_progress == 0 &&
        game_globals->options.game_mode == 3) {
        network_session_interface_set_peer_status_flag(11, true);
        m_flags |= 1;
    }
    if ((_rotl(m_flags, 28) & 1) == 0) {
        s_replicated_life_cycle_progress_post_match progress;
        get_state_from_progress(&progress);
        if (progress.countdown_running && progress.countdown <= g_network_configuration.post_match_leaderboard_refresh_countdown) {
            network_leaderboard_globals.refresh_pending = true;
            m_flags |= 16;
        }
    }
    if ((_rotl(m_flags, 26) & 1) == 0) {
        s_replicated_life_cycle_progress_post_match progress;
        get_state_from_progress(&progress);
        if (progress.countdown_running && progress.countdown == 0) {
            unsigned long time_locked = g_network_time_globals.enabled;
            life_cycle_handler_post_match_flag_set(&m_flags, 6, true);
            network_session_interface_set_peer_status_flag(_network_session_peer_properties_status_match_ready_for_next_match_bit, true);
            m_start_time = time_locked ? g_network_time_globals.time : GetTickCount();
        }
    }
    if (squad_session->get_session_membership()->is_leader())
        network_session_interface_set_peer_status_flag(15, true);
    else
        network_session_interface_set_peer_status_flag(15, false);
    if (group_session->is_host())
        update_group_host();
    if (squad_session->is_host())
        update_squad_host();
}

// @address 0x3ebd68 ?update_group_host@c_life_cycle_state_handler_post_match@@AAAXXZ
void c_life_cycle_state_handler_post_match::update_group_host()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    const c_network_session_membership *membership = group_session->get_session_membership();
    c_network_session_parameters::s_network_session_parameters_internal *group_parameters =
        &group_session->get_session_parameters()->m_parameters;
    s_replicated_life_cycle_progress_post_match state;
    get_state_from_progress(&state);
    if (membership->peer_property_flag_test(_peer_property_flag_test_all, (e_network_session_peer_properties_status_flags)11)) {
        if ((_rotl(m_flags, 31) & 1) == 0) {
            unsigned short *flags_ptr = &m_flags;
            m_2c = network_time_get();
            *flags_ptr |= 2;
        }
        long seconds = network_time_since(m_2c) / 1000;
        if (seconds > 0) {
            state.countdown = (state.countdown - seconds < 0) ? 0 : (state.countdown - seconds);
            m_2c = network_time_get();
        }
        state.countdown_running = true;
    }
    if (membership->all_peers_established()) {
        unsigned long team_masks[16];
        for (long i = 0; i < 16; i++)
            team_masks[i] = 0;
        for (long peer_index = membership->get_first_peer(); peer_index != -1; peer_index = membership->get_next_peer(peer_index)) {
            if ((_rotl(membership->m_current_membership.peers[peer_index].properties.status_flags.get_unsafe(), 20) & 1) != 0) {
                long team_index = get_peer_rematch_team_index(peer_index);
                if (team_index != -1)
                    team_masks[team_index] |= 1UL << peer_index;
            }
        }
        s_network_session_parameter_matchmaking_rematch_data rematch;
        const s_network_session_parameter_matchmaking_rematch_data *existing_teams;
        if (group_parameters->matchmaking_rematch_data.get_allowed())
            existing_teams
                = group_parameters->matchmaking_rematch_data.get();
        else
            existing_teams = &rematch;
        memset(&rematch, 0, sizeof(rematch));
        for (long team_index = 0; team_index < 16; team_index++) {
            long count = (long)count_bits(team_masks[team_index]);
            if (count > 1) {
                unsigned long valid_peer_mask = membership->m_current_membership.valid_peer_mask.get_bits_direct()[0];
                unsigned long excluded_mask = valid_peer_mask ^ team_masks[team_index];
                long host_peer_index = group_session->find_good_peer_to_be_host(
                    _bandwidth_configuration_host_preference_connectivity, false, false, false,
                    count, excluded_mask, 0, false).host_peer_index;
                if (host_peer_index != -1) {
                    const s_network_session_parameter_matchmaking_rematch_team_data *existing_team = &existing_teams->teams[team_index];
                    s_network_session_parameter_matchmaking_rematch_team_data *team = &rematch.teams[team_index];
                    team->host_peer_index = host_peer_index;
                    team->rematch_peer_mask = team_masks[team_index];
                    if (team->host_peer_index == existing_team->host_peer_index &&
                        existing_team->join_data_valid) {
                        team->join_data_valid = true;
                        memcpy(&team->host_description, &existing_team->host_description, sizeof(team->host_description));
                    }
                    rematch.team_rematch_mask |= 1UL << team_index;
                }
            }
        }
        group_parameters->matchmaking_rematch_data.set(&rematch);
    }
    {
        c_network_session *tail_group_session = get_manager()->get_group_session();
        c_network_session_parameters::s_network_session_parameters_internal *tail_parameters =
            &tail_group_session->get_session_parameters()->m_parameters;
        s_replicated_life_cycle_matchmaking_progress progress_update;
        memset(&progress_update, 0, sizeof(progress_update));
        progress_update.type = 5;
        progress_update.post_match = state;
        tail_parameters->matchmaking_progress.set(&progress_update);
    }
}
