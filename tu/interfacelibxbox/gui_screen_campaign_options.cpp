#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
#include "saved_games/autosave_queue_types.h"
// interfacelibxbox/gui_screen_campaign_options.obj: from the records 0x483578 0x4bfb60 0x4bfb90 0x4bfbc0 0x4bfc00 0x4bfc58 0x4bfdd0 0x4bfe10 0x4c0030 0x4c01d0 0x4c0270 0x4c0310 0x4c0380 0x4c0468 0x4c0500 0x4c0510 0x4c4b98
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "interface/c_gui_widget.h"
#include "cseries/crt_xdk.h"
#include "interface/gui_screen_campaign_options_externs.h"
#include "shell/shell.h"
#include <string.h>
#include <stddef.h>
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/utf8_types.h"
#include "interface/user_interface_utilities.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "simulation/simulation_view.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_options.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z

class c_font_cache;


#include <new>

#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/game_tag_parser.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/user_interface_error_manager.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_difficulty.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_level.h"
#include "interface/gui_screens/start_menu/panes/hq_screenshots/gui_start_menu_hq_screenshots_submenu.h"




#include "interface/gui_screens/campaign/campaign_select_level_message.h"



// @address 0x50cb0 ??_7c_load_campaign_select_difficulty_screen_message@@6B@
// @address 0x53c54 ??_7c_load_campaign_select_level_screen_message@@6B@
#include "interface/gui_text_widget_colors.h"




#include "saved_games/autosave_queue.h"


#include "interface/gui_screens/campaign/gui_screen_download_widget.h"

#include "interface/user_interface_allocation.h"


void c_load_campaign_select_difficulty_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    long w3 = m_difficulty;
    long w2 = m_map_id;
    long w1 = m_campaign_id;
    long w0 = m_campaign_setup_mode;
    static_cast<c_gui_screen_campaign_select_difficulty *>(screen)->m_mode = w0;
    static_cast<c_gui_screen_campaign_select_difficulty *>(screen)->m_campaign = w1;
    static_cast<c_gui_screen_campaign_select_difficulty *>(screen)->m_map = w2;
    static_cast<c_gui_screen_campaign_select_difficulty *>(screen)->m_initial_difficulty = w3;
}

// @address 0x4bfb60 gui_screen_campaign_options_get_name_text
extern "C" bool gui_screen_campaign_options_get_name_text(void *context, wchar_t *output, long capacity)
{
    static_cast<c_gui_screen_screenshot_viewer *>(context)->get_selected_item_title(output, capacity);
    return true;
}

// @address 0x4bfb90 gui_screen_campaign_options_get_description_text
extern "C" bool gui_screen_campaign_options_get_description_text(void *context, wchar_t *output, long capacity)
{
    static_cast<c_gui_screen_screenshot_viewer *>(context)->get_selected_item_description(output, capacity);
    return true;
}

// @address 0x4bfbc0 gui_screen_campaign_options_get_selected_index_text
extern "C" bool gui_screen_campaign_options_get_selected_index_text(void *context, wchar_t *output, long capacity)
{
    usnzprintf(output, capacity, L"%d", static_cast<c_gui_screen_campaign_options *>(context)->m_selected_index + 1);
    return true;
}

// @address 0x4bfc00 gui_screen_campaign_options_get_item_count_text
extern "C" bool gui_screen_campaign_options_get_item_count_text(void *context, wchar_t *output, long capacity)
{
    s_autosave_queue *list = static_cast<c_gui_screen_campaign_options *>(context)->m_queue;
    long count = list ? list->item_count : 0;
    usnzprintf(output, capacity, L"%d", count);
    return true;
}


// @address 0x4bfdd0 ?dispose@c_gui_screen_campaign_options@@UAAXXZ
void c_gui_screen_campaign_options::dispose()
{
    c_gui_screen_widget::dispose();
    if (m_queue)
        gui_custom_bitmap_storage_release_manager();
}

// @address 0x4c0030 ?handle_controller_input_message@c_gui_screen_campaign_options@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_campaign_options::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == 13)
    {
        switch (message->get_component() - 1)
        {
        case 4:
        {
            s_autosave_queue *list = m_queue;
            long count = 0;
            if (list)
                count = list->item_count;
            if (count > 0)
            {
                m_selected_index = m_selected_index - 1;
                count = list->item_count;
                if (!(m_selected_index >= 0 && m_selected_index < count))
                {
                    count = list->item_count;
                    m_selected_index = count - 1;
                }
            }
            handled = true;
            break;
        }
        case 3:
        {
            s_autosave_queue *list = m_queue;
            if (list && list->item_count > 0)
            {
                m_selected_index = m_selected_index + 1;
                if (!(m_selected_index >= 0 && m_selected_index < list->item_count))
                    m_selected_index = 0;
            }
            handled = true;
            break;
        }
        case 0:
            transition_out_with_transition_type(1, _screen_transition_normal);
            handled = true;
            break;
        case 1:
        {
            long index = m_selected_index;
            void *storage = ::user_interface_malloc_tracked(sizeof(c_load_hq_screenshots_item_options_screen_message), 0, 0);
            if (storage)
            {
                long screen_name = m_name;
                e_controller_index controller = message->get_controller();
                e_window_index window = get_render_window();
                c_load_hq_screenshots_item_options_screen_message *load =
                    new (storage) c_load_hq_screenshots_item_options_screen_message(controller, window, screen_name, index);
                load->set_parent_screen_index(m_screen_index);
                ::user_interface_messaging_post(load);
            }
            handled = true;
            break;
        }
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4bfc58 ?initialize@c_gui_screen_campaign_options@@UAAXXZ
void c_gui_screen_campaign_options::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<screenshot-title", static_cast<c_gui_screen_screenshot_viewer *>(this), gui_screen_campaign_options_get_name_text));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<screenshot-description", static_cast<c_gui_screen_screenshot_viewer *>(this),
        gui_screen_campaign_options_get_description_text));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<page-index", this, gui_screen_campaign_options_get_selected_index_text));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<page-count", this, gui_screen_campaign_options_get_item_count_text));
    m_queue = gui_custom_bitmap_storage_get_manager(get_any_responding_controller());
}

// @address 0x4c0270 ?get_selected_item_description@c_gui_screen_campaign_options@@UAAPA_WPA_WJ@Z
wchar_t *c_gui_screen_campaign_options::get_selected_item_description(wchar_t *output, long capacity)
{
    long index = m_selected_index;
    if (index >= 0)
    {
        s_autosave_queue *list = m_queue;
        long count = list->item_count;
        if (index < count && index < m_queue->item_count)
        {
            c_gui_screenshot_selected_item *item = &list->items[index];
            if (item)
            {
                ustrnzcpy(output, item->get_description(), capacity);
            }
        }
    }
    return output;
}

// @address 0x4c01d0 ?get_selected_item_title@c_gui_screen_campaign_options@@UAAPA_WPA_WJ@Z
wchar_t *c_gui_screen_campaign_options::get_selected_item_title(wchar_t *output, long capacity)
{
    long index = m_selected_index;
    if (index >= 0)
    {
        s_autosave_queue *list = m_queue;
        long count = list->item_count;
        if (index < count && index < m_queue->item_count)
        {
            c_gui_screenshot_selected_item *item = &list->items[index];
            if (item)
            {
                ustrnzcpy(output, item->get_title(), capacity);
            }
        }
    }
    return output;
}

// @address 0x4c0380 ?initialize@c_gui_screen_download_widget@@UAAXXZ
void c_gui_screen_download_widget::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<screenshot-title", static_cast<c_gui_screen_screenshot_viewer *>(this), gui_screen_campaign_options_get_name_text));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<screenshot-description", static_cast<c_gui_screen_screenshot_viewer *>(this),
        gui_screen_campaign_options_get_description_text));
}

// @address 0x4a8e80 ?dispose@c_gui_screen_download_widget@@UAAXXZ
void c_gui_screen_download_widget::dispose()
{
    c_gui_screen_widget::dispose();
}

// @address 0x4c0500 ?get_selected_item_title@c_gui_screen_download_widget@@UAAPA_WPA_WJ@Z
wchar_t *c_gui_screen_download_widget::get_selected_item_title(wchar_t *output, long capacity)
{
    return ustrnzcpy(output, m_description.metadata.name, capacity);
}

// @address 0x4c0510 ?get_selected_item_description@c_gui_screen_download_widget@@UAAPA_WPA_WJ@Z
wchar_t *c_gui_screen_download_widget::get_selected_item_description(wchar_t *output, long capacity)
{
    utf8_string_to_wchar_string((::utf8 const *)m_description.metadata.description, output, capacity, 0);
    return output;
}

void c_load_campaign_select_level_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    // match: 0x4c4b98: __restrict keeps the message fields in registers across the destination stores (no alias reloads)
    c_gui_screen_campaign_select_level *__restrict destination = static_cast<c_gui_screen_campaign_select_level *>(screen);
    destination->m_map_id = m_map_id;
    destination->m_campaign_id = m_campaign_id;
    destination->m_mode = m_campaign_setup_mode;
}



// @address 0x4c0310 gui_screen_download_delete
extern "C" c_gui_widget *gui_screen_download_delete(c_gui_widget *p0, unsigned int p1)
{
    c_http_buffer_downloader *downloader = &static_cast<c_gui_screen_download_widget *>(p0)->m_content_downloader;
    downloader->c_http_buffer_downloader::~c_http_buffer_downloader();
    downloader = &static_cast<c_gui_screen_download_widget *>(p0)->m_manifest_downloader;
    downloader->c_http_buffer_downloader::~c_http_buffer_downloader();
    p0->c_gui_widget::~c_gui_widget();
    if ((p1 & 1) != 0) {
        RtlFreeHeap(GetProcessHeap(), 0, p0);
    }
    return p0;
}



// @address 0x4c0468 ?update@c_gui_screen_download_widget@@UAAXK@Z
void c_gui_screen_download_widget::update(unsigned long current_milliseconds)
{
    c_gui_custom_bitmap_widget *bitmap =
        static_cast<c_gui_custom_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, _hq_screenshots_submenu_string_screenshots_preview));
    c_gui_screen_widget::update(current_milliseconds);
    gui_screen_download_widget_update(this);
    bool visible;
    if (m_download_state == 3 && (!bitmap || bitmap->bitmap_index == -1))
        visible = false;
    else
        visible = true;
    c_gui_widget *widget = get_child_widget(_gui_group_widget, _string_id_in_progress);
    if (widget)
        widget->set_visible(visible);
}


static __forceinline long gui_screen_campaign_options_item_count(const s_autosave_queue *list)
{
    long count;
    if (list)
        count = list->item_count;
    else
        count = 0;
    return count;
}

// @address 0x4bfe10 ?update@c_gui_screen_campaign_options@@UAAXK@Z
void c_gui_screen_campaign_options::update(unsigned long current_milliseconds)
{
    c_gui_custom_bitmap_widget *bitmap =
        static_cast<c_gui_custom_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, _hq_screenshots_submenu_string_screenshots_preview));
    c_gui_screen_widget::update(current_milliseconds);
    autosave_queue_update(m_queue);
    bool visible;
    if (m_queue->busy == 0 && (!bitmap || bitmap->bitmap_index == -1))
        visible = false;
    else
        visible = true;
    c_gui_widget *widget = get_child_widget(_gui_group_widget, _string_id_in_progress);
    if (widget)
        widget->set_visible(visible);
    long index = m_selected_index;
    if (index < 0 || index >= gui_screen_campaign_options_item_count(m_queue)) {
        if (m_queue->busy == 0) {
            long count = gui_screen_campaign_options_item_count(m_queue);
            if (count > 0)
                m_selected_index = count - 1;
            else
                m_selected_index = -1;
        }
        m_loaded_index = -1;
    }
    long selected = m_selected_index;
    if (selected != -1) {
        if (selected != m_loaded_index && bitmap) {
            s_autosave_queue *list = m_queue;
            long current = list->selection_valid ? list->selected_index : -1;
            if (current == selected) {
                if (gui_custom_bitmap_storage_load_chunk_into_widget(list, bitmap, true, 1)) {
                    m_loaded_index = m_selected_index;
                } else {
                    e_controller_index controller = get_any_responding_controller();
                    g_gui_error_manager.post_error_with_custom_message(0x20013, 0, 0, controller, false, true);
                    transition_out_with_transition_type(0, _screen_transition_normal);
                    m_selected_index = -1;
                }
            } else {
                ::autosave_queue_save_current_game_variant_to_queue(list, (e_controller_index)selected);
                memset(bitmap->m_map_image, 0, sizeof(bitmap->m_map_image));
            }
        }
    } else {
        transition_out_with_transition_type(0, _screen_transition_normal);
    }
}
