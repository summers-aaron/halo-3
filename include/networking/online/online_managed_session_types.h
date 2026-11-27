#ifndef NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_TYPES_H
#define NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_TYPES_H

#include "networking/online/online_context_types.h"
#include "networking/online/online_managed_session_flags.h"

#include "cseries/async_xoverlapped_types.h"
#include "cseries/cseries_wrapped.h"
#include "networking/transport/transport_security_types.h"
#include <stddef.h>

#include "networking/online/online_session_types.h"

#include "networking/online/online_managed_session_task.h"


typedef c_flags_no_init<e_online_managed_session_flags, unsigned long, k_online_managed_session_flags_count> c_online_managed_session_flags;
typedef c_flags_no_init<e_online_managed_session_operation_flags, unsigned short,
    k_online_managed_session_operation_flags_count> c_online_managed_session_operation_flags;

struct s_online_managed_session
{
    long session_class;
    long platform;
    union { unsigned long flags; c_online_managed_session_flags managed_flags; };
    union { unsigned short primary_flags; unsigned short current_operations; c_online_managed_session_operation_flags current_operation_bits; };
    union { unsigned short secondary_flags; unsigned short pending_operations; c_online_managed_session_operation_flags pending_operation_bits; };
    c_managed_session_overlapped_task task;
    unsigned long modification_time;
    bool create_failed_online_disabled;
    bool create_failed_network_unavailable;
    bool create_failed_session_id_in_use;
    s_online_session desired;
    s_online_session pending;
    s_online_session actual;
    unsigned __int64 queued_player_xuids[16];
    bool queued_player_filter[16];
    bool queued_player_private_slots[16];
    long queued_player_count;
    bool host_migration_requires_new_key;
    bool previous_host;
    s_transport_session_description previous_description;
    s_online_context creation_contexts[4];
    long creation_context_count;
};

struct s_online_session_manager_globals
{
    union { unsigned short current_operation_flags; c_online_managed_session_operation_flags current_operations; };
    s_online_managed_session sessions[8];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char managed_session_task_size[(sizeof(void *) != 4 || sizeof(c_managed_session_overlapped_task) == 0xf8) ? 1 : -1];
typedef char online_managed_session_size[(sizeof(void *) != 4 || sizeof(s_online_managed_session) == 0x638) ? 1 : -1];
typedef char online_managed_session_actual_offset[(sizeof(void *) != 4 || offsetof(s_online_managed_session, actual) == 0x3d0) ? 1 : -1];
typedef char online_managed_session_context_offset[(sizeof(void *) != 4 || offsetof(s_online_managed_session, creation_contexts) == 0x614) ? 1 : -1];
typedef char online_session_manager_sessions_offset[(offsetof(s_online_session_manager_globals, sessions) == 8) ? 1 : -1];
#endif

#endif // NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_TYPES_H
