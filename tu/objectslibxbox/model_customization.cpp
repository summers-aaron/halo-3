// objectslibxbox/model_customization.obj: from the records 0x354d48 0x354de0
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "game/game_globals.h"
#include "objects/model_customization.h"
#include "game/game_engine_settings_definitions.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
// @address 0x354d48 ?get_multiplayer_customized_model_character@@YAPAUs_multiplayer_customized_model_character@@W4e_player_model_choice@@@Z
// @address 0x354de0 ?model_customization_get_overrides_for_placement@@YAXPBUs_player_appearance@@PAUobject_placement_data@@@Z

#include "interface/player_model_choice.h"
#include "objects/model_customization_types.h"
#include "game/player_configuration.h"
#include "tag_files/tag_groups.h"
#include "objects/object_placement.h"

s_multiplayer_customized_model_character *get_multiplayer_customized_model_character(e_player_model_choice player_model_choice)
{
    s_multiplayer_customized_model_character *result = NULL;
    long character_name = NONE;

    switch (player_model_choice)
    {
    case spartan:
        character_name = 197;
        break;
    case elite:
        character_name = 198;
        break;
    default:
        UNREACHABLE();
    }

    s_game_globals *game_globals = global_game_globals;
    if (game_globals && game_globals->multiplayer_globals_index != NONE)
    {
        s_multiplayer_globals_definition *multiplayer_globals = (s_multiplayer_globals_definition *)tag_get('mulg', game_globals->multiplayer_globals_index);
        if (multiplayer_globals->universal.count > 0)
        {
            s_multiplayer_universal_globals_definition *universal =
                (s_multiplayer_universal_globals_definition *)tag_block_get_element_with_size(
                    &multiplayer_globals->universal, 0, sizeof(s_multiplayer_universal_globals_definition));
            for (long character_index = 0; character_index < universal->customized_characters.count; character_index++)
            {
                s_multiplayer_customized_model_character *character =
                    (s_multiplayer_customized_model_character *)tag_block_get_element_with_size(
                        &universal->customized_characters, character_index, sizeof(s_multiplayer_customized_model_character));
                if (character->name == character_name)
                {
                    result = character;
                    break;
                }
            }
        }
    }

    return result;
}

void model_customization_get_overrides_for_placement(struct s_player_appearance const * _sym_p0, struct object_placement_data * _sym_p1)
{
    s_multiplayer_customized_model_character *model = get_multiplayer_customized_model_character((e_player_model_choice)_sym_p0->player_model_choice);
    if (model && _sym_p0->player_model_choice >= 0 && (unsigned long)_sym_p0->player_model_choice < 2) {
        const unsigned char *choices = _sym_p0->model_permutations[_sym_p0->player_model_choice];
        for (long region_index = 0; region_index < 4; ++region_index) {
            s_multiplayer_customized_model_region *region_data = &model->regions[region_index];
            if (choices[region_index] < region_data->permutations.count()) {
                s_multiplayer_customized_model_permutation *choice = &region_data->permutations[choices[region_index]];
                for (long i = 0; i < choice->overrides.count(); ++i) {
                    s_object_region_permutation *override_data = &choice->overrides[i];
                    _sym_p1->region_permutations[_sym_p1->region_permutation_count].region_name = override_data->region_name;
                    _sym_p1->region_permutations[_sym_p1->region_permutation_count].permutation_name = override_data->permutation_name;
                    ++_sym_p1->region_permutation_count;
                }
            }
        }
    }
}
