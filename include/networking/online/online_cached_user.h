#ifndef NETWORKING_ONLINE_ONLINE_CACHED_USER_H
#define NETWORKING_ONLINE_ONLINE_CACHED_USER_H

#include "networking/online/online_cached_user_globals.h"

static __forceinline bool online_cached_user_can_use_file_share(long controller_index)
{
    return g_online_cached_users[controller_index].can_use_file_share;
}

static __forceinline bool online_cached_user_is_silver_or_gold(long controller_index)
{
    return g_online_cached_users[controller_index].silver_or_gold;
}
static __forceinline bool online_cached_user_is_online_enabled(long controller_index)
{
    return g_online_cached_users[controller_index].online_enabled;
}

#endif
