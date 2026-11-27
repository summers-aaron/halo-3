#ifndef INTERFACE_C_PLAYER_MARKETPLACE_H
#define INTERFACE_C_PLAYER_MARKETPLACE_H

#include "cseries/async_xoverlapped_types.h"
#include <stddef.h>
#include "cseries/cseries.h"
#include "saved_games/marketplace_asset_types.h"

// Functions defined in tu/interfacelibxbox/c_player_marketplace.cpp that other units call.

struct c_controller_interface;

#include "networking/logic/storage/network_http_buffer_downloader.h"

// One per controller: the marketplace request state machine
// (player_marketplace_handle_request_state) and its subscription and asset results.
#pragma pack(push, 4)
struct s_player_marketplace_user
{
    __forceinline s_player_marketplace_user()
        : controller_index(-1),
          refresh_requested(false),
          current_state(0),
          reserved_c(0),
          state(0),
          pending(false),
          catalogue_state(0),
          enumeration_state(0),
          owns_asset(false),
          message_start_time(-1)
    {
    }

    long controller_index;
    // Set when a consume finishes or the subscription hash changes; once both
    // requests are done (state == 2, enumeration_state == 2) they start over.
    bool refresh_requested;
    long current_state;
    union { long reserved_c; long controller_state; };
    long state;
    union { bool pending; bool signed_in; };
    long catalogue_state;
    long pad_1c;
    unsigned __int64 offer_id;
    c_static_wchar_string<256> strings[8];
    unsigned __int64 subscription_seconds;
    long catalogue_version;
    long enumeration_state;
    bool owns_asset;
    s_marketplace_asset asset;
    long message_start_time;
    long pad_1048; // nothing reads or writes it
    bool consume_pending;
};
#pragma pack(pop)

typedef char player_marketplace_user_size[(sizeof(s_player_marketplace_user) == 0x1050) ? 1 : -1];
typedef char player_marketplace_user_offer_offset[(offsetof(s_player_marketplace_user, offer_id) == 0x20) ? 1 : -1];
typedef char player_marketplace_user_enumeration_state_offset[(offsetof(s_player_marketplace_user, enumeration_state) == 0x1034) ? 1 : -1];
typedef char player_marketplace_user_owns_asset_offset[(offsetof(s_player_marketplace_user, owns_asset) == 0x1038) ? 1 : -1];
typedef char player_marketplace_user_asset_offset[(offsetof(s_player_marketplace_user, asset) == 0x103c) ? 1 : -1];
typedef char player_marketplace_user_message_start_time_offset[(offsetof(s_player_marketplace_user, message_start_time) == 0x1044) ? 1 : -1];
typedef char player_marketplace_user_consume_pending_offset[(offsetof(s_player_marketplace_user, consume_pending) == 0x104c) ? 1 : -1];

extern "C" s_player_marketplace_user g_player_marketplace_users[4];

extern "C" void player_marketplace_handle_request_state(s_player_marketplace_user *user, c_controller_interface *controller, long unused, long state);

// Data the marketplace requests use.
extern "C" {
extern c_http_stored_buffer_downloader<4096> g_marketplace_http_downloader;
extern long g_marketplace_active_controller_count;
}


class c_marketplace_request_consume_assets_task;
extern "C" c_marketplace_request_consume_assets_task g_marketplace_consume_assets_task;
#endif // INTERFACE_C_PLAYER_MARKETPLACE_H
