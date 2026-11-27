// interfacelibxbox/c_player_marketplace.obj: from the records 0x4a3fc0 0x4a42b8 0x70a708
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "cseries/cseries.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/controller_types.h"
#include "interface/c_player_marketplace.h"
#include "saved_games/marketplace_asset_task_types.h"
#include "interface/marketplace_consume_assets_task.h"
#include "interface/c_window_manager.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/session/network_session_interface_types.h"
#include "cseries/language.h"
#include "cseries/language_types.h"
#include "interface/c_deletable.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_window_manager.h"
#include <new>
#include <string.h>
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x74b540 g_current_language
// @address 0x7c1c34 g_marketplace_http_downloader
// @address 0x1bec80 ?get_language_suffix@@YAPBDW4e_language@@_N@Z

// Both asset records carry the same XDK payload.
typedef char marketplace_asset_copy_layout[(sizeof(XMARKETPLACE_ASSET) == sizeof(s_marketplace_asset)
    && offsetof(XMARKETPLACE_ASSET, dwAssetID) == offsetof(s_marketplace_asset, asset_id)
    && offsetof(XMARKETPLACE_ASSET, dwQuantity) == offsetof(s_marketplace_asset, quantity)) ? 1 : -1];
extern "C" {
c_marketplace_request_consume_assets_task g_marketplace_consume_assets_task;
}

// Other compilers only: the Xbox link takes this object's constant-initialized .data words and its initializer from the image.
#if !defined(_XBOX) || defined(__clang__)
c_http_stored_buffer_downloader<4096> g_marketplace_http_downloader;
#endif

// @address 0x4a42b8 player_marketplace_request_user_catalog
extern "C" bool player_marketplace_request_user_catalog(s_player_marketplace_user *user, c_controller_interface *controller)
{
    s_network_http_buffer_url url;
    unsigned __int64 xuid = 0;

    user->pending = false;
    url.lifetime_seconds = 600;
    user->state = 1;
    short user_index = controller->get_user_index();
    url.name[0] = 0;
    url.service_type = 0;
    url.cache_mode = 0;
    url.queue_type = 0;
    if (user_index >= 0 && user_index < 4)
        xuid = g_network_session_interface_local_users[user_index].xuid;

    e_language language = get_current_language();
    ((c_static_string<256> *)url.name)->print("/gameapi/UserGetBnetSubscription.ashx?userId=%016I64X&locale=%s", xuid, get_language_suffix(language, false));
    url.cache_mode = 0;
    url.queue_type = 1;
    url.service_type = 2;
    g_marketplace_http_downloader.reset();
    http_buffer_downloader_set_url(&g_marketplace_http_downloader, &url);
    return true;
}

static __forceinline unsigned __int64 c_player_marketplace_xuid(c_controller_interface *controller)
{
    unsigned __int64 xuid = 0;
    short index = controller->get_user_index();
    if (index >= 0 && index < 4)
        xuid = g_network_session_interface_local_users[index].xuid;
    return xuid;
}

// @address 0x4a3fc0 player_marketplace_handle_request_state
extern "C" void player_marketplace_handle_request_state(s_player_marketplace_user *user, c_controller_interface *controller, long unused, long state)
{
    bool success = false;
    s_network_http_buffer_url begin_url;
    s_network_http_buffer_url complete_url;
    switch (state)
    {
    case 0:
        if (user->message_start_time != -1)
        {
            success = true;
            c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, 0x100b3);
            if (screen)
                screen->transition_out_with_transition_type(0, _screen_transition_normal);
            user->message_start_time = -1;
        }
        else
            success = true;
        break;
    case 1:
        user->owns_asset = false;
        user->enumeration_state = 1;
        g_marketplace_asset_enumeration_overlapped_task.m_enumeration_handle = (void *)-1;
        g_marketplace_asset_enumeration_overlapped_task.m_asset_count = 0;
        g_marketplace_asset_enumeration_overlapped_task.m_controller_index = controller->get_controller_index();
        success = overlapped_task_start_internal(&g_marketplace_asset_enumeration_overlapped_task,
            "..\\interface\\c_player_marketplace.cpp" /* __FILE__ */, 0x1be);
        break;
    case 2:
        {
            s_network_http_buffer_url &url = begin_url;
            long asset_id = user->asset.asset_id;
            url.name[0] = 0;
            url.service_type = 0;
            url.cache_mode = 0;
            url.lifetime_seconds = 600;
            url.queue_type = 0;
            ((c_static_string<256> *)url.name)->print(
                "/gameapi/UserBeginConsume.ashx?userId=%016I64X&consumableId=%08X", c_player_marketplace_xuid(controller), asset_id);
            url.cache_mode = 0;
            url.service_type = 2;
            url.queue_type = 1;
            g_marketplace_http_downloader.reset();
            http_buffer_downloader_set_url(&g_marketplace_http_downloader, &url);
            success = true;
            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), (char const *)(0), 0);
            c_load_in_progress_screen_message *message = memory
                ? new (memory) c_load_in_progress_screen_message((e_controller_index)user->controller_index, _console_window, 0x100ef, 0x100f0) : 0;
            if (message)
            {
                ::user_interface_messaging_post(message);
                user->message_start_time = GetTickCount();
            }
        }
        break;
    case 3:
        g_marketplace_consume_assets_task.controller_index = controller->get_controller_index();
        memcpy(&g_marketplace_consume_assets_task.asset, &user->asset, sizeof(user->asset));
        success = overlapped_task_start_internal(&g_marketplace_consume_assets_task, "..\\interface\\c_player_marketplace.cpp" /* __FILE__ */, 0x233);
        break;
    case 4:
        {
            s_network_http_buffer_url &url = complete_url;
            long asset_id = user->asset.asset_id;
            url.name[0] = 0;
            url.service_type = 0;
            url.cache_mode = 0;
            url.lifetime_seconds = 600;
            url.queue_type = 0;
            ((c_static_string<256> *)url.name)->print(
                "/gameapi/UserCompleteConsume.ashx?userId=%016I64X&consumableId=%08X", c_player_marketplace_xuid(controller), asset_id);
            url.cache_mode = 0;
            url.service_type = 2;
            url.queue_type = 1;
            g_marketplace_http_downloader.reset();
            http_buffer_downloader_set_url(&g_marketplace_http_downloader, &url);
            success = true;
        }
        break;
    case 5:
        success = player_marketplace_request_user_catalog(user, controller);
        break;
    }
    user->current_state = success ? state : 0;
}



// @address 0x315ba30 g_player_marketplace_users
extern "C" {
s_player_marketplace_user g_player_marketplace_users[4];
}

