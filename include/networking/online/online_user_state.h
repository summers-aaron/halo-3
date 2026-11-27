#ifndef NETWORKING_ONLINE_ONLINE_USER_STATE_H
#define NETWORKING_ONLINE_ONLINE_USER_STATE_H
#include <stddef.h>

struct online_xenon_user_state
{
    bool signed_in;
    bool silver_or_gold;
    bool online_enabled;
    bool online_restricted;
    bool can_use_file_share;
    bool can_use_friends_user_created_content;
    long guest_number; // Guest number, or -1 when not a guest
    unsigned __int64 player_identifier;
    unsigned __int64 offline_xuid;
    unsigned __int64 online_xuid;
    wchar_t gamertag[16];
};

typedef char online_user_state_size[(sizeof(online_xenon_user_state) == 0x48) ? 1 : -1];
typedef char online_user_state_identifier_offset[(offsetof(online_xenon_user_state, player_identifier) == 0x10) ? 1 : -1];
typedef char online_user_state_gamertag_offset[(offsetof(online_xenon_user_state, gamertag) == 0x28) ? 1 : -1];
#endif
