#ifndef INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_EXTERNS_H
#define INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_EXTERNS_H

#include "interface/user_interface_shared_constants.h"

// External dependencies used by gui_start_menu_hq_service_record_file_share.
#include "interface/c_player_marketplace.h"
class c_gui_screen_widget;
class c_gui_widget;
class c_gui_start_menu_hq_service_record_file_share;

extern "C" bool marketplace_consume_assets(s_player_marketplace_user *user, unsigned __int64 xuid);











extern "C" void gui_start_menu_hq_service_record_file_share_update_enumerator(c_gui_start_menu_hq_service_record_file_share *screen);


#endif // INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_EXTERNS_H
