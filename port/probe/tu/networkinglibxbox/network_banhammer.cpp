// networkinglibxbox/network_banhammer.obj: from the records 0x1ad4d8 0x3c4a00 0x3d3428 0x3e8d30 0x4009a0 0x400b18 0x400b98 0x400c70 0x400df8 0x400f70 0x401040 0x401108 0x4012c0 0x401828 0x4024a0 0x4025a8 0x4026a8 0x402888 0x46da20
#include "interface/user_interface_controller_globals.h"
#include "networking/logic/network_banhammer_dependencies.h"
#include "networking/tools/network_debug_dump.h"
#include "interface/user_interface_controller.h"
#include "networking/logic/network_banhammer_globals.h"
#include "math/real_math_types.h"
#include "networking/logic/storage/hopper_load_status_types.h"
#include <stdio.h>
#include "game/game_globals_storage.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "shell/shell.h"
#include "game/map_variant.h"
#include <string.h>
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/logic/network_banhammer.h"
#include "networking/online/online_xenon.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_machine_id.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/session/network_session_membership.h"
#include "cseries/cseries.h"
#include "networking/tools/network_web_events.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "interface/user_interface_error_manager.h"
#include "input/input.h"
#include "networking/logic/network_quality_statistics_types.h"
#include "networking/logic/cheating_player_types.h"
#include "networking/logic/local_player_types.h"
#include "xbox/xnet.h"
#include "game/multiplayer_hopper_services.h"
#include "cseries/language_types.h"
#include "cseries/language.h"
#include "memory/tls_globals.h"
#include "interface/files_machine_banhammer_messages_types.h"
#include "networking/logic/network_banhammer_remote_data_types.h"
#include "interface/files_user_banhammer_messages_types.h"
#include "interface/files_user_recent_players_types.h"
#include "networking/online/online_files.h"
#include "networking/online/online_file_work_item_types.h"
#include "networking/tools/network_blf.h"
#include "networking/tools/network_blf_payload.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include <new>
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/tools/blf_authentication_types.h"
#include "input/input_globals.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "interface/controller_types.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
#include "networking/network_time_types.h"
#include "networking/network_time_access.h"
#include "interface/controller_inline.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "cseries/crt_xdk.h"
#include "cseries/cseries_types.h"
#include <stdarg.h>
#include "memory/gameplay_tls_globals.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "saved_games/saved_game_files_globals.h"
// @address 0x82308 ?print@?$c_static_string@$0BAA@@@QAAPBDPBDZZ
#if defined(_XBOX) && !defined(__clang__)
// @address 0x8c447c banhammer_network_time_globals
#define NETWORK_BANHAMMER_TIME_GLOBALS banhammer_network_time_globals
#else
#define NETWORK_BANHAMMER_TIME_GLOBALS g_network_time_globals
#endif
// @flags /TP /O2 /Oc
// @address 0x400b98 ?network_banhammer_initialize@@YA_NXZ
// @address 0x400df8 ?network_banhammer_remote_player_caught_cheating@@YAXPB_KPBUs_transport_secure_address@@PB_WPIB_KK@Z
// @address 0x400c70 ?network_banhammer_loaded_for_controller@@YA?AW4e_hopper_load_status@@W4e_controller_index@@PA_N@Z


struct utf8;

union s_banhammer_identifier_bytes
{
    unsigned __int64 value;
    unsigned char bytes[8];
};


enum e_download_status
{
    _download_status_none = 0,
    _download_status_failed = 1,
    _download_status_downloading = 2,
    _download_status_complete = 4
};


enum e_network_banhammer_download_state
{
    _network_banhammer_download_idle = 0,
    _network_banhammer_download_pending = 1,
    _network_banhammer_download_complete = 2
};

// ==================== 0x401828 network_banhammer_update_controllers ====================
struct s_banhammer_file_entry
{
    unsigned __int64 requester_xuid;
    long slot;
    unsigned __int64 xuid;
    wchar_t name[16];
    long file_type;
    long campaign_id;
    long map_id;
    long game_engine_type;
    unsigned __int64 size_in_bytes;
};
struct s_banhammer_file_chunk
{
    s_banhammer_file_entry entries[8];
};
struct s_banhammer_ban_entry
{
    unsigned long ban_type;
    unsigned long ban_category;
    unsigned __int64 start_time;
    unsigned __int64 expiry_time;
};
struct s_banhammer_ban_chunk
{
    long count;
    s_banhammer_ban_entry entries[32];
};
struct s_banhammer_ban_upload
{
    unsigned char player_type;
    unsigned long player_flags;
    long value;
    char text[52];
};
struct s_banhammer_message
{
    unsigned char text[256];
};
struct s_banhammer_message_chunk
{
    long count;
    s_banhammer_message messages[32];
};

typedef char banhammer_file_entry_size[(sizeof(s_banhammer_file_entry) == 0x50) ? 1 : -1];
typedef char banhammer_file_entry_xuid_offset[(offsetof(s_banhammer_file_entry, xuid) == 0x10) ? 1 : -1];
typedef char banhammer_ban_chunk_entries_offset[(offsetof(s_banhammer_ban_chunk, entries) == 8) ? 1 : -1];
typedef char banhammer_ban_upload_size[(sizeof(s_banhammer_ban_upload) == 0x40) ? 1 : -1];
typedef char banhammer_ban_upload_flags_offset[(offsetof(s_banhammer_ban_upload, player_flags) == 4) ? 1 : -1];


enum e_network_banhammer_controller_flags
{
    _network_banhammer_controller_valid = 0,
    _network_banhammer_controller_user_loaded = 2,
    _network_banhammer_controller_user_pending = 3,
    _network_banhammer_controller_messages_loaded = 4,
    _network_banhammer_controller_message_displayed = 5
};

struct network_banhammer7_url : s_network_http_buffer_url
{
    __forceinline network_banhammer7_url()
    {
        name[0] = 0;
        service_type = 0;
        cache_mode = 0;
        lifetime_seconds = 600;
        queue_type = 0;
    }
};


struct network_banhammer8_url : s_network_http_buffer_url
{
 __forceinline network_banhammer8_url()
 {
  name[0]=0; service_type=0; cache_mode=0; lifetime_seconds=600; queue_type=0;
 }
};

static __forceinline const gamepad_state *network_banhammer2_input_get_gamepad_state(short controller_index)
{
    const gamepad_state *result = 0;
    if (input_globals.gamepad_connected[controller_index])
    {
        if (input_globals.input_suppressed)
            result = &input_globals.suppressed_gamepad;
        else
            result = &input_globals.gamepads[controller_index];
    }
    return result;
}


static __forceinline unsigned __int64 network_banhammer2_get_local_user_xuid(short user_index)
{
    return g_network_session_interface_globals.users[user_index].configuration.client.xuid;
}


static __forceinline unsigned __int64 network_banhammer2_get_player_xuid(const c_controller_interface *controller)
{
    unsigned __int64 result = 0;
    short user_index = controller->get_user_index();
    if (user_index >= 0 && user_index < 4)
        result = network_banhammer2_get_local_user_xuid(user_index);
    return result;
}


s_network_banhammer_globals g_network_banhammer_globals;
// @address 0x8c446d g_network_banhammer_initialized
extern "C" {
bool g_network_banhammer_initialized;
}
static __forceinline unsigned long network_banhammer_get_controller_ban_flags(long controller_index)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
    unsigned long result = 0;
    if (TEST_BIT(controller->flags, 0))
        result = controller->ban_flags | g_network_banhammer_globals.ban_flags;
    return result;
}

// 0x400b98
bool network_banhammer_initialize()
{
    g_network_banhammer_globals.flags = 0;
    g_network_banhammer_globals.cheats = 0;
    g_network_banhammer_globals.ban_flags = 0;
    g_network_banhammer_globals.machine_file_index = 0;
    g_network_banhammer_globals.machine_file_time = 0;
    for (e_controller_index controller_index = first_controller(); controller_index != -1;
        controller_index = next_controller(controller_index))
    {
        memset(&g_network_banhammer_globals.controllers[controller_index], 0, sizeof(s_network_banhammer_controller_data));
        g_network_banhammer_globals.controller_downloaders[controller_index].user_file.reset();
        g_network_banhammer_globals.controller_downloaders[controller_index].recent_players.reset();
    }
    memset(g_network_banhammer_globals.remotes, 0, sizeof(g_network_banhammer_globals.remotes));
    g_network_banhammer_globals.machine_messages_downloader.set_automatic_retries_enabled(true);
    g_network_banhammer_globals.machine_file_downloader.reset();
    g_network_banhammer_initialized = true;
    return g_network_banhammer_initialized;
}

e_hopper_load_status network_banhammer_loaded_for_controller(e_controller_index controller_index, bool *out_status)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
    long status = 0;
    if (TEST_BIT(controller->flags, 0))
    {
        bool loaded = true;
        loaded = loaded && TEST_BIT(g_network_banhammer_globals.flags, 2);
        loaded = loaded && TEST_BIT(controller->flags, 2);
        if (network_banhammer_get_controller_ban_flags(controller_index) != 0)
            loaded = loaded && TEST_BIT(controller->flags, 5);
        if (!loaded)
        {
            status = TEST_BIT(g_network_banhammer_globals.flags, 0) ? 3 : 1;
        }
        else
        {
            status = 2;
            g_network_banhammer_globals.flags &= ~FLAG(0);
        }
    }
    return static_cast<e_hopper_load_status>(status);
}


// 0x400df8
void network_banhammer_remote_player_caught_cheating(const unsigned __int64 *player_xuid, const s_transport_secure_address *secure_address,
    // match: 0x400df8: hoists the *player_identifier load above the trailing name NUL store (ld r11,0(r6) before sth r30,1Eh(r7))
    const wchar_t *player_name, const unsigned __int64 *__restrict player_identifier, unsigned long cheat_flags)
{
    for (long cheater_index = 0; cheater_index < 64; cheater_index++)
    {
        s_network_banhammer_remote_data *cheater = &g_network_banhammer_globals.remotes[cheater_index];
        if (cheater->valid)
        {
            if (memcmp(&cheater->xuid, player_xuid, sizeof(cheater->xuid)) == 0)
            {
                if (cheater->cheats != cheat_flags)
                {
                    cheater->cheats |= cheat_flags;
                    cheater->reported = false;
                }
                break;
            }
        }
        else
        {
            memset(cheater, 0, sizeof(*cheater));
            ustrnzcpy_inline(cheater->name, player_name, 16);
            cheater->machine = *player_identifier;
            transport_secure_address_get_machine_id(secure_address, &cheater->identifier);
            cheater->xuid = *player_xuid;
            cheater->cheats = cheat_flags;
            cheater->reported = false;
            cheater->valid = true;
            break;
        }
    }
}

static __forceinline bool network_banhammer_life_cycle_get_observer(c_network_observer **observer)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized)
    {
        *observer = g_network_life_cycle_globals.manager.m_observer;
        result = true;
    }
    return result;
}

// 0x400f70
void network_banhammer_upload_network_statistics()
{
    if (!TEST_BIT(g_network_banhammer_globals.flags, 4))
    {
        c_network_observer *observer = 0;
        network_banhammer_life_cycle_get_observer(&observer);
        const s_network_quality_statistics *statistics = observer->quality_statistics_get();
        if (memcmp(&g_network_banhammer_globals.network_statistics, statistics, sizeof(s_network_quality_statistics)) != 0)
        {
            g_network_banhammer_globals.network_statistics_upload_downloader.reset();
            memcpy(&g_network_banhammer_globals.network_statistics, statistics, sizeof(s_network_quality_statistics));
            g_network_banhammer_globals.flags |= FLAG(4);
        }
    }
}


// 0x401040
void network_banhammer_initialize_controller(e_controller_index controller_index)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
    memset(controller, 0, sizeof(*controller));
    controller->repeated_play_index = -1;
    controller->last_activity_time = network_time_get();
    controller->session_start_time = network_time_get();
    controller->highest_skill = 1;
    g_network_banhammer_globals.controller_downloaders[controller_index].recent_players.reset();
    g_network_banhammer_globals.controller_downloaders[controller_index].user_file.reset();
}

// @address 0x4025a8 network_banhammer_get_local_players
extern "C" void network_banhammer_get_local_players(s_network_web_event_local_player *players, long maximum, long *out_count)
{
    // match: 0x4025a8: value range assumption guides branch lowering
    __assume(maximum == 4);
    long count = 0;
    memset(players, 0, maximum * sizeof(s_network_web_event_local_player));
    for (e_controller_index controller_index = first_controller(); controller_index != -1;
        controller_index = next_controller(controller_index))
    {
        s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
        if (TEST_BIT(controller->flags, 0))
        {
            s_network_web_event_local_player *player = &players[count];
            player->controller_index = controller_index;
            ustrnzcpy_inline(player->name, controller->name, 16);
            player->xuid = controller->xuid;
            count++;
        }
    }
    *out_count = count;
}


// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0IIKB@@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0IHB@@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0CIKB@@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0EAA@@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0KNB@@@6B@
// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0BHKB@@@6B@

/* Appends formatted text to a 1156-byte buffer and always terminates it. */
// @address 0x1ad4d8 ?append_print@?$c_static_string@$0EIE@@@QAAPADPBDZZ
template<long k_maximum_count>
char *c_static_string<k_maximum_count>::append_print(const char *format, ...)
{
  const char *p = m_string;
  unsigned int length;
  for (length = 0; length < k_maximum_count; length++) {
    if (*p++ == 0) break;
  }
  int remaining = k_maximum_count - length;
  char *end = m_string + length;
  va_list args;
  va_start(args, format);
  _vsnprintf(end, remaining - 1, format, args);
  end[remaining - 1] = 0;
  return m_string;
}
template char *c_static_string<1156>::append_print(const char *format, ...);
extern template char *c_static_string<256>::append_print(const char *format, ...);
extern template char *c_static_string<1024>::append_print(const char *format, ...);

static __forceinline unsigned long network_banhammer_get_controller_cheat_flags(long index)
{
    s_network_banhammer_controller_data *data = &g_network_banhammer_globals.controllers[index];
    unsigned long result = 0;
    if (data->flags & 1)
        result = g_network_banhammer_globals.controllers[index].cheats | g_network_banhammer_globals.cheats;
    return result;
}
// @address 0x4026a8 ?network_banhammer_report_on_cheaters@@YAXXZ
void network_banhammer_report_on_cheaters(void)
{
    for (e_controller_index index = _controller0; index != -1; index = next_controller(index)) {
        s_network_banhammer_controller_data *data = &g_network_banhammer_globals.controllers[index];
        if (data->flags & 1) {
            unsigned long cheats = network_banhammer_get_controller_cheat_flags(index);
            if (cheats != data->reported) {
                s_network_web_event_local_player players[4];
                long count = 0;
                cheating_player event;
                memset(&event, 0, sizeof(event));
                network_banhammer_get_local_players(players, 4, &count);
                event.cheats = cheats;
                event.controller_index = index;
                ustrnzcpy_inline(event.name, data->name, 16);
                event.xuid = data->xuid;
                if (network_web_event_report_local_cheater(&event, players, count))
                    data->reported = cheats;
            }
        }
    }
    for (long index = 0; index < 64; ++index) {
        s_network_banhammer_remote_data *data = &g_network_banhammer_globals.remotes[index];
        if (data->valid && !data->reported) {
            s_network_web_event_local_player players[4];
            long count = 0;
            cheating_player event;
            memset(&event, 0, sizeof(event));
            network_banhammer_get_local_players(players, 4, &count);
            event.cheats = data->cheats;
            ustrnzcpy_inline(event.name, data->name, 16);
            event.xuid = data->xuid;
            event.identifier = data->identifier;
            data->reported = network_web_event_report_remote_cheater(data->machine, &event,
                players, count);
        }
    }
}

// session types for find_cheaters_in_current_session and 0x46da20

// @address 0x3e8d30 network_banhammer_test_local_player_xuids
// 0x3e8d30 (no dev name): every local-peer player must hold a flagged controller with its xuid, at least one such player
extern "C" bool network_banhammer_test_local_player_xuids(const c_life_cycle_state_handler *state_handler)
{
    const c_network_session_membership *membership = state_handler->get_manager()->get_group_session()->get_session_membership();
    long local_peer_index = membership->local_peer_index();
    long count = 0;
    bool valid = true;
    for (long player_index = membership->get_first_player(); player_index != -1; player_index = membership->get_next_player(player_index))
    {
        const s_network_session_player *player = membership->get_player(player_index);
        if (player->peer_index == local_peer_index)
        {
            const s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[player->controller_index];
            bool matched = false;
            if (TEST_BIT(controller->flags, 0))
                matched = controller->xuid == player->configuration.client.xuid;
            if (!matched)
            {
                valid = false;
                break;
            }
            count++;
        }
    }
    return valid && count > 0;
}


// @address 0x4024a0 network_banhammer_find_cheaters_in_session
extern "C" void network_banhammer_find_cheaters_in_session(void)
{
    c_network_session *session = 0;
    if (network_life_cycle_in_session(&session) && session->established())
    {
        const c_network_session_membership *membership = session->get_session_membership();
        long local_peer_index = membership->local_peer_index();
        unsigned __int64 game_instance = simulation_starting_up() ? tls_game_globals()->options.game_instance : 0;
        for (long player_index = membership->get_first_player(); player_index != -1; player_index = membership->get_next_player(player_index))
        {
            const s_network_session_player *player = membership->get_player(player_index);
            if (player->peer_index != local_peer_index && player->configuration.client.cheat_flags != 0)
            {
                const s_network_session_peer *peer = membership->get_peer(player->peer_index);
                network_banhammer_remote_player_caught_cheating(&player->configuration.client.xuid, &peer->secure_address,
                    player->configuration.host.name, &game_instance, player->configuration.client.cheat_flags);
            }
        }
    }
}

static __forceinline void network_banhammer_local_box_caught_cheating(unsigned long cheat_flags)
{
    g_network_banhammer_globals.cheats |= cheat_flags;
    for (e_controller_index controller_index = first_controller(); controller_index != -1;
        controller_index = next_controller(controller_index))
        user_interface_controller_update_network_properties((e_controller_index)controller_index);
}

// @address 0x402888 network_banhammer_test_map_signature
extern "C" bool network_banhammer_test_map_signature(const unsigned char *signature)
{
    bool valid = true;
    s_network_banhammer_map_signatures *map_signatures = &g_network_banhammer_globals.map_signatures;
    if (map_signatures->valid && !map_signatures->disabled)
    {
        bool found = false;
        for (long signature_index = 0; signature_index < network_blf_payload_long(map_signatures->count); signature_index++)
        {
            if (memcmp(map_signatures->signatures[signature_index], signature, sizeof(map_signatures->signatures[signature_index])) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
            network_banhammer_local_box_caught_cheating(FLAG(0));
        valid = found;
    }
    return valid;
}


// Checks the session parameter getters.
// @address 0x46da20 network_banhammer_find_cheaters_in_current_session
extern "C" unsigned short network_banhammer_find_cheaters_in_current_session(void)
{
    c_network_session *session = 0;
    unsigned short result = 0;
    if (network_life_cycle_in_session(&session) && session->established())
    {
        const c_network_session_parameters *parameters = session->get_session_parameters();
        if (parameters->m_parameters.campaign_insertion_point.get_allowed())
            result = *parameters->m_parameters.campaign_insertion_point.get();
    }
    return result;
}




// match: 0x401828: keeps network_banhammer_format_xuid out-of-line at the url_user call (bl kept; else it inlines, frame shifts)
#pragma auto_inline(off)
// @address 0x3c4a00 ?network_banhammer_format_xuid@@YAXPAUs_network_http_buffer_url@@_K@Z
void network_banhammer_format_xuid(s_network_http_buffer_url *url, unsigned __int64 value)
{
    s_banhammer_identifier_bytes xuid;
    xuid.value = value;
    unsigned char xuid_low = xuid.bytes[7];
    c_static_string<256> *text = (c_static_string<256> *)url->name;
    text->print("/storage/user/%1X/%1X/%02X/%016I64X/", xuid_low >> 4, xuid_low & 15, xuid.bytes[6], value);
    text->append("user.bin");
    url->service_type = 1;
    url->queue_type = 0;
}


// @address 0x4009a0 ??0s_network_banhammer_globals@@QAA@XZ
s_network_banhammer_globals::s_network_banhammer_globals() : s_network_banhammer_globals_base(this), flags(0)
{
}

// @address 0x400b18 ??0s_network_banhammer_controller_downloaders@@QAA@XZ
s_network_banhammer_controller_downloaders::s_network_banhammer_controller_downloaders()
{
}

// @address 0x3d3428 network_banhammer_update_user_download
extern "C" void network_banhammer_update_user_download(c_online_service_record_manager *download)
{
    if (download->xuid != 0)
    {
        if (download->state == _network_banhammer_download_idle || download->state == _network_banhammer_download_pending)
        {
            s_network_http_buffer_url url;
            url.name[0] = 0;
            url.service_type = 0;
            url.cache_mode = 0;
            url.lifetime_seconds = 600;
            url.queue_type = 0;
            const char *data = 0;
            long size = 0;
            network_banhammer_format_xuid(&url, download->xuid);
            http_buffer_downloader_set_url(&download->downloader, &url);
            long state = download->downloader.get_data(&data, &size);
            if (state == _network_banhammer_download_complete && data != 0 && download->state != _network_banhammer_download_complete)
            {
                download->buffer.data = data;
                download->buffer.size = size;
                download->timestamp = GetTickCount();
            }
            download->state = state;
        }
    }
}

template<typename t_blf_type>
static __forceinline long network_banhammer7_simple_get_data(c_http_blf_simple_downloader<t_blf_type> *downloader,
    const t_blf_type **out_data, unsigned long *out_size, long chunk_type, short major_version)
{
    long size;
    const char *data;
    long result;
    size = 0;
    data = 0;
    result = downloader->get_data(&data, &size);
    if (result == 2 && data != 0)
    {
        if (downloader->m_last_retry_count != downloader->get_retry_count())
            downloader->m_data = (const t_blf_type *)network_blf_find_chunk_in_file(
                data, size, chunk_type, major_version, _blf_file_authentication_none, (unsigned long *)&downloader->m_data_size);
    }
    else
    {
        downloader->m_data = 0;
        downloader->m_data_size = 0;
    }
    if (out_data)
        *out_data = downloader->m_data ? downloader->m_data : 0;
    if (out_size)
        *out_size = downloader->m_data ? downloader->m_data_size : 0;
    return result;
}

// Opaque address view of the time override; avoids changing the accepted
// canonical time symbol's cross-unit address-taken analysis.

static __forceinline unsigned long network_banhammer7_time_get(const s_network_time_override &globals)
{
    if (globals.enabled)
        return globals.time;
    return GetTickCount();
}

static __forceinline long network_banhammer2_network_time_since(unsigned long time)
{
    return (long)(network_banhammer7_time_get(NETWORK_BANHAMMER_TIME_GLOBALS) - time);
}

static __forceinline void network_banhammer2_csmemset_short(void *destination, int value, long count)
{
    for (long index = 0; index < count; index++)
        ((unsigned short *)destination)[index] = (unsigned short)value;
}

static __forceinline bool network_banhammer2_xuid_is_live(const unsigned __int64 *xuid)
{
    unsigned short type = *(const unsigned short *)xuid;
    bool result = (type & 0xF) == 9 && (type & 0xC0) != 0;
    return result;
}

typedef c_flags_no_init<e_network_banhammer_controller_flags, unsigned char, 6> s_controller_flags;

static __forceinline const gamepad_state *network_banhammer7_gamepad_get(const s_input_globals &input, short index)
{
    const gamepad_state *result = 0;
    if (input.gamepad_connected[index])
    {
        if (input.input_suppressed)
            result = &input.suppressed_gamepad;
        else
            result = &input.gamepads[index];
    }
    return result;
}
static __forceinline unsigned __int64 network_banhammer7_player_xuid_get(
    const c_controller_interface *controller, const network_session_interface_globals &slots)
{
    unsigned __int64 result = 0;
    short user_index = controller->get_user_index();
    if (user_index >= 0 && user_index < 4)
        result = slots.users[user_index].configuration.client.xuid;
    return result;
}
// @address 0x401828 ?network_banhammer_update_controllers@@YAXXZ
void network_banhammer_update_controllers()
{
    for (e_controller_index controller_index = first_controller(); controller_index != -1;
        controller_index = next_controller(controller_index))
    {
        char name[32];
        char identifier_copy[33];
        gamepad_state state;
        s_network_http_buffer_url url_messages;
        s_network_http_buffer_url url_recent;
        s_online_file_description description;
        s_network_http_buffer_url url_user;
        wchar_t text[128];
        s_network_banhammer_globals &globals = g_network_banhammer_globals;
        s_network_configuration *configuration = &g_network_configuration;
        const char *identifier_format = "%016I64X";
        const wchar_t *empty_description = L"";
        s_controller *controllers = g_controller_globals.controllers;
        network_session_interface_globals &slots = g_network_session_interface_globals;
        const s_network_time_override &time_globals = NETWORK_BANHAMMER_TIME_GLOBALS;
        c_online_file_manager *file_manager = &g_online_file_manager;
        const char *hash_string = "";
        const char *filename = "recent_players.bin";
        const char *storage = "/storage/user/%1X/%1X/%02X/%016I64X/";
        const char *message_format = "%s/matchmaking_banhammer_messages.bin";
        c_gui_error_manager *errors = &g_gui_error_manager;
        s_user_interface_controller *ui_controllers = g_user_interface_controller_globals.controllers;
        const s_input_globals &input = input_globals;


        s_controller *controller = &controllers[controller_index];
        c_controller_interface *controller_interface = (c_controller_interface *)controller;
        s_network_banhammer_controller_data *controller_data = &g_network_banhammer_globals.controllers[controller_index];
        s_controller_flags *controller_flags = (s_controller_flags *)&controller_data->flags;

        if (controller_interface->is_signed_in_to_live())
        {

        unsigned __int64 xuid = network_banhammer2_get_player_xuid(controller_interface);
        if (!controller_flags->test(_network_banhammer_controller_valid) || memcmp(&xuid, &controller_data->xuid, sizeof(xuid)) != 0)
        {
            network_banhammer_initialize_controller((e_controller_index)controller_index);
            ustrnzcpy_inline(controller_data->name, controller_interface->get_display_name(), 16);
            controller_data->xuid = xuid;
            controller_flags->set(_network_banhammer_controller_valid, true);
        }

        const gamepad_state *gamepad = network_banhammer2_input_get_gamepad_state((short)controller_index);
        if (gamepad != 0)
        {
            memcpy(&state, gamepad, sizeof(state));
            network_banhammer2_csmemset_short(&state.sticks, 0, 4);
            if (memcmp(&controller_data->activity_gamepad_state, &state, sizeof(state)) != 0)
                controller_data->last_activity_time = network_banhammer7_time_get(time_globals);

            gamepad_state session_state = {0};
            memcpy(session_state.sticks, gamepad->sticks, sizeof(session_state.sticks));
            if (memcmp(&controller_data->session_gamepad_state, &session_state, sizeof(session_state)) != 0)
                controller_data->session_start_time = network_banhammer7_time_get(time_globals);
        }

        if (controller_flags->test(_network_banhammer_controller_user_loaded))
        {
            if (((s_controller_flags *)&g_network_banhammer_globals.controllers[controller_index].flags)->test(_network_banhammer_controller_user_pending))
            {
                if (network_banhammer2_network_time_since(controller_data->user_file_request_time) >= configuration->user_file_request_timeout_msec * 1000)
                {
                    if (network_life_cycle_can_download_files())
                        controller_flags->set(_network_banhammer_controller_user_loaded, false);
                }
            }
            else
            {
                if (network_banhammer2_network_time_since(controller_data->user_file_refresh_time) >= configuration->user_file_timeout_msec * 1000)
                {
                    if (network_life_cycle_can_download_files())
                    {
                        g_network_banhammer_globals.controller_downloaders[controller_index].user_file.reset();
                        controller_data->user_file_request_time = network_banhammer7_time_get(time_globals);
                        controller_flags->set(_network_banhammer_controller_user_pending, true);
                    }
                }
            }
        }

        if (!controller_flags->test(_network_banhammer_controller_user_loaded) ||
            ((s_controller_flags *)&g_network_banhammer_globals.controllers[controller_index].flags)
                ->test(_network_banhammer_controller_user_pending))
        {
            s_network_banhammer_controller_downloaders *downloaders = &g_network_banhammer_globals.controller_downloaders[controller_index];
            const char *data = 0;
            long size = 0;
            long status;
            if (!network_banhammer2_xuid_is_live(&controller_data->xuid))
            {
                url_user.name[0] = 0;
                url_user.service_type = 0;
                url_user.cache_mode = 0;
                url_user.lifetime_seconds = 600;
                url_user.queue_type = 0;
                downloaders->user_file.set_fixed_retries_enabled(true);
                network_banhammer_format_xuid(&url_user, controller_data->xuid);
                http_buffer_downloader_set_url(&downloaders->user_file, &url_user);
                status = downloaders->user_file.get_data(&data, &size);
            }
            else
            {
                status = _download_status_complete;
            }

            if (status == _download_status_complete || status == _download_status_downloading)
            {
                unsigned long ban_flags = 0;
                unsigned long ban_types = 0;
                long highest_skill = 1;
                const char *chunk_data = data;
                long chunk_size = size;
                if (status != _download_status_complete && status == _download_status_downloading)
                {
                    s_banhammer_ban_chunk *ban_chunk = (s_banhammer_ban_chunk *)network_blf_find_chunk_in_file(
                        chunk_data, chunk_size, 0x66756268, 1, _blf_file_authentication_none, 0);
                    if (ban_chunk != 0)
                    {
                        if (ban_chunk->count >= 0 && (unsigned long)ban_chunk->count <= NUMBEROF(ban_chunk->entries))
                        {
                            __int64 now = 0;
                            _time64(&now);
                            for (long index = 0; index < ban_chunk->count; index++)
                            {
                                s_banhammer_ban_entry *entry = &ban_chunk->entries[index];
                                unsigned long category = entry->ban_category;
                                unsigned __int64 expiry_time = entry->expiry_time;
                                if (entry->ban_type <= 9 && entry->ban_category <= 31 && (unsigned __int64)now >= entry->start_time)
                                {
                                    if (expiry_time == 0 || (unsigned __int64)now <= expiry_time)
                                    {
                                        ban_flags |= 1 << entry->ban_type;
                                        ban_types |= 1 << category;
                                    }
                                }
                            }
                        }
                    }

                    s_banhammer_ban_upload *ban_upload = (s_banhammer_ban_upload *)network_blf_find_chunk_in_file(
                        chunk_data, chunk_size, 0x66757064, 3, _blf_file_authentication_none, 0);
                    if (ban_upload != 0)
                    {
                        name[0] = 0;
                        highest_skill = PIN(ban_upload->value, 1, 50);
                        strncpy(name, ban_upload->text, 32);
                        name[31] = 0;
                        short user_index = controller->user_index;
                        if (user_index != -1)
                        {
                            char *slot_name = g_network_session_interface_globals.users[user_index].text;
                            memset(slot_name, 0, sizeof(g_network_session_interface_globals.users[user_index].text));
                            strncpy(slot_name, name, 64);
                        }
                        s_user_interface_controller *ui_controller = &ui_controllers[controller_index];
                        unsigned char new_type = ban_upload->player_type;
                        bool changed_type = false;
                        if (new_type != ui_controller->hopper_access_flags)
                            changed_type = true;
                        ui_controller->hopper_access_flags = new_type;
                        if (changed_type)
                            user_interface_controller_update_network_properties((e_controller_index)controller_index);
                        unsigned long new_flags = ban_upload->player_flags;
                        bool changed_flags = false;
                        if (new_flags != ui_controller->flags)
                            changed_flags = true;
                        ui_controller->flags = new_flags;
                        if (changed_flags)
                            user_interface_controller_update_network_properties((e_controller_index)controller_index);
                    }

                    s_banhammer_file_chunk *file_chunk = (s_banhammer_file_chunk *)network_blf_find_chunk_in_file(
                        chunk_data, chunk_size, 0x66696c71, 1, _blf_file_authentication_none, 0);
                    if (file_chunk != 0)
                    {
                        for (long index = 0; index < NUMBEROF(file_chunk->entries); index++)
                        {
                            s_banhammer_file_entry *entry = &file_chunk->entries[index];
                            if (entry->xuid != 0)
                            {
                                c_static_string<33> identifier;
                                identifier.print(identifier_format, entry->xuid);
                                strncpy(identifier_copy, identifier.get_string(), 33);
                                identifier_copy[32] = 0;
                                memset(&description, 0, sizeof(description));
                                description.named = false;
                                description.owner_xuid = controller_data->xuid;
                                saved_game_files_initialize_metadata(&description.metadata, -1, entry->file_type, entry->name,
                                    empty_description, entry->xuid, entry->size_in_bytes);
                                description.metadata.campaign_id = entry->campaign_id;
                                description.metadata.map_id = entry->map_id;
                                description.metadata.game_engine_type = entry->game_engine_type;
                                file_manager->enqueue_for_transfer(_queued_work_item_type_download, &description, hash_string, identifier_copy,
                                    controller_data->xuid, entry->requester_xuid, entry->slot, true);
                            }
                        }
                    }

                    if (ban_flags != 0 && ban_flags != controller_data->ban_flags)
                        controller_flags->set(_network_banhammer_controller_message_displayed, false);
                }

                controller_data->ban_flags = ban_flags;
                controller_data->ban_types = ban_types;
                controller_data->highest_skill = highest_skill;
                controller_data->user_file_refresh_time = network_banhammer7_time_get(time_globals);
                controller_flags->set(_network_banhammer_controller_user_pending, false);
                controller_flags->set(_network_banhammer_controller_user_loaded, true);
                user_interface_controller_update_network_properties((e_controller_index)controller_index);
            }
            else if (status != _download_status_failed)
            {
                g_network_banhammer_globals.flags |= FLAG(0);
            }
        }

        if (!controller_flags->test(_network_banhammer_controller_messages_loaded))
        {
            if (!network_banhammer2_xuid_is_live(&controller_data->xuid))
            {
                url_recent.name[0] = 0;
                url_recent.service_type = 0;
                url_recent.cache_mode = 0;
                url_recent.lifetime_seconds = 600;
                url_recent.queue_type = 0;
                g_network_banhammer_globals.controller_downloaders[controller_index].recent_players.set_fixed_retries_enabled(true);
                unsigned __int64 xuid = controller_data->xuid;
                ((c_static_string<256> *)url_recent.name)->print(storage,
                             *((const unsigned char *)&xuid + 7) >> 4,
                             *((const unsigned char *)&xuid + 7) & 15,
                             *((const unsigned char *)&xuid + 6),
                             xuid);
                ((c_static_string<256> *)url_recent.name)->append_print(filename);
                url_recent.service_type = 1;
                url_recent.queue_type = 0;
                http_buffer_downloader_set_url(
                    g_network_banhammer_globals.controller_downloaders[controller_index].recent_players.buffer_downloader(), &url_recent);
                const s_files_user_recent_players *recent_data;
                long status = network_banhammer7_simple_get_data(
                    &g_network_banhammer_globals.controller_downloaders[controller_index].recent_players, &recent_data, 0, 0x66757270, 2);

                if (status == _download_status_complete || status == _download_status_downloading)
                {
                    if (status == _download_status_downloading)
                    {
                        s_network_banhammer_repeated_play_list *messages = (s_network_banhammer_repeated_play_list *)recent_data;
                        if (messages != 0)
                        {
                        if (messages->entry_count >= 0 && (unsigned long)messages->entry_count <= NUMBEROF(messages->entries))
                        {
                            for (long index = 0; index < messages->entry_count; index++)
                            {
                                controller_data->repeated_play_list.entries[index].value = messages->entries[index].value;
                                memcpy(&controller_data->repeated_play_list.entries[index].history, &messages->entries[index].history,
                                    sizeof(messages->entries[index].history));
                            }
                            controller_data->repeated_play_list.entry_count = messages->entry_count;
                        }
                        else
                        {
                            controller_data->repeated_play_list.entry_count = 0;
                        }
                        }
                    }
                    ((s_controller_flags *)&g_network_banhammer_globals.controllers[controller_index].flags)
                        ->set(_network_banhammer_controller_messages_loaded, true);
                }
                else if (status != _download_status_failed)
                {
                    g_network_banhammer_globals.flags |= FLAG(1);
                }
            }
            else
            {
                ((s_controller_flags *)&g_network_banhammer_globals.controllers[controller_index].flags)
                    ->set(_network_banhammer_controller_messages_loaded, true);
            }
        }

        if (((s_controller_flags *)&g_network_banhammer_globals.controllers[controller_index].flags)
                ->test(_network_banhammer_controller_user_loaded) &&
            network_banhammer_get_controller_ban_flags(controller_index) != 0 &&
            !controller_flags->test(_network_banhammer_controller_message_displayed))
        {
            url_messages.name[0] = 0;
            url_messages.service_type = 0;
            url_messages.cache_mode = 0;
            url_messages.lifetime_seconds = 600;
            url_messages.queue_type = 0;
            g_network_banhammer_globals.messages_downloader.set_fixed_retries_enabled(true);
            online_url_set_base_path(&url_messages);
            e_language language = get_current_language();
            ((c_static_string<256> *)url_messages.name)->append_print(message_format, get_language_suffix((e_language)language, false));
            url_messages.cache_mode = 1;
            url_messages.queue_type = 0;
            http_buffer_downloader_set_url(g_network_banhammer_globals.messages_downloader.buffer_downloader(), &url_messages);
            const s_files_user_banhammer_messages *message_data;
            long status = network_banhammer7_simple_get_data(&g_network_banhammer_globals.messages_downloader, &message_data, 0, 0x62686d73, 1);
            s_banhammer_message_chunk *messages = (s_banhammer_message_chunk *)message_data;
            if (messages == 0)
            {
                if (status != _download_status_complete)
                {
                    if (status == _download_status_downloading)
                        controller_flags->set(_network_banhammer_controller_message_displayed, true);
                }
                else
                    controller_flags->set(_network_banhammer_controller_message_displayed, true);
            }
            else
            {
                if (network_blf_payload_long(messages->count) >= 0 &&
                    (unsigned long)network_blf_payload_long(messages->count) <= 32)
                {
                    unsigned long ban_types = 0;
                    if (controller_flags->test(_network_banhammer_controller_valid))
                        ban_types = controller_data->ban_types | g_network_banhammer_globals.ban_types;
                    for (long index = 0; index < network_blf_payload_long(messages->count); index++)
                    {
                        if ((1 << index) & ban_types)
                        {
                            ::utf8_string_to_wchar_string(
                                (utf8 const *)messages->messages[index].text, (wchar_t *)((unsigned short *)text), 128, (long *)(0));
                            errors->post_error_with_custom_message(0x20004, 0, text, (e_controller_index)controller_index, false, true);
                        }
                    }
                }
                controller_flags->set(_network_banhammer_controller_message_displayed, true);
            }
        }
        }
        else if (controller_flags->test(_network_banhammer_controller_valid))
        {
            network_banhammer_initialize_controller((e_controller_index)controller_index);
        }
    }
}

static __forceinline void network_banhammer8_make_machine_url(s_network_http_buffer_url *url, unsigned __int64 value)
{
 s_banhammer_identifier_bytes machine;
 machine.value=value;
 unsigned char low=machine.bytes[7];
 ((c_static_string<256> *)url->name)->print("/storage/machine/%1X/%1X/%02X/%016I64X/",low>>4,low&15,machine.bytes[6],value);
 ((c_static_string<256> *)url->name)->append("machine.bin");
 url->queue_type=0;
}
static __forceinline long network_banhammer8_time_since(unsigned long time)
{
 return (long)(network_time_get()-time);
}
// @address 0x4012c0 ?network_banhammer_update_machine@@YAXXZ
void network_banhammer_update_machine(void)
{
 s_network_banhammer_globals *globals=&g_network_banhammer_globals;
 s_network_quality_statistics copy;
 s_network_configuration *configuration=&g_network_configuration;
 if (globals->flags & 4) {
  if (globals->flags & 8) {
   if (network_banhammer8_time_since(globals->machine_file_time) >= configuration->machine_file_refresh_seconds * 1000 &&
       network_life_cycle_can_download_files())
       globals->flags &= ~4;
  } else if (network_banhammer8_time_since(globals->machine_file_index) >= configuration->machine_file_timeout_seconds*1000 &&
             network_life_cycle_can_download_files()) {
   globals->machine_file_downloader.reset(); globals->machine_file_time=network_time_get(); globals->flags |= 8;
  }
 }
 if (!(globals->flags & 4) || (globals->flags & 8)) {
  unsigned __int64 machine=transport_secure_address_get_local_machine_id();
  if (machine) {
   const char *data=0; long size=0;
   globals->machine_file_downloader.set_fixed_retries_enabled(true);
   network_banhammer8_url url;
   network_banhammer8_make_machine_url(&url,machine);
   http_buffer_downloader_set_url(&globals->machine_file_downloader,&url);
   long status=globals->machine_file_downloader.get_data(&data,&size);
   if (status==4 || status==2) {
    unsigned long ban_flags=0,ban_types=0;
    if (status!=4 && status==2) {
     s_banhammer_ban_chunk *chunk=(s_banhammer_ban_chunk *)network_blf_find_chunk_in_file(data,size,0x66756268,1,_blf_file_authentication_none,0);
     if (chunk && chunk->count>=0 && (unsigned long)chunk->count<=32) {
      __int64 now=0; _time64(&now);
      for (long i=0;i<chunk->count;++i) {

       unsigned long ban_type=chunk->entries[i].ban_type, message_index=chunk->entries[i].ban_category;
       unsigned __int64 expiry=chunk->entries[i].expiry_time;
       if (ban_type <= 9 && message_index <= 31 && (unsigned __int64)now >= chunk->entries[i].start_time &&
           (!expiry || (unsigned __int64)now <= expiry))
       {
           ban_flags |= 1 << ban_type;
           ban_types |= 1 << message_index;
       }
      }
     }
     s_network_quality_statistics *stats=(s_network_quality_statistics *)network_blf_find_chunk_in_file(data,size,0x66756e73,1,_blf_file_authentication_none,0);
     if (stats) {
      c_network_observer *observer=0; network_banhammer_life_cycle_get_observer(&observer);
      if (!observer->m_quality_statistics_valid) {
       copy=*stats; s_bandwidth_history *history=&copy.bandwidth_history;
       if ((unsigned long)history->qos_count>8 || history->measurement_count<0 || history->measurement_count>8) memset(history,0,sizeof(*history));
       observer->m_quality_statistics_valid=true; observer->m_network_quality_statistics=copy; ::network_bandwidth_update_estimate();
      }
     }
     if (ban_flags && ban_flags != globals->ban_flags)
         for (e_controller_index i = _controller0; i != -1; i = next_controller(i))
             if (globals->controllers[i].flags & 1)
                 globals->controllers[i].flags &= ~0x20;
    }
    globals->ban_flags=ban_flags; globals->ban_types=ban_types;
    for (e_controller_index i = _controller0; i != -1; i = next_controller(i))
        user_interface_controller_update_network_properties((e_controller_index)i);
    globals->machine_file_index=network_time_get(); globals->flags=(globals->flags & 0xf3)|4;
   } else if (status!=1) globals->flags |= 1;
  }
 }
 if (globals->flags & 0x10) {
  c_http_post_source source;
  unsigned __int64 machine=transport_secure_address_get_local_machine_id();
  if (machine) {
   char headers[1024]; headers[0]=0;
   network_banhammer8_url url;
   ((c_static_string<256> *)url.name)->print("/gameapi/MachineUpdateNetworkStats.ashx");
   url.cache_mode=0; url.service_type=2; url.queue_type=1;
   http_buffer_downloader_set_url(&globals->network_statistics_upload_downloader,&url);
   source.set_source_as_buffer((const char *)&globals->network_statistics, sizeof(globals->network_statistics));
   source.set_content_type("application/x-halo3networkstats");
   c_http_buffer_downloader *downloader=&globals->network_statistics_upload_downloader;
   downloader->m_post_source=source; downloader->m_has_extra_headers=true;
   c_static_string<1024> *header_string=(c_static_string<1024> *)headers;
   header_string->append_print("machineid: \"%016I64x\"\r\n",machine);
   http_buffer_downloader_set_extra_headers(&globals->network_statistics_upload_downloader,headers);
   globals->network_statistics_upload_downloader.get_data();
   long status=network_http_buffer_downloader_get_public_status(downloader->m_internal_status);
   if (status==2 || status==3 || status==4) globals->flags &= ~0x10;
  }
 }
}

// @address 0xffc k_banhammer_map_manifest_path
const char k_banhammer_map_manifest_path[] = "rsa_manifest.bin";

static __forceinline void network_banhammer_apply_map_manifest(const s_files_machine_banhammer_messages *manifest, long retry_count)
{
    if ((unsigned long)network_blf_payload_long(manifest->count) <= 128)
    {
        g_network_banhammer_globals.map_signatures.valid = true;
        g_network_banhammer_globals.map_signatures.disabled = false;
        // The copied count stays in BLF byte order, so signature lookup decodes it when read.
        memcpy(&g_network_banhammer_globals.map_signatures.count, manifest, sizeof(*manifest));
    }
    else
    {
        g_network_banhammer_globals.map_signatures.valid = false;
        g_network_banhammer_globals.map_signatures.disabled = true;
    }
    g_network_banhammer_globals.map_signatures.last_retry_count = retry_count;
}

static __forceinline long network_banhammer_get_map_manifest(
    c_http_blf_simple_downloader<s_files_machine_banhammer_messages> *downloader,
    const s_files_machine_banhammer_messages **out_data)
{
    long size = 0;
    const char *data = 0;
    long result = downloader->get_data(&data, &size);
    if (result == 2 && data != 0)
    {
        if (downloader->m_last_retry_count != downloader->get_retry_count())
            downloader->m_data = (const s_files_machine_banhammer_messages *)network_blf_find_chunk_in_file(data, size,
                0x6d61706d, 1, _blf_file_authentication_none, (unsigned long *)&downloader->m_data_size);
    }
    else
    {
        downloader->m_data = 0;
        downloader->m_data_size = 0;
    }
    // match: 0x401108: null select keeps m_data store (stwx) after the m_data_size build; manifest null test (cmplwi r4,0) stays above the status bne
    *out_data = downloader->m_data == 0 ? 0 : downloader->m_data;
    return result;
}

// @address 0x401108 network_banhammer_update_map_manifest
extern "C" void network_banhammer_update_map_manifest()
{
    network_banhammer8_url url;
    online_url_set_base_path(&url);
    ((c_static_string<256> *)url.name)->append_print(k_banhammer_map_manifest_path);
    url.queue_type = 0;
    http_buffer_downloader_set_url(g_network_banhammer_globals.machine_messages_downloader.buffer_downloader(), &url);
    g_network_banhammer_globals.machine_messages_downloader.buffer_downloader()->get_data();
    long retry_count = g_network_banhammer_globals.machine_messages_downloader.get_retry_count();
    if (g_network_banhammer_globals.map_signatures.last_retry_count == retry_count)
        return;
    if (!network_life_cycle_can_download_files())
        return;
    {
        const s_files_machine_banhammer_messages *manifest;
        long status = network_banhammer_get_map_manifest(&g_network_banhammer_globals.machine_messages_downloader, &manifest);
        if (status == 2 && manifest)
        {
            network_banhammer_apply_map_manifest(manifest, retry_count);
        }
        else if (status != 1)
        {
            g_network_banhammer_globals.map_signatures.disabled = true;
            g_network_banhammer_globals.map_signatures.last_retry_count = retry_count;
        }
    }
}
