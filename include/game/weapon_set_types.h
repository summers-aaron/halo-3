#ifndef GAME_WEAPON_SET_TYPES_H
#define GAME_WEAPON_SET_TYPES_H

#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"

enum e_weapon_set { _weapon_set_primary, _weapon_set_secondary, k_weapon_set_count };

enum e_multiplayer_weapon_selection_name
{
    _multiplayer_weapon_selection_name_none = 0x10387,
    _multiplayer_weapon_selection_name_random = 0x40003
};

struct s_weapon_set { int string_id; s_tag_block remaps; };

// One entry of s_weapon_set::remaps: the weapon selection to replace and its replacement.
struct s_weapon_set_remap { int original_selection; int replacement_selection; };

typedef char weapon_set_remap_size[(sizeof(s_weapon_set_remap) == 8) ? 1 : -1];

#endif // GAME_WEAPON_SET_TYPES_H
