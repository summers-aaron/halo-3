// interfacelibxbox/gui_start_menu_hq_service_record_file_share_choose_category.obj: from the records 0x4b0a90 0x4b2308 0x4b23a8 0x4b2588 0x4b31f8
#include "interface/c_player_marketplace.h"
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
#include "interface/user_interface_device_selection.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_utilities.h"
#include "memory/static_arrays.h"
#include "cseries/cseries.h"
// @address 0x4b2588 ?gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text@@YA_NPBVc_gui_start_menu_hq_service_record_file_share_choose_category@@PA_WK@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x486ba0 ?set_text_widget_string_id_from_datasource@c_gui_screen_widget@@QAAXJJJJ@Z
#include <string.h>
#include "interface/file_share_category_dependencies.h"
// @address 0x8a1ead g_utility_drive_mounted
#include "interface/gui_screen_widget_legacy.h"

#define SLOT(n) virtual void slot##n();
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_category.h"
#include "interface/c_controller.h"
#include "interface/c_deletable.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_animation_transform.h"
#include "saved_games/content_catalogue_types.h"
#include "interface/gui_data.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include <new>
#include "interface/gui_file_share_screen_messages.h"
#include "interface/controller_inline.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_allocation.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_item.h"

class c_font_cache;

// @address 0x515c4 ??_7c_load_file_share_choose_item_screen_message@@6B@
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z

enum
{
    _string_id_category = 254,
    _string_id_game_variant = 0x1f6,
    _string_id_map_variant = 0x1f7,
    _string_id_description = 0x29e,
    _string_id_type = 0x1002c,
    _string_id_progress = 0x1002d,
    _string_id_film = 0x10213,
    _string_id_film_clip = 0x10214,
    _string_id_screenshot = 0x1021c,
};

static __forceinline c_content_catalogue *gui_start_menu_hq_service_record_file_share_choose_category_catalogue(e_controller_index controller)
{
    return &content_catalogues().catalogues[(controller * 0x9d0) / 0x9d0];
}

// The file share category of a list item's type, or NONE for other items.
static __forceinline long gui_start_menu_hq_service_record_file_share_choose_category_category(long item)
{
    long category = NONE;
    switch (item)
    {
    case _string_id_film:
        category = 0;
        break;
    case _string_id_film_clip:
        category = 1;
        break;
    case _string_id_map_variant:
        category = 2;
        break;
    case _string_id_screenshot:
        category = 3;
        break;
    case _string_id_game_variant:
        category = 4;
        break;
    }
    return category;
}

static __forceinline bool gui_start_menu_hq_service_record_file_share_choose_category_user_ready(e_controller_index controller_index)
{
    g_player_marketplace_users[controller_index].controller_index = controller_index;
    long state = g_player_marketplace_users[controller_index].catalogue_state;
    return state == 2 || state == 3;
}
// @address 0x4b0a90 file_share_is_available
extern "C" bool file_share_is_available(e_controller_index controller_index)
{
    bool result = false;
    bool allowed = false;
    if (content_file_share_enabled()) {
        c_controller_interface *controller = controller_get(controller_index);
        allowed = controller->is_signed_in_to_live() && !controller->is_guest();
    }
    if (allowed) {
        result = controller_is_live_gold_nonguest(controller_get(controller_index));
        if (!result)
            result = gui_start_menu_hq_service_record_file_share_choose_category_user_ready(controller_index);
    }
    return result;
}

// @address 0x4b2ba8 ?initialize@c_gui_start_menu_hq_service_record_file_share_choose_category@@UAAXXZ
void c_gui_start_menu_hq_service_record_file_share_choose_category::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<quota-used", this, (bool (*)(void *, wchar_t *, long))parse_quota_used));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<quota-total", this, (bool (*)(void *, wchar_t *, long))parse_quota_total));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<quota-remaining", this,
        (bool (*)(void *, wchar_t *, long))gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text));
}

// @address 0x4b2308 ?update@c_gui_start_menu_hq_service_record_file_share_choose_category@@UAAXK@Z
void c_gui_start_menu_hq_service_record_file_share_choose_category::update(unsigned long current_milliseconds)
{
    gui_screen_widget_update(this, current_milliseconds);
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _string_id_category);
    if (list)
    {
        long datasource_name = list->m_datasource_name;
        set_text_widget_string_id_from_datasource(_string_id_description, datasource_name, list->get_focused_element_handle(), _string_id_description);
    }
    if (!file_share_is_available(get_any_responding_controller()))
        transition_out_with_transition_type(0, _screen_transition_normal);
}

// 0x4b23a8: choosing a category opens its file share choose-item screen, or the device guide when no catalogue is ready
// @address 0x4b23a8 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record_file_share_choose_category@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record_file_share_choose_category::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _string_id_category)
    {
        long selected = NONE;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), _string_id_type, &selected))
        {
            long category = gui_start_menu_hq_service_record_file_share_choose_category_category(selected);
            if (category != NONE)
            {
                e_controller_index controller = message->get_controller();
                if (!content_catalogue_has_storage_device(gui_start_menu_hq_service_record_file_share_choose_category_catalogue(controller)))
                {
                    user_interface_show_device_selection_dialog(controller, m_name, m_screen_index);
                }
                else
                {
                    void *memory = user_interface_malloc_tracked(sizeof(c_load_file_share_choose_item_screen_message), 0, 0);
                    if (memory)
                    {
                        long layer = m_name;
                        e_controller_index owner = message->get_controller();
                        long e = m_e;
                        long d = m_d;
                        long c = m_c;
                        __int64 b = m_b;
                        __int64 a = m_a;
                        e_window_index window = get_render_window();
                        c_load_file_share_choose_item_screen_message *posted = new (memory)
                            c_load_file_share_choose_item_screen_message(owner, window, layer, a, b, c, d, e,
                                (e_saved_game_file_type)category);
                        posted->m_parent_screen_index = m_screen_index;
                        ::user_interface_messaging_post((c_message const *)(c_deletable *)posted);
                    }
                }
            }
            handled = true;
        }
    }
    bool result = false;
    if (handled)
        result = handled;
    return result;
}

static __forceinline float gui_start_menu_hq_service_record_file_share_choose_category_progress_ratio(
    const c_gui_start_menu_hq_service_record_file_share_choose_category *widget)
{
    float ratio = 0.0f;
    if (widget->m_e != 0)
        ratio = (float)widget->m_d / (float)widget->m_e;
    return ratio;
}

// 0x4b2588: size text of the bytes remaining (m_e - m_d), copied into destination at most max_chars wide
bool gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text(
    const c_gui_start_menu_hq_service_record_file_share_choose_category *widget, wchar_t *destination, unsigned long max_chars)
{
    c_static_wchar_string<1024> text;
    bool result = false;
    long total = widget->m_e;
    long progress = widget->m_d;
    if (total != NONE && progress != NONE && user_interface_get_bytes_size_string(total - progress, &text))
    {
        unsigned long size = max_chars;
        if (size > 0x400)
            size = 0x400;
        ustrnzcpy(destination, text.get_string(), (long)size);
        result = true;
    }
    return result;
}

// @address 0x4b31f8 ?update_render_state@c_gui_start_menu_hq_service_record_file_share_choose_category@@UAAXK@Z
void c_gui_start_menu_hq_service_record_file_share_choose_category::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_bitmap_widget *bitmap = (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, _string_id_progress);
    if (m_d != NONE && bitmap)
    {
        float ratio = MIN(MAX(gui_start_menu_hq_service_record_file_share_choose_category_progress_ratio(this), 0.0f), 1.0f);
        s_animation_transform state;
        memcpy(&state, ((c_gui_widget *)bitmap)->get_animated_state(), sizeof(state));
        state.scale.i *= ratio;
        ((c_gui_widget *)bitmap)->set_full_animation_state((s_animation_transform const *)&state, false);
        ((c_gui_widget *)bitmap)->set_visible(ratio > 0.0f);
    }
}
