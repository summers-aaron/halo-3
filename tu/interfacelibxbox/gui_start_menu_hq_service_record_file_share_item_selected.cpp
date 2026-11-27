// interfacelibxbox/gui_start_menu_hq_service_record_file_share_item_selected.obj: from the records 0x4b40b0 0x4b4120 0x4b4128 0x4b4148 0x4b4270 0x4b4320 0x4b4420 0x4b48a0 0x4b4af8 0x4b4fd0 0x4b5228 0x4b52e0 0x4b4510 0x4b5700
// @flags /TP /O2
#include <xtl.h>
#include "saved_games/content_catalogue_types.h"
#include "networking/online/online_cached_user.h"
#include <stddef.h>
#include "saved_games/storage_device_types.h"
#include "shell/shell.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "cseries/async_xoverlapped.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_item_selected.h"
#include "interface/gui_screens/campaign/gui_screen_download_widget.h"
#include "saved_games/content_catalogue.h"
#include "interface/user_interface_error_manager.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/online/online_files.h"
#include "networking/online/online_file_work_item_types.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/user_interface_allocation.h"
#include "cseries/language.h"
#include "interface/c_gui_screen_widget.h"
#include "cseries/language_types.h"
#include "interface/c_deletable.h"
#include "interface/c_gui_list_item_widget.h"
#include <string.h>
#include "networking/online/virtual_keyboard_task.h"
#include "networking/online/online_friend_xenon.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "networking/logic/storage/network_http_request_queue.h"
#include "saved_games/storage_device.h"
#include "interface/user_interface_device_selection.h"
#include <new>
#include "cseries/cseries_unicode.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/file_share_category_dependencies.h"
#include "cseries/xenon_platform_api.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_item.h"
// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x1ebb00 ?length@?$c_static_wchar_string@$0EAA@@@QBAJXZ
// @address 0x74b540 g_current_language
// @address 0x4b40b0 ?apply_initial_state@c_load_screenshots_file_share_previewer_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x4b4120 ?get_current_item_count_internal@c_gui_file_share_selected_item_datasource@@MAAJXZ
// @address 0x4b4128 ?get_column_names@c_gui_file_share_selected_item_datasource@@UAAXQAJPAJ@Z
// @address 0x4b4148 ?initialize@c_gui_file_share_selected_item_datasource@@UAA_NJ@Z
// @address 0x4b4270 ?get_string_id_value@c_gui_file_share_selected_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4b48a0 ?handle_dialog_result@c_gui_start_menu_hq_service_record_file_share_item_selected@@UAA_NPBVc_dialog_result_message@@@Z

enum e_file_share_selected_action
{
    _file_share_confirm_recommendation = 0x3002b,
    _file_share_confirm_download = 0x3002c,
    _file_share_action_recommend = 0x10240,
    _file_share_action_recommend_to_friend = 0x10241,
    _file_share_action_download = 0x1023f,
    _file_share_action_view_screenshot = 0x1029d,
};

class c_gui_file_share_selected_item_datasource : public c_gui_ordered_data
{
public:
    virtual bool get_string_id_value(long index, long string_id, long *value);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool initialize(long name);
    __forceinline c_gui_file_share_selected_item_datasource(s_file_share_slot file_share_slot, bool is_owner);
protected:
    virtual long get_current_item_count_internal();

    s_file_share_slot m_file_share_slot;
    bool m_has_extra;
    long m_string_ids[8];
    long m_item_count;
};

struct s_file_share_localized_name
{
    long index;
    c_static_wchar_string<256> string;
};

typedef bool (*t_storage_device_query)(
    unsigned long, e_storage_device_type *, unsigned __int64 *, unsigned __int64 *, wchar_t *, long);

typedef char file_share_selected_slot_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_file_share_slot) == 0x1aa0) )
    ) ? 1 : -1];

typedef char gui_file_share_description_offset[(offsetof(s_file_share_slot, description) == 0x28) ? 1 : -1];

typedef char gui_file_share_request_offset[(offsetof(c_online_files_deleter, m_request_cookie) == 0x38) ? 1 : -1];

typedef char gui_file_share_overlapped_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_find_friend_task) == 0x2074) )
    ) ? 1 : -1];

static __forceinline void append_column_name(long *names, long *count, long name)
{
    names[*count] = name;
    (*count)++;
}

__forceinline void c_gui_screen_download_widget::set_file_share_item(
    unsigned __int64 owner, unsigned __int64 target, long slot, const char *filename, const s_online_file_description *description)
{
    m_owner_xuid = owner;
    m_target_xuid = target;
    m_slot = slot;
    memcpy(m_filename, filename, sizeof(m_filename));
    m_description = *description;
}

void c_load_screenshots_file_share_previewer_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    static_cast<c_gui_screen_download_widget *>(screen)->set_file_share_item(
        m_owner_xuid, m_target_xuid, m_slot, m_filename, &m_description);
}

long c_gui_file_share_selected_item_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

void c_gui_file_share_selected_item_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    append_column_name(column_names, column_count, _start_menu_string_name);
}

bool c_gui_file_share_selected_item_datasource::initialize(long name)
{
    bool result = false;
    m_name = name;
    m_item_count = 0;
    if (m_file_share_slot.state == 2) {
        if (m_has_extra) {
            if (m_file_share_slot.description.metadata.file_type == 13) {
                m_string_ids[m_item_count++] = _file_share_action_view_screenshot;
            }
            m_string_ids[m_item_count++] = _file_share_action_recommend;
            m_string_ids[m_item_count++] = _file_share_action_download;
            m_string_ids[m_item_count++] = _file_share_action_recommend_to_friend;
        } else {
            if (m_file_share_slot.description.metadata.file_type == 13) {
                m_string_ids[m_item_count++] = _file_share_action_view_screenshot;
            }
            m_string_ids[m_item_count++] = _file_share_action_download;
            m_string_ids[m_item_count++] = _file_share_action_recommend;
        }
        result = true;
    }
    return result;
}

bool c_gui_file_share_selected_item_datasource::get_string_id_value(
    long index, long string_id, long *value)
{
    bool found = false;
    if (index >= 0 && index < m_item_count && string_id == _start_menu_string_name) {
        *value = m_string_ids[index];
        found = true;
    }
    return found;
}

// @address 0x39ed28 ?set_default_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39edb8 ?set_title_text@c_virtual_keyboard_task@@QAAXPB_W@Z
// @address 0x39ee48 ?set_description_text@c_virtual_keyboard_task@@QAAXPB_W@Z

extern "C" {
// @address 0x518f0 k_file_share_keyboard_task_name
// @address 0x51978 k_file_share_recommendation_title_format
// The task-name field spans 136 bytes and the title format 8 wchars, both including trailing zero padding.
// @rdata 0x820518f0..0x82051988
const char k_file_share_keyboard_task_name[136] = "..\\interface\\gui_screens\\start_menu\\panes\\"
    "hq_service_record_file_share\\gui_start_menu_hq_service_record_file_share_item_selected.cpp";
const wchar_t k_file_share_recommendation_title_format[8] = L"%s\r\n%s";
// @rdata-end
// @address 0x51a60 ??_7c_gui_file_share_selected_item_datasource@@6B@

}
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z

// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
__forceinline c_gui_file_share_selected_item_datasource::c_gui_file_share_selected_item_datasource(s_file_share_slot file_share_slot, bool is_owner)
{
    m_file_share_slot = file_share_slot;
    m_has_extra = is_owner;
    m_item_count = 0;
}

// @address 0x4b4320 ?initialize_datasource@c_gui_start_menu_hq_service_record_file_share_item_selected@@UAAXXZ
void c_gui_start_menu_hq_service_record_file_share_item_selected::initialize_datasource()
{
    this->c_gui_screen_widget::initialize_datasource();
    c_gui_file_share_selected_item_datasource *datasource =
        new(0, 0, _ui_allocation_marker_dummy)
        c_gui_file_share_selected_item_datasource(m_file_share_slot, m_owner_xuid == m_player_xuid);
    if (datasource) {
        if (datasource->initialize(_start_menu_string_display_name))
            m_datasource[m_datasource_count++] = (c_gui_data *)datasource;
        else {
            datasource->~c_gui_file_share_selected_item_datasource();
            user_interface_free(datasource);
        }
    }
}

// @address 0x4064b8 ?cancel_request@c_network_http_request_queue@@QAAXJ@Z
// @address 0x20e7e8 ?overlapped_task_terminate@@YAXPAVc_overlapped_task@@@Z
// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z

// @address 0x4b4420 ?dispose@c_gui_start_menu_hq_service_record_file_share_item_selected@@UAAXXZ
void c_gui_start_menu_hq_service_record_file_share_item_selected::dispose()
{
    c_online_files_deleter *deleter = &m_deleter;
    if (deleter->m_request_cookie != 0) {
        g_network_http_request_queues[_network_http_request_queue_files].cancel_request(deleter->m_request_cookie);
        deleter->m_request_cookie = 0;
        deleter->m_xuid = 0;
        deleter->m_owner_xuid = 0;
        deleter->m_slot_index = -1;
        deleter->m_status = 3;
    }
    if (m_keyboard->busy()) {
        overlapped_task_terminate(m_keyboard);
        overlapped_task_block_until_finished(m_keyboard);
    }
    c_overlapped_task *task = &m_find_friend_task;
    if (task->busy()) {
        overlapped_task_terminate(task);
        overlapped_task_block_until_finished(task);
    }
    c_gui_screen_widget::dispose();
}

bool c_gui_start_menu_hq_service_record_file_share_item_selected::handle_dialog_result(const c_dialog_result_message *message)
{
    s_online_file_description description;
    wchar_t title[1024];
    wchar_t description_text[1024];
    bool handled = false;
    switch (message->get_dialog_name()) {
    case _file_share_confirm_recommendation:
        if (message->get_dialog_result() == _gui_dialog_choice_first) {
            // match: 0x4b48a0: volatile pins the m_size load first in the deleter field schedule
            long size = ((volatile c_gui_start_menu_hq_service_record_file_share_item_selected *)this)->m_size;
            unsigned __int64 file = this->m_player_xuid;
            unsigned __int64 owner = m_owner_xuid;
            m_deleting = true;
            c_online_files_deleter *deleter = &m_deleter;
            deleter->m_slot_index = size;
            deleter->m_owner_xuid = file;
            deleter->m_xuid = owner;
            char *name = deleter->m_name;
            strncpy(name, m_file_share_slot.filename, 33);
            name[32] = 0;
            deleter->m_status = 0;
        }
        handled = true;
        break;
    case _file_share_confirm_download:
        m_state = 0;
        switch (message->get_dialog_result()) {
        case _gui_dialog_choice_first: {
            title[0] = 0;
            description_text[0] = 0;
            get_string_by_string_id(_start_menu_string_rename_file, (c_static_wchar_string<1024> *)title);
            get_string_by_string_id(_start_menu_string_rename_file_description, (c_static_wchar_string<1024> *)description_text);
            m_keyboard->m_controller_index = message->get_controller();
            m_keyboard->set_default_text(m_new_name);
            m_keyboard->set_title_text(title);
            m_keyboard->set_description_text(description_text);
            m_keyboard->m_maximum_input_characters = 16;
            m_keyboard->m_character_flags = 0x4001;
            if (overlapped_task_start_internal(m_keyboard, k_file_share_keyboard_task_name, 364))
                m_state = 6;
            else
                m_keyboard->reset();
            break;
        }
        case _gui_dialog_choice_second: {
            description = m_file_share_slot.description;
            description.named = true;
            wchar_t *destination = description.metadata.name;
            const wchar_t *source = m_new_name;
            for (long i = 0; i <= 16; i++) {
                if (!(destination[i] = *source))
                    break;
                source++;
            }
            description.owner_xuid = m_owner_xuid;
            c_online_file_manager::get()->enqueue_for_transfer(_queued_work_item_type_download, &description,
                "", m_file_share_slot.filename,
                m_owner_xuid, m_player_xuid, m_size, false);
            m_state = 0;
            transition_out_with_transition_type(0, _screen_transition_normal);
            break;
        }
        default:
            m_state = 0;
            break;
        }
        handled = true;
        break;
    }
    if (!handled) {
        handled = false;
        if (message->get_dialog_name() == _start_menu_string_device_selection) {
            if (message->get_dialog_result() == _gui_dialog_choice_first)
                content_catalogue_display_device_selection_guide_interface(message->get_controller());
            handled = true;
        }
    }
    return handled;
}

static __forceinline c_content_catalogue *get_controller_content_catalogue(long controller)
{
    return &content_catalogues().catalogues[(controller * 0x9d0) / 0x9d0];
}
static __forceinline c_storage_device *get_controller_storage_device(long controller)
{
    return (unsigned long)controller <= 3 ? &content_catalogues().storage_devices[controller] : 0;
}
static __forceinline unsigned __int64 get_catalogue_free_bytes(c_content_catalogue *catalogue)
{
    unsigned __int64 result = 0;
    if (content_catalogue_has_storage_device(catalogue)) {
        c_storage_device *device = get_controller_storage_device(catalogue->m_controller_index);
        unsigned long id = device ? device->get_device_id() : 0xcacacaca;
        e_storage_device_type type;
        unsigned __int64 total, free;
        wchar_t name[128];
        if (((t_storage_device_query)c_storage_device::get_device_data)(id, &type, &total, &free, name, 128))
            result = free;
    }
    return result;
}
static __forceinline void copy_file_share_name(wchar_t *destination, const wchar_t *source)
{
    for (long i = 0; i <= 16; ++i) {
        if (!(destination[i] = *source)) break;
        ++source;
    }
}
// @address 0x4b4af8 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record_file_share_item_selected@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record_file_share_item_selected::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list == _start_menu_string_display_name) {
        long selected;
        if (data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &selected)) {
            e_controller_index controller = message->get_controller();
            bool allowed = online_cached_user_can_use_file_share(controller);
            switch (selected) {
            case _file_share_action_recommend:
                file_share_item_selected_show_recommendation(this);
                transition_out_with_transition_type(0, _screen_transition_normal);
                break;
            case _file_share_action_recommend_to_friend: {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (memory) {
                    long name = get_name();
                    c_load_dialog_screen_message *dialog = new(memory) c_load_dialog_screen_message(controller,
                        get_render_window(), name, _file_share_confirm_recommendation, name);
                    if (dialog) {
                        dialog->set_parent_screen_index(get_screen_index());
                        ::user_interface_messaging_post((c_message const *)(c_deletable *)dialog);
                    }
                }
                break;
            }
            case _file_share_action_download: {
                unsigned __int64 required =
                    m_file_share_slot.description.metadata.size_in_bytes + 0x100000;
                c_content_catalogue *catalogue = get_controller_content_catalogue(controller);
                if (!content_catalogue_has_storage_device(catalogue)) {
                    user_interface_show_device_selection_dialog(controller, get_name(), get_screen_index());
                } else if (get_catalogue_free_bytes(catalogue) < required) {
                    g_gui_error_manager.post_error_with_custom_message(0x2008d, 0, 0, controller, false, true);
                } else if (allowed) {
                    copy_file_share_name(
                        m_new_name, m_file_share_slot.description.metadata.name);
                    m_state = 3;
                } else {
                    g_gui_error_manager.post_error_with_custom_message(0x20011, 0, 0, controller, false, true);
                }
                break;
            }
            case _file_share_action_view_screenshot:
                if (!content_catalogue_has_storage_device(get_controller_content_catalogue(controller))) {
                    user_interface_show_device_selection_dialog(controller, get_name(), get_screen_index());
                } else if (allowed) {
                    copy_file_share_name(
                        m_new_name, m_file_share_slot.description.metadata.name);
                    m_state = 4;
                } else {
                    g_gui_error_manager.post_error_with_custom_message(0x20011, 0, 0, controller, false, true);
                }
                break;
            }
        }
        handled = true;
    }
    if (!handled) handled = false;
    return handled;
}

// 0x4b5228: fills a 256-character text from a string id, substituting one argument for "%s" when the string has it (unnamed)

// @address 0x4b5228 ?set_string_with_substitute@c_gui_start_menu_hq_service_record_file_share_item_selected@@AAA_NJW4e_saved_game_file_type@@PB_WPAV?$c_static_wchar_string@$0BAA@@@@Z
bool c_gui_start_menu_hq_service_record_file_share_item_selected::set_string_with_substitute(long string_id, e_saved_game_file_type unused,
    const wchar_t *argument, c_static_wchar_string<256> *text)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    if (get_string_by_string_id(string_id, (c_static_wchar_string<1024> *)&string))
    {
        if (string.index_of(L"%s") == -1)
            text->set(string.get_string());
        else
            text->print(string.get_string(), argument);
        result = true;
    }
    return result;
}

// 0x4b52e0: string id of the file-share category/language key, read through the screen's get_string
// @address 0x4b52e0 file_share_item_selected_get_category_text
extern "C" bool file_share_item_selected_get_category_text(
    c_gui_screen_widget *source, long language, long category, wchar_t *text)
{
    bool result = false;
    long string_id;
    if (category >= 1 && category <= 9)
    {
        switch (language)
        {
        case _language_japanese: string_id = _start_menu_string_recommend_game_variant_japanese; break;
        case _language_german: string_id = _start_menu_string_recommend_game_variant_german; break;
        case _language_french: string_id = _start_menu_string_recommend_game_variant_french; break;
        case _language_spanish: string_id = _start_menu_string_recommend_game_variant_spanish; break;
        case _language_mexican: string_id = _start_menu_string_recommend_game_variant_mexican; break;
        case _language_italian: string_id = _start_menu_string_recommend_game_variant_italian; break;
        case _language_korean: string_id = _start_menu_string_recommend_game_variant_korean; break;
        case _language_chinese_traditional: case _language_chinese_simplified: string_id = _start_menu_string_recommend_game_variant_chinese; break;
        case _language_portuguese: string_id = _start_menu_string_recommend_game_variant_portuguese; break;
        default: string_id = _start_menu_string_recommend_game_variant_english; break;
        }
    }
    else if (category == 10)
    {
        switch (language)
        {
        case _language_japanese: string_id = _start_menu_string_recommend_map_variant_japanese; break;
        case _language_german: string_id = _start_menu_string_recommend_map_variant_german; break;
        case _language_french: string_id = _start_menu_string_recommend_map_variant_french; break;
        case _language_spanish: string_id = _start_menu_string_recommend_map_variant_spanish; break;
        case _language_mexican: string_id = _start_menu_string_recommend_map_variant_mexican; break;
        case _language_italian: string_id = _start_menu_string_recommend_map_variant_italian; break;
        case _language_korean: string_id = _start_menu_string_recommend_map_variant_korean; break;
        case _language_chinese_traditional: case _language_chinese_simplified: string_id = _start_menu_string_recommend_map_variant_chinese; break;
        case _language_portuguese: string_id = _start_menu_string_recommend_map_variant_portuguese; break;
        default: string_id = _start_menu_string_recommend_map_variant_english; break;
        }
    }
    else if (category == 11)
    {
        switch (language)
        {
        case _language_japanese: string_id = _start_menu_string_recommend_film_japanese; break;
        case _language_german: string_id = _start_menu_string_recommend_film_german; break;
        case _language_french: string_id = _start_menu_string_recommend_film_french; break;
        case _language_spanish: string_id = _start_menu_string_recommend_film_spanish; break;
        case _language_mexican: string_id = _start_menu_string_recommend_film_mexican; break;
        case _language_italian: string_id = _start_menu_string_recommend_film_italian; break;
        case _language_korean: string_id = _start_menu_string_recommend_film_korean; break;
        case _language_chinese_traditional: case _language_chinese_simplified: string_id = _start_menu_string_recommend_film_chinese; break;
        case _language_portuguese: string_id = _start_menu_string_recommend_film_portuguese; break;
        default: string_id = _start_menu_string_recommend_film_english; break;
        }
    }
    else if (category == 12)
    {
        switch (language)
        {
        case _language_japanese: string_id = _start_menu_string_recommend_film_clip_japanese; break;
        case _language_german: string_id = _start_menu_string_recommend_film_clip_german; break;
        case _language_french: string_id = _start_menu_string_recommend_film_clip_french; break;
        case _language_spanish: string_id = _start_menu_string_recommend_film_clip_spanish; break;
        case _language_mexican: string_id = _start_menu_string_recommend_film_clip_mexican; break;
        case _language_italian: string_id = _start_menu_string_recommend_film_clip_italian; break;
        case _language_korean: string_id = _start_menu_string_recommend_film_clip_korean; break;
        case _language_chinese_traditional: case _language_chinese_simplified: string_id = _start_menu_string_recommend_film_clip_chinese; break;
        case _language_portuguese: string_id = _start_menu_string_recommend_film_clip_portuguese; break;
        default: string_id = _start_menu_string_recommend_film_clip_english; break;
        }
    }
    else if (category == 13)
    {
        switch (language)
        {
        case _language_japanese: string_id = _start_menu_string_recommend_screenshot_japanese; break;
        case _language_german: string_id = _start_menu_string_recommend_screenshot_german; break;
        case _language_french: string_id = _start_menu_string_recommend_screenshot_french; break;
        case _language_spanish: string_id = _start_menu_string_recommend_screenshot_spanish; break;
        case _language_mexican: string_id = _start_menu_string_recommend_screenshot_mexican; break;
        case _language_italian: string_id = _start_menu_string_recommend_screenshot_italian; break;
        case _language_korean: string_id = _start_menu_string_recommend_screenshot_korean; break;
        case _language_chinese_traditional: case _language_chinese_simplified: string_id = _start_menu_string_recommend_screenshot_chinese; break;
        case _language_portuguese: string_id = _start_menu_string_recommend_screenshot_portuguese; break;
        default: string_id = _start_menu_string_recommend_screenshot_english; break;
        }
    }
    else
        string_id = -1;
    if (string_id != -1)
        result = source->get_string_by_string_id(string_id, (c_static_wchar_string<1024> *)text);
    return result;
}

// @address 0x39e5d8 ?online_guide_show_file_share_recommendation@@YAXW4e_controller_index@@_KJPBDPBUs_service_record_identity@@PBUs_compose_text@@@Z

// @address 0x4b4fd0 file_share_item_selected_show_recommendation
extern "C" void file_share_item_selected_show_recommendation(c_gui_start_menu_hq_service_record_file_share_item_selected *screen)
{
    c_gui_screen_widget *source =
        (c_gui_screen_widget *)screen;
    struct {
        wchar_t value[256];
        wchar_t key[1024];
        wchar_t item_key[1024];
        wchar_t name[256];
        wchar_t title[256];
        wchar_t description[256];
    } l;
    l.key[0] = 0;
    e_language language = get_current_language();
    if (file_share_item_selected_get_category_text(source, language, (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type, l.key))
    {
        l.name[0] = 0;
        l.title[0] = 0;
        l.description[0] = 0;
        s_file_share_localized_name entries[12];
        if (screen->set_string_with_substitute(
                _start_menu_string_recommend_file_title,
                (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type, l.key,
                (c_static_wchar_string<256> *)l.name) &&
            screen->set_string_with_substitute(
                _start_menu_string_recommend_file_player_message,
                (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type, l.key,
                (c_static_wchar_string<256> *)l.description))
        {
            ((c_static_wchar_string<256> *)l.title)
                ->print(k_file_share_recommendation_title_format, screen->m_file_share_slot.description.metadata.name,
                    l.key);
            long i = 0;
            s_file_share_localized_name *entry = entries;
            for (; i < 12; i++)
            {
                l.item_key[0] = 0;
                if (file_share_item_selected_get_category_text(source, i,
                    (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type, l.item_key))
                {
                    l.value[0] = 0;
                    long id;
                    switch (i)
                    {
                    case _language_japanese: id = _start_menu_string_recommend_file_view_file_japanese; break;
                    case _language_german: id = _start_menu_string_recommend_file_view_file_german; break;
                    case _language_french: id = _start_menu_string_recommend_file_view_file_french; break;
                    case _language_spanish: id = _start_menu_string_recommend_file_view_file_spanish; break;
                    case _language_mexican: id = _start_menu_string_recommend_file_view_file_mexican; break;
                    case _language_italian: id = _start_menu_string_recommend_file_view_file_italian; break;
                    case _language_korean: id = _start_menu_string_recommend_file_view_file_korean; break;
                    case _language_chinese_traditional: case _language_chinese_simplified: id = _start_menu_string_recommend_file_view_file_chinese; break;
                    case _language_portuguese: id = _start_menu_string_recommend_file_view_file_portuguese; break;
                    default: id = _start_menu_string_recommend_file_view_file_english; break;
                    }
                    if (screen->set_string_with_substitute( id, (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type, l.item_key,
                            (c_static_wchar_string<256> *)l.value))
                    {
                        entry->index = i;
                        entry->string.set(l.value);
                    }
                    else
                        entry->index = -1;
                    entry++;
                }
            }
            e_controller_index controller = ((c_gui_widget *)screen)->get_any_responding_controller();
            online_guide_show_file_share_recommendation(controller,
                screen->m_player_xuid, screen->m_size, screen->m_file_share_slot.filename,
                &screen->m_identity.identity, (const s_compose_text *)l.name);
        }
    }
}


// @address 0x51a50 ??_7c_load_screenshots_file_share_previewer_screen_message@@6B@
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
// @address 0x1e83e0 ?saved_game_file_type_to_game_content_type@@YA?AW4e_game_content_type@@W4e_saved_game_file_type@@@Z
// @address 0x2841f8 ?try_and_get_personal_storage_item@c_content_catalogue@@QAAJPB_WW4e_content_item_status@@@Z

// @address 0x3bc240 ?update@c_online_files_deleter@@QAAXXZ
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
// @address 0x8a698 ?is_guest@c_controller_interface@@QBA_NXZ

static __forceinline bool file_share_item_selected_online(e_controller_index controller)
{
    bool result = false;
    if (content_file_share_enabled())
        result = controller_get(controller)->is_signed_in_to_live() && !controller_get(controller)->is_guest();
    return result;
}

static __forceinline void file_share_selected_screen_dismiss(c_gui_start_menu_hq_service_record_file_share_item_selected *screen)
{
    screen->m_state = 0;
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
}

static __forceinline wchar_t *file_share_item_selected_ustrncpy(wchar_t *dest, const wchar_t *source, long count)
{
    for (long index = 0; index <= count; ++index) {
        if (!(dest[index] = *source))
            break;
        ++source;
    }
    return dest;
}
// @address 0x4b5700 file_share_selected_update_pending_action
extern "C" void file_share_selected_update_pending_action(c_gui_start_menu_hq_service_record_file_share_item_selected * screen)
{
    switch (screen->m_state)
    {
    case 6:
    {
        if (!screen->m_keyboard || screen->m_keyboard->busy()) return;
        if (screen->m_keyboard->get_task_state() == 4)
        {
            file_share_item_selected_ustrncpy(screen->m_new_name, screen->m_keyboard->m_result_text, 16);
            screen->m_state = 3;
        }
        else screen->m_state = 0;
        screen->m_keyboard->reset();
        screen->m_keyboard->m_controller_index = k_no_controller;
        return;
    }
    case 4:
    {
        s_online_file_description description = screen->m_file_share_slot.description;
        description.named = false;
        file_share_item_selected_ustrncpy(description.metadata.name, screen->m_new_name, 16);
        description.owner_xuid = screen->m_owner_xuid;
        void *memory = user_interface_malloc_tracked(sizeof(c_load_screenshots_file_share_previewer_screen_message), 0, 0);
        if (memory)
        {
            long name = screen->m_name;
            e_controller_index active = screen->get_any_responding_controller();
            c_load_screenshots_file_share_previewer_screen_message *load = new (memory)
                c_load_screenshots_file_share_previewer_screen_message(active, screen->get_render_window(), name,
                screen->m_owner_xuid, screen->m_player_xuid, screen->m_size, &description, screen->m_file_share_slot.filename);
            load->m_parent_screen_index = screen->get_parent_screen()->get_screen_index();
            ::user_interface_messaging_post(load);
        }
        file_share_selected_screen_dismiss(screen);
        return;
    }
    case 3:
    {
        e_saved_game_file_type file_type = (e_saved_game_file_type)screen->m_file_share_slot.description.metadata.file_type;
        e_game_content_type type = saved_game_file_type_to_game_content_type(file_type);
        e_controller_index controller = screen->get_any_responding_controller();
        bool exists = false;
        if (type != _game_content_type_none)
            exists = content_catalogues().catalogues[controller]
                         .try_and_get_personal_storage_item(screen->m_new_name, (e_content_item_status)type) != -1;
        if (exists)
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
            if (memory)
            {
                long name = screen->m_name;
                e_controller_index active = screen->get_any_responding_controller();
                c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(active,
                    screen->get_render_window(), name, _start_menu_string_file_share_name_conflict, name);
                if (dialog) { dialog->m_parent_screen_index = screen->get_screen_index(); ::user_interface_messaging_post(dialog); screen->m_state = 5; }
            }
            return;
        }
        s_online_file_description description = screen->m_file_share_slot.description;
        description.named = false;
        file_share_item_selected_ustrncpy(description.metadata.name, screen->m_new_name, 16);
        unsigned __int64 requester;
        XNetRandom((unsigned char *)&requester, 8);
        description.metadata.unique_id = requester;
        description.owner_xuid = screen->m_owner_xuid;
        c_online_file_manager::get()->enqueue_for_transfer(_queued_work_item_type_download, &description, "", screen->m_file_share_slot.filename,
            screen->m_owner_xuid, screen->m_player_xuid, screen->m_size, false);
        file_share_selected_screen_dismiss(screen);
        return;
    }
    default: return;
    }
}

// @address 0x4b4510
void c_gui_start_menu_hq_service_record_file_share_item_selected::update(unsigned long milliseconds)
{
    gui_screen_widget_update(this, milliseconds);
    c_gui_text_widget *text = get_child_text_widget(_file_share_item_string_title);
    if (text)
        text->set_text(m_file_share_slot.description.metadata.name);

    m_deleter.update();

    if (m_deleting)
    {
        c_gui_screen_widget *child = static_cast<c_gui_screen_widget *>(get_child_widget(_gui_screen_widget, _start_menu_string_in_progress));
        if (m_deleter.m_request_cookie != 0)
        {
            if (!child)
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
                c_load_in_progress_screen_message *message = memory
                    ? new (memory) c_load_in_progress_screen_message(k_any_controller, _console_window,
                        _start_menu_string_file_share_deleting_title, _start_menu_string_file_share_deleting_message)
                    : 0;
                if (message)
                {
                    message->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(message);
                }
            }
        }
        else
        {
            if (child)
                child->transition_out_with_transition_type(0, _screen_transition_normal);
            c_online_file_manager *manager = c_online_file_manager::get();
            manager->m_refresh_queue = false;
            manager->m_active_transfer_tracker.m_status_dirty = true;
            manager->m_active_transfer_tracker.m_refresh_count++;
            transition_out_with_transition_type(0, _screen_transition_normal);
            m_deleting = false;
        }
    }

    long state = m_state;
    if ((state == 1 || state == 2) && !m_find_friend_task.busy())
    {
        bool was_upload = state == 2;
        m_state = 0;
        c_gui_screen_widget *child = static_cast<c_gui_screen_widget *>(get_child_widget(_gui_screen_widget, _start_menu_string_in_progress));
        if (child)
            child->transition_out_with_transition_type(0, _screen_transition_normal);
        if (m_find_friend_task.get_task_state() != _overlapped_task_state_success)
        {
            g_gui_error_manager.post_error_with_custom_message(0x20010, 0, 0, m_find_friend_task.get_controller_index(), false, true);
        }
        else
        {
            bool found = false;
            XONLINE_FRIEND record;
            unsigned long count = m_find_friend_task.get_friend_count();
            for (unsigned long index = 0; index < count; ++index)
            {
                record = *m_find_friend_task.get_friend(index);
                // skip pending friend requests
                bool sent_request = (record.dwFriendState & XONLINE_FRIENDSTATE_FLAG_SENTREQUEST) != 0;
                bool received_request = (record.dwFriendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST) != 0;
                if (record.xuid == m_player_xuid && !sent_request && !received_request)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                file_share_item_selected_ustrncpy(m_new_name, m_file_share_slot.description.metadata.name, 16);
                m_state = was_upload ? 4 : 3;
                transition_out_with_transition_type(0, _screen_transition_normal);
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20012, 0, 0, m_find_friend_task.get_controller_index(), false, true);
            }
        }
        m_find_friend_task.reset();
    }

    file_share_selected_update_pending_action(this);

    bool online = file_share_item_selected_online(get_any_responding_controller());
    if (!online)
        transition_out_with_transition_type(0, _screen_transition_normal);
}
