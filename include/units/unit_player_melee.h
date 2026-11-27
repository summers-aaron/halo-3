#ifndef UNITS_UNIT_PLAYER_MELEE_H
#define UNITS_UNIT_PLAYER_MELEE_H

#include "math/real_math_types.h"
#include "game/materials.h"

struct s_unit_player_melee_damage_target {
    real_point3d position;
    real_vector3d normal;
    long breakable_surface_set_index;
    long object_index;
    short breakable_surface_index;
    short breakable_surface_instance_index;
    long breakable_surface_bsp_index;
    long breakable_surface_cluster_index;
    long model_region_index;
    c_global_material_type material_type;
    float damage_scale;
    unsigned char hit_material_flags;
    bool lunge;
};

// Preserve the units.cpp caller's existing five-argument C-linkage view.
extern "C" void simulation_action_unit_melee_damage(
    long unit_index, long effect_index, long clang_effect_index, const s_unit_player_melee_damage_target *target, bool predicted);

#endif // UNITS_UNIT_PLAYER_MELEE_H
