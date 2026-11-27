#ifndef EFFECTS_MATERIAL_EFFECT_ENUMS_H
#define EFFECTS_MATERIAL_EFFECT_ENUMS_H

enum e_effect_type
{
    _effect_type_walk = 0,
    _effect_type_run = 1,
    _effect_type_biped_sliding = 2,
    _effect_type_biped_shuffle = 3,
    _effect_type_biped_jump = 4,
    _effect_type_biped_jump_land = 5,
    _effect_type_biped_sprint = 6,
    _effect_type_biped_jump_land_hard = 7,
    _effect_type_biped_bodyfall = 8,
    _effect_type_biped_unused_2 = 9,
    _effect_type_particle_impact = 10,
    _effect_type_obstacle_smash = 11,
    _effect_type_collision_large = 12,
    _effect_type_grinding = 13,
    _effect_type_rolling,
    _effect_type_impact,
    _effect_type_fizzle,
    _effect_type_overpenetrate,
    _effect_type_attach,
    _effect_type_bounce,
    _effect_type_ricochet,
    _effect_type_collision_damage,
    _effect_type_melee,

    k_effect_type_count
};

// Small/medium/large sound references in s_global_material_definition.
enum e_sweetener_size
{
    _sweetener_size_none = -1,
    _sweetener_size_small = 0,
    _sweetener_size_medium,
    _sweetener_size_large,

    k_sweetener_size_count
};

#endif // EFFECTS_MATERIAL_EFFECT_ENUMS_H
