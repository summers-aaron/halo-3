#ifndef NETWORKING_LOGIC_NETWORK_JOIN_H
#define NETWORKING_LOGIC_NETWORK_JOIN_H

#include "networking/session/network_session_class.h"

enum e_life_cycle_join_result
{
    _life_cycle_join_result_none = 0,
    _life_cycle_join_result_in_progress = 1,
    _life_cycle_join_result_generic_error = 2,
    _life_cycle_join_result_failed_to_find_session = 3,
    _life_cycle_join_result_not_enough_space = 4,
    _life_cycle_join_result_game_not_open = 5,
    _life_cycle_join_result_target_is_matchmaking = 6,
    _life_cycle_join_result_host_timed_out = 7,
    _life_cycle_join_result_peer_version_too_low = 8,
    _life_cycle_join_result_host_version_too_low = 9,
    _life_cycle_join_result_unable_to_connect_open_nat = 10,
    _life_cycle_join_result_unable_to_connect_moderate_nat = 11,
    _life_cycle_join_result_unable_to_connect_strict_nat = 12,
    _life_cycle_join_result_unable_to_connect_party_open_nat = 13,
    _life_cycle_join_result_unable_to_connect_party_moderate_nat = 14,
    _life_cycle_join_result_unable_to_connect_party_strict_nat = 15,
    _life_cycle_join_result_player_not_online_enabled = 16,
    _life_cycle_join_result_target_has_film_in_progress = 17,
    _life_cycle_join_result_target_has_campaign = 18,
    _life_cycle_join_result_user_content_not_permitted = 19
};

// Functions defined in tu/networkinglibxbox/network_join.cpp.

extern "C" long network_join_is_any_join_in_progress(void);

bool network_join_is_join_to_squad_in_progress(void);
void network_join_update();
void network_join_clear_squad_join_result(void);
void network_join_flush_join_queue(void);
extern "C" bool network_join_is_eula_accepted(void);
extern "C" bool network_join_is_leaving_group(void);

class c_network_session_membership;
class c_network_session;
struct s_networking_join_data;
struct s_transport_secure_address;

extern "C" unsigned char network_join_is_busy(void);
extern "C" bool network_join_test_catalogues_busy(unsigned long *mask);
extern "C" long network_join_map_refuse_reason_to_join_result(bool joining_group, const c_network_session *s, long reason);
void network_join_queue_update(void);
extern "C" void network_join_calculate_full_queue_desirability(const c_network_session_membership *membership);
void network_join_flush_our_join_from_join_queue(const s_transport_secure_address *our_address, unsigned __int64 our_join_nonce);
bool network_join_process_joins_from_queue();

// Data defined in tu/networkinglibxbox/network_join.cpp that other units use.
extern s_networking_join_data g_network_join_data;

struct s_transport_secure_identifier;
struct s_transport_secure_key;

unsigned char network_join_to_remote_squad(bool join_directly, bool join_as_friend, bool require_leader,
    e_network_session_class session_class, const s_transport_secure_identifier *session_identifier,
    const s_transport_secure_key *session_key, const s_transport_secure_address *session_address);

class c_network_session;
struct transport_address;
struct s_network_session_join_request;

void network_join_add_join_to_queue(c_network_session *session, const transport_address *address, const s_network_session_join_request *request);

struct s_online_context;
extern "C" const s_online_context k_network_join_squad_contexts[2];
extern "C" s_online_context g_network_join_group_contexts[2];
#endif // NETWORKING_LOGIC_NETWORK_JOIN_H
