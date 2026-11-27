// networkinglibxbox/network_life_cycle.obj: from the records 0x844f0 0x8b8a0 0x94138 0x94818 0x3a1600 0x3a16e0 0x3a16f0 0x3a1700 0x3a1710 0x3a1720 0x3a1730 0x3a1740 0x3a1750 0x3a1760 0x3a1770 0x3a1780 0x3a1790 0x3a17a0 0x3a17b0 0x3a17c0 0x3a17d0 0x3a17e0 0x3a17f0 0x3a1bf8 0x3a23a0 0x3a2748 0x3a27c0 0x3a2890 0x3a2908 0x3a2a88 0x3a2b78 0x3a2c68 0x3a2cc8 0x3a2d58 0x3a2e38 0x3a2ec8
// @flags /TP /O1
#include "cseries/cseries_wrapped.h"
#include "cseries/cseries.h"
#include "networking/logic/logic_qos_reply_manager.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_none.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/logic/network_session_interface.h"
#include "networking/transport/transport_qos.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_class.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/session/network_session_parameters_generic_set.h"
#if !defined(_XBOX) || defined(__clang__)
#include "networking/delivery/network_observer_globals.h"
#include "networking/session/network_session_manager_globals.h"
#include "networking/session/network_session_parameter_type_collection.h"
#endif
class c_network_session;
// @address 0x93f18 ?current_state_ready_for_state_transition_or_query@c_life_cycle_state_manager@@QBA_NXZ
// @address 0x3ddc90 ?get_matchmaking_progress_for_squad_search@c_life_cycle_state_handler@@IAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3ddec0 ?get_matchmaking_progress_for_setting_up_match@c_life_cycle_state_handler@@IAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3e3870 ?get_progress@c_life_cycle_state_handler_matchmaking_start@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3e6198 ?get_progress@c_life_cycle_state_handler_matchmaking_assemble_match@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3e9000 ?get_progress@c_life_cycle_state_handler_matchmaking_prepare_map@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3eb490 ?get_progress@c_life_cycle_state_handler_post_match@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3dd420 ?set_current_state@c_life_cycle_state_manager@@AAAXW4e_life_cycle_state@@JPAX@Z
// @address 0x3ed6c8 ?set@c_network_session_parameter_session_mode@@QAA_NW4e_network_session_mode@@@Z
// @address 0x844f0 network_life_cycle_in_session
// @address 0x8b8a0 network_life_cycle_try_and_get_squad_session
// @address 0x94138 ?reset_session_state@c_life_cycle_state_handler@@IAAXXZ
// @address 0x94818 ?network_life_cycle_in_group_session@@YA_NPAPAVc_network_session@@@Z
// @address 0x3a2908 ?network_life_cycle_get_matchmaking_progress@@YAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3a23a0 ?network_life_cycle_dispose@@YAXXZ
// @address 0x3a2748 ?network_life_cycle_squad_local_peer_is_leader@@YA_NXZ
// @address 0x3a27c0 network_life_cycle_is_squad_host
// @address 0x3a2b78 ?network_life_cycle_set_pre_game_state@@YA_NXZ
// @address 0x3a2ec8 ?set@?$c_generic_network_session_parameter@_N@@QAA_NPB_N@Z
// @address 0x3a1750 ?get_state_string@c_life_cycle_state_handler_leaving@@UAAPBDXZ
// @address 0x3a1760 ?get_state_string@c_life_cycle_state_handler_joining@@UAAPBDXZ
// @address 0x3a1600 ?is_host@c_network_session@@QBA_NXZ
// @address 0x3a16e0 ?get_state_string@c_life_cycle_state_handler_none@@UAAPBDXZ
// @address 0x3a16f0 ?get_state_string@c_life_cycle_state_handler_pre_game@@UAAPBDXZ
// @address 0x3a1700 ?get_state_string@c_life_cycle_state_handler_matchmaking_select_host@@UAAPBDXZ
// @address 0x3a1710 ?get_state_string@c_life_cycle_state_handler_start_game@@UAAPBDXZ
// @address 0x3a1720 ?get_state_string@c_life_cycle_state_handler_in_match@@UAAPBDXZ
// @address 0x3a1730 ?get_state_string@c_life_cycle_state_handler_in_game@@UAAPBDXZ
// @address 0x3a1740 ?get_state_string@c_life_cycle_state_handler_end_game_write_stats@@UAAPBDXZ
// @address 0x3a1770 ?get_state_string@c_life_cycle_state_handler_matchmaking_start@@UAAPBDXZ
// @address 0x3a1780 ?get_state_string@c_life_cycle_state_handler_matchmaking_arbitration@@UAAPBDXZ
// @address 0x3a1790 ?get_state_string@c_life_cycle_state_handler_matchmaking_find_match_client@@UAAPBDXZ
// @address 0x3a17a0 ?get_state_string@c_life_cycle_state_handler_matchmaking_find_match@@UAAPBDXZ
// @address 0x3a17b0 ?get_state_string@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@UAAPBDXZ
// @address 0x3a17c0 ?get_state_string@c_life_cycle_state_handler_matchmaking_assemble_match@@UAAPBDXZ
// @address 0x3a17d0 ?get_state_string@c_life_cycle_state_handler_matchmaking_prepare_map@@UAAPBDXZ
// @address 0x3a17e0 ?get_state_string@c_life_cycle_state_handler_end_match_write_stats@@UAAPBDXZ
// @address 0x3a17f0 ?get_state_string@c_life_cycle_state_handler_post_match@@UAAPBDXZ
// @address 0x3a2c68 network_life_cycle_handle_lost_connection
// @address 0x3a2cc8 network_life_cycle_can_download_files
// @address 0x3a2d58 ?network_life_cycle_create_local_squad@@YA_NW4e_network_session_class@@@Z
// @address 0x3a1660 ?leaving_session@c_network_session@@QBA_NXZ
// @address 0x3cca70 ?create_host_session@c_network_session@@QAA_NV?$c_flags@W4e_session_creation_flags@@E$03@@W4e_network_session_class@@W4e_transport_platform@@PBVc_network_session_membership@@PBUs_online_context@@J@Z
#include "cseries/cseries_types.h"
#include <string.h>
// match: 0x3a2ec8: intrinsic keeps the 1-byte memcmp an inline lbz/subf loop (default emits bl memcmp)
#pragma intrinsic(memcmp)
struct s_owner;
class c_network_session;
#include "networking/session/network_session_parameter_access.h"


// Bit 5 is set on the matchmaking states (start through prepare map): they leave on a lost connection.
enum e_life_cycle_state_handler_flags {
    _life_cycle_state_handler_flag_0,
    _life_cycle_state_handler_flag_1,
    _life_cycle_state_handler_flag_2,
    _life_cycle_state_handler_flag_3,
    _life_cycle_state_handler_flag_4,
    _life_cycle_state_handler_flag_5,
    k_life_cycle_state_handler_flag_count
};

#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"


#include "networking/logic/life_cycle/life_cycle_handler_pre_game.h"

#include "networking/logic/life_cycle/life_cycle_handler_start_game.h"

#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"

#include "networking/logic/life_cycle/life_cycle_handler_in_game.h"

#include "networking/logic/life_cycle/life_cycle_handler_end_game_write_stats.h"

#include "networking/logic/life_cycle/life_cycle_handler_leaving.h"

#include "networking/logic/life_cycle/life_cycle_handler_joining.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_start.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_assemble_match.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_prepare_map.h"

#include "networking/logic/life_cycle/life_cycle_handler_end_match_write_stats.h"

#include "networking/logic/life_cycle/life_cycle_handler_post_match.h"


// 0x94138

#include "game/multiplayer_game_hopper_download_storage.h"
#include "game/multiplayer_game_hopper.h"
typedef char hopper_configuration_retry_mode_offset[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, configuration_downloader)
        + offsetof(c_http_buffer_downloader, m_automatic_retry_mode) == 0x690)) ? 1 : -1];
typedef char hopper_description_retry_mode_offset[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, description_downloader)
        + offsetof(c_http_buffer_downloader, m_automatic_retry_mode) == 0x626c)) ? 1 : -1];
typedef char hopper_game_set_retry_mode_offset[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_set_downloader)
        + offsetof(c_http_buffer_downloader, m_automatic_retry_mode) == 0xb3b4)) ? 1 : -1];
typedef char hopper_game_variant_retry_mode_offset[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_variant_downloader)
        + offsetof(c_http_buffer_downloader, m_automatic_retry_mode) == 0xfef8)) ? 1 : -1];
typedef char hopper_map_variant_retry_mode_offset[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, map_variant_downloader)
        + offsetof(c_http_buffer_downloader, m_automatic_retry_mode) == 0x1109c)) ? 1 : -1];

class c_network_session_membership;





namespace {
}

class c_life_cycle_state_handler;



#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_select_host.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_arbitration.h"

struct manager;
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match_client.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match.h"

#include "networking/logic/life_cycle/life_cycle_tracker_view.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
typedef char life_cycle_globals_none_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, none) == 0x9910))) ? 1 : -1];
typedef char life_cycle_globals_pre_game_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, pre_game) == 0x9938))) ? 1 : -1];
typedef char life_cycle_globals_start_game_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, start_game) == 0x9970))) ? 1 : -1];
typedef char life_cycle_globals_in_game_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, in_game) == 0x99a0))) ? 1 : -1];
typedef char life_cycle_globals_end_game_write_stats_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, end_game_write_stats) == 0x99e0))) ? 1 : -1];
typedef char life_cycle_globals_leaving_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, leaving) == 0x9a10))) ? 1 : -1];
typedef char life_cycle_globals_joining_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, joining) == 0x9a40))) ? 1 : -1];
typedef char life_cycle_start_nat_warning_offset[
    (sizeof(void *) != 4 || ((offsetof(c_life_cycle_state_handler_matchmaking_start, m_last_nat_warning_time) == 0x40))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_start_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_start) == 0x9a78))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_find_match_client_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_find_match_client) == 0x9ac0))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_find_match_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_find_match) == 0x9af0))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_find_and_assemble_match_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, find_and_gather) == 0xe3f0))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_assemble_match_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, assemble) == 0x12de8))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_select_host_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_select_host) == 0x12f10))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_arbitration_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_arbitration) == 0x12f48))) ? 1 : -1];
typedef char life_cycle_globals_matchmaking_prepare_map_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, matchmaking_prepare_map) == 0x13108))) ? 1 : -1];
typedef char life_cycle_globals_in_match_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, in_match) == 0x13140))) ? 1 : -1];
typedef char life_cycle_globals_end_match_write_stats_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, end_match_write_stats) == 0x13180))) ? 1 : -1];
typedef char life_cycle_globals_post_match_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, post_match) == 0x131b0))) ? 1 : -1];

// @address 0xf11308 g_network_life_cycle_globals
s_life_cycle_globals g_network_life_cycle_globals;

__forceinline bool c_network_session_parameter_session_mode::request_change(e_network_session_mode session_mode)
{
    bool result = false;
    if (this->set_allowed())
        result = set(session_mode);
    else if (this->change_request_allowed()) {
        m_requested_data.mode = session_mode;
        set_desires_change_request();
        result = true;
    }
    return result;
}


// match: 0x8b8a0: favor-speed boundary; without it 0x844f0 loses its switch jump table and 0x94138 branches switch cr6 to cr0
#pragma optimize("t", on)
/* natural 0x8b8a0: no March name or pairing is available; the squad-session twin of network_life_cycle_in_group_session. */
extern "C" bool network_life_cycle_try_and_get_squad_session(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.get_active_squad_session();
        if (!current->disconnected()) {
            if (session)
                *session = current;
            result = true;
        }
    }
    return result;
}

void c_life_cycle_state_handler::reset_session_state()
{
    c_life_cycle_state_manager *holder = get_manager();
    c_network_session *context = holder->m_active_squad_session;
    int state = context->current_local_state();
    if (state == 0)
        return;
    if (context->leaving_session())
        return;
    if (context->is_host())
        context->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
    holder->m_entry_data_size = 0;
    holder->m_pending_state_change = true;
    holder->m_pending_state = _life_cycle_state_pre_game;
    for (int i = 0; i < 4; i++)
        ((long *)holder->m_entry_data)[i] = 0;
    if (holder->m_entry_data_size > 0)
        memcpy(holder->m_entry_data, 0, holder->m_entry_data_size);
}

// 0x94818
bool network_life_cycle_in_group_session(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.get_group_session();
        if (!current->disconnected()) {
            if (session)
                *session = current;
            result = true;
        }
    }
    return result;
}

// 0x844f0: fetch the session of whichever family the current state belongs to.
static __forceinline e_life_cycle_state network_life_cycle_current_state(void)
{
    e_life_cycle_state state = _life_cycle_state_none;
    if (g_network_life_cycle_globals.initialized)
        state = g_network_life_cycle_globals.manager.get_current_state();
    return state;
}

extern "C" bool network_life_cycle_in_session(c_network_session **session)
{
    bool result = false;
    switch (network_life_cycle_current_state()) {
    case _life_cycle_state_pre_game:
    case _life_cycle_state_start_game:
    case _life_cycle_state_in_game:
    case _life_cycle_state_end_game_write_stats:
    case _life_cycle_state_leaving:
    case _life_cycle_state_joining:
    case _life_cycle_state_matchmaking_start:
    case _life_cycle_state_matchmaking_find_match_client:
    case _life_cycle_state_matchmaking_find_match:
    case _life_cycle_state_matchmaking_find_and_assemble_match:
    case _life_cycle_state_matchmaking_assemble_match:
        result = network_life_cycle_try_and_get_squad_session(session);
        break;
    case _life_cycle_state_matchmaking_arbitration:
    case _life_cycle_state_matchmaking_select_host:
    case _life_cycle_state_matchmaking_prepare_map:
    case _life_cycle_state_in_match:
    case _life_cycle_state_end_match_write_stats:
    case _life_cycle_state_post_match:
        result = network_life_cycle_in_group_session(session);
        break;
    }
    return result;
}

inline bool c_network_session::is_host() const
{
    bool host = false;
    long session_state = current_local_state();
    if (session_state == _network_session_state_host_established ||
        session_state == _network_session_state_host_disband ||
        session_state == _network_session_state_host_handoff ||
        session_state == _network_session_state_host_reestablish)
    {
        ASSERT(m_session_membership.local_peer_index() == m_session_membership.host_peer_index());
        host = true;
    }
    else if (established())
    {
        ASSERT(m_session_membership.local_peer_index() != m_session_membership.host_peer_index());
    }
    return host;
}


// 0x3a16e0
const char *c_life_cycle_state_handler_none::get_state_string()
{
    return "none";
}

// 0x3a16f0
const char *c_life_cycle_state_handler_pre_game::get_state_string()
{
    return "pre-game";
}

// 0x3a1700
const char *c_life_cycle_state_handler_matchmaking_select_host::get_state_string()
{
    return "matchmaking-select-host";
}

// 0x3a1710
const char *c_life_cycle_state_handler_start_game::get_state_string()
{
    return "start-game";
}

// 0x3a1720
const char *c_life_cycle_state_handler_in_match::get_state_string()
{
    return "in-match";
}

// 0x3a1730
const char *c_life_cycle_state_handler_in_game::get_state_string()
{
    return "in-game";
}

// 0x3a1740
const char *c_life_cycle_state_handler_end_game_write_stats::get_state_string()
{
    return "end-game-write-stats";
}

// 0x3a1750
const char *c_life_cycle_state_handler_leaving::get_state_string()
{
    return "leaving";
}

// 0x3a1760
const char *c_life_cycle_state_handler_joining::get_state_string()
{
    return "joining";
}

// 0x3a1770
const char *c_life_cycle_state_handler_matchmaking_start::get_state_string()
{
    return "matchmaking-start";
}

// 0x3a1780
const char *c_life_cycle_state_handler_matchmaking_arbitration::get_state_string()
{
    return "matchmaking-arbitration";
}

// 0x3a1790
const char *c_life_cycle_state_handler_matchmaking_find_match_client::get_state_string()
{
    return "matchmaking-find-match-client";
}

// 0x3a17a0
const char *c_life_cycle_state_handler_matchmaking_find_match::get_state_string()
{
    return "matchmaking-find-match";
}

// 0x3a17b0
const char *c_life_cycle_state_handler_matchmaking_find_and_assemble_match::get_state_string()
{
    return "matchmaking-find-and-assemble-match";
}

// 0x3a17c0
const char *c_life_cycle_state_handler_matchmaking_assemble_match::get_state_string()
{
    return "matchmaking-assemble-match";
}

// 0x3a17d0
const char *c_life_cycle_state_handler_matchmaking_prepare_map::get_state_string()
{
    return "matchmaking-prepare-map";
}

// 0x3a17e0
const char *c_life_cycle_state_handler_end_match_write_stats::get_state_string()
{
    return "end-match-write-stats";
}

// 0x3a17f0
const char *c_life_cycle_state_handler_post_match::get_state_string()
{
    return "post-match";
}


// The campaign session is the active squad session.
static bool network_life_cycle_campaign_session_try_get(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.get_active_squad_session();
        if (!current->disconnected()) {
            if (session)
                *session = current;
            result = true;
        }
    }
    return result;
}

// 0x3a2748: whether there is a campaign session with an owner whose expected value is current.
bool network_life_cycle_squad_local_peer_is_leader(void)
{
    bool result = false;
    c_network_session *session = 0;
    if (network_life_cycle_campaign_session_try_get(&session))
        result = session->is_leader();
    return result;
}

// 0x3a27c0: whether there is a campaign session and it is hosted.
static bool network_life_cycle_hosted_session_try_get(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.get_active_squad_session();
        if (!current->disconnected()) {
            *session = current;
            result = true;
        }
    }
    return result;
}

extern "C" bool network_life_cycle_is_squad_host(void)
{
    bool result = false;
    c_network_session *session = 0;
    if (network_life_cycle_hosted_session_try_get(&session))
        result = session->is_host();
    return result;
}

// 0x3a2908
void network_life_cycle_get_matchmaking_progress(s_life_cycle_matchmaking_progress *progress)
{
    e_life_cycle_state state = network_life_cycle_current_state();
    memset(progress, 0, sizeof(*progress));
    progress->type = 0;
    if (g_network_life_cycle_globals.manager.current_state_ready_for_state_transition_or_query()) {
        switch (state) {
        case _life_cycle_state_matchmaking_start:
            g_network_life_cycle_globals.matchmaking_start.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_find_match_client:
            g_network_life_cycle_globals.matchmaking_find_match_client.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_find_match:
            g_network_life_cycle_globals.matchmaking_find_match.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_find_and_assemble_match:
            g_network_life_cycle_globals.find_and_gather.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_assemble_match:
            g_network_life_cycle_globals.assemble.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_select_host:
            g_network_life_cycle_globals.matchmaking_select_host.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_arbitration:
            g_network_life_cycle_globals.matchmaking_arbitration.get_progress(progress);
            break;
        case _life_cycle_state_matchmaking_prepare_map:
            g_network_life_cycle_globals.matchmaking_prepare_map.get_progress(progress);
            break;
        case _life_cycle_state_post_match:
            g_network_life_cycle_globals.post_match.get_progress(progress);
            break;
        }
    }
}

// 0x3a2b78
bool network_life_cycle_set_pre_game_state()
{
    bool result = false;
    if (g_network_life_cycle_globals.manager.get_current_state() != 0) {
        if (g_network_life_cycle_globals.manager.get_current_state() != 1) {
            c_network_session *session = g_network_life_cycle_globals.manager.get_active_squad_session();
            if (session->is_leader()) {
                c_network_session_parameter_session_mode *parameters = &session->get_session_parameters()->m_parameters.session_mode;
                if (parameters->request_change(_network_session_mode_idle))
                    result = true;
            }
        } else {
            result = true;
        }
    }
    return result;
}

// 0x3a2c68
extern "C" void network_life_cycle_handle_lost_connection(void)
{
    c_life_cycle_state_manager *state_manager = &g_network_life_cycle_globals.manager;
    if (state_manager->get_current_state_handler()->test_flag(_life_cycle_state_handler_flag_5)) {
        bool entry_data = false;
        state_manager->set_current_state(_life_cycle_state_leaving, 1, &entry_data);
    }
}

// 0x3a2cc8
extern "C" bool network_life_cycle_can_download_files(void)
{
    bool result = false;
    e_life_cycle_state state = _life_cycle_state_none;
    if (g_network_life_cycle_globals.initialized)
        state = g_network_life_cycle_globals.manager.get_current_state();
    switch (state) {
    case _life_cycle_state_none:
    case _life_cycle_state_pre_game:
    case _life_cycle_state_in_game:
    case _life_cycle_state_end_game_write_stats:
    case _life_cycle_state_in_match:
    case _life_cycle_state_post_match:
        result = true;
        break;
    }
    return result;
}


// 0x3a2d58
bool network_life_cycle_create_local_squad(e_network_session_class session_class)
{
    bool result = false;
    c_network_session *session = g_network_life_cycle_globals.manager.get_active_squad_session();
    if (session->leaving_session() && session->current_local_state() != 0) {
        session->m_disconnect_reestablish_state = 1;
        session->disconnect();
    }
    if (session->current_local_state() == 0) {
        c_flags<e_session_creation_flags, unsigned char, 4> creation_flags;
        creation_flags.set(_session_creation_flag_local, true);
        creation_flags.set(_session_creation_flag_squad, session_class == _network_session_class_xbox_live);
        s_online_context online_context[2];
        online_context[0].id = 0;
        online_context[0].value = 22;
        online_context[1].id = 1;
        online_context[1].value = 3;
        if (session->create_host_session(creation_flags, session_class, _transport_platform_xenon, 0, online_context, 2)) {
            result = true;
            ::network_session_update_properties(session);
        }
    }
    return result;
}

// @address 0x3a2e38 ?request_change@?$c_generic_network_session_parameter@_N@@QAA_NPB_N@Z
template<>
bool c_generic_network_session_parameter<bool>::request_change(const bool *parameter)
{
    bool result = false;
    if (this->set_allowed()) {
        result = set(parameter);
    } else if (this->change_request_allowed()) {
        m_requested_data = *parameter;
        set_desires_change_request();
        result = true;
    }
    return result;
}

// 0x3a2ec8
template<>
bool c_generic_network_session_parameter<bool>::set(const bool *parameter)
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

// @address 0x3a2a88 ?network_life_cycle_abort_matchmaking@@YAXXZ
void network_life_cycle_abort_matchmaking()
{
    e_life_cycle_state state = g_network_life_cycle_globals.manager.get_current_state();
    if (state == _life_cycle_state_matchmaking_start ||
        state == _life_cycle_state_matchmaking_find_match_client ||
        state == _life_cycle_state_matchmaking_find_match ||
        state == _life_cycle_state_matchmaking_find_and_assemble_match ||
        state == _life_cycle_state_matchmaking_assemble_match) {
        c_network_session *session = g_network_life_cycle_globals.manager.get_active_squad_session();
        if (!session->disconnected() && session->established() && !session->leaving_session()) {
            if (session->is_leader()) {
                bool abort = true;
                session->get_session_parameters()->m_parameters.matchmaking_abort_requested.request_change(&abort);
            }
        }
    }
}

// @address 0x3a23a0 ?network_life_cycle_dispose@@YAXXZ
void network_life_cycle_dispose()
{
    if (g_qos_reply_manager_globals.live_service_qos.flags.bits & 1) {
        transport_qos_delete(g_qos_reply_manager_globals.live_service_qos.qos_index);
        g_qos_reply_manager_globals.live_service_qos.qos_index = -1;
        g_qos_reply_manager_globals.live_service_qos.flags.bits &= ~1;
    }
    g_qos_reply_manager_globals.squad_session_qos_reply.initialized = 0;
    g_qos_reply_manager_globals.group_session_qos_reply.initialized = 0;
    g_qos_reply_manager_globals.initialized = 0;
    g_network_life_cycle_globals.none.dispose();
    g_network_life_cycle_globals.pre_game.dispose();
    g_network_life_cycle_globals.start_game.dispose();
    g_network_life_cycle_globals.in_game.dispose();
    g_network_life_cycle_globals.end_game_write_stats.dispose();
    g_network_life_cycle_globals.leaving.dispose();
    g_network_life_cycle_globals.matchmaking_start.dispose();
    g_network_life_cycle_globals.matchmaking_find_match_client.dispose();
    g_network_life_cycle_globals.matchmaking_find_match.dispose();
    g_network_life_cycle_globals.find_and_gather.dispose();
    g_network_life_cycle_globals.assemble.dispose();
    g_network_life_cycle_globals.matchmaking_select_host.dispose();
    g_network_life_cycle_globals.matchmaking_arbitration.dispose();
    g_network_life_cycle_globals.matchmaking_prepare_map.dispose();
    g_network_life_cycle_globals.in_match.dispose();
    g_network_life_cycle_globals.end_match_write_stats.dispose();
    g_network_life_cycle_globals.post_match.dispose();
    g_network_life_cycle_globals.initialized = 0;
}

static __forceinline void network_life_cycle_end_disconnect(c_network_session *session)
{
    if (!session->disconnected()) {
        session->m_disconnect_reestablish_state = 1;
        session->disconnect();
    }
}

// @address 0x3a2890 ?network_life_cycle_end@@YAXXZ
void network_life_cycle_end()
{
    network_life_cycle_end_disconnect(g_network_life_cycle_globals.manager.get_active_squad_session());
    network_life_cycle_end_disconnect(g_network_life_cycle_globals.manager.get_group_session());
    g_network_life_cycle_globals.manager.terminate();
}

bool (c_network_session::*network_life_cycle_is_host_ref())(void) const
{
    return &c_network_session::is_host;
}


class c_network_observer;
class c_network_session_manager;
typedef bool (c_network_session::*network_life_cycle_host_query)() const;
#if defined(_XBOX) && !defined(__clang__)
// match: 0x8c445c: these four pointers sit in the 0x828c4448..0x828c446c pool
// that network_globals.cpp lays out, which holds only static datums
static c_network_observer *g_network_observer;
__declspec(align(8)) static c_network_session *g_network_sessions;
static c_network_session_parameter_type_collection *g_network_session_parameter_types;
__declspec(align(8)) static c_network_session_manager *g_network_session_manager;
#else
c_network_observer *g_network_observer;
static c_network_session *g_network_sessions;
c_network_session_parameter_type_collection *g_network_session_parameter_types;
c_network_session_manager *g_network_session_manager;
#endif

c_network_observer *&network_life_cycle_observer_storage()
{
    return g_network_observer;
}

c_network_session *&network_life_cycle_session_storage()
{
    return g_network_sessions;
}

c_network_session_parameter_type_collection *&network_life_cycle_parameter_storage()
{
    return g_network_session_parameter_types;
}

c_network_session_manager *&network_life_cycle_manager_storage()
{
    return g_network_session_manager;
}

// @address 0x3a1bf8 network_life_cycle_initialize
extern "C" bool network_life_cycle_initialize(void *observer, void *session_manager, void *squad_session_one, void *squad, void *group)
{
    s_life_cycle_globals *globals=&g_network_life_cycle_globals;
    for (long index=0; index<18; ++index) globals->manager.m_handlers[index]=0;

    globals->manager.m_current_state=_life_cycle_state_none;
    globals->manager.m_observer=g_network_observer;
    globals->manager.m_session_manager=g_network_session_manager;
    globals->manager.m_active_squad_session=g_network_sessions;
    globals->manager.m_target_squad_session=(c_network_session *)squad;
    globals->manager.m_group_session=(c_network_session *)group;
    globals->manager.m_pending_state_change=false;
    globals->manager.m_cached_identifier_valid=false;
    for (long index=0; index<k_transport_secure_identifier_length; ++index) globals->manager.m_cached_identifier.key_identifier[index]=0;
    globals->manager.m_last_game_end_sequence=-1;
    s_life_cycle_globals::initialize_handler(globals,&globals->none,0,0,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->pre_game,1,1,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->start_game,2,1,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->in_game,3,1,0,0);
    globals->in_game.m_match_start_time=~0ULL;
    s_life_cycle_globals::initialize_handler(globals,&globals->end_game_write_stats,4,1,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->leaving,5,0,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->joining,6,4,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_start,7,0x25,0x120000000ULL,0);
    globals->matchmaking_start.m_last_nat_warning_time=-1;
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_find_match_client,8,0x25,0x520000000ULL,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_find_match,9,0x25,0x520000000ULL,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->find_and_gather,10,0x2f,0x520000000ULL,0x530002640ULL);
    s_life_cycle_globals::initialize_handler(globals,&globals->assemble,11,0x2f,0x520000000ULL,0x530002640ULL);
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_select_host,13,0x2f,0,0x570002740ULL);
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_arbitration,12,0x2f,0,0x570002640ULL);
    s_life_cycle_globals::initialize_handler(globals,&globals->matchmaking_prepare_map,14,0x2f,0,0x570002740ULL);
    s_life_cycle_globals::initialize_handler(globals,&globals->in_match,15,0x17,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->end_match_write_stats,16,0x17,0,0);
    s_life_cycle_globals::initialize_handler(globals,&globals->post_match,17,0x17,0,0x120000000ULL);
    s_hopper_update_globals *hopper = (s_hopper_update_globals *)&g_multiplayer_game_hopper_globals;
    hopper->configuration_downloader.set_automatic_retries_enabled(true);
    hopper->description_downloader.set_automatic_retries_enabled(true);
    hopper->game_set_downloader.set_automatic_retries_enabled(true);
    hopper->game_variant_downloader.set_automatic_retries_enabled(false);
    hopper->map_variant_downloader.set_automatic_retries_enabled(false);
    g_qos_reply_manager_globals.live_service_qos.flags.bits = 0;
    g_qos_reply_manager_globals.live_service_qos.qos_index = -1;
    g_qos_reply_manager_globals.live_service_qos.last_attempt = 0;
    g_qos_reply_manager_globals.squad_session_qos_reply.initialized=1; g_qos_reply_manager_globals.group_session_qos_reply.initialized=1;
    g_qos_reply_manager_globals.initialized=1; globals->initialized=true;
    return true;
}


// @address 0x94138 ?update@c_life_cycle_state_handler_none@@UAAXXZ
void c_life_cycle_state_handler_none::update()
{
    c_life_cycle_state_manager *holder = get_manager();
    c_network_session *context = holder->m_active_squad_session;
    if (context->disconnected() || context->leaving_session())
        return;
    if (context->is_host())
        context->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
    holder->request_state_change(_life_cycle_state_pre_game, 0, 0);
}

// @address 0x42d380 ?update_for_state_transition@c_life_cycle_state_handler_none@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_none::update_for_state_transition()
{
    return _life_cycle_state_transition_continue;
}
