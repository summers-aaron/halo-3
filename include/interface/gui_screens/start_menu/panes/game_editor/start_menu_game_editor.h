#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_H

#include "game/game_types.h"
#include "game/player_identifier_types.h"
#include "shell/shell.h"

// Functions defined in tu/interfacelibxbox/start_menu_game_editor.cpp.

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "multithreading/synchronized_value.h"
#include "saved_games/saved_game_variant_file_types.h"
#include <stddef.h>

class c_virtual_keyboard_task;
struct s_file_reference;

#pragma pack(push, 4)
struct start_menu_game_editor_saved_file
{
    signed char has_path;
    long type;
    s_player_identifier owner;
    char path[256];
};
struct s_start_menu_game_editor_saved_settings
{
    unsigned char setup_type;
    unsigned char map_variant_edited;
    long location;
    unsigned char map_variant_storage[0xe090];
    start_menu_game_editor_saved_file file;
};
#pragma pack(pop)
class c_start_menu_game_editor : public c_start_menu_pane_screen_widget
{
    friend class c_gui_map_subitem_selectable_item_datasource;
public:
    c_start_menu_game_editor(long name);
    virtual ~c_start_menu_game_editor() {}
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual void initialize_datasource(); // slot 35
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual void submenu_invoked(c_gui_list_widget *list);
protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return true; }

public:
    long m_operation;
    long m_save_state;
    long m_dialog_action;
    s_blffile_map_variant m_variant_file;
    e_controller_index m_operation_controller;
    c_virtual_keyboard_task *m_keyboard_task;
    wchar_t m_save_name[256];
    s_start_menu_game_editor_saved_settings m_saved_setup;
    long m_create_variant_task;
    long m_save_as_variant_task;
    long m_write_variant_task;
    c_synchronized_long m_write_succeeded;
    c_synchronized_long m_write_complete;
    c_synchronized_long m_content_item;
    long m_progress_message_start_time;
    friend class c_gui_screen_game_options;
    protected:
    bool load_progress_ui(e_controller_index controller_index, s_file_reference *reference);

    public:
    static __forceinline bool load_progress_ui_for_catalogue(c_start_menu_game_editor *catalogue,
        e_controller_index controller_index, s_file_reference *reference)
    {
        return catalogue->load_progress_ui(controller_index, reference);
    }
};

typedef char game_editor_saved_file_size[(sizeof(start_menu_game_editor_saved_file) == 0x110) ? 1 : -1];
typedef char game_editor_saved_file_type_offset[(offsetof(start_menu_game_editor_saved_file, type) == 4) ? 1 : -1];
typedef char game_editor_saved_settings_size[(sizeof(s_start_menu_game_editor_saved_settings) == 0xe1a8) ? 1 : -1];
typedef char game_editor_saved_settings_location_offset[
    (offsetof(s_start_menu_game_editor_saved_settings, location) == 4) ? 1 : -1];
typedef char game_editor_operation_offset[(sizeof(void *) != 4 || ((offsetof(c_start_menu_game_editor, m_operation) == 0x1b08))) ? 1 : -1];
typedef char game_editor_variant_file_offset[(sizeof(void *) != 4 || ((offsetof(c_start_menu_game_editor, m_variant_file) == 0x1b18))) ? 1 : -1];
typedef char game_editor_operation_controller_offset[
    (sizeof(void *) != 4 || ((offsetof(c_start_menu_game_editor, m_operation_controller) == 0xfd08))) ? 1 : -1];
typedef char game_editor_screen_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_game_editor) == 0x1e0d8))) ? 1 : -1];
typedef char game_editor_progress_message_time_offset[
    (sizeof(void *) != 4 || ((offsetof(c_start_menu_game_editor, m_progress_message_start_time) == 0x1e0d0))) ? 1 : -1];

extern "C" void start_menu_game_editor_begin_save_variant(c_start_menu_game_editor *editor, int controller_index);
extern "C" void start_menu_game_editor_finish_save_variant(c_start_menu_game_editor *screen);
extern "C" void start_menu_game_editor_finish_save_variant_as(c_start_menu_game_editor *screen);
extern "C" void start_menu_game_editor_request_save_variant_as(c_start_menu_game_editor *screen, e_controller_index controller_index);

class c_map_variant;

extern "C" void start_menu_game_editor_post_progress_message(c_start_menu_game_editor *editor, e_controller_index controller);
extern "C" void start_menu_game_editor_build_map_variant_file(c_start_menu_game_editor *screen,
    s_blffile_map_variant *file, const wchar_t *name, const wchar_t *description,
    e_map_id map_id, const c_map_variant *variant);


class c_controller_input_message;
extern "C" void start_menu_game_editor_request_save_variant(c_start_menu_game_editor *screen, const c_controller_input_message *message);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_EDITOR_START_MENU_GAME_EDITOR_H
