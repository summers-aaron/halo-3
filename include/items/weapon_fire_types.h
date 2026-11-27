#ifndef ITEMS_WEAPON_FIRE_TYPES_H
#define ITEMS_WEAPON_FIRE_TYPES_H

#include "math/real_math_types.h"

struct s_predicted_weapon_fire_data
{
    long projectile_id;
    bool impact_valid;
    real_point3d impact_position;
    bool conical_valid;
    unsigned short conical_random_seed;
};

struct s_simulation_action_projectile_fire_result
{
    long projectile_id;
    bool impact_valid;
    long impact_object_index;
    long impact_node_index;
    real_point3d impact_position;
    bool conical_valid;
    unsigned short conical_random_seed;
};


#endif
