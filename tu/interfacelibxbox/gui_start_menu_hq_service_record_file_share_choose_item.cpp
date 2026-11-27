// interfacelibxbox/gui_start_menu_hq_service_record_file_share_choose_item.obj: from the records 0x4b2608 0x4b27f8 0x4b2800 0x4b2810 0x4b29c0 0x4b2a68 0x4b2ba8 0x4b2cd0 0x4b2d48 0x4b2e50 0x4b2ed0 0x4b3348 0x4b35e8 0x4b36e0 0x4b39a8 0x4b3d20 0x4b3f70 0x4b3fe0 0x4b4050
#include <xtl.h>
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_catalogue_iteration.h"
#include "networking/online/file_share_slot_types.h"
#include "saved_games/saved_game_item_metadata.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
#include "interface/gui_file_share_choose_item_dependencies.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "saved_games/game_content_types.h"
#include <string.h>
#include <stddef.h>
#include "memory/static_arrays.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "tag_files/files.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_category.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_item.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_item_selected.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_animation_transform.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/c_gui_text_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/online/online_files.h"
#include "networking/online/online_file_work_item_types.h"
#include "interface/user_interface_allocation.h"
#include "cseries/cseries.h"
#include "interface/user_interface_utilities.h"
#include "interface/gui_data.h"
#include "cseries/language_types.h"
#include "cseries/language.h"
//  0x74b540 g_current_language
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_widget.h"
#include "cseries/async.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
class c_message;
// @address 0x4b2800 ?get_current_item_count_internal@c_gui_content_item_datasource@@UAAJXZ
// @address 0x4b2810 ?get_column_names@c_gui_content_item_datasource@@UAAXQAJPAJ@Z
// @address 0x4b2ba8 ?initialize@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAXXZ
// @address 0x4b2cd0 ?dispose@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAXXZ
// @address 0x4b2d48 ?initialize_datasource@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAXXZ
// @address 0x4b2e50 ?create_bitmap_widget@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
// @address 0x4b3d20 ?update_prepare_to_upload@c_gui_start_menu_hq_service_record_file_share_choose_item@@AAAXXZ
// @address 0x4b3f70 parse_quota_used
// @address 0x4b3fe0 parse_quota_total
// @address 0x3b7708 ?enqueue_for_transfer@c_online_file_manager@@QAAXW4e_queued_work_item_type@@PBUs_online_file_description@@PBD2_K3J_N@Z
// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
// @address 0x484370 ?user_interface_get_bytes_size_string@@YA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x4b4050 ?set_wchar@?$c_static_string@$0BAA@@@QAAXPB_W@Z
// @address 0x284b30 content_catalogue_item_metadata_matches
// @address 0x4b2588 ?gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text@@YA_NPBVc_gui_start_menu_hq_service_record_file_share_choose_category@@PA_WK@Z
// @address 0x4b36e0 ?handle_dialog_result@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4e6438 ??0c_gui_bitmap_widget@@QAA@XZ
// @address 0x491eb0 ??0c_gui_custom_bitmap_widget@@QAA@XZ
// @address 0x483428 ??0c_load_in_progress_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJ@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
// @address 0x1aed20 ?file_reference_get_name@@YAPADPBUs_file_reference@@KPADJ@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z
// @address 0x20e7e8 ?overlapped_task_terminate@@YAXPAVc_overlapped_task@@@Z
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x1eba98 ?set_char@?$c_static_wchar_string@$0EAA@@@QAAXPBD@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z
// @address 0x39ed28 ?set_default_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39edb8 ?set_title_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39ee48 ?set_description_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ

#include "cseries/crt_xdk.h"

template<long k_maximum_count> class c_static_wchar_string;

#include "tag_files/file_reference.h"
#include "tag_files/file_last_modification_date.h"

#include "saved_games/saved_game_item_metadata.h"
#include <new>
#include "cseries/async_xoverlapped_types.h"
#include "networking/online/virtual_keyboard_task.h"
#include "interface/file_share_enumeration_task_types.h"
#include "interface/c_player_profile.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "saved_games/content_catalogue_device_selection.h"
#include "saved_games/content_catalogue_externs.h"
#include "interface/gui_selected_item.h"
#include <ppcintrinsics.h>
#include "saved_games/content_catalogue_lookup.h"
#include "cseries/async_helpers.h"
#include "interface/gui_widget_hierarchy.h"
#include "saved_games/saved_game_file_types.h"
#include "saved_games/content_item_status_types.h"
#include "cseries/xenon_platform_api.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/file_share_category_dependencies.h"
// @address 0x8a1ead g_utility_drive_mounted

struct s_saved_game_metadata_update_task
{
    union {
        struct {
            e_controller_index controller_index;
            long content_item_index;
            const wchar_t *name;
            const wchar_t *description;
            c_synchronized_long *completion;
            c_synchronized_long *done;
        };
        s_async_task task;
    };
};


typedef char file_share_manager_queue_offset[(offsetof(c_online_file_manager, m_refresh_queue) == 8) ? 1 : -1];
typedef char file_share_manager_refresh_count_offset[
    (sizeof(void *) != 4 || ((offsetof(c_online_file_manager, m_active_transfer_tracker) +
    offsetof(c_online_files_active_transfer_tracker, m_refresh_count) == 21140))) ? 1 : -1];
typedef char file_share_manager_status_dirty_offset[
    (sizeof(void *) != 4 || ((offsetof(c_online_file_manager, m_active_transfer_tracker) +
    offsetof(c_online_files_active_transfer_tracker, m_status_dirty) == 21144))) ? 1 : -1];

long c_gui_content_item_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

bool c_gui_content_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        if (value_name == _start_menu_string_filename)
        {
            char file_name[0x100];
            value->set_char(file_reference_get_name(&m_items[element_handle].file_reference, 13, file_name, 0x100));
            result = true;
        }
        if (!result)
            result = m_items[element_handle].metadata.get_metadata_text(value_name, value);
    }
    return result;
}

// @address 0x4b2a68 ?get_integer_value@c_gui_content_item_datasource@@UAA_NJJPAJ@Z
bool c_gui_content_item_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        if (value_name == _start_menu_string_type)
        {
            *value = s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)m_items[element_handle].metadata.file_type);
            result = 1;
        }
        else
        {
            result = saved_game_files_try_and_get_metadata_property(&m_items[element_handle].metadata, value_name, value);
        }
    }
    return result;
}

void c_gui_content_item_datasource::update()
{
    update_enumeration();
}

void c_gui_content_item_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _start_menu_string_name;
    column_names[(*column_count)++] = _file_share_item_string_description;
    column_names[(*column_count)++] = _start_menu_string_filename;
    column_names[(*column_count)++] = _file_share_item_string_size;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_1_label;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_2_label;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_3_label;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_4_label;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_1;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_2;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_3;
    column_names[(*column_count)++] = _start_menu_string_metadata_line_4;
    column_names[(*column_count)++] = _start_menu_string_type;
    column_names[(*column_count)++] = _start_menu_string_game_engine_index;
    column_names[(*column_count)++] = _start_menu_string_map_id;
}

class c_gui_start_menu_hq_service_record_file_share_choose_category;

class c_controller_input_message;

class c_online_file_manager;

class c_gui_start_menu_hq_service_record_file_share_choose_item;

void c_gui_start_menu_hq_service_record_file_share_choose_item::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<quota-used", this, (bool (*)(void *, wchar_t *, long))parse_quota_used));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<quota-total", this, (bool (*)(void *, wchar_t *, long))parse_quota_total));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<quota-remaining", this,
        (bool (*)(void *, wchar_t *, long))gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text));
}

void c_gui_start_menu_hq_service_record_file_share_choose_item::dispose()
{
    c_gui_screen_widget::dispose();
    if (m_task.busy())
    {
        overlapped_task_terminate(&m_task);
        overlapped_task_block_until_finished(&m_task);
    }
}

// @address 0x51880 ??_7c_gui_content_item_datasource@@6B@

// 0x4b2d48
void c_gui_start_menu_hq_service_record_file_share_choose_item::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    c_gui_content_item_datasource *child = (c_gui_content_item_datasource *)user_interface_malloc_tracked(sizeof(c_gui_content_item_datasource), 0, 0);
    if (child)
    {
        long player = get_any_responding_controller();
        child = new (child) c_gui_content_item_datasource((e_controller_index)player, (e_saved_game_file_type)m_file_type);
        if (child->initialize(_start_menu_string_display_name))
        {
            int count = m_datasource_count;
            m_datasource = child;
            c_gui_screen_widget::m_datasource[count] = (c_gui_data *)child;
            m_datasource_count++;
        }
        else
        {
            child->~c_gui_content_item_datasource();
            user_interface_free(child);
        }
    }
}

c_gui_bitmap_widget *c_gui_start_menu_hq_service_record_file_share_choose_item::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _start_menu_string_game_variant)
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}

void c_gui_start_menu_hq_service_record_file_share_choose_item::update_prepare_to_upload()
{
    if (m_pending_action != 1)
        return;

    c_gui_screen_widget *child = static_cast<c_gui_screen_widget *>(get_child_widget(_gui_screen_widget, _start_menu_string_in_progress));
    if (m_task.get_task_state() == 4)
    {
        if (!m_task.string_verify_succeeded(0))
        {
            e_controller_index controller = get_any_responding_controller();
            g_gui_error_manager.post_error_with_custom_message(0x20041, 0, 0, controller, false, true);
        }
        else if (!m_task.string_verify_succeeded(1))
        {
            e_controller_index controller = get_any_responding_controller();
            g_gui_error_manager.post_error_with_custom_message(0x20040, 0, 0, controller, false, true);
        }
        else
        {
            c_online_file_manager::get()->enqueue_for_transfer(_queued_work_item_type_upload, &m_description, m_file_name.get_string(), 0,
                m_owner_xuid, m_target_xuid, m_a, false);
            transition_out_with_transition_type(0, _screen_transition_normal);
            if (get_parent_screen() != 0 && get_parent_screen()->m_name == _start_menu_string_start_menu_hq_service_record_file_share_choose_category)
                get_parent_screen()->transition_out_with_transition_type(0, _screen_transition_normal);
        }
        if (child != 0)
            child->transition_out_with_transition_type(0, _screen_transition_normal);
        m_pending_action = 0;
    }
    else if (m_task.get_task_state() == 5)
    {
        e_controller_index controller = get_any_responding_controller();
        g_gui_error_manager.post_error_with_custom_message(0x20042, 0, 0, controller, false, true);
        if (child != 0)
            child->transition_out_with_transition_type(0, _screen_transition_normal);
        m_pending_action = 0;
    }
    else if (child == 0)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
        c_load_in_progress_screen_message *message = memory
            ? new (memory) c_load_in_progress_screen_message(k_any_controller, _console_window,
                _start_menu_string_preparing_to_upload_title, _start_menu_string_preparing_to_upload_message)
            : 0;
        if (message != 0)
        {
            message->set_parent_screen_index(get_screen_index());
            ::user_interface_messaging_post(message);
        }
    }
}

// 0x4b36e0 handles the upload-rename and device-selection dialog results.
bool c_gui_start_menu_hq_service_record_file_share_choose_item::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _start_menu_string_file_share_upload_rename)
    {
        if (m_pending_action == 0)
        {
            c_static_wchar_string<1024> title;
            c_static_wchar_string<1024> description;
            c_static_wchar_string<1024> default_text;
            long item_index = 0;
            long maximum_length;
            long pending_state;
            handled = true;
            m_selected_item_index = -1;
            do
            {
                if ((unsigned long)message->m_dialog_result >= 1)
                {
                    if ((unsigned long)message->m_dialog_result != 1)
                        break;
                    if (!m_datasource->get_text_value(m_element_handle, _file_share_item_string_description, &default_text)
                        || !m_datasource->get_integer_value(m_element_handle, _start_menu_string_type, &item_index)
                        || !get_string_by_string_id(_start_menu_string_edit_file_description, &title)
                        || !get_string_by_string_id(_start_menu_string_edit_file_description_description, &description))
                        break;
                    maximum_length = 128;
                    pending_state = 3;
                }
                else
                {
                    if (!m_datasource->get_text_value(m_element_handle, _start_menu_string_name, &default_text)
                        || !m_datasource->get_integer_value(m_element_handle, _start_menu_string_type, &item_index)
                        || !get_string_by_string_id(_start_menu_string_rename_file, &title)
                        || !get_string_by_string_id(_start_menu_string_rename_file_description, &description))
                        break;
                    maximum_length = 16;
                    pending_state = 2;
                }
                m_selected_item_index = item_index;
                m_keyboard->m_controller_index = get_any_responding_controller();
                m_keyboard->set_default_text(default_text.m_string);
                m_keyboard->set_title_text(title.m_string);
                m_keyboard->set_description_text(description.m_string);
                m_keyboard->m_maximum_input_characters = maximum_length;
                m_keyboard->m_character_flags = 0x4001;
                if (overlapped_task_start_internal(m_keyboard,
                        "..\\interface\\gui_screens\\start_menu\\panes\\hq_service_record_file_share\\"
                        "gui_start_menu_hq_service_record_file_share_choose_item.cpp", 0x21a))
                    m_pending_action = pending_state;
                else
                    m_keyboard->reset();
            } while (false);
        }
    }
    bool result;
    if (!handled)
    {
        result = false;
        if (message->m_dialog_name == _start_menu_string_device_selection)
        {
            if (message->m_dialog_result == 0)
                content_catalogue_display_device_selection_guide_interface(message->m_controller);
            return true;
        }
    }
    else
        result = handled;
    return result;
}

bool parse_quota_used(void *context, wchar_t *buffer, unsigned long buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    c_gui_start_menu_hq_service_record_file_share_choose_item *screen =
        (c_gui_start_menu_hq_service_record_file_share_choose_item *)context;
    long quota_used = screen->get_quota_used();
    if (quota_used != -1 && user_interface_get_bytes_size_string(quota_used, &text))
    {
        text.copy_to(buffer, buffer_length);
        success = true;
    }
    return success;
}

bool parse_quota_total(void *context, wchar_t *buffer, unsigned long buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    c_gui_start_menu_hq_service_record_file_share_choose_item *screen =
        (c_gui_start_menu_hq_service_record_file_share_choose_item *)context;
    long quota_total = screen->get_quota_total();
    if (quota_total != -1 && user_interface_get_bytes_size_string(quota_total, &text))
    {
        text.copy_to(buffer, buffer_length);
        success = true;
    }
    return success;
}

template<> void c_static_string<256>::set_wchar(const wchar_t *string)
{
    const wchar_t *source = string;
    char *destination = m_string;
    long remaining = 256;
    while (remaining > 0) {
        unsigned short character = *source;
        if (remaining == 1)
            *destination = 0;
        else if (character <= 127)
            *destination = (char)character;
        else
            *destination = '?';
        source++;
        destination++;
        remaining--;
        if (character == 0)
            break;
    }
}

// @address 0x4b2608 ?update_enumeration@c_gui_content_item_datasource@@AAAXXZ
void c_gui_content_item_datasource::update_enumeration()
{
    if (m_enumeration_complete)
        return;
    c_content_catalogue *catalogue = &content_catalogues().catalogues[m_controller];
    if (!content_catalogue_has_storage_device(catalogue) || !catalogue->m_flags.test(1))
        return;
    for (long item_index = content_catalogue_get_first_item_index(catalogue);
        item_index != -1 && m_item_count < NUMBEROF(m_items);
        item_index = content_catalogue_get_next_item_index(catalogue, item_index))
    {
        s_saved_game_item_metadata_packed metadata;
        if (content_catalogue_is_item_corrupt(catalogue, item_index)
            || !content_catalogue_item_metadata_matches(catalogue, item_index, &metadata)
            || !saved_game_metadata_is_valid(&metadata))
            continue;
        long file_type = s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)metadata.file_type);
        if (file_type != m_file_type)
            continue;
        if (!c_start_menu_game_editor::load_progress_ui_for_catalogue((c_start_menu_game_editor *)catalogue,
                (e_controller_index)item_index, &m_items[m_item_count].file_reference))
            continue;
        m_items[m_item_count].valid = 1;
        memcpy(&m_items[m_item_count].metadata, &metadata, sizeof(metadata));
        m_item_count++;
    }
    for (long index = 0; index < m_item_count / 2; index++)
    {
        s_gui_content_file_share_item item = m_items[index];
        long last_index = m_item_count - index - 1;
        m_items[index] = m_items[last_index];
        m_items[last_index] = item;
    }
    m_enumeration_complete = true;
}

// @address 0x59f080 XTLGetLanguage

// @address 0x4b3348 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record_file_share_choose_item::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _start_menu_string_start_menu_items)
    {
        c_gui_list_widget *list = static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items));
        if (m_datasource != 0 && m_datasource->get_current_item_count() > 0 && list != 0 && m_pending_action == 0)
        {
            long index = (list)->get_focused_element_handle();
            c_static_wchar_string<1024> name;
            c_static_wchar_string<1024> description;
            c_static_wchar_string<1024> filename;
            const s_saved_game_item_metadata *metadata = m_datasource->get_metadata(index);
            if (metadata != 0 && metadata->size_in_bytes <= static_cast<unsigned __int64>(m_quota_total - m_quota_used) &&
                saved_game_metadata_is_valid((const s_saved_game_item_metadata_packed *)metadata))
            {
                memset(&m_description, 0, sizeof(m_description));
                if (m_target_xuid != 0
                    && m_datasource->get_text_value(index, _start_menu_string_name, &name)
                    && m_datasource->get_text_value(index, _file_share_item_string_description, &description)
                    && m_datasource->get_text_value((list)->get_focused_element_handle(), _start_menu_string_filename, &filename))
                {
                    const wchar_t *strings[2] = { name.m_string, description.m_string };
                    m_description.owner_xuid = m_target_xuid;
                    name.copy_to(m_description.metadata.name, 16);
                    wchar_string_to_utf8_string(description.m_string, m_description.metadata.description, 128, 0);
                    memcpy(&m_description.metadata, metadata, sizeof(*metadata));
                    m_file_name.set_wchar(filename.m_string);
                    string_verify_task_set_strings(&m_task, get_current_language(), strings, 2);
                    overlapped_task_start_internal(&m_task,
                        "..\\interface\\gui_screens\\start_menu\\panes\\hq_service_record_file_share\\"
                        "gui_start_menu_hq_service_record_file_share_choose_item.cpp", 412);
                    m_pending_action = 1;
                    handled = true;
                }
            }
            else
                g_gui_error_manager.post_error_with_custom_message(0x20086, 0, 0, message->m_controller, false, true);
        }
    }
    bool result = false;
    if (handled)
        result = handled;
    return result;
}

// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z

// @address 0x4b35e8 ?handle_controller_input_message@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_start_menu_hq_service_record_file_share_choose_item::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->m_event_type == 13 && message->m_component == 2)
    {
        c_gui_list_widget *list = static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items));
        if (list != 0)
        {
            long index = (list)->get_focused_element_handle();
            if (index != -1)
            {
                m_element_handle = index;
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (memory != 0)
                {
                long name = m_name;
                e_controller_index controller = get_any_responding_controller();
                c_load_dialog_screen_message *dialog = new(memory) c_load_dialog_screen_message(
                    controller, get_render_window(), name, _start_menu_string_file_share_upload_rename, name);
                if (dialog != 0)
                {
                    dialog->m_parent_screen_index = get_screen_index();
                    ::user_interface_messaging_post(dialog);
                }
                }
                handled = true;
            }
        }
    }
    bool result;
    if (!handled)
        result = c_gui_screen_widget::handle_controller_input_message(message);
    else
        result = handled;
    return result;
}

// @address 0x4b31f8 ?update_render_state@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAXK@Z
void c_gui_start_menu_hq_service_record_file_share_choose_item::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_bitmap_widget *bitmap = static_cast<c_gui_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, _start_menu_string_progress));
    if (m_quota_used != NONE && bitmap)
    {
        float ratio = MIN(MAX(m_quota_total != 0 ? (float)m_quota_used / (float)m_quota_total : 0.0f, 0.0f), 1.0f);
        s_animation_transform state;
        memcpy(&state, bitmap->get_animated_state(), sizeof(state));
        state.scale.i *= ratio;
        bitmap->set_full_animation_state(&state, false);
        bitmap->set_visible(ratio > 0.0f);
    }
}

static __forceinline c_content_catalogue *gui_start_menu_hq_service_record_file_share_choose_item_catalogue(e_controller_index controller)
{
    return &content_catalogues().catalogues[controller];
}
// @address 0x1ea490 saved_game_files_update_metadata_callback
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x1aec40 ?file_reference_create_from_path@@YAPAUs_file_reference@@PAU1@PBD_N@Z

// saved_game_files_update_metadata_async with no description, inlined at its rename call
static __forceinline long rename_content_item_async(e_controller_index controller_index, long content_item_index,
    const wchar_t *name, c_synchronized_long *completion, c_synchronized_long *done)
{
    long result;
    if (name == 0)
        result = -1;
    else
    {
        completion->set(0);
        done->set(0);
        s_saved_game_metadata_update_task task;
        task.controller_index = controller_index;
        task.content_item_index = content_item_index;
        task.name = name;
        task.description = 0;
        task.completion = completion;
        task.done = done;
        result = async_task_add_ex(_async_priority_blocking_generic, &task.task, _async_category_saved_games,
            saved_game_files_update_metadata_callback, done, true);
    }
    return result;
}
// @address 0x4b39a8 file_share_choose_item_update_rename
extern "C" void file_share_choose_item_update_rename(c_gui_start_menu_hq_service_record_file_share_choose_item *screen)
{
    if ((screen->m_pending_action == 2 || screen->m_pending_action == 3) && screen->m_keyboard != 0 && !screen->m_keyboard->busy())
    {
        if (screen->m_keyboard->get_task_state() == 4)
        {
            c_static_wchar_string<1024> filename;
            if (!screen->m_datasource->get_text_value(screen->m_element_handle, _start_menu_string_filename, &filename))
            {
                screen->m_pending_action = 0;
                return;
            }
            c_static_string<256> path;
            path.m_string[0] = 0;
            c_static_wchar_string<256> text(screen->m_keyboard->m_result_text);
            content_catalogue_trim_whitespace((unsigned short *)text.m_string);
            path.set_wchar(filename.m_string);
            if (screen->m_pending_action == 2)
            {
                screen->m_pending_action = 0;
                c_content_catalogue *catalogue = gui_start_menu_hq_service_record_file_share_choose_item_catalogue(screen->get_any_responding_controller());
                if (content_catalogue_has_storage_device(catalogue))
                {
                e_controller_index controller = screen->get_any_responding_controller();
                if (content_catalogue_is_item_name_valid(catalogue, controller, text.m_string, screen->m_selected_item_index))
                {
                    e_controller_index controller = screen->get_any_responding_controller();
                    s_file_reference reference;
                    long item = content_catalogue_find_item_by_file_reference(
                        gui_start_menu_hq_service_record_file_share_choose_item_catalogue(controller),
                        file_reference_create_from_path(&reference, path.m_string, false));
                    text.copy_to(screen->m_new_name, 256);
                    if (item != -1)
                    {
                        e_controller_index controller = screen->get_any_responding_controller();
                        screen->m_async_task_id = rename_content_item_async(
                            controller, item, screen->m_new_name, &screen->m_async_succeeded, &screen->m_async_complete);
                    }
                    if (screen->m_async_task_id != -1)
                        screen->m_pending_action = 4;
                }
                }
            }
            else if (screen->m_pending_action == 3)
            {
                e_controller_index controller = screen->get_any_responding_controller();
                s_file_reference reference;
                long item = content_catalogue_find_item_by_file_reference(
                    gui_start_menu_hq_service_record_file_share_choose_item_catalogue(controller),
                    file_reference_create_from_path(&reference, path.m_string, false));
                text.copy_to(screen->m_new_name, 256);
                if (item != -1)
                {
                    e_controller_index controller = screen->get_any_responding_controller();
                    screen->m_async_task_id = saved_game_files_update_metadata_async(
                        controller, item, 0, screen->m_new_name, &screen->m_async_succeeded, &screen->m_async_complete);
                }
                screen->m_pending_action = screen->m_async_task_id != -1 ? 4 : 0;
            }
        }
        else
            screen->m_pending_action = 0;
    }
    else if (screen->m_pending_action == 4)
    {
        if (screen->m_async_complete.peek() != 0)
        {
            volatile long &pending_action = screen->m_pending_action;
            volatile long &task_id = screen->m_async_task_id;
            pending_action = 0;
            task_id = -1;
            if (screen->m_async_succeeded.peek() != 0)
            {
                c_gui_content_item_datasource *source = screen->m_datasource;
                source->m_item_count = 0;
                source->m_enumeration_complete = false;
            }
            else
            {
                e_controller_index controller = screen->get_any_responding_controller();
                g_gui_error_manager.post_error_with_custom_message(0x2003f, 0, 0, controller, false, true);
            }
        }
    }
}

static __forceinline void gui_start_menu_hq_service_record_file_share_choose_item_show_child(
    c_gui_widget *widget, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child)
        child->set_visible(visible);
}

// @address 0x4b2ed0 ?update@c_gui_start_menu_hq_service_record_file_share_choose_item@@UAAXK@Z
void c_gui_start_menu_hq_service_record_file_share_choose_item::update(unsigned long milliseconds)
{
    bool no_items = !m_datasource || (m_datasource->m_enumeration_complete && m_datasource->get_current_item_count() == 0);
    c_gui_widget *list = get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    gui_start_menu_hq_service_record_file_share_choose_item_show_child(this, _gui_text_widget, _start_menu_string_list_empty, no_items);
    if (m_datasource && list)
    {
        c_gui_widget *preview = get_child_widget(_gui_group_widget, _start_menu_string_file_info_block);
        bool image_requested = false;
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item != 0; item = gui_widget_get_next_list_item(item, true))
        {
            long element_handle = item->get_element_handle();
            const s_saved_game_item_metadata *metadata = m_datasource->get_metadata(element_handle);
            bool fits = metadata && metadata->size_in_bytes <= (unsigned __int64)(m_quota_total - m_quota_used);
            c_gui_custom_bitmap_widget *bitmap = (c_gui_custom_bitmap_widget *)item->get_child_widget(_gui_bitmap_widget, _start_menu_string_game_variant);
            gui_widget_set_alternate_state(item, !fits);
            gui_start_menu_hq_service_record_file_share_choose_item_show_child(item, _gui_bitmap_widget, _start_menu_string_game_variant, fits);
            if (bitmap && metadata && !image_requested && bitmap->bitmap_index == -1)
            {
                gui_start_menu_hq_service_record_file_share_set_map_image(bitmap, (c_gui_data *)m_datasource, item->get_element_handle(), 1);
                image_requested = bitmap->bitmap_index != -1;
            }
        }
        if (preview)
        {
            c_gui_custom_bitmap_widget *bitmap = (c_gui_custom_bitmap_widget *)preview->get_child_widget(_gui_bitmap_widget, _start_menu_string_game_variant);
            if (bitmap)
                gui_start_menu_hq_service_record_file_share_set_map_image(bitmap, (c_gui_data *)m_datasource,
                    ((c_gui_list_widget *)list)->get_focused_element_handle(), 1);
        }
    }
    long title;
    switch (m_file_type)
    {
    case 0:
        title = _start_menu_string_film;
        break;
    case 1:
        title = _start_menu_string_film_clip;
        break;
    case 2:
        title = _start_menu_string_map_variant;
        break;
    case 4:
        title = _start_menu_string_map;
        break;
    case 3:
        title = _start_menu_string_screenshot;
        break;
    default:
        title = -1;
        break;
    }
    if (title != -1)
        set_text_widget_string_id(_start_menu_string_header, title);
    update_prepare_to_upload();
    file_share_choose_item_update_rename(this);
    if (!file_share_is_available(get_any_responding_controller()))
        transition_out_with_transition_type(0, _screen_transition_normal);
    gui_screen_widget_update(this, milliseconds);
}
