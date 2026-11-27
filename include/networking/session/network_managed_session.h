#ifndef NETWORKING_SESSION_NETWORK_MANAGED_SESSION_H
#define NETWORKING_SESSION_NETWORK_MANAGED_SESSION_H

#include "networking/session/network_session_class.h"

// Functions defined in tu/networkinglibxbox/network_managed_session.cpp.

struct s_transport_secure_identifier;

void managed_session_modify_slot_counts(long index, long public_slot_count, long private_slot_count, bool friends_only, bool request_modification);
bool managed_session_get_id(long index, s_transport_secure_identifier *identifier);
long managed_session_count_master_sessions();
void managed_session_commit_new_host(long index);
void managed_session_cancel_new_host(long index);
void managed_session_players_remove_complete(long index, bool success, unsigned long error);
void managed_session_process_pending_operations(long index);

struct s_online_session_player;
struct s_transport_secure_address;
struct s_transport_session_description;
struct transport_address;
enum e_transport_platform;

bool managed_session_get_security_information(long index, bool include_private, s_transport_session_description *description, e_transport_platform *platform);

extern "C" void managed_session_delete(long managed_session_index);
extern "C" void managed_session_reset_session(long index, bool reset_time);
extern "C" void managed_session_add_players(long index, const unsigned __int64 *player_identifiers,
    const bool *player_flags, const bool *player_flags_2, long player_count);
extern "C" void managed_session_game_start(long index);
extern "C" void managed_session_game_end(long index);
extern "C" long managed_session_retrieve_security_information(const transport_address *usable_address, long platform,
    s_transport_session_description *session_description, s_transport_secure_address *secure_address);
extern "C" unsigned char managed_session_is_valid_handle(void *handle);
extern "C" void managed_session_use_new_host(long session_index);
extern "C" void remove_from_player_list(s_online_session_player *players, long player_count, const unsigned __int64 *identifiers, long identifier_count);
extern "C" void managed_session_remove_players(long index, const unsigned __int64 *identifiers, long identifier_count);

struct s_online_managed_session;
struct s_online_session_manager_globals;

long managed_session_who_has_this_session_id(struct s_transport_secure_identifier const * session_id);
void managed_session_delete_session_internal(long index, s_online_managed_session *session);
bool managed_session_build_removal_xuid_list(long index);
bool managed_session_build_add_xuid_list(long index);
extern "C" void add_to_player_list(s_online_session_player *players, long player_count, const unsigned __int64 *xuids,
    const bool *flags, const bool *private_flags, long count);
long managed_session_allocate(void);
void managed_session_successful_creation_complete(long index);
void managed_session_creation_complete(long index, bool success, unsigned long error);
void managed_session_successful_delete_complete(long index);
void managed_session_deletion_complete(long index, bool success, unsigned long error);
void managed_session_successful_host_migration_creation_complete(long managed_session_index);
extern "C" void managed_session_successful_host_migration_delete_complete(long index);
extern "C" void managed_session_host_migration_creation_complete(long index, bool success);
void managed_session_successful_players_add_complete(long index);
void managed_session_completely_reset_session_on_error(long index);

// Data defined in tu/networkinglibxbox/network_managed_session.cpp that other units use.
extern s_online_session_manager_globals online_session_manager_globals;
bool managed_session_can_create_session(enum e_network_session_class _sym_p0);

void managed_session_connect_client_to_new_host(long index, const s_transport_session_description *description, e_transport_platform platform);

#endif // NETWORKING_SESSION_NETWORK_MANAGED_SESSION_H
