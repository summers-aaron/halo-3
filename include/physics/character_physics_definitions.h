#ifndef PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_H
#define PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/character_physics_definitions.cpp that other units call.

#include "physics/character_physics_definition_types.h"

class phys_shape;

// match: 0xe7a38, 0x10c518, 0x257e80: the inline bool boundary keeps 0/1 flag extraction at every call site.
static __forceinline bool character_physics_definition_test_flag(const s_character_physics_definition *definition, long bit)
{
    return (definition->flags & (1UL << bit)) != 0;
}

void character_physics_definition_fixup(struct s_character_physics_definition * _sym_p0);
const phys_shape *character_physics_definition_get_shape(
    const s_character_physics_definition *definition, s_character_physics_definition::e_shape_type shape_type);

#endif // PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_H
