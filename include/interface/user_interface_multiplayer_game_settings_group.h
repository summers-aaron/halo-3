#ifndef INTERFACE_USER_INTERFACE_MULTIPLAYER_GAME_SETTINGS_GROUP_H
#define INTERFACE_USER_INTERFACE_MULTIPLAYER_GAME_SETTINGS_GROUP_H

#include "game/game_variant_parameter_types.h"
#include "interface/gui_screens/game_options/game_engine_category_types.h"
#include "interface/multiplayer_variant_settings_interface_definition.h"

// Functions defined in tu/interfacelibxbox/user_interface_multiplayer_game_settings_group.cpp.

struct s_object_properties_tag_block_definition;

const s_object_properties_tag_block_definition *get_sandbox_object_property_values(void);

extern "C" const s_multiplayer_variant_settings_interface_definition *multiplayer_variant_settings_interface_definition_try_and_get_master(void);
extern "C" void *text_value_pair_definition_try_and_get_recursive(const s_multiplayer_variant_settings_interface_definition *node, int key);
extern "C" const s_variant_setting_edit_reference *variant_setting_edit_reference_try_and_get_by_setting_recursive(
    const s_multiplayer_variant_settings_interface_definition *definition, long category);

class c_game_variant;
struct s_text_value_pair;
struct s_text_value_pair_definition;

bool get_text_value_pair_value_from_variant_parameter_value(const c_game_variant *variant, e_game_variant_parameter parameter,
    e_game_variant_parameter template_parameter, s_text_value_pair *pair, short *out_index,
    const s_text_value_pair_definition **definition);

__forceinline const s_text_value_pair_definition *text_value_pair_definition_try_and_get(long parameter)
{
    const s_text_value_pair_definition *result = 0;
    const s_multiplayer_variant_settings_interface_definition *master = multiplayer_variant_settings_interface_definition_try_and_get_master();
    if (master)
        result = (const s_text_value_pair_definition *)text_value_pair_definition_try_and_get_recursive(master, parameter);
    return result;
}
e_game_variant_parameter get_player_trait_parameter_from_template_parameter( e_game_engine_category category, e_game_variant_parameter template_parameter);

#endif // INTERFACE_USER_INTERFACE_MULTIPLAYER_GAME_SETTINGS_GROUP_H
