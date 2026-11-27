// interfacelibxbox/start_menu_settings.obj: from the records 0x4ae8c0 0x4ae980 0x4b5a88 0x4b5a98 0x4b5b80 0x4b5c60 0x4b5d48 0x4b5e30 0x4b5f18 0x4b5f38 0x4b5f90 0x4b5fd0 0x4b6018 0x4b60c0 0x4b61a8 0x4b6380 0x4b6558 0x4b6640 0x4b6688 0x4b6708 0x4b6908 0x4b6a58 0x4b6c60 0x4b6d80 0x4b7040 0x4b7140 0x4b7280 0x4b7348 0x4b9028 0x4b9030 0x4b9150 0x4ba1d8 0x4ba2b0 0x4ba588
#include "interface/c_player_marketplace.h"
// @compiler xdk6130
// @flags /TP /O2
#include "interface/c_player_profile.h"
#include "interface/gui_custom_bitmap_storage_externs.h"
#include "shell/shell.h"
#include <stddef.h>
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z

#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "rasterizer/rasterizer_main.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "interface/gui_screens/start_menu/panes/settings/start_menu_settings.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance/start_menu_settings_appearance.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "interface/c_gui_widget.h"
#include "interface/user_interface_allocation.h"
#include "interface/custom_map_image_types.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "saved_games/autosave_queue_functions.h"
#include "saved_games/autosave_queue_types.h"

#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/virtual_keyboard_task.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/gui_data.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"

#include "interface/controller_types.h"
#include "interface/user_interface_utilities.h"
#include "interface/controller_inline.h"
#include "text/unicode_inline.h"

#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screen_widget_legacy.h"


typedef bool (*t_magic_string_callback)(void *context, wchar_t *output, long capacity);

// @address 0x51c78 ??_7s_start_menu_settings_datasource@@6B@
// @address 0x4c0ee0 ??_Gs_start_menu_settings_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Es_start_menu_settings_datasource@@UAAPAXI@Z

// @address 0x51c64 ??_7c_load_hq_screenshots_item_options_screen_message@@6B@

#include "interface/gui_screens/start_menu/panes/hq_screenshots/gui_start_menu_hq_screenshots_submenu.h"

#include "interface/c_gui_list_item_widget.h"

#include "interface/gui_list_widget_queries.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

#include <new>

struct s_autosave_slot_lookup
{
    s_autosave_slot_lookup() : slot(0), handle(-1) {}
    s_autosave_slot_lookup(const s_autosave_slot_lookup &other)
        : slot(other.slot), handle(other.handle) {}
    ~s_autosave_slot_lookup() {}
    c_gui_screenshot_selected_item *slot;
    long handle;
};
#include "interface/c_gui_list_item_widget.h"

#include "interface/gui_data.h"

static c_gui_error_manager *const g_error_manager = &g_gui_error_manager;

#include "interface/load_object_property_panel_screen_message.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"

#include "interface/virtual_keyboard_task.h"

// @address 0x4ae8c0 ?parse_xml_look_sensitivity@@YA_NPAXPA_WJ@Z
bool parse_xml_look_sensitivity(void *widget, wchar_t *buffer, long buffer_length)
{
    unsigned int controller_index = ((c_gui_widget *)widget)->get_any_responding_controller();
    bool success = false;
    if (controller_index <= 3 && ((c_controller_interface *)&g_controller_globals.controllers[controller_index])->in_use())
    {
        g_player_marketplace_users[controller_index].controller_index = controller_index;
        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
        if (user->signed_in && !user->consume_pending && user->offer_id != 0)
        {
            c_static_wchar_string<256> name(user->strings[0].get_string());
            name.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4ae980 ?parse_xml_look_centering@@YA_NPAXPA_WJ@Z
bool parse_xml_look_centering(void *widget, wchar_t *buffer, long buffer_length)
{
    unsigned int controller_index = ((c_gui_widget *)widget)->get_any_responding_controller();
    bool success = false;
    if (controller_index <= 3 && ((c_controller_interface *)&g_controller_globals.controllers[controller_index])->in_use())
    {
        g_player_marketplace_users[controller_index].controller_index = controller_index;
        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
        if (user->signed_in && !user->consume_pending && user->offer_id != 0)
        {
            c_static_wchar_string<256> name(user->strings[1].get_string());
            name.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4b5a88 ?apply_initial_state@c_load_hq_screenshots_item_options_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_hq_screenshots_item_options_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    static_cast<s_gui_start_menu_hq_screenshots_submenu *>(screen)->selected_index = m_selected_index;
}

// @address 0x4b5a98 parse_xml_focused_item_name
extern "C" bool parse_xml_focused_item_name(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    text.m_string[0] = 0;
    if (widget)
    {
        c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
        if (list)
        {
            c_gui_list_item_widget *item =
                (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, (list)->get_focused_element_handle());
            c_gui_data *data = ((c_gui_data *)(list)->get_data());
            if (data && item)
            {
                if (data->get_text_value(item->get_element_handle(), _string_id_title, &text))
                {
                    unsigned int length = buffer_length;
                    if (buffer_length > 1024)
                        length = 1024;
                    ustrnzcpy(buffer, text.m_string, length);
                    success = true;
                }
            }
        }
    }
    return success;
}

// @address 0x4b5b80 parse_xml_focused_item_description
extern "C" bool parse_xml_focused_item_description(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    text.m_string[0] = 0;
    c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    if (list)
    {
        c_gui_list_item_widget *item =
            (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, (list)->get_focused_element_handle());
        c_gui_data *data = ((c_gui_data *)(list)->get_data());
        if (data && item)
        {
            if (data->get_text_value(item->get_element_handle(), _string_id_description, &text))
            {
                unsigned int length = buffer_length;
                if (buffer_length > 1024)
                    length = 1024;
                ustrnzcpy(buffer, text.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

// @address 0x4b5c60 parse_xml_focused_item_author
extern "C" bool parse_xml_focused_item_author(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    text.m_string[0] = 0;
    c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    if (list)
    {
        c_gui_list_item_widget *item =
            (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, (list)->get_focused_element_handle());
        c_gui_data *data = ((c_gui_data *)(list)->get_data());
        if (data && item)
        {
            if (data->get_text_value(item->get_element_handle(), _start_menu_string_author, &text))
            {
                unsigned int length = buffer_length;
                if (buffer_length > 1024)
                    length = 1024;
                ustrnzcpy(buffer, text.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

// @address 0x4b5d48 parse_xml_focused_item_date
extern "C" bool parse_xml_focused_item_date(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    text.m_string[0] = 0;
    c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    if (list)
    {
        c_gui_list_item_widget *item =
            (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, (list)->get_focused_element_handle());
        c_gui_data *data = ((c_gui_data *)(list)->get_data());
        if (data && item)
        {
            if (data->get_text_value(item->get_element_handle(), _start_menu_string_date, &text))
            {
                unsigned int length = buffer_length;
                if (buffer_length > 1024)
                    length = 1024;
                ustrnzcpy(buffer, text.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

// @address 0x4b5e30 parse_xml_focused_item_size
extern "C" bool parse_xml_focused_item_size(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length)
{
    c_static_wchar_string<1024> text;
    bool success = false;
    text.m_string[0] = 0;
    c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    if (list)
    {
        c_gui_list_item_widget *item =
            (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, (list)->get_focused_element_handle());
        c_gui_data *data = ((c_gui_data *)(list)->get_data());
        if (data && item)
        {
            if (data->get_text_value(item->get_element_handle(), _start_menu_string_size, &text))
            {
                unsigned int length = buffer_length;
                if (buffer_length > 1024)
                    length = 1024;
                ustrnzcpy(buffer, text.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

// Bounds-checked row lookup; every datasource accessor re-checks the count
// through it after its own range check.
static __forceinline c_gui_screenshot_selected_item *start_menu_settings_queue_entry(s_autosave_queue *queue, long index)
{
    c_gui_screenshot_selected_item *result = 0;
    if (index < queue->item_count)
        result = &queue->items[index];
    return result;
}

// @address 0x4b5f18 ?get_current_item_count_internal@s_start_menu_settings_datasource@@MAAJXZ
long s_start_menu_settings_datasource::get_current_item_count_internal()
{
    long result = 0;
    if (queue)
        result = queue->item_count;
    return result;
}

// @address 0x4b5f38 ?initialize@s_start_menu_settings_datasource@@UAA_NJ@Z
bool s_start_menu_settings_datasource::initialize(long value)
{
    bool result = true;
    m_name = value;
    queue = gui_custom_bitmap_storage_get_manager(player_index);
    if (!queue)
        result = false;
    return result;
}

// @address 0x4b5f90 ?dispose@s_start_menu_settings_datasource@@UAAXXZ
void s_start_menu_settings_datasource::dispose()
{
    if (queue)
    {
        gui_custom_bitmap_storage_release_manager();
        queue = 0;
    }
}

// @address 0x4b5fd0 ?get_gui_selected_item@s_start_menu_settings_datasource@@UBAPBVc_gui_selected_item@@J@Z
const c_gui_selected_item *s_start_menu_settings_datasource::get_gui_selected_item(long element_handle) const
{
    const c_gui_screenshot_selected_item *result = 0;
    s_autosave_queue *list = queue;
    if (list && element_handle >= 0 && element_handle < list->item_count)
        result = start_menu_settings_queue_entry(queue, element_handle);
    return result;
}

// @address 0x4b6018 ?get_column_names@s_start_menu_settings_datasource@@UAAXQAJPAJ@Z
void s_start_menu_settings_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_name;
    (*column_count)++;
    column_names[*column_count] = _string_id_description;
    (*column_count)++;
    column_names[*column_count] = _hq_screenshots_submenu_string_image_index;
    (*column_count)++;
    column_names[*column_count] = _start_menu_string_author;
    (*column_count)++;
    column_names[*column_count] = _start_menu_string_date;
    (*column_count)++;
    column_names[*column_count] = _start_menu_string_size;
    (*column_count)++;
}

// @address 0x4b60c0 ?get_integer_value@s_start_menu_settings_datasource@@UAA_NJJPAJ@Z
bool s_start_menu_settings_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = NONE;
    s_autosave_queue *list = queue;
    if (list && element_handle >= 0 && element_handle < list->item_count)
    {
        c_gui_screenshot_selected_item *entry = start_menu_settings_queue_entry(queue, element_handle);
        switch (value_name)
        {
        case _start_menu_string_corrupt_file:
            *value = (unsigned char)entry->get_enabled() != 0;
            result = true;
            break;
        case _hq_screenshots_submenu_string_image_index:
            *value = element_handle + 1;
            result = true;
            break;
        }
    }
    return result;
}

// @address 0x4b61a8 ?get_text_value@s_start_menu_settings_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool s_start_menu_settings_datasource::get_text_value(
    long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    s_autosave_queue *list = queue;
    if (list && element_handle >= 0 && element_handle < list->item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(start_menu_settings_queue_entry(queue, element_handle)->get_title());
            return true;
        case _string_id_description:
            value->set(start_menu_settings_queue_entry(queue, element_handle)->get_description());
            return true;
        case _start_menu_string_author:
            value->set_char(start_menu_settings_queue_entry(queue, element_handle)->author);
            return true;
        case _start_menu_string_date:
            result = user_interface_get_date_string(start_menu_settings_queue_entry(queue, element_handle)->date, value);
            break;
        case _start_menu_string_size:
            return user_interface_get_bytes_size_string((long)start_menu_settings_queue_entry(queue, element_handle)->size_in_bytes, value);
        }
    }
    return result;
}

// @address 0x4b6380 ?initialize@s_gui_start_menu_settings_screenshots@@UAAXXZ
void s_gui_start_menu_settings_screenshots::initialize()
{
    gui_screen_widget_initialize(this);
    array = gui_custom_bitmap_storage_get_manager(get_any_responding_controller());
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<screenshot-title", this, (t_magic_string_callback)parse_xml_focused_item_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<screenshot-description", this, (t_magic_string_callback)parse_xml_focused_item_description));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<screenshot-author", this, (t_magic_string_callback)parse_xml_focused_item_author));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<screenshot-date", this, (t_magic_string_callback)parse_xml_focused_item_date));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<screenshot-size", this, (t_magic_string_callback)parse_xml_focused_item_size));
}

// @address 0x4b6558 ?initialize_datasource@s_gui_start_menu_settings_screenshots@@UAAXXZ
void s_gui_start_menu_settings_screenshots::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    // match: 0x4b6558: pointer __restrict qualifier prevents alias reload
    s_start_menu_settings_datasource *__restrict datasource = (s_start_menu_settings_datasource *)user_interface_malloc_tracked(
        sizeof(s_start_menu_settings_datasource), 0, 0);
    if (datasource)
    {
        int player_index = get_any_responding_controller();
        datasource = new (datasource) s_start_menu_settings_datasource(player_index);
        if (datasource->initialize(_start_menu_string_screenshots_list))
        {
            m_datasource[m_datasource_count] = datasource;
            m_datasource_count++;
        }
        else
        {
            datasource->~s_start_menu_settings_datasource();
            user_interface_free(datasource);
        }
    }
}

// @address 0x4b6c60 start_menu_settings_refresh_list_bitmaps
extern "C" void start_menu_settings_refresh_list_bitmaps(s_gui_start_menu_settings_screenshots *screen)
{
    if (screen->array)
    {
        c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
        c_gui_data *data = list ? ((c_gui_data *)(list)->get_data()) : 0;
        autosave_queue_update(screen->array);
        start_menu_settings_update_list_bitmaps(screen);
        if (list && data)
        {
            for (c_gui_widget *item = gui_widget_find_child_by_name((c_gui_widget *)list, 5); item;
                item = gui_widget_get_next_list_item(item, true))
            {
                long value;
                if (data->get_integer_value(
                        ((c_gui_list_item_widget *)item)->get_element_handle(), _start_menu_string_corrupt_file,
                        &value))
                {
                    bool visible = value != 0;
                    c_gui_widget *bitmap = item->get_child_widget(_gui_bitmap_widget, _start_menu_string_corrupt_file);
                    if (bitmap)
                        bitmap->set_visible(visible);
                }
            }
        }
    }
}

// @address 0x4b6640 ?dispose@s_gui_start_menu_settings_screenshots@@UAAXXZ
void s_gui_start_menu_settings_screenshots::dispose()
{
    c_gui_screen_widget::dispose();
    if (array)
    {
        gui_custom_bitmap_storage_release_manager();
        array = 0;
    }
}

// @address 0x4b6688 ?create_bitmap_widget@s_gui_start_menu_settings_screenshots@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *s_gui_start_menu_settings_screenshots::create_bitmap_widget(
    const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name != _start_menu_string_image && definition->name != _start_menu_string_screenshots_preview)
        return c_gui_widget::create_bitmap_widget(definition);
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
    if (memory != 0)
        return new (memory) c_gui_custom_bitmap_widget();
    return 0;
}

static __forceinline c_gui_screenshot_selected_item *start_menu_settings_slot_get(s_autosave_queue *array, int index)
{
    if (index >= 0 && index < array->item_count)
        return &array->items[index];
    return 0;
}

static __forceinline c_gui_screenshot_selected_item *start_menu_settings_find_slot(s_autosave_queue *array, long index)
{
    c_gui_screenshot_selected_item *result = 0;
    if (index >= 0 && index < array->item_count)
        result = &array->items[index];
    return result;
}

static __forceinline s_autosave_slot_lookup start_menu_settings_lookup_slot(s_autosave_queue *array, long index)
{
    s_autosave_slot_lookup result;
    if (index >= 0 && index < array->item_count)
        result.slot = &array->items[index];
    result.handle = result.slot ? result.slot->bitmap_list_item_index : -1;
    return result;
}

// @address 0x4b6708 ?handle_list_item_chosen@s_gui_start_menu_settings_screenshots@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool s_gui_start_menu_settings_screenshots::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    c_gui_widget *list_widget;
    s_autosave_queue *queue;
    int state;
    int element;
    c_gui_screenshot_selected_item *slot;
    bool handled = false;
    if (list == _start_menu_string_screenshots_list)
    {
        list_widget = get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
        if (list_widget != 0 && ((c_gui_data *)((c_gui_list_widget *)list_widget)->get_data()) != 0)
        {
            queue = array;
            if (queue != 0)
            {
                state = queue->busy;
                if (state == 0 || state == 1 || state == 2)
                {
                    if (gui_list_widget_find_item_by_element_handle((c_gui_list_widget *)list_widget,
                            ((c_gui_list_widget *)list_widget)->get_focused_element_handle()) != 0)
                    {
                        element = item->get_element_handle();
                        queue = array;
                        slot = 0;
                        if (element >= 0 && element < queue->item_count)
                            slot = &queue->items[element];
                        if (!slot->get_enabled() && (slot->bitmap_list_item_index != -1 || state != 0))
                        {
                            if (slot->bitmap_list_item_index == -1)
                            {
                                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                            }
                            else
                            {
#ifdef HALO3_64BIT
                                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_object_property_panel_screen_message), (char const *)(0), 0);
#else
                                void *memory = ::user_interface_malloc_tracked(64, (char const *)(0), 0);
#endif
                                if (memory != 0)
                                {
                                    long screen_value = m_name;
                                    e_controller_index controller = message->get_controller();
                                    e_window_index window = get_render_window();
                                    c_load_object_property_panel_screen_message *load = new (memory)
                                        c_load_object_property_panel_screen_message(0x100c8, controller, window, screen_value, element);
                                    load->set_parent_screen_index(m_screen_index);
                                    ::user_interface_messaging_post(load);
                                }
                            }
                        }
                        else
                        {
                            c_gui_error_manager *error_manager = &g_gui_error_manager;
                            error_manager->post_error_with_custom_message(0x20013, 0, 0, message->get_controller(), false, true);
                        }
                        handled = true;
                    }
                }
            }
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}

template<class T> class c_robust_pointer;

// @address 0x4b6d80 start_menu_settings_update_list_bitmaps
extern "C" void start_menu_settings_update_list_bitmaps(s_gui_start_menu_settings_screenshots *screen)
{
    if (!gui_widget_find_child_by_name(screen, 8) && screen->array)
    {
        c_gui_list_widget *list = static_cast<c_gui_list_widget *>(
            screen->get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list));
        if (list)
        {
            long scroll_position = list->m_scroll_position;
            bool forwards = screen->last_scroll_position <= scroll_position;
            screen->last_scroll_position = scroll_position;
            c_gui_widget *item;
            if (forwards)
                item = gui_widget_find_child_by_name(list, 5);
            else
            {
                c_gui_widget *child = list->get_children();
                c_gui_widget *last = 0;
                while (child)
                {
                    if (child->get_type() == 5)
                        last = child;
                    child = child->get_next();
                }
                item = last;
            }
            while (item)
            {
                c_gui_list_item_widget *list_item = static_cast<c_gui_list_item_widget *>(item);
                long index = list_item->get_element_handle();
                s_autosave_slot_lookup lookup = start_menu_settings_lookup_slot(screen->array, index);
                c_gui_screenshot_selected_item *slot = lookup.slot;
                long previous_handle = lookup.handle;
                c_gui_widget *previous;
                if (previous_handle != -1)
                {
                    long element_handle = list->get_element_handle_from_list_item_index(previous_handle);
                    if (element_handle != -1)
                        previous = gui_list_widget_find_item_by_element_handle(list, element_handle);
                    else
                        previous = 0;
                }
                else
                    previous = 0;
                if (item != previous)
                {
                    if (slot && !slot->get_enabled())
                    {
                        if (previous)
                            start_menu_settings_copy_item_bitmap(screen, previous, list_item);
                        else
                        {
                            long selected_index = screen->array->selection_valid ? screen->array->selected_index : -1;
                            if (selected_index == index)
                            {
                                if (!start_menu_settings_load_item_bitmap(screen, list_item))
                                    slot->mark_bitmap_load_failed();
                            }
                            else
                            {
                                start_menu_settings_reset_item_bitmap(screen, list_item);
                                autosave_queue_save_current_game_variant_to_queue(
                                    screen->array, static_cast<e_controller_index>(index));
                            }
                        }
                    }
                    else
                        start_menu_settings_reset_item_bitmap(screen, list_item);
                }
                item = forwards ? gui_widget_get_next_list_item(item, false) : gui_widget_get_previous_list_item(item, false);
            }
            for (long index = 0; index < screen->array->item_count; ++index)
            {
                c_gui_screenshot_selected_item *slot = start_menu_settings_find_slot(screen->array, index);
                c_gui_list_item_widget *item = (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, index);
                if (slot && !item)
                    slot->bitmap_list_item_index = -1;
            }
        }
    }
}

// @address 0x4b6908 ?handle_controller_input_message@s_gui_start_menu_settings_screenshots@@UAA_NPBVc_controller_input_message@@@Z
bool s_gui_start_menu_settings_screenshots::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_widget *list = get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    if (message->get_event_type() == 13)
    {
        switch (message->get_component())
        {
        case 1:
            close_current_subpane();
            handled = true;
            break;
        case 2:
            if (array != 0 && array->busy != 1)
            {
                c_gui_list_item_widget *item = (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(
                    (c_gui_list_widget *)list, ((c_gui_list_widget *)list)->get_focused_element_handle());
                if (item != 0)
                {
                    long element = item->get_element_handle();
                    if (element < array->item_count)
                    {
                        void *storage = ::user_interface_malloc_tracked(sizeof(c_load_hq_screenshots_item_options_screen_message), 0, 0);
                        if (storage != 0)
                        {
                            long screen_value = m_name;
                            e_controller_index controller = message->get_controller();
                            e_window_index window = get_render_window();
                            c_load_hq_screenshots_item_options_screen_message *load =
                                new (storage) c_load_hq_screenshots_item_options_screen_message(
                                    controller, window, screen_value, element);
                            load->set_parent_screen_index(m_screen_index);
                            ::user_interface_messaging_post(load);
                        }
                    }
                }
                handled = true;
            }
            break;
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4b6a58 ?update@s_gui_start_menu_settings_screenshots@@UAAXK@Z
void s_gui_start_menu_settings_screenshots::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _start_menu_string_screenshots_list);
    c_gui_custom_bitmap_widget *bitmap = (c_gui_custom_bitmap_widget *)get_child_widget(
        _gui_bitmap_widget, _start_menu_string_screenshots_preview);
    c_gui_list_item_widget *item =
        (c_gui_list_item_widget *)gui_list_widget_find_item_by_element_handle(list, list->get_focused_element_handle());
    c_gui_widget *button_key = get_child_widget(_gui_group_widget, _start_menu_string_screenshots_metadata);
    c_gui_widget *no_items_button_key = get_child_widget(_gui_group_widget, _start_menu_string_screenshots_help);
    bool busy = array && array->busy == 1;
    bool has_items = array && array->item_count > 0;
    start_menu_settings_refresh_list_bitmaps(this);
    if (bitmap)
        start_menu_settings_update_preview_bitmap(this, list, bitmap);
    if (busy)
    {
        if (button_key)
            button_key->set_visible(false);
        if (no_items_button_key)
            no_items_button_key->set_visible(!has_items);
    }
    else if (item && array)
    {
        long count = array->item_count;
        bool more_items = item->get_element_handle() < count;
        gui_screen_widget_set_display_group(this, 1, more_items ? 0x10056 : 0x10048);
    }
    if (button_key)
        button_key->set_visible(!busy && has_items);
    if (no_items_button_key)
        no_items_button_key->set_visible(!busy && !has_items);
}

// @address 0x42d298 ?controller_event_should_go_to_parent@s_gui_start_menu_settings_screenshots@@MAA_NPBVc_controller_input_message@@@Z

static __forceinline bool start_menu_settings_rasterizer_is_widescreen()
{
    return rasterizer_main_get_video_mode()->fIsWideScreen != 0;
}

// @address 0x4b7040 start_menu_settings_load_item_bitmap
extern "C" bool start_menu_settings_load_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item)
{
    bool result = false;
    c_gui_custom_bitmap_widget *bitmap =
        (c_gui_custom_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, _start_menu_string_image);
    if (bitmap && screen->array)
    {
        if (gui_custom_bitmap_storage_load_chunk_into_widget(screen->array, bitmap, false,
                !start_menu_settings_rasterizer_is_widescreen() + 1))
        {
            int current = list_item->get_element_handle();
            s_autosave_queue *array = screen->array;
            c_gui_screenshot_selected_item *slot = 0;
            if (current >= 0 && current < array->item_count)
                slot = &array->items[current];
            slot->bitmap_list_item_index = list_item->get_list_item_index();
            result = true;
        }
    }
    return result;
}

// @address 0x4b7140 start_menu_settings_copy_item_bitmap
extern "C" void start_menu_settings_copy_item_bitmap(s_gui_start_menu_settings_screenshots *screen,
    c_gui_widget *previous_widget, c_gui_list_item_widget *list_item)
{
    c_gui_custom_bitmap_widget *source = static_cast<c_gui_custom_bitmap_widget *>(previous_widget->get_child_widget(
        _gui_bitmap_widget, _start_menu_string_image));
    c_gui_custom_bitmap_widget *target = static_cast<c_gui_custom_bitmap_widget *>(
        list_item->get_child_widget(_gui_bitmap_widget, _start_menu_string_image));
    if (source == 0 || target == 0 || screen->array == 0)
        return;
    target->share_bitmap(source);
    for (int i = 0; i < screen->array->item_count; i++)
    {
        c_gui_screenshot_selected_item *slot = start_menu_settings_slot_get(screen->array, i);
        if (slot)
        {
            int id = slot->bitmap_list_item_index;
            if (id == list_item->get_list_item_index())
                slot->bitmap_list_item_index = -1;
        }
    }
    int current = list_item->get_element_handle();
    c_gui_screenshot_selected_item *slot = start_menu_settings_slot_get(screen->array, current);
    if (slot == 0)
        return;
    slot->bitmap_list_item_index = list_item->get_list_item_index();
}

// @address 0x4b7280 start_menu_settings_reset_item_bitmap
extern "C" void start_menu_settings_reset_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item)
{
    c_gui_custom_bitmap_widget *bitmap =
        (c_gui_custom_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, _start_menu_string_image);
    if (bitmap && screen->array)
    {
        bitmap->reset();
        for (int i = 0; i < screen->array->item_count; i++)
        {
            s_autosave_queue *array = screen->array;
            c_gui_screenshot_selected_item *slot = 0;
            if (i >= 0 && i < array->item_count)
                slot = &array->items[i];
            int id = slot->bitmap_list_item_index;
            if (id == list_item->get_list_item_index())
                slot->bitmap_list_item_index = -1;
        }
    }
}

// @address 0x4b7348 start_menu_settings_update_preview_bitmap
extern "C" void start_menu_settings_update_preview_bitmap(s_gui_start_menu_settings_screenshots *screen,
    c_gui_list_widget *list, c_gui_custom_bitmap_widget *bitmap)
{
    long element_handle = (list)->get_focused_element_handle();
    c_gui_list_item_widget *item;
    c_gui_custom_bitmap_widget *source;
    if (element_handle != -1)
    {
        item = static_cast<c_gui_list_item_widget *>(gui_list_widget_find_item_by_element_handle(list, element_handle));
        if (item)
            source = static_cast<c_gui_custom_bitmap_widget *>(item->get_child_widget(_gui_bitmap_widget, _start_menu_string_image));
        else
            source = 0;
    }
    else
    {
        item = 0;
        source = 0;
    }
    bool loaded = false;
    if (source && screen->array)
    {
        c_gui_screenshot_selected_item *slot = start_menu_settings_slot_get(screen->array, element_handle);
        if (slot)
        {
            long id = slot->bitmap_list_item_index;
            if (id == ((c_gui_list_item_widget *)item)->get_list_item_index())
            {
                bitmap->share_bitmap(source);
                loaded = true;
            }
        }
    }
    if (!loaded)
        bitmap->reset();
    bitmap->set_enabled(loaded);
}

// @address 0x4b9028 ?initialize@c_start_menu_settings@@UAAXXZ
void c_start_menu_settings::initialize()
{
    c_gui_screen_widget::initialize();
}

// @address 0x4b9030 ?update@c_start_menu_settings@@UAAXK@Z
void c_start_menu_settings::update(unsigned long current_milliseconds)
{
    int controller_name = start_menu_get_game_screen_name();
    int selected_name;
    if (controller_name != -1)
        selected_name = controller_name;
    else
        selected_name = _start_menu_string_top_menu_display_item0;
    c_gui_widget *selected = get_child_widget(_gui_text_widget, selected_name);
    c_gui_widget *child = get_child_widget(_gui_text_widget, _start_menu_string_top_menu_display_item0);
    if (child)
        child->set_visible(false);
    child = get_child_widget(_gui_text_widget, _start_menu_string_start_menu_game_campaign);
    if (child)
        child->set_visible(false);
    child = get_child_widget(_gui_text_widget, _start_menu_string_start_menu_game_cooperative);
    if (child)
        child->set_visible(false);
    child = get_child_widget(_gui_text_widget, _start_menu_string_start_menu_game_editor);
    if (child)
        child->set_visible(false);
    child = get_child_widget(_gui_text_widget, _start_menu_string_start_menu_game_multiplayer);
    if (child)
        child->set_visible(false);
    if (selected)
        selected->set_visible(true);
    c_start_menu_pane_screen_widget::update(current_milliseconds);
}

// @address 0x4b9150 ?handle_list_item_chosen@c_start_menu_settings@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_settings::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    game_globals_storage *session = tls_game_globals();
    bool handled = false;
    bool active = false;
    if (session != NULL)
    {
        if (session->game_in_progress != 0)
        {
            if (session->initializing == 0)
            {
                if (session->map_active != 0)
                    active = true;
            }
        }
    }
    bool mode = active && session->options.game_mode == 3;
    if (list == _start_menu_string_start_menu_items)
    {
        long choice = -1;
        if (data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &choice) &&
            choice == _start_menu_string_appearance)
        {
            if (!mode)
            {
                g_gui_error_manager.post_error_with_custom_message(0x2000f, 0, 0, message->get_controller(), false, true);
                handled = true;
            }
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}

static __forceinline bool start_menu_settings_request_type_is_passive(short type)
{
    return type == 1 || type == 2 || type == 3;
}

__forceinline void c_start_menu_settings_appearance::cancel_service_tag_request()
{
    if (m_virtual_keyboard_task->get_task_state() == 4)
        set_service_tag((e_controller_index)m_virtual_keyboard_task->m_controller_index,
            m_virtual_keyboard_task->get_keyboard_results());
    m_virtual_keyboard_task->reset();
    m_virtual_keyboard_task->set_controller_index(_controller_index_none);
}

// @address 0x4ba1d8 ?update@c_start_menu_settings_appearance@@UAAXK@Z
void c_start_menu_settings_appearance::update(unsigned long current_milliseconds)
{
    c_virtual_keyboard_task *request = m_virtual_keyboard_task;
    if (request && request->m_controller_index >= 0 && request->m_controller_index < 4)
    {
        if (!start_menu_settings_request_type_is_passive(request->get_task_state()))
            cancel_service_tag_request();
    }
    c_start_menu_pane_screen_widget::update(current_milliseconds);
}

static __forceinline unsigned char start_menu_settings_char_is_lowercase(int character)
{
    return character >= 'a' && character <= 'z';
}

static __forceinline int start_menu_settings_char_to_uppercase(int character)
{
    if (start_menu_settings_char_is_lowercase(character))
        return character - ('a' - 'A');
    return character;
}

static __forceinline void start_menu_settings_uppercase(char *string)
{
    for (long index = 0; index < 256; index++)
        string[index] = start_menu_settings_char_to_uppercase(string[index]);
}

// @address 0x4ba2b0 ?set_service_tag@c_start_menu_settings_appearance@@AAAXW4e_controller_index@@PB_W@Z
void c_start_menu_settings_appearance::set_service_tag(e_controller_index controller_index, const wchar_t *text)
{
    char ascii_code[4];
    wchar_t code[4];
    ustrnzcpy_inline(code, text, 4);
    ustrnupr(code, 4);
    wchar_string_to_ascii_string(code, ascii_code, 4, 0);
    wchar_t tag[4];
    ustrnzcpy_inline(tag, code, 4);
    if (c_player_profile_interface::is_service_tag_validly_formatted(tag))
    {
        c_static_wchar_string<1024> name;
        name.m_string[0] = 0;
        if (!get_string_by_string_id(0x2005e, &name))
            return;
        c_static_string<256> upper_name;
        upper_name.m_string[0] = 0;
        upper_name.set_wchar(name.m_string);
        start_menu_settings_uppercase(upper_name.m_string);
        if (upper_name.index_of(ascii_code) == -1)
        {
            if (!get_string_by_string_id(0x2005c, &name))
                return;
            c_static_string<256> upper_description;
            upper_description.m_string[0] = 0;
            upper_description.set_wchar(name.m_string);
            start_menu_settings_uppercase(upper_description.m_string);
            if (upper_description.index_of(ascii_code) == -1)
            {
                c_controller_interface *controller = controller_get(controller_index);
                c_player_profile_interface *profile = controller->get_player_profile_interface();
                if (profile)
                    profile->set_desired_service_tag(tag, true);
                return;
            }
            g_gui_error_manager.post_error_with_custom_message(0x2005c, 0, 0, (e_controller_index)controller_index, false, true);
        }
        else
        {
            g_gui_error_manager.post_error_with_custom_message(0x2005e, 0, 0, (e_controller_index)controller_index, false, true);
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x2005d, 0, 0, (e_controller_index)controller_index, false, true);
    }
}

static __forceinline bool start_menu_settings_unsigned_in_user(const c_controller_interface *controller)
{
    return (_rotl(controller->flags, 30) & 1) != 0;
}

// @address 0x4ba588 ?handle_list_item_chosen@c_start_menu_settings_appearance@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_settings_appearance::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    game_globals_storage *session = tls_game_globals();
    bool handled = false;
    bool active = false;
    if (session != NULL)
    {
        if (session->game_in_progress != 0)
        {
            if (session->initializing == 0)
            {
                if (session->map_active != 0)
                    active = true;
            }
        }
    }
    bool mode = active && session->options.game_mode == 3;
    if (list == _start_menu_string_start_menu_items)
    {
        long choice = -1;
        if (data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &choice))
        {
            if ((unsigned long)(choice - _start_menu_string_gender) != 0)
            {
                if ((unsigned long)(choice - _start_menu_string_gender) == (_start_menu_string_service_tag - _start_menu_string_gender))
                {
                    if (mode)
                    {
                        if (m_virtual_keyboard_task != 0 &&
                            !start_menu_settings_request_type_is_passive(m_virtual_keyboard_task->get_task_state()))
                        {
                            c_controller_interface *controller = controller_get(message->get_controller());
                            if (!start_menu_settings_unsigned_in_user(controller))
                            {
                                c_static_wchar_string<1024> title;
                                c_static_wchar_string<1024> description;
                                title.m_string[0] = 0;
                                description.m_string[0] = 0;
                                if (get_string_by_string_id(0x10112, &title) && get_string_by_string_id(0x10113, &description))
                                {
                                    c_player_profile_interface *profile = controller->get_player_profile_interface();
                                    m_virtual_keyboard_task->m_controller_index = message->get_controller();
                                    m_virtual_keyboard_task->set_default_text(profile->get_desired_service_tag());
                                    m_virtual_keyboard_task->set_title_text(title.m_string);
                                    m_virtual_keyboard_task->set_description_text(description.m_string);
                                    m_virtual_keyboard_task->m_maximum_input_characters = 4;
                                    m_virtual_keyboard_task->m_character_flags = 16392;
                                    if (overlapped_task_start_internal((c_overlapped_task *)m_virtual_keyboard_task,
                                        "..\\interface\\gui_screens\\start_menu\\panes\\"
                                        "settings_appearance\\start_menu_settings_appearance.cpp" /* __FILE__ */, 248))
                                        handled = true;
                                }
                            }
                            else
                            {
                                ::user_interface_play_sound(_ui_sound_error, -1);
                            }
                        }
                    }
                    else
                    {
                        g_gui_error_manager.post_error_with_custom_message(0x2000f, 0, 0, message->get_controller(), false, true);
                        handled = true;
                    }
                }
            }
            else
            {
                c_load_dialog_screen_message *dialog =
                    (c_load_dialog_screen_message *)::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                if (dialog != 0)
                {
                    long value = m_name;
                    e_controller_index controller = message->get_controller();
                    long dialog_name = _start_menu_string_settings_pick_gender;
                    c_load_dialog_screen_message *posted = new (dialog)
                        c_load_dialog_screen_message(controller, get_render_window(), value, dialog_name, value);
                    if (posted != 0)
                    {
                        posted->m_parent_screen_index = m_screen_index;
                        ::user_interface_messaging_post(posted);
                    }
                }
            }
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}
