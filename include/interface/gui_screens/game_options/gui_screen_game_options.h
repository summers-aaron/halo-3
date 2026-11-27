#ifndef INTERFACE_GUI_SCREENS_GAME_OPTIONS_GUI_SCREEN_GAME_OPTIONS_H
#define INTERFACE_GUI_SCREENS_GAME_OPTIONS_GUI_SCREEN_GAME_OPTIONS_H

#include "main/game_preferences_game_setup.h"

#include "interface/user_interface_messages.h"
#include "game/game_variant.h"
#include "interface/gui_screens/game_options/game_engine_category_types.h"

struct s_game_options_history_stack_node
{
    long category;
    long parent_category;
    long item;
};

class c_load_game_options_screen_message : public c_load_screen_message
{
public:
    c_load_game_options_screen_message(
        long screen_name,
        e_controller_index controller_index,
        e_window_index window_index,
        long layered_position,
        const c_game_variant *original_variant,
        const c_game_variant *edited_variant,
        e_game_engine_category category,
        e_game_engine_category subcategory,
        long item,
        s_game_options_history_stack_node *history,
        long history_depth);
    virtual void apply_initial_state(c_gui_screen_widget *widget) const;

    c_game_variant m_original_variant;
    c_game_variant m_edited_variant;
    long m_category;
    long m_subcategory;
    long m_item;
    s_game_options_history_stack_node m_history[16];
    long m_history_depth;
    long m_layered_position;
};

// String ids of the columns, lists and dialogs the game-options screen answers
// (mainmenu string table: value, name, description, game_options, spinner, elipsis,
// variant_save_options_dirty, variant_save_options_nonwriteable,
// pregame_selection_item_options, confirm_saved_game_delete,
// file_share_confirm_delete_from_share, save_game_settings_name,
// save_game_settings_description, game_variant_save_progress_title,
// game_variant_save_progress_message, button_key_game_options_party_leader,
// button_key_game_options_non_party_leader,
// campaign_changing_setting_will_delete_saved_game).
enum e_game_options_string_id
{
    _game_options_string_value = 0x10a,
    _game_options_string_name = 0x29c,
    _game_options_string_description = 0x29e,
    _game_options_string_game_options = 0x10384,
    _game_options_string_spinner = 0x10006,
    _game_options_string_elipsis = 0x10385,
    _game_options_string_variant_save_options_dirty = 0x30023,
    _game_options_string_variant_save_options_nonwriteable = 0x30024,
    _game_options_string_pregame_selection_item_options = 0x30025,
    _game_options_string_confirm_saved_game_delete = 0x30028,
    _game_options_string_file_share_confirm_delete_from_share = 0x3002a,
    _game_options_string_save_game_settings_name = 0x10388,
    _game_options_string_save_game_settings_description = 0x10389,
    _game_options_string_game_variant_save_progress_title = 0x100dd,
    _game_options_string_game_variant_save_progress_message = 0x100de,
    _game_options_string_button_key_game_options_party_leader = 0x10062,
    _game_options_string_button_key_game_options_non_party_leader = 0x10063,
    _game_options_string_campaign_changing_setting_will_delete_saved_game = 0x3001e,
};

// Functions defined in tu/interfacelibxbox/gui_screen_game_options.cpp.

class c_gui_screen_game_options;
class c_game_engine_base_variant;

extern "C" void gui_screen_game_options_set_variant_name(c_game_engine_base_variant *variant, const wchar_t *source);
extern "C" bool gui_screen_game_options_can_back_out(c_gui_screen_widget *screen);
bool parse_current_selected_item_nugget(void *context, long value_name, wchar_t *output, long capacity);
bool parse_current_selected_value(void *context, wchar_t *output, long capacity);
bool parse_current_selected_name(void *context, wchar_t *output, long capacity);

extern "C" const long k_game_engine_type_string_ids[];

class c_gui_widget;

#include "interface/gui_selected_items_game_variant.h"

#include "interface/c_gui_screen_widget.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "multithreading/synchronized_value.h"
#include "interface/c_player_profile.h"

class c_virtual_keyboard_task;

class c_gui_screen_game_options : public c_gui_screen_widget
{
public:
    c_gui_screen_game_options(long name);
    virtual ~c_gui_screen_game_options() {}
    virtual void initialize(void);
    virtual void initialize_datasource(void);
    virtual void post_initialize(void);
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *item, c_gui_data *data);
protected:
    void begin_save_current_edited_variant(e_controller_index controller);
    void begin_save_as_current_edited_variant(e_controller_index controller_index);
    bool delete_variant_file(const wchar_t *path);
    void propagate_changes_to_variant_without_saving(e_controller_index controller);
    void propagate_changes_from_newly_created_game_variant_file(e_controller_index controller, long content_item_index);
public:
    void load_progress_ui(e_controller_index controller_index);
    void update_save_operation();
    void update_save_as_new_operation();
    void update_rename_operation();
    void apply_initial_state(
        const c_game_variant *original_variant,
        const c_game_variant *edited_variant,
        e_game_engine_category category,
        e_game_engine_category parent_category,
        long selected_item,
        const s_game_options_history_stack_node *history,
        long depth,
        long item);

    c_game_variant m_original_variant;
    c_game_variant m_edited_variant;
    long m_editing_parameter;
    s_game_options_history_stack_node m_history[16];
    long m_history_depth;
    long m_history_item;
    long m_operation;
    long m_dialog_action;
    long m_dialog_result;
    s_blffile_game_variant m_variant_file;
    e_controller_index m_asynchronous_operation_controller;
    c_virtual_keyboard_task *m_keyboard_task;
    wchar_t m_variant_name[256];
    c_string_verify_task m_string_verify_task;
    wchar_t m_variant_title[128];
    wchar_t m_variant_description[128];
    struct s_saved_setup_snapshot {
        s_gui_game_setup_storage::s_multiplayer_settings settings;
        char following_settings[0x110];
    } m_saved_setup;
    long m_create_request;
    long m_write_request;
    long m_delete_request;
    long m_save_request;
    c_synchronized_long m_variant_success;
    c_synchronized_long m_variant_done;
    c_synchronized_long m_variant_content_item;
    c_synchronized_long m_variant_progress;

private:
    void setup_game_variant_for_write(s_blffile_game_variant *blf, const wchar_t *name, const c_game_variant *variant, bool use_string_table);
protected:
  bool back_out_from_current_editing_category(enum e_controller_index controller_index);
public:
  virtual bool handle_controller_input_message(class c_controller_input_message const *p0);
};

typedef char game_options_offset_m_variant_file[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_game_options, m_variant_file) == 0x2028))) ? 1 : -1];
typedef char game_options_offset_m_string_verify_task[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_game_options, m_string_verify_task) == 0x25ec))) ? 1 : -1];
typedef char game_options_offset_m_variant_title[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_game_options, m_variant_title) == 0x2e30))) ? 1 : -1];
typedef char game_options_offset_m_create_request[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_game_options, m_create_request) == 0x11548))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_GAME_OPTIONS_GUI_SCREEN_GAME_OPTIONS_H
