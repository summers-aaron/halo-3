// interfacelibxbox/gui_screen_campaign_select_difficulty.obj: from the records 0x46f570 0x4c3388 0x4c33c8 0x4c34f0 0x4c35b0 0x4c3660 0x4c3870
#include "interface/user_interface_clock.h"
#include "interface/user_interface_allocation.h"
#include "interface/gui_screen_campaign_select_difficulty_dependencies.h"
#include "main/game_preferences_types.h"
#include "interface/user_interface_dialog_globals.h"
#include "shell/shell.h"
#include <stddef.h>
#include "interface/c_player_profile.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "interface/gui_pregame_setup_manager.h"
#include "bink/bink_playback.h"
#include "saved_games/saved_game_files.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_difficulty.h"
#include "saved_games/content_catalogue.h"
#include "interface/user_interface_error_manager.h"
#include "interface/look_sensitivity_types.h"
#include "interface/controller_inline.h"
#include "tag_files/string_ids.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_selected_items_difficulty.h"
#include <string.h>
#include "interface/controller_types.h"
#include <new>
// @flags /TP /O2 /Oc
class c_gui_selected_item;
// @address 0x4c3388 ?initialize@c_gui_screen_campaign_select_difficulty@@UAAXXZ

class c_gui_difficulty_category_datasource;

// ---- record 0x4c34f0 ----

// @address 0x4c3388 ?initialize@c_gui_screen_campaign_select_difficulty@@UAAXXZ
void c_gui_screen_campaign_select_difficulty::initialize()
{
    c_gui_screen_widget::initialize();
    set_initial_focus(626, m_initial_difficulty);
}

// @address 0x4c34f0 ?initialize_datasource@c_gui_screen_campaign_select_difficulty@@UAAXXZ
void c_gui_screen_campaign_select_difficulty::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    c_gui_difficulty_category_datasource *datasource = new (static_cast<const char *>(0), 0, _ui_allocation_marker_dummy) c_gui_difficulty_category_datasource;
    if (datasource) {
        if (datasource->initialize(626))
            add_datasource(datasource);
        else
            ui_track_delete(datasource);
    }
}

// ---- records 0x4c33c8, 0x4c3660 ----

// @address 0x4c33c8 ?update@c_gui_screen_campaign_select_difficulty@@UAAXK@Z
void c_gui_screen_campaign_select_difficulty::update(unsigned long current_milliseconds)
{
    c_gui_screen_widget::update(current_milliseconds);
    c_gui_data *data = find_data(626, 0);
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, 626);
    c_gui_bitmap_widget *bitmap = (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, 0x1032d);
    {
        c_gui_list_widget *focused_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, 626);
        if (focused_list) {
            long datasource = focused_list->get_datasource_name();
            set_text_widget_text_from_datasource(670, datasource, focused_list->get_focused_element_handle(), 670);
        }
    }
    if (data && bitmap) {
        long frame = -1;
        if (data->get_integer_value(list->get_focused_element_handle(), 0x10327, &frame))
            bitmap->set_sprite_frame(frame);
    }
}

#define GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_GET_LAST_SETUP(setup) do { \
    bool blocked = false; \
    gui_screen_campaign_select_difficulty_check_blocked(&blocked); \
    if (blocked) return; \
    const s_gui_game_setup_storage *source = global_preferences_get_game_setup_storage(); \
    memcpy(&(setup), source, sizeof(setup)); \
} while (0)
#define GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_START_GAME_WHEN_READY(controller_index) do { \
    s_user_interface_networking_globals *state = &g_user_interface_networking_globals; \
    state->start_pending = true; \
    state->start_controller = controller_index; \
    state->start_state = 1; \
} while (0)

// @address 0x46f570 gui_screen_campaign_select_difficulty_check_blocked
extern "C" void gui_screen_campaign_select_difficulty_check_blocked(bool *blocked)
{
    if (blocked)
        *blocked = false;
    if (!g_user_interface_globals.error_posted && bink_is_idle_and_drive_empty()) {
        g_gui_error_manager.post_error_with_custom_message(0x20087, 0, 0, k_any_controller, false, true);
        g_user_interface_globals.error_posted = true;
        if (blocked)
            *blocked = true;
    }
}

// @address 0x4c3660 ?difficulty_chosen_immediate@c_gui_screen_campaign_select_difficulty@@AAAXW4e_controller_index@@@Z
void c_gui_screen_campaign_select_difficulty::difficulty_chosen_immediate(e_controller_index controller)
{
    c_gui_data *data = find_data(626, 0);
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, 626);
    if (data && list) {
        const c_gui_difficulty_selected_item *item = (const c_gui_difficulty_selected_item *)data->get_gui_selected_item(list->get_focused_element_handle());
        if (item) {
            s_gui_game_setup_storage setup;
            s_gui_game_setup_storage::s_campaign_settings &campaign_settings = setup.campaign_settings;
            long difficulty = item->m_difficulty;
            c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
            if (profile && difficulty >= 0)
                profile->set_look_sensitivity((e_look_sensitivity)difficulty, true);
            switch (m_mode) {
            case 0:
                GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_GET_LAST_SETUP(setup);
                campaign_settings.difficulty_level = difficulty;
                campaign_settings.campaign_id = (e_campaign_id)(1);
                campaign_settings.map_id = (e_map_id)(3005);
                campaign_settings.valid = true;
                global_preferences_set_last_game_setup(&setup);
                if (gui_pregame_setup_manager_initialize_squad_for_mode(0))
                    GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_START_GAME_WHEN_READY(controller);
                break;
            case 1:
                GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_GET_LAST_SETUP(setup);
                campaign_settings.difficulty_level = difficulty;
                campaign_settings.campaign_id = (e_campaign_id)(m_campaign);
                campaign_settings.map_id = (e_map_id)(m_map);
                campaign_settings.valid = true;
                global_preferences_set_last_game_setup(&setup);
                if (gui_pregame_setup_manager_initialize_squad_for_mode(0))
                    GUI_SCREEN_CAMPAIGN_SELECT_DIFFICULTY_START_GAME_WHEN_READY(controller);
                break;
            case 2:
                g_pregame_setup_manager.set_selected_item(controller, (c_gui_selected_item const *)item, false, -1);
                transition_out_with_transition_type(0, _screen_transition_normal);
                break;
            }
        }
    }
}

static __forceinline bool gui_screen_campaign_select_difficulty_handle_device_selection(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->get_dialog_name() == 0x3001e) {
        if (message->get_dialog_result() == 0)
            content_catalogue_display_device_selection_guide_interface(message->get_controller());
        handled = true;
    }
    return handled;
}

#ifdef HALO3_64BIT
extern "C" bool user_interface_networking_has_matching_saved_game(e_controller_index fallback_controller);
extern "C" void user_interface_networking_delete_squad_saved_games(e_controller_index fallback_controller);
#endif

// @address 0x4c35b0 ?handle_dialog_result@c_gui_screen_campaign_select_difficulty@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_campaign_select_difficulty::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->get_dialog_name() == 0x3001f) {
        if (message->get_dialog_result() == 0) {
#ifdef HALO3_64BIT
            user_interface_networking_delete_squad_saved_games(message->get_controller());
#else
            saved_game_files_delete_personal_storage(message->get_controller());
#endif
            difficulty_chosen_immediate(message->get_controller());
        }
        handled = true;
    }
    if (!handled)
        handled = gui_screen_campaign_select_difficulty_handle_device_selection(message);
    return handled;
}

// ---- record 0x4c3870 (CLEAN RECORD shape, accepted callee names) ----

// @address 0x4c3870 ?handle_list_item_chosen@c_gui_screen_campaign_select_difficulty@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_campaign_select_difficulty::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool choice_handled = false;
    if (list_name == _string_id_difficulty)
    {
#ifdef HALO3_64BIT
        bool show_overwrite_dialog = (long)m_mode == 2
            && user_interface_squad_exists()
            && user_interface_networking_has_matching_saved_game(message->get_controller());
#else
        bool has_personal_storage_item = false;
        if (saved_game_files_get_personal_storage_content_item(message->get_controller(), false) != -1)
            has_personal_storage_item = true;
        bool show_overwrite_dialog = has_personal_storage_item && user_interface_squad_get_player_count() == 1;
#endif
        if (show_overwrite_dialog != 0)
        {
            void *dialog_storage = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
            if (dialog_storage != 0)
            {
                long dialog_layer = m_name;
                long message_controller = message->get_controller();
                c_load_dialog_screen_message *dialog_message = new (dialog_storage)
                    c_load_dialog_screen_message((e_controller_index)message_controller, get_render_window(), dialog_layer, 0x3001f, dialog_layer);
                if (dialog_message != 0)
                {
                    dialog_message->set_parent_screen_index(get_screen_index());
                    ::user_interface_messaging_post(dialog_message);
                }
            }
        }
        else
        {
            difficulty_chosen_immediate(message->get_controller());
        }
        choice_handled = true;
    }
    // match: 0x4c3870: preserves boolean normalization before returning the handled result.
    if (!choice_handled)
        choice_handled = false;
    return choice_handled;
}

typedef char campaign_difficulty_join_error_offset[(offsetof(s_user_interface_networking_globals, last_join_error) == 0xc) ? 1 : -1];
typedef char campaign_difficulty_networking_size[(sizeof(s_user_interface_networking_globals) == 0x90) ? 1 : -1];
typedef char campaign_difficulty_start_request_offset[(offsetof(s_user_interface_networking_globals, start_state) == 0x14) ? 1 : -1];
typedef char campaign_difficulty_start_state_size[(sizeof(s_user_interface_networking_globals) == 0x90) ? 1 : -1];
