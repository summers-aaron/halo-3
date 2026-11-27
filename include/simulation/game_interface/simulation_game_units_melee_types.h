#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_UNITS_MELEE_TYPES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_UNITS_MELEE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/damage_owner.h"

struct s_simulation_unit_melee_damage_event_data
{
    real_point3d position;
    real_vector3d direction;
    long damage_definition_index;
    long clang_damage_definition_index;
    short breakable_surface_set_index;
    short breakable_surface_index;
    long instanced_geometry_instance_index;
    long surface_index;
    long material_index;
    long global_material_index;
    float scale;
    unsigned char damage_reporting_info;
};

typedef char simulation_unit_melee_event_size[(sizeof(s_simulation_unit_melee_damage_event_data) == 0x3c) ? 1 : -1];

struct s_simulation_unit_melee_clang_event_data
{
    long definition_index;
    real_point3d position;
    real_vector3d direction;
};

typedef char simulation_unit_melee_clang_event_size[(sizeof(s_simulation_unit_melee_clang_event_data) == 0x1c) ? 1 : -1];

struct s_simulation_unit_melee_damage_target {
    real_point3d position;
    real_vector3d direction;
    long breakable_surface_set_index;
    long object_index;
    short breakable_surface_index;
    short breakable_surface_instance_index;
    long breakable_surface_bsp_index;
    long breakable_surface_cluster_index;
    long model_region_index;
    short global_material_index;
    float damage_scale;
    unsigned char hit_material_flags;
    bool lunge;
};

// Melee simulation users share this fixed-size target.
typedef char simulation_unit_melee_target_size[(sizeof(s_simulation_unit_melee_damage_target) == 0x3c) ? 1 : -1];
typedef char simulation_unit_melee_target_object_offset[(offsetof(s_simulation_unit_melee_damage_target, object_index) == 0x1c) ? 1 : -1];
typedef char simulation_unit_melee_target_material_offset[(offsetof(s_simulation_unit_melee_damage_target, global_material_index) == 0x30) ? 1 : -1];
typedef char simulation_unit_melee_target_scale_offset[(offsetof(s_simulation_unit_melee_damage_target, damage_scale) == 0x34) ? 1 : -1];

extern "C" void unit_validate_synchronized_melee_damage(long unit_index, long damage_effect_index, long clang_effect_index, const s_damage_owner *damage_owner,
    const s_simulation_unit_melee_damage_target *damage_target, bool *out_synchronized, bool *out_clang);
extern "C" void unit_handle_synchronized_melee_damage(long unit_index, long damage_effect_index, long clang_effect_index,
    const s_damage_owner *damage_owner, const s_simulation_unit_melee_damage_target *damage_target, bool synchronized, bool clang);

#endif
