#ifndef INTERFACE_USER_INTERFACE_NETWORKING_EXTERNS_H
#define INTERFACE_USER_INTERFACE_NETWORKING_EXTERNS_H

#include "main/game_preferences_game_setup.h"

#include "networking/logic/network_join.h"

// External dependencies used by user_interface_networking.
struct s_advert_storage;
class c_gui_queued_error;
enum e_controller_index;

extern "C" unsigned char user_interface_global_string_get(int handle, unsigned short *out);
extern "C" void gui_pregame_setup_manager_save_current_setup(long mode, const void *source, s_advert_storage *storage);
extern "C" unsigned char network_squad_session_set_primary_skulls(int);
extern "C" unsigned char network_squad_session_set_secondary_skulls(int);
extern "C" unsigned char network_squad_session_set_campaign_metagame_scoring(unsigned short);
extern "C" unsigned char network_squad_session_set_campaign_insertion_point(unsigned short);
extern "C" c_gui_queued_error *gui_error_manager_find_queued_error(c_gui_queued_error *, e_controller_index, long, const wchar_t *);

class c_network_session;
class c_gui_screen_widget;
class c_message;
extern "C" {
void network_squad_session_set_privacy_mode(c_network_session *session, long privacy_mode, long closed_mode, long is_closed_by_user, long maximum_player_count);
}

#endif // INTERFACE_USER_INTERFACE_NETWORKING_EXTERNS_H
