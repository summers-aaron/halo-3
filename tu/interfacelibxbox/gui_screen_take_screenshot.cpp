// interfacelibxbox/gui_screen_take_screenshot.obj: from the records 0x4df578 0x4df6a8 0x4df940 0x4dfa78 0x4dfbf0 0x4dfcc8 0x4e05a0 0x4e0718 0x4e08d8 0x4e0a70 0x4e0f58 0x4e0f60 0x4e1040 0x4e1048 0x4e1080
// @flags /TP /O2 /Oc
#include "interface/gui_custom_bitmap_storage_manager.h"
#include "cseries/crt_xdk.h"
#include "scenario/scenario.h"
#include "tag_files/tag_types.h"
#include <stddef.h>
#include "shell/shell.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_widget.h"
#include "memory/static_arrays.h"
#include "main/levels_globals.h"
#include "interface/user_interface_clock.h"
#include "saved_games/content_catalogue_types.h"
// @address 0x4df578 gui_screen_take_screenshot_parse_campaign_difficulty

#include <ppcintrinsics.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_text_parser.h"
#include "rasterizer/rasterizer_main.h"
#include "game/game.h"
#include "interface/user_interface_utilities.h"
#include "saved_games/saved_film.h"
#include "saved_games/saved_game_files.h"
#include "game/game_globals.h"
#include "interface/gui_screens/director/gui_screen_take_screenshot.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "tag_files/file_reference.h"
#include "game/game_variant.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "interface/user_interface_shared_globals.h"
#include "scenario/scenario_tag.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "shell/shell_types.h"
#include "memory/data_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "input/input_abstraction_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "input/input_abstraction.h"
#include "interface/screenshot_capture_storage_types.h"
#include "interface/user_interface_allocation.h"

#include "interface/user_interface_messages.h"
#include "game/game_variant_inline.h"
#include "interface/gui_screens/saved_film/gui_screen_saved_film_control_pad.h"
#include <new>

#include "interface/gui_screen_take_screenshot_externs.h"
#include "networking/online/online_globals_types.h"
#include "cseries/async.h"
#include "saved_games/saved_film_globals.h"
// @address 0x94e3b0 g_saved_film_timer_globals




extern "C" {


// @address 0xf02d04 g_saved_film_slots


// @address 0x500d4 k_magic_string_difficulty
// @address 0x501ac k_game_details_tag_campaign_name
// @address 0x501ec k_magic_string_map_name
// @rdata 0x82055d04..0x82055d28
// @address 0x55d04 k_magic_string_date
const wchar_t k_magic_string_date[] = {
    '<', 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', '-', 'i', 'n', 'f', 'o', 0, 0, 0
};
// @rdata-end


}

s_screenshot_capture_storage g_screenshot_capture_storage;


#include "interface/c_gui_screen_widget.h"
#include "interface/gui_data.h"

#include "cseries/async_xoverlapped_types.h"
#include "interface/virtual_keyboard_task.h"
#include "interface/user_interface_window_manager.h"

#include "saved_games/saved_film_playback.h"
#include "cseries/async_helpers.h"

struct s_take_screenshot_write_task
{
    long state;
    e_controller_index controller_index;
    wchar_t *display_name;
    const void *buffer;
    unsigned long buffer_size;
    s_file_reference *file;
    long *content_item_index;
    c_synchronized_long *success;
#ifdef HALO3_64BIT
    char remaining[512];
#else
    char remaining[256];
#endif
};

#include "saved_games/saved_film_timer_control_types.h"

static __forceinline void take_screenshot_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    string->clear();
    const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
    if (globals && globals->global_strings.index != -1) {
        text_group_get_string(globals->global_strings.index, string_id, string);
    }
}

// 0x4df578
bool gui_screen_take_screenshot_parse_campaign_difficulty(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    short difficulty = tls_game_globals()->options.campaign_difficulty;
    switch (difficulty)
    {
    case 0:
        take_screenshot_global_string_get(281, &string);
        break;
    case 1:
        take_screenshot_global_string_get(173, &string);
        break;
    case 2:
        take_screenshot_global_string_get(282, &string);
        break;
    case 3:
        take_screenshot_global_string_get(283, &string);
        break;
    default:
        take_screenshot_global_string_get(173, &string);
        break;
    }
    if (string.length() > 0)
    {
        ustrnzcpy(buffer, string.m_string, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4df6a8 parse_xml_game_engine_type
bool parse_xml_game_engine_type(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    string.clear();
    if (tls_game_globals()->options.game_mode == 1)
    {
        take_screenshot_global_string_get(0x10075, &string);
    }
    else if (tls_game_globals()->options.game_mode == 2)
    {
        const c_game_variant *variant = current_game_variant();
        switch (variant->engine_type)
        {
        case 1:
            take_screenshot_global_string_get(233, &string);
            break;
        case 2:
            take_screenshot_global_string_get(235, &string);
            break;
        case 3:
            take_screenshot_global_string_get(236, &string);
            break;
        case 4:
            take_screenshot_global_string_get(237, &string);
            break;
        case 5:
            take_screenshot_global_string_get(242, &string);
            break;
        case 6:
            take_screenshot_global_string_get(239, &string);
            break;
        case 7:
            take_screenshot_global_string_get(238, &string);
            break;
        case 8:
            take_screenshot_global_string_get(240, &string);
            break;
        case 9:
            take_screenshot_global_string_get(234, &string);
            break;
        case _game_engine_type_zombiez:
            take_screenshot_global_string_get(241, &string);
            break;
        default:
            take_screenshot_global_string_get(249, &string);
            break;
        }
    }
    if (string.length() > 0)
    {
        ustrnzcpy(buffer, string.m_string, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4df940 gui_screen_take_screenshot_parse_map_name
bool gui_screen_take_screenshot_parse_map_name(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    string.clear();
    levels_map_datum level;
    scenario *sc = global_scenario;
    int map_id;
    s_data_array *data;
    if ((tls_game_globals()->options.game_mode == 1
        && (map_id = sc->map_id, data = g_level_globals.map_levels, levels_try_and_get_datum(data, map_id, &level))) ||
        (tls_game_globals()->options.game_mode == 2
        && (map_id = sc->map_id, data = g_level_globals.multiplayer_levels, levels_try_and_get_datum(data, map_id, &level))))
    {
        string.set(level.display_name);
    }
    if (string.length() > 0)
    {
        ustrnzcpy(buffer, string.m_string, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4dfa78 ?initialize@c_gui_screen_take_screenshot@@UAAXXZ
void c_gui_screen_take_screenshot::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_difficulty, this, gui_screen_take_screenshot_parse_campaign_difficulty));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_campaign_name, this, parse_xml_game_engine_type));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_map_name, this, gui_screen_take_screenshot_parse_map_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_date, this, user_interface_parse_date_string));
}

// @address 0x4dfbf0 ?dispose@c_gui_screen_take_screenshot@@UAAXXZ
void c_gui_screen_take_screenshot::dispose()
{
    c_gui_screen_widget::dispose();
    if (g_screenshot_capture_storage.slot != 0)
    {
        long count = g_saved_film_slots.streams[1].count - 1;
        g_saved_film_slots.streams[1].count = count;
        if (count == 0)
            g_saved_film_slots.streams[1].state = count;
        g_screenshot_capture_storage.slot = 0;
    }
    g_screenshot_capture_storage.slot_size = 0;
    g_screenshot_capture_storage.buffer = 0;
    g_screenshot_capture_storage.scratch_buffer = 0;
    void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
    if (memory != 0)
    {
        c_load_screen_message *message = new (memory)
            c_load_screen_message(0x100c2, k_any_controller, _console_window, get_name());
        if (message != 0)
        {
            message->set_focus_on_load_by_name(_take_screenshot_string_button_list, 0x10010, _film_control_save_primary);
            user_interface_messaging_post(message);
        }
    }
}

static __forceinline void gui_screen_take_screenshot_virtual_get_string(void *screen, long string_id, wchar_t *output)
{
    static_cast<c_gui_screen_widget *>(screen)->get_string_by_string_id(
        string_id, (c_static_wchar_string<1024> *)output);
}

static __forceinline void gui_screen_take_screenshot_virtual_reset(void *screen)
{
    static_cast<c_gui_screen_widget *>(screen)->transition_out_with_transition_type(0, _screen_transition_normal);
}

typedef char screenshot_capture_row_offset[(sizeof(void *) != 4 || ((offsetof(s_screenshot_capture_storage, row) == 0x2a) )) ? 1 : -1];
typedef char screenshot_capture_lock_offset[(sizeof(void *) != 4 || ((offsetof(s_screenshot_capture_storage, lock) == 0x34) )) ? 1 : -1];
typedef char screenshot_capture_storage_size[(sizeof(void *) != 4 || ((sizeof(s_screenshot_capture_storage) == 0x38) )) ? 1 : -1];

// @address 0x8c29c0 g_saved_film_playback
class c_content_catalogue;


static __forceinline unsigned long gui_screen_take_screenshot_milliseconds()
{
    unsigned long milliseconds = g_user_interface_globals.milliseconds.peek();
    return milliseconds;
}

static __forceinline void gui_screen_take_screenshot_copy_description(wchar_t *destination, const wchar_t *source)
{
    for (long index = 0; index <= 127; index++, source++)
    {
        if ((destination[index] = *source) == 0) break;
    }
    destination[127] = 0;
}

// @address 0x4dfcc8 ?update@c_gui_screen_take_screenshot@@UAAXK@Z
void c_gui_screen_take_screenshot::update(unsigned long time)
{
    s_take_screenshot_write_task task;
    switch (m_state)
    {
    case 2:
        {
            c_gui_screen_widget *progress = g_window_manager.get_screen_by_name(_screenshot_console_window, 0x100b3);
            if (progress) gui_screen_take_screenshot_virtual_reset(progress);
        }
        break;
    case 0:
        {
            bool captured = false;
            long width, height;
            if (rasterizer_main_get_video_mode()->fIsWideScreen)
            {
                width = 1920;
                height = 1080;
            }
            else
            {
                width = 1536;
                height = 1152;
            }
            if (gui_screen_take_screenshot_begin_capture(&g_screenshot_capture_storage, (short)width, (short)height))
            {
                if (game_is_film_playback_active()) saved_film_set_playback_game_speed(0.0f);
                captured = true;
            }
            if (captured) m_state = 1;
        }
        break;
    case 1:
        {
            s_screenshot_capture_storage *storage = &g_screenshot_capture_storage;
            bool ready = false;
            long fence = storage->lock.value;
            __lwsync();
            if (fence != 0) ready = D3DDevice_IsFencePending(fence) != 1;
            if (ready)
            {
                gui_custom_bitmap_storage_capture_screenshot(storage);
                m_state = 2;
            }
        }
        break;
    case 3:
        if (!m_keyboard->busy() && !online_guide_is_active())
        {
            c_static_wchar_string<1024> title_prompt;
            c_static_wchar_string<1024> title_help;
            if (get_string_by_string_id(_take_screenshot_string_screenshot_title_keyboard_name, &title_prompt) &&
                get_string_by_string_id(_take_screenshot_string_screenshot_title_keyboard_description, &title_help))
            {
                set_default_title();
                m_keyboard->set_controller_index(m_controller_index);
                m_keyboard->set_default_text(m_title);
                m_keyboard->set_title_text(title_prompt.m_string);
                m_keyboard->set_description_text(title_help.m_string);
                m_keyboard->set_maximum_input_characters(16);
                m_keyboard->set_character_flags(0x4000);
                if (overlapped_task_start_internal(m_keyboard, "..\\interface\\gui_screens\\director\\gui_screen_take_screenshot.cpp", 313))
                    m_state = 4;
            }
        }
        break;
    case 4:
        if ((long)m_keyboard->get_controller_index() >= 0 && (long)m_keyboard->get_controller_index() < 4 &&
            !m_keyboard->busy())
        {
            if (m_keyboard->get_task_state() == 4)
            {
                c_static_wchar_string<256> name(m_keyboard->get_keyboard_results());
                content_catalogue_trim_whitespace(reinterpret_cast<unsigned short *>(name.m_string));
                c_content_catalogue *catalogue =
                    &content_catalogues().catalogues[get_any_responding_controller()];
                if (content_catalogue_has_storage_device(catalogue)
                    && content_catalogue_is_item_name_valid(catalogue, m_keyboard->get_controller_index(), name.m_string, 14))
                {
                    ustrnzcpy(m_title, name.m_string, 16);
                    m_state = 5;
                }
                else m_state = 2;
            }
            else m_state = 2;
            m_keyboard->reset();
            m_keyboard->set_controller_index(k_no_controller);
        }
        break;
    case 5:
        if (!m_keyboard->busy() && !online_guide_is_active())
        {
            c_static_wchar_string<1024> description_prompt;
            c_static_wchar_string<1024> description_help;
            if (get_string_by_string_id(_take_screenshot_string_screenshot_description_keyboard_name, &description_prompt) &&
                get_string_by_string_id(_take_screenshot_string_screenshot_description_keyboard_description, &description_help))
            {
                set_default_description();
                m_keyboard->set_controller_index(m_controller_index);
                m_keyboard->set_default_text(m_description);
                m_keyboard->set_title_text(description_prompt.m_string);
                m_keyboard->set_description_text(description_help.m_string);
                m_keyboard->set_maximum_input_characters(128);
                m_keyboard->set_character_flags(0x4000);
                if (overlapped_task_start_internal(m_keyboard, "..\\interface\\gui_screens\\director\\gui_screen_take_screenshot.cpp", 379))
                    m_state = 6;
            }
        }
        break;
    case 6:
        if ((long)m_keyboard->get_controller_index() >= 0 && (long)m_keyboard->get_controller_index() < 4 &&
            !m_keyboard->busy())
        {
            if (m_keyboard->get_task_state() == 4)
            {
                gui_screen_take_screenshot_copy_description(m_description, m_keyboard->get_keyboard_results());
                m_state = 7;
            }
            else m_state = 3;
            m_keyboard->reset();
            m_keyboard->set_controller_index(k_no_controller);
        }
        break;
    case 7:
        if (g_saved_film_playback->state == 0)
        {
            c_load_in_progress_screen_message *message = new (0, 0, _ui_allocation_marker_dummy)
                c_load_in_progress_screen_message(m_controller_index,
                    get_render_window(), 0x100e9, 0x100ea);
            if (message)
            {
                message->set_parent_screen_index(m_screen_index);
                user_interface_messaging_post(message);
            }
            memset(&task, 0, sizeof(task));
            task.state = 0;
            task.controller_index = m_controller_index;
            task.display_name = m_title;
            s_screenshot_capture_storage *storage = &g_screenshot_capture_storage;
            char *buffer = 0;
            if (storage->valid)
            {
                buffer = storage->scratch_buffer;
                saved_film_encode_screenshot_header(buffer, storage->data_size, m_controller_index, 13, m_title,
                    m_description, (__int64)(long)storage->data_size, 0);
            }
            task.buffer = reinterpret_cast<const void *>(buffer);
            unsigned long buffer_size = 0;
            if (storage->valid) buffer_size = storage->data_size + 345;
            task.buffer_size = buffer_size;
            task.file = &m_file;
            task.content_item_index = &m_content_item_index;
            task.success = &m_task_success;
            m_task_index = async_task_add_ex((e_async_priority)_async_priority_blocking_generic,
                (s_async_task *)&task,
                (e_async_category)_async_category_saved_games,
                (e_async_completion (*)(s_async_task *))saved_game_files_write_screenshot,
                (c_synchronized_long *)&m_task_done, true);
            if (m_task_index != -1) m_state = 8;
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20056, 0, 0, m_controller_index, false, true);
                m_state = 2;
            }
        }
        break;
    case 8:
        if ((unsigned long)(gui_screen_take_screenshot_milliseconds() - m_started_time) > 1500 && m_task_done.peek())
        {
            m_task_index = -1;
            if (m_task_success.peek())
            {
                c_content_catalogue *catalogue = &content_catalogues().catalogues[m_controller_index];
                if (content_catalogue_has_storage_device(catalogue))
                {
                    s_screenshot_capture_storage *storage = &g_screenshot_capture_storage;
                    content_catalogue_set_item_metadata(catalogue, m_content_item_index,
                        &((const s_blffile_saved_game_file *)storage->scratch_buffer)->get_metadata());
                }
                saved_film_initialize_upload(g_saved_film_playback, m_controller_index, m_content_item_index, &m_file,
                    tls_game_globals()->options.game_instance);
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20056, 0, 0, m_controller_index, false, true);
                m_state = 2;
            }
            gui_screen_take_screenshot_virtual_reset(this);
        }
        break;
    }
    c_gui_screen_widget::update(time);
}

// @address 0x4e05a0 ?handle_list_item_chosen@c_gui_screen_take_screenshot@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_take_screenshot::handle_list_item_chosen(const c_controller_input_message *message,
    long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *datasource)
{
    bool handled = false;
    if (list_name == _take_screenshot_string_screenshot_option_list)
    {
        long code = -1;
        if (datasource->get_string_id_value(
                list_item_widget->get_element_handle(), 0x10010, &code) != 0)
        {
            handled = true;
            if (code - 0x10000 - 0x578 != 0)
            {
                long button = code - 0x10000 - 0x578;
                if ((unsigned int)button != 1)
                {
                    if ((unsigned int)button != 2)
                    {
                        handled = false;
                    }
                    else if (m_state == 2)
                    {
                        gui_screen_take_screenshot_virtual_reset(this);
                        long controller = message->m_controller;
                        if (controller != -1)
                        {
                            c_abstract_button *input_state = &input_abstraction_globals.input_states[controller].buttons[10];
                            input_state->latch();
                        }
                    }
                }
                else if (m_state == 2)
                {
                    m_controller_index = message->m_controller;
                    set_default_title();
                    set_default_description();
                    m_state = 7;
                }
            }
            else if (m_state == 2)
            {
                m_controller_index = message->m_controller;
                m_state = 3;
            }
        }
    }
    return handled;
}


static __forceinline void gui_screen_take_screenshot_copy_title(wchar_t *destination, const wchar_t *source)
{
    wchar_t *output = destination;
    long index = 0;
    do
    {
        wchar_t character = *source;
        output[0] = character;
        if (character == 0) break;
        ++source;
        character = *source;
        output[1] = character;
        if (character == 0) break;
        ++source;
        character = *source;
        output[2] = character;
        if (character == 0) break;
        ++source;
        character = *source;
        output[3] = character;
        if (character == 0) break;
        ++source;
        index += 4;
        output += 4;
    } while (index <= 15);
    destination[15] = 0;
}

// @address 0x4e0718 ?set_default_title@c_gui_screen_take_screenshot@@AAAXXZ
void c_gui_screen_take_screenshot::set_default_title()
{
    wchar_t text[1028];
    text[0] = 0;
    bool success = false;
    long string_id = 0;
    switch (tls_game_globals()->options.game_mode)
    {
    case 1:
    case 2:
        string_id = _take_screenshot_string_default_title_multiplayer;
        break;
    case 0:
        string_id = _take_screenshot_string_default_title_campaign;
        break;
    case 3:
        string_id = _take_screenshot_string_default_title_editor;
        break;
    default:
        break;
    }
    if (string_id != 0)
    {
        bool overflowed = false;
        memset(m_title, 0, sizeof(m_title));
        gui_screen_take_screenshot_virtual_get_string(this, string_id, text);
        if (user_interface_text_parse_xml(text, m_title, 16, &overflowed, m_screen_index) && !overflowed &&
            string_contains_special_characters(m_title) && user_interface_parse_string(-1, m_title, 16))
        {
            saved_game_files_find_unique_name(m_controller_index, 13, m_title, 16);
            success = true;
        }
    }
    if (!success)
        gui_screen_take_screenshot_copy_title(m_title, L"");
}

// @address 0x4e08d8 ?set_default_description@c_gui_screen_take_screenshot@@AAAXXZ
void c_gui_screen_take_screenshot::set_default_description()
{
    wchar_t text[1028];
    text[0] = 0;
    bool overflowed = true;
    long string_id = 0;
    switch (tls_game_globals()->options.game_mode)
    {
    case 1:
    case 2:
        string_id = _take_screenshot_string_default_description_multiplayer;
        break;
    case 0:
        string_id = _take_screenshot_string_default_description_campaign;
        break;
    case 3:
        string_id = _take_screenshot_string_default_description_editor;
        break;
    default:
        break;
    }
    if (string_id != 0)
    {
        wchar_t *buffer = m_description;
        memset(buffer, 0, sizeof(m_description));
        gui_screen_take_screenshot_virtual_get_string(this, string_id, text);
        user_interface_text_parse_xml(text, buffer, 128, &overflowed, m_screen_index);
        if (string_contains_special_characters(buffer) && !overflowed)
            overflowed = !user_interface_parse_string(-1, buffer, 128);
    }
    if (overflowed)
    {
        wchar_t *buffer = m_description;
        const wchar_t *source = L"";
        wchar_t *destination = buffer;
        long index = 0;
        do
        {
            wchar_t c0 = *source;
            destination[0] = c0;
            if (c0 == 0)
                break;
            source++;
            wchar_t c1 = *source;
            destination[1] = c1;
            if (c1 == 0)
                break;
            source++;
            wchar_t c2 = *source;
            destination[2] = c2;
            if (c2 == 0)
                break;
            source++;
            wchar_t c3 = *source;
            destination[3] = c3;
            if (c3 == 0)
                break;
            source++;
            index += 4;
            destination += 4;
        } while (index <= 127);
        buffer[127] = 0;
    }
}

struct D3DBaseTexture;



// @address 0x4e0a70 gui_screen_take_screenshot_begin_capture
extern "C" bool gui_screen_take_screenshot_begin_capture(s_screenshot_capture_storage *, short width, short height)
{
    s_screenshot_capture_storage *storage = &g_screenshot_capture_storage;
    bool result = false;
    if (storage->slot == 0)
    {
        storage->row = 0;
        storage->column = 0;
        storage->width = width;
        storage->height = height;
        if ((unsigned char)saved_film_is_update_pending() == 0)
        {
            storage->slot = saved_film_try_and_get_slot(&g_saved_film_slots, 1);
            if (storage->slot != 0)
            {
                long texture_width = (width + 63) & ~63;
                long texture_height = (height + 63) & ~63;
                storage->texture_size = XGSetTextureHeader(texture_width, texture_height, 1, 4, 0x18280086, 1, 0, -1, 0, 0, 0, 0);
                storage->rows_per_block = (short)(storage->texture_size / texture_height);
                storage->aligned_texture_size = (storage->texture_size + 0xfff) & ~0xfff;
                storage->slot_size = storage->slot[2];
                storage->scratch_size = 0x96000;
                storage->buffer = (storage->slot_size + 0xfff) & ~0xfff;
                // slot[2] holds the film memory block base address; scratch starts one aligned texture past it.
                storage->scratch_buffer = ((char **)storage->slot)[2] + storage->aligned_texture_size;
                storage->secondary_size = 0xa00000;
                storage->secondary_buffer = storage->scratch_buffer + storage->scratch_size;
                __lwsync();
                _InterlockedExchange(&storage->lock.value, 0);
                main_screenshot_start_capture(texture_width * 2, texture_height * 2, texture_width, texture_height, storage->buffer,
                    storage->texture_size, &storage->lock.value);
                result = true;
            }
        }
    }
    return result;
}

void c_gui_screen_saved_film_snippet::initialize()
{
    c_gui_screen_widget::initialize();
}

// @address 0x4e0f58 ?initialize_datasource@c_gui_screen_saved_film_snippet@@UAAXXZ
void c_gui_screen_saved_film_snippet::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
}

// @address 0x4e0f60 ?dispose@c_gui_screen_saved_film_snippet@@UAAXXZ
void c_gui_screen_saved_film_snippet::dispose()
{
    c_gui_screen_widget::dispose();
    void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
    if (memory == 0)
        return;
    c_load_screen_message *message = new (memory)
        c_load_screen_message(0x100c2, k_any_controller, _console_window, get_name());
    if (message == 0)
        return;
    long string_id;
    long state = saved_film_get_save_state();
    if (state == 5 || state == 6)
        string_id = _film_control_set_end;
    else if (!(g_saved_film_timer_globals.playback_speed <= 0.0f))
        string_id = _film_control_pause;
    else
        string_id = _film_control_play;
    message->set_focus_on_load_by_name(_take_screenshot_string_button_list, 0x10010, string_id);
    user_interface_messaging_post(message);
}

// @address 0x4e1040 ?update@c_gui_screen_saved_film_snippet@@UAAXK@Z
void c_gui_screen_saved_film_snippet::update(unsigned long time)
{
    c_gui_screen_widget::update(time);
}

// @address 0x4e1048 ?handle_controller_input_message@c_gui_screen_saved_film_snippet@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_saved_film_snippet::handle_controller_input_message(const c_controller_input_message *message)
{
    bool result = false;
    e_controller_component component = message->get_component();
    bool skip = !(component != _controller_component_button_b && component != _controller_component_button_y);
    if (!skip)
        result = c_gui_screen_widget::handle_controller_input_message(message);
    return result;
}

// @address 0x4e1080 ?handle_list_item_chosen@c_gui_screen_saved_film_snippet@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_saved_film_snippet::handle_list_item_chosen(const c_controller_input_message *message,
    long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *datasource)
{
    bool handled = false;
    if (list_name == _take_screenshot_string_snippet_option_list)
    {
        long selected_name = -1;
        if (datasource->get_string_id_value(list_item_widget->get_element_handle(), 0x10010, &selected_name))
        {
            handled = true;
            switch (selected_name)
            {
            case _take_screenshot_string_save_as:
                saved_film_handle_keyboard_save(message->m_controller);
                gui_screen_take_screenshot_virtual_reset(this);
                break;
            case _take_screenshot_string_save_auto:
                saved_film_handle_save_with_profile_name(message->m_controller);
                gui_screen_take_screenshot_virtual_reset(this);
                break;
            case _take_screenshot_string_snippet_discard:
                saved_film_handle_resume_without_save();
                gui_screen_take_screenshot_virtual_reset(this);
                break;
            case _take_screenshot_string_snippet_preview:
                if (saved_film_is_recording() && g_saved_film_recording_globals.state == 4)
                {
                    if (!saved_film_reset_cameras_to_authored())
                    {
                        s_saved_film_timer_globals *timer = &g_saved_film_timer_globals;
                        if (!timer->request_pending)
                        {
                            timer->request_pending = true;
                            timer->requested_state = 6;
                        }
                    }
                    else
                    {
                        saved_film_set_playback_rate(0.0f, 1);
                    }
                }
                gui_screen_take_screenshot_virtual_reset(this);
                break;
            default:
                handled = false;
                break;
            }
        }
    }
    return handled;
}
