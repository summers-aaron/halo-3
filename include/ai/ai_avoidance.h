#ifndef AI_AI_AVOIDANCE_H
#define AI_AI_AVOIDANCE_H

#include "math/real_math_types.h"

struct vector_avoidance_ray
{
    float weight;
    real_vector3d offset;
    real_vector3d direction;
};

extern float sense_elevation_scale;
extern float avoidance_elevations[2];
extern vector_avoidance_ray avoidance_rays[8][2];
extern real_vector3d avoidance_directions[8];
extern vector_avoidance_ray sense_rays[9];

#endif // AI_AI_AVOIDANCE_H
