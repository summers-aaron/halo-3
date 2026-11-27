#ifndef EFFECTS_MATERIAL_EFFECTS_H
#define EFFECTS_MATERIAL_EFFECTS_H

#include "effects/material_effect_search_types.h"

// Functions defined in tu/blamlibxbox/material_effects.cpp that other units call.

extern "C" long material_effect_resolve_effect_type(long material_effects_index, e_material_effect_search search, long effect_type);

union real_point3d;
class c_global_material_type;
struct s_material_effect;
struct s_material_effect_material;

s_material_effect_material *material_effect_find_material_internal(
    const s_material_effect *effect, e_material_effect_search search, c_global_material_type material_type);
enum e_effect_type;
enum e_sweetener_size;
void material_effect_find_effects(bool, const real_point3d *, long, c_global_material_type,
    c_global_material_type, bool, e_effect_type, e_sweetener_size, long *);

#endif // EFFECTS_MATERIAL_EFFECTS_H
