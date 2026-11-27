// interfacelibxbox/user_interface_multiplayer_game_settings_group.obj: from the records 0x4f4198 0x4f41f8 0x4f4260 0x4f4400 0x4f47a0 0x4f4908
// @flags /TP /O2
#include "scenario/scenario.h"
#include "game/game_variant.h"
#include "tag_files/tag_groups.h"
#include "interface/user_interface.h"
#include "interface/user_interface_multiplayer_game_settings_group.h"
#include "interface/sandbox_object_properties.h"
#include "game/game_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_variant_parameter_types.h"
#include "interface/gui_screens/game_options/game_engine_category_types.h"
#include "game/game_engine_base_variant.h"
#include "interface/text_value_pair_types.h"

// @address 0x4f41f8 ?get_sandbox_object_property_values@@YAPBUs_object_properties_tag_block_definition@@XZ
const s_object_properties_tag_block_definition *get_sandbox_object_property_values(void)
{
    void *result = 0;
    s_game_globals *game_globals = global_game_globals;
    if (game_globals && game_globals->multiplayer_globals.index != -1) {
        s_multiplayer_globals_definition *definition = (s_multiplayer_globals_definition *)tag_get(0, game_globals->multiplayer_globals.index);
        if (definition && definition->universal.count > 0 && definition->universal.address) {
            s_multiplayer_universal_globals_definition *element = (s_multiplayer_universal_globals_definition *)definition->universal.address;
            result = tag_get(0, element->sandbox_object_properties.index);
        }
    }
    return (const s_object_properties_tag_block_definition *)result;
}

// @address 0x4f4198 multiplayer_variant_settings_interface_definition_try_and_get_master
const s_multiplayer_variant_settings_interface_definition *multiplayer_variant_settings_interface_definition_try_and_get_master(void)
{
    const s_multiplayer_variant_settings_interface_definition *result = 0;
    const s_user_interface_tag_globals *definition = user_interface_tag_globals_try_and_get();
    if (definition && definition->multiplayer_variant_settings.index != -1)
        result = (const s_multiplayer_variant_settings_interface_definition *)global_tag_instances[
            (unsigned short)definition->multiplayer_variant_settings.index].definition;
    return result;
}

// @address 0x4f4400 ?get_player_trait_parameter_from_template_parameter@@YA?AW4e_game_variant_parameter@@W4e_game_engine_category@@W41@@Z
e_game_variant_parameter get_player_trait_parameter_from_template_parameter(
    e_game_engine_category category, e_game_variant_parameter template_parameter)
{
    e_game_variant_parameter parameter = _game_variant_parameter_none;
    const long first_template_parameter = _template_shld_dmg_resistance;
    const long last_template_parameter = _template_forced_change_colors;
    if (template_parameter >= first_template_parameter && template_parameter <= last_template_parameter)
    {
        long trait_parameter_offset = template_parameter - first_template_parameter;
        switch (category)
        {
        case _game_engine_category_player_traits:
        case _game_engine_category_player_traits_shields:
        case _game_engine_category_player_traits_weapons:
        case _game_engine_category_player_traits_movement:
        case _game_engine_category_player_traits_appearance:
        case _game_engine_category_player_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _player_shld_dmg_resistance);
            break;
        case _game_engine_category_respawn_traits:
        case _game_engine_category_respawn_traits_shields:
        case _game_engine_category_respawn_traits_weapons:
        case _game_engine_category_respawn_traits_movement:
        case _game_engine_category_respawn_traits_appearance:
        case _game_engine_category_respawn_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _respawn_shld_dmg_resistance);
            break;
        case _game_engine_category_red_powerup_traits:
        case _game_engine_category_red_powerup_traits_shields:
        case _game_engine_category_red_powerup_traits_weapons:
        case _game_engine_category_red_powerup_traits_movement:
        case _game_engine_category_red_powerup_traits_appearance:
        case _game_engine_category_red_powerup_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _red_powerup_shld_dmg_resistance);
            break;
        case _game_engine_category_blue_powerup_traits:
        case _game_engine_category_blue_powerup_traits_shields:
        case _game_engine_category_blue_powerup_traits_weapons:
        case _game_engine_category_blue_powerup_traits_movement:
        case _game_engine_category_blue_powerup_traits_appearance:
        case _game_engine_category_blue_powerup_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _blue_powerup_shld_dmg_resistance);
            break;
        case _game_engine_category_yellow_powerup_traits:
        case _game_engine_category_yellow_powerup_traits_shields:
        case _game_engine_category_yellow_powerup_traits_weapons:
        case _game_engine_category_yellow_powerup_traits_movement:
        case _game_engine_category_yellow_powerup_traits_appearance:
        case _game_engine_category_yellow_powerup_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _yellow_powerup_shld_dmg_resistance);
            break;
        case _game_engine_category_slayer_leader_traits:
        case _game_engine_category_slayer_leader_traits_shields:
        case _game_engine_category_slayer_leader_traits_weapons:
        case _game_engine_category_slayer_leader_traits_movement:
        case _game_engine_category_slayer_leader_traits_appearance:
        case _game_engine_category_slayer_leader_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _slayer_leader_shld_dmg_resistance);
            break;
        case _game_engine_category_ball_carrier_traits:
        case _game_engine_category_ball_carrier_traits_shields:
        case _game_engine_category_ball_carrier_traits_weapons:
        case _game_engine_category_ball_carrier_traits_movement:
        case _game_engine_category_ball_carrier_traits_appearance:
        case _game_engine_category_ball_carrier_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _ball_carrier_shld_dmg_resistance);
            break;
        case _game_engine_category_assault_carrier_traits:
        case _game_engine_category_assault_carrier_traits_shields:
        case _game_engine_category_assault_carrier_traits_weapons:
        case _game_engine_category_assault_carrier_traits_movement:
        case _game_engine_category_assault_carrier_traits_appearance:
        case _game_engine_category_assault_carrier_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _assault_carrier_shld_dmg_resistance);
            break;
        case _game_engine_category_flag_carrier_traits:
        case _game_engine_category_flag_carrier_traits_shields:
        case _game_engine_category_flag_carrier_traits_weapons:
        case _game_engine_category_flag_carrier_traits_movement:
        case _game_engine_category_flag_carrier_traits_appearance:
        case _game_engine_category_flag_carrier_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _flag_carrier_shld_dmg_resistance);
            break;
        case _game_engine_category_juggernaut_traits:
        case _game_engine_category_juggernaut_traits_shields:
        case _game_engine_category_juggernaut_traits_weapons:
        case _game_engine_category_juggernaut_traits_movement:
        case _game_engine_category_juggernaut_traits_appearance:
        case _game_engine_category_juggernaut_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _juggernaut_shld_dmg_resistance);
            break;
        case _game_engine_category_on_hill_traits:
        case _game_engine_category_on_hill_traits_shields:
        case _game_engine_category_on_hill_traits_weapons:
        case _game_engine_category_on_hill_traits_movement:
        case _game_engine_category_on_hill_traits_appearance:
        case _game_engine_category_on_hill_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _on_hill_shld_dmg_resistance);
            break;
        case _game_engine_category_zombie_traits:
        case _game_engine_category_zombie_traits_shields:
        case _game_engine_category_zombie_traits_weapons:
        case _game_engine_category_zombie_traits_movement:
        case _game_engine_category_zombie_traits_appearance:
        case _game_engine_category_zombie_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _zombie_shld_dmg_resistance);
            break;
        case _game_engine_category_first_zombie_traits:
        case _game_engine_category_first_zombie_traits_shields:
        case _game_engine_category_first_zombie_traits_weapons:
        case _game_engine_category_first_zombie_traits_movement:
        case _game_engine_category_first_zombie_traits_appearance:
        case _game_engine_category_first_zombie_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _first_zombie_shld_dmg_resistance);
            break;
        case _game_engine_category_haven_defender_traits:
        case _game_engine_category_haven_defender_traits_shields:
        case _game_engine_category_haven_defender_traits_weapons:
        case _game_engine_category_haven_defender_traits_movement:
        case _game_engine_category_haven_defender_traits_appearance:
        case _game_engine_category_haven_defender_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _haven_defender_shld_dmg_resistance);
            break;
        case _game_engine_category_last_human_traits:
        case _game_engine_category_last_human_traits_shields:
        case _game_engine_category_last_human_traits_weapons:
        case _game_engine_category_last_human_traits_movement:
        case _game_engine_category_last_human_traits_appearance:
        case _game_engine_category_last_human_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _last_human_shld_dmg_resistance);
            break;
        case _game_engine_category_territory_defender_traits:
        case _game_engine_category_territory_defender_traits_shields:
        case _game_engine_category_territory_defender_traits_weapons:
        case _game_engine_category_territory_defender_traits_movement:
        case _game_engine_category_territory_defender_traits_appearance:
        case _game_engine_category_territory_defender_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _territory_defender_shld_dmg_resistance);
            break;
        case _game_engine_category_territory_attacker_traits:
        case _game_engine_category_territory_attacker_traits_shields:
        case _game_engine_category_territory_attacker_traits_weapons:
        case _game_engine_category_territory_attacker_traits_movement:
        case _game_engine_category_territory_attacker_traits_appearance:
        case _game_engine_category_territory_attacker_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _territory_attacker_shld_dmg_resistance);
            break;
        case _game_engine_category_vip_team_traits:
        case _game_engine_category_vip_team_traits_shields:
        case _game_engine_category_vip_team_traits_weapons:
        case _game_engine_category_vip_team_traits_movement:
        case _game_engine_category_vip_team_traits_appearance:
        case _game_engine_category_vip_team_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _vip_team_shld_dmg_resistance);
            break;
        case _game_engine_category_vip_influence_traits:
        case _game_engine_category_vip_influence_traits_shields:
        case _game_engine_category_vip_influence_traits_weapons:
        case _game_engine_category_vip_influence_traits_movement:
        case _game_engine_category_vip_influence_traits_appearance:
        case _game_engine_category_vip_influence_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _vip_influence_shld_dmg_resistance);
            break;
        case _game_engine_category_vip_traits:
        case _game_engine_category_vip_traits_shields:
        case _game_engine_category_vip_traits_weapons:
        case _game_engine_category_vip_traits_movement:
        case _game_engine_category_vip_traits_appearance:
        case _game_engine_category_vip_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _vip_shld_dmg_resistance);
            break;
        case _game_engine_category_sandbox_editor_traits:
        case _game_engine_category_sandbox_editor_traits_shields:
        case _game_engine_category_sandbox_editor_traits_weapons:
        case _game_engine_category_sandbox_editor_traits_movement:
        case _game_engine_category_sandbox_editor_traits_appearance:
        case _game_engine_category_sandbox_editor_traits_sensors:
            parameter = (e_game_variant_parameter)(trait_parameter_offset + _sandbox_editor_shld_dmg_resistance);
            break;
        default:
            break;
        }
    }
    return parameter;
}


// @address 0x4f47a0 text_value_pair_definition_try_and_get_recursive
void *text_value_pair_definition_try_and_get_recursive(const s_multiplayer_variant_settings_interface_definition *node, int key)
{
    void *result = 0;
    for (int i = 0; i < node->edit_references.count; i++) {
        if (result)
            break;
        const s_variant_setting_edit_reference *group = &((const s_variant_setting_edit_reference *)node->edit_references.address)[i];
        int j;
        for (j = 0; j < group->options.count; j++) {
            const s_variant_setting_option *entry = &((const s_variant_setting_option *)group->options.address)[j];
            if (entry->parameter.index != -1) {
                int *definition = (int *)tag_get(0, entry->parameter.index);
                if (*definition == key) {
                    result = definition;
                    break;
                }
            }
        }
        if (!result) {
            for (j = 0; j < group->options.count; j++) {
                const s_variant_setting_option *entry = &((const s_variant_setting_option *)group->options.address)[j];
                if (entry->category.index != -1) {
                    result = text_value_pair_definition_try_and_get_recursive(
                        (const s_multiplayer_variant_settings_interface_definition *)tag_get(0, entry->category.index), key);
                    if (result)
                        break;
                }
            }
        }
        if (!result) {
            for (j = 0; j < group->options.count; j++) {
                const s_variant_setting_option *entry = &((const s_variant_setting_option *)group->options.address)[j];
                if (entry->template_category.index != -1) {
                    result = text_value_pair_definition_try_and_get_recursive(
                        (const s_multiplayer_variant_settings_interface_definition *)tag_get(0, entry->template_category.index), key);
                    if (result)
                        break;
                }
            }
        }
    }
    return result;
}

// @address 0x4f4908 variant_setting_edit_reference_try_and_get_by_setting_recursive
const s_variant_setting_edit_reference *variant_setting_edit_reference_try_and_get_by_setting_recursive(
    const s_multiplayer_variant_settings_interface_definition *definition, long category)
{
    const s_variant_setting_edit_reference *result = 0;
    for (long reference_index = 0; reference_index < definition->edit_references.count && !result; reference_index++)
    {
        const s_variant_setting_edit_reference *edit_reference =
            &((const s_variant_setting_edit_reference *)definition->edit_references.address)[reference_index];
        if (edit_reference->category == category)
        {
            result = edit_reference;
            break;
        }
        else
        {
            for (long option_index = 0; option_index < edit_reference->options.count && !result; option_index++)
            {
                const s_variant_setting_option *option = &((const s_variant_setting_option *)edit_reference->options.address)[option_index];
                if (option->category.index != -1)
                {
                    result = variant_setting_edit_reference_try_and_get_by_setting_recursive(
                        (const s_multiplayer_variant_settings_interface_definition *)tag_get(0, option->category.index), category);
                    if (result)
                        break;
                }
                if (option->template_category.index != -1)
                {
                    result = variant_setting_edit_reference_try_and_get_by_setting_recursive(
                        (const s_multiplayer_variant_settings_interface_definition *)tag_get(0, option->template_category.index), category);
                    if (result)
                        break;
                }
            }
        }
    }
    return result;
}

// @address 0x4f4260 ?get_text_value_pair_value_from_variant_parameter_value@@YA_NPBVc_game_variant@@W4e_game_variant_parameter@@1PAUs_text_value_pair@@PAFPAPBUs_text_value_pair_definition@@@Z
bool get_text_value_pair_value_from_variant_parameter_value(const c_game_variant *variant, e_game_variant_parameter parameter,
    // match: 0x4f4260: pointer __restrict qualifier prevents alias reload
    e_game_variant_parameter template_parameter, s_text_value_pair *pair, short *__restrict out_index,
    const s_text_value_pair_definition **definition)
{
    *out_index = -1;
    pair->flags = 0;
    pair->type = c_game_variant::get_expected_game_engine_setting_parameter_type(parameter);
    pair->value = 0;
    pair->string_value = -1;
    pair->display_string = 0x10386;
    pair->description_string = 0x10386;
    if (definition)
        *definition = 0;
    bool result;
    switch (pair->type)
    {
    case _text_value_pair_parameter_integer:
        result = variant->get_integer_game_engine_setting(parameter, &pair->value);
        break;
    case _text_value_pair_parameter_string_id:
        result = variant->get_string_id_game_engine_setting(parameter, &pair->string_value);
        if (result)
            pair->display_string = pair->string_value;
        break;
    default:
        result = false;
        break;
    }
    if (result)
    {
        const s_text_value_pair_definition *def = text_value_pair_definition_try_and_get(template_parameter);
        if (def)
        {
            for (long i = 0; i < def->values.count; i++)
            {
                const s_text_value_pair *value = &((const s_text_value_pair *)def->values.address)[i];
                bool equal;
                switch (pair->type)
                {
                case _text_value_pair_parameter_integer:
                    equal = value->value == pair->value;
                    break;
                case _text_value_pair_parameter_string_id:
                    equal = value->string_value == pair->string_value;
                    break;
                default:
                    equal = false;
                    break;
                }
                if (equal)
                {
                    pair->flags = value->flags;
                    pair->display_string = value->display_string;
                    pair->description_string = value->description_string;
                    *out_index = (short)i;
                    break;
                }
            }
            if (definition)
                *definition = def;
        }
    }
    return result;
}
