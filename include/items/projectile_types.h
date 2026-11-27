#ifndef ITEMS_PROJECTILE_TYPES_H
#define ITEMS_PROJECTILE_TYPES_H

#include <stddef.h>
#include "objects/object_datum.h"

#include "game/materials.h"
#include "items/projectile_flags.h"
#include "simulation/game_interface/simulation_game_projectiles.h"

// Projectile-specific state stored with its object datum.
struct _projectile_datum {
c_flags_no_init<e_projectile_flags, unsigned long, 21> flags;
    signed char action;
    unsigned char collision_count;
    c_global_material_type impact_material;
    real_vector3d hit_normal;
    long parent_unit_index;
    long team;
    union {
        s_projectile_targeting_data targeting_data;
        struct {
            long target_object_index;
            short target_node_index;
        };
    };
    union { long ignore_object_index; long weapon_index; };
    long collision_object;
    long collision_node;
    real_point3d collision_position;
    union { float scale_a; float detonation_timer; };
    union { float timer_rate; float detonation_timer_delta; };
    float supercombine_timer;
    float supercombine_timer_rate;
    float scale_b;
    float action_timer_rate;
    float time;
    float collision_time;
    float velocity_scale;
    float maximum_damage_distance;
    real_vector3d rotation_axis;
    float rotation_sine;
    float rotation_cosine;
    float damage_scale;
    float tracking_angular_velocity;
    float acceleration_velocity;
    float distance;
    real_point3d initial_position;
    short shot_id;
    short age;
};

struct projectile_datum {
    object_datum object;
    _projectile_datum projectile;
};

typedef char projectile_datum_size[(sizeof(projectile_datum) == 0x1f8) ? 1 : -1];
typedef char projectile_datum_flags_offset[(offsetof(projectile_datum, projectile.flags) == 0x160) ? 1 : -1];
typedef char projectile_datum_parent_unit_index_offset[(offsetof(projectile_datum, projectile.parent_unit_index) == 0x174) ? 1 : -1];
typedef char projectile_datum_targeting_data_offset[(offsetof(projectile_datum, projectile.targeting_data) == 0x17c) ? 1 : -1];
typedef char projectile_datum_weapon_index_offset[(offsetof(projectile_datum, projectile.weapon_index) == 0x184) ? 1 : -1];
typedef char projectile_datum_damage_scale_offset[(offsetof(projectile_datum, projectile.damage_scale) == 0x1d8) ? 1 : -1];
typedef char projectile_datum_shot_id_offset[(offsetof(projectile_datum, projectile.shot_id) == 0x1f4) ? 1 : -1];

#endif // ITEMS_PROJECTILE_TYPES_H
