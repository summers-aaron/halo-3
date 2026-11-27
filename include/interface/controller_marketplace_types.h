#ifndef INTERFACE_CONTROLLER_MARKETPLACE_TYPES_H
#define INTERFACE_CONTROLLER_MARKETPLACE_TYPES_H

#include <stddef.h>
#include "shell/shell.h"
#include "memory/static_arrays.h"

// Shared controller marketplace request and subscription response state.
struct s_controller_update {
    e_controller_index controller_index;
    bool consume_pending;
    long state;
    long update_count;
    long state_10;
    bool active_14;
    long subscription_status;
    unsigned __int64 offer_id;
    c_static_wchar_string<256> hq_button;
    c_static_wchar_string<256> hq_message;
    c_static_wchar_string<256> file_share_button;
    c_static_wchar_string<256> file_share_message;
    c_static_wchar_string<256> file_share_help;
    c_static_wchar_string<256> just_subscribed_message;
    c_static_wchar_string<256> currently_subscribed_message;
    c_static_wchar_string<256> over_quota_message;
    unsigned __int64 subscription_seconds;
    long subscription_hash;
    long marketplace_state;
    bool owns_asset;
};

typedef char controller_marketplace_state_offset[(offsetof(s_controller_update, state) == 8) ? 1 : -1];
typedef char controller_marketplace_offer_offset[(offsetof(s_controller_update, offer_id) == 0x20) ? 1 : -1];
typedef char controller_marketplace_request_offset[(offsetof(s_controller_update, marketplace_state) == 0x1034) ? 1 : -1];

typedef char controller_marketplace_size[(sizeof(s_controller_update) == 0x1040) ? 1 : -1];

#endif
