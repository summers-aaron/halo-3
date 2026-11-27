#include "networking/session/network_session_parameters_generic_set.h"
// networkinglibxbox/life_cycle_handler_matchmaking_arbitration.obj: from the records 0x3e7b48 0x3e80d8 0x3e8250 0x3e8388 0x3e8530 0x3e86c0 0x3e8890 0x3e8900 0x3e8a48 0x3e8c10 0x3e8e18
// @flags /TP /O2 /J /Oc
#include "interface/user_interface_controller.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/network_banhammer.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "shell/shell.h"
// @address 0x3e80d8 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_arbitration@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e8250 ?enter@c_life_cycle_state_handler_matchmaking_arbitration@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
// @address 0x3e8388 ?exit@c_life_cycle_state_handler_matchmaking_arbitration@@UAAXPAVc_life_cycle_state_handler@@@Z
// @address 0x3e8530 ?update_arbitration@c_life_cycle_state_handler_matchmaking_arbitration@@AAAXXZ
// @address 0x3e86c0 ?session_membership_matches_arbitration_registration_results@c_life_cycle_state_handler_matchmaking_arbitration@@AAA_NXZ
// @address 0x3e8890 ?mark_arbitration_complete@c_life_cycle_state_handler_matchmaking_arbitration@@AAAXXZ
// @address 0x3e8900 ?update_start_match@c_life_cycle_state_handler_matchmaking_arbitration@@AAAXXZ
// @address 0x3e8a48 ?update_initial_stats_write@c_life_cycle_state_handler_matchmaking_arbitration@@AAAXXZ
// @address 0x3e8c10 ?ready_to_start@c_life_cycle_state_handler_matchmaking_arbitration@@AAA_NXZ
// @address 0x3e8e18 ?update_repeated_play@c_life_cycle_state_handler_matchmaking_arbitration@@AAAXXZ
#include "cseries/cseries_types.h"
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_arbitration.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/session/network_managed_session.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/network_leaderboard_write.h"
#include "networking/session/network_session_membership.h"
#include "networking/logic/network_arbitration_globals_types.h"

class c_network_session;
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"

#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"

#include "networking/session/network_managed_session_status.h"

#include "networking/online/online_arbitration_types.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_arbitration.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_session_interface_globals.h"
struct s_networking_join_data;
#include "networking/network_configuration_types.h"
#include "networking/network_time_types.h"
#include "networking/network_configuration.h"
#include "xbox/xnet_services.h"
extern "C" {
#include "game/multiplayer_game_hopper_playability.h"
}
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/logic/network_banhammer.h"



#include "networking/logic/network_banhammer_controller_types.h"

#include "networking/logic/logic_matchmaking_composition_data_types.h"

static __forceinline void life_cycle_arbitration_flag_set(unsigned int *flags, unsigned int bit)
{
    *flags |= bit;
}

extern "C" {
// @address 0x8c447c g_network_time_globals
}
struct s_network_configuration;
static __forceinline unsigned long arbitration_network_time_get() {
    if (g_network_time_globals.enabled) return g_network_time_globals.time;
    return GetTickCount();
}
static __forceinline long arbitration_network_time_since(unsigned long time) {
    return (long)(arbitration_network_time_get() - time);
}
static __forceinline bool life_cycle_arbitration_flag_test(unsigned int flags, long bit) {
    bool result = (flags & (1UL << bit)) != 0;
    return result;
}

static __forceinline e_network_session_mode life_cycle_arbitration_session_mode(const c_network_session *session)
{
    const c_network_session_parameter_session_mode *parameter = &session->get_session_parameters()->m_parameters.session_mode;
    e_network_session_mode mode = _network_session_mode_none;
    if (parameter->get_allowed())
        mode = parameter->m_data.mode;
    return mode;
}

e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_arbitration::update_for_state_transition()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    c_network_session *squad_session = manager->get_active_squad_session();
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    switch (life_cycle_arbitration_session_mode(group_session)) {
    case _network_session_mode_matchmaking_arbitrating:
        result = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_disbanding:
        if (((const c_network_session *)(squad_session))->is_host())
            squad_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
        result = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_setup:
    case _network_session_mode_in_game:
    case _network_session_mode_matchmaking_choosing_game:
        manager->request_state_change(_life_cycle_state_matchmaking_select_host, 0, 0);
        result = _life_cycle_state_transition_change;
        break;
    }
    if (result == _life_cycle_state_transition_continue) {
        result = _life_cycle_state_transition_none;
        switch (life_cycle_arbitration_session_mode(squad_session)) {
        case _network_session_mode_matchmaking_slave:
            result = _life_cycle_state_transition_continue;
            break;
        case _network_session_mode_matchmaking_start:
            manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            result = _life_cycle_state_transition_change;
            break;
        }
    }
    return result;
}

void c_life_cycle_state_handler_matchmaking_arbitration::exit(c_life_cycle_state_handler *next_handler)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_active_squad_session();
    c_life_cycle_state_handler::exit(next_handler);
    if (((const c_network_session *)(session))->is_host()) {
        c_matchmaking_quality *quality = manager->get_matchmaking_quality();
        long arbitration_time = arbitration_network_time_since(arbitration_start_time) / 1000;
        quality->set_arbitration_time(arbitration_time);
        if (next_handler->get_state() != 13) {
            c_network_session_parameters *parameters = session->get_session_parameters();
            if (parameters->m_parameters.matchmaking_abort_requested.get_allowed() && *parameters->m_parameters.matchmaking_abort_requested.get())
                quality->set_aborted();
            if (!life_cycle_arbitration_flag_test(arbitration_flags, 18) &&
                arbitration_network_time_since(arbitration_start_time) >= g_network_configuration.matchmaking_arbitration_registration_timeout_seconds * 1000)
                quality->set_registration_timed_out();
            else if (life_cycle_arbitration_flag_test(arbitration_flags, 18) &&
                arbitration_network_time_since(completion_start_time) >= g_network_configuration.matchmaking_arbitration_completion_timeout_seconds * 1000)
                quality->set_completion_timed_out();
            quality->set_left_arbitration();
            if (life_cycle_arbitration_flag_test(arbitration_flags, 16))
                quality->set_not_enough_hosts();
            ((c_matchmaking_quality *)quality)->submit_to_webstats();
        }
    }
}

void c_life_cycle_state_handler_matchmaking_arbitration::mark_arbitration_complete()
{
    bool established;
    g_network_session_interface_globals.peer.flags |= 4;
    life_cycle_arbitration_flag_set(&arbitration_flags, 8);
    established = ((const c_network_session *)(m_manager->m_group_session))->is_host();
    if (established) {
        life_cycle_arbitration_flag_set(&arbitration_flags, 0x10);
    }
}

bool c_life_cycle_state_handler_matchmaking_arbitration::ready_to_start()
{
    bool arbitration_complete = false;
    bool parameters_transmitted = false;
    bool initial_stats_written = false;
    bool final_stats_written = false;
    c_network_session *group_session = get_manager()->get_group_session();
    unsigned int flags = arbitration_flags;
    if (life_cycle_arbitration_flag_test(flags, 3) && !life_cycle_arbitration_flag_test(flags, 8))
        arbitration_complete = true;
    if (life_cycle_arbitration_flag_test(flags, 7) &&
        group_session->get_session_parameters()->parameters_transmitted_to_peers(0x100))
        parameters_transmitted = true;
    if (life_cycle_arbitration_flag_test(flags, 10) && !life_cycle_arbitration_flag_test(flags, 11) &&
        group_session->get_session_membership_for_update()->peer_property_flag_test(_peer_property_flag_test_all,
        _network_session_peer_properties_status_start_match_complete_bit))
        initial_stats_written = true;
    if (life_cycle_arbitration_flag_test(flags, 13) && !life_cycle_arbitration_flag_test(flags, 14) &&
        group_session->get_session_membership_for_update()->peer_property_flag_test(_peer_property_flag_test_all,
        _network_session_peer_properties_status_initial_stats_written_bit))
        final_stats_written = true;
    return arbitration_complete && parameters_transmitted && initial_stats_written && final_stats_written;
}

static __forceinline const s_network_session_matchmaking_hopper *life_cycle_arbitration_hopper(c_network_session *session)
{
    c_network_session_parameter_matchmaking_hopper *parameter
        = (c_network_session_parameter_matchmaking_hopper *)&session->get_session_parameters()->m_parameters.matchmaking_hopper;
    const s_network_session_matchmaking_hopper *result = 0;
    if (parameter->get_allowed())
        result = &parameter->m_data;
    return result;
}
static __forceinline const game_player_options *life_cycle_arbitration_initial_stats_players(c_network_session *session)
{
    c_network_session_parameter_initial_participants *parameter = &session->get_session_parameters()->m_parameters.initial_participants;
    const game_player_options *result = 0;
    if (parameter->get_allowed() && parameter->m_data.valid)
        result = parameter->m_data.player_options;
    return result;
}
static __forceinline const s_network_session_matchmaking_hopper *life_cycle_arbitration_initial_stats_hopper_get(
    const c_network_session_parameters *parameters)
{
    const c_network_session_parameter_matchmaking_hopper *parameter = &parameters->m_parameters.matchmaking_hopper;
    const s_network_session_matchmaking_hopper *result = 0;
    if (parameter->get_allowed())
        result = (const s_network_session_matchmaking_hopper *)&parameter->m_data;
    return result;
}
static __forceinline const game_player_options *life_cycle_arbitration_initial_stats_players_get(const c_network_session_parameters *parameters)
{
    const c_network_session_parameter_initial_participants *parameter = &parameters->m_parameters.initial_participants;
    const game_player_options *result = 0;
    if (parameter->get_allowed() && parameter->m_data.valid)
        result = parameter->m_data.player_options;
    return result;
}
static __forceinline bool life_cycle_arbitration_player_valid(const c_network_session_membership *membership, long index)
{
    return (membership->m_current_membership.valid_player_mask.get_bits_direct()[index >> 5] & (1UL << (index & 31))) != 0;
}
static __forceinline bool life_cycle_arbitration_guest(const unsigned __int64 *xuid)
{
    unsigned short high = *(const unsigned short *)xuid;
    return (high & 15) == 9 && (high & 192) != 0;
}
static __forceinline void life_cycle_arbitration_missing_set(unsigned long *bits, long index)
{
    unsigned long mask = 1UL << (index & 31);
    bits[index >> 5] = bits[index >> 5] | mask;
}
static __forceinline bool life_cycle_arbitration_missing_clear(const unsigned long *bits)
{
    bool result = true;
    result &= (bits[0] == 0);
    return result;
}
#define life_cycle_arbitration_machine_id(address, machine_id, result) do { \
    (result) = false; \
    if (::online_local_xuid_is_online_enabled() && XNetXnAddrToMachineId((const XNADDR *)(address), (machine_id)) == 0) \
        (result) = true; \
} while (0)

bool c_life_cycle_state_handler_matchmaking_arbitration::session_membership_matches_arbitration_registration_results()
{
    bool result = true;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->m_group_session;
    const c_network_session_membership *membership = session->get_session_membership_for_update();
    const s_network_session_matchmaking_hopper *hopper = life_cycle_arbitration_hopper(session);
    if (hopper->is_ranked && membership->m_current_membership.player_count != arbitration_result.registrant_count) {
        result = false;
    } else {
        unsigned long missing_players[1];
        long player_index;
        memset(missing_players, 0, sizeof(missing_players));
        unsigned long *missing = missing_players;
        for (player_index = 0; player_index < 16; ++player_index) {
            if (life_cycle_arbitration_player_valid(membership, player_index)) {
                const s_network_session_player *player = &membership->m_current_membership.players[player_index];
                if (hopper->is_ranked || !life_cycle_arbitration_guest(&player->configuration.client.xuid)) {
                    const s_network_session_peer *peer = &membership->m_current_membership.peers[player->peer_index];
                    unsigned __int64 machine_id = 0;
                    bool found = false;
                    bool machine_id_valid;
                    life_cycle_arbitration_machine_id(peer, &machine_id, machine_id_valid);
                    if (machine_id_valid) {
                        long registrant_index;
                        for (registrant_index = 0; registrant_index < arbitration_result.registrant_count; ++registrant_index) {
                            const s_online_arbitration_registrant *registrant = &arbitration_result.registrants[registrant_index];
                            if (registrant->machine_id == machine_id && registrant->xuid == player->configuration.client.xuid) {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                        life_cycle_arbitration_missing_set(missing, player_index);
                }
            }
        }
        if (!life_cycle_arbitration_missing_clear(missing))
            result = false;
    }
    return result;
}

static __forceinline long life_cycle_arbitration_session_index(struct c_network_session *session)
{
    return ((c_network_session *)session)->m_managed_session_index;
}
static __forceinline const unsigned __int64 *life_cycle_arbitration_nonce(struct c_network_session *session)
{
    const c_generic_network_session_parameter<unsigned __int64> *p = &session->get_session_parameters()->m_parameters.matchmaking_arbitration_nonce;
    const unsigned __int64 *result = 0;
    if (p->get_allowed())
        result = &p->m_data;
    return result;
}
static __forceinline bool life_cycle_arbitration_update_test(const unsigned int *flags, long bit)
{
    bool result = ((*flags >> bit) & 1) != 0;
    return result;
}
static __forceinline void life_cycle_arbitration_update_set(unsigned int *flags, long bit, bool enabled)
{
    unsigned int mask = 1U << bit;
    *flags = (*flags & ~mask) | (enabled ? mask : 0);
}
static __forceinline bool life_cycle_arbitration_start_match_status_test(unsigned short flags, long bit)
{
    bool result = (flags & 1 << bit) != 0;
    return result;
}
static __forceinline void life_cycle_arbitration_start_match_mark_complete(unsigned int *flags)
{
    g_network_session_interface_globals.peer.flags |= 64;
    life_cycle_arbitration_update_set(flags, 10, 1);
}
static __forceinline void life_cycle_arbitration_start_match_mark_failed(unsigned int *flags)
{
    life_cycle_arbitration_start_match_mark_complete(flags);
    g_network_session_interface_globals.peer.flags |= 128;
    life_cycle_arbitration_update_set(flags, 11, 1);
}
static __forceinline bool life_cycle_arbitration_update_results(long index, s_online_arbitration_result *dest)
{
    bool result = false;
    if (index == network_arbitration_globals.index && network_arbitration_globals.status == 2) {
        memcpy(dest, (char *)&network_arbitration_globals + 16, sizeof(s_online_arbitration_result));
        result = true;
    }
    return result;
}
#define life_cycle_arbitration_update_failed() do { \
    mark_arbitration_complete(); \
    g_network_session_interface_globals.peer.flags |= 8; \
    life_cycle_arbitration_update_set(&arbitration_flags, 8, 1); \
} while (0)

static __forceinline void arbitration_lock_slots(c_network_session_membership *membership)
{
    membership->m_current_membership.request_modification = true;
    managed_session_modify_slot_counts(membership->m_session->m_managed_session_index,
        membership->m_current_membership.private_slot_count, membership->m_current_membership.public_slot_count,
        membership->m_current_membership.friends_only, true);
    membership->m_current_membership.update_number++;
    membership->m_local_membership_update_number++;
}

void c_life_cycle_state_handler_matchmaking_arbitration::enter(c_life_cycle_state_handler *previous_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous_handler, 0, 0);
    arbitration_flags = 0;
    memset(&arbitration_result, 0, sizeof(arbitration_result));
    arbitration_start_time = arbitration_network_time_get();
    completion_start_time = 0;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    if (((const c_network_session *)(group_session))->is_host()) {
        group_session->get_session_parameters()->clear_parameters_with_mask(256ui64);
        arbitration_flags |= 1;
        group_session->get_session_membership_for_update()->set_slot_counts(0, 0, false);
        arbitration_lock_slots(group_session->get_session_membership_for_update());
    }
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}

void c_life_cycle_state_handler_matchmaking_arbitration::update_arbitration()
{
    c_network_session *session = m_manager->m_group_session;
    long managed_session_index = life_cycle_arbitration_session_index(session);
    if (!life_cycle_arbitration_update_test(&arbitration_flags, 1)) {
        long status = network_arbitration_globals.status;
        if (status == 1) {
            if (!life_cycle_arbitration_update_test(&arbitration_flags, 2)) {
                life_cycle_arbitration_update_set(&arbitration_flags, 2, 1);
            }
        } else {
            unsigned __int64 nonce = 0;
            nonce = *life_cycle_arbitration_nonce(session);
            if (life_cycle_arbitration_update_test(&arbitration_flags, 2)) {
                life_cycle_arbitration_update_set(&arbitration_flags, 2, 0);
            }
            if (!network_arbitration_initiate(managed_session_index, nonce)) {
                life_cycle_arbitration_update_failed();
            }
            life_cycle_arbitration_update_set(&arbitration_flags, 1, 1);
        }
    }
    if (life_cycle_arbitration_update_test(&arbitration_flags, 1) && !life_cycle_arbitration_update_test(&arbitration_flags, 8)) {
        long status = network_arbitration_globals.status;
        switch (status) {
        case 2:
            {
                bool result = life_cycle_arbitration_update_results(managed_session_index, &arbitration_result);
                if (!result) {
                    life_cycle_arbitration_update_failed();
                } else {
                    mark_arbitration_complete();
                }
            }
            break;
        case 3:
            life_cycle_arbitration_update_failed();
            break;
        case 0:
            life_cycle_arbitration_update_failed();
            break;
        }
    }
}

void c_life_cycle_state_handler_matchmaking_arbitration::update_start_match()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    c_network_session_membership *membership = group_session->get_session_membership_for_update();
    long managed_session_index = life_cycle_arbitration_session_index(group_session);
    if (!life_cycle_arbitration_update_test(&arbitration_flags, 9)) {
        managed_session_game_start(managed_session_index);
        managed_session_game_start(life_cycle_arbitration_session_index(manager->get_active_squad_session()));
        life_cycle_arbitration_update_set(&arbitration_flags, 9, 1);
    }
    if (!life_cycle_arbitration_update_test(&arbitration_flags, 10) &&
        !life_cycle_arbitration_update_test(&arbitration_flags, 11)) {
        unsigned short status;
        managed_session_get_status(&status, managed_session_index);
        if (!life_cycle_arbitration_start_match_status_test(status, 8)) {
            if (life_cycle_arbitration_start_match_status_test(status, 9))
                life_cycle_arbitration_start_match_mark_complete(&arbitration_flags);
            else if (life_cycle_arbitration_start_match_status_test(status, 10))
                life_cycle_arbitration_start_match_mark_failed(&arbitration_flags);
        }
    }
    if (((const c_network_session *)(group_session))->is_host() &&
        membership->peer_property_flag_test_any_peer(_network_session_peer_properties_status_start_match_failed_bit))
        disband_group_session();
}

void c_life_cycle_state_handler_matchmaking_arbitration::update_initial_stats_write()
{
    c_network_session *group_session = m_manager->m_group_session;
    c_network_session_membership *membership = group_session->get_session_membership_for_update();
    long managed_session_index = life_cycle_arbitration_session_index(group_session);
    if (!life_cycle_arbitration_update_test(&arbitration_flags, 12)) {
        long write_status = network_leaderboard_get_write_status();
        if (write_status == 1) {
            if (!life_cycle_arbitration_update_test(&arbitration_flags, 15)) {
                life_cycle_arbitration_update_set(&arbitration_flags, 15, 1);
            }
        } else {
            const c_network_session_parameters *parameters =
                (const c_network_session_parameters *)group_session->get_session_parameters();
            const s_network_session_matchmaking_hopper *hopper = 0;
            const game_player_options *players = 0;
            if (life_cycle_arbitration_update_test(&arbitration_flags, 15)) {
                life_cycle_arbitration_update_set(&arbitration_flags, 15, 0);
            }
            hopper = life_cycle_arbitration_initial_stats_hopper_get(parameters);
            players = life_cycle_arbitration_initial_stats_players_get(parameters);
            network_leaderboard_write_initial_stats(managed_session_index, hopper->hopper_identifier, hopper->is_ranked,
                hopper->team_game, hopper->trueskill_draw_probability, hopper->trueskill_beta_performance_variation, hopper->trueskill_tau_dynamics_factor,
                hopper->experience_penalty_decrement, hopper->system_seconds, players);
            life_cycle_arbitration_update_set(&arbitration_flags, 12, 1);
        }
    }
    if (life_cycle_arbitration_update_test(&arbitration_flags, 12) && !life_cycle_arbitration_update_test(&arbitration_flags, 13)) {
        long write_status = network_leaderboard_get_write_status();
        if (write_status != 1) {
        if (write_status == 2) {
            g_network_session_interface_globals.peer.flags |= 0x100;
            life_cycle_arbitration_update_set(&arbitration_flags, 13, 1);
        } else if (write_status == 0) {
            g_network_session_interface_globals.peer.flags |= 0x40;
            life_cycle_arbitration_update_set(&arbitration_flags, 10, 1);
            g_network_session_interface_globals.peer.flags |= 0x200;
            life_cycle_arbitration_update_set(&arbitration_flags, 14, 1);
        }
        }
    }
    if (((const c_network_session *)(group_session))->is_host() &&
        membership->peer_property_flag_test_any_peer(_network_session_peer_properties_status_initial_stats_write_failed_bit))
        disband_group_session();
}

struct s_network_banhammer_globals;

static __forceinline const s_network_session_matchmaking_hopper *life_cycle_arbitration_repeated_play_hopper_get(
    const c_network_session_parameter_matchmaking_hopper *parameter)
{
    const s_network_session_matchmaking_hopper *result = 0;
    if (parameter->get_allowed())
        result = &parameter->m_data;
    return result;
}
static __forceinline void life_cycle_arbitration_repeated_play_coefficient_set(long controller, long coefficient)
{
    s_network_banhammer_controller_data *state = &g_network_banhammer_globals.controllers[controller];
    if (state->flags & 1) {
        state->repeated_play_index = coefficient;
        user_interface_controller_update_network_properties((e_controller_index)controller);
    }
}
static __forceinline void life_cycle_arbitration_repeated_play_banned_set(long controller, bool banned)
{
    s_network_banhammer_controller_data *state = &g_network_banhammer_globals.controllers[controller];
    if (state->flags & 1) {
        if (banned)
            state->flags |= 2;
        else
            state->flags &= ~2;
        user_interface_controller_update_network_properties((e_controller_index)controller);
    }
}

void c_life_cycle_state_handler_matchmaking_arbitration::update_repeated_play()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    const c_network_session_membership *members = group_session->get_session_membership_for_update();
    const s_network_session_matchmaking_hopper *configuration = life_cycle_arbitration_repeated_play_hopper_get(
        (const c_network_session_parameter_matchmaking_hopper *)&group_session->get_session_parameters()->m_parameters.matchmaking_hopper);
    long local_peer = members->m_local_peer_index;
    long local_player_found_count = 0;
    for (long index = ((const c_network_session_membership *)members)->get_first_player();
        index != -1;
        index = ((const c_network_session_membership *)members)->get_next_player(index))
    {
        const s_network_session_player *local_player = &members->m_current_membership.players[index];
        if (local_player->peer_index == local_peer) {
            long team = local_player->configuration.host.team_index;
            long opponent_count = 0;
            unsigned __int64 opponents[16];
            for (long other = ((const c_network_session_membership *)members)->get_first_player();
                other != -1;
                other = ((const c_network_session_membership *)members)->get_next_player(other))
            {
                const s_network_session_player *opponent = &members->m_current_membership.players[other];
                if (team != opponent->configuration.host.team_index)
                    opponents[opponent_count++] = opponent->configuration.client.xuid;
            }
            long coefficient = 0;
            bool allowed = false;
            long result_coefficient;
            bool result_allowed;
            if (opponent_count > 0 && configuration->repeated_opponents_to_consider_for_penalty > 0) {
                multiplayer_game_hopper_update_player_history(local_player->controller_index, configuration->hopper_identifier, opponents, opponent_count);
                multiplayer_game_hopper_client_compute_repeated_play_adjustment_weight(local_player->controller_index, configuration->hopper_identifier,
                    configuration->repeated_opponents_to_consider_for_penalty, configuration->repeated_opponents_experience_threshold,
                    configuration->repeated_opponents_skill_throttle_start, configuration->repeated_opponents_skill_throttle_stop, opponents,
                    opponent_count, &coefficient, &allowed);
                result_coefficient = coefficient;
                result_allowed = allowed;
            } else {
                result_coefficient = 100;
                result_allowed = true;
            }
            life_cycle_arbitration_repeated_play_coefficient_set(local_player->controller_index, result_coefficient);
            life_cycle_arbitration_repeated_play_banned_set(local_player->controller_index, !result_allowed);
            local_player_found_count++;
        }
    }
    g_network_session_interface_globals.peer.flags |= 0x20;
    life_cycle_arbitration_flag_set(&arbitration_flags, 64);
}

static __forceinline bool life_cycle_handler_matchmaking_arbitration_peers_registered(
    const c_network_session_membership *membership)
{
    const c_network_session_membership *members = membership;
    bool result = true;
    for (long peer = 0; peer < 16; ++peer) {
        if ((members->m_current_membership.valid_peer_mask.get_bits_direct()[peer >> 5] & (1UL << (peer & 31))) != 0) {
            bool remote = true;
            if (members->m_current_membership.host_peer_index == peer)
                remote = false;
            if (remote
                && !life_cycle_arbitration_update_test((const unsigned int *)&members->m_current_membership.peers[peer].properties.status_flags.m_storage, 2)) {
                result = false;
                break;
            }
        }
    }
    return result;
}
static __forceinline void life_cycle_handler_matchmaking_arbitration_begin_completion(
    unsigned long *start_time, unsigned int *flags)
{
    *start_time = arbitration_network_time_get();
    *flags |= 1UL << 18;
}
static __forceinline bool life_cycle_handler_matchmaking_arbitration_composition_valid(c_network_session *session)
{
    s_network_session_matchmaking_composition composition;
    bool valid = false;
    if (network_session_build_matchmaking_composition((const c_network_session *)session, &composition)
        == _network_session_matchmaking_composition_build_result_success)
        valid = true;
    return valid;
}

// @address 0x3e7b48 ?update@c_life_cycle_state_handler_matchmaking_arbitration@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_arbitration::update()
{
    c_network_session *group_session = get_manager()->get_group_session();
    c_network_session_membership *membership = group_session->get_session_membership_for_update();
    if (((const c_network_session *)(group_session))->is_host()) {
        s_replicated_life_cycle_matchmaking_progress progress;
        memset(&progress, 0, sizeof(progress));
        progress.type = 3;
        c_network_session_parameters *parameters = group_session->get_session_parameters();
        parameters->m_parameters.matchmaking_progress.set(&progress);
    }
    if (((const c_network_session *)(group_session))->is_host() && !life_cycle_arbitration_flag_test(arbitration_flags, 0)) {
        disband_group_session();
    } else if (((const c_network_session *)(group_session))->is_host() && !life_cycle_arbitration_flag_test(arbitration_flags, 18) &&
        arbitration_network_time_since(arbitration_start_time) >= g_network_configuration.matchmaking_arbitration_registration_timeout_seconds * 1000) {
        disband_group_session();
    } else if (((const c_network_session *)(group_session))->is_host() && life_cycle_arbitration_flag_test(arbitration_flags, 18) &&
        arbitration_network_time_since(completion_start_time) >= g_network_configuration.matchmaking_arbitration_completion_timeout_seconds * 1000) {
        disband_group_session();
    } else if (!(((const c_network_session *)(group_session))->is_host() && !membership->all_peers_established())
        && !(((const c_network_session *)(group_session))->is_host() && !network_session_membership_is_synchronized(membership))) {
        if (((const c_network_session *)(group_session))->is_host() && !life_cycle_handler_matchmaking_arbitration_composition_valid(group_session)) {
        disband_group_session();
    } else if (((const c_network_session *)(group_session))->is_host() && !pick_teams_for_group_internal(false, 0)) {
        disband_group_session();
    } else {
        c_network_session_parameters *parameters = group_session->get_session_parameters();
        if (!life_cycle_arbitration_flag_test(arbitration_flags, 3) && !((const c_network_session *)(group_session))->is_host())
            update_arbitration();
        unsigned int flags = arbitration_flags;
        if (!life_cycle_arbitration_flag_test(flags, 4) && ((const c_network_session *)(group_session))->is_host()) {
            if (!life_cycle_arbitration_flag_test(flags, 18)) {
                life_cycle_handler_matchmaking_arbitration_begin_completion(&completion_start_time, &arbitration_flags);
            }
            if (membership->peer_property_flag_test_any_peer(_network_session_peer_properties_status_arbitration_failed_bit))
                disband_group_session();
            else if (life_cycle_handler_matchmaking_arbitration_peers_registered(membership))
                update_arbitration();
        }
        flags = arbitration_flags;
        if (life_cycle_arbitration_flag_test(flags, 4) && ((const c_network_session *)(group_session))->is_host()) {
            if (life_cycle_arbitration_flag_test(flags, 8)) {
                disband_group_session();
            } else if (!life_cycle_arbitration_flag_test(flags, 5)) {
                if (!session_membership_matches_arbitration_registration_results()) {
                    disband_group_session();
                } else if (!pick_teams_for_group_internal(true, 0)) {
                    disband_group_session();
                } else if (!((c_network_session *)group_session)->has_multiple_potential_hosts()) {
                    arbitration_flags |= 1UL << 16;
                    disband_group_session();
                } else {
                    g_network_session_interface_globals.peer.flags |= 16;
                    arbitration_flags |= 1UL << 5;
                }
            } else if (!life_cycle_arbitration_flag_test(flags, 7) &&
                membership->peer_property_flag_test(_peer_property_flag_test_all, _network_session_peer_properties_status_repeated_play_complete_bit)) {
                bool coefficients_ready = true;
                const c_network_session_membership *members = membership;
                for (long player = (membership)->get_first_player(); player != -1;
                    player = (membership)->get_next_player(player))
                {
                    if (members->m_current_membership.players[player].configuration.client.repeated_play_coefficient == -1)
                        coefficients_ready = false;
                }
                if (coefficients_ready) {
                    if (!setup_initial_participants((c_network_session *)group_session))
                        disband_group_session();
                    else
                        arbitration_flags |= 1UL << 7;
                }
            }
        }
        flags = arbitration_flags;
        if (!life_cycle_arbitration_flag_test(flags, 6) &&
            membership->peer_property_flag_test(_peer_property_flag_test_host, _network_session_peer_properties_status_arbitration_validated_bit)) {
            if (!network_banhammer_test_local_player_xuids(this)) {
                if (!life_cycle_arbitration_flag_test(flags, 17))
                    life_cycle_arbitration_update_set(&arbitration_flags, 17, 1);
            } else {
                update_repeated_play();
            }
        }
        if (parameters->m_parameters.initial_participants.get_allowed())
            update_start_match();
        if (life_cycle_arbitration_flag_test(arbitration_flags, 10))
            update_initial_stats_write();
        if (((const c_network_session *)(group_session))->is_host() && ready_to_start())
            group_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_setup);
    }
    }
}
