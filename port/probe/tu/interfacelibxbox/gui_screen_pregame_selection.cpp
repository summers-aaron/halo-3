// interfacelibxbox/gui_screen_pregame_selection.obj: from the records 0x1e99a0 0x4c0ee0 0x4c7778 0x4c7848 0x4c78d0 0x4c7920 0x4c79c8 0x4c7e78 0x4c7fb8 0x4c7fd0 0x4c8320 0x4c8798 0x4c8cf8 0x4c8e28 0x4c8e60 0x4c9008 0x4c90c8 0x4c91c0 0x4c93d0 0x4c98d0 0x4c9aa8 0x4c9ed0 0x4ca320 0x4ca378 0x4ca3c8 0x4ca5c0 0x4ca8d0 0x4caae8 0x4cab80 0x4cac38 0x4caca8 0x4d7500
#include "interface/gui_pregame_selection_transfer_storage.h"
#include "shell/utility_drive.h"
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x11401e8 g_controller_globals
// @address 0xf283c8 g_network_session_interface_local_users
#include "interface/gui_selected_item.h"
#include "interface/gui_selected_items_map.h"
#include "interface/gui_selected_items_hopper.h"
#include "interface/gui_selected_items_saved_film.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/gui_selected_items_level.h"
#include "saved_games/content_catalogue_device_selection.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/user_interface_utilities.h"
#include "interface/user_interface_clock.h"
#include "interface/c_gui_widget.h"
#include "math/real_math_types.h"
#include "interface/custom_map_image_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_animation_transform.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/user_interface_matchmaking.h"
#include "tag_files/files.h"
#include "cseries/async.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "multithreading/synchronized_value.h"
#include "networking/session/network_session_interface_types.h"
#include "saved_games/saved_game_files.h"
#include "saved_games/saved_game_file_category_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include <new>
#include "tag_files/file_reference.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/gui_selected_items.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#if !defined(_XBOX) || defined(__clang__)
#include "interface/gui_widget_navigation.h"
#include "interface/gui_widget_tree.h"
#endif
#include "interface/virtual_keyboard_task.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_selection.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "saved_games/saved_game_file_types.h"
struct s_hopper_information;


class c_font_cache;
template <long k_maximum_count> class c_static_wchar_string;
#include "saved_games/content_catalogue_types.h"
#include "interface/gui_screen_pregame_selection_dependencies.h"



#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"


#include "interface/gui_pregame_setup_manager_types.h"

extern "C" {

#include "interface/user_interface_device_selection.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "interface/c_gui_list_item_widget.h"
}



#include "interface/gui_screens/matchmaking/matchmaking_screen_literals.h"

#include "interface/hopper_information_types.h"


#include "text/utf8_conversion.h"

#include "interface/gui_selected_item_metadata.h"

#include "saved_games/saved_game_item_metadata.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/saved_game_files_operations.h"

#include "multithreading/compiler_barrier.h"
#include "cseries/async_block.h"
#include "saved_games/content_catalogue_lookup.h"
#include "interface/gui_simple_selected_item.h"
#include "interface/gui_selected_items_difficulty.h"
#include "interface/gui_game_variant_datasource_types.h"
#include "interface/gui_selected_items_game_variant.h"
#include "interface/gui_selected_items_network_mode.h"
#include "saved_games/game_content_types.h"
#include "interface/controller_inline.h"
#include "cseries/cseries.h"
#include "tag_files/string_ids.h"

// Dialog string ids (set 3) of the pregame selection screen.
enum
{
    _string_id_need_to_select_storage_device_to_save = 0x3001e,
    _string_id_pregame_selection_item_options = 0x30026,
    _string_id_pregame_selection_item_options_corrupt = 0x30027,
    _string_id_confirm_saved_game_delete = 0x30029,
};



__forceinline c_gui_level_category_datasource::c_gui_level_category_datasource() :
    c_gui_ordered_data()
{
    m_item_count = 0;
}
__forceinline c_gui_hopper_subitem_selectable_item_datasource::c_gui_hopper_subitem_selectable_item_datasource() :
    c_gui_ordered_data()
{
    m_item_count = 0;
}

// @address 0x1e99a0 pregame_selection_pack_record
extern "C" void pregame_selection_pack_record(s_saved_game_item_metadata *compact, const s_gui_selected_items_block *record)
{
    memset(compact, 0, sizeof(*compact));
    compact->unique_id = record->unique_id;
    ustrnzcpy_inline(compact->name, record->name, 16);
    wchar_string_to_utf8_string(record->description, compact->description, 128, 0);
    csstrnzcpy(compact->author, record->author, 16);
    compact->file_type = record->file_type;
    compact->author_is_online = record->author_is_online;
    compact->author_id = record->author_id;
    compact->size_in_bytes = record->size_in_bytes;
    compact->date = record->date;
    compact->length_seconds = record->length_seconds;
    compact->campaign_id = record->campaign_id;
    compact->map_id = record->map_id;
    compact->game_engine_type = record->game_engine_type;
    compact->campaign_difficulty = record->campaign_difficulty;
    compact->campaign_insertion_point = record->campaign_insertion_point;
    compact->campaign_survival_enabled = record->campaign_survival_enabled;
    compact->game_id = record->game_id;
}

extern "C" {
// @address 0x54368 ??_7c_gui_screen_pregame_selection@@6B@
}

// @address 0x4c7778 ??0c_gui_screen_pregame_selection@@QAA@J@Z
c_gui_screen_pregame_selection::c_gui_screen_pregame_selection(long name) :
    c_gui_screen_widget(name),
    m_selection_type(-1),
    m_selected_index(-1),
    m_current_category_datasource(0),
    m_pending_category(-1),
    m_pending_delete_task(-1),
    m_pending_metadata_update_task(-1),
    m_pending_copy_task(-1),
    m_copy_file_category(-1),
    m_sync_1aa8(0),
    m_sync_1aac(0),
    m_sync_1ab0(0),
    m_pending_save_task(-1),
    m_pending_save_category(-1),
    m_progress_ui_flags(0),
    m_progress_ui_request_time(0)
{
    memset(&m_file_reference, 0, sizeof(m_file_reference));
    m_virtual_keyboard_task = ::c_virtual_keyboard_task::get_instance(
        "..\\interface\\gui_screens\\pregame_lobby\\gui_screen_pregame_selection.cpp" /* __FILE__ */, 60,
        (e_controller_index)(long)k_no_controller, (wchar_t const *)(0), (wchar_t const *)(0), (wchar_t const *)(0), 256, 0);
    memset(m_keyboard_text, 0, sizeof(m_keyboard_text));
    m_pending_name_edit = 0;
    m_pending_description_edit = 0;
    m_content_type = -1;
    m_network_join_busy = 0;
}

// @address 0x4c7848 ?initialize@c_gui_screen_pregame_selection@@UAAXXZ
void c_gui_screen_pregame_selection::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<their-skill-minimum", this, parse_xml_hopper_maximum_party_size));
}

// @address 0x4c78d0 ?post_initialize@c_gui_screen_pregame_selection@@UAAXXZ
void c_gui_screen_pregame_selection::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    set_initial_focused_category();
    if (m_selection_type != 7) {
        gui_screen_widget_set_display_group(this, 1, 0x2e1);
    }
}

// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z
// @address 0x4c7920 ?dispose@c_gui_screen_pregame_selection@@UAAXXZ
void c_gui_screen_pregame_selection::dispose()
{
    c_gui_screen_widget::dispose();
    if (m_pending_delete_task != -1) {
        async_block_until_done(&m_sync_1aa8, simple_yield_function, false, true, 0, 0);
        m_pending_delete_task = -1;
    } else if (m_pending_metadata_update_task != -1) {
        async_block_until_done(&m_sync_1aa8, simple_yield_function, false, true, 0, 0);
        m_pending_metadata_update_task = -1;
    }
}

// @address 0x4c7e78 ?set_initial_focused_category@c_gui_screen_pregame_selection@@AAAXXZ
void c_gui_screen_pregame_selection::set_initial_focused_category()
{
    c_gui_data *datasource = 0;
    for (long index = 0; index < m_datasource_count; index++) {
        if (m_datasource[index]->get_name() == _string_id_category) {
            datasource = m_datasource[index];
            break;
        }
    }
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    if (list == 0 || datasource == 0) return;
    long category = g_pregame_setup_manager.get_selected_item_category_id((e_gui_selected_item_type)m_selection_type);
    long element_handle = datasource->get_first_element_handle();
    if (element_handle == -1) return;
    do {
        long element_category = -1;
        if (datasource->get_integer_value(element_handle, _string_id_category, &element_category) && element_category == category) {
            (this)->transfer_focus_to_list(list, element_handle, false, false);
            return;
        }
        element_handle = datasource->get_next_element_handle(element_handle);
    } while (element_handle != -1);
}

// @address 0x4c7fb8 ?reconstruct@c_gui_screen_pregame_selection@@UAAXXZ
void c_gui_screen_pregame_selection::reconstruct()
{
    m_current_category_datasource = 0;
    m_selected_index = -1;
    c_gui_screen_widget::reconstruct();
}

// @address 0x4c8e28 ?update_render_state@c_gui_screen_pregame_selection@@UAAXK@Z
void c_gui_screen_pregame_selection::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    shift_subitem_menu();
}

// @address 0x4c8e60 ?shift_subitem_menu@c_gui_screen_pregame_selection@@AAAXXZ
void c_gui_screen_pregame_selection::shift_subitem_menu()
{
    c_gui_list_widget *focus_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    c_gui_list_widget *item_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_subitem);
    c_gui_widget *first_group = get_child_widget(_gui_group_widget, _string_id_subitem);
    c_gui_widget *second_group = get_child_widget(_gui_group_widget, _string_id_subitem_edit);
    if (focus_list != 0 && item_list != 0 && first_group != 0 && second_group != 0) {
        long item_count = item_list->get_item_count(true);
        long indexed_count = item_list->get_item_count(false);
        if (item_count == 0)
            return;
        long category_row = focus_list->m_focused_item_index - focus_list->m_scroll_position;
        long max_steps = item_count - indexed_count;
        long steps = PIN(category_row - (indexed_count - 1) / 2, 0L, max_steps);
        if ((m_selection_type == _gui_selected_item_map || m_selection_type == _gui_selected_item_game_variant) && item_count >= 2) {
            // Slot origins are already in render coordinates. Child icon and text bounds can overhang them.
            c_gui_widget *first_item = gui_widget_find_child_by_name(item_list, _gui_list_item_widget);
            c_gui_widget *next_item = gui_widget_get_next_sibling_of_type(first_item, _gui_list_item_widget);
            real_rectangle2d first_bounds;
            real_rectangle2d next_bounds;
            first_item->get_current_bounds(&first_bounds);
            next_item->get_current_bounds(&next_bounds);
            const float row_spacing = next_bounds.y0 - first_bounds.y0;
            offset_widget_recursive(first_group, 0.0f, row_spacing * steps);
            offset_widget_recursive(second_group, 0.0f, row_spacing * (steps + indexed_count));
            return;
        }
        long step_size;
        {
            /* bounds is a short-lived temporary used only to derive the item height */
            real_rectangle2d bounds;
            item_list->get_child_list_item_widget_bounds(&bounds);
            step_size = (long)((bounds.y1 - bounds.y0) / (float)item_count);
        }
        offset_widget_recursive(first_group, 0.0f, (float)(step_size * steps));
        offset_widget_recursive(second_group, 0.0f, (float)(step_size * (steps + indexed_count)));
    }
}

// @address 0x4c8cf8 ?update_map_image@c_gui_screen_pregame_selection@@AAAXXZ
void c_gui_screen_pregame_selection::update_map_image()
{
    c_gui_bitmap_widget *bitmap = (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, _string_id_map_image);
    if (bitmap != 0) {
        c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, bitmap->get_value_override_list());
        c_gui_data *data = list != 0 ? (list)->get_data() : 0;
        long map_id = -1;
        if (data != 0) {
            long identifier = bitmap->get_value_identifier();
            if (data->get_integer_value((list)->get_focused_element_handle(), identifier, &map_id) && map_id != -1) {
                long image_type = 1;
                long value = -1;
                if (data->get_integer_value((list)->get_focused_element_handle(), _string_id_map_image_type, &value)) {
                    image_type = value;
                }
                ((c_gui_custom_bitmap_widget *)bitmap)->set_map_image((e_custom_map_image_type)image_type, (e_map_id)map_id, true);
            } else {
                memset(((c_gui_custom_bitmap_widget *)bitmap)->m_map_image, 0, sizeof(((c_gui_custom_bitmap_widget *)bitmap)->m_map_image));
            }
        } else {
            memset(((c_gui_custom_bitmap_widget *)bitmap)->m_map_image, 0, sizeof(((c_gui_custom_bitmap_widget *)bitmap)->m_map_image));
        }
    }
}

// 0x4c9008
// @address 0x4c9008 ?offset_widget_recursive@c_gui_screen_pregame_selection@@AAAXPAVc_gui_widget@@MM@Z
void c_gui_screen_pregame_selection::offset_widget_recursive(c_gui_widget *widget, float dx, float dy)
{
    s_animation_transform state = widget->m_animated_state;
    state.position.x = state.position.x + dx;
    state.position.y = state.position.y + dy;
    widget->set_full_animation_state(&state, false);
    c_gui_widget *child = widget->get_children();
    if (child != 0) {
        do {
            offset_widget_recursive(child, dx, dy);
            child = child->get_next();
        } while (child != 0);
    }
}

// @address 0x4c90c8 ?gui_screen_pregame_selection_load_chosen_screen@@YAXPAVc_gui_screen_pregame_selection@@JPAVc_gui_data@@JJ@Z
void gui_screen_pregame_selection_load_chosen_screen(
    c_gui_screen_pregame_selection *screen, long controller, c_gui_data *datasource, long element_handle, long value_name)
{
    long value;
    if (datasource != 0 && datasource->get_integer_value(element_handle, value_name, &value)) {
        if ((unsigned long)value >= 2) {
            long error_controller = screen->get_any_responding_controller();
            g_gui_error_manager.post_error_with_custom_message(0x20031, 0, 0, (e_controller_index)error_controller, false, true);
            ::user_interface_play_sound(_ui_sound_error, screen->m_core_definition.sound_override_index);
            return;
        }
        void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
        if (memory != 0) {
            long layer = screen->get_name();
            c_load_screen_message *message = new (memory) c_load_screen_message(_string_id_maximum_party_size, (e_controller_index)controller,
                screen->get_render_window(), layer);
            if (message != 0) {
                message->set_parent_screen_index(screen->get_screen_index());
                ::user_interface_messaging_post(message);
            }
        }
    }
}

// @address 0x4ca320 ?get_current_category_element_handle@c_gui_screen_pregame_selection@@AAAJXZ
long c_gui_screen_pregame_selection::get_current_category_element_handle()
{
    long element_handle = -1;
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    if (list != 0) {
        element_handle = (list)->get_focused_element_handle();
    }
    return element_handle;
}

// @address 0x4ca378 ?is_current_category_recent_items_list@c_gui_screen_pregame_selection@@AAA_NXZ
bool c_gui_screen_pregame_selection::is_current_category_recent_items_list()
{
    bool result = false;
    if ((unsigned int)(m_selection_type - 2) <= 1) {
        result = get_current_category_element_handle() == 0;
    }
    return result;
}

class c_content_catalogue;


// @address 0x4ca3c8 ?begin_deletion_of_selected_item@c_gui_screen_pregame_selection@@QAAXW4e_controller_index@@@Z
void c_gui_screen_pregame_selection::begin_deletion_of_selected_item(e_controller_index controller)
{
    c_gui_list_widget *list = (c_gui_list_widget *)this->get_child_widget(_gui_list_widget, _string_id_subitem);
    if (list != 0) {
        c_gui_data *data = (list)->get_data();
        if (data != 0) {
            const c_gui_selected_item *item = data->get_gui_selected_item((list)->get_focused_element_handle());
            if (item != 0 && item->get_location() == _gui_stored_item_location_saved_game_file) {
                if (content_catalogue_has_storage_device(&content_catalogues().catalogues[(controller * 0x9d0) / 0x9d0])) {
                    char path[256];
                    if (item->get_file_path(path, 256)) {
                        s_file_reference reference;
                        __lwsync();
                        _InterlockedExchange(&this->m_sync_1aa8.value, 0);
                        __lwsync();
                        _InterlockedExchange(&this->m_sync_1aac.value, 0);
                        __lwsync();
                        _InterlockedExchange(&this->m_sync_1ab0.value, 0);
                        long file = content_catalogue_find_item_by_file_reference(
                            &content_catalogues().catalogues[controller],
                            file_reference_create_from_path(&reference, path, false));
                        if (file != -1)
                            this->m_pending_delete_task = saved_game_files_delete_content_item_async(controller, file, &this->m_sync_1aac, &this->m_sync_1aa8);
                        if (this->m_pending_delete_task != -1) {
                            if ((unsigned long)this->m_progress_ui_flags == 0) {
                                unsigned long now = g_user_interface_globals.milliseconds.peek();
                                this->m_progress_ui_flags = now;
                            }
                            this->m_progress_title = _string_id_pregame_selection_deleting_title;
                            this->m_progress_message = _string_id_pregame_selection_deleting_message;
                        }
                    }
                } else {
                    user_interface_show_device_selection_dialog(controller, this->m_name, this->m_screen_index);
                }
            }
        }
    }
}

// @address 0x4ca8d0 ?begin_saved_game_file_save_changes@c_gui_screen_pregame_selection@@QAAXW4e_controller_index@@PB_W1@Z
void c_gui_screen_pregame_selection::begin_saved_game_file_save_changes(e_controller_index controller, const wchar_t *name, const wchar_t *description)
{
    if (this->m_pending_metadata_update_task == -1) {
        if (content_catalogue_has_storage_device(&content_catalogues().catalogues[(controller * 0x9d0) / 0x9d0])) {
            c_gui_list_widget *list = (c_gui_list_widget *)this->get_child_widget(_gui_list_widget, _string_id_subitem);
            if (list != 0) {
                c_gui_data *data = (list)->get_data();
                if (data != 0) {
                    const c_gui_selected_item *item = data->get_gui_selected_item((list)->get_focused_element_handle());
                    if (item != 0 && item->get_location() == _gui_stored_item_location_saved_game_file) {
                        char path[256];
                        if (item->get_file_path(path, 256)) {
                            s_file_reference reference;
                            __lwsync();
                            _InterlockedExchange(&this->m_sync_1aa8.value, 0);
                            __lwsync();
                            _InterlockedExchange(&this->m_sync_1aac.value, 0);
                            __lwsync();
                            _InterlockedExchange(&this->m_sync_1ab0.value, 0);
                            long file = content_catalogue_find_item_by_file_reference(
                                &content_catalogues().catalogues[controller],
                                file_reference_create_from_path(&reference, path, false));
                            if (file != -1) {
                                this->m_pending_metadata_update_task = saved_game_files_update_metadata_async(
                                    controller, file, name, description, &this->m_sync_1aac, &this->m_sync_1aa8);
                            }
                            if (this->m_pending_metadata_update_task != 0) {
                                if ((unsigned long)this->m_progress_ui_flags == 0) {
                                    unsigned long now = g_user_interface_globals.milliseconds.peek();
                                    this->m_progress_ui_flags = now;
                                }
                                this->m_progress_title = _string_id_updating_metadata_title;
                                this->m_progress_message = _string_id_updating_metadata_message;
                            }
                        }
                    }
                }
            }
        } else {
            user_interface_show_device_selection_dialog(controller, this->m_name, this->m_screen_index);
        }
    }
}

// @address 0x4ca5c0 ?begin_selected_item_text_edit@c_gui_screen_pregame_selection@@QAAXW4e_controller_index@@_N1@Z
void c_gui_screen_pregame_selection::begin_selected_item_text_edit(e_controller_index controller_index, bool edit_name, bool edit_description)
{
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_subitem);
    c_gui_data *data = (list)->get_data();
    if (data == 0) {
        return;
    }
    const c_gui_selected_item *item = data->get_gui_selected_item((list)->get_focused_element_handle());
    if (item == 0) {
        return;
    }
    if (item->get_enabled()) {
        return;
    }
    if (item->get_location() != _gui_stored_item_location_saved_game_file) {
        return;
    }
    if (asynchronous_operation_in_progress()) {
        return;
    }
    if (m_virtual_keyboard_task == 0 || m_virtual_keyboard_task->busy()) {
        return;
    }
    if (!content_catalogue_has_storage_device(&content_catalogues().catalogues[(controller_index * 0x9d0) / 0x9d0])) {
        user_interface_show_device_selection_dialog(controller_index, m_name, m_screen_index);
        return;
    }
    c_static_wchar_string<1024> title;
    long title_id = 0;
    c_static_wchar_string<1024> description;
    long description_id = 0;
    c_static_wchar_string<128> default_text;
    long maximum_input_characters = 0;
    if (edit_name) {
        title_id = _string_id_edit_title;
        description_id = _string_id_edit_title_message;
        default_text.set(item->get_title());
        maximum_input_characters = 16;
    } else if (edit_description) {
        title_id = _string_id_edit_description;
        description_id = _string_id_edit_description_message;
        default_text.set(item->get_description());
        maximum_input_characters = 128;
    }
    get_string_by_string_id(title_id, &title);
    get_string_by_string_id(description_id, &description);
    m_virtual_keyboard_task->set_controller_index(controller_index);
    m_virtual_keyboard_task->set_default_text(default_text.m_string);
    m_virtual_keyboard_task->set_title_text(title.m_string);
    m_virtual_keyboard_task->set_description_text(description.m_string);
    m_virtual_keyboard_task->set_maximum_input_characters(maximum_input_characters);
    m_virtual_keyboard_task->set_character_flags(0x4001);
    m_keyboard_text_length = 0;
    if (overlapped_task_start_internal(m_virtual_keyboard_task,
        "..\\interface\\gui_screens\\pregame_lobby\\gui_screen_pregame_selection.cpp" /* __FILE__ */, 0x663)) {
        if (edit_name) {
            m_pending_name_edit = true;
        } else if (edit_description) {
            m_pending_description_edit = true;
        }
    } else {
        m_virtual_keyboard_task->reset();
    }
}

static __forceinline unsigned long gui_screen_pregame_selection_clock_now()
{
    unsigned long milliseconds = g_user_interface_globals.milliseconds.peek();
    return milliseconds;
}

// @address 0x4caae8 ?load_progress_ui@c_gui_screen_pregame_selection@@QAAXXZ
void c_gui_screen_pregame_selection::load_progress_ui()
{
    long message_id;
    if (m_copy_file_category != 0) {
        if (m_copy_file_category != 2) {
            message_id = 0x20053;
        } else {
            message_id = 0x20054;
        }
    } else {
        message_id = 0x20052;
    }
    e_controller_index controller_index = get_any_responding_controller();
    g_gui_error_manager.post_error_with_custom_message(message_id, 0, 0, controller_index, false, true);
    dispose_progress_ui(true);
}

// @address 0x4cab80 ?dispose_progress_ui@c_gui_screen_pregame_selection@@QAAX_N@Z
void c_gui_screen_pregame_selection::dispose_progress_ui(bool force)
{
    c_gui_screen_widget *progress = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, _string_id_in_progress);
    m_progress_ui_flags = 0;
    if (progress != 0) {
        bool busy = progress->transitioning_out();
        if (!busy &&
            (force || m_progress_ui_request_time == 0 ||
                (unsigned long)(gui_screen_pregame_selection_clock_now() - m_progress_ui_request_time) >= 0xbb8))
        {
            m_progress_ui_request_time = 0;
            progress->transition_out_with_transition_type(false, _screen_transition_normal);
        }
    }
}

// @address 0x4cac38 ?restore_focus_to_selected_category@c_gui_screen_pregame_selection@@QAAXXZ
void c_gui_screen_pregame_selection::restore_focus_to_selected_category()
{
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    if (list != 0) {
        c_gui_data *data = (list)->get_data();
        if (data != 0) {
            set_current_category(data, get_current_category_element_handle());
        }
    }
}

// @address 0x4caca8 ?parse_xml_hopper_maximum_party_size@@YA_NPAXPA_WJ@Z
bool parse_xml_hopper_maximum_party_size(void *screen, wchar_t *buffer, long buffer_count)
{
    bool result = false;
    s_hopper_information information;
    if (::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &information)) {
        c_static_wchar_string<1024> maximum_party_size;
        maximum_party_size.copy_to(buffer, buffer_count);
        result = true;
    }
    return result;
}

// @address 0x4d7500 ?create_bitmap_widget@c_gui_screen_pregame_selection@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_screen_pregame_selection::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _string_id_map_image) {
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    }
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}



extern "C" {
// @address 0x56600 ??_7c_gui_map_category_datasource@@6B@
// @address 0x244198 ?set@?$c_static_wchar_string@$0DA@@@QAAXPB_W@Z
// @address 0x4f3930 ?get_column_names@c_gui_hopper_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
// @address 0x4c0ee0 ??_Gc_gui_hopper_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_hopper_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Gc_gui_level_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_level_category_datasource@@UAAPAXI@Z
// @address 0x56c90 ??_7c_gui_hopper_category_datasource@@6B@
// @address 0x56900 ??_7c_gui_level_category_datasource@@6B@
// @address 0x56710 ??_7c_gui_game_variant_category_datasource@@6B@
// @address 0x4c0ee0 ??_Ec_gui_game_variant_category_datasource@@UAAPAXI@Z
// @address 0x56820 ??_7c_gui_saved_film_category_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_saved_film_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_saved_film_category_datasource@@UAAPAXI@Z
// @address 0x56d00 ??_7c_gui_hopper_subitem_selectable_item_datasource@@6B@
// @address 0x56780 ??_7c_gui_game_variant_subitem_selectable_item_datasource@@6B@
}
struct s_item;





__forceinline c_gui_map_category_datasource::c_gui_map_category_datasource(const wchar_t *title, const wchar_t *description,
    const wchar_t *default_title, const wchar_t *default_description) :
    c_gui_ordered_data(),
    m_first_name(title), m_first_description(description),
    m_second_name(default_title), m_second_description(default_description)
{
    m_item_count = 0;
}

__forceinline c_gui_game_variant_category_datasource::c_gui_game_variant_category_datasource(
    const wchar_t *title, const wchar_t *description, const wchar_t *default_title, const wchar_t *default_description) :
    c_gui_ordered_data(),
    m_title(title), m_description(description),
    m_default_title(default_title), m_default_description(default_description)
{
    m_item_count = 0;
}

__forceinline c_gui_hopper_category_datasource::c_gui_hopper_category_datasource() : c_gui_ordered_data()
{
    m_item_count = 0;
}


static __forceinline void gui_screen_pregame_selection_track_delete(c_gui_data *object)
{
    object->~c_gui_data();
    ::user_interface_free((void const *)(object));
}

// @address 0x4c79c8 ?initialize_datasource@c_gui_screen_pregame_selection@@UAAXXZ
void c_gui_screen_pregame_selection::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *datasource = 0;
    switch (this->m_selection_type)
    {
    case 2:
    {
        c_static_wchar_string<1024> title;
        c_static_wchar_string<1024> description;
        c_static_wchar_string<1024> default_title;
        c_static_wchar_string<1024> default_description;
        this->get_string_by_string_id(_string_id_recent_maps, &title);
        this->get_string_by_string_id(_string_id_recent_maps_description, &description);
        this->get_string_by_string_id(_string_id_file_share_owner, &default_title);
        this->get_string_by_string_id(_string_id_file_share_owner_description, &default_description);
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_map_category_datasource(title.m_string, description.m_string,
            default_title.m_string, default_description.m_string);
        break;
    }
    case 4:
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_hopper_category_datasource();
        break;
    case 0:
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_level_category_datasource();
        break;
    case 1:
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_difficulty_category_datasource();
        break;
    case 3:
    {
        c_static_wchar_string<1024> title;
        c_static_wchar_string<1024> description;
        c_static_wchar_string<1024> default_title;
        c_static_wchar_string<1024> default_description;
        this->get_string_by_string_id(_string_id_recent_games, &title);
        this->get_string_by_string_id(_string_id_recent_games_description, &description);
        this->get_string_by_string_id(_string_id_file_share_owner, &default_title);
        this->get_string_by_string_id(_string_id_file_share_owner_description, &default_description);
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_game_variant_category_datasource(title.m_string, description.m_string,
            default_title.m_string, default_description.m_string);
        break;
    }
    case 5:
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_saved_film_category_datasource(this->m_screen_index);
        break;
    case 7:
        datasource = (void *)new(0, 0, _ui_allocation_marker_dummy) c_gui_network_mode_category_datasource(this);
        break;
    case 6:
    default:
        break;
    }
    if (datasource)
    {
        if (((c_gui_data *)datasource)->initialize(0xfe))
            this->m_datasource[this->m_datasource_count++] = (c_gui_data *)datasource;
        else
        {
            ((c_gui_data *)datasource)->~c_gui_data();
            ::user_interface_free((void const *)(datasource));
        }
    }
}

// @address 0x4c93d0 ?set_current_category@c_gui_screen_pregame_selection@@AAAXPAVc_gui_data@@J@Z
void c_gui_screen_pregame_selection::set_current_category(class c_gui_data * data, long element_handle)
{
    this->m_selected_index = element_handle;
    if (this->m_current_category_datasource)
    {
        for (long i = 0; i < this->m_datasource_count; i++)
        {
            if (this->m_datasource[i] == this->m_current_category_datasource)
            {
                for (long j = i; j < this->m_datasource_count - 1; j++)
                    this->m_datasource[j] = this->m_datasource[j + 1];
                this->m_datasource[--this->m_datasource_count] = 0;
                break;
            }
        }
        (this->m_current_category_datasource)->dispose();
        gui_screen_pregame_selection_track_delete(this->m_current_category_datasource);
        this->m_current_category_datasource = 0;
    }
    if (element_handle == -1)
        return;

    bool disabled = false;
    long disabled_value = -1;
    if (data->get_integer_value(element_handle, _string_id_special_status, &disabled_value) && disabled_value == 1)
        disabled = true;
    if (disabled)
        return;

    long category = -1;
    if (data->get_integer_value(element_handle, 0xfe, &category))
    {
        switch (this->m_selection_type)
        {
        case 2:
            if (!get_alpha_is_internal_beta())
            {
                c_gui_map_subitem_selectable_item_datasource *datasource =
                    new(0, 0, _ui_allocation_marker_dummy) c_gui_map_subitem_selectable_item_datasource();
                if (datasource)
                {
                    e_controller_index controller_index = this->get_any_responding_controller();
                    if (datasource->initialize(_string_id_subitem, (e_map_id)category, controller_index))
                        this->m_current_category_datasource = datasource;
                    else
                        gui_screen_pregame_selection_track_delete(datasource);
                }
            }
            break;
        case 4:
        {
            c_gui_hopper_subitem_selectable_item_datasource *datasource = new (0, 0, _ui_allocation_marker_dummy)
                c_gui_hopper_subitem_selectable_item_datasource();
            if (datasource)
            {
                if (datasource->initialize(_string_id_subitem, (e_hopper_category_id)category))
                    this->m_current_category_datasource = datasource;
                else
                    gui_screen_pregame_selection_track_delete(datasource);
            }
            break;
        }
        case 0:
        {
            c_gui_level_subitem_selectable_item_datasource *datasource =
                new(0, 0, _ui_allocation_marker_dummy) c_gui_level_subitem_selectable_item_datasource();
            if (datasource)
            {
                long controller_index = this->get_any_responding_controller();
                if (datasource->initialize_levels(controller_index, _string_id_subitem, category))
                    this->m_current_category_datasource = datasource;
                else
                    gui_screen_pregame_selection_track_delete(datasource);
            }
            break;
        }
        case 3:
        {
            c_gui_game_variant_subitem_selectable_item_datasource *datasource =
                new (0, 0, _ui_allocation_marker_dummy)
                c_gui_game_variant_subitem_selectable_item_datasource();
            if (datasource)
            {
                e_controller_index controller_index = this->get_any_responding_controller();
                if (datasource->initialize(_string_id_subitem, (e_game_engine_type)category, controller_index))
                    this->m_current_category_datasource = datasource;
                else
                    gui_screen_pregame_selection_track_delete(datasource);
            }
            break;
        }
        case 5:
        {
            c_gui_data *datasource = (c_gui_data *)user_interface_malloc_tracked(sizeof(c_gui_saved_film_subitem_datasource), 0, 0);
            if (datasource)
            {
                long controller_index = this->get_any_responding_controller();
                datasource = (c_gui_data *)new (datasource) c_gui_saved_film_subitem_datasource(controller_index, category);
            }
            if (datasource)
            {
                if (((c_gui_data *)datasource)->initialize(_string_id_subitem))
                    this->m_current_category_datasource = datasource;
                else
                    gui_screen_pregame_selection_track_delete(datasource);
            }
            break;
        }
        case 7:
            if (category != 2)
            {
                c_gui_network_mode_subitem_selectable_item_datasource *datasource = new (0, 0, _ui_allocation_marker_dummy)
                    c_gui_network_mode_subitem_selectable_item_datasource(this);
                if (datasource)
                {
                    if (datasource->initialize(_string_id_subitem, (e_network_mode_category)category))
                        this->m_current_category_datasource = datasource;
                    else
                        gui_screen_pregame_selection_track_delete(datasource);
                }
            }
            break;
        }
    }
    if (this->m_current_category_datasource)
    {
        this->m_datasource[this->m_datasource_count++] =
            this->m_current_category_datasource;
        (this->m_current_category_datasource)->update();
    }
}

class c_content_catalogue;

// @address 0x7e8928 g_pregame_selection_file_transfer_storage

// @address 0x4c91c0 gui_screen_pregame_selection_enqueue_file_transfer
extern "C" void gui_screen_pregame_selection_enqueue_file_transfer(c_gui_screen_pregame_selection *screen,
    e_controller_index transfer_controller, const c_gui_selected_item *item)
{
    int copy_task_id;
    int file_category;
    unsigned int progress_ui_flags;
    int variant_progress_start_time;
    int map_progress_start_time;
    int film_progress_start_time;
    struct s_file_copy_desc {
        s_saved_game_file_copy_task task;
#ifdef HALO3_64BIT
        int rest[128];
#else
        int rest[64];
#endif
    } desc;
    int compact_metadata[64];
    s_file_reference source_file;
    int item_path[64];
    int item_metadata[96];
    screen->m_copy_file_category = -1;
    if (item->get_file_path((char *)item_path, 0x100)) {
        file_reference_create_from_path(&source_file, (const char *)item_path, false);
        item->get_metadata((s_gui_selected_items_block *)item_metadata);
        pregame_selection_pack_record((s_saved_game_item_metadata *)compact_metadata, (const s_gui_selected_items_block *)item_metadata);
        memset(&desc, 0, sizeof(desc));
        desc.task.state = 0;
        desc.task.controller_index = transfer_controller;
        desc.task.source = (s_file_reference *)&g_pregame_selection_file_transfer_storage[1];
        memcpy(&g_pregame_selection_file_transfer_storage[1], &source_file, offsetof(s_file_reference, handle));
        desc.task.destination = (s_file_reference *)&g_pregame_selection_file_transfer_storage[273];
        desc.task.metadata = (s_saved_game_item_metadata_packed *)&g_pregame_selection_file_transfer_storage[552];
        s_saved_game_item_metadata *destination_metadata =
            (s_saved_game_item_metadata *)&g_pregame_selection_file_transfer_storage[552];
        s_saved_game_item_metadata *source_metadata = (s_saved_game_item_metadata *)compact_metadata;
        *destination_metadata = *source_metadata;
        desc.task.content_item_index = -1;
        desc.task.file_size = 0;
        desc.task.completion = &screen->m_sync_1aac;
        copy_task_id = async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&desc, _async_category_saved_games,
            (e_async_completion (*)(s_async_task *))saved_game_files_copy_file_callback, &screen->m_sync_1aa8, true);
        screen->m_pending_copy_task = copy_task_id;
        if (!((copy_task_id == -1))) {
            file_category = s_saved_game_item_metadata::get_file_type_from_saved_game_type(
                (e_saved_game_file_type)((s_saved_game_item_metadata *)compact_metadata)->file_type);
            progress_ui_flags = screen->m_progress_ui_flags;
            screen->m_copy_file_category = file_category;
            if (!((file_category == 0))) {
                if (!((file_category == 2))) {
                    if (!((progress_ui_flags != 0))) {
                        variant_progress_start_time = g_user_interface_globals.milliseconds.peek();
                        screen->m_progress_ui_flags = variant_progress_start_time;
                    }
                    screen->m_progress_title = _string_id_game_variant_save_progress_title;
                    screen->m_progress_message = _string_id_game_variant_save_progress_message;
                    return;
                }
                if (!((progress_ui_flags != 0))) {
                    map_progress_start_time = g_user_interface_globals.milliseconds.peek();
                    screen->m_progress_ui_flags = map_progress_start_time;
                }
                screen->m_progress_title = _string_id_map_variant_save_progress_title;
                screen->m_progress_message = _string_id_map_variant_save_progress_message;
                return;
            }
            if (!((progress_ui_flags != 0))) {
                film_progress_start_time = g_user_interface_globals.milliseconds.peek();
                screen->m_progress_ui_flags = film_progress_start_time;
            }
            screen->m_progress_title = _string_id_film_save_progress_title;
            screen->m_progress_message = _string_id_film_save_progress_message;
            return;
        }
        screen->load_progress_ui();
        return;
    }
    ::user_interface_play_sound(_ui_sound_error, screen->m_core_definition.sound_override_index);
    return;
}

// @address 0x4c98d0 ?handle_dialog_result@c_gui_screen_pregame_selection@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_pregame_selection::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    switch (dialog_result_message->get_dialog_name())
    {
    case _string_id_pregame_selection_item_options:
        handled = true;
        switch (dialog_result_message->get_dialog_result())
        {
        case _gui_dialog_choice_first:
            begin_selected_item_text_edit(dialog_result_message->get_controller(), true, false);
            break;
        case _gui_dialog_choice_second:
            begin_selected_item_text_edit(dialog_result_message->get_controller(), false, true);
            break;
        case 2:
            if (m_pending_delete_task == -1)
            {
                void *allocation = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (allocation)
                {
                    long layer = get_name();
                    c_load_dialog_screen_message *message = new (allocation) c_load_dialog_screen_message(
                        dialog_result_message->get_controller(), get_render_window(), layer, _string_id_confirm_saved_game_delete, layer);
                    if (message)
                    {
                        message->set_parent_screen_index(m_screen_index);
                        user_interface_messaging_post(message);
                    }
                }
            }
            break;
        }
        break;
    case _string_id_pregame_selection_item_options_corrupt:
        handled = true;
        if (dialog_result_message->get_dialog_result() == _gui_dialog_choice_first && m_pending_delete_task == -1)
        {
            void *allocation = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
            if (allocation)
            {
                long layer = get_name();
                c_load_dialog_screen_message *message = new (allocation) c_load_dialog_screen_message(
                    dialog_result_message->get_controller(), get_render_window(), layer, _string_id_confirm_saved_game_delete, layer);
                if (message)
                {
                    message->set_parent_screen_index(m_screen_index);
                    user_interface_messaging_post(message);
                }
            }
        }
        break;
    case _string_id_confirm_saved_game_delete:
        handled = true;
        if (dialog_result_message->get_dialog_result() == _gui_dialog_choice_first)
            begin_deletion_of_selected_item(dialog_result_message->get_controller());
        break;
    }
    if (!handled)
    {
        // The screen widget base handles the storage device selection dialog.
        bool base_handled = false;
        if (dialog_result_message->get_dialog_name() == _string_id_need_to_select_storage_device_to_save)
        {
            if (dialog_result_message->get_dialog_result() == _gui_dialog_choice_first)
                content_catalogue_display_device_selection_guide_interface(dialog_result_message->get_controller());
            base_handled = true;
        }
        handled = base_handled;
    }
    return handled;
}

extern "C" {
}

// @address 0x4c7fd0 ?update@c_gui_screen_pregame_selection@@UAAXK@Z
void c_gui_screen_pregame_selection::update(unsigned long current_milliseconds)
{
    c_gui_widget *list = this->get_child_widget(_gui_list_widget, _string_id_category);
    c_gui_widget *other = this->get_child_widget(_gui_list_widget, _string_id_subitem);
    c_gui_widget *focused = this->get_focused_widget();
    int header = -1;
    int help = -1;
    c_gui_data *data = 0;
    if (list)
        data = ((c_gui_list_widget *)list)->get_data();
    switch (this->m_selection_type) {
    case 2: header = _string_id_select_map; help = _string_id_subitem_map; break;
    case 0: header = _string_id_select_level; help = _string_id_subitem_level; break;
    case 1: header = _string_id_select_difficulty; help = _string_id_subitem_difficulty; break;
    case 3: header = _string_id_select_game; help = _string_id_subitem_game; break;
    case 4: header = _string_id_select_hopper; help = _string_id_subitem_hopper; break;
    case 5: header = _string_id_select_film; help = _string_id_subitem_film; break;
    case 7: header = _string_id_select_network_mode; help = _string_id_subitem_network_mode; break;
    }
    this->set_text_widget_string_id(0x29d, header);
    this->set_text_widget_string_id(_string_id_subitem_header, help);
    if (list && other && data) {
        if (((c_gui_list_widget *)list)->get_focused_element_handle() != this->m_selected_index) {
            this->set_current_category(data, this->get_current_category_element_handle());
            ((c_gui_list_widget *)other)->set_focused_item_index(0, 0);
        }
    }
    if (list && other && data) {
        {
            bool list_focused = focused && focused->get_type() == 5 && gui_widget_get_parent_list_widget(focused) == list;
            bool focus = list_focused;
            c_gui_data *source = data;
            if (!focus)
                source = this->m_current_category_datasource;
            if (source) {
                long value = -1;
                if (source->get_string_id_value(0, _string_id_image_name, &value))
                    gui_screen_widget_set_display_group(this, 0, value);
            }
            list->set_enabled(list_focused);
            other->set_enabled(!focus);
            this->update_map_image();
            gui_screen_pregame_selection_update_list_ui(this);
        }
    }
    gui_screen_pregame_selection_update_pending_tasks(this);
    if ((unsigned long)this->m_progress_ui_flags && gui_screen_pregame_selection_clock_now() > (unsigned long)this->m_progress_ui_flags + 500) {
        this->m_progress_ui_flags = 0;
        this->m_progress_ui_request_time = gui_screen_pregame_selection_clock_now();
        void *storage = ::user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), (char const *)(0), 0);
        if (storage) {
            e_controller_index controller = this->get_any_responding_controller();
            c_load_in_progress_screen_message *message = new (storage)
                c_load_in_progress_screen_message(controller, this->get_render_window(), this->m_progress_title, this->m_progress_message);
            if (message) {
                message->set_parent_screen_index(this->m_screen_index);
                ::user_interface_messaging_post(message);
            }
        }
    }
    c_gui_screen_widget::update(current_milliseconds);
}

static __forceinline void gui_screen_pregame_selection_set_child_visible(c_gui_widget *widget, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child != 0)
        child->set_visible(visible);
}

// @address 0x4c8320 gui_screen_pregame_selection_update_list_ui
extern "C" void gui_screen_pregame_selection_update_list_ui(c_gui_screen_pregame_selection *screen)
{
    c_gui_list_widget *category_list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _string_id_category);
    c_gui_widget *category_item = category_list != 0 ? gui_list_widget_find_item_by_element_handle(category_list,
        category_list->get_focused_element_handle()) : 0;
    c_gui_list_widget *subitem_list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _string_id_subitem);
    if (screen->m_current_category_datasource != 0 && subitem_list != 0 && category_list != 0)
    {
        bool show_no_items_text = !(screen->m_current_category_datasource)->is_busy() && (screen->m_current_category_datasource)->get_current_item_count() == 0;
        gui_screen_pregame_selection_set_child_visible(screen, _gui_text_widget, _string_id_list_empty, show_no_items_text);
        if (category_item != 0)
            gui_screen_pregame_selection_set_child_visible(category_item, _gui_bitmap_widget, _string_id_list_subarrow, !show_no_items_text);
        if (show_no_items_text)
        {
            long string_id = -1;
            long no_hdd_string_id = -1;
            long default_string_id = -1;
            bool no_hdd = !g_utility_drive_mounted;
            if ((screen->m_current_category_datasource)->get_string_id_value(-1, _string_id_no_hdd, &string_id))
                no_hdd_string_id = string_id;
            if ((screen->m_current_category_datasource)->get_string_id_value(-1, _string_id_no_items, &string_id))
                default_string_id = string_id;
            string_id = no_hdd ? no_hdd_string_id : default_string_id;
            if (string_id != -1)
                screen->set_text_widget_string_id(_string_id_list_empty, string_id);
        }
        gui_screen_pregame_selection_set_child_visible(screen, _gui_group_widget, _string_id_subitem_edit, false);
        if (subitem_list->within_focus_chain())
        {
            long button_key_string_id = -1;
            const c_gui_selected_item *selected_item =
                (screen->m_current_category_datasource)->get_gui_selected_item(subitem_list->get_focused_element_handle());
            if (selected_item != 0)
            {
                if (selected_item->get_location() == _gui_stored_item_location_built_in)
                {
                    long string_id = -1;
                    if ((screen->m_current_category_datasource)->get_string_id_value(-1, _string_id_item_built_in, &string_id))
                        button_key_string_id = string_id;
                }
                else if (selected_item->get_location() == _gui_stored_item_location_autosave_queue)
                {
                    long value = 0;
                    if ((screen->m_current_category_datasource)->get_integer_value(subitem_list->get_focused_element_handle(), _string_id_item_saved, &value))
                    {
                        long string_id = -1;
                        if ((screen->m_current_category_datasource)->get_string_id_value(-1,
                            value != 0 ? _string_id_item_saved : _string_id_item_can_be_saved, &string_id))
                            button_key_string_id = string_id;
                    }
                }
                else if (selected_item->get_location() == _gui_stored_item_location_saved_game_file)
                {
                    long string_id = -1;
                    if ((screen->m_current_category_datasource)->get_string_id_value(-1, _string_id_item_options, &string_id))
                        button_key_string_id = string_id;
                }
            }
            else
            {
                screen->transfer_focus_to_list(category_list, category_list->get_focused_element_handle(), false, true);
            }
            if (button_key_string_id != -1)
            {
                screen->set_text_widget_string_id(_string_id_item_options, button_key_string_id);
                gui_screen_pregame_selection_set_child_visible(screen, _gui_group_widget, _string_id_subitem_edit, true);
            }
        }
    }
    else
    {
        gui_screen_pregame_selection_set_child_visible(screen, _gui_text_widget, _string_id_list_empty, false);
        gui_screen_pregame_selection_set_child_visible(screen, _gui_group_widget, _string_id_subitem_edit, false);
        if (category_item != 0)
            gui_screen_pregame_selection_set_child_visible(category_item, _gui_bitmap_widget, _string_id_list_subarrow, false);
    }
}

#define PREGAME_SELECTION_MIN(a, b) ((a) < (b) ? (a) : (b))
#define PREGAME_SELECTION_MAX(a, b) ((a) > (b) ? (a) : (b))

// @address 0x4c8798 gui_screen_pregame_selection_update_pending_tasks
extern "C" void gui_screen_pregame_selection_update_pending_tasks(c_gui_screen_pregame_selection *screen)
{
    bool blocked = false;
    if (network_join_is_busy() && (screen->m_selection_type == 2 || screen->m_selection_type == 3 || screen->m_selection_type == 5))
        blocked = true;
    if (blocked) {
        if (!screen->m_network_join_busy) {
            if ((unsigned long)screen->m_progress_ui_flags == 0)
                screen->m_progress_ui_flags = gui_screen_pregame_selection_clock_now();
            screen->m_progress_title = _string_id_pregame_selection_enumeration_in_progress_title;
            screen->m_progress_message = _string_id_pregame_selection_enumeration_in_progress_message;
            screen->m_network_join_busy = network_join_is_busy();
            return;
        }
    } else if (screen->m_pending_delete_task != -1) {
        if (screen->m_sync_1aa8.peek() == 1) {
            c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _string_id_subitem);
            if (list) {
                c_gui_data *data = (list)->get_data();
                if (data) {
                    long index = PREGAME_SELECTION_MIN(PREGAME_SELECTION_MAX(list->m_focused_item_index, 0), data->get_current_item_count() - 2);
                    screen->m_pending_delete_task = -1;
                    screen->restore_focus_to_selected_category();
                    (screen)
                        ->transfer_focus_to_list(list,
                            (list)->get_element_handle_from_list_item_index(index), true, false);
                    screen->m_network_join_busy = network_join_is_busy();
                    return;
                }
            }
        }
    } else if (screen->m_pending_metadata_update_task != -1) {
        if (screen->m_sync_1aa8.peek() == 1) {
            c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _string_id_subitem);
            long element_handle = list ? (list)->get_focused_element_handle() : -1;
            screen->m_pending_metadata_update_task = -1;
            screen->restore_focus_to_selected_category();
            if (list && element_handle != -1) {
                (screen)->transfer_focus_to_list(list, element_handle, false, false);
                screen->m_network_join_busy = network_join_is_busy();
                return;
            }
        }
    } else if (screen->m_pending_name_edit) {
        c_virtual_keyboard_task *task = screen->m_virtual_keyboard_task;
        bool finished = false;
        if (task) {
            if (!task->busy()) {
                finished = true;
                if (screen->m_virtual_keyboard_task->get_task_state() == _overlapped_task_state_success) {
                    c_content_catalogue *catalogue = &content_catalogues().catalogues[screen->get_any_responding_controller()];
                    wchar_t name[256];
                    ((c_static_wchar_string<256> *)name)->set(screen->m_virtual_keyboard_task->get_keyboard_results());
                    content_catalogue_trim_whitespace((unsigned short *)name);
                    if (content_catalogue_has_storage_device(catalogue)) {
                        e_controller_index controller = screen->get_any_responding_controller();
                        if (content_catalogue_is_item_name_valid(catalogue, controller, name, screen->m_content_type)) {
                            ustrnzcpy(screen->m_keyboard_text, name, 256);
                            screen->begin_saved_game_file_save_changes(screen->m_virtual_keyboard_task->get_controller_index(), screen->m_keyboard_text, 0);
                        }
                    }
                }
                screen->m_virtual_keyboard_task->reset();
                screen->m_virtual_keyboard_task->set_controller_index(k_no_controller);
            }
        } else {
            e_controller_index controller_index = screen->get_any_responding_controller();
            g_gui_error_manager.post_error_with_custom_message(0x20057, 0, 0, controller_index, false, true);
            finished = true;
        }
        if (finished) {
            screen->m_pending_name_edit = false;
            screen->m_network_join_busy = network_join_is_busy();
            return;
        }
    } else if (screen->m_pending_description_edit) {
        c_virtual_keyboard_task *task = screen->m_virtual_keyboard_task;
        bool finished = false;
        if (task) {
            if (!task->busy()) {
                finished = true;
                if (screen->m_virtual_keyboard_task->get_task_state() == _overlapped_task_state_success) {
                    ustrnzcpy_inline(screen->m_keyboard_text, task->get_keyboard_results(), 256);
                    screen->begin_saved_game_file_save_changes(screen->m_virtual_keyboard_task->get_controller_index(), 0, screen->m_keyboard_text);
                }
                screen->m_virtual_keyboard_task->reset();
                screen->m_virtual_keyboard_task->set_controller_index(k_no_controller);
            }
        } else {
            e_controller_index controller_index = screen->get_any_responding_controller();
            g_gui_error_manager.post_error_with_custom_message(0x20057, 0, 0, controller_index, false, true);
            finished = true;
        }
        if (finished) {
            screen->m_pending_description_edit = false;
            screen->m_network_join_busy = network_join_is_busy();
            return;
        }
    } else if (screen->m_pending_copy_task != -1) {
        if (screen->m_sync_1aa8.peek() == 1) {
            if (screen->m_sync_1aac.peek() == 0)
                screen->load_progress_ui();
            screen->m_pending_copy_task = -1;
            screen->m_network_join_busy = network_join_is_busy();
            return;
        }
    } else {
        screen->dispose_progress_ui(false);
    }
    screen->m_network_join_busy = network_join_is_busy();
}

// @address 0x4c9aa8 ?handle_controller_input_message@c_gui_screen_pregame_selection@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_selection::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_list_widget *category_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    c_gui_list_widget *item_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_subitem);
    c_gui_widget *focused = get_focused_widget();
    bool category_focused = focused && focused->get_type() == 5 &&
        gui_widget_get_parent_list_widget(focused) == (c_gui_widget *)category_list;
    if (message->get_event_type() == 13) {
        switch (message->get_component()) {
        case 1:
            if (category_focused)
                transition_out_with_transition_type(1, _screen_transition_normal);
            else if (category_list)
                (this)
                    ->transfer_focus_to_list(category_list,
                        (category_list)->get_focused_element_handle(), false, true);
            handled = true;
            break;
        case 2:
            if (::user_interface_squad_in_or_after_countdown()) {
                ::user_interface_squad_stop_countdown_timer(message->get_controller(), -1, false);
                handled = true;
            } else if (m_selection_type == 7) {
                c_gui_data *data = (category_list)->get_data();
                long element_handle = (category_list)->get_focused_element_handle();
                if (data && element_handle != -1)
                    gui_screen_pregame_selection_load_chosen_screen(
                        this, message->get_controller(), data, element_handle, ((c_gui_widget *)category_list)->m_name);
                handled = true;
            } else if (!category_focused) {
                c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_subitem);
                c_gui_data *data = (list)->get_data();
                if (data) {
                    const c_gui_selected_item *item = data->get_gui_selected_item((list)->get_focused_element_handle());
                    if (item) {
                        s_gui_selected_items_block metadata;
                        s_saved_game_item_metadata dialog_compact;
                        s_saved_game_item_metadata file_compact;
                        switch (item->get_location()) {
                        case 1: {
                            long dialog_name = item->get_enabled()
                                ? _string_id_pregame_selection_item_options_corrupt
                                : _string_id_pregame_selection_item_options;
                            item->get_metadata(&metadata);
                            pregame_selection_pack_record(&dialog_compact, &metadata);
                            m_content_type = saved_game_file_type_to_game_content_type((e_saved_game_file_type)dialog_compact.file_type);
                            e_controller_index controller = message->get_controller();
                            void *storage = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                            if (storage) {
                                long layer = this->get_name();
                                c_load_dialog_screen_message *dialog = new (storage)
                                    c_load_dialog_screen_message(controller, get_render_window(), layer, dialog_name, layer);
                                if (dialog) {
                                    dialog->set_parent_screen_index(m_screen_index);
                                    ::user_interface_messaging_post(dialog);
                                }
                            }
                            handled = true;
                            break;
                        }
                        case 2: {
                            long element_handle = (list)->get_focused_element_handle();
                            c_gui_data *pregame_data = m_current_category_datasource;
                            long value = 0;
                            if (pregame_data->get_integer_value(element_handle, _string_id_item_saved, &value) && value == 0) {
                                item->get_metadata(&metadata);
                                pregame_selection_pack_record(&file_compact, &metadata);
                                if (saved_game_file_type_to_game_content_type((e_saved_game_file_type)file_compact.file_type) != _game_content_type_none) {
                                    gui_screen_pregame_selection_enqueue_file_transfer(this, message->get_controller(), item);
                                    handled = true;
                                }
                            }
                            break;
                        }
                        }
                    }
                }
            }
            break;
        }
    } else if (message->get_event_type() == 2 && !category_focused && category_list) {
        (this)
            ->transfer_focus_to_list(category_list,
                (category_list)->get_focused_element_handle(), false, true);
        handled = true;
    }
    else if (message->get_event_type() == 4 && category_focused && category_list && (category_list)->get_data() &&
        get_current_category_element_handle() != -1)
    {
        if (item_list && (item_list)->get_data() && (item_list)->get_data()->get_current_item_count())
            (this)
                ->transfer_focus_to_list(
                    item_list, (item_list)->get_focused_element_handle(), true, false);
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

static __forceinline bool gui_screen_pregame_selection_base_handle_list_item_chosen()
{
    return false;
}

static __forceinline c_gui_pregame_setup_manager *gui_screen_pregame_selection_setup_manager_get()
{
    return &g_pregame_setup_manager;
}

static __forceinline unsigned __int64 gui_screen_pregame_selection_controller_get_player_xuid(const c_controller_interface *controller)
{
    unsigned __int64 xuid = 0;
    if (controller->user_index >= 0 && controller->user_index < 4)
        xuid = g_network_session_interface_local_users[controller->user_index].xuid;
    return xuid;
}

// @address 0x4c9ed0 ?handle_list_item_chosen@c_gui_screen_pregame_selection@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_pregame_selection::handle_list_item_chosen(
    const c_controller_input_message *message, long p2, c_gui_list_item_widget *list_item_widget, c_gui_data *gui_data)
{
    bool handled = false;
    if (p2 == _string_id_category) {
        long category = get_current_category_element_handle();
        long chosen = list_item_widget->get_element_handle();
        if (chosen != category) {
            c_gui_widget *category_list = get_child_widget(_gui_list_widget, _string_id_category);
            if (category_list != 0) {
                set_current_category(gui_data, chosen);
                category = chosen;
                ((c_gui_screen_widget *)(this))->transfer_focus_to_list((c_gui_list_widget *)(category_list), chosen, true, true);
            }
        }
        if (category != -1) {
            c_gui_widget *item_list = get_child_widget(_gui_list_widget, _string_id_subitem);
            if (item_list != 0) {
                if (((c_gui_list_widget *)(item_list))->get_data() != 0 &&
                    ((c_gui_list_widget *)(item_list))->get_data()->get_current_item_count() > 0) {
                    ((c_gui_screen_widget *)(this))
                        ->transfer_focus_to_list((c_gui_list_widget *)(item_list),
                            ((c_gui_list_widget *)(item_list))->get_focused_element_handle(), true, false);
                    handled = true;
                } else {
                    long disabled_value = -1;
                    if (gui_data->get_integer_value(category, _string_id_special_status, &disabled_value) && disabled_value == 1) {
                        c_controller_interface *controller = controller_get(message->get_controller());
                        s_player_identifier identifier;
                        ((const c_controller_interface *)controller)->get_player_identifier(&identifier);
                        unsigned __int64 xuid = gui_screen_pregame_selection_controller_get_player_xuid(controller);
                        c_start_menu_screen_widget::load_start_menu_for_file_share(
                            get_any_responding_controller(), &identifier, &xuid, 0, -1, true);
                        handled = true;
                    }
                    if (!handled) {
                        switch (m_selection_type) {
                        case 1: {
                            const c_gui_selected_item *item = gui_data->get_gui_selected_item(list_item_widget->get_element_handle());
                            if (item != 0) {
                                long screen_index = m_screen_index;
                                e_controller_index controller_index = message->get_controller();
                                gui_screen_pregame_selection_setup_manager_get()->set_selected_item(
                                    controller_index, item, is_current_category_recent_items_list(), screen_index);
                                handled = true;
                            }
                            break;
                        }
                        case 2: {
                            if (get_alpha_is_internal_beta()) {
                                const c_gui_selected_item *item = gui_data->get_gui_selected_item(list_item_widget->get_element_handle());
                                if (item != 0) {
                                    long screen_index = m_screen_index;
                                    e_controller_index controller_index = message->get_controller();
                                    gui_screen_pregame_selection_setup_manager_get()->set_selected_item(
                                        controller_index, item, is_current_category_recent_items_list(), screen_index);
                                    handled = true;
                                }
                            }
                        }
                        // fall through
                        case 7: {
                            const c_gui_selected_item *item = gui_data->get_gui_selected_item(list_item_widget->get_element_handle());
                            if (item != 0) {
                                long screen_index = m_screen_index;
                                e_controller_index controller_index = message->get_controller();
                                gui_screen_pregame_selection_setup_manager_get()->set_selected_item(
                                    controller_index, item, is_current_category_recent_items_list(), screen_index);
                                handled = true;
                            }
                            break;
                        }
                        }
                    }
                }
            }
        }
    } else if (p2 == _string_id_subitem) {
        const c_gui_selected_item *item = gui_data->get_gui_selected_item(list_item_widget->get_element_handle());
        if (item != 0) {
            if (item->get_enabled()) {
                void *storage = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (storage != 0) {
                    long layer = this->get_name();
                    e_controller_index controller_index = get_any_responding_controller();
                    c_load_dialog_screen_message *dialog = new (storage)
                        c_load_dialog_screen_message(controller_index, get_render_window(), layer, _string_id_pregame_selection_item_options_corrupt, layer);
                    if (dialog != 0) {
                        dialog->set_parent_screen_index(m_screen_index);
                        user_interface_messaging_post((c_message const *)(dialog));
                    }
                }
            } else {
                long screen_index = m_screen_index;
                e_controller_index controller_index = message->get_controller();
                gui_screen_pregame_selection_setup_manager_get()->set_selected_item(
                    controller_index, item, is_current_category_recent_items_list(), screen_index);
            }
            handled = true;
        }
    }
    if (!handled)
        handled = gui_screen_pregame_selection_base_handle_list_item_chosen();
    return handled;
}
