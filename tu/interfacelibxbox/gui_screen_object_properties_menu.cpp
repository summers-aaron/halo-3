// interfacelibxbox/gui_screen_object_properties_menu.obj: from the records 0x4dcca0 0x4dcce8 0x4dd010 0x4dd028 0x4dd0b0 0x4dd898 0x4dda90 0x4ddae0 0x4ddd08 0x4dddc0 0x4de330 0x4de480 0x4de4c8 0x4de620
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "interface/gui_screen_object_properties_externs.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
// @address 0x4dd0b0 ?show_only_necessary_fields@c_sandbox_object_properties_screen_widget@@QAAXPBUs_variant_object_datum@@@Z
// @address 0x4dcca0 ?get_current_item_count_internal@c_gui_property_value_datasource@@MAAJXZ
// @address 0x4dcce8 ?get_text_value@c_sandbox_object_properties_items_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4dd010 ?get_string_id_value@c_sandbox_object_properties_items_datasource@@UAA_NJJPAJ@Z
// @address 0x4dd028 ?post_initialize@c_sandbox_object_properties_screen_widget@@UAAXXZ
// @address 0x4dd898 ?initialize_datasource@c_sandbox_object_properties_screen_widget@@UAAXXZ
// @address 0x1ce398 ??0s_variant_object_datum@@QAA@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4865f8 ?get_data@c_gui_screen_widget@@QAAPAVc_gui_data@@JPAJ@Z
// @address 0x1d20b8 ?get_variant_object_datum@c_map_variant@@QBA_NJPAUs_variant_object_datum@@@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x4dda90 object_properties_find_category
// @address 0x4ddd08 ?update@c_sandbox_object_properties_screen_widget@@UAAXK@Z
// @address 0x4de480 ?get_element@c_gui_property_value_datasource@@UAA_NJPAXJ@Z
// @address 0x4de4c8 ?get_text_value@c_gui_property_value_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4de620 ?get_string_id_value@c_gui_property_value_datasource@@UAA_NJJPAJ@Z
#include <string.h>
#include <stdlib.h>
#include "game/game_globals.h"

#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_multiplayer_game_settings_group.h"
#include "interface/sandbox_object_properties.h"
#include "items/weapons.h"
#include "game/map_variant.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_sandbox.h"
#include "interface/gui_screens/sandbox_editor/gui_screen_object_properties_menu.h"
#include "interface/gui_screens/sandbox_menu/gui_screen_sandbox_menu.h"
#include <new>
#include "game/game_engine_scoring_globals.h"
#include "interface/user_interface.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/controller_inline.h"
#include "memory/tls_globals.h"
#include "cache/cache_files.h"
#include "interface/c_gui_widget.h"
class c_message;

typedef c_static_wchar_string<1024> text_string;
#include "interface/gui_property_value_datasource.h"
#include "scenario/scenario_light_types.h"



class c_controller_input_message;
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/controller_types.h"
#include "game/player_mapping_types.h"
#include "memory/gameplay_tls_globals.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_map_variant_utilities.h"
#include "game/game_engine_get_inline.h"
#include "game/game_engine_settings_definitions.h"
#include "tag_files/tag_groups.h"


#include "game/universal_types.h"



long c_gui_property_value_datasource::get_current_item_count_internal()
{
    if (real_range)
    {
        return (long)((maximum - minimum) * 2.0f + 1.0f);
    }
    return option_count;
}

bool c_sandbox_object_properties_items_datasource::get_text_value(long element_handle, long value_name, text_string *value)
{
    bool result = false;
    value->clear();
    if (element_handle != -1 && value_name == _object_properties_string_value)
    {
        long property_name = -1;
        c_gui_data *base = get_base_datasource();
        if (base->get_string_id_value(element_handle, _object_properties_string_name, &property_name))
        {
            // match: 0x4dcce8 inlines the datasource search; the out-of-line get_data call shape differs.
            c_gui_data *values = 0;
            c_gui_screen_widget *screen = owner;
            for (long i = 0; i < screen->m_datasource_count; i++)
            {
                if (screen->m_datasource[i]->get_name() == property_name)
                {
                    values = screen->m_datasource[i];
                    break;
                }
            }
            if (values)
            {
                s_variant_object_datum object;
                tls_game_engine_globals()->map_info.get_variant_object_datum(
                    owner->map_variant_index, &object);
                s_variant_multiplayer_object_properties_definition *properties = &object.properties;
                switch (property_name)
                {
                case _object_properties_string_respawn_rate:
                {
                    long value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->spawn_rate);
                    if (value_index == -1)
                        value->clear();
                    else
                        values->get_text_value(value_index, _object_properties_string_name, value);
                    break;
                }
                case _object_properties_string_spare_clips:
                case _object_properties_string_teleporter_channel:
                case _object_properties_string_spawn_order:
                {
                    long value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->spare_clips);
                    if (value_index == -1)
                        value->clear();
                    else
                        values->get_text_value(value_index, _object_properties_string_name, value);
                    break;
                }
                case _object_properties_string_unique_object:
                case _object_properties_string_minimum_count:
                case _object_properties_string_maximum_count:
                    break;
                case _object_properties_string_on_map_at_start:
                    values->get_text_value(!(properties->placement_flags.get_unsafe() & 2), _object_properties_string_name, value);
                    break;
                case _object_properties_string_symmetry:
                {
                    bool asymmetric = (bool)(_rotl(properties->placement_flags.get_unsafe(), 29) & 1);
                    bool symmetric = (bool)(_rotl(properties->placement_flags.get_unsafe(), 30) & 1);
                    long index = _game_engine_symmetric_placement_default;
                    if (symmetric != asymmetric)
                    {
                        if (symmetric)
                            index = _game_engine_symmetric_placement_symmetric;
                        else if (asymmetric)
                            index = _game_engine_symmetric_placement_asymmetric;
                    }
                    values->get_text_value(index, _object_properties_string_name, value);
                    break;
                }
                case _object_properties_string_team_affiliation:
                    values->get_text_value(properties->team_designator, _object_properties_string_name, value);
                    break;
                case _object_properties_string_shape_type:
                    values->get_text_value(map_variant_value_pair_get_value_index(property_name, (long)properties->boundary_shape),
                        _object_properties_string_name, value);
                    break;
                case _object_properties_string_shape_radius:
                case _object_properties_string_shape_width:
                    value->print(L"%.2f", (double)properties->boundary_width);
                    break;
                case _object_properties_string_shape_top:
                    value->print(L"%.2f", (double)properties->boundary_positive_height);
                    break;
                case _object_properties_string_shape_bottom:
                    value->print(L"%.2f", (double)properties->boundary_negative_height);
                    break;
                case _object_properties_string_shape_depth:
                    value->print(L"%.2f", (double)properties->boundary_box_length);
                    break;
                default:
                    break;
                }
                result = true;
            }
        }
    }
    return result;
}

bool c_sandbox_object_properties_items_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    return get_base_datasource()->get_string_id_value(element_handle, value_name, value);
}

void c_sandbox_object_properties_screen_widget::post_initialize()
{
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _object_properties_string_object_properties_list);
    if (list != 0)
    {
        c_gui_data *data = list->get_data();
        long element_handle = data != 0 ? data->get_first_element_handle() : -1;
        transfer_focus_to_list(list, element_handle, true, false);
    }
}

extern "C" long object_properties_find_category(s_tag_block *block, long definition_index)
{
    for (short i = 0; i < block->count; ++i)
    {
        const scenario_object_palette_entry *entry = &((const scenario_object_palette_entry *)block->address)[i];
        if (entry->object.index == definition_index)
        {
            return entry->category;
        }
    }
    return 0;
}

bool c_gui_property_value_datasource::get_element(long index, void *element, long element_size)
{
    memcpy(element, &options[index].string_id, sizeof(options[index].string_id));
    return true;
}

static __forceinline void gui_screen_object_properties_menu_sandbox_get_string(long string_id, text_string *text)
{
    s_game_globals *game_globals = global_game_globals;
    text->clear();
    if (string_id != -1 && string_id != 0 && game_globals && game_globals->multiplayer_globals_index != -1)
    {
        const s_multiplayer_globals_definition *multiplayer_globals =
            TAG_GET('mulg', const s_multiplayer_globals_definition, game_globals->multiplayer_globals_index);
        if (multiplayer_globals->universal.count > 0)
        {
            const s_multiplayer_universal_globals_definition *universal =
                (const s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
            text_group_get_string(universal->sandbox_strings.index, string_id, text);
        }
    }
}

bool c_gui_property_value_datasource::get_text_value(long element_handle, long value_name, text_string *value)
{
    bool result = false;
    value->clear();
    if (real_range)
    {
        float range_value = element_handle * 0.5f;
        range_value += minimum;
        value->print(L"%.2f", range_value);
    }
    else if (value_name == _object_properties_string_name && element_handle >= 0 && element_handle < option_count)
    {
        if (options[element_handle].value)
            gui_screen_object_properties_menu_sandbox_get_string(options[element_handle].value, value);
        else
            value->print(L"%d", options[element_handle].string_id);
        result = true;
    }
    return result;
}

bool c_gui_property_value_datasource::get_string_id_value(long index, long type, long *value)
{
    bool result = false;
    if (type == _object_properties_string_name)
    {
        if (index >= 0 && index < option_count)
        {
            *value = options[index].value;
            result = true;
        }
    }
    return result;
}

// @address 0x4de330 ?initialize@c_gui_property_value_datasource@@UAA_NJ@Z
bool c_gui_property_value_datasource::initialize(long name)
{
    bool result = false;
    real_range = false;
    m_name = name;
    result = true;
    if (result)
    {
        const s_object_properties_tag_block_definition *properties = get_sandbox_object_property_values();
        if (properties)
        {
            for (short i = 0; i < properties->properties.count(); ++i)
            {
                const s_object_property_definition *property = &properties->properties[i];
                if (property->name == property_name)
                {
                    if (property->values.count() == 2)
                    {
                        const s_text_value_pair *value_pair = &property->values[0];
                        if (value_pair->type == _text_value_pair_parameter_real)
                        {
                            real_range = true;
                            minimum = (float)value_pair->value;
                            value_pair = &property->values[1];
                            maximum = (float)value_pair->value;
                        }
                    }
                    if (!real_range)
                    {
                        for (short j = 0; j < property->values.count(); ++j)
                        {
                            const s_text_value_pair *value_pair = &property->values[j];
                            if (value_pair)
                            {
                                long index = option_count++;
                                s_gui_property_value_option &pair = options[index];
                                /* the record's pair type is {value, string_id}; this unit's option is {string_id, value} */
                                pair.string_id = value_pair->value;
                                pair.value = value_pair->display_string;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

void c_sandbox_object_properties_screen_widget::show_only_necessary_fields(const s_variant_object_datum *variant_object)
{
    c_gui_data *fields = source;
    if (fields)
    {
        fields->clear_disabled_elements();
        switch ((signed char)variant_object->properties.teleporter_channel)
        {
        case _multiplayer_object_type_none:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_teleporter_channel);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_type);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spawn_order);
            break;
        case _multiplayer_object_type_grenade:
        case _multiplayer_object_type_powerup:
        case _multiplayer_object_type_equipment:
        case _multiplayer_object_type_light_land_vehicle:
        case _multiplayer_object_type_heavy_land_vehicle:
        case _multiplayer_object_type_flying_vehicle:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_teleporter_channel);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_type);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spawn_order);
            break;
        case _multiplayer_object_type_weapon:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_teleporter_channel);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_type);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spawn_order);
            if (variant_object->quota_index != -1 && weapon_is_age_based(
                tls_game_engine_globals()->map_info.quotas[variant_object->quota_index].object_definition_index))
            {
                fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            }
            break;
        case _multiplayer_object_type_koth_hill:
        case _multiplayer_object_type_juggernaut_destination:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_symmetry);
        case _multiplayer_object_type_infection_haven:
        case _multiplayer_object_type_territory:
        case _multiplayer_object_type_vip_destination:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
        case _multiplayer_object_type_respawn_zone:
        {
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_respawn_rate);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_on_map_at_start);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_teleporter_channel);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            long shape = variant_object->properties.boundary_shape;
            if (shape != _multiplayer_object_boundary_shape_sphere)
            {
                if (shape != _multiplayer_object_boundary_shape_cylinder)
                {
                    if (shape == _multiplayer_object_boundary_shape_box)
                        fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
                }
                else
                {
                    fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
                    fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
                }
            }
            else
            {
                fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
                fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
                fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
                fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            }
            break;
        }
        case _multiplayer_object_type_oddball_ball_spawn_point:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_symmetry);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
        case _multiplayer_object_type_spawn_point:
        case _multiplayer_object_type_flag_or_bomb_spawn_point:
        case _multiplayer_object_type_target_spawn_location:
        case _multiplayer_object_type_flag_return_or_bomb_goal_area:
        case _multiplayer_object_type_vip_boundary:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_respawn_rate);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_on_map_at_start);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_teleporter_channel);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_type);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spawn_order);
            break;
        case _multiplayer_object_type_teleporter_2way:
        case _multiplayer_object_type_teleporter_sender:
        case _multiplayer_object_type_teleporter_receiver:
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_on_map_at_start);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spare_clips);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_spawn_order);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_respawn_rate);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_type);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_radius);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_width);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_top);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_bottom);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_shape_depth);
            fields->set_disabled_element(_object_properties_string_name, _object_properties_string_team_affiliation);
            break;
        default:
            break;
        }
    }
}

// @address 0x55b88 ??_7c_sandbox_object_properties_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_sandbox_object_properties_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_sandbox_object_properties_items_datasource@@UAAPAXI@Z
// @address 0x55b18 ??_7c_gui_property_value_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_property_value_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_property_value_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_sandbox_object_properties_items_datasource@@UAAXQAJPAJ@Z
void c_sandbox_object_properties_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _object_properties_string_value;
    ++*column_count;
}

// @address 0x4a8578 ?get_column_names@c_gui_property_value_datasource@@UAAXQAJPAJ@Z
void c_gui_property_value_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
}

void c_sandbox_object_properties_screen_widget::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    long property_name = -1;
    c_gui_data *source = (c_gui_data *)get_data(_object_properties_string_object_properties_datasource, &property_name);
    if (source != 0)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_sandbox_object_properties_items_datasource), 0, 0);
        c_sandbox_object_properties_items_datasource *items = memory != 0 ? new (memory) c_sandbox_object_properties_items_datasource(source, this) : 0;
        if (items != 0)
        {
            if (items->initialize())
            {
                m_datasource[m_datasource_count] = items;
                m_datasource_count++;
            }
            else
            {
                items->~c_sandbox_object_properties_items_datasource();
                user_interface_free(items);
            }
        }
        this->source = source;
        for (long element = source->get_first_element_handle(); element != -1; element = source->get_next_element_handle(element))
        {
            source->get_string_id_value(element, _object_properties_string_name, &property_name);
            void *value_memory = user_interface_malloc_tracked(sizeof(c_gui_property_value_datasource), 0, 0);
            c_gui_property_value_datasource *property = value_memory != 0 ? new (value_memory) c_gui_property_value_datasource(property_name) : 0;
            property->initialize(property_name);
            m_datasource[m_datasource_count] = property;
            m_datasource_count++;
        }
        if (map_variant_index != -1)
        {
            s_variant_object_datum variant_object;
            tls_game_engine_globals()->map_info.get_variant_object_datum(map_variant_index, &variant_object);
            show_only_necessary_fields(&variant_object);
        }
    }
}

void c_sandbox_object_properties_screen_widget::update(unsigned long current_milliseconds)
{
    if (map_variant_index != -1)
    {
        s_variant_object_datum variant_object;
        tls_game_engine_globals()->map_info.get_variant_object_datum(map_variant_index, &variant_object);
        show_only_necessary_fields(&variant_object);
        if (variant_object.flags.is_empty() || variant_object.flags.test(_variant_object_placement_scenario_object_removed_bit))
            transition_out_with_transition_type(0, _screen_transition_normal);
    }
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4ddae0 ?handle_controller_input_message@c_sandbox_object_properties_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_sandbox_object_properties_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->m_event_type == _event_type_button_press && message->m_component == _controller_component_button_x)
    {
        if (map_variant_index != -1)
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
            s_variant_object_datum object;
            c_map_variant *map_variant = game_engine_get_runtime_map_variant();
            map_variant->get_variant_object_datum(map_variant_index, &object);
            e_window_index window = message->m_window;
            void *memory = user_interface_malloc_tracked(sizeof(c_load_budget_screen_message), 0, 0);
            if (memory)
            {
                long &budget_index = object.quota_index;
                long definition_index = map_variant->quotas[budget_index].object_definition_index;
                long category = 0;
                if (global_scenario)
                {
                    category = object_properties_find_category(&global_scenario->sandbox_vehicles, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_weapons, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_equipment, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_scenery, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_machines, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_sound_scenery, definition_index);
                    if (!category) category = object_properties_find_category(&global_scenario->sandbox_crates, definition_index);
                }
                c_load_budget_screen_message *load = new (memory) c_load_budget_screen_message(
                    message->m_controller, window, budget_index, m_name, category, secondary);
                load->m_parent_screen_index = m_screen_index;
                ::user_interface_messaging_post(load);
            }
        }
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4dddc0 ?handle_list_item_chosen@c_sandbox_object_properties_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_sandbox_object_properties_screen_widget::handle_list_item_chosen(const c_controller_input_message *message,
    long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    long property_name;
    switch (list_name)
    {
    case _object_properties_string_object_properties_list:
    {
        if (data->get_string_id_value(item->get_element_handle(), _object_properties_string_name, &property_name) &&
            property_name != -1 && property_name != 0 && item->get_enabled())
        {
            gui_widget_get_parent_list_widget(item)->invoke_submenu(_object_properties_string_common_spinner, property_name, false);
            c_gui_widget *list = get_child_widget(_gui_list_widget, _object_properties_string_common_spinner);
            if (list)
            {
                s_variant_object_datum object;
                game_engine_get_runtime_map_variant()->get_variant_object_datum(map_variant_index, &object);
                const s_variant_multiplayer_object_properties_definition *properties = &object.properties;
                long value_index;
                switch (property_name)
                {
                case _object_properties_string_respawn_rate:
                    value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->spawn_rate);
                    break;
                case _object_properties_string_spare_clips:
                case _object_properties_string_teleporter_channel:
                case _object_properties_string_spawn_order:
                    value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->spare_clips);
                    break;
                case _object_properties_string_on_map_at_start:
                value_index = map_variant_value_pair_get_value_index(
                    property_name, (long)(((unsigned long)~properties->placement_flags.get_unsafe() >> 1) & 1));
                break;
                case _object_properties_string_team_affiliation:
                    value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->team_designator);
                    break;
                case _object_properties_string_shape_type:
                    value_index = map_variant_value_pair_get_value_index(property_name, (long)properties->boundary_shape);
                    break;
                case _object_properties_string_symmetry:
                {
                    long placement = _game_engine_symmetric_placement_default;
                    if (!properties->placement_flags.test(_variant_placement_symmetric_bit))
                        placement = _game_engine_symmetric_placement_asymmetric;
                    else if (!properties->placement_flags.test(_variant_placement_asymmetric_bit))
                        placement = _game_engine_symmetric_placement_symmetric;
                    value_index = map_variant_value_pair_get_value_index(property_name, placement);
                    break;
                }
                case _object_properties_string_shape_radius:
                case _object_properties_string_shape_width:
                    value_index = map_variant_value_pair_get_value_index(property_name, properties->boundary_width);
                    break;
                case _object_properties_string_shape_top:
                    value_index = map_variant_value_pair_get_value_index(property_name, properties->boundary_positive_height);
                    break;
                case _object_properties_string_shape_bottom:
                    value_index = map_variant_value_pair_get_value_index(property_name, properties->boundary_negative_height);
                    break;
                case _object_properties_string_shape_depth:
                    value_index = map_variant_value_pair_get_value_index(property_name, properties->boundary_box_length);
                    break;
                }
                ((c_gui_list_widget *)list)->set_focused_element_handle(value_index, false);
            }
            handled = true;
        }
    }
    break;
    case _object_properties_string_common_spinner:
    {
        property_name = data->get_name();
        long value_index = item->get_element_handle();
        s_variant_object_datum object;
        game_engine_get_runtime_map_variant()->get_variant_object_datum(map_variant_index, &object);
        s_variant_multiplayer_object_properties_definition *properties = &object.properties;
        long name = property_name;
        switch (name)
        {
        case _object_properties_string_respawn_rate:
            if (!properties->placement_flags.test(_variant_placement_flag1_bit) || map_variant_value_pair_get_integer(name, value_index) != 0)
                properties->spawn_rate = (unsigned char)map_variant_value_pair_get_integer(name, value_index);
            break;
        case _object_properties_string_spare_clips:
        case _object_properties_string_teleporter_channel:
        case _object_properties_string_spawn_order:
            properties->spare_clips = (unsigned char)map_variant_value_pair_get_integer(name, value_index);
            break;
        case _object_properties_string_on_map_at_start:
        {
            bool symmetric = map_variant_value_pair_get_integer(name, value_index) == 0;
            if ((signed char)properties->spawn_rate == 0) symmetric = false;
            properties->placement_flags.set(_variant_placement_flag1_bit, symmetric);
            break;
        }
        case _object_properties_string_symmetry:
            switch ((unsigned long)map_variant_value_pair_get_integer(name, value_index))
            {
            case _game_engine_symmetric_placement_default:
            properties->placement_flags.set(_variant_placement_symmetric_bit, true);
            properties->placement_flags.set(_variant_placement_asymmetric_bit, true);
            break;
            case _game_engine_symmetric_placement_symmetric:
            properties->placement_flags.set(_variant_placement_symmetric_bit, true);
            properties->placement_flags.set(_variant_placement_asymmetric_bit, false);
            break;
            case _game_engine_symmetric_placement_asymmetric:
            properties->placement_flags.set(_variant_placement_symmetric_bit, false);
            properties->placement_flags.set(_variant_placement_asymmetric_bit, true);
            break;
            }
            break;
        case _object_properties_string_team_affiliation:
            properties->team_designator = (e_variant_team_designator)map_variant_value_pair_get_integer(name, value_index);
            break;
        case _object_properties_string_shape_type:
            properties->boundary_shape = (e_multiplayer_object_boundary_shape)map_variant_value_pair_get_integer(name, value_index);
            memset(&properties->boundary_width, 0, 4 * sizeof(float));
            switch (properties->boundary_shape)
            {
            case _multiplayer_object_boundary_shape_sphere:
                properties->boundary_width = 1.0f;
                break;
            case _multiplayer_object_boundary_shape_cylinder:
                properties->boundary_positive_height = properties->boundary_width = 1.0f;
                break;
            case _multiplayer_object_boundary_shape_box:
                properties->boundary_box_length = properties->boundary_positive_height = properties->boundary_width = 1.0f;
                break;
            }
            show_only_necessary_fields(&object);
            break;
        case _object_properties_string_shape_radius:
        case _object_properties_string_shape_width: properties->boundary_width = map_variant_value_pair_get_real(name, value_index); break;
        case _object_properties_string_shape_top: properties->boundary_positive_height = map_variant_value_pair_get_real(name, value_index); break;
        case _object_properties_string_shape_bottom: properties->boundary_negative_height = map_variant_value_pair_get_real(name, value_index); break;
        case _object_properties_string_shape_depth: properties->boundary_box_length = map_variant_value_pair_get_real(name, value_index); break;
        }
        const c_controller_interface *controller = controller_get(message->m_controller);
        if (controller)
        {
            long player_index = -1;
            long user_index = controller->user_index;
            if (user_index != -1)
                player_index = tls_player_mapping_globals()->player_user_mapping[user_index];
            game_engine_sandbox_request_properties_change(player_index, map_variant_index, (const int *)properties);
        }
        item->handle_widget_back_out();
        handled = true;
    }
    break;
    }
    return handled;
}

