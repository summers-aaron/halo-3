#ifndef GAME_UNIVERSAL_TYPES_H
#define GAME_UNIVERSAL_TYPES_H

#include <stddef.h>

#include "game/weapon_set_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// March's selection readers and multiplayer_definitions.cpp agree on the blocks
// at 0x68, 0x74, 0x80, 0x8c and 0x98. Selection references retain their tag types.
struct s_multiplayer_weapon_selection
{
    long name;
    float weight;
    s_tag_reference weapon;
};

struct s_multiplayer_vehicle_selection
{
    long name;
    s_tag_reference vehicle;
};

struct s_multiplayer_vehicle_set
{
    long name;
    s_tag_block remaps;
};

// One entry of s_multiplayer_vehicle_set::remaps: the vehicle selection to replace and its replacement.
struct s_multiplayer_vehicle_set_remap { int original_selection; int replacement_selection; };

typedef char multiplayer_vehicle_set_remap_size[(sizeof(s_multiplayer_vehicle_set_remap) == 8) ? 1 : -1];

// March game_engine_settings_try_and_get uses a 180-byte universal-data element
// and its final tag reference.
struct s_multiplayer_universal_globals_definition
{
    s_tag_reference random_player_names;
    s_tag_reference team_names;
    c_typed_tag_block<real_rgb_color> team_colors;
    s_tag_block customized_characters;
    char spare_after_customized_characters[0x44 - 0x38];
    long strings_index;
    // The object-properties screen reads its sandbox string list at +0x54.
    s_tag_reference sandbox_strings;
    s_tag_reference sandbox_object_properties;
    long weapon_selection_count;
    TAG_POINTER(s_multiplayer_weapon_selection) weapon_selections;
    TAG_POINTER(void) weapon_selection_definition;
    long vehicle_selection_count;
    TAG_POINTER(s_multiplayer_vehicle_selection) vehicle_selections;
    TAG_POINTER(void) vehicle_selection_definition;
    s_tag_block grenade_selections;
    int weapon_set_count;
    TAG_POINTER(s_weapon_set) weapon_sets;
    TAG_POINTER(void) weapon_set_definition;
    long vehicle_set_count;
    TAG_POINTER(s_multiplayer_vehicle_set) vehicle_sets;
    TAG_POINTER(void) vehicle_set_definition;
    s_tag_reference game_engine_settings;
};

typedef char universal_strings_index_offset[(offsetof(s_multiplayer_universal_globals_definition, strings_index) == 0x44) ? 1 : -1];

// Compatibility spelling used by game_engine_traits.cpp.
typedef s_multiplayer_universal_globals_definition s_universal;

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char universal_weapon_selection_size[(sizeof(s_multiplayer_weapon_selection) == 0x18) ? 1 : -1];
typedef char universal_vehicle_selection_size[(sizeof(s_multiplayer_vehicle_selection) == 0x14) ? 1 : -1];
typedef char universal_vehicle_set_size[(sizeof(s_multiplayer_vehicle_set) == 0x10) ? 1 : -1];
typedef char universal_size[(sizeof(s_universal) == 0xb4) ? 1 : -1];
typedef char universal_weapon_set_offset[(offsetof(s_universal, weapon_set_count) == 0x8c) ? 1 : -1];
typedef char universal_vehicle_set_offset[(offsetof(s_universal, vehicle_set_count) == 0x98) ? 1 : -1];
#endif

#endif // GAME_UNIVERSAL_TYPES_H
