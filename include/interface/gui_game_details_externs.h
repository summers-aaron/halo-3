#include "networking/online/online_cached_user.h"
#ifndef INTERFACE_GUI_GAME_DETAILS_EXTERNS_H
#define INTERFACE_GUI_GAME_DETAILS_EXTERNS_H

// External dependencies used by gui_game_details.
template<long k_maximum_count> class c_static_wchar_string;
struct s_data_array;
struct levels_map_datum;

extern "C" unsigned char user_interface_get_timespan_string(long timespan, long format, long relative, c_static_wchar_string<1024> *string);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, levels_map_datum *result);
extern "C" long network_squad_session_get_countdown_delayed_culprit(void);


#include "cseries/async_xoverlapped.h"
struct s_levels_globals;
class c_gui_screen_widget;
class c_gui_game_details;
extern "C" {
unsigned long XShowMessageComposeUI(unsigned long controller_index, const unsigned __int64 *recipients, unsigned long recipient_count, const wchar_t *text);
}

#endif // INTERFACE_GUI_GAME_DETAILS_EXTERNS_H
