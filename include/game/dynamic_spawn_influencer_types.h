#ifndef GAME_DYNAMIC_SPAWN_INFLUENCER_TYPES_H
#define GAME_DYNAMIC_SPAWN_INFLUENCER_TYPES_H

#include "math/matrix_math_types.h"

struct s_dynamic_spawn_influencer
{
    unsigned char shape;
    unsigned char type;
    short team_designator;
    float inner_radius, outer_radius, height;
    real_matrix4x3 transform;
    float weight;
};

#endif // GAME_DYNAMIC_SPAWN_INFLUENCER_TYPES_H
