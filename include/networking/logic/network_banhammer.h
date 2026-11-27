#ifndef NETWORKING_LOGIC_NETWORK_BANHAMMER_H
#define NETWORKING_LOGIC_NETWORK_BANHAMMER_H

// Functions defined in tu/networkinglibxbox/network_banhammer.cpp that other units call.

#include "networking/online/online_service_record.h"

enum e_controller_index;
enum e_hopper_load_status;

e_hopper_load_status network_banhammer_loaded_for_controller(e_controller_index controller_index, bool *unused_status);

bool network_banhammer_initialize();
void network_banhammer_upload_network_statistics();
void network_banhammer_report_on_cheaters(void);
void network_banhammer_update_controllers();
void network_banhammer_update_machine(void);

class c_life_cycle_state_handler;
extern "C" bool network_banhammer_test_local_player_xuids(const c_life_cycle_state_handler *owner);

extern "C" void network_banhammer_find_cheaters_in_session(void);
extern "C" bool network_banhammer_test_map_signature(const unsigned char *signature);
extern "C" unsigned short network_banhammer_find_cheaters_in_current_session(void);
extern "C" void network_banhammer_update_user_download(c_online_service_record_manager *download);
extern "C" void network_banhammer_update_map_manifest();

// Data defined in tu/networkinglibxbox/network_banhammer.cpp that other units use.
struct s_network_banhammer_globals;
extern s_network_banhammer_globals g_network_banhammer_globals;


#endif // NETWORKING_LOGIC_NETWORK_BANHAMMER_H
