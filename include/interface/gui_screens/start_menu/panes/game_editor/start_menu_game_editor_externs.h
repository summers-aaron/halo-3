#ifndef INCLUDE_INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_EXTERNS_H
#define INCLUDE_INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_EXTERNS_H

#include "main/game_preferences_game_setup.h"

#include "interface/user_interface_error_manager.h"
#include "shell/shell.h"
class c_content_catalogue;
class c_map_variant;
class c_start_menu_game_editor;
struct c_synchronized_long;
struct s_content_item;

extern "C" {
s_content_item *content_catalogue_try_and_get_item(const c_content_catalogue *catalogue, long content_item_index);
long saved_game_files_write_content_item(long catalogue_index, long content_item_index, long file_type, const void *buffer,
    unsigned long size, c_synchronized_long *completion, void *done);
#include "saved_games/content_catalogue_lookup.h"
void user_interface_show_device_selection_dialog(int, int, int);
void start_menu_game_multiplayer_handle_leave_game(c_start_menu_game_editor *screen, long controller);
void content_catalogue_trim_whitespace(unsigned short *string);
bool content_catalogue_is_item_name_valid(c_content_catalogue *catalogue, e_controller_index controller, const wchar_t *name, long type);
long saved_game_files_create_content_item_async(
    int controller, const wchar_t *name, int type, c_synchronized_long *success, c_synchronized_long *complete, c_synchronized_long *item);
void game_engine_set_map_info_name(c_map_variant *variant, const wchar_t *name);
}

struct c_static_wchar_string_1024;
struct s_saved_game_item_metadata;
typedef s_saved_game_item_metadata s_saved_game_item_metadata_record;
struct s_data_array;
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string_1024 *string);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, void *result);

extern "C" void saved_game_files_initialize_metadata(s_saved_game_item_metadata_record *out, unsigned int index, int value,
    const wchar_t *name, const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes);


extern "C" long _InterlockedDecrement(volatile long *);
extern "C" long _InterlockedExchange(long volatile *, long);

class c_window_manager;
class c_gui_screen_widget;
struct s_game_engine_globals;

extern "C" {

extern c_window_manager g_window_manager;
}

#endif // INCLUDE_INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_EXTERNS_H
