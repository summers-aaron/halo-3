// interfacelibxbox/gui_screen_game_options.obj: from the records 0x2659a8 0x4b92c0 0x4d31f0 0x4d3260 0x4d32e8 0x4d3340 0x4d33c0 0x4d3480 0x4d3660 0x4d36c0 0x4d3820 0x4d3878 0x4d3a98 0x4d3b80 0x4d3dc8 0x4d3e60 0x4d3f48 0x4d4150 0x4d42c0 0x4d4430 0x4d4a40 0x4d4f40 0x4d5278 0x4d5a10 0x4d5b30 0x4d5cb0 0x4d5d90 0x4d5ed8 0x4d60b8 0x4d6208 0x4d6468 0x4d6568 0x4d66b8 0x4d66c8 0x4d4b18
// @address 0x11401e8 g_controller_globals
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x74b540 g_current_language
#include "saved_games/content_catalogue_types.h"
#include "saved_games/saved_game_files_operations.h"
// @flags /TP /O2 /Oc
#include "interface/gui_screen_widget_text.h"
#include "cseries/cseries_unicode.h"
#include "networking/logic/network_session_interface.h"
#include "interface/user_interface_clock.h"
#include "tag_files/tag_groups.h"
#include "saved_games/content_item_status_types.h"
#include "cseries/cseries_wrapped.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/game_content_types.h"
#include <stddef.h>
#include "memory/static_arrays.h"
#include "saved_games/saved_game_item_metadata.h"
#include "interface/gui_selected_items.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_data.h"
#include "cseries/cseries.h"
#include "tag_files/tag_types.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "game/game_engine_variant.h"
#include "interface/gui_pregame_setup_manager.h"
#include "main/game_preferences.h"
#include "networking/logic/network_life_cycle.h"
#include "tag_files/files.h"
#include "tag_files/string_ids.h"
#include "interface/user_interface_utilities.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/saved_game_files.h"
#include "interface/gui_screens/game_options/gui_screen_game_options.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "cseries/async_xoverlapped_types.h"
#include "networking/session/network_session_class.h"
#include "game/game_variant_parameter_types.h"
#include "saved_games/saved_game_files_context_types.h"
#include "interface/user_interface_messages.h"
#include "interface/text_value_pair_types.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_screens/game_options/game_engine_category_types.h"
#include "cache/cache_files.h"
#include "interface/sandbox_object_properties.h"
#include "interface/user_interface_multiplayer_game_settings_group.h"
#include "interface/c_player_profile.h"
enum e_game_variant_parameter;
class c_game_variant;
struct utf8;
class c_gui_screen_widget;
class c_gui_screen_game_options;
class c_virtual_keyboard_task;
class c_content_catalogue;
class c_gui_selected_item;
#include "game/game_variant.h"
#include "game/game_engine_base_variant.h"
#include <new>
#include "interface/gui_screen_game_options_externs.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_group_widget.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "multithreading/synchronized_value.h"
#include "saved_games/saved_game_file_types.h"
#include "interface/virtual_keyboard_task.h"
#include "interface/user_interface_allocation.h"
#include "text/utf8_conversion.h"
#include <ppcintrinsics.h>
#include "cseries/compiler_intrinsics.h"
#include "tag_files/file_reference.h"
#include "saved_games/game_content_conversion.h"
#include "interface/user_interface_device_selection.h"
#include "interface/user_interface_session_game_setup.h"
#include "interface/gui_selected_items_game_variant.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include "cseries/crt_xdk.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include <string.h>
#include "networking/session/network_session_interface_types.h"
#include "cseries/language.h"
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x485008 ??0c_gui_screen_widget@@QAA@J@Z
// @address 0x485190 ?reload_next_frame@c_gui_screen_widget@@UAAXXZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x483428 ??0c_load_in_progress_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJ@Z

struct s_game_options_category_entry { long name; long category; };

class c_game_options_parameter_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_game_options_parameter_datasource(e_game_variant_parameter parameter) : m_parameter(parameter) {}
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long index, long value_name, long *value);
    virtual bool get_string_id_value(long index, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal(void);
    const s_text_value_pair *text_value_pair_try_and_get(long index) const;

    long m_parameter;
};

class c_game_options_category_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_game_options_category_datasource(e_game_engine_category template_category, e_game_engine_category category,
        const c_game_variant *variant) :
        m_variant(variant), m_template_category(template_category), m_category(category)
    {
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_element(long index, void *element, long element_size);
    virtual bool get_string_id_value(long index, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal(void);

    const c_game_variant *m_variant;
    long m_template_category;
    long m_category;
};

struct s_game_options_parameter_definition
{
    long parameter;
    long name;
    long description;
};

struct s_gui_screen_game_options_saved_game_context
{
    int index;
    int zero4;
    unsigned short *name;
    int zero12;
    int handle;
};

struct s_game_variant_option_tag
{
    e_game_variant_parameter parameter;
    long item_count;
    TAG_POINTER(const s_game_options_category_entry) entries;
};


// @address 0x2659a8 gui_screen_game_options_set_variant_name
void gui_screen_game_options_set_variant_name(c_game_engine_base_variant *variant, const wchar_t *source)
{
    memset(variant->m_metadata.name, 0, sizeof(variant->m_metadata.name));
    ustrnzcpy_inline(variant->m_metadata.name, source, NUMBEROF(variant->m_metadata.name));
}




static __forceinline const s_text_value_pair *gui_screen_game_options_text_value_pair_try_and_get_by_index(
    const s_text_value_pair_definition *definition, short index)
{
    const s_text_value_pair *result;
    if (index >= 0 && index < definition->values.count)
        result = &((const s_text_value_pair *)definition->values.address)[index];
    else
        result = 0;
    return result;
}


// @address 0x4d31f0 ?get_integer_value@c_game_options_parameter_datasource@@UAA_NJJPAJ@Z
bool c_game_options_parameter_datasource::get_integer_value(long index, long value_name, long *value)
{
    bool result = false;
    if (value_name == _game_options_string_value)
    {
        const s_text_value_pair *pair = text_value_pair_try_and_get(index);
        if (pair && pair->type == _text_value_pair_parameter_integer)
        {
            *value = pair->value;
            result = true;
        }
    }
    return result;
}


// @address 0x4d3260 ?get_string_id_value@c_game_options_parameter_datasource@@UAA_NJJPAJ@Z
bool c_game_options_parameter_datasource::get_string_id_value(long index, long value_name, long *value)
{
    const s_text_value_pair *pair = text_value_pair_try_and_get(index);
    bool result = false;
    if (pair)
    {
        switch (value_name)
        {
        case _game_options_string_value:
            if (pair->type == _text_value_pair_parameter_string_id)
            {
                *value = pair->string_value;
                result = true;
            }
            break;
        case _game_options_string_name:
            *value = pair->display_string;
            result = true;
            break;
        case _game_options_string_description:
            *value = pair->description_string;
            result = true;
            break;
        }
    }
    return result;
}

long c_game_options_parameter_datasource::get_current_item_count_internal(void)
{
    long count;
    const s_text_value_pair_definition *definition = text_value_pair_definition_try_and_get(m_parameter);
    if (definition)
        count = definition->values.count;
    else
        count = 0;
    return count;
}

inline const s_text_value_pair *c_game_options_parameter_datasource::text_value_pair_try_and_get(long index) const
{
    const s_text_value_pair *result;
    const s_text_value_pair_definition *definition = text_value_pair_definition_try_and_get(m_parameter);
    if (definition)
        result = gui_screen_game_options_text_value_pair_try_and_get_by_index(definition, (short)index);
    else
        result = 0;
    return result;
}


static __forceinline const s_variant_setting_edit_reference *gui_screen_game_options_variant_setting_edit_reference_try_and_get(long setting)
{
    const s_variant_setting_edit_reference *result;
    const s_multiplayer_variant_settings_interface_definition *definition = multiplayer_variant_settings_interface_definition_try_and_get_master();
    if (definition)
        result = variant_setting_edit_reference_try_and_get_by_setting_recursive(definition, setting);
    else
        result = 0;
    return result;
}


// @address 0x4d3480 ?get_string_id_value@c_game_options_category_datasource@@UAA_NJJPAJ@Z
bool c_game_options_category_datasource::get_string_id_value(long index, long value_name, long *value)
{
    const s_variant_setting_edit_reference *reference = (m_category >= 0 && m_category < 217)
        ? gui_screen_game_options_variant_setting_edit_reference_try_and_get(m_category) : 0;
    bool result = false;
    if (reference && index >= 0 && index < reference->options.count)
    {
        const s_variant_setting_option *option = &((const s_variant_setting_option *)reference->options.address)[index];
        const void *category = option->category.index != NONE
            ? tag_get(option->category.group_tag, option->category.index) : NULL;
        const void *template_category = option->template_category.index != NONE
            ? tag_get(option->template_category.group_tag, option->template_category.index) : NULL;
        const s_game_options_parameter_definition *parameter = option->parameter.index != NONE
            ? (const s_game_options_parameter_definition *)tag_get(option->parameter.group_tag, option->parameter.index)
            : NULL;
        switch (value_name)
        {
        case _game_options_string_value:
            if (category || template_category)
                *value = _game_options_string_elipsis;
            else if (parameter)
            {
                long template_category_id = m_template_category;
                long display_parameter = parameter->parameter;
                long parameter_id = display_parameter;
                if (template_category_id != -1)
                    parameter_id = get_player_trait_parameter_from_template_parameter(
                        (e_game_engine_category)template_category_id, (e_game_variant_parameter)parameter_id);
                s_text_value_pair pair;
                pair.flags = 0;
                short pair_index;
                const s_text_value_pair_definition *definition;
                bool found = get_text_value_pair_value_from_variant_parameter_value(m_variant,
                    (e_game_variant_parameter)parameter_id, (e_game_variant_parameter)display_parameter,
                    &pair, &pair_index, &definition);
                long selected_value;
                if (found)
                    selected_value = pair.display_string;
                else
                    selected_value = _game_options_string_value;
                *value = selected_value;
            }
            else
                break;
            result = true;
            break;
        case _game_options_string_name:
            if (category || template_category)
                *value = option->name;
            else if (parameter)
                *value = parameter->name;
            else
                break;
            result = true;
            break;
        case _game_options_string_description:
            if (category || template_category)
                *value = option->description;
            else if (parameter)
                *value = parameter->description;
            else
                break;
            result = true;
            break;
        default:
            break;
        }
    }
    return result;
}

// @address 0x4bace0 ?get_column_names@c_game_options_parameter_datasource@@UAAXQAJPAJ@Z
void c_game_options_parameter_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _game_options_string_value;
    column_names[(*column_count)++] = _game_options_string_name;
    column_names[(*column_count)++] = _game_options_string_description;
}

// @address 0x4b92c0 ?get_column_names@c_game_options_category_datasource@@UAAXQAJPAJ@Z
void c_game_options_category_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _game_options_string_value;
    ++*column_count;
}

// @address 0x4d33c0 ?get_element@c_game_options_category_datasource@@UAA_NJPAXJ@Z
bool c_game_options_category_datasource::get_element(long index, void *element, long element_size)
{
    const s_variant_setting_edit_reference *reference =
        (m_category >= 0 && m_category < 217) ? gui_screen_game_options_variant_setting_edit_reference_try_and_get(m_category) : 0;
    bool result;
    if (element && element_size == sizeof(s_variant_setting_option) && reference && index >= 0 && index < reference->options.count)
    {
        const s_variant_setting_option *option = &((const s_variant_setting_option *)reference->options.address)[index];
        *(s_variant_setting_option *)element = *option;
        result = true;
    }
    else
        result = false;
    return result;
}

// @address 0x4d3660 ?get_current_item_count_internal@c_game_options_category_datasource@@MAAJXZ
long c_game_options_category_datasource::get_current_item_count_internal(void)
{
    long count;
    const s_variant_setting_edit_reference *reference =
        (m_category >= 0 && m_category < 217) ? gui_screen_game_options_variant_setting_edit_reference_try_and_get(m_category) : 0;
    if (reference)
        count = reference->options.count;
    else
        count = 0;
    return count;
}



static __forceinline void gui_screen_game_options_copy_variant_name(wchar_t *destination, const c_game_variant *variant)
{
    const wchar_t *source = variant->name;
    long index = 0;
    do
    {
        wchar_t character = *source;
        destination[index] = character;
        if (character == 0)
            break;
        source++;
        index++;
    } while (index <= 127);
    destination[127] = 0;
}








// @address 0x4d36c0 ??0c_load_game_options_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@JPBVc_game_variant@@2W4e_game_engine_category@@3JPAUs_game_options_history_stack_node@@J@Z
c_load_game_options_screen_message::c_load_game_options_screen_message(
    long screen_name,
    e_controller_index controller_index,
    e_window_index window_index,
    long layered_position,
    const c_game_variant *original_variant,
    const c_game_variant *edited_variant,
    e_game_engine_category category,
    e_game_engine_category subcategory,
    long item,
    s_game_options_history_stack_node *history,
    long history_depth)
    : c_load_screen_message(_game_options_string_game_options, controller_index, window_index, layered_position)
{
    m_category = category;
    m_subcategory = subcategory;
    m_item = item;
    memset(m_history, 0, sizeof(m_history));
    m_layered_position = layered_position;
    m_history_depth = history_depth;
    game_variant_set_from(&m_original_variant, original_variant);
    game_variant_set_from(&m_edited_variant, edited_variant);
    if (history && m_subcategory >= 0 && m_subcategory < 217)
    {
        if (history_depth >= 0 && (unsigned long)history_depth < 16)
        {
            memcpy(m_history, history, history_depth * sizeof(s_game_options_history_stack_node));
            return;
        }
        m_history_depth = 15;
        return;
    }
    memset(m_history, -1, sizeof(m_history));
}


extern "C" {
// @address 0x54ef0 ??_7c_game_options_category_datasource@@6B@
// @address 0x4c0ee0 ??_Ec_game_options_category_datasource@@UAAPAXI@Z
// @address 0x54e80 ??_7c_game_options_parameter_datasource@@6B@
// @address 0x4c0ee0 ??_Ec_game_options_parameter_datasource@@UAAPAXI@Z
// @address 0x54db8 ??_7c_gui_screen_game_options@@6B@
// @address 0x4d39e8 ??_Ec_gui_screen_game_options@@UAAPAXI@Z
}

// @address 0x4d3878 ??0c_gui_screen_game_options@@QAA@J@Z
c_gui_screen_game_options::c_gui_screen_game_options(long name) :
    c_gui_screen_widget(name),
    m_original_variant(),
    m_edited_variant(),
    m_editing_parameter(-1),
    m_history(),
    m_history_depth(0),
    m_history_item(-1),
    m_operation(0),
    m_dialog_action(-1),
    m_dialog_result(0),
    m_variant_file(),
    m_asynchronous_operation_controller(_controller_none),
    m_keyboard_task(0),
    m_variant_name(),
    m_string_verify_task("..\\interface\\gui_screens\\game_options\\gui_screen_game_options.cpp" /* __FILE__ */, 0x190),
    m_variant_title(),
    m_variant_description(),
    m_create_request(-1),
    m_write_request(-1),
    m_delete_request(-1),
    m_save_request(-1),
    m_variant_success(0),
    m_variant_done(0),
    m_variant_content_item(-1),
    m_variant_progress(0)
{
}


// @address 0x4d3820 ?apply_initial_state@c_load_game_options_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_game_options_screen_message::apply_initial_state(c_gui_screen_widget *widget) const
{
    ((c_gui_screen_game_options *)widget)->apply_initial_state(
        &m_original_variant,
        &m_edited_variant,
        (e_game_engine_category)m_category,
        (e_game_engine_category)m_subcategory,
        m_item,
        m_history,
        m_history_depth,
        m_layered_position);
}




static __forceinline long gui_screen_game_options_get_template_setting(c_gui_screen_game_options *screen)
{
    if (screen->m_history_depth >= 0 && (unsigned long)screen->m_history_depth < 16)
        return screen->m_history[screen->m_history_depth].category;
    return -1;
}

static __forceinline long gui_screen_game_options_get_interface_setting(c_gui_screen_game_options *screen)
{
    if (screen->m_history_depth >= 0 && (unsigned long)screen->m_history_depth < 16)
        return screen->m_history[screen->m_history_depth].parent_category;
    return -1;
}

// The datasource list lives in the base widget class, not in this unit's declaration.
// @address 0x4d3b80 ?initialize_datasource@c_gui_screen_game_options@@UAAXXZ
void c_gui_screen_game_options::initialize_datasource(void)
{
    c_gui_screen_widget::initialize_datasource();

    void *memory = user_interface_malloc_tracked(sizeof(c_game_options_category_datasource), 0, 0);
    if (memory)
    {
        long interface_setting = gui_screen_game_options_get_interface_setting(this);
        long template_setting = gui_screen_game_options_get_template_setting(this);
        c_gui_data *datasource = (c_gui_data *)new (memory)
            c_game_options_category_datasource((e_game_engine_category)template_setting, (e_game_engine_category)interface_setting, &m_original_variant);
        if (datasource->initialize(66436))
        {
            add_datasource(datasource);
            long count = datasource->get_current_item_count();
            for (long index = 0; index < count; index++)
            {
                s_variant_setting_option element;
                datasource->get_element(index, &element, 60);
                if (element.category.index == -1 && element.template_category.index == -1)
                {
                    if (element.parameter.index != -1)
                    {
                        const s_text_value_pair_definition *definition =
                            TAG_GET(element.parameter.group_tag, const s_text_value_pair_definition, element.parameter.index);
                        void *parameter_memory = user_interface_malloc_tracked(sizeof(c_game_options_parameter_datasource), 0, 0);
                        if (parameter_memory)
                        {
                            c_gui_data *parameter = (c_gui_data *)new (parameter_memory)
                                c_game_options_parameter_datasource((e_game_variant_parameter)definition->parameter);
                            if (parameter->initialize(definition->name))
                            {
                                add_datasource(parameter);
                            }
                            else
                            {
                                parameter->~c_gui_data();
                                user_interface_free(parameter);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            datasource->~c_gui_data();
            user_interface_free(datasource);
        }
    }
}



// @address 0x4d3a98 ?initialize@c_gui_screen_game_options@@UAAXXZ
void c_gui_screen_game_options::initialize(void)
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<current-value", this, parse_current_selected_value));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<current-name", this, parse_current_selected_name));
}

// @address 0x4d3dc8 ?post_initialize@c_gui_screen_game_options@@UAAXXZ
void c_gui_screen_game_options::post_initialize(void)
{
    c_gui_screen_widget::post_initialize();

    if (m_history_depth >= 0 && (unsigned long)m_history_depth < 16)
    {
        long focus_item = m_history[m_history_depth].item;
        if (focus_item != -1)
        {
            c_gui_list_widget *list = get_child_list_widget(_game_options_string_game_options);
            if (list)
                transfer_focus_to_list(list, focus_item, false, false);
        }
    }
}

// @address 0x4d3e60 ?apply_initial_state@c_gui_screen_game_options@@QAAXPBVc_game_variant@@0W4e_game_engine_category@@1JPBUs_game_options_history_stack_node@@JJ@Z
void c_gui_screen_game_options::apply_initial_state(
    const c_game_variant *original_variant,
    const c_game_variant *edited_variant,
    e_game_engine_category category,
    e_game_engine_category parent_category,
    long selected_item,
    const s_game_options_history_stack_node *history,
    long depth,
    long item)
{
    game_variant_set_from(&m_original_variant, original_variant);
    game_variant_set_from(&m_edited_variant, edited_variant);
    if (history)
    {
        if (depth >= 0 && (unsigned long)depth < 16)
        {
            memcpy(m_history, history, sizeof(m_history));
        }
    }
    else
    {
        memset(m_history, -1, sizeof(m_history));
    }
    m_history_depth = depth;
    m_history_item = item;
    if ((unsigned long)m_history_depth < 16)
    {
        m_history[m_history_depth].category = category;
        m_history[m_history_depth].parent_category = parent_category;
        m_history[m_history_depth].item = selected_item;
    }
    else
    {
        m_history_depth = 15;
    }
}


// @address 0x1e8578 ?game_engine_index_to_saved_game_file_type@@YA?AW4e_saved_game_file_type@@W4e_game_engine_type@@@Z

// @address 0x4d6208 ?begin_save_as_current_edited_variant@c_gui_screen_game_options@@IAAXW4e_controller_index@@@Z
void c_gui_screen_game_options::begin_save_as_current_edited_variant(e_controller_index controller_index)
{
    s_gui_screen_game_options_saved_game_context descriptor;
    wchar_t name[16];

    if (m_operation != 0)
        return;
    if (m_keyboard_task == NULL)
        m_keyboard_task = c_virtual_keyboard_task::get_instance(
            "..\\interface\\gui_screens\\game_options\\gui_screen_game_options.cpp" /* __FILE__ */, 0x88e,
            _controller_none, 0, 0, 0, 0x100, 0);
    if (m_keyboard_task == NULL)
        return;
    short task_state = (short)m_keyboard_task->get_task_state();
    bool busy = task_state == _overlapped_task_state_starting || task_state == _overlapped_task_state_pending
        || task_state == _overlapped_task_state_completing;
    if (busy)
        return;
    const s_gui_game_setup_storage *setup = global_preferences_get_game_setup_storage();
    memcpy(&m_saved_setup, &setup->multiplayer_settings.game_variant.flags, sizeof(m_saved_setup));
    long engine_type = m_original_variant.engine_type;
    c_static_wchar_string<1024> title_text;
    c_static_wchar_string<1024> description_text;
    descriptor.zero4 = 0;
    descriptor.zero12 = 0;
    descriptor.name = (unsigned short *)m_original_variant.name;
    short user_index = g_controller_globals.controllers[controller_index].user_index;
    descriptor.handle = -1;
    descriptor.index = user_index;
#ifdef HALO3_64BIT
    // The descriptor mirrors s_saved_game_files_context's 32-bit layout (index, first, second, third, difficulty);
    // with 8-byte string pointers its name would land on third_string's upper half and its zeroes and -1 on
    // second_string, which the <game_variant_name> parser then copies from (0xffffffff00000000).
    s_saved_game_files_context descriptor_context = {};
    descriptor_context.user_index = descriptor.index;
    descriptor_context.second_string = m_original_variant.name;
    descriptor_context.campaign_difficulty = descriptor.handle;
    saved_game_files_set_text_parse_context(&descriptor_context);
#else
    saved_game_files_set_text_parse_context((const s_saved_game_files_context *)&descriptor);
#endif
    bool found = user_interface_get_formatted_string(user_index, 0x73617665, 0x70002, name, 16);
    if (found)
        saved_game_files_find_unique_name(controller_index, game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type),
            name, 16);
    saved_game_files_reset_text_parse_context();
    if (!found)
    {
        const wchar_t *source = m_original_variant.name;
        for (long index = 0; index <= 15; index++)
        {
            wchar_t character = *source;
            name[index] = character;
            if (character == 0)
                break;
            source++;
        }
        name[15] = 0;
    }
    get_string_by_string_id(_game_options_string_save_game_settings_name, &title_text);
    get_string_by_string_id(_game_options_string_save_game_settings_description, &description_text);
    m_keyboard_task->set_controller_index(controller_index);
    m_keyboard_task->set_default_text(name);
    m_keyboard_task->set_title_text(title_text.get_string());
    m_keyboard_task->set_description_text(description_text.get_string());
    m_keyboard_task->set_maximum_input_characters(16);
    m_keyboard_task->set_character_flags(0x4001);
    if (overlapped_task_start_internal(m_keyboard_task,
            "..\\interface\\gui_screens\\game_options\\gui_screen_game_options.cpp" /* __FILE__ */, 0x8b0))
    {
        m_asynchronous_operation_controller = controller_index;
        m_operation = 2;
        return;
    }
    m_keyboard_task->reset();
}

// @address 0x33614 k_game_engine_type_string_ids

// @address 0x4d3f48 ?setup_game_variant_for_write@c_gui_screen_game_options@@AAAXPAUs_blffile_game_variant@@PB_WPBVc_game_variant@@_N@Z
void c_gui_screen_game_options::setup_game_variant_for_write(
    s_blffile_game_variant *blf, const wchar_t *name, const c_game_variant *variant, bool use_string_table)
{
    long controller = get_any_responding_controller();
    long engine_type = variant->engine_type;
    wchar_t name_buffer[128];

    if (use_string_table)
    {
        long string_table_id = (unsigned long)engine_type <= 10 ? k_game_engine_type_string_ids[engine_type] : _string_id_unknown;
        unsigned short string_table_text[1024];
        string_table_text[0] = 0;
        if (user_interface_global_string_get(string_table_id, string_table_text))
        {
            ustrnzcpy(name_buffer, (const wchar_t *)string_table_text, 128);
        }
        else
        {
            gui_screen_game_options_copy_variant_name(name_buffer, variant);
        }
    }
    else
    {
        gui_screen_game_options_copy_variant_name(name_buffer, variant);
    }

    struct s_setup_game_variant_key
    {
        long f_0;
        long f_4;
        wchar_t *f_8;
        long f_c;
        long f_10;
    } key;
    short user_index = g_controller_globals.controllers[controller].user_index;
    key.f_4 = 0;
    key.f_c = 0;
    key.f_8 = name_buffer;
    key.f_10 = -1;
    key.f_0 = user_index;

#ifdef HALO3_64BIT
    // key mirrors s_saved_game_files_context's 32-bit layout: build the real one (8-byte string pointers)
    s_saved_game_files_context key_context = {};
    key_context.user_index = key.f_0;
    key_context.second_string = key.f_8;
    key_context.campaign_difficulty = key.f_10;
    saved_game_files_set_text_parse_context(&key_context);
#else
    saved_game_files_set_text_parse_context((const s_saved_game_files_context *)&key);
#endif
    wchar_t *description = m_variant_description;
    bool description_found = user_interface_get_formatted_string(user_index, 1935767141, 458755, description, 128);
    saved_game_files_reset_text_parse_context();
    if (!description_found)
    {
        unsigned short description_text[256];
        description_text[0] = 0;
        utf8_string_to_wchar_string((const utf8 *)variant->get_active_variant()->m_metadata.description,
            (wchar_t *)description_text, 256, NULL);
        ustrnzcpy(description, (const wchar_t *)description_text, 128);
    }

    char description_utf8[128];
    wchar_string_to_utf8_string(description, description_utf8, NUMBEROF(description_utf8), 0);

    c_game_engine_base_variant *base_variant = (c_game_engine_base_variant *)variant->get_active_variant();
    base_variant->set_built_in(false);
    game_engine_variant_set_description(base_variant, description_utf8);

    unsigned __int64 identifier = game_variant_compute_unique_id(variant);
    saved_game_files_initialize_metadata((s_saved_game_item_metadata_record *)&blf->get_metadata(), controller,
        game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type), name, description, identifier, 956);
    blf->get_metadata().game_engine_type = engine_type;
    game_variant_set_from(&blf->game_variant.game_variant, variant);
}

// @address 0x4d5a10 gui_screen_game_options_can_back_out
bool gui_screen_game_options_can_back_out(c_gui_screen_widget *screen)
{
    bool result = true;
    c_gui_widget *focused_widget = screen->get_focused_widget();
    e_animation_state transition_states[12];
    transition_states[0] = _animation_state_transition_from_screen;
    transition_states[1] = _animation_state_transition_to_screen;
    transition_states[2] = _animation_state_transition_back_from_screen;
    transition_states[3] = _animation_state_transition_back_to_screen;
    transition_states[4] = _animation_state_cycle_display_panes_previous;
    transition_states[5] = _animation_state_cycle_display_panes_next;
    transition_states[6] = _animation_state_list_display_group_transition_in;
    transition_states[7] = _animation_state_list_display_group_transition_out;
    transition_states[8] = _animation_state_custom_transition_in0;
    transition_states[9] = _animation_state_custom_transition_out0;
    transition_states[10] = _animation_state_custom_transition_in1;
    transition_states[11] = _animation_state_custom_transition_out1;
    if (focused_widget != NULL)
    {
        c_gui_widget *child = gui_widget_find_child_by_name(focused_widget, 0);
        if (child != NULL)
        {
            for (unsigned long index = 0; index < NUMBEROF(transition_states); index++)
            {
                if (child->is_animation_active(transition_states[index]))
                {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}












static __forceinline unsigned long gui_screen_game_options_clock_now()
{
    unsigned long milliseconds = g_user_interface_globals.milliseconds.peek();
    return milliseconds;
}

// @address 0x4d6468 ?load_progress_ui@c_gui_screen_game_options@@QAAXW4e_controller_index@@@Z
void c_gui_screen_game_options::load_progress_ui(e_controller_index controller_index)
{
    c_load_in_progress_screen_message *message = new(0, 0, _ui_allocation_marker_dummy)
        c_load_in_progress_screen_message(controller_index, get_render_window(),
            _game_options_string_game_variant_save_progress_title, _game_options_string_game_variant_save_progress_message);
    if (message)
    {
        message->set_parent_screen_index(get_screen_index());
        user_interface_messaging_post(message);
        m_variant_progress.value = gui_screen_game_options_clock_now();
    }
}

// @address 0x4d60b8 ?begin_save_current_edited_variant@c_gui_screen_game_options@@IAAXW4e_controller_index@@@Z
void c_gui_screen_game_options::begin_save_current_edited_variant(e_controller_index controller)
{
    if (m_operation != 0)
        return;
    long *async_slot = &m_save_request;
    if (*async_slot != -1)
        return;
    const s_gui_game_setup_storage *setup = global_preferences_get_game_setup_storage();
    memcpy(&m_saved_setup, &setup->multiplayer_settings, sizeof(m_saved_setup));
    if (m_saved_setup.settings.game_variant.delete_pending != 0 && m_saved_setup.settings.game_variant.delete_type == 1)
    {
        long engine_type = m_original_variant.engine_type;
        c_game_variant *variant = &m_original_variant;
        s_file_reference file;
        s_file_reference *reference = file_reference_create_from_path(&file, m_saved_setup.settings.game_variant.path, false);
        long content_item = content_catalogue_find_item_by_file_reference((const c_content_catalogue *)&content_catalogues().catalogues[controller], reference);
        __lwsync();
        c_synchronized_long *slot = &m_variant_content_item;
        _InterlockedExchange((long *)slot, content_item);
        long stored = slot->value;
        __lwsync();
        if (stored != -1)
        {
            s_blffile_game_variant *blf = &m_variant_file;
            setup_game_variant_for_write(blf, m_original_variant.name, variant, false);
            long current = slot->value;
            __lwsync();
            c_synchronized_long *completion = &m_variant_success;
            c_synchronized_long *done = &m_variant_done;
            e_saved_game_file_type file_type = game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type);
            *async_slot = saved_game_files_write_content_item((long)controller, current, (long)file_type, blf, 0x3bc, completion, done);
        }
    }
    if (*async_slot == -1)
        return;
    m_asynchronous_operation_controller = controller;
    m_operation = 1;
    load_progress_ui(controller);
}




// @address 0x4d6568 ?parse_current_selected_item_nugget@@YA_NPAXJPA_WJ@Z
static __forceinline long gui_screen_game_options_get_handle(c_gui_widget *widget)
{
    long handle;
    if (widget)
        handle = widget->get_element_handle();
    else
        handle = -1;
    return handle;
}

bool parse_current_selected_item_nugget(void *context, long value_name, wchar_t *output, long capacity)
{
    c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
    bool result = false;
    c_gui_widget *selected = screen->get_focused_widget();
    for (long element_handle = gui_screen_game_options_get_handle(selected); element_handle != -1; element_handle = -1)
    {
        c_gui_list_widget *list = gui_widget_get_parent_list_widget(selected);
        if (!list)
            continue;
        c_gui_data *data = (list->get_data());
        if (!data)
            continue;
        long integer_value;
        if (data->get_integer_value(element_handle, value_name, &integer_value))
        {
            usnzprintf(output, capacity, L"%ld", integer_value);
            result = true;
            continue;
        }
        long string_id;
        result = data->get_string_id_value(element_handle, value_name, &string_id);
        if (!result)
        {
            result = false;
            continue;
        }
        c_static_wchar_string<1024> text;
        result = screen->get_string_by_string_id(string_id, &text);
        if (!result)
        {
            result = false;
            continue;
        }
        ustrnzcpy(output, text.get_string(), capacity);
        result = true;
        continue;
    }
    return result;
}

// @address 0x4d66b8 ?parse_current_selected_value@@YA_NPAXPA_WJ@Z
bool parse_current_selected_value(void *context, wchar_t *output, long capacity)
{
    return parse_current_selected_item_nugget(context, _game_options_string_value, output, capacity);
}

// @address 0x4d66c8 ?parse_current_selected_name@@YA_NPAXPA_WJ@Z
bool parse_current_selected_name(void *context, wchar_t *output, long capacity)
{
    return parse_current_selected_item_nugget(context, _game_options_string_name, output, capacity);
}



typedef long (__cdecl *gui_screen_game_options_delete_async_abi)(int controller_index, const unsigned short *path, int file_type,
    c_synchronized_long *completion, c_synchronized_long *done, c_synchronized_long *status);

// @address 0x4d4a40 ?delete_variant_file@c_gui_screen_game_options@@IAA_NPB_W@Z
bool c_gui_screen_game_options::delete_variant_file(const wchar_t *path)
{
    int file_type = game_engine_index_to_saved_game_file_type((e_game_engine_type)m_original_variant.engine_type);
    if (file_type != -1)
    {
        c_game_variant *variant = &m_original_variant;
        c_game_engine_base_variant *base_variant = (c_game_engine_base_variant *)variant->get_active_variant();
        base_variant->set_built_in(false);
        gui_screen_game_options_set_variant_name(base_variant, m_variant_name);
        c_synchronized_long *completion = &m_variant_success;
        c_synchronized_long *done = &m_variant_done;
        c_synchronized_long *status = &m_variant_content_item;
        long result = reinterpret_cast<gui_screen_game_options_delete_async_abi>(&saved_game_files_create_content_item_async)(
            m_asynchronous_operation_controller, (const unsigned short *)path, file_type, completion, done, status);
        m_create_request = result;
        if (result != -1)
        {
            load_progress_ui(m_asynchronous_operation_controller);
        }
        else
        {
            g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
    }
    return false;
}





static __forceinline c_content_catalogue *gui_screen_game_options_get_controller_catalogue(e_controller_index controller)
{
    const c_controller_interface *owner = &g_controller_globals.controllers[controller];
    long index = owner - g_controller_globals.controllers;
    return (c_content_catalogue *)&content_catalogues().catalogues[index];
}

static __forceinline c_content_catalogue *gui_screen_game_options_get_catalogue(e_controller_index controller)
{
    return (c_content_catalogue *)&content_catalogues().catalogues[controller];
}

static __forceinline bool gui_screen_game_options_variant_file_exists(c_gui_screen_game_options *screen, e_controller_index controller)
{
    c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(controller);
    e_game_content_type type = game_engine_type_to_game_content_type((e_game_engine_type)screen->m_original_variant.engine_type);
    long item = catalogue->try_and_get_personal_storage_item(screen->m_original_variant.get_name(), (e_content_item_status)type);
    return item != -1;
}

static __forceinline bool gui_screen_game_options_handle_base_dialog(const c_dialog_result_message *message)
{
    bool result = false;
    if (message->m_dialog_name == _game_options_string_campaign_changing_setting_will_delete_saved_game)
    {
        if (message->m_dialog_result == 0)
            content_catalogue_display_device_selection_guide_interface(message->m_controller);
        result = true;
    }
    return result;
}

static bool gui_screen_game_options_saved_game_variant_file_already_exists(c_gui_screen_game_options *screen, const c_dialog_result_message *message)
{
    bool result = false;
    if (message)
    {
        c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(message->m_controller);
        e_game_content_type type = game_engine_type_to_game_content_type((e_game_engine_type)screen->m_original_variant.engine_type);
        result = catalogue->try_and_get_personal_storage_item(
                     screen->m_original_variant.get_name(), (e_content_item_status)type) != -1;
    }
    return result;
}

// @address 0x4d4f40 ?handle_dialog_result@c_gui_screen_game_options@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_game_options::handle_dialog_result(const c_dialog_result_message *message)
{
    long action = -1;
    bool handled = true;
    switch (message->m_dialog_name)
    {
    case _game_options_string_variant_save_options_dirty:
        switch (message->m_dialog_result)
        {
        case 0: action = 1; break;
        case 1: action = 3; break;
        }
        break;
    case _game_options_string_variant_save_options_nonwriteable:
        switch (message->m_dialog_result)
        {
        case 0: action = 1; break;
        case 1: action = 0; break;
        case 2: action = 2; break;
        case 3: action = 3; break;
        }
        break;
    case _game_options_string_pregame_selection_item_options:
        switch (message->m_dialog_result)
        {
        case 0: action = 1; break;
        case 1: action = 2; break;
        case 2: action = 3; break;
        }
        break;
    case _game_options_string_confirm_saved_game_delete:
        switch (message->m_dialog_result)
        {
        case 0:
            action = m_dialog_action;
            m_dialog_result = 1;
            break;
        case 1:
            m_dialog_result = 2;
            break;
        }
        break;
    case _game_options_string_file_share_confirm_delete_from_share:
        switch (message->m_dialog_result)
        {
        case 0:
            action = m_dialog_action;
            m_dialog_result = 1;
            break;
        case 1:
            m_dialog_result = 2;
            break;
        }
        break;
    default:
        handled = false;
        break;
    }
    if (handled)
    {
        switch (action)
        {
        case 0:
        {
            bool file_exists = gui_screen_game_options_saved_game_variant_file_already_exists(this, message);
            bool overwrite_confirmed = m_dialog_result == 1;
            if (file_exists && !overwrite_confirmed)
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (memory)
                {
                    long name = m_name;
                    e_controller_index controller = message->m_controller;
                    c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(controller,
                        get_render_window(), name, _game_options_string_confirm_saved_game_delete, name);
                    if (dialog)
                    {
                        dialog->set_parent_screen_index(m_screen_index);
                        ::user_interface_messaging_post(dialog);
                        m_dialog_action = 0;
                        m_dialog_result = 0;
                    }
                }
            }
            else
            {
                e_controller_index controller = message->m_controller;
                if (!content_catalogue_has_storage_device(gui_screen_game_options_get_controller_catalogue(controller)))
                    user_interface_show_device_selection_dialog(controller, m_name, m_screen_index);
                else
                {
                    begin_save_current_edited_variant(controller);
                    m_dialog_action = -1;
                    m_dialog_result = 0;
                }
            }
            break;
        }
        case 1:
        {
            e_controller_index controller = message->m_controller;
            if (!content_catalogue_has_storage_device(gui_screen_game_options_get_controller_catalogue(controller)))
                user_interface_show_device_selection_dialog(controller, m_name, m_screen_index);
            else if (m_dialog_action != 1)
            {
                    begin_save_as_current_edited_variant(controller);
                    m_dialog_action = -1;
                    m_dialog_result = 0;
            }
            break;
        }
        case 2:
            m_operation = 4;
            break;
        }
    }
    if (!handled)
        handled = gui_screen_game_options_handle_base_dialog(message);
    return handled;
}






static __forceinline bool gui_screen_game_options_can_edit()
{
    bool allowed;
    if (user_interface_squad_exists())
        allowed = ::network_life_cycle_squad_local_peer_is_leader();
    else
        allowed = true;
    return allowed;
}

// @address 0x4d5cb0 ?propagate_changes_to_variant_without_saving@c_gui_screen_game_options@@IAAXW4e_controller_index@@@Z
void c_gui_screen_game_options::propagate_changes_to_variant_without_saving(e_controller_index controller)
{
    const c_game_variant *session_variant = user_interface_game_settings_get_game_variant();
    if (session_variant)
    {
        c_game_variant *variant = &m_original_variant;
        if (!session_variant->is_equal_to(variant))
        {
            if (!game_engine_variant_is_valid(variant))
                game_engine_variant_describe_invalidity(variant);
            else if (::network_life_cycle_squad_local_peer_is_leader())
                user_interface_squad_set_game_variant_with_verify(variant);
        }
        if (session_variant->get_game_engine_index() != 5)
        {
            const s_gui_game_setup_storage *source = global_preferences_get_game_setup_storage();
            s_gui_game_setup_storage setup;
            memcpy(&setup, source, sizeof(setup));
            setup.multiplayer_settings.game_variant.variant.copy_from_and_validate(variant);
            global_preferences_set_last_game_setup(&setup);
        }
    }
}



static __forceinline long gui_screen_game_options_synchronized_read(const c_synchronized_long *value)
{
    long result = value->value;
    __lwsync();
    return result;
}

// @address 0x4d4150 ?update@c_gui_screen_game_options@@UAAXK@Z
void c_gui_screen_game_options::update(unsigned long current_milliseconds)
{
    switch (m_operation)
    {
    case 0:
    {
        c_gui_screen_widget *progress = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, _string_id_in_progress);
        if (progress != 0)
        {
            bool busy = progress->transitioning_out();
            if (!busy && (unsigned long)(gui_screen_game_options_clock_now() - m_variant_progress.value) >= 0xbb8)
            {
                progress->transition_out_with_transition_type(0, _screen_transition_normal);
                m_variant_progress.value = 0;
            }
        }
        break;
    }
    case 1:
        update_save_operation();
        break;
    case 2:
        update_save_as_new_operation();
        break;
    case 3:
        update_rename_operation();
        break;
    case 4:
        m_original_variant.copy_from_and_validate(&m_edited_variant);
        m_operation = 0;
        m_dialog_action = -1;
        m_dialog_result = 0;
        break;
    default:
        // match: 0x4d4150: unreachable hint omits dead branch check
        __assume(0);
    }

    c_gui_group_widget *display_group = m_current_display_group_widgets[1];
    if (display_group != 0)
    {
        long name = global_preferences_get_last_game_setup(m_original_variant.engine_type)
            ? _game_options_string_button_key_game_options_party_leader
            : _game_options_string_button_key_game_options_non_party_leader;
        if (display_group->m_name != name)
            gui_screen_widget_set_display_group(this, 1, name);
    }
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4d42c0 ?update_save_operation@c_gui_screen_game_options@@QAAXXZ
void c_gui_screen_game_options::update_save_operation()
{
    bool completed = false;
    long *request = &this->m_save_request;
    bool success = false;
    if (*request != -1)
    {
        if (gui_screen_game_options_synchronized_read(&this->m_variant_done) == 1)
        {
            completed = true;
            if (gui_screen_game_options_synchronized_read(&this->m_variant_success) == 1)
            {
                success = true;
                this->propagate_changes_to_variant_without_saving((e_controller_index)this->m_asynchronous_operation_controller);
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
            }
            *request = -1;
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
        completed = true;
    }
    if (completed)
    {
        long *content_item = (long *)&this->m_variant_content_item.value;
        if (gui_screen_game_options_synchronized_read((const c_synchronized_long *)content_item) != -1)
        {
            e_controller_index controller = (e_controller_index)this->m_asynchronous_operation_controller;
            c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(controller);
            long index = gui_screen_game_options_synchronized_read((const c_synchronized_long *)content_item);
            content_catalogue_close_item(catalogue, index, false);
            __lwsync();
            _InterlockedExchange(content_item, -1);
        }
        this->m_operation = 0;
        this->m_dialog_action = -1;
        this->m_dialog_result = 0;
        if (success)
            game_variant_set_from(&this->m_edited_variant, &this->m_original_variant);
    }
}






// @address 0x4d5d90 ?propagate_changes_from_newly_created_game_variant_file@c_gui_screen_game_options@@IAAXW4e_controller_index@@J@Z
void c_gui_screen_game_options::propagate_changes_from_newly_created_game_variant_file(e_controller_index controller, long content_item_index)
{
    const c_content_catalogue *catalogue = (const c_content_catalogue *)&content_catalogues().catalogues[controller];
    char path[256];
    path[0] = 0;
    bool success = false;
    if (content_catalogue_get_item_file_path(catalogue, content_item_index, path, 256))
        success = true;

    if (success)
    {
        wchar_t description[256];
        description[0] = 0;
        utf8_string_to_wchar_string((const utf8 *)m_original_variant.get_active_variant()->m_metadata.description, description, 256, NULL);

        long engine_type = m_original_variant.engine_type;
        unsigned __int64 unique_id = game_variant_compute_unique_id(&m_original_variant);
        s_saved_game_item_metadata record;
        saved_game_files_initialize_metadata(&record, controller, game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type),
            m_original_variant.get_name(), description, unique_id, 956);
        record.game_engine_type = m_original_variant.engine_type;

        s_gui_selected_items_block metadata;
        gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&metadata), (const s_saved_game_item_metadata_packed *)(&record));
        bool is_new = content_catalogue_is_item_created(catalogue, content_item_index);
        bool corrupt = content_catalogue_is_item_corrupt(catalogue, content_item_index);
        s_file_reference file;
        c_gui_game_variant_selected_item selected_item(&metadata, static_cast<e_gui_selected_item_location>(1), controller,
            file_reference_create_from_path(&file, path, false), -1, -1, corrupt, is_new);
        g_pregame_setup_manager.set_selected_item(controller, (const c_gui_selected_item *)&selected_item, false, -1);
    }
}


static __forceinline bool gui_screen_game_options_user_is_editing_setting(const c_gui_screen_game_options *screen)
{
    return screen->m_editing_parameter >= 0 && screen->m_editing_parameter < 0x26d;
}

static __forceinline void gui_screen_game_options_finish_editing_setting(c_gui_screen_game_options *screen)
{
    screen->m_editing_parameter = -1;
}

// @address 0x4d5ed8 ?back_out_from_current_editing_category@c_gui_screen_game_options@@IAA_NW4e_controller_index@@@Z
bool c_gui_screen_game_options::back_out_from_current_editing_category(enum e_controller_index controller_index)
{
    bool result = true;

    if (gui_screen_game_options_user_is_editing_setting(this))
    {
        gui_screen_game_options_finish_editing_setting(this);
        result = false;
    }
    else if (m_history_depth >= 0)
    {
        long transition = m_history_depth > 0 ? 3 : 1;

        transition_out_with_transition_type(1, (e_screen_transition_type)transition);

        m_history[m_history_depth].category = -1;
        m_history[m_history_depth].parent_category = -1;
        m_history[m_history_depth].item = -1;
        m_history_depth--;

        if (m_history_depth >= 0)
        {
            long category = m_history[m_history_depth].category;
            long parent_category = m_history[m_history_depth].parent_category;
            long item = m_history[m_history_depth].item;

            c_load_game_options_screen_message *message = new (0, 0, _ui_allocation_marker_dummy) c_load_game_options_screen_message(
                _game_options_string_game_options,
                controller_index,
                get_render_window(),
                m_history_item,
                &m_original_variant,
                &m_edited_variant,
                (e_game_engine_category)category,
                (e_game_engine_category)parent_category,
                item,
                m_history,
                m_history_depth);
            if (message)
            {
                message->m_transition_type = _screen_transition_start_menu_previous;
                ::user_interface_messaging_post(message);
            }
        }
        else if (gui_screen_game_options_can_edit())
        {
            propagate_changes_to_variant_without_saving(controller_index);
        }
    }

    return result;
}

// @address 0x4d5b30 ?handle_controller_input_message@c_gui_screen_game_options@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_game_options::handle_controller_input_message(class c_controller_input_message const *p0)
{
    const c_controller_input_message *message = p0;
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press)
    {
        switch (message->get_component())
        {
        case 1:
        case _controller_component_button_back:
            if (gui_screen_game_options_can_back_out(this))
                handled = back_out_from_current_editing_category(message->get_controller());
            break;
        case 2:
            if (((unsigned char)::global_preferences_get_last_game_setup(m_original_variant.engine_type)))
            {
                s_gui_game_setup_storage *setup = global_preferences_get_game_setup_storage();
                bool existing = false;
                if (setup->multiplayer_settings.game_variant.delete_pending != 0 && setup->multiplayer_settings.game_variant.delete_type == 1)
                {
                    s_file_reference file;
                    long controller = message->get_controller();
                    s_file_reference *reference = file_reference_create_from_path(&file, setup->multiplayer_settings.game_variant.path, false);
                    if (content_catalogue_find_item_by_file_reference((const c_content_catalogue *)&content_catalogues().catalogues[controller],
                        reference) != -1)
                        existing = true;
                }
                long dialog_name;
                if (existing)
                    dialog_name = _game_options_string_variant_save_options_nonwriteable;
                else
                    dialog_name = _game_options_string_variant_save_options_dirty;
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (memory)
                {
                    long name = m_name;
                    e_controller_index controller = message->get_controller();
                    c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(
                        controller, get_render_window(), _string_id_top_most, dialog_name, name);
                    if (dialog)
                    {
                        ::user_interface_messaging_post(dialog);
                        handled = true;
                    }
                }
            }
            break;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(p0);
    return handled;
}


static __forceinline bool gui_screen_game_options_base_handle_list_item_chosen()
{
    return false;
}

static __forceinline c_gui_list_item_widget *gui_screen_game_options_get_focused_widget(c_gui_screen_game_options *screen)
{
    return (c_gui_list_item_widget *)screen->get_focused_widget();
}

static __forceinline void gui_screen_game_options_set_variant_author(c_gui_screen_game_options *screen, e_controller_index controller_index)
{
    s_saved_game_item_metadata_packed metadata;
    memcpy(&metadata, &screen->m_original_variant.get_active_variant()->m_metadata, sizeof(metadata));
    c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
    memset(metadata.author, 0, sizeof(metadata.author));
    if (controller->is_signed_in_to_live())
    {
        const wchar_t *source = controller->display_name;
        char *destination = metadata.author;
        long remaining = 16;
        wchar_t character;
        do
        {
            character = *source;
            if (remaining == 1)
                *destination = 0;
            else if (character <= 127)
                *destination = (char)character;
            else
                *destination = '?';
            ++source;
            ++destination;
            --remaining;
        } while (character != 0 && remaining > 0);
    }
    unsigned __int64 xuid = 0;
    if (controller->user_index >= 0 && controller->user_index < 4)
        xuid = g_network_session_interface_local_users[controller->user_index].xuid;
    metadata.author_id = xuid;
    metadata.author_is_online = ((const c_controller_interface *)controller)->is_signed_in_to_live();
    c_game_engine_base_variant *base = (c_game_engine_base_variant *)screen->m_original_variant.get_active_variant();
    base->set_built_in(false);
    game_engine_variant_set_metadata((s_game_engine_base_variant_metadata_view *)base, &metadata);
}

// @address 0x4d5278 ?handle_list_item_chosen@c_gui_screen_game_options@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_game_options::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;

    switch (list_name)
    {
    case _game_options_string_game_options:
        if (gui_screen_game_options_can_back_out(this))
        {
            long element_handle = item->get_element_handle();
            s_variant_setting_option option;
            if (data->get_element(element_handle, &option, sizeof(option)))
            {
                if (option.template_category.index != -1)
                {
                    const s_game_variant_option_tag *definition = option.template_category.index != NONE
                        ? (const s_game_variant_option_tag *)tag_get(option.template_category.group_tag, option.template_category.index)
                        : NULL;
                    long template_setting = option.flags;
                    long category = definition->item_count > 0 ? definition->entries[0].category : -1;
                    if (category != -1 && m_history_depth >= 0 && (unsigned long)m_history_depth < 16)
                    {
                        m_history[m_history_depth].item = element_handle;
                        ++m_history_depth;
                        c_load_game_options_screen_message *load_message = new (0, 0, _ui_allocation_marker_dummy) c_load_game_options_screen_message(
                            _game_options_string_game_options,
                            message->get_controller(),
                            get_render_window(),
                            m_history_item,
                            &m_original_variant,
                            &m_edited_variant,
                            (e_game_engine_category)template_setting,
                            (e_game_engine_category)category,
                            -1,
                            m_history,
                            m_history_depth);
                        if (load_message)
                        {
                            load_message->m_transition_type = _screen_transition_start_menu_next;
                            ::user_interface_messaging_post(load_message);
                            handled = true;
                        }
                    }
                }
                if (!handled)
                {
                    if (option.category.index != -1)
                    {
                        const s_game_variant_option_tag *definition = option.category.index != NONE
                            ? (const s_game_variant_option_tag *)tag_get(option.category.group_tag, option.category.index)
                            : NULL;
                        long template_setting = gui_screen_game_options_get_template_setting(this);
                        long category = definition->item_count > 0 ? definition->entries[0].category : -1;
                        if (category != -1 && m_history_depth >= 0 && (unsigned long)m_history_depth < 16)
                        {
                            m_history[m_history_depth].item = element_handle;
                            ++m_history_depth;
                            c_load_game_options_screen_message *load_message = new (0, 0, _ui_allocation_marker_dummy) c_load_game_options_screen_message(
                                _game_options_string_game_options,
                                message->get_controller(),
                                get_render_window(),
                                m_history_item,
                                &m_original_variant,
                                &m_edited_variant,
                                (e_game_engine_category)template_setting,
                                (e_game_engine_category)category,
                                -1,
                                m_history,
                                m_history_depth);
                            if (load_message)
                            {
                                load_message->m_transition_type = _screen_transition_start_menu_next;
                                ::user_interface_messaging_post(load_message);
                                handled = true;
                            }
                        }
                    }
                    else if (option.parameter.index != -1)
                    {
                        if (gui_screen_game_options_can_edit())
                        {
                            const s_game_variant_option_tag *definition = option.parameter.index != NONE
                                ? (const s_game_variant_option_tag *)tag_get(option.parameter.group_tag, option.parameter.index)
                                : NULL;
                            long parameter = gui_screen_game_options_get_template_setting(this) != -1
                                ? get_player_trait_parameter_from_template_parameter(
                                      (e_game_engine_category)gui_screen_game_options_get_template_setting(this), definition->parameter)
                                : definition->parameter;
                            if (((c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)item))
                                    ->invoke_submenu(_game_options_string_spinner, definition->item_count, false))
                            {
                                c_gui_list_widget *submenu = (c_gui_list_widget *)item->get_child_widget(_gui_list_widget, _game_options_string_spinner);
                                if (submenu)
                                {
                                    const s_text_value_pair_definition *pair_definition = 0;
                                    s_text_value_pair pair;
                                    pair.flags = 0;
                                    short pair_index;
                                    if (::get_text_value_pair_value_from_variant_parameter_value(&m_original_variant, (e_game_variant_parameter)parameter,
                                        definition->parameter, &pair, &pair_index, &pair_definition)
                                        && pair_index != -1)
                                        submenu->set_focused_element_handle(pair_index, false);
                                }
                                m_editing_parameter = definition->parameter;
                                handled = true;
                            }
                        }
                    }
                }
            }
        }
        break;
    case _game_options_string_spinner:
        if (gui_screen_game_options_can_edit() && gui_screen_game_options_user_is_editing_setting(this))
        {
            c_gui_list_item_widget *focused_widget = gui_screen_game_options_get_focused_widget(this);
            c_gui_list_item_widget *focused_item = (focused_widget && focused_widget->m_type == 5) ? focused_widget : 0;
            c_gui_list_widget *parent_list =
                focused_item ? (c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)focused_item) : 0;
            c_gui_data *datasource = parent_list ? parent_list->get_data() : 0;
            long handle = focused_item ? focused_item->get_element_handle() : -1;
            if (datasource)
            {
                long template_setting = gui_screen_game_options_get_template_setting(this);
                long editing_parameter = m_editing_parameter;
                long parameter = template_setting != -1
                    ? get_player_trait_parameter_from_template_parameter((e_game_engine_category)template_setting, (e_game_variant_parameter)editing_parameter)
                    : editing_parameter;
                e_text_value_pair_parameter_type type = c_game_variant::get_expected_game_engine_setting_parameter_type((e_game_variant_parameter)parameter);
                switch (type)
                {
                case _text_value_pair_parameter_integer:
                {
                    long integer_value;
                    if (datasource->get_integer_value(handle, _game_options_string_value, &integer_value)
                        && m_original_variant.set_integer_game_engine_setting((e_game_variant_parameter)parameter, integer_value))
                        gui_screen_game_options_set_variant_author(this, message->m_controller);
                    break;
                }
                case _text_value_pair_parameter_string_id:
                {
                    long string_id_value;
                    if (datasource->get_string_id_value(handle, _game_options_string_value, &string_id_value)
                        && m_original_variant.set_string_id_game_engine_setting((e_game_variant_parameter)parameter, string_id_value))
                        gui_screen_game_options_set_variant_author(this, message->m_controller);
                    break;
                }
                }
            }
            if (focused_item)
                handled = focused_item->handle_widget_back_out();
        }
        gui_screen_game_options_finish_editing_setting(this);
        break;
    }
    if (!handled)
        handled = gui_screen_game_options_base_handle_list_item_chosen();
    return handled;
}


// @address 0x4d4430 ?update_save_as_new_operation@c_gui_screen_game_options@@QAAXXZ
void c_gui_screen_game_options::update_save_as_new_operation()
{
    bool finished = false;
    bool copy_edited = false;
    const wchar_t *verify_strings[4];
    s_saved_game_item_metadata_packed packed_metadata;
    s_file_reference progress_file;
    wchar_t variant_name[256];
    s_gui_selected_items_block content_metadata;

    if (m_keyboard_task && m_keyboard_task->get_controller_index() >= 0 && m_keyboard_task->get_controller_index() < 4)
    {
        if (!m_keyboard_task->busy())
        {
            if (m_keyboard_task->get_task_state() == _overlapped_task_state_success)
            {
                c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(m_keyboard_task->get_controller_index());
                ((c_static_wchar_string<256> *)variant_name)->set(m_keyboard_task->get_keyboard_results());
                content_catalogue_trim_whitespace((unsigned short *)variant_name);
                long engine_type = m_original_variant.engine_type;
                if (content_catalogue_has_storage_device(catalogue)
                    && content_catalogue_is_item_name_valid(catalogue, m_keyboard_task->get_controller_index(), variant_name,
                    game_engine_type_to_game_content_type((e_game_engine_type)engine_type)))
                {
                    wchar_t *name_buffer = (wchar_t *)m_variant_name;
                    ustrnzcpy(name_buffer, variant_name, 256);
                    if (network_squad_session_get_session_class() == _network_session_class_xbox_live)
                    {
                        verify_strings[0] = name_buffer;
                        e_language language = get_current_language();
                        c_string_verify_task *verify_task = &m_string_verify_task;
                        string_verify_task_set_strings(verify_task, language, verify_strings, 1);
                        overlapped_task_start_internal((c_overlapped_task *)verify_task,
                            "..\\interface\\gui_screens\\game_options\\gui_screen_game_options.cpp" /* __FILE__ */, 0x300);
                    }
                    else
                    {
                        finished = delete_variant_file(name_buffer);
                    }
                }
                else
                {
                    finished = true;
                }
            }
            else
            {
                finished = true;
            }
            m_keyboard_task->reset();
            m_keyboard_task->set_controller_index(_controller_none);
            m_keyboard_task = 0;
        }
    }
    else if (m_string_verify_task.m_result_count > 0)
    {
        c_overlapped_task *verify_task = &m_string_verify_task;
        if (!verify_task->busy())
        {
            if (m_string_verify_task.m_string_verified[0])
            {
                finished = delete_variant_file((const wchar_t *)m_variant_name);
            }
            else if (!(unsigned long)m_variant_progress.value)
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *message = memory
                    ? new (memory) c_load_dialog_screen_message(m_asynchronous_operation_controller, get_render_window(),
                        m_name, _game_options_string_file_share_confirm_delete_from_share, m_name)
                    : 0;
                if (message)
                {
                    message->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(message);
                    m_dialog_action = 1;
                    m_dialog_result = 0;
                }
                else
                {
                    finished = true;
                }
            }
            verify_task->reset();
        }
    }
    else if (m_dialog_action == 1)
    {
        switch (m_dialog_result)
        {
        case 1:
            finished = delete_variant_file(m_original_variant.get_name());
            m_dialog_action = -1;
            m_dialog_result = 0;
            break;
        case 2:
            m_dialog_action = -1;
            m_dialog_result = 0;
            finished = true;
            break;
        }
    }
    else if (m_create_request != -1)
    {
        if (m_variant_done.peek() == 1)
        {
            if (m_variant_success.peek() == 1)
            {
                if (m_variant_content_item.peek() != -1)
                {
                    copy_edited = true;
                    s_blffile_game_variant *blf = &m_variant_file;
                    setup_game_variant_for_write(blf, (const wchar_t *)m_variant_name, &m_original_variant, true);
                    long write_handle = saved_game_files_write_content_item(m_asynchronous_operation_controller, m_variant_content_item.peek(),
                        m_variant_file.get_metadata().file_type, blf, 0x3bc, &m_variant_success, &m_variant_done);
                    m_write_request = write_handle;
                    if (write_handle == -1)
                    {
                        g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
                        finished = true;
                    }
                }
                else
                {
                    g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
                    finished = true;
                }
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
            }
            m_create_request = -1;
        }
    }
    else if (m_write_request != -1)
    {
        if (m_variant_done.peek() == 1)
        {
            finished = true;
            if (m_variant_success.peek() == 1)
            {
                copy_edited = true;
                c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(m_asynchronous_operation_controller);
                if (content_catalogue_has_storage_device(catalogue)
                    &&((c_start_menu_game_editor *)catalogue)->load_progress_ui((e_controller_index)m_variant_content_item.peek(), &progress_file)
                    && content_catalogue_item_metadata_matches(catalogue, m_variant_content_item.peek(), &packed_metadata))
                {
                    gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&content_metadata),
                        (const s_saved_game_item_metadata_packed *)((const s_saved_game_item_metadata *)&packed_metadata));
                    c_gui_game_variant_selected_item selected_item(&content_metadata, static_cast<e_gui_selected_item_location>(1),
                        m_asynchronous_operation_controller, &progress_file, -1, -1, false, true);
                    g_pregame_setup_manager.set_selected_item(m_asynchronous_operation_controller,
                        (const c_gui_selected_item *)&selected_item, false, m_screen_index);
                }
                long content_item = m_variant_content_item.peek();
                if (content_item != -1)
                {
                    propagate_changes_from_newly_created_game_variant_file(m_asynchronous_operation_controller, content_item);
                }
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
            }
            m_write_request = -1;
        }
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
        finished = true;
    }

    if (finished)
    {
        m_operation = 0;
        m_dialog_action = -1;
        m_dialog_result = 0;
        if (copy_edited)
            game_variant_set_from(&m_edited_variant, &m_original_variant);
        if (m_variant_content_item.peek() != -1)
        {
            c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(m_asynchronous_operation_controller);
            content_catalogue_close_item(catalogue, m_variant_content_item.peek(), false);
            __lwsync();
            _InterlockedExchange((long *)&m_variant_content_item, -1);
        }
    }
}



// @address 0x4d4b18 ?update_rename_operation@c_gui_screen_game_options@@QAAXXZ
void c_gui_screen_game_options::update_rename_operation()
{
    bool finished = false;
    bool copy_edited = false;
    s_file_reference file;

    if (m_keyboard_task && m_keyboard_task->get_controller_index() >= 0 && m_keyboard_task->get_controller_index() < 4)
    {
        if (!m_keyboard_task->busy())
        {
            if (m_keyboard_task->get_task_state() == _overlapped_task_state_success)
            {
                ustrnzcpy_inline((wchar_t *)m_variant_name, m_keyboard_task->get_keyboard_results(), 256);
                finished = delete_variant_file((const wchar_t *)m_variant_name);
            }
            else
            {
                finished = true;
            }
            m_keyboard_task->reset();
            m_keyboard_task->set_controller_index(_controller_none);
            m_keyboard_task = 0;
        }
    }
    else if (m_create_request != -1)
    {
        if (m_variant_done.peek() == 1)
        {
            if (m_variant_success.peek() == 1)
            {
                if (m_variant_content_item.peek() != -1)
                {
                    copy_edited = true;
                    s_blffile_game_variant *blf = &m_variant_file;
                    setup_game_variant_for_write(blf, (const wchar_t *)m_variant_name, &m_original_variant, false);
                    long engine_type = m_original_variant.engine_type;
                    long write_handle = saved_game_files_write_content_item(m_asynchronous_operation_controller, m_variant_content_item.peek(),
                        (long)game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type), blf, 0x3bc, &m_variant_success, &m_variant_done);
                    m_write_request = write_handle;
                    if (write_handle == -1)
                    {
                        g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
                        finished = true;
                    }
                }
                else
                {
                    g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
                    finished = true;
                }
            }
            else
            {
                g_gui_error_manager.post_error_with_custom_message(0x20053, 0, 0, k_any_controller, false, true);
            }
            m_create_request = -1;
        }
    }
    else if (m_write_request != -1)
    {
        if (m_variant_done.peek() == 1)
        {
            finished = true;
            if (m_variant_success.peek() == 1)
            {
                copy_edited = true;
                long content_item_index = m_variant_content_item.peek();
                if (content_item_index != -1)
                    propagate_changes_from_newly_created_game_variant_file(m_asynchronous_operation_controller, content_item_index);
                if (m_saved_setup.settings.game_variant.delete_pending
                    && m_saved_setup.settings.game_variant.delete_type == 1)
                {
                    c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(m_asynchronous_operation_controller);
                    long content_item = content_catalogue_find_item_by_file_reference(catalogue,
                        file_reference_create_from_path(&file, m_saved_setup.settings.game_variant.path, false));
                    if (content_item != -1)
                        m_delete_request = saved_game_files_delete_content_item_async(
                            m_asynchronous_operation_controller, content_item, &m_variant_success, &m_variant_done);
                    if (m_delete_request != -1)
                        finished = false;
                }
            }
            m_write_request = -1;
        }
    }
    else if (m_delete_request != -1)
    {
        if (m_variant_done.peek() == 1)
        {
            finished = true;
            if (m_variant_success.peek() == 1)
                copy_edited = true;
            m_delete_request = -1;
        }
    }
    else
    {
        finished = true;
    }

    if (finished)
    {
        if (m_variant_content_item.peek() != -1)
        {
            c_content_catalogue *catalogue = gui_screen_game_options_get_catalogue(m_asynchronous_operation_controller);
            content_catalogue_close_item(catalogue, m_variant_content_item.peek(), false);
            __lwsync();
            _InterlockedExchange((long *)&m_variant_content_item, -1);
        }
        m_operation = 0;
        m_dialog_action = -1;
        m_dialog_result = 0;
        if (copy_edited)
            game_variant_set_from(&m_edited_variant, &m_original_variant);
    }
}

typedef char game_options_packed_author_offset[(offsetof(s_saved_game_item_metadata_packed, author_id) == 0xc0) ? 1 : -1];
typedef char game_options_record_author_offset[(offsetof(s_saved_game_item_metadata, author_id) == 0xc0) ? 1 : -1];
typedef char game_options_last_setup_type_offset[(0x20 + offsetof(s_gui_game_setup_storage::s_multiplayer_settings::s_game_variant_settings,
    delete_type) == 0x28c) ? 1 : -1];
typedef char game_options_pending_save_type_offset[(offsetof(s_gui_game_setup_storage::s_multiplayer_settings::s_game_variant_settings,
    delete_type) == 0x26c) ? 1 : -1];
