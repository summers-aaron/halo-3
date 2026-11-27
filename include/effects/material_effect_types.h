#ifndef EFFECTS_MATERIAL_EFFECT_TYPES_H
#define EFFECTS_MATERIAL_EFFECT_TYPES_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"
#include "game/materials.h"

#include "effects/material_effect_enums.h"

// One element of a material effects tag's effects block (36 bytes), indexed by e_effect_type.
struct s_material_effect
{
    s_tag_block old_materials;
    s_tag_block first_materials;
    s_tag_block second_materials;
};

// Values of s_material_effect_material::sweetener_mode.
enum e_material_effect_sweetener_mode
{
    _material_effect_sweetener_mode_default = 0,
    _material_effect_sweetener_mode_enabled,
    _material_effect_sweetener_mode_disabled,

    k_material_effect_sweetener_mode_count
};

// One element of s_material_effect::first_materials / second_materials (40 bytes).
struct s_material_effect_material
{
    s_tag_reference effect;
    s_tag_reference second_effect;
    string_id material_name;
    c_global_material_type global_material_type;
    // e_material_effect_sweetener_mode: a three-valued byte (0 default, 1 enabled, 2 disabled), not a bool.
    unsigned char sweetener_mode;
};

typedef char material_effect_size[(sizeof(void *) != 4 || sizeof(s_material_effect) == 0x24) ? 1 : -1];
typedef char material_effect_first_materials[(sizeof(void *) != 4 || offsetof(s_material_effect, first_materials) == 0x0c) ? 1 : -1];
typedef char material_effect_material_size[(sizeof(void *) != 4 || sizeof(s_material_effect_material) == 0x28) ? 1 : -1];
typedef char material_effect_material_second_effect[(sizeof(void *) != 4 || offsetof(s_material_effect_material, second_effect) == 0x10) ? 1 : -1];
typedef char material_effect_material_name[(sizeof(void *) != 4 || offsetof(s_material_effect_material, material_name) == 0x20) ? 1 : -1];
typedef char material_effect_material_global_material_type[
    (sizeof(void *) != 4 || offsetof(s_material_effect_material, global_material_type) == 0x24) ? 1 : -1];
typedef char material_effect_material_sweetener_mode[(sizeof(void *) != 4 || offsetof(s_material_effect_material, sweetener_mode) == 0x26) ? 1 : -1];

#endif // EFFECTS_MATERIAL_EFFECT_TYPES_H
