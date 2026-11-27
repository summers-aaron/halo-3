#ifndef NETWORKING_ONLINE_MANAGED_SESSION_STATUS_FLAGS_H
#define NETWORKING_ONLINE_MANAGED_SESSION_STATUS_FLAGS_H

enum e_managed_session_status_flags
{
    _managed_session_status_created = 1,
    _managed_session_operation_in_progress_bit = 11,
    _managed_session_end_succeeded_bit = 12,
    _managed_session_end_failed_bit = 13
};

#endif // NETWORKING_ONLINE_MANAGED_SESSION_STATUS_FLAGS_H
