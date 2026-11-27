#ifndef NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_H
#define NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_H

#include "networking/session/network_session_class.h"

// Functions defined in tu/networkinglibxbox/network_life_cycle.cpp that other units call.

class c_network_session_manager;
struct s_life_cycle_matchmaking_progress;

bool network_life_cycle_squad_local_peer_is_leader(void);
void network_life_cycle_get_matchmaking_progress(s_life_cycle_matchmaking_progress *progress);
bool network_life_cycle_set_pre_game_state();
void network_life_cycle_abort_matchmaking();
void network_life_cycle_dispose();
void network_life_cycle_end();
class c_network_session_parameter_type_collection;
c_network_session_parameter_type_collection *&network_life_cycle_parameter_storage();
c_network_session_manager *&network_life_cycle_manager_storage();
class c_network_observer;
c_network_observer *&network_life_cycle_observer_storage();

class c_network_session;

extern "C" bool network_life_cycle_try_and_get_squad_session(c_network_session **session);
extern "C" bool network_life_cycle_in_session(c_network_session **session);
extern "C" bool network_life_cycle_is_squad_host(void);
extern "C" void network_life_cycle_handle_lost_connection(void);
extern "C" bool network_life_cycle_can_download_files(void);
c_network_session *&network_life_cycle_session_storage();
extern "C" bool network_life_cycle_initialize(void *observer, void *session_manager, void *squad_session_one, void *squad, void *group);
bool network_life_cycle_create_local_squad(e_network_session_class session_class);

bool network_life_cycle_in_group_session(c_network_session **session);

#endif // NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_H
