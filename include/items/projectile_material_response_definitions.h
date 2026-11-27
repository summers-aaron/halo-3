#ifndef ITEMS_PROJECTILE_MATERIAL_RESPONSE_DEFINITIONS_H
#define ITEMS_PROJECTILE_MATERIAL_RESPONSE_DEFINITIONS_H

#include "tag_files/tag_types.h"
#include "game/materials.h"
#include "game/material_constants.h"

struct s_projectile_material_response_definition {
    __forceinline s_projectile_material_response_definition() : material(k_global_material_type_default) {}
    unsigned short flags;
    short response;
    char spare_after_response[4];
    c_global_material_type material;
    char spare_before_potential_response[2];
    short potential_response;
    unsigned short potential_flags;
    float chance;
    real_bounds incidence_angle;
    real_bounds velocity;
    short scale_effects_by;
    float angle_noise;
    float velocity_noise;
    float initial_friction;
    float spare_before_parallel_friction;
    float parallel_friction;
    float perpendicular_friction;
};

typedef char projectile_material_response_size[(sizeof(s_projectile_material_response_definition) == 0x40) ? 1 : -1];

#endif
