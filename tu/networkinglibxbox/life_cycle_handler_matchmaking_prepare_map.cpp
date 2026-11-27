#include "networking/session/network_session_parameter_game_types.h"
#include "networking/session/network_session_parameters_generic_set.h"
// networkinglibxbox/life_cycle_handler_matchmaking_prepare_map.obj: from the records 0x3e9000 0x3e93b8 0x3e95f0 0x3e9730 0x3e9838 0x3e9920 0x3e9cd0 0x3e9ec0 0x3ea068 0x3ea270 0x3ea398 0x3ea4b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "memory/static_arrays.h"
#include "game/map_variant.h"
#include "cseries/cseries.h"
#include "cseries/bit_operations.h"
// @address 0x3e9838 ?exit@c_life_cycle_state_handler_matchmaking_prepare_map@@UAAXPAVc_life_cycle_state_handler@@@Z
// @address 0x3a1600 ?is_host@c_network_session@@QBA_NXZ
// @address 0x3ed6c8 ?set@c_network_session_parameter_session_mode@@QAA_NW4e_network_session_mode@@@Z
// @address 0x412e50 ?set@c_network_session_parameter_map_variant@@QAA_NPBVc_map_variant@@@Z
// @address 0x413140 ?set@c_network_session_parameter_game_variant@@QAA_NPBVc_game_variant@@@Z
// @address 0x3e9ec0 ?update_countdown@c_life_cycle_state_handler_matchmaking_prepare_map@@QAAXPAUs_replicated_life_cycle_matchmaking_progress_configuring_match@@@Z
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "networking/logic/network_join.h"
#include "networking/session/network_session.h"
#include "main/levels.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/online/online_url.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_prepare_map.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "game/game_variant.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/network_configuration_types.h"

struct utf8;

#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "networking/session/network_session_parameters.h"
#include "networking/network_time_types.h"
#include "game/multiplayer_game_hopper_download_storage.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/network_configuration.h"

typedef char prepare_map_game_downloader_offset[(sizeof(void *) != 4 || ((offsetof(s_hopper_update_globals, game_variant_downloader) == 0xf878))) ? 1 : -1];
typedef char prepare_map_map_downloader_offset[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, map_variant_downloader) == 0x10a1c)) ? 1 : -1];
typedef char prepare_map_game_variant_offset[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_variant) == 0x1f9e8)) ? 1 : -1];
typedef char prepare_map_map_variant_offset[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, map_variant) == 0x1fc50)) ? 1 : -1];

typedef unsigned short wchar;

// 0x3e95f0
e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_prepare_map::update_for_state_transition()
{
    c_life_cycle_state_manager *state_manager = m_manager;
    c_network_session *group = state_manager->m_group_session;
    c_network_session *squad = state_manager->m_active_squad_session;
    long transition = 0;
    switch (group->get_session_parameters()->m_parameters.session_mode.get()) {
    case _network_session_mode_matchmaking_choosing_game:
        transition = 1;
        break;
    case _network_session_mode_matchmaking_disbanding:
        if (((c_network_session *)squad)->is_host())
            squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
        transition = 1;
        break;
    case _network_session_mode_in_game:
        state_manager->request_state_change(_life_cycle_state_in_match, 0, 0);
        transition = 2;
        break;
    }
    if (transition == 1) {
        transition = 0;
        switch (squad->get_session_parameters()->m_parameters.session_mode.get()) {
        case _network_session_mode_matchmaking_slave:
            transition = 1;
            break;
        case _network_session_mode_matchmaking_start:
            state_manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            transition = 2;
            break;
        }
    }
    return (e_life_cycle_state_transition_type)transition;
}

static __forceinline void prepare_map_ustrnzcpy(wchar *dest, const wchar *source, int size)
{
    for (int i = 0; i <= size - 1; i++, source++) {
        if ((dest[i] = *source) == 0)
            break;
    }
    dest[size - 1] = 0;
}

void c_life_cycle_state_handler_matchmaking_prepare_map::get_progress(s_life_cycle_matchmaking_progress *progress)
{
    c_network_session *group_session = (c_network_session *)m_manager->m_group_session;
    c_network_session *squad_session = (c_network_session *)m_manager->m_active_squad_session;
    c_network_session_parameters *parameters = group_session->get_session_parameters();
    c_network_session_membership *membership = group_session->get_session_membership_for_update();
    if (parameters->m_parameters.matchmaking_progress.get_allowed() && parameters->m_parameters.matchmaking_hopper_list.get_allowed() &&
        parameters->m_parameters.matchmaking_hopper.get_allowed())
    {
        const s_replicated_life_cycle_matchmaking_progress *status = parameters->m_parameters.matchmaking_progress.get();
        if (status->type == _life_cycle_matchmaking_progress_preparing_map)
        {
            progress->progress_type = _life_cycle_matchmaking_progress_preparing_map;
            progress->configuring_match = status->configuring_match;
            long local_peer_index = membership->local_peer_index();
            progress->ready_player_mask = 0;
            for (long player_index = membership->get_first_player(); player_index != NONE; player_index = membership->get_next_player(player_index))
            {
                s_network_session_player *player = membership->get_player(player_index);
                if (player->peer_index == local_peer_index)
                    SET_BIT(progress->ready_player_mask, player_index, player->configuration.client.ready);
            }

            if (parameters->m_parameters.game_variant.get_allowed() && parameters->m_parameters.map_variant.get_allowed() &&
                parameters->m_parameters.map.get_allowed())
            {
                const c_game_variant *game_variant = parameters->m_parameters.game_variant.get();
                const c_map_variant *map_variant = parameters->m_parameters.map_variant.get();
                const s_network_session_parameter_map *lobby = parameters->m_parameters.map.get();
                progress->lobby_type = lobby->map_id;
                progress->game_engine_index = game_variant->game_engine_index;
                prepare_map_ustrnzcpy((wchar *)progress->game_variant_name, (const wchar *)game_variant->get_name(), 32);
                prepare_map_ustrnzcpy((wchar *)progress->map_variant_name, (const wchar *)map_variant->m_metadata.name, 32);
            }
            else
            {
                progress->game_engine_index = 0;
                progress->lobby_type = NONE;
                memset(progress->game_variant_name, 0, sizeof(progress->game_variant_name));
                memset(progress->map_variant_name, 0, sizeof(progress->map_variant_name));
            }

            const s_network_session_matchmaking_hopper_list *hopper = parameters->m_parameters.matchmaking_hopper_list.get();
            const s_network_session_matchmaking_hopper *hopper_map = parameters->m_parameters.matchmaking_hopper.get();
            for (long map_index = 0; map_index < hopper->hopper_count; map_index++)
            {
                if (hopper->hoppers[map_index].identifier == hopper_map->hopper_identifier)
                {
                    ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)hopper->hoppers[map_index].name,
                        (wchar_t *)(progress->map_name), 32, (long *)(NULL));
                    break;
                }
            }
            progress->map_flags = hopper_map->is_ranked;
            progress->map_options = hopper_map->team_game;
            progress->squad_player_count = squad_session->get_session_membership()->get_player_count();
            progress->group_player_count = membership->get_player_count();
        }
    }
}


extern "C" {
// @address 0x8c447c g_network_time_globals
}

static __forceinline unsigned long prepare_map_network_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    return GetTickCount();
}

static __forceinline long prepare_map_network_time_since(unsigned long time)
{
    return (long)(prepare_map_network_time_get() - time);
}

// 0x3e9838
void c_life_cycle_state_handler_matchmaking_prepare_map::exit(c_life_cycle_state_handler *next_handler)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *session = (c_network_session *)manager->m_active_squad_session;
    c_life_cycle_state_handler::exit(next_handler);
    if (session->is_host())
    {
        c_matchmaking_quality *quality = manager->get_matchmaking_quality();
        long prepare_map_time = prepare_map_network_time_since(m_start_time) / 1000;
        quality->set_prepare_map_time(prepare_map_time);
        if (next_handler->get_state() != 15)
        {
            c_network_session_parameters *parameters = session->get_session_parameters();
            if (parameters->m_parameters.matchmaking_abort_requested.get_allowed() && *parameters->m_parameters.matchmaking_abort_requested.get())
                quality->set_aborted();
            quality->set_left_prepare_map();
            ((c_matchmaking_quality *)quality)->submit_to_webstats();
        }
    }
}


namespace {
static __forceinline s_hopper_update_globals &prepare_map_download_storage()
{
    return *(s_hopper_update_globals *)&g_multiplayer_game_hopper_globals;
}
static __forceinline long prepare_map_map_variant_download_status()
{
    long status = 0;
    if (prepare_map_download_storage().map_variant_failed)
        status = 3;
    else if (prepare_map_download_storage().map_variant_valid)
        status = 2;
    else if (prepare_map_download_storage().map_variant_downloader.m_url.m_string.m_string[0])
        status = 1;
    return status;
}
}


// 0x3e9cd0
// @address 0x3e9cd0 ?request_next_game_and_map@c_life_cycle_state_handler_matchmaking_prepare_map@@AAAXPAUs_replicated_life_cycle_matchmaking_progress_configuring_match@@@Z
void c_life_cycle_state_handler_matchmaking_prepare_map::request_next_game_and_map(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *group = manager->m_group_session;
    if (group->is_host())
    {
        c_network_session_membership *membership = group->get_session_membership_for_update();
        long player_count = membership->m_current_membership.player_count;
        progress->current_veto_count = 0;
        progress->max_veto_count = player_count;
        long player_index = membership->get_first_player();
        if (player_index != NONE)
        {
            do
            {
                if (membership->get_player(player_index)->configuration.client.ready)
                    progress->current_veto_count++;
                player_index = membership->get_next_player(player_index);
            } while (player_index != NONE);
        }
        long countdown_start = m_time2;
        bool time_enabled = g_network_time_globals.enabled;
        long time;
        unsigned long network_time;
        if (time_enabled)
        {
            network_time = g_network_time_globals.time;
            time = network_time;
        }
        else
        {
            time = GetTickCount();
            network_time = g_network_time_globals.time;
            time_enabled = g_network_time_globals.enabled;
        }
        long zero = 0;
        long seconds_remaining = g_network_configuration.matchmaking_veto_countdown -
            (time - countdown_start) / 1000 + 1;
        const long *countdown = seconds_remaining < 0 ? &zero : &seconds_remaining;
        progress->countdown_timer = *countdown;
        if (*countdown == 0)
        {
            if (progress->current_veto_count > progress->max_veto_count / 2)
            {
                const s_game_hopper_picked_game_collection *games = NULL;
                const c_generic_network_session_parameter<s_game_hopper_picked_game_collection> *parameter =
                    &group->get_session_parameters()->m_parameters.matchmaking_game_list;
                if (parameter->get_allowed())
                    games = parameter->get();
                long game_index = progress->game_map_index;
                const s_game_hopper_picked_game *vetoed_game = &games->games[game_index];
                c_matchmaking_quality *quality = manager->get_matchmaking_quality();
                quality->m_vetoed = true;
                memcpy(&quality->m_vetoed_game, vetoed_game, sizeof(s_game_hopper_picked_game));
                long next_game_index = game_index + 1;
                if (next_game_index >= 4)
                    disband_group_session();
                else
                {
                    progress->game_map_index = next_game_index;
                    m_flags.m_storage = (unsigned char)(m_flags.m_storage | 1);
                }
                progress->state = 2;
            }
            else
            {
                long value;
                if (time_enabled)
                    value = network_time;
                else
                    value = GetTickCount();
                m_time2 = value;
                progress->state = 4;
            }
        }
    }
}
// 0x3e9ec0


template<typename t_type> static __forceinline const t_type &prepare_map_max(const t_type &a, const t_type &b)
{
    return a > b ? a : b;
}

static __forceinline long prepare_map_countdown_remaining(unsigned long start_time, const long &countdown)
{
    long elapsed = prepare_map_network_time_since(start_time) / 1000;
    long remaining = countdown - elapsed + 1;
    return prepare_map_max(0L, remaining);
}

void c_life_cycle_state_handler_matchmaking_prepare_map::update_countdown(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress)
{
    c_network_session *group = m_manager->m_group_session;
    if (((const c_network_session *)group)->is_host())
    {
        switch (progress->state)
        {
        case 5:
        {
            unsigned long start_time = m_time2;
            progress->countdown_timer = prepare_map_countdown_remaining(start_time, g_network_configuration.matchmaking_ready_countdown);
            break;
        }
        case 4:
        {
            unsigned long start_time = m_time2;
            progress->countdown_timer = prepare_map_countdown_remaining(start_time, g_network_configuration.matchmaking_loading_countdown);
            break;
        }
        case 3:
        {
            unsigned long start_time = m_time2;
            progress->countdown_timer = prepare_map_countdown_remaining(start_time, g_network_configuration.matchmaking_verify_countdown);
            break;
        }
        }
        if (progress->countdown_timer == 0)
            group->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_in_game);
    }
}

// @address 0x3ea270 prepare_map_try_and_get_game_variant
extern "C" bool prepare_map_try_and_get_game_variant(c_life_cycle_state_handler_matchmaking_prepare_map *handler)
{
    long status = 0;
    if (prepare_map_download_storage().game_variant_failed != 0)
        status = 3;
    else if (prepare_map_download_storage().game_variant_valid != 0)
        status = 2;
    else if (prepare_map_download_storage().game_variant_downloader.m_url.m_string.m_string[0] != 0)
        status = 1;
    bool result = false;
    if (status == 1)
        result = true;
    else if (status == 2)
    {
        c_network_session *group_session = (c_network_session *)handler->get_manager()->m_group_session;
        c_network_session_parameters *parameters = group_session->get_session_parameters();
        const s_network_session_matchmaking_hopper *hopper_map = parameters->m_parameters.matchmaking_hopper.get();
        if (hopper_map->team_game == (prepare_map_download_storage().game_variant.flags & 1) &&
            ((c_network_session_parameter_game_variant *)&parameters->m_parameters.game_variant)->set(&prepare_map_download_storage().game_variant))
        {
            result = true;
            handler->m_flags.m_storage |= 8;
        }
    }
    return result;
}

// @address 0x3ea398 prepare_map_try_and_get_map_variant
extern "C" bool prepare_map_try_and_get_map_variant(c_life_cycle_state_handler_matchmaking_prepare_map *handler, long game_index)
{
    long status = prepare_map_map_variant_download_status();
    bool result = false;
    if (status == 1)
        result = true;
    else if (status == 2)
    {
        c_network_session *group_session = (c_network_session *)handler->get_manager()->m_group_session;
        c_network_session_parameters *parameters = group_session->get_session_parameters();
        const s_game_hopper_picked_game_collection *picked_games = parameters->m_parameters.matchmaking_game_list.get();
        if (parameters->m_parameters.map_variant.set(&prepare_map_download_storage().map_variant) &&
            picked_games->games[game_index].map_id == prepare_map_download_storage().map_variant.map_id)
        {
            handler->m_flags.m_storage |= 16;
            result = true;
        }
    }
    return result;
}

// @address 0x3e9730 ?enter@c_life_cycle_state_handler_matchmaking_prepare_map@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_prepare_map::enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous, 0, 0);
    m_flags.m_storage = 0;
    m_start_time = prepare_map_network_time_get();
    m_time2 = prepare_map_network_time_get();
    m_time3 = prepare_map_network_time_get();
    c_life_cycle_state_manager *owner = m_manager;
    c_network_session *group = (c_network_session *)owner->m_group_session;
    if (group->is_host())
    {
        m_flags.set(_matchmaking_prepare_map_started_bit, true);
        m_flags.set(_matchmaking_prepare_map_request_next_game_and_map_bit, true);
    }
    ::network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != 4)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}


static __forceinline c_http_buffer_downloader *prepare_map_game_variant_downloader()
{
    return &prepare_map_download_storage().game_variant_downloader;
}

static __forceinline c_http_buffer_downloader *prepare_map_map_variant_downloader()
{
    return &prepare_map_download_storage().map_variant_downloader;
}

// @address 0x3ea4b0 ?update_game_and_map_download@c_life_cycle_state_handler_matchmaking_prepare_map@@AAAXPAUs_replicated_life_cycle_matchmaking_progress_configuring_match@@@Z
void c_life_cycle_state_handler_matchmaking_prepare_map::update_game_and_map_download(s_replicated_life_cycle_matchmaking_progress_configuring_match *progress)
{
    bool failed = false;
    bool complete = false;

    if (!flag_mask(_matchmaking_prepare_map_game_variant_downloaded_bit))
    {
        if (!prepare_map_try_and_get_game_variant(this))
            failed = true;
    }
    if (!flag_mask(_matchmaking_prepare_map_map_variant_downloaded_bit))
    {
        if (!prepare_map_try_and_get_map_variant((c_life_cycle_state_handler_matchmaking_prepare_map *)this, progress->game_map_index))
            failed = true;
    }

    if (!failed && flag_mask(_matchmaking_prepare_map_game_variant_downloaded_bit) &&
        flag_mask(_matchmaking_prepare_map_map_variant_downloaded_bit))
    {
        c_network_session *group_session = (c_network_session *)m_manager->m_group_session;
        c_network_session_parameters *parameters = group_session->get_session_parameters();
        const s_game_hopper_picked_game_collection *picked_games = parameters->m_parameters.matchmaking_game_list.get();
        s_network_session_parameter_map settings;
        memset(&settings, 0, sizeof(settings));
        settings.campaign_id = _campaign_id_none;
        settings.map_id = (e_map_id)picked_games->games[progress->game_map_index].map_id;
        levels_get_map_scenario_path(NONE, settings.map_id, settings.scenario_path, sizeof(settings.scenario_path));
        if (settings.scenario_path[0] == 0)
            failed = true;
        else if (!parameters->m_parameters.map.set(&settings))
            failed = true;
        else
            complete = true;
    }

    if (failed)
    {
        long next_game_index = progress->game_map_index + 1;
        if (next_game_index >= 4)
        {
            disband_group_session();
        }
        else
        {
            progress->game_map_index = next_game_index;
            flag_set(_matchmaking_prepare_map_request_next_game_and_map_bit, true);
        }
        flag_set(_matchmaking_prepare_map_downloading_bit, false);
    }
    else if (complete)
    {
        m_time3 = prepare_map_network_time_get();
        flag_set(_matchmaking_prepare_map_downloading_bit, false);
    }
}

static __forceinline void prepare_map_start_loading_countdown(
    s_replicated_life_cycle_matchmaking_progress_configuring_match *progress, c_network_session_membership *membership,
    const s_network_session_matchmaking_hopper *hopper_map, long state)
{
    if (state == 0)
    {
        if (hopper_map->veto_allowed)
        {
            if (progress->game_map_index >= 3)
                progress->state = 3;
            else
            {
                progress->state = 1;
                progress->current_veto_count = 0;
                progress->max_veto_count = membership->get_player_count();
            }
        }
        else
            progress->state = 3;
    }
    else
        progress->state = 5;
}

// @address 0x3e9920 ?update_loading@c_life_cycle_state_handler_matchmaking_prepare_map@@AAAXPAUs_replicated_life_cycle_matchmaking_progress_configuring_match@@@Z
// match: 0x3e9920: pointer __restrict qualifier prevents alias reload
void c_life_cycle_state_handler_matchmaking_prepare_map::update_loading(s_replicated_life_cycle_matchmaking_progress_configuring_match * __restrict progress)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *group = (c_network_session *)manager->m_group_session;
    c_network_session_parameters *parameters = group->get_session_parameters();
    bool parameters_available = parameters->m_parameters.map.get_allowed() &&
        parameters->m_parameters.game_variant.get_allowed() && parameters->m_parameters.map_variant.get_allowed();
    if (group->is_host())
    {
        if (!parameters_available)
        {
            long next_game = progress->game_map_index + 1;
            if (next_game >= 4)
                disband_group_session();
            else
            {
                progress->game_map_index = next_game;
                flag_set(_matchmaking_prepare_map_request_next_game_and_map_bit, true);
            }
        }
        else if (!parameters->parameters_transmitted_to_peers(128))
            progress->map_load_progress_percentage = 0;
        else
        {
            const s_network_session_parameter_map *lobby = parameters->m_parameters.map.get();
            c_network_session_membership *membership = group->get_session_membership_for_update();
            bool loaded = true;
            long percent = 100;
            unsigned long failed_peers = 0;
            for (long peer_index = membership->get_first_peer(); peer_index != NONE;
                 peer_index = membership->get_next_peer(peer_index))
            {
                const s_network_session_peer *peer = membership->get_peer(peer_index);
                if (peer->properties.map_id != lobby->map_id)
                {
                    percent = 0;
                    loaded = false;
                    break;
                }
                switch (peer->properties.map_status)
                {
                case 0:
                    percent = 0;
                    loaded = false;
                    break;
                case 1:
                    failed_peers |= 1 << peer_index;
                    break;
                case 2:
                    if (percent > peer->properties.map_progress)
                        percent = peer->properties.map_progress;
                    loaded = false;
                    break;
                }
            }
            if (failed_peers)
            {
                if (!progress->load_failure_encountered)
                {
                    long next_game = progress->game_map_index + 1;
                    if (next_game >= 4)
                        disband_group_session();
                    else
                    {
                        progress->game_map_index = next_game;
                        flag_set(_matchmaking_prepare_map_request_next_game_and_map_bit, true);
                    }
                    progress->load_failure_encountered = true;
                    manager->m_game_exhausted = true;
                }
                else
                {
                    manager->m_game_exhausted_handled = true;
                    if (failed_peers & (1 << membership->local_peer_index()))
                    {
                        group->host_boot_machine(membership->local_peer_index(), _network_session_boot_reason_prepare_map_failed);
                        disband_group_session();
                    }
                    else
                    {
                        for (long peer_index = membership->get_first_peer(); peer_index != NONE;
                             peer_index = membership->get_next_peer(peer_index))
                        {
                            if (failed_peers & (1 << peer_index))
                                group->host_boot_machine(peer_index, _network_session_boot_reason_prepare_map_failed);
                        }
                    }
                }
            }
            else
            {
                const s_network_session_matchmaking_hopper *hopper_map = parameters->m_parameters.matchmaking_hopper.get();
                progress->map_load_progress_percentage = percent;
                if (loaded)
                {
                    long elapsed = prepare_map_network_time_since(m_time3) / 1000;
                    if (!parameters->parameters_transmitted_to_peers(0x180000))
                        progress->map_load_progress_percentage = 99;
                    else if (elapsed > g_network_configuration.matchmaking_loading_timeout)
                    {
                        unsigned long now = prepare_map_network_time_get();
                        long state = progress->state;
                        m_time2 = now;
                        prepare_map_start_loading_countdown(progress, membership, hopper_map, state);
                    }
                }
            }
        }
    }
}


static __forceinline void prepare_map_initialize_http_url(s_network_http_buffer_url *url)
{
    url->name[0] = 0;
    url->service_type = 0;
    url->cache_mode = 0;
    url->lifetime_seconds = 600;
    url->queue_type = 0;
    online_url_set_base_path((c_online_url_string *)url);
}

static __forceinline void prepare_map_close_download_session(c_network_session *group)
{
    group->get_session_membership_for_update()->set_slot_counts(0, 0, false);
    group->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_disbanding);
}

// @address 0x3ea068 prepare_map_initialize_game_and_map_download
extern "C" void prepare_map_initialize_game_and_map_download(c_life_cycle_state_handler_matchmaking_prepare_map *handler, long game_index)
{
    c_network_session_parameters *parameters = ((c_network_session *)handler->get_manager()->m_group_session)->get_session_parameters();
    const s_network_session_matchmaking_hopper *hopper_map = parameters->m_parameters.matchmaking_hopper.get();
    const s_game_hopper_picked_game_collection *picked_games = parameters->m_parameters.matchmaking_game_list.get();
    if (game_index >= 0 && (unsigned long)game_index < 4)
    {
        unsigned short hopper_id = hopper_map->hopper_identifier;
        s_network_http_buffer_url game_url;
        prepare_map_initialize_http_url(&game_url);
        const s_game_hopper_picked_game *game = &picked_games->games[game_index];
        ((c_static_string<256> *)game_url.name)->append_print("%05u/%s_%03u.bin", hopper_id, game->game_variant_name, 10);
        game_url.queue_type = 0;
        game_url.cache_mode = 1;
        http_buffer_downloader_set_url(prepare_map_game_variant_downloader(), &game_url);
        prepare_map_game_variant_downloader()->set_hash(&game->game_variant_hash);
        prepare_map_game_variant_downloader()->get_data();
        prepare_map_download_storage().game_variant_failed = 0;
        prepare_map_download_storage().game_variant_valid = 0;

        unsigned short map_hopper_id = hopper_map->hopper_identifier;
        s_network_http_buffer_url map_url;
        prepare_map_initialize_http_url(&map_url);
        ((c_static_string<256> *)map_url.name)->append_print("%05u/map_variants/%s_%03u.bin", map_hopper_id, game->map_variant_name, 12);
        map_url.cache_mode = 1;
        map_url.queue_type = 0;
        http_buffer_downloader_set_url(prepare_map_map_variant_downloader(), &map_url);
        prepare_map_map_variant_downloader()->set_hash(&game->map_variant_hash);
        prepare_map_map_variant_downloader()->get_data();
        prepare_map_download_storage().map_variant_failed = 0;
        prepare_map_download_storage().map_variant_valid = 0;
        handler->flag_set(_matchmaking_prepare_map_game_variant_downloaded_bit, false);
        handler->flag_set(_matchmaking_prepare_map_map_variant_downloaded_bit, false);
        handler->flag_set(_matchmaking_prepare_map_request_next_game_and_map_bit, false);
        handler->flag_set(_matchmaking_prepare_map_downloading_bit, true);
    }
    else
    {
        prepare_map_close_download_session(((c_network_session *)handler->get_manager()->m_group_session));
    }
}

static __forceinline void prepare_map_get_state_from_progress(
    c_life_cycle_state_handler_matchmaking_prepare_map *handler, s_replicated_life_cycle_matchmaking_progress_configuring_match *progress)
{
    c_network_session *group = (c_network_session *)handler->get_manager()->m_group_session;
    c_network_session_parameters *parameters = group->get_session_parameters();
    if (parameters->m_parameters.matchmaking_progress.get_allowed())
    {
        const s_replicated_life_cycle_matchmaking_progress *status = parameters->m_parameters.matchmaking_progress.get();
        if (status->type == _life_cycle_matchmaking_progress_preparing_map)
            *progress = status->configuring_match;
    }
}

static __forceinline void prepare_map_update_progress(c_life_cycle_state_handler_matchmaking_prepare_map *handler,
    const s_replicated_life_cycle_matchmaking_progress_configuring_match *progress)
{
    s_replicated_life_cycle_matchmaking_progress status;
    memset(&status, 0, sizeof(status));
    status.type = _life_cycle_matchmaking_progress_preparing_map;
    status.configuring_match = *progress;
    c_network_session *group = (c_network_session *)handler->get_manager()->m_group_session;
    group->get_session_parameters()->m_parameters.matchmaking_progress.set(&status);
}

static __forceinline bool prepare_map_flag_test(unsigned char flags, long bit)
{
    bool result = (bool)((flags >> bit) & 1);
    return result;
}

// @address 0x3e93b8 ?update@c_life_cycle_state_handler_matchmaking_prepare_map@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_prepare_map::update()
{
    s_replicated_life_cycle_matchmaking_progress_configuring_match progress = {};
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *group = (c_network_session *)manager->m_group_session;
    prepare_map_get_state_from_progress(this, &progress);
    if (group->is_host())
    {
        // match: 0x3e93b8: pointer __restrict qualifier prevents alias reload
        t_flags *__restrict flags = &m_flags;
        if (!prepare_map_flag_test(flags->m_storage, _matchmaking_prepare_map_downloading_bit) &&
            !prepare_map_flag_test(flags->m_storage, _matchmaking_prepare_map_request_next_game_and_map_bit) &&
            group->is_host() && group->get_session_membership()->all_peers_established())
        {
            if (!prepare_map_flag_test(flags->m_storage, _matchmaking_prepare_map_started_bit))
            {
                m_flags.set(_matchmaking_prepare_map_started_bit, true);
                m_time2 = prepare_map_network_time_get();
            }
            switch (progress.state)
            {
            case 0:
            case 2:
                update_loading(&progress);
                break;
            case 1:
                request_next_game_and_map(&progress);
                break;
            case 3:
            case 4:
            case 5:
                update_countdown(&progress);
                break;
            default:
                // match: 0x3e93b8: unreachable hint omits dead branch check
                __assume(0);
            }
        }
        if (flag_test(_matchmaking_prepare_map_request_next_game_and_map_bit))
            prepare_map_initialize_game_and_map_download(this, progress.game_map_index);
        if (flag_test(_matchmaking_prepare_map_downloading_bit))
            update_game_and_map_download(&progress);
        prepare_map_update_progress(this, &progress);
    }
}
