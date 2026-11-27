#ifndef NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_ACCESS_H
#define NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_ACCESS_H
#include "networking/online/online_managed_session_types.h"
enum e_managed_session_creation_flags { _managed_session_creation_master };
extern s_online_session_manager_globals online_session_manager_globals;
static __forceinline s_online_managed_session *online_session_get(long index)
{
    return &online_session_manager_globals.sessions[index];
}
// Preserve the shift-and-extract form used by the managed-session callers.
static __forceinline bool online_managed_session_test_bit(unsigned long flags, long bit)
{
    bool result = (flags >> bit) & 1;
    return result;
}
// March network_managed_session.obj: bool managed_session_is_master_session(long).
static __forceinline bool managed_session_is_master_session(long index)
{
    bool result = false;
    if (index != -1 && online_managed_session_test_bit(online_session_manager_globals.sessions[index].flags, 1))
        result = true;
    return result;
}
#endif
