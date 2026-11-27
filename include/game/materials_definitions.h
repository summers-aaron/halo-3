#ifndef GAME_MATERIALS_DEFINITIONS_H
#define GAME_MATERIALS_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "game/materials.h"
#include "game/game_globals.h"

struct water_physics_material_properties
{
    float drag_q, drag_k, drag_e;
    float unused;
    float densities[5];
    float tail;
};

typedef char water_physics_material_properties_size[(sizeof(water_physics_material_properties) == 0x28) ? 1 : -1];

struct s_underwater_material_proxy
{
    long underwater_material_name;
    long proxy_material_name;
    c_global_material_type underwater_material_type;
    c_global_material_type proxy_material_type;
};

typedef char underwater_material_proxy_size[
    (sizeof(s_underwater_material_proxy) == 0x0c) ? 1 : -1];

// Material properties and sound references for the globals tag.
struct s_global_material_definition
{
    long material_id;
    long parent_material_name;
    c_global_material_type parent_material_type;
    unsigned short flags;
    long armor_type_a;
    long armor_type_b;
    unsigned long physics_flags;
    float friction;
    float restitution;
    float density;
    c_typed_tag_block<water_physics_material_properties> water_properties;
    s_tag_reference breakable_surface;
    union
    {
        s_tag_reference sounds[17];
        struct
        {
            s_tag_reference effect_sounds[3];
            s_tag_reference effect_looping_sound;
            s_tag_reference sound_sweetener_grinding;
            s_tag_reference sound_sweetener_melee[3];
            s_tag_reference sweetener_sounds[3];
            s_tag_reference rolling_looping_sound;
            s_tag_reference grinding_looping_sound;
            s_tag_reference effect_sweetener_melee;
            s_tag_reference default_sounds[3];
        };
    };
    unsigned long sweetener_inheritance_flags;
    s_tag_reference material_effects;
    s_tag_block underwater_proxies;
};

typedef char global_material_definition_parent_material_name_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, parent_material_name) == 0x04) ? 1 : -1];
typedef char global_material_definition_physics_flags_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, physics_flags) == 0x14) ? 1 : -1];
typedef char global_material_definition_density_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, density) == 0x20) ? 1 : -1];
typedef char global_material_definition_sound_sweetener_grinding_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, sound_sweetener_grinding) == 0x80) ? 1 : -1];
typedef char global_material_definition_sound_sweetener_melee_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, sound_sweetener_melee) == 0x90) ? 1 : -1];
typedef char global_material_definition_effect_sweetener_melee_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, effect_sweetener_melee) == 0x110) ? 1 : -1];
typedef char global_material_definition_sweetener_inheritance_flags_offset[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, sweetener_inheritance_flags) == 0x150) ? 1 : -1];

typedef char global_material_definition_flags_offset[
    (offsetof(s_global_material_definition, flags) == 0xa) ? 1 : -1];

typedef char global_material_definition_id_offset[
    (offsetof(s_global_material_definition, material_id) == 0) ? 1 : -1];

typedef char global_material_definition_size[
    (sizeof(void *) != 4 || sizeof(s_global_material_definition) == 0x170) ? 1 : -1];
typedef char global_material_definition_parent[
    (offsetof(s_global_material_definition, parent_material_type) == 0x08) ? 1 : -1];
typedef char global_material_definition_armor_type_a[
    (offsetof(s_global_material_definition, armor_type_a) == 0x0c) ? 1 : -1];
typedef char global_material_definition_armor_type_b[
    (offsetof(s_global_material_definition, armor_type_b) == 0x10) ? 1 : -1];
typedef char global_material_definition_friction[
    (offsetof(s_global_material_definition, friction) == 0x18) ? 1 : -1];
typedef char global_material_definition_restitution[
    (offsetof(s_global_material_definition, restitution) == 0x1c) ? 1 : -1];
typedef char global_material_definition_breakable_surface[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, breakable_surface) == 0x30) ? 1 : -1];
typedef char global_material_definition_sounds[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, sounds) == 0x40) ? 1 : -1];
typedef char global_material_definition_effect_sounds[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, effect_sounds) == 0x40) ? 1 : -1];
typedef char global_material_definition_sweetener_sounds[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, sweetener_sounds) == 0xc0) ? 1 : -1];
typedef char global_material_definition_default_sounds[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, default_sounds) == 0x120) ? 1 : -1];
typedef char global_material_definition_material_effects[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, material_effects) == 0x154) ? 1 : -1];
typedef char global_material_definition_underwater_proxies[
    (sizeof(void *) != 4 || offsetof(s_global_material_definition, underwater_proxies) == 0x164) ? 1 : -1];

__forceinline s_global_material_definition *c_global_material_type::get_material(const s_game_globals *globals) const
{
    s_global_material_definition *result = 0;
    if (m_index != -1)
    {
        if (m_index >= 0 && m_index < globals->materials.count())
            result = TAG_BLOCK_GET_ELEMENT(&globals->materials, m_index, s_global_material_definition);
    }
    return result;
}

#endif // GAME_MATERIALS_DEFINITIONS_H
