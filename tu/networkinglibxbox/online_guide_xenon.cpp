// networkinglibxbox/online_guide_xenon.obj: from the records 0x95fc8 0x96160 0x96468 0x281d38 0x39da40 0x39da50 0x39da60 0x39da70 0x39dc80 0x39dce8 0x39dda0 0x39ddd8 0x39de70 0x39dec0 0x39df30 0x39e008 0x39e078 0x39e0e8 0x39e2a8 0x39e3d8 0x39e5d8 0x39e7b8 0x39e9c8 0x39eac0 0x39ebc8 0x39ed28 0x39edb8 0x39ee48 0x39eed8 0x39eee8 0x39f010 0x39f1c8 0x411088 0x46b168 0x4cfeb0
// @flags /TP /O2
#include <xtl.h>
#include "shell/shell.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "game/player_types.h"
#include "interface/c_deletable.h"
// @address 0x39dce8 string_verify_task_set_strings
// @address 0x39dda0 ?reset@c_string_verify_task@@UAAXXZ
// @address 0x39de70 ?success@c_string_verify_task@@UAAXK@Z
// @address 0x39dec0 ?start@c_invite_task@@UAAKPAX@Z
// @address 0x39e008 ?online_guide_show_gamer_card_ui@@YAKW4e_controller_index@@_K@Z
// @address 0x39e078 ?online_guide_show_friends_ui@@YAKW4e_controller_index@@@Z
// @address 0x39eac0 ??0s_network_squad_status_data@@QAA@XZ
// @address 0x39ed28 ?set_default_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39edb8 ?set_title_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39ee48 ?set_description_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
// @address 0x4834a0 ??0c_load_in_progress_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@JJJJ@Z
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/game_details/load_game_details_screen_message.h"
#include "tag_files/string_ids.h"
#include "networking/transport/transport_qos_types.h"

#if defined(_XBOX) && !defined(__clang__)
#pragma pack(push, 1)
#endif
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "networking/network_game_definitions.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/transport/transport_qos.h"
#include "camera/camera.h"
#include "networking/online/online_session_search_xenon.h"
#include "networking/online/online_session_search_types.h"
#include "networking/transport/transport_security_types.h"
#include "networking/network_utilities.h"
#include "networking/tools/network_blf.h"
#include "interface/user_interface_shared_globals.h"
#include "cseries/async_xoverlapped_types.h"
#include "networking/transport/transport_qos_target_types.h"
struct s_network_squad_status_data;
#if defined(_XBOX) && !defined(__clang__)
#pragma pack(pop)
#endif
#include "networking/logic/storage/network_http_buffer_downloader.h"

#include "interface/controller_inline.h"

#include "networking/online/online_invite_task.h"
#include "networking/online/online_guide_xenon_services.h"

#include "networking/online/online_guide_state_types.h"
#include "interface/virtual_keyboard_task.h"

struct s_online_guide_dynamic_action
{
    unsigned short id;
    wchar_t text[23];
    unsigned long flags;
};

#include "interface/c_player_profile.h"
#include "networking/online/online_session_search_tasks.h"
#include "interface/user_interface_error_manager.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "game/game_engine_string_types.h"
#include <new>
#include "cseries/async_xoverlapped_allocation.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "networking/logic/network_search_types.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include "networking/online/online_service_record.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/tools/blf_authentication_types.h"
#include "networking/online/online_session_results.h"
#include "networking/online/online_session_results_types.h"
#include "interface/user_interface_content_globals.h"

struct s_custom_game_action_press
{
    long header[12];
    long identity[26];
};

struct s_online_guide_service_record_context : c_gui_screen_widget
{
    s_player_identifier target_player_identifier;
    s_player_identifier identifier;
    s_player_configuration player;
    bool update_pending;
};


STATIC_ASSERT_ABI32(offsetof(s_online_guide_service_record_context, identifier) == 0x1a90, online_guide_context_identifier_offset);

// @address 0x39df30 ??0s_online_guide_state@@QAA@XZ
s_online_guide_state::s_online_guide_state()
    : s_online_guide_state_base(this)
{
}

s_online_guide_state g_online_guide_globals;

static __forceinline bool online_guide_xenon_get_friend(const c_online_find_friend_task *task, XONLINE_FRIEND *friend_info, unsigned __int64 xuid)
{
    bool found = false;
    memset(friend_info, 0, sizeof(*friend_info));
    for (unsigned long friend_index = 0; friend_index < (unsigned long)task->get_friend_count(); ++friend_index)
    {
        if (task->get_friend(friend_index)->xuid == xuid)
        {
            memcpy(friend_info, task->get_friend(friend_index), sizeof(*friend_info));
            found = true;
            break;
        }
        else
            continue;
    }
    return found;
}

static __forceinline bool online_guide_xenon_friend_is_online(const XONLINE_FRIEND *friend_info)
{
    return (friend_info->dwFriendState & 1) != 0;
}

static __forceinline bool online_guide_xenon_friend_is_playing_our_game(const XONLINE_FRIEND *friend_info)
{
    return friend_info->dwTitleID == 0x4d5307e6;
}

static __forceinline bool online_guide_xenon_friend_request_sent(const XONLINE_FRIEND *friend_info)
{
    return (bool)(_rotl(friend_info->dwFriendState, 2) & 1);
}

static __forceinline bool online_guide_xenon_friend_request_received(const XONLINE_FRIEND *friend_info)
{
    return (unsigned char)(friend_info->dwFriendState >> 31);
}

static __forceinline unsigned __int64 *online_guide_xenon_friend_get_sessionid(const XONLINE_FRIEND *friend_info, unsigned __int64 *session_id)
{
    memcpy(session_id, (const unsigned long *)&friend_info->sessionID, sizeof(*session_id));
    return session_id;
}

// @address 0x95fc8 ?online_guide_update_gamercard_dynamic_actions@@YAXXZ
void online_guide_update_gamercard_dynamic_actions()
{
    if (g_online_guide_globals.find_friend_active) {
        long state = g_online_guide_globals.find_friend.get_task_state();
        if (state == 4) {
            bool joinable = false;
            XONLINE_FRIEND friend_info;
            bool found = online_guide_xenon_get_friend(&g_online_guide_globals.find_friend, &friend_info, g_online_guide_globals.xuid);
            g_online_guide_globals.find_friend_active = false;
            if (found && online_guide_xenon_friend_is_online(&friend_info) && online_guide_xenon_friend_is_playing_our_game(&friend_info) &&
                !online_guide_xenon_friend_request_sent(&friend_info) && !online_guide_xenon_friend_request_received(&friend_info)) {
                unsigned __int64 session;
                unsigned __int64 zero_session;
                online_guide_xenon_friend_get_sessionid(&friend_info, &session);
                memset(&zero_session, 0, sizeof(zero_session));
                if (memcmp(&session, &zero_session, sizeof(session))) {
                    memcpy(&g_online_guide_globals.session, &session, sizeof(session));
                    joinable = true;
                }
            }
            bool actions = joinable && online_guide_can_join();
            online_guide_set_dynamic_actions(true, actions);
        } else if (state == 5) {
            g_online_guide_globals.find_friend_active = false;
            online_guide_set_dynamic_actions(true, false);
        }
    }
}

// @address 0x281d38 ?set@?$c_static_wchar_string@$0IA@@@QAAXPB_W@Z
template<> void c_static_wchar_string<128>::set(const wchar_t *string)
{
    ustrnzcpy_inline(m_string, string, 128);
}

const char *c_string_verify_task::get_context_string() const
{
    return "XStringVerify";
}

// @address 0x44130 ??_7c_invite_task@@6B@
// @address 0x39eb28 ??_Ec_invite_task@@UAAPAXI@Z
// @address 0x39eb28 ??_Gc_invite_task@@UAAPAXI@Z
c_invite_task::c_invite_task() :
    c_overlapped_task(__FILE__, __LINE__), m_controller_index(_controller_index_none), m_target_xuid(0)
{
}

const char *c_invite_task::get_context_string() const
{
    return "XInviteSend";
}

const char *c_show_custom_message_compose_task::get_context_string() const
{
    return "XShowCustomMessageComposeUI";
}

// @rdata 0x82044088..0x820440d8
static const s_compose_mapping k_message_compose_mappings[10] =
{
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 3, 4 },
    { 4, 5 },
    { 5, 5 },
    { 6, 6 },
    { 7, 7 },
    { 8, 8 },
    { 10, 9 },
};
// @rdata-end

// @address 0x39da70 ?start@c_show_custom_message_compose_task@@UAAKPAX@Z
unsigned long c_show_custom_message_compose_task::start(void *platform_handle)
{
    long option_index = 0;
    memset(buttons, 0, sizeof(buttons));
    button_count = 0;
    buttons[button_count].type = 3;
    for (long string_index = 0; string_index < 12; string_index++)
    {
        for (unsigned long mapping_index = 0; mapping_index < 10; mapping_index++)
        {
            const s_compose_mapping *mapping = &k_message_compose_mappings[mapping_index];
            if (mapping->string_index == string_index)
            {
                if (string_index == 0)
                    ustrnzcpy_inline(buttons[button_count].text, m_texts.strings[0].text, 23);
                buttons[button_count].options[option_index].type = mapping->option_type;
                ustrnzcpy_inline(buttons[button_count].options[option_index].text, m_texts.strings[string_index].text, 23);
                option_index++;
            }
        }
    }
    buttons[button_count].option_count = 1;
    button_count++;
    custom_data.xuid = source.xuid;
    custom_data.value = source.value;
    strncpy(custom_data.name, source.name, 33);
    custom_data.payload = source.payload;
    unsigned long result;
    if (controller_get(static_cast<e_controller_index>(controller_index))->is_signed_in_to_live())
        result = XShowCustomMessageComposeUI(controller_index, 0, 0, 0, m_texts.title, m_texts.text, m_texts.description, 0, 0,
        (const XMSG_CUSTOMACTION *)buttons, button_count, (const BYTE *)&custom_data, 144, 0, (XOVERLAPPED *)platform_handle);
    else
        result = 0x80004005;
    return result;
}

c_string_verify_task::c_string_verify_task(const char *file, long line)
    : c_overlapped_task(file, line)
{
    initialize_strings();
}

__forceinline c_string_verify_task::c_string_verify_task()
    : c_overlapped_task(0, 0)
{
    initialize_strings();
}

__forceinline void c_string_verify_task::initialize_strings()
{
    m_language = 0;
    for (long string_index = 0; string_index < 4; string_index++)
    {
        memset(m_strings[string_index], 0, sizeof(m_strings[string_index]));
        m_string_verified[string_index] = false;
    }
}


// @address 0x39dce8 string_verify_task_set_strings
void string_verify_task_set_strings(c_string_verify_task *task, long language, const wchar_t **strings, long count)
{
    task->m_language = language;
    for (long string_index = 0; string_index < count; string_index++)
    {
        ustrnzcpy_inline(task->m_strings[string_index], strings[string_index], 256);
        task->m_string_verified[string_index] = false;
    }
    task->m_result_count = count;
}

// @address 0x39dda0 ?reset@c_string_verify_task@@UAAXXZ
void c_string_verify_task::reset()
{
    c_overlapped_task::reset();
    m_result_count = 0;
}


// @address 0x39ddd8 ?start@c_string_verify_task@@UAAKPAX@Z
unsigned long c_string_verify_task::start(void *overlapped)
{
    for (long string_index = 0; string_index < m_result_count; string_index++)
    {
        m_string_data[string_index].wStringSize = (unsigned short)wcslen(m_strings[string_index]);
        m_string_data[string_index].pszString = m_strings[string_index];
        m_string_verified[string_index] = false;
    }
    unsigned long result = XStringVerify(0, get_language_iso_639_1_name((e_language)m_language), m_result_count, m_string_data,
        sizeof(m_verify_results), (STRING_VERIFY_RESPONSE *)m_verify_results, (XOVERLAPPED *)overlapped);
    return result;
}

void c_string_verify_task::success(unsigned long return_result)
{
    STRING_VERIFY_RESPONSE *response = (STRING_VERIFY_RESPONSE *)m_verify_results;
    for (long string_index = 0; string_index < m_result_count; string_index++)
        m_string_verified[string_index] = response->pStringResult[string_index] >= 0;
}

unsigned long c_invite_task::start(void *overlapped)
{
    unsigned long result = 0x80004005;
    if (controller_get(static_cast<e_controller_index>(m_controller_index))->is_signed_in_to_live())
        result = XInviteSend(m_controller_index, 1, &m_target_xuid, m_message_text, (XOVERLAPPED *)overlapped);
    return result;
}

unsigned long online_guide_show_gamer_card_ui(e_controller_index controller_index, unsigned __int64 xuid)
{
    unsigned long result;
    if (controller_get(static_cast<e_controller_index>(controller_index))->is_signed_in_to_live())
        result = XShowGamerCardUI(controller_index, xuid);
    else
        result = 0x80004005;
    return result;
}

unsigned long online_guide_show_friends_ui(e_controller_index controller_index)
{
    if (controller_get(static_cast<e_controller_index>(controller_index))->is_signed_in_to_live())
        return XShowFriendsUI(controller_index);
    return 0x80004005;
}

// @address 0x39e0e8 ?online_guide_show_virtual_keyboard_ui@@YAKW4e_controller_index@@KPB_W11PA_WKPAU_XOVERLAPPED@@@Z
unsigned long online_guide_show_virtual_keyboard_ui(e_controller_index controller_index, unsigned long character_flags,
    const wchar_t *default_text, const wchar_t *title_text, const wchar_t *description_text, wchar_t *result_text,
    unsigned long maximum_character_count, _XOVERLAPPED *platform_handle)
{
    unsigned long flags = 0;
    if (character_flags & 2) flags = 1;
    else if (character_flags & 4) flags = 2;
    else if (character_flags & 8) flags = 4;
    else if (character_flags & 16) flags = 8;
    else if (character_flags & 32) flags = 16;
    else if (character_flags & 64) flags = 32;
    else if (character_flags & 128) flags = 64;
    else if (character_flags & 256) flags = 128;
    else if (character_flags & 512) flags = 256;
    else if (character_flags & 1024) flags = 4096;
    else if (character_flags & 2048) flags = 8192;
    else if (character_flags & 4096) flags = 16384;
    else {
        bool supported = false;
        unsigned long language = XTLGetLanguage();
        if (language >= 1 && language <= 10) supported = true;
        flags = !supported;
    }
    if (character_flags & 8192) flags |= 0x10000000;
    if (character_flags & 16384) flags |= 0x20000000;
    unsigned long result;
    if (controller_get(static_cast<e_controller_index>(controller_index))->is_signed_in_to_machine())
        result = XShowKeyboardUI(controller_index, flags, default_text, title_text, description_text, result_text, maximum_character_count, platform_handle);
    else
        result = 0x80004005;
    return result;
}

static __forceinline bool online_session_search_by_id_initiate(const s_transport_secure_identifier *session_id, e_controller_index controller_index)
{
    bool result = false;
    if (online_session_search_by_id_get_status() != _online_session_search_status_pending)
    {
        online_session_search_globals.search_by_id_task->initialize(session_id, controller_index);
        result = overlapped_task_start_internal(
            online_session_search_globals.search_by_id_task, "..\\networking\\online\\online_session_search_xenon.cpp", 261);
    }
    return result;
}
struct c_static_wchar_string_1024;

static __forceinline bool online_guide_xenon_alpha_locked_down()
{
    bool result;
    if (!g_user_interface_is_alpha || (g_network_configuration.alpha_locked_down != 0 && g_eula_accepted))
        result = false;
    else
        result = true;
    return result;
}

static __forceinline bool online_guide_xenon_global_string_get(long string_id, c_static_wchar_string_1024 *text)
{
    bool found = false;
    text->string[0] = 0;
    const s_user_interface_shared_globals *table = ::user_interface_shared_tag_globals_try_and_get();
    if (table && table->global_strings.index != -1)
        found = reinterpret_cast<bool (*)(long, long, c_static_wchar_string_1024 *)>(text_group_get_string)(table->global_strings.index, string_id, text);
    return found;
}

// @address 0x39e2a8 ?online_guide_handle_custom_gamercard@@YAX_N@Z
void online_guide_handle_custom_gamercard(bool closing)
{
    unsigned long controller = 0;
    unsigned __int64 xuid = 0;
    if (g_online_guide_globals.find_friend_active)
    {
        overlapped_task_block_until_finished(&g_online_guide_globals.find_friend);
        g_online_guide_globals.find_friend_active = false;
    }
    g_online_guide_globals.actions = XCustomGetCurrentGamercard(&controller, &xuid) != 0;
    if (g_online_guide_globals.actions)
    {
        g_online_guide_globals.controller_index = (e_controller_index)controller;
        g_online_guide_globals.xuid = xuid;
        g_online_guide_globals.find_friend = c_online_find_friend_task("..\\networking\\online\\online_guide_xenon.cpp",
            658, g_online_guide_globals.controller_index, xuid);
        overlapped_task_start_internal(&g_online_guide_globals.find_friend, "..\\networking\\online\\online_guide_xenon.cpp", 661);
        g_online_guide_globals.find_friend_active = true;
    }
}

static __forceinline void online_guide_xenon_show_loading_dialog(long title, long message)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
    if (memory)
    {
        c_load_in_progress_screen_message *screen = new (memory)
            c_load_in_progress_screen_message(0x100b3, k_any_controller, _console_window, title, message, 0, 1);
        if (screen)
            ::user_interface_messaging_post((c_message const *)(c_deletable *)screen);
    }
}

// @address 0x39e3d8 online_guide_handle_dynamic_action
extern "C" void online_guide_handle_dynamic_action(void)
{
    unsigned long controller;
    long type;
    long action_xuid[4];
    s_custom_game_action_press press;
    unsigned short size = 144;
    if (XCustomGetLastActionPressEx(&controller, (DWORD *)&type, (XUID *)action_xuid, (BYTE *)&press, &size) != 0)
        return;
    switch (type)
    {
    case 0:
        online_guide_xenon_show_loading_dialog(0x105ab, 0x105ac);
        g_online_guide_globals.service_record_mode = 1;
        camera_input_inhibited();
        return;
    case 1:
        online_guide_xenon_show_loading_dialog(0x105ab, 0x105ac);
        g_online_guide_globals.service_record_mode = 2;
        camera_input_inhibited();
        return;
    case 2:
        if (!online_guide_can_join())
            return;
        online_guide_xenon_show_loading_dialog(0x105af, 0x105b0);
        online_guide_show_player_details_for_player((e_controller_index)controller, (const s_transport_secure_identifier *)&g_online_guide_globals.session);
        return;
    case 3:
        c_start_menu_screen_widget::load_start_menu_for_file_share((e_controller_index)controller, 0, (const unsigned __int64 *)&press,
        (const s_service_record_identity *)press.identity, press.header[2], true);
        return;
    }
}

// @address 0x39e5d8 ?online_guide_show_file_share_recommendation@@YAXW4e_controller_index@@_KJPBDPBUs_service_record_identity@@PBUs_compose_text@@@Z
void online_guide_show_file_share_recommendation(e_controller_index controller_index, unsigned __int64 xuid, long value, const char *name,
    const s_service_record_identity *identity, const s_compose_text *texts)
{
    if (!g_online_guide_globals.compose.busy())
    {
        g_online_guide_globals.compose = c_show_custom_message_compose_task("..\\networking\\online\\online_guide_xenon.cpp",
            781, controller_index, xuid, value, name,
        identity, texts);
        overlapped_task_start_internal(&g_online_guide_globals.compose, "..\\networking\\online\\online_guide_xenon.cpp", 781);
    }
}

// @address 0x39e7b8 ?online_guide_set_dynamic_actions@@YAX_N0@Z
void online_guide_set_dynamic_actions(bool show_details, bool allow_join)
{
    c_static_wchar_string_1024 text;
    text.string[0] = 0;
    long count = 0;
    s_online_guide_dynamic_action actions[4];
    if (!online_guide_xenon_alpha_locked_down())
    {
        bool found;
        found = online_guide_xenon_global_string_get(0x1028e, &text);
        if (found)
        {
            actions[count].id = 0;
            ustrnzcpy(actions[count].text, text.string, 23);
            actions[count].flags = 1;
            ++count;
        }
        found = online_guide_xenon_global_string_get(0x10202, &text);
        if (found)
        {
            actions[count].id = 1;
            ustrnzcpy(actions[count].text, text.string, 23);
            actions[count].flags = 1;
            ++count;
        }
    }
    if (!online_guide_xenon_alpha_locked_down() && allow_join)
    {
        bool found;
        found = online_guide_xenon_global_string_get(0x1028f, &text);
        if (found)
        {
            actions[count].id = 2;
            ustrnzcpy(actions[count].text, text.string, 23);
            actions[count].flags = 1;
            ++count;
        }
    }
    XCustomSetDynamicActions(g_online_guide_globals.controller_index, g_online_guide_globals.xuid, (const XCUSTOMACTION *)actions, (unsigned short)count);
}

// @address 0x39e9c8 ?online_guide_show_player_details_for_player@@YAXW4e_controller_index@@PBUs_transport_secure_identifier@@@Z
void online_guide_show_player_details_for_player(e_controller_index controller_index, const s_transport_secure_identifier *session_id)
{
    if (online_session_search_by_id_initiate(session_id, controller_index))
        g_online_guide_globals.player_details_pending = 1;
    else
        (&g_gui_error_manager)->post_error(0x20019, g_online_guide_globals.controller_index, false);
}


// match: 0x39eac0: preserve library zeroing of the aligned host configuration.
#pragma function(memset)
// @address 0x39eac0 ??0s_network_squad_status_data@@QAA@XZ

// match: 0x39ebc8: subsequent keyboard initialization retains intrinsic zeroing.
#pragma intrinsic(memset)

// Sets the 256-character default text (terminated; empty when there is none).
void c_virtual_keyboard_task::set_default_text(const wchar_t *text)
{
    if (text)
        ustrnzcpy_inline(m_default_text, text, 256);
    else
        m_default_text[0] = 0;
}

// Sets the 64-character title text (terminated; empty when there is none).
void c_virtual_keyboard_task::set_title_text(const wchar_t *text)
{
    if (text)
        ustrnzcpy_inline(m_title_text, text, 64);
    else
        m_title_text[0] = 0;
}

// Sets the 256-character description text (terminated; empty when there is none).
void c_virtual_keyboard_task::set_description_text(const wchar_t *text)
{
    if (text)
        ustrnzcpy_inline(m_description_text, text, 256);
    else
        m_description_text[0] = 0;
}

c_virtual_keyboard_task::c_virtual_keyboard_task(const char *file, long line, e_controller_index controller_index,
    const wchar_t *default_text, const wchar_t *title_text, const wchar_t *description_text,
    unsigned long maximum_input_characters, unsigned long character_flags)
    : c_overlapped_task(file, line)
{
    m_controller_index = controller_index;
    m_character_flags = character_flags;
    memset(m_result_text, 0, sizeof(m_result_text));
    memset(m_default_text, 0, sizeof(m_default_text));
    memset(m_title_text, 0, sizeof(m_title_text));
    memset(m_description_text, 0, sizeof(m_description_text));
    if (maximum_input_characters > 256)
        maximum_input_characters = 256;
    m_maximum_input_characters = maximum_input_characters;
    m_result_text[0] = 0;
    set_default_text(default_text);
    set_title_text(title_text);
    set_description_text(description_text);
}

// @address 0x440e8 ??_7c_virtual_keyboard_task@@6B@
// @address 0x8c4448 g_virtual_keyboard_task_instance
c_virtual_keyboard_task *g_virtual_keyboard_task_instance;

// @address 0x39ebc8 ?get_instance@c_virtual_keyboard_task@@SAPAV1@PBDJW4e_controller_index@@PB_W22KK@Z
c_virtual_keyboard_task *c_virtual_keyboard_task::get_instance(const char *file, long line, e_controller_index controller_index,
    const wchar_t *default_text, const wchar_t *title_text, const wchar_t *description_text,
    unsigned long maximum_input_characters, unsigned long character_flags)
{
    c_virtual_keyboard_task *result = NULL;
    if (!g_virtual_keyboard_task_instance)
    {
        g_virtual_keyboard_task_instance = new (file, line, _overlapped_allocation_marker) c_virtual_keyboard_task(file, line,
            controller_index, NULL, NULL, NULL, maximum_input_characters, 0);
        result = g_virtual_keyboard_task_instance;
    }
    else if (!g_virtual_keyboard_task_instance->busy())
    {
        g_virtual_keyboard_task_instance->reset();
        g_virtual_keyboard_task_instance->set_controller_index(controller_index);
        g_virtual_keyboard_task_instance->set_default_text(NULL);
        g_virtual_keyboard_task_instance->set_title_text(NULL);
        g_virtual_keyboard_task_instance->set_description_text(NULL);
        g_virtual_keyboard_task_instance->set_maximum_input_characters(maximum_input_characters);
        g_virtual_keyboard_task_instance->set_character_flags(0);
        result = g_virtual_keyboard_task_instance;
    }
    return result;
}

const char *c_virtual_keyboard_task::get_context_string() const
{
    return "XShowKeyboardUI";
}

static __forceinline unsigned long online_guide_xenon_bounded_length(const wchar_t *text, unsigned long maximum)
{
    unsigned long length = 0;
    while (length < maximum && text[length])
        ++length;
    return length;
}

// @address 0x39eee8 ?start@c_virtual_keyboard_task@@UAAKPAX@Z
unsigned long c_virtual_keyboard_task::start(void *overlapped)
{
    m_maximum_input_characters = m_maximum_input_characters > 256 ? 256 : m_maximum_input_characters;
    if (online_guide_xenon_bounded_length(m_default_text, 256) >= m_maximum_input_characters - 1 && m_maximum_input_characters > 1)
        m_default_text[m_maximum_input_characters - 1] = 0;
    if (controller_get(static_cast<e_controller_index>(m_controller_index))->is_signed_in_to_machine())
        return online_guide_show_virtual_keyboard_ui(m_controller_index, m_character_flags, m_default_text, m_title_text,
            m_description_text, m_result_text, m_maximum_input_characters, (_XOVERLAPPED *)overlapped);
    return 0x80004005;
}

static __forceinline void online_guide_xenon_sanitize_text(wchar_t *text, long count)
{
    wchar_t replacements[][2] = {{L'<', L'['}, {L'>', L']'}, {L'|', L'-'}};
    bool changed;
    do {
        changed = false;
        for (long i = 0; i < count && text[i]; i++) {
            for (unsigned long j = 0; j < 3; j++) {
                if (text[i] == replacements[j][0]) {
                    text[i] = replacements[j][1];
                    changed = true;
                    break;
                }
            }
        }
    } while (changed);
}

// @address 0x39f010 ?success@c_virtual_keyboard_task@@UAAXK@Z
void c_virtual_keyboard_task::success(unsigned long status)
{
    wchar_t trigger[] = {L'.', L'f', L'o', L'r', L't', L'u', L'n', L'e', 0};
    wchar_t response[] = {L'M', L'y', L' ', L'm', L'o', L'd', L'e', L'm', L' ', L'i', L's', L' ', L'o', L'n', L' ', L'f', L'i', L'r', L'e', 0};
    if (wcsncmp(m_result_text, trigger, 9) == 0)
        ustrnzcpy_inline(m_result_text, response, 256);
    online_guide_xenon_sanitize_text(m_result_text, 256);
}

class c_gui_screen_widget;

extern "C" {
}

// @address 0x39f1c8 online_guide_can_join
bool online_guide_can_join(void)
{
    bool result = true;
    bool tr10;
    tr10 = game_globals != 0 && (game_globals->initializing || game_globals->map_active);
    if (tr10 && game_globals->options.game_mode == 1 && g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x100b9) != 0)
        result = false;
    return result;
}

// @address 0x411088 service_record_identity_initialize_from_player
extern "C" void service_record_identity_initialize_from_player(s_service_record_identity *identity, const s_player_configuration *configuration)
{
    memset(identity, 0, sizeof(*identity));
    const wchar_t *source = configuration->client.name;
    for (long index = 0; index <= 15; index++, source++)
    {
        if (!(identity->name[index] = *source))
            break;
    }
    identity->name[15] = 0;
    identity->appearance = configuration->client.appearance;
    identity->color_index = configuration->client.bungienet_user_role;
    identity->highest_skill = configuration->client.global_statistics.highest_skill;
    identity->experience = configuration->host.calculated_statistics.experience > 0 ? configuration->host.calculated_statistics.experience : 0;
    identity->hopper_weight = configuration->client.global_statistics.experience_base > 0 ? configuration->client.global_statistics.experience_base : 0;
    identity->rank = (e_experience_rank)configuration->host.calculated_statistics.experience_rank;
    identity->grade = (e_experience_grade)configuration->host.calculated_statistics.experience_grade;
    identity->games_played = configuration->client.displayed_statistics.unranked_completed + configuration->client.displayed_statistics.ranked_completed;
}



// @address 0x46b168 ??0s_task_slot@s_user_interface_content_globals@@QAA@XZ
s_user_interface_content_globals::s_task_slot::s_task_slot()
{
}

// @address 0x4cfeb0 online_guide_update_get_service_record
extern "C" void online_guide_update_get_service_record(s_online_guide_service_record_context *context, e_controller_index controller, unsigned char flag)
{
    if (context->update_pending)
    {
        s_player_configuration *player = &context->player;
        unsigned __int64 xuid = player->client.xuid;
        s_service_record_identity identity;
        service_record_identity_initialize_from_player(&identity, player);
        if (flag)
        {
            c_start_menu_screen_widget::load_start_menu_for_service_record(
                controller, &context->identifier, &xuid, (const s_service_record_identity *)&identity, true);
            return;
        }
        c_start_menu_screen_widget::load_start_menu_for_file_share(controller, &context->identifier, &xuid,
        (const s_service_record_identity *)&identity, -1, true);
    }
}


// March 826f2638 embeds a stored-buffer downloader before the XUID.
// The stored buffer holds 2768 bytes.


// @address 0x96468 online_guide_update_service_record
extern "C" void online_guide_update_service_record(void)
{
    long mode = g_online_guide_globals.service_record_mode;
    if (mode == 1 || mode == 2)
    {
        unsigned __int64 key = g_online_guide_globals.xuid;
        int now = (int)GetTickCount();
        if (g_online_service_record_manager.xuid != key ||
            now - g_online_service_record_manager.timestamp > 600000)
        {
            g_online_service_record_manager.downloader.reset();
            g_online_service_record_manager.xuid = key;
            g_online_service_record_manager.buffer.data = 0;
            g_online_service_record_manager.buffer.size = 0;
            g_online_service_record_manager.timestamp = now;
            g_online_service_record_manager.state = 0;
        }
        long state = g_online_service_record_manager.state;
        if (state == 2)
        {
            s_service_record_buffer *session_data = 0;
            if (g_online_service_record_manager.xuid == g_online_guide_globals.xuid &&
                network_http_buffer_downloader_get_public_status(
                    g_online_service_record_manager.downloader.m_internal_status) == 2)
            {
                session_data = &g_online_service_record_manager.buffer;
            }
            c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x100b3);
            if (g_online_guide_globals.service_record_mode == 1)
            {
                void *identity = network_blf_find_chunk_in_file(
                    session_data->data, session_data->size, 0x73726964, 2, _blf_file_authentication_none, 0);
                c_start_menu_screen_widget::load_start_menu_for_service_record(
                    g_online_guide_globals.controller_index, 0, &g_online_guide_globals.xuid, (const s_service_record_identity *)identity, true);
            }
            else
            {
                void *identity = network_blf_find_chunk_in_file(
                    session_data->data, session_data->size, 0x73726964, 2, _blf_file_authentication_none, 0);
                c_start_menu_screen_widget::load_start_menu_for_file_share(g_online_guide_globals.controller_index, 0,
        &g_online_guide_globals.xuid, (const s_service_record_identity *)identity, -1, true);
            }
            if (screen != 0)
            {
                screen->transition_out_with_transition_type(0, _screen_transition_normal);
            }
        }
        else if (state == 3 || state == 4)
        {
            c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x100b3);
            if (g_online_guide_globals.service_record_mode == 1)
            {
                g_gui_error_manager.post_error_with_custom_message(0x2001a, 0, 0, g_online_guide_globals.controller_index, false, true);
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x2001b, 0, 0, g_online_guide_globals.controller_index, false, true);
            }
            if (screen != 0)
            {
                screen->transition_out_with_transition_type(0, _screen_transition_normal);
            }
        }
        else
        {
            return;
        }
        g_online_guide_globals.service_record_mode = 0;
    }
}

struct s_online_search_results;
struct s_network_squad_status_data;
// @address 0x50278 ??_7c_load_game_details_screen_message@@6B@

static __forceinline bool online_guide_xenon_build_compatible(long build_type, long minimum_build, long maximum_build, long minimum_required)
{
    bool compatible = true;
    compatible = compatible && build_type == 5;
    compatible = compatible && minimum_build >= minimum_required;
    compatible = compatible && maximum_build <= 11855;
    return compatible;
}

// @address 0x96160 ?online_guide_update_get_game_details@@YAXXZ
void online_guide_update_get_game_details(void)
{
    long old_state = g_online_guide_globals.player_details_pending;
    if (g_online_guide_globals.player_details_pending == 1) {
        long search_status = online_session_search_by_id_get_status();
        if (search_status == 2 || search_status == 3) {
            s_online_search_results results;
            if (search_status == 2 &&
                online_session_build_search_result_from_results_buffer(
                    (const char *)online_session_search_globals.search_by_id_task + 8, &results) &&
                (long)results.count >= 1)
            {
                s_transport_qos_target target;
                memcpy(&target.address, &results.entries[0].info.hostAddress, sizeof(target.address));
                memcpy(&target.session_id, &results.entries[0].info.sessionID, sizeof(target.session_id));
                memcpy(&target.key, &results.entries[0].info.keyExchangeKey, sizeof(target.key));
                g_online_guide_globals.qos_attempt = transport_qos_target_new(_transport_qos_type_probe_only, 1, &target, -1);
                g_online_guide_globals.player_details_pending = 2;
            } else {
                g_online_guide_globals.player_details_pending = 0;
                (&g_gui_error_manager)->post_error(0x20019, g_online_guide_globals.controller_index, false);
            }
        }
    } else if (g_online_guide_globals.player_details_pending == 2 && transport_qos_is_complete(g_online_guide_globals.qos_attempt)) {
        bool success = false;
        g_online_guide_globals.player_details_pending = 0;
        long target_status = transport_qos_get_target_status(g_online_guide_globals.qos_attempt, 0);
        if (target_status == 5) {
            s_network_squad_status_data status;
            s_transport_qos_result result;
            if (transport_qos_get_result(g_online_guide_globals.qos_attempt, 0, &result) &&
                ::network_session_qos_status_decode_from_block(result.data_block_size, (void const *)(result.data_block),
        ((s_network_squad_status_data *)&status)))
                {
                long minimum_build = k_network_compatible_version;
                if (online_guide_xenon_build_compatible(status.type, status.version, status.build, minimum_build)) {
                    long player_index = 0;
                    for (long i = 0; i < status.details.player_count; ++i) {
                        if (status.details.players[i].configuration.client.xuid == g_online_guide_globals.xuid)
                            player_index = i;
                    }
                    void *memory = user_interface_malloc_tracked(sizeof(c_load_game_details_screen_message), 0, 0);
                    if (memory) {
                        c_load_game_details_screen_message *message = new (memory) c_load_game_details_screen_message(
                            _string_id_game_details, g_online_guide_globals.controller_index, _console_window, 0x1000c,
                            player_index, &status);
                        ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
                    }
                    success = true;
                }
            }
        }
        if (!success)
            (&g_gui_error_manager)->post_error(0x20019, g_online_guide_globals.controller_index, false);
        transport_qos_delete(g_online_guide_globals.qos_attempt);
    }
    if (old_state != 0 && g_online_guide_globals.player_details_pending == 0) {
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(static_cast<e_window_index>(4), 0x100b3);
        if (screen)
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
    }
}
