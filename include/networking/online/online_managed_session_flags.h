#ifndef NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_FLAGS_H
#define NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_FLAGS_H

enum e_online_managed_session_flags
{
    _online_managed_session_created_bit = 0,
    _online_managed_session_retry_pending_bit = 2,
    _online_managed_session_established_bit = 4,
    _online_managed_session_create_failed_bit = 5,
    _online_managed_session_host_migration_bit = 7,
    _online_managed_session_modify_locked_pending_bit = 17,
    _online_managed_session_modify_locked_bit = 18,
    k_online_managed_session_flags_count = 19
};

enum e_online_managed_session_operation_flags
{
    _online_managed_session_operation_delete_bit = 0,
    _online_managed_session_operation_create_bit = 2,
    _online_managed_session_operation_delete_host_migration_bit = 3,
    _online_managed_session_operation_create_host_migration_bit = 4,
    _online_managed_session_operation_reset_players_bit = 5,
    _online_managed_session_operation_remove_players_bit = 6,
    _online_managed_session_operation_modify_bit = 7,
    _online_managed_session_operation_add_players_bit = 8,
    _online_managed_session_operation_game_end_bit = 9,
    _online_managed_session_operation_game_start_bit = 10,
    k_online_managed_session_operation_flags_count = 11
};

#endif // NETWORKING_ONLINE_ONLINE_MANAGED_SESSION_FLAGS_H
