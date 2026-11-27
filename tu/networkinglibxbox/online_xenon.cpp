// networkinglibxbox/online_xenon.obj: from the records 0x8a9d8 0x3a72d0
// @flags /TP /O2 /J
// @address 0x8a9d8 ?online_local_xuid_is_online_enabled@@YA_NXZ

#include <xtl.h>
#include "networking/online/online_nat_types.h"
#include "networking/online/online_xenon.h"
#include "networking/online/online_globals_types.h"
#include "networking/online/online_cached_user_globals.h"



bool online_has_signed_in_user(long controller_index)
{
    return g_online_cached_users[controller_index].signed_in;
}

unsigned __int64 online_user_get_xuid(long controller_index)
{
    unsigned __int64 xuid = 0;
    if (g_online_cached_users[controller_index].online_xuid != 0)
        xuid = g_online_cached_users[controller_index].online_xuid;
    else
        xuid = g_online_cached_users[controller_index].offline_xuid;
    return xuid;
}

const wchar_t *online_user_get_name(long controller_index)
{
    const wchar_t *name = 0;
    if (g_online_cached_users[controller_index].signed_in)
        name = g_online_cached_users[controller_index].gamertag;
    return name;
}

bool online_local_xuid_is_silver_or_gold_live(long controller_index)
{
    return g_online_cached_users[controller_index].silver_or_gold;
}

bool online_xenon_user_is_online_enabled(long controller_index)
{
    return g_online_cached_users[controller_index].online_enabled;
}

bool online_local_xuid_is_online_restricted(long controller_index)
{
    return g_online_cached_users[controller_index].online_restricted;
}

unsigned __int64 online_user_get_player_identifier(long controller_index)
{
    return g_online_cached_users[controller_index].player_identifier;
}

bool online_xenon_user_can_use_file_share(long controller_index)
{
    return g_online_cached_users[controller_index].can_use_file_share;
}

bool online_xenon_user_get_unknown5(long controller_index)
{
    return g_online_cached_users[controller_index].can_use_friends_user_created_content;
}

extern "C" {
}
extern "C" {
}
extern "C" {
}
extern "C" {

}
extern "C" {
// @address 0x8086b6 g_network_online_connected

}

bool online_local_xuid_is_online_enabled(void)
{
    bool ethernet_available = XNetGetEthernetLinkStatus() != 0;
    bool online_user_found = false;

    if (online_guide_is_active())
    {
        online_user_found = true;
    }
    else
    {
        for (int index = 0; index < 4; index++)
        {
            if (XUserGetSigninState(index) == 2)
            {
                online_user_found = true;
                break;
            }
        }
    }

    return ethernet_available && online_user_found && g_network_online_connected;
}

extern "C" {
}

e_online_nat_type online_get_nat_type(void)
{
    long platformType = XOnlineGetNatType();
    e_online_nat_type type = _online_nat_type_unknown;
    switch (platformType) {
    case 1:
        type = _online_nat_type_open;
        break;
    case 2:
        type = _online_nat_type_moderate;
        break;
    case 3:
        type = _online_nat_type_strict;
        break;
    default:
        break;
    }
    return type;
}
