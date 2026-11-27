#ifndef OBJECTS_DAMAGE_DATA_H
#define OBJECTS_DAMAGE_DATA_H

#include "objects/damage_owner.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "game/materials.h"
#include <stddef.h>

struct damage_proximity_object;
struct s_projectile_material_response_definition;

// Damage event data shared by damage, action and weapon code.
struct s_damage_data
{
    s_damage_data() {}

    long effect;
    unsigned long flags;
    union { s_damage_owner damage_owner; s_damage_owner owner; s_damage_owner owner_data; };
    long damage_owner_object_index;
    long object_index;
    unsigned long damage_identifier;
    s_location location;
    real_point3d origin;
    real_point3d position;
    real_vector3d direction;
    real_vector3d initial_direction;
    union {
        float scale[3];
        struct { float amount; float shield_amount; union { float cone_amount; float multiplier; }; };
    };
    float distance;
    float weight;
    unsigned char close;
    unsigned char in_range;
    real_vector3d cone_direction;
    long ultimate_parent_index;
    float shield_vitality;
    c_global_material_type material_type;
    short shot_id;
    const s_projectile_material_response_definition *material_response;
    unsigned char reporting_info;
    damage_proximity_object *proximity;
    long model_region_index;
};

typedef char damage_data_size[(sizeof(void *) != 4 || sizeof(s_damage_data) == 0x94) ? 1 : -1];
typedef char damage_data_material_offset[(offsetof(s_damage_data, material_type) == 0x80) ? 1 : -1];
typedef char damage_data_amount_offset[(offsetof(s_damage_data, amount) == 0x54) ? 1 : -1];
typedef char damage_data_shot_id_offset[(offsetof(s_damage_data, shot_id) == 0x82) ? 1 : -1];
typedef char damage_data_region_offset[(sizeof(void *) != 4 || offsetof(s_damage_data, model_region_index) == 0x90) ? 1 : -1];

#endif // OBJECTS_DAMAGE_DATA_H
