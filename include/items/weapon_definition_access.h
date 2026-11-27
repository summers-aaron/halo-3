#ifndef ITEMS_WEAPON_DEFINITION_ACCESS_H
#define ITEMS_WEAPON_DEFINITION_ACCESS_H

#include "items/weapon_definitions.h"
#include "cache/cache_file_globals.h"
#include "memory/datum_index.h"
#include "objects/object_header_access.h"

static __forceinline weapon_definition *weapon_definition_get(unsigned long definition_index)
{
    weapon_definition *result = (weapon_definition *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
    return result;
}

inline bool weapon_is_support_weapon(long weapon_index)
{
    const object_datum *weapon = object_get(weapon_index);
    const weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    return definition->m_flags.test(_weapon_definition_support_weapon);
}

// Shared literal-pool anchor: zero at +0, pitch fallback (130.0f) at +0x76a8.
extern "C" const float k_weapon_pitch_throttling_constants[];

#endif
