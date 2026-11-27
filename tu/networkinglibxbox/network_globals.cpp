// @address 0x8c4488 g_network_session_parameter_message_types
// networkinglibxbox/network_globals.obj: from the records 0x8efe8 0x8f2b8 0x8f578 0x94ac8 0x122e40 0x3a3cb8 0x3a46e8 0x3a4b10 0x3a4b38
// @flags /TP /O2 /J /Oc
#include <xtl.h>
#include "networking/logic/logic_qos_reply_manager.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/session_tracker_class.h"
#include "networking/network_globals.h"
#include "networking/online/online_managed_session_flags.h"
#include "cseries/crt_xdk.h"
#include "networking/transport/transport_globals.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include <stdlib.h>
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "simulation/simulation.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "cseries/async_xoverlapped_allocation.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/logic/network_banhammer.h"
#include "networking/online/online_service_record.h"
#include "networking/logic/network_broadcast_search.h"
#include "networking/logic/network_broadcast_search_types.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/logic/network_recruiting_search.h"
#include "networking/logic/network_recruiting_search_internals.h"
#include "networking/logic/network_search.h"
#include "networking/logic/storage/network_storage_queue.h"
#include "networking/logic/storage/network_storage_cache.h"
#include "networking/logic/storage/network_http_request_queue.h"
#include "networking/messages/network_messages_out_of_band.h"
#include "networking/messages/network_messages_session_protocol.h"
#include "networking/messages/network_messages_simulation_synchronous.h"
#include "networking/network_configuration.h"
#include "networking/network_memory.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/online/online_lsp_xenon.h"
#include "networking/online/online_session_search_xenon.h"
#include "networking/online/online_statistics_xenon.h"
#include "networking/online/online_xenon.h"
#include "networking/online/voice_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_manager.h"
#include "networking/session/network_session_parameter_type_collection.h"
#include "networking/session/network_session_parameter_types.h"
#include "networking/transport/transport.h"
#include "networking/transport/transport_available.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_types.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/network_session_interface.h"
#include "networking/tools/network_webstats.h"
#include "networking/messages/network_messages_test.h"
#include "interface/user_interface_text.h"
#include "networking/online/online_arbitration_xenon.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/messages/network_messages_simulation.h"
#include "memory/bitstream.h"
#include "simulation/simulation_globals.h"
#include "networking/network_statistics.h"
#include "networking/messages/network_messages_connect.h"
#include "networking/messages/network_messages_session_membership.h"
#include "networking/online/online_files.h"
#include "networking/online/online_lsp.h"
#include "networking/online/online_rich_presence.h"
#include "networking/online/online_url_name_globals.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/online/online_lsp_manager.h"
#include "networking/network_time_types.h"
#include "networking/delivery/network_link.h"
#include "interface/controller_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_configuration_globals.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
// match: 0x8f2b8: the canonical lifecycle header currently collides with online search task types.
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "main/main_flag_state.h"
#include "main/main_runtime_globals.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/online/online_arbitration_types.h"
#include "networking/logic/network_arbitration.h"
#include "networking/logic/network_arbitration_globals_types.h"
#include "networking/logic/storage/network_http_request_cache.h"
#include "cseries/async_xoverlapped_types.h"
#include "saved_games/game_state_pending_request.h"
#include "networking/logic/storage/network_storage_manifest.h"
#include "networking/network_globals_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/messages/network_message_gateway.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_distributed_world_types.h"
#include "networking/simulation/simulation_entity_database_operations.h"
#include "networking/session/network_session_idle.h"
#include "networking/online/online_cached_user_globals.h"
#include "text/ascii_wchar_conversion.h"
#include "networking/online/online_session_search_tasks.h"
#include "networking/online/online_guide_state_types.h"
#include "networking/online/online_lsp_search_task.h"
#include "networking/logic/storage/network_storage_item.h"
#include "networking/transport/transport_types.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "networking/network_memory_types.h"
#include "networking/network_configuration_types.h"
#include "interface/achievement_globals_types.h"
#include "networking/online/voice_interface_types.h"
#include "networking/transport/transport_qos_globals.h"
#include "networking/session/network_session_interface_globals_types.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "networking/messages/network_messages_session_parameters.h"
#include "networking/messages/network_messages_synchronous_results.h"
// @split_bss
// @shared_bss 0x82806460..0x8280a3b8
// @shared_bss 0x828c4448..0x828c446c
// Tail 0x828c446c..0x828c4470 unpooled: g_webstats_enabled is defined
// native-extern in network_webstats.cpp (shared ranges forbid native
// storage); wpl places it by name via data-owners.tsv.
// @shared_bss_zero_gaps

// @address 0x948f78 main_globals
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x47090 ??_7c_arbitration_overlapped_task@@6B@
// @address 0x45c54 ??_7c_lsp_search_task@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0DKKB@@@6B@
// @address 0x96df48 g_game_state_pending_request
// @address 0x7b0ecc g_network_storage_cache
struct s_online_session_manager_globals;




struct s_signin_info {
    unsigned __int64 xuid;
    unsigned long flags;
    unsigned long signin_state;
    unsigned long guest_number;
    unsigned long sponsor_user_index;
    char name[16];
};

#if defined(_XBOX) && !defined(__clang__)
struct s_network_send_state {
    c_network_message_gateway *gateway;
    c_network_message_handler *handler;
    c_network_observer *observer;
    c_network_session *sessions;
};
// @address 0x8c4454 g_network_send_state

struct s_network_interfaces
{
    c_network_link *link;
    c_network_message_type_collection *message_types;
    c_network_message_gateway *gateway;
    c_network_message_handler *handler;
    c_network_observer *observer;
    c_network_session *sessions;
    c_network_session_parameter_type_collection *parameter_types;
    c_network_session_manager *manager;
};
// Xbox groups the eight interface pointers from network_globals.obj and network_life_cycle.obj.
// @address 0x8c444c g_network_interfaces

// match: 0x3a3cb8: pooled online globals must share g_online_cached_users as their base anchor.
struct network_globals_online_initialization_view
{
    online_xenon_user_state users[4];
    real_orientation giant_scratch_orientations[255];
    s_simulation_globals simulation;
    s_achievement_globals achievements;
    __declspec(align(8)) unsigned long achievement_progress[49];
    s_network_shared_memory_globals shared_memory;
    s_network_configuration configuration;
    s_transport_globals transport;
    s_voice_interface_globals voice;
    __declspec(align(8)) s_online_url_name_globals url;
    s_transport_qos_globals qos;
    s_bandwidth_globals bandwidth;
    s_available_squad_globals searches;
    s_online_lsp_globals lsp;
    s_network_broadcast_search_globals broadcast;
};
typedef char network_globals_online_simulation_offset[(offsetof(network_globals_online_initialization_view, simulation) == 0x2100) ? 1 : -1];
typedef char network_globals_online_achievements_offset[(offsetof(network_globals_online_initialization_view, achievements) == 0x2224) ? 1 : -1];
typedef char network_globals_online_achievement_progress_offset[(offsetof(network_globals_online_initialization_view,
    achievement_progress) == 0x2258) ? 1 : -1];
typedef char network_globals_online_shared_memory_offset[(offsetof(network_globals_online_initialization_view, shared_memory) == 0x231c) ? 1 : -1];
typedef char network_globals_online_configuration_offset[(offsetof(network_globals_online_initialization_view, configuration) == 0x2348) ? 1 : -1];
typedef char network_globals_online_voice_offset[(offsetof(network_globals_online_initialization_view, voice) == 0x3a18) ? 1 : -1];
typedef char network_globals_online_url_offset[(offsetof(network_globals_online_initialization_view, url) == 0x3dd8) ? 1 : -1];
typedef char network_globals_online_qos_offset[(offsetof(network_globals_online_initialization_view, qos) == 0x3e58) ? 1 : -1];
typedef char network_globals_online_transport_offset[(offsetof(network_globals_online_initialization_view, transport) == 0x3990) ? 1 : -1];
typedef char network_globals_online_bandwidth_offset[(offsetof(network_globals_online_initialization_view, bandwidth) == 0x3e60) ? 1 : -1];
typedef char network_globals_online_lsp_offset[(offsetof(network_globals_online_initialization_view, lsp) == 0x3ea0) ? 1 : -1];
typedef char network_globals_online_broadcast_offset[(offsetof(network_globals_online_initialization_view, broadcast) == 0x3f30) ? 1 : -1];
#else
struct network_globals_online_initialization_view
{
    s_transport_globals &transport;
    s_bandwidth_globals &bandwidth;
    s_network_broadcast_search_globals &broadcast;
};
#endif

struct s_network_observer_setup_result
{
    bool initialized;
    c_network_observer *observer;
};

// @address 0x8c447c g_network_time_globals

class c_network_message_handler;


// @address 0x3a4b10 ?network_startup_transport@@YAXPAX@Z
void network_startup_transport(void *)
{
    if (!network_globals.initialized) return;
    if (g_network_link != 0) g_network_link->create_endpoints();
}

void network_shutdown_transport(void *)
{
  if (network_globals.initialized != '\0') {
    if (network_globals_link_storage() != 0) {
      network_globals_link_storage()->destroy_endpoints();
    }
    if (network_life_cycle_observer_storage() != 0) {
      network_life_cycle_observer_storage()->set_online_network_environment(false);
    }
  }
  return;
}

// @address 0x8efe8 ?network_receive@@YAXXZ
void network_receive(void)
{
    if (network_globals.initialized)
    {
        network_globals.entered = true;
        g_network_time_globals.enabled = true;
        g_network_time_globals.time = GetTickCount();
        network_link_process_incoming_packets(reinterpret_cast<void *>(g_network_link));
        g_network_time_globals.enabled = false;
        g_network_time_globals.time = GetTickCount();
        network_globals.entered = false;
    }
}

static __forceinline bool network_send_mode_is_custom(long mode)
{
    return mode == 7 || mode == 8;
}

static __forceinline bool network_send_mode_allows_object(long mode)
{
    return mode != 4 && mode != 6 && mode != 8;
}

void network_send(void)
{
    if (network_globals.initialized) {
        network_globals.entered = true;
        g_network_time_globals.enabled = 1;
        g_network_time_globals.time = GetTickCount();
#if defined(_XBOX) && !defined(__clang__)
        s_network_send_state &interfaces = g_network_send_state;
        for (int i = 0; i < 3; i++)
            network_session_idle(&interfaces.sessions[i]);
        c_network_observer::monitor(interfaces.observer);
#else
        for (int i = 0; i < 3; i++)
            network_session_idle(&network_life_cycle_session_storage()[i]);
        c_network_observer::monitor(network_life_cycle_observer_storage());
#endif
        if (!simulation_globals_get()->aborted) {
            c_simulation_world *session = simulation_globals_get()->world;
            long mode = session->m_world_type;
            if (mode != 0 && simulation_starting_up() && network_send_mode_is_custom(mode) && network_send_mode_allows_object(mode))
                session->m_distributed_world->m_entity_database.process_pending_updates();
        }
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x4097d0: the link loads at -8 from the g_network_send_state base register
        ((c_network_link *)*(void **)((char *)&g_network_send_state - 8))->process_all_channels();
        interfaces.gateway->send_all_pending_messages();
#else
        g_network_link->process_all_channels();
        network_globals_gateway_storage()->send_all_pending_messages();
#endif
        g_network_time_globals.enabled = 0;
        g_network_time_globals.time = GetTickCount();
        network_globals.entered = false;
    }
}

// --- 0x8f578 transport transition ---


// @address 0x8f578 network_update_transport

void network_update_transport(void)
{
    if (transport_startup_state.initialized) {
        bool available = transport_network_available();
        if (transport_startup_state.network_available != available) {
            transport_startup_state.network_available = available;
            if (available)
                transport_startup();
            else
                transport_shutdown();
        }
        transport_secure_address_resolve();
    }
}

extern "C" {
// match: 0x3a3cb8: this unit owns the shared cache that anchors the pooled online initialization globals.
// @address 0x806460 g_online_cached_users
online_xenon_user_state g_online_cached_users[4];
}

// XUSER_SIGNIN_INFO


// @address 0x7fc4f8 ?network_globals@@3Us_network_globals@@A
s_network_globals network_globals;

// @address 0x7fc368 g_transport_security_globals
s_transport_security_globals g_transport_security_globals;

static __forceinline const s_machine_identifier *transport_unique_identifier_get()
{
    return &g_transport_security_globals.local_machine_identifier;
}
// The final mix of fast_checksum_buffer (Bob Jenkins' lookup3).
static __forceinline void fast_checksum_finalize(unsigned long &a, unsigned long &b, unsigned long &c)
{
    c ^= b;
    c -= (b >> 18) | (b << 14);
    a ^= c;
    a -= (c >> 21) | (c << 11);
    b ^= a;
    b -= (a << 25) | (a >> 7);
    c ^= b;
    c -= (b >> 16) | (b << 16);
    a ^= c;
    a -= (c >> 28) | (c << 4);
    b ^= a;
    b -= (a >> 18) | (a << 14);
    c ^= b;
    c -= (b << 24) | (b >> 8);
}

template <class T> static __forceinline unsigned long fast_checksum(unsigned long checksum, const T *data)
{
    unsigned long a, b, c;
    const unsigned long *words = (const unsigned long *)data;
    a = b = c = 0xdeadbeef + sizeof(T) + checksum;
    switch (sizeof(T)) {
    case 8: a += words[0]; b += words[1]; break;
#if defined(_XBOX) && !defined(__clang__)
    case 6: a += words[0]; b += words[1] >> 16; break;
#else
    case 6: {
        unsigned long word0;
        unsigned short half2;
        memcpy(&word0, (const unsigned char *)data, sizeof(word0));
        memcpy(&half2, (const unsigned char *)data + 4, sizeof(half2));
        a += word0;
        b += half2;
        break;
    }
#endif
    case 4: a += words[0]; break;
    }
    fast_checksum_finalize(a, b, c);
    return c;
}

// the identifier is passed by value: c2 reads its first word straight from transport_security_globals and only the
// second word, which runs past the 6 byte struct, from the copy
static __forceinline unsigned __int64 online_user_make_identifier(unsigned long seed, unsigned __int64 xuid, s_machine_identifier identifier)
{
    unsigned long a, b, c;
    const unsigned long *xuid_words = (const unsigned long *)&xuid;
    a = b = c = seed + 0xdeadbeef + sizeof(xuid);
    a += xuid_words[0];
    b += xuid_words[1];
    fast_checksum_finalize(a, b, c);
    unsigned long low = c;
    a = b = c = low + 0xdeadbeef + sizeof(identifier);
#if defined(_XBOX) && !defined(__clang__)
    const unsigned long *identifier_words = (const unsigned long *)&identifier;
    a += identifier_words[0];
    b += identifier_words[1] >> 16;
#else
    unsigned long id_word0;
    unsigned short id_half2;
    memcpy(&id_word0, identifier.bytes, sizeof(id_word0));
    memcpy(&id_half2, identifier.bytes + 4, sizeof(id_half2));
    a += id_word0;
    b += id_half2;
#endif
    fast_checksum_finalize(a, b, c);
    return ((unsigned __int64)c << 32) | low;
}

// @address 0x94ac8 network_update_online_users

// @address 0x94ac8
void network_update_online_users(void)
{
    if (!network_globals.initialized)
        return;
    bool attention = false;
    bool online_allowed = true;
    bool live_allowed = true;
    // Privilege 253 supplements the named XDK privilege checks.
    BOOL privilege_result;
    for (long user_index = 0; user_index < 4; user_index++) {
        online_xenon_user_state *entry = &g_online_cached_users[user_index];
        bool no_guest = false;
        long query_user = user_index;
        s_signin_info signin;
        s_signin_info sponsor;
        unsigned long status = overlapped_get_signin_info(user_index, 2, &signin);
        if (status == 0 && (signin.flags & 2) != 0) {
            query_user = signin.sponsor_user_index;
            status = overlapped_get_signin_info(query_user, 2, &sponsor);
            if (status == 0)
                signin.xuid = sponsor.xuid;
            if (XUserCheckPrivilege(query_user, (XPRIVILEGE_TYPE)253, &privilege_result) == 0)
                no_guest = entry->online_enabled && privilege_result == 0;
        }
        if (status == 0) {
            bool changed = false;
            if (signin.xuid != entry->offline_xuid
                || ((signin.flags & 2) != 0 && entry->guest_number == -1))
                changed = true;
            else if ((signin.flags & 2) == 0 && entry->guest_number != -1)
                changed = true;
            if (changed) {
                entry->online_xuid = 0;
                unsigned long seed = 0xffffffff;
                if ((signin.flags & 2) != 0) {
                    entry->guest_number = signin.guest_number;
                    seed = fast_checksum(seed, &user_index);
                } else {
                    entry->guest_number = -1;
                }
                unsigned __int64 combined = online_user_make_identifier(seed, signin.xuid, *transport_unique_identifier_get());
                entry->player_identifier = combined;
                if (combined == 0)
                    entry->player_identifier = 1;
            }
            ascii_string_to_wchar_string(signin.name, entry->gamertag, 16, 0);
            bool live = (signin.flags & 1) != 0;
            long state = signin.signin_state;
            entry->offline_xuid = signin.xuid;
            entry->signed_in = true;
            entry->silver_or_gold = live;
            if (state == 2) {
                if (overlapped_get_signin_info(user_index, 1, &sponsor) == 0)
                    entry->online_xuid = sponsor.xuid;
                if (XUserCheckPrivilege(user_index, XPRIVILEGE_MULTIPLAYER_SESSIONS, &privilege_result) == 0) {
                    entry->online_enabled = privilege_result == 1;
                    if (!entry->online_enabled) {
                        bool valid_slot = false;
                        if (controller_globals().controllers[user_index].user_index >= 0 && controller_globals().controllers[user_index].user_index < 4)
                            valid_slot = g_network_session_interface_local_users[controller_globals().controllers[user_index].user_index].state == 2;
                        if (valid_slot)
                            attention = true;
                    }
                }
                if (entry->guest_number == -1) {
                    if (XUserCheckPrivilege(user_index, (XPRIVILEGE_TYPE)253, &privilege_result) == 0)
                        entry->online_restricted = entry->online_enabled && privilege_result == 0;
                } else {
                    entry->online_restricted = no_guest;
                }
            } else if (!live || online_local_xuid_is_online_enabled()) {
                entry->online_enabled = false;
            }
            if (XUserCheckPrivilege(query_user, XPRIVILEGE_USER_CREATED_CONTENT, &privilege_result) == 0 && privilege_result == 0) {
                online_allowed = false;
                if (XUserCheckPrivilege(query_user, XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY, &privilege_result) == 0 && privilege_result == 0)
                    live_allowed = false;
            }
        } else {
            if (entry->signed_in) {
                memset(entry, 0, sizeof(*entry));
                transport_secure_random(8, (unsigned char *)&entry->player_identifier);
                if (entry->player_identifier == 0)
                    entry->player_identifier = 2;
            }
        }
    }
    for (long i = 0; i < 4; i++) {
        online_xenon_user_state *user = &g_online_cached_users[i];
        user->can_use_file_share = online_allowed;
        user->can_use_friends_user_created_content = live_allowed;
        if (attention && user->signed_in && user->silver_or_gold)
            user->online_enabled = false;
    }
}

// @address 0x8f2b8 network_update
extern "C" void network_update(void)
{
    network_update_transport();
    if (network_globals.initialized)
    {
        network_globals.entered = true;
        g_network_time_globals.enabled = true;
        g_network_time_globals.time = GetTickCount();
        network_configuration_update();
        network_bandwidth_update();
        network_broadcast_search_update();
        network_recruiting_search_update();
        ((c_session_tracker *)&session_tracker_globals)->update();
        network_session_interface_update();
        network_join_update();
        if (g_network_life_cycle_globals.initialized)
        {
            multiplayer_game_hopper_update();
            g_network_life_cycle_globals.manager.update();
            if ((long)main_globals.recursion_count == 0)
            {
                g_qos_reply_manager_globals.live_service_qos.update();
                g_qos_reply_manager_globals.squad_session_qos_reply.update();
                g_qos_reply_manager_globals.group_session_qos_reply.update();
            }
        }
        network_banhammer_update_map_manifest();
        network_banhammer_update_machine();
        network_banhammer_update_controllers();
        network_banhammer_find_cheaters_in_session();
        network_banhammer_report_on_cheaters();
        for (unsigned long session_index = 0; session_index < 8; ++session_index)
        {
            unsigned long flags = online_session_manager_globals.sessions[session_index].flags;
            if ((flags & (1ul << _online_managed_session_created_bit)))
                managed_session_process_pending_operations(session_index);
        }
        network_update_online_users();
        voice_update();
        network_leaderboard_remove_users();
        parse_player_gamertag_string();
        network_leaderboard_update_leaderboards();
        network_leaderboard_update_queries();
        network_leaderboard_update_writes_in_progress();
        network_session_update_team_indices();
        if (network_arbitration_globals.status == 1)
        {
            switch (g_online_arbitration_globals.registration->get_task_state())
            {
            case 0:
            case 5:
                network_arbitration_globals.status = 3;
                break;
            case 1:
            case 2:
            case 3:
                break;
            case 4:
                network_arbitration_globals.status = 2 + !online_arbitration_registration_get_result(&network_arbitration_globals.result);
                break;
            default:
                // match: 0x8f2b8: unreachable hint omits dead branch check
                __assume(0);
            }
        }
        network_storage_queue_update();
        c_network_storage_manifest *storage_manifest = network_storage_manifest_get();
        if (storage_manifest)
            network_storage_manifest_update(storage_manifest);
        network_storage_cache_update(&g_network_storage_cache);
        network_webstats_update_upload_state();
        online_guide_update_gamercard_dynamic_actions();
        online_guide_update_get_game_details();
        online_guide_update_service_record();
        g_rich_presence_manager.update();
        g_online_file_manager.update();
        long last_time = g_network_request_cache_globals.first_entry_time;
        unsigned long now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
        if ((long)(now - last_time) > g_network_configuration.request_cache_clear_interval)
            memset(&g_network_request_cache_globals, 0, sizeof(g_network_request_cache_globals));
        network_banhammer_update_user_download(&g_online_service_record_manager);
        g_online_lsp_manager.update();
        if (g_game_state_pending_request.enabled)
        {
            if (GetTickCount() > g_game_state_pending_request.last_time + 500)
                g_game_state_pending_request.last_time = GetTickCount();
            g_game_state_pending_request.state = 1;
        }
        else if (g_game_state_pending_request.state != 0)
            g_game_state_pending_request.state = 0;
        g_network_time_globals.enabled = false;
        g_network_time_globals.time = GetTickCount();
        network_globals.entered = false;
    }
}

// @address 0x8c444c g_network_link
#if defined(_XBOX) && !defined(__clang__)
// match: 0x8c444c: @shared_bss 0x828c4448..0x828c446c places these pointers only as static datums
static c_network_link *network_globals_link;
static c_network_message_type_collection *network_globals_message_types;
#else
c_network_link *g_network_link;
c_network_message_type_collection *g_network_message_type_collection;
#endif
static c_network_message_gateway *network_globals_message_gateway;
static c_network_message_handler *network_globals_message_handler;

c_network_link *&network_globals_link_storage()
{
#if defined(_XBOX) && !defined(__clang__)
    return network_globals_link;
#else
    return g_network_link;
#endif
}
c_network_message_type_collection *&network_globals_message_types_storage()
{
#if defined(_XBOX) && !defined(__clang__)
    return network_globals_message_types;
#else
    return g_network_message_type_collection;
#endif
}
c_network_message_gateway *&network_globals_gateway_storage()
{
    return network_globals_message_gateway;
}
c_network_message_handler *&network_globals_handler_storage()
{
    return network_globals_message_handler;
}

// @address 0x80a238 g_online_url_name_globals
#if !defined(_XBOX) || defined(__clang__)
// The Xbox image places these globals in its shared BSS pool.
s_online_url_name_globals g_online_url_name_globals;
// @address 0x808560 g_simulation_globals
s_simulation_globals g_simulation_globals;
#endif

static __forceinline void network_globals_destroy_session(c_network_session *session)
{
    if (session->m_session_manager)
        session->m_session_manager->m_sessions[session->m_session_index] = 0;
    if (session->m_local_state != -1)
    {
        session->m_disconnect_reestablish_state = 1;
        session->disconnect();
    }
    session->m_local_state = static_cast<e_network_session_state>(-1);
    session->m_session_class = static_cast<e_network_session_class>(-1);
    session->m_session_type = static_cast<e_network_session_type>(0);
    session->m_message_gateway = 0;
    session->m_observer = 0;
}

static __forceinline network_globals_online_initialization_view &network_globals_online_storage()
{
#if defined(_XBOX) && !defined(__clang__)
    return *reinterpret_cast<network_globals_online_initialization_view *>(g_online_cached_users);
#else
    static network_globals_online_initialization_view interfaces = {transport_startup_state,
        network_bandwidth_globals, g_broadcast_search_globals};
    return interfaces;
#endif
}
// @address 0x79e228 g_network_configuration_state

static __forceinline void network_globals_register_message(c_network_message_type_collection *collection, e_network_message_type index, const char *name,
    long flags, long minimum_size, long maximum_size,
    void (*encode)(c_bitstream *, long, const void *), bool (*decode)(c_bitstream *, long, void *),
    bool (*compare)(long, void *, void *), void (*dispose)(long, void *))
{
    s_network_message_type_definition *definition = &collection->m_message_types[index];
    definition->name = name;
    definition->flags = flags;
    definition->minimum_size = minimum_size;
    definition->maximum_size = maximum_size;
    definition->encode = encode;
    definition->decode = decode;
    definition->compare = compare;
    definition->dispose = dispose;
    definition->initialized = true;
}
static __forceinline bool network_globals_initialize_link(c_network_link *link)
{
    c_network_link *storage = link;
    storage->m_game_sent_statistics.initialize(2000);
    storage->m_game_received_statistics.initialize(2000);
    storage->m_wire_sent_statistics.initialize(2000);
    storage->m_wire_received_statistics.initialize(2000);
    if (transport_available(network_globals_online_storage().transport))
        link->create_endpoints();
    storage->m_initialized = true;
    return true;
}
static __forceinline bool network_globals_initialize_gateway(c_network_message_gateway *gateway, c_network_link *link, c_network_message_type_collection *types)
{
    c_network_message_gateway *storage = gateway;
    storage->m_link = link;
    storage->m_link->m_out_of_band_consumer = gateway;
    storage->m_message_types = types->m_message_types;
    storage->m_message_handler = 0;
    storage->m_outgoing_packet_pending = false;
    storage->m_initialized = true;
    return true;
}
static __forceinline bool network_globals_initialize_handler(
    c_network_message_handler *handler, c_network_link *link, c_network_message_type_collection *types, c_network_message_gateway *gateway)
{
    c_network_message_handler *storage = handler;
    storage->m_link = link;
    storage->m_message_gateway = gateway;
    storage->m_message_types = types;
    gateway->m_message_handler = handler;
    storage->m_observer = 0;
    storage->m_session_manager = 0;
    storage->m_initialized = true;
    return true;
}
static __forceinline bool network_globals_initialize_session_manager(c_network_session_manager *manager)
{
    c_network_session_manager *storage = manager;
    storage->m_sessions[0] = 0;
    storage->m_sessions[1] = 0;
    storage->m_sessions[2] = 0;
    return true;
}
static __forceinline bool network_globals_initialize_broadcast(c_network_link *link, c_network_message_gateway *gateway)
{
    s_network_broadcast_search_globals &storage = network_globals_online_storage().broadcast;
    storage.link = link;
    storage.message_gateway = gateway;
    storage.initialized = true;
    return true;
}
static __forceinline bool network_globals_initialize_recruiting()
{
    memset(&g_recruiting_search_globals, 0, sizeof(g_recruiting_search_globals));
    g_recruiting_search_globals.initialized = true;
    return true;
}
static __forceinline bool network_globals_initialize_interface(c_network_session_manager *manager)
{
    network_session_interface_globals *storage = &g_network_session_interface_globals;
    memset(storage, 0, sizeof(*storage));
    storage->sessions = reinterpret_cast<s_network_session **>(manager->m_sessions);
    storage->peer.initialized = true;
    storage->peer.map_hopper_identifier = 0xffff;
    storage->peer.hopper_identifier = 0xffff;
    storage->peer.hopper_error = 0;
    storage->film_reset_time = -1;
    for (unsigned long index = 0; index < 4; ++index)
        storage->users[index].desired_team_index = -1;
    return storage->peer.initialized;
}
static __forceinline bool network_globals_initialize_leaderboard()
{
    s_network_leaderboard_globals *storage = &network_leaderboard_globals;
    memset(storage, 0, sizeof(*storage));
    storage->initialized = true;
    memset(storage->users, 0, sizeof(storage->users));
    storage->global_leaderboard = static_cast<e_online_leaderboard_id>(1);
    storage->custom_leaderboard = static_cast<e_online_leaderboard_id>(2);
    storage->playlist = 0xffff;
    storage->playlist_skill_leaderboard = k_online_leaderboard_id_invalid;
    storage->playlist_leaderboard = k_online_leaderboard_id_invalid;
    storage->refresh_time = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    return true;
}
static __forceinline bool network_globals_initialize_arbitration()
{
    memset(&network_arbitration_globals, 0, sizeof(network_arbitration_globals));
    network_arbitration_globals.index = -1;
    network_arbitration_globals.initialized = true;
    return true;
}
static inline bool network_globals_can_initialize()
{
    const bool &initialize = !network_globals.initialized;
    return initialize;
}
static __forceinline void network_globals_register_observer(c_network_message_handler *handler, c_network_observer *observer)
{
    handler->m_observer = observer;
}
static __forceinline bool network_globals_initialize_bandwidth(c_network_observer *observer)
{
    s_bandwidth_globals &storage = network_globals_online_storage().bandwidth;
    storage.observer = observer;
    storage.settings = &g_network_configuration.bandwidth;
    storage.online_network_environment = false;
    storage.estimated = false;
    storage.bandwidth = -1;
    storage.capacity = 16;
    storage.history = &observer->m_network_quality_statistics.bandwidth_history;
    storage.initialized = true;
    return true;
}

#if defined(_XBOX) && !defined(__clang__)
static s_network_observer_setup_result network_globals_observer_setup(bool initialized, s_network_interfaces &interfaces)
#else
static s_network_observer_setup_result network_globals_observer_setup(bool initialized, c_network_observer *&observer,
    c_network_link *&link, c_network_message_type_collection *&message_types,
    c_network_message_gateway *&gateway, c_network_message_handler *&handler)
#endif
{
#if defined(_XBOX) && !defined(__clang__)
    c_network_observer *&observer = interfaces.observer;
    c_network_link *&link = interfaces.link;
    c_network_message_type_collection *&message_types = interfaces.message_types;
    c_network_message_gateway *&gateway = interfaces.gateway;
    c_network_message_handler *&handler = interfaces.handler;
#endif
    s_network_observer_setup_result result;
    result.initialized = initialized;
    if (initialized)
        result.initialized = c_network_observer::initialize_observer(observer, link,
            message_types, gateway, handler,
            &g_network_configuration.observer) != false;
    result.observer = observer;
    return result;
}

// @address 0x3a3cb8 ?network_initialize@@YAXXZ
void network_initialize()
{
    bool initialize = network_globals_can_initialize();
    if (initialize)
    {
        network_configuration_initialize_private(false);
        g_network_configuration_state.download.m_downloader.m_automatic_retry_mode = 2;
#if defined(_XBOX) && !defined(__clang__)
        s_network_interfaces &interfaces = g_network_interfaces;
        c_network_link *&link = interfaces.link;
        c_network_message_type_collection *&message_types = interfaces.message_types;
        c_network_message_gateway *&gateway = interfaces.gateway;
        c_network_message_handler *&handler = interfaces.handler;
        c_network_observer *&observer = interfaces.observer;
        c_network_session *&sessions = interfaces.sessions;
        c_network_session_parameter_type_collection *&parameter_types = interfaces.parameter_types;
        c_network_session_manager *&manager = interfaces.manager;
#else
        c_network_link *&link = network_globals_link_storage();
        c_network_message_type_collection *&message_types = network_globals_message_types_storage();
        c_network_message_gateway *&gateway = network_globals_gateway_storage();
        c_network_message_handler *&handler = network_globals_handler_storage();
        c_network_observer *&observer = network_life_cycle_observer_storage();
        c_network_session *&sessions = network_life_cycle_session_storage();
        c_network_session_parameter_type_collection *&parameter_types = network_life_cycle_parameter_storage();
        c_network_session_manager *&manager = network_life_cycle_manager_storage();
#endif
        bool memory_initialized = network_memory_initialize(&link, &message_types, &gateway,
            &handler, &observer, &sessions,
            &manager, &parameter_types);
        if (memory_initialized)
        {
            bool success = true;
            success = success && network_globals_initialize_link(link);
            memset(message_types, 0, sizeof(*message_types));
            network_message_types_register_out_of_band(message_types);
            network_message_types_register_connect(message_types);
            network_message_types_register_session_protocol(message_types);
            network_message_types_register_session_membership(message_types);
            {
                c_network_message_type_collection *types = message_types;
                network_globals_register_message(types, static_cast<e_network_message_type>(34), "parameters-update", 0, 0x1820, 0x1820,
                    c_network_message_parameters_update::encode, c_network_message_parameters_update::decode, 0, 0);
                network_globals_register_message(types, static_cast<e_network_message_type>(35), "parameters-request", 0, 0x1810, 0x1810,
                    c_network_message_parameters_request::encode, c_network_message_parameters_request::decode, 0, 0);
            }
            {
                c_network_message_type_collection *types = message_types;
                network_globals_register_message(types, static_cast<e_network_message_type>(36), "view-establishment", 0, 76, 76,
                    c_network_message_view_establishment::encode, c_network_message_view_establishment::decode, 0, 0);
                network_globals_register_message(types, static_cast<e_network_message_type>(37), "player-acknowledge", 0, 136, 136,
                    c_network_message_player_acknowledge::encode, c_network_message_player_acknowledge::decode, 0, 0);
            }
            network_message_types_register_simulation_synchronous(message_types);
            network_globals_register_message(message_types, static_cast<e_network_message_type>(43), "game-results", 0, 0x63e0,
                0x63e0, reinterpret_cast<void (*)(c_bitstream *, long, const void *)>(network_message_synchronous_encode_game_results),
                    network_message_synchronous_decode_game_results, 0, 0);
            network_globals_register_message(message_types, static_cast<e_network_message_type>(45), "test", 0, 8, 8,
                network_message_test_encode, c_network_message_test::decode, 0, 0);
            success = success && network_globals_initialize_gateway(gateway, link, message_types);
            success = success && network_globals_initialize_handler(handler, link, message_types, gateway);
#if defined(_XBOX) && !defined(__clang__)
            s_network_observer_setup_result observer_result = network_globals_observer_setup(success, interfaces);
#else
            s_network_observer_setup_result observer_result = network_globals_observer_setup(success, observer, link, message_types, gateway, handler);
#endif
            success = observer_result.initialized;
            network_globals_register_observer(handler, observer_result.observer);
            memset(parameter_types, 0, sizeof(*parameter_types));
            network_session_parameter_types_register(parameter_types);
            g_network_session_parameter_message_types = parameter_types;
            success = success && network_globals_initialize_session_manager(manager);
            g_network_time_globals.session_manager = manager;
            handler->m_session_manager = manager;
            success = success &&
                sessions
                    ->initialize_session(0, _network_session_type_squad, gateway, observer, manager);
            success = success &&
                sessions[1].initialize_session(1, _network_session_type_squad, gateway, observer, manager);
            success = success &&
                sessions[2].initialize_session(2, _network_session_type_group, gateway, observer, manager);
            success = success && network_banhammer_initialize();
            success = success && network_globals_initialize_broadcast(link, gateway);
            success = success && network_globals_initialize_recruiting();
            success = success && network_search_initialize();
            success = success && network_life_cycle_initialize(observer, manager,
                sessions,
                    &sessions[1],
                &sessions[2]);
            success = success && network_globals_initialize_interface(manager);
            success = success && network_globals_initialize_leaderboard();
            success = success && network_globals_initialize_arbitration();
            success = success && network_globals_initialize_bandwidth(observer);
            success = success && ((c_session_tracker *)&session_tracker_globals)->initialize(false);
            if (success)
            {
                memset(&g_online_arbitration_globals, 0, sizeof(g_online_arbitration_globals));
                transport_register_transition_functions(network_startup_transport, network_shutdown_transport, 0, 0);
                g_online_arbitration_globals.registration
                    = new (0, 0, _overlapped_allocation_marker) c_arbitration_overlapped_task(0, 0);
                g_online_arbitration_globals.initialized = true;
                online_statistics_initialize();
                online_session_search_initialize();
                for (unsigned long index = 0; index < 4; ++index)
                {
                    transport_secure_random(8, reinterpret_cast<unsigned char *>(&g_online_cached_users[index].player_identifier));
                    if (!g_online_cached_users[index].player_identifier)
                        g_online_cached_users[index].player_identifier = 3;
                    g_online_cached_users[index].can_use_file_share = true;
                    g_online_cached_users[index].can_use_friends_user_created_content = true;
                }
                memset(&g_online_url_name_globals, 0, sizeof(g_online_url_name_globals));
                network_storage_queue_clear();
                c_network_storage_manifest *storage_manifest = network_storage_manifest_get();
                if (storage_manifest)
                {
                    storage_manifest->m_expiration_time = 0;
                    storage_manifest->m_manifest = 0;
                    storage_manifest->m_downloader.c_http_blf_simple_downloader<s_online_file_manifest>::c_http_blf_simple_downloader();
                    storage_manifest->m_downloader.set_automatic_retries_enabled(true);
                    memset(storage_manifest->m_untracked_entries, 0, sizeof(storage_manifest->m_untracked_entries));
                    network_storage_cache_reset_manifest(storage_manifest);
                }
                g_network_storage_cache.initialize(0x3200000);
                voice_initialize();
                memset(&g_online_lsp_globals, 0, sizeof(g_online_lsp_globals));
                g_online_lsp_globals.search_task = new (0, 0, _overlapped_allocation_marker) c_lsp_search_task(0, 0);
                for (unsigned long index = 0; index < 16; ++index)
                {
                    g_online_lsp_globals.registered_servers[index].registered = false;
                    g_online_lsp_globals.registered_servers[index].ip_address = 0;
                }
                s_online_guide_state *guide = &g_online_guide_globals;
                guide->actions = false;
                guide->find_friend_active = false;
                guide->player_details_pending = 0;
                guide->service_record_mode = 0;
                XEnableGuestSignin(true);
                g_storage_url_subdirectory.print("tracked%c%05u", '/', 11855);
            }
            if (success)
            {
                network_globals_online_storage().bandwidth.online_network_environment = false;
                network_globals_online_storage().bandwidth.observer->set_online_network_environment(false);
                network_bandwidth_update_estimate();
                transport_reset();
                network_globals.initialized = true;
            }
            else
                network_dispose();
        }
    }
}

// @address 0x3a46e8 ?network_dispose@@YAXXZ
void network_dispose()
{
    if (network_globals.initialized)
    {
        for (long member_index = 0; member_index < 32; ++member_index)
            network_storage_queue_dispose_member(member_index, _network_storage_queue_task_result_aborted);
        network_storage_queue_clear();
        overlapped_task_block_until_finished(&g_online_guide_globals.compose);
        overlapped_task_block_until_finished(&g_online_guide_globals.find_friend);
        g_online_lsp_globals.desired_state = 0;
        if (g_online_lsp_globals.current_state == 2 || g_online_lsp_globals.current_state == 3)
            reset_immediate();
        overlapped_task_block_until_finished(g_online_lsp_globals.search_task);
        reset_immediate();
        overlapped_track_delete(g_online_lsp_globals.search_task);
        g_online_lsp_globals.search_task = 0;
        memset(&g_online_url_name_globals, 0, sizeof(g_online_url_name_globals));
        overlapped_track_delete(static_cast<c_overlapped_task *>(g_online_arbitration_globals.registration));
        memset(&g_online_arbitration_globals, 0, sizeof(g_online_arbitration_globals));
        online_statistics_dispose();
        overlapped_track_delete(online_session_search_globals.search_task);
        overlapped_track_delete(online_session_search_globals.search_by_id_task);
        memset(&online_session_search_globals, 0, sizeof(online_session_search_globals));
        network_bandwidth_dispose();
        memset(&network_leaderboard_globals, 0, sizeof(network_leaderboard_globals));
        memset(&network_arbitration_globals, 0, sizeof(network_arbitration_globals));
        network_arbitration_globals.index = -1;
        memset(&g_network_session_interface_globals, 0, sizeof(g_network_session_interface_globals));
        network_life_cycle_dispose();
        network_search_end();
        memset(&g_recruiting_search_globals, 0, sizeof(g_recruiting_search_globals));
        network_broadcast_search_dispose();
        if (((c_session_tracker *)&session_tracker_globals)->m_flags.test(_session_tracker_initialized_bit))
            ((c_session_tracker *)&session_tracker_globals)->release_storage();
        for (long session_index = 0; session_index < 3; ++session_index)
            network_globals_destroy_session(&network_life_cycle_session_storage()[session_index]);
        c_network_observer *observer = network_life_cycle_observer_storage();
        observer->destroy_observer();
        memset(network_life_cycle_parameter_storage(), 0, sizeof(c_network_session_parameter_type_collection));
        memset(online_session_manager_globals.sessions, 0, sizeof(online_session_manager_globals.sessions));
        network_memory_shared_dispose();
        c_network_message_handler *handler = network_globals_message_handler;
        if (handler->m_initialized)
        {
            if (handler->m_message_gateway)
            {
                handler->m_message_gateway->m_message_handler = 0;
                handler->m_message_gateway = 0;
            }
            handler->m_session_manager = 0;
            handler->m_message_types = 0;
            handler->m_link = 0;
            handler->m_initialized = false;
        }
        c_network_message_gateway *gateway = network_globals_message_gateway;
        if (gateway->m_initialized)
        {
            if (gateway->m_link)
            {
                gateway->m_link->m_out_of_band_consumer = 0;
                gateway->m_link = 0;
            }
            gateway->m_message_handler = 0;
            gateway->m_message_types = 0;
            gateway->m_initialized = false;
        }
        memset(network_globals_message_types_storage(), 0, sizeof(*network_globals_message_types_storage()));
        c_network_link *link = network_globals_link_storage();
        link->destroy_endpoints();
        c_network_link *link_data = link;
        link_data->m_out_of_band_consumer = 0;
        link_data->m_initialized = false;
        network_globals.initialized = false;
        network_globals_link_storage() = 0;
        network_globals_message_types_storage() = 0;
        network_globals_message_gateway = 0;
        network_globals_message_handler = 0;
        network_life_cycle_observer_storage() = 0;
        network_life_cycle_session_storage() = 0;
        network_memory_base_dispose();
        memset(&g_network_configuration, 0, sizeof(g_network_configuration));
    }
}
