// interfacelibxbox/start_menu_game_editor.obj: from the records 0x284d70 0x4ab778 0x4ab780 0x4ab928 0x4abd30 0x4abdc8 0x4ac028 0x4ac1e0 0x4ac7d8 0x4acae8 0x4ad220 0x4ad310 0x4ad4f0 0x4ad848 0x4ad8e0 0x4ad918 0x4ad968 0x4ad988 0x4ada80 0x4b7c78
// @compiler xdk6130
#include "saved_games/content_catalogue_device_selection.h"
#include "saved_games/content_item_status_types.h"
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_list_widget.h"
#include "interface/user_interface_allocation.h"
#include "saved_games/game_content_types.h"
#include "saved_games/content_critical_section_types.h"
#include "game/game_engine_scoring_globals.h"
#include "interface/user_interface_clock.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "interface/controller_inline.h"
#include "scenario/scenario.h"
// @flags /TP /O2 /Oc
// @address 0x2251c k_real_zero_storage
// @address 0x11401e8 g_controller_globals
// @address 0x96e058 ?g_level_globals@@3Us_levels_globals@@A
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_inline.h"
#include "math/real_math_constants.h"
#include "multithreading/synchronized_value.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor_externs.h"
#include "interface/gui_screens/alpha/gui_screen_alpha_motd.h"
#include "interface/gui_widget_types.h"
#include <string.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "game/game_globals.h"
#include "multithreading/synchronization.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "networking/logic/network_life_cycle.h"
#include "tag_files/files.h"
#include "game/game_engine.h"
#include "game/game_engine_variant.h"
#include "interface/gui_screens/start_menu/panes/game_multiplayer/start_menu_game_multiplayer.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_utilities.h"
#include "main/game_preferences.h"
#include "networking/logic/network_session_interface.h"
#include "saved_games/saved_game_files.h"
#include "scenario/scenario_map_variant.h"
#include "animations/animation_manager.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/saved_game_files_context_types.h"
#include "game/map_variant.h"
#include "interface/user_interface_error_manager.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/virtual_keyboard_task.h"
#include "cache/cache_files.h"

#include "saved_games/saved_game_variant_file_types.h"
#include "interface/gui_screens/start_menu/panes/hq_screenshots/gui_start_menu_hq_screenshots_submenu.h"

#include "saved_games/content_item_types.h"
#include "interface/user_interface_messages.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include <new>
#include "interface/controller_types.h"
#include "networking/session/network_session_class.h"
#include "game/game_engine_util.h"
#include "game/map_variant_object_properties_types.h"
#include "main/levels_globals.h"
#include "game/game_engine_sandbox_dependencies.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "memory/datum_index.h"

struct s_start_menu_game_editor_save_request
{
    int mode;
    s_start_menu_game_editor_saved_settings setup;
    unsigned char remaining_settings[0xfa40 - sizeof(s_start_menu_game_editor_saved_settings)];
};

struct s_start_menu_game_editor_preferences
{
    char header[0xe538];
    s_start_menu_game_editor_saved_settings saved_setup;
};

struct s_start_menu_game_editor_saved_context
{
    long index;
    const wchar_t *name;
    long zero8;
    long zero12;
    long handle;
    e_campaign_id campaign_id;
};

class c_load_game_editor_variant_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_game_editor_variant_screen_message(
        e_controller_index controller, e_window_index window, c_start_menu_game_editor *screen, bool save_as)
        : c_load_screen_message(0x100ca, controller, window, _string_id_top_most)
    {
        m_screen = screen;
        m_save_as = save_as;
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    c_start_menu_game_editor *m_screen;
    bool m_save_as;
};

class c_gui_gametype_category_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_gametype_category_datasource() : m_number_of_gametypes(0) {}
    virtual void get_column_names(long *const column_names, long *column_count) { *column_count = 0; }
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool initialize(long name);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
protected:
    virtual long get_current_item_count_internal();
private:
    c_static_array<e_game_engine_type, 32> m_gametypes;
    long m_number_of_gametypes;
};

typedef char game_editor_category_size[(sizeof(void *) != 4 || ((sizeof(c_gui_gametype_category_datasource) == 0x190))) ? 1 : -1];

#include "game/game_engine_string_types.h"

#include "game/game_engine_variant_display.h"


#include "game/game_engine_settings_definitions.h"


#include "saved_games/content_catalogue_types.h"



class c_message;
struct utf8;
class c_content_catalogue;
extern "C" {


}
#include "saved_games/saved_game_scope_lock.h"




long c_gui_gametype_category_datasource::get_current_item_count_internal()
{
    return m_number_of_gametypes;
}

bool c_gui_gametype_category_datasource::get_element(long element_handle, void *element, long element_size)
{
    memcpy(element, &m_gametypes[element_handle], sizeof(e_game_engine_type));
    return true;
}

bool c_gui_gametype_category_datasource::initialize(long name)
{
    bool success = c_gui_data::initialize(name);
    if (success)
    {
        short number_of_gametypes = 0;
        for (e_game_engine_type type = _game_engine_none; type < k_game_engine_type_count; type = static_cast<e_game_engine_type>(type + 1))
        {
            if (type != _game_engine_unused)
            {
                m_gametypes[number_of_gametypes] = type;
                number_of_gametypes++;
            }
        }
        m_number_of_gametypes = number_of_gametypes;
    }
    return success;
}

bool c_gui_gametype_category_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    *value = m_gametypes[element_handle];
    return true;
}








// @address 0x4ad988 ?get_text_value@c_gui_gametype_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_gametype_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *text)
{
    c_static_wchar_string_1024 *value = (c_static_wchar_string_1024 *)text;
    e_game_engine_type gametype = m_gametypes[element_handle];
    if (gametype != _game_engine_none)
    {
        c_game_variant variant;
        game_engine_variant_initialize_default(&variant, gametype);
        ((c_game_engine_base_variant *)variant.m_variant_storage)->get_game_engine_name((c_static_wchar_string<1024> *)value);
    }
    else
    {
        value->string[0] = 0;
        if (global_game_globals != 0 && global_game_globals->multiplayer_globals_index != -1)
        {
            const cache_file_tag_instance *instance =
                &global_tag_instances[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)];
            s_tag_block *block = &((s_multiplayer_globals_definition *)instance->definition)->universal;
            if (block->count > 0)
                text_group_get_string(((s_multiplayer_universal_globals_definition *)block->address)->strings_index, 0x3db, value);
        }
    }
    return true;
}



static __forceinline unsigned long start_menu_game_editor_milliseconds()
{
    unsigned long value = g_user_interface_globals.milliseconds.peek();
    return value;
}


#pragma pack(push, 4)

class c_controller_input_message;
class c_dialog_result_message;

// @address 0x284d70 ?load_progress_ui@c_start_menu_game_editor@@IAA_NW4e_controller_index@@PAUs_file_reference@@@Z
bool c_start_menu_game_editor::load_progress_ui(e_controller_index controller_index, s_file_reference *reference)
{
    bool success = false;
    c_saved_game_scope_lock lock;
    const c_content_catalogue *catalogue = static_cast<const c_content_catalogue *>(static_cast<const void *>(this));
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *content_item = content_catalogue_try_and_get_item(catalogue, static_cast<long>(controller_index));
        if (content_item != 0)
            success = content_item->get_file_reference(reference);
    }
    return success;
}

struct s_game_engine_globals;

static __forceinline c_map_variant *start_menu_game_editor_game_variant() { return &game_engine_globals->map_info; }
static __forceinline const wchar_t *start_menu_game_editor_variant_name(const c_map_variant *variant) { return variant->m_metadata.name; }
static __forceinline c_content_catalogue *start_menu_game_editor_catalogue(int controller) { return &content_catalogues().catalogues[controller]; }

// @address 0x4ad220 start_menu_game_editor_request_save_variant
extern "C" void start_menu_game_editor_request_save_variant(c_start_menu_game_editor *p0, const c_controller_input_message *p1)
{
    c_content_catalogue *catalogue = start_menu_game_editor_catalogue(p1->m_controller);
    const wchar_t *variant_name = start_menu_game_editor_variant_name(start_menu_game_editor_game_variant());
    if (catalogue->try_and_get_personal_storage_item(variant_name, (e_content_item_status)_game_content_type_map) != -1)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory != 0)
        {
            long screen_name = p0->m_name;
            e_controller_index controller = p1->m_controller;
            c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(
                controller, p0->get_render_window(), screen_name, _start_menu_string_confirm_saved_game_overwrite, screen_name);
            if (dialog != 0)
            {
                dialog->m_parent_screen_index = p0->m_screen_index;
                ::user_interface_messaging_post(dialog);
                p0->m_dialog_action = 0;
                p0->m_save_state = 0;
            }
        }
    }
    else
    {
        start_menu_game_editor_begin_save_variant(p0, p1->m_controller);
        p0->m_save_state = -1;
        p0->m_dialog_action = 0;
    }
}


extern "C" {

}

// @address 0x4ad310 start_menu_game_editor_begin_save_variant
extern "C" void start_menu_game_editor_begin_save_variant(c_start_menu_game_editor *editor, int controller_index)
{
    wchar_t variant_description[128];
    s_file_reference reference;
    if (editor->m_operation != 0 || editor->m_write_variant_task != -1)
        return;
    c_content_catalogue *catalogue = &content_catalogues().catalogues[
        controller_get((e_controller_index)controller_index)->get_controller_index()];
    if (content_catalogue_has_storage_device(catalogue)) {
        const s_start_menu_game_editor_preferences *preferences =
            (const s_start_menu_game_editor_preferences *)global_preferences_get_game_setup_storage();
        editor->m_saved_setup = preferences->saved_setup;
        if (editor->m_saved_setup.file.has_path != 0 && editor->m_saved_setup.file.type == 1) {
            s_file_reference *file = file_reference_create_from_path(&reference, editor->m_saved_setup.file.path, false);
            long catalogue_item_index = content_catalogue_find_item_by_file_reference(
                &content_catalogues().catalogues[controller_index], file);
            editor->m_content_item.set(catalogue_item_index);
            if (editor->m_content_item.peek() != -1) {
                const c_map_variant *variant = &game_engine_globals->map_info;
                utf8_string_to_wchar_string((const utf8 *)variant->m_metadata.description, variant_description, 128, 0);
                start_menu_game_editor_build_map_variant_file(editor, &editor->m_variant_file, variant->m_metadata.name,
                    variant_description, variant->m_map_id, variant);
                long write_catalogue_item_index = editor->m_content_item.peek();
                editor->m_write_variant_task = saved_game_files_write_content_item(controller_index,
                    write_catalogue_item_index, 10, &editor->m_variant_file, sizeof(editor->m_variant_file),
                    &editor->m_write_succeeded, &editor->m_write_complete);
            }
        }
        if (editor->m_write_variant_task != -1) {
            editor->m_operation = 1;
            editor->m_operation_controller = (e_controller_index)controller_index;
            start_menu_game_editor_post_progress_message(editor, (e_controller_index)controller_index);
        }
    } else {
        user_interface_show_device_selection_dialog(controller_index, editor->get_name(), editor->get_screen_index());
    }
}

// @address 0x4ad848 start_menu_game_editor_post_progress_message
extern "C" void start_menu_game_editor_post_progress_message(c_start_menu_game_editor *editor, e_controller_index controller)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
    if (memory != 0)
    {
        c_load_in_progress_screen_message *message = new (memory) c_load_in_progress_screen_message(controller, editor->get_render_window(), 0x100dd, 0x100de);
        if (message != 0)
        {
            message->set_parent_screen_index(editor->get_screen_index());
            ::user_interface_messaging_post(message);
            editor->m_progress_message_start_time = start_menu_game_editor_milliseconds();
        }
    }
}

// @address 0x4b7c78 start_menu_game_editor_update_loading_message
extern "C" void start_menu_game_editor_update_loading_message(s_gui_start_menu_hq_screenshots_submenu *editor, bool loading, bool *message_active)
{
    if (!loading)
        editor->timestamp = -1;
    if (editor->timestamp != -1)
    {
        unsigned long now = start_menu_game_editor_milliseconds();
        if (now > (unsigned long)(editor->timestamp + 500))
        {
            editor->timestamp = -1;
            editor->message_shown_time = start_menu_game_editor_milliseconds();
            void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
            c_load_in_progress_screen_message *message = memory
                ? new (memory) c_load_in_progress_screen_message((e_controller_index)editor->controller_index, editor->get_render_window(), 0x100eb, 0x100ec)
                : 0;
            if (message != 0)
            {
                message->set_parent_screen_index(editor->get_screen_index());
                ::user_interface_messaging_post(message);
            }
        }
    }
    if (editor->message_shown_time == -1
        || start_menu_game_editor_milliseconds() > (unsigned long)(editor->message_shown_time + 3000))
    {
        c_gui_screen_widget *message_screen = g_window_manager.get_screen_by_name(_screenshot_console_window, _string_id_in_progress);
        editor->message_shown_time = -1;
        if (message_screen != 0)
            message_screen->transition_out_with_transition_type(0, _screen_transition_normal);
    }
    if (message_active != 0)
        *message_active = editor->timestamp != -1 || editor->message_shown_time != -1;
}

// @address 0x50f70 ??_7c_start_menu_game_editor@@6B@
// @address 0x4a8838 ??_Ec_start_menu_game_editor@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_start_menu_game_editor@@UAAPAXI@Z
// @address 0x42d380 ?controller_event_should_go_to_parent@c_start_menu_game_editor@@MAA_NPBVc_controller_input_message@@@Z

// @address 0x4ab780 ??0c_start_menu_game_editor@@QAA@J@Z
c_start_menu_game_editor::c_start_menu_game_editor(long name) :
    c_start_menu_pane_screen_widget(name, NONE),
    m_operation(0),
    m_save_state(NONE),
    m_dialog_action(0),
    m_operation_controller(_controller_index_none),
    m_keyboard_task(c_virtual_keyboard_task::get_instance(
        "..\\interface\\gui_screens\\start_menu\\panes\\game_editor\\start_menu_game_editor.cpp",
        52, _controller_index_none, 0, 0, 0, 256, 0)),
    m_write_succeeded(c_synchronized_long::s_defer_initialization()),
    m_write_complete(c_synchronized_long::s_defer_initialization()),
    m_content_item(c_synchronized_long::s_defer_initialization())
{
    memset(m_save_name, 0, sizeof(m_save_name));
    ((c_map_variant *)m_saved_setup.map_variant_storage)->c_map_variant::c_map_variant();
    m_create_variant_task = NONE;
    m_save_as_variant_task = NONE;
    m_write_variant_task = NONE;
    m_write_succeeded.c_synchronized_long::c_synchronized_long(0);
    m_write_complete.c_synchronized_long::c_synchronized_long(0);
    m_content_item.c_synchronized_long::c_synchronized_long(NONE);
    m_progress_message_start_time = 0;
    m_automatic_button_key = _string_id_a_confirm_b_cancel;
}

// @address 0x4ab8a0 ?initialize@c_start_menu_game_editor@@UAAXXZ
void c_start_menu_game_editor::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<lobby-privacy", this, parse_lobby_privacy));
}

// @address 0x4ab160 ?submenu_invoked@c_start_menu_game_editor@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_game_editor::submenu_invoked(c_gui_list_widget *list)
{
    if (list->m_name == _start_menu_string_change_teams)
    {
        const short user = controller_get(get_any_responding_controller())->get_user_index();
        if (user != NONE)
        {
            const long team = network_session_interface_get_user_team_index(user);
            if (team != NONE)
                list->set_focused_datasource_element_from_value(_start_menu_string_team, team, false);
        }
    }
}

extern "C" {

// @address 0x51040 ??_7c_gui_gametype_category_datasource@@6B@

}

// @address 0x4ab928 ?initialize_datasource@c_start_menu_game_editor@@UAAXXZ
void c_start_menu_game_editor::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    c_gui_data *data = find_data(_start_menu_string_start_menu_game_items, 0);
    if (data)
    {
        bool in_edit_mode = false;
        data->clear_disabled_elements();
        c_controller_interface *controller = &g_controller_globals.controllers[get_any_responding_controller()];
        if (!is_change_teams_allowed(get_any_responding_controller()))
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_change_teams);
        short user_index = controller->user_index;
        if (user_index != -1)
        {
            long player_index = player_mapping_get_player_for_user(user_index);
            if (player_index != -1)
                in_edit_mode = game_engine_is_player_in_edit_mode(player_index, 0, 0);
        }
        if (!::network_life_cycle_squad_local_peer_is_leader())
        {
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_reset_map);
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_end_session);
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_change_gametype);
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_change_game);
        }
        else
        {
            long local_player_count = user_interface_squad_get_local_player_count();
            if (user_interface_squad_get_player_count() <= local_player_count)
                data->set_disabled_element(_start_menu_string_name, _start_menu_string_leave_editor);
            if (!in_edit_mode)
                data->set_disabled_element(_start_menu_string_name, _start_menu_string_change_gametype);
        }
        if (in_edit_mode)
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_enter_editor_mode);
        else
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_exit_editor_mode);
        const s_start_menu_game_editor_preferences *preferences = (const s_start_menu_game_editor_preferences *)global_preferences_get_game_setup_storage();
        m_saved_setup = preferences->saved_setup;
        if (m_saved_setup.file.type != 1)
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_save_map);
        if (!::network_life_cycle_squad_local_peer_is_leader())
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
        if (network_squad_session_get_session_class() != 2)
            data->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
        e_controller_index controller_index = get_any_responding_controller();
        if (controller_index != _controller_index_none)
        {
            controller = &g_controller_globals.controllers[controller_index];
            if (((c_controller_interface *)controller)->is_guest() || controller->is_unsigned_in_user())
            {
                data->set_disabled_element(_start_menu_string_name, _start_menu_string_save_map);
                data->set_disabled_element(_start_menu_string_name, _start_menu_string_save_map_as);
            }
        }
    }
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_gametype_category_datasource), 0, 0);
    c_gui_data *category = memory ? new (memory) c_gui_gametype_category_datasource() : 0;
    if (category->initialize(0x10530))
    {
        m_datasource[m_datasource_count] = category;
        ++m_datasource_count;
    }
    else
    {
        category->~c_gui_data();
        user_interface_free(category);
    }
}


// @address 0x4abd30 start_menu_game_editor_build_map_variant_file
extern "C" void start_menu_game_editor_build_map_variant_file(c_start_menu_game_editor *screen,
    s_blffile_map_variant *file, const wchar_t *name, const wchar_t *description, e_map_id map_id,
    const c_map_variant *variant)
{
    e_controller_index controller = ((const c_gui_widget *)screen)->get_any_responding_controller();
    saved_game_files_initialize_metadata((s_saved_game_item_metadata_record *)&file->get_metadata(), controller, 10, name, description,
        map_variant_compute_unique_id(variant), 0xe1f0);
    variant->save_to(&file->map_variant.map_variant);
    file->get_metadata().map_id = map_id;
    map_variant_ensure_required_objects(&file->map_variant.map_variant);
    map_variant_reset_from_encoding(&file->map_variant.map_variant);
    map_variant_set_metadata(&file->map_variant.map_variant, (const s_saved_game_item_metadata_packed *)&file->get_metadata());
    file->map_variant.map_variant.m_map_id = map_id;
}



// @address 0x4abdc8 ?update@c_start_menu_game_editor@@UAAXK@Z
void c_start_menu_game_editor::update(unsigned long current_milliseconds)
{
    c_gui_data *data = find_data(0x10017, 0);
    if (data) {
        const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
        e_campaign_id campaign_id;
        e_map_id map_id;
        if (network_life_cycle_session_get_map(&campaign_id, &map_id) && game_variant) {
            data->clear_disabled_elements();
            unsigned short map_mask = game_engine_get_active_team_mask();
            for (long element_handle = data->get_first_element_handle(); element_handle != -1; element_handle = data->get_next_element_handle(element_handle)) {
                long element_map_id;
                if (data->get_integer_value(element_handle, _start_menu_string_team, &element_map_id) && ((1 << element_map_id) & map_mask) == 0) {
                    long value = -1;
                    if (data->get_string_id_value(element_handle, _start_menu_string_name, &value))
                        data->set_disabled_element(_start_menu_string_name, value);
                }
            }
        }
    }
    switch (m_operation) {
    case 0: {
        c_gui_widget *widget = get_child_widget(_gui_screen_widget, _string_id_in_progress);
        if (widget && !((c_gui_screen_widget *)widget)->transitioning_out()) {
            if (start_menu_game_editor_milliseconds() - m_progress_message_start_time >= 3000)
                ((c_gui_screen_widget *)widget)->transition_out_with_transition_type(0, _screen_transition_normal);
        }
        break;
    }
    case 1:
        start_menu_game_editor_finish_save_variant(this);
        break;
    case 2:
        start_menu_game_editor_finish_save_variant_as(this);
        break;
    default:
        // match: 0x4abdc8: __assume(0) omits the switch bound check; removal emits cmplwi r11,3 and renumbers the find_data loop registers
        __assume(0);
    }
    c_start_menu_pane_screen_widget::update(current_milliseconds);
}






// @address 0x4ac7d8 ?handle_dialog_result@c_start_menu_game_editor@@UAA_NPBVc_dialog_result_message@@@Z
bool c_start_menu_game_editor::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = c_start_menu_game_multiplayer::handle_leave_game_response(this, message);
    if (!handled) {
        switch (message->get_dialog_name()) {
        case _start_menu_string_in_game_end_forge_session:
            if (message->get_dialog_result() == 0) {
                if (::network_life_cycle_squad_local_peer_is_leader()) network_squad_session_end_game();
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_in_game_forge_reset_map:
            if (message->get_dialog_result() == 0) {
                c_controller_interface *controller = &g_controller_globals.controllers[message->get_controller()];
                if (controller && (short)controller->user_index != -1) {
                    int player_index = player_mapping_get_player_for_user((short)controller->user_index);
                    if (player_index != -1) {
                        game_engine_sandbox_editor_action action;
                        action.properties.placement_flags.set_unsafe(12);
                        action.properties.game_engine_flags.clear();
                        action.properties.boundary_shape = _multiplayer_object_boundary_shape_unused;
                        action.properties.spawn_rate = 0;
                        action.properties.spare_clips = 0;
                        action.properties.teleporter_channel = 0;
                        action.properties.boundary_negative_height = k_real_zero_storage;
                        action.properties.boundary_positive_height = k_real_zero_storage;
                        action.properties.boundary_box_length = k_real_zero_storage;
                        action.properties.boundary_width = k_real_zero_storage;
                        action.properties.team_designator = _variant_team_designator_neutral;
                        __int64 *p = (__int64 *)&action;
                        unsigned int count = 8;
                        do { *p++ = 0; } while (--count);
                        action.action = 7U;
                        int editor_index = game_engine_globals->map_info.m_game_engine_subtype;
                        action.player_index = player_index & 0xffff;
                        action.subtype = editor_index;
                        simulation_queue_editor_action_insert(&action);
                    }
                }
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_lobby_privacy: {
            int mode = user_interface_networking_get_session_advertisement();
            switch (message->get_dialog_result()) {
            case 0: mode = 0; break;
            case 1: mode = 1; break;
            case 2: mode = 2; break;
            }
            user_interface_networking_set_session_advertisement(mode);
            handled = true;
            break;
        }
        case _start_menu_string_confirm_saved_game_overwrite:
            switch (message->get_dialog_result()) {
            case 0:
                m_dialog_action = 1;
                if (m_save_state == 0) {
                    start_menu_game_editor_begin_save_variant(this, message->get_controller());
                    m_save_state = -1;
                    m_dialog_action = 0;
                }
                break;
            case 1:
                m_dialog_action = 2;
                break;
            }
            handled = true;
            break;
        }
    }
    if (!(unsigned char)handled) {
        handled = false;
        if (message->get_dialog_name() == _start_menu_string_device_selection) {
            if (message->get_dialog_result() == 0) content_catalogue_display_device_selection_guide_interface((e_controller_index)message->get_controller());
            handled = true;
        }
    }
    return handled;
}



extern "C" {
// @address 0x510b0 ??_7c_load_game_editor_variant_screen_message@@6B@
}


// @address 0x4ada80 ?apply_initial_state@c_load_game_editor_variant_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_game_editor_variant_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_gui_screen_alpha_motd_editor *prompt = static_cast<c_gui_screen_alpha_motd_editor *>(screen);
    prompt->m_editor = m_screen;
    prompt->m_save_as = m_save_as;
}

static __forceinline void start_menu_game_editor_load_variant(c_start_menu_game_editor *screen, const c_controller_input_message *message, bool save_as)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_load_game_editor_variant_screen_message), 0, 0);
    c_load_game_editor_variant_screen_message *load = memory
        ? new (memory) c_load_game_editor_variant_screen_message(message->get_controller(), screen->get_render_window(), screen, save_as)
        : 0;
    if (load)
    {
        load->set_parent_screen_index(screen->get_screen_index());
        ::user_interface_messaging_post(load);
    }
}

// match: 0x4acae8: direct TLS slot reads preserve the original pointer/index register allocation.
// @address 0x4acae8 ?handle_list_item_chosen@c_start_menu_game_editor@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_game_editor::handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    switch (list)
    {
    case _start_menu_string_start_menu_items:
    {
        long choice = -1;
        handled = data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &choice);
        if (handled)
        {
            switch (choice)
            {
            case _start_menu_string_leave_editor:
                start_menu_game_multiplayer_handle_leave_game(this, message->get_controller());
                break;
            case _start_menu_string_end_session:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_game_end_forge_session, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                break;
            }
            case _start_menu_string_reset_map:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_game_forge_reset_map, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                break;
            }
            case _start_menu_string_save_map:
            {
                e_controller_index controller_index = message->get_controller();
                c_controller_interface *controller = controller_get(controller_index);
                c_content_catalogue *catalogue = &content_catalogues().catalogues[controller->get_controller_index()];
                if (content_catalogue_has_storage_device(catalogue))
                {
                    if (!game_engine_globals->sandbox.variant_file_exists)
                        start_menu_game_editor_load_variant(this, message, false);
                    else
                        start_menu_game_editor_request_save_variant(this, message);
                }
                else
                    user_interface_show_device_selection_dialog(controller_index, get_name(), get_screen_index());
                break;
            }
            case _start_menu_string_save_map_as:
            {
                e_controller_index controller_index = message->get_controller();
                c_controller_interface *controller = controller_get(controller_index);
                c_content_catalogue *catalogue = &content_catalogues().catalogues[controller->get_controller_index()];
                if (content_catalogue_has_storage_device(catalogue))
                {
                    if (!game_engine_globals->sandbox.variant_file_exists)
                        start_menu_game_editor_load_variant(this, message, true);
                    else
                    {
                        start_menu_game_editor_request_save_variant_as(this, controller_index);
                        m_save_state = -1;
                        m_dialog_action = 0;
                    }
                }
                else
                    user_interface_show_device_selection_dialog(controller_index, get_name(), get_screen_index());
                break;
            }
            case _start_menu_string_enter_editor_mode:
            case _start_menu_string_exit_editor_mode:
            {
                c_controller_interface *controller = &g_controller_globals.controllers[message->get_controller()];
                if (controller)
                {
                    short user_index = (short)controller->user_index;
                    if (user_index != -1)
                    {
                        long player_index = player_mapping_get_player_for_user(user_index);
                        if (player_index != -1)
                        {
                            game_engine_sandbox_editor_action action;
                            action.properties.placement_flags.set_unsafe(12);
                            action.properties.game_engine_flags.clear();
                            action.properties.boundary_shape = _multiplayer_object_boundary_shape_unused;
                            action.properties.spawn_rate = 0;
                            action.properties.spare_clips = 0;
                            action.properties.teleporter_channel = 0;
                            action.properties.boundary_negative_height = k_real_zero_storage;
                            action.properties.boundary_positive_height = k_real_zero_storage;
                            action.properties.boundary_box_length = k_real_zero_storage;
                            action.properties.boundary_width = k_real_zero_storage;
                            action.properties.team_designator = _variant_team_designator_neutral;
                            memset(&action, 0, sizeof(action));
                            action.action = 2;
                            action.player_index = player_index & 0xffff;
                            simulation_queue_editor_action_insert(&action);
                            close_start_menu();
                        }
                    }
                }
                break;
            }
            case _start_menu_string_lobby_privacy:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(), _start_menu_string_lobby_privacy, get_name())
                    : 0;
                if (dialog)
                {
                    dialog->m_parent_screen_index = get_screen_index();
                    ::user_interface_messaging_post(dialog);
                }
                handled = true;
                break;
            }
            default:
                handled = false;
                break;
            }
        }
        break;
    }
    case _start_menu_string_change_gametype:
    {
        long gametype;
        if (data->get_integer_value(item->get_element_handle(), _start_menu_string_name, &gametype))
        {
            c_controller_interface *controller = &g_controller_globals.controllers[message->get_controller()];
            if (controller)
            {
                short user_index = (short)controller->user_index;
                if (user_index != -1)
                {
                    long player_index = player_mapping_get_player_for_user(user_index);
                    if (player_index != -1)
                    {
                        game_engine_sandbox_editor_action action;
                        action.properties.placement_flags.set_unsafe(12);
                        action.properties.game_engine_flags.clear();
                        action.properties.boundary_shape = _multiplayer_object_boundary_shape_unused;
                        action.properties.spawn_rate = 0;
                        action.properties.spare_clips = 0;
                        action.properties.teleporter_channel = 0;
                        action.properties.boundary_negative_height = k_real_zero_storage;
                        action.properties.boundary_positive_height = k_real_zero_storage;
                        action.properties.boundary_box_length = k_real_zero_storage;
                        action.properties.boundary_width = k_real_zero_storage;
                        action.properties.team_designator = _variant_team_designator_neutral;
                        memset(&action, 0, sizeof(action));
                        action.action = 7;
                        long game_engine;
                        switch (gametype)
                        {
                        case 5: game_engine = 10; break;
                        case 1: game_engine = 0; break;
                        case 2: game_engine = 1; break;
                        case 9: game_engine = 6; break;
                        case 4: game_engine = 3; break;
                        case 6: game_engine = 7; break;
                        case 3: game_engine = 2; break;
                        case 8: game_engine = 5; break;
                        case _game_engine_infection: game_engine = _game_engine_territories; break;
                        case 7: game_engine = 4; break;
                        case 0: game_engine = 10; break;
                        // match: 0x4acae8: unreachable hint omits dead branch check
                        default: __assume(0);
                        }
                        action.subtype = game_engine;
                        action.player_index = player_index & 0xffff;
                        if (game_engine != game_engine_globals->map_info.m_game_engine_subtype)
                            simulation_queue_editor_action_insert(&action);
                        close_start_menu();
                    }
                }
            }
        }
        handled = true;
        break;
    }
    case _start_menu_string_change_teams:
    {
        long value = -1;
        if (data->get_integer_value(item->get_element_handle(), _start_menu_string_team, &value))
        {
            if ((value >= 0 && value < 16) || value == -1)
            {
                long controller = this->get_any_responding_controller();
                start_menu_game_multiplayer_set_player_team(controller, value);
                close_start_menu();
            }
        }
        break;
    }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}

extern "C" {

}

// @address 0x4ad4f0 start_menu_game_editor_request_save_variant_as
extern "C" void start_menu_game_editor_request_save_variant_as(c_start_menu_game_editor *screen, e_controller_index controller_index)
{
    wchar_t name[256];
    struct { wchar_t title[1024]; wchar_t description[1024]; } text;
    s_start_menu_game_editor_save_request request;
    s_start_menu_game_editor_saved_context context;
    levels_map_datum level;
    e_map_id map_id;

    if (screen->m_operation != 0)
        return;
    c_virtual_keyboard_task **task_slot = &screen->m_keyboard_task;
    if (*task_slot == 0)
        return;
    short type = (*task_slot)->get_task_state();
    unsigned char busy = type == 1 || type == 2 || type == 3;
    if (busy)
        return;
    const c_map_variant *variant = start_menu_game_editor_game_variant();
    const s_start_menu_game_editor_preferences *preferences =
        (const s_start_menu_game_editor_preferences *)global_preferences_get_game_setup_storage();
    const s_start_menu_game_editor_saved_settings *source = &preferences->saved_setup;
    if (source->setup_type == 0)
    {
        memset(&request, 0, sizeof(request));
        e_map_id default_map = variant->m_map_id;
        request.mode = 3;
        ((c_map_variant *)request.setup.map_variant_storage)->create_default(default_map);
        memset(&request.setup.file, 0, sizeof(request.setup.file));
        request.setup.file.has_path = 1;
        request.setup.file.type = -1;
        request.setup.setup_type = 1;
        global_preferences_set_mode_setup((int *)&request);
    }
    text.title[0] = 0;
    text.description[0] = 0;
    map_id = variant->m_map_id;
    const wchar_t *level_name = variant->m_metadata.name;
    memcpy(&screen->m_saved_setup, source, sizeof(screen->m_saved_setup));
    if ((*level_name == 0 || map_id == _map_id_none)
        && network_life_cycle_session_get_map(&context.campaign_id, &map_id)
        && levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, &level))
        level_name = level.display_name;
    context.name = level_name;
    context.zero8 = 0;
    context.zero12 = 0;
    context.handle = -1;
    short user_index = g_controller_globals.controllers[controller_index].user_index;
    context.index = user_index;
#ifdef HALO3_64BIT
    // context mirrors s_saved_game_files_context's 32-bit layout (index, first, second, third, difficulty); with
    // 8-byte string pointers its handle and campaign_id would form third_string. Build the real one.
    s_saved_game_files_context parse_context = {};
    parse_context.user_index = context.index;
    parse_context.first_string = context.name;
    parse_context.campaign_difficulty = context.handle;
    saved_game_files_set_text_parse_context(&parse_context);
#else
    saved_game_files_set_text_parse_context((const s_saved_game_files_context *)&context);
#endif
    bool found = user_interface_get_formatted_string(user_index, 0x73617665, 0x70000, name, 256);
    if (found)
        saved_game_files_find_unique_name(controller_index, 10, name, 256);
    saved_game_files_reset_text_parse_context();
    if (!found)
    {
        const wchar_t *src = level_name;
        for (long i = 0; i <= 255; i++, src++)
        {
            if (!(name[i] = *src))
                break;
        }
        name[255] = 0;
    }
    screen->get_string_by_string_id(0x10388, (c_static_wchar_string<1024> *)text.title);
    screen->get_string_by_string_id(0x10389, (c_static_wchar_string<1024> *)text.description);
    (*task_slot)->m_controller_index = controller_index;
    (*task_slot)->set_default_text(name);
    (*task_slot)->set_title_text(text.title);
    (*task_slot)->set_description_text(text.description);
    (*task_slot)->m_maximum_input_characters = 16;
    (*task_slot)->m_character_flags = 0x4001;
    if (overlapped_task_start_internal((c_overlapped_task *)*task_slot,
            "..\\interface\\gui_screens\\start_menu\\panes\\game_editor\\start_menu_game_editor.cpp" /* __FILE__ */, 0x4d1))
    {
        screen->m_operation = 2;
        screen->m_operation_controller = controller_index;
        return;
    }
    c_virtual_keyboard_task *task = *task_slot;
    task->reset();
}

extern "C" {


}

// @address 0x4ac028 start_menu_game_editor_finish_save_variant
extern "C" void start_menu_game_editor_finish_save_variant(c_start_menu_game_editor *screen)
{
    bool complete = false;
    if (screen->m_write_variant_task != -1)
    {
        if (screen->m_write_complete.peek() == 1)
        {
            complete = true;
            if (screen->m_write_succeeded.peek() == 1)
            {
                s_start_menu_game_editor_save_request request;
                const s_start_menu_game_editor_preferences *preferences =
                    (const s_start_menu_game_editor_preferences *)global_preferences_get_game_setup_storage();
                request.mode = 3;
                memcpy(&request.setup, &preferences->saved_setup, sizeof(request.setup));
                start_menu_game_editor_game_variant()->save_to((c_map_variant *)request.setup.map_variant_storage);
                map_variant_ensure_required_objects((c_map_variant *)request.setup.map_variant_storage);
                map_variant_reset_from_encoding((c_map_variant *)request.setup.map_variant_storage);
                request.setup.map_variant_edited = 1;
                global_preferences_set_mode_setup(&request.mode);
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20057, 0, 0, k_any_controller, false, true);
            }
            screen->m_write_variant_task = -1;
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
        complete = true;
    }
    if (complete)
    {
        if (screen->m_content_item.peek() != -1)
        {
            c_content_catalogue *catalogue = start_menu_game_editor_catalogue(screen->m_operation_controller);
            content_catalogue_close_item(catalogue, screen->m_content_item.peek(), false);
            __lwsync();
            _InterlockedExchange(&screen->m_content_item.value, -1);
        }
        screen->m_operation = 0;
        screen->m_save_state = -1;
        screen->m_dialog_action = 0;
    }
}

struct utf8;

extern "C" {
}


// @address 0x4ac1e0 start_menu_game_editor_finish_save_variant_as
extern "C" void start_menu_game_editor_finish_save_variant_as(c_start_menu_game_editor *screen)
{
    bool complete = false;
    wchar_t name[256];
    wchar_t description[128];
    if (screen->m_keyboard_task != 0 && screen->m_keyboard_task->m_controller_index >= 0 && screen->m_keyboard_task->m_controller_index < 4)
    {
        if (!screen->m_keyboard_task->busy())
        {
            complete = true;
            if (screen->m_keyboard_task->get_task_state() == 4)
            {
                c_content_catalogue *catalogue = start_menu_game_editor_catalogue(
                    ((const c_gui_widget *)screen)->get_any_responding_controller());
                ((c_static_wchar_string<256> *)name)->set(screen->m_keyboard_task->m_result_text);
                content_catalogue_trim_whitespace((unsigned short *)name);
                if (content_catalogue_has_storage_device(catalogue))
                {
                    e_controller_index controller = ((const c_gui_widget *)screen)->get_any_responding_controller();
                    if (content_catalogue_is_item_name_valid(catalogue, controller, name, 1))
                    {
                    ustrnzcpy(screen->m_save_name, name, 256);
                    screen->m_create_variant_task = saved_game_files_create_content_item_async(screen->m_operation_controller,
                        screen->m_save_name, 10, &screen->m_write_succeeded, &screen->m_write_complete, &screen->m_content_item);
                    if (screen->m_create_variant_task != -1)
                    {
                        game_engine_set_map_info_name(start_menu_game_editor_game_variant(), screen->m_save_name);
                        start_menu_game_editor_post_progress_message(screen, screen->m_operation_controller);
                        complete = false;
                    }
                    else
                        g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
                    }
                }
            }
            screen->m_keyboard_task->reset();
            screen->m_keyboard_task->m_controller_index = _controller_index_none;
        }
    }
    else if (screen->m_save_state == 1)
    {
        switch (screen->m_dialog_action)
        {
        case 1:
            screen->m_create_variant_task = saved_game_files_create_content_item_async(screen->m_operation_controller, screen->m_save_name,
                10, &screen->m_write_succeeded, &screen->m_write_complete, &screen->m_content_item);
            if (screen->m_create_variant_task != -1)
                start_menu_game_editor_post_progress_message(screen, screen->m_operation_controller);
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
                complete = true;
            }
            screen->m_save_state = -1;
            screen->m_dialog_action = 0;
            break;
        case 2:
            screen->m_save_state = -1;
            screen->m_dialog_action = 0;
            complete = true;
            break;
        }
    }
    else if (screen->m_create_variant_task != -1)
    {
        if (screen->m_write_complete.peek() == 1)
        {
            if (screen->m_write_succeeded.peek() == 1)
            {
                if (screen->m_content_item.peek() != -1)
                {
                    const wchar_t *name = screen->m_keyboard_task->m_result_text;
                    const c_map_variant *variant = start_menu_game_editor_game_variant();
                    utf8_string_to_wchar_string((const utf8 *)variant->m_metadata.description, description, 128, 0);
                    s_blffile_map_variant *file = &screen->m_variant_file;
                    start_menu_game_editor_build_map_variant_file(screen, file, name, description, variant->m_map_id, variant);
                    screen->m_save_as_variant_task = saved_game_files_write_content_item(screen->m_operation_controller,
                        screen->m_content_item.peek(), 10, file, sizeof(*file), &screen->m_write_succeeded,
                        &screen->m_write_complete);
                    if (screen->m_save_as_variant_task == -1)
                    {
                        g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
                        complete = true;
                    }
                }
                else
                {
                    g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
                    complete = true;
                }
            }
            else
                g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
            screen->m_create_variant_task = -1;
        }
    }
    else if (screen->m_save_as_variant_task != -1)
    {
        if (screen->m_write_complete.peek() == 1)
        {
            complete = true;
            if (screen->m_write_succeeded.peek() == 1)
            {
                if (screen->m_content_item.peek() != -1)
                {
                    c_content_catalogue *catalogue = start_menu_game_editor_catalogue(screen->m_operation_controller);
                    if (screen->m_content_item.peek() != -1)
                    {
                        s_start_menu_game_editor_save_request request;
                        const s_start_menu_game_editor_preferences *preferences =
                            (const s_start_menu_game_editor_preferences *)global_preferences_get_game_setup_storage();
                        request.mode = 3;
                        memcpy(&request.setup, &preferences->saved_setup, sizeof(request.setup));
                        s_start_menu_game_editor_saved_settings *setup = &request.setup;
                        content_catalogue_get_item_file_path(catalogue, screen->m_content_item.peek(), setup->file.path, 256);
                        setup->file.has_path = true;
                        setup->file.type = 1;
                        request.setup.map_variant_edited = 1;
                        start_menu_game_editor_game_variant()->save_to((c_map_variant *)request.setup.map_variant_storage);
                        map_variant_ensure_required_objects((c_map_variant *)request.setup.map_variant_storage);
                        map_variant_reset_from_encoding((c_map_variant *)request.setup.map_variant_storage);
                        global_preferences_set_mode_setup(&request.mode);
                    }
                }
            }
            else
                g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
            screen->m_save_as_variant_task = -1;
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20054, 0, 0, k_any_controller, false, true);
        complete = true;
    }
    if (complete)
    {
        if (screen->m_content_item.peek() != -1)
        {
            c_content_catalogue *catalogue = start_menu_game_editor_catalogue(screen->m_operation_controller);
            content_catalogue_close_item(catalogue, screen->m_content_item.peek(), false);
            __lwsync();
            _InterlockedExchange(&screen->m_content_item.value, -1);
        }
        screen->m_operation = 0;
        screen->m_save_state = -1;
        screen->m_dialog_action = 0;
    }
}
