#ifndef OBJECTS_MODEL_CUSTOMIZATION_TYPES_H
#define OBJECTS_MODEL_CUSTOMIZATION_TYPES_H

#include "tag_files/tag_types.h"
#include "objects/object_placement.h"

// The customization tag's character -> region -> permutation -> override hierarchy.
// c_player_profile.cpp and start_menu_settings_appearance_model.cpp also read the
// permutation's name, description, flags and unlockable at +0, +4, +8 and +0xc.
struct s_multiplayer_customized_model_permutation
{
    long name;
    long description;
    short flags;
    short padding;
    long unlockable;
    c_typed_tag_block<s_object_region_permutation> overrides;
};

struct s_multiplayer_customized_model_region
{
    long name;
    c_typed_tag_block<s_multiplayer_customized_model_permutation> permutations;
};

struct s_multiplayer_customized_model_character
{
    long name;
    c_typed_tag_block<s_multiplayer_customized_model_region> regions;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char multiplayer_customized_model_permutation_size[(sizeof(s_multiplayer_customized_model_permutation) == 28) ? 1 : -1];
typedef char multiplayer_customized_model_region_size[(sizeof(s_multiplayer_customized_model_region) == 16) ? 1 : -1];
typedef char multiplayer_customized_model_character_size[(sizeof(s_multiplayer_customized_model_character) == 16) ? 1 : -1];
#endif

#endif
