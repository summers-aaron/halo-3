#ifndef INTERFACE_GUI_GAME_BROWSER_EXTERNS_H
#define INTERFACE_GUI_GAME_BROWSER_EXTERNS_H

// External dependencies used by gui_game_browser.
struct s_data_array;
struct levels_map_datum;
template<long k_maximum_count> class c_static_wchar_string;
struct s_network_squad_status_data;
class c_gui_widget;
class c_gui_screen_widget;

extern "C" unsigned char player_appearance_valid(const char *appearance);
extern "C" bool levels_try_and_get_datum(s_data_array *data, long unique_id, levels_map_datum *result);
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" bool user_interface_remote_squad_is_joinable(const s_network_squad_status_data *squad);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);


extern "C" unsigned long GetTickCount(void);

#endif // INTERFACE_GUI_GAME_BROWSER_EXTERNS_H
