// networkinglibxbox/life_cycle_handler_in_match.obj: from the records 0x3ea680 0x3ea990 0x3eaa38 0x3eab30
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x8c447c g_network_time_globals
// @address 0x95e608 main_game_globals
// @address 0x808560 g_simulation_globals
#include <string.h>
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "main/main_game.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "simulation/simulation_globals.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_game.h"
#include "cseries/crt_xdk.h"
#include "networking/network_time_since_access.h"
#include "game/game_state_accessors.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"
#include "simulation/simulation_abort.h"
#include "networking/logic/life_cycle/life_cycle_in_match.h"
#include "networking/logic/life_cycle/life_cycle_session.h"

namespace
{
static __forceinline bool life_cycle_handler_in_match_in_group_session(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized)
    {
        c_network_session *group = g_network_life_cycle_globals.manager.m_group_session;
#ifdef HALO3_64BIT
        if (group->current_local_state() != 0)
#else
        if (((s_life_cycle_handler_in_match_session *)group)->group_state != 0)
#endif
        {
            *session = group;
            result = true;
        }
    }
    return result;
}
}

// @address 0x3ea990 ?update_for_state_transition@c_life_cycle_state_handler_in_match@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_in_match::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_group_session();
    c_network_session_parameters *parameters = session->get_session_parameters();
    if (m_match_start_time == -1)
    {
        transition = _life_cycle_state_transition_continue;
    }
    else
    {
        e_network_session_mode mode = parameters->m_parameters.session_mode.get();
        switch (mode)
        {
        case _network_session_mode_in_game:
            transition = _life_cycle_state_transition_continue;
            break;
        default:
            manager->request_state_change(_life_cycle_state_end_match_write_stats, 0, 0);
            transition = _life_cycle_state_transition_change;
            break;
        }
    }
    return transition;
}

// @address 0x3eaa38 ?enter@c_life_cycle_state_handler_in_match@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_in_match::enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous, 0, 0);
    m_match_start_time = -1;
    m_time = network_time_get();
    m_flags.set(0, true);
    m_index = 0xffffffff;
    m_simulation_aborted = false;
    c_network_session *session;
    if (life_cycle_handler_in_match_in_group_session(&session))
    {
#ifdef HALO3_64BIT
        session->set_close_session_for_matchmaking(true);
#else
        ((s_life_cycle_handler_in_match_session *)session)->speculative_host_migration_enabled = true;
#endif
    }
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking) network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}

// @address 0x3eab30 ?exit@c_life_cycle_state_handler_in_match@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_in_match::exit(c_life_cycle_state_handler *next)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *active_squad_session = manager->get_active_squad_session();
    c_network_session *session;
    if (life_cycle_handler_in_match_in_group_session(&session))
    {
#ifdef HALO3_64BIT
        session->set_close_session_for_matchmaking(false);
#else
        ((s_life_cycle_handler_in_match_session *)session)->speculative_host_migration_enabled = false;
#endif
    }
    if (game_in_progress() && tls_game_globals()->options.game_instance == m_match_start_time)
        simulation_abort_immediate(_simulation_abort_reason_leaving_match);
    if (active_squad_session->is_host())
    {
#ifdef HALO3_64BIT
        c_matchmaking_quality *quality = manager->get_matchmaking_quality();
#else
        c_matchmaking_quality *quality = &((s_life_cycle_handler_in_match_manager *)manager)->matchmaking_quality;
#endif
        quality->in_match_time = network_time_since(m_time) / 1000;
        quality->submit_to_webstats();
    }
    if (next->get_state() != _life_cycle_state_end_match_write_stats && !main_game_globals.game_change_in_progress)
        main_menu_launch();
}

/* ---- in_match update (0x3ea680) ---- */
__forceinline bool c_life_cycle_state_handler_in_match::group_has_required_session_parameters() const
{
    c_network_session *session = get_manager()->get_group_session();
    return session->get_session_parameters()->parameters_valid(0x15c0609dULL << 6);
}

// @address 0x3ea680 ?update@c_life_cycle_state_handler_in_match@@UAAXXZ
void c_life_cycle_state_handler_in_match::update()
{
    bool leave_group = false;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_group_session();
    c_network_session_parameters *params = session->get_session_parameters();
    if (m_flags.test(0))
    {
        if (!main_game_globals.game_change_in_progress)
        {
            if (!group_has_required_session_parameters())
            {
                if (session->is_host())
                    leave_group = true;
            }
            else
            {
                const s_network_session_matchmaking_hopper *hopper = params->m_parameters.matchmaking_hopper.get();
                const s_network_session_matchmaking_hopper_list *hopper_list = params->m_parameters.matchmaking_hopper_list.get();
                s_game_matchmaking_options options;
                memset(&options, 0, sizeof(options));
                options.hopper_identifier = hopper->hopper_identifier;
                options.xlast_index = hopper->xlast_index;
                options.is_ranked = hopper->is_ranked;
                options.is_team_game = hopper->team_game;
                for (long i = 0; i < hopper_list->hopper_count; i++)
                {
                    if (hopper_list->hoppers[i].identifier == options.hopper_identifier)
                    {
                        ::utf8_string_to_wchar_string((utf8 const *)hopper_list->hoppers[i].name, options.hopper_name, 32, (long *)(0));
                        break;
                    }
                }
                options.trueskill_draw_probability = hopper->trueskill_draw_probability;
                options.trueskill_beta_performance_variation = hopper->trueskill_beta_performance_variation;
                options.trueskill_tau_dynamics_factor = hopper->trueskill_tau_dynamics_factor;
                options.team_count = hopper->experience_base_increment;
                options.player_count = hopper->experience_penalty_decrement;
                if (life_cycle_in_game_initialize_options_from_session(session, &options))
                    m_flags.set(0, false);
                else
                    leave_group = true;
            }
        }
    }
    else if (m_match_start_time == -1)
    {
        m_match_start_time = c_life_cycle_state_handler_in_game::end_load_map(session);
    }
    else
    {
        life_cycle_precache_mainmenu_after_game(this);
        bool ready = g_simulation_globals.initialized != 0 && g_simulation_globals.aborted != 0;
        if (ready)
        {
            if (!life_cycle_handle_simulation_abort(session, &m_simulation_aborted, &m_index))
                leave_group = true;
        }
        else if (simulation_starting_up())
        {
            game_globals_storage *game_globals = tls_game_globals();
            bool finished = game_globals->game_finished != 0 && game_globals->finish_ticks == 0;
            if (finished && session->is_host())
                params->m_parameters.session_mode.set(_network_session_mode_end_game);
        }
    }
    if (leave_group)
    {
        session->disband_and_reestablish_as_host(true);
        params->m_parameters.session_mode.set(_network_session_mode_end_game);
        manager->request_state_change(_life_cycle_state_end_match_write_stats, 0, 0);
    }
}
