// interfacelibxbox/gui_screen_sandbox_menu.obj: from the records 0x4a0c88 0x4a69e8 0x4a69f0 0x4a6a00 0x4a6b78 0x4a6bb0 0x4a6c38 0x4a6e78 0x4a6e80 0x4a71c0 0x4a7780 0x4a77f8 0x4a79e8 0x4a7f48 0x4a8158 0x4a81a0 0x4a82c0 0x4a8380 0x4a8470 0x4a8518 0x4a8560 0x4a8578 0x4a8588 0x4a85a0 0x4a85d8 0x4a8640 0x4a8650 0x4a86c0 0x4a86e0 0x4a8760 0x4a8778 0x4a8790 0x4a87a8 0x4a87c0 0x4a87d8 0x4a87f0 0x4a8808 0x4a8820 0x4b5a78
#include <stdio.h>
#include <math.h>
#include "objects/object_header.h"
#include "game/player_mapping_types.h"
// @address 0xeded8 floor
// @address 0x187780 _vsnwprintf_s
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "tag_files/tag_groups.h"
#include <stddef.h>
#include "game/game_globals_storage.h"
#include "memory/static_arrays.h"
#include "shell/shell.h"
#include "game/map_variant.h"
#include "interface/gui_widget.h"
#include "interface/gui_widget_types.h"
#include "tag_files/tag_types.h"
#include <string.h>
#include <stdarg.h>
#include "game/game.h"
#include "game/game_engine.h"
#include "game/game_engine_scoring_globals.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "simulation/simulation.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_sandbox_dependencies.h"
#include "game/game_engine_util.h"
#include "scenario/scenario_map_variant.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_window_manager.h"
#include "game/game_engine_default.h"
#include "interface/user_interface_shared_globals.h"
#include "cache/cache_files.h"
#include "interface/gui_screens/dialog/gui_screen_dialog.h"
#include "scenario/scenario.h"
#include "interface/user_interface_utilities.h"
class c_gui_data;
#include "interface/c_gui_list_widget.h"
#include "scenario/scenario_tag.h"
#include "interface/c_gui_list_item_widget.h"
class c_gui_data;
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screen_widget_legacy.h"

#include "text/text_group.h"

#include "interface/load_object_property_panel_screen_message.h"
#include "interface/gui_screens/sandbox_editor/gui_screen_object_properties_menu.h"
#ifdef HALO3_64BIT
#include "objects/object_datum.h"
#endif


struct s_sandbox_placeable_object_block;

#include "interface/gui_screens/sandbox_menu/gui_screen_sandbox_menu.h"

// @address 0x509a8 ??_7c_gui_placeable_object_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_placeable_object_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_placeable_object_datasource@@UAAPAXI@Z

#include "interface/gui_data.h"
#include "interface/user_interface_data.h"
#include "interface/gui_screen_widget_text.h"
#include "cseries/placement_new_fwd.h"
#include <new>
#include "interface/controller_inline.h"

#ifdef HALO3_64BIT
typedef object_datum s_sandbox_object;
#else
struct s_sandbox_object
{
    char state[24];
    short variant_index;
    char state1a[0x154 - 26];
    unsigned short multiplayer_properties_offset;
};
#endif

#include "interface/c_gui_widget.h"
#include "scenario/scenario_light_types.h"

struct s_sandbox_placeable_object_block
{
    long count;
    TAG_POINTER(scenario_object_palette_entry) address;
};

// match: 0x4a81a0: the Boolean helper preserves the explicit result materialization;
// direct canonical multiplayer_object.count() testing shrinks this body by 20 bytes.
#include "objects/object_definitions.h"
struct s_sandbox_object_filter_definition : s_object_definition
{
    __forceinline bool has_game_engine_masks() const { return multiplayer_object.count() > 0; }
};

namespace
{
    __forceinline bool initialize_datasource(long &datasource_name, long name)
    {
        datasource_name = name;
        return true;
    }
}


static __forceinline void gui_screen_sandbox_menu_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    string->m_string[0] = 0;
    if (string_id != -1)
    {
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals)
        {
            long string_list_index = globals->global_strings.index;
            if (string_list_index != -1)
                text_group_get_string(string_list_index, string_id, string);
        }
    }
}

// @address 0x4a0c88 gui_screen_sandbox_menu_find_element_by_name
extern "C" long gui_screen_sandbox_menu_find_element_by_name(c_gui_data *data, long, long name)
{
    long result = -1;
    for (long element_handle = data->get_first_element_handle(); element_handle != -1;
         element_handle = data->get_next_element_handle(element_handle))
    {
        long element_name = -1;
        if (data->get_string_id_value(element_handle, 668, &element_name) && element_name == name)
        {
            result = element_handle;
            break;
        }
    }
    return result;
}

long c_gui_placeable_object_datasource::get_current_item_count_internal()
{
    return number_of_placeable_object_types;
}

// @address 0x4a69f0 ?get_current_item_count_internal@c_gui_budget_range_datasource@@MAAJXZ
long c_gui_budget_range_datasource::get_current_item_count_internal()
{
    return m_maximum - m_minimum;
}



// @address 0x4a6a00 ?get_text_value@c_sandbox_budget_items_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_sandbox_budget_items_datasource::get_text_value(
    long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    *value->m_string = 0;
    if (element_handle == -1 || value_name != _string_id_value)
    {
        return false;
    }

    long string_id = -1;
    if (!m_base_datasource->get_string_id_value(element_handle, _string_id_name, &string_id))
    {
        return false;
    }

    long category = m_screen->m_selected_category;
    if (category == -1)
    {
        return false;
    }

    s_game_engine_globals *globals = tls_game_engine_globals();
    const s_variant_quota *quota = &globals->map_info.quotas[category];

    switch (string_id)
    {
    case _sandbox_menu_string_budget_total_price:
    {
        gui_screen_sandbox_menu_global_string_get(0x4a0, value);
        value->append_print(L"%d", (long)((float)quota->maximum_count * quota->price));
        return false;
    }
    case _sandbox_menu_string_budget_placed:
        value->print(L"%d", quota->placed_on_map);
        return false;
    case _sandbox_menu_string_budget_maximum:
        value->print(L"%d", quota->maximum_count);
        return false;
    case _sandbox_menu_string_budget_minumum:
        value->print(L"%d", quota->minimum_count);
        return false;
    case _sandbox_menu_string_budget_total:
        value->print(L"%d", quota->maximum_allowed);
        return false;
    default:
        return false;
    }
    return false;
}

// @address 0x4a6b78 ?get_string_id_value@c_sandbox_budget_items_datasource@@UAA_NJJPAJ@Z
bool c_sandbox_budget_items_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    if (value_name == _string_id_item)
    {
        *value = m_value;
        return true;
    }
    return m_base_datasource->get_string_id_value(element_handle, value_name, value);
}

// 0x4a6bb0: focuses the screen's budget list on its current selection.

extern "C" {
// @address 0x72fdf4 g_sandbox_menu_focused_handle
long g_sandbox_menu_focused_handle = -1;
}

static __forceinline c_gui_list_widget *gui_screen_sandbox_menu_get_child_list_widget(c_gui_screen_widget *screen, long name)
{
    return (c_gui_list_widget *)((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, name);
}

// @address 0x4a6bb0 ?post_initialize@c_sandbox_budget_summary_screen_widget@@UAAXXZ
void c_sandbox_budget_summary_screen_widget::post_initialize()
{
    c_gui_list_widget *list_widget = gui_screen_sandbox_menu_get_child_list_widget(this, _sandbox_menu_string_create_category_list);
    if (list_widget)
    {
        c_gui_data *data = ((c_gui_data *)(list_widget)->get_data());
        long element_handle = data ? data->get_first_element_handle() : -1;
        (this)->transfer_focus_to_list(list_widget, element_handle, true, false);
    }
}

static __forceinline s_sandbox_object *sandbox_object_get(long index)
{
    return (s_sandbox_object *)object_get(index);
}

#ifdef HALO3_64BIT
static __forceinline bool sandbox_object_has_multiplayer_properties(const s_sandbox_object *object)
{
    return object->multiplayer_data_offset != -1;
}
#endif

static __forceinline e_controller_index gui_screen_sandbox_menu_message_controller(const c_controller_input_message *message) { return message->m_controller; }
static __forceinline e_window_index gui_screen_sandbox_menu_message_window(const c_controller_input_message *message) { return message->m_window; }

static __forceinline c_message *gui_screen_sandbox_menu_creation_message(const c_controller_input_message *message, e_window_index window, bool secondary)
{
    void *memory = user_interface_malloc_tracked(sizeof(c_load_object_creation_screen_message), 0, 0);
    return memory ? new (memory) c_load_object_creation_screen_message(
                        gui_screen_sandbox_menu_message_controller(message), window, _string_id_top_most, _sandbox_menu_string_category_weapons, secondary)
                  : 0;
}

static __forceinline c_message *gui_screen_sandbox_menu_property_message(
    const c_controller_input_message *message, e_window_index window, const s_sandbox_object *object, bool secondary)
{
#ifdef HALO3_64BIT
    void *memory = user_interface_malloc_tracked(sizeof(c_load_object_property_panel_screen_message), 0, 0);
    const short variant_index = object->map_variant_index;
#else
    void *memory = user_interface_malloc_tracked(64, 0, 0);
    const short variant_index = object->variant_index;
#endif
    return memory ? new (memory) c_load_object_property_panel_screen_message(
                        gui_screen_sandbox_menu_message_controller(message), window, _string_id_top_most, variant_index, secondary)
                  : 0;
}

static __forceinline bool gui_screen_sandbox_menu_placement_flag(unsigned short flags)
{
    return ((flags >> 7) & 1) != 0;
}

// @address 0x4a6e78 ?handle_controller_input_message@c_sandbox_budget_summary_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_sandbox_budget_summary_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    return c_gui_screen_widget::handle_controller_input_message(message);
}


static __forceinline c_gui_data *gui_screen_sandbox_menu_find_data(c_gui_screen_widget *screen, long name);
static __forceinline c_gui_budget_range_datasource *gui_screen_sandbox_menu_range(c_gui_screen_widget *screen, long name)
{
    return (c_gui_budget_range_datasource *)gui_screen_sandbox_menu_find_data(screen, name);
}
static __forceinline void gui_screen_sandbox_menu_adjust_range(
    c_sandbox_budget_summary_screen_widget *screen, long property, const s_variant_quota *quota, bool *valid)
{
    switch (property)
    {
    case _sandbox_menu_string_budget_minumum:
    {
        c_gui_budget_range_datasource *range = gui_screen_sandbox_menu_range(screen, _sandbox_menu_string_budget_minumum);
        if (range)
        {
            long maximum = quota->placed_on_map + 1;
            range->m_minimum = 0;
            range->m_maximum = maximum;
        }
    }
    break;
    case _sandbox_menu_string_budget_maximum:
    {
        c_gui_budget_range_datasource *range = gui_screen_sandbox_menu_range(screen, _sandbox_menu_string_budget_maximum);
        if (range)
        {
            long maximum;
            if (quota->price == 0.0f)
                maximum = quota->maximum_allowed;
            else
            {
                float count = (float)floor((double)(
                    (tls_game_engine_globals()->map_info.maximum_budget - tls_game_engine_globals()->map_info.current_budget) /
                    quota->price));
                maximum = quota->maximum_count + (long)count;
                if (maximum > quota->maximum_allowed)
                    maximum = quota->maximum_allowed;
            }
            if (maximum == 0)
                *valid = false;
            else if (quota->placed_on_map > quota->maximum_allowed)
            {
                long minimum = quota->maximum_count;
                range->m_minimum = minimum;
                range->m_maximum = minimum + 1;
            }
            else
            {
                long minimum = quota->placed_on_map;
                long limit = maximum + 1;
                range->m_minimum = minimum;
                range->m_maximum = limit;
            }
        }
    }
    break;
    }
}


// @address 0x4a7780 ?post_initialize@c_sandbox_object_menu_screen_widget@@UAAXXZ
void c_sandbox_object_menu_screen_widget::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    gui_screen_widget_set_display_group(this, 0, m_category);
    long index = g_sandbox_menu_focused_handle;
    if (index != -1)
    {
        c_gui_list_widget *list_widget = gui_screen_sandbox_menu_get_child_list_widget(this, m_category);
        if (list_widget != 0)
        {
            list_widget->set_focused_item_index(index, true);
        }
    }
}

// @address 0x8c4950 g_gui_last_dialog_name

static __forceinline c_gui_data *gui_screen_sandbox_menu_find_data(c_gui_screen_widget *screen, long name)
{
    c_gui_data *result = 0;
    for (long i = 0; i < screen->m_datasource_count; ++i)
    {
        if (screen->m_datasource[i]->m_name == name)
        {
            result = screen->m_datasource[i];
            break;
        }
    }
    return result;
}

// @address 0x4a77f8 ?initialize_datasource@c_sandbox_object_menu_screen_widget@@UAAXXZ
void c_sandbox_object_menu_screen_widget::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    c_gui_data *categories = (c_gui_data *)gui_screen_sandbox_menu_find_data(this, _sandbox_menu_string_categories_datasource);
    for (long handle = categories->get_first_element_handle(); handle != -1; handle = categories->get_next_element_handle(handle))
    {
        long category;
        categories->get_string_id_value(handle, 668, &category);
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_placeable_object_datasource), 0, 0);
        c_gui_placeable_object_datasource *source = memory ? new (memory) c_gui_placeable_object_datasource(category)
            : 0;
        if (source)
        {
            if (((c_gui_data *)source)->initialize(category))
                m_datasource[m_datasource_count++] = (c_gui_data *)source;
            else
            {
                source->~c_gui_placeable_object_datasource();
                user_interface_free(source);
            }
        }
    }
    if (g_gui_last_dialog_name)
    {
        long selected = gui_screen_sandbox_menu_find_element_by_name(categories, 668, g_gui_last_dialog_name);
        if (selected != -1 && categories->get_string_id_value(selected, 668, &m_category))
        {
            c_gui_list_widget *list = gui_screen_sandbox_menu_get_child_list_widget(this, 254);
            if (list)
                (list)->set_focused_element_handle(selected, false);
        }
    }
}

// @address 0x50ab0 ??_7c_sandbox_budget_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_sandbox_budget_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_sandbox_budget_items_datasource@@UAAPAXI@Z
// @address 0x50a18 ??_7c_gui_budget_range_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_budget_range_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_budget_range_datasource@@UAAPAXI@Z

// @address 0x4a6c38 ?initialize_datasource@c_sandbox_budget_summary_screen_widget@@UAAXXZ
void c_sandbox_budget_summary_screen_widget::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    c_gui_data *source = gui_screen_sandbox_menu_find_data(this, _sandbox_menu_string_budget_datasource);
    if (source)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_sandbox_budget_items_datasource), 0, 0);
        c_sandbox_budget_items_datasource *items = memory ? new (memory) c_sandbox_budget_items_datasource(source, this, m_budget_item_string_id)
            : 0;
        if (items)
        {
            if (items->initialize())
                m_datasource[m_datasource_count++] = (c_gui_data *)items;
            else
            {
                items->~c_sandbox_budget_items_datasource();
                user_interface_free(items);
            }
        }
        long category = m_selected_category;
        if (category != -1)
        {
            for (long handle = source->get_first_element_handle(); handle != -1; handle = source->get_next_element_handle(handle))
            {
                long budget_index = -1;
                long object_definition = -1;
                source->get_string_id_value(handle, 668, &budget_index);
                source->get_string_id_value(handle, 266, &object_definition);
                if (object_definition != 0 && object_definition != -1)
                {
                    void *range_memory = user_interface_malloc_tracked(sizeof(c_gui_budget_range_datasource), 0, 0);
                    c_gui_budget_range_datasource *range = range_memory ? new (range_memory) c_gui_budget_range_datasource()
                        : 0;
                    range->initialize(budget_index, 0, 0);
                    m_datasource[m_datasource_count++] = (c_gui_data *)range;
                }
            }
        }
    }
}

// @address 0x50a9c ??_7c_load_budget_screen_message@@6B@

// @address 0x4a79e8 ?handle_controller_input_message@c_sandbox_object_menu_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_sandbox_object_menu_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    long component = message->m_component;
    if (message->m_event_type == 13)
    {
        if (component == 2)
        {
            c_gui_list_widget *list = gui_screen_sandbox_menu_get_child_list_widget(this, m_category);
            if (list)
            {
                c_gui_data *data = (list)->get_data();
                if (data && data->get_current_item_count() > 0)
                {
                    c_gui_placeable_object_datasource::s_sandbox_placeable_object object;
                    if (data->get_element((list)->get_focused_element_handle(), &object, sizeof(object)))
                    {
                        long quota_index = map_variant_find_quota_index(&tls_game_engine_globals()->map_info, object.object_definition_index);
                        if (quota_index != -1)
                        {
                            bool secondary = false;
                            long horizontal = 0, vertical = 0;
                            if (message->m_controller != -1)
                            {
                                const c_controller_interface *controller = controller_get(message->m_controller);
                                if (controller)
                                {
                                    user_interface_get_number_of_render_windows(controller->user_index, &horizontal, &vertical);
                                    secondary = horizontal > 1 || vertical > 1;
                                }
                            }
                            e_window_index window = message->m_window;
                            void *memory = user_interface_malloc_tracked(sizeof(c_load_budget_screen_message), 0, 0);
                            c_load_budget_screen_message *load = memory
                                ? new (memory) c_load_budget_screen_message(
                                      message->m_controller, window, quota_index, m_name, object.name, secondary)
                                : 0;
                            if (load)
                            {
                                load->set_parent_screen_index(m_screen_index);
                                ::user_interface_messaging_post(load);
                            }
                        }
                    }
                }
            }
            handled = true;
        }
        else if (component == 4 || component == 5)
        {
            c_gui_data *categories = gui_screen_sandbox_menu_find_data(this, _sandbox_menu_string_categories_datasource);
            if (categories)
            {
                long original = m_category;
                long handle = gui_screen_sandbox_menu_find_element_by_name((c_gui_data *)categories, 668, original);
                if (component == 4)
                {
                    long next = categories->get_next_element_handle(handle);
                    handle = next != -1 ? next : categories->get_first_element_handle();
                }
                else
                {
                    long previous = categories->get_previous_element_handle(handle);
                    if (previous != -1)
                        handle = previous;
                    else
                    {
                        for (long next = categories->get_next_element_handle(handle); next != -1; next = categories->get_next_element_handle(handle))
                            handle = next;
                    }
                }
                if (categories->get_string_id_value(handle, 668, &m_category))
                {
                    c_gui_list_widget *list = gui_screen_sandbox_menu_get_child_list_widget(this, 254);
                    if (list)
                        (list)->set_focused_element_handle(handle, false);
                }
                else
                    m_category = original;
            }
            handled = true;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}
// @address 0x4a7f48 ?handle_list_item_chosen@c_sandbox_object_menu_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_sandbox_object_menu_screen_widget::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    c_gui_placeable_object_datasource::s_sandbox_placeable_object object;
    data->get_element(item->get_element_handle(), &object, sizeof(object));
    s_game_engine_globals *globals = tls_game_engine_globals();
    c_map_variant *map = &globals->map_info;
    long quota_index = map_variant_find_quota_index(map, object.object_definition_index);
    const s_variant_quota *quota = &globals->map_info.quotas[quota_index];
    if (map->current_budget + object.weight <= map->maximum_budget
        || (quota->maximum_count > quota->placed_on_map && quota->maximum_allowed > quota->placed_on_map))
    {
        const c_controller_interface *controller = controller_get(message->m_controller);
        if (controller)
        {
            long user_index = controller->user_index;
            if (user_index != -1)
            {
                long player_index = tls_player_mapping_globals()->player_user_mapping[user_index];
                if (player_index != -1)
                {
                    game_engine_sandbox_editor_action action;
                    {
                        unsigned __int64 *p = (unsigned __int64 *)&action;
                        unsigned int count = 8;
                        do { *p++ = 0; } while (--count);
                    }
                    action.action = 0;
                    action.definition_index = object.object_definition_index;
                    action.player_index = player_index & 0xffff;
                    s_sandbox_globals *sandbox = &globals->sandbox;
                    action.position = sandbox->edit_position[action.player_index];
                    simulation_queue_editor_action_insert(&action);
                }
            }
            handled = true;
            g_gui_last_dialog_name = m_category;
            g_sandbox_menu_focused_handle = item->get_element_handle();
            transition_out_with_transition_type(0, _screen_transition_normal);
        }
    }
    return handled;
}

bool c_gui_placeable_object_datasource::get_element(long element_handle, void *element, long element_size)
{
    memcpy(element, &m_placeable_object_types[element_handle], sizeof(s_sandbox_placeable_object));
    return true;
}


// @address 0x4a81a0 ?gui_placeable_object_datasource_update_from_block@@YAXPAVc_gui_placeable_object_datasource@@PAUs_sandbox_placeable_object_block@@@Z
void gui_placeable_object_datasource_update_from_block(c_gui_placeable_object_datasource *datasource, s_sandbox_placeable_object_block *block)
{
    for (short index = 0; index < block->count; index++)
    {
        scenario_object_palette_entry *element = &block->address[index];
        c_gui_placeable_object_datasource::s_sandbox_placeable_object object;
        object.name = element->name;
        object.object_definition_index = element->object.index;
        object.weight = element->price;
        s_sandbox_object_filter_definition *definition =
            TAG_GET('obje', s_sandbox_object_filter_definition, object.object_definition_index);
        bool valid = true;
        if (definition->has_game_engine_masks())
        {
            unsigned short mask = definition->multiplayer_object[0].game_engine_flags.m_storage;
            if ((mask & 0x3ff) != 0)
            {
                long game_engine_type = tls_game_engine_globals()->map_info.m_game_engine_subtype;
                if (mask != 0x3ff)
                {
                    if (game_engine_type == 10 || ((1 << game_engine_type) & mask) == 0)
                    {
                        valid = false;
                    }
                }
            }
        }
        if (valid)
        {
            datasource->m_placeable_object_types[datasource->number_of_placeable_object_types++] = object;
        }
    }
}



// @address 0x4a82c0 ?initialize@c_gui_placeable_object_datasource@@UAA_NJ@Z
bool c_gui_placeable_object_datasource::initialize(long context)
{
    this->m_name = context;
    scenario *table = global_scenario;
    if (table)
    {
        this->number_of_placeable_object_types = 0;
        s_tag_block *block;
        switch (this->m_block_type)
        {
        case _sandbox_menu_string_category_vehicles:
            block = &table->sandbox_vehicles;
            break;
        case _sandbox_menu_string_category_weapons:
            block = &table->sandbox_weapons;
            break;
        case _sandbox_menu_string_category_equipment:
            block = &table->sandbox_equipment;
            break;
        case _sandbox_menu_string_category_scenery:
            block = &table->sandbox_scenery;
            break;
        case _sandbox_menu_string_category_teleporters:
            block = &table->sandbox_machines;
            break;
        case _sandbox_menu_string_category_spawners:
            block = &table->sandbox_sound_scenery;
            break;
        case _sandbox_menu_string_category_goals:
            block = &table->sandbox_crates;
            break;
        default:
            return true;
        }
        gui_placeable_object_datasource_update_from_block(this, (s_sandbox_placeable_object_block *)block);
    }
    return true;
}



// @address 0x4a8380 ?get_integer_value@c_gui_placeable_object_datasource@@UAA_NJJPAJ@Z
bool c_gui_placeable_object_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < number_of_placeable_object_types)
    {
        if (value_name == _sandbox_menu_string_items_left)
        {
            s_game_engine_globals *globals = tls_game_engine_globals();
            long quota_index = map_variant_find_quota_index(&globals->map_info, m_placeable_object_types[element_handle].object_definition_index);
            if (quota_index != -1)
            {
                const s_variant_quota *quota = &globals->map_info.quotas[quota_index];
                if (quota)
                {
                    *value = quota->maximum_allowed - quota->placed_on_map;
                    result = true;
                }
            }
            else
            {
                *value = -1;
                result = true;
            }
        }
    }
    return result;
}

// @address 0x4a8470 ?get_text_value@c_gui_placeable_object_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_placeable_object_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < number_of_placeable_object_types && value_name == _sandbox_menu_string_item_cost)
    {
        gui_screen_sandbox_menu_global_string_get(0x4a0, value);
        value->append_print(L"%d", (long)m_placeable_object_types[element_handle].weight);
        result = true;
    }
    return result;
}

bool c_gui_placeable_object_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (value_name == _string_id_name && element_handle >= 0 && element_handle < number_of_placeable_object_types)
    {
        *value = m_placeable_object_types[element_handle].name;
        result = true;
    }
    return result;
}

// @address 0x4a8560 ?initialize@c_gui_budget_range_datasource@@UAA_NJJJ@Z
bool c_gui_budget_range_datasource::initialize(long name, long minimum, long maximum)
{
    bool result = initialize_datasource(m_name, name);
    m_maximum = maximum;
    m_minimum = minimum;
    return result;
}

// @address 0x4a8578 ?get_column_names@c_gui_budget_range_datasource@@UAAXQAJPAJ@Z
void c_gui_budget_range_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
}

// @address 0x4a8578 ?get_column_names@c_gui_placeable_object_datasource@@UAAXQAJPAJ@Z
void c_gui_placeable_object_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
}

// @address 0x4a8588 ?get_element@c_gui_budget_range_datasource@@UAA_NJPAXJ@Z
bool c_gui_budget_range_datasource::get_element(long element_handle, void *element, long element_size)
{
    *static_cast<long *>(element) = m_minimum + element_handle;
    return true;
}

// @address 0x4a85a0 ?get_integer_value@c_gui_budget_range_datasource@@UAA_NJJPAJ@Z
bool c_gui_budget_range_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_maximum - m_minimum)
    {
        *value = element_handle + m_minimum;
        result = true;
    }
    return result;
}

// @address 0x4a85d8 ??0c_load_object_creation_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJ_N@Z
c_load_object_creation_screen_message::c_load_object_creation_screen_message(
    e_controller_index controller, e_window_index window, long layered_position, long category, bool is_secondary)
    : c_load_screen_message(is_secondary ? _sandbox_menu_string_sandbox_objects_menu_splitscreen
            : _sandbox_menu_string_sandbox_objects_menu,
        controller, window,
        // match: 0x4a85d8: value range assumption guides branch lowering
        (__assume(layered_position == _string_id_top_most), layered_position))
{
    // match: 0x4a85d8: value range assumption guides branch lowering
    m_category = (__assume(category == _sandbox_menu_string_category_weapons), category);
}

// @address 0x4a8640 ?apply_initial_state@c_load_object_creation_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_object_creation_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    static_cast<c_sandbox_object_menu_screen_widget *>(screen_widget)->set_category(m_category);
}

// @address 0x4a8650 ??0c_load_object_property_panel_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJ_N@Z
c_load_object_property_panel_screen_message::c_load_object_property_panel_screen_message(
    e_controller_index controller, e_window_index window, long layered_position, long object_index, bool is_secondary)
    : c_load_screen_message(is_secondary ? _sandbox_menu_string_sandbox_object_properties_panel_splitscreen
            : _sandbox_menu_string_sandbox_object_properties_panel,
        controller, window,
        // match: 0x4a8650: value range assumption guides branch lowering
        (__assume(layered_position == _string_id_top_most), layered_position))
{
    m_object_index = object_index;
}

// @address 0x4a86e0 ?append_print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ
template<long k_maximum_count> const wchar_t *c_static_wchar_string<k_maximum_count>::append_print(const wchar_t *format, ...)
{
    long length = this->length();
    long size = k_maximum_count - length;
    wchar_t *end = m_string + length;
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(end, size - 1, (unsigned int)-1, format, args);
    end[size - 1] = 0;
    return m_string;
}

template const wchar_t *c_static_wchar_string<1024>::append_print(const wchar_t *format, ...);

// @address 0x4a86c0 ?apply_initial_state@c_load_budget_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_budget_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_sandbox_budget_summary_screen_widget *summary = static_cast<c_sandbox_budget_summary_screen_widget *>(screen);
    summary->m_selected_category = m_budget_index;
    summary->m_invoked_from = m_invoked_from;
    summary->m_budget_item_string_id = m_budget_item_string_id;
}

// @address 0x4b92c0 ?get_column_names@c_sandbox_budget_items_datasource@@UAAXQAJPAJ@Z
void c_sandbox_budget_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4a8760 ?set_element@c_gui_data_proxy@@UAAXJPBXJ@Z
void c_gui_data_proxy::set_element(long element_handle, const void *element, long element_size)
{
    m_base_datasource->set_element(element_handle, element, element_size);
}

// @address 0x4a8778 ?get_maximum_item_count@c_gui_data_proxy@@UAAJXZ
long c_gui_data_proxy::get_maximum_item_count()
{
    return m_base_datasource->get_maximum_item_count();
}

// @address 0x4a8790 ?get_integer_value@c_gui_data_proxy@@UAA_NJJPAJ@Z
bool c_gui_data_proxy::get_integer_value(long element_handle, long value_name, long *value)
{
    return m_base_datasource->get_integer_value(element_handle, value_name, value);
}

// @address 0x4a87a8 ?get_element@c_gui_data_proxy@@UAA_NJPAXJ@Z
bool c_gui_data_proxy::get_element(long element_handle, void *element, long element_size)
{
    return m_base_datasource->get_element(element_handle, element, element_size);
}

// @address 0x4a87c0 ?get_previous_element_handle@c_gui_data_proxy@@UAAJJ@Z
long c_gui_data_proxy::get_previous_element_handle(long element_handle)
{
    return m_base_datasource->get_previous_element_handle(element_handle);
}

// @address 0x4a87d8 ?get_next_element_handle@c_gui_data_proxy@@UAAJJ@Z
long c_gui_data_proxy::get_next_element_handle(long element_handle)
{
    return m_base_datasource->get_next_element_handle(element_handle);
}

// @address 0x4a87f0 ?get_first_element_handle@c_gui_data_proxy@@UAAJXZ
long c_gui_data_proxy::get_first_element_handle()
{
    return m_base_datasource->get_first_element_handle();
}

// @address 0x4a8808 ?get_current_item_count@c_gui_data_proxy@@UAAJXZ
long c_gui_data_proxy::get_current_item_count()
{
    return m_base_datasource->get_current_item_count();
}

// @address 0x4a8820 ?get_text_value@c_gui_data_proxy@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_data_proxy::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    return m_base_datasource->get_text_value(element_handle, value_name, value);
}

// @address 0x4b5a78 ?apply_initial_state@c_load_object_property_panel_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_object_property_panel_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    static_cast<c_sandbox_object_properties_screen_widget *>(screen)->map_variant_index = m_object_index;
}

static __forceinline bool gui_screen_sandbox_menu_secondary_user(long user)
{
    long horizontal = 0;
    long vertical = 0;
    user_interface_get_number_of_render_windows(user, &horizontal, &vertical);
    return horizontal > 1 || vertical > 1;
}


static __forceinline long gui_screen_sandbox_menu_player_from_user(long user)
{
    long player = -1;
    if (user != -1)
        player = tls_player_mapping_globals()->player_user_mapping[user];
    return player;
}
// @address 0x4a6e80 ?handle_global_messages@c_sandbox_budget_summary_screen_widget@@SA_NPBVc_controller_input_message@@@Z
bool c_sandbox_budget_summary_screen_widget::handle_global_messages(const c_controller_input_message *message)
{
    bool handled = false;
    if (!g_window_manager.get_responding_screen_in_window(gui_screen_sandbox_menu_message_controller(message),
        gui_screen_sandbox_menu_message_window(message)) &&
        simulation_starting_up() && tls_game_globals()->options.game_mode == 2 && !game_is_cooperative() &&
        game_engine_in_round() && message->m_event_type == 13 &&
        (message->m_component == 1 || message->m_component == 2))
    {
        e_window_index window = gui_screen_sandbox_menu_message_window(message);
        const c_game_engine *engine = current_game_engine();
        if (engine && engine->get_type() == 5)
        {
            const c_controller_interface *controller = controller_get(gui_screen_sandbox_menu_message_controller(message));
            if (controller)
            {
                short user = controller->user_index;
                {
                long player = gui_screen_sandbox_menu_player_from_user(user);
                if (player != -1)
                {
                    bool secondary = gui_screen_sandbox_menu_secondary_user(user);
                    long object_index = -1;
                    long editor_index = -1;
                    if (game_engine_is_player_in_edit_mode(player, &object_index, &editor_index))
                    {
                        if (message->m_component == 2)
                        {
                            c_message *load = 0;
#ifdef HALO3_64BIT
                            if (object_index != -1 && sandbox_object_has_multiplayer_properties(sandbox_object_get(object_index)))
#else
                            if (object_index != -1 && sandbox_object_get(object_index)->multiplayer_properties_offset != 0xffff)
#endif
                            {
                                s_sandbox_object *object = sandbox_object_get(object_index);
                                load = gui_screen_sandbox_menu_property_message(message, window, object, secondary);
                            }
                            else if (editor_index != -1)
                            {
                                s_sandbox_object *editor = sandbox_object_get(editor_index);
                                bool force_properties = false;
#ifdef HALO3_64BIT
                                const short variant_index = editor->map_variant_index;
#else
                                const short variant_index = editor->variant_index;
#endif
                                if (variant_index != -1)
                                {
                                    s_variant_object_datum placement;
                                    tls_game_engine_globals()->map_info.get_variant_object_datum(variant_index, &placement);
                                    if (placement.helper_object_index == editor_index)
                                    {
                                        if (gui_screen_sandbox_menu_placement_flag(placement.flags))
                                            force_properties = true;
                                        if (placement.object_index != -1)
                                            editor_index = placement.object_index;
                                    }
                                    if ((!::game_engine_is_object_being_edited(editor_index, (long *)(0)) &&
#ifdef HALO3_64BIT
                                            sandbox_object_has_multiplayer_properties(sandbox_object_get(editor_index))) ||
#else
                                            sandbox_object_get(editor_index)->multiplayer_properties_offset != 0xffff) ||
#endif
                                        force_properties)
                                        load = gui_screen_sandbox_menu_property_message(message, window, editor, secondary);
                                    else
                                        load = gui_screen_sandbox_menu_creation_message(message, window, secondary);
                                }
                            }
                            else
                                load = gui_screen_sandbox_menu_creation_message(message, window, secondary);
                            if (load)
                                ::user_interface_messaging_post(load);
                        }
                        handled = true;
                    }
                }
                }
            }
        }
    }
    return handled;
}

static __forceinline void gui_screen_sandbox_menu_get_budget_focus(
    c_sandbox_budget_summary_screen_widget *screen, long property, const s_variant_quota *quota, long *element)
{
    switch (property)
    {
    case _sandbox_menu_string_budget_minumum:
    {
        c_gui_budget_range_datasource *range = gui_screen_sandbox_menu_range(screen, _sandbox_menu_string_budget_minumum);
        if (range)
            *element = quota->minimum_count - range->m_minimum;
    } break;
    case _sandbox_menu_string_budget_maximum:
    {
        c_gui_budget_range_datasource *range = gui_screen_sandbox_menu_range(screen, _sandbox_menu_string_budget_maximum);
        if (range)
            *element = quota->maximum_count - range->m_minimum;
    } break;
    }
}

static __forceinline long gui_screen_sandbox_menu_controller_player(const c_controller_interface *input)
{
    long result;
    long user_index;
    if (input && (user_index = input->user_index) != -1)
        result = tls_player_mapping_globals()->player_user_mapping[user_index];
    else
        result = -1;
    return result;
}

// @address 0x4a71c0 ?handle_list_item_chosen@c_sandbox_budget_summary_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_sandbox_budget_summary_screen_widget::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    c_gui_data *view = data;
    switch (list_name)
    {
    case _sandbox_menu_string_create_category_list:
    {
        long element_value;
        long unused_value;
        if (view->get_string_id_value(item->get_list_item_index(), _string_id_name, &element_value) &&
            view->get_string_id_value(item->get_list_item_index(), _string_id_value, &unused_value))
        {
            long category = m_selected_category;
            if (category != -1)
            {
                const s_variant_quota *quota = &tls_game_engine_globals()->map_info.quotas[category];
                bool valid = true;
                gui_screen_sandbox_menu_adjust_range(this, element_value, quota, &valid);
                if (quota->placed_on_map != 0 && valid)
                {
                    const long submenu_name = _sandbox_menu_string_common_spinner;
                    ((c_gui_list_widget *)gui_widget_get_parent_list_widget((c_gui_widget *)item))->invoke_submenu(submenu_name, element_value, false);
                    c_gui_list_widget *child = (c_gui_list_widget *)this->get_child_widget(_gui_list_widget, submenu_name);
                    if (child)
                    {
                        if (m_selected_category != -1)
                        {
                            quota = &tls_game_engine_globals()->map_info.quotas[m_selected_category];
                            long focus_element;
                            gui_screen_sandbox_menu_get_budget_focus(this, element_value, quota, &focus_element);
                            child->set_focused_element_handle(focus_element, false);
                        }
                    }
                }
                else
                {
                    user_interface_play_sound(_ui_sound_error, -1);
                }
                handled = true;
            }
        }
    }
    break;
    case _sandbox_menu_string_common_spinner:
    {
        long category = m_selected_category;
        if (category != -1)
        {
            const s_variant_quota *quota = &tls_game_engine_globals()->map_info.quotas[category];
            long player_index = gui_screen_sandbox_menu_controller_player(controller_get(gui_screen_sandbox_menu_message_controller(message)));
            long name = data->m_name;
            long handle = item->get_element_handle();
            game_engine_sandbox_editor_action action;
            {
                unsigned __int64 *p = (unsigned __int64 *)&action;
                unsigned int count = 8;
                do { *p++ = 0; } while (--count);
            }
            action.definition_index = quota->object_definition_index;
            action.minimum_count = quota->minimum_count;
            action.maximum_count = quota->maximum_count;
            action.action = 6;
            action.player_index = player_index & 0xffff;
            switch (name)
            {
            case _sandbox_menu_string_budget_minumum:
            {
                long value;
                view->get_integer_value(handle, _string_id_value, &value);
                action.minimum_count = (unsigned char)value;
            }
            break;
            case _sandbox_menu_string_budget_maximum:
            {
                long value;
                view->get_integer_value(handle, _string_id_value, &value);
                action.maximum_count = (unsigned char)value;
            }
            break;
            }
            if (action.maximum_count != quota->maximum_count || action.minimum_count != quota->minimum_count)
                simulation_queue_editor_action_insert(&action);
        }
        item->handle_widget_back_out();
        handled = true;
    }
    break;
    }
    return handled;
}
