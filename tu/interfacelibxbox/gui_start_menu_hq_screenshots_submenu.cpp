// interfacelibxbox/gui_start_menu_hq_screenshots_submenu.obj: from the records 0x4b7458 0x4b7498 0x4b74d8 0x4b7560 0x4b7dd0 0x4b7ee8 0x4b7f80 0x4b7fd8 0x4b80d8
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "saved_games/content_catalogue.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include "interface/gui_widget.h"
#include <ppcintrinsics.h>
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "saved_games/autosave_queue.h"
#include "interface/gui_custom_bitmap_storage.h"


#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/gui_screens/start_menu/panes/hq_screenshots/gui_start_menu_hq_screenshots_submenu.h"
#include "networking/online/online_globals_types.h"
#include "interface/user_interface_clock.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/virtual_keyboard_task.h"
#include <new>

#include "interface/c_gui_list_item_widget.h"


#include "saved_games/autosave_queue_types.h"

struct c_start_menu_game_editor;
struct c_content_catalogue;
#include "saved_games/content_catalogue_types.h"
#include "tag_files/string_ids.h"
extern "C" {
// @address 0x51ce8 k_gui_start_menu_hq_screenshots_submenu_source_file
// @rdata 0x82051ce8..0x82051d4c
const char k_gui_start_menu_hq_screenshots_submenu_source_file[] =
    "..\\interface\\gui_screens\\start_menu\\panes\\"
    "hq_screenshots\\gui_start_menu_hq_screenshots_submenu.cpp";
}
// @rdata-end

static __forceinline c_gui_screenshot_selected_item *gui_start_menu_hq_screenshots_submenu_item_list_get(s_autosave_queue *list, long index)
{
    c_gui_screenshot_selected_item *result = 0;
    if (index >= 0 && index < list->item_count)
        result = &list->items[index];
    return result;
}





// @address 0x4b7458 ?initialize@c_gui_start_menu_hq_screenshots_submenu@@UAAXXZ
void c_gui_start_menu_hq_screenshots_submenu::initialize()
{
    c_gui_screen_widget::initialize();
    list = gui_custom_bitmap_storage_get_manager(get_any_responding_controller());
}

// @address 0x4b7498 ?dispose@c_gui_start_menu_hq_screenshots_submenu@@UAAXXZ
void c_gui_start_menu_hq_screenshots_submenu::dispose()
{
    c_gui_screen_widget::dispose();
    if (list)
        gui_custom_bitmap_storage_release_manager();
}

// @address 0x4b74d8 ?create_bitmap_widget@c_gui_screenshot_screen@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_screenshot_screen::create_bitmap_widget(
    const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _hq_screenshots_submenu_string_screenshots_preview)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
        if (memory != 0)
        {
            return new (memory) c_gui_custom_bitmap_widget();
        }
    }
    else
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
        if (memory != 0)
        {
            return new (memory) c_gui_bitmap_widget();
        }
    }
    return 0;
}

// @address 0x4b7560 ?update@c_gui_start_menu_hq_screenshots_submenu@@UAAXK@Z
void c_gui_start_menu_hq_screenshots_submenu::update(unsigned long current_milliseconds)
{
    wchar_t case4_title_text[1024];
    wchar_t case7_title_text[1024];
    wchar_t case4_description_text[1024];
    wchar_t case7_description_text[1024];
    c_gui_screen_widget::update(current_milliseconds);
    autosave_queue_update(list);
    switch (state) {
    case _hq_screenshots_submenu_state_confirm_delete: {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (!memory)
            break;
        long screen_name = m_name;
        c_load_dialog_screen_message *message = new (memory) c_load_dialog_screen_message(
            (e_controller_index)controller_index, get_render_window(),
            screen_name, _hq_screenshots_submenu_string_confirm_saved_game_delete, screen_name);
        if (!message)
            break;
        message->set_parent_screen_index(m_screen_index);
        user_interface_messaging_post((const c_message *)message);
        state = _hq_screenshots_submenu_state_idle;
        break;
    }
    case _hq_screenshots_submenu_state_delete_item: {
        bool result = autosave_queue_delete_item(list, selected_index);
        if (!result)
            break;
        unsigned long current_time = g_user_interface_globals.milliseconds.peek();
        timestamp = current_time;
        state = _hq_screenshots_submenu_state_wait_for_delete;
        break;
    }
    case _hq_screenshots_submenu_state_edit_title: {
        if (keyboard_task->busy())
            break;
        if (online_guide_is_active())
            break;
        case4_title_text[0] = 0;
        case4_description_text[0] = 0;
        if (!get_string_by_string_id(
                _hq_screenshots_submenu_string_screenshot_title_keyboard_name, (c_static_wchar_string<1024> *)case4_title_text))
            break;
        if (!get_string_by_string_id(
                _hq_screenshots_submenu_string_screenshot_title_keyboard_description,
                (c_static_wchar_string<1024> *)case4_description_text))
            break;
        gui_start_menu_hq_screenshots_submenu_get_item_name(this);
        keyboard_task->m_controller_index = (e_controller_index)controller_index;
        keyboard_task->set_default_text(name);
        keyboard_task->set_title_text(case4_title_text);
        keyboard_task->set_description_text(case4_description_text);
        keyboard_task->m_maximum_input_characters = 16;
        keyboard_task->m_character_flags = 0x4000;
        if (!overlapped_task_start_internal((c_overlapped_task *)keyboard_task, k_gui_start_menu_hq_screenshots_submenu_source_file, 165))
            break;
        state = _hq_screenshots_submenu_state_wait_for_title_keyboard;
        break;
    }
    case _hq_screenshots_submenu_state_wait_for_title_keyboard: {
        if (keyboard_task->m_controller_index < 0 || keyboard_task->m_controller_index >= 4 || keyboard_task->busy())
            break;
        if (keyboard_task->get_task_state() == 4) {
            c_content_catalogue *catalogue
                = (c_content_catalogue *)&content_catalogues().catalogues[get_any_responding_controller()];
            c_static_wchar_string<256> name_text(keyboard_task->m_result_text);
            content_catalogue_trim_whitespace((unsigned short *)name_text.m_string);
            if (content_catalogue_has_storage_device(catalogue) &&
                content_catalogue_is_item_name_valid(
                    catalogue, (e_controller_index)keyboard_task->m_controller_index, name_text.m_string, 14))
            {
                ustrnzcpy(name, name_text.m_string, 16);
                state = _hq_screenshots_submenu_state_save_title;
            } else
                state = _hq_screenshots_submenu_state_idle;
        } else
            state = _hq_screenshots_submenu_state_idle;
        keyboard_task->reset();
        keyboard_task->m_controller_index = _controller_index_none;
        return;
    }
    case _hq_screenshots_submenu_state_save_title: {
        if (!autosave_queue_set_item_text(list, selected_index, name, 0))
            break;
        void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
        if (memory) {
            c_load_in_progress_screen_message *message = new (memory) c_load_in_progress_screen_message(
                (e_controller_index)controller_index, get_render_window(),
                _hq_screenshots_submenu_string_updating_metadata_title, _hq_screenshots_submenu_string_updating_metadata_message);
            if (message) {
                message->set_parent_screen_index(m_screen_index);
                user_interface_messaging_post((const c_message *)message);
            }
        }
        unsigned long current_time = g_user_interface_globals.milliseconds.peek();
        timestamp = current_time;
        state = _hq_screenshots_submenu_state_wait_for_metadata_update;
        break;
    }
    case _hq_screenshots_submenu_state_edit_description: {
        if (keyboard_task->busy())
            break;
        if (online_guide_is_active())
            break;
        case7_title_text[0] = 0;
        case7_description_text[0] = 0;
        if (!get_string_by_string_id(
                _hq_screenshots_submenu_string_screenshot_description_keyboard_name, (c_static_wchar_string<1024> *)case7_title_text))
            break;
        if (!get_string_by_string_id(
                _hq_screenshots_submenu_string_screenshot_description_keyboard_description,
                (c_static_wchar_string<1024> *)case7_description_text))
            break;
        gui_start_menu_hq_screenshots_submenu_get_item_description(this);
        keyboard_task->m_controller_index = (e_controller_index)controller_index;
        keyboard_task->set_default_text(description);
        keyboard_task->set_title_text(case7_title_text);
        keyboard_task->set_description_text(case7_description_text);
        keyboard_task->m_maximum_input_characters = 128;
        keyboard_task->m_character_flags = 0x4000;
        if (!overlapped_task_start_internal((c_overlapped_task *)keyboard_task, k_gui_start_menu_hq_screenshots_submenu_source_file, 251))
            break;
        state = _hq_screenshots_submenu_state_wait_for_description_keyboard;
        break;
    }
    case _hq_screenshots_submenu_state_wait_for_description_keyboard: {
        if (keyboard_task->m_controller_index < 0 || keyboard_task->m_controller_index >= 4 || keyboard_task->busy())
            break;
        if (keyboard_task->get_task_state() == 4) {
            ustrnzcpy_inline(description, keyboard_task->m_result_text, 128);
            state = _hq_screenshots_submenu_state_save_description;
        } else
            state = _hq_screenshots_submenu_state_idle;
        keyboard_task->reset();
        keyboard_task->m_controller_index = _controller_index_none;
        return;
    }
    case _hq_screenshots_submenu_state_save_description: {
        if (!autosave_queue_set_item_text(list, selected_index, 0, description))
            break;
        void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
        if (memory) {
            c_load_in_progress_screen_message *message = new (memory) c_load_in_progress_screen_message(
                (e_controller_index)controller_index, get_render_window(),
                _hq_screenshots_submenu_string_updating_metadata_title, _hq_screenshots_submenu_string_updating_metadata_message);
            if (message) {
                message->set_parent_screen_index(m_screen_index);
                user_interface_messaging_post((const c_message *)message);
            }
        }
        unsigned long current_time = g_user_interface_globals.milliseconds.peek();
        state = _hq_screenshots_submenu_state_wait_for_metadata_update;
        timestamp = current_time;
        break;
    }
    case _hq_screenshots_submenu_state_wait_for_delete: {
        bool message_active = false;
        start_menu_game_editor_update_loading_message(this, list->busy != 0, &message_active);
        if (message_active)
            break;
        transition_out_with_transition_type(false, _screen_transition_normal);
        state = _hq_screenshots_submenu_state_idle;
        break;
    }
    case _hq_screenshots_submenu_state_wait_for_metadata_update: {
        bool message_active = false;
        start_menu_game_editor_update_loading_message(this, list->busy != 0, &message_active);
        if (message_active)
            break;
        transition_out_with_transition_type(false, _screen_transition_normal);
        state = _hq_screenshots_submenu_state_idle;
        break;
    }
    default:
        break;
    }
}

// @address 0x4b7dd0 ?handle_list_item_chosen@c_gui_start_menu_hq_screenshots_submenu@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_screenshots_submenu::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _hq_screenshots_submenu_string_screenshots_option) {
        long item_name;
        const bool &has_item_name = data->get_string_id_value(item->get_element_handle(), _string_id_name, &item_name);
        if (has_item_name) {
            switch (item_name) {
            case _hq_screenshots_submenu_string_cancel:
                transition_out_with_transition_type(false, _screen_transition_normal);
                handled = false;
                break;
            case _hq_screenshots_submenu_string_delete: {
                int controller = message->get_controller();
                handled = true;
                controller_index = controller;
                state = _hq_screenshots_submenu_state_confirm_delete;
                break;
            }
            case _hq_screenshots_submenu_string_edit_title: {
                int controller = message->get_controller();
                handled = true;
                state = _hq_screenshots_submenu_state_edit_title;
                controller_index = controller;
                break;
            }
            case _hq_screenshots_submenu_string_edit_description: {
                int controller = message->get_controller();
                handled = true;
                state = _hq_screenshots_submenu_state_edit_description;
                controller_index = controller;
                break;
            }
            }
        }
    }
    bool result = false;
    if ((unsigned int)(handled & 0xff) != 0) {
        result = handled;
    }
    return result;
}

// @address 0x4b7ee8 ?handle_controller_input_message@c_gui_start_menu_hq_screenshots_submenu@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_start_menu_hq_screenshots_submenu::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (state == _hq_screenshots_submenu_state_idle) {
        if (message->get_event_type() == _event_type_button_press && message->get_component() == 1) {
            transition_out_with_transition_type(true, _screen_transition_normal);
            handled = true;
        }
        if (!handled)
            handled = c_gui_screen_widget::handle_controller_input_message(message);
    }
    return handled;
}

// @address 0x4b7f80 ?handle_dialog_result@c_gui_start_menu_hq_screenshots_submenu@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_start_menu_hq_screenshots_submenu::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->get_dialog_name() == _hq_screenshots_submenu_string_confirm_saved_game_delete) {
        handled = true;
        switch (message->get_dialog_result()) {
        case _gui_dialog_choice_first:
            state = _hq_screenshots_submenu_state_delete_item;
            break;
        case _gui_dialog_choice_second:
            state = _hq_screenshots_submenu_state_idle;
            break;
        }
    }
    if (!handled)
        return c_gui_screen_widget::process_message(message);
    return handled;
}

// @address 0x4b7fd8 gui_start_menu_hq_screenshots_submenu_get_item_name
void gui_start_menu_hq_screenshots_submenu_get_item_name(c_gui_start_menu_hq_screenshots_submenu *screen)
{
    c_gui_screenshot_selected_item *item = gui_start_menu_hq_screenshots_submenu_item_list_get(screen->list, screen->selected_index);
    if (item) {
        wchar_t *name = screen->name;
        ustrnzcpy_inline(name, item->get_title(), 16);
    }
    else
        screen->name[0] = 0;
}

// @address 0x4b80d8 gui_start_menu_hq_screenshots_submenu_get_item_description
void gui_start_menu_hq_screenshots_submenu_get_item_description(c_gui_start_menu_hq_screenshots_submenu *screen)
{
    c_gui_screenshot_selected_item *item = gui_start_menu_hq_screenshots_submenu_item_list_get(screen->list, screen->selected_index);
    if (item) {
        wchar_t *name = screen->description;
        ustrnzcpy_inline(name, item->get_description(), 128);
    }
    else
        screen->description[0] = 0;
}
